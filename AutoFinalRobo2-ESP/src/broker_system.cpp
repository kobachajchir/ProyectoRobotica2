#include <user_interface.h>
// AFR2-20261024-07. Build identity, boot reports and deferred restart.
// UART is protocol-only; neither compilation nor UI authoring triggers reset.
#include "broker.h"
#include "afr2_build_info.h"
#include <string.h>

void Broker::systemInfo(JsonObject out) {
    storage_.health(out["filesystem"].to<JsonObject>());
    JsonObject esp=out["esp"].to<JsonObject>();
    esp["version"]=AFR2_FIRMWARE_VERSION;esp["compiledAt"]=__DATE__ " " __TIME__;
    esp["compiler"]=__VERSION__;esp["revision"]=AFR2_BUILD_REVISION;
    esp["core"]=ESP.getCoreVersion();esp["sdk"]=ESP.getSdkVersion();
    esp["bootId"]=mega_.ownBootId();esp["resetReason"]=ESP.getResetReason();
    esp["uptimeMs"]=millis();esp["clockMHz"]=ESP.getCpuFreqMHz();
    esp["sketchBytes"]=ESP.getSketchSize();esp["freeHeapBytes"]=ESP.getFreeHeap();
    esp["flashBytes"]=ESP.getFlashChipRealSize();
    if(infoReady_ && mega_.ready()) {
        JsonObject avr=out["atmega"].to<JsonObject>();
        const char *keys[]={"version","compiledAt","compiler","architecture","revision"};
        for(uint8_t i=0;i<5;i++)avr[keys[i]]=megaInfo_[i];
        avr["bootId"]=infoBoot_;avr["resetCauseBits"]=infoReset_;
        avr["extensionVersion"]=extensionVersion_;
    } else out["atmega"]=nullptr;
}
void Broker::publishBoot(const char *device,uint32_t id,uint8_t cause,const char *observation) {
    JsonDocument report;report["device"]=device;report["bootId"]=id;
    report["resetCauseBits"]=cause;report["observation"]=observation;
    broadcast("system.boot",report);
}
bool Broker::dispatchSystem(const BrokerRequest &req,uint32_t now) {
    if(!strcmp(req.command,"system.info")) {
        JsonDocument f;systemInfo(f["payload"].to<JsonObject>());answer(req,f);return true;
    }
    if(strcmp(req.command,"system.reboot") || req.wireCommand==AFR2_EXT_REBOOT)return false;
    if(mega_.ready() && (!productReady_ || !productStopped_ || (uint32_t)(now-productStateAt_)>1500u)) {
        error(req,"BUSY_IN_FLIGHT","Detene el robot y espera confirmacion antes de reiniciar.",true);return true;
    }
    if(safetyRequested_ || rebootAt_) {
        error(req,"BUSY_IN_FLIGHT","Hay control o parada pendientes; espera confirmacion.",true);return true;
    }
    ++generation_;rebootAt_=now+300u;
    JsonDocument f;f["status"]="accepted";f["payload"]["target"]="esp";
    f["payload"]["restartScheduled"]=true;f["payload"]["previousBootId"]=mega_.ownBootId();answer(req,f);
    return true;
}
bool Broker::finishSystem(const BrokerRequest &req,const MegaResult &r) {
    if(!strcmp(req.command,"_info")) {
        if(r.timeout || r.transportFault || r.payload[8]!=AFR2_RESULT_OK) {infoPage_=0;infoReady_=false;infoAt_=millis();return true;}
        const uint8_t *p=r.payload;uint32_t boot=afr2_get_u32(p+10);
        if(infoPage_ && infoBoot_!=boot){infoPage_=0;infoReady_=false;return true;}
        infoBoot_=boot;infoReset_=p[14];
        memcpy(megaInfo_[infoPage_],p+16,p[15]);megaInfo_[infoPage_][p[15]]=0;
        if(++infoPage_==5){
            infoReady_=true;JsonDocument all;systemInfo(all.to<JsonObject>());broadcast("system.info",all);
            if(lastInfoBoot_!=infoBoot_){lastInfoBoot_=infoBoot_;publishBoot("atmega",infoBoot_,infoReset_,"identity-snapshot");}
        }
        return true;
    }
    if(req.wireCommand!=AFR2_EXT_REBOOT)return false;
    if(r.timeout || r.transportFault || r.payload[8]!=AFR2_RESULT_OK) {
        error(req,"CAPABILITY_UNAVAILABLE","El ATmega no acepto el reinicio; verifica salidas detenidas y EEPROM inactiva.",true,"atmega");return true;
    }
    ++generation_;
    JsonDocument f;f["status"]="accepted";f["payload"]["target"]="atmega";
    f["payload"]["restartScheduled"]=true;f["payload"]["previousBootId"]=afr2_get_u32(r.payload+10);answer(req,f);
    infoReady_=false;infoPage_=0;return true;
}
void Broker::tickSystem(uint32_t now) {
    if(mega_.takeBoot()) {
        ++generation_;infoReady_=false;infoPage_=0;
        publishBoot("atmega",mega_.bootId(),mega_.bootCause(),"uart-boot-event");
    }
    if(wifi_.restartDue(now) && !waiting_ && !deferredClient_){
        bool drained=true;for(auto &s:sessions_)if(s.used)if(auto *p=ws_.client(s.id))if(p->queueLen())drained=false;
        if(drained && !rebootAt_){stopWebOperations();rebootAt_=now+100u;}
    }
    if(rebootAt_ && (int32_t)(now-rebootAt_)>=0){ws_.closeAll(1012,"ESP restarting");ESP.restart();return;}
    if(!mega_.ready()){infoReady_=false;infoPage_=0;return;}
    if(rebootAt_ || waiting_ || mega_.busy() || stopQueued_ || !productReady_ || extensionVersion_<4 || infoReady_)return;
    if((uint32_t)(now-infoAt_)<300u)return;
    uint8_t page=infoPage_;if(productRequest(AFR2_EXT_INFO,&page,1)) {
        pending_=BrokerRequest{};strlcpy(pending_.command,"_info",33);waiting_=true;infoAt_=now;
    }
}
