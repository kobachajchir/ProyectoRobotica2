#include "ap_context.h"
#include "app_ap_config.h"

#include <string.h>

#include "utils/macros_utils.h"

#define now_ms() (Ap_Context->services->now_ms())
#define WifiBridge_SendCommand (Ap_Context->services->WifiBridge_SendCommand)
#define WifiBridge_IsWaitingValidation (Ap_Context->services->WifiBridge_IsWaitingValidation)
#define WifiBridge_GetActiveBackend (Ap_Context->services->WifiBridge_GetActiveBackend)
#define AppPreferences_Get (Ap_Context->services->AppPreferences_Get)
#define AppPreferences_GetWifiApConfig (Ap_Context->services->AppPreferences_GetWifiApConfig)
#define AppPreferences_SetWifiApConfig (Ap_Context->services->AppPreferences_SetWifiApConfig)
#define AppPreferences_SetWifiApApplyPending (Ap_Context->services->AppPreferences_SetWifiApApplyPending)
#define wifiBridge (*Ap_Context->wifiBridge)
#define appApApplyState (Ap_Context->appApApplyState)
#define appApNotBeforeMs (Ap_Context->appApNotBeforeMs)
#define appApApplyDeadlineMs (Ap_Context->appApApplyDeadlineMs)
#define autoMicroSystem (*Ap_Context->system)

#define APP_AP_WEB_RESPONSE_MARGIN_MS  75u
#define APP_AP_RETRY_PERIOD_MS         2000u
#define APP_AP_APPLY_TIMEOUT_MS         1500u





static size_t AppApConfig_StrLen(const char *text, size_t capacity)
{
    size_t length = 0u;

    if (text == NULL) {
        return capacity;
    }
    while (length < capacity && text[length] != '\0') {
        length++;
    }
    return length;
}

static bool AppApConfig_SsidIsValid(const char *ssid)
{
    const size_t length = AppApConfig_StrLen(ssid, APP_AP_SSID_MAX_BYTES + 1u);

    if (length == 0u || length > APP_AP_SSID_MAX_BYTES) {
        return false;
    }
    for (size_t i = 0u; i < length; ++i) {
        const uint8_t value = (uint8_t)ssid[i];
        if (value < 0x20u || value == 0x7Fu) {
            return false;
        }
    }
    return true;
}

static bool AppApConfig_PasswordIsValid(const char *password, bool protected)
{
    const size_t length = AppApConfig_StrLen(
        password, APP_AP_PASSWORD_MAX_BYTES + 1u);

    if (length > APP_AP_PASSWORD_MAX_BYTES) {
        return false;
    }
    if (length == 0u) {
        return !protected;
    }
    return length >= APP_AP_PASSWORD_MIN_BYTES;
}

bool AppApConfig_Ipv4IsValid(const uint8_t ipv4[4])
{
    if (ipv4 == NULL) {
        return false;
    }
    if ((ipv4[0] | ipv4[1] | ipv4[2] | ipv4[3]) == 0u) {
        return true;
    }
    return ipv4[0] > 0u && ipv4[0] < 224u && ipv4[0] != 127u &&
           ipv4[3] > 0u && ipv4[3] < 255u;
}

AppApStatus_t AppApConfig_ParseIpv4(const char *text, uint8_t out[4])
{
    uint8_t parsed[4] = {0u};
    uint16_t value = 0u;
    uint8_t octet = 0u;
    bool hasDigit = false;

    if (text == NULL || out == NULL || text[0] == '\0') {
        return APP_AP_STATUS_INVALID_IPV4;
    }
    for (size_t i = 0u;; ++i) {
        const char ch = text[i];
        if (ch >= '0' && ch <= '9') {
            hasDigit = true;
            value = (uint16_t)(value * 10u + (uint16_t)(ch - '0'));
            if (value > 255u) {
                return APP_AP_STATUS_INVALID_IPV4;
            }
        } else if (ch == '.' || ch == '\0') {
            if (!hasDigit || octet >= 4u) {
                return APP_AP_STATUS_INVALID_IPV4;
            }
            parsed[octet++] = (uint8_t)value;
            value = 0u;
            hasDigit = false;
            if (ch == '\0') {
                break;
            }
        } else {
            return APP_AP_STATUS_INVALID_IPV4;
        }
    }
    if (octet != 4u || !AppApConfig_Ipv4IsValid(parsed)) {
        return APP_AP_STATUS_INVALID_IPV4;
    }
    memcpy(out, parsed, sizeof(parsed));
    return APP_AP_STATUS_OK;
}

void AppApConfig_Init(void)
{
    const AppPreferences_t *preferences = AppPreferences_Get();

    appApApplyState = preferences->wifiApApplyPending
        ? APP_AP_APPLY_PENDING
        : APP_AP_APPLY_SYNCED;
    appApNotBeforeMs = now_ms();
}

void AppApConfig_Get(AppApConfig_t *out)
{
    if (out == NULL) {
        return;
    }
    AppPreferences_GetWifiApConfig(out);
    out->applyState = appApApplyState;
}

AppApStatus_t AppApConfig_ApplyPatch(const AppApConfigPatch_t *patch,
                                     uint8_t *changedMask)
{
    AppApConfig_t current;
    AppApConfig_t next;
    uint8_t changed = 0u;

    if (changedMask != NULL) {
        *changedMask = 0u;
    }
    if (patch == NULL || patch->fieldMask == 0u ||
        (patch->fieldMask & (uint8_t)~APP_AP_FIELD_ALL) != 0u) {
        return APP_AP_STATUS_INVALID_PAYLOAD;
    }

    AppApConfig_Get(&current);
    next = current;
    if ((patch->fieldMask & APP_AP_FIELD_SSID) != 0u) {
        if (!AppApConfig_SsidIsValid(patch->values.ssid)) {
            return APP_AP_STATUS_INVALID_SSID;
        }
        if (strcmp(next.ssid, patch->values.ssid) != 0) {
            memcpy(next.ssid, patch->values.ssid, sizeof(next.ssid));
            changed |= APP_AP_FIELD_SSID;
        }
    }
    if ((patch->fieldMask & APP_AP_FIELD_PASSWORD) != 0u) {
        if (!AppApConfig_PasswordIsValid(patch->values.password, false) ||
            patch->values.password[0] == '\0') {
            return APP_AP_STATUS_INVALID_PASSWORD;
        }
        if (strcmp(next.password, patch->values.password) != 0) {
            memcpy(next.password, patch->values.password, sizeof(next.password));
            changed |= APP_AP_FIELD_PASSWORD;
        }
    }
    if ((patch->fieldMask & APP_AP_FIELD_PROTECTED) != 0u) {
        const bool protected = patch->values.protected ? true : false;
        if (next.protected != protected) {
            next.protected = protected;
            changed |= APP_AP_FIELD_PROTECTED;
        }
    }
    if ((patch->fieldMask & APP_AP_FIELD_IPV4) != 0u) {
        if (!AppApConfig_Ipv4IsValid(patch->values.ipv4)) {
            return APP_AP_STATUS_INVALID_IPV4;
        }
        if (memcmp(next.ipv4, patch->values.ipv4, sizeof(next.ipv4)) != 0) {
            memcpy(next.ipv4, patch->values.ipv4, sizeof(next.ipv4));
            changed |= APP_AP_FIELD_IPV4;
        }
    }
    if (!AppApConfig_PasswordIsValid(next.password, next.protected)) {
        return APP_AP_STATUS_INVALID_PASSWORD;
    }
    if (changed == 0u) {
        if (changedMask != NULL) {
            *changedMask = 0u;
        }
        return APP_AP_STATUS_OK;
    }
    if (AppPreferences_SetWifiApConfig(&next, true) != AM_OK) {
        return APP_AP_STATUS_STORAGE_ERROR;
    }

    appApApplyState = APP_AP_APPLY_PENDING;
    appApNotBeforeMs = now_ms() + APP_AP_WEB_RESPONSE_MARGIN_MS;
    if (changedMask != NULL) {
        *changedMask = changed;
    }
    return APP_AP_STATUS_OK;
}

void AppApConfig_RequestRetry(void)
{
    if (AppPreferences_Get()->wifiApApplyPending) {
        appApApplyState = APP_AP_APPLY_PENDING;
        appApNotBeforeMs = now_ms();
    }
}

void AppApConfig_OnEspApplyResponse(uint8_t status)
{
    appApApplyDeadlineMs = 0u;
    if (status == 0u) {
        if (AppPreferences_SetWifiApApplyPending(false) == AM_OK) {
            appApApplyState = APP_AP_APPLY_SYNCED;
        } else {
            appApApplyState = APP_AP_APPLY_FAILED;
            appApNotBeforeMs = now_ms() + APP_AP_RETRY_PERIOD_MS;
        }
    } else {
        appApApplyState = APP_AP_APPLY_FAILED;
        appApNotBeforeMs = now_ms() + APP_AP_RETRY_PERIOD_MS;
    }
}

void AppApConfig_Task(void)
{
    AppApConfig_t config;
    uint8_t payload[1u + APP_AP_SSID_MAX_BYTES + 1u +
                    APP_AP_PASSWORD_MAX_BYTES + 1u + 4u];
    uint8_t offset = 0u;
    size_t ssidLength;
    size_t passwordLength;
    const uint32_t now = now_ms();
    const WifiBridge_Backend backend = WifiBridge_GetActiveBackend(&wifiBridge);

    if (appApApplyState == APP_AP_APPLY_APPLYING) {
        if ((int32_t)(now - appApApplyDeadlineMs) < 0) {
            return;
        }
        appApApplyState = APP_AP_APPLY_FAILED;
        appApNotBeforeMs = now + APP_AP_RETRY_PERIOD_MS;
        return;
    }
    if (!AppPreferences_Get()->wifiApApplyPending ||
        (int32_t)(now - appApNotBeforeMs) < 0 ||
        WifiBridge_IsWaitingValidation(&wifiBridge)) {
        return;
    }
    if (backend != WIFI_BRIDGE_BACKEND_WEB && backend != WIFI_BRIDGE_BACKEND_AT) {
        return;
    }
    if (backend == WIFI_BRIDGE_BACKEND_AT &&
        !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)) {
        return;
    }

    AppApConfig_Get(&config);
    ssidLength = strlen(config.ssid);
    passwordLength = strlen(config.password);
    payload[offset++] = (uint8_t)ssidLength;
    memcpy(&payload[offset], config.ssid, ssidLength);
    offset = (uint8_t)(offset + ssidLength);
    payload[offset++] = (uint8_t)passwordLength;
    memcpy(&payload[offset], config.password, passwordLength);
    offset = (uint8_t)(offset + passwordLength);
    payload[offset++] = config.protected ? 1u : 0u;
    memcpy(&payload[offset], config.ipv4, sizeof(config.ipv4));
    offset = (uint8_t)(offset + sizeof(config.ipv4));

    if (WifiBridge_SendCommand(&wifiBridge,
                               WIFI_BRIDGE_CMD_ID_SET_AP_CONFIG,
                               payload,
                               offset) == WIFI_BRIDGE_OK) {
        appApApplyState = APP_AP_APPLY_APPLYING;
        appApApplyDeadlineMs = now + APP_AP_APPLY_TIMEOUT_MS;
    } else {
        appApApplyState = APP_AP_APPLY_FAILED;
        appApNotBeforeMs = now + APP_AP_RETRY_PERIOD_MS;
    }
}
