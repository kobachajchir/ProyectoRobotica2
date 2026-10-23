#include "uner_queue.h"
#include <string.h>

UNER_Status UNER_Queue_Init(UNER_Queue *q, UNER_Packet *slots, uint8_t slot_count, uint8_t *payload_pool, uint16_t payload_pool_size) {
    if (!q || !slots || !payload_pool || slot_count == 0) return UNER_ERR_LEN;
    uint16_t stride = (uint16_t)(payload_pool_size / slot_count);
    if (stride == 0 || stride > UNER_MAX_PAYLOAD) return UNER_ERR_LEN;
    q->pkt_slots = slots;
    q->payload_pool = payload_pool;
    q->payload_stride = stride;
    q->slot_count = slot_count;
    q->q_head = 0;
    q->q_tail = 0;
    q->q_count = 0;
    memset(slots, 0, sizeof(UNER_Packet) * slot_count);
    return UNER_OK;
}

UNER_Status UNER_Queue_Enqueue(UNER_Queue *q, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len, uint8_t ver, uint8_t transport_id) {
    if (!q || len > q->payload_stride) return UNER_ERR_LEN;
    if (q->q_count >= q->slot_count) return UNER_ERR_QUEUE_FULL;

    uint8_t idx = q->q_tail;
    UNER_Packet *slot = &q->pkt_slots[idx];
    uint8_t *dst_payload = &q->payload_pool[(uint16_t)idx * q->payload_stride];
    if (len > 0 && payload) memcpy(dst_payload, payload, len);

    slot->ver = ver;
    slot->src = src;
    slot->dst = dst;
    slot->cmd = cmd;
    slot->len = len;
    slot->payload = dst_payload;
    slot->transport_id = transport_id;

    q->q_tail = (uint8_t)((q->q_tail + 1u) % q->slot_count);
    q->q_count++;
    return UNER_OK;
}

uint8_t UNER_Queue_Dequeue(UNER_Queue *q, UNER_Packet *out) {
    if (!q || !out || q->q_count == 0) return 0;
    *out = q->pkt_slots[q->q_head];
    q->q_head = (uint8_t)((q->q_head + 1u) % q->slot_count);
    q->q_count--;
    return 1;
}
