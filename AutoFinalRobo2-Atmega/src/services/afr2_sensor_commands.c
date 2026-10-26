/* AFR2-20261025-14. Same services as local encoder; read commands never move hardware. */
#include "afr2_app.h"
#include "afr2_product_wire.h"
static uint8_t result(uint8_t r){return r==AFR2_OK?AFR2_RESULT_OK:r==AFR2_BAD?AFR2_RESULT_BAD_REQUEST:AFR2_RESULT_UNAVAILABLE;}
static void servo(uint8_t *out,const Afr2SensorHead *h,uint32_t now){
    afr2_put_u32(out,Afr2Boot_Id());afr2_put_u32(out+4,h->servo_sequence);afr2_put_u16(out+8,h->pulse_us);
    afr2_put_u16(out+10,h->target_us);out[12]=h->angle;out[13]=h->enabled;afr2_put_u32(out+14,now-h->servo_ms);
}
uint8_t Afr2App_SensorCommand(void *ctx,uint8_t cmd,const uint8_t *d,uint8_t n,uint8_t *out,uint8_t *length){
    Afr2App *a=ctx;Afr2Product *p=a->product;const Afr2SensorHead *h=Afr2Head_State();uint8_t r=AFR2_OK;
    *length=0;
    switch(cmd){
    case AFR2_EXT_US_GET:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        afr2_put_u32(out,Afr2Boot_Id());afr2_put_u32(out+4,p->now);Afr2Head_Record(out+8,&h->latest);
        afr2_put_u16(out+24,h->interval_ms);out[26]=p->sensors.ultrasonic_valid;out[27]=h->angle;out[28]=h->enabled;
        afr2_put_u16(out+29,h->pulse_us);afr2_put_u16(out+31,h->target_us);out[33]=h->running;out[34]=h->scan_revision;*length=35;break;
    case AFR2_EXT_US_CONFIG:
        if(n!=2)return AFR2_RESULT_BAD_REQUEST;
        if(!p->stopped)return AFR2_RESULT_UNAVAILABLE;
        r=Afr2Head_Interval(afr2_get_u16(d));afr2_put_u16(out,h->interval_ms);*length=2;break;
    case AFR2_EXT_IR_GET:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        afr2_put_u32(out,Afr2Boot_Id());afr2_put_u32(out+4,h->ir_sequence);afr2_put_u32(out+8,h->ir_valid?p->now-p->sensors.line_ms:AFR2_AGE_UNKNOWN);
        out[12]=p->config.line_kind;out[13]=h->ir_valid;out[14]=p->config.line_calibrated;out[15]=p->sensors.line_bits;
        for(uint8_t i=0;i<5;i++)afr2_put_u16(out+16+2*i,p->sensors.line_raw[i]);
        out[26]=p->sensors.line_valid;*length=27;break;
    case AFR2_EXT_SERVO_SET:
        if(n!=2)return AFR2_RESULT_BAD_REQUEST;
        if(!p->stopped || p->fault || p->hall_measuring)return AFR2_RESULT_UNAVAILABLE;
        r=Afr2Head_Servo(d[0],d[1]);
        /* target is accepted now, applied pulse is reported only after Timer5 latches it. */
        servo(out,h,p->now);*length=18;break;
    case AFR2_EXT_SERVO_GET:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        servo(out,h,p->now);*length=18;break;
    case AFR2_EXT_RADAR_CONFIG:
        if(n!=9 || d[0]>1)return AFR2_RESULT_BAD_REQUEST;
        if(d[0]){if(!p->stopped || p->fault)return AFR2_RESULT_UNAVAILABLE;
            r=Afr2Head_Configure(d[1],d[2],d[3],afr2_get_u16(d+4),afr2_get_u16(d+6),d[8]);}
        out[0]=h->min_angle;out[1]=h->max_angle;out[2]=h->speed;afr2_put_u16(out+3,h->settle_ms);afr2_put_u16(out+5,h->interval_ms);out[7]=h->inverted;*length=8;break;
    case AFR2_EXT_RADAR_START:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        if(!p->stopped || p->fault || p->hall_measuring || (p->sequence.state>=1 && p->sequence.state<=3))return AFR2_RESULT_UNAVAILABLE;
        r=Afr2Head_Start();out[0]=h->running;*length=1;break;
    case AFR2_EXT_RADAR_STOP:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        Afr2Head_Stop();out[0]=0;*length=1;break;
    case AFR2_EXT_RADAR_GET:
        if(n!=5)return AFR2_RESULT_BAD_REQUEST;
        *length=Afr2Head_Read(d[0],afr2_get_u32(d+1),out,p->now,Afr2Boot_Id());if(!*length)return AFR2_RESULT_BAD_REQUEST;break;
    case AFR2_EXT_UI_STATUS:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        afr2_put_u32(out,Afr2Ui_ReportedScreen(a->ui));afr2_put_u32(out+4,a->ui->navigation_revision);
        out[8]=a->ui->selection;out[9]=a->ui->firstVisible;out[10]=Afr2App_UiPrivate(a);*length=11;break;
    case AFR2_EXT_MOTION_HALT:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        Afr2Product_HaltMotion(p);out[0]=1;*length=1;break;
    default:return AFR2_RESULT_UNAVAILABLE;
    }
    if(r){*length=0;return result(r);}return AFR2_RESULT_OK;
}
