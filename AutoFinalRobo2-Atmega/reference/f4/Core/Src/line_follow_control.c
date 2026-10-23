/* Portable optical calibration and line control. Application callbacks own all
 * IO, storage and motion. No HAL, concrete MPU, PID or motor dependencies. */
#include "line_follow_control.h"
#include <math.h>
#include <string.h>

static float clampf(float x, float lo, float hi) { return x < lo ? lo : (x > hi ? hi : x); }
static float maxf(float a, float b) { return a > b ? a : b; }
static uint16_t median32(const volatile uint16_t values[32][3], unsigned channel) {
    uint16_t v[32];
    for (unsigned i=0; i<32; ++i) v[i]=values[i][channel];
    for (unsigned i=1; i<32; ++i) {
        uint16_t x=v[i]; unsigned j=i;
        while (j && v[j-1]>x) { v[j]=v[j-1]; --j; } v[j]=x;
    }
    return (uint16_t)(((uint32_t)v[15]+v[16])/2u);
}
bool LineCalibration_Ready(const LineCalibration_Store *s) {
    return s && s->white.valid && s->track.valid && s->white_revision &&
        s->track_white_revision == s->white_revision;
}
bool LineCalibration_Busy(const LineCalibration_Handle *h) {
    return h && h->state >= LINE_CAL_PROMPT && h->state <= LINE_CAL_SAVING;
}
void LineCalibration_Init(LineCalibration_Handle *h, const LineCalibration_Port *p, const LineCalibration_Store *s) {
    memset(h,0,sizeof(*h)); h->port=*p; if (s) h->store=*s;
}
static void cal_finish(LineCalibration_Handle *h, LineCalibration_State state, uint8_t reason) {
    h->state=state; h->reason=reason; h->port.restore(h->port.ctx);
}
uint8_t LineCalibration_Control(LineCalibration_Handle *h, uint8_t op, uint32_t session, uint32_t now) {
    if (op<=1u) {
        if (LineCalibration_Busy(h)) return LINE_FOLLOW_REJECT_BUSY;
        if (op==1u && !h->store.white.valid) return LINE_FOLLOW_REJECT_CALIBRATION;
        ++h->session_id; if (!h->session_id) ++h->session_id;
        memset(&h->pending,0,sizeof(h->pending));
        h->target=op; h->phase=0; h->progress=0; h->reason=0;
        h->state=LINE_CAL_PROMPT; h->started_ms=now;
        return 0;
    }
    if (!session || session!=h->session_id || !LineCalibration_Busy(h)) return LINE_FOLLOW_REJECT_SESSION;
    if (op==3u) { cal_finish(h,LINE_CAL_CANCELLED,0); return 0; }
    if (op!=2u || h->state!=LINE_CAL_PROMPT) return LINE_FOLLOW_REJECT_SESSION;
    h->state=LINE_CAL_STILL; h->started_ms=now; h->still_ms=0; h->last_pose_seq=0;
    return 0;
}
static void cal_phase(LineCalibration_Handle *h, uint32_t now) {
    h->state=LINE_CAL_STILL; /* prevent DMA accumulation while resetting */
    h->scans=0; memset((void *)h->sum,0,sizeof(h->sum));
    memset((void *)h->squares,0,sizeof(h->squares));
    memset((void *)h->window_sum,0,sizeof(h->window_sum));
    h->phase_ms=now;
    h->port.lights(h->port.ctx,h->phase==1u || h->phase==2u,h->phase>=2u);
    h->state=LINE_CAL_CAPTURE;
}
void LineCalibration_PushScan(LineCalibration_Handle *h, const uint16_t raw[3], uint32_t seq, uint32_t now) {
    if (h->state!=LINE_CAL_CAPTURE || seq==h->last_adc_seq ||
        (uint32_t)(now-h->phase_ms)<130u || h->scans>=1280u) return;
    h->last_adc_seq=seq;
    unsigned n=h->scans;
    for (unsigned c=0;c<3;++c) {
        uint32_t x=raw[c]; h->sum[c]+=x; h->squares[c]+=(uint64_t)x*x;
        h->window_sum[c]+=x;
        if ((n+1u)%40u==0u) { h->windows[n/40u][c]=(uint16_t)(h->window_sum[c]/40u); h->window_sum[c]=0; }
    }
    h->scans=(uint16_t)(n+1u);
}
static bool cal_pose_ok(const LineCalibration_Pose *p, uint32_t now) {
    return p->valid && (uint32_t)(now-p->acquired_ms)<=50u && isfinite(p->pitch_deg) &&
        isfinite(p->roll_deg) && isfinite(p->gyro_max_dps) && fabsf(p->pitch_deg)<=10.0f &&
        fabsf(p->roll_deg)<=5.0f && p->gyro_max_dps<3.0f;
}
static bool cal_quality(const LineCalibration_Handle *h, const LineCalibration_Store *next) {
    const LineCalibration_Profile *p=&h->pending;
    for (unsigned mode=0;mode<2;++mode) {
        unsigned on=mode ? 2u : 1u, off=mode ? 3u : 0u;
        for (unsigned c=0;c<3;++c) {
            if (p->mean[on][c]<=5u || p->mean[on][c]>=4090u || p->noise[on][c]>40u) return false;
            if (!h->target && fabsf((float)p->mean[on][c]-p->mean[off][c]) <
                maxf(20.0f,6.0f*maxf(p->noise[on][c],p->noise[off][c]))) return false;
        }
        if (h->target) {
            const LineCalibration_Profile *w=&next->white;
            float contrast=fabsf((float)p->mean[on][1]-w->mean[on][1]);
            if (contrast<maxf(100.0f,8.0f*maxf(p->noise[on][1],w->noise[on][1]))) return false;
            for (unsigned c=0;c<3;c+=2) if (fabsf((float)p->mean[on][c]-w->mean[on][c])>
                maxf(80.0f,6.0f*maxf(p->noise[on][c],w->noise[on][c]))) return false;
        }
    }
    return true;
}
void LineCalibration_Task(LineCalibration_Handle *h, const LineCalibration_Pose *p, uint32_t now) {
    if (h->state==LINE_CAL_PROMPT) {
        if ((uint32_t)(now-h->started_ms)>60000u) cal_finish(h,LINE_CAL_FAILED,LINE_FOLLOW_REJECT_TIMEOUT);
        return;
    }
    if (h->state!=LINE_CAL_STILL && h->state!=LINE_CAL_CAPTURE) return;
    bool quiet=cal_pose_ok(p,now);
    if (h->state==LINE_CAL_STILL) {
        if ((uint32_t)(now-h->started_ms)>=3000u) { cal_finish(h,LINE_CAL_FAILED,LINE_FOLLOW_REJECT_UNSTABLE); return; }
        if (!quiet) { h->still_ms=0; return; }
        if (!h->still_ms || fabsf(p->pitch_deg-h->still_reference.pitch_deg)>1.0f ||
            fabsf(p->roll_deg-h->still_reference.roll_deg)>1.0f) {
            h->still_ms=now; h->still_reference=*p; return;
        }
        if ((uint32_t)(now-h->still_ms)>=500u) {
            h->reference=*p; h->pending.pitch_deg=p->pitch_deg;
            h->pending.roll_deg=p->roll_deg; h->pending.yaw_deg=p->yaw_deg;
            cal_phase(h,now);
        }
        return;
    }
    if (!quiet || fabsf(p->pitch_deg-h->reference.pitch_deg)>1.0f ||
        fabsf(p->roll_deg-h->reference.roll_deg)>1.0f) {
        cal_finish(h,LINE_CAL_FAILED,LINE_FOLLOW_REJECT_UNSTABLE); return;
    }
    h->progress=(uint8_t)(((unsigned)h->phase*1280u+h->scans)*100u/5120u);
    if ((uint32_t)(now-h->phase_ms)>=2000u) { cal_finish(h,LINE_CAL_FAILED,LINE_FOLLOW_REJECT_TIMEOUT); return; }
    if (h->scans<1280u) return;
    for (unsigned c=0;c<3;++c) {
        h->pending.mean[h->phase][c]=median32(h->windows,c);
        float mean=(float)h->sum[c]/1280.0f;
        float variance=(float)h->squares[c]/1280.0f-mean*mean;
        h->pending.noise[h->phase][c]=(uint16_t)ceilf(sqrtf(maxf(0.0f,variance)));
    }
    if (++h->phase<4u) { cal_phase(h,now); return; }
    h->state=LINE_CAL_SAVING;
    LineCalibration_Store next=h->store;
    h->pending.useful_scans=5120u; h->pending.valid=1;
    if (!cal_quality(h,&next)) { cal_finish(h,LINE_CAL_FAILED,LINE_FOLLOW_REJECT_QUALITY); return; }
    if (!h->target) {
        next.white=h->pending; ++next.white_revision; if (!next.white_revision) ++next.white_revision;
        next.track.valid=0; next.track_white_revision=0;
    } else { next.track=h->pending; next.track_white_revision=next.white_revision; }
    if (!h->port.save(h->port.ctx,&next)) { cal_finish(h,LINE_CAL_FAILED,LINE_FOLLOW_REJECT_STORAGE); return; }
    h->store=next; h->progress=100;
    cal_finish(h,LINE_CAL_DONE,0);
}

bool LineFollow_Evaluate(const LineCalibration_Store *s, const uint16_t raw[3], bool lights,
                        const LineFollow_Geometry *g, LineFollow_Route *r) {
    memset(r,0,sizeof(*r)); r->alignment=4u;
    r->leftRaw=raw[0]; r->centerRaw=raw[1]; r->rightRaw=raw[2];
    r->leftNorm=4095u-raw[0]; r->centerNorm=4095u-raw[1]; r->rightNorm=4095u-raw[2];
    r->lineWidthMm=g->black_width_mm; r->centeredMarginMm=g->centered_margin_mm;
    if (!LineCalibration_Ready(s)) return false;
    unsigned phase=lights ? 2u : 1u;
    float span=(float)s->track.mean[phase][1]-s->white.mean[phase][1];
    if (fabsf(span)<100.0f) return false;
    float direction=span>0 ? 1.0f : -1.0f;
    float dark[3];
    for (unsigned c=0;c<3;++c) {
        if (c==1u) dark[c]=clampf(((float)raw[c]-s->white.mean[phase][c])/span,0,1);
        else {
            float white=((float)s->white.mean[phase][c]+s->track.mean[phase][c])*0.5f;
            float noise=maxf(s->white.noise[phase][c],s->track.noise[phase][c]);
            float threshold=maxf(20.0f,6.0f*noise);
            dark[c]=clampf(((float)raw[c]-white)*direction/threshold,0,1);
        }
    }
    r->leftDarkness=dark[0]; r->centerDarkness=dark[1]; r->rightDarkness=dark[2];
    r->leftBlack=dark[0]>=1.0f; r->rightBlack=dark[2]>=1.0f;
    r->centerBlack=dark[1]>=0.65f;
    r->leftWhite=dark[0]<=0.45f; r->rightWhite=dark[2]<=0.45f;
    r->pattern=(r->leftBlack?4u:0u)|(r->centerBlack?2u:0u)|(r->rightBlack?1u:0u);
    float sum=dark[0]+dark[1]+dark[2];
    r->lateralErrorMm=sum>0.05f ? (g->sensor_position_mm[0]*dark[0]+g->sensor_position_mm[2]*dark[2])/sum : 0;
    r->lateralErrorNorm=r->lateralErrorMm/16.0f;
    r->ambiguous=(r->leftBlack && r->rightBlack) || (!r->leftWhite && !r->leftBlack) ||
        (!r->rightWhite && !r->rightBlack) || (dark[1]>0.45f && dark[1]<0.65f);
    r->confidence=(uint8_t)(clampf(sum,0,1)*100.0f);
    if (r->ambiguous && r->confidence>30u) r->confidence=30u;
    r->alignment=r->ambiguous ? 3u : (r->leftBlack ? 1u : (r->rightBlack ? 2u :
        (r->centerBlack && r->leftWhite && r->rightWhite ? 0u : 4u)));
    return true;
}

void LineFollowControl_GetDefaultConfig(LineFollow_Config_t *c) {
    memset(c,0,sizeof(*c)); c->line_config.black_width_mm=18;
    c->line_config.sensor_position_mm[0]=-16; c->line_config.sensor_position_mm[2]=16;
    c->line_config.centered_margin_mm=7; c->forward_percent=12; c->recover_turn_percent=35;
    c->max_turn_percent=90; c->kp=4; c->ki=0; c->kd=0.1f; c->yaw_rate_damping=0.25f;
    c->recover_timeout_ms=700;
}
AM_Result LineFollowControl_Init(LineFollowControl_Handle *h,const LineFollow_Config_t *c) {
    if (!h || !c || !c->read || !c->command || !c->hold) return AM_ERROR;
    memset(h,0,sizeof(*h)); h->config=*c; h->initialized=1;
    LineFollowControl_Reset(h); return AM_OK;
}
void LineFollowControl_Reset(LineFollowControl_Handle *h) {
    memset(&h->status,0,sizeof(h->status)); h->status.fsm=LINE_FOLLOW_SEARCH;
    h->status.line_width_mm=h->config.line_config.black_width_mm;
    h->integral=0; h->previous_error=0; h->filtered_derivative=0;
    h->last_ir_seq=0; h->candidate_valid=0; h->unstable=0; h->ambiguity_pending=0;
}
void LineFollowControl_SetEnabled(LineFollowControl_Handle *h,uint8_t on) {
    if (!h) return;
    h->config.enabled=on?1:0;
    if (!on) { if (h->config.hold) h->config.hold(h->config.ctx); h->status.forward_percent=0; h->status.turn_percent=0; }
}
void LineFollowControl_SetLineBlackWidth(LineFollowControl_Handle *h,uint16_t width) {
    h->config.line_config.black_width_mm=width;
    h->config.line_config.centered_margin_mm=maxf(0,16.0f-(float)width/2.0f);
    h->status.line_width_mm=width;
}
static void line_fault(LineFollowControl_Handle *h,uint8_t reason,uint32_t now) {
    h->status.fsm=LINE_FOLLOW_FAULT; h->status.reject_reason=(LineFollow_RejectReason_t)reason;
    h->status.state_started_ms=now; LineFollowControl_SetEnabled(h,0);
}
void LineFollowControl_Task(LineFollowControl_Handle *h,uint32_t now) {
    if (!h || !h->initialized || !h->config.enabled) return;
    LineFollow_Input in;
    if (!h->config.read(h->config.ctx,&in)) { line_fault(h,LINE_FOLLOW_REJECT_NOT_READY,now); return; }
    uint8_t reason=!in.mode_follow ? LINE_FOLLOW_REJECT_MODE :
        (!in.calibrated ? LINE_FOLLOW_REJECT_CALIBRATION :
        (!in.armed || !in.balance_active ? LINE_FOLLOW_REJECT_DISARMED :
        (!in.imu_valid || (uint32_t)(now-in.imu_acquired_ms)>50u ? LINE_FOLLOW_REJECT_NOT_READY : 0)));
    if (reason) { line_fault(h,reason,now); return; }
    if (in.illumination_settling) { h->config.hold(h->config.ctx); return; }
    if (!in.ir_valid || (uint32_t)(now-in.ir_acquired_ms)>30u) { line_fault(h,LINE_FOLLOW_REJECT_IR_STALE,now); return; }
    if (fabsf(in.pitch_error_deg)>5.0f || fabsf(in.roll_deviation_deg)>5.0f) {
        if (!h->unstable) { h->unstable=1; h->unstable_ms=now; }
        if ((uint32_t)(now-h->unstable_ms)>=100u) { line_fault(h,LINE_FOLLOW_REJECT_UNSTABLE,now); return; }
        h->config.hold(h->config.ctx); return;
    } else h->unstable=0;
    if (in.ir_seq==h->last_ir_seq) return;
    h->last_ir_seq=in.ir_seq;
    h->status.sample=in.route; h->status.sensors_valid=1;
    h->status.lateral_error_mm=in.route.lateralErrorMm;
    h->status.lateral_error_norm=in.route.lateralErrorNorm;
    h->status.confidence_percent=in.route.confidence; h->status.ambiguous=in.route.ambiguous;
    h->status.yaw_rate_dps=in.yaw_valid?in.yaw_rate_ccw_dps:0;
    if (in.route.ambiguous || (in.route.pattern&5u)==5u) {
        if (!h->ambiguity_pending) { h->ambiguity_pending=1; h->ambiguous_ms=now; }
        h->config.hold(h->config.ctx);
        if ((uint32_t)(now-h->ambiguous_ms)>=100u) line_fault(h,LINE_FOLLOW_REJECT_AMBIGUOUS,now);
        return;
    }
    h->ambiguity_pending=0;
    if (in.route.pattern!=h->status.candidate_pattern || !h->candidate_valid) {
        h->status.candidate_pattern=in.route.pattern; h->candidate_ms=now; h->candidate_valid=1;
    }
    if ((uint32_t)(now-h->candidate_ms)<20u) {
        h->config.hold(h->config.ctx); return;
    }
    h->status.stable_pattern=in.route.pattern;
    float forward=0, turn=0;
    if (!in.route.pattern) {
        if (h->status.fsm==LINE_FOLLOW_SEARCH || fabsf(h->status.last_track_error_mm)<0.05f) {
            line_fault(h,LINE_FOLLOW_REJECT_LOST,now); return;
        }
        if (h->status.fsm!=LINE_FOLLOW_RECOVER) { h->status.fsm=LINE_FOLLOW_RECOVER; h->status.state_started_ms=now; }
        if ((uint32_t)(now-h->status.state_started_ms)>=h->config.recover_timeout_ms) {
            line_fault(h,LINE_FOLLOW_REJECT_LOST,now); return;
        }
        /* Negative lateral error means line on LEFT: positive CCW command. */
        turn=h->status.last_track_error_mm<0 ? h->config.recover_turn_percent : -h->config.recover_turn_percent;
    } else {
        float dt=h->status.last_update_ms ? (now-h->status.last_update_ms)*0.001f : 0.01f;
        dt=clampf(dt,0.005f,0.1f);
        float error=-in.route.lateralErrorMm;
        h->filtered_derivative+=dt/(0.03f+dt)*((error-h->previous_error)/dt-h->filtered_derivative);
        h->integral=clampf(h->integral+error*dt,-20,20);
        turn=h->config.kp*error+h->config.ki*h->integral+h->config.kd*h->filtered_derivative-
            h->config.yaw_rate_damping*h->status.yaw_rate_dps;
        forward=h->config.forward_percent;
        if (!in.route.centerBlack) forward*=0.35f;
        if (fabsf(in.route.lateralErrorMm)>0.05f) h->status.last_track_error_mm=in.route.lateralErrorMm;
        h->previous_error=error; h->status.fsm=LINE_FOLLOW_TRACK;
    }
    turn=clampf(turn,-h->config.max_turn_percent,h->config.max_turn_percent);
    if (!h->config.command(h->config.ctx,forward,turn,50u,now)) { line_fault(h,LINE_FOLLOW_REJECT_MOTION,now); return; }
    h->status.forward_percent=forward; h->status.turn_percent=turn; h->status.last_update_ms=now;
}
const LineFollow_Status_t *LineFollowControl_GetStatus(const LineFollowControl_Handle *h) { return h?&h->status:0; }
const char *LineFollowControl_GetFsmText(LineFollow_Fsm_t f) {
    switch(f) { case LINE_FOLLOW_TRACK:return "TRACK"; case LINE_FOLLOW_RECOVER:return "RECOVER";
        case LINE_FOLLOW_FAULT:return "FAULT"; default:return "SEARCH"; }
}
