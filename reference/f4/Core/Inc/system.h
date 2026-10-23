#ifndef AUTOMICRO_SYSTEM_H
#define AUTOMICRO_SYSTEM_H
#include <stdbool.h>
#include <stdint.h>
#include "types/bitmap_type.h"
#include "types/IPStruct_t.h"
#include "types/carmode_type.h"
#define WIFI_SCAN_MAX_NETWORKS 8u
#define WIFI_SSID_MAX_LEN 32u
#define WIFI_ENCRYPTION_MAX_LEN 16u
#define WIFI_SELECTED_NETWORK_INVALID 0xFFu
typedef struct {
    IPStruct_t staIp;
    IPStruct_t apIp;
    char staSsid[WIFI_SSID_MAX_LEN + 1u];
    char apSsid[WIFI_SSID_MAX_LEN + 1u];
    char apPassword[65u];
    uint8_t staIpValid;
    uint8_t apIpValid;
    uint8_t apClients;
    uint8_t apInfoValid;
} ESPWiFiConnectionInfo_t;
typedef struct {
    volatile Byte_Flag_Struct systemFlags;
    volatile Byte_Flag_Struct systemFlags2;
    volatile Byte_Flag_Struct systemFlags3;
    volatile Byte_Flag_Struct carModeFlags;
    volatile uint16_t sensor_raw_data[8];
    volatile uint8_t inside_menu_flag;
    volatile uint8_t encoder_fast_scroll_enabled;
    volatile bool oled_first_draw;
    volatile bool mpu_trigger;
    CarMode_t auxCarMode;
} SystemRuntime;
typedef struct {
    uint16_t wifiSearchingTimeout;
    uint8_t networksFound;
    char wifiNetworkSsids[WIFI_SCAN_MAX_NETWORKS][WIFI_SSID_MAX_LEN + 1u];
    char wifiNetworkEncryptions[WIFI_SCAN_MAX_NETWORKS][WIFI_ENCRYPTION_MAX_LEN + 1u];
    int8_t wifiNetworkSignalStrengths[WIFI_SCAN_MAX_NETWORKS];
    uint8_t wifiNetworkChannels[WIFI_SCAN_MAX_NETWORKS];
    uint8_t wifiNetworkEncryptionTypes[WIFI_SCAN_MAX_NETWORKS];
    uint8_t wifiNetworkDetailValid[WIFI_SCAN_MAX_NETWORKS];
    uint8_t wifiSelectedNetworkIndex;
    volatile uint8_t wifiScanSessionActive;
    volatile uint8_t wifiScanResultsPending;
    volatile IPStruct_t espStaIp;
    volatile IPStruct_t espApIp;
    ESPWiFiConnectionInfo_t espWifiConnection;
    char espFirmwareVersion[33];
} SystemConnectivity;
typedef struct {
    SystemRuntime runtime;
    SystemConnectivity connectivity;
    struct { const void *preferences; uint32_t capabilities; bool ir_objects_enabled, ir_line_enabled, ir_lights_enabled; } configuration;
    struct { void *permissions; } security;
    struct { const char *name; const char *version; const char *target; const char *build_date, *build_time, *build_profile, *build_compiler; } metadata;
    struct {
        void *encoder, *button, *motors, *mpu, *ir, *i2c, *menu, *display;
        void *balance, *motion, *estimator, *line, *wifi, *protocol;
        void *screens, *tasks, *events, *preferences;
        void *oled_ui, *favorites, *ap_config, *ui_router;
    } handles;
} SystemHandle;
extern SystemHandle autoMicroSystem;
#endif
