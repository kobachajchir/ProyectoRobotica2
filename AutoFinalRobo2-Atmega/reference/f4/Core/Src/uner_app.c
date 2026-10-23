/*
 * uner_app.c - compact UNER v2 application wiring for the ESP WEB backend.
 */
#include "uner_app.h"

#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "automicro_boot_contract.h"
#include "app_preferences_binding.h"
#include "app_ap_config.h"
#include "app_tasks.h"
#include "balance_control.h"
#include "globals.h"
#include "i2c_manager.h"
#include "line_follow_control.h"
#include "main.h"
#include "app_menu_binding.h"
#include "motion_control.h"
#include "motion_estimator.h"
#include "motor_control.h"
#include "mpu9250.h"
#include "oled_utils.h"
#include "oled_handle.h"
#include "oled_canvas.h"
#include "permissions.h"
#include "screenWrappers.h"
#include "ui_event_router.h"
#include "uner_handle.h"
#include "uner_transport_uart1_dma.h"
#include "uner_transport_usbcdc.h"
#include "usbd_cdc_if.h"
#include "uner_v2.h"
#include "app_wifi_binding.h"

#define UNER_QUEUE_SLOTS                       6u
#define WIFI_SCAN_RESULTS_POLL_RUNNING_MS    750u
#define WIFI_SCAN_RESULTS_POLL_PENDING_MS    100u
#define WIFI_SCAN_RESULTS_POLL_RETRY_MS      250u
#define MPU_STREAM_DEFAULT_PERIOD_MS         100u
#define MPU_STREAM_MIN_PERIOD_MS              20u
#define MPU_STREAM_MAX_PERIOD_MS            1000u
#define MPU_SNAPSHOT_PAYLOAD_SIZE             42u
#define MPU_SNAPSHOT_FLAG_SAMPLE_VALID   (1u << 0)
#define MPU_SNAPSHOT_FLAG_CALIBRATED     (1u << 1)
#define MPU_SNAPSHOT_FLAG_MAG_VALID      (1u << 2)
#define MPU_SNAPSHOT_FLAG_STATIONARY     (1u << 3)
#define MPU_SNAPSHOT_FLAG_CALIBRATING    (1u << 4)
#define MPU_SNAPSHOT_FLAG_ERROR          (1u << 5)
#define IR_STREAM_DEFAULT_PERIOD_MS          100u
#define IR_STREAM_MIN_PERIOD_MS               20u
#define IR_STREAM_MAX_PERIOD_MS             1000u
#define IR_SNAPSHOT_PAYLOAD_SIZE              56u
#define IR_STREAM_FLAG_LIGHTS_ON        (1u << 0)
#define IR_STREAM_FLAG_FOLLOW_MODE      (1u << 1)
#define IR_STREAM_FLAG_GROUP5_ON         (1u << 2)
#define IR_STREAM_FLAG_GROUP3_ON         (1u << 3)
#define IR_STREAM_FLAG_ROUTE_VALID       (1u << 4)
#define UNER_PREF_RESPONSE_MAX_BYTES         128u
#define UNER_TELEMETRY_PAYLOAD_SIZE          112u
#define UNER_CONTROL_SNAPSHOT_PAYLOAD_SIZE    64u
#define UNER_TEXT_FIELD_MAX_BYTES             64u
#define UNER_BUILD_SHORT_FIELD_MAX_BYTES       16u
#define UNER_COMPILER_FIELD_MAX_BYTES         96u
#define UNER_NACK_STATUS_UNSUPPORTED_TRANSPORT 5u
#define UNER_NACK_STATUS_INVALID_ARGUMENT      2u
#define UNER_NACK_STATUS_DEPRECATED_COMMAND    6u
#define UNER_NACK_STATUS_AUTH_REQUIRED         7u
#define UNER_NACK_STATUS_LOCAL_PIN_ACTIVE      8u
#define UNER_NACK_STATUS_INVALID_SOURCE         9u
#define UNER_NACK_STATUS_BUSY                   1u
#define UNER_NACK_STATUS_TIMEOUT               10u
#define UNER_RESET_MCU_ACK_DELAY_MS            75u
#define UNER_AUTH_STATUS_OK                    0u
#define UNER_AUTH_STATUS_BAD_PAYLOAD           1u
#define UNER_AUTH_STATUS_UNSUPPORTED_SCOPE     2u
#define UNER_AUTH_STATUS_SCREEN_MISMATCH       3u
#define UNER_AUTH_STATUS_NO_PENDING_PERMISSION 4u
#define UNER_AUTH_STATUS_STORAGE_FAILURE       5u
#define UNER_AUTH_RESPONSE_SIZE                9u
#define UNER_AUTH_SESSION_RESPONSE_SIZE        6u
#define UNER_AUTH_REPLY_QUEUE_SLOTS UNER_QUEUE_SLOTS
#define UNER_AUTH_REPLY_MAX_PAYLOAD             9u

typedef enum {
    UNER_CMD_ACK = 0xE0u,
    UNER_CMD_NACK = 0xE1u,

    UNER_EVT_BOOT = 0x80u,
    UNER_EVT_MODE_CHANGED = 0x81u,
    UNER_EVT_STA_CONNECTED = 0x82u,
    UNER_EVT_STA_DISCONNECTED = 0x83u,
    UNER_EVT_AP_CLIENT_JOIN = 0x84u,
    UNER_EVT_AP_CLIENT_LEAVE = 0x85u,
    UNER_EVT_APP_USER_CONNECTED = 0x86u,
    UNER_EVT_APP_USER_DISCONNECTED = 0x87u,
    UNER_EVT_WEBSERVER_UP = 0x88u,
    UNER_EVT_WEBSERVER_CLIENT_CONNECTED = 0x89u,
    UNER_EVT_WEBSERVER_CLIENT_DISCONNECTED = 0x8Au,
    UNER_EVT_LASTWIFI_NOTFOUND = 0x8Bu,
    UNER_EVT_CONTROLLER_CONNECTED = 0x8Cu,
    UNER_EVT_CONTROLLER_DISCONNECTED = 0x8Du,
    UNER_EVT_USB_CONNECTED = 0x8Eu,
    UNER_EVT_USB_DISCONNECTED = 0x8Fu,
    UNER_EVT_WIFI_CONNECTED = 0x92u,
    UNER_EVT_NETWORK_IP = 0x93u,
    UNER_EVT_BOOT_COMPLETE = 0x94u,
    UNER_EVT_SCREEN_CHANGED = WIFI_BRIDGE_EVT_ID_SCREEN_CHANGED,
    UNER_EVT_MENU_SELECTION_CHANGED = WIFI_BRIDGE_EVT_ID_MENU_SELECTION_CHANGED,
    UNER_EVT_QT_USB_STATE = 0x9Eu,
    UNER_EVT_MCU_BOOT_REPORT = WIFI_BRIDGE_EVT_ID_MCU_BOOT_REPORT,
} UNER_CommandId;

typedef enum {
    WIFI_MODE_OFF    = 0x00u,
    WIFI_MODE_STA    = 0x01u,
    WIFI_MODE_AP     = 0x02u,
    WIFI_MODE_AP_STA = 0x03u
} UNER_WifiModeEsp;

static WifiBridge_Handle *uner_bridge;
static UNER_Handle uner_handle;
static UNER_TransportUart1Dma uner_uart1;
static UNER_TransportUsbCdc uner_usbcdc;
static UNER_Packet uner_slots[UNER_QUEUE_SLOTS];
static uint8_t uner_payload_pool[UNER_QUEUE_SLOTS * 255u];
static volatile uint8_t uner_uart1_rx_hint;
static volatile uint8_t uner_waiting_validation;
static volatile uint8_t uner_wait_cmd_id;
static uint8_t uner_wait_is_startup_reboot;
static volatile uint32_t uner_wait_started_at;
static volatile uint16_t uner_wait_timeout_ms;

typedef struct {
    uint8_t active;
    uint8_t origin_transport_id;
    uint8_t origin_node;
    uint8_t cmd;
    uint8_t flags;
    uint8_t ack_seen;
} UNER_EspProxyState;

static UNER_EspProxyState uner_esp_proxy;
static UNER_Packet uner_at_remote_request;
static uint8_t uner_at_remote_bool_value = 0u;
static uint8_t uner_at_remote_pending;
static uint32_t uner_at_remote_deadline;
static uint8_t uner_usb_scan_owned;
static uint8_t reset_mcu_pending;
static uint32_t reset_mcu_requested_at;
static uint8_t reset_mcu_transport_id;

static uint8_t binary_asset_buffer[UNER_BINARY_ASSET_MAX_BYTES];
static uint8_t binary_asset_id;
static uint8_t binary_asset_width;
static uint8_t binary_asset_height;
static uint16_t binary_asset_total_len;
static uint16_t binary_asset_received_len;
static uint8_t binary_asset_ready;
static uint8_t binary_asset_in_progress;
static uint32_t wifi_scan_results_next_poll_at;

/* Eight routed Web nodes plus Qt USB and Qt over the AT UART backend. */
#define UNER_STREAM_SUBSCRIBER_MAX 10u
typedef struct {
    uint8_t active;
    uint8_t transport_id;
    uint8_t dst_node;
    uint16_t period_ms;
    uint16_t last_seq;
    uint32_t next_due_ms;
} UNER_StreamState;
static UNER_StreamState mpu_streams[UNER_STREAM_SUBSCRIBER_MAX];
static UNER_StreamState ir_streams[UNER_STREAM_SUBSCRIBER_MAX];
static uint16_t ir_snapshot_seq;

static UNER_StreamState *UNER_App_FindStream(UNER_StreamState *streams,
                                             uint8_t transport_id,
                                             uint8_t node, uint8_t allocate)
{
    UNER_StreamState *free_slot = NULL;
    for (uint8_t i = 0u; i < UNER_STREAM_SUBSCRIBER_MAX; ++i) {
        UNER_StreamState *stream = &streams[i];
        if (stream->active && stream->transport_id == transport_id &&
            stream->dst_node == node) return stream;
        if (!stream->active && free_slot == NULL) free_slot = stream;
    }
    return allocate ? free_slot : NULL;
}

static void UNER_App_StopStreamEndpoint(UNER_StreamState *streams,
                                        uint8_t transport_id, uint8_t node)
{
    UNER_StreamState *stream = UNER_App_FindStream(streams, transport_id, node, 0u);
    if (stream != NULL) memset(stream, 0, sizeof(*stream));
}

static void UNER_App_StopTransportStreams(uint8_t transport_id)
{
    for (uint8_t i = 0u; i < UNER_STREAM_SUBSCRIBER_MAX; ++i) {
        if (mpu_streams[i].active && mpu_streams[i].transport_id == transport_id)
            memset(&mpu_streams[i], 0, sizeof(mpu_streams[i]));
        if (ir_streams[i].active && ir_streams[i].transport_id == transport_id)
            memset(&ir_streams[i], 0, sizeof(ir_streams[i]));
    }
}

static uint16_t UNER_App_MinMpuStreamPeriod(void)
{
    uint16_t minimum = MPU_STREAM_MAX_PERIOD_MS;
    uint8_t active = 0u;
    for (uint8_t i = 0u; i < UNER_STREAM_SUBSCRIBER_MAX; ++i) {
        if (mpu_streams[i].active) {
            active = 1u;
            if (mpu_streams[i].period_ms < minimum) minimum = mpu_streams[i].period_ms;
        }
    }
    return active ? minimum : MPU_STREAM_DEFAULT_PERIOD_MS;
}

static uint8_t UNER_App_AnyMpuStreamActive(void)
{
    for (uint8_t i = 0u; i < UNER_STREAM_SUBSCRIBER_MAX; ++i)
        if (mpu_streams[i].active) return 1u;
    return 0u;
}

typedef struct {
    uint8_t transport_id;
    uint8_t dst_node;
    uint8_t cmd;
    uint8_t len;
    uint8_t payload[UNER_AUTH_REPLY_MAX_PAYLOAD];
} UNER_AuthReply;

typedef struct {
    UNER_AuthReply slots[UNER_AUTH_REPLY_QUEUE_SLOTS];
    uint8_t head;
    uint8_t tail;
    uint8_t count;
} UNER_AuthReplyQueue;

static UNER_AuthReplyQueue uner_auth_reply_queue;
static OledSimpleNotificationId uner_deferred_auth_notification;
static uint8_t uner_usb_qt_state_pending;
static uint8_t uner_usb_qt_state_value;

#define UNER_BOOT_REPORT_SENT_UART  (1u << 0)
#define UNER_BOOT_REPORT_SENT_NRF   (1u << 1)
#define UNER_BOOT_REPORT_SENT_USB   (1u << 2)

static AutoMicroBootMailbox uner_boot_mailbox;
static uint8_t uner_boot_mailbox_valid;
static uint8_t uner_boot_report_sent_mask;
static uint8_t uner_nrf_link_connected;

static void UNER_App_ExecuteCommand(void *ctx, const UNER_Packet *packet);
static void UNER_App_OnAnyPacket(void *ctx, const UNER_Packet *packet);
static uint8_t UNER_App_SendOledCanvasResponse(uint8_t transport_id,
                                               uint8_t destination_node,
                                              const uint8_t *payload,
                                               uint8_t len,
                                               void *ctx);
static void UNER_App_ParseFirmwareResponse(const UNER_Packet *packet);
static void UNER_App_ParseStatusResponse(const UNER_Packet *packet);
static void UNER_App_ParseScanResultsResponse(const UNER_Packet *packet);
static void UNER_App_HandleGetStatusLocal(const UNER_Packet *packet);
static void UNER_App_ParseWifiFinalizer(const UNER_Packet *packet);
static void UNER_App_ParseNetworkDetail(const UNER_Packet *packet);
static void UNER_App_ParseWifiCredentialsWebResult(const UNER_Packet *packet);
static void UNER_App_ParseBinaryAssetStream(const UNER_Packet *packet);
static void UNER_App_ServiceWifiScanResults(void);
static void UNER_App_ServiceMpuStream(void);
static void UNER_App_ServiceIrStream(void);
static void UNER_App_StopUnauthorizedRemoteStreams(void);
static bool UNER_App_QueueAuthReply(uint8_t transport_id,
                                   uint8_t dst_node,
                                   uint8_t cmd,
                                  const uint8_t *payload,
                                   uint8_t len);
static void UNER_App_ServiceAuthReplyQueue(void);
static void UNER_App_ServiceUsbQtState(void);
static void UNER_App_RequestAuthNotification(OledSimpleNotificationId notification_id);
static void UNER_App_ServiceDeferredAuthNotification(void);
static void UNER_App_ServiceDeferredMcuReset(void);
static void UNER_App_UpdateModeFlags(uint8_t mode);
static void UNER_App_StoreWifiInfoSsid(const uint8_t *payload, uint8_t len);
static uint8_t UNER_App_ReadEspBootResetReason(const UNER_Packet *packet);
static void UNER_App_StoreTargetCredentialsSsid(const uint8_t *payload, uint8_t len);
static void UNER_App_StoreNetworkIpPayload(const uint8_t *payload, uint8_t len);
static void UNER_App_SetStaIp(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
static void UNER_App_SetApIp(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4);
static void UNER_App_SetOledSsidFromBytes(const uint8_t *ssid, uint8_t len);
static void UNER_App_SetWifiNetworkEncryptionText(uint8_t index, uint8_t encryption_type);
static int8_t UNER_App_FindWifiNetworkIndexBySsid(const uint8_t *ssid, uint8_t len);
static uint8_t UNER_App_IsStaActive(void);
static void UNER_App_MarkStaActive(void);
static void UNER_App_ShowWifiConnectedNotification(uint8_t was_wifi_active);
static void UNER_App_RequestNetworkUiRefresh(void);
static void UNER_App_WriteScreenReportPayload(uint8_t *payload, uint32_t screen_code, uint8_t source);
static void UNER_App_WriteMenuSelectionReportPayload(uint8_t *payload,
                                                     uint32_t screen_code,
                                                     uint8_t selected_index,
                                                     uint8_t item_count,
                                                     uint8_t source);
static void UNER_App_SendCommandStatus(const UNER_Packet *packet, uint8_t status);
static void UNER_App_SendCommandNack(const UNER_Packet *packet, uint8_t status);
static UNER_Status UNER_App_Reply(const UNER_Packet *packet,
                                  uint8_t command_id,
                                  const uint8_t *payload,
                                  uint8_t length);

static uint8_t UNER_App_PacketScreenMatchesCurrent(const UNER_Packet *packet);
static uint8_t UNER_App_DispatchRemoteUiEvent(UserEvent_t event);
static uint8_t UNER_App_CommandTransportAllowed(uint8_t cmd, uint8_t transport_id);
static uint16_t UNER_App_ReadU16Le(const uint8_t *payload);
static uint32_t UNER_App_ReadU32Le(const uint8_t *payload);
static float UNER_App_ReadFloatLe(const uint8_t *payload);
static void UNER_App_WriteU16Le(uint8_t *payload, uint16_t value);
static void UNER_App_WriteU32Le(uint8_t *payload, uint32_t value);
static void UNER_App_WriteFloatLe(uint8_t *payload, float value);
static void UNER_App_ResetBinaryAssetState(uint8_t asset_id);
static void UNER_App_ShowNotification(RenderFunction renderFn, uint16_t timeout_ms);
static void UNER_App_ShowSimpleNotification(OledSimpleNotificationId notificationId, uint16_t timeout_ms);
static void UNER_App_ShowConnectivityNotification(RenderFunction renderFn, uint16_t timeout_ms);
static void UNER_App_ShowConnectivitySimpleNotification(OledSimpleNotificationId notificationId,
                                                        uint16_t timeout_ms);
static uint8_t UNER_App_IsConnectivityMutationCommand(uint8_t cmd);
static void UNER_App_HandleGetPreferences(const UNER_Packet *packet);
static void UNER_App_HandleSetPreferences(const UNER_Packet *packet);
static void UNER_App_HandleGetApConfig(const UNER_Packet *packet);
static void UNER_App_HandleUpdateApConfig(const UNER_Packet *packet);
static void UNER_App_HandleSetI2CPolicy(const UNER_Packet *packet);
static void UNER_App_HandleGetI2CPolicy(const UNER_Packet *packet);
static void UNER_App_HandleGetMpuSnapshot(const UNER_Packet *packet);
static void UNER_App_HandleSetMpuStream(const UNER_Packet *packet);
static void UNER_App_HandleStopMpuStream(const UNER_Packet *packet);
static void UNER_App_HandleGetIrSnapshot(const UNER_Packet *packet);
static void UNER_App_HandleSetIrStream(const UNER_Packet *packet);
static void UNER_App_HandleStopIrStream(const UNER_Packet *packet);
static void UNER_App_HandleGetTelemetrySnapshot(const UNER_Packet *packet);
static void UNER_App_HandleGetControlSnapshot(const UNER_Packet *packet);
static void UNER_App_HandleRequestFirmwareCompat(const UNER_Packet *packet);
static void UNER_App_HandleGetFirmwareInfo(const UNER_Packet *packet);
static void UNER_App_HandleGetBuildInfo(const UNER_Packet *packet);
static void UNER_App_HandleGetBalancePid(const UNER_Packet *packet);
static void UNER_App_HandleSetBalancePid(const UNER_Packet *packet);
static void UNER_App_HandleGetBootReport(const UNER_Packet *packet);
static uint8_t UNER_App_BuildBootReportPayload(
    uint8_t payload[WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE]);
static UNER_Status UNER_App_SendBootReport(uint8_t transport_id,
                                          uint8_t destination_node);
static void UNER_App_ServiceBootReport(void);
static void UNER_App_HandleAuthValidatePin(const UNER_Packet *packet);
static void UNER_App_HandleAuthSession(const UNER_Packet *packet);
static void UNER_App_HandleResetMcu(const UNER_Packet *packet);
static void UNER_App_HandleGetExtensionProfile(const UNER_Packet *packet);
static void UNER_App_HandleSetExtensionProfile(const UNER_Packet *packet);
static void UNER_App_HandleRebootIntoProfile(const UNER_Packet *packet);
static uint8_t UNER_App_IsEspEvent(uint8_t cmd);
static uint8_t UNER_App_IsEspControlPlaneCommand(uint8_t cmd);
static uint8_t UNER_App_IsEspProxyCommand(uint8_t cmd);
static uint8_t UNER_App_RelayEspPacketToUsb(const UNER_Packet *packet);
static uint8_t UNER_App_ForwardUsbCommandToEsp(const UNER_Packet *packet);
static void UNER_App_ClearEspProxy(void);
static uint8_t UNER_App_IsPublicRemoteCommand(uint8_t cmd);
static uint8_t UNER_App_AllowForward(void *ctx,
                                     const UNER_Packet *packet,
                                     uint8_t out_transport_id);
static uint8_t UNER_App_IsRemoteUiCommand(uint8_t cmd);
static uint8_t UNER_App_BuildMpuSnapshotPayload(uint8_t *payload, MPU9250_Snapshot_t *snapshot_out);
static uint8_t UNER_App_MpuStreamTransportAllowed(uint8_t transport_id);
static uint8_t UNER_App_BuildIrSnapshotPayload(uint8_t *payload,
                                               uint16_t sample_seq,
                                               uint16_t period_ms,
                                               uint32_t tick_ms);
static uint8_t UNER_App_IrStreamTransportAllowed(uint8_t transport_id);
static uint16_t UNER_App_NormalizeIrRaw(uint8_t sensor_index, uint16_t raw);
static void UNER_App_HandleMotion(const UNER_Packet *packet);
static uint8_t UNER_App_IsPidValueValid(float value);
static uint8_t UNER_App_AppendTextField(uint8_t *payload,
                                        uint8_t payload_len,
                                        uint8_t *offset,
                                        const char *text,
                                        uint8_t max_text_len);

static uint8_t line_cal_owner_transport, line_cal_owner_node;
static uint32_t line_cal_owner_session;
static const UNER_CommandSpec uner_commands[] = {
    { 0x2Bu, 0u, 0u, UNER_SPEC_F_RESP, 500u, NULL },
    { 0x23u, 0u, 0u, UNER_SPEC_F_RESP, 500u, NULL },
    { 0x24u, 8u, 8u, UNER_SPEC_F_RESP, 500u, NULL },
    { 0x25u, 0u, 0u, UNER_SPEC_F_RESP, 500u, NULL },
    { 0x26u, 186u, 186u, UNER_SPEC_F_RESP, 500u, NULL },
    { 0x27u, 0u, 0u, UNER_SPEC_F_RESP, 500u, NULL },
    { 0x28u, 13u, 13u, UNER_SPEC_F_RESP, 500u, NULL },
    { 0x29u, 0u, 0u, UNER_SPEC_F_RESP, 500u, NULL },
    { 0x2au, 88u, 88u, UNER_SPEC_F_RESP, 500u, NULL },
    { UNER_CMD_ID_LINE_CALIBRATION_CONTROL, 5u, 5u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_GET_LINE_STATE, 0u, 0u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_LINE_FOLLOW, 1u, 1u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_MODE_AP, 0u, 0u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 300u, NULL },
    { UNER_CMD_ID_SET_MODE_STA, 0u, 0u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 300u, NULL },
    { UNER_CMD_ID_SET_CREDENTIALS, 0u, 255u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 300u, NULL },
    { UNER_CMD_ID_CLEAR_CREDENTIALS, 0u, 0u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_START_SCAN, 0u, 0u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 200u, NULL },
    { UNER_CMD_ID_GET_SCAN_RESULTS, 0u, 255u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_REBOOT_ESP, 1u, 1u, UNER_SPEC_F_ACK, 50u, NULL },
    { UNER_CMD_ID_FACTORY_RESET, 0u, 0u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 200u, NULL },
    { UNER_CMD_ID_STOP_SCAN, 0u, 0u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_RESET_MCU, 0u, 0u, UNER_SPEC_F_ACK, 100u, NULL },
    { UNER_CMD_ID_GET_EXTENSION_PROFILE, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_SET_EXTENSION_PROFILE, 1u, 1u, UNER_SPEC_F_ACK, 50u, NULL },
    { UNER_CMD_ID_REBOOT_INTO_PROFILE, 1u, 1u, UNER_SPEC_F_ACK, 50u, NULL },
    { UNER_CMD_ID_GET_AP_FALLBACK, 0u, 0u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_GET_IR_EMITTERS, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_SET_IR_EMITTERS, 3u, 3u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_GET_MOTION_STATE, 0u, 0u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_BALANCE_ENABLED, 1u, 1u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_MOTION, 10u, 10u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_STOP_MOTION, 1u, 1u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_MOTOR_TEST, 10u, 10u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_WIFI_GET_DETAIL, 1u, (WIFI_SSID_MAX_LEN + 4u), UNER_SPEC_F_RESP, 150u, NULL },
    { UNER_CMD_ID_GET_STATUS, 0u, 255u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_PING, 0u, 16u, UNER_SPEC_F_RESP, 0u, NULL },
    { UNER_CMD_ID_GET_PREFERENCES, 0u, 1u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_PREFERENCES, 4u, 64u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 250u, NULL },
    { UNER_CMD_ID_GET_AP_CONFIG, 0u, 0u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_AP_CONFIG_DESIRED, 1u, 255u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 250u, NULL },
    { UNER_CMD_ID_REQUEST_FIRMWARE, 0u, 64u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 200u, NULL },
    { UNER_CMD_ID_ECHO, 0u, 255u, 0u, 0u, NULL },
    { UNER_CMD_ID_CONNECT_WIFI, 0u, 255u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 300u, NULL },
    { UNER_CMD_ID_DISCONNECT_WIFI, 0u, 255u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_AP_CONFIG, 1u, 255u, UNER_SPEC_F_RESP, 300u, NULL },
    { UNER_CMD_ID_START_AP, 0u, 255u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 300u, NULL },
    { UNER_CMD_ID_STOP_AP, 0u, 255u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_SET_AP_FALLBACK, 1u, 2u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 1000u, NULL },
    { UNER_CMD_ID_SET_AUTO_RECONNECT, 1u, 1u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_AUTH_VALIDATE_PIN, 6u, 10u, UNER_SPEC_F_RESP, 500u, NULL },
    { UNER_CMD_ID_GET_CURRENT_SCREEN, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_MENU_ITEM_CLICK, 4u, 5u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_TRIGGER_ENCODER_BUTTON, 4u, 5u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_TRIGGER_USER_BUTTON, 4u, 5u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_REQUEST_SCREEN_PAGE, 4u, 5u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_LEFT, 4u, 4u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_RIGHT, 4u, 4u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_AUTH_PIN_GRANTED, 4u, 4u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_BINARY_ASSET_STREAM, 1u, 255u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 1000u, NULL },
    { UNER_CMD_ID_GET_CAR_MODE, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_WIFI_CREDENTIALS_WEB_RESULT, 2u, 96u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 200u, NULL },
    { UNER_CMD_ID_AUTH_REMOTE_RESULT, 1u, 1u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_SET_I2C_POLICY, 1u, 1u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_GET_I2C_POLICY, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_GET_MPU_SNAPSHOT, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_SET_MPU_STREAM, 1u, 3u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_STOP_MPU_STREAM, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_GET_TELEMETRY_SNAPSHOT, 0u, 0u, UNER_SPEC_F_RESP, 80u, NULL },
    { UNER_CMD_ID_GET_BALANCE_PID, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_SET_BALANCE_PID, 5u, 13u, UNER_SPEC_F_ACK | UNER_SPEC_F_RESP, 80u, NULL },
    { UNER_CMD_ID_GET_CONTROL_SNAPSHOT, 0u, 0u, UNER_SPEC_F_RESP, 80u, NULL },
    { UNER_CMD_ID_GET_FIRMWARE_INFO, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_GET_BUILD_INFO, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_GET_BOOT_REPORT, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_GET_IR_SNAPSHOT, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_SET_IR_STREAM, 1u, 3u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_STOP_IR_STREAM, 0u, 0u, UNER_SPEC_F_RESP, 50u, NULL },
    { UNER_CMD_ID_OLED_CANVAS_TRANSFER, 3u, 118u, UNER_SPEC_F_RESP, 5000u, NULL },
    { WIFI_BRIDGE_CMD_ID_SET_CAR_MODE, 1u, 1u, UNER_SPEC_F_RESP, 100u, NULL },
    { UNER_CMD_ID_WIFI_CREDENTIALS_FLOW, 1u, 255u, UNER_SPEC_F_RESP, 500u, NULL },
    { UNER_CMD_ID_AP_CLIENTS_PAGE, 2u, 2u, UNER_SPEC_F_RESP, 250u, NULL },
    { UNER_CMD_ID_AP_CLIENT_DISCONNECT, 6u, 6u, UNER_SPEC_F_RESP, 250u, NULL },
    { UNER_CMD_ACK, 1u, 2u, 0u, 0u, NULL },
    { UNER_CMD_NACK, 1u, 2u, 0u, 0u, NULL },
    { UNER_EVT_BOOT, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_MODE_CHANGED, 0u, 255u, UNER_SPEC_F_EVT | UNER_SPEC_F_ACK, 50u, NULL },
    { UNER_EVT_STA_CONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_STA_DISCONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_AP_CLIENT_JOIN, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_AP_CLIENT_LEAVE, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_APP_USER_CONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_APP_USER_DISCONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_WEBSERVER_UP, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_WEBSERVER_CLIENT_CONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_WEBSERVER_CLIENT_DISCONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_LASTWIFI_NOTFOUND, 0u, 255u, UNER_SPEC_F_EVT | UNER_SPEC_F_ACK, 50u, NULL },
    { UNER_EVT_CONTROLLER_CONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_CONTROLLER_DISCONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_USB_CONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_USB_DISCONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_WIFI_CONNECTED, 0u, 255u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_NETWORK_IP, 5u, 5u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_BOOT_COMPLETE, 5u, 5u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_SCREEN_CHANGED, 5u, 9u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_MENU_SELECTION_CHANGED, 7u, 7u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_ID_MPU_STREAM, MPU_SNAPSHOT_PAYLOAD_SIZE, MPU_SNAPSHOT_PAYLOAD_SIZE, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_ID_IR_STREAM, IR_SNAPSHOT_PAYLOAD_SIZE, IR_SNAPSHOT_PAYLOAD_SIZE, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_QT_USB_STATE, 1u, 1u, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_MCU_BOOT_REPORT, WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE, WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE, UNER_SPEC_F_EVT, 0u, NULL },
    { UNER_EVT_ID_IR_EMITTERS_CHANGED, 3u, 3u, UNER_SPEC_F_EVT, 0u, NULL },
};

static void UNER_App_ClearEspProxy(void)
{
    memset(&uner_esp_proxy, 0, sizeof(uner_esp_proxy));
}

static void UNER_App_ClearWaitingValidation(void)
{
    uner_waiting_validation = 0u;
    uner_wait_cmd_id = 0u;
    uner_wait_started_at = 0u;
    uner_wait_timeout_ms = 0u;
    uner_wait_is_startup_reboot = 0u;
    UNER_App_ClearEspProxy();
}

static void UNER_App_ShowNotification(RenderFunction renderFn, uint16_t timeout_ms)
{
    OledUtils_ShowNotificationMs(renderFn, timeout_ms);
}

static void UNER_App_ShowSimpleNotification(OledSimpleNotificationId notificationId, uint16_t timeout_ms)
{
    OledUtils_ShowSimpleNotificationMs(notificationId, timeout_ms);
}

static void UNER_App_ShowConnectivityNotification(RenderFunction renderFn, uint16_t timeout_ms)
{
    Dashboard_InvalidateConnectivity();
    OledUtils_ShowNotificationMsEx(renderFn,
                                   timeout_ms,
                                   NULL,
                                   Dashboard_ReturnAfterConnectivityNotification);
}

static void UNER_App_ShowConnectivitySimpleNotification(OledSimpleNotificationId notificationId,
                                                        uint16_t timeout_ms)
{
    Dashboard_InvalidateConnectivity();
    OledUtils_ShowSimpleNotificationMsEx(notificationId,
                                         timeout_ms,
                                         NULL,
                                         Dashboard_ReturnAfterConnectivityNotification);
}

static void UNER_App_ShowClientNotification(OledSimpleNotificationId notificationId,
                                             uint8_t clientNumber,
                                             uint16_t timeout_ms)
{
    Dashboard_InvalidateConnectivity();
    OledUtils_ShowNumberedSimpleNotificationMsEx(notificationId, clientNumber, timeout_ms,
                                                  NULL, Dashboard_ReturnAfterConnectivityNotification);
}

static uint8_t UNER_App_IsConnectivityMutationCommand(uint8_t cmd)
{
    switch (cmd) {
        case UNER_CMD_ID_REBOOT_ESP:
        case UNER_CMD_ID_SET_CREDENTIALS:
        case UNER_CMD_ID_CONNECT_WIFI:
        case UNER_CMD_ID_DISCONNECT_WIFI:
        case UNER_CMD_ID_SET_AP_CONFIG:
        case UNER_CMD_ID_START_AP:
        case UNER_CMD_ID_STOP_AP:
        case UNER_CMD_ID_SET_AUTO_RECONNECT:
            return 1u;
        default:
            return 0u;
    }
}

static bool UNER_App_QueueAuthReply(uint8_t transport_id,
                                   uint8_t dst_node,
                                   uint8_t cmd,
                                  const uint8_t *payload,
                                   uint8_t len)
{
    UNER_AuthReply *reply;

    if (len > UNER_AUTH_REPLY_MAX_PAYLOAD ||
        (len > 0u && payload == NULL) ||
        uner_auth_reply_queue.count >= UNER_AUTH_REPLY_QUEUE_SLOTS) {
        return false;
    }

    reply = &uner_auth_reply_queue.slots[uner_auth_reply_queue.tail];
    reply->transport_id = transport_id;
    reply->dst_node = dst_node;
    reply->cmd = cmd;
    reply->len = len;
    if (len > 0u) {
        memcpy(reply->payload, payload, len);
    }
    uner_auth_reply_queue.tail =
        (uint8_t)((uner_auth_reply_queue.tail + 1u) % UNER_AUTH_REPLY_QUEUE_SLOTS);
    uner_auth_reply_queue.count++;
    return true;
}

static void UNER_App_ServiceAuthReplyQueue(void)
{
    while (uner_auth_reply_queue.count > 0u) {
        UNER_AuthReply *reply =
            &uner_auth_reply_queue.slots[uner_auth_reply_queue.head];
        UNER_Status status = UNER_Send(&uner_handle,
                                       reply->transport_id,
                                       UNER_NODE_MCU,
                                       reply->dst_node,
                                       reply->cmd,
                                       reply->payload,
                                       reply->len);

        if (status != UNER_OK) {
            return;
        }

        memset(reply, 0, sizeof(*reply));
        uner_auth_reply_queue.head =
            (uint8_t)((uner_auth_reply_queue.head + 1u) % UNER_AUTH_REPLY_QUEUE_SLOTS);
        uner_auth_reply_queue.count--;
    }
}

static void UNER_App_ServiceUsbQtState(void)
{
    uint8_t payload[1u];

    if (!uner_usb_qt_state_pending) {
        return;
    }

    payload[0] = uner_usb_qt_state_value;
    if (UNER_Send(&uner_handle,
                  UNER_TR_UART1_ESP,
                  UNER_NODE_MCU,
                  UNER_NODE_BROADCAST,
                  UNER_EVT_QT_USB_STATE,
                  payload,
                  (uint8_t)sizeof(payload)) == UNER_OK) {
        uner_usb_qt_state_pending = 0u;
    }
}

static void UNER_App_RequestAuthNotification(OledSimpleNotificationId notification_id)
{
    PermissionAuthNoticeId_t permission_notice = PERMISSION_AUTH_NOTICE_NONE;

    switch (notification_id) {
        case OLED_SIMPLE_NOTIFICATION_PIN_CORRECT:
            permission_notice = PERMISSION_AUTH_NOTICE_PIN_CORRECT;
            break;
        case OLED_SIMPLE_NOTIFICATION_PIN_INCORRECT:
            permission_notice = PERMISSION_AUTH_NOTICE_PIN_INCORRECT;
            break;
        case OLED_SIMPLE_NOTIFICATION_PIN_BYPASSED:
            permission_notice = PERMISSION_AUTH_NOTICE_PIN_BYPASSED;
            break;
        case OLED_SIMPLE_NOTIFICATION_PIN_MODIFIED:
            permission_notice = PERMISSION_AUTH_NOTICE_PIN_MODIFIED;
            break;
        default:
            break;
    }

    if (Permission_GetRequestedPermission() != PERMISSION_NONE) {
        if (permission_notice != PERMISSION_AUTH_NOTICE_NONE &&
            Permission_ShowRemoteAuthNotice(permission_notice, 2200u)) {
            uner_deferred_auth_notification = OLED_SIMPLE_NOTIFICATION_NONE;
            return;
        }
        uner_deferred_auth_notification = notification_id;
        return;
    }

    uner_deferred_auth_notification = OLED_SIMPLE_NOTIFICATION_NONE;
    UNER_App_ShowSimpleNotification(notification_id, 2200u);
}

static void UNER_App_ServiceDeferredAuthNotification(void)
{
    if (uner_deferred_auth_notification == OLED_SIMPLE_NOTIFICATION_NONE ||
        Permission_GetRequestedPermission() != PERMISSION_NONE) {
        return;
    }

    UNER_App_ShowSimpleNotification(uner_deferred_auth_notification, 2200u);
    uner_deferred_auth_notification = OLED_SIMPLE_NOTIFICATION_NONE;
}

static void UNER_App_ServiceDeferredMcuReset(void)
{
    if (!reset_mcu_pending || uner_bridge == NULL ||
        (uint32_t)(HAL_GetTick() - reset_mcu_requested_at) <
            UNER_RESET_MCU_ACK_DELAY_MS) {
        return;
    }

    if (reset_mcu_transport_id == UNER_TR_UART1_ESP) {
        if (uner_bridge->tx_busy != 0u) {
            return;
        }
    } else if (reset_mcu_transport_id == UNER_TR_USB_CDC) {
        if (!CDC_IsDtrActive_FS()) {
            reset_mcu_pending = 0u;
            reset_mcu_requested_at = 0u;
            return;
        }
        if (UNER_TransportUsbCdc_HasPendingTx()) {
            return;
        }
    } else {
        reset_mcu_pending = 0u;
        reset_mcu_requested_at = 0u;
        return;
    }

    __DSB();
    __ISB();
    NVIC_SystemReset();

    for (;;) {
        __NOP();
    }
}

static const UNER_CommandSpec *UNER_App_FindSpecById(uint8_t cmd)
{
    for (uint8_t i = 0u; i < (uint8_t)(sizeof(uner_commands) / sizeof(uner_commands[0])); ++i) {
        if (uner_commands[i].id == cmd) {
            return &uner_commands[i];
        }
    }

    return NULL;
}

static uint8_t UNER_App_SendOledCanvasResponse(uint8_t transport_id,
                                               uint8_t destination_node,
                                              const uint8_t *payload,
                                               uint8_t len,
                                               void *ctx)
{
    (void)ctx;
    return UNER_Send(&uner_handle,
                     transport_id,
                     UNER_NODE_MCU,
                     destination_node,
                     UNER_CMD_ID_OLED_CANVAS_TRANSFER,
                     payload,
                     len) == UNER_OK ? 1u : 0u;
}

static uint8_t UNER_App_IsEspEvent(uint8_t cmd)
{
    return ((cmd >= UNER_EVT_BOOT && cmd <= UNER_EVT_USB_DISCONNECTED) ||
            cmd == UNER_EVT_WIFI_CONNECTED ||
            cmd == UNER_EVT_NETWORK_IP ||
            cmd == UNER_EVT_BOOT_COMPLETE) ? 1u : 0u;
}

static uint8_t UNER_App_IsEspControlPlaneCommand(uint8_t cmd)
{
    switch (cmd) {
        case UNER_CMD_ID_SET_MODE_AP:
        case UNER_CMD_ID_SET_MODE_STA:
        case UNER_CMD_ID_SET_CREDENTIALS:
        case UNER_CMD_ID_CLEAR_CREDENTIALS:
        case UNER_CMD_ID_START_SCAN:
        case UNER_CMD_ID_GET_SCAN_RESULTS:
        case UNER_CMD_ID_REBOOT_ESP:
        case UNER_CMD_ID_FACTORY_RESET:
        case UNER_CMD_ID_STOP_SCAN:
        case UNER_CMD_ID_WIFI_GET_DETAIL:
        case UNER_CMD_ID_GET_STATUS:
        case UNER_CMD_ID_REQUEST_FIRMWARE:
        case UNER_CMD_ID_CONNECT_WIFI:
        case UNER_CMD_ID_DISCONNECT_WIFI:
        case UNER_CMD_ID_SET_AP_CONFIG:
        case UNER_CMD_ID_START_AP:
        case UNER_CMD_ID_STOP_AP:
        case UNER_CMD_ID_SET_AUTO_RECONNECT:
        case UNER_CMD_ID_SET_AP_FALLBACK:
        case UNER_CMD_ID_BINARY_ASSET_STREAM:
        case UNER_CMD_ID_WIFI_CREDENTIALS_WEB_RESULT:
        case UNER_CMD_ID_WIFI_CREDENTIALS_FLOW:
        case UNER_CMD_ID_AP_CLIENTS_PAGE:
        case UNER_CMD_ID_AP_CLIENT_DISCONNECT:
            return 1u;
        default:
            return 0u;
    }
}

static uint8_t UNER_App_IsEspProxyCommand(uint8_t cmd)
{
    switch (cmd) {
        case UNER_CMD_ID_SET_MODE_AP:
        case UNER_CMD_ID_SET_MODE_STA:
        case UNER_CMD_ID_SET_CREDENTIALS:
        case UNER_CMD_ID_CLEAR_CREDENTIALS:
        case UNER_CMD_ID_START_SCAN:
        case UNER_CMD_ID_GET_SCAN_RESULTS:
        case UNER_CMD_ID_REBOOT_ESP:
        case UNER_CMD_ID_FACTORY_RESET:
        case UNER_CMD_ID_STOP_SCAN:
        case UNER_CMD_ID_WIFI_GET_DETAIL:
        case UNER_CMD_ID_GET_STATUS:
        case UNER_CMD_ID_CONNECT_WIFI:
        case UNER_CMD_ID_DISCONNECT_WIFI:
        case UNER_CMD_ID_SET_AP_CONFIG:
        case UNER_CMD_ID_START_AP:
        case UNER_CMD_ID_STOP_AP:
        case UNER_CMD_ID_SET_AUTO_RECONNECT:
        case UNER_CMD_ID_SET_AP_FALLBACK:
        case UNER_CMD_ID_WIFI_CREDENTIALS_FLOW:
        case UNER_CMD_ID_AP_CLIENTS_PAGE:
        case UNER_CMD_ID_AP_CLIENT_DISCONNECT:
            return 1u;
        default:
            return 0u;
    }
}

static uint8_t UNER_App_RelayEspPacketToUsb(const UNER_Packet *packet)
{
    if (packet == NULL || !CDC_IsDtrActive_FS()) {
        return 0u;
    }

    if (UNER_App_IsEspEvent(packet->cmd) ||
        (packet->cmd == UNER_CMD_ID_GET_SCAN_RESULTS && packet->len == 1u &&
         packet->payload != NULL && packet->payload[0] == 0xFEu)) {
        UNER_TransportUsbCdc_SetNextTxPriority(UNER_USB_CDC_TX_PRIORITY_EVENT);
    }

    return UNER_Send(&uner_handle,
                     UNER_TR_USB_CDC,
                     UNER_NODE_MCU,
                     UNER_NODE_PC_QT,
                     packet->cmd,
                     packet->payload,
                     packet->len) == UNER_OK ? 1u : 0u;
}

void UNER_App_OnAtBridgeEvent(const WifiBridge_Event *event)
{
    if (event == NULL || !uner_at_remote_pending) return;
    const uint8_t cmd=uner_at_remote_request.cmd;
    uint8_t response[5u]={0u,0u,0u,0u,0u}; uint8_t length=1u;
    uint8_t finish=0u;
    if (event->id == WIFI_BRIDGE_EVT_ERROR) { response[0]=1u;finish=1u; }
    else if (cmd==UNER_CMD_ID_CONNECT_WIFI) {
        if ((event->id==WIFI_BRIDGE_EVT_GOT_IP || event->id==WIFI_BRIDGE_EVT_STATUS_REFRESHED) &&
            autoMicroSystem.connectivity.espWifiConnection.staIpValid) {
            response[0]=0xFEu;response[1]=autoMicroSystem.connectivity.espWifiConnection.staIp.block1;
            response[2]=autoMicroSystem.connectivity.espWifiConnection.staIp.block2;
            response[3]=autoMicroSystem.connectivity.espWifiConnection.staIp.block3;
            response[4]=autoMicroSystem.connectivity.espWifiConnection.staIp.block4;
            length=5u;finish=1u;
        }
    } else if (cmd==UNER_CMD_ID_START_SCAN) {
        if(event->id==WIFI_BRIDGE_EVT_SCAN_DONE || event->id==WIFI_BRIDGE_EVT_COMMAND_DONE) {
            response[0]=event->id==WIFI_BRIDGE_EVT_COMMAND_DONE?event->status:0u;
            response[1]=autoMicroSystem.connectivity.networksFound;length=2u;finish=1u;
        }
    } else if (event->id==WIFI_BRIDGE_EVT_COMMAND_DONE) {
        response[0]=event->status;finish=1u;
        if(cmd==UNER_CMD_ID_SET_AUTO_RECONNECT) { response[1]=uner_at_remote_bool_value;length=2u; }
        if(cmd==UNER_CMD_ID_START_AP && event->status==0u) {
            response[1]=autoMicroSystem.connectivity.espWifiConnection.apIp.block1;
            response[2]=autoMicroSystem.connectivity.espWifiConnection.apIp.block2;
            response[3]=autoMicroSystem.connectivity.espWifiConnection.apIp.block3;
            response[4]=autoMicroSystem.connectivity.espWifiConnection.apIp.block4;length=5u;
        }
    }
    if(finish) { uner_at_remote_pending=0u;(void)UNER_App_Reply(&uner_at_remote_request,cmd,response,length); }
}

static uint8_t UNER_App_ForwardUsbCommandToEsp(const UNER_Packet *packet)
{
    const UNER_CommandSpec *spec;
    UNER_Status status;

    if (packet == NULL ||
        (packet->transport_id != UNER_TR_USB_CDC &&
         !(packet->transport_id == UNER_TR_UART1_ESP &&
           WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_AT)) ||
        packet->src != UNER_NODE_PC_QT || !UNER_App_IsEspProxyCommand(packet->cmd)) {
        return 0u;
    }
    if (WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_AT) {
        WifiBridge_Result result = WIFI_BRIDGE_BAD_ARG;
        char ssid[WIFI_BRIDGE_SSID_MAX_LEN + 1u];
        char pass[WIFI_BRIDGE_PASS_MAX_LEN + 1u];

        if (packet->cmd == UNER_CMD_ID_GET_STATUS) {
            return 0u;
        }
        if (uner_at_remote_pending) { UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_BUSY); return 1u; }
        switch (packet->cmd) {
            case UNER_CMD_ID_START_AP:
            case UNER_CMD_ID_STOP_AP:
            case UNER_CMD_ID_CLEAR_CREDENTIALS:
            case UNER_CMD_ID_FACTORY_RESET:
            case UNER_CMD_ID_SET_AUTO_RECONNECT:
                result = WifiBridge_SendCommand(uner_bridge, packet->cmd, packet->payload, packet->len);
                break;
            case UNER_CMD_ID_GET_SCAN_RESULTS: {
                uint8_t response[255u] = {0u, 0u}; uint16_t offset = 2u;
                for (uint8_t i=0u;i<autoMicroSystem.connectivity.networksFound;i++) {
                    const char *name=autoMicroSystem.connectivity.wifiNetworkSsids[i];
                    uint8_t length=0u;while(name[length] && length<32u)length++;
                    if (offset+length+1u>sizeof(response)) break;
                    response[offset++]=length;memcpy(&response[offset],name,length);offset+=length;response[1]++;
                }
                (void)UNER_App_Reply(packet, packet->cmd, response, (uint8_t)offset); return 1u;
            }
            case UNER_CMD_ID_WIFI_GET_DETAIL: {
                uint8_t response[36u] = {0u}; uint8_t length=0u;
                if (packet->payload != NULL && packet->len>=2u && packet->payload[0]+1u==packet->len) {
                    for(uint8_t i=0u;i<autoMicroSystem.connectivity.networksFound;i++) {
                        const char *name=autoMicroSystem.connectivity.wifiNetworkSsids[i];
                        if(strlen(name)==packet->payload[0] && memcmp(name,&packet->payload[1],packet->payload[0])==0) {
                            response[0]=packet->payload[0];
                            memcpy(&response[1],name,packet->payload[0]);
                            const uint8_t offset=(uint8_t)(1u+packet->payload[0]);
                            response[offset]=(uint8_t)autoMicroSystem.connectivity.wifiNetworkSignalStrengths[i];
                            response[offset+1u]=autoMicroSystem.connectivity.wifiNetworkEncryptionTypes[i];
                            response[offset+2u]=autoMicroSystem.connectivity.wifiNetworkChannels[i];
                            length=(uint8_t)(offset+3u);break;
                        }
                    }
                }
                if(length==0u)UNER_App_SendCommandNack(packet,UNER_NACK_STATUS_INVALID_ARGUMENT);
                else (void)UNER_App_Reply(packet, packet->cmd,response,length);
                return 1u;
            }
            case UNER_CMD_ID_SET_MODE_AP:
                result = WifiBridge_SetModeAP(uner_bridge);
                break;
            case UNER_CMD_ID_SET_MODE_STA:
                result = WifiBridge_SetModeSTA(uner_bridge);
                break;
            case UNER_CMD_ID_CONNECT_WIFI:
                result = WifiBridge_Connect(uner_bridge);
                break;
            case UNER_CMD_ID_DISCONNECT_WIFI:
                result = WifiBridge_Disconnect(uner_bridge);
                break;
            case UNER_CMD_ID_START_SCAN:
                result = WifiBridge_StartScan(uner_bridge);
                break;
            case UNER_CMD_ID_STOP_SCAN:
                result = WifiBridge_StopScan(uner_bridge);
                break;
            case UNER_CMD_ID_REBOOT_ESP:
                result = packet->payload != NULL && packet->len == 1u
                    ? WifiBridge_SendEspReboot(uner_bridge, packet->payload[0])
                    : WIFI_BRIDGE_BAD_ARG;
                break;
            case UNER_CMD_ID_SET_CREDENTIALS:
                if (packet->payload != NULL && packet->len >= 2u) {
                    const uint8_t ssid_len = packet->payload[0];
                    const uint16_t pass_len_pos = (uint16_t)ssid_len + 1u;
                    if (ssid_len <= WIFI_BRIDGE_SSID_MAX_LEN &&
                        pass_len_pos < packet->len) {
                        const uint8_t pass_len = packet->payload[pass_len_pos];
                        if (pass_len <= WIFI_BRIDGE_PASS_MAX_LEN &&
                            pass_len_pos + 1u + pass_len == packet->len) {
                            memcpy(ssid, &packet->payload[1], ssid_len);
                            ssid[ssid_len] = '\0';
                            memcpy(pass, &packet->payload[pass_len_pos + 1u], pass_len);
                            pass[pass_len] = '\0';
                            result = WifiBridge_SetCredentials(uner_bridge, ssid, pass);
                        }
                    }
                }
                break;
            default:
                UNER_App_SendCommandNack(packet,
                                         UNER_NACK_STATUS_UNSUPPORTED_TRANSPORT);
                return 1u;
        }
        if (result == WIFI_BRIDGE_OK) {
            if (packet->cmd == UNER_CMD_ID_SET_CREDENTIALS || packet->cmd == UNER_CMD_ID_STOP_SCAN) {
                const uint8_t final_status = 0u;
                (void)UNER_App_Reply(packet, packet->cmd, &final_status, 1u);
            } else if (packet->cmd == UNER_CMD_ID_REBOOT_ESP) {
                UNER_App_SendCommandStatus(packet, 0u);
            } else {
                uner_at_remote_bool_value = packet->len==1u ? packet->payload[0] : 0u;
                uner_at_remote_request = *packet;
                uner_at_remote_request.payload = NULL;
                uner_at_remote_request.len = 0u;
                uner_at_remote_pending = 1u;
                uner_at_remote_deadline = HAL_GetTick() + 20000u;
                // Acceptance is intermediate; the modem event produces the final reply.
                UNER_App_SendCommandStatus(packet, 0u);
            }
        } else {
            UNER_App_SendCommandNack(
                packet, result == WIFI_BRIDGE_BAD_ARG
                    ? UNER_NACK_STATUS_INVALID_ARGUMENT
                    : UNER_NACK_STATUS_BUSY);
        }
        return 1u;
    }


    spec = UNER_App_FindSpecById(packet->cmd);
    if (spec == NULL || uner_waiting_validation) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_BUSY);
        return 1u;
    }

    status = UNER_Send(&uner_handle,
                       UNER_TR_UART1_ESP,
                       UNER_NODE_MCU,
                       UNER_NODE_WEB_APP,
                       packet->cmd,
                       packet->payload,
                       packet->len);
    if (status != UNER_OK) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_BUSY);
        return 1u;
    }

    uner_esp_proxy.active = 1u;
    uner_esp_proxy.origin_transport_id = packet->transport_id;
    uner_esp_proxy.origin_node = packet->src;
    uner_esp_proxy.cmd = packet->cmd;
    uner_esp_proxy.flags = spec->flags;
    uner_esp_proxy.ack_seen = 0u;
    uner_waiting_validation = 1u;
    uner_wait_cmd_id = packet->cmd;
    uner_wait_started_at = HAL_GetTick();
    uner_wait_timeout_ms = spec->timeout_ms;

    if (packet->cmd == UNER_CMD_ID_START_SCAN) {
        uner_usb_scan_owned = 1u;
    }
    return 1u;
}

static uint8_t UNER_App_IsPublicRemoteCommand(uint8_t cmd)
{
    switch (cmd) {
        case UNER_CMD_ID_PING:
        case UNER_CMD_ID_AUTH_VALIDATE_PIN:
        case UNER_CMD_ID_AUTH_PIN_GRANTED:
        case UNER_CMD_ID_AUTH_REMOTE_RESULT:
        case UNER_CMD_ID_GET_STATUS:
        case UNER_CMD_ID_REQUEST_FIRMWARE:
        case UNER_CMD_ID_GET_FIRMWARE_INFO:
        case UNER_CMD_ID_GET_BUILD_INFO:
        case UNER_CMD_ID_GET_BOOT_REPORT:
        /* Cleanup may stop only its own endpoint after logout/TTL expiry. */
        case UNER_CMD_ID_STOP_MPU_STREAM:
        case UNER_CMD_ID_STOP_IR_STREAM:
            return 1u;
        default:
            return 0u;
    }
}

static uint8_t UNER_App_AllowForward(void *ctx,
                                     const UNER_Packet *packet,
                                     uint8_t out_transport_id)
{
    (void)ctx;
    if (packet == NULL) {
        return 0u;
    }

    if (packet->transport_id == UNER_TR_USB_CDC &&
        out_transport_id == UNER_TR_UART1_ESP) {
        if (packet->src != UNER_NODE_PC_QT ||
            (packet->dst != UNER_NODE_WEB_APP &&
             packet->dst != UNER_NODE_BROADCAST)) {
            UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_INVALID_SOURCE);
            return 0u;
        }
        if (!UNER_App_IsPublicRemoteCommand(packet->cmd) &&
            !Permission_IsRemoteNodeAuthorizedNow(UNER_NODE_PC_QT)) {
            UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_AUTH_REQUIRED);
            return 0u;
        }
        Permission_TouchRemoteNode(UNER_NODE_PC_QT, HAL_GetTick());
        return 1u;
    }

    if (packet->transport_id == UNER_TR_UART1_ESP &&
        out_transport_id == UNER_TR_USB_CDC) {
        if ((packet->dst != UNER_NODE_PC_QT &&
             packet->dst != UNER_NODE_BROADCAST) ||
            packet->src != UNER_NODE_WEB_APP) {
            return 0u;
        }
        return Permission_IsRemoteNodeAuthorizedNow(UNER_NODE_PC_QT) ? 1u : 0u;
    }

    return 1u;
}
static uint8_t UNER_App_IsRemoteUiCommand(uint8_t cmd)
{
    return (cmd >= UNER_CMD_ID_MENU_ITEM_CLICK &&
            cmd <= UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_RIGHT) ? 1u : 0u;
}

static void UNER_App_OnAnyPacket(void *ctx, const UNER_Packet *packet)
{
    WifiBridge_Backend previous_backend;
    uint8_t was_connected;

    (void)ctx;
    if ((uner_bridge == NULL) || (packet == NULL)) {
        return;
    }

    if (packet->transport_id != UNER_TR_UART1_ESP) {
        return;
    }

    if (WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_AT) {
        WifiBridge_RegisterBackendAlive(uner_bridge, WIFI_BRIDGE_BACKEND_AT,
                                        uner_bridge->mode);
        return;
    }
    previous_backend = WifiBridge_GetActiveBackend(uner_bridge);
    was_connected = WifiBridge_IsConnected(uner_bridge);
    WifiBridge_RegisterBackendAlive(uner_bridge, WIFI_BRIDGE_BACKEND_WEB, uner_bridge->mode);
    if (!was_connected || previous_backend != WIFI_BRIDGE_BACKEND_WEB) {
        Dashboard_InvalidateConnectivity();
    }
}

static void UNER_App_BroadcastIrEmittersChanged(void)
{
    uint8_t payload[3u];
    payload[0] = APP_IsIrObjectsEnabled() ? 1u : 0u;
    payload[1] = APP_IsIrLineEnabled() ? 1u : 0u;
    payload[2] = APP_IsIrLightsEnabled() ? 1u : 0u;

    if (CDC_IsDtrActive_FS()) {
        UNER_TransportUsbCdc_SetNextTxPriority(UNER_USB_CDC_TX_PRIORITY_EVENT);
        (void)UNER_Send(&uner_handle, UNER_TR_USB_CDC,
                        UNER_NODE_MCU, UNER_NODE_PC_QT,
                        UNER_EVT_ID_IR_EMITTERS_CHANGED,
                        payload, (uint8_t)sizeof(payload));
    }
    if (WifiBridge_IsConnected(uner_bridge)) {
        (void)UNER_Send(&uner_handle, UNER_TR_UART1_ESP,
                        UNER_NODE_MCU, UNER_NODE_BROADCAST,
                        UNER_EVT_ID_IR_EMITTERS_CHANGED,
                        payload, (uint8_t)sizeof(payload));
    }
}

static void UNER_App_ExecuteCommand(void *ctx, const UNER_Packet *packet)
{
    uint8_t response_to_pending = 0u;

    (void)ctx;
    if (packet == NULL) {
        return;
    }

    if ((packet->cmd == UNER_CMD_ACK || packet->cmd == UNER_CMD_NACK) &&
        packet->len >= 1u && packet->payload != NULL) {
        if (packet->transport_id == UNER_TR_UART1_ESP &&
            packet->src == UNER_NODE_WEB_APP &&
            uner_waiting_validation &&
            uner_wait_cmd_id == packet->payload[0]) {
            const uint8_t completed_cmd = uner_wait_cmd_id;
            const uint8_t startup_reboot = uner_wait_is_startup_reboot;
            const UNER_CommandSpec *completed_spec = UNER_App_FindSpecById(completed_cmd);
            const uint8_t wait_for_response =
                (completed_spec != NULL && (completed_spec->flags & UNER_SPEC_F_RESP) != 0u) ? 1u : 0u;

            if (uner_esp_proxy.active && uner_esp_proxy.cmd == completed_cmd) {
                (void)UNER_App_RelayEspPacketToUsb(packet);
            }
            if (packet->cmd == UNER_CMD_NACK || !wait_for_response) {
                UNER_App_ClearWaitingValidation();
            } else if (uner_esp_proxy.active) {
                uner_esp_proxy.ack_seen = 1u;
            }
            if (packet->cmd == UNER_CMD_NACK &&
                UNER_App_IsConnectivityMutationCommand(completed_cmd) &&
                !(startup_reboot && APP_DeferEspStartupRebootFailure())) {
                UNER_App_ShowConnectivitySimpleNotification(
                    OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
                    OLED_NOTIFICATION_DEFAULT_MS);
            }
        }
        return;
    }

    if (packet->transport_id == UNER_TR_UART1_ESP &&
        packet->src == UNER_NODE_WEB_APP &&
        uner_waiting_validation &&
        packet->cmd == uner_wait_cmd_id) {
        response_to_pending = 1u;
        if (uner_esp_proxy.active && uner_esp_proxy.cmd == packet->cmd) {
            (void)UNER_App_RelayEspPacketToUsb(packet);
            if (packet->cmd == UNER_CMD_ID_GET_SCAN_RESULTS ||
                packet->cmd == UNER_CMD_ID_STOP_SCAN) {
                uner_usb_scan_owned = 0u;
            }
        }
        UNER_App_ClearWaitingValidation();
    }

    if (!UNER_App_CommandTransportAllowed(packet->cmd, packet->transport_id)) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_UNSUPPORTED_TRANSPORT);
        return;
    }

    if (packet->transport_id == UNER_TR_USB_CDC &&
        packet->src != UNER_NODE_PC_QT) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_INVALID_SOURCE);
        return;
    }

    if (packet->transport_id == UNER_TR_UART1_ESP &&
        WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_AT &&
        packet->src != UNER_NODE_PC_QT) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_INVALID_SOURCE);
        return;
    }

    if (packet->cmd == UNER_CMD_ID_OLED_CANVAS_TRANSFER &&
        !((packet->transport_id == UNER_TR_UART1_ESP &&
           ((packet->src >= UNER_NODE_WEB_CLIENT_FIRST &&
             packet->src <= UNER_NODE_WEB_CLIENT_LAST) ||
            (WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_AT &&
             packet->src == UNER_NODE_PC_QT))) ||
          (packet->transport_id == UNER_TR_USB_CDC &&
           packet->src == UNER_NODE_PC_QT))) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_INVALID_SOURCE);
        return;
    }

    if (packet->transport_id == UNER_TR_UART1_ESP &&
        WifiBridge_GetActiveBackend(uner_bridge) != WIFI_BRIDGE_BACKEND_AT &&
        UNER_App_IsEspEvent(packet->cmd) &&
        packet->src != UNER_NODE_WEB_APP) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_INVALID_SOURCE);
        return;
    }

    if ((packet->transport_id == UNER_TR_UART1_ESP &&
        !response_to_pending &&
        !UNER_App_IsEspEvent(packet->cmd) &&
        !(packet->src == UNER_NODE_WEB_APP &&
          UNER_App_IsEspControlPlaneCommand(packet->cmd)) &&
        !UNER_App_IsPublicRemoteCommand(packet->cmd)) ||
        (packet->transport_id == UNER_TR_USB_CDC &&
         packet->src == UNER_NODE_PC_QT &&
         !UNER_App_IsPublicRemoteCommand(packet->cmd))) {
        if (packet->cmd == UNER_CMD_ID_RESET_MCU &&
            packet->transport_id == UNER_TR_UART1_ESP &&
            !((packet->src >= UNER_NODE_WEB_CLIENT_FIRST &&
               packet->src <= UNER_NODE_WEB_CLIENT_LAST) ||
              (WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_AT &&
               packet->src == UNER_NODE_PC_QT))) {
            UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_INVALID_SOURCE);
            return;
        }
        if ((UNER_App_IsRemoteUiCommand(packet->cmd) ||
             packet->cmd == UNER_CMD_ID_RESET_MCU) &&
            Permission_GetRequestedPermission() != PERMISSION_NONE) {
            UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_LOCAL_PIN_ACTIVE);
            return;
        }
        if (!Permission_IsRemoteNodeAuthorizedNow(packet->src)) {
            UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_AUTH_REQUIRED);
            return;
        }
        Permission_TouchRemoteNode(packet->src, HAL_GetTick());
    }

    if (UNER_App_ForwardUsbCommandToEsp(packet)) {
        return;
    }

    switch (packet->cmd) {
        case 0x23u: case 0x24u: case 0x25u: case 0x26u:
        case 0x27u: case 0x28u: case 0x29u: case 0x2Au: case 0x2Bu: {
            uint8_t response[AUTO_MOTION_STATE_BYTES];
            uint8_t length=APP_AutoMotionRequest(packet->cmd,packet->payload,packet->len,response);
            (void)UNER_App_Reply(packet,packet->cmd,response,length);break;
        }
        case UNER_CMD_ID_LINE_CALIBRATION_CONTROL:
        case UNER_CMD_ID_GET_LINE_STATE:
        case UNER_CMD_ID_SET_LINE_FOLLOW: {
            uint8_t reason=0, state[APP_LINE_STATE_BYTES];
            if (packet->cmd==UNER_CMD_ID_LINE_CALIBRATION_CONTROL) {
                uint8_t op=packet->payload[0]; uint32_t session=UNER_App_ReadU32Le(packet->payload+1);
                if (op>3u || (op<=1u && session)) reason=LINE_FOLLOW_REJECT_SESSION;
                else if (op>=2u && (session!=line_cal_owner_session || line_cal_owner_transport!=packet->transport_id || line_cal_owner_node!=packet->src)) reason=LINE_FOLLOW_REJECT_SESSION;
                else reason=APP_LineCalibrationControl(op,session);
                if (!reason && op<=1u) { line_cal_owner_transport=packet->transport_id; line_cal_owner_node=packet->src; }
            } else if (packet->cmd==UNER_CMD_ID_SET_LINE_FOLLOW) {
                reason=packet->payload[0]>1u?LINE_FOLLOW_REJECT_SESSION:APP_LineFollowSetEnabled(packet->payload[0]!=0);
            }
            APP_GetLineState(state,reason);
            if (!reason && packet->cmd==UNER_CMD_ID_LINE_CALIBRATION_CONTROL && packet->payload[0]<=1u) line_cal_owner_session=UNER_App_ReadU32Le(state+12);
            (void)UNER_App_Reply(packet,packet->cmd,state,APP_LINE_STATE_BYTES);
            break;
        }
        case UNER_CMD_ID_PING:
            (void)UNER_App_Reply(packet,
                                 UNER_CMD_ID_PING,
                                 packet->payload,
                                 packet->len);
            break;
        case UNER_CMD_ID_RESET_MCU:
            UNER_App_HandleResetMcu(packet);
            break;
        case UNER_CMD_ID_GET_EXTENSION_PROFILE:
            UNER_App_HandleGetExtensionProfile(packet);
            break;
        case UNER_CMD_ID_SET_EXTENSION_PROFILE:
            UNER_App_HandleSetExtensionProfile(packet);
            break;
        case UNER_CMD_ID_REBOOT_INTO_PROFILE:
            UNER_App_HandleRebootIntoProfile(packet);
            break;
        case UNER_CMD_ID_GET_AP_FALLBACK: {
            const uint8_t resp[3u] = {0u, AppPreferences_IsWifiApFallbackEnabled() ? 1u : 0u,
                                      APP_GetWifiApFallbackSyncState()};
            (void)UNER_App_Reply(packet, UNER_CMD_ID_GET_AP_FALLBACK, resp, sizeof(resp));
            break;
        }
        case UNER_CMD_ID_GET_IR_EMITTERS: {
            uint8_t resp[3u];
            resp[0] = APP_IsIrObjectsEnabled() ? 1u : 0u;
            resp[1] = APP_IsIrLineEnabled() ? 1u : 0u;
            resp[2] = APP_IsIrLightsEnabled() ? 1u : 0u;
            (void)UNER_App_Reply(packet,
                                 UNER_CMD_ID_GET_IR_EMITTERS,
                                 resp, (uint8_t)sizeof(resp));
            break;
        }
        case UNER_CMD_ID_SET_IR_EMITTERS: {
            uint8_t resp[4u];
            resp[0] = APP_LineCalibrationBusy() ? 1u : 0u;
            if (!resp[0]) {
                APP_SetIrObjectsEnabled(packet->payload[0] != 0u);
                APP_SetIrLineEnabled(packet->payload[1] != 0u);
                APP_SetIrLightsEnabled(packet->payload[2] != 0u);
            }
            resp[1] = APP_IsIrObjectsEnabled() ? 1u : 0u;
            resp[2] = APP_IsIrLineEnabled() ? 1u : 0u;
            resp[3] = APP_IsIrLightsEnabled() ? 1u : 0u;
            (void)UNER_App_Reply(packet,
                                 UNER_CMD_ID_SET_IR_EMITTERS,
                                 resp, (uint8_t)sizeof(resp));
            if (!resp[0]) UNER_App_BroadcastIrEmittersChanged();
            break;
        }
        case UNER_CMD_ID_ECHO:
            UNER_App_ShowSimpleNotification(OLED_SIMPLE_NOTIFICATION_COMMAND_RECEIVED, OLED_NOTIFICATION_DEFAULT_MS);
            break;
        case UNER_CMD_ID_AUTH_VALIDATE_PIN:
            UNER_App_HandleAuthValidatePin(packet);
            break;
        case UNER_CMD_ID_AUTH_PIN_GRANTED:
            UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_DEPRECATED_COMMAND);
            break;
        case UNER_CMD_ID_AUTH_REMOTE_RESULT:
            UNER_App_HandleAuthSession(packet);
            break;
        case UNER_CMD_ID_GET_PREFERENCES:
            UNER_App_HandleGetPreferences(packet);
            break;
        case UNER_CMD_ID_SET_PREFERENCES:
            UNER_App_HandleSetPreferences(packet);
            break;
        case UNER_CMD_ID_GET_AP_CONFIG:
            UNER_App_HandleGetApConfig(packet);
            break;
        case UNER_CMD_ID_SET_AP_CONFIG_DESIRED:
            UNER_App_HandleUpdateApConfig(packet);
            break;
        case UNER_CMD_ID_SET_AP_CONFIG:
            if (packet->transport_id == UNER_TR_UART1_ESP &&
                packet->src == UNER_NODE_WEB_APP &&
                packet->payload != NULL && packet->len >= 1u) {
                AppApConfig_OnEspApplyResponse(packet->payload[0]);
            }
            break;
        case UNER_CMD_ID_REQUEST_FIRMWARE:
            if (packet->transport_id == UNER_TR_USB_CDC) {
                UNER_App_HandleRequestFirmwareCompat(packet);
            } else {
                UNER_App_ParseFirmwareResponse(packet);
            }
            break;
        case UNER_CMD_ID_GET_STATUS:
            if (packet->len == 0u &&
                (packet->transport_id == UNER_TR_USB_CDC ||
                 WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_AT)) {
                UNER_App_HandleGetStatusLocal(packet);
            } else {
                UNER_App_ParseStatusResponse(packet);
            }
            break;
        case UNER_CMD_ID_GET_SCAN_RESULTS:
        case UNER_CMD_ID_START_SCAN:
            UNER_App_ParseScanResultsResponse(packet);
            break;
        case UNER_CMD_ID_SET_AP_FALLBACK:
            if (packet->transport_id == UNER_TR_UART1_ESP &&
                packet->src == UNER_NODE_WEB_APP && packet->payload != NULL &&
                packet->len == 2u && packet->payload[1] <= 1u) {
                APP_ESPWifiApFallbackResponse(packet->payload[0], packet->payload[1]);
            }
            break;
        case UNER_CMD_ID_CONNECT_WIFI:
        case UNER_CMD_ID_START_AP:
        case UNER_CMD_ID_STOP_AP:
        case UNER_CMD_ID_DISCONNECT_WIFI:
            UNER_App_ParseWifiFinalizer(packet);
            break;
        case UNER_CMD_ID_SET_I2C_POLICY:
            UNER_App_HandleSetI2CPolicy(packet);
            break;
        case UNER_CMD_ID_GET_I2C_POLICY:
            UNER_App_HandleGetI2CPolicy(packet);
            break;
        case UNER_CMD_ID_GET_MPU_SNAPSHOT:
            UNER_App_HandleGetMpuSnapshot(packet);
            break;
        case UNER_CMD_ID_SET_MPU_STREAM:
            UNER_App_HandleSetMpuStream(packet);
            break;
        case UNER_CMD_ID_STOP_MPU_STREAM:
            UNER_App_HandleStopMpuStream(packet);
            break;
        case UNER_CMD_ID_GET_IR_SNAPSHOT:
            UNER_App_HandleGetIrSnapshot(packet);
            break;
        case UNER_CMD_ID_SET_IR_STREAM:
            UNER_App_HandleSetIrStream(packet);
            break;
        case UNER_CMD_ID_STOP_IR_STREAM:
            UNER_App_HandleStopIrStream(packet);
            break;
        case UNER_CMD_ID_GET_TELEMETRY_SNAPSHOT:
            UNER_App_HandleGetTelemetrySnapshot(packet);
            break;
        case UNER_CMD_ID_GET_CONTROL_SNAPSHOT:
            UNER_App_HandleGetControlSnapshot(packet);
            break;
        case UNER_CMD_ID_GET_FIRMWARE_INFO:
            UNER_App_HandleGetFirmwareInfo(packet);
            break;
        case UNER_CMD_ID_GET_BUILD_INFO:
            UNER_App_HandleGetBuildInfo(packet);
            break;
        case UNER_CMD_ID_GET_BOOT_REPORT:
            UNER_App_HandleGetBootReport(packet);
            break;
        case UNER_CMD_ID_GET_MOTION_STATE:
        case UNER_CMD_ID_SET_BALANCE_ENABLED:
        case UNER_CMD_ID_SET_MOTION:
        case UNER_CMD_ID_STOP_MOTION:
        case UNER_CMD_ID_SET_MOTOR_TEST:
            UNER_App_HandleMotion(packet);
            break;
        case UNER_CMD_ID_GET_BALANCE_PID:
            UNER_App_HandleGetBalancePid(packet);
            break;
        case UNER_CMD_ID_SET_BALANCE_PID:
            UNER_App_HandleSetBalancePid(packet);
            break;
        case UNER_CMD_ID_WIFI_GET_DETAIL:
            UNER_App_ParseNetworkDetail(packet);
            break;
        case UNER_CMD_ID_WIFI_CREDENTIALS_WEB_RESULT:
            UNER_App_ParseWifiCredentialsWebResult(packet);
            break;
        case UNER_CMD_ID_BINARY_ASSET_STREAM:
            UNER_App_ParseBinaryAssetStream(packet);
            break;
        case UNER_CMD_ID_OLED_CANVAS_TRANSFER:
            OledCanvas_HandlePacket(packet);
            break;
        case UNER_CMD_ID_GET_CURRENT_SCREEN: {
            uint8_t payload[64u];
            uint8_t payload_len = 5u;
            uint16_t remaining_ms;
            uint16_t total_ms;
            bool has_menu_selection;
            UNER_Status send_status;

            has_menu_selection = MenuSys_RequestCurrentMenuSelectionSnapshot(&menuSystem);
            UNER_App_WriteScreenReportPayload(payload,
                                              MenuSys_GetCurrentScreenCode(&menuSystem),
                                              MenuSys_GetCurrentScreenSource(&menuSystem));
            if (OledUtils_GetNotificationProgress(&remaining_ms, &total_ms)) {
                UNER_App_WriteU16Le(&payload[5], remaining_ms);
                UNER_App_WriteU16Le(&payload[7], total_ms);
                payload_len = 9u;
                const uint8_t clientNumber = OledUtils_GetNotificationClientNumber();
                if (clientNumber != 0u) {
                    payload[9] = clientNumber;
                    payload_len = 10u;
                }
            } else if (MenuSys_GetCurrentScreenCode(&menuSystem) == SCREEN_CODE_CORE_DASHBOARD) {
                OledDashboardState dashboard;

                if (OledUtils_GetDashboardState(&dashboard)) {
                    size_t ssid_len = strnlen(dashboard.ssid, OLED_DASHBOARD_SSID_MAX_LEN);
                    uint8_t flags = 0u;

                    if (dashboard.espPresent) flags |= (1u << 0);
                    if (dashboard.staActive) flags |= (1u << 1);
                    if (dashboard.apActive) flags |= (1u << 2);
                    if (dashboard.usbActive) flags |= (1u << 3);
                    if (dashboard.rfActive) flags |= (1u << 4);
                    payload[5] = flags;
                    payload[6] = dashboard.backend;
                    payload[7] = dashboard.carMode;
                    memcpy(&payload[8], dashboard.ip, sizeof(dashboard.ip));
                    payload[12] = (uint8_t)ssid_len;
                    memcpy(&payload[13], dashboard.ssid, ssid_len);
                    payload_len = (uint8_t)(13u + ssid_len);
                }
            } else if (has_menu_selection) {
                payload[5] = menuSystem.current_selected_index;
                payload[6] = menuSystem.current_item_count;
                payload_len = 7u;
            }
            send_status = UNER_App_Reply(packet,
                                         UNER_CMD_ID_GET_CURRENT_SCREEN,
                                         payload,
                                         payload_len);
            (void)send_status;
            break;
        }
        case UNER_CMD_ID_GET_CAR_MODE: {
            uint8_t payload[1u] = { (uint8_t)GET_CAR_MODE() };
            (void)UNER_App_Reply(packet,
                                 UNER_CMD_ID_GET_CAR_MODE, payload, (uint8_t)sizeof(payload));
            break;
        }
        case WIFI_BRIDGE_CMD_ID_SET_CAR_MODE: {
            uint8_t response[2u];
            const uint8_t requested_mode = packet->payload[0];
            uint8_t mode_applied = 0u;

            response[0] = 0u;
            response[1] = (uint8_t)GET_CAR_MODE();
            if (requested_mode >= (uint8_t)CAR_MODE_MAX) {
                response[0] = 1u;
            } else {
                if (APP_TransitionCarMode((CarMode_t)requested_mode)) {
                    response[1] = requested_mode;
                    mode_applied = 1u;
                } else {
                    response[0] = 1u;
                }
            }

            /* La respuesta correlacionada tiene prioridad sobre los eventos
             * derivados. Asi la Web confirma el comando antes de recibir el
             * cambio de pantalla/notificacion provocado por el nuevo modo. */
            (void)UNER_App_Reply(packet, WIFI_BRIDGE_CMD_ID_SET_CAR_MODE,
                                 response, (uint8_t)sizeof(response));
            if (mode_applied) {
                (void)WifiBridge_ReportCarModeChanged(&wifiBridge, requested_mode);
                OledUtils_ShowRemoteModeChangedNotification(requested_mode, 2500u);
            }
            break;
        }
        case UNER_CMD_ID_MENU_ITEM_CLICK:
        case UNER_CMD_ID_TRIGGER_ENCODER_BUTTON:
        case UNER_CMD_ID_TRIGGER_USER_BUTTON:
        case UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_LEFT:
        case UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_RIGHT:
        case UNER_CMD_ID_REQUEST_SCREEN_PAGE: {
            uint8_t status = UNER_REMOTE_ACTION_ACCEPTED;
            if (packet->payload == NULL || packet->len < 4u) {
                UNER_App_SendCommandStatus(packet, UNER_REMOTE_ACTION_INVALID_STATE);
                break;
            }
            if (!UNER_App_PacketScreenMatchesCurrent(packet)) {
                UNER_App_SendCommandStatus(packet, UNER_REMOTE_ACTION_SCREEN_MISMATCH);
                break;
            }

            switch (packet->cmd) {
                case UNER_CMD_ID_MENU_ITEM_CLICK: {
                    if (packet->len == 5u &&
                        !MenuSys_SelectVisibleItem(&menuSystem, packet->payload[4])) {
                        status = UNER_REMOTE_ACTION_INVALID_STATE;
                        break;
                    }
                    status = UNER_App_DispatchRemoteUiEvent(UE_ENC_SHORT_PRESS);
                    break;
                }
                case UNER_CMD_ID_TRIGGER_ENCODER_BUTTON: {
                    const uint8_t press_kind = (packet->len == 5u)
                        ? packet->payload[4]
                        : 0u;
                    if (press_kind > 1u) {
                        status = UNER_REMOTE_ACTION_INVALID_STATE;
                        break;
                    }
                    status = UNER_App_DispatchRemoteUiEvent(press_kind != 0u
                        ? UE_ENC_LONG_PRESS
                        : UE_ENC_SHORT_PRESS);
                    break;
                }
                case UNER_CMD_ID_TRIGGER_USER_BUTTON: {
                    const uint8_t press_kind = (packet->len == 5u)
                        ? packet->payload[4]
                        : 0u;
                    if (press_kind > 1u) {
                        status = UNER_REMOTE_ACTION_INVALID_STATE;
                        break;
                    }
                    status = UNER_App_DispatchRemoteUiEvent(press_kind != 0u
                        ? UE_LONG_PRESS
                        : UE_SHORT_PRESS);
                    break;
                }
                case UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_LEFT:
                    status = UNER_App_DispatchRemoteUiEvent(UE_ROTATE_CCW);
                    break;
                case UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_RIGHT:
                    status = UNER_App_DispatchRemoteUiEvent(UE_ROTATE_CW);
                    break;
                case UNER_CMD_ID_REQUEST_SCREEN_PAGE:
                    if (packet->len != 5u || packet->payload[4] > 1u ||
                        !MenuSys_MoveVisiblePage(&menuSystem,
                            packet->payload[4] == 0u ? -1 : 1)) {
                        status = UNER_REMOTE_ACTION_INVALID_STATE;
                    }
                    break;
                default:
                    status = UNER_REMOTE_ACTION_INVALID_STATE;
                    break;
            }

            UNER_App_SendCommandStatus(packet, status);
            break;
        }
        case UNER_EVT_BOOT: {
            const uint8_t reset_reason = UNER_App_ReadEspBootResetReason(packet);
            OledSimpleNotificationId notification_id =
                OLED_SIMPLE_NOTIFICATION_ESP_BOOT_RECEIVED;

            /* An ESP boot does not revoke the independent USB Qt session. */
            for (uint8_t node = UNER_NODE_WEB_CLIENT_FIRST;
                 node <= UNER_NODE_WEB_CLIENT_LAST; ++node)
                Permission_RevokeRemoteNode(node);
            UNER_App_StopTransportStreams(UNER_TR_UART1_ESP);
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
            if (reset_reason == WIFI_BRIDGE_ESP_RESET_REASON_HW_WATCHDOG ||
                reset_reason == WIFI_BRIDGE_ESP_RESET_REASON_SW_WATCHDOG) {
                notification_id = OLED_SIMPLE_NOTIFICATION_ESP_WATCHDOG_RESET;
            } else if (reset_reason == WIFI_BRIDGE_ESP_RESET_REASON_EXCEPTION) {
                notification_id = OLED_SIMPLE_NOTIFICATION_ESP_EXCEPTION_RESET;
            }
            UNER_App_ShowConnectivitySimpleNotification(
                notification_id,
                2000u);
            break;
        }
        case UNER_EVT_MODE_CHANGED:
            if (packet->payload == NULL || packet->len == 0u) {
                break;
            }
            {
                const uint8_t mode = packet->payload[0];
                const uint8_t has_embedded_ip = (packet->len >= 6u) ? 1u : 0u;

                if (mode > WIFI_MODE_AP_STA) {
                    break;
                }

                UNER_App_UpdateModeFlags(mode);
                UNER_App_StoreWifiInfoSsid(packet->payload, packet->len);

                if (mode == WIFI_MODE_STA) {
                    UNER_App_SetApIp(0u, 0u, 0u, 0u);
                    if (has_embedded_ip) {
                        UNER_App_SetStaIp(packet->payload[2], packet->payload[3],
                                          packet->payload[4], packet->payload[5]);
                    } else {
                        UNER_App_SetStaIp(0u, 0u, 0u, 0u);
                    }
                } else if (mode == WIFI_MODE_AP) {
                    UNER_App_SetStaIp(0u, 0u, 0u, 0u);
                    if (has_embedded_ip) {
                        UNER_App_SetApIp(packet->payload[2], packet->payload[3],
                                         packet->payload[4], packet->payload[5]);
                    } else {
                        UNER_App_SetApIp(0u, 0u, 0u, 0u);
                    }
                } else if (mode == WIFI_MODE_OFF) {
                    UNER_App_SetStaIp(0u, 0u, 0u, 0u);
                    UNER_App_SetApIp(0u, 0u, 0u, 0u);
                } else if (mode == WIFI_MODE_AP_STA && has_embedded_ip) {
                    /* Un unico campo IP no identifica interfaz en AP+STA.
                     * Esperar EVT_NETWORK_IP para no duplicarlo en ambas. */
                }
            }
            UNER_App_ShowConnectivitySimpleNotification(
                OLED_SIMPLE_NOTIFICATION_ESP_MODE_CHANGED,
                2200u);
            break;
        case UNER_EVT_STA_CONNECTED: {
            const uint8_t was_wifi_active = UNER_App_IsStaActive();
            UNER_App_MarkStaActive();
            if (packet->payload != NULL) {
                UNER_App_StoreWifiInfoSsid(packet->payload, packet->len);
                if (packet->len >= 6u) {
                    UNER_App_SetStaIp(packet->payload[2], packet->payload[3],
                                      packet->payload[4], packet->payload[5]);
                }
            }
            UNER_App_ShowWifiConnectedNotification(was_wifi_active);
            UNER_App_RequestNetworkUiRefresh();
            break;
        }
        case UNER_EVT_STA_DISCONNECTED:
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
            UNER_App_SetStaIp(0u, 0u, 0u, 0u);
            if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE) &&
                autoMicroSystem.connectivity.espWifiConnection.apIpValid &&
                !OledUtils_IsNotificationShowing(OledUtils_RenderESPAPStartedNotification)) {
                UNER_App_ShowConnectivityNotification(OledUtils_RenderESPAPStartedNotification,
                                                      2500u);
            }
            UNER_App_RequestNetworkUiRefresh();
            break;
        case UNER_EVT_AP_CLIENT_JOIN:
            if (packet->payload != NULL && packet->len >= 1u) {
                autoMicroSystem.connectivity.espWifiConnection.apClients = packet->payload[0];
                UNER_App_RequestNetworkUiRefresh();
            }
            UNER_App_ShowClientNotification(
                OLED_SIMPLE_NOTIFICATION_ESP_AP_DEVICE_CONNECTED,
                packet->payload != NULL && packet->len >= 2u ? packet->payload[1] : 0u, 2000u);
            break;
        case UNER_EVT_AP_CLIENT_LEAVE:
            if (packet->payload != NULL && packet->len >= 1u) {
                autoMicroSystem.connectivity.espWifiConnection.apClients = packet->payload[0];
                UNER_App_RequestNetworkUiRefresh();
            }
            UNER_App_ShowClientNotification(
                OLED_SIMPLE_NOTIFICATION_ESP_AP_DEVICE_DISCONNECTED,
                packet->payload != NULL && packet->len >= 2u ? packet->payload[1] : 0u, 2000u);
            break;
        case UNER_EVT_WIFI_CONNECTED: {
            const uint8_t was_wifi_active = UNER_App_IsStaActive();
            UNER_App_MarkStaActive();
            if (packet->payload != NULL) {
                UNER_App_StoreTargetCredentialsSsid(packet->payload, packet->len);
            }
            if (autoMicroSystem.connectivity.espWifiConnection.staIpValid) {
                UNER_App_ShowWifiConnectedNotification(was_wifi_active);
            }
            UNER_App_RequestNetworkUiRefresh();
            break;
        }
        case UNER_EVT_NETWORK_IP:
        case UNER_EVT_BOOT_COMPLETE:
            if (packet->payload != NULL) {
                const uint8_t was_wifi_active = UNER_App_IsStaActive();
                const uint8_t is_sta_ip = (packet->len >= 5u && packet->payload[0] == 0x01u) ? 1u : 0u;
                const uint8_t is_ap_ip = (packet->len >= 5u && packet->payload[0] == 0x02u) ? 1u : 0u;
                UNER_App_StoreNetworkIpPayload(packet->payload, packet->len);
                if (is_sta_ip) {
                    UNER_App_ShowWifiConnectedNotification(was_wifi_active);
                } else if (is_ap_ip && !UNER_App_IsStaActive() &&
                           autoMicroSystem.connectivity.espWifiConnection.apIpValid &&
                           !OledUtils_IsNotificationShowing(OledUtils_RenderESPAPStartedNotification)) {
                    UNER_App_ShowConnectivityNotification(OledUtils_RenderESPAPStartedNotification,
                                                          2500u);
                }
                UNER_App_RequestNetworkUiRefresh();
            }
            break;
        case UNER_EVT_WEBSERVER_UP:
            UNER_App_ShowSimpleNotification(OLED_SIMPLE_NOTIFICATION_ESP_WEB_SERVER_UP, OLED_NOTIFICATION_DEFAULT_MS);
            break;
        case UNER_EVT_WEBSERVER_CLIENT_CONNECTED:
            UNER_App_ShowClientNotification(
                OLED_SIMPLE_NOTIFICATION_ESP_WEB_CLIENT_CONNECTED,
                packet->payload != NULL && packet->len >= 5u ? packet->payload[4] : 0u, OLED_NOTIFICATION_DEFAULT_MS);
            (void)UNER_App_ReportUsbQtState(
                IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, USB_ACTIVE) ? 1u : 0u);
            break;
        case UNER_EVT_WEBSERVER_CLIENT_DISCONNECTED:
            UNER_App_ShowClientNotification(
                OLED_SIMPLE_NOTIFICATION_ESP_WEB_CLIENT_DISCONNECTED,
                packet->payload != NULL && packet->len >= 5u ? packet->payload[4] : 0u, OLED_NOTIFICATION_DEFAULT_MS);
            break;
        case UNER_EVT_CONTROLLER_CONNECTED:
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, RF_ACTIVE);
            UNER_App_ShowSimpleNotification(OLED_SIMPLE_NOTIFICATION_CONTROLLER_CONNECTED, OLED_NOTIFICATION_DEFAULT_MS);
            break;
        case UNER_EVT_CONTROLLER_DISCONNECTED:
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, RF_ACTIVE);
            UNER_App_ShowSimpleNotification(OLED_SIMPLE_NOTIFICATION_CONTROLLER_DISCONNECTED, OLED_NOTIFICATION_DEFAULT_MS);
            break;
        case UNER_EVT_USB_CONNECTED:
            UNER_App_ShowSimpleNotification(OLED_SIMPLE_NOTIFICATION_USB_CONNECTED, OLED_NOTIFICATION_DEFAULT_MS);
            break;
        case UNER_EVT_USB_DISCONNECTED:
            UNER_App_ShowSimpleNotification(OLED_SIMPLE_NOTIFICATION_USB_DISCONNECTED, OLED_NOTIFICATION_DEFAULT_MS);
            break;
        default:
            break;
    }
}

static void UNER_App_InitConfig(UNER_CoreConfig *cfg)
{
    cfg->this_node = UNER_NODE_MCU;
    cfg->accept_src_mask = 0xFFFFu;
    cfg->accept_dst_mask = 0xFFFFu;
    cfg->accept_broadcast = 1u;
    cfg->default_max_payload = 255u;
    cfg->on_packet = UNER_App_OnAnyPacket;
    cfg->cb_ctx = NULL;
}

UNER_Status UNER_App_Init(WifiBridge_Handle *bridge)
{
    UNER_CoreConfig cfg;

    if ((bridge == NULL) || (bridge->huart == NULL) || (((UART_HandleTypeDef *)bridge->huart)->hdmarx == NULL)) {
        return UNER_ERR_LEN;
    }

    autoMicroSystem.handles.protocol = &uner_handle;
    uner_bridge = bridge;
    memset(&uner_boot_mailbox, 0, sizeof(uner_boot_mailbox));
    uner_boot_mailbox_valid =
        AutoMicroBootMailbox_Consume(&uner_boot_mailbox) ? 1u : 0u;
    uner_boot_report_sent_mask = 0u;
    uner_nrf_link_connected = 0u;
    UNER_App_InitConfig(&cfg);

    (void)UNER_Handle_Init(&uner_handle,
                           &cfg,
                           uner_slots,
                           UNER_QUEUE_SLOTS,
                           uner_payload_pool,
                           sizeof(uner_payload_pool));

    (void)UNER_TransportUart1Dma_Init(&uner_uart1,
                                      UNER_TR_UART1_ESP,
                                      bridge->huart,
                                      ((UART_HandleTypeDef *)bridge->huart)->hdmarx,
                                      bridge->rx_dma,
                                      WIFI_BRIDGE_UART_RX_DMA_SIZE,
                                      &bridge->tx_busy);

    (void)UNER_TransportUsbCdc_Init(&uner_usbcdc, UNER_TR_USB_CDC);
    (void)UNER_Handle_RegisterTransport(&uner_handle, &uner_uart1.base);
    (void)UNER_Handle_RegisterTransport(&uner_handle, &uner_usbcdc.base);
    (void)UNER_Handle_RegisterRoute(&uner_handle, UNER_NODE_WEB_APP, UNER_TR_UART1_ESP);
    (void)UNER_Handle_RegisterRoute(&uner_handle, UNER_NODE_PC_QT, UNER_TR_USB_CDC);
    UNER_Handle_SetForwardGuard(&uner_handle, UNER_App_AllowForward, NULL);
    (void)UNER_Handle_RegisterCommands(&uner_handle,
                                       uner_commands,
                                       (uint8_t)(sizeof(uner_commands) / sizeof(uner_commands[0])),
                                       UNER_App_ExecuteCommand,
                                       NULL);
    OledCanvas_Init(UNER_App_SendOledCanvasResponse, NULL);

    UNER_App_ResetUart1RxPos();
    UNER_App_ClearWaitingValidation();
    memset(binary_asset_buffer, 0, sizeof(binary_asset_buffer));
    binary_asset_id = 0u;
    binary_asset_width = 0u;
    binary_asset_height = 0u;
    binary_asset_total_len = 0u;
    binary_asset_received_len = 0u;
    binary_asset_ready = 0u;
    binary_asset_in_progress = 0u;
    wifi_scan_results_next_poll_at = 0u;
    memset(mpu_streams, 0, sizeof(mpu_streams));
    memset(ir_streams, 0, sizeof(ir_streams));
    ir_snapshot_seq = 0u;
    memset(&uner_auth_reply_queue, 0, sizeof(uner_auth_reply_queue));
    uner_deferred_auth_notification = OLED_SIMPLE_NOTIFICATION_NONE;
    uner_usb_qt_state_pending = 0u;
    uner_usb_qt_state_value = 0u;
    reset_mcu_pending = 0u;
    reset_mcu_requested_at = 0u;
    reset_mcu_transport_id = UNER_TR_UART1_ESP;
    return UNER_OK;
}

void UNER_App_Poll(void)
{
    if (uner_bridge == NULL) {
        return;
    }

    UNER_TransportUsbCdc_ServiceTx();
    OledCanvas_Poll();
    if (uner_at_remote_pending && (int32_t)(HAL_GetTick()-uner_at_remote_deadline)>=0) {
        uner_at_remote_pending=0u;UNER_App_SendCommandNack(&uner_at_remote_request,UNER_NACK_STATUS_TIMEOUT);
    }

    if (uner_auth_reply_queue.count > 0u) {
        UNER_App_ServiceAuthReplyQueue();
        UNER_App_ServiceDeferredMcuReset();
        return;
    }

    if (uner_uart1_rx_hint || UNER_TransportUsbCdc_HasPendingRx() ||
        UNER_TransportUsbCdc_HasPendingTx() ||
        uner_handle.core.q_count != 0u) {
        uner_uart1_rx_hint = 0u;
        UNER_Handle_Poll(&uner_handle);
        UNER_Handle_ProcessPending(&uner_handle);
    } else {
        UNER_Handle_Poll(&uner_handle);
    }

    OledCanvas_Poll();
    if (uner_at_remote_pending && (int32_t)(HAL_GetTick()-uner_at_remote_deadline)>=0) {
        uner_at_remote_pending=0u;UNER_App_SendCommandNack(&uner_at_remote_request,UNER_NACK_STATUS_TIMEOUT);
    }

    if (uner_auth_reply_queue.count > 0u) {
        UNER_App_ServiceAuthReplyQueue();
        UNER_App_ServiceDeferredMcuReset();
        return;
    }

    if (uner_waiting_validation &&
        uner_wait_timeout_ms > 0u &&
        ((uint32_t)(HAL_GetTick() - uner_wait_started_at) >= (uint32_t)uner_wait_timeout_ms)) {
        const uint8_t timed_out_cmd = uner_wait_cmd_id;
        const uint8_t startup_reboot = uner_wait_is_startup_reboot;
        if (uner_esp_proxy.active && CDC_IsDtrActive_FS()) {
            uint8_t nack_payload[2u];
            nack_payload[0] = timed_out_cmd;
            nack_payload[1] = UNER_NACK_STATUS_TIMEOUT;
            (void)UNER_Send(&uner_handle,
                            UNER_TR_USB_CDC,
                            UNER_NODE_MCU,
                            UNER_NODE_PC_QT,
                            UNER_CMD_NACK,
                            nack_payload,
                            (uint8_t)sizeof(nack_payload));
        }
        UNER_App_ClearWaitingValidation();
        if (timed_out_cmd == UNER_CMD_ID_BINARY_ASSET_STREAM) {
            UNER_App_ResetBinaryAssetState(binary_asset_id);
        } else if (UNER_App_IsConnectivityMutationCommand(timed_out_cmd) &&
                   !(startup_reboot && APP_DeferEspStartupRebootFailure())) {
            UNER_App_ShowConnectivitySimpleNotification(
                OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
                2000u);
        }
    }

    UNER_App_ServiceDeferredMcuReset();
    if (reset_mcu_pending) {
        return;
    }

    UNER_App_ServiceUsbQtState();
    UNER_App_ServiceBootReport();
    UNER_App_ServiceWifiScanResults();
    UNER_App_StopUnauthorizedRemoteStreams();
    UNER_App_ServiceDeferredAuthNotification();
    UNER_App_ServiceMpuStream();
    UNER_App_ServiceIrStream();
    OledCanvas_Poll();
    if (uner_at_remote_pending && (int32_t)(HAL_GetTick()-uner_at_remote_deadline)>=0) {
        uner_at_remote_pending=0u;UNER_App_SendCommandNack(&uner_at_remote_request,UNER_NACK_STATUS_TIMEOUT);
    }
    MenuSys_FlushScreenReport(&menuSystem);
    MenuSys_FlushMenuSelectionReport(&menuSystem);
}

UNER_Status UNER_App_SendCommand(uint8_t cmd, const uint8_t *payload, uint8_t len)
{
    const UNER_CommandSpec *spec = UNER_App_FindSpecById(cmd);
    uint8_t dst = UNER_NODE_WEB_APP;
    UNER_Status status;

    if (spec == NULL) {
        return UNER_ERR_LEN;
    }

    if (len < spec->min_args || len > spec->max_args) {
        return UNER_ERR_LEN;
    }

    if (cmd != UNER_CMD_ID_PING && uner_waiting_validation) {
        return UNER_ERR_BUSY;
    }

    if ((spec->flags & UNER_SPEC_F_EVT) != 0u) {
        dst = UNER_NODE_BROADCAST;
    }

    status = UNER_Send(&uner_handle, UNER_TR_UART1_ESP, UNER_NODE_MCU, dst, cmd, payload, len);
    if (status == UNER_OK && cmd != UNER_CMD_ID_PING) {
        if ((spec->flags & (UNER_SPEC_F_ACK | UNER_SPEC_F_RESP)) != 0u) {
            uner_waiting_validation = 1u;
            uner_wait_cmd_id = cmd;
            uner_wait_started_at = HAL_GetTick();
            uner_wait_timeout_ms = spec->timeout_ms;
        } else {
            UNER_App_ClearWaitingValidation();
        }

        if (cmd == UNER_CMD_ID_CONNECT_WIFI) {
            UNER_App_ShowNotification(OledUtils_RenderESPWifiConnectingNotification, 5000u);
        }
    }

    return status;
}

UNER_Status UNER_App_SendUsbAlive(uint32_t alive_count)
{
    uint8_t payload[4u];

    UNER_App_WriteU32Le(payload, alive_count);
    UNER_TransportUsbCdc_SetNextTxPriority(UNER_USB_CDC_TX_PRIORITY_LOW);
    return UNER_Send(&uner_handle,
                     UNER_TR_USB_CDC,
                     UNER_NODE_MCU,
                     UNER_NODE_PC_QT,
                     UNER_CMD_ID_PING,
                     payload,
                     (uint8_t)sizeof(payload));
}

void UNER_App_OnUsbDtrChanged(uint8_t connected)
{
    if (connected) {
        UNER_TransportUsbCdc_OnDisconnect();
        UNER_TransportUsbCdc_SetNextTxPriority(UNER_USB_CDC_TX_PRIORITY_EVENT);
        (void)UNER_Send(&uner_handle,
                        UNER_TR_USB_CDC,
                        UNER_NODE_MCU,
                        UNER_NODE_PC_QT,
                        UNER_EVT_USB_CONNECTED,
                        NULL,
                        0u);
        uner_boot_report_sent_mask &= (uint8_t)~UNER_BOOT_REPORT_SENT_USB;
        UNER_App_ServiceBootReport();
        return;
    }

    UNER_App_StopTransportStreams(UNER_TR_USB_CDC);
    if (uner_esp_proxy.active) {
        UNER_App_ClearWaitingValidation();
    }
    uner_usb_scan_owned = 0u;
    Permission_RevokeRemoteNode(UNER_NODE_PC_QT);
    uner_boot_report_sent_mask &= (uint8_t)~UNER_BOOT_REPORT_SENT_USB;

    UNER_TransportUsbCdc_OnDisconnect();
}

void UNER_App_OnNrfLinkChanged(uint8_t connected)
{
    if (AppPreferences_GetExtProfile() != EXT_PROFILE_NRF24) {
        uner_nrf_link_connected = 0u;
        uner_boot_report_sent_mask &= (uint8_t)~UNER_BOOT_REPORT_SENT_NRF;
        return;
    }

    uner_nrf_link_connected = connected ? 1u : 0u;
    if (!uner_nrf_link_connected) {
        uner_boot_report_sent_mask &= (uint8_t)~UNER_BOOT_REPORT_SENT_NRF;
    }
    UNER_App_ServiceBootReport();
}

UNER_Status UNER_App_SendWifiCredentialRequest(const char *ssid)
{
    static const char password[] = "connRequest";
    uint8_t payload[3u + WIFI_SSID_MAX_LEN + sizeof(password)];
    uint8_t ssid_len;
    uint8_t pass_len;
    uint8_t offset = 0u;

    if (ssid == NULL || ssid[0] == '\0') {
        return UNER_ERR_LEN;
    }

    ssid_len = (uint8_t)strlen(ssid);
    if (ssid_len > WIFI_SSID_MAX_LEN) {
        ssid_len = WIFI_SSID_MAX_LEN;
    }

    pass_len = (uint8_t)(sizeof(password) - 1u);
    payload[offset++] = 1u;
    payload[offset++] = ssid_len;
    memcpy(&payload[offset], ssid, ssid_len);
    offset = (uint8_t)(offset + ssid_len);
    payload[offset++] = pass_len;
    memcpy(&payload[offset], password, pass_len);
    offset = (uint8_t)(offset + pass_len);

    return UNER_App_SendCommand(UNER_CMD_ID_SET_CREDENTIALS, payload, offset);
}

UNER_Status UNER_App_RequestWifiNetworkDetail(const char *ssid)
{
    uint8_t payload[1u + WIFI_SSID_MAX_LEN];
    uint8_t ssid_len;

    if (ssid == NULL || ssid[0] == '\0') {
        return UNER_ERR_LEN;
    }

    ssid_len = (uint8_t)strlen(ssid);
    if (ssid_len == 0u || ssid_len > WIFI_SSID_MAX_LEN) {
        return UNER_ERR_LEN;
    }

    payload[0] = ssid_len;
    memcpy(&payload[1], ssid, ssid_len);
    return UNER_App_SendCommand(UNER_CMD_ID_WIFI_GET_DETAIL, payload, (uint8_t)(ssid_len + 1u));
}

UNER_Status UNER_App_SendEspReboot(uint8_t boot_mode)
{
    uint8_t payload[1u];

    if (boot_mode != UNER_ESP_REBOOT_BOOT_MODE_NORMAL &&
        boot_mode != UNER_ESP_REBOOT_BOOT_MODE_AP) {
        return UNER_ERR_LEN;
    }

    payload[0] = boot_mode;
    return UNER_App_SendCommand(UNER_CMD_ID_REBOOT_ESP, payload, (uint8_t)sizeof(payload));
}

UNER_Status UNER_App_SendStartupEspReboot(void)
{
    const UNER_Status status = UNER_App_SendEspReboot(UNER_ESP_REBOOT_BOOT_MODE_NORMAL);
    if (status == UNER_OK) {
        /* Internal origin marker; manual/proxied reboot requests keep their errors. */
        uner_wait_is_startup_reboot = 1u;
    }
    return status;
}

uint8_t UNER_App_IsWaitingValidation(void)
{
    return uner_waiting_validation;
}

uint8_t UNER_App_GetWaitingCommandId(void)
{
    return uner_wait_cmd_id;
}

uint8_t UNER_App_IsMcuResetPending(void)
{
    return reset_mcu_pending;
}

uint8_t UNER_App_GetBootloaderVersion(uint32_t *version)
{
    if (version == NULL || !uner_boot_mailbox_valid) {
        return 0u;
    }

    *version = uner_boot_mailbox.bootloader_version;
    return 1u;
}

UNER_Status UNER_App_ReportScreenChanged(uint32_t screen_code, uint8_t source)
{
    uint8_t payload[5u];

    if (screen_code == SCREEN_CODE_NONE) {
        return UNER_ERR_LEN;
    }

    UNER_App_WriteScreenReportPayload(payload, screen_code, source);
    return UNER_Send(&uner_handle,
                     UNER_TR_UART1_ESP,
                     UNER_NODE_MCU,
                     UNER_NODE_BROADCAST,
                     UNER_EVT_ID_SCREEN_CHANGED,
                     payload,
                     (uint8_t)sizeof(payload));
}

UNER_Status UNER_App_ReportNotificationChanged(uint32_t screen_code,
                                               uint8_t source,
                                               uint16_t remaining_ms,
                                               uint16_t total_ms)
{
    uint8_t payload[10u];
    uint8_t payload_len = 9u;

    if (screen_code == SCREEN_CODE_NONE || total_ms == 0u || remaining_ms > total_ms) {
        return UNER_ERR_LEN;
    }

    UNER_App_WriteScreenReportPayload(payload, screen_code, source);
    UNER_App_WriteU16Le(&payload[5], remaining_ms);
    UNER_App_WriteU16Le(&payload[7], total_ms);
    const uint8_t clientNumber = OledUtils_GetNotificationClientNumber();
    if (clientNumber != 0u) {
        payload[9] = clientNumber;
        payload_len = 10u;
    }
    return UNER_Send(&uner_handle,
                     UNER_TR_UART1_ESP,
                     UNER_NODE_MCU,
                     UNER_NODE_BROADCAST,
                     UNER_EVT_ID_SCREEN_CHANGED,
                     payload,
                     payload_len);
}

UNER_Status UNER_App_ReportMenuSelectionChanged(uint32_t screen_code,
                                                uint8_t selected_index,
                                                uint8_t item_count,
                                                uint8_t source)
{
    uint8_t payload[UNER_SELECTION_PAYLOAD_SIZE];

    if (screen_code == SCREEN_CODE_NONE || item_count == 0u || selected_index >= item_count) {
        return UNER_ERR_LEN;
    }

    UNER_App_WriteMenuSelectionReportPayload(payload, screen_code, selected_index, item_count, source);
    return UNER_Send(&uner_handle,
                     UNER_TR_UART1_ESP,
                     UNER_NODE_MCU,
                     UNER_NODE_BROADCAST,
                     UNER_EVT_ID_MENU_SELECTION_CHANGED,
                     payload,
                     (uint8_t)sizeof(payload));
}

UNER_Status UNER_App_ReportCarModeChanged(uint8_t mode)
{
    uint8_t payload[1u] = { mode };

    return UNER_Send(&uner_handle,
                     UNER_TR_UART1_ESP,
                     UNER_NODE_MCU,
                     UNER_NODE_BROADCAST,
                     WIFI_BRIDGE_EVT_ID_CAR_MODE_CHANGED,
                     payload,
                     (uint8_t)sizeof(payload));
}

UNER_Status UNER_App_ReportUsbQtState(uint8_t connected)
{
    uner_usb_qt_state_value = connected ? 1u : 0u;
    uner_usb_qt_state_pending = 1u;
    UNER_App_ServiceUsbQtState();
    return uner_usb_qt_state_pending ? UNER_ERR_BUSY : UNER_OK;
}

UNER_Status UNER_App_RequestBinaryAsset(uint8_t asset_id)
{
    BinaryAssetRequest_t request;
    UNER_Status status;

    if (asset_id == 0u) {
        return UNER_ERR_LEN;
    }

    if (binary_asset_ready && binary_asset_id == asset_id) {
        return UNER_OK;
    }

    if (binary_asset_in_progress && binary_asset_id == asset_id) {
        return UNER_OK;
    }

    request.asset_id = asset_id;
    status = UNER_App_SendCommand(UNER_CMD_ID_BINARY_ASSET_STREAM,
                                  (const uint8_t *)&request,
                                  (uint8_t)sizeof(request));
    if (status == UNER_OK) {
        UNER_App_ResetBinaryAssetState(asset_id);
        binary_asset_in_progress = 1u;
    }

    return status;
}

uint8_t UNER_App_GetBinaryAsset(uint8_t asset_id, BinaryAssetView_t *asset)
{
    if (asset == NULL || !binary_asset_ready || binary_asset_id != asset_id) {
        return 0u;
    }

    asset->asset_id = binary_asset_id;
    asset->width = binary_asset_width;
    asset->height = binary_asset_height;
    asset->len = binary_asset_total_len;
    asset->data = binary_asset_buffer;
    return 1u;
}

void UNER_App_SetRouterForwardingEnabled(uint8_t enabled)
{
    UNER_Handle_SetForwardingEnabled(&uner_handle, enabled);
}

uint8_t UNER_App_IsRouterForwardingEnabled(void)
{
    return UNER_Handle_IsForwardingEnabled(&uner_handle);
}

void UNER_App_GetRouterSnapshot(UNER_RouterSnapshot *out)
{
    UNER_Handle_GetRouterSnapshot(&uner_handle, out);
}

void UNER_App_GetRouterPacketSnapshot(uint8_t tx_view,
                                      uint8_t transport_id,
                                      UNER_RouterSnapshot *out)
{
    UNER_Handle_GetRouterPacketSnapshot(&uner_handle, tx_view, transport_id, out);
}

void UNER_App_OnUart1TxComplete(void)
{
    UNER_TransportUart1Dma_OnTxComplete(&uner_uart1);
}

void UNER_App_NotifyUart1Rx(void)
{
    uner_uart1_rx_hint = 1u;
}
void UNER_App_FeedUart1Bytes(const uint8_t *data, uint16_t len)
{
    if (data == NULL || len == 0u) {
        return;
    }
    for (uint16_t i = 0u; i < len; ++i) {
        (void)UNER_Core_PushByte(&uner_handle.core, UNER_TR_UART1_ESP,
                                 uner_uart1.base.max_payload, data[i]);
    }
    uner_uart1_rx_hint = 1u;
}

void UNER_App_OnAtNetworkDisconnected(void)
{
    uner_handle.core.state = UNER_S_H0;
    uner_handle.core.len_expected = 0u;
    uner_handle.core.payload_index = 0u;
    uner_handle.core.chk_acc = 0u;
    uner_handle.core.drop_frame = 0u;
    uner_uart1_rx_hint = 0u;
    UNER_App_StopTransportStreams(UNER_TR_UART1_ESP);
    if (uner_esp_proxy.active &&
        uner_esp_proxy.origin_transport_id == UNER_TR_UART1_ESP) {
        memset(&uner_esp_proxy, 0, sizeof(uner_esp_proxy));
    }
    if (uner_waiting_validation) {
        UNER_App_ClearWaitingValidation();
    }
    UNER_App_ResetUart1RxPos();
}


uint16_t UNER_App_Uart1RxGetWritePos(void)
{
    if (uner_bridge == NULL || uner_bridge->huart == NULL || ((UART_HandleTypeDef *)uner_bridge->huart)->hdmarx == NULL) {
        return 0u;
    }

    return (uint16_t)(WIFI_BRIDGE_UART_RX_DMA_SIZE -
                      __HAL_DMA_GET_COUNTER(((UART_HandleTypeDef *)uner_bridge->huart)->hdmarx));
}

void UNER_App_ResetUart1RxPos(void)
{
    uner_uart1.last_pos = UNER_App_Uart1RxGetWritePos();
}

static void UNER_App_SetStaIp(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
    autoMicroSystem.connectivity.espStaIp.block1 = b1;
    autoMicroSystem.connectivity.espStaIp.block2 = b2;
    autoMicroSystem.connectivity.espStaIp.block3 = b3;
    autoMicroSystem.connectivity.espStaIp.block4 = b4;
    autoMicroSystem.connectivity.espWifiConnection.staIp.block1 = b1;
    autoMicroSystem.connectivity.espWifiConnection.staIp.block2 = b2;
    autoMicroSystem.connectivity.espWifiConnection.staIp.block3 = b3;
    autoMicroSystem.connectivity.espWifiConnection.staIp.block4 = b4;
    autoMicroSystem.connectivity.espWifiConnection.staIpValid = (b1 | b2 | b3 | b4) ? 1u : 0u;
}

static void UNER_App_SetApIp(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
    autoMicroSystem.connectivity.espApIp.block1 = b1;
    autoMicroSystem.connectivity.espApIp.block2 = b2;
    autoMicroSystem.connectivity.espApIp.block3 = b3;
    autoMicroSystem.connectivity.espApIp.block4 = b4;
    autoMicroSystem.connectivity.espWifiConnection.apIp.block1 = b1;
    autoMicroSystem.connectivity.espWifiConnection.apIp.block2 = b2;
    autoMicroSystem.connectivity.espWifiConnection.apIp.block3 = b3;
    autoMicroSystem.connectivity.espWifiConnection.apIp.block4 = b4;
    autoMicroSystem.connectivity.espWifiConnection.apIpValid = (b1 | b2 | b3 | b4) ? 1u : 0u;
}

static void UNER_App_UpdateModeFlags(uint8_t mode)
{
    if (uner_bridge != NULL) {
        uner_bridge->mode = mode;
    }

    SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
    if (mode == WIFI_MODE_STA) {
        /* El modo STA habilita la interfaz, pero no confirma asociacion.
         * WIFI_ACTIVE conserva el ultimo estado informado por STA_CONNECTED,
         * NETWORK_IP o GET_STATUS. */
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    } else if (mode == WIFI_MODE_AP) {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
        SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    } else if (mode == WIFI_MODE_AP_STA) {
        /* AP+STA tampoco implica que STA ya obtuvo conexion o direccion. */
        SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    } else {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    }
}

static void UNER_App_SetOledSsidFromBytes(const uint8_t *ssid, uint8_t len)
{
    char ssid_buf[WIFI_SSID_MAX_LEN + 1u];
    uint8_t copy_len = len;

    if (ssid == NULL || len == 0u) {
        return;
    }

    if (copy_len > WIFI_SSID_MAX_LEN) {
        copy_len = WIFI_SSID_MAX_LEN;
    }

    memcpy(ssid_buf, ssid, copy_len);
    ssid_buf[copy_len] = '\0';
    OledUtils_SetWiFiConnectedSsid(ssid_buf);
    if (uner_bridge != NULL) {
        memcpy(uner_bridge->ssid, ssid_buf, copy_len + 1u);
    }
}

static void UNER_App_StoreWifiInfoSsid(const uint8_t *payload, uint8_t len)
{
    if (payload == NULL || len < 7u) {
        return;
    }

    const uint8_t ssid_len = payload[6u];
    if (ssid_len == 0u || ((uint16_t)7u + ssid_len) > len) {
        return;
    }

    UNER_App_SetOledSsidFromBytes(&payload[7u], ssid_len);
}

static uint8_t UNER_App_ReadEspBootResetReason(const UNER_Packet *packet)
{
    uint16_t trailer_offset;

    if (packet == NULL || packet->payload == NULL || packet->len < 11u) {
        return WIFI_BRIDGE_ESP_RESET_REASON_UNKNOWN;
    }

    trailer_offset = (uint16_t)7u + packet->payload[6u];
    if (trailer_offset + 4u > packet->len ||
        packet->payload[trailer_offset] != WIFI_BRIDGE_ESP_BOOT_DIAG_MARKER ||
        packet->payload[trailer_offset + 1u] != WIFI_BRIDGE_ESP_BOOT_DIAG_VERSION) {
        return WIFI_BRIDGE_ESP_RESET_REASON_UNKNOWN;
    }

    return packet->payload[trailer_offset + 2u];
}

static void UNER_App_StoreTargetCredentialsSsid(const uint8_t *payload, uint8_t len)
{
    if (payload == NULL || len < 2u) {
        return;
    }

    const uint8_t ssid_len = payload[1u];
    if (ssid_len == 0u || ((uint16_t)2u + ssid_len) > len) {
        return;
    }

    UNER_App_SetOledSsidFromBytes(&payload[2u], ssid_len);
}

static void UNER_App_StoreNetworkIpPayload(const uint8_t *payload, uint8_t len)
{
    if (payload == NULL || len < 5u) {
        return;
    }

    switch (payload[0]) {
        case 0x01u:
            UNER_App_SetStaIp(payload[1], payload[2], payload[3], payload[4]);
            UNER_App_MarkStaActive();
            break;
        case 0x02u:
            UNER_App_SetApIp(payload[1], payload[2], payload[3], payload[4]);
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
            break;
        default:
            break;
    }
}

static void UNER_App_SetWifiNetworkEncryptionText(uint8_t index, uint8_t encryption_type)
{
    const char *text = NULL;
    char *dst;

    if (index >= WIFI_SCAN_MAX_NETWORKS) {
        return;
    }

    dst = autoMicroSystem.connectivity.wifiNetworkEncryptions[index];
    switch (encryption_type) {
        case 0u: text = "OPEN"; break;
        case 1u: text = "WEP"; break;
        case 2u: text = "WPA"; break;
        case 3u: text = "WPA2"; break;
        case 4u: text = "WPA/WPA2"; break;
        case 5u: text = "WPA2 ENT"; break;
        case 6u: text = "WPA3"; break;
        case 7u: text = "WPA2/WPA3"; break;
        case 8u: text = "WAPI"; break;
        default: break;
    }

    if (text != NULL) {
        strncpy(dst, text, WIFI_ENCRYPTION_MAX_LEN);
        dst[WIFI_ENCRYPTION_MAX_LEN] = '\0';
        return;
    }

    memcpy(dst, "ENC ", 4u);
    if (encryption_type >= 100u) {
        dst[4] = (char)('0' + ((encryption_type / 100u) % 10u));
        dst[5] = (char)('0' + ((encryption_type / 10u) % 10u));
        dst[6] = (char)('0' + (encryption_type % 10u));
        dst[7] = '\0';
    } else if (encryption_type >= 10u) {
        dst[4] = (char)('0' + ((encryption_type / 10u) % 10u));
        dst[5] = (char)('0' + (encryption_type % 10u));
        dst[6] = '\0';
    } else {
        dst[4] = (char)('0' + encryption_type);
        dst[5] = '\0';
    }
}

static int8_t UNER_App_FindWifiNetworkIndexBySsid(const uint8_t *ssid, uint8_t len)
{
    if (ssid == NULL || len == 0u) {
        return -1;
    }

    for (uint8_t i = 0u; i < autoMicroSystem.connectivity.networksFound && i < WIFI_SCAN_MAX_NETWORKS; ++i) {
        if (strlen(autoMicroSystem.connectivity.wifiNetworkSsids[i]) == len &&
            memcmp(autoMicroSystem.connectivity.wifiNetworkSsids[i], ssid, len) == 0) {
            return (int8_t)i;
        }
    }

    return -1;
}

static uint8_t UNER_App_IsStaActive(void)
{
    return IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE) ? 1u : 0u;
}

static void UNER_App_MarkStaActive(void)
{
    SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
    SET_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
    if (uner_bridge == NULL || uner_bridge->mode != WIFI_MODE_AP_STA) {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    }
}

static void UNER_App_ShowWifiConnectedNotification(uint8_t was_wifi_active)
{
    if (OledUtils_IsNotificationShowing(OledUtils_RenderESPWifiConnectedNotification)) {
        return;
    }

    if (!was_wifi_active ||
        OledUtils_IsNotificationShowing(OledUtils_RenderESPWifiConnectingNotification) ||
        OledUtils_IsSimpleNotificationShowing(OLED_SIMPLE_NOTIFICATION_ESP_CHECKING_CONNECTION)) {
        UNER_App_ShowConnectivityNotification(OledUtils_RenderESPWifiConnectedNotification,
                                              2500u);
    }
}

static void UNER_App_RequestNetworkUiRefresh(void)
{
    Dashboard_InvalidateConnectivity();
}

static void UNER_App_ParseFirmwareResponse(const UNER_Packet *packet)
{
    uint8_t fw_len;

    if (packet == NULL || packet->payload == NULL || packet->len < 2u) {
        return;
    }

    if (packet->payload[0] != 0u) {
        autoMicroSystem.connectivity.espFirmwareVersion[0] = '\0';
        return;
    }

    SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
    fw_len = (uint8_t)(packet->len - 1u);
    if (fw_len > 32u) {
        fw_len = 32u;
    }

    memcpy(autoMicroSystem.connectivity.espFirmwareVersion, &packet->payload[1], fw_len);
    autoMicroSystem.connectivity.espFirmwareVersion[fw_len] = '\0';

    OledUtils_DismissNotification();
    menuSystem.renderFn = OledUtils_RenderESPFirmwareScreen_Wrapper;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void UNER_App_HandleGetStatusLocal(const UNER_Packet *packet)
{
    uint8_t payload[93u];
    uint8_t ssid_len = 0u;
    uint8_t base_end;
    const char *ssid;
    const WifiBridge_AT_NetworkCounters *counters;

    if (packet == NULL || uner_bridge == NULL) {
        return;
    }

    memset(payload, 0, sizeof(payload));
    payload[0] = 0u;
    payload[1] = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE) ? 1u : 0u;
    payload[2] = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE) ? 1u : 0u;
    payload[3] = uner_bridge->mode;
    payload[4] = autoMicroSystem.connectivity.espWifiConnection.staIpValid ? 3u : 0u;
    payload[5] = autoMicroSystem.connectivity.espWifiConnection.staIp.block1;
    payload[6] = autoMicroSystem.connectivity.espWifiConnection.staIp.block2;
    payload[7] = autoMicroSystem.connectivity.espWifiConnection.staIp.block3;
    payload[8] = autoMicroSystem.connectivity.espWifiConnection.staIp.block4;
    ssid = autoMicroSystem.connectivity.espWifiConnection.staSsid;
    while (ssid[ssid_len] != '\0' && ssid_len < WIFI_SSID_MAX_LEN) {
        ssid_len++;
    }
    payload[9] = ssid_len;
    memcpy(&payload[10], ssid, ssid_len);
    base_end = (uint8_t)(10u + ssid_len);
    payload[base_end] = autoMicroSystem.connectivity.espWifiConnection.staIp.block1;
    payload[base_end + 1u] = autoMicroSystem.connectivity.espWifiConnection.staIp.block2;
    payload[base_end + 2u] = autoMicroSystem.connectivity.espWifiConnection.staIp.block3;
    payload[base_end + 3u] = autoMicroSystem.connectivity.espWifiConnection.staIp.block4;
    payload[base_end + 4u] = autoMicroSystem.connectivity.espWifiConnection.apIp.block1;
    payload[base_end + 5u] = autoMicroSystem.connectivity.espWifiConnection.apIp.block2;
    payload[base_end + 6u] = autoMicroSystem.connectivity.espWifiConnection.apIp.block3;
    payload[base_end + 7u] = autoMicroSystem.connectivity.espWifiConnection.apIp.block4;

    payload[92] = 0xA1u; /* fixed-layout F4 status, preserving all six counters */
    payload[64] = (uint8_t)uner_bridge->active_backend;
    payload[65] = (uint8_t)uner_bridge->at_network_protocol;
    payload[66] = uner_bridge->at_network_ready;
    payload[67] = uner_bridge->at_frame_queue_count;
    counters = &uner_bridge->at_network_counters;
    UNER_App_WriteU32Le(&payload[68], counters->at_timeouts);
    UNER_App_WriteU32Le(&payload[72], counters->send_failures);
    UNER_App_WriteU32Le(&payload[76], counters->queue_drops);
    UNER_App_WriteU32Le(&payload[80], counters->ipd_header_errors);
    UNER_App_WriteU32Le(&payload[84], counters->udp_datagram_drops);
    UNER_App_WriteU32Le(&payload[88], counters->rejected_peers);

    (void)UNER_App_Reply(packet, UNER_CMD_ID_GET_STATUS,
                         payload, (uint8_t)sizeof(payload));
}

static void UNER_App_ParseStatusResponse(const UNER_Packet *packet)
{
    uint8_t ssid_len;
    uint16_t base_end;
    uint16_t cursor;

    if (packet == NULL || packet->payload == NULL || packet->len < 18u) {
        return;
    }

    if (packet->payload[0] != 0u) {
        return;
    }

    SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);

    if (packet->payload[1] != 0u) {
        SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    } else {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    }

    if (packet->payload[2] != 0u) {
        SET_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
    } else {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
    }

    UNER_App_UpdateModeFlags(packet->payload[3]);

    /* Response: status + ap_active + wifi_active + append_wifi_info().
     * append_wifi_info lleva siete bytes fijos y luego el SSID elegido. */
    ssid_len = packet->payload[9u];
    base_end = (uint16_t)10u + ssid_len;
    if ((uint16_t)base_end + 8u > packet->len) {
        return;
    }

    if (ssid_len > 0u) {
        if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)) {
            uint8_t copy_len = ssid_len > WIFI_SSID_MAX_LEN
                ? WIFI_SSID_MAX_LEN : ssid_len;
            memcpy(autoMicroSystem.connectivity.espWifiConnection.apSsid, &packet->payload[10u], copy_len);
            autoMicroSystem.connectivity.espWifiConnection.apSsid[copy_len] = '\0';
        } else {
            UNER_App_SetOledSsidFromBytes(&packet->payload[10u], ssid_len);
        }
    }

    UNER_App_SetStaIp(packet->payload[base_end],
                      packet->payload[base_end + 1u],
                      packet->payload[base_end + 2u],
                      packet->payload[base_end + 3u]);
    UNER_App_SetApIp(packet->payload[base_end + 4u],
                     packet->payload[base_end + 5u],
                     packet->payload[base_end + 6u],
                     packet->payload[base_end + 7u]);

    /* Extension opcional ESP actual:
     * [ap_ssid_len, ap_ssid, ap_pass_preview_len, ap_pass_preview, clients]. */
    cursor = base_end + 8u;
    if (cursor == packet->len) {
        autoMicroSystem.connectivity.espWifiConnection.apPassword[0] = '\0';
        autoMicroSystem.connectivity.espWifiConnection.apClients = 0u;
        autoMicroSystem.connectivity.espWifiConnection.apInfoValid = 1u;
    }
    if (cursor < packet->len) {
        uint8_t ap_ssid_len = packet->payload[cursor++];
        if ((uint16_t)cursor + ap_ssid_len <= packet->len) {
            uint8_t copy_len = ap_ssid_len > WIFI_SSID_MAX_LEN
                ? WIFI_SSID_MAX_LEN : ap_ssid_len;
            memcpy(autoMicroSystem.connectivity.espWifiConnection.apSsid, &packet->payload[cursor], copy_len);
            autoMicroSystem.connectivity.espWifiConnection.apSsid[copy_len] = '\0';
            cursor += ap_ssid_len;
        } else {
            cursor = packet->len;
        }
    }
    if (cursor < packet->len) {
        uint8_t ap_pass_len = packet->payload[cursor++];
        if ((uint16_t)cursor + ap_pass_len <= packet->len) {
            uint8_t copy_len = ap_pass_len >= sizeof(autoMicroSystem.connectivity.espWifiConnection.apPassword)
                ? (uint8_t)(sizeof(autoMicroSystem.connectivity.espWifiConnection.apPassword) - 1u)
                : ap_pass_len;
            memcpy(autoMicroSystem.connectivity.espWifiConnection.apPassword, &packet->payload[cursor], copy_len);
            autoMicroSystem.connectivity.espWifiConnection.apPassword[copy_len] = '\0';
            cursor += ap_pass_len;
        } else {
            cursor = packet->len;
        }
    }
    if (cursor < packet->len) {
        autoMicroSystem.connectivity.espWifiConnection.apClients = packet->payload[cursor];
        autoMicroSystem.connectivity.espWifiConnection.apInfoValid = 1u;
    }
    UNER_App_RequestNetworkUiRefresh();

    if (OledUtils_IsSimpleNotificationShowing(OLED_SIMPLE_NOTIFICATION_ESP_CHECKING_CONNECTION)) {
        OledUtils_RemoveSimpleNotification(OLED_SIMPLE_NOTIFICATION_ESP_CHECKING_CONNECTION);
        if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE)) {
            OledUtils_ShowNotificationMs(OledUtils_RenderESPWifiConnectedNotification, OLED_NOTIFICATION_DEFAULT_MS);
        } else {
            OledUtils_ShowNotificationMs(OledUtils_RenderWiFiNotConnected, OLED_NOTIFICATION_DEFAULT_MS);
        }
    }
}

static void UNER_App_ParseWifiFinalizer(const UNER_Packet *packet)
{
    if (packet == NULL || packet->payload == NULL || packet->len < 1u) {
        return;
    }

    if (packet->payload[0] != 0xFEu && packet->payload[0] != 0u) {
        if (packet->cmd == UNER_CMD_ID_CONNECT_WIFI) {
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
            autoMicroSystem.connectivity.espWifiConnection.staIpValid = 0u;
        }
        UNER_App_ShowConnectivitySimpleNotification(
            OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
            2000u);
        return;
    }

    // A status-0 response with Wi-Fi details only accepts the request.
    // Legacy 5-byte finalizers and FE+IPv4 are the actual completion.
    if ((packet->cmd == UNER_CMD_ID_CONNECT_WIFI || packet->cmd == UNER_CMD_ID_START_AP) &&
        packet->payload[0] == 0u && packet->len != 5u) {
        return;
    }

    if (packet->cmd == UNER_CMD_ID_CONNECT_WIFI) {
        if (packet->len < 5u) {
            return;
        }
        const uint8_t was_wifi_active = UNER_App_IsStaActive();
        UNER_App_SetStaIp(packet->payload[1], packet->payload[2], packet->payload[3], packet->payload[4]);
        UNER_App_MarkStaActive();
        UNER_App_ShowWifiConnectedNotification(was_wifi_active);
        UNER_App_RequestNetworkUiRefresh();
    } else if (packet->cmd == UNER_CMD_ID_START_AP) {
        if (packet->len < 5u) {
            return;
        }
        UNER_App_UpdateModeFlags(WIFI_MODE_AP);
        UNER_App_SetStaIp(0u, 0u, 0u, 0u);
        UNER_App_SetApIp(packet->payload[1], packet->payload[2], packet->payload[3], packet->payload[4]);
        UNER_App_ShowConnectivityNotification(OledUtils_RenderESPAPStartedNotification,
                                              2500u);
    } else if (packet->cmd == UNER_CMD_ID_STOP_AP) {
        UNER_App_UpdateModeFlags(WIFI_MODE_STA);
        UNER_App_SetApIp(0u, 0u, 0u, 0u);
        UNER_App_ShowConnectivitySimpleNotification(
            OLED_SIMPLE_NOTIFICATION_ESP_MODE_CHANGED,
            1800u);
    } else if (packet->cmd == UNER_CMD_ID_DISCONNECT_WIFI) {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
        UNER_App_SetStaIp(0u, 0u, 0u, 0u);
        UNER_App_RequestNetworkUiRefresh();
    }
}

static void UNER_App_ServiceWifiScanResults(void)
{
    if (uner_bridge == NULL ||
        WifiBridge_GetActiveBackend(uner_bridge) != WIFI_BRIDGE_BACKEND_WEB) {
        wifi_scan_results_next_poll_at = 0u;
        return;
    }

    const uint8_t scan_in_progress =
        (autoMicroSystem.connectivity.wifiScanSessionActive && IS_FLAG_SET(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING)) ? 1u : 0u;
    uint32_t now;
    UNER_Status status;

    if (!scan_in_progress && !autoMicroSystem.connectivity.wifiScanResultsPending) {
        wifi_scan_results_next_poll_at = 0u;
        return;
    }

    if (uner_waiting_validation) {
        return;
    }

    now = HAL_GetTick();
    if (wifi_scan_results_next_poll_at != 0u &&
        (int32_t)(now - wifi_scan_results_next_poll_at) < 0) {
        return;
    }

    status = UNER_App_SendCommand(UNER_CMD_ID_GET_SCAN_RESULTS, NULL, 0u);
    wifi_scan_results_next_poll_at = now + ((status == UNER_OK)
        ? (scan_in_progress ? WIFI_SCAN_RESULTS_POLL_RUNNING_MS : WIFI_SCAN_RESULTS_POLL_PENDING_MS)
        : WIFI_SCAN_RESULTS_POLL_RETRY_MS);
}

static void UNER_App_ServiceMpuStream(void)
{
    uint8_t payload[MPU_SNAPSHOT_PAYLOAD_SIZE];
    MPU9250_Snapshot_t snapshot;
    uint32_t now = HAL_GetTick();
    uint8_t len = 0u;
    uint8_t built = 0u;
    for (uint8_t i = 0u; i < UNER_STREAM_SUBSCRIBER_MAX; ++i) {
        UNER_StreamState *stream = &mpu_streams[i];
        if (!stream->active || (stream->next_due_ms != 0u &&
            (int32_t)(now - stream->next_due_ms) < 0)) continue;
        if (!built) {
            len = UNER_App_BuildMpuSnapshotPayload(payload, &snapshot);
            built = 1u;
        }
        if (payload[0] == 0u && snapshot.sample_seq == stream->last_seq) {
            stream->next_due_ms = now + stream->period_ms;
            continue;
        }
        if (stream->transport_id == UNER_TR_USB_CDC)
            UNER_TransportUsbCdc_SetNextTxPriority(UNER_USB_CDC_TX_PRIORITY_EVENT);
        UNER_Status status = UNER_Send(&uner_handle, stream->transport_id,
                                       UNER_NODE_MCU, stream->dst_node,
                                       UNER_EVT_ID_MPU_STREAM, payload, len);
        if (status == UNER_OK) {
            if (payload[0] == 0u) stream->last_seq = snapshot.sample_seq;
            Permission_TouchRemoteNode(stream->dst_node, now);
        }
        stream->next_due_ms = now + stream->period_ms;
    }
}

static void UNER_App_ServiceIrStream(void)
{
    uint8_t payload[IR_SNAPSHOT_PAYLOAD_SIZE];
    uint32_t now = HAL_GetTick();
    for (uint8_t i = 0u; i < UNER_STREAM_SUBSCRIBER_MAX; ++i) {
        UNER_StreamState *stream = &ir_streams[i];
        if (!stream->active || (stream->next_due_ms != 0u &&
            (int32_t)(now - stream->next_due_ms) < 0)) continue;
        uint16_t seq = (uint16_t)(stream->last_seq + 1u);
        uint8_t len = UNER_App_BuildIrSnapshotPayload(payload, seq, stream->period_ms, now);
        if (stream->transport_id == UNER_TR_USB_CDC)
            UNER_TransportUsbCdc_SetNextTxPriority(UNER_USB_CDC_TX_PRIORITY_EVENT);
        if (UNER_Send(&uner_handle, stream->transport_id, UNER_NODE_MCU,
                      stream->dst_node, UNER_EVT_ID_IR_STREAM, payload, len) == UNER_OK) {
            stream->last_seq = seq;
            Permission_TouchRemoteNode(stream->dst_node, now);
        }
        stream->next_due_ms = now + stream->period_ms;
    }
}

static void UNER_App_StopUnauthorizedRemoteStreams(void)
{
    const uint8_t uart_ready = uner_bridge != NULL &&
        (WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_WEB ||
         WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_AT);
    for (uint8_t i = 0u; i < UNER_STREAM_SUBSCRIBER_MAX; ++i) {
        UNER_StreamState *streams[2] = { &mpu_streams[i], &ir_streams[i] };
        for (uint8_t kind = 0u; kind < 2u; ++kind) {
            UNER_StreamState *stream = streams[kind];
            if (!stream->active) continue;
            if ((stream->transport_id == UNER_TR_UART1_ESP && !uart_ready) ||
                (stream->transport_id == UNER_TR_USB_CDC &&
                 !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, USB_ACTIVE)) ||
                !Permission_IsRemoteNodeAuthorizedNow(stream->dst_node))
                memset(stream, 0, sizeof(*stream));
        }
    }
}

static void UNER_App_ParseScanResultsResponse(const UNER_Packet *packet)
{
    uint8_t status;
    uint8_t reported_count;
    uint8_t offset = 2u;

    if (packet == NULL || packet->payload == NULL || packet->len < 2u) {
        return;
    }

    if (!autoMicroSystem.connectivity.wifiScanSessionActive &&
        !autoMicroSystem.connectivity.wifiScanResultsPending &&
        !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING) &&
        MenuSys_GetCurrentScreenCode(&menuSystem) != SCREEN_CODE_CONNECTIVITY_WIFI_RESULTS) {
        return;
    }

    status = packet->payload[0];
    reported_count = packet->payload[1];

    if (status == 1u) {
        autoMicroSystem.connectivity.wifiScanSessionActive = 1u;
        autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
        SET_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);
        wifi_scan_results_next_poll_at = HAL_GetTick() + WIFI_SCAN_RESULTS_POLL_RUNNING_MS;
        return;
    }

    WiFiScan_ClearResults();

    if (status == 0u) {
        while (offset < packet->len &&
               autoMicroSystem.connectivity.networksFound < reported_count &&
               autoMicroSystem.connectivity.networksFound < WIFI_SCAN_MAX_NETWORKS) {
            const uint8_t ssid_len = packet->payload[offset++];
            uint8_t copy_len = ssid_len;

            if ((uint16_t)offset + ssid_len > packet->len) {
                break;
            }

            if (copy_len > WIFI_SSID_MAX_LEN) {
                copy_len = WIFI_SSID_MAX_LEN;
            }

            memcpy(autoMicroSystem.connectivity.wifiNetworkSsids[autoMicroSystem.connectivity.networksFound], &packet->payload[offset], copy_len);
            autoMicroSystem.connectivity.wifiNetworkSsids[autoMicroSystem.connectivity.networksFound][copy_len] = '\0';
            autoMicroSystem.connectivity.wifiNetworkEncryptions[autoMicroSystem.connectivity.networksFound][0] = '\0';
            autoMicroSystem.connectivity.wifiNetworkSignalStrengths[autoMicroSystem.connectivity.networksFound] = 0;
            autoMicroSystem.connectivity.wifiNetworkChannels[autoMicroSystem.connectivity.networksFound] = 0u;
            autoMicroSystem.connectivity.wifiNetworkEncryptionTypes[autoMicroSystem.connectivity.networksFound] = 0u;
            autoMicroSystem.connectivity.wifiNetworkDetailValid[autoMicroSystem.connectivity.networksFound] = 0u;

            offset = (uint8_t)(offset + ssid_len);
            autoMicroSystem.connectivity.networksFound++;
        }
    }

    autoMicroSystem.connectivity.wifiSearchingTimeout = 0u;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);
    autoMicroSystem.connectivity.wifiScanSessionActive = 0u;
    autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
    wifi_scan_results_next_poll_at = 0u;
    menuSystem.currentMenu = &wifiResultsMenu;
    menuSystem.renderFn = OledUtils_RenderWiFiSearchResults_Wrapper;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = false;

    if (status == 0u) {
        UNER_App_ShowNotification(OledUtils_ShowWifiResults, 1200u);
    }
}

static void UNER_App_ParseNetworkDetail(const UNER_Packet *packet)
{
    uint8_t ssid_len;
    uint8_t offset;
    int8_t index;

    if (packet == NULL || packet->payload == NULL || packet->len < 4u) {
        return;
    }

    ssid_len = packet->payload[0u];
    offset = 1u;

    if (ssid_len == 0u ||
        ssid_len > WIFI_SSID_MAX_LEN ||
        ((uint16_t)offset + ssid_len + 3u) != packet->len) {
        return;
    }

    index = UNER_App_FindWifiNetworkIndexBySsid(&packet->payload[offset], ssid_len);
    if (index < 0) {
        return;
    }

    offset = (uint8_t)(offset + ssid_len);
    autoMicroSystem.connectivity.wifiNetworkSignalStrengths[(uint8_t)index] = (int8_t)packet->payload[offset++];
    autoMicroSystem.connectivity.wifiNetworkEncryptionTypes[(uint8_t)index] = packet->payload[offset++];
    autoMicroSystem.connectivity.wifiNetworkChannels[(uint8_t)index] = packet->payload[offset];
    autoMicroSystem.connectivity.wifiNetworkDetailValid[(uint8_t)index] = 1u;
    UNER_App_SetWifiNetworkEncryptionText((uint8_t)index,
                                          autoMicroSystem.connectivity.wifiNetworkEncryptionTypes[(uint8_t)index]);

    if (autoMicroSystem.connectivity.wifiSelectedNetworkIndex == (uint8_t)index &&
        MenuSys_GetCurrentScreenCode(&menuSystem) == SCREEN_CODE_CONNECTIVITY_WIFI_DETAILS) {
        menuSystem.renderFlag = true;
        autoMicroSystem.runtime.oled_first_draw = true;
    }
}

static void UNER_App_ParseWifiCredentialsWebResult(const UNER_Packet *packet)
{
    uint8_t result;
    uint8_t ssid_len;
    uint8_t offset;
    uint8_t remaining;

    if (packet == NULL || packet->payload == NULL || packet->len < 2u) {
        return;
    }

    result = packet->payload[0u];
    ssid_len = packet->payload[1u];
    offset = 2u;

    if (ssid_len > WIFI_SSID_MAX_LEN ||
        ((uint16_t)offset + ssid_len) > packet->len) {
        return;
    }

    remaining = (uint8_t)(packet->len - (uint8_t)(offset + ssid_len));

    if (result == UNER_WIFI_WEB_CREDENTIALS_STATUS_SUCCESS) {
        if (remaining != 4u) {
            return;
        }

        if (ssid_len > 0u) {
            UNER_App_SetOledSsidFromBytes(&packet->payload[offset], ssid_len);
        }
        offset = (uint8_t)(offset + ssid_len);
        UNER_App_SetStaIp(packet->payload[offset],
                          packet->payload[offset + 1u],
                          packet->payload[offset + 2u],
                          packet->payload[offset + 3u]);
        OledUtils_SetWiFiCredentialsWebResultStatus(result);
        UNER_App_MarkStaActive();
        UNER_App_ShowConnectivityNotification(
            OledUtils_RenderWiFiCredentialsSucceededNotification,
            2500u);
    } else if (result == UNER_WIFI_WEB_CREDENTIALS_STATUS_FAILED ||
               result == UNER_WIFI_WEB_CREDENTIALS_STATUS_TIMEOUT ||
               result == UNER_WIFI_WEB_CREDENTIALS_STATUS_CANCELED) {
        if (remaining != 0u) {
            return;
        }

        OledUtils_SetWiFiCredentialsWebResultStatus(result);
        UNER_App_ShowConnectivityNotification(
            OledUtils_RenderWiFiCredentialsFailedNotification,
            2500u);
    } else {
        return;
    }
}

static void UNER_App_ResetBinaryAssetState(uint8_t asset_id)
{
    binary_asset_id = asset_id;
    binary_asset_width = 0u;
    binary_asset_height = 0u;
    binary_asset_total_len = 0u;
    binary_asset_received_len = 0u;
    binary_asset_ready = 0u;
    binary_asset_in_progress = 0u;
    memset(binary_asset_buffer, 0, sizeof(binary_asset_buffer));
}

static void UNER_App_ParseBinaryAssetStream(const UNER_Packet *packet)
{
    const BinaryAssetChunkHeader_t *header;

    if (packet == NULL || packet->payload == NULL || packet->len == 0u) {
        return;
    }

    if (packet->len == 1u) {
        if (packet->payload[0] != 0u) {
            UNER_App_ResetBinaryAssetState(binary_asset_id);
        }
        return;
    }

    if (packet->len < UNER_BINARY_ASSET_CHUNK_HEADER_SIZE) {
        return;
    }

    header = (const BinaryAssetChunkHeader_t *)packet->payload;
    if (header->status != 0u ||
        header->total_len > UNER_BINARY_ASSET_MAX_BYTES ||
        (uint16_t)(header->offset + header->data_len) > header->total_len ||
        (uint16_t)(UNER_BINARY_ASSET_CHUNK_HEADER_SIZE + header->data_len) > packet->len) {
        UNER_App_ResetBinaryAssetState(binary_asset_id);
        return;
    }

    if (!binary_asset_in_progress || binary_asset_id != header->asset_id) {
        UNER_App_ResetBinaryAssetState(header->asset_id);
        binary_asset_in_progress = 1u;
    }

    binary_asset_width = header->width;
    binary_asset_height = header->height;
    binary_asset_total_len = header->total_len;
    memcpy(&binary_asset_buffer[header->offset],
           &packet->payload[UNER_BINARY_ASSET_CHUNK_HEADER_SIZE],
           header->data_len);

    if ((uint16_t)(header->offset + header->data_len) > binary_asset_received_len) {
        binary_asset_received_len = (uint16_t)(header->offset + header->data_len);
    }

    if (binary_asset_received_len >= binary_asset_total_len) {
        binary_asset_ready = 1u;
        binary_asset_in_progress = 0u;
    }
}

static void UNER_App_WriteScreenReportPayload(uint8_t *payload, uint32_t screen_code, uint8_t source)
{
    if (payload == NULL) {
        return;
    }

    payload[0] = (uint8_t)(screen_code & 0xFFu);
    payload[1] = (uint8_t)((screen_code >> 8) & 0xFFu);
    payload[2] = (uint8_t)((screen_code >> 16) & 0xFFu);
    payload[3] = (uint8_t)((screen_code >> 24) & 0xFFu);
    payload[4] = source;
}

static void UNER_App_WriteMenuSelectionReportPayload(uint8_t *payload,
                                                     uint32_t screen_code,
                                                     uint8_t selected_index,
                                                     uint8_t item_count,
                                                     uint8_t source)
{
    payload[0] = (uint8_t)(screen_code & 0xFFu);
    payload[1] = (uint8_t)((screen_code >> 8) & 0xFFu);
    payload[2] = (uint8_t)((screen_code >> 16) & 0xFFu);
    payload[3] = (uint8_t)((screen_code >> 24) & 0xFFu);
    payload[4] = selected_index;
    payload[5] = item_count;
    payload[6] = source;
}

static void UNER_App_HandleResetMcu(const UNER_Packet *packet)
{
    uint8_t payload[2u];

    if (packet == NULL) {
        return;
    }

    if (packet->transport_id == UNER_TR_USB_CDC &&
        packet->src != UNER_NODE_PC_QT) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_INVALID_SOURCE);
        return;
    }

    if (reset_mcu_pending) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_BUSY);
        return;
    }

    MotionControl_EmergencyStop(&motionControl);
    MotionControl_Disarm(&motionControl);
    CLEAR_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    MotorDrive_StopAll(&motorTask);

    /* The reset handler is shared by RESET_MCU and REBOOT_INTO_PROFILE. */
    payload[0] = packet->cmd;
    payload[1] = 0u;
    if (UNER_App_Reply(packet,
                       UNER_CMD_ACK,
                       payload,
                       (uint8_t)sizeof(payload)) == UNER_OK) {
        reset_mcu_requested_at = HAL_GetTick();
        reset_mcu_transport_id = packet->transport_id;
        reset_mcu_pending = 1u;
    }
}

/* Callers keep their guards, explicit reply command and send ordering. */
static UNER_Status UNER_App_Reply(const UNER_Packet *packet,
                                  uint8_t command_id,
                                  const uint8_t *payload,
                                  uint8_t length)
{
    return UNER_Send(&uner_handle,
                     packet->transport_id,
                     UNER_NODE_MCU,
                     packet->src,
                     command_id,
                     payload,
                     length);
}

static void UNER_App_SendCommandStatus(const UNER_Packet *packet, uint8_t status)
{
    uint8_t payload[2u];

    if (packet == NULL) {
        return;
    }

    payload[0] = packet->cmd;
    payload[1] = status;
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ACK,
                         payload,
                         (uint8_t)sizeof(payload));
}

static void UNER_App_SendCommandNack(const UNER_Packet *packet, uint8_t status)
{
    uint8_t payload[2u];

    if (packet == NULL) {
        return;
    }

    payload[0] = packet->cmd;
    payload[1] = status;
    if ((packet->cmd == UNER_CMD_ID_AUTH_VALIDATE_PIN ||
         packet->cmd == UNER_CMD_ID_AUTH_PIN_GRANTED ||
         packet->cmd == UNER_CMD_ID_AUTH_REMOTE_RESULT) &&
        UNER_App_QueueAuthReply(packet->transport_id,
                                packet->src,
                                UNER_CMD_NACK,
                                payload,
                                (uint8_t)sizeof(payload))) {
        return;
    }

    (void)UNER_App_Reply(packet,
                         UNER_CMD_NACK,
                         payload,
                         (uint8_t)sizeof(payload));
}

static uint8_t UNER_App_CommandTransportAllowed(uint8_t cmd, uint8_t transport_id)
{
    const UNER_CommandSpec *spec;

    spec = UNER_App_FindSpecById(cmd);
    if (transport_id == UNER_TR_UART1_ESP) {
        return 1u;
    }

    if (transport_id == UNER_TR_NRF24_SPI2) {
        return (AppPreferences_GetExtProfile() == EXT_PROFILE_NRF24 &&
                spec != NULL &&
                (spec->flags & UNER_SPEC_F_EVT) == 0u) ? 1u : 0u;
    }

    if (transport_id != UNER_TR_USB_CDC) {
        return 0u;
    }

    /* USB CDC is an authenticated extension of the same UNER catalog. F4-owned
     * commands execute locally; ESP-owned commands are proxied after the common
     * source and permission guards above. Events remain inbound-only. */
    return (spec != NULL && (spec->flags & UNER_SPEC_F_EVT) == 0u) ? 1u : 0u;
}

static uint8_t UNER_App_PacketScreenMatchesCurrent(const UNER_Packet *packet)
{
    if (packet == NULL || packet->payload == NULL || packet->len < 4u) {
        return 0u;
    }

    return UNER_App_ReadU32Le(packet->payload) ==
           (uint32_t)MenuSys_GetCurrentScreenCode(&menuSystem);
}

static uint8_t UNER_App_DispatchRemoteUiEvent(UserEvent_t event)
{
    return UiEventRouter_HandleEvent(event) == UI_EVENT_ROUTER_CONSUMED
        ? UNER_REMOTE_ACTION_ACCEPTED
        : UNER_REMOTE_ACTION_NOT_CONSUMED;
}

static uint32_t UNER_App_ReadU32Le(const uint8_t *payload)
{
    return ((uint32_t)payload[0]) |
           ((uint32_t)payload[1] << 8) |
           ((uint32_t)payload[2] << 16) |
           ((uint32_t)payload[3] << 24);
}

static uint16_t UNER_App_ReadU16Le(const uint8_t *payload)
{
    return (uint16_t)(((uint16_t)payload[0]) |
                      ((uint16_t)payload[1] << 8));
}

static float UNER_App_ReadFloatLe(const uint8_t *payload)
{
    union {
        float f;
        uint8_t b[4];
    } v;

    v.b[0] = payload[0];
    v.b[1] = payload[1];
    v.b[2] = payload[2];
    v.b[3] = payload[3];
    return v.f;
}

static void UNER_App_WriteU16Le(uint8_t *payload, uint16_t value)
{
    payload[0] = (uint8_t)(value & 0xFFu);
    payload[1] = (uint8_t)((value >> 8) & 0xFFu);
}

static void UNER_App_WriteU32Le(uint8_t *payload, uint32_t value)
{
    payload[0] = (uint8_t)(value & 0xFFu);
    payload[1] = (uint8_t)((value >> 8) & 0xFFu);
    payload[2] = (uint8_t)((value >> 16) & 0xFFu);
    payload[3] = (uint8_t)((value >> 24) & 0xFFu);
}

static uint8_t UNER_App_AuthDigitsAreValid(const uint8_t *digits)
{
    uint8_t valid = (digits != NULL) ? 1u : 0u;

    if (digits == NULL) {
        return 0u;
    }
    for (uint8_t i = 0u; i < PERMISSION_PIN_DIGITS; ++i) {
        valid &= (digits[i] <= 9u) ? 1u : 0u;
    }

    return valid;
}

static void UNER_App_SendAuthResponse(const UNER_Packet *packet,
                                      uint8_t request_id,
                                      uint8_t scope,
                                      uint8_t granted,
                                      uint32_t ttl_ms,
                                      uint8_t attempts_left)
{
    uint8_t payload[UNER_AUTH_RESPONSE_SIZE];

    payload[0] = UNER_AUTH_STATUS_OK;
    payload[1] = request_id;
    payload[2] = scope;
    payload[3] = granted ? 1u : 0u;
    UNER_App_WriteU32Le(&payload[4], ttl_ms);
    payload[8] = attempts_left;
    (void)UNER_App_QueueAuthReply(packet->transport_id,
                                  packet->src,
                                  UNER_CMD_ID_AUTH_VALIDATE_PIN,
                                  payload,
                                  (uint8_t)sizeof(payload));
}

static void UNER_App_HandleAuthValidatePin(const UNER_Packet *packet)
{
    uint8_t request_id;
    uint8_t scope;
    uint8_t attempts_left = 0u;
    uint32_t ttl_ms = 0u;
    bool new_session = false;
    bool pin_lock_enabled;
    bool granted;

    if (packet == NULL || packet->payload == NULL ||
        (packet->len != 6u && packet->len != 10u)) {
        UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_BAD_PAYLOAD);
        return;
    }

    request_id = packet->payload[0];
    scope = packet->payload[1];
    if (request_id == 0u || !UNER_App_AuthDigitsAreValid(&packet->payload[2])) {
        UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_BAD_PAYLOAD);
        return;
    }

    if (scope == WIFI_BRIDGE_AUTH_SCOPE_REMOTE_SESSION) {
        if (packet->len != 6u) {
            UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_BAD_PAYLOAD);
            return;
        }
    } else if (scope == WIFI_BRIDGE_AUTH_SCOPE_LOCAL_PERMISSION) {
        uint32_t requested_screen;

        if (packet->len != 10u) {
            UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_BAD_PAYLOAD);
            return;
        }
        if (!Permission_HasActiveLocalRequest()) {
            UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_NO_PENDING_PERMISSION);
            return;
        }
        requested_screen = UNER_App_ReadU32Le(&packet->payload[6]);
        if (requested_screen != (uint32_t)MenuSys_GetCurrentScreenCode(&menuSystem)) {
            UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_SCREEN_MISMATCH);
            return;
        }
    } else if (scope == WIFI_BRIDGE_AUTH_SCOPE_CHANGE_PIN) {
        if (packet->len != 10u ||
            !UNER_App_AuthDigitsAreValid(&packet->payload[6])) {
            UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_BAD_PAYLOAD);
            return;
        }
    } else {
        UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_UNSUPPORTED_SCOPE);
        return;
    }

    pin_lock_enabled = AppPreferences_IsPinLockEnabled();
    if (scope == WIFI_BRIDGE_AUTH_SCOPE_CHANGE_PIN &&
        !pin_lock_enabled &&
        !AppPreferences_VerifyAuthPin(&packet->payload[2])) {
        UNER_App_RequestAuthNotification(
            OLED_SIMPLE_NOTIFICATION_PIN_INCORRECT);
        UNER_App_SendAuthResponse(packet,
                                  request_id,
                                  scope,
                                  0u,
                                  0u,
                                  PERMISSION_REMOTE_MAX_ATTEMPTS);
        return;
    }
    granted = Permission_RemoteAuthenticate(packet->src,
                                            &packet->payload[2],
                                            HAL_GetTick(),
                                            &ttl_ms,
                                            &attempts_left,
                                            &new_session);
    if (!granted) {
        UNER_App_RequestAuthNotification(
            OLED_SIMPLE_NOTIFICATION_PIN_INCORRECT);
        UNER_App_SendAuthResponse(packet,
                                  request_id,
                                  scope,
                                  0u,
                                  0u,
                                  attempts_left);
        return;
    }

    if (scope == WIFI_BRIDGE_AUTH_SCOPE_LOCAL_PERMISSION) {
        if (!Permission_GrantCurrentRequest(0u)) {
            Permission_RevokeRemoteNode(packet->src);
            UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_NO_PENDING_PERMISSION);
            return;
        }
    } else if (scope == WIFI_BRIDGE_AUTH_SCOPE_CHANGE_PIN) {
        if (AppPreferences_ChangeAuthPin(&packet->payload[2],
                                         &packet->payload[6]) != HAL_OK) {
            if (new_session) {
                Permission_RevokeRemoteNode(packet->src);
            }
            UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_STORAGE_FAILURE);
            return;
        }

        Permission_CancelLocalRequest();
        Permission_RevokeAll();
        Permission_RevokeAllRemote();
        (void)Permission_RemoteAuthenticate(packet->src,
                                            &packet->payload[6],
                                            HAL_GetTick(),
                                            &ttl_ms,
                                            &attempts_left,
                                            &new_session);
        UNER_App_RequestAuthNotification(OLED_SIMPLE_NOTIFICATION_PIN_MODIFIED);
    } else if (pin_lock_enabled) {
        UNER_App_RequestAuthNotification(
            OLED_SIMPLE_NOTIFICATION_PIN_CORRECT);
    } else {
        UNER_App_RequestAuthNotification(
            OLED_SIMPLE_NOTIFICATION_PIN_BYPASSED);
    }

    UNER_App_SendAuthResponse(packet,
                              request_id,
                              scope,
                              1u,
                              ttl_ms,
                              attempts_left);
}

static void UNER_App_HandleAuthSession(const UNER_Packet *packet)
{
    uint8_t payload[UNER_AUTH_SESSION_RESPONSE_SIZE];
    uint8_t authenticated;
    uint32_t ttl_ms;

    if (packet == NULL || packet->payload == NULL || packet->len != 1u ||
        packet->payload[0] > WIFI_BRIDGE_AUTH_SESSION_ACTION_QUERY) {
        UNER_App_SendCommandNack(packet, UNER_AUTH_STATUS_BAD_PAYLOAD);
        return;
    }

    if (packet->payload[0] == WIFI_BRIDGE_AUTH_SESSION_ACTION_LOGOUT) {
        Permission_RevokeRemoteNode(packet->src);
        UNER_App_StopStreamEndpoint(mpu_streams, packet->transport_id, packet->src);
        UNER_App_StopStreamEndpoint(ir_streams, packet->transport_id, packet->src);
    } else {
        /* Touch only a still-authorized session; never revive an expired node. */
        Permission_TouchRemoteNode(packet->src, HAL_GetTick());
    }

    authenticated = Permission_IsRemoteNodeAuthorizedNow(packet->src) ? 1u : 0u;
    ttl_ms = Permission_GetRemoteTtlRemaining(packet->src, HAL_GetTick());
    payload[0] = UNER_AUTH_STATUS_OK;
    payload[1] = authenticated;
    UNER_App_WriteU32Le(&payload[2], ttl_ms);
    (void)UNER_App_QueueAuthReply(packet->transport_id,
                                  packet->src,
                                  UNER_CMD_ID_AUTH_REMOTE_RESULT,
                                  payload,
                                  (uint8_t)sizeof(payload));
}

static void UNER_App_WriteFloatLe(uint8_t *payload, float value)
{
    union {
        float f;
        uint8_t b[4];
    } v;

    v.f = value;
    payload[0] = v.b[0];
    payload[1] = v.b[1];
    payload[2] = v.b[2];
    payload[3] = v.b[3];
}

static uint8_t UNER_App_AppendTextField(uint8_t *payload,
                                        uint8_t payload_len,
                                        uint8_t *offset,
                                        const char *text,
                                        uint8_t max_text_len)
{
    uint8_t len = 0u;

    if (payload == NULL || offset == NULL || text == NULL) {
        return 0u;
    }

    while (text[len] != '\0' && len < max_text_len) {
        len++;
    }

    if ((uint16_t)(*offset + 1u + len) > payload_len) {
        return 0u;
    }

    payload[(*offset)++] = len;
    memcpy(&payload[*offset], text, len);
    *offset = (uint8_t)(*offset + len);
    return 1u;
}

static uint8_t UNER_App_ParsePreferenceValue(const uint8_t *payload,
                                             uint8_t len,
                                             uint8_t *offset,
                                             const AppPreferencesDescriptor_t *desc,
                                             AppPreferences_t *prefs)
{
    uint16_t value;

    if (payload == NULL || offset == NULL || desc == NULL || prefs == NULL) {
        return 0u;
    }

    if (desc->type == APP_PREFS_VALUE_BOOL) {
        if (*offset >= len) {
            return 0u;
        }
        value = payload[*offset] ? 1u : 0u;
        *offset = (uint8_t)(*offset + 1u);
    } else if (desc->type == APP_PREFS_VALUE_U16) {
        if ((uint16_t)(*offset + 2u) > len) {
            return 0u;
        }
        value = UNER_App_ReadU16Le(&payload[*offset]);
        *offset = (uint8_t)(*offset + 2u);
    } else {
        return 0u;
    }

    switch (desc->field) {
        case APP_PREFS_FIELD_ENCODER_INVERTED:
            prefs->encoderInverted = value ? true : false;
            break;
        case APP_PREFS_FIELD_SCREEN_LOCK_ENABLED:
            prefs->screenLockEnabled = value ? true : false;
            break;
        case APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT:
            prefs->screenLockTimeoutSec = value;
            break;
        case APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM:
            prefs->motorLeftMinPwm = value;
            break;
        case APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM:
            prefs->motorRightMinPwm = value;
            break;
        case APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT:
            prefs->systemActionTimeoutSec = value;
            break;
        case APP_PREFS_FIELD_USB_ALIVE_PERIOD:
            prefs->usbAlivePeriodMs = value;
            break;
        case APP_PREFS_FIELD_OLED_ROTATION:
            prefs->oledRotationDeg = value;
            break;
        case APP_PREFS_FIELD_LINE_BLACK_WIDTH:
            prefs->lineBlackWidthMm = value;
            break;
        case APP_PREFS_FIELD_OLED_AUTO_ROTATION:
            prefs->oledAutoRotationEnabled = value ? true : false;
            break;
        case APP_PREFS_FIELD_UNER_ROUTER_FORWARDING:
            prefs->unerRouterForwardingEnabled = value ? true : false;
            break;
        case APP_PREFS_FIELD_PIN_LOCK_ENABLED:
            prefs->pinLockEnabled = value ? true : false;
            break;
        case APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED:
            prefs->wifiApFallbackEnabled = value ? true : false;
            break;
        case APP_PREFS_FIELD_WEB_ALIVE_PERIOD:
            prefs->webAlivePeriodMs = value;
            break;
        case APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED:
            prefs->wifiApPasswordProtected = value ? true : false;
            break;
        default:
            return 0u;
    }

    return 1u;
}

static void UNER_App_HandleGetPreferences(const UNER_Packet *packet)
{
    uint8_t payload[UNER_PREF_RESPONSE_MAX_BYTES];
    AppPreferences_t prefs;
    const AppPreferencesDescriptor_t *descs;
    uint8_t count = 0u;
    uint8_t offset = 0u;

    if (packet == NULL) {
        return;
    }

    descs = AppPreferences_GetDescriptors(&count);
    if (packet->len == 1u && packet->payload != NULL) {
        const uint8_t index = packet->payload[0];
        uint8_t key_len = 0u;

        payload[offset++] = (index < count && descs != NULL) ? 0u : 2u;
        payload[offset++] = 1u;
        payload[offset++] = index;
        payload[offset++] = count;

        if (index < count && descs != NULL) {
            const AppPreferencesDescriptor_t *desc = &descs[index];
            const char *key = desc->key ? desc->key : "";

            while (key[key_len] != '\0' && key_len < 40u) {
                key_len++;
            }

            UNER_App_WriteU32Le(&payload[offset], desc->field);
            offset = (uint8_t)(offset + 4u);
            payload[offset++] = (uint8_t)desc->type;
            UNER_App_WriteU16Le(&payload[offset], desc->minValue);
            offset = (uint8_t)(offset + 2u);
            UNER_App_WriteU16Le(&payload[offset], desc->maxValue);
            offset = (uint8_t)(offset + 2u);
            UNER_App_WriteU16Le(&payload[offset], desc->stepValue);
            offset = (uint8_t)(offset + 2u);
            UNER_App_WriteU16Le(&payload[offset], desc->defaultValue);
            offset = (uint8_t)(offset + 2u);
            payload[offset++] = key_len;
            memcpy(&payload[offset], key, key_len);
            offset = (uint8_t)(offset + key_len);
        }

        (void)UNER_App_Reply(packet,
                             UNER_CMD_ID_GET_PREFERENCES, payload, offset);
        return;
    }

    AppPreferences_GetSnapshot(&prefs);
    payload[offset++] = 0u;
    payload[offset++] = 0u;
    payload[offset++] = count;
    UNER_App_WriteU32Le(&payload[offset], APP_PREFS_FIELD_ALL & ~APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED);
    offset = (uint8_t)(offset + 4u);
    payload[offset++] = prefs.encoderInverted ? 1u : 0u;
    payload[offset++] = prefs.screenLockEnabled ? 1u : 0u;
    UNER_App_WriteU16Le(&payload[offset], prefs.screenLockTimeoutSec);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], prefs.motorLeftMinPwm);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], prefs.motorRightMinPwm);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], prefs.systemActionTimeoutSec);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], prefs.usbAlivePeriodMs);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], prefs.oledRotationDeg);
    offset = (uint8_t)(offset + 2u);
    payload[offset++] = prefs.oledAutoRotationEnabled ? 1u : 0u;
    payload[offset++] = prefs.unerRouterForwardingEnabled ? 1u : 0u;
    UNER_App_WriteU16Le(&payload[offset], prefs.lineBlackWidthMm);
    offset = (uint8_t)(offset + 2u);
    payload[offset++] = prefs.mpuCalibration.valid ? 1u : 0u;
    payload[offset++] = prefs.pinLockEnabled ? 1u : 0u;
    UNER_App_WriteU16Le(&payload[offset], prefs.webAlivePeriodMs);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU32Le(&payload[offset], AppPreferences_GetRecordSizeBytes());
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteU32Le(&payload[offset], AppPreferences_GetRecordCapacity());
    offset = (uint8_t)(offset + 4u);

    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_PREFERENCES, payload, offset);
}

static void UNER_App_HandleSetPreferences(const UNER_Packet *packet)
{
    uint8_t payload[5u];
    AppPreferencesPatch_t patch;
    AppPreferencesFieldMask_t fields;
    AppPreferencesFieldMask_t changed = 0u;
    const AppPreferencesDescriptor_t *descs;
    uint8_t count = 0u;
    uint8_t offset = 4u;
    uint8_t status = 0u;

    if (packet == NULL || packet->payload == NULL || packet->len < 4u) {
        return;
    }

    fields = UNER_App_ReadU32Le(packet->payload);
    if ((fields & ~APP_PREFS_FIELD_ALL) != 0u ||
        (fields & APP_PREFS_FIELD_MPU_CALIBRATION) != 0u) {
        status = 4u;
    } else if (fields == 0u) {
        status = 2u;
    }

    AppPreferences_GetSnapshot(&patch.values);
    patch.fields = fields;
    descs = AppPreferences_GetDescriptors(&count);

    if (status == 0u) {
        for (uint8_t i = 0u; i < count; ++i) {
            if ((fields & descs[i].field) == 0u) {
                continue;
            }

            if (!UNER_App_ParsePreferenceValue(packet->payload,
                                               packet->len,
                                               &offset,
                                               &descs[i],
                                               &patch.values)) {
                status = 1u;
                break;
            }
        }
    }

    if (status == 0u && offset != packet->len) {
        status = 1u;
    }

    if (status == 0u) {
        if (AppPreferences_ApplyPatch(&patch, &changed) == HAL_OK) {
            APP_ApplyPreferenceRuntimeChanges(changed);
        } else {
            status = 3u;
        }
    }

    payload[0] = status;
    UNER_App_WriteU32Le(&payload[1], changed);
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_SET_PREFERENCES, payload, (uint8_t)sizeof(payload));
}

static void UNER_App_HandleGetApConfig(const UNER_Packet *packet)
{
    AppApConfig_t config;
    uint8_t payload[1u + 1u + 1u + 1u + 1u + APP_AP_SSID_MAX_BYTES + 4u];
    uint8_t offset = 0u;
    const size_t ssidLength = (packet != NULL) ? strlen(AppPreferences_GetWifiApSsid()) : 0u;

    if (packet == NULL) {
        return;
    }
    AppApConfig_Get(&config);
    payload[offset++] = APP_AP_STATUS_OK;
    payload[offset++] = (uint8_t)config.applyState;
    payload[offset++] = config.protected ? 1u : 0u;
    payload[offset++] = config.password[0] != '\0' ? 1u : 0u;
    payload[offset++] = (uint8_t)ssidLength;
    memcpy(&payload[offset], config.ssid, ssidLength);
    offset = (uint8_t)(offset + ssidLength);
    memcpy(&payload[offset], config.ipv4, sizeof(config.ipv4));
    offset = (uint8_t)(offset + sizeof(config.ipv4));
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_AP_CONFIG, payload, offset);
}

static void UNER_App_HandleUpdateApConfig(const UNER_Packet *packet)
{
    AppApConfigPatch_t patch = {0};
    AppApConfig_t state;
    uint8_t response[3u];
    uint8_t cursor = 1u;
    uint8_t changed = 0u;
    AppApStatus_t status = APP_AP_STATUS_INVALID_PAYLOAD;

    if (packet == NULL || packet->payload == NULL || packet->len < 1u) {
        return;
    }
    patch.fieldMask = packet->payload[0];
    if (patch.fieldMask != 0u &&
        (patch.fieldMask & (uint8_t)~APP_AP_FIELD_ALL) == 0u) {
        status = APP_AP_STATUS_OK;
        if ((patch.fieldMask & APP_AP_FIELD_SSID) != 0u) {
            uint8_t length;
            if (cursor >= packet->len) { status = APP_AP_STATUS_INVALID_PAYLOAD; }
            else {
                length = packet->payload[cursor++];
                if (length == 0u || length > APP_AP_SSID_MAX_BYTES ||
                    (uint16_t)cursor + length > packet->len) {
                    status = APP_AP_STATUS_INVALID_SSID;
                } else {
                    memcpy(patch.values.ssid, &packet->payload[cursor], length);
                    cursor = (uint8_t)(cursor + length);
                }
            }
        }
        if (status == APP_AP_STATUS_OK &&
            (patch.fieldMask & APP_AP_FIELD_PASSWORD) != 0u) {
            uint8_t length;
            if (cursor >= packet->len) { status = APP_AP_STATUS_INVALID_PAYLOAD; }
            else {
                length = packet->payload[cursor++];
                if (length == 0u) {
                    patch.fieldMask &= (uint8_t)~APP_AP_FIELD_PASSWORD;
                } else if (length < APP_AP_PASSWORD_MIN_BYTES ||
                           length > APP_AP_PASSWORD_MAX_BYTES ||
                           (uint16_t)cursor + length > packet->len) {
                    status = APP_AP_STATUS_INVALID_PASSWORD;
                } else {
                    memcpy(patch.values.password, &packet->payload[cursor], length);
                    cursor = (uint8_t)(cursor + length);
                }
            }
        }
        if (status == APP_AP_STATUS_OK &&
            (patch.fieldMask & APP_AP_FIELD_PROTECTED) != 0u) {
            if (cursor >= packet->len || packet->payload[cursor] > 1u) {
                status = APP_AP_STATUS_INVALID_PAYLOAD;
            } else {
                patch.values.protected = packet->payload[cursor++] != 0u;
            }
        }
        if (status == APP_AP_STATUS_OK &&
            (patch.fieldMask & APP_AP_FIELD_IPV4) != 0u) {
            if ((uint16_t)cursor + 4u > packet->len) {
                status = APP_AP_STATUS_INVALID_PAYLOAD;
            } else {
                memcpy(patch.values.ipv4, &packet->payload[cursor], 4u);
                cursor = (uint8_t)(cursor + 4u);
            }
        }
        if (status == APP_AP_STATUS_OK && cursor != packet->len) {
            status = APP_AP_STATUS_INVALID_PAYLOAD;
        }
        if (status == APP_AP_STATUS_OK && patch.fieldMask == 0u) {
            AppApConfig_Get(&state);
        } else if (status == APP_AP_STATUS_OK) {
            status = AppApConfig_ApplyPatch(&patch, &changed);
        }
    }
    AppApConfig_Get(&state);
    response[0] = (uint8_t)status;
    response[1] = changed;
    response[2] = (uint8_t)state.applyState;
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_SET_AP_CONFIG_DESIRED, response, (uint8_t)sizeof(response));
}
static void UNER_App_HandleSetI2CPolicy(const UNER_Packet *packet)
{
    uint8_t payload[2u];
    I2C_PolicyMode policy;
    uint8_t status = 0u;

    if (packet == NULL || packet->payload == NULL || packet->len != 1u) {
        return;
    }

    policy = (I2C_PolicyMode)packet->payload[0];
    if (I2C_Manager_SetPolicy(&i2cManager, policy) != HAL_OK) {
        status = 2u;
    }

    payload[0] = status;
    payload[1] = (uint8_t)I2C_Manager_GetPolicy(&i2cManager);
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_SET_I2C_POLICY, payload, (uint8_t)sizeof(payload));
}

static void UNER_App_HandleGetI2CPolicy(const UNER_Packet *packet)
{
    uint8_t payload[2u];

    if (packet == NULL) {
        return;
    }

    payload[0] = (i2cManager.hi2c != NULL) ? 0u : 3u;
    payload[1] = (uint8_t)I2C_Manager_GetPolicy(&i2cManager);
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_I2C_POLICY, payload, (uint8_t)sizeof(payload));
}

static uint8_t UNER_App_BuildMpuSnapshotPayload(uint8_t *payload, MPU9250_Snapshot_t *snapshot_out)
{
    MPU9250_Snapshot_t snapshot;
    MPU9250_Attitude_t attitude;
    uint8_t offset = 0u;
    uint8_t wire_flags = 0u;
    HAL_StatusTypeDef snap_status;
    HAL_StatusTypeDef attitude_status;

    if (payload == NULL) {
        return 0u;
    }

    memset(&snapshot, 0, sizeof(snapshot));
    memset(&attitude, 0, sizeof(attitude));

    snap_status = MPU9250_GetLatest(&mpuTask, &snapshot);
    attitude_status = MPU9250_GetAttitude(&mpuTask, &attitude);
    if (snap_status == HAL_OK) {
        wire_flags |= MPU_SNAPSHOT_FLAG_SAMPLE_VALID;
    }
    if ((snapshot.flags & CALIBRATED) != 0u) {
        wire_flags |= MPU_SNAPSHOT_FLAG_CALIBRATED;
    }
    if (attitude_status == HAL_OK && attitude.mag_valid != 0u) {
        wire_flags |= MPU_SNAPSHOT_FLAG_MAG_VALID;
    }
    if ((snapshot.flags & MPU_STATIONARY) != 0u) {
        wire_flags |= MPU_SNAPSHOT_FLAG_STATIONARY;
    }
    if ((snapshot.flags & MPU_CALIBRATING) != 0u) {
        wire_flags |= MPU_SNAPSHOT_FLAG_CALIBRATING;
    }
    if ((snapshot.flags & MPU_ERROR) != 0u) {
        wire_flags |= MPU_SNAPSHOT_FLAG_ERROR;
    }
    payload[offset++] = (snap_status == HAL_OK) ? 0u : 3u;
    payload[offset++] = wire_flags;
    UNER_App_WriteU16Le(&payload[offset], snapshot.sample_seq);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], snapshot.sample_dt_us);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteFloatLe(&payload[offset], (attitude_status == HAL_OK) ? attitude.roll_deg : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (attitude_status == HAL_OK) ? attitude.pitch_deg : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (attitude_status == HAL_OK) ? attitude.yaw_deg : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (float)snapshot.accel_x_mg * 0.001f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (float)snapshot.accel_y_mg * 0.001f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (float)snapshot.accel_z_mg * 0.001f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (float)snapshot.gyro_x_mdps * 0.001f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (float)snapshot.gyro_y_mdps * 0.001f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (float)snapshot.gyro_z_mdps * 0.001f);
    offset = (uint8_t)(offset + 4u);

    if (snapshot_out != NULL) {
        *snapshot_out = snapshot;
    }

    return offset;
}

static void UNER_App_HandleGetMpuSnapshot(const UNER_Packet *packet)
{
    uint8_t payload[MPU_SNAPSHOT_PAYLOAD_SIZE];
    uint8_t len;

    if (packet == NULL) {
        return;
    }

    len = UNER_App_BuildMpuSnapshotPayload(payload, NULL);
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_MPU_SNAPSHOT, payload, len);
}

static uint8_t UNER_App_MpuStreamTransportAllowed(uint8_t transport_id)
{
    return (transport_id == UNER_TR_UART1_ESP ||
            transport_id == UNER_TR_USB_CDC) ? 1u : 0u;
}

static void UNER_App_HandleSetMpuStream(const UNER_Packet *packet)
{
    uint8_t payload[4u];
    uint16_t period_ms = MPU_STREAM_DEFAULT_PERIOD_MS;
    uint8_t status = 0u;
    uint8_t active = 0u;
    if (packet == NULL || packet->payload == NULL || packet->len < 1u) return;
    if (!UNER_App_MpuStreamTransportAllowed(packet->transport_id)) status = 2u;
    if (packet->len >= 3u) {
        period_ms = UNER_App_ReadU16Le(&packet->payload[1]);
        if (period_ms < MPU_STREAM_MIN_PERIOD_MS) period_ms = MPU_STREAM_MIN_PERIOD_MS;
        else if (period_ms > MPU_STREAM_MAX_PERIOD_MS) period_ms = MPU_STREAM_MAX_PERIOD_MS;
    }
    if (status == 0u) {
        if (packet->payload[0] == 0u) {
            UNER_App_StopStreamEndpoint(mpu_streams, packet->transport_id, packet->src);
        } else {
            UNER_StreamState *stream = UNER_App_FindStream(mpu_streams, packet->transport_id,
                                                           packet->src, 1u);
            if (stream == NULL) {
                UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_BUSY);
                return;
            }
            memset(stream, 0, sizeof(*stream));
            stream->active = 1u;
            stream->transport_id = packet->transport_id;
            stream->dst_node = packet->src;
            stream->period_ms = period_ms;
            stream->next_due_ms = HAL_GetTick();
            active = 1u;
        }
    }
    payload[0] = status;
    payload[1] = active;
    UNER_App_WriteU16Le(&payload[2], period_ms);
    (void)UNER_App_Reply(packet, UNER_CMD_ID_SET_MPU_STREAM, payload, sizeof(payload));
    if (status == 0u)
        UNER_App_ShowSimpleNotification(active ? OLED_SIMPLE_NOTIFICATION_MPU_STREAM_STARTED
            : OLED_SIMPLE_NOTIFICATION_MPU_STREAM_STOPPED, 1200u);
}

static void UNER_App_HandleStopMpuStream(const UNER_Packet *packet)
{
    uint8_t payload[1u] = { 0u };
    if (packet == NULL) return;
    UNER_App_StopStreamEndpoint(mpu_streams, packet->transport_id, packet->src);
    (void)UNER_App_Reply(packet, UNER_CMD_ID_STOP_MPU_STREAM, payload, sizeof(payload));
}

static uint16_t UNER_App_NormalizeIrRaw(uint8_t sensor_index, uint16_t raw)
{
    switch ((TCRT5000_SensorId_t)sensor_index) {
        case TCRT5000_SENSOR_PA1:
        case TCRT5000_SENSOR_PA2:
        case TCRT5000_SENSOR_PA0:
            return TCRT5000_NormalizeForDisplay(raw, TCRT5000_POLARITY_PULL_UP);
        default:
            return TCRT5000_NormalizeForDisplay(raw, TCRT5000_POLARITY_PULL_DOWN);
    }
}

static uint8_t UNER_App_BuildIrSnapshotPayload(uint8_t *payload,
                                               uint16_t sample_seq,
                                               uint16_t period_ms,
                                               uint32_t tick_ms)
{
    TCRT5000_RouteSample_t route;
    uint16_t adc_raw[8];
    /* Route and raw channels come from the same completed DMA scan. */
    uint8_t flags = 0u;
    uint8_t offset = 0u;
    uint8_t ir_lights_on;

    if (payload == NULL) {
        return 0u;
    }

    memset(&route, 0, sizeof(route));
    route.alignment = TCRT5000_ROUTE_SEARCH;
    if (TCRT5000_EmittersGroup5IsOn(&irSensors)) {
        flags |= IR_STREAM_FLAG_GROUP5_ON;
    }
    if (TCRT5000_EmittersGroup3IsOn(&irSensors)) {
        flags |= IR_STREAM_FLAG_GROUP3_ON;
    }

    ir_lights_on = (HAL_GPIO_ReadPin(Luces_IR_GPIO_Port, Luces_IR_Pin) == GPIO_PIN_SET) ? 1u : 0u;
    if (ir_lights_on) {
        flags |= IR_STREAM_FLAG_LIGHTS_ON;
    }
    if (GET_CAR_MODE() == FOLLOW_MODE) {
        flags |= IR_STREAM_FLAG_FOLLOW_MODE;
    }


    if (APP_ReadIrRouteSnapshot(adc_raw,&route)) flags |= IR_STREAM_FLAG_ROUTE_VALID;

    payload[offset++] = 0u;
    payload[offset++] = flags;
    UNER_App_WriteU16Le(&payload[offset], sample_seq);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], period_ms);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU32Le(&payload[offset], tick_ms);
    offset = (uint8_t)(offset + 4u);

    for (uint8_t i = 0u; i < TCRT5000_SENSOR_COUNT; ++i) {
        UNER_App_WriteU16Le(&payload[offset], adc_raw[i]);
        offset = (uint8_t)(offset + 2u);
    }

    for (uint8_t i = 0u; i < TCRT5000_SENSOR_COUNT; ++i) {
        const uint16_t raw = adc_raw[i];
        UNER_App_WriteU16Le(&payload[offset], UNER_App_NormalizeIrRaw(i, raw));
        offset = (uint8_t)(offset + 2u);
    }

    payload[offset++] = route.pattern;
    payload[offset++] = (uint8_t)route.alignment;
    payload[offset++] = route.confidence;
    payload[offset++] = route.ambiguous ? 1u : 0u;
    UNER_App_WriteU16Le(&payload[offset], route.lineWidthMm);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteFloatLe(&payload[offset], route.lateralErrorMm);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], route.lateralErrorNorm);
    offset = (uint8_t)(offset + 4u);

    return offset;
}

static uint8_t UNER_App_IrStreamTransportAllowed(uint8_t transport_id)
{
    return (transport_id == UNER_TR_UART1_ESP ||
            transport_id == UNER_TR_USB_CDC) ? 1u : 0u;
}

static void UNER_App_HandleGetIrSnapshot(const UNER_Packet *packet)
{
    uint8_t payload[IR_SNAPSHOT_PAYLOAD_SIZE];
    uint8_t len;

    if (packet == NULL) {
        return;
    }

    ir_snapshot_seq++;
    UNER_StreamState *stream = UNER_App_FindStream(ir_streams, packet->transport_id, packet->src, 0u);
    len = UNER_App_BuildIrSnapshotPayload(payload,
                                          ir_snapshot_seq,
                                          stream ? stream->period_ms : IR_STREAM_DEFAULT_PERIOD_MS,
                                          HAL_GetTick());
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_IR_SNAPSHOT, payload, len);
}

static void UNER_App_HandleSetIrStream(const UNER_Packet *packet)
{
    uint8_t payload[4u];
    uint16_t period_ms = IR_STREAM_DEFAULT_PERIOD_MS;
    uint8_t status = 0u;
    uint8_t active = 0u;
    if (packet == NULL || packet->payload == NULL || packet->len < 1u) return;
    if (!UNER_App_IrStreamTransportAllowed(packet->transport_id)) status = 2u;
    if (packet->len >= 3u) {
        period_ms = UNER_App_ReadU16Le(&packet->payload[1]);
        if (period_ms < IR_STREAM_MIN_PERIOD_MS) period_ms = IR_STREAM_MIN_PERIOD_MS;
        else if (period_ms > IR_STREAM_MAX_PERIOD_MS) period_ms = IR_STREAM_MAX_PERIOD_MS;
    }
    if (status == 0u) {
        if (packet->payload[0] == 0u) {
            UNER_App_StopStreamEndpoint(ir_streams, packet->transport_id, packet->src);
        } else {
            UNER_StreamState *stream = UNER_App_FindStream(ir_streams, packet->transport_id,
                                                           packet->src, 1u);
            if (stream == NULL) {
                UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_BUSY);
                return;
            }
            memset(stream, 0, sizeof(*stream));
            stream->active = 1u;
            stream->transport_id = packet->transport_id;
            stream->dst_node = packet->src;
            stream->period_ms = period_ms;
            stream->next_due_ms = HAL_GetTick();
            active = 1u;
        }
    }
    payload[0] = status;
    payload[1] = active;
    UNER_App_WriteU16Le(&payload[2], period_ms);
    (void)UNER_App_Reply(packet, UNER_CMD_ID_SET_IR_STREAM, payload, sizeof(payload));
}

static void UNER_App_HandleStopIrStream(const UNER_Packet *packet)
{
    uint8_t payload[1u] = { 0u };
    if (packet == NULL) return;
    UNER_App_StopStreamEndpoint(ir_streams, packet->transport_id, packet->src);
    (void)UNER_App_Reply(packet, UNER_CMD_ID_STOP_IR_STREAM, payload, sizeof(payload));
}

static void UNER_App_HandleGetTelemetrySnapshot(const UNER_Packet *packet)
{
    uint8_t payload[UNER_TELEMETRY_PAYLOAD_SIZE];
    const MotionControl_Status_t *motion_status;
    const BalanceControl_Status_t *balance_status;
    MPU9250_Attitude_t attitude;
    uint8_t offset = 0u;

    if (packet == NULL) {
        return;
    }

    memset(payload, 0, sizeof(payload));
    motion_status = MotionControl_GetStatus(&motionControl);
    balance_status = BalanceControl_GetStatus(&balanceControl);
    memset(&attitude, 0, sizeof(attitude));
    const HAL_StatusTypeDef attitude_status = MPU9250_GetAttitude(&mpuTask, &attitude);

    payload[offset++] = (attitude_status == HAL_OK) ? 0u : 3u;
    payload[offset++] = (uint8_t)WifiBridge_GetActiveBackend(&wifiBridge);
    payload[offset++] = WifiBridge_GetMode(&wifiBridge);
    payload[offset++] = WifiBridge_IsConnected(&wifiBridge);
    UNER_App_WriteU32Le(&payload[offset], WifiBridge_GetCapabilities(&wifiBridge));
    offset = (uint8_t)(offset + 4u);
    payload[offset++] = (uint8_t)I2C_Manager_GetPolicy(&i2cManager);
    payload[offset++] = MotionControl_IsArmed(&motionControl);
    payload[offset++] = motion_status ? motion_status->balance_active : 0u;
    payload[offset++] = motion_status ? motion_status->command_active : 0u;
    payload[offset++] = motion_status ? motion_status->safety_stop : 0u;

    UNER_App_WriteFloatLe(&payload[offset], attitude.pitch_deg);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], motion_status ? motion_status->pitch_setpoint_deg : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], motion_status ? motion_status->balance_output_percent : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (motorTask.leftMotor.direction == MOTOR_DIR_BACKWARD ? -1.0f : 1.0f) * motorTask.leftMotor.speedPercent);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], (motorTask.rightMotor.direction == MOTOR_DIR_BACKWARD ? -1.0f : 1.0f) * motorTask.rightMotor.speedPercent);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], balanceControl.config.kp);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], balanceControl.config.ki);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], balanceControl.config.kd);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], attitude.roll_deg);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], attitude.pitch_deg);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset], attitude.yaw_deg);
    offset = (uint8_t)(offset + 4u);

    payload[offset++] = UNER_App_AnyMpuStreamActive();
    UNER_App_WriteU16Le(&payload[offset], UNER_App_MinMpuStreamPeriod());
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], motorTask.leftMotor.min_pwm_forward);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU16Le(&payload[offset], motorTask.rightMotor.min_pwm_forward);
    offset = (uint8_t)(offset + 2u);

    for (uint8_t i = 0u; i < APP_ADC_CHANNEL_COUNT; ++i) {
        UNER_App_WriteU16Le(&payload[offset], autoMicroSystem.runtime.sensor_raw_data[i]);
        offset = (uint8_t)(offset + 2u);
    }

    if (balance_status != NULL) {
        UNER_App_WriteFloatLe(&payload[offset], balance_status->error_deg);
        offset = (uint8_t)(offset + 4u);
        UNER_App_WriteFloatLe(&payload[offset], balance_status->integral_deg_s);
        offset = (uint8_t)(offset + 4u);
        UNER_App_WriteFloatLe(&payload[offset], balance_status->derivative_deg_s);
        offset = (uint8_t)(offset + 4u);
    }

    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_TELEMETRY_SNAPSHOT, payload, offset);
}

static void UNER_App_HandleGetControlSnapshot(const UNER_Packet *packet)
{
    uint8_t payload[UNER_CONTROL_SNAPSHOT_PAYLOAD_SIZE];
    const LineFollow_Status_t *line_status;
    const MotionEstimator_Status_t *est_status;
    const MotionControl_Status_t *motion_status;
    uint8_t offset = 0u;

    if (packet == NULL) {
        return;
    }

    memset(payload, 0, sizeof(payload));
    line_status = LineFollowControl_GetStatus(&lineFollowControl);
    est_status = MotionEstimator_GetStatus(&motionEstimator);
    motion_status = MotionControl_GetStatus(&motionControl);

    payload[offset++] = 0u;
    payload[offset++] = (uint8_t)GET_CAR_MODE();
    payload[offset++] = line_status ? (uint8_t)line_status->fsm : 0u;
    payload[offset++] = line_status ? line_status->stable_pattern : 0u;
    payload[offset++] = line_status ? line_status->confidence_percent : 0u;
    payload[offset++] = line_status ? line_status->ambiguous : 0u;
    payload[offset++] = line_status ? line_status->reject_reason : 0u;
    UNER_App_WriteU16Le(&payload[offset],
                        line_status ? line_status->line_width_mm : 0u);
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteFloatLe(&payload[offset],
                          line_status ? line_status->lateral_error_mm : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset],
                          line_status ? line_status->lateral_error_norm : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset],
                          est_status ? est_status->linear_accel_mps2 : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset],
                          est_status ? est_status->velocity_mps : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset],
                          est_status ? est_status->yaw_rate_dps : 0.0f);
    offset = (uint8_t)(offset + 4u);
    payload[offset++] = est_status ? est_status->confidence_percent : 0u;
    payload[offset++] = est_status ? est_status->flags : 0u;
    UNER_App_WriteFloatLe(&payload[offset],
                          motion_status ? motion_status->u_common_percent : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset],
                          motion_status ? motion_status->u_diff_percent : 0.0f);
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteFloatLe(&payload[offset],
                          motion_status ? motion_status->turn_limit_percent : 0.0f);
    offset = (uint8_t)(offset + 4u);
    payload[offset++] = motion_status ? motion_status->saturation_limited : 0u;
    payload[offset++] = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, USB_ACTIVE) ? 1u : 0u;
    UNER_App_WriteU16Le(&payload[offset], AppPreferences_GetUsbAlivePeriodMs());
    offset = (uint8_t)(offset + 2u);
    UNER_App_WriteU32Le(&payload[offset], APP_USB_GetAliveCount());
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteU32Le(&payload[offset], UNER_TransportUsbCdc_GetRxOverflowCount());
    offset = (uint8_t)(offset + 4u);
    UNER_App_WriteU32Le(&payload[offset], UNER_TransportUsbCdc_GetTxQueueFullCount());
    offset = (uint8_t)(offset + 4u);
    payload[offset++] = UNER_TransportUsbCdc_GetQueuedTxCount();
    UNER_App_WriteU32Le(&payload[offset], UNER_TransportUsbCdc_GetTxQueueDropCount());
    offset = (uint8_t)(offset + 4u);

    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_CONTROL_SNAPSHOT, payload, offset);
}

static void UNER_App_HandleGetFirmwareInfo(const UNER_Packet *packet)
{
    uint8_t payload[1u + (3u * (1u + UNER_TEXT_FIELD_MAX_BYTES))];
    uint8_t offset = 0u;

    if (packet == NULL) {
        return;
    }

    payload[offset++] = 0u;
    if (!UNER_App_AppendTextField(payload, (uint8_t)sizeof(payload), &offset,
                                  firmwareName, UNER_TEXT_FIELD_MAX_BYTES) ||
        !UNER_App_AppendTextField(payload, (uint8_t)sizeof(payload), &offset,
                                  firmwareVersion, UNER_TEXT_FIELD_MAX_BYTES) ||
        !UNER_App_AppendTextField(payload, (uint8_t)sizeof(payload), &offset,
                                  firmwareTarget, UNER_TEXT_FIELD_MAX_BYTES)) {
        payload[0] = 1u;
        offset = 1u;
    }

    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_FIRMWARE_INFO, payload, offset);
}

static void UNER_App_HandleRequestFirmwareCompat(const UNER_Packet *packet)
{
    static const char separator[] = " ";
    const char *fields[] = {
        firmwareName,
        separator,
        firmwareVersion,
        separator,
        firmwareTarget,
    };
    uint8_t payload[33u];
    uint8_t offset = 0u;

    if (packet == NULL) {
        return;
    }

    payload[offset++] = 0u;
    for (uint8_t field = 0u; field < (uint8_t)(sizeof(fields) / sizeof(fields[0])); ++field) {
        const char *text = fields[field];
        while (text != NULL && *text != '\0' && offset < (uint8_t)sizeof(payload)) {
            payload[offset++] = (uint8_t)*text;
            text++;
        }
    }

    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_REQUEST_FIRMWARE, payload, offset);
}

static void UNER_App_HandleGetBuildInfo(const UNER_Packet *packet)
{
    uint8_t payload[1u + (3u * (1u + UNER_BUILD_SHORT_FIELD_MAX_BYTES)) +
                    (1u + UNER_COMPILER_FIELD_MAX_BYTES)];
    uint8_t offset = 0u;

    if (packet == NULL) {
        return;
    }

    payload[offset++] = 0u;
    if (!UNER_App_AppendTextField(payload, (uint8_t)sizeof(payload), &offset,
                                  buildDate, UNER_BUILD_SHORT_FIELD_MAX_BYTES) ||
        !UNER_App_AppendTextField(payload, (uint8_t)sizeof(payload), &offset,
                                  buildTime, UNER_BUILD_SHORT_FIELD_MAX_BYTES) ||
        !UNER_App_AppendTextField(payload, (uint8_t)sizeof(payload), &offset,
                                  buildProfile, UNER_BUILD_SHORT_FIELD_MAX_BYTES) ||
        !UNER_App_AppendTextField(payload, (uint8_t)sizeof(payload), &offset,
                                  buildCompiler, UNER_COMPILER_FIELD_MAX_BYTES)) {
        payload[0] = 1u;
        offset = 1u;
    }

    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_BUILD_INFO, payload, offset);
}

static uint8_t UNER_App_IsPidValueValid(float value)
{
    return (value == value &&
            value >= APP_PREFS_BALANCE_PID_MIN &&
            value <= APP_PREFS_BALANCE_PID_MAX) ? 1u : 0u;
}

/* AMI-20261003-01: all five commands finish with the same 8-byte state. */
static void UNER_App_HandleMotion(const UNER_Packet *packet)
{
    uint8_t status = 0u;
    uint8_t payload[8];
    const MotionControl_Status_t *state;
    if (packet->cmd == UNER_CMD_ID_SET_BALANCE_ENABLED) {
        if (packet->payload[0] > 1u) status = 1u;
        else if (!packet->payload[0]) { (void)APP_BalanceSetEnabled(false); }
        else if (GET_CAR_MODE() == TEST_MODE) status = 2u;
        else {
            if (APP_BalanceSetEnabled(true)!=0u) status=3u;
        }
    } else if (packet->cmd == UNER_CMD_ID_STOP_MOTION) {
        if (packet->payload[0] > 1u) status = 1u;
        else {
            APP_AutoMotionCancel(); APP_LineFollowStop();
            if (GET_CAR_MODE() == TEST_MODE) APP_MotorTestStop();
            MotionControl_Stop(&motionControl, MOTION_STOP_COAST);
            if (packet->payload[0]) MotionControl_Disarm(&motionControl);
        }
    } else if (packet->cmd == UNER_CMD_ID_SET_MOTION || packet->cmd == UNER_CMD_ID_SET_MOTOR_TEST) {
        const float a = UNER_App_ReadFloatLe(packet->payload);
        const float b = UNER_App_ReadFloatLe(&packet->payload[4]);
        const uint16_t duration = UNER_App_ReadU16Le(&packet->payload[8]);
        if (APP_LineCalibrationBusy() || APP_AutoMotionBusy()) status=2u;
        else if (!isfinite(a) || !isfinite(b) || a < -100.0f || a > 100.0f ||
            b < -100.0f || b > 100.0f || duration < 50u || duration > 2000u) status = 1u;
        else if (packet->cmd == UNER_CMD_ID_SET_MOTOR_TEST) {
            if (APP_MotorTestSet(a, b, duration) != HAL_OK) status = 2u;
        } else if (GET_CAR_MODE() != IDLE_MODE && GET_CAR_MODE() != GOTO_MODE) status = 2u;
        else {
            MotionControl_Command_t command;
            MotionControl_GetDefaultCommand(&command);
            command.source = MOTION_SOURCE_WEB;
            command.forward_percent = a;
            command.turn_percent = b;
            command.duration_ms = duration;
            if (MotionControl_SetCommand(&motionControl, &command, HAL_GetTick()) != HAL_OK) status = 3u;
        }
    }
    state = MotionControl_GetStatus(&motionControl);
    payload[0] = status;
    payload[1] = MotionControl_IsArmed(&motionControl);
    payload[2] = state ? state->balance_active : 0u;
    payload[3] = state ? state->command_active : 0u;
    payload[4] = state ? state->safety_stop : 0u;
    payload[5] = state ? (uint8_t)state->reject_reason : MOTION_REJECT_NOT_INITIALIZED;
    payload[6] = (uint8_t)GET_CAR_MODE();
    payload[7] = APP_MotorTestIsActive();
    (void)UNER_App_Reply(packet,
                         packet->cmd, payload, sizeof(payload));
}

static void UNER_App_HandleGetBalancePid(const UNER_Packet *packet)
{
    uint8_t payload[13u];

    if (packet == NULL) {
        return;
    }

    payload[0] = 0u;
    UNER_App_WriteFloatLe(&payload[1], balanceControl.config.kp);
    UNER_App_WriteFloatLe(&payload[5], balanceControl.config.ki);
    UNER_App_WriteFloatLe(&payload[9], balanceControl.config.kd);
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_BALANCE_PID, payload, (uint8_t)sizeof(payload));
}

static void UNER_App_HandleSetBalancePid(const UNER_Packet *packet)
{
    AppPreferencesBalancePid_t savedPid;
    uint8_t payload[14u];
    uint8_t status = 0u;
    uint8_t changed_mask = 0u;
    uint8_t offset = 1u;
    const uint8_t mask = (packet != NULL && packet->payload != NULL) ? packet->payload[0] : 0u;
    float kp = balanceControl.config.kp;
    float ki = balanceControl.config.ki;
    float kd = balanceControl.config.kd;

    if (packet == NULL || packet->payload == NULL || packet->len < 5u) {
        return;
    }

    if (mask == 0u || (mask & (uint8_t)~0x07u) != 0u) {
        status = 2u;
    }

    if (status == 0u && (mask & 0x01u) != 0u) {
        if ((uint16_t)(offset + 4u) > packet->len) {
            status = 1u;
        } else {
            kp = UNER_App_ReadFloatLe(&packet->payload[offset]);
            offset = (uint8_t)(offset + 4u);
            changed_mask |= 0x01u;
        }
    }

    if (status == 0u && (mask & 0x02u) != 0u) {
        if ((uint16_t)(offset + 4u) > packet->len) {
            status = 1u;
        } else {
            ki = UNER_App_ReadFloatLe(&packet->payload[offset]);
            offset = (uint8_t)(offset + 4u);
            changed_mask |= 0x02u;
        }
    }

    if (status == 0u && (mask & 0x04u) != 0u) {
        if ((uint16_t)(offset + 4u) > packet->len) {
            status = 1u;
        } else {
            kd = UNER_App_ReadFloatLe(&packet->payload[offset]);
            offset = (uint8_t)(offset + 4u);
            changed_mask |= 0x04u;
        }
    }

    if (status == 0u && offset != packet->len) {
        status = 1u;
    }

    if (status == 0u &&
        (!UNER_App_IsPidValueValid(kp) ||
         !UNER_App_IsPidValueValid(ki) ||
         !UNER_App_IsPidValueValid(kd))) {
        status = 3u;
    }

    if (status == 0u) {
        if (AppPreferences_SetBalancePid(kp, ki, kd) != HAL_OK) {
            status = 4u;
        } else {
            AppPreferences_GetBalancePid(&savedPid);
            BalanceControl_SetPid(&balanceControl,
                                  savedPid.kp,
                                  savedPid.ki,
                                  savedPid.kd);
            BalanceControl_Reset(&balanceControl);
        }
    }

    payload[0] = status;
    payload[1] = (status == 0u) ? changed_mask : 0u;
    UNER_App_WriteFloatLe(&payload[2], balanceControl.config.kp);
    UNER_App_WriteFloatLe(&payload[6], balanceControl.config.ki);
    UNER_App_WriteFloatLe(&payload[10], balanceControl.config.kd);
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_SET_BALANCE_PID, payload, (uint8_t)sizeof(payload));
}

static uint8_t UNER_App_BuildBootReportPayload(
    uint8_t payload[WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE])
{
    if (payload == NULL) {
        return 0u;
    }

    memset(payload, 0, WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE);
    payload[0] = WIFI_BRIDGE_BOOT_REPORT_SCHEMA;
    payload[1] = uner_boot_mailbox_valid
        ? WIFI_BRIDGE_BOOT_REPORT_STATUS_VALID
        : WIFI_BRIDGE_BOOT_REPORT_STATUS_UNAVAILABLE;
    payload[2] = uner_boot_mailbox_valid
        ? (uint8_t)uner_boot_mailbox.handoff
        : (uint8_t)AUTOMICRO_BOOT_HANDOFF_NONE;
    payload[3] = uner_boot_mailbox_valid
        ? (uint8_t)uner_boot_mailbox.reset_reason_flags
        : 0u;
    payload[4] = uner_boot_mailbox_valid
        ? (uint8_t)uner_boot_mailbox.app_validation_flags
        : 0u;
    payload[5] = (uint8_t)AppPreferences_GetExtProfile();
    if (uner_boot_mailbox_valid) {
        UNER_App_WriteU32Le(&payload[6], uner_boot_mailbox.bootloader_version);
        UNER_App_WriteU32Le(&payload[10], uner_boot_mailbox.app_version);
        UNER_App_WriteU32Le(&payload[14], uner_boot_mailbox.app_size);
        UNER_App_WriteU32Le(&payload[18], uner_boot_mailbox.app_crc32);
    }

    return WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE;
}

static UNER_Status UNER_App_SendBootReport(uint8_t transport_id,
                                          uint8_t destination_node)
{
    uint8_t payload[WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE];
    uint8_t payload_len = UNER_App_BuildBootReportPayload(payload);

    if (payload_len == 0u) {
        return UNER_ERR_LEN;
    }
    if (transport_id == UNER_TR_NRF24_SPI2 &&
        AppPreferences_GetExtProfile() != EXT_PROFILE_NRF24) {
        return UNER_ERR_LEN;
    }
    if (transport_id == UNER_TR_USB_CDC) {
        UNER_TransportUsbCdc_SetNextTxPriority(UNER_USB_CDC_TX_PRIORITY_EVENT);
    }

    return UNER_Send(&uner_handle,
                     transport_id,
                     UNER_NODE_MCU,
                     destination_node,
                     UNER_EVT_MCU_BOOT_REPORT,
                     payload,
                     payload_len);
}

static void UNER_App_ServiceBootReport(void)
{
    if ((uner_boot_report_sent_mask & UNER_BOOT_REPORT_SENT_UART) == 0u &&
        uner_bridge != NULL &&
        WifiBridge_IsConnected(uner_bridge) &&
        WifiBridge_GetActiveBackend(uner_bridge) == WIFI_BRIDGE_BACKEND_WEB &&
        UNER_App_SendBootReport(UNER_TR_UART1_ESP,
                                UNER_NODE_WEB_APP) == UNER_OK) {
        uner_boot_report_sent_mask |= UNER_BOOT_REPORT_SENT_UART;
    }

    if ((uner_boot_report_sent_mask & UNER_BOOT_REPORT_SENT_USB) == 0u &&
        CDC_IsDtrActive_FS() &&
        UNER_App_SendBootReport(UNER_TR_USB_CDC,
                                UNER_NODE_PC_QT) == UNER_OK) {
        uner_boot_report_sent_mask |= UNER_BOOT_REPORT_SENT_USB;
    }

    if ((uner_boot_report_sent_mask & UNER_BOOT_REPORT_SENT_NRF) == 0u &&
        uner_nrf_link_connected &&
        AppPreferences_GetExtProfile() == EXT_PROFILE_NRF24 &&
        UNER_App_SendBootReport(UNER_TR_NRF24_SPI2,
                                UNER_NODE_REMOTE_NRF) == UNER_OK) {
        uner_boot_report_sent_mask |= UNER_BOOT_REPORT_SENT_NRF;
    }
}

static void UNER_App_HandleGetBootReport(const UNER_Packet *packet)
{
    uint8_t payload[WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE];

    if (packet == NULL) {
        return;
    }

    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_BOOT_REPORT,
                         payload,
                         UNER_App_BuildBootReportPayload(payload));
}

static void UNER_App_HandleGetExtensionProfile(const UNER_Packet *packet)
{
    uint8_t payload[2];
    payload[0] = 0;
    payload[1] = AppPreferences_GetExtProfile();
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_GET_EXTENSION_PROFILE, payload, 2);
}

static void UNER_App_HandleSetExtensionProfile(const UNER_Packet *packet)
{
    uint8_t payload[1u] = { 1u };

    if (packet != NULL && packet->payload != NULL && packet->len == 1u) {
        const ext_profile_t requested = (ext_profile_t)packet->payload[0];

        if (requested != EXT_PROFILE_NRF24 &&
            requested != EXT_PROFILE_BEEPER_PB12) {
            payload[0] = 2u;
        } else if (AppPreferences_SetExtProfile(requested) != HAL_OK) {
            payload[0] = 3u;
        } else {
            payload[0] = 0u;
        }
    }
    (void)UNER_App_Reply(packet,
                         UNER_CMD_ID_SET_EXTENSION_PROFILE, payload, 1u);
}

static void UNER_App_HandleRebootIntoProfile(const UNER_Packet *packet)
{
    if (packet == NULL || packet->payload == NULL || packet->len != 1u ||
        AppPreferences_SetExtProfile((ext_profile_t)packet->payload[0]) != HAL_OK) {
        UNER_App_SendCommandNack(packet, UNER_NACK_STATUS_INVALID_ARGUMENT);
        return;
    }
    UNER_App_HandleResetMcu(packet);
}
