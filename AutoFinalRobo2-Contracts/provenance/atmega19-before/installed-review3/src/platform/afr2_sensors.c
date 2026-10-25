/* AFR2-20261023-19. Line5, HC-SR04 and proposed Hall bank, avr-libc.
 * Timer3 samples ECHO only during a measurement (25us, no OC outputs).
 * Timer5 drives optional servo D13 by compare IRQ; Timer1 PWM untouched.
 */
#include "afr2_product.h"
#include "afr2_board.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#ifndef AFR2_HALL_CONNECTED
#define AFR2_HALL_CONNECTED 0 /* User: S1/S2 not connected yet. */
#endif
static volatile int32_t hall[4];
static volatile uint8_t previous_k, echo_state, echo_error, echo_done;
static volatile uint16_t echo_ticks, echo_rise, echo_width, servo_width;
static uint8_t adc_channel, adc_running,line_kind,scans,history[5];
static uint16_t raw[5];
static uint32_t line_ms, echo_ms;
static uint32_t servo_ms;
static uint16_t servo_target;
static uint8_t servo_used;
void Afr2Sensors_Init(void) {
    DDRF&=(uint8_t)~0x1f; PORTF&=(uint8_t)~0x1f;
    DDRC|=_BV(PC7); PORTC&=(uint8_t)~_BV(PC7); DDRC&=(uint8_t)~_BV(PC6);
    PORTC&=(uint8_t)~_BV(PC6);
    TCCR3A=0; TCCR3B=_BV(WGM32)|_BV(CS31); OCR3A=49; TIMSK3=0;
    /* Prescaler8 gives 0.5us Timer5 units. No output compare pin enabled. */
    TCCR5A=0; TCCR5B=_BV(CS51); TIMSK5=0; DDRB|=_BV(PB7); PORTB&=(uint8_t)~_BV(PB7);
#if AFR2_HALL_CONNECTED
    /* Proposal: FL A8/A9, FR A10/A11, RL A12/A13, RR A14/A15.
     * External compatible pull-ups/levels must be confirmed before enabling. */
    DDRK=0; PORTK=0; previous_k=PINK; PCMSK2=0x55; PCIFR=_BV(PCIF2); PCICR|=_BV(PCIE2);
#endif
    ADMUX=_BV(REFS0)|4; ADCSRB=0; ADCSRA=_BV(ADEN)|_BV(ADPS2)|_BV(ADPS1)|_BV(ADPS0);
}
ISR(PCINT2_vect) {
    uint8_t k=PINK, rising=(uint8_t)(k&~previous_k&0x55u); previous_k=k;
    for(uint8_t i=0;i<4;i++)if(rising&(1u<<(i*2u))) {
        /* x1 rising A, B supplies direction. Unsigned arithmetic avoids signed overflow. */
        uint32_t n=(uint32_t)hall[i]; n+=(k&(2u<<(i*2u)))?UINT32_MAX:1u; hall[i]=(int32_t)n;
    }
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
        echo_state=0; echo_error=1; echo_done=1; TIMSK3=0;
    }
}
ISR(TIMER5_COMPA_vect) {
    if(PORTB&_BV(PB7)) { PORTB&=(uint8_t)~_BV(PB7); OCR5A+=(uint16_t)(40000u-servo_width); }
    else { PORTB|=_BV(PB7); OCR5A+=servo_width; }
}
void Afr2Servo_Set(uint16_t us) {
    if(us && (us<1000 || us>2000))return;
    if(us!=servo_target){servo_ms=Afr2Board_Millis();if(us){servo_target=us;servo_used=1;}}
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(!us) { TIMSK5=0; PORTB&=(uint8_t)~_BV(PB7); servo_width=0; }
        else if(us>=1000 && us<=2000) {
            servo_width=us*2u;
            if(!(TIMSK5&_BV(OCIE5A))) { OCR5A=TCNT5+200u; TIFR5=_BV(OCF5A); TIMSK5=_BV(OCIE5A); }
        }
    }
}
void Afr2Sensors_Task(Afr2Sensors *s,const Afr2Config *c,uint32_t now) {
    uint8_t updated=0;
    if(c->line_kind!=line_kind){line_kind=c->line_kind;scans=0;adc_running=0;
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
        if(scans<3)++scans;s->line_bits=0;
        for(uint8_t i=0;i<5;i++) {
            s->line_raw[i]=raw[i];
            uint8_t active=c->line_kind==1?(raw[i]!=0):(raw[i]>=c->line_threshold[i]);
            active^=(c->line_invert>>i)&1u;history[i]=(uint8_t)((history[i]<<1)|active)&7u;
            uint8_t votes=(history[i]&1u)+((history[i]>>1)&1u)+((history[i]>>2)&1u);
            if(votes>=2)s->line_bits|=(1u<<i);
        }
    }
    s->line_valid=c->line_kind && scans>=3 && c->line_calibrated && (uint32_t)(now-s->line_ms)<=100u;
    if(echo_done) {
        uint16_t width; uint8_t err;
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { width=echo_width; err=echo_error; echo_done=0; }
        s->echo_error=err; s->ultrasonic_valid=!err && width>0;
        if(s->ultrasonic_valid)s->distance_mm=(uint16_t)(((uint32_t)width*25u*10u)/58u);
        s->ultrasonic_ms=now;
    }
    if((uint32_t)(now-echo_ms)>=70u && !echo_state) {
        echo_ms=now;
        if(PINC&_BV(PC6)) { s->echo_error=2; s->ultrasonic_valid=0; }
        else ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
            echo_ticks=0; echo_error=0; echo_state=1; TCNT3=0;
            PORTC|=_BV(PC7); TIFR3=_BV(OCF3A); TIMSK3=_BV(OCIE3A);
        }
    }
    if((uint32_t)(now-s->ultrasonic_ms)>200u)s->ultrasonic_valid=0;
    /* Commanded orientation is not measured orientation. Once moved, accept
     * frontal readings only after settling at nominal center; off doesn't center. */
    if(servo_used && (servo_target<1450 || servo_target>1550 || (uint32_t)(now-servo_ms)<300u)) {
        s->ultrasonic_valid=0;s->echo_error=3;
    }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { for(uint8_t i=0;i<4;i++)s->counts[i]=hall[i]; }
    for(uint8_t i=0;i<4;i++)if(c->hall_signs&(1u<<i))s->counts[i]=(int32_t)(0u-(uint32_t)s->counts[i]);
    s->hall_available=AFR2_HALL_CONNECTED && c->hall_wired; if(s->hall_available)s->hall_ms=now;
}
