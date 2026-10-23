#include "uart_transaction_manager.h"

#include <string.h>

#define UTM_RETRY_BUSY_DELAY_MS 1u

static uint8_t time_reached(uint32_t now_ms, uint32_t deadline_ms)
{
    return (uint8_t)(((int32_t)(now_ms - deadline_ms)) >= 0);
}

static uint32_t add_ms_saturated(uint32_t now_ms, uint32_t delta_ms)
{
    return now_ms + delta_ms;
}

static uint32_t earlier_deadline(uint32_t first, uint32_t second)
{
    return ((int32_t)(first - second) <= 0) ? first : second;
}

static void notify_completion(UartTransactionManager *manager,
                              const UtmRequest *request,
                              UtmCompletionStatus status,
                              const UtmFrame *frame)
{
    if (manager->completion_fn) {
        manager->completion_fn(manager->completion_ctx, request, status, frame);
    }
}

static void remove_queue_at(UartTransactionManager *manager, uint8_t index)
{
    uint8_t i;
    if (!manager || index >= manager->queue_count) return;
    for (i = index; (uint8_t)(i + 1u) < manager->queue_count; ++i) {
        manager->queue[i] = manager->queue[i + 1u];
    }
    manager->queue_count--;
    memset(&manager->queue[manager->queue_count], 0, sizeof(manager->queue[0]));
}

static uint32_t default_deadline_span(const UtmRequest *request)
{
    uint32_t attempts = 1u;
    uint32_t span;
    if (request->retry_policy == UTM_RETRY_IDEMPOTENT && request->idempotent) {
        attempts += request->max_retries;
    }
    if (request->timeout_ms != 0u && attempts > (UINT32_MAX / request->timeout_ms)) {
        return UINT32_MAX;
    }
    span = request->timeout_ms * attempts;
    return span == 0u ? 1u : span;
}

static uint8_t request_is_valid(const UtmRequest *request)
{
    if (!request) return 0u;
    if (request->node > 0x0Fu || request->dst_node > 0x0Fu) return 0u;
    if (request->payload_len > UTM_MAX_PAYLOAD) return 0u;
    if (request->timeout_ms == 0u) return 0u;
    if (request->response_policy > UTM_RESPONSE_ACK_AND_COMMAND) return 0u;
    if (request->retry_policy > UTM_RETRY_IDEMPOTENT) return 0u;
    if (request->retry_policy == UTM_RETRY_IDEMPOTENT && !request->idempotent) return 0u;
    if (request->command_id == UTM_EVENT_MPU_SAMPLE || request->command_id == UTM_EVENT_IR_SAMPLE) return 0u;
    return 1u;
}

static void expire_queued(UartTransactionManager *manager, uint32_t now_ms)
{
    uint8_t i = 0u;
    while (i < manager->queue_count) {
        if (time_reached(now_ms, manager->queue[i].deadline_ms)) {
            UtmRequest expired = manager->queue[i];
            remove_queue_at(manager, i);
            manager->metrics.timed_out++;
            manager->metrics.completed++;
            notify_completion(manager, &expired, UTM_COMPLETED_TIMEOUT, NULL);
        } else {
            i++;
        }
    }
}

static void complete_inflight(UartTransactionManager *manager,
                              UtmCompletionStatus status,
                              const UtmFrame *frame,
                              uint32_t now_ms);

static void try_start_next(UartTransactionManager *manager, uint32_t now_ms)
{
    if (!manager || manager->inflight_active || !manager->send_fn) return;

    expire_queued(manager, now_ms);
    while (!manager->inflight_active && manager->queue_count > 0u) {
        UtmRequest next = manager->queue[0];
        UtmSendResult result;

        if (time_reached(now_ms, next.deadline_ms)) {
            remove_queue_at(manager, 0u);
            manager->metrics.timed_out++;
            manager->metrics.completed++;
            notify_completion(manager, &next, UTM_COMPLETED_TIMEOUT, NULL);
            continue;
        }

        result = manager->send_fn(manager->send_ctx, &next);
        if (result == UTM_SEND_BUSY) {
            manager->metrics.send_busy++;
            return;
        }

        remove_queue_at(manager, 0u);
        if (result != UTM_SEND_OK) {
            manager->metrics.uart_errors++;
            manager->metrics.completed++;
            notify_completion(manager, &next, UTM_COMPLETED_UART_ERROR, NULL);
            continue;
        }

        manager->inflight = next;
        manager->inflight_active = 1u;
        manager->inflight_orphaned = 0u;
        manager->ack_seen = 0u;
        manager->response_seen = 0u;
        manager->attempt_deadline_ms = earlier_deadline(
            add_ms_saturated(now_ms, next.timeout_ms), next.deadline_ms);
        manager->metrics.started++;
    }
}

static void complete_inflight(UartTransactionManager *manager,
                              UtmCompletionStatus status,
                              const UtmFrame *frame,
                              uint32_t now_ms)
{
    UtmRequest completed;
    uint8_t orphaned;
    if (!manager || !manager->inflight_active) return;

    completed = manager->inflight;
    orphaned = manager->inflight_orphaned;
    memset(&manager->inflight, 0, sizeof(manager->inflight));
    manager->inflight_active = 0u;
    manager->inflight_orphaned = 0u;
    manager->ack_seen = 0u;
    manager->response_seen = 0u;
    manager->attempt_deadline_ms = 0u;
    manager->metrics.completed++;

    if (status == UTM_COMPLETED_F4_NACK) manager->metrics.f4_nacks++;
    if (status == UTM_COMPLETED_TIMEOUT) manager->metrics.timed_out++;
    if (status == UTM_COMPLETED_UART_ERROR) manager->metrics.uart_errors++;
    if (!orphaned) notify_completion(manager, &completed, status, frame);

    try_start_next(manager, now_ms);
}

void utm_init(UartTransactionManager *manager,
              UtmSendFn send_fn,
              void *send_ctx,
              UtmCompletionFn completion_fn,
              void *completion_ctx)
{
    if (!manager) return;
    memset(manager, 0, sizeof(*manager));
    manager->send_fn = send_fn;
    manager->send_ctx = send_ctx;
    manager->completion_fn = completion_fn;
    manager->completion_ctx = completion_ctx;
}

void utm_request_init(UtmRequest *request)
{
    if (!request) return;
    memset(request, 0, sizeof(*request));
    request->owner = UTM_OWNER_F4;
    request->response_policy = UTM_RESPONSE_COMMAND;
    request->retry_policy = UTM_RETRY_NONE;
    request->timeout_ms = 700u;
}

uint8_t utm_client_queued_count(const UartTransactionManager *manager,
                                uint32_t client_id,
                                uint32_t generation)
{
    uint8_t i;
    uint8_t count = 0u;
    if (!manager) return 0u;
    for (i = 0u; i < manager->queue_count; ++i) {
        if (manager->queue[i].client_id == client_id &&
            manager->queue[i].generation == generation) {
            count++;
        }
    }
    return count;
}

UtmEnqueueResult utm_enqueue(UartTransactionManager *manager,
                             const UtmRequest *request,
                             uint32_t now_ms)
{
    UtmRequest queued;
    if (!manager || !manager->send_fn || !request_is_valid(request)) {
        if (manager) manager->metrics.rejected_invalid++;
        return UTM_ENQUEUE_INVALID;
    }
    if (request->owner != UTM_OWNER_F4 && request->owner != UTM_OWNER_DUAL) {
        manager->metrics.rejected_owner++;
        return UTM_ENQUEUE_INCOMPATIBLE_OWNER;
    }
    if (manager->queue_count >= UTM_QUEUE_CAPACITY) {
        manager->metrics.rejected_full++;
        return UTM_ENQUEUE_QUEUE_FULL;
    }
    if (utm_client_queued_count(manager, request->client_id, request->generation) >=
        UTM_MAX_QUEUED_PER_CLIENT) {
        manager->metrics.rejected_client_limit++;
        return UTM_ENQUEUE_CLIENT_LIMIT;
    }

    queued = *request;
    queued.request_id[UTM_REQUEST_ID_CAPACITY - 1u] = '\0';
    queued.command_name[UTM_REQUEST_ID_CAPACITY - 1u] = '\0';
    queued.retries_done = 0u;
    if (queued.deadline_ms == 0u) {
        queued.deadline_ms = add_ms_saturated(now_ms, default_deadline_span(&queued));
    }
    manager->queue[manager->queue_count++] = queued;
    manager->metrics.enqueued++;
    if (manager->queue_count > manager->metrics.max_queue_depth) {
        manager->metrics.max_queue_depth = manager->queue_count;
    }
    try_start_next(manager, now_ms);
    return UTM_ENQUEUE_OK;
}

void utm_service(UartTransactionManager *manager, uint32_t now_ms)
{
    if (!manager) return;
    expire_queued(manager, now_ms);

    if (manager->inflight_active) {
        uint8_t overall_expired = time_reached(now_ms, manager->inflight.deadline_ms);
        uint8_t attempt_expired = time_reached(now_ms, manager->attempt_deadline_ms);
        uint8_t can_retry = (uint8_t)(
            !manager->inflight_orphaned &&
            manager->inflight.idempotent &&
            manager->inflight.retry_policy == UTM_RETRY_IDEMPOTENT &&
            manager->inflight.retries_done < manager->inflight.max_retries);

        if (overall_expired) {
            complete_inflight(manager, UTM_COMPLETED_TIMEOUT, NULL, now_ms);
            return;
        }
        if (attempt_expired) {
            if (!can_retry) {
                complete_inflight(manager, UTM_COMPLETED_TIMEOUT, NULL, now_ms);
                return;
            }

            {
                UtmSendResult result = manager->send_fn(manager->send_ctx, &manager->inflight);
                if (result == UTM_SEND_OK) {
                    manager->inflight.retries_done++;
                    manager->metrics.retries++;
                    manager->attempt_deadline_ms = earlier_deadline(
                        add_ms_saturated(now_ms, manager->inflight.timeout_ms),
                        manager->inflight.deadline_ms);
                } else if (result == UTM_SEND_BUSY) {
                    manager->metrics.send_busy++;
                    manager->attempt_deadline_ms = earlier_deadline(
                        add_ms_saturated(now_ms, UTM_RETRY_BUSY_DELAY_MS),
                        manager->inflight.deadline_ms);
                } else {
                    manager->metrics.uart_errors++;
                    manager->attempt_deadline_ms = earlier_deadline(
                        add_ms_saturated(now_ms, UTM_RETRY_BUSY_DELAY_MS),
                        manager->inflight.deadline_ms);
                }
            }
        }
    }

    if (!manager->inflight_active) try_start_next(manager, now_ms);
}

UtmFrameResult utm_on_frame(UartTransactionManager *manager,
                            const UtmFrame *frame,
                            uint32_t now_ms)
{
    UtmRequest *request;
    uint8_t inverse_route;
    if (!manager || !frame) return UTM_FRAME_UNMATCHED;
    if (frame->command_id == UTM_EVENT_MPU_SAMPLE || frame->command_id == UTM_EVENT_IR_SAMPLE) {
        manager->metrics.ignored_stream_events++;
        return UTM_FRAME_IGNORED_EVENT;
    }
    if (!manager->inflight_active) {
        manager->metrics.unmatched_frames++;
        return UTM_FRAME_UNMATCHED;
    }

    request = &manager->inflight;
    inverse_route = (uint8_t)(frame->src == request->dst_node && frame->dst == request->node);
    if (!inverse_route) {
        manager->metrics.unmatched_frames++;
        return UTM_FRAME_UNMATCHED;
    }

    if (frame->command_id == UTM_CMD_NACK) {
        if (!frame->payload || frame->payload_len < 1u || frame->payload[0] != request->command_id) {
            manager->metrics.unmatched_frames++;
            return UTM_FRAME_UNMATCHED;
        }
        complete_inflight(manager, UTM_COMPLETED_F4_NACK, frame, now_ms);
        return UTM_FRAME_COMPLETED;
    }

    if (frame->command_id == UTM_CMD_ACK) {
        if (!frame->payload || frame->payload_len < 1u || frame->payload[0] != request->command_id) {
            manager->metrics.unmatched_frames++;
            return UTM_FRAME_UNMATCHED;
        }
        manager->ack_seen = 1u;
        if (request->response_policy == UTM_RESPONSE_ACK ||
            request->response_policy == UTM_RESPONSE_ACK_OR_COMMAND ||
            (request->response_policy == UTM_RESPONSE_ACK_AND_COMMAND && manager->response_seen)) {
            complete_inflight(manager, UTM_COMPLETED_ACK, frame, now_ms);
            return UTM_FRAME_COMPLETED;
        }
        return UTM_FRAME_MATCHED_PENDING;
    }

    if (frame->command_id == request->command_id) {
        manager->response_seen = 1u;
        if (request->response_policy == UTM_RESPONSE_COMMAND ||
            request->response_policy == UTM_RESPONSE_ACK_OR_COMMAND ||
            (request->response_policy == UTM_RESPONSE_ACK_AND_COMMAND && manager->ack_seen)) {
            complete_inflight(manager, UTM_COMPLETED_RESPONSE, frame, now_ms);
            return UTM_FRAME_COMPLETED;
        }
        return UTM_FRAME_MATCHED_PENDING;
    }

    manager->metrics.unmatched_frames++;
    return UTM_FRAME_UNMATCHED;
}

void utm_disconnect_client(UartTransactionManager *manager,
                           uint32_t client_id,
                           uint32_t generation,
                           uint32_t now_ms)
{
    uint8_t i = 0u;
    if (!manager) return;

    while (i < manager->queue_count) {
        if (manager->queue[i].client_id == client_id &&
            manager->queue[i].generation == generation) {
            UtmRequest canceled = manager->queue[i];
            remove_queue_at(manager, i);
            manager->metrics.canceled_queued++;
            manager->metrics.completed++;
            notify_completion(manager, &canceled, UTM_COMPLETED_CANCELED, NULL);
        } else {
            i++;
        }
    }

    if (manager->inflight_active &&
        manager->inflight.client_id == client_id &&
        manager->inflight.generation == generation) {
        manager->inflight_orphaned = 1u;
        manager->inflight.retry_policy = UTM_RETRY_NONE;
        manager->metrics.orphaned_inflight++;
    }

    if (!manager->inflight_active) try_start_next(manager, now_ms);
}

void utm_cancel_all(UartTransactionManager *manager)
{
    uint8_t canceled_count;
    if (!manager) return;

    canceled_count = (uint8_t)(manager->queue_count +
                               (manager->inflight_active ? 1u : 0u));
    manager->metrics.canceled_queued += manager->queue_count;
    if (manager->inflight_active) manager->metrics.orphaned_inflight++;
    manager->metrics.completed += canceled_count;

    memset(manager->queue, 0, sizeof(manager->queue));
    manager->queue_count = 0u;
    memset(&manager->inflight, 0, sizeof(manager->inflight));
    manager->inflight_active = 0u;
    manager->inflight_orphaned = 0u;
    manager->ack_seen = 0u;
    manager->response_seen = 0u;
    manager->attempt_deadline_ms = 0u;

}

uint8_t utm_queued_count(const UartTransactionManager *manager)
{
    return manager ? manager->queue_count : 0u;
}

const UtmRequest *utm_inflight(const UartTransactionManager *manager)
{
    return (manager && manager->inflight_active) ? &manager->inflight : NULL;
}

const UtmMetrics *utm_metrics(const UartTransactionManager *manager)
{
    return manager ? &manager->metrics : NULL;
}
