#pragma once
#include <Arduino.h>
#include "config.h"
#include "wifi_service_types.h"
#include "uner_handle.h"
#include "wifi_scan_arbiter.h"
#include "wifi_scan_snapshot.h"

struct BridgeWifiOps {
    void (*SetSessionConfig)(void *, const AppConfig *) = nullptr;
    bool (*BeginSessionMode)(void *, const AppConfig &, WifiRunMode) = nullptr;
    int (*StartScan)(void *, bool async, bool show_hidden) = nullptr;
    int (*ScanComplete)(void *) = nullptr;
    void (*DeleteScanResults)(void *) = nullptr;
    void (*RestoreModeAfterScan)(void *) = nullptr;
    WifiLinkStatus (*ConnectSTA)(void *, const char* ssid, const char* password, bool keep_ap, bool disconnect_first, const uint8_t* bssid) = nullptr;
    bool (*DisconnectSTA)(void *, bool radio_off, bool erase_sdk_credentials) = nullptr;
    bool (*ApplySTAConfig)(void *, const AppConfig& cfg) = nullptr;
    bool (*ApplyAPConfig)(void *, const AppConfig& cfg) = nullptr;
    bool (*StartAP)(void *, const char* ssid, const char* password, bool keep_sta, uint8_t channel, bool hidden, uint8_t max_clients, bool ap_protected) = nullptr;
    bool (*StopAP)(void *, bool radio_off) = nullptr;
    void (*SetSTAReconnectEnabled)(void *, bool enabled) = nullptr;
    bool (*IsSTAReconnectEnabled)(void *) = nullptr;
    WifiServiceState (*GetState)(void *) = nullptr;
    WifiIpv4 (*GetAP_IP)(void *) = nullptr;
    WifiRadioMode (*GetMode)(void *) = nullptr;
    bool (*ConsumeAPFallbackReady)(void *) = nullptr;
    bool (*ConsumeSTAAttemptFailure)(void *) = nullptr;
    void (*NotifySTAConfigurationFailure)(void *) = nullptr;
};
struct BridgeWifiPort {
    const BridgeWifiOps *ops = nullptr;
    void *context = nullptr;
    void SetSessionConfig(const AppConfig *config) const { if (ops->SetSessionConfig) ops->SetSessionConfig(context, config); }
    bool BeginSessionMode(const AppConfig &config, WifiRunMode mode) const { return ops->BeginSessionMode && ops->BeginSessionMode(context, config, mode); }
    int StartScan(bool async = true, bool show_hidden = false) const { return ops->StartScan(context, async, show_hidden); }
    int ScanComplete() const { return ops->ScanComplete(context); }
    void DeleteScanResults() const { return ops->DeleteScanResults(context); }
    void RestoreModeAfterScan() const { return ops->RestoreModeAfterScan(context); }
    WifiLinkStatus ConnectSTA(const char* ssid, const char* password, bool keep_ap = false, bool disconnect_first = false, const uint8_t* bssid = nullptr) const { return ops->ConnectSTA(context, ssid, password, keep_ap, disconnect_first, bssid); }
    bool DisconnectSTA(bool radio_off = true, bool erase_sdk_credentials = false) const { return ops->DisconnectSTA(context, radio_off, erase_sdk_credentials); }
    bool ApplySTAConfig(const AppConfig& cfg) const { return ops->ApplySTAConfig(context, cfg); }
    bool ApplyAPConfig(const AppConfig& cfg) const { return ops->ApplyAPConfig(context, cfg); }
    bool StartAP(const char* ssid, const char* password, bool keep_sta = false, uint8_t channel = 1, bool hidden = false, uint8_t max_clients = 4, bool ap_protected = true) const { return ops->StartAP(context, ssid, password, keep_sta, channel, hidden, max_clients, ap_protected); }
    bool StopAP(bool radio_off = true) const { return ops->StopAP(context, radio_off); }
    void SetSTAReconnectEnabled(bool enabled) const { return ops->SetSTAReconnectEnabled(context, enabled); }
    bool IsSTAReconnectEnabled() const { return ops->IsSTAReconnectEnabled(context); }
    WifiServiceState GetState() const { return ops->GetState(context); }
    WifiIpv4 GetAP_IP() const { return ops->GetAP_IP(context); }
    WifiRadioMode GetMode() const { return ops->GetMode(context); }
    bool ConsumeAPFallbackReady() const { return ops->ConsumeAPFallbackReady(context); }
    bool ConsumeSTAAttemptFailure() const { return ops->ConsumeSTAAttemptFailure && ops->ConsumeSTAAttemptFailure(context); }
    void NotifySTAConfigurationFailure() const { if (ops->NotifySTAConfigurationFailure) ops->NotifySTAConfigurationFailure(context); }
};
struct BridgeBackendOps {
    void *context = nullptr;
    uint8_t *reboot_pending = nullptr;
    uint8_t *reboot_boot_mode = nullptr;
    uint32_t *reboot_not_before_ms = nullptr;
    UNER_Status (*send_peer)(void *, uint8_t, const uint8_t *, uint8_t) = nullptr;
    UNER_Status (*send_from_node)(void *, uint8_t, uint8_t, uint8_t, const uint8_t *, uint8_t) = nullptr;
    uint8_t (*web_page_ready)(void *, uint32_t) = nullptr;
    void (*notify_ap_fallback)(void *) = nullptr;
    void (*intercept_packets)(void *, UNER_PacketInterceptorFn, void *) = nullptr;
    WifiScanAcquireResult (*acquire_scan)(void *, WifiScanOwner) = nullptr;
    uint8_t (*release_scan)(void *, WifiScanOwner) = nullptr;
    void (*scan_snapshot)(void *, WifiScanSnapshot *, int) = nullptr;
    const AppConfig *(*config)(void *) = nullptr;
    bool (*save_config)(void *, const AppConfig &) = nullptr;
    bool (*storage_ready)(void *) = nullptr;
};
// Platform operations for information and SDK resources used by the bridge.
class AsyncWebSocketClient;
struct BridgePlatformOps {
    uint32_t (*now_ms)() = nullptr;
    uint32_t (*random_u32)() = nullptr;
    uint32_t (*free_heap)() = nullptr;
    uint32_t (*max_free_block)() = nullptr;
    uint8_t (*heap_fragmentation)() = nullptr;
    String (*core_version)() = nullptr;
    const char *(*sdk_version)() = nullptr;
    bool (*asset_exists)(const char *) = nullptr;
    WifiRadioMode (*radio_mode)() = nullptr;
    WifiLinkStatus (*radio_status)() = nullptr;
    WifiIpv4 (*sta_ip)() = nullptr;
    WifiIpv4 (*ap_ip)() = nullptr;
    String (*sta_ssid)() = nullptr;
    int32_t (*sta_rssi)() = nullptr;
    uint8_t (*ap_clients)() = nullptr;
    WifiIpv4 (*client_ip)(AsyncWebSocketClient *) = nullptr;
    void *(*station_first)() = nullptr;
    void *(*station_next)(void *) = nullptr;
    const uint8_t *(*station_mac)(void *) = nullptr;
    uint32_t (*station_ip)(void *) = nullptr;
    void (*station_free)() = nullptr;
    bool (*station_deauth)(uint8_t *) = nullptr;
};
const BridgePlatformOps &BridgePlatform_ESP();

const BridgeBackendOps &Bridge_DefaultBackend();
const BridgeWifiPort &Bridge_DefaultWifi();
