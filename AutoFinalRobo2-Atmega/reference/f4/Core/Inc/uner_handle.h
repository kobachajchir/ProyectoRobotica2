#ifndef INC_UNER_HANDLE_H_
#define INC_UNER_HANDLE_H_
#include "uner_handle_types.h"
#ifdef __cplusplus
extern "C" {
#endif
UNER_Status UNER_Handle_Init(
    UNER_Handle *handle,
    const UNER_CoreConfig *cfg,
    UNER_Packet *slots,
    uint8_t slot_count,
    uint8_t *payload_pool_bytes,
    uint16_t payload_pool_size);

UNER_Status UNER_Handle_RegisterTransport(UNER_Handle *handle, UNER_Transport *transport);
UNER_Status UNER_Handle_RegisterRoute(UNER_Handle *handle, uint8_t node, uint8_t transport_id);
void UNER_Handle_SetForwardingEnabled(UNER_Handle *handle, uint8_t enabled);
void UNER_Handle_SetForwardGuard(UNER_Handle *handle,
                                UNER_ForwardGuardFn guard,
                                void *ctx);
uint8_t UNER_Handle_IsForwardingEnabled(const UNER_Handle *handle);
void UNER_Handle_GetRouterSnapshot(const UNER_Handle *handle, UNER_RouterSnapshot *out);
void UNER_Handle_GetRouterPacketSnapshot(const UNER_Handle *handle,
                                         uint8_t tx_view,
                                         uint8_t transport_id,
                                         UNER_RouterSnapshot *out);
UNER_Status UNER_Handle_RegisterCommands(
    UNER_Handle *handle,
    const UNER_CommandSpec *table,
    uint8_t count,
    UNER_ExecuteCommandFn execute_command,
    void *execute_ctx);
void UNER_Handle_Poll(UNER_Handle *handle);
void UNER_Handle_ProcessPending(UNER_Handle *handle);

UNER_Status UNER_Send(
    UNER_Handle *handle,
    uint8_t transport_id,
    uint8_t src,
    uint8_t dst,
    uint8_t cmd,
    const uint8_t *payload,
    uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* INC_UNER_HANDLE_H_ */
