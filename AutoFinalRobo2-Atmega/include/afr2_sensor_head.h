/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
#ifndef AFR2_SENSOR_HEAD_H
#define AFR2_SENSOR_HEAD_H
#include "afr2_sensor_wire.h"
typedef struct { uint32_t id,ms; uint16_t distance_mm,ticks,pulse_us; uint8_t angle,error; } Afr2RangeSample;
typedef struct { uint32_t id,ms; uint16_t distance_mm; uint8_t error; } Afr2RadarBin;
typedef struct {
    Afr2RangeSample latest,queue[AFR2_RADAR_QUEUE];
    Afr2RadarBin bins[AFR2_RADAR_BINS];
    uint32_t sequence,servo_sequence,servo_ms,ir_sequence,step_ms;
    uint16_t interval_ms,settle_ms,pulse_us,target_us;
    uint8_t enabled,angle,min_angle,max_angle,speed,inverted,running,phase,index,head,count,ir_valid,scan_revision;
    uint8_t paused; /* pending editor resume; STOP clears it */
    int8_t direction;
} Afr2SensorHead;
const Afr2SensorHead *Afr2Head_State(void);
uint8_t Afr2Head_Servo(uint8_t operation,uint8_t angle);
uint8_t Afr2Head_Interval(uint16_t ms);
uint8_t Afr2Head_Configure(uint8_t min,uint8_t max,uint8_t speed,uint16_t settle,uint16_t interval,uint8_t inverted);
uint8_t Afr2Head_Start(void);
void Afr2Head_Stop(void);
void Afr2Head_Pause(void);
void Afr2Head_Resume(void);
void Afr2Head_CancelResume(void);
void Afr2Head_Record(uint8_t *out,const Afr2RangeSample *s);
uint8_t Afr2Head_Read(uint8_t mode,uint32_t cursor,uint8_t *out,uint32_t now,uint32_t boot);
#endif
