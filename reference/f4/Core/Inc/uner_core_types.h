/*
 * uner_core.h - UNER v2 core parser/queue
 */
#ifndef INC_UNER_CORE_H_DATA
#define INC_UNER_CORE_H_DATA

#include <stdint.h>
#include <stddef.h>
#include "uner_v2.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    UNER_RESET_NONE = 0u,
    UNER_RESET_STATE_INVALID,
    UNER_RESET_LEN_FAIL,
    UNER_RESET_TOKEN_FAIL,
    UNER_RESET_VER_FAIL,
    UNER_RESET_CHK_FAIL,
    UNER_RESET_FRAME_DONE,
    UNER_RESET_DEFAULT_CASE
} UNER_ResetReason;

#include "uner_status.h"

typedef struct {
    uint8_t ver;
    uint8_t src;
    uint8_t dst;
    uint8_t cmd;
    uint8_t len;
    uint8_t *payload;
    uint8_t transport_id;
} UNER_Packet;

typedef void (*UNER_OnPacketCb)(void *ctx, const UNER_Packet *p);

typedef struct {
    uint8_t this_node;
    uint16_t accept_src_mask;
    uint16_t accept_dst_mask;
    uint8_t accept_broadcast;
    uint8_t default_max_payload;
    UNER_OnPacketCb on_packet;
    void *cb_ctx;
} UNER_CoreConfig;

typedef enum __attribute__((packed)) {
    UNER_S_H0 = 0,
    UNER_S_H1,
    UNER_S_H2,
    UNER_S_H3,
    UNER_S_LEN,
    UNER_S_TOKEN,
    UNER_S_VER,
    UNER_S_ROUTE,
    UNER_S_CMD,
    UNER_S_PAYLOAD,
    UNER_S_CHK
} UNER_ParserState;

typedef struct {
    UNER_CoreConfig cfg;

    UNER_ParserState state;
    uint8_t len_expected;
    uint8_t payload_index;
    uint8_t chk_acc;
    uint8_t route;
    uint8_t cmd;
    uint8_t ver;
    uint8_t transport_id;
    uint8_t max_payload_transport;
    uint8_t drop_frame;

    UNER_Packet *pkt_slots;
    uint8_t *payload_pool;
    uint16_t payload_stride;
    uint8_t slot_count;
    volatile uint8_t q_head;
    volatile uint8_t q_tail;
    volatile uint8_t q_count;

    uint32_t ok_frames;
    uint32_t chk_fail;
    uint32_t len_fail;
    uint32_t header_resync;
    uint32_t token_fail;
    uint32_t ver_fail;
    uint32_t queue_overflow;

    UNER_ResetReason last_reset_reason;
    uint8_t last_reset_state;
} UNER_Core;


#ifdef __cplusplus
}
#endif
#endif
