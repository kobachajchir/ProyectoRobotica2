/* AFR2-20261023-15. ROBO2 endpoint; no hardware access from the protocol core. */
#ifndef AFR2_LINK_H
#define AFR2_LINK_H
#include <stdint.h>
#include "afr2_wire.h"
typedef struct {
    uint8_t mode;                 /* 0 idle, 1 manual, 2 line, 3 test, 4 goto, 5 fault, 255 unknown */
    uint8_t stopped;              /* 0 false, 1 confirmed by application, 255 unknown */
    uint8_t line_valid, line_bits, ultrasonic_valid, encoders_wired, encoders_valid;
    uint16_t distance_mm;
    uint32_t state_age_ms, line_age_ms, ultrasonic_age_ms, encoders_age_ms;
    int32_t counts[4];            /* FL FR RL RR */
} Afr2LinkSnapshot;
typedef struct {
    void *ctx;
    void (*snapshot)(void *ctx, uint32_t now_ms, Afr2LinkSnapshot *out);
    uint8_t (*stop)(void *ctx);    /* return 1 ONLY after authoritative output shutdown */
    uint16_t capabilities;       /* only services really integrated by the application */
} Afr2LinkOps;
typedef struct {
    Afr2WireParser parser;
    Afr2LinkOps ops;
    uint32_t session, last_transaction, last_rx_ms, last_request_ms;
    uint8_t negotiated, supervised;
    uint8_t reply[AFR2_WIRE_FRAME_MAX], reply_length;
} Afr2Link;
void Afr2Link_Init(Afr2Link *link, const Afr2LinkOps *ops);
void Afr2Link_Tick(Afr2Link *link, uint32_t now_ms);
void Afr2Link_RxFault(Afr2Link *link);
void Afr2Link_Push(Afr2Link *link, uint8_t byte, uint32_t now_ms);
/* Do not push new bytes while reply_length != 0. Copy to TX then clear it. */
#endif
