#include "system_flags.h"
#include "utils/macros_utils.h"
/*
 * wifi_bridge.c - runtime dispatch for ESP-AT and ESP-WEB/UNER backends.
 */
#include "wifi_bridge.h"

#include <string.h>

#include "wifi_bridge_at.h"
#include "wifi_bridge_uner.h"

static const WifiBridge_CapabilityMask_t WIFI_BRIDGE_AT_CAPABILITIES =
    WIFI_BRIDGE_CAP_ALIVE |
    WIFI_BRIDGE_CAP_STATUS |
    WIFI_BRIDGE_CAP_FIRMWARE |
    WIFI_BRIDGE_CAP_WIFI_STA |
    WIFI_BRIDGE_CAP_WIFI_AP |
    WIFI_BRIDGE_CAP_WIFI_SCAN;

static const WifiBridge_CapabilityMask_t WIFI_BRIDGE_WEB_CAPABILITIES =
    WIFI_BRIDGE_AT_CAPABILITIES |
    WIFI_BRIDGE_CAP_REMOTE_UI |
    WIFI_BRIDGE_CAP_PREFERENCES |
    WIFI_BRIDGE_CAP_I2C_POLICY |
    WIFI_BRIDGE_CAP_MPU_SNAPSHOT |
    WIFI_BRIDGE_CAP_MPU_STREAM |
    WIFI_BRIDGE_CAP_IR_STREAM |
    WIFI_BRIDGE_CAP_BINARY_ASSETS |
    WIFI_BRIDGE_CAP_MOTION_CONTROL |
    WIFI_BRIDGE_CAP_SENSOR_TELEMETRY |
    WIFI_BRIDGE_CAP_REMOTE_AUTH;

static WifiBridge_Result WifiBridge_CopyRuntimeText(char *dst,
                                                    size_t dst_len,
                                                    const char *src)
{
    size_t i = 0u;
    size_t len = 0u;

    if ((dst == NULL) || (dst_len == 0u) || (src == NULL)) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    while (src[len] != '\0') {
        if (len + 1u >= dst_len) {
            return WIFI_BRIDGE_BAD_ARG;
        }
        len++;
    }

    while (i < len) {
        dst[i] = src[i];
        i++;
    }

    dst[i] = '\0';
    return WIFI_BRIDGE_OK;
}

static void WifiBridge_SetModeFlags(WifiBridge_Handle *h, uint8_t mode)
{
    if (mode == WIFI_BRIDGE_MODE_AP) {
        SET_FLAG((*h->system).runtime.systemFlags2, AP_ACTIVE);
        CLEAR_FLAG((*h->system).runtime.systemFlags2, WIFI_ACTIVE);
    } else if (mode == WIFI_BRIDGE_MODE_STA) {
        CLEAR_FLAG((*h->system).runtime.systemFlags2, AP_ACTIVE);
        SET_FLAG((*h->system).runtime.systemFlags2, WIFI_ACTIVE);
    } else if (mode == WIFI_BRIDGE_MODE_AP_STA) {
        SET_FLAG((*h->system).runtime.systemFlags2, AP_ACTIVE);
        SET_FLAG((*h->system).runtime.systemFlags2, WIFI_ACTIVE);
    }
}

static void WifiBridge_ClearModeFlags(WifiBridge_Handle *h)
{
    CLEAR_FLAG((*h->system).runtime.systemFlags2, ESP_PRESENT);
    CLEAR_FLAG((*h->system).runtime.systemFlags2, WIFI_ACTIVE);
    CLEAR_FLAG((*h->system).runtime.systemFlags2, AP_ACTIVE);
}

static void WifiBridge_MarkConnectionLost(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }

    if (h->flags.bits.connected || IS_FLAG_SET((*h->system).runtime.systemFlags2, ESP_PRESENT)) {
        h->connection_lost_event = 1u;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        WifiBridge_AT_ResetNetworkSession(h);
    }

    h->flags.bits.connected = 0u;
    h->alive_pending = 0u;
    h->missed_alive_count = 0u;
    h->active_backend = WIFI_BRIDGE_BACKEND_DETECTING;
    h->probe = WIFI_BRIDGE_PROBE_AT;
    h->mode = WIFI_BRIDGE_MODE_UNKNOWN;
    WifiBridge_ClearModeFlags(h);
}

static WifiBridge_Backend WifiBridge_ProbeBackend(const WifiBridge_Handle *h)
{
    return (h != NULL && h->probe == WIFI_BRIDGE_PROBE_WEB)
        ? WIFI_BRIDGE_BACKEND_WEB
        : WIFI_BRIDGE_BACKEND_AT;
}

static void WifiBridge_ToggleProbe(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }

    h->probe = (h->probe == WIFI_BRIDGE_PROBE_AT)
        ? WIFI_BRIDGE_PROBE_WEB
        : WIFI_BRIDGE_PROBE_AT;
}

static WifiBridge_Result WifiBridge_SendAliveForBackend(WifiBridge_Handle *h,
                                                        WifiBridge_Backend backend,
                                                        uint16_t web_period_ms)
{
    uint8_t payload[2u];

    if (backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_SendCommand(h, WIFI_BRIDGE_CMD_ID_PING, NULL, 0u);
    }

    if (backend == WIFI_BRIDGE_BACKEND_WEB) {
        payload[0] = (uint8_t)(web_period_ms & 0xFFu);
        payload[1] = (uint8_t)(web_period_ms >> 8);
        return WifiBridge_UNER_SendCommand(h,
                                           WIFI_BRIDGE_CMD_ID_PING,
                                           payload,
                                           (uint8_t)sizeof(payload));
    }

    return WIFI_BRIDGE_UNSUPPORTED;
}

static WifiBridge_CapabilityMask_t WifiBridge_CommandRequiredCapability(uint8_t cmd)
{
    switch (cmd) {
        case WIFI_BRIDGE_CMD_ID_PING:
            return WIFI_BRIDGE_CAP_ALIVE;

        case WIFI_BRIDGE_CMD_ID_GET_STATUS:
            return WIFI_BRIDGE_CAP_STATUS;

        case WIFI_BRIDGE_CMD_ID_REQUEST_FIRMWARE:
            return WIFI_BRIDGE_CAP_FIRMWARE;

        case WIFI_BRIDGE_CMD_ID_SET_MODE_STA:
        case WIFI_BRIDGE_CMD_ID_SET_CREDENTIALS:
        case WIFI_BRIDGE_CMD_ID_CLEAR_CREDENTIALS:
        case WIFI_BRIDGE_CMD_ID_CONNECT_WIFI:
        case WIFI_BRIDGE_CMD_ID_DISCONNECT_WIFI:
        case WIFI_BRIDGE_CMD_ID_WIFI_GET_DETAIL:
        case WIFI_BRIDGE_CMD_ID_GET_CREDENTIALS:
        case WIFI_BRIDGE_CMD_ID_SET_AUTO_RECONNECT:
            return WIFI_BRIDGE_CAP_WIFI_STA;

        case WIFI_BRIDGE_CMD_ID_SET_MODE_AP:
        case WIFI_BRIDGE_CMD_ID_SET_AP_CONFIG:
        case WIFI_BRIDGE_CMD_ID_START_AP:
        case WIFI_BRIDGE_CMD_ID_STOP_AP:
        case WIFI_BRIDGE_CMD_ID_GET_CONNECTED_USERS:
        case WIFI_BRIDGE_CMD_ID_GET_USER_INFO:
        case WIFI_BRIDGE_CMD_ID_GET_CONNECTED_USERS_MODE:
            return WIFI_BRIDGE_CAP_WIFI_AP;

        case WIFI_BRIDGE_CMD_ID_START_SCAN:
        case WIFI_BRIDGE_CMD_ID_GET_SCAN_RESULTS:
        case WIFI_BRIDGE_CMD_ID_STOP_SCAN:
            return WIFI_BRIDGE_CAP_WIFI_SCAN;

        case WIFI_BRIDGE_CMD_ID_SET_AP_FALLBACK:
        case WIFI_BRIDGE_CMD_ID_GET_PREFERENCES:
        case WIFI_BRIDGE_CMD_ID_SET_PREFERENCES:
            return WIFI_BRIDGE_CAP_PREFERENCES;

        case WIFI_BRIDGE_CMD_ID_SET_I2C_POLICY:
        case WIFI_BRIDGE_CMD_ID_GET_I2C_POLICY:
            return WIFI_BRIDGE_CAP_I2C_POLICY;

        case WIFI_BRIDGE_CMD_ID_GET_MPU_SNAPSHOT:
            return WIFI_BRIDGE_CAP_MPU_SNAPSHOT;

        case WIFI_BRIDGE_CMD_ID_SET_MPU_STREAM:
        case WIFI_BRIDGE_CMD_ID_STOP_MPU_STREAM:
            return WIFI_BRIDGE_CAP_MPU_STREAM;

        case WIFI_BRIDGE_CMD_ID_SET_IR_STREAM:
        case WIFI_BRIDGE_CMD_ID_STOP_IR_STREAM:
            return WIFI_BRIDGE_CAP_IR_STREAM;

        case WIFI_BRIDGE_CMD_ID_GET_CURRENT_SCREEN:
        case WIFI_BRIDGE_CMD_ID_MENU_ITEM_CLICK:
        case WIFI_BRIDGE_CMD_ID_TRIGGER_ENCODER_BUTTON:
        case WIFI_BRIDGE_CMD_ID_TRIGGER_USER_BUTTON:
        case WIFI_BRIDGE_CMD_ID_REQUEST_SCREEN_PAGE:
        case WIFI_BRIDGE_CMD_ID_TRIGGER_ENCODER_ROTATE_LEFT:
        case WIFI_BRIDGE_CMD_ID_TRIGGER_ENCODER_ROTATE_RIGHT:
        case WIFI_BRIDGE_CMD_ID_GET_CAR_MODE:
        case WIFI_BRIDGE_CMD_ID_SET_ENCODER_FAST:
            return WIFI_BRIDGE_CAP_REMOTE_UI;

        case WIFI_BRIDGE_CMD_ID_AUTH_VALIDATE_PIN:
        case WIFI_BRIDGE_CMD_ID_AUTH_PIN_GRANTED:
        case WIFI_BRIDGE_CMD_ID_AUTH_REMOTE_RESULT:
            return WIFI_BRIDGE_CAP_REMOTE_AUTH;

        case WIFI_BRIDGE_CMD_ID_BINARY_ASSET_STREAM:
            return WIFI_BRIDGE_CAP_BINARY_ASSETS;

        case WIFI_BRIDGE_CMD_ID_GET_IR_SNAPSHOT:
            return WIFI_BRIDGE_CAP_SENSOR_TELEMETRY;

        case WIFI_BRIDGE_CMD_ID_GET_TELEMETRY_SNAPSHOT:
        case WIFI_BRIDGE_CMD_ID_GET_BALANCE_PID:
        case WIFI_BRIDGE_CMD_ID_SET_BALANCE_PID:
        case WIFI_BRIDGE_CMD_ID_GET_CONTROL_SNAPSHOT:
            return WIFI_BRIDGE_CAP_SENSOR_TELEMETRY |
                   WIFI_BRIDGE_CAP_MOTION_CONTROL;

        default:
            return 0u;
    }
}

static WifiBridge_Result WifiBridge_DispatchCommand(WifiBridge_Handle *h,
                                                    uint8_t cmd,
                                                    const uint8_t *payload,
                                                    uint8_t len)
{
    WifiBridge_CapabilityMask_t required;

    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    required = WifiBridge_CommandRequiredCapability(cmd);
    if ((required != 0u) && !WifiBridge_HasCapability(h, required)) {
        return WIFI_BRIDGE_UNSUPPORTED;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_SendCommand(h, cmd, payload, len);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_SendCommand(h, cmd, payload, len);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

WifiBridge_Result WifiBridge_Init(WifiBridge_Handle *h, void *huart, const WifiBridge_Hardware *hardware,
    const WifiBridge_Services *services, void *services_ctx, SystemHandle *system, WifiBridge_Credentials *credentials)
{
    WifiBridge_Result status;

    if (h == NULL || huart == NULL || hardware == NULL || services == NULL || system == NULL || credentials == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    memset(h, 0, sizeof(*h));
    h->huart = huart;
    h->hardware = hardware; h->services = services; h->services_ctx = services_ctx;
    h->system = system; h->credentials = credentials;
    h->active_backend = WIFI_BRIDGE_BACKEND_DETECTING;
    h->probe = WIFI_BRIDGE_PROBE_AT;
    h->mode = WIFI_BRIDGE_MODE_UNKNOWN;

    status = WifiBridge_AT_Init(h, huart);
    if (status != WIFI_BRIDGE_OK) {
        return status;
    }

    status = WifiBridge_UNER_Init(h, huart);
    if (status != WIFI_BRIDGE_OK) {
        return status;
    }

    return WIFI_BRIDGE_OK;
}

WifiBridge_Result WifiBridge_Start(WifiBridge_Handle *h)
{
    if (h == NULL || h->huart == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (!h->rx_started) {
        if (h->hardware->rx_start(h->huart, h->rx_dma, WIFI_BRIDGE_UART_RX_DMA_SIZE) != AM_OK) {
            return WIFI_BRIDGE_BUSY;
        }
        h->rx_started = 1u;
    }

    h->hardware->enable_idle(h->huart);
    (void)WifiBridge_AT_Start(h);
    (void)WifiBridge_UNER_Start(h);
    WifiBridge_ResetRxPos(h);
    return WIFI_BRIDGE_OK;
}

void WifiBridge_Process(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }

    WifiBridge_AT_Process(h);
    WifiBridge_UNER_Process(h);
}

static uint8_t WifiBridge_GetAliveMaxMisses(const WifiBridge_Handle *h)
{
    return (h != NULL && h->active_backend == WIFI_BRIDGE_BACKEND_WEB)
        ? WIFI_BRIDGE_WEB_ALIVE_MAX_MISSES
        : WIFI_BRIDGE_ALIVE_MAX_MISSES;
}

void WifiBridge_RxCheck(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }

    WIFI_FLAG_SET(h, WIFI_FLAG_RX_PENDING);
    WifiBridge_UNER_RxCheck(h);
}

void WifiBridge_OnIdleIrq(WifiBridge_Handle *h)
{
    WifiBridge_RxCheck(h);
}

void WifiBridge_OnTxComplete(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }

    WifiBridge_AT_OnTxComplete(h);
    WifiBridge_UNER_OnTxComplete(h);
}

void WifiBridge_OnRxHalfComplete(WifiBridge_Handle *h)
{
    WifiBridge_RxCheck(h);
}

void WifiBridge_OnRxComplete(WifiBridge_Handle *h)
{
    WifiBridge_RxCheck(h);
}

void WifiBridge_OnError(WifiBridge_Handle *h)
{
    if (h != NULL) {
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_BUSY);
        WIFI_FLAG_CLEAR(h, WIFI_FLAG_TX_BUSY);
        h->tx_busy = 0u;
        h->rx_started = 0u;
        WIFI_FLAG_SET(h, WIFI_FLAG_GOT_ERROR);
    }
}

void WifiBridge_ResetRxPos(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return;
    }

    WifiBridge_AT_ResetRxPos(h);
    WifiBridge_UNER_ResetRxPos(h);
}

void WifiBridge_SetEventCallback(WifiBridge_Handle *h, WifiBridge_EventCallback cb)
{
    if (h != NULL) {
        h->on_event = cb;
    }
}

void WifiBridge_EmitEvent(WifiBridge_Handle *h, const WifiBridge_Event *evt)
{
    if ((h == NULL) || (evt == NULL)) {
        return;
    }

    h->last_event = *evt;
    if (h->on_event != NULL) {
        h->on_event(evt);
    }
}

WifiBridge_Result WifiBridge_SetRuntimeCredentials_H(WifiBridge_Handle *h, const char *ssid, const char *password)
{
    WifiBridge_Result status = WifiBridge_SetRuntimeSsid_H(h, ssid);
    if (status != WIFI_BRIDGE_OK) {
        return status;
    }

    return WifiBridge_SetRuntimePassword_H(h, password);
}

WifiBridge_Result WifiBridge_SetRuntimeSsid_H(WifiBridge_Handle *h, const char *ssid)
{
    WifiBridge_Result status = WifiBridge_CopyRuntimeText((*h->credentials).ssid,
                                                          sizeof((*h->credentials).ssid),
                                                          ssid);
    if (status == WIFI_BRIDGE_OK) {
        (*h->credentials).flags.bits.ssid_set = (ssid[0] != '\0') ? 1u : 0u;
    }

    return status;
}

WifiBridge_Result WifiBridge_SetRuntimePassword_H(WifiBridge_Handle *h, const char *password)
{
    WifiBridge_Result status = WifiBridge_CopyRuntimeText((*h->credentials).password,
                                                          sizeof((*h->credentials).password),
                                                          password);
    if (status == WIFI_BRIDGE_OK) {
        (*h->credentials).flags.bits.password_set = (password[0] != '\0') ? 1u : 0u;
    }

    return status;
}

const WifiBridge_Credentials *WifiBridge_GetRuntimeCredentials_H(WifiBridge_Handle *h)
{
    return &(*h->credentials);
}

WifiBridge_Result WifiBridge_SetModeSTA(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_SET_MODE_STA, NULL, 0u);
}

WifiBridge_Result WifiBridge_SetModeAP(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_SET_MODE_AP, NULL, 0u);
}

WifiBridge_Result WifiBridge_SetCredentials(WifiBridge_Handle *h,
                                            const char *ssid,
                                            const char *pass)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_SetCredentials(h, ssid, pass);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_SetCredentials(h, ssid, pass);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

WifiBridge_Result WifiBridge_Connect(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_CONNECT_WIFI, NULL, 0u);
}

WifiBridge_Result WifiBridge_Disconnect(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_DISCONNECT_WIFI, NULL, 0u);
}

WifiBridge_Result WifiBridge_StartScan(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_START_SCAN, NULL, 0u);
}

WifiBridge_Result WifiBridge_StopScan(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_STOP_SCAN, NULL, 0u);
}

WifiBridge_Result WifiBridge_GetScanResults(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_GET_SCAN_RESULTS, NULL, 0u);
}

WifiBridge_Result WifiBridge_GetStatus(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_GET_STATUS, NULL, 0u);
}

WifiBridge_Result WifiBridge_StartAP(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_START_AP, NULL, 0u);
}

WifiBridge_Result WifiBridge_StopAP(WifiBridge_Handle *h)
{
    return WifiBridge_DispatchCommand(h, WIFI_BRIDGE_CMD_ID_STOP_AP, NULL, 0u);
}

WifiBridge_Result WifiBridge_SendCommand(WifiBridge_Handle *h,
                                         uint8_t cmd,
                                         const uint8_t *payload,
                                         uint8_t len)
{
    if (cmd == WIFI_BRIDGE_CMD_ID_PING) {
        uint16_t web_period_ms = 0u;

        if (payload != NULL && len == 2u) {
            web_period_ms = (uint16_t)(((uint16_t)payload[0]) |
                                       ((uint16_t)payload[1] << 8));
        } else if (len != 0u) {
            return WIFI_BRIDGE_BAD_ARG;
        }

        return WifiBridge_SendAliveWithWebPeriod(h, web_period_ms);
    }

    return WifiBridge_DispatchCommand(h, cmd, payload, len);
}

WifiBridge_Result WifiBridge_SendWifiCredentialRequest(WifiBridge_Handle *h,
                                                       const char *ssid)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_SendWifiCredentialRequest(h, ssid);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_SendWifiCredentialRequest(h, ssid);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

WifiBridge_Result WifiBridge_RequestWifiNetworkDetail(WifiBridge_Handle *h,
                                                      const char *ssid)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_RequestWifiNetworkDetail(h, ssid);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_RequestWifiNetworkDetail(h, ssid);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

WifiBridge_Result WifiBridge_SendEspReboot(WifiBridge_Handle *h, uint8_t boot_mode)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_SendEspReboot(h, boot_mode);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_SendEspReboot(h, boot_mode);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

WifiBridge_Result WifiBridge_SendAlive(WifiBridge_Handle *h)
{
    return WifiBridge_SendAliveWithWebPeriod(h, 0u);
}

WifiBridge_Result WifiBridge_SendAliveWithWebPeriod(WifiBridge_Handle *h,
                                                    uint16_t web_period_ms)
{
    const uint32_t now = h->hardware->now_ms(h->huart);
    WifiBridge_Backend backend;
    WifiBridge_Result result;

    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    WifiBridge_RxCheck(h);
    WifiBridge_Process(h);

    if (h->tx_busy) {
        return WIFI_BRIDGE_BUSY;
    }

    if (h->alive_pending) {
        if ((uint32_t)(now - h->last_probe_ms) < WIFI_BRIDGE_PROBE_TIMEOUT_MS) {
            return WIFI_BRIDGE_BUSY;
        }

        if (h->missed_alive_count < 0xFFu) {
            h->missed_alive_count++;
        }

        h->alive_pending = 0u;

        if (h->active_backend == WIFI_BRIDGE_BACKEND_DETECTING) {
            WifiBridge_ToggleProbe(h);
        } else if (h->missed_alive_count >= WifiBridge_GetAliveMaxMisses(h)) {
            WifiBridge_MarkConnectionLost(h);
        }
    }

    backend = (h->active_backend == WIFI_BRIDGE_BACKEND_DETECTING)
        ? WifiBridge_ProbeBackend(h)
        : h->active_backend;

    result = WifiBridge_SendAliveForBackend(h, backend, web_period_ms);
    if (result == WIFI_BRIDGE_OK) {
        h->alive_pending = 1u;
        h->last_probe_ms = now;
    }

    return result;
}

void WifiBridge_RegisterAliveResponse(WifiBridge_Handle *h, uint8_t mode)
{
    WifiBridge_Backend backend;

    if (h == NULL) {
        return;
    }

    backend = (h->active_backend == WIFI_BRIDGE_BACKEND_DETECTING)
        ? WifiBridge_ProbeBackend(h)
        : h->active_backend;
    WifiBridge_RegisterBackendAlive(h, backend, mode);
}

void WifiBridge_RegisterBackendAlive(WifiBridge_Handle *h,
                                     WifiBridge_Backend backend,
                                     uint8_t mode)
{
    WifiBridge_Backend previous_backend;

    if (h == NULL || backend == WIFI_BRIDGE_BACKEND_DETECTING) {
        return;
    }
    previous_backend = h->active_backend;

    if (previous_backend == WIFI_BRIDGE_BACKEND_AT &&
        backend != WIFI_BRIDGE_BACKEND_AT) {
        WifiBridge_AT_ResetNetworkSession(h);
    }
    h->active_backend = backend;
    h->alive_pending = 0u;
    h->missed_alive_count = 0u;
    h->connection_lost_event = 0u;
    h->last_rx_ms = h->hardware->now_ms(h->huart);
    h->flags.bits.connected = 1u;
    SET_FLAG((*h->system).runtime.systemFlags2, ESP_PRESENT);

    if (mode != WIFI_BRIDGE_MODE_UNKNOWN) {
        h->mode = mode;
        WifiBridge_SetModeFlags(h, mode);
        if (backend == WIFI_BRIDGE_BACKEND_AT &&
            (mode == WIFI_BRIDGE_MODE_STA || mode == WIFI_BRIDGE_MODE_AP_STA) &&
            !(*h->system).connectivity.espWifiConnection.staIpValid) {
            CLEAR_FLAG((*h->system).runtime.systemFlags2, WIFI_ACTIVE);
        }
    }
    if (backend == WIFI_BRIDGE_BACKEND_AT && previous_backend != backend) {
        (void)WifiBridge_AT_StartNetworkService(h);
    }
}

uint8_t WifiBridge_ConsumeConnectionLostEvent(WifiBridge_Handle *h)
{
    uint8_t lost;

    if (h == NULL) {
        return 0u;
    }

    lost = h->connection_lost_event;
    h->connection_lost_event = 0u;
    return lost;
}

WifiBridge_Result WifiBridge_ReportScreenChanged(WifiBridge_Handle *h,
                                                 uint32_t screen_code,
                                                 uint8_t source)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_ReportScreenChanged(h, screen_code, source);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_ReportScreenChanged(h, screen_code, source);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

WifiBridge_Result WifiBridge_ReportMenuSelectionChanged(WifiBridge_Handle *h,
                                                        uint32_t screen_code,
                                                        uint8_t selected_index,
                                                        uint8_t item_count,
                                                        uint8_t source)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_ReportMenuSelectionChanged(h, screen_code, selected_index, item_count, source);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_ReportMenuSelectionChanged(h, screen_code, selected_index, item_count, source);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

WifiBridge_Result WifiBridge_ReportCarModeChanged(WifiBridge_Handle *h, uint8_t mode)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_ReportCarModeChanged(h, mode);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_ReportCarModeChanged(h, mode);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

WifiBridge_Result WifiBridge_RequestBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id)
{
    if (h == NULL) {
        return WIFI_BRIDGE_BAD_ARG;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_RequestBinaryAsset(h, asset_id);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_RequestBinaryAsset(h, asset_id);
    }

    return WIFI_BRIDGE_TIMEOUT;
}

uint8_t WifiBridge_GetBinaryAsset(WifiBridge_Handle *h,
                                  uint8_t asset_id,
                                  WifiBridge_BinaryAssetView *asset)
{
    if (h == NULL) {
        return 0u;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_GetBinaryAsset(h, asset_id, asset);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_GetBinaryAsset(h, asset_id, asset);
    }

    return 0u;
}

uint8_t WifiBridge_IsWaitingValidation(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return 0u;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_IsWaitingValidation(h);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_IsWaitingValidation(h);
    }

    return 0u;
}

uint8_t WifiBridge_GetWaitingCommandId(WifiBridge_Handle *h)
{
    if (h == NULL) {
        return 0u;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WifiBridge_AT_GetWaitingCommandId(h);
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WifiBridge_UNER_GetWaitingCommandId(h);
    }

    return 0u;
}

WifiBridge_Backend WifiBridge_GetActiveBackend(const WifiBridge_Handle *h)
{
    return (h != NULL) ? h->active_backend : WIFI_BRIDGE_BACKEND_DETECTING;
}

uint8_t WifiBridge_IsConnected(const WifiBridge_Handle *h)
{
    return (h != NULL &&
            h->active_backend != WIFI_BRIDGE_BACKEND_DETECTING &&
            h->flags.bits.connected != 0u) ? 1u : 0u;
}

WifiBridge_CapabilityMask_t WifiBridge_GetCapabilities(const WifiBridge_Handle *h)
{
    if (!WifiBridge_IsConnected(h)) {
        return 0u;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return WIFI_BRIDGE_AT_CAPABILITIES | h->at_detected_capabilities;
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return WIFI_BRIDGE_WEB_CAPABILITIES;
    }

    return 0u;
}

uint8_t WifiBridge_HasCapability(const WifiBridge_Handle *h,
                                 WifiBridge_CapabilityMask_t capability)
{
    WifiBridge_CapabilityMask_t available;

    if (capability == 0u) {
        return 1u;
    }

    available = WifiBridge_GetCapabilities(h);
    return ((available & capability) == capability) ? 1u : 0u;
}

uint8_t WifiBridge_GetMode(const WifiBridge_Handle *h)
{
    return (h != NULL) ? h->mode : WIFI_BRIDGE_MODE_UNKNOWN;
}

const char *WifiBridge_GetBackendLabel(const WifiBridge_Handle *h)
{
    if (h == NULL || h->active_backend == WIFI_BRIDGE_BACKEND_DETECTING) {
        return "...";
    }

    if (!WifiBridge_IsConnected(h)) {
        return "No ESP";
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_AT) {
        return "AT";
    }

    if (h->active_backend == WIFI_BRIDGE_BACKEND_WEB) {
        return "WEB";
    }

    return "No ESP";
}
