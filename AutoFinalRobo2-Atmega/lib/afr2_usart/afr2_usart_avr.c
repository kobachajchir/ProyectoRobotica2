/* AFR2-20261108-01. afr2_usart AVR implementation (main-context API).
 * Interrupt handlers live in the application through AFR2_USART_BIND_ISR(n).
 */
#include "afr2_usart.h"
#include <string.h>
#include <avr/pgmspace.h>
#include <util/atomic.h>

#ifndef F_CPU
#error "afr2_usart: define F_CPU"
#endif

Afr2Usart *afr2_usart_slot[AFR2_USART_PORT_COUNT];

__attribute__((weak)) uint8_t afr2_usart_isr_bound_0(void) { return 0u; }
__attribute__((weak)) uint8_t afr2_usart_isr_bound_1(void) { return 0u; }
__attribute__((weak)) uint8_t afr2_usart_isr_bound_2(void) { return 0u; }
__attribute__((weak)) uint8_t afr2_usart_isr_bound_3(void) { return 0u; }

/* Fixed silicon mapping. Pins use Arduino Mega digital numbering (D0..D19).
 * PORTx sits at DDRx + 1 in data memory on these parts. */
typedef struct { uint16_t regs, ddr; uint8_t rx_bit, tx_bit, rx_pin, tx_pin; } Afr2UsartHw;
static const Afr2UsartHw hw_table[AFR2_USART_PORT_COUNT] PROGMEM = {
    { _SFR_MEM_ADDR(UCSR0A), _SFR_MEM_ADDR(DDRE), 0u, 1u, 0u, 1u },   /* RXD0 PE0 D0,  TXD0 PE1 D1  */
    { _SFR_MEM_ADDR(UCSR1A), _SFR_MEM_ADDR(DDRD), 2u, 3u, 19u, 18u }, /* RXD1 PD2 D19, TXD1 PD3 D18 */
    { _SFR_MEM_ADDR(UCSR2A), _SFR_MEM_ADDR(DDRH), 0u, 1u, 17u, 16u }, /* RXD2 PH0 D17, TXD2 PH1 D16 */
    { _SFR_MEM_ADDR(UCSR3A), _SFR_MEM_ADDR(DDRJ), 0u, 1u, 15u, 14u }, /* RXD3 PJ0 D15, TXD3 PJ1 D14 */
};

static uint8_t isr_bound(uint8_t port) {
    switch (port) {
    case 0: return afr2_usart_isr_bound_0();
    case 1: return afr2_usart_isr_bound_1();
    case 2: return afr2_usart_isr_bound_2();
    case 3: return afr2_usart_isr_bound_3();
    default: return 0u;
    }
}
static uint8_t ring_size_ok(uint16_t n) { return n >= 2u && n <= 256u && !(n & (n - 1u)); }

/* Closest divisor in normal (/16) or double speed (/8) mode; ties keep /16,
 * which samples more per bit and tolerates more noise. */
static uint8_t pick_divisor(uint32_t baud, uint16_t max_permille, uint16_t *ubrr, uint8_t *u2x) {
    uint32_t best = UINT32_MAX;
    for (uint8_t mode = 0; mode < 2u; ++mode) {
        uint32_t scale = mode ? 8UL : 16UL, divider = scale * baud;
        uint32_t n = (F_CPU + divider / 2u) / divider;
        if (!n || n > 4096UL) continue;
        uint32_t actual = F_CPU / (scale * n);
        uint32_t diff = actual > baud ? actual - baud : baud - actual;
        uint32_t error = diff * 1000UL / baud;
        if (error < best) { best = error; *ubrr = (uint16_t)(n - 1u); *u2x = mode; }
    }
    return best <= max_permille;
}

uint8_t Afr2Usart_PortCount(void) { return AFR2_USART_PORT_COUNT; }

uint8_t Afr2Usart_PortInfo(uint8_t port, Afr2UsartPortInfo *out) {
    if (port >= AFR2_USART_PORT_COUNT || !out) return 0u;
    out->rx_pin = pgm_read_byte(&hw_table[port].rx_pin);
    out->tx_pin = pgm_read_byte(&hw_table[port].tx_pin);
    return 1u;
}

uint8_t Afr2Usart_IsOpen(const Afr2Usart *u) {
    return u && (u->flags & AFR2_USART_F_OPEN) && u->port < AFR2_USART_PORT_COUNT && afr2_usart_slot[u->port] == u;
}

void Afr2Usart_Deinit(Afr2Usart *u) {
    if (!Afr2Usart_IsOpen(u)) return;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        u->regs[1] = 0;                   /* receiver, transmitter and their IRQs off */
        afr2_usart_slot[u->port] = NULL;
        u->flags = 0; u->pending = 0;
    }
}

Afr2UsartStatus Afr2Usart_Init(Afr2Usart *u, const Afr2UsartConfig *c) {
    if (!u || !c || !c->baud) return AFR2_USART_ERR_ARG;
    if (c->port >= AFR2_USART_PORT_COUNT) return AFR2_USART_ERR_PORT;
    if (c->data_bits < 5u || c->data_bits > 8u || (c->stop_bits != 1u && c->stop_bits != 2u) ||
        (c->parity != AFR2_USART_PARITY_NONE && c->parity != AFR2_USART_PARITY_EVEN && c->parity != AFR2_USART_PARITY_ODD))
        return AFR2_USART_ERR_ARG;
    uint8_t rx = c->rx_buffer && c->rx_size, tx = c->tx_buffer && c->tx_size;
    if ((!rx && !tx) || (rx && !ring_size_ok(c->rx_size)) || (tx && !ring_size_ok(c->tx_size)))
        return AFR2_USART_ERR_BUFFER;
    Afr2UsartHw hw; memcpy_P(&hw, &hw_table[c->port], sizeof(hw));
    if ((c->rx_pin != AFR2_USART_PIN_DEFAULT && c->rx_pin != hw.rx_pin) ||
        (c->tx_pin != AFR2_USART_PIN_DEFAULT && c->tx_pin != hw.tx_pin))
        return AFR2_USART_ERR_PIN;
    if (afr2_usart_slot[c->port] && afr2_usart_slot[c->port] != u) return AFR2_USART_ERR_BUSY;
    if (!isr_bound(c->port)) return AFR2_USART_ERR_NO_ISR;
    uint16_t ubrr = 0; uint8_t u2x = 0;
    if (!pick_divisor(c->baud, c->max_baud_error_permille ? c->max_baud_error_permille : 25u, &ubrr, &u2x))
        return AFR2_USART_ERR_BAUD;

    Afr2Usart_Deinit(u);                  /* re-init (same or other port) releases the old one */
    memset(u, 0, sizeof(*u));
    u->regs = (volatile uint8_t *)hw.regs;
    u->port = c->port; u->ubrr = ubrr; u->u2x = u2x;
    u->rx_buf = rx ? c->rx_buffer : NULL; u->rx_mask = rx ? (uint8_t)(c->rx_size - 1u) : 0u;
    u->tx_buf = tx ? c->tx_buffer : NULL; u->tx_mask = tx ? (uint8_t)(c->tx_size - 1u) : 0u;
    u->event_mask = c->event_mask; u->on_event = c->on_event; u->ctx = c->ctx;
    u->line_idle = 1u;
    u->flags = (uint8_t)(AFR2_USART_F_OPEN | (rx ? AFR2_USART_F_RX : 0u) | (tx ? AFR2_USART_F_TX : 0u) |
        (c->rx_error_policy == AFR2_USART_RX_KEEP_ERRORS ? AFR2_USART_F_KEEP_ERRORS : 0u) |
        ((tx && (c->event_mask & AFR2_USART_EVT_TX_DONE)) ? AFR2_USART_F_TXC_IRQ : 0u));

    volatile uint8_t *r = u->regs, *ddr = (volatile uint8_t *)hw.ddr, *port = ddr + 1;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        r[1] = 0;
        afr2_usart_slot[c->port] = u;
        if (tx) { *port |= _BV(hw.tx_bit); *ddr |= _BV(hw.tx_bit); }   /* idle high before TXEN */
        if (rx) {
            *ddr &= (uint8_t)~_BV(hw.rx_bit);
            if (c->rx_pullup) *port |= _BV(hw.rx_bit); else *port &= (uint8_t)~_BV(hw.rx_bit);
        }
        r[5] = (uint8_t)(ubrr >> 8); r[4] = (uint8_t)ubrr;
        r[0] = (uint8_t)((u2x ? _BV(U2X0) : 0u) | _BV(TXC0));          /* writing 1 clears TXC */
        r[2] = (uint8_t)((c->parity << UPM00) | (c->stop_bits == 2u ? _BV(USBS0) : 0u) |
                         (((c->data_bits - 5u) & 3u) << UCSZ00));
        r[1] = (uint8_t)((rx ? (_BV(RXEN0) | _BV(RXCIE0)) : 0u) | (tx ? _BV(TXEN0) : 0u) |
                         ((u->flags & AFR2_USART_F_TXC_IRQ) ? _BV(TXCIE0) : 0u));
    }
    return AFR2_USART_OK;
}

uint32_t Afr2Usart_ActualBaud(const Afr2Usart *u) {
    if (!Afr2Usart_IsOpen(u)) return 0u;
    return F_CPU / ((u->u2x ? 8UL : 16UL) * ((uint32_t)u->ubrr + 1u));
}

uint16_t Afr2Usart_TxFree(const Afr2Usart *u) {
    if (!u || !(u->flags & AFR2_USART_F_TX)) return 0u;
    return (uint8_t)((uint8_t)(u->tx_tail - u->tx_head - 1u) & u->tx_mask);
}

static void tx_publish(Afr2Usart *u, uint8_t head) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        u->line_idle = 0u;
        u->regs[0] = (uint8_t)((u->regs[0] & (uint8_t)(_BV(U2X0) | _BV(MPCM0))) | _BV(TXC0));
        u->tx_head = head;
        u->regs[1] |= _BV(UDRIE0);
    }
}

uint8_t Afr2Usart_Write(Afr2Usart *u, const void *data, uint16_t length) {
    if (!u || !(u->flags & AFR2_USART_F_TX) || !data || !length) return 0u;
    if (length > Afr2Usart_TxFree(u)) { ++u->tx_rejected; return 0u; }
    /* The ISR only sees the published head, so copying needs no lock. */
    const uint8_t *p = (const uint8_t *)data;
    uint8_t head = u->tx_head;
    for (uint16_t i = 0; i < length; ++i) { u->tx_buf[head] = p[i]; head = (uint8_t)((head + 1u) & u->tx_mask); }
    tx_publish(u, head);
    return 1u;
}

uint8_t Afr2Usart_PutByte(Afr2Usart *u, uint8_t b) { return Afr2Usart_Write(u, &b, 1u); }

uint8_t Afr2Usart_TxIdle(const Afr2Usart *u) {
    if (!u || !(u->flags & AFR2_USART_F_TX)) return 1u;
    return u->tx_head == u->tx_tail && (u->line_idle || (u->regs[0] & _BV(TXC0)));
}

uint16_t Afr2Usart_Available(const Afr2Usart *u) {
    if (!u || !(u->flags & AFR2_USART_F_RX)) return 0u;
    return (uint8_t)((uint8_t)(u->rx_head - u->rx_tail) & u->rx_mask);
}

uint8_t Afr2Usart_Read(Afr2Usart *u, uint8_t *b) {
    if (!u || !b || !(u->flags & AFR2_USART_F_RX)) return 0u;
    uint8_t tail = u->rx_tail;
    if (tail == u->rx_head) return 0u;
    *b = u->rx_buf[tail];
    u->rx_tail = (uint8_t)((tail + 1u) & u->rx_mask);   /* single-byte store: atomic */
    return 1u;
}

uint16_t Afr2Usart_ReadInto(Afr2Usart *u, uint8_t *dst, uint16_t max) {
    uint16_t n = 0;
    while (dst && n < max && Afr2Usart_Read(u, dst + n)) ++n;
    return n;
}

void Afr2Usart_FlushRx(Afr2Usart *u) {
    if (!u || !(u->flags & AFR2_USART_F_RX)) return;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { u->rx_tail = u->rx_head; }
}

uint8_t Afr2Usart_TakeErrors(Afr2Usart *u) {
    uint8_t e = 0;
    if (!u) return 0u;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { e = u->errors; u->errors = 0; }
    return e;
}

uint8_t Afr2Usart_TakeEvents(Afr2Usart *u) {
    uint8_t e = 0;
    if (!u) return 0u;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { e = u->pending; u->pending = 0; }
    return e;
}

void Afr2Usart_Poll(Afr2Usart *u) {
    if (!Afr2Usart_IsOpen(u)) return;
    uint8_t events = (uint8_t)(Afr2Usart_TakeEvents(u) & u->event_mask);
    if (events && u->on_event) u->on_event(u, events, u->ctx);
}

void Afr2Usart_GetStats(const Afr2Usart *u, Afr2UsartStats *out) {
    if (!out) return;
    memset(out, 0, sizeof(*out));
    if (!u) return;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        out->rx_overflow = u->rx_overflow; out->rx_errors = u->rx_errors;
    }
    out->tx_rejected = u->tx_rejected;
}
