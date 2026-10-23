#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UNER_HEADER_0 ((uint8_t)'U')
#define UNER_HEADER_1 ((uint8_t)'N')
#define UNER_HEADER_2 ((uint8_t)'E')
#define UNER_HEADER_3 ((uint8_t)'R')
#define UNER_TOKEN ((uint8_t)0x3A)
#define UNER_VER_V2 ((uint8_t)0x02)

#define UNER_MAX_PAYLOAD 255u
#define UNER_OVERHEAD 10u
#define UNER_MAX_FRAME (UNER_OVERHEAD + UNER_MAX_PAYLOAD)
#define UNER_BROADCAST_NODE 0x0Fu

#define UNER_NODE_MCU 0x1u
#define UNER_NODE_PC_QT 0x2u
#define UNER_NODE_WEB_APP 0x3u
#define UNER_NODE_REMOTE_NRF 0x4u
#define UNER_NODE_WEB_CLIENT_MIN 0x5u
#define UNER_NODE_WEB_CLIENT_MAX 0xCu

#define UNER_ROUTE_MAKE(src, dst) ((uint8_t)((((src) & 0x0Fu) << 4) | ((dst) & 0x0Fu)))
#define UNER_ROUTE_SRC(route) ((uint8_t)(((route) >> 4) & 0x0Fu))
#define UNER_ROUTE_DST(route) ((uint8_t)((route) & 0x0Fu))

typedef enum {
    UNER_OK = 0,
    UNER_ERR_BUSY,
    UNER_ERR_LEN,
    UNER_ERR_CHK,
    UNER_ERR_HEADER,
    UNER_ERR_TOKEN,
    UNER_ERR_VER,
    UNER_ERR_QUEUE_FULL,
    UNER_ERR_FILTER,
    UNER_ERR_UNKNOWN_CMD,
    UNER_ERR_ARGS
} UNER_Status;

typedef struct {
    uint8_t ver;
    uint8_t src;
    uint8_t dst;
    uint8_t cmd;
    uint8_t len;
    uint8_t *payload;
    uint8_t transport_id;
} UNER_Packet;

#ifdef __cplusplus
}
#endif
