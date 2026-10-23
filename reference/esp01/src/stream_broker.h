#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "ws_limits.h"

#define SB_MAX_CLIENTS WS_MAX_SESSIONS
#define SB_PERIOD_MIN_MS 20u
#define SB_PERIOD_MAX_MS 1000u
#define SB_CONTROL_NODE 0x03u

typedef enum {
    SB_STREAM_MPU = 0,
    SB_STREAM_IR = 1,
    SB_STREAM_COUNT = 2
} SbStream;

typedef enum {
    SB_OK = 0,
    SB_INVALID,
    SB_CAPACITY,
    SB_NOT_FOUND,
    SB_STALE_ACTION
} SbResult;

typedef enum {
    SB_ACTION_NONE = 0,
    SB_ACTION_SET,
    SB_ACTION_STOP
} SbActionType;

typedef struct {
    uint32_t client_id;
    uint32_t generation;
    uint8_t node;
} SbEndpoint;

typedef struct {
    uint8_t used;
    SbEndpoint endpoint;
    uint8_t subscribed[SB_STREAM_COUNT];
    uint16_t period_ms[SB_STREAM_COUNT];
} SbSubscriber;

typedef struct {
    SbActionType type;
    SbStream stream;
    SbEndpoint endpoint;
    uint16_t period_ms;
    uint8_t reconcile;
} SbAction;

typedef struct {
    uint8_t desired_active;
    uint16_t desired_period_ms;
    uint8_t desired_owner_valid;
    SbEndpoint desired_owner;
    uint8_t applied_active;
    uint16_t applied_period_ms;
    uint8_t applied_owner_valid;
    SbEndpoint applied_owner;
    uint8_t needs_known_stop;
    uint8_t pending;
    uint8_t action_blocked;
    SbAction pending_action;
} SbStreamState;

typedef struct {
    uint32_t subscriptions;
    uint32_t unsubscriptions;
    uint32_t client_removals;
    uint32_t desired_period_changes;
    uint32_t set_actions;
    uint32_t stop_actions;
    uint32_t owner_transfers;
    uint32_t reconcile_cycles;
    uint32_t action_failures;
    uint32_t stale_action_results;
    uint32_t pending_actions_aborted;
    uint32_t action_failures_blocked;
    uint32_t explicit_retries;
    uint32_t fanout_allowed;
    uint32_t fanout_denied;
} SbMetrics;

typedef struct {
    SbSubscriber subscribers[SB_MAX_CLIENTS];
    SbStreamState streams[SB_STREAM_COUNT];
    SbMetrics metrics;
} StreamBroker;

void sb_init(StreamBroker *broker);
uint16_t sb_clamp_period(uint16_t period_ms);

SbResult sb_subscribe(StreamBroker *broker,
                      uint32_t client_id,
                      uint32_t generation,
                      uint8_t node,
                      SbStream stream,
                      uint16_t period_ms);

SbResult sb_unsubscribe(StreamBroker *broker,
                        uint32_t client_id,
                        uint32_t generation,
                        SbStream stream);

uint8_t sb_remove_client(StreamBroker *broker,
                         uint32_t client_id,
                         uint32_t generation);

SbAction sb_take_action(StreamBroker *broker, SbStream stream);
SbResult sb_complete_action(StreamBroker *broker,
                            const SbAction *action,
                            uint8_t success);

uint8_t sb_is_converged(const StreamBroker *broker, SbStream stream);

void sb_backend_reconnected(StreamBroker *broker);

uint8_t sb_should_fanout(StreamBroker *broker,
                         uint32_t client_id,
                         uint32_t generation,
                         SbStream stream,
                         uint8_t authenticated);

const SbStreamState *sb_stream_state(const StreamBroker *broker, SbStream stream);
const SbMetrics *sb_metrics(const StreamBroker *broker);

#ifdef __cplusplus
}
#endif
