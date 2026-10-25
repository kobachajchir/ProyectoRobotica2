/* AFR2-20261023-19. Own four-wheel application; F4 motion/PID/state ideas,
 * no balance/MPU dependency. All expensive work in main, no float in ISR.
 */
#include "afr2_product.h"
#include <string.h>
#include <math.h>
static int32_t abs32(int32_t x){return x==INT32_MIN?INT32_MAX:x<0?-x:x;}
static int16_t clamp(int32_t x,int16_t lim){return x>lim?lim:x<-lim?-lim:(int16_t)x;}
void Afr2Product_Defaults(Afr2Config *c) {
    memset(c,0,sizeof(*c));c->encoder_steps=4;c->contrast=0xcf;c->pwm_limit=80;c->ramp_step=4;
    c->wheel_mm=80;c->radius_sum_mm=0; /* Geometry not calibrated. */
    c->speed_limit=150;c->test_ms=500;c->kp=128;c->ki=8;c->kd=0;c->line_kp=100;c->line_speed=80;
    for(uint8_t i=0;i<5;i++)c->line_threshold[i]=512;
    /* No assumed 11*gearbox conversion. Cpr filled only by measured calibration. */
}
uint8_t Afr2Product_ConfigValid(const Afr2Config *c) {
    if(c->line_kind>2 || c->line_invert>31 || c->line_calibrated>1 || c->motor_signs>15 ||
       c->motor_ready>1 || c->hall_wired>1 || c->hall_signs>15 || c->geometry_ready>1 ||
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
    Afr2Product *p=ctx;Afr2Motor_Stop();
    memset(p->pwm,0,sizeof(p->pwm));memset(p->target,0,sizeof(p->target));
    memset(p->integral,0,sizeof(p->integral));memset(p->last_error,0,sizeof(p->last_error));
    p->stopped=1;p->mode=p->fault?AFR2_FAULT:AFR2_IDLE;p->owner=AFR2_NONE;p->owner_token=0;p->lease_ms=0;
    if(p->hall_measuring){p->hall_measuring=0;p->hall_bad=1;p->hall_delta=0;}
    if(p->goto_state==1)p->goto_state=3;
    return 1; /* Confirms output/consigna shutdown, not measured zero wheel speed. */
}
void Afr2Product_Snapshot(void *ctx,uint32_t now,Afr2LinkSnapshot *s) {
    Afr2Product *p=ctx;s->mode=p->mode;s->stopped=p->stopped;s->state_age_ms=now-p->now;
    s->line_valid=p->sensors.line_valid;s->line_bits=p->sensors.line_bits;s->line_age_ms=now-p->sensors.line_ms;
    s->ultrasonic_valid=p->sensors.ultrasonic_valid;s->distance_mm=p->sensors.distance_mm;
    s->ultrasonic_age_ms=now-p->sensors.ultrasonic_ms;s->encoders_wired=p->sensors.hall_available;
    s->encoders_valid=p->sensors.hall_available;s->encoders_age_ms=s->encoders_valid?now-p->sensors.hall_ms:AFR2_AGE_UNKNOWN;
    memcpy(s->counts,p->sensors.counts,sizeof(s->counts));
}
uint8_t Afr2Product_Acquire(Afr2Product *p,uint8_t owner,uint32_t token,uint16_t ttl) {
    if(owner<AFR2_LOCAL || owner>AFR2_REMOTE || !token || ttl<500 || ttl>5000)return AFR2_BAD;
    if(p->fault || (p->hall_measuring && (p->owner!=owner || p->owner_token!=token)))return AFR2_DENIED;
    if(p->owner && (p->owner!=owner || p->owner_token!=token))return AFR2_DENIED;
    p->owner=owner;p->owner_token=token;p->lease_ms=ttl;p->lease_started_ms=p->now;return AFR2_OK;
}
static uint8_t calibrated(const Afr2Product *p) {
    if(!p->sensors.hall_available || !p->config.geometry_ready)return 0;
    for(uint8_t i=0;i<4;i++)if(!p->config.counts_per_rev[i])return 0;
    return 1;
}
uint8_t Afr2Product_Drive(Afr2Product *p,int16_t vx,int16_t vy,int16_t wz,uint16_t ttl) {
    if(!p->owner || p->fault || p->hall_measuring)return AFR2_DENIED;
    if(!p->config.motor_ready || !calibrated(p))return AFR2_UNAVAILABLE;
    if(ttl<200 || ttl>1000 || abs32(vx)>p->config.speed_limit || abs32(vy)>p->config.speed_limit || abs32(wz)>3140)return AFR2_BAD;
    int32_t turn=(int32_t)wz*p->config.radius_sum_mm/1000;
    int32_t v[4]={vx-vy-turn,vx+vy+turn,vx+vy-turn,vx-vy+turn},peak=1;
    for(uint8_t i=0;i<4;i++)if(abs32(v[i])>peak)peak=abs32(v[i]);
    for(uint8_t i=0;i<4;i++)p->target[i]=(int16_t)(peak>p->config.speed_limit?v[i]*p->config.speed_limit/peak:v[i]);
    p->mode=AFR2_MANUAL;p->command_ms=p->now;p->lease_started_ms=p->now;p->test_started_ms=ttl;p->stopped=0;return AFR2_OK;
}
uint8_t Afr2Product_Wheel(Afr2Product *p,uint8_t wheel,int16_t pwm,uint16_t duration) {
    if(wheel>3 || abs32(pwm)>p->config.pwm_limit || duration<100 || duration>2000)return AFR2_BAD;
    if(!p->owner || p->fault || p->hall_measuring)return AFR2_DENIED;
    memset(p->target,0,sizeof(p->target));p->target[wheel]=pwm;
    p->mode=AFR2_WHEEL_TEST;p->stopped=0;p->command_ms=p->now;p->test_started_ms=duration;return AFR2_OK;
}
uint8_t Afr2Product_StartLine(Afr2Product *p) {
    if(!p->owner || p->fault)return AFR2_DENIED;
    if(!p->config.motor_ready || !p->sensors.line_valid || !calibrated(p))return AFR2_UNAVAILABLE;
    p->mode=AFR2_LINE;p->stopped=0;p->command_ms=p->now;return AFR2_OK;
}
uint8_t Afr2Product_StartGoto(Afr2Product *p,int32_t x,int32_t y) {
    if(abs32(x)>5000 || abs32(y)>5000 || (!x && !y))return AFR2_BAD;
    if(!p->owner || p->fault)return AFR2_DENIED;
    if(!p->config.motor_ready || !calibrated(p) || !p->pose_valid)return AFR2_UNAVAILABLE;
    /* X/Y are relative to robot heading when the mission starts. */
    float angle=(float)p->yaw_mrad/1000.0f;
    p->mission_x=p->x_mm+(int32_t)(x*cosf(angle)-y*sinf(angle));
    p->mission_y=p->y_mm+(int32_t)(x*sinf(angle)+y*cosf(angle));
    p->target_x=x;p->target_y=y;p->mission_started_ms=p->now;p->mode=AFR2_GOTO;
    p->goto_state=1;p->stopped=0;return AFR2_OK;
}
void Afr2Product_HallBegin(Afr2Product *p) {
    uint8_t old_owner=p->owner;uint32_t token=p->owner_token;uint16_t ttl=p->lease_ms;
    Afr2Product_Stop(p);p->owner=old_owner;p->owner_token=token;
    p->lease_ms=ttl;p->lease_started_ms=p->now;
    p->hall_bad=0;p->hall_delta=0;
    if(!p->sensors.hall_available){p->notice=2;return;}
    p->hall_start=p->sensors.counts[p->selected_wheel];p->hall_measuring=1;
}
uint8_t Afr2Product_HallEnd(Afr2Product *p) {
    if(!p->hall_measuring)return AFR2_DENIED;
    p->hall_measuring=0;
    if(p->hall_bad || !p->hall_delta || abs32(p->hall_delta)>1000000)return AFR2_BAD;
    return AFR2_OK;
}
void Afr2Product_HallSave(Afr2Product *p) {
    if(!p->hall_measuring && !p->hall_bad && p->hall_delta) {
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
static void odometry(Afr2Product *p,uint32_t dt) {
    float d[4];
    if(!calibrated(p)){p->pose_valid=0;p->odom_ready=0;for(uint8_t i=0;i<4;i++)p->previous_counts[i]=p->sensors.counts[i];return;}
    if(!p->odom_ready){
        p->odom_x=p->odom_y=p->odom_yaw=0;p->x_mm=p->y_mm=p->yaw_mrad=0;
        for(uint8_t i=0;i<4;i++)p->previous_counts[i]=p->sensors.counts[i];
        for(uint8_t i=0;i<4;i++)p->hall_change_ms[i]=p->now;
        p->odom_ready=1;p->pose_valid=1;return;
    }
    for(uint8_t i=0;i<4;i++) {
        int32_t delta=(int32_t)((uint32_t)p->sensors.counts[i]-(uint32_t)p->previous_counts[i]);
        if(delta)p->hall_change_ms[i]=p->now;
        p->previous_counts[i]=p->sensors.counts[i];
        if(abs32(delta)>100000){p->pose_valid=0;p->fault=2;return;}
        d[i]=(float)delta*(float)p->config.wheel_mm*3.14159265f/(float)p->config.counts_per_rev[i];
        p->speed[i]=clamp((int32_t)(d[i]*1000.0f/(float)dt),2000);
    }
    float dx=(d[0]+d[1]+d[2]+d[3])*0.25f;
    float dy=(-d[0]+d[1]+d[2]-d[3])*0.25f;
    float angle=(-d[0]+d[1]-d[2]+d[3])*0.25f/(float)p->config.radius_sum_mm;
    float heading=p->odom_yaw+angle*0.5f;
    p->odom_x+=dx*cosf(heading)-dy*sinf(heading);p->odom_y+=dx*sinf(heading)+dy*cosf(heading);p->odom_yaw+=angle;
    while(p->odom_yaw>3.14159265f)p->odom_yaw-=6.2831853f;
    while(p->odom_yaw<-3.14159265f)p->odom_yaw+=6.2831853f;
    p->x_mm=(int32_t)p->odom_x;p->y_mm=(int32_t)p->odom_y;p->yaw_mrad=(int32_t)(p->odom_yaw*1000.0f);p->pose_valid=1;
}
void Afr2Product_Task(Afr2Product *p,uint32_t now) {
    p->now=now;Afr2Sensors_Task(&p->sensors,&p->config,now);
    uint8_t ps=Afr2Prefs_Task();if(p->prefs_state==1 && ps==2){p->prefs_state=2;p->notice=1;}
    if(p->hall_measuring) {
        int32_t d=(int32_t)((uint32_t)p->sensors.counts[p->selected_wheel]-(uint32_t)p->hall_start);
        if(p->hall_delta && d && ((d<0)!=(p->hall_delta<0)))p->hall_bad=1;
        /* Detect direction reversal even before net count crosses zero. */
        if((p->hall_delta>0 && d<p->hall_delta)||(p->hall_delta<0 && d>p->hall_delta))p->hall_bad=1;
        p->hall_delta=d;
    }
    if(p->owner && (uint32_t)(now-p->lease_started_ms)>p->lease_ms)Afr2Product_Stop(p);
    if((p->mode==AFR2_MANUAL || p->mode==AFR2_WHEEL_TEST) &&
       (uint32_t)(now-p->command_ms)>=p->test_started_ms)Afr2Product_Stop(p);
    if(p->fault){Afr2Product_Stop(p);return;}
    uint32_t dt=now-p->control_ms;if(dt<10u)return;
    p->control_ms=now;if(dt>100u && !p->stopped){p->fault=1;Afr2Product_Stop(p);return;}
    odometry(p,dt);
    if(!p->stopped && p->mode!=AFR2_WHEEL_TEST && !calibrated(p)){p->fault=2;Afr2Product_Stop(p);return;}
    if(p->mode==AFR2_LINE) {
        if(!p->sensors.line_valid || !p->sensors.line_bits){Afr2Product_Stop(p);p->notice=2;return;}
        int16_t error=0,n=0;for(uint8_t i=0;i<5;i++)if(p->sensors.line_bits&(1u<<i)){error+=(int16_t)i-2;n++;}
        uint8_t mode=p->mode;uint32_t lease_ms=p->lease_started_ms;
        if(Afr2Product_Drive(p,p->config.line_speed,0,(int16_t)(error*p->config.line_kp/n),500)!=AFR2_OK)Afr2Product_Stop(p);
        else p->mode=mode;
        p->lease_started_ms=lease_ms;
    }
    if(p->mode==AFR2_GOTO) {
        if(!p->pose_valid || (uint32_t)(now-p->mission_started_ms)>60000u){p->goto_state=4;Afr2Product_Stop(p);p->notice=2;return;}
        int32_t ex=p->mission_x-p->x_mm,ey=p->mission_y-p->y_mm;
        if(abs32(ex)<=10 && abs32(ey)<=10){p->goto_state=2;Afr2Product_Stop(p);p->notice=1;return;}
        float angle=(float)p->yaw_mrad/1000.0f;
        int16_t vx=clamp((int32_t)(ex*cosf(angle)+ey*sinf(angle)),p->config.speed_limit);
        int16_t vy=clamp((int32_t)(-ex*sinf(angle)+ey*cosf(angle)),p->config.speed_limit);
        uint32_t lease_ms=p->lease_started_ms;
        if(Afr2Product_Drive(p,vx,vy,0,500)!=AFR2_OK){Afr2Product_Stop(p);return;}
        p->lease_started_ms=lease_ms;p->mode=AFR2_GOTO;
    }
    /* Front-only obstacle condition. No claim of lateral/rear coverage. */
    if(!p->stopped && p->mode!=AFR2_WHEEL_TEST &&
       (!p->sensors.ultrasonic_valid || p->sensors.distance_mm<150)) {
        Afr2Product_Stop(p);p->notice=2;return;
    }
    for(uint8_t i=0;i<4;i++) {
        int16_t wanted=p->target[i];
        if(p->mode!=AFR2_WHEEL_TEST && !p->stopped) {
            int16_t error=clamp((int32_t)p->target[i]-p->speed[i],2000);
            p->integral[i]+=error;
            if(p->integral[i]>32767)p->integral[i]=32767;if(p->integral[i]<-32767)p->integral[i]=-32767;
            int32_t output=(int32_t)p->config.kp*error+(int32_t)p->config.ki*p->integral[i]+
                (int32_t)p->config.kd*(error-p->last_error[i]);p->last_error[i]=error;
            wanted=clamp(output/256,p->config.pwm_limit);
        }
        p->pwm[i]=ramp(p->pwm[i],p->stopped?0:wanted,p->config.ramp_step);
        if(p->stopped || p->mode==AFR2_WHEEL_TEST || abs32(p->target[i])<20 || abs32(p->pwm[i])<40)p->hall_change_ms[i]=now;
        else if((uint32_t)(now-p->hall_change_ms[i])>500u){p->fault=3;Afr2Product_Stop(p);return;}
    }
    Afr2Motor_Write(p->pwm,p->config.motor_signs);
}
