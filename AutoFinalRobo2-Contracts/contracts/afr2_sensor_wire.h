/* AFR2-20261025-14. Sensor head EX01v8. Explicit LE bytes, never sizeof(struct). */
#ifndef AFR2_SENSOR_WIRE_H
#define AFR2_SENSOR_WIRE_H
#include "afr2_wire.h"
#define AFR2_EXT_US_GET       0x60u
#define AFR2_EXT_US_CONFIG    0x61u
#define AFR2_EXT_IR_GET       0x62u
#define AFR2_EXT_SERVO_GET    0x63u
#define AFR2_EXT_SERVO_SET    0x64u
#define AFR2_EXT_RADAR_CONFIG 0x65u
#define AFR2_EXT_RADAR_START  0x66u
#define AFR2_EXT_RADAR_STOP   0x67u
#define AFR2_EXT_RADAR_GET    0x68u
#define AFR2_EXT_UI_STATUS    0x69u
#define AFR2_EXT_MOTION_HALT  0x6au
#define AFR2_RADAR_BINS 37u
#define AFR2_RADAR_QUEUE 16u
#define AFR2_SAMPLE_BYTES 16u
enum { AFR2_ECHO_OK, AFR2_ECHO_NO_RISE, AFR2_ECHO_NO_FALL,
       AFR2_ECHO_HIGH, AFR2_ECHO_RANGE, AFR2_ECHO_MOVING, AFR2_ECHO_STALE, AFR2_ECHO_UNKNOWN };
static inline uint8_t afr2_sensor_record_valid(const uint8_t *p) {
    uint16_t pulse=afr2_get_u16(p+12),mm=afr2_get_u16(p+8);
    return p[14]<=180 && p[15]<=AFR2_ECHO_UNKNOWN && (!pulse || (pulse>=1000 && pulse<=2000)) &&
        (p[15]!=AFR2_ECHO_OK || (afr2_get_u32(p) && mm>=20 && mm<=4000));
}
/* p is body AFTER status/session/transaction; n excludes those 9 bytes. */
static inline uint8_t afr2_sensor_reply_valid(uint8_t cmd,const uint8_t *p,uint8_t n,const uint8_t *q,uint8_t qn) {
    switch(cmd) {
    case AFR2_EXT_US_GET:return qn==0 && n==35 && afr2_get_u32(p) && afr2_sensor_record_valid(p+8) &&
        afr2_get_u16(p+24)>=70 && afr2_get_u16(p+24)<=1000 && p[26]<=1 && p[27]<=180 && p[28]<=1 && p[33]<=1;
    case AFR2_EXT_US_CONFIG:return qn==2 && n==2 && afr2_get_u16(p)==afr2_get_u16(q);
    case AFR2_EXT_IR_GET:return qn==0 && n==27 && afr2_get_u32(p) && p[12]<=2 && p[13]<=1 && p[14]<=1 && !(p[15]&0xe0u) && p[26]<=1 &&
        afr2_get_u16(p+16)<=1023 && afr2_get_u16(p+18)<=1023 && afr2_get_u16(p+20)<=1023 && afr2_get_u16(p+22)<=1023 && afr2_get_u16(p+24)<=1023;
    case AFR2_EXT_SERVO_GET:case AFR2_EXT_SERVO_SET:
        return qn==(cmd==AFR2_EXT_SERVO_GET?0:2) && n==18 && afr2_get_u32(p) && p[12]<=180 && p[13]<=1 &&
            (!p[13] || (afr2_get_u16(p+8)>=1000 && afr2_get_u16(p+8)<=2000));
    case AFR2_EXT_RADAR_CONFIG:return qn==9 && n==8 && p[0]<p[1] && p[1]<=180 && p[0]%5==0 && p[1]%5==0 && p[2]>=1 && p[2]<=30 &&
        afr2_get_u16(p+3)>=50 && afr2_get_u16(p+3)<=500 && afr2_get_u16(p+5)>=70 && afr2_get_u16(p+5)<=1000 && p[7]<=1;
    case AFR2_EXT_RADAR_START:case AFR2_EXT_RADAR_STOP:return qn==0 && n==1 && p[0]==(cmd==AFR2_EXT_RADAR_START);
    case AFR2_EXT_RADAR_GET:
        if(qn!=5 || n<23 || p[14]>2 || n!=23+p[14]*AFR2_SAMPLE_BYTES || !afr2_get_u32(p) || p[15]>1 || p[16]>180 || p[17]>1)return 0;
        for(uint8_t i=0;i<p[14];i++)if(!afr2_sensor_record_valid(p+23+i*AFR2_SAMPLE_BYTES))return 0;
        return 1;
    case AFR2_EXT_UI_STATUS:return qn==0 && n==11 && p[10]<=1;
    case AFR2_EXT_MOTION_HALT:return qn==0 && n==1 && p[0]==1;
    default:return 0;
    }
}
#endif
