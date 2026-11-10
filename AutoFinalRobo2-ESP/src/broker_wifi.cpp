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
uint8_t Broker::readyWifiClients(){
    uint8_t count=0;for(auto &s:sessions_)if(s.used && s.ready){auto *peer=ws_.client(s.id);if(peer && peer->status()==WS_CONNECTED)++count;}return count;
}
void Broker::draftStatus(JsonObject out) {
    out["pending"]=wifiAwaitWeb_;out["jobId"]=wifiAwaitWeb_?wifiJob_:0;
    if(wifiAwaitWeb_){out["mode"]=modeName(wifiRequestedMode_);out["ssid"]=oledDraft_.ssid;out["expiresInMs"]=(uint32_t)(millis()-wifiJobAt_)<115000u?115000u-(uint32_t)(millis()-wifiJobAt_):0;}
}
bool Broker::dispatchWifiDraft(const BrokerRequest &q,uint32_t now) {
    bool submit=!strcmp(q.command,"wifi.draft.submit"),cancel=!strcmp(q.command,"wifi.draft.cancel");
    if(!submit && !cancel)return false;
    if(!productReady_ || !wifiAwaitWeb_ || afr2_get_u32(q.body)!=wifiJob_ || (uint32_t)(now-wifiJobAt_)>=115000u){error(q,"BAD_REQUEST",F("El pedido OLED vencio o fue cancelado."));return true;}
    if(cancel){wifiAwaitWeb_=false;eraseConfig(oledDraft_);wifiDraftReady_=false;wifiPhase_=0;wifiResult_=5;}
    else {
        if((strcmp(q.wifi.mode,modeName(wifiRequestedMode_)) && strcmp(q.wifi.mode,"STA") && strcmp(q.wifi.mode,"AP_STA")) || !WifiService::valid(q.wifi)){error(q,"BAD_REQUEST",F("Modo o credenciales fuera del pedido OLED."));return true;}
        if(!wifi_.stage(q.wifi,now)){error(q,"INTERNAL_ERROR",F("No se pudo guardar o aplicar la configuracion Wi-Fi."));return true;}
        eraseConfig(oledDraft_);wifiDraftReady_=false;wifiAwaitWeb_=false;wifiPhase_=0;wifiResult_=2;wifiJobAt_=now;
    }
    JsonDocument f;f["payload"]["draftReady"]=submit;f["payload"]["applied"]=submit;f["payload"]["restartScheduled"]=submit;answer(q,f);publishStatus();return true;
}
void Broker::clearWifiNetworks() {
    for(auto &name:wifiNetworks_)name=String();
    wifiNetworkCount_=0;wifiScanAt_=0;
}
void Broker::scanForOled(JsonObjectConst result) {
    clearWifiNetworks();
    bool forOled=wifiScanOled_;wifiScanOled_=false;wifiNetworkCount_=wifiNetworkIndex_=0;
    ++wifiScanId_;if(!wifiScanId_)++wifiScanId_;wifiScanPage_=0;wifiScanTruncated_=result["truncated"]|false;
    if(result["status"]!="completed"){if(forOled)wifiResult_=4;return;}
    // Web-only scans already own their JSON result; do not retain another SSID list.
    if(!forOled)return;
    wifiScanAt_=millis();
    for(JsonObjectConst entry:result["networks"].as<JsonArrayConst>()) {
        JsonString name=entry["ssid"].as<JsonString>();const char *s=name.c_str();
        if(s && name.size()<=32 && name.size()==strlen(s) && wifiNetworkCount_<16){wifiSecurity_[wifiNetworkCount_]=entry["securityCode"]|0;wifiNetworks_[wifiNetworkCount_++]=s;}
    }
    if(forOled){wifiPhase_=9;wifiResult_=3;}
}
bool Broker::finishWifi(const BrokerRequest &req,const MegaResult &r) {
    if(r.command!=AFR2_EXT_WIFI_POLL && r.command!=AFR2_EXT_WIFI_UPDATE)return false;
    if(r.timeout || r.transportFault || r.length<9 || r.payload[8]!=AFR2_RESULT_OK) {
        if(!strcmp(req.command,"_wifi_status") || !strcmp(req.command,"_wifi_details")){wifiDetailsPending_=false;return true;}
        if(!strcmp(req.command,"_wifi_credentials")){memset(wifiCredentials_,0,sizeof(wifiCredentials_));wifiPhase_=0;wifiResult_=4;return true;}
        if(!strcmp(req.command,"_wifi_network")){wifiResult_=4;wifiPhase_=0;clearWifiNetworks();return true;}
        if(!strcmp(req.command,"_wifi_result") && wifiResetPending_)wifiResetPending_=false;
        if(r.command==AFR2_EXT_WIFI_POLL){wifiOffset_=0;return true;}
        wifiJob_=0;wifiResult_=0;wifiPhase_=0;wifiAwaitWeb_=false;wifiDraftReady_=false;eraseConfig(oledDraft_);return true;
    }
    if(r.command==AFR2_EXT_WIFI_UPDATE) {
        if(!strcmp(req.command,"_wifi_result") && wifiPhase_==5)wifiPhase_=0;
        if(!strcmp(req.command,"_wifi_status"))wifiDetailsPending_=true;
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
        if(op!=12)clearWifiNetworks();
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
    if(op==12){
        if(!wifiScanAt_ || total!=5 || afr2_get_u32((uint8_t *)wifiCredentials_)!=wifiScanId_ || (uint8_t)wifiCredentials_[4]>1){wifiResult_=4;return true;}
        wifiScanAt_=millis();
        wifi_.holdFallback(millis());wifiScanPage_=(uint8_t)wifiCredentials_[4];wifiNetworkIndex_=wifiScanPage_*8u;wifiPhase_=9;wifiResult_=3;
    } else if(op==11){
        WifiConfig saved;
        if(!wifi_.savedConfig(wifiRequestedMode_,saved)){wifiResult_=4;return true;}
        memset(wifiCredentials_,0,sizeof(wifiCredentials_));
        strlcpy(wifiCredentials_,saved.ssid,33);strlcpy(wifiCredentials_+33,saved.password,64);eraseConfig(saved);
        wifiNetworkIndex_=0;wifiPhase_=8;
    } else if(op==1){wifiStatusAt_=0;wifiResult_=3;}
    else if(op==2){wifi_.holdFallback(millis());if(wifi_.scan()){wifiScanOled_=true;wifiPhase_=4;}else wifiResult_=4;}
    else if(op==3) {
        bool hasClient=readyWifiClients()>0 || ((WiFi.getMode()&WIFI_AP) && WiFi.softAPgetStationNum()>0);
        if(!hasClient){wifiResult_=6;return true;}
        if(total>32 || memchr(wifiCredentials_,0,total)){wifiResult_=4;return true;}
        strlcpy(oledDraft_.mode,modeName(wifiRequestedMode_),sizeof(oledDraft_.mode));
        memcpy(oledDraft_.ssid,wifiCredentials_,total);oledDraft_.ssid[total]=0;wifiAwaitWeb_=true;wifiPhase_=2;wifi_.holdFallback(millis());publishStatus();
    } else if(op==4) {
        char *end=(char *)memchr(wifiCredentials_,0,total);
        if(!end || end-wifiCredentials_>32 || total-(end-wifiCredentials_)-1>63){wifiResult_=4;return true;}
        strlcpy(oledDraft_.mode,modeName(wifiRequestedMode_),sizeof(oledDraft_.mode));
        strlcpy(oledDraft_.ssid,wifiCredentials_,sizeof(oledDraft_.ssid));
        memcpy(oledDraft_.password,end+1,total-(end-wifiCredentials_)-1);
        oledDraft_.password[total-(end-wifiCredentials_)-1]=0;
        volatile char *wipe=wifiCredentials_;for(size_t i=0;i<sizeof(wifiCredentials_);i++)wipe[i]=0;
        if(!WifiService::valid(oledDraft_) || !wifi_.stage(oledDraft_,millis())){eraseConfig(oledDraft_);wifiResult_=4;}
        else{wifiResult_=2;wifiPhase_=0;wifiDraftReady_=false;eraseConfig(oledDraft_);publishStatus();}
    } else if(op==5) {
        if(!wifiDraftReady_ || afr2_get_u32(p+19)!=wifiDraftToken_ || !wifi_.stage(oledDraft_,millis()))wifiResult_=4;
        else{wifiResult_=2;wifiPhase_=0;wifiDraftReady_=false;eraseConfig(oledDraft_);}
    } else if(op==9){
        if(wifi_.disconnectStation()){wifiResult_=2;wifiPhase_=7;}else wifiResult_=4;
    } else if(op==10){
        wifiResult_=wifi_.changeMode(wifiRequestedMode_)?2:4;wifiStatusAt_=0;publishStatus();
    } else if(op==6){eraseConfig(oledDraft_);wifiDraftReady_=false;wifiDraftToken_=0;wifiResult_=5;publishStatus();}
    else if(op==7){if(wifi_.retry()){wifiRequestedMode_=1;wifiResult_=2;wifiPhase_=6;}else wifiResult_=4;}
    // AFR2-20261108-06: respuesta del selector sin Wi-Fi. 13 Iniciar AP; 14 Seguir esperando (2 bytes: segundos u16LE).
    else if(op==13){wifiResult_=wifi_.startAp()?3:4;wifiStatusAt_=0;}
    else if(op==AFR2_WIFI_OP_ASK_FALLBACK){wifiResult_=total==1 && (uint8_t)wifiCredentials_[0]<=1 && wifi_.setAskFallback(wifiCredentials_[0]!=0)?3:4;wifiStatusAt_=0;}
    else if(op==AFR2_WIFI_OP_AP_STOP){wifiResult_=wifi_.stopAp()?3:4;wifiStatusAt_=0;}
    else if(op==14){wifiResult_=total==2 && wifi_.answerAsk(false,afr2_get_u16((const uint8_t *)wifiCredentials_),millis())?3:4;}
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
        wifiAwaitWeb_=wifiDraftReady_=false;wifiJob_=0;wifiOffset_=0;eraseConfig(oledDraft_);clearWifiNetworks();return;
    }
    if(wifiPhase_!=0 || wifiAwaitWeb_ || wifiDraftReady_ || wifi_.busy() || (wifiScanId_ && (uint32_t)(now-wifiJobAt_)<60000u))wifi_.holdFallback(now);
    bool hasClient=readyWifiClients()>0 || ((WiFi.getMode()&WIFI_AP) && WiFi.softAPgetStationNum()>0);
    if(wifiAwaitWeb_ && !hasClient){wifiAwaitWeb_=false;wifiDraftReady_=false;eraseConfig(oledDraft_);wifiPhase_=0;wifiResult_=6;}
    if(wifiAwaitWeb_ && (uint32_t)(now-wifiJobAt_)>=115000u){wifiAwaitWeb_=false;wifiDraftReady_=false;eraseConfig(oledDraft_);wifiResult_=5;publishStatus();}
    if(wifiDraftReady_ && (uint32_t)(now-wifiJobAt_)>=115000u){wifiDraftReady_=false;wifiDraftToken_=0;eraseConfig(oledDraft_);}
    // A job can be cancelled locally at any phase. Poll before emitting its result.
    if((uint32_t)(now-wifiPollAt_)>=400u) {
        uint8_t b=wifiPhase_==1?wifiOffset_:0;
        if(productRequest(AFR2_EXT_WIFI_POLL,&b,1)){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_poll",33);waiting_=true;wifiPollAt_=now;}return;
    }
    if(wifiPhase_==8){
        uint8_t b[39]={5};afr2_put_u32(b+1,wifiJob_);b[5]=wifiNetworkIndex_;b[6]=33+strlen(wifiCredentials_+33);
        uint8_t count=b[6]-wifiNetworkIndex_;if(count>32)count=32;memcpy(b+7,wifiCredentials_+wifiNetworkIndex_,count);
        if(productRequest(AFR2_EXT_WIFI_UPDATE,b,7+count)){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_credentials",33);waiting_=true;wifiNetworkIndex_+=count;if(wifiNetworkIndex_==b[6]){wifiPhase_=0;memset(wifiCredentials_,0,sizeof(wifiCredentials_));}}return;
    }
    if(wifiDetailsPending_){
        if(productRequest(AFR2_EXT_WIFI_UPDATE,wifiDetails_,sizeof(wifiDetails_))){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_details",33);waiting_=true;wifiDetailsPending_=false;}return;
    }
    if(!wifiStatusAt_ || (uint32_t)(now-wifiStatusAt_)>=2000u) {
        JsonDocument status;wifi_.status(status.to<JsonObject>());uint8_t b[46]={};
        const char *m=status["mode"]|"OFF",*ssid=status["staSsid"]|(status["apSsid"]|"");
        b[1]=!strcmp(m,"STA")?1:!strcmp(m,"AP")?2:!strcmp(m,"AP_STA")?3:0;b[2]=status["staConnected"]|false;
        afr2_put_u16(b+3,(int16_t)(status["rssiDbm"]|-127));b[5]=strlen(ssid)>32?32:strlen(ssid);memcpy(b+6,ssid,b[5]);
        IPAddress ip,ap;ip.fromString(status["staIp"]|"0.0.0.0");ap.fromString(status["apIp"]|"0.0.0.0");
        for(uint8_t i=0;i<4;i++){b[6+b[5]+i]=ip[i];b[10+b[5]+i]=ap[i];}
        memset(wifiDetails_,0,sizeof(wifiDetails_));wifiDetails_[0]=4;wifiDetails_[1]=b[1];wifiDetails_[2]=b[2];
        wifiDetails_[3]=status["securityCode"]|0;wifiDetails_[4]=status["apSecurityCode"]|0;wifiDetails_[5]=readyWifiClients();wifiDetails_[18]=status["askFallback"]|false;
        IPAddress mask,gateway;mask.fromString(status[b[2]?"staMask":"apMask"]|"0.0.0.0");gateway.fromString(status[b[2]?"staGateway":"apGateway"]|"0.0.0.0");
        for(uint8_t i=0;i<4;i++){wifiDetails_[6+i]=ip[i];wifiDetails_[10+i]=mask[i];wifiDetails_[14+i]=gateway[i];}
        if(productRequest(AFR2_EXT_WIFI_UPDATE,b,14+b[5])){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_status",33);waiting_=true;wifiStatusAt_=now;}return;
    }
    uint8_t pageEnd=wifiNetworkCount_<wifiScanPage_*8u+8u?wifiNetworkCount_:wifiScanPage_*8u+8u;
    uint8_t pageCount=pageEnd>=wifiScanPage_*8u?pageEnd-wifiScanPage_*8u:0;
    if(wifiPhase_==9){
        uint8_t b[13]={6};afr2_put_u32(b+1,wifiJob_);afr2_put_u32(b+5,wifiScanId_);b[9]=wifiScanPage_;b[10]=wifiNetworkCount_;b[11]=wifiScanTruncated_;b[12]=pageCount;
        if(productRequest(AFR2_EXT_WIFI_UPDATE,b,sizeof(b))){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_network",33);waiting_=true;wifiPhase_=5;}return;
    }
    if(wifiPhase_==5 && wifiNetworkIndex_<pageEnd) {
        const String &name=wifiNetworks_[wifiNetworkIndex_];uint8_t b[41]={1,(uint8_t)(wifiNetworkIndex_%8u),(uint8_t)name.length(),wifiSecurity_[wifiNetworkIndex_],pageCount};afr2_put_u32(b+5,wifiJob_);memcpy(b+9,name.c_str(),name.length());
        if(productRequest(AFR2_EXT_WIFI_UPDATE,b,9+b[2])){pending_=BrokerRequest{};strlcpy(pending_.command,"_wifi_network",33);waiting_=true;wifiNetworkIndex_++;}return;
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
    if(wifiPhase_==7){
        JsonDocument state;wifi_.status(state.to<JsonObject>());
        if(!(state["staConnected"]|true)){wifiResult_=3;wifiPhase_=0;wifiStatusAt_=0;publishStatus();}
        else if((uint32_t)(now-wifiJobAt_)>=5000u){wifiResult_=4;wifiPhase_=0;}
    }
    if(wifiPhase_==6) {
        JsonDocument status;wifi_.status(status.to<JsonObject>());
        if(!(status["applying"]|false)) {
            bool ap=wifiRequestedMode_==2;bool connected=status["staConnected"]|false;
            if((ap && status["mode"]=="AP" && !status["apIp"].isNull() && status["apIp"]!="0.0.0.0") || (!ap && connected)){wifiResult_=3;wifiPhase_=0;wifiStatusAt_=0;publishStatus();}
            else if(!status["configError"].isNull() || status["staStatus"]=="wrong-password" || status["staStatus"]=="network-not-found" ||
                    status["staStatus"]=="connection-failed" || (uint32_t)(now-wifiJobAt_)>=25000u){wifiResult_=4;wifiPhase_=0;}
        }
    }
}
