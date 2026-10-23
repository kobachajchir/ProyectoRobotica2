#include "app_bindings.h"
#include "uner_app_handlers.h"
#include "uner_app_ports.h"
#include "webserver_ws.h"
#include "webserver_ports.h"
#include "ws_json_bridge.h"
#include "wifi_utils.h"
#include "nvs_utils.h"
#include "uner_transport_serial.h"
#include <string.h>
extern "C" {
#include <user_interface.h>
}

static WifiIpv4 neutralIP(IPAddress ip) { return WifiIpv4(ip[0],ip[1],ip[2],ip[3]); }

static const BridgeWifiOps &wifiOps() {
    static const BridgeWifiOps ops = [] {
        BridgeWifiOps value;
        value.SetSessionConfig = [](void *, const AppConfig *config) { WifiUtils_SetSessionConfig(config); };
        value.BeginSessionMode = [](void *, const AppConfig &config, WifiRunMode mode) { if (mode == WIFI_RUN_AP_ONLY) return WifiUtils_ApplyAPConfig(config) && WifiUtils_StartAP(config.apSsid, config.apPass, false, 1u, false, 4u, config.apProtected != 0u); return WifiUtils_Begin(config, mode); };
        value.StartScan = [](void *, bool async, bool show_hidden) { return WifiUtils_StartScan(async, show_hidden); };
        value.ScanComplete = [](void *) { return WifiUtils_ScanComplete(); };
        value.DeleteScanResults = [](void *) { return WifiUtils_DeleteScanResults(); };
        value.RestoreModeAfterScan = [](void *) { return WifiUtils_RestoreModeAfterScan(); };
        value.ConnectSTA = [](void *, const char* ssid, const char* password, bool keep_ap, bool disconnect_first, const uint8_t* bssid) { return static_cast<WifiLinkStatus>(WifiUtils_ConnectSTA(ssid, password, keep_ap, disconnect_first, bssid)); };
        value.DisconnectSTA = [](void *, bool radio_off, bool erase_sdk_credentials) { return WifiUtils_DisconnectSTA(radio_off, erase_sdk_credentials); };
        value.ApplySTAConfig = [](void *, const AppConfig& cfg) { return WifiUtils_ApplySTAConfig(cfg); };
        value.ApplyAPConfig = [](void *, const AppConfig& cfg) { return WifiUtils_ApplyAPConfig(cfg); };
        value.StartAP = [](void *, const char* ssid, const char* password, bool keep_sta, uint8_t channel, bool hidden, uint8_t max_clients, bool ap_protected) { return WifiUtils_StartAP(ssid, password, keep_sta, channel, hidden, max_clients, ap_protected); };
        value.StopAP = [](void *, bool radio_off) { return WifiUtils_StopAP(radio_off); };
        value.SetSTAReconnectEnabled = [](void *, bool enabled) { return WifiUtils_SetSTAReconnectEnabled(enabled); };
        value.IsSTAReconnectEnabled = [](void *) { return WifiUtils_IsSTAReconnectEnabled(); };
        value.GetState = [](void *) { return WifiUtils_GetState(); };
        value.GetAP_IP = [](void *) { return neutralIP(WifiUtils_GetAP_IP()); };
        value.GetMode = [](void *) { return static_cast<WifiRadioMode>(WifiUtils_GetMode()); };
        value.ConsumeAPFallbackReady = [](void *) { return WifiUtils_ConsumeAPFallbackReady(); };
        value.ConsumeSTAAttemptFailure = [](void *) { return WifiUtils_ConsumeSTAAttemptFailure(); };
        value.NotifySTAConfigurationFailure = [](void *) { WifiUtils_NotifySTAConfigurationFailure(); };
        return value;
    }();
    return ops;
}
static BridgeBackendOps bridgeBackend(UNER_App *app) {
    BridgeBackendOps v;
    v.context = app;
    if (app) {
        v.reboot_pending = &app->reboot_pending;
        v.reboot_boot_mode = &app->reboot_boot_mode;
        v.reboot_not_before_ms = &app->reboot_not_before_ms;
    }
    v.send_peer = [](void *ctx,uint8_t cmd,const uint8_t *p,uint8_t len) {
        return UNER_App_SendRawToPeer(static_cast<UNER_App *>(ctx),cmd,p,len);
    };
    v.send_from_node = [](void *ctx,uint8_t src,uint8_t dst,uint8_t cmd,const uint8_t *p,uint8_t len) {
        return UNER_App_SendRawFromNode(static_cast<UNER_App *>(ctx),src,dst,cmd,p,len);
    };
    v.web_page_ready = [](void *, uint32_t id) { return WebServerWS_MarkPageReady(id); };
    v.notify_ap_fallback = [](void *ctx) { UNER_App_NotifyApFallbackReady(static_cast<UNER_App *>(ctx)); };
    v.intercept_packets = [](void *ctx,UNER_PacketInterceptorFn fn,void *user) {
        UNER_Handle_SetPacketInterceptor(&static_cast<UNER_App *>(ctx)->handle,fn,user);
    };
    v.acquire_scan = [](void *,WifiScanOwner owner) { return UNER_App_TryAcquireWifiScan(owner); };
    v.release_scan = [](void *,WifiScanOwner owner) { return UNER_App_ReleaseWifiScan(owner); };
    v.scan_snapshot = [](void *,WifiScanSnapshot *out,int count) { UNER_App_UpdateScanSnapshotFromDriver(out,count); };
    v.config = [](void *) { return WifiUtils_GetRuntimeConfig(); };
    v.save_config = [](void *,const AppConfig &config) { return NVS_Save(config); };
    v.storage_ready = [](void *) { return NVS_StorageReady(); };
    return v;
}
// Legacy initialization still constructs the same connections for existing callers.
void WSJsonBridge_Init(AsyncWebSocket *socket, UNER_App *app) {
    BridgeWifiPort wifi;
    wifi.ops = &wifiOps();
    WSJsonBridge_InitWithPorts(socket, bridgeBackend(app), wifi, BridgePlatform_ESP());
}
static uint8_t map_scan_encryption_type(uint8_t raw_type)
{
#if defined(ESP8266)
  if (raw_type == ENC_TYPE_WEP) return 1u;
  if (raw_type == ENC_TYPE_TKIP) return 2u;
  if (raw_type == ENC_TYPE_CCMP) return 3u;
  if (raw_type == ENC_TYPE_AUTO) return 5u;
#ifdef ENC_TYPE_GCMP
  if (raw_type == ENC_TYPE_GCMP) return 4u;
#endif
  return 0u;
#elif defined(ESP32)
  if (raw_type == WIFI_AUTH_WEP) return 1u;
  if (raw_type == WIFI_AUTH_WPA_PSK) return 2u;
  if (raw_type == WIFI_AUTH_WPA2_PSK) return 3u;
  if (raw_type == WIFI_AUTH_WPA_WPA2_PSK) return 5u;
#ifdef WIFI_AUTH_WPA3_PSK
  if (raw_type == WIFI_AUTH_WPA3_PSK) return 4u;
#endif
  return 0u;
#else
  (void)raw_type;
  return 0u;
#endif
}

static int8_t clamp_scan_rssi(int32_t rssi)
{
  if (rssi < -128) return -128;
  if (rssi > 127) return 127;
  return (int8_t)rssi;
}

static bool read_scan_entry(uint8_t index, WifiScanSnapshotEntry *out) {
    const bss_info *scan_info = WiFi.getScanInfoByIndex(index);
    if (!scan_info || !out) return false;

    uint8_t ssid_len = 0u;
    while (ssid_len < SSID_MAX_LEN && scan_info->ssid[ssid_len] != 0u) ssid_len++;
    WifiScanSnapshotEntry entry{};

    for (uint8_t k = 0; k < ssid_len; ++k) entry.ssid[k] = (char)scan_info->ssid[k];
    entry.ssid[ssid_len] = '\0';
    entry.signal_strength = clamp_scan_rssi(scan_info->rssi);
    entry.encryption_type = map_scan_encryption_type((uint8_t)WiFi.encryptionType(index));
    entry.channel = (uint8_t)scan_info->channel;
    memcpy(entry.bssid, scan_info->bssid, sizeof(entry.bssid));
    *out = entry;
    return true;
}

const UNER_AppServices &UNER_App_DefaultServices() {
    static const UNER_AppServices value = [] {
        UNER_AppServices v;
        v.wifi.ops = &wifiOps();
        v.platform = &BridgePlatform_ESP();
        v.config = NVS_GetCached;
        v.save_config = NVS_Save;
        v.scan_entry = read_scan_entry;
        v.reset_diagnostic = [](uint8_t *reason,uint8_t *exception) {
            const rst_info *info = ESP.getResetInfoPtr();
            *reason = info ? (uint8_t)info->reason : 0xFFu;
            *exception = info ? (uint8_t)info->exccause : 0xFFu;
        };
        v.flush_serial = [] { Serial.flush(); };
        v.cooperate = [] { yield(); };
        v.delay_ms = [](uint32_t ms) { delay(ms); };
        v.restart = [] { ESP.restart(); };
        v.initialize_serial_transport = [](UNER_Transport *transport) {
            static UNER_SerialTransportCtx context;
            UNER_SerialTransport_InitWithContext(transport,&context,0,Serial,UNER_MAX_PAYLOAD);
        };
        v.connected_count = WebServerWS_GetConnectedCount;
        v.client_info = WebServerWS_GetClientInfoByIndex;
        v.request_credentials = WSJsonBridge_RequestWifiCredentials;
        v.credentials_state = WSJsonBridge_GetWifiCredentialsState;
        v.submit_credentials = WSJsonBridge_SubmitWifiCredentials;
        v.cancel_credentials = WSJsonBridge_CancelWifiCredentials;
        v.ap_clients_page = WSJsonBridge_BuildApClientsPage;
        v.disconnect_ap_client = WSJsonBridge_ScheduleApClientDisconnect;
        return v;
    }();
    return value;
}
const WebServerServices &WebServer_DefaultServices() {
    static const WebServerServices value = [] {
        WebServerServices v;
        v.storage_ready = NVS_StorageReady;
        v.bridge_initialize = [](void *ctx, AsyncWebSocket *socket) {
            WSJsonBridge_Init(socket,static_cast<UNER_App *>(ctx));
        };
        v.bridge_connect = WSJsonBridge_OnConnect;
        v.bridge_disconnect = WSJsonBridge_OnDisconnect;
        v.bridge_data = WSJsonBridge_OnData;
        v.bridge_loop = WSJsonBridge_Loop;
        v.notify_connected = [](void *ctx,uint32_t id) { UNER_App_NotifyWsClientConnected(static_cast<UNER_App *>(ctx),id); };
        v.notify_disconnected = [](void *ctx,uint32_t id) { UNER_App_NotifyWsClientDisconnected(static_cast<UNER_App *>(ctx),id); };
        v.notify_page_ready = [](void *ctx,uint32_t id,uint8_t n) { UNER_App_NotifyWebPageReady(static_cast<UNER_App *>(ctx),id,n); };
        v.notify_page_closed = [](void *ctx,uint32_t id,uint8_t n) { UNER_App_NotifyWebPageClosed(static_cast<UNER_App *>(ctx),id,n); };
        v.notify_ap_join = [](void *ctx,uint8_t n,uint8_t aid) { UNER_App_NotifyApClientJoined(static_cast<UNER_App *>(ctx),n,aid); };
        v.notify_ap_leave = [](void *ctx,uint8_t n,uint8_t aid) { UNER_App_NotifyApClientLeft(static_cast<UNER_App *>(ctx),n,aid); };
        v.notify_server_up = [](void *ctx) { UNER_App_NotifyWebserverUp(static_cast<UNER_App *>(ctx)); };
        return v;
    }();
    return value;
}
void AppBindings_Configure(UNER_App *app) {
    UNER_App_SetServices(UNER_App_DefaultServices());
    WebServerServices services = WebServer_DefaultServices();
    services.context = app;
    WebServerWS_SetServices(services);
}
void WebServerWS_SetUNERApp(void *app) { AppBindings_Configure(static_cast<UNER_App *>(app)); }

const BridgeBackendOps &Bridge_DefaultBackend() {
    static const BridgeBackendOps ops = bridgeBackend(nullptr);
    return ops;
}
const BridgeWifiPort &Bridge_DefaultWifi() {
    static const BridgeWifiPort port = [] {
        BridgeWifiPort v;
        v.ops = &wifiOps();
        return v;
    }();
    return port;
}
