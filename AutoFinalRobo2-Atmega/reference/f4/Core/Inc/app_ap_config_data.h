#ifndef APP_AP_CONFIG_DATA_H
#define APP_AP_CONFIG_DATA_H
#include "am_port.h"
#define APP_AP_SSID_MAX_BYTES       32u
#define APP_AP_PASSWORD_MIN_BYTES    8u
#define APP_AP_PASSWORD_MAX_BYTES   63u

#define APP_AP_FIELD_SSID           (1u << 0)
#define APP_AP_FIELD_PASSWORD       (1u << 1)
#define APP_AP_FIELD_PROTECTED      (1u << 2)
#define APP_AP_FIELD_IPV4           (1u << 3)
#define APP_AP_FIELD_ALL            (APP_AP_FIELD_SSID | APP_AP_FIELD_PASSWORD | \
                                     APP_AP_FIELD_PROTECTED | APP_AP_FIELD_IPV4)

typedef enum {
    APP_AP_APPLY_SYNCED = 0u,
    APP_AP_APPLY_PENDING = 1u,
    APP_AP_APPLY_APPLYING = 2u,
    APP_AP_APPLY_FAILED = 3u,
} AppApApplyState_t;

typedef enum {
    APP_AP_STATUS_OK = 0u,
    APP_AP_STATUS_INVALID_PAYLOAD = 1u,
    APP_AP_STATUS_INVALID_SSID = 2u,
    APP_AP_STATUS_INVALID_PASSWORD = 3u,
    APP_AP_STATUS_INVALID_IPV4 = 4u,
    APP_AP_STATUS_STORAGE_ERROR = 5u,
} AppApStatus_t;

typedef struct {
    char ssid[APP_AP_SSID_MAX_BYTES + 1u];
    char password[APP_AP_PASSWORD_MAX_BYTES + 1u];
    bool protected;
    uint8_t ipv4[4];
    AppApApplyState_t applyState;
} AppApConfig_t;

typedef struct {
    uint8_t fieldMask;
    AppApConfig_t values;
} AppApConfigPatch_t;


#endif
