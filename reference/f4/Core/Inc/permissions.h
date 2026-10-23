#ifndef INC_PERMISSIONS_H_
#define INC_PERMISSIONS_H_
#include "permissions_types.h"
#ifdef __cplusplus
extern "C" {
#endif
void Permission_Init(void);
void Permission_Task(uint32_t now_ms);

bool Permission_IsAuthorized(PermissionId_t permission, uint32_t now_ms);
bool Permission_IsAuthorizedNow(PermissionId_t permission);

bool Permission_Request(PermissionId_t permission,
                        PermissionContinuationFn on_granted,
                        void *ctx);

void Permission_Revoke(PermissionId_t permission);
void Permission_RevokeAll(void);
void Permission_CancelLocalRequest(void);

void Permission_HandleUserEvent(UserEvent_t ev);
void Permission_RenderPinEntry(void);
bool Permission_GrantCurrentRequest(uint32_t ttl_ms);
bool Permission_HasActiveLocalRequest(void);
PermissionId_t Permission_GetRequestedPermission(void);
AuthState_t Permission_GetState(void);
void Permission_SetEventCallback(PermissionEventCallback callback);
bool Permission_ShowRemoteAuthNotice(PermissionAuthNoticeId_t notice_id,
                                     uint16_t timeout_ms);

bool Permission_RemoteAuthenticate(
    uint8_t source_node,
    const uint8_t pin_digits[PERMISSION_PIN_DIGITS],
    uint32_t now_ms,
    uint32_t *ttl_ms,
    uint8_t *attempts_left,
    bool *new_session);
bool Permission_IsRemoteNodeAuthorized(uint8_t source_node, uint32_t now_ms);
bool Permission_IsRemoteNodeAuthorizedNow(uint8_t source_node);
void Permission_TouchRemoteNode(uint8_t source_node, uint32_t now_ms);
void Permission_RevokeRemoteNode(uint8_t source_node);
void Permission_RevokeAllRemote(void);
uint32_t Permission_GetRemoteTtlRemaining(uint8_t source_node, uint32_t now_ms);

#ifdef __cplusplus
}
#endif

#endif /* INC_PERMISSIONS_H_ */
