// AFR2-20261023-11. LittleFS owner; never formats or exposes /config over HTTP.
#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
struct WebAsset {
    String route, path, mime, etag;
    size_t bytes=0;
};
class Storage {
public:
    void begin();
    bool mounted() const { return mounted_; }
    bool webReady() const { return webReady_; }
    const char *fault() const { return fault_; }
    const WebAsset *asset(const String &route) const;
    bool loadWifi(JsonDocument &doc);
    bool hasWifiConfig() const;
    bool saveWifi(const JsonDocument &doc);
    void health(JsonObject out) const;
private:
    bool mounted_=false, webReady_=false;
    const char *fault_="not-initialized";
    static constexpr uint8_t kMaxAssets=24;
    static constexpr size_t kMaxWifiConfigBytes=1024;
    WebAsset assets_[kMaxAssets];
    uint8_t count_=0;
    // Manifest identity only; this is not a runtime checksum of the asset bytes.
    uint32_t gzipBytes_=0;
    char webVersion_[33]={},packagedAt_[25]={},profile_[5]={},indexSha256_[65]={};
};
