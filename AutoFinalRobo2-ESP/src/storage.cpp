// AFR2-20261023-11. Architecture derived from ESP01 webserver_ws/nvs_littlefs.
#include "storage.h"
#include <LittleFS.h>
#include <ctype.h>
static bool safePath(const String &p) {
    if (!p.startsWith("/") || p.length()>100 || p.indexOf("..")>=0 ||
        p.indexOf("//")>=0 || p.indexOf('\\')>=0 || p.indexOf('%')>=0) return false;
    for (size_t i=0;i<p.length();++i) if ((uint8_t)p[i]<33 || p[i]=='?' || p[i]=='#') return false;
    return true;
}
static String mimeFor(const String &p) {
    if (p.endsWith(".html")) return "text/html; charset=utf-8";
    if (p.endsWith(".js") || p.endsWith(".mjs")) return "application/javascript; charset=utf-8";
    if (p.endsWith(".css")) return "text/css; charset=utf-8";
    if (p.endsWith(".json")) return "application/json; charset=utf-8";
    if (p.endsWith(".png")) return "image/png";
    if (p.endsWith(".svg")) return "image/svg+xml";
    if (p.endsWith(".wasm")) return "application/wasm";
    if (p.endsWith(".glb")) return "model/gltf-binary";
    if (p.endsWith(".webp")) return "image/webp";
    if (p.endsWith(".ico")) return "image/x-icon";
    if (p.endsWith(".txt")) return "text/plain; charset=utf-8";
    return "application/octet-stream";
}
void Storage::begin() {
    count_=0;webReady_=false;gzipBytes_=0;
    webVersion_[0]=packagedAt_[0]=profile_[0]=indexSha256_[0]=0;
    LittleFSConfig config; config.setAutoFormat(false); LittleFS.setConfig(config);
    mounted_=LittleFS.begin();
    if (!mounted_) { fault_="mount-failed"; return; }
    fault_="manifest-missing";
    File f=LittleFS.open("/dist/manifest.json","r");
    if (!f || !f.size() || f.size()>12288) return;
    JsonDocument doc;
    auto error=deserializeJson(doc,f,DeserializationOption::NestingLimit(5)); f.close();
    fault_="manifest-invalid";
    if (error || doc["schema"]!="afr2-littlefs-manifest-1" || doc["project"]!="AutoFinalRobo2" ||
        doc["rootIndexServedPath"]!="/index.html.gz" || !doc["files"].is<JsonArray>()) return;
    JsonArray list=doc["files"].as<JsonArray>();
    if (!list.size() || list.size()>kMaxAssets) return;
    size_t total=0; bool index=false;
    for (JsonObject entry:list) {
        String route=entry["route"] | "";
        String path=entry["servedPath"] | "";
        String mime=entry["contentType"] | "";
        String hash=entry["sha256"] | "";
        if (!safePath(route) || !safePath(path) || path!=route+".gz" ||
            route=="/ws" || route.startsWith("/api/") || route.startsWith("/config/") ||
            mime!=mimeFor(route) || entry["contentEncoding"]!="gzip" ||
            !entry["gzipBytes"].is<uint32_t>() || hash.length()!=64) return;
        for (size_t i=0;i<hash.length();++i) if (!isxdigit((unsigned char)hash[i])) return;
        for (uint8_t i=0;i<count_;++i) if (assets_[i].route==route) return;
        File assetFile=LittleFS.open("/dist"+path,"r");
        size_t size=entry["gzipBytes"].as<uint32_t>();
        if (!assetFile || !size || assetFile.isDirectory() || assetFile.size()!=size) {
            fault_="asset-missing-or-size"; return;
        }
        assetFile.close();
        assets_[count_++]={route,"/dist"+path,mime,"W/\""+hash+"\"",size};
        total+=size; index|=route=="/index.html";
    }
    if (!index || total!=doc["totalGzipBytes"].as<uint32_t>()) return;
    gzipBytes_=total;
    const char *version=doc["webVersion"]|"",*packaged=doc["packagedAt"]|"",*profile=doc["buildProfile"]|"";
    if(strlen(version)<sizeof(webVersion_))strlcpy(webVersion_,version,sizeof(webVersion_));
    if(strlen(packaged)<sizeof(packagedAt_))strlcpy(packagedAt_,packaged,sizeof(packagedAt_));
    if(!strcmp(profile,"esp") || !strcmp(profile,"full"))strlcpy(profile_,profile,sizeof(profile_));
    for(JsonObject entry:list)if(entry["route"]=="/index.html")strlcpy(indexSha256_,entry["sha256"]|"",sizeof(indexSha256_));
    webReady_=true; fault_="none";
}
const WebAsset *Storage::asset(const String &route) const {
    if (!webReady_) return nullptr;
    for (uint8_t i=0;i<count_;++i) if (assets_[i].route==route) return &assets_[i];
    return nullptr;
}
bool Storage::loadWifi(JsonDocument &doc) {
    if (!mounted_) return false;
    File f=LittleFS.open("/config/wifi.json","r");
    if (!f || !f.size() || f.size()>512) return false;
    return !deserializeJson(doc,f,DeserializationOption::NestingLimit(3)) && (doc["schema"]=="afr2-wifi-1" || doc["schema"]=="afr2-wifi-2");
}
bool Storage::saveWifi(const JsonDocument &doc) {
    if (!mounted_ || !LittleFS.mkdir("/config")) {
        if (!mounted_ || !LittleFS.exists("/config")) return false;
    }
    File f=LittleFS.open("/config/wifi.tmp","w");
    if (!f) return false;
    size_t expected=measureJson(doc), written=serializeJson(doc,f);
    f.flush(); f.close();
    if (!written || written!=expected) { LittleFS.remove("/config/wifi.tmp"); return false; }
    // LittleFS rename replaces atomically; never remove the last good configuration first.
    if (!LittleFS.rename("/config/wifi.tmp","/config/wifi.json")) {
        LittleFS.remove("/config/wifi.tmp"); return false;
    }
    return true;
}
void Storage::health(JsonObject out) const {
    out["mounted"]=mounted_; out["webReady"]=webReady_; out["fault"]=fault_;
    FSInfo info;
    if (mounted_ && LittleFS.info(info)) { out["totalBytes"]=info.totalBytes; out["usedBytes"]=info.usedBytes; }
    else { out["totalBytes"]=nullptr; out["usedBytes"]=nullptr; }
    out["assetIntegrity"]="manifest-and-size-only";
    out["assetCount"]=webReady_?count_:0;
    if(webReady_)out["gzipBytes"]=gzipBytes_;else out["gzipBytes"]=nullptr;
    out["webVersion"]=webReady_ && webVersion_[0]?webVersion_:nullptr;
    out["packagedAt"]=webReady_ && packagedAt_[0]?packagedAt_:nullptr;
    out["buildProfile"]=webReady_ && profile_[0]?profile_:nullptr;
    out["indexSha256"]=webReady_ && indexSha256_[0]?indexSha256_:nullptr;
}
