/*
 * wifi_bridge_types.h - common API types shared by WifiBridge backends.
 */

#ifndef INC_WIFI_BRIDGE_TYPES_H_
#define INC_WIFI_BRIDGE_TYPES_H_

#include "wifi_bridge_port.h"
#include "wifi_bridge_config.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Optional EVT_BOOT trailer appended after the variable-length WiFi info:
 * [marker, version, reset_reason, exception_cause]. */
#define WIFI_BRIDGE_ESP_BOOT_DIAG_MARKER              0xE5u
#define WIFI_BRIDGE_ESP_BOOT_DIAG_VERSION             0x01u
#define WIFI_BRIDGE_ESP_RESET_REASON_DEFAULT          0u
#define WIFI_BRIDGE_ESP_RESET_REASON_HW_WATCHDOG      1u
#define WIFI_BRIDGE_ESP_RESET_REASON_EXCEPTION        2u
#define WIFI_BRIDGE_ESP_RESET_REASON_SW_WATCHDOG      3u
#define WIFI_BRIDGE_ESP_RESET_REASON_SOFT_RESTART     4u
#define WIFI_BRIDGE_ESP_RESET_REASON_DEEP_SLEEP       5u
#define WIFI_BRIDGE_ESP_RESET_REASON_EXTERNAL         6u
#define WIFI_BRIDGE_ESP_RESET_REASON_UNKNOWN          0xFFu

typedef enum {
    WIFI_BRIDGE_OK = 0,
    WIFI_BRIDGE_ERROR,
    WIFI_BRIDGE_BUSY,
    WIFI_BRIDGE_QUEUE_FULL,
    WIFI_BRIDGE_BAD_ARG,
    WIFI_BRIDGE_TIMEOUT,
    WIFI_BRIDGE_UNSUPPORTED,
} WifiBridge_Result;

typedef enum {
    WIFI_BRIDGE_EVT_NONE = 0,
    WIFI_BRIDGE_EVT_BOOT,
    WIFI_BRIDGE_EVT_MODE_CHANGED,
    WIFI_BRIDGE_EVT_STA_CONNECTED,
    WIFI_BRIDGE_EVT_STA_DISCONNECTED,
    WIFI_BRIDGE_EVT_GOT_IP,
    WIFI_BRIDGE_EVT_SCAN_DONE,
    WIFI_BRIDGE_EVT_AP_STARTED,
    WIFI_BRIDGE_EVT_AP_STOPPED,
    WIFI_BRIDGE_EVT_STATUS_REFRESHED,
    WIFI_BRIDGE_EVT_FIRMWARE_RECEIVED,
    WIFI_BRIDGE_EVT_ERROR,
    WIFI_BRIDGE_EVT_COMMAND_DONE
} WifiBridge_EventId;

typedef struct {
    WifiBridge_EventId id;
    uint8_t mode;
    uint8_t connected;
    uint8_t got_ip;
    uint8_t ip[WIFI_BRIDGE_IP_LEN];
    char ssid[WIFI_BRIDGE_SSID_MAX_LEN + 1u];
    uint8_t status;
} WifiBridge_Event;

typedef union {
    uint8_t raw;
    struct {
        uint8_t busy       : 1;
        uint8_t tx_busy    : 1;
        uint8_t rx_pending : 1;
        uint8_t got_ok     : 1;
        uint8_t got_error  : 1;
        uint8_t got_ip     : 1;
        uint8_t connected  : 1;
        uint8_t reserved   : 1;
    } bits;
} WifiBridge_Flags;

typedef void (*WifiBridge_EventCallback)(const WifiBridge_Event *evt);

#define WIFI_FLAG_BUSY        bits.busy
#define WIFI_FLAG_TX_BUSY     bits.tx_busy
#define WIFI_FLAG_RX_PENDING  bits.rx_pending
#define WIFI_FLAG_GOT_OK      bits.got_ok
#define WIFI_FLAG_GOT_ERROR   bits.got_error
#define WIFI_FLAG_GOT_IP      bits.got_ip
#define WIFI_FLAG_CONNECTED   bits.connected

#define WIFI_FLAG_SET(h, flag)      ((h)->flags.flag = 1u)
#define WIFI_FLAG_CLEAR(h, flag)    ((h)->flags.flag = 0u)
#define WIFI_FLAG_IS_SET(h, flag)   ((h)->flags.flag != 0u)

typedef struct {
    uint8_t asset_id;
    uint8_t width;
    uint8_t height;
    uint16_t len;
    const uint8_t *data;
} WifiBridge_BinaryAssetView;

typedef union {
    uint8_t raw;
    struct {
        uint8_t ssid_set     : 1;
        uint8_t password_set : 1;
        uint8_t reserved     : 6;
    } bits;
} WifiBridge_CredentialFlags;

typedef struct {
    WifiBridge_CredentialFlags flags;
    char ssid[WIFI_BRIDGE_SSID_MAX_LEN + 1u];
    char password[WIFI_BRIDGE_PASS_MAX_LEN + 1u];
} WifiBridge_Credentials;

typedef enum {
    WIFI_BRIDGE_MODE_UNKNOWN = 0u,
    WIFI_BRIDGE_MODE_STA     = 1u,
    WIFI_BRIDGE_MODE_AP      = 2u,
    WIFI_BRIDGE_MODE_AP_STA  = 3u,
} WifiBridge_Mode;

typedef enum {
    WIFI_BRIDGE_BACKEND_DETECTING = 0u,
    WIFI_BRIDGE_BACKEND_AT,
    WIFI_BRIDGE_BACKEND_WEB,
} WifiBridge_Backend;
typedef enum {
    WIFI_BRIDGE_NETWORK_TCP = 0u,
    WIFI_BRIDGE_NETWORK_UDP = 1u,
} WifiBridgeNetworkProtocol;


typedef uint32_t WifiBridge_CapabilityMask_t;

#define WIFI_BRIDGE_CAP_ALIVE              (1u << 0)
#define WIFI_BRIDGE_CAP_STATUS             (1u << 1)
#define WIFI_BRIDGE_CAP_FIRMWARE           (1u << 2)
#define WIFI_BRIDGE_CAP_WIFI_STA           (1u << 3)
#define WIFI_BRIDGE_CAP_WIFI_AP            (1u << 4)
#define WIFI_BRIDGE_CAP_WIFI_SCAN          (1u << 5)
#define WIFI_BRIDGE_CAP_REMOTE_UI          (1u << 6)
#define WIFI_BRIDGE_CAP_PREFERENCES        (1u << 7)
#define WIFI_BRIDGE_CAP_I2C_POLICY         (1u << 8)
#define WIFI_BRIDGE_CAP_MPU_SNAPSHOT       (1u << 9)
#define WIFI_BRIDGE_CAP_MPU_STREAM         (1u << 10)
#define WIFI_BRIDGE_CAP_BINARY_ASSETS      (1u << 11)
#define WIFI_BRIDGE_CAP_MOTION_CONTROL     (1u << 12)
#define WIFI_BRIDGE_CAP_SENSOR_TELEMETRY   (1u << 13)
#define WIFI_BRIDGE_CAP_REMOTE_AUTH        (1u << 14)
#define WIFI_BRIDGE_CAP_IR_STREAM          (1u << 15)
#define WIFI_BRIDGE_CAP_AT_TCP             (1u << 16)
#define WIFI_BRIDGE_CAP_AT_UDP             (1u << 17)

typedef enum {
    WIFI_BRIDGE_PROBE_AT = 0u,
    WIFI_BRIDGE_PROBE_WEB,
} WifiBridge_Probe;

typedef enum {
    WIFI_BRIDGE_AT_OP_NONE = 0u,
    WIFI_BRIDGE_AT_OP_SET_STA,
    WIFI_BRIDGE_AT_OP_SET_AP,
    WIFI_BRIDGE_AT_OP_CONNECT,
    WIFI_BRIDGE_AT_OP_DISCONNECT,
    WIFI_BRIDGE_AT_OP_SCAN,
    WIFI_BRIDGE_AT_OP_STATUS_MODE,
    WIFI_BRIDGE_AT_OP_STATUS_LINK,
    WIFI_BRIDGE_AT_OP_STATUS_IP,
    WIFI_BRIDGE_AT_OP_FIRMWARE,
    WIFI_BRIDGE_AT_OP_REBOOT,
    WIFI_BRIDGE_AT_OP_PING,
    WIFI_BRIDGE_AT_OP_START_AP_MODE,
    WIFI_BRIDGE_AT_OP_APPLY_AP_IP,
    WIFI_BRIDGE_AT_OP_START_AP,
    WIFI_BRIDGE_AT_OP_STOP_AP,
    WIFI_BRIDGE_AT_OP_CLEAR_CREDENTIALS,
    WIFI_BRIDGE_AT_OP_AUTO_RECONNECT,
    WIFI_BRIDGE_AT_OP_FACTORY_RESET,
    WIFI_BRIDGE_AT_OP_NETWORK_CONFIG,
    WIFI_BRIDGE_AT_OP_NETWORK_CLOSE,
    WIFI_BRIDGE_AT_OP_CIPSEND,
} WifiBridge_AT_Operation;

typedef enum {
    WIFI_BRIDGE_AT_SEND_IDLE = 0u,
    WIFI_BRIDGE_AT_SEND_WAIT_PROMPT,
    WIFI_BRIDGE_AT_SEND_PAYLOAD_DMA,
    WIFI_BRIDGE_AT_SEND_WAIT_RESULT,
} WifiBridge_AT_SendState;

typedef struct {
    uint32_t at_timeouts;
    uint32_t send_failures;
    uint32_t queue_drops;
    uint32_t ipd_header_errors;
    uint32_t udp_datagram_drops;
    uint32_t rejected_peers;
} WifiBridge_AT_NetworkCounters;

typedef struct WifiBridge_Handle
{
    void *huart;
    const WifiBridge_Hardware *hardware;
    const WifiBridge_Services *services;
    void *services_ctx;
    SystemHandle *system;
    WifiBridge_Credentials *credentials;
    WifiBridge_Flags flags;
    WifiBridge_EventCallback on_event;
    WifiBridge_Event last_event;
    uint8_t mode;
    uint8_t status;
    uint8_t ip[WIFI_BRIDGE_IP_LEN];
    char ssid[WIFI_BRIDGE_SSID_MAX_LEN + 1u];

    uint8_t rx_dma[WIFI_BRIDGE_UART_RX_DMA_SIZE];
    uint8_t rx_started;
    volatile uint8_t tx_busy;

    WifiBridge_Backend active_backend;
    WifiBridge_Probe probe;
    uint32_t last_probe_ms;
    uint32_t last_rx_ms;
    uint8_t alive_pending;
    uint8_t missed_alive_count;
    uint8_t connection_lost_event;

    uint16_t at_rx_last_pos;
    char at_line[WIFI_BRIDGE_AT_LINE_SIZE];
    uint16_t at_line_len;
    char at_tx_buf[WIFI_BRIDGE_AT_TX_SIZE];
    uint16_t at_tx_len;
    uint32_t at_deadline_ms;
    WifiBridge_AT_Operation at_active_op;
    uint8_t at_scan_count;
    uint8_t at_status_sta_ip_seen;
    uint8_t at_status_ap_ip_seen;
    uint8_t at_reboot_pending;
    uint8_t at_reboot_mode;
    WifiBridge_CapabilityMask_t at_detected_capabilities;
    WifiBridgeNetworkProtocol at_network_protocol;
    uint8_t at_network_stage;
    uint8_t at_network_restart_pending;
    uint8_t at_network_ready;
    int8_t at_tcp_link_id;
    int8_t at_pending_close_link_id;
    uint8_t at_udp_peer_valid;
    uint8_t at_udp_peer_ip[WIFI_BRIDGE_IP_LEN];
    uint16_t at_udp_peer_port;
    uint32_t at_udp_peer_last_rx_ms;
    char at_ipd_header[WIFI_BRIDGE_AT_IPD_HEADER_SIZE];
    uint16_t at_ipd_header_len;
    uint16_t at_ipd_expected_len;
    uint16_t at_ipd_received_len;
    int8_t at_ipd_link_id;
    uint8_t at_ipd_peer_ip[WIFI_BRIDGE_IP_LEN];
    uint16_t at_ipd_peer_port;
    uint8_t at_ipd_active;
    uint8_t at_ipd_payload[WIFI_BRIDGE_AT_MAX_FRAME_SIZE];
    uint8_t at_frame_queue[WIFI_BRIDGE_AT_TX_QUEUE_SLOTS][WIFI_BRIDGE_AT_MAX_FRAME_SIZE];
    uint16_t at_frame_queue_len[WIFI_BRIDGE_AT_TX_QUEUE_SLOTS];
    uint8_t at_frame_queue_head;
    uint8_t at_frame_queue_tail;
    uint8_t at_frame_queue_count;
    WifiBridge_AT_SendState at_send_state;
    uint8_t at_send_payload_started;
    WifiBridge_AT_NetworkCounters at_network_counters;
} WifiBridge_Handle;

#define WIFI_BRIDGE_CMD_ID_SET_MODE_AP                 0x10u
#define WIFI_BRIDGE_CMD_ID_SET_MODE_STA                0x11u
#define WIFI_BRIDGE_CMD_ID_SET_CREDENTIALS             0x12u
#define WIFI_BRIDGE_CMD_ID_CLEAR_CREDENTIALS           0x13u
#define WIFI_BRIDGE_CMD_ID_START_SCAN                  0x14u
#define WIFI_BRIDGE_CMD_ID_GET_SCAN_RESULTS            0x15u
#define WIFI_BRIDGE_CMD_ID_REBOOT_ESP                  0x16u
#define WIFI_BRIDGE_CMD_ID_FACTORY_RESET               0x17u
#define WIFI_BRIDGE_CMD_ID_STOP_SCAN                   0x18u
#define WIFI_BRIDGE_CMD_ID_RESET_MCU                   0x19u
#define WIFI_BRIDGE_CMD_ID_SET_AP_FALLBACK             0x1Bu
#define WIFI_BRIDGE_CMD_ID_GET_AP_FALLBACK             0x1Cu
#define WIFI_BRIDGE_CMD_ID_LINE_CALIBRATION_CONTROL    0x1Du
#define WIFI_BRIDGE_CMD_ID_GET_LINE_STATE              0x1Eu
#define WIFI_BRIDGE_CMD_ID_SET_LINE_FOLLOW              0x1Fu
#define WIFI_BRIDGE_CMD_ID_WIFI_GET_DETAIL             0x1Au
#define WIFI_BRIDGE_CMD_ID_GET_STATUS                  0x30u
#define WIFI_BRIDGE_CMD_ID_PING                        0x31u
#define WIFI_BRIDGE_CMD_ID_ALIVE                       WIFI_BRIDGE_CMD_ID_PING
#define WIFI_BRIDGE_CMD_ID_GET_PREFERENCES             0x40u
#define WIFI_BRIDGE_CMD_ID_REQUEST_FIRMWARE            0x41u
#define WIFI_BRIDGE_CMD_ID_ECHO                        0x42u
#define WIFI_BRIDGE_CMD_ID_SET_ENCODER_FAST            0x43u
#define WIFI_BRIDGE_CMD_ID_GET_CONNECTED_USERS         0x44u
#define WIFI_BRIDGE_CMD_ID_GET_USER_INFO               0x45u
#define WIFI_BRIDGE_CMD_ID_GET_INTERFACES_CONNECTED    0x46u
#define WIFI_BRIDGE_CMD_ID_GET_CREDENTIALS             0x47u
#define WIFI_BRIDGE_CMD_ID_CONNECT_WIFI                0x48u
#define WIFI_BRIDGE_CMD_ID_DISCONNECT_WIFI             0x49u
#define WIFI_BRIDGE_CMD_ID_SET_AP_CONFIG               0x4Au
#define WIFI_BRIDGE_CMD_ID_START_AP                    0x4Bu
#define WIFI_BRIDGE_CMD_ID_STOP_AP                     0x4Cu
#define WIFI_BRIDGE_CMD_ID_GET_CONNECTED_USERS_MODE    0x4Du
#define WIFI_BRIDGE_CMD_ID_SET_AUTO_RECONNECT          0x4Eu
#define WIFI_BRIDGE_CMD_ID_AUTH_VALIDATE_PIN           0x51u
#define WIFI_BRIDGE_CMD_ID_GET_CURRENT_SCREEN          0x52u
#define WIFI_BRIDGE_CMD_ID_MENU_ITEM_CLICK             0x53u
#define WIFI_BRIDGE_CMD_ID_TRIGGER_ENCODER_BUTTON      0x54u
#define WIFI_BRIDGE_CMD_ID_TRIGGER_USER_BUTTON         0x55u
#define WIFI_BRIDGE_CMD_ID_REQUEST_SCREEN_PAGE         0x56u
#define WIFI_BRIDGE_CMD_ID_TRIGGER_ENCODER_ROTATE_LEFT 0x57u
#define WIFI_BRIDGE_CMD_ID_TRIGGER_ENCODER_ROTATE_RIGHT 0x58u
#define WIFI_BRIDGE_CMD_ID_AUTH_PIN_GRANTED            0x59u
#define WIFI_BRIDGE_CMD_ID_BINARY_ASSET_STREAM         0x5Au
#define WIFI_BRIDGE_CMD_ID_GET_CAR_MODE                0x5Bu
#define WIFI_BRIDGE_CMD_ID_WIFI_CREDENTIALS_WEB_RESULT 0x5Cu
#define WIFI_BRIDGE_CMD_ID_AUTH_REMOTE_RESULT          0x5Du
#define WIFI_BRIDGE_CMD_ID_SET_I2C_POLICY              0x5Eu
#define WIFI_BRIDGE_CMD_ID_GET_I2C_POLICY              0x5Fu
#define WIFI_BRIDGE_CMD_ID_GET_MPU_SNAPSHOT            0x60u
#define WIFI_BRIDGE_CMD_ID_SET_MPU_STREAM              0x61u
#define WIFI_BRIDGE_CMD_ID_STOP_MPU_STREAM             0x62u
#define WIFI_BRIDGE_CMD_ID_SET_PREFERENCES             0x63u
#define WIFI_BRIDGE_CMD_ID_GET_TELEMETRY_SNAPSHOT      0x64u
#define WIFI_BRIDGE_CMD_ID_GET_BALANCE_PID             0x65u
#define WIFI_BRIDGE_CMD_ID_SET_BALANCE_PID             0x66u
#define WIFI_BRIDGE_CMD_ID_GET_CONTROL_SNAPSHOT        0x67u
#define WIFI_BRIDGE_CMD_ID_GET_FIRMWARE_INFO           0x68u
#define WIFI_BRIDGE_CMD_ID_GET_AP_CONFIG               0x77u
#define WIFI_BRIDGE_CMD_ID_SET_AP_CONFIG_DESIRED            0x78u

#define WIFI_BRIDGE_AP_CONFIG_STATUS_OK                 0u
#define WIFI_BRIDGE_AP_CONFIG_STATUS_STORAGE_FAILURE    1u
#define WIFI_BRIDGE_AP_CONFIG_STATUS_INVALID_VALUE      2u

#define WIFI_BRIDGE_PREFS_AP_EXTENSION_VERSION          1u
#define WIFI_BRIDGE_PREFS_AP_FLAG_PASSWORD_PROTECTED   (1u << 0)
#define WIFI_BRIDGE_PREFS_AP_FLAG_PASSWORD_SET         (1u << 1)
#define WIFI_BRIDGE_CMD_ID_GET_BUILD_INFO              0x69u
#define WIFI_BRIDGE_CMD_ID_GET_IR_SNAPSHOT             0x6Au
#define WIFI_BRIDGE_CMD_ID_SET_IR_STREAM               0x6Bu
#define WIFI_BRIDGE_CMD_ID_STOP_IR_STREAM              0x6Cu
#define WIFI_BRIDGE_CMD_ID_OLED_CANVAS_TRANSFER        0x6Du
#define WIFI_BRIDGE_CMD_ID_SET_CAR_MODE                0x6Eu
#define WIFI_BRIDGE_CMD_ID_GET_BOOT_REPORT             0x6Fu
#define WIFI_BRIDGE_BOOT_REPORT_SCHEMA                  0x01u
#define WIFI_BRIDGE_BOOT_REPORT_PAYLOAD_SIZE            22u
#define WIFI_BRIDGE_BOOT_REPORT_STATUS_VALID            0x00u
#define WIFI_BRIDGE_BOOT_REPORT_STATUS_UNAVAILABLE      0x01u
#define WIFI_BRIDGE_CMD_ID_WIFI_CREDENTIALS_FLOW       0x70u
#define WIFI_BRIDGE_CMD_ID_AP_CLIENTS_PAGE              0x71u
#define WIFI_BRIDGE_CMD_ID_AP_CLIENT_DISCONNECT         0x72u
#define WIFI_BRIDGE_CMD_ID_GET_EXTENSION_PROFILE        0x73u
#define WIFI_BRIDGE_CMD_ID_SET_EXTENSION_PROFILE        0x74u
#define WIFI_BRIDGE_CMD_ID_GET_EXTENSION_PIN_MAP        0x75u
#define WIFI_BRIDGE_CMD_ID_REBOOT_INTO_PROFILE          0x76u
#define WIFI_BRIDGE_CMD_ID_GET_IR_EMITTERS             0x79u
#define WIFI_BRIDGE_CMD_ID_SET_IR_EMITTERS             0x7Au
#define WIFI_BRIDGE_CMD_ID_GET_MOTION_STATE 0x7Bu
#define WIFI_BRIDGE_CMD_ID_SET_BALANCE_ENABLED 0x7Cu
#define WIFI_BRIDGE_CMD_ID_SET_MOTION 0x7Du
#define WIFI_BRIDGE_CMD_ID_STOP_MOTION 0x7Eu
#define WIFI_BRIDGE_CMD_ID_SET_MOTOR_TEST 0x7Fu

#define WIFI_BRIDGE_EVT_ID_SCREEN_CHANGED              0x95u
#define WIFI_BRIDGE_EVT_ID_MENU_SELECTION_CHANGED      0x96u
#define WIFI_BRIDGE_EVT_ID_CAR_MODE_CHANGED            0x97u
#define WIFI_BRIDGE_EVT_ID_IR_EMITTERS_CHANGED         0x98u
#define WIFI_BRIDGE_EVT_ID_MPU_STREAM                  0x90u
#define WIFI_BRIDGE_EVT_ID_IR_STREAM                   0x91u
#define WIFI_BRIDGE_EVT_ID_MCU_BOOT_REPORT              0x9Fu

#define WIFI_BRIDGE_SELECTION_INDEX_INVALID            0xFFu
#define WIFI_BRIDGE_SELECTION_PAYLOAD_SIZE             7u

#define WIFI_BRIDGE_AUTH_SCOPE_REMOTE_SESSION          0x00u
#define WIFI_BRIDGE_AUTH_SCOPE_LOCAL_PERMISSION        0x01u
#define WIFI_BRIDGE_AUTH_SCOPE_CHANGE_PIN              0x02u

#define WIFI_BRIDGE_AUTH_SESSION_ACTION_LOGOUT         0x00u
#define WIFI_BRIDGE_AUTH_SESSION_ACTION_QUERY          0x01u

#define WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_SUCCESS  0x00u
#define WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_FAILED   0x01u
#define WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_TIMEOUT  0x02u
#define WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_CANCELED 0x03u

#define WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_NORMAL          0x00u
#define WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_AP              0x01u

#define WIFI_BRIDGE_BINARY_ASSET_ID_GITHUB_QR            0x01u
#define WIFI_BRIDGE_BINARY_ASSET_MAX_BYTES               1024u

#ifdef __cplusplus
}
#endif

#endif /* INC_WIFI_BRIDGE_TYPES_H_ */
