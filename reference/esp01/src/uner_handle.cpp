#include "uner_handle.h"
#include <string.h>

static uint8_t tx_enqueue(UNER_Handle *h, const uint8_t *frame, uint16_t len) {
    if (h->tx_q_count >= UNER_TX_SLOTS) return 0;
    memcpy(h->tx_q_data[h->tx_q_tail], frame, len);
    h->tx_q_len[h->tx_q_tail] = len;
    h->tx_q_tail = (uint8_t)((h->tx_q_tail + 1u) % UNER_TX_SLOTS);
    h->tx_q_count++;
    return 1;
}

UNER_Status UNER_Handle_InitWithOps(UNER_Handle *h, const UNER_CoreConfig *cfg, const UNER_CommandSpec *table, uint8_t table_count, void *app_ctx, const UNER_HandleOps *ops) {
    if (!h || !cfg || !table || table_count == 0) return UNER_ERR_LEN;
    memset(h, 0, sizeof(*h));
    h->command_table = table;
    h->command_count = table_count;
    h->app_ctx = app_ctx;
    h->last_ms = 0;
    if (ops) {
        h->now_ms = ops->now_ms;
        h->clock_ctx = ops->clock_ctx;
        h->log_packet = ops->log_packet;
        h->log_ctx = ops->log_ctx;
    }
    return UNER_Core_Init(&h->core, cfg, h->rx_slots, UNER_RX_SLOTS, h->rx_payload_pool, sizeof(h->rx_payload_pool));
}

UNER_Status UNER_Handle_AddTransport(UNER_Handle *h, UNER_Transport *t) {
    if (!h || !t || !t->poll_rx || !t->try_send) return UNER_ERR_LEN;
    if (h->transport_count >= UNER_MAX_TRANSPORTS) return UNER_ERR_BUSY;
    h->transports[h->transport_count++] = t;
    return UNER_OK;
}

void UNER_Handle_SetPacketInterceptor(UNER_Handle *h, UNER_PacketInterceptorFn fn, void *ctx) {
    if (!h) return;
    h->packet_interceptor = fn;
    h->packet_interceptor_ctx = ctx;
}

void UNER_Handle_SetClock(UNER_Handle *h, UNER_NowMsFn fn, void *ctx) {
    if (!h) return;
    h->now_ms = fn;
    h->clock_ctx = ctx;
}
void UNER_Handle_SetPacketLogger(UNER_Handle *h, UNER_PacketLogFn fn, void *ctx) {
    if (!h) return;
    h->log_packet = fn;
    h->log_ctx = ctx;
}

void UNER_Handle_PollRx(UNER_Handle *h) {
    if (!h) return;
    uint8_t buf[UNER_POLL_BUDGET];
    for (uint8_t t = 0; t < h->transport_count; ++t) {
        UNER_Transport *tr = h->transports[t];
        uint16_t got = tr->poll_rx(tr->ctx, buf, UNER_POLL_BUDGET);
        for (uint16_t i = 0; i < got; ++i) {
            UNER_Core_PushByte(&h->core, tr->id, tr->max_payload, buf[i]);
        }
    }
}

void UNER_Handle_Dispatch(UNER_Handle *h) {
    if (!h) return;
    UNER_Packet pkt;
    while (UNER_Core_Dequeue(&h->core, &pkt)) {
        if (h->log_packet) h->log_packet(h->log_ctx, &pkt);
//         Serial.printf("[UNER][DISPATCH] cmd=0x%02X len=%u src=%u dst=%u\n", pkt.cmd, pkt.len, pkt.src, pkt.dst);
        if (h->packet_interceptor && h->packet_interceptor(h->packet_interceptor_ctx, &pkt)) {
            continue;
        }

        const UNER_CommandSpec *spec = UNER_FindCommandSpec(h->command_table, h->command_count, pkt.cmd);
        if (!spec) {
            continue;
        }
        if (pkt.len < spec->min_args || pkt.len > spec->max_args) {
            if ((spec->flags & UNER_SPEC_F_ACK) != 0u &&
                (spec->flags & UNER_SPEC_F_MANUAL_ACK) != 0u &&
                pkt.cmd != UNER_CMD_ACK && pkt.cmd != UNER_CMD_NACK) {
                (void)UNER_Handle_SendNack(h, pkt.dst, pkt.src, pkt.cmd,
                                           (uint8_t)UNER_ERR_ARGS);
            }
            continue;
        }
        if (!spec->handler) {
            continue;
        }
        if ((spec->flags & UNER_SPEC_F_ACK) != 0u &&
            (spec->flags & UNER_SPEC_F_MANUAL_ACK) == 0u &&
            pkt.cmd != UNER_CMD_ACK && pkt.cmd != UNER_CMD_NACK) {
            UNER_Handle_SendAck(h, pkt.dst, pkt.src, pkt.cmd, 0);
        }
        spec->handler(h->app_ctx, &pkt);
    }
}

void UNER_Handle_TxPump(UNER_Handle *h) {
    if (!h || h->tx_q_count == 0 || h->transport_count == 0) return;
    UNER_Transport *t = h->transports[0];
    if (t->try_send(t->ctx, h->tx_q_data[h->tx_q_head], h->tx_q_len[h->tx_q_head]) > 0) {
        h->tx_q_head = (uint8_t)((h->tx_q_head + 1u) % UNER_TX_SLOTS);
        h->tx_q_count--;
    }
}

void UNER_Handle_Tick(UNER_Handle *h) {
    if (!h) return;
    uint32_t now = h->now_ms ? h->now_ms(h->clock_ctx) : 0u;
    if (h->last_ms == 0) h->last_ms = now;
    uint8_t guard = 0;
    while ((uint32_t)(now - h->last_ms) >= 1u && guard < 10u) {
        h->last_ms++;
        h->flag_1ms = 1;
        if (++h->tick10 >= 10) { h->tick10 = 0; h->flag_10ms = 1; }
        if (++h->tick50 >= 50) { h->tick50 = 0; h->flag_50ms = 1; }
        if (++h->tick100 >= 100) { h->tick100 = 0; h->flag_100ms = 1; }

        guard++;
    }
}

UNER_Status UNER_Handle_SendRaw(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len) {
    if (!h) return UNER_ERR_LEN;
    if (UNER_BuildFrame(h->tx_buf, sizeof(h->tx_buf), src, dst, cmd, payload, len) != UNER_OK) return UNER_ERR_LEN;
    uint16_t frame_len = (uint16_t)(UNER_OVERHEAD + len);
    if (!tx_enqueue(h, h->tx_buf, frame_len)) return UNER_ERR_BUSY;
    return UNER_OK;
}

UNER_Status UNER_Handle_SendAck(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t acked_cmd, uint8_t status) {
    uint8_t p[2] = {acked_cmd, status};
    return UNER_Handle_SendRaw(h, src, dst, UNER_CMD_ACK, p, 2);
}

UNER_Status UNER_Handle_SendNack(UNER_Handle *h, uint8_t src, uint8_t dst, uint8_t cmd, uint8_t reason) {
    uint8_t p[2] = {cmd, reason};
    return UNER_Handle_SendRaw(h, src, dst, UNER_CMD_NACK, p, 2);
}

UNER_Status UNER_Handle_SendResponse(UNER_Handle *h, const UNER_Packet *req, uint8_t status, const uint8_t *data, uint8_t data_len) {
    uint8_t payload[UNER_MAX_PAYLOAD];
    if (data_len >= UNER_MAX_PAYLOAD) return UNER_ERR_LEN;
    uint8_t len = (uint8_t)(1u + data_len);
    payload[0] = status;
    if (data_len > 0 && data) memcpy(&payload[1], data, data_len);
    return UNER_Handle_SendRaw(h, req->dst, req->src, req->cmd, payload, len);
}
