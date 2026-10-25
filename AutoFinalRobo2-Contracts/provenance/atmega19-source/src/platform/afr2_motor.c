/* New avr-libc driver. Pin provenance: Contracts/hardware/PINOUT.md.
 * FL D10/PB4, FR D9/PH6, RL D12/PB6, RR D11/PB5. No Arduino/DMA.
 */
#include "afr2_product.h"
#include <avr/io.h>
#include <util/atomic.h>
void Afr2Motor_Stop(void) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    TCCR1A&=(uint8_t)~(_BV(COM1A1)|_BV(COM1A0)|_BV(COM1B1)|_BV(COM1B0));
    TCCR2A&=(uint8_t)~(_BV(COM2A1)|_BV(COM2A0)|_BV(COM2B1)|_BV(COM2B0));
    OCR2A=OCR2B=OCR1A=OCR1B=0;
    PORTB&=(uint8_t)~(_BV(PB4)|_BV(PB5)|_BV(PB6));PORTH&=(uint8_t)~_BV(PH6);
    PORTA&=(uint8_t)~0x55u; PORTE&=(uint8_t)~_BV(PE3);
    PORTH&=(uint8_t)~(_BV(PH3)|_BV(PH4)|_BV(PH5));
    }
}
void Afr2Motor_Init(void) {
    Afr2Motor_Stop();
    DDRB|=_BV(PB4)|_BV(PB5)|_BV(PB6); DDRH|=_BV(PH6)|_BV(PH3)|_BV(PH4)|_BV(PH5);
    DDRA|=0x55; DDRE|=_BV(PE3);
    /* Fast PWM8, prescaler64: candidate 976Hz. Timer0 remains the clock. */
    TCCR1A=_BV(WGM10)|_BV(COM1A1)|_BV(COM1B1); TCCR1B=_BV(WGM12)|_BV(CS11)|_BV(CS10);
    TCCR2A=_BV(WGM20)|_BV(WGM21)|_BV(COM2A1)|_BV(COM2B1); TCCR2B=_BV(CS22);
    Afr2Motor_Stop();
}
static void direction(volatile uint8_t *p,uint8_t a,uint8_t b,int16_t v) {
    uint8_t n=*p; n&=(uint8_t)~(_BV(a)|_BV(b));
    if(v>0)n|=_BV(a); else if(v<0)n|=_BV(b); *p=n;
}
void Afr2Motor_Write(const int16_t values[4],uint8_t signs) {
    if(!values[0] && !values[1] && !values[2] && !values[3]){Afr2Motor_Stop();return;}
    int16_t v[4];
    for(uint8_t i=0;i<4;i++) {
        v[i]=values[i]; if(v[i]>255)v[i]=255; if(v[i]<-255)v[i]=-255;
        if(signs&(1u<<i))v[i]=-v[i];
    }
    direction(&PORTA,PA4,PA6,v[0]); direction(&PORTA,PA0,PA2,v[1]);
    direction(&PORTH,PH4,PH5,v[2]);
    PORTE=(PORTE&(uint8_t)~_BV(PE3))|(v[3]>0?_BV(PE3):0);
    PORTH=(PORTH&(uint8_t)~_BV(PH3))|(v[3]<0?_BV(PH3):0);
    OCR2A=(uint8_t)(v[0]<0?-v[0]:v[0]); OCR2B=(uint8_t)(v[1]<0?-v[1]:v[1]);
    OCR1B=(uint16_t)(v[2]<0?-v[2]:v[2]); OCR1A=(uint16_t)(v[3]<0?-v[3]:v[3]);
    TCCR1A|=_BV(COM1A1)|_BV(COM1B1);TCCR2A|=_BV(COM2A1)|_BV(COM2B1);
}
