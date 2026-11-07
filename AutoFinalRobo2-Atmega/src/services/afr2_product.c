/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
/* AFR2-20261023-19. Own four-wheel application; F4 motion/PID/state ideas,
 * no balance/MPU dependency. All expensive work in main, no float in ISR.
 */
#include "afr2_product.h"
#include "afr2_motion_reference.h"
#include <string.h>
#include <math.h>
static int32_t abs32(int32_t x){return x==INT32_MIN?INT32_MAX:x<0?-x:x;}
static int16_t clamp(int32_t x,int16_t lim){return x>lim?lim:x<-lim?-lim:(int16_t)x;}
void Afr2Product_Defaults(Afr2Config *c) {
    /* AFR2-20261030-04: user requests full 8-bit PWM range by default. */
    memset(c,0,sizeof(*c));c->encoder_steps=4;c->contrast=0xcf;c->pwm_limit=255;c->ramp_step=4;
    /* AFR2-20261029-10: mounting verified with standalone diagnostic09. */
    c->encoder_invert=1;c->rotation=1;
    c->hall_wired=1; /* AFR2-20261029-15: user confirms wheel Hall wiring. */
    /* AFR2-20261030-03: user requests raw ADC10 acquisition on A4..A0.
     * This selects voltage sampling; it does not identify sensor output type.
     * Digital IR still requires explicit calibration; legacy threshold bytes are retained. */
    c->line_kind=1;c->line_invert=31;c->radar_trace=1;
    c->wheel_mm=AFR2_FIXED_WHEEL_MM;c->radius_sum_mm=AFR2_FIXED_RADIUS_SUM_MM;c->geometry_ready=1;
    c->long_press_ms=800;c->servo_inverted=0; /* nominal dimensions, CPR still required */
    c->speed_limit=150;c->test_ms=500;c->kp=128;c->ki=8;c->kd=0;c->line_kp=100;c->line_speed=80;
    for(uint8_t i=0;i<5;i++)c->line_threshold[i]=512;
    /* No assumed 11*gearbox conversion. Cpr filled only by measured calibration. */
}
uint8_t Afr2Product_ConfigValid(const Afr2Config *c) {
    if(c->long_press_ms<800 || c->long_press_ms>3000 || c->long_press_ms%100 || c->servo_inverted>1)return 0;
    if(c->radar_range>3 || c->radar_trace>1 || c->line_kind>1 || c->line_invert>31 || c->line_calibrated>1 || c->motor_signs>15 ||
       c->hall_cw_mask>15 || c->motor_ready>1 || c->hall_wired>1 || c->hall_signs>15 || c->geometry_ready>1 ||
       (c->encoder_steps!=2 && c->encoder_steps!=4) || c->encoder_invert>1 || c->rotation>1 ||
       !c->pwm_limit || !c->ramp_step || c->ramp_step>32 || c->wheel_mm<10 || c->wheel_mm>500 ||
       c->radius_sum_mm>1000 || c->speed_limit<10 || c->speed_limit>1000 ||
       c->test_ms<100 || c->test_ms>2000 || c->kp>2048 || c->ki>1024 || c->kd>2048 ||
       c->line_speed>c->speed_limit || c->line_kp>1000)return 0;
    if(c->geometry_ready && !c->radius_sum_mm)return 0;
    for(uint8_t i=0;i<5;i++)if(c->line_threshold[i]>1023)return 0;
    for(uint8_t i=0;i<4;i++)if(c->counts_per_rev[i]>1000000UL)return 0;
    return 1;
}
void Afr2Product_Init(Afr2Product *p,uint8_t reset) {
    memset(p,0,sizeof(*p));p->reset_cause=reset;p->stopped=1;
    Afr2Prefs_Load(&p->config,&p->prefs_loaded);Afr2Motor_Init();Afr2Sensors_Init();
}
uint8_t Afr2Product_Stop(void *ctx) {
    Afr2Head_Stop();return Afr2Product_HaltMotion(ctx);
}
uint8_t Afr2Product_HaltMotion(void *ctx) {
    Afr2Product *p=ctx;uint8_t was_stopped=p->stopped;Afr2Head_CancelResume();Afr2Motor_Stop();
    if(p->sequence.state>=1 && p->sequence.state<=3)p->sequence.state=5;
    memset(p->pwm,0,sizeof(p->pwm));memset(p->target,0,sizeof(p->target));
    memset(p->integral,0,sizeof(p->integral));memset(p->last_error,0,sizeof(p->last_error));
    p->stopped=1;p->mode=p->fault?AFR2_FAULT:AFR2_IDLE;p->origin=AFR2_SOURCE_NONE;p->local_timeout_ms=0;p->web_seen=0;
    /* AFR2-20261106-34: una detencion de enlace/ESP con el auto ya detenido no es movimiento que abortar.
     * Antes borraba la medicion de pulsos por vuelta (la rueda se gira a mano) y la pantalla quedaba en 0. */
    if(p->hall_measuring && !was_stopped){p->hall_measuring=0;p->hall_bad=1;p->hall_delta=0;}
    if(p->goto_state==1)p->goto_state=3;
    return 1; /* Confirms output/consigna shutdown, not measured zero wheel speed. */
}
void Afr2Product_Snapshot(void *ctx,uint32_t now,Afr2LinkSnapshot *s) {
    Afr2Product *p=ctx;s->mode=p->mode>AFR2_FAULT?AFR2_MANUAL:p->mode;s->stopped=p->stopped;s->state_age_ms=now-p->now;
    s->line_valid=p->sensors.line_valid;s->line_bits=p->sensors.line_bits;s->line_age_ms=now-p->sensors.line_ms;
    s->ultrasonic_valid=p->sensors.ultrasonic_valid;s->distance_mm=p->sensors.distance_mm;
    s->ultrasonic_age_ms=now-p->sensors.ultrasonic_ms;s->encoders_wired=p->sensors.hall_available;
    s->encoders_valid=p->sensors.hall_available;s->encoders_age_ms=s->encoders_valid?now-p->sensors.hall_ms:AFR2_AGE_UNKNOWN;
    memcpy(s->counts,p->sensors.counts,sizeof(s->counts));
}
/* Liveness observation only: never grants a permission or starts outputs. */
void Afr2Product_WebPulse(Afr2Product *p,uint16_t age) {
    p->web_seen=1;p->web_ms=p->now-age;
}
static uint8_t calibrated(const Afr2Product *p) {
    if(!p->sensors.hall_available || !p->config.geometry_ready)return 0;
    for(uint8_t i=0;i<4;i++)if(!p->config.counts_per_rev[i])return 0;
    return 1;
}
uint8_t Afr2Product_Drive(Afr2Product *p,int16_t vx,int16_t vy,int16_t wz,uint16_t ttl) {
    if(Afr2Head_State()->running)return AFR2_BUSY;
    if(p->fault || p->hall_measuring)return AFR2_DENIED;
    if(!p->config.motor_ready || !calibrated(p))return AFR2_UNAVAILABLE;
    /* AFR2-20261106-13: current motion modes do not require HC-SR04. */
    if(ttl<200 || ttl>1000 || abs32(vx)>p->config.speed_limit || abs32(vy)>p->config.speed_limit || abs32(wz)>3140)return AFR2_BAD;
    /* AFR2-20261030-13: logical chassis axes -> existing wheel/Hall basis. */
    vx*=AFR2_FORWARD_WHEEL_SIGN;vy*=AFR2_LATERAL_WHEEL_SIGN;
    int32_t turn=(int32_t)wz*p->config.radius_sum_mm/1000*AFR2_YAW_WHEEL_SIGN;
    int32_t v[4]={vx-vy-turn,vx+vy+turn,vx+vy-turn,vx-vy+turn},peak=1;
    for(uint8_t i=0;i<4;i++)if(abs32(v[i])>peak)peak=abs32(v[i]);
    for(uint8_t i=0;i<4;i++)p->target[i]=(int16_t)(peak>p->config.speed_limit?v[i]*p->config.speed_limit/peak:v[i]);
    if(p->mode==AFR2_GOTO && p->goto_state==1)p->goto_state=3;
    p->mode=AFR2_MANUAL;p->command_ms=p->now;p->local_started_ms=p->now;p->test_started_ms=ttl;p->stopped=0;return AFR2_OK;
}
/* AFR2-20261029-27: explicit open-loop percentages, independent of odometry/HC. */
uint8_t Afr2Product_DrivePwm(Afr2Product *p,int16_t forward,int16_t strafe,int16_t turn,uint16_t ttl) {
    if(ttl<200 || ttl>1000 || abs32(forward)>100 || abs32(strafe)>100 || abs32(turn)>100)return AFR2_BAD;
    /* AFR2-20261106: Conducir manual omite permisos (cabeza, secuencia, Hall, fallo, motores, prefs):
     * se anula lo que estorbe, se habilitan motores y se limpia el fallo. STOP y TTL siguen vigentes. */
    if(Afr2Head_State()->running || p->fault || p->hall_measuring || (p->sequence.state>=1 && p->sequence.state<=3))Afr2Product_Stop(p);
    p->fault=0;p->config.motor_ready=1;
    /* Same reference as closed-loop drive; never invert again in Web/ESP. */
    forward*=AFR2_FORWARD_WHEEL_SIGN;strafe*=AFR2_LATERAL_WHEEL_SIGN;turn*=AFR2_YAW_WHEEL_SIGN;
    int16_t v[4]={forward-strafe-turn,forward+strafe+turn,forward+strafe-turn,forward-strafe+turn},peak=100;
    for(uint8_t i=0;i<4;i++)if(abs32(v[i])>peak)peak=abs32(v[i]);
    if(p->stopped || p->mode!=AFR2_PWM_MANUAL){
        Afr2Motor_Stop();memset(p->pwm,0,sizeof(p->pwm));
        memset(p->integral,0,sizeof(p->integral));memset(p->last_error,0,sizeof(p->last_error));
        for(uint8_t i=0;i<4;i++)p->hall_change_ms[i]=p->now;
    }
    for(uint8_t i=0;i<4;i++)p->target[i]=(int32_t)v[i]*p->config.pwm_limit/peak;
    if(p->goto_state==1)p->goto_state=3;
    p->mode=AFR2_PWM_MANUAL;p->command_ms=p->local_started_ms=p->now;p->test_started_ms=ttl;p->stopped=0;return AFR2_OK;
}
uint8_t Afr2Product_Wheel(Afr2Product *p,uint8_t wheel,int16_t pwm,uint16_t duration) {
    if(Afr2Head_State()->running)return AFR2_BUSY;
    if(!p->config.motor_ready || p->prefs_state==1)return AFR2_UNAVAILABLE;
    if(wheel>3 || abs32(pwm)>p->config.pwm_limit || duration<100 || duration>2000)return AFR2_BAD;
    if(p->fault || p->hall_measuring)return AFR2_DENIED;
    Afr2Motor_Stop();memset(p->pwm,0,sizeof(p->pwm));memset(p->integral,0,sizeof(p->integral));memset(p->last_error,0,sizeof(p->last_error));
    if(p->goto_state==1)p->goto_state=3;
    memset(p->target,0,sizeof(p->target));p->target[wheel]=pwm;
    p->mode=AFR2_WHEEL_TEST;p->stopped=0;p->command_ms=p->now;p->test_started_ms=duration;return AFR2_OK;
}
uint8_t Afr2Product_StartLine(Afr2Product *p) {
    if(Afr2Head_State()->running)return AFR2_BUSY;
    if(p->fault || p->hall_measuring)return AFR2_DENIED;
    if(!p->config.motor_ready || !p->sensors.line_valid || !calibrated(p))return AFR2_UNAVAILABLE;
    if(p->goto_state==1)p->goto_state=3;
    p->mode=AFR2_LINE;p->stopped=0;p->command_ms=p->now;return AFR2_OK;
}
uint8_t Afr2Product_StartGoto(Afr2Product *p,int32_t x,int32_t y) {
    return Afr2Product_StartGotoFull(p,x,y,0,p->config.speed_limit);
}
uint8_t Afr2Product_StartGotoFull(Afr2Product *p,int32_t x,int32_t y,int16_t theta,uint16_t speed) {
    if(Afr2Head_State()->running)return AFR2_BUSY;
    if(abs32(x)>5000 || abs32(y)>5000 || abs32(theta)>3140 || (!x && !y && !theta) ||
       speed<10 || speed>p->config.speed_limit)return AFR2_BAD;
    if(p->fault || p->hall_measuring)return AFR2_DENIED;
    if(!p->config.motor_ready || !calibrated(p) || !p->pose_valid)return AFR2_UNAVAILABLE;
    /* X/Y are relative to robot heading when the mission starts. */
    float angle=(float)p->yaw_mrad/1000.0f;
    p->mission_x=p->x_mm+(int32_t)(x*cosf(angle)-y*sinf(angle));
    p->mission_y=p->y_mm+(int32_t)(x*sinf(angle)+y*cosf(angle));
    p->mission_yaw=p->yaw_mrad+theta;
    if(p->mission_yaw>3142)p->mission_yaw-=6283;
    if(p->mission_yaw<-3142)p->mission_yaw+=6283;
    p->mission_speed=speed;
    p->target_x=x;p->target_y=y;p->mission_started_ms=p->now;p->mode=AFR2_GOTO;
    p->goto_state=1;p->stopped=0;return AFR2_OK;
}
void Afr2Product_HallBegin(Afr2Product *p) {
    Afr2Product_Stop(p);
    p->hall_bad=0;p->hall_delta=0;
    if(!p->sensors.hall_available){p->notice=2;return;}
    p->hall_start=p->sensors.counts[p->selected_wheel];p->hall_measuring=1;
}
uint8_t Afr2Product_HallEnd(Afr2Product *p) {
    if(!p->hall_measuring)return AFR2_DENIED;
    p->hall_measuring=0;
    if(p->hall_bad || !p->hall_delta || abs32(p->hall_delta)>1000000){p->hall_bad=1;return AFR2_BAD;}
    return AFR2_OK;
}
void Afr2Product_HallSave(Afr2Product *p) {
    if(!p->hall_measuring && !p->hall_bad && p->hall_delta && abs32(p->hall_delta)<=1000000) {
        p->config.counts_per_rev[p->selected_wheel]=(uint32_t)abs32(p->hall_delta);p->notice=1;
        if(p->hall_delta<0)p->config.hall_signs^=(1u<<p->selected_wheel);
        p->pose_valid=0;p->odom_ready=0;p->hall_delta=0;
    } else p->notice=2;
}
void Afr2Product_Save(Afr2Product *p) {
    if(!p->stopped){p->notice=2;return;}
    if(Afr2Prefs_Save(&p->config)){p->prefs_state=1;p->notice=3;}else p->notice=2;
}
static int16_t ramp(int16_t value,int16_t wanted,uint8_t step) {
    if(value && wanted && ((value<0)!=(wanted<0)))wanted=0;
    if(value<wanted)return wanted-value<=step?wanted:value+step;
    if(value>wanted)return value-wanted<=step?wanted:value-step;
    return value;
}
/* Same basis as odometry: Hall signs already applied by sensors; calibrated right wheels
 * (exterior CW = forward) are negated so every wheel shares one sign convention. */
static int32_t normalized_delta(const Afr2Product *p,uint8_t i,int32_t delta) {
    if((p->config.hall_cw_mask&(1u<<i)) && (i&1u))delta=-delta;
    return delta;
}
/* AFR2-20261107-39: learn whether each wheel's PWM sign moves its Hall count the same way.
 * Only judged with clear drive (|pwm|>=60) and observed pulses; resets when any sign setting changes. */
static void track_sign(Afr2Product *p,uint8_t i,int32_t delta) {
    int8_t sign=p->pwm[i]>0?1:p->pwm[i]<0?-1:0;
    if(sign!=p->hp_last_sign[i]){p->hp_last_sign[i]=sign;p->hp_sign_ms[i]=(uint16_t)p->now;}
    /* Ignore the wheel still coasting the old way right after a reversal. */
    if(abs32(p->pwm[i])<60 || !delta || (uint16_t)((uint16_t)p->now-p->hp_sign_ms[i])<300u)return;
    int8_t s=p->hp_score[i];
    if((delta>0)==(p->pwm[i]>0)){if(s<20)++s;}else if(s>-20)--s;
    p->hp_score[i]=s;
}
uint8_t Afr2Product_HallPwmState(const Afr2Product *p,uint8_t wheel) {
    if(wheel>3)return 0;
    return p->hp_score[wheel]>=10?1:p->hp_score[wheel]<=-10?2:0;
}
static void odometry(Afr2Product *p,uint32_t dt) {
    float d[4];
    uint16_t signature=(uint16_t)p->config.motor_signs|((uint16_t)p->config.hall_signs<<4)|((uint16_t)p->config.hall_cw_mask<<8);
    if(signature!=p->hp_signature){p->hp_signature=signature;memset(p->hp_score,0,sizeof(p->hp_score));}
    if(!calibrated(p)){p->pose_valid=0;p->odom_ready=0;for(uint8_t i=0;i<4;i++){int32_t delta=(int32_t)((uint32_t)p->sensors.counts[i]-(uint32_t)p->previous_counts[i]);
        if(delta)p->hall_change_ms[i]=p->now;if(p->sensors.hall_available)track_sign(p,i,normalized_delta(p,i,delta));p->previous_counts[i]=p->sensors.counts[i];}return;}
    if(!p->odom_ready){
        p->odom_x=p->odom_y=p->odom_yaw=0;p->x_mm=p->y_mm=p->yaw_mrad=0;
        for(uint8_t i=0;i<4;i++)p->previous_counts[i]=p->sensors.counts[i];
        for(uint8_t i=0;i<4;i++)p->hall_change_ms[i]=p->now;
        ++p->pose_frame;if(!p->pose_frame)++p->pose_frame;p->pose_sequence=1;
        p->odom_ready=1;p->pose_valid=1;return;
    }
    for(uint8_t i=0;i<4;i++) {
        int32_t delta=(int32_t)((uint32_t)p->sensors.counts[i]-(uint32_t)p->previous_counts[i]);
        if(delta)p->hall_change_ms[i]=p->now;
        p->previous_counts[i]=p->sensors.counts[i];
        if(abs32(delta)>100000){p->pose_valid=0;p->fault=2;return;}
        /* Exterior CW is forward on right wheels, reverse on left.
         * Existing kinematic basis uses negative wheel speed for forward. */
        delta=normalized_delta(p,i,delta);track_sign(p,i,delta);
        d[i]=(float)delta*(float)p->config.wheel_mm*3.14159265f/(float)p->config.counts_per_rev[i];
        p->speed[i]=clamp((int32_t)(d[i]*1000.0f/(float)dt),2000);
    }
    float dx=(d[0]+d[1]+d[2]+d[3])*0.25f*AFR2_FORWARD_WHEEL_SIGN;
    float dy=(-d[0]+d[1]+d[2]-d[3])*0.25f*AFR2_LATERAL_WHEEL_SIGN;
    float angle=(-d[0]+d[1]-d[2]+d[3])*0.25f/(float)p->config.radius_sum_mm*AFR2_YAW_WHEEL_SIGN;
    float heading=p->odom_yaw+angle*0.5f;
    p->odom_x+=dx*cosf(heading)-dy*sinf(heading);p->odom_y+=dx*sinf(heading)+dy*cosf(heading);p->odom_yaw+=angle;
    while(p->odom_yaw>3.14159265f)p->odom_yaw-=6.2831853f;
    while(p->odom_yaw<-3.14159265f)p->odom_yaw+=6.2831853f;
    p->x_mm=(int32_t)p->odom_x;p->y_mm=(int32_t)p->odom_y;p->yaw_mrad=(int32_t)(p->odom_yaw*1000.0f);p->pose_valid=1;
    ++p->pose_sequence;if(!p->pose_sequence){++p->pose_frame;if(!p->pose_frame)++p->pose_frame;p->pose_sequence=1;}
}
void Afr2Product_Task(Afr2Product *p,uint32_t now) {
    p->now=now;Afr2Sensors_Task(&p->sensors,&p->config,now);
    uint8_t ps=Afr2Prefs_Task();if(p->prefs_state==1 && ps==2){p->prefs_state=2;p->notice=1;}else if(p->prefs_state==1 && ps==3){p->prefs_state=3;p->notice=2;}
    if(p->hall_measuring) {
        int32_t d=(int32_t)((uint32_t)p->sensors.counts[p->selected_wheel]-(uint32_t)p->hall_start);
        if(p->hall_delta && d && ((d<0)!=(p->hall_delta<0)))p->hall_bad=1;
        /* Detect direction reversal even before net count crosses zero. */
        if((p->hall_delta>0 && d<p->hall_delta)||(p->hall_delta<0 && d>p->hall_delta))p->hall_bad=1;
        p->hall_delta=d;
    }
    if(p->origin==AFR2_SOURCE_LOCAL && !p->stopped && (uint32_t)(now-p->local_started_ms)>p->local_timeout_ms)Afr2Product_Stop(p);
    if(p->origin==AFR2_SOURCE_WEB && !p->stopped && (!p->web_seen || (uint32_t)(now-p->web_ms)>=1500u))Afr2Product_Stop(p);
    if((p->mode==AFR2_MANUAL || p->mode==AFR2_PWM_MANUAL || p->mode==AFR2_WHEEL_TEST) &&
       (uint32_t)(now-p->command_ms)>=p->test_started_ms)Afr2Product_Stop(p);
    if(!p->stopped && !p->config.motor_ready && p->mode!=AFR2_PWM_MANUAL)Afr2Product_HaltMotion(p);
    if(p->fault){Afr2Product_Stop(p);return;}
    Afr2Sequence_Tick(p);
    uint32_t dt=now-p->control_ms;if(dt<10u)return;
    p->control_ms=now;if(dt>100u && !p->stopped){p->fault=1;Afr2Product_Stop(p);return;}
    odometry(p,dt);
    if(p->fault){Afr2Product_Stop(p);return;}
    if(!p->stopped && p->mode!=AFR2_WHEEL_TEST && p->mode!=AFR2_PWM_MANUAL && !calibrated(p)){p->fault=2;Afr2Product_Stop(p);return;}
    if(p->mode==AFR2_LINE) {
        if(!p->sensors.line_valid || !p->sensors.line_bits){Afr2Product_Stop(p);p->notice=2;return;}
        /* AFR2-20261107-43: IR1 (bit0, A4) esta a la izquierda (confirmado por el usuario). Linea a la
         * izquierda => error positivo => wz positivo = giro antihorario (hacia la linea). */
        int16_t error=0,n=0;for(uint8_t i=0;i<5;i++)if(p->sensors.line_bits&(1u<<i)){error+=2-(int16_t)i;n++;}
        uint8_t mode=p->mode;uint32_t local_timeout_ms=p->local_started_ms;
        if(Afr2Product_Drive(p,p->config.line_speed,0,(int16_t)((int32_t)error*p->config.line_kp/n),500)!=AFR2_OK)Afr2Product_Stop(p);
        else p->mode=mode;
        p->local_started_ms=local_timeout_ms;
    }
    if(p->mode==AFR2_GOTO) {
        if(!p->pose_valid || (uint32_t)(now-p->mission_started_ms)>60000u){p->goto_state=4;Afr2Product_Stop(p);p->notice=2;return;}
        int32_t ex=p->mission_x-p->x_mm,ey=p->mission_y-p->y_mm;
        int32_t eyaw=p->mission_yaw-p->yaw_mrad;
        if(eyaw>3142)eyaw-=6283;
        if(eyaw<-3142)eyaw+=6283;
        uint8_t pure_turn=!p->target_x && !p->target_y;
        if(((abs32(ex)<=10 && abs32(ey)<=10) || pure_turn) && abs32(eyaw)<=25){p->goto_state=2;Afr2Product_Stop(p);p->notice=1;return;}
        float angle=(float)p->yaw_mrad/1000.0f;
        int16_t vx=clamp((int32_t)(ex*cosf(angle)+ey*sinf(angle)),p->mission_speed);
        int16_t vy=clamp((int32_t)(-ex*sinf(angle)+ey*cosf(angle)),p->mission_speed);
        uint32_t angular_limit=(uint32_t)p->mission_speed*1000u/p->config.radius_sum_mm;
        if(angular_limit>3140u)angular_limit=3140u;
        int16_t wz=clamp(eyaw,(int16_t)angular_limit);
        if((abs32(ex)<=10 && abs32(ey)<=10) || (!p->target_x && !p->target_y))vx=vy=0; /* pure turn: never translate */
        /* AFR2-20261107-39: the P law commands ~error mm/s; near the goal that falls under the motors'
         * dead band and the car stalls until the 60 s timeout. Keep a minimum while outside tolerance. */
        {
            const int32_t minimum=p->mission_speed<40u?p->mission_speed:40;
            int32_t linear=abs32(vx)+abs32(vy);
            if(linear && linear<minimum){vx=(int16_t)((int32_t)vx*minimum/linear);vy=(int16_t)((int32_t)vy*minimum/linear);}
            int32_t wmin=minimum*1000/(int32_t)p->config.radius_sum_mm;
            if(!linear && abs32(eyaw)>25 && abs32(wz)<wmin)wz=(int16_t)(eyaw>0?wmin:-wmin);
        }
        int32_t peak=abs32(vx)+abs32(vy)+(int32_t)abs32(wz)*p->config.radius_sum_mm/1000;
        if(peak>p->mission_speed){vx=(int32_t)vx*p->mission_speed/peak;vy=(int32_t)vy*p->mission_speed/peak;wz=(int32_t)wz*p->mission_speed/peak;}
        uint32_t local_timeout_ms=p->local_started_ms;
        if(Afr2Product_Drive(p,vx,vy,wz,500)!=AFR2_OK){Afr2Product_Stop(p);return;}
        p->local_started_ms=local_timeout_ms;p->mode=AFR2_GOTO;p->goto_state=1;
    }
    /* AFR2-20261106-13: HC-SR04 is telemetry, not a motion stop condition. */
    /* AFR2-20261107-39: closed loop with a crossed PWM/Hall sign is positive feedback (the wheel
     * runs away at full PWM). Stop instead; open-loop PWM and wheel test only report it. */
    if(!p->stopped && p->mode!=AFR2_WHEEL_TEST && p->mode!=AFR2_PWM_MANUAL)
        for(uint8_t i=0;i<4;i++)if(p->hp_score[i]<=-10){p->fault=AFR2_FAULT_SIGN_MISMATCH;Afr2Product_Stop(p);return;}
    for(uint8_t i=0;i<4;i++) {
        int16_t wanted=p->target[i];
        if(p->mode!=AFR2_WHEEL_TEST && p->mode!=AFR2_PWM_MANUAL && !p->stopped) {
            if(!p->target[i]){p->integral[i]=0;p->last_error[i]=0;wanted=0;}
            else {
            int16_t error=clamp((int32_t)p->target[i]-p->speed[i],2000);
            p->integral[i]+=error;
            if(p->integral[i]>32767)p->integral[i]=32767;
            if(p->integral[i]<-32767)p->integral[i]=-32767;
            int32_t output=(int32_t)p->config.kp*error+(int32_t)p->config.ki*p->integral[i]+
                (int32_t)p->config.kd*(error-p->last_error[i]);p->last_error[i]=error;
            wanted=clamp(output/256,p->config.pwm_limit);
            }
        }
        p->pwm[i]=ramp(p->pwm[i],p->stopped?0:wanted,p->config.ramp_step);
        if(p->stopped || p->mode==AFR2_WHEEL_TEST || p->mode==AFR2_PWM_MANUAL || abs32(p->target[i])<20 || abs32(p->pwm[i])<40)p->hall_change_ms[i]=now;
        else if((uint32_t)(now-p->hall_change_ms[i])>500u){p->fault=3;Afr2Product_Stop(p);return;}
    }
    Afr2Motor_Write(p->pwm,p->config.motor_signs);
}
