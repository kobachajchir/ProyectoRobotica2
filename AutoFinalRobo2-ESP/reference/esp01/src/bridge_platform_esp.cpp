#include "bridge_ports.h"
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
extern "C" {
#include <user_interface.h>
bool wifi_softap_deauth(uint8 *mac);
}

static WifiIpv4 neutralIP(IPAddress ip) { return WifiIpv4(ip[0],ip[1],ip[2],ip[3]); }
const BridgePlatformOps &BridgePlatform_ESP() {
    static const BridgePlatformOps ops = [] {
        BridgePlatformOps v;
        v.now_ms = [] { return (uint32_t)millis(); };
        v.random_u32 = [] { return (uint32_t)os_random(); };
        v.free_heap = [] { return ESP.getFreeHeap(); };
        v.max_free_block = [] { return ESP.getMaxFreeBlockSize(); };
        v.heap_fragmentation = [] { return ESP.getHeapFragmentation(); };
        v.core_version = [] { return ESP.getCoreVersion(); };
        v.sdk_version = [] { return ESP.getSdkVersion(); };
        v.asset_exists = [](const char *path) { return LittleFS.exists(path); };
        v.radio_mode = [] { return static_cast<WifiRadioMode>(WiFi.getMode()); };
        v.radio_status = [] { return static_cast<WifiLinkStatus>(WiFi.status()); };
        v.sta_ip = [] { return neutralIP(WiFi.localIP()); };
        v.ap_ip = [] { return neutralIP(WiFi.softAPIP()); };
        v.sta_ssid = [] { return WiFi.SSID(); };
        v.sta_rssi = []() -> int32_t { return WiFi.RSSI(); };
        v.client_ip = [](AsyncWebSocketClient *client) { return neutralIP(client->remoteIP()); };
        v.ap_clients = [] { return WiFi.softAPgetStationNum(); };
        v.station_first = []() -> void * { return wifi_softap_get_station_info(); };
        v.station_next = [](void *item) -> void * { return STAILQ_NEXT(static_cast<station_info *>(item), next); };
        v.station_mac = [](void *item) -> const uint8_t * { return static_cast<station_info *>(item)->bssid; };
        v.station_ip = [](void *item) { return static_cast<station_info *>(item)->ip.addr; };
        v.station_free = [] { wifi_softap_free_station_info(); };
        v.station_deauth = [](uint8_t *mac) { return wifi_softap_deauth(mac); };
        return v;
    }();
    return ops;
}
