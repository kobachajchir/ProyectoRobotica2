/*
 * wifi_bridge_config.h
 *
 * Shared ESP bridge configuration for the STM32F411 port. Both backends are
 * built: ESP-AT text commands and the WEB/UNER frame protocol.
 */

#ifndef INC_WIFI_BRIDGE_CONFIG_H_
#define INC_WIFI_BRIDGE_CONFIG_H_

#define WIFI_BRIDGE_SSID_MAX_LEN          32u
#define WIFI_BRIDGE_PASS_MAX_LEN          64u
#define WIFI_BRIDGE_IP_LEN                 4u

#define WIFI_BRIDGE_UART_RX_DMA_SIZE      512u
#define WIFI_BRIDGE_AT_RX_DMA_SIZE       512u
#define WIFI_BRIDGE_AT_TX_SIZE           256u
#define WIFI_BRIDGE_AT_LINE_SIZE         160u
#define WIFI_BRIDGE_AT_TIMEOUT_MS       8000u
#define WIFI_BRIDGE_AT_PING_TIMEOUT_MS   150u
#define WIFI_BRIDGE_AT_SCAN_TIMEOUT_MS 15000u
#define WIFI_BRIDGE_AT_JOIN_TIMEOUT_MS 20000u
#define WIFI_BRIDGE_AT_NETWORK_PORT     3333u
#define WIFI_BRIDGE_AT_TX_QUEUE_SLOTS      4u
#define WIFI_BRIDGE_AT_MAX_FRAME_SIZE    265u
#define WIFI_BRIDGE_AT_IPD_HEADER_SIZE    80u
#define WIFI_BRIDGE_AT_UDP_PEER_TIMEOUT_MS 7000u

#define WIFI_BRIDGE_ALIVE_PERIOD_MS       50u
#define WIFI_BRIDGE_AT_ALIVE_PERIOD_MS  1000u
#define WIFI_BRIDGE_ALIVE_MAX_MISSES       3u
#define WIFI_BRIDGE_WEB_ALIVE_MAX_MISSES  10u
#define WIFI_BRIDGE_PROBE_TIMEOUT_MS      150u

/*
 * Factory AT firmware has no WEB credential flow. If the UI asks the AT backend
 * to connect from a scanned SSID, this build-time password is used.
 */
#ifndef WIFI_BRIDGE_AT_DEFAULT_PASSWORD
#define WIFI_BRIDGE_AT_DEFAULT_PASSWORD ""
#endif

#ifndef WIFI_BRIDGE_AT_AP_DEFAULT_CHANNEL
#define WIFI_BRIDGE_AT_AP_DEFAULT_CHANNEL   6u
#endif

#ifndef WIFI_BRIDGE_AT_AP_DEFAULT_ENCRYPTION
#define WIFI_BRIDGE_AT_AP_DEFAULT_ENCRYPTION 3u
#endif

#ifndef WIFI_BRIDGE_AT_AP_DEFAULT_MAX_CONN
#define WIFI_BRIDGE_AT_AP_DEFAULT_MAX_CONN  4u
#endif

#endif /* INC_WIFI_BRIDGE_CONFIG_H_ */
