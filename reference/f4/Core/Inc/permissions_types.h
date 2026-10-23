/*
 * permissions.h
 *
 * Local permission and remote PIN session manager.
 */

#ifndef INC_PERMISSIONS_H_DATA
#define INC_PERMISSIONS_H_DATA

#include <stdbool.h>
#include <stdint.h>

#include "types/userEvent_type.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PERMISSION_PIN_DIGITS                  4u
#define PERMISSION_LOCAL_REQUEST_TIMEOUT_MS 60000u
#define PERMISSION_LOCAL_BLOCK_MS           60000u
#define PERMISSION_REMOTE_SESSION_TTL_MS    60000u
#define PERMISSION_REMOTE_BLOCK_MS          60000u
#define PERMISSION_REMOTE_MAX_ATTEMPTS          3u

typedef enum {
    PERMISSION_NONE = 0,
    PERMISSION_ESP_RESET,
    PERMISSION_FACTORY_RESET,
    PERMISSION_WIFI_CREDENTIALS,
    PERMISSION_MOTOR_TEST,
    PERMISSION_SYSTEM_CONFIG,
    PERMISSION_CAR_MODE_TEST,
    PERMISSION_COUNT
} PermissionId_t;

typedef enum {
    AUTH_IDLE = 0,
    AUTH_INPUT_PIN,
    AUTH_SUBMITTING,
    AUTH_GRANTED,
    AUTH_DENIED,
    AUTH_CANCELLED,
    AUTH_TIMEOUT,
    AUTH_EXPIRED,
    AUTH_LOCKED
} AuthState_t;

typedef enum {
    PERMISSION_AUTH_NOTICE_NONE = 0,
    PERMISSION_AUTH_NOTICE_PIN_CORRECT,
    PERMISSION_AUTH_NOTICE_PIN_INCORRECT,
    PERMISSION_AUTH_NOTICE_PIN_BYPASSED,
    PERMISSION_AUTH_NOTICE_PIN_MODIFIED
} PermissionAuthNoticeId_t;

typedef enum {
    PERMISSION_EVT_REQUIRED = 0,
    PERMISSION_EVT_PIN_ENTRY_STARTED,
    PERMISSION_EVT_PIN_SUBMITTED,
    PERMISSION_EVT_GRANTED,
    PERMISSION_EVT_DENIED,
    PERMISSION_EVT_TIMEOUT,
    PERMISSION_EVT_EXPIRED,
    PERMISSION_EVT_REVOKED,
    PERMISSION_EVT_CANCELLED,
    PERMISSION_EVT_LOCKED,
    PERMISSION_EVT_BUSY
} PermissionEventId_t;

typedef struct {
    PermissionId_t id;
    uint32_t ttl_ms;
    uint8_t max_attempts;
} PermissionPolicy_t;

typedef struct {
    PermissionEventId_t event;
    PermissionId_t permission;
    uint8_t request_id;
    uint8_t attempts_left;
} PermissionEvent_t;

typedef void (*PermissionContinuationFn)(void *ctx);
typedef void (*PermissionEventCallback)(const PermissionEvent_t *event);


#ifdef __cplusplus
}
#endif
#endif
