// AFR2-20261109-09: bound concurrent large downloads without blocking the WS loop.
#include "http_assets.h"
#include <LittleFS.h>

namespace {
constexpr size_t largeBytes=65536;
struct WaitingAsset { AsyncWebServerRequestPtr request; const WebAsset *asset=nullptr; uint32_t since=0; };
WaitingAsset waiting[3];
uint8_t count=0;
bool active=false;
uint32_t served=0,rejected=0;
void unavailable(AsyncWebServerRequest *request) {
    ++rejected;
    auto *response=request->beginResponse(503,"text/plain","Asset server busy; retry shortly");
    response->addHeader("Retry-After","1");response->addHeader("Cache-Control","no-store");request->send(response);
}
void sendAsset(AsyncWebServerRequest *request,const WebAsset &asset,bool large) {
    auto *response=request->beginResponse(LittleFS,asset.path,asset.mime);
    if(!response){unavailable(request);return;}
    response->addHeader("Content-Encoding","gzip");response->addHeader("Vary","Accept-Encoding");
    response->addHeader("Cache-Control",asset.route=="/index.html"?"no-store":"no-cache");
    response->addHeader("ETag",asset.etag);response->addHeader("X-Content-Type-Options","nosniff");
    if(large){active=true;request->onDisconnect([](){active=false;});}
    ++served;request->send(response);
}
void pop() {
    for(uint8_t i=1;i<count;++i)waiting[i-1]=std::move(waiting[i]);
    if(count)waiting[--count]=WaitingAsset{};
}
}
void serveAsset(AsyncWebServerRequest *request,const WebAsset &asset) {
    if(asset.bytes<largeBytes){sendAsset(request,asset,false);return;}
    if(count==3 || ESP.getFreeHeap()<4096u){unavailable(request);return;}
    // pause() supplies a weak observer that expires if the browser disconnects.
    // No file or response send buffer is allocated until tickAssets admits it.
    waiting[count++]={request->pause(),&asset,millis()};
    tickAssets(millis());
}
void tickAssets(uint32_t now) {
    // Expired or timed-out observers never occupy admission slots indefinitely.
    for(uint8_t i=0;i<count;){
        auto request=waiting[i].request.lock();
        const bool timeout=(uint32_t)(now-waiting[i].since)>=30000u;
        if(!request || timeout){
            for(uint8_t j=i+1;j<count;++j)waiting[j-1]=std::move(waiting[j]);
            waiting[--count]=WaitingAsset{};
            if(request)unavailable(request.get());
        }else ++i;
    }
    if(active || !count || ESP.getFreeHeap()<8192u || ESP.getMaxFreeBlockSize()<4096u)return;
    auto request=waiting[0].request.lock();const WebAsset *asset=waiting[0].asset;pop();
    if(request)sendAsset(request.get(),*asset,true);
}
void assetStatus(JsonObject out) {
    out["largeActive"]=active;out["largeWaiting"]=count;out["served"]=served;out["rejected"]=rejected;
}
