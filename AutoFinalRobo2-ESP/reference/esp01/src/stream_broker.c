#include "stream_broker.h"

#include <string.h>

static uint8_t stream_valid(SbStream stream)
{
    return (uint8_t)(stream >= SB_STREAM_MPU && stream < SB_STREAM_COUNT);
}

static uint8_t endpoint_equal(const SbEndpoint *a, const SbEndpoint *b)
{
    return (uint8_t)(a && b &&
                     a->client_id == b->client_id &&
                     a->generation == b->generation &&
                     a->node == b->node);
}

static SbSubscriber *find_subscriber(StreamBroker *broker,
                                     uint32_t client_id,
                                     uint32_t generation)
{
    uint8_t i;
    if (!broker) return NULL;
    for (i = 0u; i < SB_MAX_CLIENTS; ++i) {
        SbSubscriber *subscriber = &broker->subscribers[i];
        if (subscriber->used &&
            subscriber->endpoint.client_id == client_id &&
            subscriber->endpoint.generation == generation) {
            return subscriber;
        }
    }
    return NULL;
}

static uint8_t subscriber_is_empty(const SbSubscriber *subscriber)
{
    return (uint8_t)(subscriber &&
                     !subscriber->subscribed[SB_STREAM_MPU] &&
                     !subscriber->subscribed[SB_STREAM_IR]);
}

static uint8_t owner_is_still_subscribed(const StreamBroker *broker,
                                         SbStream stream,
                                         const SbEndpoint *owner)
{
    uint8_t i;
    for (i = 0u; i < SB_MAX_CLIENTS; ++i) {
        const SbSubscriber *subscriber = &broker->subscribers[i];
        if (subscriber->used && subscriber->subscribed[stream] &&
            endpoint_equal(&subscriber->endpoint, owner)) {
            return 1u;
        }
    }
    return 0u;
}

static void recompute(StreamBroker *broker, SbStream stream)
{
    uint8_t i;
    uint8_t active = 0u;
    uint16_t minimum = SB_PERIOD_MAX_MS;
    SbEndpoint first_owner;
    uint8_t first_owner_valid = 0u;
    SbStreamState *state = &broker->streams[stream];
    uint16_t old_period = state->desired_period_ms;
    uint8_t old_active = state->desired_active;

    memset(&first_owner, 0, sizeof(first_owner));
    for (i = 0u; i < SB_MAX_CLIENTS; ++i) {
        SbSubscriber *subscriber = &broker->subscribers[i];
        if (!subscriber->used || !subscriber->subscribed[stream]) continue;
        if (!first_owner_valid) {
            first_owner = subscriber->endpoint;
            first_owner_valid = 1u;
        }
        if (!active || subscriber->period_ms[stream] < minimum) {
            minimum = subscriber->period_ms[stream];
        }
        active = 1u;
    }

    state->desired_active = active;
    state->desired_period_ms = active ? minimum : 0u;
    if (!active) {
        state->desired_owner_valid = 0u;
        memset(&state->desired_owner, 0, sizeof(state->desired_owner));
    } else if (!state->desired_owner_valid ||
               !owner_is_still_subscribed(broker, stream, &state->desired_owner)) {
        state->desired_owner = first_owner;
        state->desired_owner_valid = first_owner_valid;
    }

    if (old_active != state->desired_active || old_period != state->desired_period_ms) {
        broker->metrics.desired_period_changes++;
    }
}

static SbSubscriber *ensure_subscriber(StreamBroker *broker,
                                       uint32_t client_id,
                                       uint32_t generation,
                                       uint8_t node)
{
    uint8_t i;
    SbSubscriber *subscriber = find_subscriber(broker, client_id, generation);
    if (subscriber) return subscriber;
    for (i = 0u; i < SB_MAX_CLIENTS; ++i) {
        if (!broker->subscribers[i].used) {
            subscriber = &broker->subscribers[i];
            memset(subscriber, 0, sizeof(*subscriber));
            subscriber->used = 1u;
            subscriber->endpoint.client_id = client_id;
            subscriber->endpoint.generation = generation;
            subscriber->endpoint.node = node;
            return subscriber;
        }
    }
    return NULL;
}

static void abort_pending_for_stream(StreamBroker *broker,
                                     SbStream stream,
                                     const SbEndpoint *endpoint)
{
    SbStreamState *state = &broker->streams[stream];
    if (state->pending && endpoint_equal(&state->pending_action.endpoint, endpoint)) {
        state->pending = 0u;
        memset(&state->pending_action, 0, sizeof(state->pending_action));
        broker->metrics.pending_actions_aborted++;
    }
}

static void abort_pending_for_endpoint(StreamBroker *broker, const SbEndpoint *endpoint)
{
    abort_pending_for_stream(broker, SB_STREAM_MPU, endpoint);
    abort_pending_for_stream(broker, SB_STREAM_IR, endpoint);
}

static uint8_t action_equal(const SbAction *a, const SbAction *b)
{
    return (uint8_t)(a && b &&
                     a->type == b->type &&
                     a->stream == b->stream &&
                     a->period_ms == b->period_ms &&
                     a->reconcile == b->reconcile &&
                     endpoint_equal(&a->endpoint, &b->endpoint));
}

static void allow_explicit_retry(StreamBroker *broker, SbStream stream)
{
    SbStreamState *state = &broker->streams[stream];
    if (state->action_blocked) {
        state->action_blocked = 0u;
        broker->metrics.explicit_retries++;
    }
}

void sb_init(StreamBroker *broker)
{
    if (!broker) return;
    memset(broker, 0, sizeof(*broker));
}

uint16_t sb_clamp_period(uint16_t period_ms)
{
    if (period_ms < SB_PERIOD_MIN_MS) return SB_PERIOD_MIN_MS;
    if (period_ms > SB_PERIOD_MAX_MS) return SB_PERIOD_MAX_MS;
    return period_ms;
}

SbResult sb_subscribe(StreamBroker *broker,
                      uint32_t client_id,
                      uint32_t generation,
                      uint8_t node,
                      SbStream stream,
                      uint16_t period_ms)
{
    SbSubscriber *subscriber;
    if (!broker || !stream_valid(stream) ||
        node < 0x05u || node > 0x0Cu || generation == 0u) {
        return SB_INVALID;
    }
    subscriber = ensure_subscriber(broker, client_id, generation, node);
    if (!subscriber) return SB_CAPACITY;
    if (subscriber->endpoint.node != node) return SB_INVALID;

    subscriber->subscribed[stream] = 1u;
    subscriber->period_ms[stream] = sb_clamp_period(period_ms);
    broker->metrics.subscriptions++;
    allow_explicit_retry(broker, stream);
    recompute(broker, stream);
    return SB_OK;
}

SbResult sb_unsubscribe(StreamBroker *broker,
                        uint32_t client_id,
                        uint32_t generation,
                        SbStream stream)
{
    SbSubscriber *subscriber;
    SbEndpoint endpoint;
    if (!broker || !stream_valid(stream)) return SB_INVALID;
    subscriber = find_subscriber(broker, client_id, generation);
    if (!subscriber || !subscriber->subscribed[stream]) {
        /* Repetir explicitamente un unsubscribe es la orden que habilita un
         * nuevo STOP despues de un fallo; nunca lo hace el loop por si solo. */
        allow_explicit_retry(broker, stream);
        return SB_NOT_FOUND;
    }
    endpoint = subscriber->endpoint;
    subscriber->subscribed[stream] = 0u;
    subscriber->period_ms[stream] = 0u;
    broker->metrics.unsubscriptions++;
    allow_explicit_retry(broker, stream);
    abort_pending_for_stream(broker, stream, &endpoint);
    if (subscriber_is_empty(subscriber)) memset(subscriber, 0, sizeof(*subscriber));
    recompute(broker, stream);
    return SB_OK;
}

uint8_t sb_remove_client(StreamBroker *broker,
                         uint32_t client_id,
                         uint32_t generation)
{
    SbSubscriber *subscriber;
    SbEndpoint endpoint;
    uint8_t changed_mpu;
    uint8_t changed_ir;
    if (!broker) return 0u;
    subscriber = find_subscriber(broker, client_id, generation);
    if (!subscriber) return 0u;
    endpoint = subscriber->endpoint;
    changed_mpu = subscriber->subscribed[SB_STREAM_MPU];
    changed_ir = subscriber->subscribed[SB_STREAM_IR];
    abort_pending_for_endpoint(broker, &endpoint);
    memset(subscriber, 0, sizeof(*subscriber));
    if (changed_mpu) {
        allow_explicit_retry(broker, SB_STREAM_MPU);
        recompute(broker, SB_STREAM_MPU);
    }
    if (changed_ir) {
        allow_explicit_retry(broker, SB_STREAM_IR);
        recompute(broker, SB_STREAM_IR);
    }
    broker->metrics.client_removals++;
    return 1u;
}

SbAction sb_take_action(StreamBroker *broker, SbStream stream)
{
    SbAction action;
    SbStreamState *state;
    memset(&action, 0, sizeof(action));
    action.type = SB_ACTION_NONE;
    action.stream = stream;
    if (!broker || !stream_valid(stream)) return action;
    state = &broker->streams[stream];
    if (state->pending || state->action_blocked) return action;

    if (state->needs_known_stop) {
        action.type = SB_ACTION_STOP;
        action.reconcile = 1u;
        if (state->desired_owner_valid) action.endpoint = state->desired_owner;
        else if (state->applied_owner_valid) action.endpoint = state->applied_owner;
        else action.endpoint.node = SB_CONTROL_NODE;
    } else if (state->applied_active &&
               (!state->desired_active ||
                !endpoint_equal(&state->desired_owner, &state->applied_owner))) {
        action.type = SB_ACTION_STOP;
        action.endpoint = state->applied_owner;
    } else if (state->desired_active &&
               (!state->applied_active ||
                state->desired_period_ms != state->applied_period_ms ||
                !state->applied_owner_valid ||
                !endpoint_equal(&state->desired_owner, &state->applied_owner))) {
        action.type = SB_ACTION_SET;
        action.endpoint = state->desired_owner;
        action.period_ms = state->desired_period_ms;
    }

    if (action.type != SB_ACTION_NONE) {
        state->pending = 1u;
        state->pending_action = action;
        if (action.type == SB_ACTION_SET) {
            broker->metrics.set_actions++;
            if (state->applied_active && state->applied_owner_valid &&
                !endpoint_equal(&state->applied_owner, &action.endpoint)) {
                broker->metrics.owner_transfers++;
            }
        } else {
            broker->metrics.stop_actions++;
            if (action.reconcile) broker->metrics.reconcile_cycles++;
        }
    }
    return action;
}

SbResult sb_complete_action(StreamBroker *broker,
                            const SbAction *action,
                            uint8_t success)
{
    SbStreamState *state;
    if (!broker || !action || !stream_valid(action->stream)) return SB_INVALID;
    state = &broker->streams[action->stream];
    if (!state->pending || !action_equal(&state->pending_action, action)) {
        broker->metrics.stale_action_results++;
        return SB_STALE_ACTION;
    }

    state->pending = 0u;
    memset(&state->pending_action, 0, sizeof(state->pending_action));
    if (!success) {
        broker->metrics.action_failures++;
        broker->metrics.action_failures_blocked++;
        state->action_blocked = 1u;
        return SB_OK;
    }

    if (action->type == SB_ACTION_SET) {
        state->applied_active = 1u;
        state->applied_period_ms = action->period_ms;
        state->applied_owner_valid = 1u;
        state->applied_owner = action->endpoint;
    } else if (action->type == SB_ACTION_STOP) {
        state->applied_active = 0u;
        state->applied_period_ms = 0u;
        state->applied_owner_valid = 0u;
        memset(&state->applied_owner, 0, sizeof(state->applied_owner));
        if (action->reconcile) state->needs_known_stop = 0u;
    }
    return SB_OK;
}

uint8_t sb_is_converged(const StreamBroker *broker, SbStream stream)
{
    const SbStreamState *state;
    if (!broker || !stream_valid(stream)) return 0u;
    state = &broker->streams[stream];
    if (state->pending || state->needs_known_stop || state->action_blocked) return 0u;
    if (state->desired_active != state->applied_active) return 0u;
    if (!state->desired_active) return 1u;
    return (uint8_t)(state->desired_period_ms == state->applied_period_ms &&
                     state->desired_owner_valid && state->applied_owner_valid &&
                     endpoint_equal(&state->desired_owner, &state->applied_owner));
}

void sb_backend_reconnected(StreamBroker *broker)
{
    uint8_t stream;
    if (!broker) return;
    for (stream = 0u; stream < SB_STREAM_COUNT; ++stream) {
        SbStreamState *state = &broker->streams[stream];
        state->needs_known_stop = 1u;
        state->pending = 0u;
        state->action_blocked = 0u;
        memset(&state->pending_action, 0, sizeof(state->pending_action));
    }
}

uint8_t sb_should_fanout(StreamBroker *broker,
                         uint32_t client_id,
                         uint32_t generation,
                         SbStream stream,
                         uint8_t authenticated)
{
    SbSubscriber *subscriber;
    uint8_t allowed;
    if (!broker || !stream_valid(stream)) return 0u;
    subscriber = find_subscriber(broker, client_id, generation);
    allowed = (uint8_t)(authenticated && subscriber && subscriber->subscribed[stream]);
    if (allowed) broker->metrics.fanout_allowed++;
    else broker->metrics.fanout_denied++;
    return allowed;
}

const SbStreamState *sb_stream_state(const StreamBroker *broker, SbStream stream)
{
    return (broker && stream_valid(stream)) ? &broker->streams[stream] : NULL;
}

const SbMetrics *sb_metrics(const StreamBroker *broker)
{
    return broker ? &broker->metrics : NULL;
}
