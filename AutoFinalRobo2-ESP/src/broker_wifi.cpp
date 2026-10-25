#include "afr2_build_info.h"
// AFR2-20261024-02. OLED-originated Wi-Fi jobs. Credentials stay in bounded RAM;
// a Web draft never changes radio configuration before physical confirmation.
#include "broker.h"
#include <ESP8266WiFi.h>
#include <string.h>
extern "C" {
#include <user_interface.h>
}
static void eraseConfig(WifiConfig &c) { volatile char *p=c.password;for(size_t i=0;i<sizeof(c.password);i++)p[i]=0;c=WifiConfig{}; }
static uint32_t draftToken() { uint32_t v=os_random();return v?v:1; }
static const char *modeName(uint8_t mode) {return mode==2?"AP":mode==3?"AP_STA":"STA";}
void Broker::draftStatus(JsonObject out) {
    out["pending"]=wifiAwaitWeb_;out["jobId"]=wifiAwaitWeb_?wifiJob_:0;
    if(wifiAwaitWeb_){out["mode"]=modeName(wifiRequestedMode_);out["ssid"]=oledDraft_.ssid;out["expiresInMs"]=(uint32_t)(millis()-wifiJobAt_)<115000u?115000u-(uint32_t)(millis()-wifiJobAt_):0;}
}
bool Broker::dispatchWifiDraft(const BrokerRequest &q,uint32_t now) {
    bool submit=!strcmp(q.command,"wifi.draft.submit"),cancel=!strcmp(q.command,"wifi.draft.cancel");
    if(!submit && !cancel)return false;
    if(!productReady_ || !wifiAwaitWeb_ || afr2_get_u32(q.body)!=wifiJob_ || (uint32_t)(now-wifiJobAt_)>=115000u){error(q,"BAD_REQUEST","El pedido OLED vencio o fue cancelado.");return true;}
    if(cancel){wifiAwaitWeb_=false;eraseConfig(oledDraft_);wifiDraftReady_=false;wifiResult_=5;}
    else {
        if(strcmp(q.wifi.mode,modeName(wifiRequestedMode_)) || !WifiService::valid(q.wifi)){error(q,"BAD_REQUEST","Modo o credenciales fuera del pedido OLED.");return true;}
        eraseConfig(oledDraft_);oledDraft_=q.wifi;wifiDraftToken_=draftToken();wifiDraftReady_=true;wifiAwaitWeb_=false;wifiResult_=1;wifiJobAt_=now;
    }
    JsonDocument f;f["payload"]["draftReady"]=submit;f["payload"]["applied"]=false;answer(q,f);publishStatus();return true;
}
void Broker::scanForOled(JsonObjectConst result) {
    if(!wifiScanOled_)return;
    wifiScanOled_=false;wifiNetworkCount_=wifiNetworkIndex_=0;
    if(result["status"]!="completed"){wifiResult_=4;return;}
    for(JsonObjectConst entry:result["networks"].as<JsonArrayConst>()) {
        JsonString name=entry["ssid"].as<JsonString>();const char *s=name.c_str();
        if(s && name.size() && name.size()<=32 && name.size()==strlen(s) && wifiNetworkCount_<8)wifiNetworks_[wifiNetworkCount_++]=s;
    }
    wifiPhase_=5;wifiResult_=3;
}
bool Broker::finishWifi(const BrokerRequest &req,const MegaResult &r) {
    if(r.command!=AFR2_EXT_WIFI_POLL && r.command!=AFR2_EXT_WIFI_UPDATE)return false;
    if(r.timeout || r.transportFault || r.length<9 || r.payload[8]!=AFR2_RESULT_OK) {
        if(!strcmp(req.command,"_wifi_result") && wifiResetPending_)wifiResetPending_=false;
        if(r.command==AFR2_EXT_WIFI_POLL){wifiOffset_=0;return true;}
        wifiJob_=0;wifiResult_=0;wifiPhase_=0;wifiAwaitWeb_=false;wifiDraftReady_=false;eraseConfig(oledDraft_);return true;
    }
    if(r.command==AFR2_EXT_WIFI_UPDATE) {
        if(!strcmp(req.command,"_wifi_result") && wifiResetPending_) {
            wifiResetPending_=false;
            if(wifiResetGeneration_==generation_){++generation_;rebootAt_=millis()+300u;}
        }
        return true;
    }
    const uint8_t *p=r.payload;
    if(!p[9])return true;
    uint32_t job=afr2_get_u32(p+10);uint8_t op=p[14],total=p[15],offset=p[16],count=p[17];
    if(job!=wifiJob_) {
        wifiJob_=job;wifiOp_=op;wifiJobAt_=millis();wifiOffset_=0;wifiTotal_=total;
        wifiRequestedMode_=p[18];wifiAwaitWeb_=false;wifiResult_=0;wifiPhase_=1;
        memset(wifiCredentials_,0,sizeof(wifiCredentials_));
        if(op==3 || op==4){eraseConfig(oledDraft_);wifiDraftReady_=false;wifiDraftToken_=0;}
    }
    if(op!=wifiOp_ || total!=wifiTotal_ || offset!=wifiOffset_ || (uint16_t)offset+count>sizeof(wifiCredentials_)-1){wifiOffset_=0;return true;}
    if(wifiPhase_!=1)return true;
    if(count)memcpy(wifiCredentials_+offset,p+23,count);
    wifiOffset_+=count;if(wifiOffset_<total)return true;
    wifiOffset_=0;wifiPhase_=0;
    if(op==1){wifiStatusAt_=0;wifiResult_=3;}
    else if(op==2){if(wifi_.scan()){wifiScanOled_=true;wifiPhase_=4;}else wifiResult_=4;}
    else if(op==3) {
        bool browser=false;for(auto &s:sessions_)if(s.used && s.ready)browser=true;
        if(!browser || total>32 || memchr(wifiCredentials_,0,total)){wifiResult_=4;return true;}
        strlcpy(oledDraft_.mode,modeName(wifiRequestedMode_),sizeof(oledDraft_.mode));
        memcpy(oledDraft_.ssid,wifiCredentials_,total);oledDraft_.ssid[total]=0;wifiAwaitWeb_=true;wifiPhase_=2;publishStatus();
    } else if(op==4) {
        char *end=(char *)memchr(wifiCredentials_,0,total);
        if(!end || end-wifiCredentials_>32 || total-(end-wifiCredentials_)-1>63){wifiResult_=4;return true;}
        strlcpy(oledDraft_.mode,modeName(wifiRequestedMode_),sizeof(oledDraft_.mode));
        strlcpy(oledDraft_.ssid,wifiCredentials_,sizeof(oledDraft_.ssid));
        memcpy(oledDraft_.password,end+1,total-(end-wifiCredentials_)-1);
        oledDraft_.password[total-(end-wifiCredentials_)-1]=0;
        if(!WifiService::valid(oledDraft_))wifiResult_=4;
        else{wifiDraftToken_=draftToken();wifiDraftReady_=true;wifiResult_=1;}
        volatile char *wipe=wifiCredentials_;for(size_t i=0;i<sizeof(wifiCredentials_);i++)wipe[i]=0;
    } else if(op==5) {
        if(!wifiDraftReady_ || afr2_get_u32(p+19)!=wifiDraftToken_ || !wifi_.stage(oledDraft_,millis()))wifiResult_=4;
        else{wifiResult_=2;wifiPhase_=6;wifiDraftReady_=false;eraseConfig(oledDraft_);}
    } else if(op==6){eraseConfig(oledDraft_);wifiDraftReady_=false;wifiDraftToken_=0;wifiResult_=5;publishStatus();}
    else if(op==7){if(wifi_.retry()){wifiResult_=2;wifiPhase_=6;}else wifiResult_=4;}
    else if(op==8) {
        uint32_t now=millis();
        if(extensionVersion_<5 || !productStopped_ || (uint32_t)(now-productStateAt_)>1500u || safetyRequested_ || rebootAt_)wifiResult_=4;
        else {
            wifiResetPending_=true;wifiResetAt_=now;wifiResetGeneration_=++generation_;wifiResult_=2;
        }
    }
    return true;
}
void Broker::tickWifi(uint32_t now) {
    if(wifiResetPending_ && (uint32_t)(now-wifiResetAt_)>=3000u){wifiResetPending_=false;wifiResult_=4;}
    if(!productReady_ || !(productFeatures_&AFR2_EXT_F_WIFI)) {
        wifiAwaitWeb_=wifiDraftReady_=false;wifiJob_=0;wifiOffset_=0;eraseConfig(oledDraft_);return;
    }
    if(wifiAwaitWeb_ && (uint32_t)(now-wifiJobAt_)>=115000u){wifiAwaitWeb_=false;wifiDraftReady_=false;eraseConfig(oledDraft_);wifiResult_=5;publishStatus();}
    if(wifiDraftReady_ && (uint32_t)(now-wifiJobAt_)>=115000u){wifiDraftReady_=false;wifiDraftToken_=0;eraseConfig(oledDraft_);}
    // A job can be cancelled locally at any phase. Poll before emitting its result.
    if((uint32_t)(now-wifiPollAt_)>=400u) {
        uint8_t b=wifiPhase_==1?wifiOffset_:0;
        if(productRequest(AFR2_EXT_WIFI_POLL,&b,1)){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_poll",33);waiting_=true;wifiPollAt_=now;}return;
    }
    if(!wifiStatusAt_ || (uint32_t)(now-wifiStatusAt_)>=2000u) {
        JsonDocument status;wifi_.status(status.to<JsonObject>());uint8_t b[46]={};
        const char *m=status["mode"]|"OFF",*ssid=status["staSsid"]|(status["apSsid"]|"");
        b[1]=!strcmp(m,"STA")?1:!strcmp(m,"AP")?2:!strcmp(m,"AP_STA")?3:0;b[2]=status["staConnected"]|false;
        afr2_put_u16(b+3,(int16_t)(status["rssiDbm"]|-127));b[5]=strlen(ssid)>32?32:strlen(ssid);memcpy(b+6,ssid,b[5]);
        IPAddress ip,ap;ip.fromString(status["staIp"]|"0.0.0.0");ap.fromString(status["apIp"]|"0.0.0.0");
        for(uint8_t i=0;i<4;i++){b[6+b[5]+i]=ip[i];b[10+b[5]+i]=ap[i];}
        if(productRequest(AFR2_EXT_WIFI_UPDATE,b,14+b[5])){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_status",33);waiting_=true;wifiStatusAt_=now;}return;
    }
    if(wifiPhase_==5 && wifiNetworkIndex_<wifiNetworkCount_) {
        const String &name=wifiNetworks_[wifiNetworkIndex_];uint8_t b[35]={1,wifiNetworkIndex_,(uint8_t)name.length()};memcpy(b+3,name.c_str(),name.length());
        if(productRequest(AFR2_EXT_WIFI_UPDATE,b,3+b[2])){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_network",33);waiting_=true;wifiNetworkIndex_++;}return;
    }
    if(wifiResult_) {
        uint8_t b[43]={2};afr2_put_u32(b+1,wifiJob_);b[5]=wifiResult_;afr2_put_u32(b+6,wifiDraftToken_);
        b[10]=wifiResult_==1?strlen(oledDraft_.ssid):0;if(b[10])memcpy(b+11,oledDraft_.ssid,b[10]);
        if(productRequest(AFR2_EXT_WIFI_UPDATE,b,11+b[10])){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_result",33);waiting_=true;wifiResult_=0;}return;
    }
    if(wifiVersions_<2) {
        const char *version=wifiVersions_?webVersion_:AFR2_FIRMWARE_VERSION;
        uint8_t b[18]={3,wifiVersions_,(uint8_t)strlen(version)};memcpy(b+3,version,b[2]);
        if(productRequest(AFR2_EXT_WIFI_UPDATE,b,3+b[2])){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_version",33);waiting_=true;++wifiVersions_;}return;
    }
    if(wifiPhase_==6) {
        JsonDocument status;wifi_.status(status.to<JsonObject>());
        if(!(status["applying"]|false)) {
            bool ap=wifiRequestedMode_==2;bool connected=status["staConnected"]|false;
            if((ap && !status["apIp"].isNull()) || (!ap && connected)){wifiResult_=3;wifiPhase_=0;}
            else if(!status["configError"].isNull() || status["staStatus"]=="wrong-password" || status["staStatus"]=="network-not-found" ||
                    status["staStatus"]=="connection-failed" || (uint32_t)(now-wifiJobAt_)>=25000u){wifiResult_=4;wifiPhase_=0;}
        }
    }
}
