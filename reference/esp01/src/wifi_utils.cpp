#include "wifi_utils.h"
#include "wifi_service.h"
#include "globals.h"
#include "timebase.h"
#include "nvs_utils.h"
#include "esp_platform.h"

static WifiIpv4 fromIP(IPAddress ip) { return WifiIpv4(ip[0],ip[1],ip[2],ip[3]); }
static IPAddress toIP(WifiIpv4 ip) { return IPAddress(ip.octets[0],ip.octets[1],ip.octets[2],ip.octets[3]); }
static const WifiDriverOps &driverOps() {
    static const WifiDriverOps ops = [] {
        WifiDriverOps v;
        v.initialize = [](void *) {
            WiFi.persistent(false);
            WiFi.setSleepMode(WIFI_NONE_SLEEP);
            WiFi.setAutoReconnect(false);
            WiFi.setAutoConnect(false);
        };
        v.set_mode = [](void *, WifiRadioMode m) { return WiFi.mode(static_cast<WiFiMode_t>(m)); };
        v.get_mode = [](void *) { return static_cast<WifiRadioMode>(WiFi.getMode()); };
        v.status = [](void *) { return static_cast<WifiLinkStatus>(WiFi.status()); };
        v.connect = [](void *,const char *s,const char *p,const uint8_t *b) {
            return static_cast<WifiLinkStatus>(b ? WiFi.begin(s,p,0,b,true) : WiFi.begin(s,p));
        };
        v.disconnect = [](void *,bool off,bool erase) { return WiFi.disconnect(off,erase); };
        v.reconnect = [](void *) { return WiFi.reconnect(); };
        v.start_ap = [](void *,const char *s,const char *p,uint8_t ch,bool hidden,uint8_t max) {
            return WiFi.softAP(s,p,ch,hidden ? 1 : 0,max);
        };
        v.stop_ap = [](void *,bool off) { return WiFi.softAPdisconnect(off); };
        v.configure_sta = [](void *,WifiIpv4 ip,WifiIpv4 gw,WifiIpv4 mask) { return WiFi.config(toIP(ip),toIP(gw),toIP(mask)); };
        v.configure_ap = [](void *,WifiIpv4 ip,WifiIpv4 gw,WifiIpv4 mask) { return WiFi.softAPConfig(toIP(ip),toIP(gw),toIP(mask)); };
        v.auto_reconnect = [](void *,bool on) { WiFi.setAutoReconnect(on); };
        v.scan_start = [](void *,bool async,bool hidden) { return (int)WiFi.scanNetworks(async,hidden); };
        v.scan_complete = [](void *) { return (int)WiFi.scanComplete(); };
        v.scan_delete = [](void *) { WiFi.scanDelete(); };
        v.sta_ip = [](void *) { return fromIP(WiFi.localIP()); };
        v.ap_ip = [](void *) { return fromIP(WiFi.softAPIP()); };
        v.ap_clients = [](void *) { return WiFi.softAPgetStationNum(); };
        return v;
    }();
    return ops;
}
static const AppConfig *sessionConfig = nullptr;
const AppConfig *WifiUtils_GetRuntimeConfig() { return sessionConfig ? sessionConfig : NVS_GetCached(); }
void WifiUtils_SetSessionConfig(const AppConfig *config) { sessionConfig = config; }

static WifiServiceHandle &service() {
    static WifiServiceHandle h;
    if (!h.driver.ops) {
        WifiDriverPort driver;
        driver.ops = &driverOps();
        WifiServiceOps services;
        services.ticks = [](void *) { return Timebase_Ticks(); };
        services.config = [](void *) { return WifiUtils_GetRuntimeConfig(); };
        services.cooperate = EspPlatform_Delay0;
        WifiService_Init(h, driver, services, &flagsGlobals, &tm_sta_wait, &tm_sta_retry);
    }
    return h;
}
bool WifiUtils_Begin(const AppConfig &c,WifiRunMode m) { return WifiService_Begin(service(),c,m); }
void WifiUtils_Loop() { WifiService_Loop(service()); }
int WifiUtils_StartScan(bool async,bool hidden) { return WifiService_StartScan(service(),async,hidden); }
int WifiUtils_ScanComplete() { return WifiService_ScanComplete(service()); }
void WifiUtils_DeleteScanResults() { WifiService_DeleteScanResults(service()); }
void WifiUtils_RestoreModeAfterScan() { WifiService_RestoreModeAfterScan(service()); }
wl_status_t WifiUtils_ConnectSTA(const char *s,const char *p,bool ap,bool disc,const uint8_t *b) {
    return static_cast<wl_status_t>(WifiService_ConnectSTA(service(),s,p,ap,disc,b));
}
bool WifiUtils_DisconnectSTA(bool off,bool erase) { return WifiService_DisconnectSTA(service(),off,erase); }
bool WifiUtils_ReconnectSTA() { return WifiService_ReconnectSTA(service()); }
bool WifiUtils_ConfigureSTA(IPAddress ip,IPAddress gw,IPAddress mask) { return WifiService_ConfigureSTA(service(),fromIP(ip),fromIP(gw),fromIP(mask)); }
bool WifiUtils_ConfigureAP(IPAddress ip,IPAddress gw,IPAddress mask) { return WifiService_ConfigureAP(service(),fromIP(ip),fromIP(gw),fromIP(mask)); }
bool WifiUtils_ApplySTAConfig(const AppConfig &c) { return WifiService_ApplySTAConfig(service(),c); }
bool WifiUtils_ApplyAPConfig(const AppConfig &c) { return WifiService_ApplyAPConfig(service(),c); }
bool WifiUtils_StartAP(const char *s,const char *p,bool sta,uint8_t ch,bool hidden,uint8_t max,bool protect) {
    return WifiService_StartAP(service(),s,p,sta,ch,hidden,max,protect);
}
bool WifiUtils_StopAP(bool off) { return WifiService_StopAP(service(),off); }
void WifiUtils_SetSTAReconnectEnabled(bool on) { WifiService_SetSTAReconnectEnabled(service(),on); }
bool WifiUtils_IsSTAReconnectEnabled() { return WifiService_IsSTAReconnectEnabled(service()); }
WifiServiceState WifiUtils_GetState() { return WifiService_GetState(service()); }
IPAddress WifiUtils_GetSTA_IP() { return toIP(WifiService_GetSTA_IP(service())); }
IPAddress WifiUtils_GetAP_IP() { return toIP(WifiService_GetAP_IP(service())); }
bool WifiUtils_STA_Connected() { return WifiService_STA_Connected(service()); }
WiFiMode_t WifiUtils_GetMode() { return static_cast<WiFiMode_t>(WifiService_GetMode(service())); }
wl_status_t WifiUtils_GetSTAStatus() { return static_cast<wl_status_t>(WifiService_GetSTAStatus(service())); }
uint8_t WifiUtils_GetAPClientCount() { return WifiService_GetAPClientCount(service()); }
bool WifiUtils_ConsumeAPFallbackReady() { return WifiService_ConsumeAPFallbackReady(service()); }

bool WifiUtils_ConsumeSTAAttemptFailure() { return WifiService_ConsumeSTAAttemptFailure(service()); }

void WifiUtils_NotifySTAConfigurationFailure() { WifiService_NotifySTAConfigurationFailure(service()); }
