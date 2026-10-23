#include "webserver_ws.h"
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESP8266WiFi.h>
#include <LittleFS.h>
#include <FS.h>
#include <pgmspace.h>
#include "webserver_ports.h"
#include "ws_limits.h"

// ===== Paths en LittleFS =====
static const char *INDEX_HTM = "/dist/index.html";
static const char *INDEX_GZ = "/dist/index.html.gz";
// Final Web model copied into LittleFS data/dist/models (AMI-20261002-05/06).
static const char *GLB_PATH = "/dist/models/auto_micro.glb";
static const char *DRACO_WRAP = "/dist/draco/draco_wasm_wrapper.js";
static const char *DRACO_JS = "/dist/draco/draco_decoder.js";
static const char *DRACO_WASM = "/dist/draco/draco_decoder.wasm";

// ===== Servidor / WS =====
static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

// ===== Contadores / concesion de la interfaz =====
static const uint8_t WS_CLIENT_TRACK_MAX = WS_MAX_SESSIONS;

struct WebServerContext {
    uint16_t wsClients = 0;
    void *unerApp = nullptr;
    WebServerServices services{};
    uint32_t wsClientIds[WS_CLIENT_TRACK_MAX]{};
    uint8_t wsClientIps[WS_CLIENT_TRACK_MAX][4]{};
    uint8_t wsClientUsed[WS_CLIENT_TRACK_MAX]{};
    uint8_t wsPageReady[WS_CLIENT_TRACK_MAX]{};
    WiFiEventHandler apStaConnHandler{};
    WiFiEventHandler apStaDisconnHandler{};
};
static WebServerContext s_http;

static const WebServerServices &services() {
    return s_http.services.storage_ready ? s_http.services : WebServer_DefaultServices();
}
void WebServerWS_SetServices(const WebServerServices &value) {
    s_http.services = value;
    s_http.unerApp = value.context;
}

// ===== Utils =====
static inline bool fsExists(const char *p) { return LittleFS.exists(p); }

static void addNoCacheHeaders(AsyncWebServerResponse *r)
{
    if (r == nullptr)
        return;

    r->addHeader("Cache-Control", "no-cache, no-store, must-revalidate");
    r->addHeader("Pragma", "no-cache");
    r->addHeader("Expires", "0");
}

static void addImmutableCacheHeaders(AsyncWebServerResponse *r)
{
    if (r == nullptr)
        return;

    r->addHeader("Cache-Control", "public, max-age=31536000, immutable");
}

static AsyncWebServerResponse *makeGzipOnlyResponse(AsyncWebServerRequest *req,
                                                    const char *path,
                                                    const char *contentType,
                                                    bool noCache = false)
{
    if (req == nullptr || path == nullptr || contentType == nullptr)
    {
        return nullptr;
    }

    String gz = String(path) + ".gz";

    if (!LittleFS.exists(gz))
    {
        return req->beginResponse(500, "text/plain", "Missing required gzip file");
    }

    auto *r = req->beginResponse(LittleFS, gz, contentType);
    if (r == nullptr)
    {
        return req->beginResponse(500, "text/plain", "Failed to open gzip file");
    }

    r->addHeader("Content-Encoding", "gzip");

    if (noCache)
    {
        addNoCacheHeaders(r);
    }
    else
    {
        addImmutableCacheHeaders(r);
    }

    return r;
}

static AsyncWebServerResponse *makeFileResponse(AsyncWebServerRequest *req,
                                                const char *path,
                                                const char *contentType,
                                                bool allowGzip = true,
                                                bool noCache = false)
{
    if (req == nullptr || path == nullptr || contentType == nullptr)
    {
        return nullptr;
    }

    if (allowGzip)
    {
        String gz = String(path) + ".gz";
        if (LittleFS.exists(gz))
        {
            auto *r = req->beginResponse(LittleFS, gz, contentType);
            if (r == nullptr)
            {
                return req->beginResponse(500, "text/plain", "Failed to open gzip file");
            }

            r->addHeader("Content-Encoding", "gzip");

            if (noCache)
            {
                addNoCacheHeaders(r);
            }
            else
            {
                addImmutableCacheHeaders(r);
            }

            return r;
        }
    }

    if (!LittleFS.exists(path))
    {
        return req->beginResponse(404, "text/plain", "Not found");
    }

    auto *r = req->beginResponse(LittleFS, path, contentType);
    if (r == nullptr)
    {
        return req->beginResponse(500, "text/plain", "Failed to open file");
    }

    if (noCache)
    {
        addNoCacheHeaders(r);
    }
    else
    {
        addImmutableCacheHeaders(r);
    }

    return r;
}

static void ws_track_add(uint32_t id, IPAddress ip)
{
    for (uint8_t i = 0; i < WS_CLIENT_TRACK_MAX; ++i)
    {
        if (!s_http.wsClientUsed[i])
        {
            s_http.wsClientUsed[i] = 1;
            s_http.wsPageReady[i] = 0;
            s_http.wsClientIds[i] = id;
            s_http.wsClientIps[i][0] = ip[0];
            s_http.wsClientIps[i][1] = ip[1];
            s_http.wsClientIps[i][2] = ip[2];
            s_http.wsClientIps[i][3] = ip[3];
            return;
        }
    }
}

static bool ws_track_remove(uint32_t id)
{
    for (uint8_t i = 0; i < WS_CLIENT_TRACK_MAX; ++i)
    {
        if (s_http.wsClientUsed[i] && s_http.wsClientIds[i] == id)
        {
            if (s_http.wsPageReady[i] && s_http.unerApp && services().notify_page_closed)
                services().notify_page_closed(s_http.unerApp, id, (uint8_t)(i + 1u));
            s_http.wsPageReady[i] = 0;
            s_http.wsClientUsed[i] = 0;
            return true;
        }
    }
    return false;
}

uint8_t WebServerWS_MarkPageReady(uint32_t client_id)
{
    for (uint8_t i = 0; i < WS_CLIENT_TRACK_MAX; ++i)
    {
        if (!s_http.wsClientUsed[i] || s_http.wsClientIds[i] != client_id) continue;
        if (!s_http.wsPageReady[i])
        {
            s_http.wsPageReady[i] = 1;
            if (s_http.unerApp && services().notify_page_ready)
                services().notify_page_ready(s_http.unerApp, client_id, (uint8_t)(i + 1u));
        }
        return (uint8_t)(i + 1u);
    }
    return 0u;
}

static void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                      AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
    {
        if (s_http.wsClients >= WS_MAX_SESSIONS)
        {
            client->text("{\"api\":1,\"type\":\"error\",\"code\":\"busy\",\"message\":\"Se alcanzo el maximo de dos sesiones WebSocket\"}");
            client->close();
            break;
        }
        //         Serial.printf("[WS] Cliente #%u CONECTADO desde %s\n",
        //                       client->id(), client->remoteIP().toString().c_str());
        // The upgrade request owns the query string only during CONNECT.
        AsyncWebServerRequest *upgrade = static_cast<AsyncWebServerRequest *>(arg);
        String resumeToken;
        if (upgrade && upgrade->hasParam("resumeToken"))
            resumeToken = upgrade->getParam("resumeToken")->value();
        if (!services().bridge_connect(client, resumeToken.c_str()))
        {
            break;
        }
        s_http.wsClients++;
        ws_track_add(client->id(), client->remoteIP());
        if (s_http.unerApp) services().notify_connected(s_http.unerApp, client->id());
        break;
    }

    case WS_EVT_DISCONNECT:
        if (!ws_track_remove(client->id())) break;
        if (s_http.wsClients > 0) s_http.wsClients--;
        //         Serial.printf("[WS] Cliente #%u DESCONECTADO\n", client->id());
        if (services().bridge_disconnect(client->id()) && s_http.unerApp)
            services().notify_disconnected(s_http.unerApp, client->id());
        break;

    case WS_EVT_DATA:
    {
        AwsFrameInfo *info = (AwsFrameInfo *)arg;

        if (info->message_opcode == WS_BINARY)
        {
            client->text("{\"api\":1,\"type\":\"error\",\"code\":\"invalid_request\",\"message\":\"Only WS_TEXT JSON is accepted on /ws\",\"details\":{}}");
        }
        else if (info->message_opcode == WS_TEXT)
        {
            services().bridge_data(client, info, data, len);
        }
    }
    break;

    case WS_EVT_PONG:
    case WS_EVT_ERROR:
    default:
        break;
    }
}

// Fallback en PROGMEM si falta index.html.gz
static const char kFallback[] PROGMEM =
    "<!doctype html><html><head><meta charset='utf-8'/>"
    "<title>ESP-01S</title></head><body>"
    "<h1>ESP-01S</h1><p>LittleFS sin /dist/index.html.gz</p>"
    "<p>Subí el FS con: <code>pio run -t uploadfs</code></p>"
    "</body></html>";

void WebServerWS_Init()
{
    // El montaje pertenece al servicio de persistencia (nvs_utils, JSON sobre
    // LittleFS). HTTP solo consume el estado ya inicializado y no mantiene una
    // segunda autoridad de montaje.
    if (services().storage_ready())
    {
        FSInfo info;
        if (LittleFS.info(info))
        {
            //             Serial.printf("[FS] total=%u used=%u\n", info.totalBytes, info.usedBytes);
        }
    }

    // WS
    services().bridge_initialize(s_http.unerApp, &ws);
    ws.addMiddleware([](AsyncWebServerRequest *request, ArMiddlewareNext next)
                     {
      const String &origin = request->header("Origin");
      if (origin.length() == 0u || origin == (String("http://") + request->host())) {
        next();
        return;
      }
      request->send(403, "text/plain", "WebSocket origin rejected"); });
    ws.onEvent(onWSEvent);
    server.addHandler(&ws);

    // ===== Rutas explícitas =====

    // 1) Raíz "/" -> index.html.gz obligatorio
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *req)
              {
    if (fsExists(INDEX_GZ)) {
      auto* r = makeGzipOnlyResponse(req, INDEX_HTM, "text/html", true);
      req->send(r);
    } else {
      req->send_P(500, "text/html", kFallback);
    } });

    // 2) GLB
    server.on("/models/auto_micro.glb", HTTP_GET, [](AsyncWebServerRequest *req)
              {
    if (!fsExists(GLB_PATH)) { req->send(404, "text/plain", "Not found"); return; }
    // This stable URL now serves the final model. Do not retain an older GLB
    // for a year after replacing its LittleFS file.
    auto* r = makeFileResponse(req, GLB_PATH, "model/gltf-binary", false, true);
    req->send(r); });

    // 3) Draco wrapper JS
    server.on("/draco/draco_wasm_wrapper.js", HTTP_GET, [](AsyncWebServerRequest *req)
              {
    if (!fsExists(DRACO_WRAP) && !fsExists("/dist/draco/draco_wasm_wrapper.js.gz")) {
      req->send(404, "text/plain", "Not found");
      return;
    }
    auto* r = makeFileResponse(req, DRACO_WRAP, "application/javascript", true, false);
    req->send(r); });

    // 4) Draco decoder JS
    server.on("/draco/draco_decoder.js", HTTP_GET, [](AsyncWebServerRequest *req)
              {
    if (!fsExists(DRACO_JS) && !fsExists("/dist/draco/draco_decoder.js.gz")) {
      req->send(404, "text/plain", "Not found");
      return;
    }
    auto* r = makeFileResponse(req, DRACO_JS, "application/javascript", true, false);
    req->send(r); });

    // 5) Draco decoder WASM. En LittleFS se prioriza la variante gzip para
    // reducir el tiempo de radio ocupado y la presion de buffers TCP.
    server.on("/draco/draco_decoder.wasm", HTTP_GET, [](AsyncWebServerRequest *req)
              {
    if (!fsExists(DRACO_WASM) && !fsExists("/dist/draco/draco_decoder.wasm.gz")) {
      req->send(404, "text/plain", "Not found");
      return;
    }
    auto* r = makeFileResponse(req, DRACO_WASM, "application/wasm", true, false);
    req->send(r); });

    // NotFound → SPA fallback solo si no hay extensión, y existe index.gz
    server.onNotFound([](AsyncWebServerRequest *req)
                      {
    String url = req->url();
    IPAddress ip = req->client()->remoteIP();
    bool spaFallback = (url.indexOf('.') < 0 && fsExists(INDEX_GZ));
//     Serial.printf(spaFallback ? "[HTTP] SPA fallback %s desde %s\n" : "[HTTP] 404 real %s desde %s\n", url.c_str(), ip.toString().c_str());

    if (spaFallback) {
      auto* r = makeGzipOnlyResponse(req, INDEX_HTM, "text/html", true);
      req->send(r);
    } else {
      req->send(404, "text/plain", "Not found");
    } });

    s_http.apStaConnHandler = WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected &evt)
                                                           {
      uint8_t ap_clients = WiFi.softAPgetStationNum();
//       Serial.printf("[AP] STA JOIN mac=%02X:%02X:%02X:%02X:%02X:%02X aid=%u total=%u\n",
//                     evt.mac[0], evt.mac[1], evt.mac[2], evt.mac[3], evt.mac[4], evt.mac[5], evt.aid, ap_clients);
      if (s_http.unerApp) services().notify_ap_join(s_http.unerApp, ap_clients, evt.aid); });

    s_http.apStaDisconnHandler = WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected &evt)
                                                                 {
      uint8_t ap_clients = WiFi.softAPgetStationNum();
//       Serial.printf("[AP] STA LEAVE mac=%02X:%02X:%02X:%02X:%02X:%02X aid=%u total=%u\n",
//                     evt.mac[0], evt.mac[1], evt.mac[2], evt.mac[3], evt.mac[4], evt.mac[5], evt.aid, ap_clients);
      if (s_http.unerApp) services().notify_ap_leave(s_http.unerApp, ap_clients, evt.aid); });

    server.begin();
    //     Serial.println("[HTTP] Server iniciado en :80");
    if (s_http.unerApp)
        services().notify_server_up(s_http.unerApp);
}

void WebServerWS_Loop()
{
    services().bridge_loop();
    // La capacidad se decide en services().bridge_connect(). cleanupClients()
    // usa por defecto cuatro clientes y, si ve un quinto aunque este ya haya
    // sido rechazado/cerrado, cierra primero el WebSocket mas antiguo. Eso
    // expulsaba una sesion autenticada al abrir otro dispositivo. Un limite
    // alto conserva aqui solamente la recoleccion de sockets ya cerrados; el
    // bridge sigue rechazando de forma explicita solo al cliente nuevo.
    ws.cleanupClients(0xFFFFu);
}

uint8_t WebServerWS_GetConnectedCount() { return (uint8_t)s_http.wsClients; }

bool WebServerWS_GetClientInfoByIndex(uint8_t index, uint32_t *id, uint8_t ip_out[4])
{
    uint8_t seen = 0;
    for (uint8_t i = 0; i < WS_CLIENT_TRACK_MAX; ++i)
    {
        if (!s_http.wsClientUsed[i])
            continue;

        if (seen == index)
        {
            if (id)
                *id = s_http.wsClientIds[i];

            if (ip_out)
            {
                ip_out[0] = s_http.wsClientIps[i][0];
                ip_out[1] = s_http.wsClientIps[i][1];
                ip_out[2] = s_http.wsClientIps[i][2];
                ip_out[3] = s_http.wsClientIps[i][3];
            }

            return true;
        }
        seen++;
    }
    return false;
}
