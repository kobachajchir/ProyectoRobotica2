/* AFR2-20261108-01. afr2_usart: reusable, instance-based USART driver.
 *
 * - One handle (Afr2Usart) per hardware USART; any number of ports, chosen by index.
 * - Caller-owned ring buffers (power of two, 2..256 bytes). No dynamic memory.
 * - Configuration by initializer (AFR2_USART_CONFIG_INIT) plus field overrides.
 * - Events are LATCHED in interrupt context and DELIVERED only from
 *   Afr2Usart_Poll() in main context. Callbacks never run inside an ISR.
 *
 * This header is portable. The port header included at the end supplies the
 * complete struct Afr2Usart and the interrupt binding for each target
 * (AVR now; an ESP8266 port can implement the same functions over its UARTs).
 * See README.md in this folder.
 */
#ifndef AFR2_USART_H
#define AFR2_USART_H
#include <stdint.h>
#include <stddef.h>
#ifdef __cplusplus
extern "C" {
#endif

/* Pin value meaning "the port's default pins". On AVR the USART pins are fixed
 * by silicon, so any other value must match them or Init fails with ERR_PIN. */
#define AFR2_USART_PIN_DEFAULT 0xFFu

typedef enum {
    AFR2_USART_PARITY_NONE = 0,
    AFR2_USART_PARITY_EVEN = 2,
    AFR2_USART_PARITY_ODD = 3
} Afr2UsartParity;

typedef enum {
    AFR2_USART_OK = 0,
    AFR2_USART_ERR_ARG,     /* null handle/config or invalid frame format */
    AFR2_USART_ERR_PORT,    /* port index does not exist on this target */
    AFR2_USART_ERR_BUSY,    /* port already owned by another open handle */
    AFR2_USART_ERR_BAUD,    /* no divisor within max_baud_error_permille */
    AFR2_USART_ERR_BUFFER,  /* size not a power of two in 2..256, or no direction enabled */
    AFR2_USART_ERR_PIN,     /* requested pins are not available for this port */
    AFR2_USART_ERR_NO_ISR   /* interrupt handlers for this port were not bound/linked */
} Afr2UsartStatus;

/* Event bits (Afr2UsartConfig.event_mask and callback argument). */
enum {
    AFR2_USART_EVT_RX = 0x01u,          /* new bytes are available */
    AFR2_USART_EVT_RX_OVERFLOW = 0x02u, /* RX ring full: bytes discarded */
    AFR2_USART_EVT_RX_ERROR = 0x04u,    /* frame/overrun/parity, see Afr2Usart_TakeErrors */
    AFR2_USART_EVT_TX_EMPTY = 0x08u,    /* TX ring handed completely to the hardware */
    AFR2_USART_EVT_TX_DONE = 0x10u,     /* last stop bit has left the pin (line idle) */
    AFR2_USART_EVT_ALL = 0x1Fu
};

/* Error flags returned by Afr2Usart_TakeErrors. */
enum {
    AFR2_USART_ERRF_FRAME = 0x01u,
    AFR2_USART_ERRF_OVERRUN = 0x02u,  /* hardware lost bytes before the current one */
    AFR2_USART_ERRF_PARITY = 0x04u
};

/* Policy for bytes received with a frame or parity error. Overrun does not
 * corrupt the current byte, so that byte is always kept. */
enum { AFR2_USART_RX_DROP_ERRORS = 0u, AFR2_USART_RX_KEEP_ERRORS = 1u };

typedef struct Afr2Usart Afr2Usart;
typedef Afr2Usart *Afr2UsartHandle;
/* Runs in main context, from Afr2Usart_Poll only. */
typedef void (*Afr2UsartEventFn)(Afr2UsartHandle usart, uint8_t events, void *ctx);

typedef struct {
    uint8_t port;              /* hardware index, 0..Afr2Usart_PortCount()-1 */
    uint32_t baud;
    uint8_t data_bits;         /* 5..8 */
    uint8_t parity;            /* Afr2UsartParity */
    uint8_t stop_bits;         /* 1 or 2 */
    uint8_t rx_pin, tx_pin;    /* board pin numbers or AFR2_USART_PIN_DEFAULT */
    uint8_t rx_pullup;         /* 1: internal pull-up on RX */
    uint8_t *rx_buffer;        /* NULL/0: receiver disabled */
    uint16_t rx_size;
    uint8_t *tx_buffer;        /* NULL/0: transmitter disabled */
    uint16_t tx_size;
    uint8_t rx_error_policy;   /* AFR2_USART_RX_DROP_ERRORS / KEEP_ERRORS */
    uint8_t event_mask;        /* events delivered to on_event (TX_DONE also enables its IRQ) */
    Afr2UsartEventFn on_event; /* optional */
    void *ctx;
    uint16_t max_baud_error_permille; /* 0 = default 25 (2.5 %) */
} Afr2UsartConfig;

/* 8N1, default pins, no buffers, no events. Assign buffers before Init. */
#define AFR2_USART_CONFIG_INIT(port_, baud_) \
    { (port_), (baud_), 8u, AFR2_USART_PARITY_NONE, 1u, \
      AFR2_USART_PIN_DEFAULT, AFR2_USART_PIN_DEFAULT, 0u, \
      NULL, 0u, NULL, 0u, AFR2_USART_RX_DROP_ERRORS, 0u, NULL, NULL, 0u }

typedef struct {
    uint16_t rx_overflow;  /* bytes lost because the RX ring was full */
    uint16_t rx_errors;    /* bytes flagged by hardware (frame/overrun/parity) */
    uint16_t tx_rejected;  /* Write/PutByte calls refused for lack of space */
} Afr2UsartStats;

typedef struct { uint8_t rx_pin, tx_pin; } Afr2UsartPortInfo;

uint8_t Afr2Usart_PortCount(void);
uint8_t Afr2Usart_PortInfo(uint8_t port, Afr2UsartPortInfo *out);

Afr2UsartStatus Afr2Usart_Init(Afr2Usart *usart, const Afr2UsartConfig *config);
void Afr2Usart_Deinit(Afr2Usart *usart);
uint8_t Afr2Usart_IsOpen(const Afr2Usart *usart);
uint32_t Afr2Usart_ActualBaud(const Afr2Usart *usart);

/* Non-blocking. Write is all-or-nothing: 1 = queued, 0 = no space/closed. */
uint8_t Afr2Usart_Write(Afr2Usart *usart, const void *data, uint16_t length);
uint8_t Afr2Usart_PutByte(Afr2Usart *usart, uint8_t byte);
uint16_t Afr2Usart_TxFree(const Afr2Usart *usart);
uint8_t Afr2Usart_TxIdle(const Afr2Usart *usart);

uint16_t Afr2Usart_Available(const Afr2Usart *usart);
uint8_t Afr2Usart_Read(Afr2Usart *usart, uint8_t *byte);
uint16_t Afr2Usart_ReadInto(Afr2Usart *usart, uint8_t *dst, uint16_t max);
void Afr2Usart_FlushRx(Afr2Usart *usart);

uint8_t Afr2Usart_TakeErrors(Afr2Usart *usart);  /* AFR2_USART_ERRF_*, cleared */
uint8_t Afr2Usart_TakeEvents(Afr2Usart *usart);  /* pending AFR2_USART_EVT_*, cleared */
void Afr2Usart_Poll(Afr2Usart *usart);           /* delivers pending & event_mask */
void Afr2Usart_GetStats(const Afr2Usart *usart, Afr2UsartStats *out);

#ifdef __cplusplus
}
#endif

#if defined(__AVR__)
#include "afr2_usart_avr.h"
#else
#error "afr2_usart: no port for this target yet (see README.md, Portar a otro micro)"
#endif
#endif
