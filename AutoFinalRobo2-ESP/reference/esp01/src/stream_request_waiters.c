#include "stream_request_waiters.h"

#include <stddef.h>
#include <string.h>

static uint8_t stream_valid(SbStream stream)
{
    return (uint8_t)(stream >= SB_STREAM_MPU && stream < SB_STREAM_COUNT);
}

static uint8_t time_reached(uint32_t now_ms, uint32_t deadline_ms)
{
    return (uint8_t)(((int32_t)(now_ms - deadline_ms)) >= 0);
}

static void copy_text(char *destination, uint8_t capacity, const char *source)
{
    if (!destination || capacity == 0u) return;
    if (!source) source = "";
    strncpy(destination, source, (size_t)capacity - 1u);
    destination[capacity - 1u] = '\0';
}

static void record_completion(StreamRequestWaiters *manager,
                              SrwCompletionStatus status)
{
    if (status == SRW_COMPLETED_CONVERGED) manager->metrics.converged++;
    else if (status == SRW_COMPLETED_TIMEOUT) manager->metrics.timed_out++;
    else if (status == SRW_COMPLETED_CANCELED) manager->metrics.canceled++;
    else manager->metrics.failed++;
}

static uint8_t take_at(StreamRequestWaiters *manager,
                       uint8_t index,
                       SrwCompletionStatus status,
                       SrwCompletion *completion)
{
    if (!manager || !completion || index >= SRW_CAPACITY ||
        !manager->waiters[index].used) {
        return 0u;
    }
    completion->waiter = manager->waiters[index];
    completion->status = status;
    memset(&manager->waiters[index], 0, sizeof(manager->waiters[index]));
    record_completion(manager, status);
    return 1u;
}

void srw_init(StreamRequestWaiters *manager)
{
    if (!manager) return;
    memset(manager, 0, sizeof(*manager));
}

SrwAddResult srw_add(StreamRequestWaiters *manager,
                     uint32_t client_id,
                     uint32_t generation,
                     uint8_t api_v1,
                     SbStream stream,
                     uint8_t subscribed,
                     uint16_t period_ms,
                     const char *request_id,
                     const char *command,
                     uint32_t deadline_ms)
{
    uint8_t index;
    uint8_t free_index = SRW_CAPACITY;
    uint8_t count = 0u;
    if (!manager || generation == 0u || !stream_valid(stream) ||
        !request_id || !request_id[0] || !command || !command[0] ||
        deadline_ms == 0u) {
        return SRW_ADD_INVALID;
    }

    for (index = 0u; index < SRW_CAPACITY; ++index) {
        const SrwWaiter *waiter = &manager->waiters[index];
        if (!waiter->used) {
            if (free_index == SRW_CAPACITY) free_index = index;
            continue;
        }
        count++;
        if (waiter->client_id == client_id &&
            waiter->generation == generation &&
            waiter->stream == stream) {
            manager->metrics.rejected_duplicate++;
            return SRW_ADD_ALREADY_PENDING;
        }
    }
    if (free_index == SRW_CAPACITY) {
        manager->metrics.rejected_full++;
        return SRW_ADD_FULL;
    }

    {
        SrwWaiter *waiter = &manager->waiters[free_index];
        memset(waiter, 0, sizeof(*waiter));
        waiter->used = 1u;
        waiter->client_id = client_id;
        waiter->generation = generation;
        waiter->api_v1 = api_v1 ? 1u : 0u;
        waiter->stream = stream;
        waiter->subscribed = subscribed ? 1u : 0u;
        waiter->period_ms = waiter->subscribed ? sb_clamp_period(period_ms) : 0u;
        waiter->deadline_ms = deadline_ms;
        copy_text(waiter->request_id, SRW_REQUEST_ID_CAPACITY, request_id);
        copy_text(waiter->command, SRW_COMMAND_CAPACITY, command);
    }
    manager->metrics.added++;
    count++;
    if (count > manager->metrics.high_watermark) manager->metrics.high_watermark = count;
    return SRW_ADD_OK;
}

uint8_t srw_has_pending(const StreamRequestWaiters *manager,
                        uint32_t client_id,
                        uint32_t generation,
                        SbStream stream)
{
    uint8_t index;
    if (!manager || !stream_valid(stream)) return 0u;
    for (index = 0u; index < SRW_CAPACITY; ++index) {
        const SrwWaiter *waiter = &manager->waiters[index];
        if (waiter->used && waiter->client_id == client_id &&
            waiter->generation == generation && waiter->stream == stream) {
            return 1u;
        }
    }
    return 0u;
}

uint8_t srw_take_for_stream(StreamRequestWaiters *manager,
                            SbStream stream,
                            SrwCompletionStatus status,
                            SrwCompletion *completion)
{
    uint8_t index;
    if (!manager || !completion || !stream_valid(stream)) return 0u;
    for (index = 0u; index < SRW_CAPACITY; ++index) {
        if (manager->waiters[index].used &&
            manager->waiters[index].stream == stream) {
            return take_at(manager, index, status, completion);
        }
    }
    return 0u;
}

uint8_t srw_take_endpoint_stream(StreamRequestWaiters *manager,
                                 uint32_t client_id,
                                 uint32_t generation,
                                 SbStream stream,
                                 SrwCompletionStatus status,
                                 SrwCompletion *completion)
{
    uint8_t index;
    if (!manager || !completion || !stream_valid(stream)) return 0u;
    for (index = 0u; index < SRW_CAPACITY; ++index) {
        const SrwWaiter *waiter = &manager->waiters[index];
        if (waiter->used && waiter->client_id == client_id &&
            waiter->generation == generation && waiter->stream == stream) {
            return take_at(manager, index, status, completion);
        }
    }
    return 0u;
}

uint8_t srw_take_expired(StreamRequestWaiters *manager,
                         uint32_t now_ms,
                         SrwCompletion *completion)
{
    uint8_t index;
    if (!manager || !completion) return 0u;
    for (index = 0u; index < SRW_CAPACITY; ++index) {
        if (manager->waiters[index].used &&
            time_reached(now_ms, manager->waiters[index].deadline_ms)) {
            return take_at(manager, index, SRW_COMPLETED_TIMEOUT, completion);
        }
    }
    return 0u;
}

uint8_t srw_remove_client(StreamRequestWaiters *manager,
                          uint32_t client_id,
                          uint32_t generation)
{
    uint8_t index;
    uint8_t removed = 0u;
    if (!manager) return 0u;
    for (index = 0u; index < SRW_CAPACITY; ++index) {
        SrwWaiter *waiter = &manager->waiters[index];
        if (waiter->used && waiter->client_id == client_id &&
            waiter->generation == generation) {
            memset(waiter, 0, sizeof(*waiter));
            manager->metrics.canceled++;
            removed++;
        }
    }
    return removed;
}

uint8_t srw_count(const StreamRequestWaiters *manager)
{
    uint8_t index;
    uint8_t count = 0u;
    if (!manager) return 0u;
    for (index = 0u; index < SRW_CAPACITY; ++index) {
        if (manager->waiters[index].used) count++;
    }
    return count;
}

const SrwMetrics *srw_metrics(const StreamRequestWaiters *manager)
{
    return manager ? &manager->metrics : NULL;
}
