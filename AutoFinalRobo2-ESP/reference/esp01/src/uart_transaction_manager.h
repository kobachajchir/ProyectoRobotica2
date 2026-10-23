#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UTM_QUEUE_CAPACITY 8u
#define UTM_MAX_QUEUED_PER_CLIENT 2u
#define UTM_REQUEST_ID_CAPACITY 65u
#define UTM_MAX_PAYLOAD 255u

#define UTM_CMD_ACK 0xE0u
#define UTM_CMD_NACK 0xE1u
#define UTM_EVENT_MPU_SAMPLE 0x90u
#define UTM_EVENT_IR_SAMPLE 0x91u

#define UTM_INTERNAL_WEB 0u
#define UTM_INTERNAL_STREAM_MPU 1u
#define UTM_INTERNAL_STREAM_IR 2u
#define UTM_INTERNAL_AUTH_CLEANUP 3u
#define UTM_INTERNAL_STREAM_RECONCILE 4u
#define UTM_INTERNAL_OLED_CANVAS 5u

typedef enum {
    UTM_OWNER_F4 = 0,
    UTM_OWNER_ESP,
    UTM_OWNER_DUAL,
    UTM_OWNER_RESERVED
} UtmCommandOwner;

typedef enum {
    UTM_RESPONSE_ACK = 0,
    UTM_RESPONSE_COMMAND,
    UTM_RESPONSE_ACK_OR_COMMAND,
    UTM_RESPONSE_ACK_AND_COMMAND
} UtmResponsePolicy;

typedef enum {
    UTM_RETRY_NONE = 0,
    UTM_RETRY_IDEMPOTENT
} UtmRetryPolicy;

typedef enum {
    UTM_SEND_ERROR = -1,
    UTM_SEND_OK = 0,
    UTM_SEND_BUSY = 1
} UtmSendResult;

typedef enum {
    UTM_ENQUEUE_OK = 0,
    UTM_ENQUEUE_INVALID,
    UTM_ENQUEUE_QUEUE_FULL,
    UTM_ENQUEUE_CLIENT_LIMIT,
    UTM_ENQUEUE_INCOMPATIBLE_OWNER
} UtmEnqueueResult;

typedef enum {
    UTM_COMPLETED_RESPONSE = 0,
    UTM_COMPLETED_ACK,
    UTM_COMPLETED_F4_NACK,
    UTM_COMPLETED_TIMEOUT,
    UTM_COMPLETED_UART_ERROR,
    UTM_COMPLETED_CANCELED
} UtmCompletionStatus;

typedef enum {
    UTM_FRAME_UNMATCHED = 0,
    UTM_FRAME_MATCHED_PENDING,
    UTM_FRAME_COMPLETED,
    UTM_FRAME_IGNORED_EVENT
} UtmFrameResult;

typedef struct {
    uint8_t src;
    uint8_t dst;
    uint8_t command_id;
    const uint8_t *payload;
    uint16_t payload_len;
} UtmFrame;

typedef struct {
    uint32_t client_id;
    uint32_t generation;
    uint8_t node;
    uint8_t dst_node;
    uint8_t command_id;
    UtmCommandOwner owner;
    UtmResponsePolicy response_policy;
    UtmRetryPolicy retry_policy;
    uint8_t idempotent;
    uint8_t max_retries;
    uint8_t retries_done;
    uint16_t payload_len;
    uint8_t payload[UTM_MAX_PAYLOAD];
    char request_id[UTM_REQUEST_ID_CAPACITY];
    char command_name[UTM_REQUEST_ID_CAPACITY];
    uint8_t api_v1;
    uint8_t internal_kind;
    uint32_t timeout_ms;
    uint32_t deadline_ms;
} UtmRequest;

typedef struct {
    uint32_t enqueued;
    uint32_t rejected_invalid;
    uint32_t rejected_full;
    uint32_t rejected_client_limit;
    uint32_t rejected_owner;
    uint32_t started;
    uint32_t completed;
    uint32_t f4_nacks;
    uint32_t timed_out;
    uint32_t uart_errors;
    uint32_t retries;
    uint32_t send_busy;
    uint32_t canceled_queued;
    uint32_t orphaned_inflight;
    uint32_t ignored_stream_events;
    uint32_t unmatched_frames;
    uint8_t max_queue_depth;
} UtmMetrics;

typedef UtmSendResult (*UtmSendFn)(void *ctx, const UtmRequest *request);
typedef void (*UtmCompletionFn)(void *ctx,
                                const UtmRequest *request,
                                UtmCompletionStatus status,
                                const UtmFrame *frame);

typedef struct {
    UtmRequest queue[UTM_QUEUE_CAPACITY];
    uint8_t queue_count;
    UtmRequest inflight;
    uint8_t inflight_active;
    uint8_t inflight_orphaned;
    uint8_t ack_seen;
    uint8_t response_seen;
    uint32_t attempt_deadline_ms;
    UtmSendFn send_fn;
    void *send_ctx;
    UtmCompletionFn completion_fn;
    void *completion_ctx;
    UtmMetrics metrics;
} UartTransactionManager;

void utm_init(UartTransactionManager *manager,
              UtmSendFn send_fn,
              void *send_ctx,
              UtmCompletionFn completion_fn,
              void *completion_ctx);

void utm_request_init(UtmRequest *request);

UtmEnqueueResult utm_enqueue(UartTransactionManager *manager,
                             const UtmRequest *request,
                             uint32_t now_ms);

void utm_service(UartTransactionManager *manager, uint32_t now_ms);

UtmFrameResult utm_on_frame(UartTransactionManager *manager,
                            const UtmFrame *frame,
                            uint32_t now_ms);

void utm_disconnect_client(UartTransactionManager *manager,
                           uint32_t client_id,
                           uint32_t generation,
                           uint32_t now_ms);

// Cancels every queued/in-flight request without starting another one. Used
// when F4 is rebooting or its alive lease is lost.
void utm_cancel_all(UartTransactionManager *manager);

uint8_t utm_queued_count(const UartTransactionManager *manager);
uint8_t utm_client_queued_count(const UartTransactionManager *manager,
                                uint32_t client_id,
                                uint32_t generation);
const UtmRequest *utm_inflight(const UartTransactionManager *manager);
const UtmMetrics *utm_metrics(const UartTransactionManager *manager);

#ifdef __cplusplus
}
#endif
