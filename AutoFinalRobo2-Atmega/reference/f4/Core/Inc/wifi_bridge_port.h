#ifndef WIFI_BRIDGE_PORT_H
#define WIFI_BRIDGE_PORT_H
#include "am_port.h"
#include "system.h"
#include "app_ap_config_data.h"
#include "binary_asset_types.h"
#include "uner_v2.h"
#include "uner_status.h"
typedef struct WifiBridge_Handle WifiBridge_Handle;
typedef struct {
    uint32_t (*now_ms)(void *ctx);
    AM_Result (*rx_start)(void *ctx, uint8_t *buffer, uint16_t length);
    AM_Result (*tx)(void *ctx, const uint8_t *data, uint16_t length);
    void (*enable_idle)(void *ctx);
    bool (*rx_ready)(void *ctx);
    uint16_t (*rx_remaining)(void *ctx);
} WifiBridge_Hardware;
typedef struct {
    void (*AppApConfig_Get)(void *ctx, AppApConfig_t *out);
    void (*AppApConfig_OnEspApplyResponse)(void *ctx, uint8_t status);
    uint8_t (*AppPreferences_GetWifiAtProtocol)(void *ctx);
    void (*Permission_RevokeRemoteNode)(void *ctx, uint8_t source_node);
    void (*UNER_App_FeedUart1Bytes)(void *ctx, const uint8_t *data, uint16_t len);
    uint8_t (*UNER_App_GetBinaryAsset)(void *ctx, uint8_t asset_id, BinaryAssetView_t *asset);
    uint8_t (*UNER_App_GetWaitingCommandId)(void *ctx);
    UNER_Status (*UNER_App_Init)(void *ctx, WifiBridge_Handle *bridge);
    uint8_t (*UNER_App_IsWaitingValidation)(void *ctx);
    void (*UNER_App_NotifyUart1Rx)(void *ctx);
    void (*UNER_App_OnAtNetworkDisconnected)(void *ctx);
    void (*UNER_App_OnUart1TxComplete)(void *ctx);
    void (*UNER_App_Poll)(void *ctx);
    UNER_Status (*UNER_App_ReportCarModeChanged)(void *ctx, uint8_t mode);
    UNER_Status (*UNER_App_ReportMenuSelectionChanged)(void *ctx, uint32_t screen_code,
                                                uint8_t selected_index,
                                                uint8_t item_count,
                                                uint8_t source);
    UNER_Status (*UNER_App_ReportScreenChanged)(void *ctx, uint32_t screen_code, uint8_t source);
    UNER_Status (*UNER_App_RequestBinaryAsset)(void *ctx, uint8_t asset_id);
    UNER_Status (*UNER_App_RequestWifiNetworkDetail)(void *ctx, const char *ssid);
    void (*UNER_App_ResetUart1RxPos)(void *ctx);
    UNER_Status (*UNER_App_SendCommand)(void *ctx, uint8_t cmd, const uint8_t *payload, uint8_t len);
    UNER_Status (*UNER_App_SendEspReboot)(void *ctx, uint8_t boot_mode);
    UNER_Status (*UNER_App_SendWifiCredentialRequest)(void *ctx, const char *ssid);
    uint8_t (*checksum)(const uint8_t *data, uint16_t length);
    void (*clear_scan_results)(void);
} WifiBridge_Services;
#endif
