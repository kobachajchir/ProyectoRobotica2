#pragma once
#include <ESP8266WiFi.h>
#include "config.h"

#include "wifi_service_types.h"

const AppConfig *WifiUtils_GetRuntimeConfig();
void WifiUtils_SetSessionConfig(const AppConfig *config);

bool WifiUtils_Begin(const AppConfig& cfg, WifiRunMode mode);
void WifiUtils_Loop();

// All Wi-Fi driver mutations belong here. Bridge/UNER handlers should call
// these operations instead of using the global WiFi object directly.
int WifiUtils_StartScan(bool async = true, bool show_hidden = false);
int WifiUtils_ScanComplete();
void WifiUtils_DeleteScanResults();
void WifiUtils_RestoreModeAfterScan();

wl_status_t WifiUtils_ConnectSTA(const char* ssid, const char* password,
                                 bool keep_ap = false, bool disconnect_first = false,
                                 const uint8_t* bssid = nullptr);
bool WifiUtils_DisconnectSTA(bool radio_off = true, bool erase_sdk_credentials = false);
bool WifiUtils_ReconnectSTA();

bool WifiUtils_ConfigureSTA(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
bool WifiUtils_ConfigureAP(IPAddress local_ip, IPAddress gateway, IPAddress subnet);
bool WifiUtils_ApplySTAConfig(const AppConfig& cfg);
bool WifiUtils_ApplyAPConfig(const AppConfig& cfg);
bool WifiUtils_StartAP(const char* ssid, const char* password,
                       bool keep_sta = false, uint8_t channel = 1,
                       bool hidden = false, uint8_t max_clients = 4,
                       bool ap_protected = true);
bool WifiUtils_StopAP(bool radio_off = true);

void WifiUtils_SetSTAReconnectEnabled(bool enabled);
bool WifiUtils_IsSTAReconnectEnabled();
WifiServiceState WifiUtils_GetState();

IPAddress WifiUtils_GetSTA_IP();
IPAddress WifiUtils_GetAP_IP();
bool      WifiUtils_STA_Connected();
WiFiMode_t WifiUtils_GetMode();
wl_status_t WifiUtils_GetSTAStatus();
uint8_t WifiUtils_GetAPClientCount();
/* Latch de una transicion STA fallida hacia un SoftAP que nunca se detuvo. */
bool WifiUtils_ConsumeAPFallbackReady();
bool WifiUtils_ConsumeSTAAttemptFailure();
void WifiUtils_NotifySTAConfigurationFailure();
