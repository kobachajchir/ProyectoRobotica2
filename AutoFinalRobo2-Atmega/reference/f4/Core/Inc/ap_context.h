#ifndef AP_CONTEXT_H
#define AP_CONTEXT_H
#include "app_ap_config_data.h"
#include "app_preferences_types.h"
#include "wifi_bridge_types.h"
#include "system.h"
#include "system_flags.h"
typedef struct {
    uint32_t (*now_ms)(void);
    const AppPreferences_t * (*AppPreferences_Get)(void);
    void (*AppPreferences_GetWifiApConfig)(AppApConfig_t *out);
    AM_Result (*AppPreferences_SetWifiApApplyPending)(bool pending);
    AM_Result (*AppPreferences_SetWifiApConfig)(const AppApConfig_t *config, bool applyPending);
    WifiBridge_Backend (*WifiBridge_GetActiveBackend)(const WifiBridge_Handle *h);
    uint8_t (*WifiBridge_IsWaitingValidation)(WifiBridge_Handle *h);
    WifiBridge_Result (*WifiBridge_SendCommand)(WifiBridge_Handle *h,
                                         uint8_t cmd,
                                         const uint8_t *payload,
                                         uint8_t len);
} ApServices;
typedef struct {
    const ApServices *services;
    SystemHandle *system;
    WifiBridge_Handle *wifiBridge;
    AppApApplyState_t appApApplyState;
    uint32_t appApNotBeforeMs;
    uint32_t appApApplyDeadlineMs;
} ApHandle;
extern ApHandle *Ap_Context;
#endif
