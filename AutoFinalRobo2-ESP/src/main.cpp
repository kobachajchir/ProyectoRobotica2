#include "afr2_build_info.h"
// AFR2-20261023-11. Arduino ESP8266, LittleFS, HTTP :80 + WebSocket /ws.
// AutoMicro/ESP01 is read-only evidence. No AT, raw upload API or UART logging.
#include <Arduino.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include "storage.h"
#include "wifi_service.h"
#include "mega_link.h"
#include "broker.h"
static Storage storage;
static WifiService wifi(storage);
static MegaLink mega;
static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");
static Broker broker(ws,mega,wifi,storage);
static uint32_t cleanupAt;

static bool acceptsGzip(String header) {
    // Explicit gzip exclusion overrides a wildcard.
    bool wildcard=false, explicitGzip=false, gzip=false;
    header.toLowerCase();
    int start=0;
    while (start<(int)header.length()) {
        int end=header.indexOf(',',start); if (end<0) end=header.length();
        String token=header.substring(start,end); token.trim();
        int semi=token.indexOf(';');
        String coding=semi<0?token:token.substring(0,semi); coding.trim();
        bool permitted=true;
        if (semi>=0) {
            String params=token.substring(semi+1); params.trim();
            int q=params.indexOf("q=");
            if (q>=0) permitted=params.substring(q+2).toFloat()>0;
        }
        if (coding=="gzip") { explicitGzip=true; gzip=permitted; }
        else if (coding=="*") wildcard=permitted;
        start=end+1;
    }
    return explicitGzip?gzip:wildcard;
}
static void staticRequest(AsyncWebServerRequest *request) {
    if (request->method()!=HTTP_GET) { request->send(405,"text/plain","GET required"); return; }
    String route=request->url();
    if (route=="/") route="/index.html";
    // Exact manifest whitelist. /config, traversal, .gz aliases and unknown routes never map to disk.
    const WebAsset *asset=storage.asset(route);
    if (!asset) {
        if (route=="/index.html" && !storage.webReady()) {
            request->send(503,"text/html; charset=utf-8",
                "<!doctype html><meta charset='utf-8'><title>AutoFinalRobo2</title>"
                "<h1>AutoFinalRobo2 ESP</h1><p>Paquete Web no disponible en LittleFS.</p>"
                "<p>Diagnostico: <a href='/diagnostico'>estado del servicio</a>.</p>");
        } else request->send(404,"text/plain","Not found");
        return;
    }
    if (!acceptsGzip(request->header("Accept-Encoding"))) {
        request->send(406,"text/plain","gzip representation required"); return;
    }
    bool index=route=="/index.html";
    if (!index && request->header("If-None-Match")==asset->etag) {
        auto *response=request->beginResponse(304);
        response->addHeader("ETag",asset->etag); response->addHeader("Vary","Accept-Encoding");
        response->addHeader("Cache-Control","no-cache"); request->send(response); return;
    }
    auto *response=request->beginResponse(LittleFS,asset->path,asset->mime);
    if (!response) { request->send(503,"text/plain","Asset unavailable"); return; }
    response->addHeader("Content-Encoding","gzip"); response->addHeader("Vary","Accept-Encoding");
    response->addHeader("Cache-Control",index?"no-store":"no-cache");
    response->addHeader("ETag",asset->etag); response->addHeader("X-Content-Type-Options","nosniff");
    request->send(response);
    if(index)broker.networkEvent(AFR2_NET_WEB_SERVED); // HTTP response queued, not a browser-render acknowledgement.
}
void setup() {
    mega.begin(); storage.begin(); wifi.begin(); broker.begin(); server.addHandler(&ws);
    server.on("/diagnostico",HTTP_GET,[](AsyncWebServerRequest *request){
        JsonDocument doc;storage.health(doc["filesystem"].to<JsonObject>());wifi.status(doc["wifi"].to<JsonObject>());
        doc["brokerVersion"]=AFR2_FIRMWARE_VERSION;doc["bootId"]=mega.ownBootId();
        doc["resetReason"]=ESP.getResetInfoPtr()?ESP.getResetInfoPtr()->reason:255;
        String json;serializeJson(doc,json);json.replace("&","&amp;");json.replace("<","&lt;");json.replace(">","&gt;");
        auto *response=request->beginResponse(200,"text/html; charset=utf-8",String("<!doctype html><meta charset='utf-8'><meta name='viewport' content='width=device-width,initial-scale=1'><style>:root{color-scheme:light dark}body{font:16px system-ui;max-width:52rem;margin:24px auto;padding:0 16px}pre{white-space:pre-wrap;overflow-wrap:anywhere}</style><title>Diagnostico AFR2</title><h1>AutoFinalRobo2</h1><p>Servidor HTTP accesible. Estado de red y paquete:</p><pre>")+json+"</pre><a href='/'>Abrir interfaz</a>");
        response->addHeader("Cache-Control","no-store");request->send(response);
    });
    server.on("/api/health",HTTP_GET,[](AsyncWebServerRequest *request) {
        JsonDocument doc; doc["product"]="AutoFinalRobo2"; doc["brokerVersion"]=AFR2_FIRMWARE_VERSION;
        doc["megaLink"]=mega.state(); doc["uptimeMs"]=millis();
        storage.health(doc["filesystem"].to<JsonObject>());wifi.status(doc["wifi"].to<JsonObject>());
        doc["bootId"]=mega.ownBootId();doc["resetReason"]=ESP.getResetInfoPtr()?ESP.getResetInfoPtr()->reason:255;
        String body; serializeJson(doc,body);
        auto *response=request->beginResponse(200,"application/json; charset=utf-8",body);
        response->addHeader("Cache-Control","no-store"); request->send(response);
    });
    // AFR2-20261102-04: public asset metadata only. Never expose /config or arbitrary paths.
    server.on("/api/web-manifest",HTTP_GET,[](AsyncWebServerRequest *request) {
        if (!storage.webReady()) { request->send(503,"application/json","{\"error\":\"Web package unavailable\"}"); return; }
        auto *response=request->beginResponse(LittleFS,"/dist/manifest.json","application/json; charset=utf-8");
        if (!response) { request->send(503,"application/json","{\"error\":\"Manifest unavailable\"}"); return; }
        response->addHeader("Cache-Control","no-store");
        response->addHeader("X-Content-Type-Options","nosniff");
        response->addHeader("X-AFR2-Package-Source","esp-littlefs");
        request->send(response);
    });
    server.onNotFound(staticRequest); server.begin();broker.networkServerReady();
}
void loop() {
    uint32_t now=millis();
    mega.tick(now); broker.tick(now); wifi.tick(now);
    if ((uint32_t)(now-cleanupAt)>=1000u) { cleanupAt=now; ws.cleanupClients(); }
    yield();
}
