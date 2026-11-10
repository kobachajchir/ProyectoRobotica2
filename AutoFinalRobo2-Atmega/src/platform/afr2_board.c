/* AFR2-20261023-15. Timer0 and board GPIO, shared by both compositions.
 * Pinout documented, not physically verified. Never enables drive outputs.
 */
#include "afr2_board.h"
#include "board_pinmap.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#if F_CPU != 16000000UL
#error "Recalculate Timer0 and UART1 for this clock"
#endif
#ifndef AFR2_ENCODER_INTERNAL_PULLUPS
#define AFR2_ENCODER_INTERNAL_PULLUPS 1
#endif
static volatile uint32_t ticks_ms;
/* AFR2-20261024-11. Single ISR producer / main consumer, 31 usable edges.
 * No drawing, UI decoding or application actions inside an interrupt. */
#define ENCODER_QUEUE_SIZE 32u
#define ENCODER_QUEUE_MASK (ENCODER_QUEUE_SIZE-1u)
static volatile uint8_t encoder_queue[ENCODER_QUEUE_SIZE];
static volatile uint8_t encoder_head,encoder_tail,encoder_overflow;
static volatile uint8_t encoder_ab;
/* AFR2-20261110-15: interrupt-storm guard. Motor PWM switching couples onto the Hall and
 * encoder lines (internal pull-ups only, hardware cannot change). Every edge is an IRQ with
 * priority over Timer0/UART/TWI, so a burst can starve the main loop past the 100 ms control
 * deadline. Each source gets a fixed IRQ budget per 1 ms tick; when it is spent the source is
 * masked until the next tick, which re-arms it from the current pin levels. Real rates stay
 * far below the budgets (Hall ~15 edges/ms at 4 wheels, encoder by hand <1/ms). */
#define AFR2_HALL_IRQ_BUDGET 64u
#define AFR2_ENCODER_IRQ_BUDGET 8u
static volatile uint8_t hall_budget=AFR2_HALL_IRQ_BUDGET,encoder_budget=AFR2_ENCODER_IRQ_BUDGET;
static volatile uint16_t hall_storms,encoder_storms;
#if AFR2_HALL_CONNECTED
static volatile int32_t hall_counts[4];
static volatile uint8_t hall_previous, hall_enabled;
static inline void hall_capture(void) {
    uint8_t k=AFR2_HALL_PIN;
    uint8_t rising=(uint8_t)(k&~hall_previous&AFR2_HALL_A_MASK);
    hall_previous=k;
    /* Unrolled fixed work; retain x1 counts and unsigned wrap semantics. */
    if(rising&_BV(AFR2_HALL_FL_A_BIT))hall_counts[0]=(int32_t)((uint32_t)hall_counts[0]+((k&_BV(AFR2_HALL_FL_B_BIT))?UINT32_MAX:1u));
    if(rising&_BV(AFR2_HALL_FR_A_BIT))hall_counts[1]=(int32_t)((uint32_t)hall_counts[1]+((k&_BV(AFR2_HALL_FR_B_BIT))?UINT32_MAX:1u));
    if(rising&_BV(AFR2_HALL_RL_A_BIT))hall_counts[2]=(int32_t)((uint32_t)hall_counts[2]+((k&_BV(AFR2_HALL_RL_B_BIT))?UINT32_MAX:1u));
    if(rising&_BV(AFR2_HALL_RR_A_BIT))hall_counts[3]=(int32_t)((uint32_t)hall_counts[3]+((k&_BV(AFR2_HALL_RR_B_BIT))?UINT32_MAX:1u));
}
ISR(PCINT2_vect) {
    hall_capture();
    if(!--hall_budget){PCICR&=(uint8_t)~_BV(PCIE2);if(hall_storms!=UINT16_MAX)++hall_storms;}
}
#endif
static uint8_t encoder_levels_ab(void) {
    uint8_t pins=PINE;
    return (uint8_t)(((pins&_BV(AFR2_ENC_UI_CLK_BIT))?2u:0u)|
                     ((pins&_BV(AFR2_ENC_UI_DT_BIT))?1u:0u));
}
static void encoder_capture(void) {
#if AFR2_HALL_CONNECTED
    /* AVR vector priority is fixed: INT4/5 precede PCINT2. Service a pending
     * Hall sample FIRST here. Do not clear PCIF2: an edge arriving during this
     * capture must retain its IRQ; the later PCINT2 entry cannot double count. */
    if(hall_enabled && (PCICR&_BV(PCIE2)) && (PCIFR&_BV(PCIF2)))hall_capture();
#endif
    uint8_t ab=encoder_levels_ab();
    if(ab==encoder_ab)return; /* Both INT flags can refer to one port sample. */
    encoder_ab=ab;
    if(encoder_overflow)return;
    uint8_t next=(uint8_t)((encoder_head+1u)&ENCODER_QUEUE_MASK);
    if(next==encoder_tail){encoder_overflow=1;return;}
    encoder_queue[encoder_head]=ab;
    encoder_head=next; /* Publish only after the entry is written. */
}
static void encoder_irq(void) {
    encoder_capture();
    if(!--encoder_budget){EIMSK&=(uint8_t)~(_BV(INT4)|_BV(INT5));if(encoder_storms!=UINT16_MAX)++encoder_storms;}
}
ISR(INT4_vect) { encoder_irq(); }
ISR(INT5_vect) { encoder_irq(); }
ISR(TIMER0_COMPA_vect) {
    ++ticks_ms;
#if AFR2_HALL_CONNECTED
    /* Re-arm from current levels: edges seen while masked were mostly noise; never count
     * a phantom edge against a stale previous sample. */
    if(hall_enabled && !(PCICR&_BV(PCIE2))){hall_previous=AFR2_HALL_PIN;PCIFR=_BV(PCIF2);PCICR|=_BV(PCIE2);}
#endif
    hall_budget=AFR2_HALL_IRQ_BUDGET;
    if(!encoder_budget){EIFR=_BV(INTF4)|_BV(INTF5);encoder_capture();EIMSK|=_BV(INT4)|_BV(INT5);}
    encoder_budget=AFR2_ENCODER_IRQ_BUDGET;
}
void Afr2Board_NoiseCounts(uint16_t *hall,uint16_t *encoder) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(hall)*hall=hall_storms;
        if(encoder)*encoder=encoder_storms;
    }
}
uint32_t Afr2Board_Millis(void) {
    uint32_t n; ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { n=ticks_ms; } return n;
}
void Afr2Board_Init(void) {
    cli();
    /* Detach compare outputs before GPIO latches. No claim of physical STOP. */
    TCCR1A=0; TCCR2A=0; TCCR3A=0; TCCR4A=0;
    PORTH&=(uint8_t)~(_BV(PH6)|_BV(PH3)|_BV(PH4)|_BV(PH5));
    PORTB&=(uint8_t)~(_BV(PB4)|_BV(PB5)|_BV(PB6));
    PORTA&=(uint8_t)~(_BV(PA0)|_BV(PA2)|_BV(PA4)|_BV(PA6));
    PORTE&=(uint8_t)~_BV(PE3);
    DDRH|=_BV(PH6)|_BV(PH3)|_BV(PH4)|_BV(PH5);
    DDRB|=_BV(PB4)|_BV(PB5)|_BV(PB6);
    DDRA|=_BV(PA0)|_BV(PA2)|_BV(PA4)|_BV(PA6); DDRE|=_BV(PE3);
    ticks_ms=0; TCCR0A=_BV(WGM01); TCCR0B=_BV(CS01)|_BV(CS00); OCR0A=249;
    TCNT0=0; TIFR0=_BV(OCF0A); TIMSK0=_BV(OCIE0A);
}
void Afr2Board_EncoderInit(void) {
    EIMSK&=(uint8_t)~(_BV(INT4)|_BV(INT5));
    AFR2_ENC_UI_CLK_DDR&=(uint8_t)~_BV(AFR2_ENC_UI_CLK_BIT);
    AFR2_ENC_UI_DT_DDR&=(uint8_t)~_BV(AFR2_ENC_UI_DT_BIT);
    AFR2_ENC_UI_SW_DDR&=(uint8_t)~_BV(AFR2_ENC_UI_SW_BIT);
#if AFR2_ENCODER_INTERNAL_PULLUPS
    AFR2_ENC_UI_CLK_PORT|=_BV(AFR2_ENC_UI_CLK_BIT);
    AFR2_ENC_UI_DT_PORT|=_BV(AFR2_ENC_UI_DT_BIT);
    AFR2_ENC_UI_SW_PORT|=_BV(AFR2_ENC_UI_SW_BIT);
#else
    AFR2_ENC_UI_CLK_PORT&=(uint8_t)~_BV(AFR2_ENC_UI_CLK_BIT);
    AFR2_ENC_UI_DT_PORT&=(uint8_t)~_BV(AFR2_ENC_UI_DT_BIT);
    AFR2_ENC_UI_SW_PORT&=(uint8_t)~_BV(AFR2_ENC_UI_SW_BIT);
#endif
    encoder_head=encoder_tail=encoder_overflow=0;
    /* Any logical change, ISC41:40=01 and ISC51:50=01; preserve INT6/7. */
    EICRB=(uint8_t)((EICRB&~(_BV(ISC41)|_BV(ISC40)|_BV(ISC51)|_BV(ISC50)))|
                   _BV(ISC40)|_BV(ISC50));
    EIFR=_BV(INTF4)|_BV(INTF5);
    encoder_ab=encoder_levels_ab();
    EIMSK|=_BV(INT4)|_BV(INT5);
}
uint8_t Afr2Board_EncoderLevels(void) {
    uint8_t ab=encoder_levels_ab(), sw=AFR2_ENC_UI_SW_PIN;
    return (uint8_t)(ab|((sw&_BV(AFR2_ENC_UI_SW_BIT))?0u:4u));
}
uint8_t Afr2Board_EncoderRead(uint8_t *ab) {
    uint8_t result=0;
    if(!ab)return 0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if(encoder_overflow) {
            encoder_tail=encoder_head;
            encoder_ab=encoder_levels_ab();*ab=encoder_ab;
            encoder_overflow=0;result=2;
        } else if(encoder_tail!=encoder_head) {
            *ab=encoder_queue[encoder_tail];
            encoder_tail=(uint8_t)((encoder_tail+1u)&ENCODER_QUEUE_MASK);result=1;
        }
    }
    return result;
}
void Afr2Board_HallInit(void) {
#if AFR2_HALL_CONNECTED
    PCICR&=(uint8_t)~_BV(PCIE2);hall_enabled=0;
    /* AFR2-20261106-07: internal pull-ups on A8..A15. Open-collector Hall outputs float without them
     * (no edges counted); push-pull outputs are unaffected. */
    AFR2_HALL_DDR=0;AFR2_HALL_PORT=0xFF;
    PCMSK2=AFR2_HALL_A_MASK;PCIFR=_BV(PCIF2);
    hall_previous=AFR2_HALL_PIN;
    for(uint8_t i=0;i<4;i++)hall_counts[i]=0;
    hall_enabled=1;PCICR|=_BV(PCIE2);
#endif
}
void Afr2Board_HallSnapshot(int32_t counts[4]) {
    if(!counts)return;
#if AFR2_HALL_CONNECTED
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        for(uint8_t i=0;i<4;i++)counts[i]=hall_counts[i];
    }
#else
    for(uint8_t i=0;i<4;i++)counts[i]=0;
#endif
}
