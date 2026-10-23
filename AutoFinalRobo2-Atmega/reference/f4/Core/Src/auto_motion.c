#include "auto_motion.h"
#include <math.h>
#include <string.h>
static float clamp(float v,float lo,float hi) { return fminf(hi,fmaxf(lo,v)); }
static float angle(float a) { while(a>180) a-=360; while(a<-180) a+=360; return a; }
static uint32_t u32(const uint8_t *p) { return (uint32_t)p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24); }
static uint16_t u16(const uint8_t *p) { return p[0]|((uint16_t)p[1]<<8); }
static float f32(const uint8_t *p) { uint32_t v=u32(p);float f;memcpy(&f,&v,4);return f; }
static void put32(uint8_t *p,uint32_t v) { for(unsigned i=0;i<4;++i)p[i]=(uint8_t)(v>>(8*i)); }
static void putf(uint8_t *p,float f) { uint32_t v;memcpy(&v,&f,4);put32(p,v); }
bool AutoMotion_Busy(const AutoMotion_Handle *h) { return h->state==1 || h->state==2; }
static void end(AutoMotion_Handle *h,uint8_t state,uint8_t reason) { h->state=state;h->reason=reason;h->forward=0;h->applied_forward=0;h->port.hold(h->port.ctx); }
void AutoMotion_Cancel(AutoMotion_Handle *h) { if(AutoMotion_Busy(h)) end(h,4,AUTO_CANCELLED); }
void AutoMotion_Init(AutoMotion_Handle *h,const AutoMotion_Port *p,const AutoMotion_Profile *profile) {
 memset(h,0,sizeof(*h));h->port=*p;if(profile)h->profile=*profile;h->persisted=1;
}
static uint8_t ready(AutoMotion_Handle *h,bool calibration) {
 h->port.read(h->port.ctx,&h->input);AutoMotion_Input *i=&h->input;
 if(AutoMotion_Busy(h))return AUTO_BUSY;
 if(i->mode!=3 && (!calibration || i->mode!=0))return AUTO_MODE;
 if(!i->mpu_ready || !i->mpu_calibrated)return AUTO_MPU;
 if(!i->armed || !i->balance || !i->settled || i->safety)return AUTO_BALANCE;
 if(calibration && (!i->line_ready || !i->line_fresh || !i->center_black))return AUTO_LINE;
 if(!calibration && !h->profile.count)return AUTO_MOTOR_PROFILE;
 if(!h->profile.ir_valid || !i->objects_fresh || !i->objects_enabled)return AUTO_IR_PROFILE;
 return 0;
}
static bool obstacles(AutoMotion_Handle *h) {
 h->obstacle_mask=0;
 for(unsigned c=0;c<5;++c) {
  const AutoMotion_IrChannel *cal=&h->profile.ir[c];float raw=h->input.objects[c];
  float margin=fmaxf(20,6.0f*cal->noise), nearest=100000;bool matched=false;
  for(unsigned j=1;j<cal->count;++j) {
   float a=cal->points[j-1].adc,b=cal->points[j].adc;
   if(raw>=fminf(a,b)-margin && raw<=fmaxf(a,b)+margin) {
    float t=fabsf(b-a)>1?clamp((raw-a)/(b-a),0,1):0;
    float mm=cal->points[j-1].distance_mm+t*((float)cal->points[j].distance_mm-cal->points[j-1].distance_mm);
    nearest=fminf(nearest,mm);matched=true;
   }
  }
  /* Multiple distance candidates use the nearest, never a fabricated clear path. */
  if(matched && nearest<=200)h->obstacle_mask|=(1u<<c);
  if(!matched && fabsf(raw-cal->baseline)>margin)return false;
 }
 return true;
}
static float model(const AutoMotion_Handle *h,float percent) {
 const AutoMotion_Profile *p=&h->profile;
 if(!p->count)return 0;
 if(percent<=0)return 0;
 if(percent<=p->points[0].percent)return p->points[0].speed_cm_s*percent/p->points[0].percent;
 if(p->count==1)return p->points[0].speed_cm_s;
 for(unsigned j=1;j<p->count;++j)if(percent<=p->points[j].percent) {
  const AutoMotion_Point *a=&p->points[j-1],*b=&p->points[j];
  return a->speed_cm_s+(b->speed_cm_s-a->speed_cm_s)*(percent-a->percent)/(b->percent-a->percent);
 }
 return p->points[p->count-1].speed_cm_s;
}
static void begin(AutoMotion_Handle *h,uint8_t kind,float percent,uint32_t ramp,uint32_t timeout) {
 h->kind=kind;h->state=1;h->phase=0;h->reason=0;h->result_valid=0;
 if(++h->id==0)++h->id;
 h->started_ms=h->last_ms=h->input.now_ms;h->last_imu_seq=h->input.imu_seq;
 h->origin_yaw=h->input.yaw_deg;h->heading=0;h->x=h->y=0;
 h->forward=percent;h->applied_forward=0;h->drive_elapsed=0;h->ramp_ms=ramp;h->timeout_ms=timeout;h->measured_ms=0;h->measure_ms=0;
 memset(h->active_ms,0,sizeof(h->active_ms));memset(h->forward_ms,0,sizeof(h->forward_ms));
 memset(h->reverse_ms,0,sizeof(h->reverse_ms));memset(h->cycles,0,sizeof(h->cycles));
 memset(h->duty_seconds,0,sizeof(h->duty_seconds));memset(h->max_duty,0,sizeof(h->max_duty));
 h->yaw_error_max=0;h->black_candidate=h->input.center_black;h->candidate_ms=h->input.now_ms;
 h->best_distance=100000;h->last_progress_ms=h->input.now_ms;
}
uint8_t AutoMotion_Goto(AutoMotion_Handle *h,float x,float y) {
 if(!isfinite(x)||!isfinite(y)||hypotf(x,y)>200 || hypotf(x,y)<10)return AUTO_ARGUMENT;
 uint8_t r=ready(h,false);if(r)return r;
 if(!obstacles(h))return AUTO_OBSTACLE;
 AutoMotion_Point *p=&h->profile.points[h->profile.count/2];
 begin(h,0,p->percent,p->ramp_ms,30000);h->goal_x=x;h->goal_y=y;return 0;
}
static bool save(AutoMotion_Handle *h,const AutoMotion_Profile *next) {
 if(h->input.armed || AutoMotion_Busy(h))return false; /* Flash cannot stall a balancing vehicle. */
 if(!h->port.save(h->port.ctx,next))return false;
 h->profile=*next;h->persisted=1;return true;
}
uint8_t AutoMotion_Calibrate(AutoMotion_Handle *h,uint8_t op,float percent,uint32_t ramp,uint32_t timeout) {
 h->port.read(h->port.ctx,&h->input);
 if(op==1){AutoMotion_Cancel(h);return 0;}
 if(op==2 || op==3) {
  if(!h->result_valid || h->state!=3 || !h->measured_ms)return AUTO_ARGUMENT;
  if(h->input.armed)return AUTO_SAVE;
  AutoMotion_Profile next=h->profile;float v=100000.0f/h->measured_ms;unsigned n=0;
  for(;n<next.count;++n)if(fabsf(next.points[n].percent-h->speed)<0.01f)break;
  if(n==next.count){if(n>=AUTO_MOTION_POINTS)return AUTO_ARGUMENT;++next.count;next.points[n]=(AutoMotion_Point){h->speed,v,h->ramp_ms,1};}
  else if(op==3 || next.points[n].ramp_ms!=h->ramp_ms)next.points[n]=(AutoMotion_Point){h->speed,v,h->ramp_ms,1};
  else { AutoMotion_Point *p=&next.points[n];p->speed_cm_s=(p->speed_cm_s*p->runs+v)/(p->runs+1);++p->runs; }
  for(unsigned a=0;a<next.count;++a)for(unsigned b=a+1;b<next.count;++b)if(next.points[b].percent<next.points[a].percent){AutoMotion_Point p=next.points[a];next.points[a]=next.points[b];next.points[b]=p;}
  ++next.revision;if(!save(h,&next))return AUTO_SAVE;h->result_valid=0;return 0;
 }
 if(op!=0 || !isfinite(percent)||percent<1||percent>20||ramp>5000||timeout<1000||timeout>30000)return AUTO_ARGUMENT;
 uint8_t r=ready(h,true);if(r)return r;if(!obstacles(h)||h->obstacle_mask)return AUTO_OBSTACLE;
 begin(h,1,percent,ramp,timeout);h->speed=percent;h->goal_x=0;h->goal_y=100;return 0;
}
void AutoMotion_Task(AutoMotion_Handle *h) {
 if(!AutoMotion_Busy(h))return;
 h->port.read(h->port.ctx,&h->input);AutoMotion_Input *i=&h->input;
 uint32_t dt=i->now_ms-h->last_ms;h->last_ms=i->now_ms;
 if(!i->mpu_ready||!i->mpu_calibrated){end(h,5,AUTO_MPU);return;}
 if(!i->armed||!i->balance||i->safety){end(h,5,AUTO_BALANCE);return;}
 if((h->kind==0&&i->mode!=3)||(h->kind==1&&i->mode!=0&&i->mode!=3)){end(h,5,AUTO_MODE);return;}
 if(i->now_ms-h->started_ms>=h->timeout_ms){end(h,5,AUTO_TIMEOUT);return;}
 if(!i->objects_fresh||!i->objects_enabled||!obstacles(h)){end(h,5,AUTO_OBSTACLE);return;}
 if(dt==0||dt>50){h->port.hold(h->port.ctx);return;}
 /* One coherent IMU sequence per orientation update; distance is calibrated feedforward. */
 if(i->imu_seq!=h->last_imu_seq){h->heading=angle(i->yaw_deg-h->origin_yaw);h->last_imu_seq=i->imu_seq;}
 float forward=0,turn=0;
 if(h->kind==1) {
  if(!i->line_fresh||!i->line_ready){end(h,5,AUTO_LINE);return;}
  if(h->obstacle_mask){end(h,5,AUTO_OBSTACLE);return;}
  h->yaw_error_max=fmaxf(h->yaw_error_max,fabsf(h->heading));
  if(h->yaw_error_max>12){end(h,5,AUTO_HEADING);return;}
  /* Debounce the central lower sensor independently of the FOLLOW 111 classifier. */
  if(h->black_candidate!=i->center_black){h->black_candidate=i->center_black;h->candidate_ms=i->now_ms;}
  else if(i->now_ms-h->candidate_ms>=20) {
   if(h->phase==0&&!i->center_black){h->phase=1;h->measure_ms=h->candidate_ms;}
   else if(h->phase==1&&i->center_black)h->phase=2;
   else if(h->phase==2&&!i->center_black){
    h->measured_ms=h->candidate_ms-h->measure_ms;
    if(h->measured_ms<500){end(h,5,AUTO_LINE);return;}
    h->result_valid=1;end(h,3,0);return;
   }
  }
  forward=h->speed*clamp((float)(i->now_ms-h->started_ms)/(h->ramp_ms?h->ramp_ms:1),0,1);
  turn=clamp(-h->heading*2,-40,40);
  if(h->phase==1||h->phase==2) {
   float duty[2]={i->left_duty,i->right_duty};
   for(unsigned c=0;c<2;++c) {
    float d=fabsf(duty[c]);h->duty_seconds[c]+=d*0.01f*dt*0.001f;h->max_duty[c]=fmaxf(h->max_duty[c],d);
    if(d>0){h->active_ms[c]+=dt;h->cycles[c]+=i->pwm_hz*dt*0.001f;if(duty[c]>0)h->forward_ms[c]+=dt;else h->reverse_ms[c]+=dt;}
   }
   h->measured_ms=i->now_ms-h->measure_ms;
  }
 } else {
  /* Integrate the prior accepted intention; PWM remains controlled by balance. */
  float v=model(h,h->applied_forward);h->speed=v;
  h->x-=sinf(h->heading*0.0174532925f)*v*dt*0.001f;h->y+=cosf(h->heading*0.0174532925f)*v*dt*0.001f;
  float distance=hypotf(h->goal_x-h->x,h->goal_y-h->y);
  if(distance<=10){end(h,3,0);return;}
  if(distance<h->best_distance-2){h->best_distance=distance;h->last_progress_ms=i->now_ms;}
  if(i->now_ms-h->last_progress_ms>8000){end(h,5,AUTO_OBSTACLE);return;}
  float target=atan2f(-(h->goal_x-h->x),h->goal_y-h->y)*57.2957795f;
  if(h->state==1 && (h->obstacle_mask&14)) {
   /* Sensor order: far left, left, front, right, far right. */
   bool left=(h->obstacle_mask&3)==0,right=(h->obstacle_mask&24)==0;
   if(!left&&!right){end(h,5,AUTO_OBSTACLE);return;}
   h->state=2;h->avoid_heading=angle(h->heading+(left?60:-60));h->avoid_x=h->x;h->avoid_y=h->y;h->avoid_since=i->now_ms;
  }
  if(h->state==2) {
   target=h->avoid_heading;
   if(i->now_ms-h->avoid_since>5000){end(h,5,AUTO_OBSTACLE);return;}
   if(hypotf(h->x-h->avoid_x,h->y-h->avoid_y)>=25 && !(h->obstacle_mask&14))h->state=1;
  }
  float error=angle(target-h->heading);turn=clamp(error*2,-65,65);
  if(fabsf(error)<15 && !(h->obstacle_mask&14)) {
   h->drive_elapsed+=dt;forward=h->forward*clamp((float)h->drive_elapsed/(h->ramp_ms?h->ramp_ms:1),0,1);
  } else h->drive_elapsed=0;
 }
 if(!h->port.command(h->port.ctx,forward,turn,i->now_ms))end(h,5,AUTO_MOTION_REJECTED);
 else h->applied_forward=forward;
}
uint8_t AutoMotion_SetIr(AutoMotion_Handle *h,const uint8_t *p,uint8_t len) {
 h->port.read(h->port.ctx,&h->input);
 if(len!=AUTO_MOTION_IR_BYTES||p[0]!=1)return AUTO_ARGUMENT;
 if(AutoMotion_Busy(h))return AUTO_BUSY;
 AutoMotion_Profile next=h->profile;
 for(unsigned c=0;c<5;++c){
  const uint8_t *q=p+1+c*37;AutoMotion_IrChannel *s=&next.ir[c];
  s->baseline=u16(q);s->noise=u16(q+2);s->count=q[4];
  if(s->count<2||s->count>8||s->baseline>4095||s->noise>4095)return AUTO_ARGUMENT;
  for(unsigned j=0;j<8;++j){s->points[j].distance_mm=u16(q+5+j*4);s->points[j].adc=u16(q+7+j*4);
   if(j<s->count&&(s->points[j].adc>4095||!s->points[j].distance_mm||(j&&s->points[j].distance_mm<=s->points[j-1].distance_mm)))return AUTO_ARGUMENT;}
 }
 next.ir_valid=1;++next.revision;return save(h,&next)?0:AUTO_SAVE;
}
void AutoMotion_GetProfile(AutoMotion_Handle *h,uint8_t *p,uint8_t reason) {
 memset(p,0,AUTO_MOTION_PROFILE_BYTES);p[0]=reason?1:0;p[1]=1;p[2]=h->profile.count;p[3]=h->profile.ir_valid;put32(p+4,h->profile.revision);
 for(unsigned n=0;n<AUTO_MOTION_POINTS;++n){const AutoMotion_Point *v=&h->profile.points[n];putf(p+8+n*16,v->percent);putf(p+12+n*16,v->speed_cm_s);put32(p+16+n*16,v->ramp_ms);put32(p+20+n*16,v->runs);}
}
uint8_t AutoMotion_SetProfile(AutoMotion_Handle *h,const uint8_t *p,uint8_t len) {
 h->port.read(h->port.ctx,&h->input);
 if(len!=AUTO_MOTION_PROFILE_BYTES||p[1]!=1||p[2]>AUTO_MOTION_POINTS)return AUTO_ARGUMENT;
 if(AutoMotion_Busy(h))return AUTO_BUSY;
 AutoMotion_Profile next=h->profile;next.count=p[2];
 for(unsigned n=0;n<next.count;++n){AutoMotion_Point *v=&next.points[n];v->percent=f32(p+8+n*16);v->speed_cm_s=f32(p+12+n*16);v->ramp_ms=u32(p+16+n*16);v->runs=u32(p+20+n*16);
  if(!isfinite(v->percent)||v->percent<1||v->percent>20||!isfinite(v->speed_cm_s)||v->speed_cm_s<=0||v->speed_cm_s>300||v->ramp_ms>5000||v->runs<1||v->runs>1000||(n&&v->percent<=next.points[n-1].percent))return AUTO_ARGUMENT;
 }
 ++next.revision;return save(h,&next)?0:AUTO_SAVE;
}
void AutoMotion_State(AutoMotion_Handle *h,uint8_t *p,uint8_t reason) {
 h->port.read(h->port.ctx,&h->input);AutoMotion_Input *i=&h->input;
 memset(p,0,AUTO_MOTION_STATE_BYTES);p[0]=reason?1:0;p[1]=1;p[2]=h->kind;p[3]=h->state;p[4]=reason?reason:h->reason;p[5]=i->mode;
 p[6]=(i->mpu_ready?1:0)|(i->mpu_calibrated?2:0)|(h->profile.count?4:0)|(h->profile.ir_valid?8:0)|(i->balance?16:0)|(i->settled?32:0)|(i->safety?64:0)|(i->line_ready?128:0);p[7]=h->obstacle_mask;
 put32(p+8,h->id);put32(p+12,h->state ? h->last_ms-h->started_ms:0);
 putf(p+16,h->x);putf(p+20,h->y);putf(p+24,h->goal_x);putf(p+28,h->goal_y);putf(p+32,h->heading);putf(p+36,h->kind==1&&h->measured_ms?100000.0f/h->measured_ms:h->speed);
 putf(p+40,h->kind==1?h->speed:h->forward);put32(p+44,h->ramp_ms);put32(p+48,h->measured_ms);putf(p+52,i->pwm_hz);
 for(unsigned c=0;c<2;++c){putf(p+56+c*4,h->measured_ms?h->duty_seconds[c]*100000/h->measured_ms:0);putf(p+64+c*4,h->max_duty[c]);putf(p+72+c*4,h->duty_seconds[c]);putf(p+80+c*4,h->cycles[c]);}
 put32(p+88,h->forward_ms[0]);put32(p+92,h->reverse_ms[0]);put32(p+96,h->forward_ms[1]);put32(p+100,h->reverse_ms[1]);
 p[104]=h->profile.count;p[105]=h->phase;p[106]=h->persisted;p[107]=h->result_valid;
 putf(p+108,100);putf(p+112,h->yaw_error_max);put32(p+116,h->measure_ms?h->measure_ms-h->started_ms:0);
 p[120]=i->objects_enabled;p[121]=i->lights;p[122]=i->objects_fresh;p[123]=i->line_fresh;put32(p+124,h->profile.revision);
}
