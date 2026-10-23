/*
 * uner_handle.h - UNER v2 manager
 */
#ifndef INC_UNER_HANDLE_H_DATA
#define INC_UNER_HANDLE_H_DATA

#include <stdint.h>
#include "uner_transport.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UNER_MAX_TRANSPORTS 4u
#define UNER_MAX_ROUTES     6u
#define UNER_ROUTER_PAYLOAD_PREVIEW_BYTES 4u

typedef void (*UNER_CommandHandler)(void *ctx, const UNER_Packet *packet);

#define UNER_SPEC_F_ACK   (1u << 0)
#define UNER_SPEC_F_RESP  (1u << 1)
#define UNER_SPEC_F_EVT   (1u << 2)

typedef struct {
    uint8_t id;
    uint8_t min_args;
    uint8_t max_args;
    uint8_t flags;
    uint16_t timeout_ms;
    UNER_CommandHandler handler;
} UNER_CommandSpec;

typedef void (*UNER_ExecuteCommandFn)(void *ctx, const UNER_Packet *packet);
typedef uint8_t (*UNER_ForwardGuardFn)(void *ctx,
                                       const UNER_Packet *packet,
                                       uint8_t out_transport_id);

typedef struct {
    uint8_t node;
    uint8_t transport_id;
    uint8_t enabled;
} UNER_RouteEntry;

typedef enum {
    UNER_ROUTER_STATUS_NONE = 0u,
    UNER_ROUTER_STATUS_RECEIVED,
    UNER_ROUTER_STATUS_FORWARDED,
    UNER_ROUTER_STATUS_SENT,
    UNER_ROUTER_STATUS_DISABLED,
    UNER_ROUTER_STATUS_ROUTE_MISS,
    UNER_ROUTER_STATUS_TX_BUSY,
    UNER_ROUTER_STATUS_SAME_TRANSPORT,
    UNER_ROUTER_STATUS_INVALID,
    UNER_ROUTER_STATUS_BLOCKED
} UNER_RouterStatus;

typedef enum {
    UNER_ROUTER_PACKET_RX = 0u,
    UNER_ROUTER_PACKET_TX = 1u
} UNER_RouterPacketDirection;

typedef struct {
    uint8_t valid;
    uint8_t forwarding_enabled;
    uint8_t direction;
    uint8_t src;
    uint8_t dst;
    uint8_t cmd;
    uint8_t len;
    uint8_t in_transport;
    uint8_t out_transport;
    uint8_t status;
    uint8_t payload_preview_len;
    uint8_t payload_preview[UNER_ROUTER_PAYLOAD_PREVIEW_BYTES];
    uint32_t sequence;
    uint32_t forwarded_frames;
    uint32_t route_miss;
    uint32_t route_tx_busy;
} UNER_RouterSnapshot;

typedef struct {
    UNER_Core core;
    UNER_Transport *transports[UNER_MAX_TRANSPORTS];
    uint8_t transport_count;
    UNER_RouteEntry routes[UNER_MAX_ROUTES];
    uint8_t route_count;
    UNER_ForwardGuardFn forward_guard;
    void *forward_guard_ctx;
    uint8_t forwarding_enabled;
    UNER_RouterSnapshot router_snapshot;
    UNER_RouterSnapshot rx_snapshot;
    UNER_RouterSnapshot tx_snapshot;
    UNER_RouterSnapshot rx_transport_snapshots[UNER_MAX_TRANSPORTS];
    UNER_RouterSnapshot tx_transport_snapshots[UNER_MAX_TRANSPORTS];
    volatile uint8_t packet_pending;
    const UNER_CommandSpec *command_table;
    uint8_t command_count;
    UNER_ExecuteCommandFn execute_command;
    void *execute_ctx;
    UNER_OnPacketCb app_on_packet;
    void *app_on_packet_ctx;
    uint8_t tx_buf[UNER_OVERHEAD + 255u];
    uint32_t forwarded_frames;
    uint32_t route_miss;
    uint32_t route_tx_busy;
    uint32_t packet_sequence;
} UNER_Handle;


#ifdef __cplusplus
}
#endif
#endif
