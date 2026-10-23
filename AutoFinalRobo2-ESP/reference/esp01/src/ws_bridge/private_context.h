#pragma once
// Private types and singleton state for this cohesive JSON bridge.
#include "../ws_json_bridge.h"

#include <ArduinoJson.h>
#include <math.h>
#include <pgmspace.h>
#include <string.h>

#include "../config.h"
#include "../boot_mode.h"
#include "../f4_command_registry.h"
#include "../f4_extension_profiles.h"
#include "../f4_payload_decoders.h"
#include "../oled_canvas_transfer.h"
#include "../stream_broker.h"
#include "../stream_request_waiters.h"
#include "../uart_transaction_manager.h"
#include "../ws_session_manager.h"

static const size_t WS_JSON_MAX = 1024;
static const uint16_t STM_FRAME_MAX = UNER_MAX_FRAME;
static const uint8_t STM_UNER_OVERHEAD = UNER_OVERHEAD;
static const uint8_t WS_CLIENT_MAX = WS_MAX_SESSIONS;
static const uint32_t STM_TIMEOUT_MS = 700;
static const uint32_t JSON_SCAN_TIMEOUT_MS = 10000;
static const uint32_t JSON_SCAN_RECONNECT_GRACE_MS = 20000u;
static const uint32_t JSON_SCAN_MIN_FREE_HEAP = 9000u;
static const uint32_t JSON_SCAN_MIN_CONTIGUOUS_HEAP = 2048u;
static const uint8_t JSON_SCAN_MAX_RETURNED_NETWORKS = 8u;
static const uint32_t WIFI_CREDENTIALS_CONNECT_TIMEOUT_MS = 15000;
static const uint32_t AP_PROVISIONING_EXIT_DELAY_MS = 500;
static const uint32_t STREAM_REQUEST_TIMEOUT_MS = 5000u;
static const uint8_t STM_EVT_MENU_SELECTION_CHANGED = 0x96;
static const uint8_t STM_EVT_CAR_MODE_CHANGED = 0x97u;
static const uint8_t STM_EVT_QT_USB_STATE = 0x9Eu;
static const uint8_t STM_EVT_MCU_BOOT_REPORT = 0x9Fu;
static const uint32_t STM_SCREEN_CODE_DASHBOARD = 0x010101u;
static const uint8_t STM_AUTH_SCOPE_LOGIN = 0x00u;
static const uint8_t STM_AUTH_SCOPE_SCREEN = 0x01u;
static const uint8_t STM_AUTH_SCOPE_CHANGE = 0x02u;
static const uint8_t STM_CMD_RESET_MCU = 0x19u;
static const uint32_t AP_APPLY_FLUSH_DELAY_MS = 75u;
static const uint32_t WEB_REBOOT_FLUSH_DELAY_MS = 250u;
static const uint16_t F4_WEB_ALIVE_DEFAULT_PERIOD_MS = 1000u;
static const uint16_t F4_WEB_ALIVE_MIN_PERIOD_MS = 100u;
static const uint16_t F4_WEB_ALIVE_MAX_PERIOD_MS = 10000u;
static const uint8_t F4_WEB_ALIVE_MAX_MISSES = 10u;
static const uint32_t F4_WEB_ALIVE_MARGIN_MS = 100u;
static const uint32_t F4_BACKEND_OFFLINE_CONFIRM_MS = 30000u;
static const uint32_t STM_PREF_FIELD_WEB_ALIVE_PERIOD = (1UL << 13);
static const uint32_t OLED_CANVAS_UPLOAD_TIMEOUT_MS = 5000u;
static const uint32_t OLED_CANVAS_UART_TOTAL_TIMEOUT_MS = 15000u;
static const uint32_t OLED_CANVAS_CANCEL_TIMEOUT_MS = 2000u;
static const uint32_t OLED_CANVAS_UART_PHASE_TIMEOUT_MS = 1000u;
static const uint32_t OLED_CANVAS_INTERNAL_CLIENT_ID = 0xFFFFFFFFUL;
static const uint8_t STM_TEST_MODE = 2u;
static const uint32_t WS_RECONNECT_GRACE_MS = 20000u;
static const uint32_t AP_CLIENT_DEAUTH_FLUSH_MS = 200u;

static const uint8_t STM_CMD_MENU_ITEM_CLICK = 0x53u;
static const uint8_t STM_CMD_TRIGGER_ENCODER_BUTTON = 0x54u;
static const uint8_t STM_CMD_TRIGGER_USER_BUTTON = 0x55u;
static const uint8_t STM_CMD_REQUEST_SCREEN_PAGE = 0x56u;
static const uint8_t STM_CMD_TRIGGER_ENCODER_ROTATE_LEFT = 0x57u;
static const uint8_t STM_CMD_TRIGGER_ENCODER_ROTATE_RIGHT = 0x58u;
static const uint8_t STM_CMD_GET_CAR_MODE = 0x5Bu;
static const uint8_t STM_CMD_SET_CAR_MODE = 0x6Eu;
static const uint8_t STM_CMD_AUTH_REMOTE_RESULT = 0x5Du;
static const uint8_t STM_CMD_SET_I2C_POLICY = 0x5Eu;
static const uint8_t STM_CMD_GET_I2C_POLICY = 0x5Fu;
static const uint8_t STM_CMD_GET_MPU_SNAPSHOT = 0x60u;
static const uint8_t STM_CMD_SET_MPU_STREAM = 0x61u;
static const uint8_t STM_CMD_STOP_MPU_STREAM = 0x62u;

static const uint8_t STM_SCREEN_PAGE_DIR_UP = 0x00u;
static const uint8_t STM_SCREEN_PAGE_DIR_DOWN = 0x01u;
static const uint8_t STM_PRESS_KIND_SHORT = 0x00u;
static const uint8_t STM_PRESS_KIND_LONG = 0x01u;

#ifndef UNER_BRIDGE_DEBUG
#define UNER_BRIDGE_DEBUG 0
#endif

#ifndef WS_JSON_SCAN_DEBUG
#define WS_JSON_SCAN_DEBUG 0
#endif

#if WS_JSON_SCAN_DEBUG
#define WS_SCAN_LOG(...) Serial.printf(__VA_ARGS__)
#else
#define WS_SCAN_LOG(...)
#endif

struct WsClientState
{
    uint8_t used;
    uint32_t id;
    uint32_t generation;
    uint8_t preferred_api_v1;
    uint8_t dropping;
    uint32_t backpressure_drops;
    size_t rx_len;
    char rx[WS_JSON_MAX + 1];
};

struct TimedTelemetryStop
{
    uint8_t active;
    uint32_t client_id;
    char request_id[65];
    uint32_t deadline_ms;
};

struct PendingWifiCredentialsWeb
{
    uint8_t session_only;
    uint8_t requested;
    uint8_t connecting;
    uint8_t notify_stm;
    uint8_t previous_auto_reconnect;
    uint8_t has_bssid;
    uint8_t ap_provisioning;
    uint32_t client_id;
    uint32_t generation;
    char request_id[65];
    uint8_t bssid[6];
    AppConfig previous_config;
    AppConfig candidate_config;
    uint32_t started_ms;
    uint32_t deadline_ms;
};

struct PendingApCredentialsWeb
{
    uint8_t session_only;
    uint8_t apply_radio;
    uint8_t active;
    uint8_t keep_sta;
    uint32_t client_id;
    uint32_t generation;
    uint32_t apply_at_ms;
    char request_id[65];
    char command[65];
    AppConfig previous_config;
    AppConfig candidate_config;
    WifiIpv4 previous_ip;
    WifiIpv4 candidate_ip;
};

struct ApCredentialsResult
{
    uint8_t available;
    uint8_t success;
    uint32_t completed_ms;
    char request_id[65];
    char ssid[SSID_MAX_LEN + 1];
    char status[16];
    char reason[24];
    char configured_ip[16];
};

struct AutoScreenRefresh
{
    uint8_t active;
    uint8_t reason_cmd;
    uint32_t due_ms;
    char request_id[65];
};

struct PendingApProvisioningExit
{
    uint8_t active;
    uint32_t stop_at_ms;
};

struct PendingApClientDisconnect
{
    uint8_t active;
    uint8_t mac[6];
    uint32_t execute_at_ms;
};

enum OledCanvasBridgePhase
{
    OLED_CANVAS_BRIDGE_IDLE = 0u,
    OLED_CANVAS_BRIDGE_UPLOADING,
    OLED_CANVAS_BRIDGE_WAITING_UART,
    OLED_CANVAS_BRIDGE_UART,
    OLED_CANVAS_BRIDGE_CANCEL_UART,
};

struct OledCanvasBridge
{
    OledCanvasAssembly assembly;
    uint8_t phase;
    uint8_t api_v1;
    uint8_t expected_operation;
    uint8_t uart_inflight;
    uint8_t begin_confirmed;
    uint8_t cancel_requested;
    uint8_t owner_connected;
    uint8_t commit_resolved;
    uint32_t client_id;
    uint32_t generation;
    uint8_t node;
    uint16_t uart_next_offset;
    uint16_t expected_next_offset;
    uint32_t upload_deadline_ms;
    uint32_t total_deadline_ms;
    char commit_request_id[65];
    char cancel_request_id[65];
};

enum BridgeCommandRoute
{
    BRIDGE_ROUTE_LOCAL_ESP,
    BRIDGE_ROUTE_FORWARD_STM,
};

typedef bool (*BridgeParamsValidatorFn)(JsonObject root_payload, JsonObject &params, const char **message);
typedef void (*BridgeCommandExecFn)(uint32_t client_id, const char *request_id, const char *command, JsonObject params);

struct BridgeCommandSpec
{
    const char *json_name;
    const char *default_target;
    BridgeCommandRoute route;
    BridgeParamsValidatorFn validate_params;
    BridgeCommandExecFn execute;
    const char *decoder;
};

static bool validate_ap_client_disconnect_params(JsonObject root_payload,
                                                 JsonObject &params,
                                                 const char **message);

struct DebugWifiScope
{
    uint8_t active;
    uint8_t owner_node;
    uint8_t previous_reconnect;
    uint8_t mode_pending;
    uint8_t radio_dirty;
    WifiRadioMode previous_mode;
    uint32_t mode_apply_at_ms;
    AppConfig config;
};

static bool debug_wifi_begin(WsmSession *session);
static void debug_wifi_release();
static void service_debug_wifi_scope();

struct WsJsonBridgeContext {
    AsyncWebSocket *ws = nullptr;
    void *app = nullptr;
    BridgeBackendOps backend = Bridge_DefaultBackend();
    BridgeWifiPort wifi = Bridge_DefaultWifi();
    BridgePlatformOps platform = BridgePlatform_ESP();
    WsClientState clients[WS_CLIENT_MAX]{};
    UartTransactionManager transactions{};
    WsSessionManager sessions{};
    StreamBroker streams{};
    StreamRequestWaiters stream_waiters{};
    uint8_t logout_enqueued[WSM_MAX_SESSIONS]{};
    uint32_t logout_retry_at_ms[WSM_MAX_SESSIONS]{};
    uint32_t cleanup_release_at_ms[WSM_MAX_SESSIONS]{};
    uint8_t auth_request_id = 1u;
    TimedTelemetryStop telem_stop{};
    DebugWifiScope debug_wifi{};
    PendingWifiCredentialsWeb wifi_credentials_web{};
    PendingApCredentialsWeb ap_credentials_web{};
    ApCredentialsResult ap_credentials_result{};
    AutoScreenRefresh auto_screen_refresh{};
    PendingApProvisioningExit ap_provisioning_exit{};
    PendingApClientDisconnect ap_client_disconnect{};
    OledCanvasBridge oled_canvas{};
    uint16_t oled_canvas_next_transfer_id = 0u;
    uint8_t json_scan_active = 0;
    uint8_t json_scan_cleanup_pending = 0;
    uint32_t json_scan_started_ms = 0;
    uint32_t json_scan_last_log_ms = 0;
    uint32_t json_scan_client_id = 0;
    uint32_t json_scan_generation = 0;
    uint32_t json_scan_origin_ip_key = 0;
    uint32_t json_scan_reconnect_until_ms = 0;
    uint8_t json_scan_origin_api_v1 = 0;
    uint8_t json_scan_previous_auto_reconnect = 0;
    char json_scan_request_id[65]{};
    WifiScanSnapshot web_scan_cache{};
    uint8_t json_scan_result_pending = 0;
    uint8_t json_scan_result_timed_out = 0;
    uint8_t json_scan_result_failed = 0;
    uint8_t json_scan_result_count = 0;
    uint8_t json_scan_result_api_v1 = 0;
    uint32_t json_scan_result_ip_key = 0;
    uint32_t json_scan_result_expires_ms = 0;
    char json_scan_result_request_id[65]{};
    uint8_t last_wifi_mode = 0xFF;
    uint8_t last_sta_status = 0xFF;
    uint8_t last_ap_clients = 0xFF;
    uint32_t ws_backpressure_drops = 0u;
    uint32_t api_v1_requests = 0u;
    uint32_t legacy_envelopes = 0u;
    uint32_t legacy_raw_requests = 0u;
    uint32_t last_f4_alive_ms = 0u;
    uint32_t f4_offline_since_ms = 0u;
    uint8_t f4_alive_seen = 0u;
    uint8_t f4_backend_online = 0u;
    uint16_t f4_alive_period_ms = F4_WEB_ALIVE_DEFAULT_PERIOD_MS;
};
static WsJsonBridgeContext s_bridge;

typedef WifiScanSnapshotEntry UNER_WifiScanCacheEntry;

static const F4_CommandSpec *lookup_f4_command(uint8_t id, F4_CommandSpec &storage)
{
    return F4_CommandRegistry_Lookup(id, &storage) ? &storage : nullptr;
}

static const uint32_t STM_SESSION_MAX_TTL_MS = 60000u;
static const uint32_t STM_LOGOUT_RETRY_MS = 1000u;
static const uint32_t STM_CLEANUP_QUARANTINE_MS = STM_SESSION_MAX_TTL_MS + 1000u;

