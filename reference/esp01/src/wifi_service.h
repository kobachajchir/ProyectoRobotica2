#pragma once
#include "config.h"
#include "bitMap.h"
#include "wifi_driver_port.h"

struct WifiServiceOps {
    void *context = nullptr;
    uint32_t (*ticks)(void *) = nullptr;
    const AppConfig *(*config)(void *) = nullptr;
    void (*cooperate)(void *) = nullptr;
};
struct WifiServiceHandle {
    WifiDriverPort driver;
    WifiServiceOps services;
    volatile t_bitMap *flags = nullptr;
    volatile uint16_t *sta_wait = nullptr;
    volatile uint16_t *sta_retry = nullptr;
    uint32_t sta_start_tick = 0;
    uint32_t last_retry_tick = 0;
    WifiRunMode mode = WIFI_RUN_AP_STA;
    WifiServiceState state = WIFI_SERVICE_IDLE;
    bool ap_started = false;
    bool keep_ap_with_sta = false;
    bool sta_reconnect_enabled = false;
    WifiRadioMode scan_previous_mode = WIFI_RADIO_OFF;
    bool scan_mode_restore_pending = false;
    bool scan_keep_ap_hybrid = false;
    bool ap_fallback_ready_pending = false;
    bool sta_attempt_failed_pending = false;
};
// All driver operations and runtime pointers must be valid for this handle.
void WifiService_Init(WifiServiceHandle &h, const WifiDriverPort &driver,
    const WifiServiceOps &services, volatile t_bitMap *flags,
    volatile uint16_t *sta_wait, volatile uint16_t *sta_retry);

bool WifiService_Begin(WifiServiceHandle &h, const AppConfig& cfg, WifiRunMode mode);
void WifiService_Loop(WifiServiceHandle &h);

// All Wi-Fi driver mutations belong here. Bridge/UNER handlers should call
// these operations instead of using the global WiFi object directly.
int WifiService_StartScan(WifiServiceHandle &h, bool async = true, bool show_hidden = false);
int WifiService_ScanComplete(WifiServiceHandle &h);
void WifiService_DeleteScanResults(WifiServiceHandle &h);
void WifiService_RestoreModeAfterScan(WifiServiceHandle &h);

WifiLinkStatus WifiService_ConnectSTA(WifiServiceHandle &h, const char* ssid, const char* password,
                                 bool keep_ap = false, bool disconnect_first = false,
                                 const uint8_t* bssid = nullptr);
bool WifiService_DisconnectSTA(WifiServiceHandle &h, bool radio_off = true, bool erase_sdk_credentials = false);
bool WifiService_ReconnectSTA(WifiServiceHandle &h);
void WifiService_NotifySTAConfigurationFailure(WifiServiceHandle &h);

bool WifiService_ConfigureSTA(WifiServiceHandle &h, WifiIpv4 local_ip, WifiIpv4 gateway, WifiIpv4 subnet);
bool WifiService_ConfigureAP(WifiServiceHandle &h, WifiIpv4 local_ip, WifiIpv4 gateway, WifiIpv4 subnet);
bool WifiService_ApplySTAConfig(WifiServiceHandle &h, const AppConfig& cfg);
bool WifiService_ApplyAPConfig(WifiServiceHandle &h, const AppConfig& cfg);
bool WifiService_StartAP(WifiServiceHandle &h, const char* ssid, const char* password,
                       bool keep_sta = false, uint8_t channel = 1,
                       bool hidden = false, uint8_t max_clients = 4,
                       bool ap_protected = true);
bool WifiService_StopAP(WifiServiceHandle &h, bool radio_off = true);

void WifiService_SetSTAReconnectEnabled(WifiServiceHandle &h, bool enabled);
bool WifiService_IsSTAReconnectEnabled(WifiServiceHandle &h);
WifiServiceState WifiService_GetState(WifiServiceHandle &h);

WifiIpv4 WifiService_GetSTA_IP(WifiServiceHandle &h);
WifiIpv4 WifiService_GetAP_IP(WifiServiceHandle &h);
bool      WifiService_STA_Connected(WifiServiceHandle &h);
WifiRadioMode WifiService_GetMode(WifiServiceHandle &h);
WifiLinkStatus WifiService_GetSTAStatus(WifiServiceHandle &h);
uint8_t WifiService_GetAPClientCount(WifiServiceHandle &h);
/* Latch de una transicion STA fallida hacia un SoftAP que nunca se detuvo. */
bool WifiService_ConsumeAPFallbackReady(WifiServiceHandle &h);
bool WifiService_ConsumeSTAAttemptFailure(WifiServiceHandle &h);
