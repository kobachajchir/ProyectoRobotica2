/* AFR2-20261108-01. afr2_usart AVR port: ATmega2560/1280/640 (USART0..3).
 * Included by afr2_usart.h; do not include directly.
 *
 * Interrupt ownership is explicit: the application binds a port's vectors in
 * exactly ONE translation unit with AFR2_USART_BIND_ISR(n). A port that is not
 * bound cannot be opened (Init returns AFR2_USART_ERR_NO_ISR), so an enabled
 * interrupt never lands on an empty vector (which on AVR resets the MCU).
 * Ports owned by other drivers (for example afr2_uart1.c) stay unbound here.
 */
#ifndef AFR2_USART_AVR_H
#define AFR2_USART_AVR_H
#include <avr/io.h>
#include <avr/interrupt.h>

#if defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega640__)
#define AFR2_USART_PORT_COUNT 4u
#else
#error "afr2_usart_avr: add this MCU to the port table (README.md, Portar a otro micro)"
#endif

/* Internal flags (Afr2Usart.flags). */
#define AFR2_USART_F_OPEN 0x01u
#define AFR2_USART_F_RX 0x02u
#define AFR2_USART_F_TX 0x04u
#define AFR2_USART_F_KEEP_ERRORS 0x08u
#define AFR2_USART_F_TXC_IRQ 0x10u

/* Complete handle. Treat fields as private; use the API. Fields touched by the
 * ISRs are volatile; multi-byte counters are read under ATOMIC_BLOCK. */
struct Afr2Usart {
    volatile uint8_t *regs;          /* UCSRnA; +1 B, +2 C, +4 UBRRnL, +5 UBRRnH, +6 UDRn */
    uint8_t *rx_buf, *tx_buf;
    uint8_t rx_mask, tx_mask;        /* ring size - 1 */
    volatile uint8_t rx_head, rx_tail, tx_head, tx_tail;
    volatile uint8_t pending, errors, line_idle;
    volatile uint16_t rx_overflow, rx_errors;
    uint16_t tx_rejected, ubrr;
    uint8_t port, flags, event_mask, u2x;
    Afr2UsartEventFn on_event;
    void *ctx;
};

/* Handle bound to each hardware port, read by the bound ISRs. */
extern Afr2Usart *afr2_usart_slot[AFR2_USART_PORT_COUNT];
/* Weak defaults return 0; AFR2_USART_BIND_ISR(n) provides the strong one. */
uint8_t afr2_usart_isr_bound_0(void);
uint8_t afr2_usart_isr_bound_1(void);
uint8_t afr2_usart_isr_bound_2(void);
uint8_t afr2_usart_isr_bound_3(void);

/* ISR bodies. Register bit positions are identical on USART0..3. Inlined so a
 * bound vector touches only the registers it needs (no full call frame). */
static inline __attribute__((always_inline))
void afr2_usart_isr_rx(Afr2Usart *u, uint8_t status, uint8_t data) {
    if (!u) return;
    uint8_t hw = status & (uint8_t)(_BV(FE0) | _BV(DOR0) | _BV(UPE0));
    if (hw) {
        uint8_t f = 0;
        if (hw & _BV(FE0)) f |= AFR2_USART_ERRF_FRAME;
        if (hw & _BV(DOR0)) f |= AFR2_USART_ERRF_OVERRUN;
        if (hw & _BV(UPE0)) f |= AFR2_USART_ERRF_PARITY;
        u->errors |= f; ++u->rx_errors; u->pending |= AFR2_USART_EVT_RX_ERROR;
        /* Overrun lost earlier bytes; this one is valid. FE/UPE corrupt it. */
        if ((hw & (uint8_t)(_BV(FE0) | _BV(UPE0))) && !(u->flags & AFR2_USART_F_KEEP_ERRORS)) return;
    }
    uint8_t head = u->rx_head, next = (uint8_t)((head + 1u) & u->rx_mask);
    if (next == u->rx_tail) { ++u->rx_overflow; u->pending |= AFR2_USART_EVT_RX_OVERFLOW; return; }
    u->rx_buf[head] = data; u->rx_head = next; u->pending |= AFR2_USART_EVT_RX;
}
static inline __attribute__((always_inline))
void afr2_usart_isr_udre(Afr2Usart *u, volatile uint8_t *ucsrb, volatile uint8_t *udr) {
    if (!u || u->tx_head == u->tx_tail) {
        *ucsrb &= (uint8_t)~_BV(UDRIE0);
        if (u) u->pending |= AFR2_USART_EVT_TX_EMPTY;
        return;
    }
    uint8_t tail = u->tx_tail;
    *udr = u->tx_buf[tail]; u->tx_tail = (uint8_t)((tail + 1u) & u->tx_mask);
}
static inline __attribute__((always_inline))
void afr2_usart_isr_txc(Afr2Usart *u) {
    if (u) { u->line_idle = 1u; u->pending |= AFR2_USART_EVT_TX_DONE; }
}

/* Place once per owned port, at file scope, e.g. AFR2_USART_BIND_ISR(0).
 * The indirection lets n be a macro such as AFR2_DEBUG_PORT. */
#define AFR2_USART_BIND_ISR(n) AFR2_USART_BIND_ISR_(n)
#define AFR2_USART_BIND_ISR_(n) \
    uint8_t afr2_usart_isr_bound_##n(void) { return 1u; } \
    ISR(USART##n##_RX_vect) { \
        uint8_t status_ = UCSR##n##A; uint8_t data_ = UDR##n; \
        afr2_usart_isr_rx(afr2_usart_slot[n], status_, data_); \
    } \
    ISR(USART##n##_UDRE_vect) { afr2_usart_isr_udre(afr2_usart_slot[n], &UCSR##n##B, &UDR##n); } \
    ISR(USART##n##_TX_vect) { afr2_usart_isr_txc(afr2_usart_slot[n]); }

#endif
