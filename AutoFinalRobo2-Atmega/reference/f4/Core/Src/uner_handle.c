/*
 * uner_handle.c - UNER v2 manager
 */

#include "uner_handle.h"
#include <string.h>

static void UNER_Handle_OnPacketQueued(void *ctx, const UNER_Packet *p)
{
    UNER_Handle *handle = (UNER_Handle *)ctx;
    (void)p;
    if (!handle) {
        return;
    }
    handle->packet_pending = 1u;
    (void)0;
}


static uint8_t UNER_Handle_IsRawAsyncPush(const UNER_Packet *packet)
{
    if (!packet || packet->len == 0u || !packet->payload) {
        return 0u;
    }

    if ((packet->cmd == 0x15u || packet->cmd == 0x48u || packet->cmd == 0x4Bu) && packet->payload[0] == 0xFEu) {
        return 1u;
    }

    return 0u;
}

static uint8_t UNER_Handle_IsLengthValid(const UNER_CommandSpec *spec, const UNER_Packet *packet)
{
    if (!spec || !packet) {
        return 0u;
    }

    if (packet->len >= spec->min_args && packet->len <= spec->max_args) {
        return 1u;
    }

    if ((spec->flags & UNER_SPEC_F_RESP) && packet->len >= 1u) {
        return 1u;
    }

    if (UNER_Handle_IsRawAsyncPush(packet)) {
        return 1u;
    }

    return 0u;
}

static uint8_t UNER_Handle_FindCommandSpec(
    const UNER_CommandSpec *table,
    uint8_t count,
    uint8_t cmd,
    UNER_CommandSpec *out)
{
    if (!table || !out) {
        return 0u;
    }

    for (uint8_t i = 0u; i < count; ++i) {
        if (table[i].id == cmd) {
            *out = table[i];
            return 1u;
        }
    }
    return 0u;
}

static UNER_Transport *UNER_Handle_FindTransport(UNER_Handle *handle, uint8_t transport_id)
{
    if (handle == NULL) {
        return NULL;
    }

    for (uint8_t i = 0u; i < handle->transport_count; ++i) {
        if (handle->transports[i] != NULL &&
            handle->transports[i]->id == transport_id) {
            return handle->transports[i];
        }
    }

    return NULL;
}

static uint8_t UNER_Handle_FindRoute(UNER_Handle *handle, uint8_t node, uint8_t *transport_id)
{
    if (handle == NULL || transport_id == NULL) {
        return 0u;
    }

    for (uint8_t i = 0u; i < handle->route_count; ++i) {
        if (handle->routes[i].enabled && handle->routes[i].node == node) {
            *transport_id = handle->routes[i].transport_id;
            return 1u;
        }
    }

    return 0u;
}

static uint32_t UNER_Handle_NextPacketSequence(UNER_Handle *handle)
{
    if (handle == NULL) {
        return 0u;
    }

    handle->packet_sequence++;
    if (handle->packet_sequence == 0u) {
        handle->packet_sequence = 1u;
    }

    return handle->packet_sequence;
}

static void UNER_Handle_FillPacketSnapshot(UNER_Handle *handle,
                                           UNER_RouterSnapshot *snapshot,
                                           const UNER_Packet *packet,
                                           uint8_t direction,
                                           uint8_t out_transport,
                                           uint8_t status,
                                           uint32_t sequence)
{
    uint8_t preview_len;

    if (handle == NULL || snapshot == NULL || packet == NULL) {
        return;
    }

    memset(snapshot, 0, sizeof(*snapshot));
    snapshot->valid = 1u;
    snapshot->forwarding_enabled = handle->forwarding_enabled ? 1u : 0u;
    snapshot->direction = direction;
    snapshot->src = packet->src;
    snapshot->dst = packet->dst;
    snapshot->cmd = packet->cmd;
    snapshot->len = packet->len;
    snapshot->in_transport = packet->transport_id;
    snapshot->out_transport = out_transport;
    snapshot->status = status;
    snapshot->sequence = sequence;
    snapshot->forwarded_frames = handle->forwarded_frames;
    snapshot->route_miss = handle->route_miss;
    snapshot->route_tx_busy = handle->route_tx_busy;

    preview_len = packet->len;
    if (preview_len > UNER_ROUTER_PAYLOAD_PREVIEW_BYTES) {
        preview_len = UNER_ROUTER_PAYLOAD_PREVIEW_BYTES;
    }
    if (preview_len > 0u && packet->payload != NULL) {
        memcpy(snapshot->payload_preview, packet->payload, preview_len);
        snapshot->payload_preview_len = preview_len;
    }
}

static void UNER_Handle_RecordPacketSnapshot(UNER_Handle *handle,
                                             const UNER_Packet *packet,
                                             uint8_t direction,
                                             uint8_t out_transport,
                                             uint8_t status)
{
    UNER_RouterSnapshot *snapshot;
    UNER_RouterSnapshot *transport_snapshot = NULL;
    uint8_t transport_id;
    uint32_t sequence;

    if (handle == NULL || packet == NULL) {
        return;
    }

    snapshot = (direction == UNER_ROUTER_PACKET_TX)
        ? &handle->tx_snapshot
        : &handle->rx_snapshot;
    transport_id = (direction == UNER_ROUTER_PACKET_TX)
        ? out_transport
        : packet->transport_id;
    if (transport_id < UNER_MAX_TRANSPORTS) {
        transport_snapshot = (direction == UNER_ROUTER_PACKET_TX)
            ? &handle->tx_transport_snapshots[transport_id]
            : &handle->rx_transport_snapshots[transport_id];
    }

    sequence = UNER_Handle_NextPacketSequence(handle);
    UNER_Handle_FillPacketSnapshot(handle,
                                   snapshot,
                                   packet,
                                   direction,
                                   out_transport,
                                   status,
                                   sequence);
    if (transport_snapshot != NULL) {
        UNER_Handle_FillPacketSnapshot(handle,
                                       transport_snapshot,
                                       packet,
                                       direction,
                                       out_transport,
                                       status,
                                       sequence);
    }
}

static void UNER_Handle_RecordRouterPacket(UNER_Handle *handle,
                                           const UNER_Packet *packet,
                                           uint8_t out_transport,
                                           uint8_t status)
{
    if (handle == NULL || packet == NULL) {
        return;
    }

    UNER_Handle_FillPacketSnapshot(handle,
                                   &handle->router_snapshot,
                                   packet,
                                   UNER_ROUTER_PACKET_RX,
                                   out_transport,
                                   status,
                                   handle->packet_sequence);
}

static UNER_Status UNER_Handle_ForwardToTransport(UNER_Handle *handle,
                                                  const UNER_Packet *packet,
                                                  uint8_t transport_id)
{
    UNER_Transport *transport;
    UNER_Status status;
    uint16_t total;

    if (handle == NULL || packet == NULL) {
        return UNER_ERR_LEN;
    }

    if (transport_id == packet->transport_id) {
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       transport_id,
                                       UNER_ROUTER_STATUS_SAME_TRANSPORT);
        return UNER_OK;
    }

    if (handle->forward_guard != NULL &&
        !handle->forward_guard(handle->forward_guard_ctx, packet, transport_id)) {
        UNER_Handle_RecordPacketSnapshot(handle,
                                         packet,
                                         UNER_ROUTER_PACKET_TX,
                                         transport_id,
                                         UNER_ROUTER_STATUS_BLOCKED);
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       transport_id,
                                       UNER_ROUTER_STATUS_BLOCKED);
        return UNER_ERR_LEN;
    }

    transport = UNER_Handle_FindTransport(handle, transport_id);
    if (transport == NULL) {
        handle->route_miss++;
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       transport_id,
                                       UNER_ROUTER_STATUS_ROUTE_MISS);
        return UNER_ERR_LEN;
    }

    if (packet->len > transport->max_payload) {
        handle->route_miss++;
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       transport_id,
                                       UNER_ROUTER_STATUS_INVALID);
        return UNER_ERR_LEN;
    }

    status = UNER_BuildFrame(handle->tx_buf,
                             sizeof(handle->tx_buf),
                             packet->src,
                             packet->dst,
                             packet->cmd,
                             packet->payload,
                             packet->len);
    if (status != UNER_OK) {
        UNER_Handle_RecordPacketSnapshot(handle,
                                         packet,
                                         UNER_ROUTER_PACKET_TX,
                                         transport_id,
                                         UNER_ROUTER_STATUS_INVALID);
        return status;
    }

    total = (uint16_t)(UNER_OVERHEAD + packet->len);
    status = transport->try_send(transport, handle->tx_buf, total);
    if (status == UNER_OK) {
        handle->forwarded_frames++;
        UNER_Handle_RecordPacketSnapshot(handle,
                                         packet,
                                         UNER_ROUTER_PACKET_TX,
                                         transport_id,
                                         UNER_ROUTER_STATUS_FORWARDED);
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       transport_id,
                                       UNER_ROUTER_STATUS_FORWARDED);
    } else if (status == UNER_ERR_BUSY) {
        handle->route_tx_busy++;
        UNER_Handle_RecordPacketSnapshot(handle,
                                         packet,
                                         UNER_ROUTER_PACKET_TX,
                                         transport_id,
                                         UNER_ROUTER_STATUS_TX_BUSY);
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       transport_id,
                                       UNER_ROUTER_STATUS_TX_BUSY);
    } else {
        UNER_Handle_RecordPacketSnapshot(handle,
                                         packet,
                                         UNER_ROUTER_PACKET_TX,
                                         transport_id,
                                         UNER_ROUTER_STATUS_INVALID);
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       transport_id,
                                       UNER_ROUTER_STATUS_INVALID);
    }

    return status;
}

static void UNER_Handle_ForwardPacket(UNER_Handle *handle, const UNER_Packet *packet)
{
    uint8_t transport_id;

    if (handle == NULL || packet == NULL) {
        return;
    }

    if (!handle->forwarding_enabled) {
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       packet->transport_id,
                                       UNER_ROUTER_STATUS_DISABLED);
        return;
    }

    if (packet->dst == UNER_NODE_BROADCAST) {
        for (uint8_t i = 0u; i < handle->transport_count; ++i) {
            if (handle->transports[i] != NULL) {
                (void)UNER_Handle_ForwardToTransport(handle,
                                                     packet,
                                                     handle->transports[i]->id);
            }
        }
        return;
    }

    if (!UNER_Handle_FindRoute(handle, packet->dst, &transport_id)) {
        handle->route_miss++;
        UNER_Handle_RecordRouterPacket(handle,
                                       packet,
                                       packet->transport_id,
                                       UNER_ROUTER_STATUS_ROUTE_MISS);
        return;
    }

    (void)UNER_Handle_ForwardToTransport(handle, packet, transport_id);
}

UNER_Status UNER_Handle_Init(
    UNER_Handle *handle,
    const UNER_CoreConfig *cfg,
    UNER_Packet *slots,
    uint8_t slot_count,
    uint8_t *payload_pool_bytes,
    uint16_t payload_pool_size)
{
    if (!handle || !cfg) {
        return UNER_ERR_LEN;
    }

    handle->transport_count = 0;
    memset(handle->transports, 0, sizeof(handle->transports));
    handle->route_count = 0u;
    memset(handle->routes, 0, sizeof(handle->routes));
    handle->forwarding_enabled = 1u;
    handle->forward_guard = NULL;
    handle->forward_guard_ctx = NULL;
    memset(&handle->router_snapshot, 0, sizeof(handle->router_snapshot));
    memset(&handle->rx_snapshot, 0, sizeof(handle->rx_snapshot));
    memset(&handle->tx_snapshot, 0, sizeof(handle->tx_snapshot));
    memset(handle->rx_transport_snapshots, 0, sizeof(handle->rx_transport_snapshots));
    memset(handle->tx_transport_snapshots, 0, sizeof(handle->tx_transport_snapshots));

    handle->packet_pending = 0u;
    handle->command_table = NULL;
    handle->command_count = 0u;
    handle->execute_command = NULL;
    handle->execute_ctx = NULL;
    handle->app_on_packet = cfg->on_packet;
    handle->app_on_packet_ctx = cfg->cb_ctx;
    handle->forwarded_frames = 0u;
    handle->route_miss = 0u;
    handle->route_tx_busy = 0u;
    handle->packet_sequence = 0u;

    UNER_CoreConfig cfg_local = *cfg;
    cfg_local.on_packet = UNER_Handle_OnPacketQueued;
    cfg_local.cb_ctx = handle;

    return UNER_Core_Init(
        &handle->core,
        &cfg_local,
        slots,
        slot_count,
        payload_pool_bytes,
        payload_pool_size);
}

UNER_Status UNER_Handle_RegisterTransport(UNER_Handle *handle, UNER_Transport *transport)
{
    if (!handle || !transport) {
        return UNER_ERR_LEN;
    }

    if (handle->transport_count >= UNER_MAX_TRANSPORTS) {
        return UNER_ERR_LEN;
    }

    handle->transports[handle->transport_count++] = transport;
    return UNER_OK;
}

UNER_Status UNER_Handle_RegisterRoute(UNER_Handle *handle, uint8_t node, uint8_t transport_id)
{
    if (handle == NULL || node == UNER_NODE_MCU) {
        return UNER_ERR_LEN;
    }

    if (UNER_Handle_FindTransport(handle, transport_id) == NULL) {
        return UNER_ERR_LEN;
    }

    for (uint8_t i = 0u; i < handle->route_count; ++i) {
        if (handle->routes[i].node == node) {
            handle->routes[i].transport_id = transport_id;
            handle->routes[i].enabled = 1u;
            return UNER_OK;
        }
    }

    if (handle->route_count >= UNER_MAX_ROUTES) {
        return UNER_ERR_LEN;
    }

    handle->routes[handle->route_count].node = node;
    handle->routes[handle->route_count].transport_id = transport_id;
    handle->routes[handle->route_count].enabled = 1u;
    handle->route_count++;
    return UNER_OK;
}

void UNER_Handle_SetForwardingEnabled(UNER_Handle *handle, uint8_t enabled)
{
    if (handle != NULL) {
        handle->forwarding_enabled = enabled ? 1u : 0u;
        handle->router_snapshot.forwarding_enabled = handle->forwarding_enabled;
        handle->rx_snapshot.forwarding_enabled = handle->forwarding_enabled;
        handle->tx_snapshot.forwarding_enabled = handle->forwarding_enabled;
        for (uint8_t i = 0u; i < UNER_MAX_TRANSPORTS; ++i) {
            handle->rx_transport_snapshots[i].forwarding_enabled = handle->forwarding_enabled;
            handle->tx_transport_snapshots[i].forwarding_enabled = handle->forwarding_enabled;
        }
    }
}

void UNER_Handle_SetForwardGuard(UNER_Handle *handle,
                                UNER_ForwardGuardFn guard,
                                void *ctx)
{
    if (handle != NULL) {
        handle->forward_guard = guard;
        handle->forward_guard_ctx = ctx;
    }
}

uint8_t UNER_Handle_IsForwardingEnabled(const UNER_Handle *handle)
{
    return (handle != NULL && handle->forwarding_enabled) ? 1u : 0u;
}

void UNER_Handle_GetRouterSnapshot(const UNER_Handle *handle, UNER_RouterSnapshot *out)
{
    if (out == NULL) {
        return;
    }

    memset(out, 0, sizeof(*out));
    if (handle != NULL) {
        *out = handle->router_snapshot;
        out->forwarding_enabled = handle->forwarding_enabled ? 1u : 0u;
        out->forwarded_frames = handle->forwarded_frames;
        out->route_miss = handle->route_miss;
        out->route_tx_busy = handle->route_tx_busy;
    }
}

void UNER_Handle_GetRouterPacketSnapshot(const UNER_Handle *handle,
                                         uint8_t tx_view,
                                         uint8_t transport_id,
                                         UNER_RouterSnapshot *out)
{
    if (out == NULL) {
        return;
    }

    memset(out, 0, sizeof(*out));
    if (handle != NULL) {
        if (transport_id < UNER_MAX_TRANSPORTS) {
            *out = tx_view
                ? handle->tx_transport_snapshots[transport_id]
                : handle->rx_transport_snapshots[transport_id];
        } else {
            *out = tx_view ? handle->tx_snapshot : handle->rx_snapshot;
        }
        out->forwarding_enabled = handle->forwarding_enabled ? 1u : 0u;
        out->forwarded_frames = handle->forwarded_frames;
        out->route_miss = handle->route_miss;
        out->route_tx_busy = handle->route_tx_busy;
    }
}

UNER_Status UNER_Handle_RegisterCommands(
    UNER_Handle *handle,
    const UNER_CommandSpec *table,
    uint8_t count,
    UNER_ExecuteCommandFn execute_command,
    void *execute_ctx)
{
    if (!handle || !table || count == 0u) {
        return UNER_ERR_LEN;
    }

    handle->command_table = table;
    handle->command_count = count;
    handle->execute_command = execute_command;
    handle->execute_ctx = execute_ctx;
    return UNER_OK;
}

void UNER_Handle_Poll(UNER_Handle *handle)
{
    if (!handle) {
        return;
    }

    for (uint8_t i = 0; i < handle->transport_count; ++i) {
        UNER_Transport *transport = handle->transports[i];
        if (transport && transport->poll_rx) {
            uint8_t busy_retries = 0u;

            while (transport->poll_rx(transport, &handle->core) == UNER_ERR_BUSY) {
                if (handle->core.q_count == 0u) {
                    break;
                }

                UNER_Handle_ProcessPending(handle);

                busy_retries++;
                if (busy_retries >= handle->core.slot_count) {
                    break;
                }
            }
        }
    }
}

void UNER_Handle_ProcessPending(UNER_Handle *handle)
{
    if (!handle) {
        return;
    }

    if (!handle->packet_pending && handle->core.q_count == 0u) {
        return;
    }

    UNER_Packet packet;
    (void)0;
    while (UNER_Core_Dequeue(&handle->core, &packet)) {
        uint8_t valid_cmd = 0u;
        uint8_t should_forward_after_local = 0u;
        UNER_CommandSpec spec = {0u, 0u, 0u, 0u, 0u, NULL};

        UNER_Handle_RecordPacketSnapshot(handle,
                                         &packet,
                                         UNER_ROUTER_PACKET_RX,
                                         packet.transport_id,
                                         UNER_ROUTER_STATUS_RECEIVED);

        if (packet.dst == UNER_NODE_BROADCAST) {
            should_forward_after_local = 1u;
        } else if (packet.dst != handle->core.cfg.this_node) {
            UNER_Handle_ForwardPacket(handle, &packet);
            UNER_Core_ReleasePacket(&handle->core, &packet);
            continue;
        }

        if (UNER_Handle_FindCommandSpec(handle->command_table, handle->command_count, packet.cmd, &spec)) {
            (void)0;
            if (UNER_Handle_IsLengthValid(&spec, &packet)) {
                valid_cmd = 1u;
            }
        }

        if (valid_cmd) {
            if (spec.handler) {
                spec.handler(handle->execute_ctx, &packet);
            } else if (handle->execute_command) {
            	(void)0;
                handle->execute_command(handle->execute_ctx, &packet);
            }

            if (handle->app_on_packet) {
                handle->app_on_packet(handle->app_on_packet_ctx, &packet);
            }
        }

        if (should_forward_after_local) {
            UNER_Handle_ForwardPacket(handle, &packet);
        }

        UNER_Core_ReleasePacket(&handle->core, &packet);
    }

    handle->packet_pending = 0u;
    (void)0;
}

UNER_Status UNER_Send(
    UNER_Handle *handle,
    uint8_t transport_id,
    uint8_t src,
    uint8_t dst,
    uint8_t cmd,
    const uint8_t *payload,
    uint8_t len)
{
    if (!handle) {
        return UNER_ERR_LEN;
    }

    UNER_Transport *transport = UNER_Handle_FindTransport(handle, transport_id);

    if (!transport) {
        return UNER_ERR_LEN;
    }

    if (len > transport->max_payload) {
        return UNER_ERR_LEN;
    }

    UNER_Status status = UNER_BuildFrame(handle->tx_buf, sizeof(handle->tx_buf), src, dst, cmd, payload, len);
    if (status != UNER_OK) {
        return status;
    }

    uint16_t total = (uint16_t)(UNER_OVERHEAD + len);
    status = transport->try_send(transport, handle->tx_buf, total);
    {
        UNER_Packet packet = {
            .ver = UNER_VERSION,
            .src = src,
            .dst = dst,
            .cmd = cmd,
            .len = len,
            .payload = (uint8_t *)payload,
            .transport_id = transport_id
        };
        UNER_Handle_RecordPacketSnapshot(handle,
                                         &packet,
                                         UNER_ROUTER_PACKET_TX,
                                         transport_id,
                                         (status == UNER_OK) ? UNER_ROUTER_STATUS_SENT :
                                         ((status == UNER_ERR_BUSY) ? UNER_ROUTER_STATUS_TX_BUSY :
                                          UNER_ROUTER_STATUS_INVALID));
    }
    return status;
}
