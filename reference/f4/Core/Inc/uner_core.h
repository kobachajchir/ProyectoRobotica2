#ifndef INC_UNER_CORE_H_
#define INC_UNER_CORE_H_
#include "uner_core_types.h"
#ifdef __cplusplus
extern "C" {
#endif
UNER_Status UNER_Core_Init(
    UNER_Core *core,
    const UNER_CoreConfig *cfg,
    UNER_Packet *slots,
    uint8_t slot_count,
    uint8_t *payload_pool_bytes,
    uint16_t payload_pool_size);


UNER_Status UNER_Core_PushByte(
    UNER_Core *core,
    uint8_t transport_id,
    uint8_t max_payload_for_transport,
    uint8_t byte);

uint8_t UNER_Core_Dequeue(UNER_Core *core, UNER_Packet *out);
void UNER_Core_ReleasePacket(UNER_Core *core, const UNER_Packet *p);

uint8_t UNER_Core_ComputeChecksum(const uint8_t *data, uint16_t len);
UNER_Status UNER_BuildFrame(
    uint8_t *out,
    uint16_t out_max,
    uint8_t src,
    uint8_t dst,
    uint8_t cmd,
    const uint8_t *payload,
    uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* INC_UNER_CORE_H_ */
