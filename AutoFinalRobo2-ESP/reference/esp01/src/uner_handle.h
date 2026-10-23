#pragma once

#include "uner_core.h"
#include "uner_cmdspec.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UNER_MAX_TRANSPORTS 4u
#define UNER_RX_SLOTS 6u
#define UNER_TX_SLOTS 4u
#define UNER_POLL_BUDGET 64u

typedef uint16_t (*UNER_TransportPollRxFn)(void *ctx, uint8_t *out, uint16_t max_len);
typedef int (*UNER_TransportTrySendFn)(void *ctx, const uint8_t *data, uint16_t len);
typedef uint8_t (*UNER_PacketInterceptorFn)(void *ctx, const UNER_Packet *packet);

typedef struct {
    uint8_t id;
    uint8_t max_payload;
    void *ctx;
    UNER_TransportPollRxFn poll_rx;
    UNER_TransportTrySendFn try_send;
} UNER_Transport;

typedef uint32_t (*UNER_NowMsFn)(void *ctx);
typedef void (*UNER_PacketLogFn)(void *ctx, const UNER_Packet *packet);

typedef struct {
    UNER_NowMsFn now_ms;
    void *clock_ctx;
    UNER_PacketLogFn log_packet;
    void *log_ctx;
} UNER_HandleOps;

typedef struct {
    UNER_NowMsFn now_ms;
    void *clock_ctx;
    UNER_PacketLogFn log_packet;
    void *log_ctx;
    UNER_Core core;
    UNER_Transport *transports[UNER_MAX_TRANSPORTS];
    uint8_t transport_count;
    const UNER_CommandSpec *command_table;
    uint8_t command_count;
    void *app_ctx;
    UNER_PacketInterceptorFn packet_interceptor;
    void *packet_interceptor_ctx;

    UNER_Packet rx_slots[UNER_RX_SLOTS];
    uint8_t rx_payload_pool[UNER_RX_SLOTS * UNER_MAX_PAYLOAD];

    uint8_t tx_buf[UNER_MAX_FRAME];
    uint8_t tx_q_data[UNER_TX_SLOTS][UNER_MAX_FRAME];
    uint16_t tx_q_len[UNER_TX_SLOTS];
    uint8_t tx_q_head;
    uint8_t tx_q_tail;
    uint8_t tx_q_count;

    volatile uint8_t flag_1ms;
    volatile uint8_t flag_10ms;
    volatile uint8_t flag_50ms;
    volatile uint8_t flag_100ms;
    uint32_t last_ms;
    uint8_t tick10;
    uint8_t tick50;
    uint8_t tick100;
} UNER_Handle;

UNER_Status UNER_Handle_Init(UNER_Handle *h, const UNER_CoreConfig *cfg, const UNER_CommandSpec *table, uint8_t table_count, void *app_ctx);
UNER_Status UNER_Handle_InitWithOps(UNER_Handle *h, const UNER_CoreConfig *cfg,
    const UNER_CommandSpec *table, uint8_t table_count, void *app_ctx,
    const UNER_HandleOps *ops);
UNER_Status UNER_Handle_AddTransport(UNER_Handle *h, UNER_Transport *t);
void UNER_Handle_SetPacketInterceptor(UNER_Handle *h, UNER_PacketInterceptorFn fn, void *ctx);
void UNER_Handle_SetClock(UNER_Handle *h, UNER_NowMsFn fn, void *ctx);
void UNER_Handle_SetPacketLogger(UNER_Handle *h, UNER_PacketLogFn fn, void *ctx);
void UNER_Handle_PollRx(UNER_Handle *h);
void UNER_Handle_Dispatch(UNER_Handle *h);
void UNER_Handle_Tick(UNER_Handle *h);
void UNER_Handle_TxPump(UNER_Handle *h);

UNER_Status UNER_Handle_SendRaw(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len);
UNER_Status UNER_Handle_SendAck(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t acked_cmd, uint8_t status);
UNER_Status UNER_Handle_SendNack(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t cmd, uint8_t reason);
UNER_Status UNER_Handle_SendResponse(UNER_Handle *h, const UNER_Packet *req, uint8_t status, const uint8_t *data, uint8_t data_len);

#ifdef __cplusplus
}
#endif
