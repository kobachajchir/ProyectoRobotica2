/*
 * wifi_bridge.h - backend-neutral WiFi API for app/UI code.
 */

#ifndef INC_WIFI_BRIDGE_H_
#define INC_WIFI_BRIDGE_H_

#include "wifi_bridge_types.h"

#ifdef __cplusplus
extern "C" {
#endif




WifiBridge_Result WifiBridge_Init(WifiBridge_Handle *h, void *huart, const WifiBridge_Hardware *hardware,
    const WifiBridge_Services *services, void *services_ctx, SystemHandle *system, WifiBridge_Credentials *credentials);
WifiBridge_Result WifiBridge_Start(WifiBridge_Handle *h);
void WifiBridge_Process(WifiBridge_Handle *h);
void WifiBridge_RxCheck(WifiBridge_Handle *h);
void WifiBridge_OnIdleIrq(WifiBridge_Handle *h);
void WifiBridge_OnTxComplete(WifiBridge_Handle *h);
void WifiBridge_OnRxHalfComplete(WifiBridge_Handle *h);
void WifiBridge_OnRxComplete(WifiBridge_Handle *h);
void WifiBridge_OnError(WifiBridge_Handle *h);
void WifiBridge_ResetRxPos(WifiBridge_Handle *h);

void WifiBridge_SetEventCallback(WifiBridge_Handle *h, WifiBridge_EventCallback cb);
void WifiBridge_EmitEvent(WifiBridge_Handle *h, const WifiBridge_Event *evt);
WifiBridge_Result WifiBridge_SetRuntimeCredentials_H(WifiBridge_Handle *h, const char *ssid, const char *password);
WifiBridge_Result WifiBridge_SetRuntimeSsid_H(WifiBridge_Handle *h, const char *ssid);
WifiBridge_Result WifiBridge_SetRuntimePassword_H(WifiBridge_Handle *h, const char *password);
const WifiBridge_Credentials *WifiBridge_GetRuntimeCredentials_H(WifiBridge_Handle *h);

WifiBridge_Result WifiBridge_SetModeSTA(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_SetModeAP(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_SetCredentials(WifiBridge_Handle *h,
                                            const char *ssid,
                                            const char *pass);
WifiBridge_Result WifiBridge_Connect(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_Disconnect(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_StartScan(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_StopScan(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_GetScanResults(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_GetStatus(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_StartAP(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_StopAP(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_SendCommand(WifiBridge_Handle *h,
                                         uint8_t cmd,
                                         const uint8_t *payload,
                                         uint8_t len);
WifiBridge_Result WifiBridge_SendWifiCredentialRequest(WifiBridge_Handle *h,
                                                       const char *ssid);
WifiBridge_Result WifiBridge_RequestWifiNetworkDetail(WifiBridge_Handle *h,
                                                      const char *ssid);
WifiBridge_Result WifiBridge_SendEspReboot(WifiBridge_Handle *h, uint8_t boot_mode);
WifiBridge_Result WifiBridge_SendAlive(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_SendAliveWithWebPeriod(WifiBridge_Handle *h,
                                                    uint16_t web_period_ms);
void WifiBridge_RegisterAliveResponse(WifiBridge_Handle *h, uint8_t mode);
void WifiBridge_RegisterBackendAlive(WifiBridge_Handle *h,
                                     WifiBridge_Backend backend,
                                     uint8_t mode);
uint8_t WifiBridge_ConsumeConnectionLostEvent(WifiBridge_Handle *h);
WifiBridge_Result WifiBridge_ReportScreenChanged(WifiBridge_Handle *h,
                                                 uint32_t screen_code,
                                                 uint8_t source);
WifiBridge_Result WifiBridge_ReportMenuSelectionChanged(WifiBridge_Handle *h,
                                                        uint32_t screen_code,
                                                        uint8_t selected_index,
                                                        uint8_t item_count,
                                                        uint8_t source);
WifiBridge_Result WifiBridge_ReportCarModeChanged(WifiBridge_Handle *h, uint8_t mode);
WifiBridge_Result WifiBridge_RequestBinaryAsset(WifiBridge_Handle *h, uint8_t asset_id);
uint8_t WifiBridge_GetBinaryAsset(WifiBridge_Handle *h,
                                  uint8_t asset_id,
                                  WifiBridge_BinaryAssetView *asset);
uint8_t WifiBridge_IsWaitingValidation(WifiBridge_Handle *h);
uint8_t WifiBridge_GetWaitingCommandId(WifiBridge_Handle *h);
WifiBridge_Backend WifiBridge_GetActiveBackend(const WifiBridge_Handle *h);
WifiBridge_CapabilityMask_t WifiBridge_GetCapabilities(const WifiBridge_Handle *h);
uint8_t WifiBridge_HasCapability(const WifiBridge_Handle *h,
                                 WifiBridge_CapabilityMask_t capability);
uint8_t WifiBridge_IsConnected(const WifiBridge_Handle *h);
uint8_t WifiBridge_GetMode(const WifiBridge_Handle *h);
const char *WifiBridge_GetBackendLabel(const WifiBridge_Handle *h);

#ifdef __cplusplus
}
#endif

#endif /* INC_WIFI_BRIDGE_H_ */
