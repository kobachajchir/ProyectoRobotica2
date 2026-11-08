/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
// AFR2-20261023-11. Adaptation of ESP01 Wi-Fi states and async scan, new config schema.
#include "wifi_service.h"
#include "wifi_defaults.h"
#include <ESP8266WiFi.h>
#include <string.h>
extern "C" {
#include <user_interface.h>
}
static uint8_t scanSecurity(uint8_t enc){
    switch(enc){case ENC_TYPE_NONE:return 1;case ENC_TYPE_WEP:return 2;case ENC_TYPE_TKIP:return 3;case ENC_TYPE_CCMP:return 4;case ENC_TYPE_AUTO:return 5;default:return 6;}
}
static const char *securityName(uint8_t n){static const char *names[]={"unknown","open","WEP","WPA","WPA2","WPA/WPA2","protected"};return names[n<=6?n:0];}

/* v2 keeps AP and STA identities separate. Never expose this document. */
static void migrateWifi(JsonDocument &doc) {
    if(doc["schema"]=="afr2-wifi-2")return;
    String mode=doc["mode"]|"STA",ssid=doc["ssid"]|"",password=doc["password"]|"";
    doc.clear();doc["schema"]="afr2-wifi-2";doc["mode"]=mode;
    JsonObject part=doc[mode=="AP"?"ap":"sta"].to<JsonObject>();part["ssid"]=ssid;part["password"]=password;
}
/* AFR2-20261107-38: red STA por defecto (wifi_defaults.local.h). Vacía = sin red por defecto. */
static bool defaultStation(WifiConfig &c){
    strlcpy(c.mode,"STA",sizeof(c.mode));strlcpy(c.ssid,AFR2_DEFAULT_STA_SSID,sizeof(c.ssid));strlcpy(c.password,AFR2_DEFAULT_STA_PASSWORD,sizeof(c.password));
    return WifiService::valid(c);
}
static bool readPart(JsonDocument &doc,const char *part,WifiConfig &c){
    JsonObject p=doc[part].as<JsonObject>();
    if(!p["ssid"].is<const char *>() || !p["password"].is<const char *>() || strlen(p["ssid"])>32 || strlen(p["password"])>63)return false;
    strlcpy(c.ssid,p["ssid"],sizeof(c.ssid));strlcpy(c.password,p["password"],sizeof(c.password));return c.ssid[0] && (!c.password[0] || strlen(c.password)>=8);
}
bool WifiService::savedConfig(uint8_t mode,WifiConfig &c){
    JsonDocument doc;if(!storage_.loadWifi(doc))return false;migrateWifi(doc);
    strlcpy(c.mode,mode==2?"AP":mode==3?"AP_STA":"STA",sizeof(c.mode));
    return readPart(doc,mode==2?"ap":"sta",c) && valid(c);
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
    if(storage_.loadWifi(doc)){
        migrateWifi(doc);const char *mode=doc["mode"]|"";
        strlcpy(config.mode,mode,sizeof(config.mode));
        WifiConfig ap;if(readPart(doc,"ap",ap)){apSsid_=ap.ssid;strlcpy(apPassword_,ap.password,sizeof(apPassword_));}
        WifiConfig previous;strlcpy(previous.mode,"STA",sizeof(previous.mode));
        hasPrevious_=readPart(doc,"staPrevious",previous) && valid(previous) && strcmp(previous.ssid,doc["sta"]["ssid"]|"");
        WifiConfig fallback;if(!hasPrevious_ && strcmp(mode,"AP") && defaultStation(fallback) && strcmp(fallback.ssid,doc["sta"]["ssid"]|""))hasPrevious_=true; // la red por defecto actúa de respaldo
        currentGood_=doc["staGood"]|false;
        if(readPart(doc,!strcmp(mode,"AP")?"ap":"sta",config) && valid(config)){persisted_=1;apply(config);return;}
    }
    // Sin configuración guardada: usar la red por defecto (en RAM) y, si no conecta, el AP de provisión.
    WifiConfig fallback;if(defaultStation(fallback)){persisted_=0;hasPrevious_=false;apply(fallback);return;}
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
        if (wantAp) apOn_=WiFi.softAP(apSsid_.c_str(),apPassword_[0]?apPassword_:nullptr);
        if (WiFi.begin(c.ssid,c.password)==WL_CONNECT_FAILED) configError_="STA_START_FAILED";
    }
    if (wantAp && !apOn_) { configError_="AP_START_FAILED"; startFallback(); }
    usingPrevious_=false;staStarted_=lastRetry_=millis();
}
bool WifiService::stage(const WifiConfig &c, uint32_t now) {
    if (busy() || !valid(c)) return false;
    if(!c.persist)return false;
    JsonDocument doc;storage_.loadWifi(doc);migrateWifi(doc);
    doc["schema"]="afr2-wifi-2";doc["mode"]=c.mode;
    if(strcmp(c.mode,"AP")){
        // AFR2-20261106-37: al cambiar de red STA, la anterior pasa a respaldo solo si alguna vez conecto.
        const char *oldSsid=doc["sta"]["ssid"]|"";
        if(oldSsid[0] && strcmp(oldSsid,c.ssid) && (doc["staGood"]|false)){
            String ssid=oldSsid,password=doc["sta"]["password"]|"";
            JsonObject previous=doc["staPrevious"].to<JsonObject>();previous["ssid"]=ssid;previous["password"]=password;
        }
        if(strcmp(oldSsid,c.ssid))doc["staGood"]=false;
    }
    JsonObject part=doc[!strcmp(c.mode,"AP")?"ap":"sta"].to<JsonObject>();part["ssid"]=c.ssid;part["password"]=c.password;
    if(!storage_.saveWifi(doc))return false;
    persisted_=1;
    pending_=c; stagedAt_=now; applyPending_=true; return true;
}
void WifiService::tick(uint32_t now) {
    // Broker waits for acknowledgement/drain, then restarts; never apply in place.
    if (staWanted_ && !disconnectedByUser_ && WiFi.status()!=WL_CONNECTED) {
        if ((uint32_t)(now-staStarted_)>=15000u && hasPrevious_ && !usingPrevious_ && !scanPending_ && !applyPending_ && (uint32_t)(now-lastRetry_)>=15000u) {
            // AFR2-20261106-37: la red configurada no aparece o no conecta; probar la ultima que funciono.
            if(beginStation(true)){lastRetry_=now;staStarted_=now;} // nueva gracia de 15 s para la anterior
        }
        if (!apOn_ && (uint32_t)(now-staStarted_)>=15000u) startFallback();
        if ((uint32_t)(now-lastRetry_)>=30000u && !scanPending_ && !applyPending_) {
            lastRetry_=now;
            if(hasPrevious_)(void)beginStation(!usingPrevious_); // alternar configurada / anterior
            else WiFi.reconnect();
        }
    } else if (staWanted_) {
        staStarted_=now; // grace period starts again on a later disconnection
        if(!usingPrevious_ && !currentGood_ && WiFi.status()==WL_CONNECTED){
            // Primera conexion exitosa de la red configurada: queda apta para ser «anterior» en el futuro.
            // AFR2-20261107-53: un solo intento por arranque y solo con wifi.json guardado. Antes, sin archivo
            // (red por defecto en RAM) o si fallaba la escritura, abria LittleFS en cada vuelta del loop y
            // chocaba con las lecturas del servidor HTTP asincronico (descarga del .glb) y reiniciaba el ESP.
            currentGood_=true;
            if(persisted_==1){JsonDocument doc;if(storage_.loadWifi(doc)){migrateWifi(doc);doc["staGood"]=true;(void)storage_.saveWifi(doc);}}
        }
    }
    if (scanPending_) {
        int n=WiFi.scanComplete();
        if (n!=WIFI_SCAN_RUNNING) {
            scanPending_=false;
            if (!scanTimedOut_) scanReady_=true;
            else {WiFi.scanDelete();if(scanRestoreAp_ && !staWanted_){WiFi.mode(WIFI_AP);scanRestoreAp_=false;}}
        } else if (!scanTimedOut_ && (uint32_t)(now-scanStarted_)>20000u) {
            // scanDelete does not cancel a running SDK scan. Keep it busy until it actually ends.
            scanTimedOut_=true; scanReady_=true;
        }
    }
}
bool WifiService::beginStation(bool previous){
    JsonDocument doc;bool stored=storage_.loadWifi(doc);if(stored)migrateWifi(doc);
    WifiConfig c;strlcpy(c.mode,"STA",sizeof(c.mode));
    bool ok=stored && readPart(doc,previous?"staPrevious":"sta",c) && valid(c);
    if(!ok)ok=defaultStation(c); // sin anterior guardada (o sin archivo): red por defecto
    if(!ok)return false;
    if(WiFi.begin(c.ssid,c.password)==WL_CONNECT_FAILED)return false;
    usingPrevious_=previous;return true;
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
    WifiConfig c;if(!savedConfig(mode,c))return false;
    return stage(c,millis());
}
bool WifiService::takeScan(JsonObject out) {
    if (!scanReady_) return false;
    scanReady_=false; int n=scanTimedOut_?WIFI_SCAN_FAILED:WiFi.scanComplete();
    out["status"]=n>=0?"completed":"failed";
    JsonArray list=out["networks"].to<JsonArray>();
    int count=n>16?16:n; out["truncated"]=n>16;authCount_=0;authScanAt_=millis();
    int16_t selected[16];for(int i=0;i<16;i++)selected[i]=-1;
    for(int i=0;i<n;i++)for(int j=0;j<16;j++)if(selected[j]<0 || WiFi.RSSI(i)>WiFi.RSSI(selected[j])){
        for(int k=15;k>j;k--){selected[k]=selected[k-1];}
        selected[j]=i;break;
    }
    for (int j=0;j<count;++j) {int i=selected[j];
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
    out["staNetwork"]=!staWanted_?"none":usingPrevious_?"previous":"configured";out["previousAvailable"]=hasPrevious_;
    out["ageMs"]=0; out["applying"]=applyPending_;out["restartScheduled"]=applyPending_;
    out["configuredMode"]=applyPending_?pending_.mode:"";
    out["fallbackActive"]=fallbackActive_ && apOn_;
    out["scanRunning"]=scanPending_;
    if (persisted_<0) out["configPersisted"]=nullptr; else out["configPersisted"]=persisted_==1;
    out["configError"]=configError_;
    const auto sta=WiFi.status();
    out["staStatus"]=!staWanted_?"disabled":connected?"connected":disconnectedByUser_?"disconnected":
        sta==WL_NO_SSID_AVAIL?"network-not-found":sta==WL_CONNECT_FAILED?"connection-failed":
        sta==WL_WRONG_PASSWORD?"wrong-password":"connecting";
}
