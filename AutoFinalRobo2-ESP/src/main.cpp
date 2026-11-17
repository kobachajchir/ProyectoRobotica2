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
#include "http_assets.h"
static Storage storage;
static WifiService wifi(storage);
static MegaLink mega;
static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");
static Broker broker(ws,mega,wifi,storage);
static uint32_t cleanupAt;

// Immutable pages stay in flash; runtime diagnostics come from the small JSON endpoint.
static const char diagnosticPage[] PROGMEM = R"HTML(<!doctype html><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><style>:root{color-scheme:light dark}body{font:16px system-ui;max-width:52rem;margin:24px auto;padding:0 16px}pre{white-space:pre-wrap;overflow-wrap:anywhere}</style><title>Diagnostico AFR2</title><h1>AutoFinalRobo2</h1><p>Servidor HTTP accesible. Estado de red y paquete:</p><pre id="status">Consultando...</pre><a href="/">Abrir interfaz</a><script>fetch('/api/health',{cache:'no-store'}).then(r=>{if(!r.ok)throw Error('HTTP '+r.status);return r.json()}).then(d=>document.getElementById('status').textContent=JSON.stringify(d,null,2)).catch(e=>document.getElementById('status').textContent='No se pudo leer el estado: '+e.message)</script><noscript>Consultar /api/health para ver el estado sin JavaScript.</noscript>)HTML";
static const char unavailablePage[] PROGMEM = "<!doctype html><meta charset='utf-8'><title>AutoFinalRobo2</title>"
    "<h1>AutoFinalRobo2 ESP</h1><p>Paquete Web no disponible en LittleFS.</p>"
    "<p>Diagnostico: <a href='/diagnostico'>estado del servicio</a>.</p>";
static const char busyPage[] PROGMEM = R"HTML(<!doctype html><html lang="es"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>AutoFinalRobo2 — Auto ocupado</title><style>:root{color-scheme:dark light;--bg:#0d131a;--card:#151f2b;--border:#29384b;--fg:#eef4fb;--muted:#9fb0c3;--accent:#3b82f6;--warn:#f59e0b}@media(prefers-color-scheme:light){:root{--bg:#f4f7fb;--card:#ffffff;--border:#d4deea;--fg:#101824;--muted:#526277}}*{box-sizing:border-box}body{margin:0;min-height:100vh;display:grid;place-items:center;padding:20px;font:15px/1.5 system-ui,-apple-system,sans-serif;background:var(--bg);color:var(--fg)}.card{width:min(34rem,100%);background:var(--card);border:1px solid var(--border);border-radius:14px;padding:24px;box-shadow:0 12px 32px rgba(0,0,0,.22)}.badge{display:inline-flex;align-items:center;gap:8px;padding:4px 10px;border-radius:999px;font-size:12px;font-weight:600;background:rgba(245,158,11,.16);color:var(--warn);border:1px solid rgba(245,158,11,.35)}.badge.free{background:rgba(16,185,129,.16);color:#10b981;border-color:rgba(16,185,129,.35)}h1{margin:12px 0 6px;font-size:20px}p{margin:0 0 16px;color:var(--muted)}dl{display:grid;grid-template-columns:1fr 1fr;gap:10px;margin:0 0 18px;padding:12px;border-radius:10px;border:1px solid var(--border)}dt{font-size:12px;color:var(--muted)}dd{margin:2px 0 0;font-weight:600}.actions{display:flex;gap:10px;flex-wrap:wrap}a.btn,button.btn{padding:9px 14px;border-radius:8px;border:1px solid var(--border);background:var(--accent);color:#fff;font-weight:600;text-decoration:none;cursor:pointer}button.secondary{background:transparent;color:var(--fg)}</style></head><body><main class="card"><span id="badge" class="badge">Cliente Web activo en uso</span><h1>Auto ocupado por otro cliente</h1><p id="desc">Esta versión embebida admite hasta 4 dispositivos en el Wi-Fi, pero atiende datos de aplicación a 1 único cliente Web por vez. En cuanto el cliente actual se desconecte, podrás ingresar automáticamente.</p><dl><div><dt>Sesión Web activa</dt><dd id="ws">1 / 1</dd></div><div><dt>Clientes Wi-Fi AP</dt><dd id="ap">— / 4</dd></div><div><dt>Modo del auto</dt><dd id="mode">—</dd></div><div><dt>Movimiento</dt><dd id="stopped">—</dd></div></dl><div class="actions"><a class="btn" href="/">Intentar conectar ahora</a><button class="btn secondary" type="button" onclick="poll()">Actualizar estado</button></div></main><script>function poll(){fetch('/api/busy',{cache:'no-store'}).then(r=>r.json()).then(d=>{document.getElementById('ws').textContent=(d.wsClients??(d.busy?1:0))+' / '+(d.maxWebClients??1);document.getElementById('ap').textContent=(d.apClients??0)+' / '+(d.maxWifiClients??4);document.getElementById('mode').textContent=d.robotMode||'—';document.getElementById('stopped').textContent=d.robotStopped===false?'En movimiento':'Detenido';if(!d.busyForRequester&&!d.busy){const b=document.getElementById('badge');b.className='badge free';b.textContent='Auto disponible — redirigiendo...';location.replace('/');}}).catch(()=>{})}poll();setInterval(poll,2000);</script></body></html>)HTML";

static bool redirectIfBusy(AsyncWebServerRequest *request) {
    if (request->client() && broker.isBusyFor(request->client()->remoteIP())) {
        request->redirect("/busy");
        return true;
    }
    return false;
}

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
    if (redirectIfBusy(request)) return;
    String route=request->url();
    if (route=="/") route="/index.html";
    // Exact manifest whitelist. /config, traversal, .gz aliases and unknown routes never map to disk.
    const WebAsset *asset=storage.asset(route);
    if (!asset) {
        if (route=="/index.html" && !storage.webReady()) {
            request->send(request->beginResponse_P(503,"text/html; charset=utf-8",unavailablePage));
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
    serveAsset(request,*asset);
    if(index)broker.networkEvent(AFR2_NET_WEB_SERVED); // HTTP response queued, not a browser-render acknowledgement.
}
void setup() {
    mega.begin(); storage.begin(); wifi.begin(); broker.begin(); server.addHandler(&ws);
    server.on("/busy",HTTP_GET,[](AsyncWebServerRequest *request){
        auto *response=request->beginResponse_P(200,"text/html; charset=utf-8",busyPage);
        response->addHeader("Cache-Control","no-store");request->send(response);
    });
    server.on("/api/busy",HTTP_GET,[](AsyncWebServerRequest *request){
        JsonDocument doc;
        IPAddress ip=request->client()?request->client()->remoteIP():IPAddress(0,0,0,0);
        broker.busyStatus(doc.to<JsonObject>(),ip);
        String body; serializeJson(doc,body); doc.clear();
        auto *response=request->beginResponse(200,"application/json; charset=utf-8",body);
        response->addHeader("Cache-Control","no-store"); request->send(response);
    });
    server.on("/diagnostico",HTTP_GET,[](AsyncWebServerRequest *request){
        if (redirectIfBusy(request)) return;
        auto *response=request->beginResponse_P(200,"text/html; charset=utf-8",diagnosticPage);
        response->addHeader("Cache-Control","no-store");request->send(response);
    });
    server.on("/api/health",HTTP_GET,[](AsyncWebServerRequest *request) {
        if (redirectIfBusy(request)) return;
        JsonDocument doc; doc["product"]="AutoFinalRobo2"; doc["brokerVersion"]=AFR2_FIRMWARE_VERSION;
        doc["megaLink"]=mega.state(); doc["uptimeMs"]=millis();
        storage.health(doc["filesystem"].to<JsonObject>());wifi.status(doc["wifi"].to<JsonObject>());
        doc["bootId"]=mega.ownBootId();doc["resetReason"]=ESP.getResetInfoPtr()?ESP.getResetInfoPtr()->reason:255;
        doc["freeHeapBytes"]=ESP.getFreeHeap();doc["maxFreeBlockBytes"]=ESP.getMaxFreeBlockSize();
        assetStatus(doc["httpAssets"].to<JsonObject>());
        String body; serializeJson(doc,body);doc.clear();
        auto *response=request->beginResponse(200,"application/json; charset=utf-8",body);
        response->addHeader("Cache-Control","no-store"); request->send(response);
    });
    // AFR2-20261102-04: public asset metadata only. Never expose /config or arbitrary paths.
    server.on("/api/web-manifest",HTTP_GET,[](AsyncWebServerRequest *request) {
        if (redirectIfBusy(request)) return;
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
    mega.tick(now); broker.tick(now); wifi.tick(now);tickAssets(now);
    if ((uint32_t)(now-cleanupAt)>=1000u) { cleanupAt=now; ws.cleanupClients(); }
    yield();
}
