#ifndef APP_WIFI_PORTS_H
#define APP_WIFI_PORTS_H
#include "main.h"
#include "wifi_bridge.h"
#include "uner_app.h"
#include "permissions.h"
#include "app_preferences_binding.h"
#include "app_ap_config.h"
#include "screenWrappers.h"
static inline uint32_t AppWifiTick(void *ctx) { (void)ctx; return HAL_GetTick(); }
static inline AM_Result AppWifiRx(void *ctx,uint8_t *data,uint16_t len) { return HAL_UART_Receive_DMA(ctx,data,len); }
static inline AM_Result AppWifiTx(void *ctx,const uint8_t *data,uint16_t len) { return HAL_UART_Transmit_DMA(ctx,(uint8_t *)data,len); }
static inline void AppWifiIdle(void *ctx) { UART_HandleTypeDef *uart=ctx; __HAL_UART_ENABLE_IT(uart,UART_IT_IDLE); }
static inline bool AppWifiReady(void *ctx) { return ((UART_HandleTypeDef *)ctx)->hdmarx != NULL; }
static inline uint16_t AppWifiRemaining(void *ctx) { return __HAL_DMA_GET_COUNTER(((UART_HandleTypeDef *)ctx)->hdmarx); }
static const WifiBridge_Hardware appWifiHardware={AppWifiTick,AppWifiRx,AppWifiTx,AppWifiIdle,AppWifiReady,AppWifiRemaining};
static inline void AppWifi_AppApConfig_Get(void *ctx, AppApConfig_t *out) { (void)ctx; AppApConfig_Get(out); }
static inline void AppWifi_AppApConfig_OnEspApplyResponse(void *ctx, uint8_t status) { (void)ctx; AppApConfig_OnEspApplyResponse(status); }
static inline uint8_t AppWifi_AppPreferences_GetWifiAtProtocol(void *ctx) { (void)ctx; return AppPreferences_GetWifiAtProtocol(); }
static inline void AppWifi_Permission_RevokeRemoteNode(void *ctx, uint8_t source_node) { (void)ctx; Permission_RevokeRemoteNode(source_node); }
static inline void AppWifi_UNER_App_FeedUart1Bytes(void *ctx, const uint8_t *data, uint16_t len) { (void)ctx; UNER_App_FeedUart1Bytes(data, len); }
static inline uint8_t AppWifi_UNER_App_GetBinaryAsset(void *ctx, uint8_t asset_id, BinaryAssetView_t *asset) { (void)ctx; return UNER_App_GetBinaryAsset(asset_id, asset); }
static inline uint8_t AppWifi_UNER_App_GetWaitingCommandId(void *ctx) { (void)ctx; return UNER_App_GetWaitingCommandId(); }
static inline UNER_Status AppWifi_UNER_App_Init(void *ctx, WifiBridge_Handle *bridge) { (void)ctx; return UNER_App_Init(bridge); }
static inline uint8_t AppWifi_UNER_App_IsWaitingValidation(void *ctx) { (void)ctx; return UNER_App_IsWaitingValidation(); }
static inline void AppWifi_UNER_App_NotifyUart1Rx(void *ctx) { (void)ctx; UNER_App_NotifyUart1Rx(); }
static inline void AppWifi_UNER_App_OnAtNetworkDisconnected(void *ctx) { (void)ctx; UNER_App_OnAtNetworkDisconnected(); }
static inline void AppWifi_UNER_App_OnUart1TxComplete(void *ctx) { (void)ctx; UNER_App_OnUart1TxComplete(); }
static inline void AppWifi_UNER_App_Poll(void *ctx) { (void)ctx; UNER_App_Poll(); }
static inline UNER_Status AppWifi_UNER_App_ReportCarModeChanged(void *ctx, uint8_t mode) { (void)ctx; return UNER_App_ReportCarModeChanged(mode); }
static inline UNER_Status AppWifi_UNER_App_ReportMenuSelectionChanged(void *ctx, uint32_t screen_code,
                                                uint8_t selected_index,
                                                uint8_t item_count,
                                                uint8_t source) { (void)ctx; return UNER_App_ReportMenuSelectionChanged(screen_code, selected_index, item_count, source); }
static inline UNER_Status AppWifi_UNER_App_ReportScreenChanged(void *ctx, uint32_t screen_code, uint8_t source) { (void)ctx; return UNER_App_ReportScreenChanged(screen_code, source); }
static inline UNER_Status AppWifi_UNER_App_RequestBinaryAsset(void *ctx, uint8_t asset_id) { (void)ctx; return UNER_App_RequestBinaryAsset(asset_id); }
static inline UNER_Status AppWifi_UNER_App_RequestWifiNetworkDetail(void *ctx, const char *ssid) { (void)ctx; return UNER_App_RequestWifiNetworkDetail(ssid); }
static inline void AppWifi_UNER_App_ResetUart1RxPos(void *ctx) { (void)ctx; UNER_App_ResetUart1RxPos(); }
static inline UNER_Status AppWifi_UNER_App_SendCommand(void *ctx, uint8_t cmd, const uint8_t *payload, uint8_t len) { (void)ctx; return UNER_App_SendCommand(cmd, payload, len); }
static inline UNER_Status AppWifi_UNER_App_SendEspReboot(void *ctx, uint8_t boot_mode) { (void)ctx; return UNER_App_SendEspReboot(boot_mode); }
static inline UNER_Status AppWifi_UNER_App_SendWifiCredentialRequest(void *ctx, const char *ssid) { (void)ctx; return UNER_App_SendWifiCredentialRequest(ssid); }
static const WifiBridge_Services appWifiServices={
 .checksum=UNER_Core_ComputeChecksum,
 .clear_scan_results=WiFiScan_ClearResults,
 .AppApConfig_Get=AppWifi_AppApConfig_Get,
 .AppApConfig_OnEspApplyResponse=AppWifi_AppApConfig_OnEspApplyResponse,
 .AppPreferences_GetWifiAtProtocol=AppWifi_AppPreferences_GetWifiAtProtocol,
 .Permission_RevokeRemoteNode=AppWifi_Permission_RevokeRemoteNode,
 .UNER_App_FeedUart1Bytes=AppWifi_UNER_App_FeedUart1Bytes,
 .UNER_App_GetBinaryAsset=AppWifi_UNER_App_GetBinaryAsset,
 .UNER_App_GetWaitingCommandId=AppWifi_UNER_App_GetWaitingCommandId,
 .UNER_App_Init=AppWifi_UNER_App_Init,
 .UNER_App_IsWaitingValidation=AppWifi_UNER_App_IsWaitingValidation,
 .UNER_App_NotifyUart1Rx=AppWifi_UNER_App_NotifyUart1Rx,
 .UNER_App_OnAtNetworkDisconnected=AppWifi_UNER_App_OnAtNetworkDisconnected,
 .UNER_App_OnUart1TxComplete=AppWifi_UNER_App_OnUart1TxComplete,
 .UNER_App_Poll=AppWifi_UNER_App_Poll,
 .UNER_App_ReportCarModeChanged=AppWifi_UNER_App_ReportCarModeChanged,
 .UNER_App_ReportMenuSelectionChanged=AppWifi_UNER_App_ReportMenuSelectionChanged,
 .UNER_App_ReportScreenChanged=AppWifi_UNER_App_ReportScreenChanged,
 .UNER_App_RequestBinaryAsset=AppWifi_UNER_App_RequestBinaryAsset,
 .UNER_App_RequestWifiNetworkDetail=AppWifi_UNER_App_RequestWifiNetworkDetail,
 .UNER_App_ResetUart1RxPos=AppWifi_UNER_App_ResetUart1RxPos,
 .UNER_App_SendCommand=AppWifi_UNER_App_SendCommand,
 .UNER_App_SendEspReboot=AppWifi_UNER_App_SendEspReboot,
 .UNER_App_SendWifiCredentialRequest=AppWifi_UNER_App_SendWifiCredentialRequest,
};
#endif
