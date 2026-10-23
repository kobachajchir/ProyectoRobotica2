#include "uner_core.h"
#include <string.h>

static void acc(UNER_Core *core, uint8_t b) { core->chk_acc ^= b; }

static void resync_from_byte(UNER_Core *core, uint8_t byte) {
    UNER_Core_ResetParser(core);
    if (byte == UNER_HEADER_0) {
        acc(core, byte);
        core->state = UNER_S_H1;
    }
}

static uint8_t should_accept(const UNER_Core *core, uint8_t src, uint8_t dst) {
    uint8_t src_ok = (uint8_t)((core->cfg.accept_src_mask & (1u << src)) != 0u);
    uint8_t dst_ok = (uint8_t)((core->cfg.accept_dst_mask & (1u << dst)) != 0u);
    if (!src_ok) return 0;
    if (dst_ok) return 1;
    if (core->cfg.accept_broadcast && dst == UNER_BROADCAST_NODE) return 1;
    return 0;
}

UNER_Status UNER_Core_Init(UNER_Core *core, const UNER_CoreConfig *cfg, UNER_Packet *slots, uint8_t slot_count, uint8_t *payload_pool_bytes, uint16_t payload_pool_size) {
    if (!core || !cfg) return UNER_ERR_LEN;
    memset(core, 0, sizeof(*core));
    core->cfg = *cfg;
    UNER_Status s = UNER_Queue_Init(&core->queue, slots, slot_count, payload_pool_bytes, payload_pool_size);
    UNER_Core_ResetParser(core);
    return s;
}

void UNER_Core_ResetParser(UNER_Core *core) {
    core->state = UNER_S_H0;
    core->len_expected = 0;
    core->payload_index = 0;
    core->chk_acc = 0;
    core->route = 0;
    core->cmd = 0;
    core->ver = 0;
    core->transport_id = 0;
    core->drop_frame = 0;
}

UNER_Status UNER_Core_PushByte(UNER_Core *core, uint8_t transport_id, uint8_t max_payload_for_transport, uint8_t byte) {
    if (!core) return UNER_ERR_LEN;
    core->transport_id = transport_id;
    core->max_payload_transport = max_payload_for_transport;

    switch (core->state) {
        case UNER_S_H0:
            if (byte == UNER_HEADER_0) { core->chk_acc = 0; acc(core, byte); core->state = UNER_S_H1; }
            else core->header_resync++;
            break;
        case UNER_S_H1:
            if (byte == UNER_HEADER_1) { acc(core, byte); core->state = UNER_S_H2; }
            else { core->header_resync++; resync_from_byte(core, byte); }
            break;
        case UNER_S_H2:
            if (byte == UNER_HEADER_2) { acc(core, byte); core->state = UNER_S_H3; }
            else { core->header_resync++; resync_from_byte(core, byte); }
            break;
        case UNER_S_H3:
            if (byte == UNER_HEADER_3) { acc(core, byte); core->state = UNER_S_LEN; }
            else { core->header_resync++; resync_from_byte(core, byte); }
            break;
        case UNER_S_LEN:
            core->len_expected = byte;
            core->payload_index = 0;
            core->drop_frame = (byte > core->cfg.default_max_payload || byte > core->max_payload_transport) ? 1u : 0u;
            if (core->drop_frame) core->len_fail++;
            acc(core, byte);
            core->state = UNER_S_TOKEN;
            break;
        case UNER_S_TOKEN:
            if (byte != UNER_TOKEN) { core->token_fail++; resync_from_byte(core, byte); return UNER_ERR_TOKEN; }
            acc(core, byte);
            core->state = UNER_S_VER;
            break;
        case UNER_S_VER:
            if (byte != UNER_VER_V2) { core->ver_fail++; resync_from_byte(core, byte); return UNER_ERR_VER; }
            core->ver = byte;
            acc(core, byte);
            core->state = UNER_S_ROUTE;
            break;
        case UNER_S_ROUTE:
            core->route = byte;
            acc(core, byte);
            core->state = UNER_S_CMD;
            break;
        case UNER_S_CMD:
            core->cmd = byte;
            acc(core, byte);
            core->state = (core->len_expected == 0u) ? UNER_S_CHK : UNER_S_PAYLOAD;
            break;
        case UNER_S_PAYLOAD:
            if (!core->drop_frame && core->payload_index < UNER_MAX_PAYLOAD) {
                core->payload_buf[core->payload_index] = byte;
            }
            core->payload_index++;
            acc(core, byte);
            if (core->payload_index >= core->len_expected) core->state = UNER_S_CHK;
            break;
        case UNER_S_CHK: {
            uint8_t src = UNER_ROUTE_SRC(core->route);
            uint8_t dst = UNER_ROUTE_DST(core->route);
            if (byte != core->chk_acc) { core->chk_fail++; resync_from_byte(core, byte); return UNER_ERR_CHK; }
            if (core->drop_frame) { UNER_Core_ResetParser(core); return UNER_ERR_LEN; }
            if (!should_accept(core, src, dst)) { core->filter_drop++; UNER_Core_ResetParser(core); return UNER_ERR_FILTER; }
            UNER_Status q = UNER_Queue_Enqueue(&core->queue, src, dst, core->cmd, core->payload_buf, core->len_expected, core->ver, core->transport_id);
            if (q != UNER_OK) { core->queue_overflow++; UNER_Core_ResetParser(core); return q; }
            core->ok_frames++;
            if (core->cfg.on_packet) {
                UNER_Packet temp = { .ver = core->ver, .src = src, .dst = dst, .cmd = core->cmd, .len = core->len_expected, .payload = core->payload_buf, .transport_id = core->transport_id };
                core->cfg.on_packet(core->cfg.cb_ctx, &temp);
            }
            UNER_Core_ResetParser(core);
        } break;
    }
    return UNER_OK;
}

uint8_t UNER_Core_Dequeue(UNER_Core *core, UNER_Packet *out) { return UNER_Queue_Dequeue(&core->queue, out); }

uint8_t UNER_Core_ComputeChecksum(const uint8_t *data, uint16_t len) {
    uint8_t x = 0;
    for (uint16_t i = 0; i < len; ++i) x ^= data[i];
    return x;
}

UNER_Status UNER_BuildFrame(uint8_t *out, uint16_t out_max, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len) {
    uint16_t need = (uint16_t)(UNER_OVERHEAD + len);
    if (!out || need > out_max) return UNER_ERR_LEN;
    uint16_t i = 0;
    out[i++] = UNER_HEADER_0;
    out[i++] = UNER_HEADER_1;
    out[i++] = UNER_HEADER_2;
    out[i++] = UNER_HEADER_3;
    out[i++] = len;
    out[i++] = UNER_TOKEN;
    out[i++] = UNER_VER_V2;
    out[i++] = UNER_ROUTE_MAKE(src, dst);
    out[i++] = cmd;
    for (uint8_t k = 0; k < len; ++k) out[i++] = payload ? payload[k] : 0;
    out[i] = UNER_Core_ComputeChecksum(out, i);
    return UNER_OK;
}
