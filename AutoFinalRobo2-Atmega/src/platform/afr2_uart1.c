/* AFR2-20261023-15. AVR IRQ USART1, single producer/consumer rings.
 * No parser/dispatch, logs, waits, Arduino or DMA inside IRQs.
 */
#include "afr2_uart1.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#if F_CPU != 16000000UL
#error "Recalculate USART1 baud for this clock"
#endif
#define RING_MASK 127u
static volatile uint8_t rx[128], tx[128], rx_head, rx_tail, tx_head, tx_tail, fault;
void Afr2Uart1_Init(void) {
    UCSR1B=0; rx_head=rx_tail=tx_head=tx_tail=fault=0;
    /* Candidate 115200: 117647 nominal baud (+2.12%), not measured. */
    UCSR1A=_BV(U2X1); UBRR1H=0; UBRR1L=16;
    UCSR1C=_BV(UCSZ11)|_BV(UCSZ10);
    DDRD|=_BV(PD3); DDRD&=(uint8_t)~_BV(PD2);
    UCSR1B=_BV(RXEN1)|_BV(TXEN1)|_BV(RXCIE1);
}
ISR(USART1_RX_vect) {
    uint8_t status=UCSR1A, b=UDR1, next=(rx_head+1u)&RING_MASK;
    if (status&(_BV(FE1)|_BV(DOR1)|_BV(UPE1))) { fault=1; return; }
    if (next==rx_tail) { fault=1; return; }
    if (fault) return; /* Preserve fault barrier until main drains/reset session. */
    rx[rx_head]=b; rx_head=next;
}
ISR(USART1_UDRE_vect) {
    if (tx_head==tx_tail) { UCSR1B&=(uint8_t)~_BV(UDRIE1); return; }
    UDR1=tx[tx_tail]; tx_tail=(tx_tail+1u)&RING_MASK;
}
uint8_t Afr2Uart1_Read(uint8_t *b) {
    uint8_t ok=0;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (!fault && rx_head!=rx_tail) {
            *b=rx[rx_tail]; rx_tail=(rx_tail+1u)&RING_MASK; ok=1;
        }
    }
    return ok;
}
uint8_t Afr2Uart1_Write(const uint8_t *data,uint8_t length) {
    uint8_t head=tx_head;
    if (!data || !length || length>RING_MASK || ((tx_tail-head-1u)&RING_MASK)<length) return 0;
    /* ISR sees only published head, so the copy does not mask interrupts. */
    for (uint8_t i=0;i<length;++i) { tx[head]=data[i]; head=(head+1u)&RING_MASK; }
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { UCSR1A|=_BV(TXC1);tx_head=head; UCSR1B|=_BV(UDRIE1); }
    return 1;
}
uint8_t Afr2Uart1_TakeFault(void) {
    uint8_t r;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        r=fault; if (r) { rx_tail=rx_head; fault=0; }
    }
    return r;
}

uint8_t Afr2Uart1_TxIdle(void) { return tx_head==tx_tail && (UCSR1A&_BV(TXC1)); }
