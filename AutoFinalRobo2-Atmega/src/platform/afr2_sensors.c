/* AFR2-20261023-19. Line5, HC-SR04 and proposed Hall bank, avr-libc.
 * Timer3 samples ECHO only during a measurement (25us, no OC outputs).
 * Timer5 drives optional servo D13 by compare IRQ; Timer1 PWM untouched.
 */
#include "afr2_product.h"
#include "afr2_board.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <string.h>
/* AFR2-20261024-11. Hall ISR/counters moved to board: pending Hall is serviced
 * before UI edge capture too. No change to x1 calibration units or wheel order. */
static volatile uint8_t echo_state, echo_error, echo_done;
static volatile uint16_t echo_ticks, echo_rise, echo_width, servo_width;
static uint8_t adc_channel, adc_running,line_kind,scans,history[5];
static uint16_t raw[5];
static uint32_t line_ms, echo_ms;
static Afr2SensorHead head;
static volatile uint16_t servo_pending,servo_applied;
static volatile uint32_t servo_revision;
static uint32_t echo_servo_revision;
static uint16_t first_settle;
static uint8_t echo_angle;
static uint16_t echo_pulse;
const Afr2SensorHead *Afr2Head_State(void){return &head;}
static uint16_t pulse_for(uint8_t angle){if(head.inverted)angle=180u-angle;return 1000u+((uint32_t)angle*1000u+90u)/180u;}
uint8_t Afr2Head_Interval(uint16_t ms){if(ms<70 || ms>1000)return AFR2_BAD;if(head.running)return AFR2_BUSY;head.interval_ms=ms;return AFR2_OK;}
uint8_t Afr2Head_Configure(uint8_t min,uint8_t max,uint8_t speed,uint16_t settle,uint16_t interval,uint8_t inverted){
    if(min>=max || max>180 || min%5 || max%5 || speed<1 || speed>30 || settle<50 || settle>500 || interval<70 || interval>1000 || inverted>1)return AFR2_BAD;
    if(head.running || head.enabled || head.target_us)return AFR2_BUSY;
    head.min_angle=min;head.max_angle=max;head.speed=speed;head.settle_ms=settle;head.interval_ms=interval;head.inverted=inverted;return AFR2_OK;
}
uint8_t Afr2Head_Servo(uint8_t operation,uint8_t angle){
    if(operation>2 || (operation==1 && angle>180))return AFR2_BAD;
    if(head.running)return AFR2_BUSY;
    Afr2Servo_Set(operation==0?0:pulse_for(operation==2?90:angle));return AFR2_OK;
}
uint8_t Afr2Head_Start(void){
    if(head.running)return AFR2_OK;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){TIMSK3=0;echo_state=0;echo_done=0;PORTC&=(uint8_t)~_BV(PC7);}
    memset(head.bins,0,sizeof(head.bins));for(uint8_t i=0;i<AFR2_RADAR_BINS;i++)head.bins[i].error=AFR2_ECHO_UNKNOWN;
    if(!++head.scan_revision)++head.scan_revision;
    head.head=head.count=0;head.running=1;head.phase=1;head.direction=1;head.index=head.min_angle/5;
    head.step_ms=Afr2Board_Millis();first_settle=300;Afr2Servo_Set(pulse_for(head.min_angle));return AFR2_OK;
}
void Afr2Head_Stop(void){head.running=0;head.phase=0;Afr2Servo_Set(0);}
void Afr2Head_Record(uint8_t *out,const Afr2RangeSample *s){
    afr2_put_u32(out,s->id);afr2_put_u32(out+4,s->ms);afr2_put_u16(out+8,s->distance_mm);
    afr2_put_u16(out+10,s->ticks);afr2_put_u16(out+12,s->pulse_us);out[14]=s->angle;out[15]=s->error;
}
uint8_t Afr2Head_Read(uint8_t mode,uint32_t cursor,uint8_t *out,uint32_t now,uint32_t boot){
    uint8_t count=0;uint16_t dropped=0;uint32_t next=cursor;
    if(mode==0){
        if(cursor>AFR2_RADAR_BINS)return 0;
        while(next<AFR2_RADAR_BINS && count<2){
            const Afr2RadarBin *b=&head.bins[next];Afr2RangeSample s={b->id,b->ms,b->distance_mm,0,pulse_for(next*5u),next*5u,b->error};
            Afr2Head_Record(out+23+count*16,&s);++next;++count;
        }
    }else if(mode==1){
        uint8_t oldest=(head.head+AFR2_RADAR_QUEUE-head.count)%AFR2_RADAR_QUEUE;
        if(head.count && cursor && (int32_t)(head.queue[oldest].id-cursor)>1){uint32_t gap=head.queue[oldest].id-cursor-1;dropped=gap>65535?65535:gap;}
        for(uint8_t i=0;i<head.count && count<2;i++){
            const Afr2RangeSample *s=&head.queue[(oldest+i)%AFR2_RADAR_QUEUE];
            if(!cursor || (int32_t)(s->id-cursor)>0){Afr2Head_Record(out+23+count*16,s);next=s->id;++count;}
        }
    }else return 0;
    afr2_put_u32(out,boot);afr2_put_u32(out+4,now);afr2_put_u32(out+8,next);afr2_put_u16(out+12,dropped);
    out[14]=count;out[15]=head.running;out[16]=head.angle;out[17]=head.enabled;
    afr2_put_u16(out+18,head.pulse_us);afr2_put_u16(out+20,head.target_us);out[22]=head.scan_revision;return 23+count*16;
}
static void publish_echo(Afr2Sensors *s,uint16_t width,uint8_t error,uint32_t now){
    uint16_t mm=((uint32_t)width*250u)/58u;
    if(!error && (mm<20 || mm>4000))error=AFR2_ECHO_RANGE;
    if(echo_servo_revision!=head.servo_sequence)error=AFR2_ECHO_MOVING;
    if(!++head.sequence)++head.sequence;
    head.latest=(Afr2RangeSample){head.sequence,now,error?0:mm,width,echo_pulse,echo_angle,error};
    s->echo_error=error;s->distance_mm=error?0:mm;s->ultrasonic_ms=now;
    if(head.running){
        head.queue[head.head]=head.latest;head.head=(head.head+1u)%AFR2_RADAR_QUEUE;if(head.count<AFR2_RADAR_QUEUE)++head.count;
        head.bins[head.index]=(Afr2RadarBin){head.sequence,now,error?0:mm,error};head.phase=3;
    }
}
void Afr2Sensors_Init(void) {
    memset(&head,0,sizeof(head));head.interval_ms=70;head.settle_ms=100;head.speed=10;head.max_angle=180;
    head.angle=90;head.latest.angle=90;head.latest.error=AFR2_ECHO_UNKNOWN;
    for(uint8_t i=0;i<AFR2_RADAR_BINS;i++)head.bins[i].error=AFR2_ECHO_UNKNOWN;
    /* PF4 is also JTAG TCK. Release it in software, no fuse writes.
     * Init runs before sei(); two consecutive volatile writes meet JTD sequence. */
    uint8_t jtag=MCUCR|_BV(JTD);MCUCR=jtag;MCUCR=jtag;
    DDRF&=(uint8_t)~0x1f; PORTF&=(uint8_t)~0x1f;
    DDRC|=_BV(PC7); PORTC&=(uint8_t)~_BV(PC7); DDRC&=(uint8_t)~_BV(PC6);
    PORTC&=(uint8_t)~_BV(PC6);
    TCCR3A=0; TCCR3B=_BV(WGM32)|_BV(CS31); OCR3A=49; TIMSK3=0;
    /* Prescaler8 gives 0.5us Timer5 units. No output compare pin enabled. */
    TCCR5A=0; TCCR5B=_BV(CS51); TIMSK5=0; DDRB|=_BV(PB7); PORTB&=(uint8_t)~_BV(PB7);
    Afr2Board_HallInit();
    ADMUX=_BV(REFS0)|4; ADCSRB=0; ADCSRA=_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0);
}
ISR(TIMER3_COMPA_vect) {
    ++echo_ticks;
    if(echo_state==1) {
        PORTC&=(uint8_t)~_BV(PC7); echo_state=2;
    } else if(echo_state==2 && (PINC&_BV(PC6))) { echo_rise=echo_ticks; echo_state=3; }
    else if(echo_state==3 && !(PINC&_BV(PC6))) {
        echo_width=echo_ticks-echo_rise; echo_error=0; echo_done=1; echo_state=0; TIMSK3=0;
    }
    if(echo_ticks>=1280u && echo_state) { /* 32ms total bound; no blocking. */
        echo_error=echo_state==3?AFR2_ECHO_NO_FALL:AFR2_ECHO_NO_RISE;echo_width=0;
        echo_state=0; echo_done=1; TIMSK3=0;
    }
}
ISR(TIMER5_COMPA_vect) {
    if(PORTB&_BV(PB7)) { PORTB&=(uint8_t)~_BV(PB7); OCR5A+=(uint16_t)(40000u-servo_width); }
    else {
        if(servo_applied!=servo_pending){servo_applied=servo_pending;++servo_revision;}
        servo_width=servo_applied*2u;PORTB|=_BV(PB7); OCR5A+=servo_width;
    }
}
void Afr2Servo_Set(uint16_t us) {
    if(us && (us<1000 || us>2000))return;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        servo_pending=us;
        if(!us) { TIMSK5=0; PORTB&=(uint8_t)~_BV(PB7); servo_width=0;if(servo_applied){servo_applied=0;++servo_revision;} }
        else if(us>=1000 && us<=2000) {
            if(!(TIMSK5&_BV(OCIE5A))) { OCR5A=TCNT5+200u; TIFR5=_BV(OCF5A); TIMSK5=_BV(OCIE5A); }
        }
    }
    head.target_us=us;
}
void Afr2Sensors_Task(Afr2Sensors *s,const Afr2Config *c,uint32_t now) {
    uint16_t applied;uint32_t revision;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){applied=servo_applied;revision=servo_revision;}
    if(revision!=head.servo_sequence){head.servo_sequence=revision;head.servo_ms=now;head.pulse_us=applied;head.enabled=applied!=0;
        if(applied){uint8_t angle=((uint32_t)(applied-1000u)*180u+500u)/1000u;head.angle=head.inverted?180u-angle:angle;}}
    uint8_t updated=0;
    if(c->line_kind!=line_kind){line_kind=c->line_kind;scans=0;adc_running=0;memset(history,0,sizeof(history));
        ADCSRA&=(uint8_t)~_BV(ADEN);ADCSRA|=_BV(ADEN);}
    if((uint32_t)(now-line_ms)>=5u) {
        if(c->line_kind==1) { /* Digital: IR1 PF4 .. IR5 PF0 */
            uint8_t pins=PINF; for(uint8_t i=0;i<5;i++)raw[i]=(pins&(1u<<(4u-i)))?1:0;
            line_ms=now; s->line_ms=now;updated=1;
        } else if(c->line_kind==2) {
            if(!adc_running) { adc_channel=0; ADMUX=_BV(REFS0)|4; ADCSRA|=_BV(ADSC); adc_running=1; }
            else if(!(ADCSRA&_BV(ADSC))) {
                raw[adc_channel]=ADC;
                if(++adc_channel==5) { adc_running=0; line_ms=now; s->line_ms=now;updated=1; }
                else { ADMUX=_BV(REFS0)|(4u-adc_channel); ADCSRA|=_BV(ADSC); }
            }
        }
    }
    if(updated) {
        ++head.ir_sequence;
        if(scans<3)++scans;
        s->line_bits=0;
        for(uint8_t i=0;i<5;i++) {
            s->line_raw[i]=raw[i];
            uint8_t active=c->line_kind==1?(raw[i]!=0):(raw[i]>=c->line_threshold[i]);
            active^=(c->line_invert>>i)&1u;history[i]=(uint8_t)((history[i]<<1)|active)&7u;
            uint8_t votes=(history[i]&1u)+((history[i]>>1)&1u)+((history[i]>>2)&1u);
            if(votes>=2)s->line_bits|=(1u<<i);
        }
    }
    s->line_valid=c->line_kind && scans>=3 && c->line_calibrated && (uint32_t)(now-s->line_ms)<=100u;
    head.ir_valid=c->line_kind && scans && (uint32_t)(now-s->line_ms)<=100u;
    if(echo_done) {
        uint16_t width; uint8_t err;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { width=echo_width; err=echo_error; echo_done=0; }
        publish_echo(s,width,err,now);
    }
    if(head.running && head.phase==3 && !echo_state && (uint32_t)(now-head.step_ms)>=5000u/head.speed){
        int16_t next=(int16_t)head.index+head.direction;
        if(next>head.max_angle/5 || next<head.min_angle/5){head.direction=-head.direction;next=(int16_t)head.index+head.direction;}
        head.index=next;head.step_ms=now;head.phase=1;first_settle=head.settle_ms;Afr2Servo_Set(pulse_for(head.index*5u));
    }
    uint8_t settled=head.target_us==head.pulse_us && (!head.enabled || (uint32_t)(now-head.servo_ms)>=(head.running?first_settle:300u));
    if((uint32_t)(now-echo_ms)>=head.interval_ms && !echo_state && settled && (!head.running || head.phase==1)) {
        echo_ms=now;
        echo_angle=head.angle;echo_pulse=head.pulse_us;echo_servo_revision=head.servo_sequence;
        if(head.running)head.phase=2;
        if(PINC&_BV(PC6)) { publish_echo(s,0,AFR2_ECHO_HIGH,now); }
        else ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            echo_ticks=0; echo_error=0; echo_state=1; TCNT3=0;
            PORTC|=_BV(PC7); TIFR3=_BV(OCF3A); TIMSK3=_BV(OCIE3A);
        }
    }
    s->ultrasonic_valid=head.latest.id && head.latest.error==AFR2_ECHO_OK && (uint32_t)(now-head.latest.ms)<=200u &&
        head.enabled && head.pulse_us>=1450 && head.pulse_us<=1550 && head.target_us==head.pulse_us &&
        (uint32_t)(now-head.servo_ms)>=300u && head.latest.pulse_us==head.pulse_us;
    Afr2Board_HallSnapshot(s->counts);
    for(uint8_t i=0;i<4;i++)if(c->hall_signs&(1u<<i))s->counts[i]=(int32_t)(0u-(uint32_t)s->counts[i]);
    s->hall_available=AFR2_HALL_CONNECTED && c->hall_wired; if(s->hall_available)s->hall_ms=now;
}
