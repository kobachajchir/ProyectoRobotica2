# Servidor HTTP y conexiones WebSocket

ID: `esp:module/src/webserver_ws.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Sirve assets LittleFS y mantiene conexiones /ws del navegador.

HTTP, archivos comprimidos, mensajes WS_TEXT, clientes y eventos de socket; el bridge interpreta la API.

<a id="modularity"></a>
## Modularidad observada

Dependiente de ESPAsyncWebServer/AsyncTCP y filesystem; la frontera de protocolo queda en WSJsonBridge.

<a id="state"></a>
## Handle, estado y atributos externos

Servidor/socket y lista acotada de conexiones; los buffers async deben copiarse o conservarse según el API real.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- SetUNERApp enlaza aplicación antes de Init.
- Al conectar asigna/solicita sesión; los mensajes de texto JSON pasan al bridge.
- Al desconectar se limpia cliente/autoridad/requests; loop atiende mantenimiento.

<a id="interactions"></a>
## Interacciones

- Entrega bytes JSON al bridge y sirve el bundle Web producido por otro repositorio.
- HTTP y WS tienen límites de memoria/buffers distintos.

<a id="limitations"></a>
## Precauciones y límites

- Máximo dos sesiones según WS_MAX_SESSIONS actual.
- No confundir bundle copiado, firmware cargado y comportamiento verificado en navegador.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/webserver_ws.cpp` | c-cpp-structural | `19f51be9df1a37d5060783d687220fd6b40dde0226fa6eebfb190a57e6d08c20` |
| `src/webserver_ws.h` | c-cpp-structural | `19e6899c017e7c6ba6ee1b9b500616d3efed392531b5b23b0370d11c6924080d` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WebServerContext`

`esp:src/webserver_ws.cpp:27`

| Atributo / firma | Propósito |
| --- | --- |
| `uint16_t wsClients` | Dato de estado/configuración `wsClients`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *unerApp` | Dato de estado/configuración `unerApp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WebServerServices services{ estructura anidada }` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `uint32_t wsClientIds[WS_CLIENT_TRACK_MAX]{ estructura anidada }` | Dato de estado/configuración `wsClientIds`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wsClientIps[WS_CLIENT_TRACK_MAX][4]{ estructura anidada }` | Dato de estado/configuración `wsClientIps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wsClientUsed[WS_CLIENT_TRACK_MAX]{ estructura anidada }` | Dato de estado/configuración `wsClientUsed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wsPageReady[WS_CLIENT_TRACK_MAX]{ estructura anidada }` | Dato de estado/configuración `wsPageReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WiFiEventHandler apStaConnHandler{ estructura anidada }` | Dato de estado/configuración `apStaConnHandler`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WiFiEventHandler apStaDisconnHandler{ estructura anidada }` | Dato de estado/configuración `apStaDisconnHandler`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-webserverws_getclientinfobyindex"></a>`bool WebServerWS_GetClientInfoByIndex(uint8_t index, uint32_t *id, uint8_t ip_out[4])` | Consulta Client Info By Index dentro de Servidor HTTP y conexiones WebSocket. | `esp:src/webserver_ws.cpp:412`; inferred-name |
| <a id="fn-webserverws_getconnectedcount"></a>`uint8_t WebServerWS_GetConnectedCount()` | Consulta Connected Count dentro de Servidor HTTP y conexiones WebSocket. | `esp:src/webserver_ws.cpp:410`; inferred-name |
| <a id="fn-webserverws_init"></a>`void WebServerWS_Init()` | Configura rutas HTTP/WS y pone en marcha el servidor. | `esp:src/webserver_ws.cpp:284`; editorial |
| <a id="fn-webserverws_loop"></a>`void WebServerWS_Loop()` | Atiende mantenimiento de conexiones y bridge. | `esp:src/webserver_ws.cpp:398`; editorial |
| <a id="fn-webserverws_markpageready"></a>`uint8_t WebServerWS_MarkPageReady(uint32_t client_id)` | Coordina `notify_page_ready`, `services` para Servidor HTTP y conexiones WebSocket. | `esp:src/webserver_ws.cpp:200`; structural |
| <a id="fn-webserverws_setservices"></a>`void WebServerWS_SetServices(const WebServerServices &value)` | Configura Services dentro de Servidor HTTP y conexiones WebSocket. | `esp:src/webserver_ws.cpp:43`; inferred-name |
| <a id="fn-webserverws_setunerapp"></a>`void WebServerWS_SetUNERApp(void* app_ptr)` | Presta al servidor la instancia UNER que usará el bridge. | `esp:src/webserver_ws.h:5`; editorial |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-addimmutablecacheheaders"></a>`static void addImmutableCacheHeaders(AsyncWebServerResponse *r)` | Adapta la llamada a `addHeader` con el contexto de esta integración. | `esp:src/webserver_ws.cpp:61`; structural |
| <a id="fn-addnocacheheaders"></a>`static void addNoCacheHeaders(AsyncWebServerResponse *r)` | Adapta la llamada a `addHeader` con el contexto de esta integración. | `esp:src/webserver_ws.cpp:51`; structural |
| <a id="fn-fsexists"></a>`static inline bool fsExists(const char *p)` | Adapta la llamada a `exists` con el contexto de esta integración. | `esp:src/webserver_ws.cpp:49`; structural |
| <a id="fn-makefileresponse"></a>`static AsyncWebServerResponse *makeFileResponse(AsyncWebServerRequest *req, const char *path, const char *contentType, bool allowGzip, bool noCache)` | Coordina `String`, `addHeader`, `addImmutableCacheHeaders` para Servidor HTTP y conexiones WebSocket. | `esp:src/webserver_ws.cpp:106`; structural |
| <a id="fn-makegziponlyresponse"></a>`static AsyncWebServerResponse *makeGzipOnlyResponse(AsyncWebServerRequest *req, const char *path, const char *contentType, bool noCache)` | Coordina `String`, `addHeader`, `addImmutableCacheHeaders` para Servidor HTTP y conexiones WebSocket. | `esp:src/webserver_ws.cpp:69`; structural |
| <a id="fn-onwsevent"></a>`static void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)` | Coordina `bridge_connect`, `bridge_data`, `bridge_disconnect` para Servidor HTTP y conexiones WebSocket. | `esp:src/webserver_ws.cpp:216`; structural |
| <a id="fn-server"></a>`static AsyncWebServer server(80)` | Operación server de Servidor HTTP y conexiones WebSocket; semántica fina pendiente de ampliación. | `esp:src/webserver_ws.cpp:21`; inferred-name |
| <a id="fn-services"></a>`static const WebServerServices &services()` | Adapta la llamada a `WebServer_DefaultServices` con el contexto de esta integración. | `esp:src/webserver_ws.cpp:40`; structural |
| <a id="fn-ws"></a>`static AsyncWebSocket ws( )` | Operación ws de Servidor HTTP y conexiones WebSocket; semántica fina pendiente de ampliación. | `esp:src/webserver_ws.cpp:22`; inferred-name |
| <a id="fn-ws_track_add"></a>`static void ws_track_add(uint32_t id, IPAddress ip)` | Operación ws track add de Servidor HTTP y conexiones WebSocket; semántica fina pendiente de ampliación. | `esp:src/webserver_ws.cpp:166`; inferred-name |
| <a id="fn-ws_track_remove"></a>`static bool ws_track_remove(uint32_t id)` | Coordina `notify_page_closed`, `services` para Servidor HTTP y conexiones WebSocket. | `esp:src/webserver_ws.cpp:184`; structural |
