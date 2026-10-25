// AFR2-20261023-11. Adaptation of ESP01 Wi-Fi states and async scan, new config schema.
#include "wifi_service.h"
#include <ESP8266WiFi.h>
#include <string.h>
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
    WiFi.persistent(false); WiFi.setAutoReconnect(true);
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
    if (staWanted_ && WiFi.status()!=WL_CONNECTED) {
        if (!apOn_ && (uint32_t)(now-staStarted_)>=15000u) startFallback();
        if ((uint32_t)(now-lastRetry_)>=30000u && !scanPending_ && !applyPending_) {
            lastRetry_=now; WiFi.reconnect();
        }
    } else if (staWanted_) staStarted_=now; // grace period starts again on a later disconnection
    if (scanPending_) {
        int n=WiFi.scanComplete();
        if (n!=WIFI_SCAN_RUNNING) {
            scanPending_=false;
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
    if (WiFi.getMode()==WIFI_AP) WiFi.mode(WIFI_AP_STA);
    scanStarted_=millis(); scanTimedOut_=false;
    int result=WiFi.scanNetworks(true,true);
    scanPending_=result==WIFI_SCAN_RUNNING;
    scanReady_=!scanPending_;
    return true; // Accepted asynchronous operation; a failed scan has its own terminal event.
}
bool WifiService::retry() {
    if(busy() || !staWanted_)return false;
    staStarted_=lastRetry_=millis();return WiFi.reconnect();
}
bool WifiService::takeScan(JsonObject out) {
    if (!scanReady_) return false;
    scanReady_=false; int n=scanTimedOut_?WIFI_SCAN_FAILED:WiFi.scanComplete();
    out["status"]=n>=0?"completed":"failed";
    JsonArray list=out["networks"].to<JsonArray>();
    int count=n>16?16:n; out["truncated"]=n>16;
    for (int i=0;i<count;++i) {
        JsonObject entry=list.add<JsonObject>(); entry["ssid"]=WiFi.SSID(i);
        entry["rssiDbm"]=WiFi.RSSI(i); entry["channel"]=WiFi.channel(i);
        entry["secure"]=WiFi.encryptionType(i)!=ENC_TYPE_NONE;
    }
    if (!scanPending_) WiFi.scanDelete();
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
    bool connected=WiFi.status()==WL_CONNECTED;
    out["staConnected"]=connected;
    out["staSsid"]=nullptr; out["staIp"]=nullptr; out["rssiDbm"]=nullptr;
    if (connected) { out["staSsid"]=WiFi.SSID(); out["staIp"]=WiFi.localIP().toString(); out["rssiDbm"]=WiFi.RSSI(); }
    out["apSsid"]=nullptr; out["apIp"]=nullptr;
    if (apOn_) { out["apSsid"]=apSsid_; out["apIp"]=WiFi.softAPIP().toString(); }
    out["ageMs"]=0; out["applying"]=applyPending_;
    out["fallbackActive"]=fallbackActive_ && apOn_;
    out["scanRunning"]=scanPending_;
    if (persisted_<0) out["configPersisted"]=nullptr; else out["configPersisted"]=persisted_==1;
    out["configError"]=configError_;
    const auto sta=WiFi.status();
    out["staStatus"]=!staWanted_?"disabled":connected?"connected":
        sta==WL_NO_SSID_AVAIL?"network-not-found":sta==WL_CONNECT_FAILED?"connection-failed":
        sta==WL_WRONG_PASSWORD?"wrong-password":"connecting";
}
