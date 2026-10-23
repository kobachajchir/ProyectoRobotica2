#pragma once
#include "uner_v2.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    UNER_Packet *pkt_slots;
    uint8_t *payload_pool;
    uint16_t payload_stride;
    uint8_t slot_count;
    volatile uint8_t q_head;
    volatile uint8_t q_tail;
    volatile uint8_t q_count;
} UNER_Queue;

UNER_Status UNER_Queue_Init(UNER_Queue *q, UNER_Packet *slots, uint8_t slot_count, uint8_t *payload_pool, uint16_t payload_pool_size);
UNER_Status UNER_Queue_Enqueue(UNER_Queue *q, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len, uint8_t ver, uint8_t transport_id);
uint8_t UNER_Queue_Dequeue(UNER_Queue *q, UNER_Packet *out);

#ifdef __cplusplus
}
#endif
