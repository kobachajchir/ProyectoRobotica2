/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
// AFR2-20261023-11. Adaptation of ESP01 Wi-Fi states and async scan, new config schema.
#include "wifi_service.h"
#include <ESP8266WiFi.h>
#include <string.h>
extern "C" {
#include <user_interface.h>
}
static uint8_t scanSecurity(uint8_t enc){
    switch(enc){case ENC_TYPE_NONE:return 1;case ENC_TYPE_WEP:return 2;case ENC_TYPE_TKIP:return 3;case ENC_TYPE_CCMP:return 4;case ENC_TYPE_AUTO:return 5;default:return 6;}
}
static const char *securityName(uint8_t n){static const char *names[]={"unknown","open","WEP","WPA","WPA2","WPA/WPA2","protected"};return names[n<=6?n:0];}

static void configJson(const WifiConfig &c, JsonDocument &d) {
    d["schema"]="afr2-wifi-1"; d["mode"]=c.mode; d["ssid"]=c.ssid; d["password"]=c.password;
}
bool WifiService::valid(const WifiConfig &c) {
    if (strcmp(c.mode,"STA") && strcmp(c.mode,"AP") && strcmp(c.mode,"AP_STA")) return false;
    size_t ssid=strlen(c.ssid), password=strlen(c.password);
    if (!ssid || ssid>32) return false;
    // WPA passphrase only. Empty selects an open network; raw 64-digit PSK is not accepted.
    if (password && (password<8 || password>63)) return false;
    return true;
}
void WifiService::begin() {
    WiFi.persistent(false); WiFi.setAutoReconnect(false);
    WiFi.hostname("autofinalrobo2");
    apSsid_="AutoFinalRobo2-"+String(ESP.getChipId(),HEX);
    WifiConfig config;
    JsonDocument doc;
    if (storage_.loadWifi(doc) && doc["mode"].is<const char *>() &&
        doc["ssid"].is<const char *>() && doc["password"].is<const char *>() &&
        strlen(doc["mode"])<sizeof(config.mode) && strlen(doc["ssid"])<sizeof(config.ssid) &&
        strlen(doc["password"])<sizeof(config.password)) {
        strlcpy(config.mode,doc["mode"],sizeof(config.mode));
        strlcpy(config.ssid,doc["ssid"],sizeof(config.ssid));
        strlcpy(config.password,doc["password"],sizeof(config.password));
        if (valid(config)) { persisted_=1; apply(config); config=WifiConfig{}; return; }
    }
    startFallback();
}
void WifiService::startFallback() {
    memset(apPassword_,0,sizeof(apPassword_));
    apSsid_="AutoFinalRobo2-"+String(ESP.getChipId(),HEX);
    fallbackActive_=true;
    bool modeOk=WiFi.mode(staWanted_?WIFI_AP_STA:WIFI_AP);
    apOn_=modeOk && WiFi.softAP(apSsid_.c_str());
    if (!apOn_) configError_="AP_START_FAILED"; // Explicit open provisioning AP, no embedded credential.
}
void WifiService::apply(const WifiConfig &c) {
    disconnectedByUser_=false;
    memset(apPassword_,0,sizeof(apPassword_));
    WiFi.scanDelete(); scanReady_=false;
    staWanted_=strcmp(c.mode,"AP")!=0;
    bool wantAp=strcmp(c.mode,"STA")!=0;
    fallbackActive_=false; configError_=nullptr;
    apOn_=false;
    if (!WiFi.mode(staWanted_?(wantAp?WIFI_AP_STA:WIFI_STA):WIFI_AP)) {
        configError_="MODE_FAILED"; startFallback(); return;
    }
    if (strcmp(c.mode,"AP")==0) {
        WiFi.disconnect(false);
        apSsid_=c.ssid;
        apOn_=WiFi.softAP(c.ssid,c.password[0]?c.password:nullptr);
        if(apOn_)strlcpy(apPassword_,c.password,sizeof(apPassword_));
    } else {
        apSsid_="AutoFinalRobo2-"+String(ESP.getChipId(),HEX);
        // AP_STA uses the stable provisioning AP; supplied SSID/password target STA.
        if (wantAp) apOn_=WiFi.softAP(apSsid_.c_str());
        if (WiFi.begin(c.ssid,c.password)==WL_CONNECT_FAILED) configError_="STA_START_FAILED";
    }
    if (wantAp && !apOn_) { configError_="AP_START_FAILED"; startFallback(); }
    staStarted_=lastRetry_=millis();
}
bool WifiService::stage(const WifiConfig &c, uint32_t now) {
    if (busy() || !valid(c)) return false;
    if (c.persist) {
        JsonDocument doc; configJson(c,doc);
        if (!storage_.saveWifi(doc)) return false;
    }
    persisted_=c.persist?1:0;
    pending_=c; stagedAt_=now; applyPending_=true; return true;
}
void WifiService::tick(uint32_t now) {
    if (applyPending_ && (uint32_t)(now-stagedAt_)>=350u) {
        applyPending_=false; apply(pending_); pending_=WifiConfig{};
    }
    if (staWanted_ && !disconnectedByUser_ && WiFi.status()!=WL_CONNECTED) {
        if (!apOn_ && (uint32_t)(now-staStarted_)>=15000u) startFallback();
        if ((uint32_t)(now-lastRetry_)>=30000u && !scanPending_ && !applyPending_) {
            lastRetry_=now; WiFi.reconnect();
        }
    } else if (staWanted_) staStarted_=now; // grace period starts again on a later disconnection
    if (scanPending_) {
        int n=WiFi.scanComplete();
        if (n!=WIFI_SCAN_RUNNING) {
            scanPending_=false;
            if(scanRestoreAp_ && !staWanted_){WiFi.mode(WIFI_AP);scanRestoreAp_=false;}
            if (!scanTimedOut_) scanReady_=true;
            else WiFi.scanDelete();
        } else if (!scanTimedOut_ && (uint32_t)(now-scanStarted_)>20000u) {
            // scanDelete does not cancel a running SDK scan. Keep it busy until it actually ends.
            scanTimedOut_=true; scanReady_=true;
        }
    }
}
bool WifiService::scan() {
    if (busy() || scanReady_) return false;
    // AP-only mode needs the station interface for asynchronous scanning.
    scanRestoreAp_=WiFi.getMode()==WIFI_AP;
    if(scanRestoreAp_ && !WiFi.mode(WIFI_AP_STA)){scanRestoreAp_=false;return false;}
    scanStarted_=millis(); scanTimedOut_=false;
    int result=WiFi.scanNetworks(true,true);
    scanPending_=result==WIFI_SCAN_RUNNING;
    scanReady_=!scanPending_;
    return true; // Accepted asynchronous operation; a failed scan has its own terminal event.
}
bool WifiService::retry() {
    if(busy() || !staWanted_)return false;
    disconnectedByUser_=false;
    if(WiFi.status()==WL_CONNECTED)return true;
    staStarted_=lastRetry_=millis();return WiFi.reconnect();
}
bool WifiService::disconnectStation(){
    if(busy() || !(WiFi.getMode()&WIFI_STA))return false;
    disconnectedByUser_=true;WiFi.setAutoReconnect(false);
    if(WiFi.status()!=WL_CONNECTED)return true;
    if(!WiFi.disconnect(false,false)){disconnectedByUser_=false;return false;}
    return true; // accepted; caller must observe status before announcing success
}
bool WifiService::changeMode(uint8_t mode){
    if(busy() || mode<1 || mode>3)return false;
    WiFiMode_t target=mode==1?WIFI_STA:mode==2?WIFI_AP:WIFI_AP_STA;
    if(!WiFi.mode(target))return false;
    staWanted_=mode!=2;
    if(mode&2u){if(!apOn_)apOn_=WiFi.softAP(apSsid_.c_str(),apPassword_[0]?apPassword_:nullptr);}
    else apOn_=false;
    if(staWanted_)staStarted_=lastRetry_=millis();
    return WiFi.getMode()==target && (!(mode&2u) || apOn_);
}
bool WifiService::takeScan(JsonObject out) {
    if (!scanReady_) return false;
    scanReady_=false; int n=scanTimedOut_?WIFI_SCAN_FAILED:WiFi.scanComplete();
    out["status"]=n>=0?"completed":"failed";
    JsonArray list=out["networks"].to<JsonArray>();
    int count=n>16?16:n; out["truncated"]=n>16;authCount_=0;authScanAt_=millis();
    for (int i=0;i<count;++i) {
        JsonObject entry=list.add<JsonObject>(); entry["ssid"]=WiFi.SSID(i);
        entry["rssiDbm"]=WiFi.RSSI(i); entry["channel"]=WiFi.channel(i);
        uint8_t auth=scanSecurity(WiFi.encryptionType(i));if(WiFi.BSSID(i)){memcpy(scanBssid_[authCount_],WiFi.BSSID(i),6);scanAuth_[authCount_++]=auth;}entry["securityCode"]=auth;entry["security"]=securityName(auth);entry["secure"]=auth!=1;
    }
    if (!scanPending_){WiFi.scanDelete();if(scanRestoreAp_ && !staWanted_){WiFi.mode(WIFI_AP);scanRestoreAp_=false;}}
    return true;
}
void WifiService::displayInfo(WifiDisplayInfo &out) const {
    memset(&out,0,sizeof(out));out.rssi=-127;
    auto mode=WiFi.getMode();out.mode=mode==WIFI_STA?1:mode==WIFI_AP?2:mode==WIFI_AP_STA?3:0;
    IPAddress ip=WiFi.localIP(),ap=WiFi.softAPIP();
    out.staConnected=(out.mode&1u) && WiFi.status()==WL_CONNECTED && (uint32_t)ip!=0;
    out.apReady=(out.mode&2u) && apOn_ && (uint32_t)ap!=0;
    if(out.staConnected){strlcpy(out.staSsid,WiFi.SSID().c_str(),sizeof(out.staSsid));out.rssi=WiFi.RSSI();for(uint8_t i=0;i<4;i++)out.staIp[i]=ip[i];}
    if(out.apReady){strlcpy(out.apSsid,apSsid_.c_str(),sizeof(out.apSsid));strlcpy(out.apPassword,apPassword_,sizeof(out.apPassword));
        out.clients=WiFi.softAPgetStationNum();for(uint8_t i=0;i<4;i++)out.apIp[i]=ap[i];}
}
void WifiService::status(JsonObject out) const {
    WiFiMode_t mode=WiFi.getMode();
    out["mode"]=mode==WIFI_AP?"AP":mode==WIFI_STA?"STA":mode==WIFI_AP_STA?"AP_STA":"OFF";
    bool connected=(mode&WIFI_STA) && WiFi.status()==WL_CONNECTED && (uint32_t)WiFi.localIP()!=0;
    out["staConnected"]=connected;
    out["staSsid"]=nullptr; out["staIp"]=nullptr; out["rssiDbm"]=nullptr;
    if (connected) { out["staSsid"]=WiFi.SSID(); out["staIp"]=WiFi.localIP().toString(); out["rssiDbm"]=WiFi.RSSI(); }
    out["apSsid"]=nullptr; out["apIp"]=nullptr;
    if (apOn_) { out["apSsid"]=apSsid_; out["apIp"]=WiFi.softAPIP().toString(); }
    out["staMask"]=connected?WiFi.subnetMask().toString():String();out["staGateway"]=connected?WiFi.gatewayIP().toString():String();
    uint8_t auth=0;
    if(connected && WiFi.BSSID() && (uint32_t)(millis()-authScanAt_)<=120000u)for(uint8_t i=0;i<authCount_;i++)if(!memcmp(scanBssid_[i],WiFi.BSSID(),6)){auth=scanAuth_[i];break;}
    out["securityCode"]=auth;out["security"]=securityName(auth);
    if(auth)out["secure"]=auth!=1;else out["secure"]=nullptr;
    uint8_t apAuth=0;struct softap_config apConfig={};
    if(apOn_ && wifi_softap_get_config(&apConfig))apAuth=apConfig.authmode==AUTH_OPEN?1:apConfig.authmode==AUTH_WEP?2:apConfig.authmode==AUTH_WPA_PSK?3:apConfig.authmode==AUTH_WPA2_PSK?4:apConfig.authmode==AUTH_WPA_WPA2_PSK?5:6;
    out["apSecurityCode"]=apAuth;out["apSecurity"]=securityName(apAuth);
    struct ip_info info={};bool apInfo=apOn_ && wifi_get_ip_info(SOFTAP_IF,&info);
    out["apMask"]=apInfo?IPAddress(info.netmask.addr).toString():String();out["apGateway"]=apInfo?IPAddress(info.gw.addr).toString():String();
    out["disconnectedByUser"]=disconnectedByUser_;
    out["ageMs"]=0; out["applying"]=applyPending_;
    out["fallbackActive"]=fallbackActive_ && apOn_;
    out["scanRunning"]=scanPending_;
    if (persisted_<0) out["configPersisted"]=nullptr; else out["configPersisted"]=persisted_==1;
    out["configError"]=configError_;
    const auto sta=WiFi.status();
    out["staStatus"]=!staWanted_?"disabled":connected?"connected":disconnectedByUser_?"disconnected":
        sta==WL_NO_SSID_AVAIL?"network-not-found":sta==WL_CONNECT_FAILED?"connection-failed":
        sta==WL_WRONG_PASSWORD?"wrong-password":"connecting";
}
