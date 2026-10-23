# Bridge JSON, sesiones y traducción de pantallas

ID: `esp:module/src/ws_json_bridge.h` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Despacha API JSON, autoriza por sesión, codifica solicitudes y traduce respuestas/eventos F4 a Web.

Una unidad C++ incluye partes .inc ordenadas: common, validation, canvas, requests, comandos ESP/STM, debug, dispatch, reports, services y lifecycle.

<a id="modularity"></a>
## Modularidad observada

Operaciones externas por puertos, con ArduinoJson, sockets y modelos concretos; capa de integración, no núcleo neutral.

<a id="state"></a>
## Handle, estado y atributos externos

Contexto privado s_bridge posee clientes, sesiones, broker UART, streams, credenciales temporales, canvas y refrescos. Los .inc comparten linkage estático; no son doce librerías independientes.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Init enlaza app/backend y prepara estados.
- Un request valida sobre y forma, comprueba permiso y encola o ejecuta localmente.
- requestAccepted es intermedio; response/error final respeta client/generation/requestId/command.
- add_screen_report traduce identidad, selección, notificaciones y extensión dashboard; no publica un snapshot general de renderers.

<a id="interactions"></a>
## Interacciones

- wsm autoriza, UTM correlaciona, StreamBroker reconcilia y decoders convierten unidades.
- F4 envía screenCode/source y datos específicos; ScreenContext y resolver Web completan su modelo local.
- PIN JSON string de cuatro dígitos se codifica como cuatro valores numéricos 0..9 en wire; ver shared:message/auth-pin/v1 para scopes y respuestas.

<a id="limitations"></a>
## Precauciones y límites

- Campos ausentes no deben darse por recibidos: selectedIndex/itemCount inicializados a cero pueden ocultar ausencia.
- No hay snapshot completo de OSK, scroll o borradores por los reportes actuales.
- validate_auto_params y cmd_stm_auto conservan la incongruencia AMDOC-ESP-001.

## Hallazgos conectados

- [AMDOC-ESP-001: El validador AutoMotion acepta formas de argumentos de otros comandos](../../../review/FINDINGS.md#amdoc-esp-001)
- [AMDOC-ESP-002: Los reportes sin selección publican selectedIndex/itemCount como cero](../../../review/FINDINGS.md#amdoc-esp-002)
- [AMDOC-WEB-004: La ventana del menú reconstruida en Web puede ejecutar otra fila en F4](../../../review/FINDINGS.md#amdoc-web-004)
- [AMDOC-WEB-006: Los builders de sensores no reciben el mismo modelo que la telemetría actual](../../../review/FINDINGS.md#amdoc-web-006)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/ws_bridge/common.inc` | c-cpp-structural | `d1d1e998378177d62cf61f3c4399594be3c159f75d20bec23e6499ffa3ae1cd0` |
| `src/ws_bridge/debug_wifi.inc` | c-cpp-structural | `7600e1af509d88cf424487ec23cd5e3177a9ae8af7121130a6551cf76b80ce9f` |
| `src/ws_bridge/dispatch.inc` | c-cpp-structural | `a68f8b623ae2a6afca04bc276373a13b4e3c4805563377cf4d2880de2ef18c87` |
| `src/ws_bridge/esp_commands.inc` | c-cpp-structural | `50aa0efbff724f6f83585807b913f3f6c742655b96cf798d172103a33e18e459` |
| `src/ws_bridge/lifecycle.inc` | c-cpp-structural | `a8df95a1f6f6a48f2469cd5b2bba03914c166f4f851c51e0afa0ceaac3ba105a` |
| `src/ws_bridge/oled_canvas.inc` | c-cpp-structural | `88857bca8551905eeda1a883698b9c51a3149a56d4380bddcbb5a2d2eeacc78a` |
| `src/ws_bridge/private_context.h` | c-cpp-structural | `7a4fd0ada613e4ad5f8694ca4be8b481d32a788a7e545d28f3fd773c34feaa63` |
| `src/ws_bridge/reports.inc` | c-cpp-structural | `393d3ccb97259dd5356a65c2a6420e13bce8213bf923e96454bd6fdb6234090c` |
| `src/ws_bridge/requests.inc` | c-cpp-structural | `bb8214cb1384534c64963c77e9cad065576737648ac3419694a011d415b51890` |
| `src/ws_bridge/services.inc` | c-cpp-structural | `1b7dd5c992672f045423a734888994490d40eaaf049e5207cf35ce10370c8412` |
| `src/ws_bridge/stm_commands.inc` | c-cpp-structural | `12972e9eba358fad483c9d68d280b2eaf5dc3fea40c47f1750933c22c9301ea5` |
| `src/ws_bridge/validation.inc` | c-cpp-structural | `a46da6c9d3e40f6008f8890b20573529260199035ba30879347ab9214fc431f0` |
| `src/ws_json_bridge.cpp` | c-cpp-structural | `ceaffe8d9c0688aaaecc20a4012d7753477f2ffeb9c1f40385179beaab99139b` |
| `src/ws_json_bridge.h` | c-cpp-structural | `4c9f80a80a6164a82bf1b9f9df3532ea0372307d262236ba9656c001e7056fd4` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WsClientState`

`esp:src/ws_bridge/private_context.h:95`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t used` | Dato de estado/configuración `used`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t id` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t generation` | Dato de estado/configuración `generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t preferred_api_v1` | Dato de estado/configuración `preferred_api_v1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t dropping` | Dato de estado/configuración `dropping`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t backpressure_drops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `size_t rx_len` | Dato de estado/configuración `rx_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char rx[WS_JSON_MAX + 1]` | Dato de estado/configuración `rx`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TimedTelemetryStop`

`esp:src/ws_bridge/private_context.h:107`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t client_id` | Dato de estado/configuración `client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char request_id[65]` | Dato de estado/configuración `request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t deadline_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `PendingWifiCredentialsWeb`

`esp:src/ws_bridge/private_context.h:115`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t session_only` | Dato de estado/configuración `session_only`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t requested` | Dato de estado/configuración `requested`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t connecting` | Dato de estado/configuración `connecting`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t notify_stm` | Dato de estado/configuración `notify_stm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t previous_auto_reconnect` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t has_bssid` | Dato de estado/configuración `has_bssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ap_provisioning` | Dato de estado/configuración `ap_provisioning`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t client_id` | Dato de estado/configuración `client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t generation` | Dato de estado/configuración `generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char request_id[65]` | Dato de estado/configuración `request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bssid[6]` | Dato de estado/configuración `bssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppConfig previous_config` | Memoria de la observación anterior usada por la máquina de estados. |
| `AppConfig candidate_config` | Dato de estado/configuración `candidate_config`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t started_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t deadline_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `PendingApCredentialsWeb`

`esp:src/ws_bridge/private_context.h:134`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t session_only` | Dato de estado/configuración `session_only`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t apply_radio` | Dato de estado/configuración `apply_radio`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t keep_sta` | Dato de estado/configuración `keep_sta`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t client_id` | Dato de estado/configuración `client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t generation` | Dato de estado/configuración `generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t apply_at_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `char request_id[65]` | Dato de estado/configuración `request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char command[65]` | Dato de estado/configuración `command`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppConfig previous_config` | Memoria de la observación anterior usada por la máquina de estados. |
| `AppConfig candidate_config` | Dato de estado/configuración `candidate_config`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiIpv4 previous_ip` | Memoria de la observación anterior usada por la máquina de estados. |
| `WifiIpv4 candidate_ip` | Dato de estado/configuración `candidate_ip`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ApCredentialsResult`

`esp:src/ws_bridge/private_context.h:151`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t available` | Dato de estado/configuración `available`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t success` | Dato de estado/configuración `success`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t completed_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `char request_id[65]` | Dato de estado/configuración `request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char ssid[SSID_MAX_LEN + 1]` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char status[16]` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `char reason[24]` | Dato de estado/configuración `reason`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char configured_ip[16]` | Dato de estado/configuración `configured_ip`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AutoScreenRefresh`

`esp:src/ws_bridge/private_context.h:163`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t reason_cmd` | Dato de estado/configuración `reason_cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t due_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `char request_id[65]` | Dato de estado/configuración `request_id`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PendingApProvisioningExit`

`esp:src/ws_bridge/private_context.h:171`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stop_at_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `PendingApClientDisconnect`

`esp:src/ws_bridge/private_context.h:177`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t mac[6]` | Dato de estado/configuración `mac`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t execute_at_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `OledCanvasBridge`

`esp:src/ws_bridge/private_context.h:193`

| Atributo / firma | Propósito |
| --- | --- |
| `OledCanvasAssembly assembly` | Dato de estado/configuración `assembly`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t phase` | Dato de estado/configuración `phase`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t api_v1` | Dato de estado/configuración `api_v1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t expected_operation` | Dato de estado/configuración `expected_operation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t uart_inflight` | Dato de estado/configuración `uart_inflight`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t begin_confirmed` | Dato de estado/configuración `begin_confirmed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cancel_requested` | Dato de estado/configuración `cancel_requested`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t owner_connected` | Dato de estado/configuración `owner_connected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t commit_resolved` | Dato de estado/configuración `commit_resolved`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t client_id` | Dato de estado/configuración `client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t generation` | Dato de estado/configuración `generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t node` | Dato de estado/configuración `node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t uart_next_offset` | Dato de estado/configuración `uart_next_offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t expected_next_offset` | Dato de estado/configuración `expected_next_offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t upload_deadline_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t total_deadline_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `char commit_request_id[65]` | Dato de estado/configuración `commit_request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char cancel_request_id[65]` | Dato de estado/configuración `cancel_request_id`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BridgeCommandSpec`

`esp:src/ws_bridge/private_context.h:224`

| Atributo / firma | Propósito |
| --- | --- |
| `const char *json_name` | Dato de estado/configuración `json_name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *default_target` | Dato de estado/configuración `default_target`; declaración estructural, interpretación específica pendiente de ampliar. |
| `BridgeCommandRoute route` | Dato de estado/configuración `route`; declaración estructural, interpretación específica pendiente de ampliar. |
| `BridgeParamsValidatorFn validate_params` | Dato de estado/configuración `validate_params`; declaración estructural, interpretación específica pendiente de ampliar. |
| `BridgeCommandExecFn execute` | Dato de estado/configuración `execute`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *decoder` | Dato de estado/configuración `decoder`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DebugWifiScope`

`esp:src/ws_bridge/private_context.h:238`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t owner_node` | Dato de estado/configuración `owner_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t previous_reconnect` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t mode_pending` | Dato de estado/configuración `mode_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t radio_dirty` | Dato de estado/configuración `radio_dirty`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiRadioMode previous_mode` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t mode_apply_at_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `AppConfig config` | Configuración copiada por la instancia; los punteros que contiene siguen siendo prestados. |

### `WsJsonBridgeContext`

`esp:src/ws_bridge/private_context.h:254`

| Atributo / firma | Propósito |
| --- | --- |
| `AsyncWebSocket *ws` | Dato de estado/configuración `ws`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *app` | Dato de estado/configuración `app`; declaración estructural, interpretación específica pendiente de ampliar. |
| `BridgeBackendOps backend` | Dato de estado/configuración `backend`; declaración estructural, interpretación específica pendiente de ampliar. |
| `BridgeWifiPort wifi` | Dato de estado/configuración `wifi`; declaración estructural, interpretación específica pendiente de ampliar. |
| `BridgePlatformOps platform` | Dato de estado/configuración `platform`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WsClientState clients[WS_CLIENT_MAX]{ estructura anidada }` | Dato de estado/configuración `clients`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UartTransactionManager transactions{ estructura anidada }` | Dato de estado/configuración `transactions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WsSessionManager sessions{ estructura anidada }` | Dato de estado/configuración `sessions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `StreamBroker streams{ estructura anidada }` | Dato de estado/configuración `streams`; declaración estructural, interpretación específica pendiente de ampliar. |
| `StreamRequestWaiters stream_waiters{ estructura anidada }` | Dato de estado/configuración `stream_waiters`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t logout_enqueued[WSM_MAX_SESSIONS]{ estructura anidada }` | Dato de estado/configuración `logout_enqueued`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t logout_retry_at_ms[WSM_MAX_SESSIONS]{ estructura anidada }` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t cleanup_release_at_ms[WSM_MAX_SESSIONS]{ estructura anidada }` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t auth_request_id` | Dato de estado/configuración `auth_request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `TimedTelemetryStop telem_stop{ estructura anidada }` | Dato de estado/configuración `telem_stop`; declaración estructural, interpretación específica pendiente de ampliar. |
| `DebugWifiScope debug_wifi{ estructura anidada }` | Dato de estado/configuración `debug_wifi`; declaración estructural, interpretación específica pendiente de ampliar. |
| `PendingWifiCredentialsWeb wifi_credentials_web{ estructura anidada }` | Dato de estado/configuración `wifi_credentials_web`; declaración estructural, interpretación específica pendiente de ampliar. |
| `PendingApCredentialsWeb ap_credentials_web{ estructura anidada }` | Dato de estado/configuración `ap_credentials_web`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ApCredentialsResult ap_credentials_result{ estructura anidada }` | Dato de estado/configuración `ap_credentials_result`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AutoScreenRefresh auto_screen_refresh{ estructura anidada }` | Dato de estado/configuración `auto_screen_refresh`; declaración estructural, interpretación específica pendiente de ampliar. |
| `PendingApProvisioningExit ap_provisioning_exit{ estructura anidada }` | Dato de estado/configuración `ap_provisioning_exit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `PendingApClientDisconnect ap_client_disconnect{ estructura anidada }` | Dato de estado/configuración `ap_client_disconnect`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledCanvasBridge oled_canvas{ estructura anidada }` | Dato de estado/configuración `oled_canvas`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oled_canvas_next_transfer_id` | Dato de estado/configuración `oled_canvas_next_transfer_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t json_scan_active` | Dato de estado/configuración `json_scan_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t json_scan_cleanup_pending` | Dato de estado/configuración `json_scan_cleanup_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t json_scan_started_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t json_scan_last_log_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t json_scan_client_id` | Dato de estado/configuración `json_scan_client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t json_scan_generation` | Dato de estado/configuración `json_scan_generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t json_scan_origin_ip_key` | Dato de estado/configuración `json_scan_origin_ip_key`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t json_scan_reconnect_until_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t json_scan_origin_api_v1` | Dato de estado/configuración `json_scan_origin_api_v1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t json_scan_previous_auto_reconnect` | Dato de estado/configuración `json_scan_previous_auto_reconnect`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char json_scan_request_id[65]{ estructura anidada }` | Dato de estado/configuración `json_scan_request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiScanSnapshot web_scan_cache{ estructura anidada }` | Dato de estado/configuración `web_scan_cache`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t json_scan_result_pending` | Dato de estado/configuración `json_scan_result_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t json_scan_result_timed_out` | Dato de estado/configuración `json_scan_result_timed_out`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t json_scan_result_failed` | Dato de estado/configuración `json_scan_result_failed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t json_scan_result_count` | Dato de estado/configuración `json_scan_result_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t json_scan_result_api_v1` | Dato de estado/configuración `json_scan_result_api_v1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t json_scan_result_ip_key` | Dato de estado/configuración `json_scan_result_ip_key`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t json_scan_result_expires_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `char json_scan_result_request_id[65]{ estructura anidada }` | Dato de estado/configuración `json_scan_result_request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t last_wifi_mode` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t last_sta_status` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t last_ap_clients` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t ws_backpressure_drops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `uint32_t api_v1_requests` | Dato de estado/configuración `api_v1_requests`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t legacy_envelopes` | Dato de estado/configuración `legacy_envelopes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t legacy_raw_requests` | Dato de estado/configuración `legacy_raw_requests`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t last_f4_alive_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t f4_offline_since_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t f4_alive_seen` | Dato de estado/configuración `f4_alive_seen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t f4_backend_online` | Dato de estado/configuración `f4_backend_online`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t f4_alive_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `BridgeParamsValidatorFn`

`esp:src/ws_bridge/private_context.h:220`

Callback: `bool (*BridgeParamsValidatorFn)(JsonObject root_payload, JsonObject &params, const char **message)`. Dispatcher invoca el validador con objeto args; bool decide admisión semántica. La vista JSON vive durante procesamiento; validador debe coincidir con el codificador de esa ruta.

### `BridgeCommandExecFn`

`esp:src/ws_bridge/private_context.h:222`

Callback: `void (*BridgeCommandExecFn)(uint32_t client_id, const char *request_id, const char *command, JsonObject params)`. Dispatcher invoca handler con client_id, requestId, command y args prestados; puede iniciar operación local o encolar F4. Copiar correlación/datos que sobrevivan a la recepción.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-wsjsonbridge_buildapclientspage"></a>`uint8_t WSJsonBridge_BuildApClientsPage(uint8_t cursor, uint8_t maximum, uint8_t *out, uint8_t capacity, uint8_t *out_len)` | Construye Ap Clients Page dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:108`; inferred-name |
| <a id="fn-wsjsonbridge_cancelwificredentials"></a>`uint8_t WSJsonBridge_CancelWifiCredentials(const char *ssid)` | Coordina `WifiIpv4`, `finish_wifi_credentials_flow`, `strcmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1329`; structural |
| <a id="fn-wsjsonbridge_getcontext"></a>`void *WSJsonBridge_GetContext()` | Consulta Context dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/lifecycle.inc:457`; inferred-name |
| <a id="fn-wsjsonbridge_getwificredentialsstate"></a>`uint8_t WSJsonBridge_GetWifiCredentialsState(uint8_t *flags, char *sta_ssid, size_t sta_ssid_capacity, char *ap_ssid, size_t ap_ssid_capacity)` | Consulta Wifi Credentials State dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1223`; inferred-name |
| <a id="fn-wsjsonbridge_init"></a>`void WSJsonBridge_Init(AsyncWebSocket *socket, UNER_App *app)` | Inicializa Bridge JSON, sesiones y traducción de pantallas dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_json_bridge.h:9`; inferred-name |
| <a id="fn-wsjsonbridge_initwithports"></a>`void WSJsonBridge_InitWithPorts(AsyncWebSocket *socket, const BridgeBackendOps &backend, const BridgeWifiPort &wifi, const BridgePlatformOps &platform)` | Inicializa With Ports dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/lifecycle.inc:2`; inferred-name |
| <a id="fn-wsjsonbridge_loop"></a>`void WSJsonBridge_Loop()` | Coordina `now_ms`, `service_ap_client_disconnect`, `service_ap_credentials_flow` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/lifecycle.inc:286`; structural |
| <a id="fn-wsjsonbridge_onconnect"></a>`bool WSJsonBridge_OnConnect(AsyncWebSocketClient *client, const char *resume_token)` | Atiende la notificación de Connect dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/lifecycle.inc:46`; inferred-name |
| <a id="fn-wsjsonbridge_ondata"></a>`void WSJsonBridge_OnData(AsyncWebSocketClient *client, AwsFrameInfo *info, uint8_t *data, size_t len)` | Atiende la notificación de Data dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/lifecycle.inc:236`; inferred-name |
| <a id="fn-wsjsonbridge_ondisconnect"></a>`bool WSJsonBridge_OnDisconnect(uint32_t client_id)` | Atiende la notificación de Disconnect dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/lifecycle.inc:191`; inferred-name |
| <a id="fn-wsjsonbridge_onunerpacket"></a>`uint8_t WSJsonBridge_OnUNERPacket(void *ctx, const UNER_Packet *pkt)` | Atiende la notificación de UNERPacket dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/lifecycle.inc:307`; inferred-name |
| <a id="fn-wsjsonbridge_requestwificredentials"></a>`void WSJsonBridge_RequestWifiCredentials(const char *ssid)` | Solicita Wifi Credentials dentro de Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1204`; inferred-name |
| <a id="fn-wsjsonbridge_scheduleapclientdisconnect"></a>`uint8_t WSJsonBridge_ScheduleApClientDisconnect(const uint8_t mac[6])` | Coordina `ap_client_is_connected`, `memcpy`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:152`; structural |
| <a id="fn-wsjsonbridge_submitwificredentials"></a>`uint8_t WSJsonBridge_SubmitWifiCredentials(const char *ssid, const char *password, bool reuse_password, const char *ip, const char *gateway, const char *subnet, const char *bssid)` | Coordina `ApplySTAConfig`, `ConnectSTA`, `IsSTAReconnectEnabled` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1246`; structural |
| <a id="fn-debug_wifi_begin"></a>`static bool debug_wifi_begin(WsmSession *session)` | Operación debug wifi begin de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/private_context.h:250`; inferred-name |
| <a id="fn-debug_wifi_release"></a>`static void debug_wifi_release()` | Operación debug wifi release de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/private_context.h:251`; inferred-name |
| <a id="fn-doc"></a>`DynamicJsonDocument doc(3072)` | Operación doc de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:990`; inferred-name |
| <a id="fn-legacy_doc"></a>`DynamicJsonDocument legacy_doc(1536)` | Operación legacy doc de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:147`; inferred-name |
| <a id="fn-lookup_f4_command"></a>`static const F4_CommandSpec *lookup_f4_command(uint8_t id, F4_CommandSpec &storage)` | Adapta la llamada a `F4_CommandRegistry_Lookup` con el contexto de esta integración. | `esp:src/ws_bridge/private_context.h:316`; structural |
| <a id="fn-service_debug_wifi_scope"></a>`static void service_debug_wifi_scope()` | Operación service debug wifi scope de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/private_context.h:252`; inferred-name |
| <a id="fn-v1_doc"></a>`DynamicJsonDocument v1_doc(3072)` | Operación v1 doc de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:270`; inferred-name |
| <a id="fn-validate_ap_client_disconnect_params"></a>`static bool validate_ap_client_disconnect_params(JsonObject root_payload, JsonObject &params, const char **message)` | Operación validate ap client disconnect params de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/private_context.h:234`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-add_ap_clients"></a>`static void add_ap_clients(JsonObject data)` | Coordina `createNestedArray`, `createNestedObject`, `format_ap_client_mac` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:164`; structural |
| <a id="fn-add_boot_report_fields"></a>`static bool add_boot_report_fields(JsonObject data, const uint8_t *payload, size_t len)` | Coordina `F4_DecodeBootReport`, `boot_handoff_name`, `boot_profile_name` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:101`; structural |
| <a id="fn-add_ir_emitters_fields"></a>`static void add_ir_emitters_fields(JsonObject data, const F4_IrEmitters &state)` | Operación add ir emitters fields de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:66`; inferred-name |
| <a id="fn-add_ir_snapshot_fields"></a>`static bool add_ir_snapshot_fields(JsonObject data, const uint8_t *p, size_t len)` | Coordina `F4_DecodeIrSnapshot`, `add`, `createNestedArray` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:40`; structural |
| <a id="fn-add_line_fields"></a>`static void add_line_fields(JsonObject data,const uint8_t *p)` | Coordina `add`, `createNestedArray`, `createNestedObject` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:343`; structural |
| <a id="fn-add_mpu_snapshot_fields"></a>`static void add_mpu_snapshot_fields(JsonObject data, const uint8_t *p)` | Coordina `F4_DecodeMpuSnapshot`, `createNestedObject` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:13`; structural |
| <a id="fn-add_reboot_target_info"></a>`static void add_reboot_target_info(JsonObject data, const AppConfig &config, uint8_t boot_mode)` | Coordina `strcmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:601`; structural |
| <a id="fn-add_scan_network_fields"></a>`static void add_scan_network_fields(JsonObject obj, const UNER_WifiScanCacheEntry &entry, uint8_t index)` | Coordina `String`, `add`, `createNestedArray` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:937`; structural |
| <a id="fn-add_scan_network_summary_fields"></a>`static void add_scan_network_summary_fields(JsonObject obj, const UNER_WifiScanCacheEntry &entry, uint8_t index)` | Coordina `String`, `snprintf` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:958`; structural |
| <a id="fn-add_screen_report"></a>`static bool add_screen_report(JsonObject data, const UNER_Packet *pkt)` | Traduce reportes binarios de pantalla a JSON con variantes según comando/source. | `esp:src/ws_bridge/reports.inc:186`; editorial |
| <a id="fn-add_wifi_info"></a>`static void add_wifi_info(JsonObject data, bool prefer_sta)` | Coordina `ap_clients`, `ap_ip`, `config` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:576`; structural |
| <a id="fn-ap_apply_state_name"></a>`static const char *ap_apply_state_name(uint8_t state)` | Operación ap apply state name de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/stm_commands.inc:220`; inferred-name |
| <a id="fn-ap_client_is_connected"></a>`static bool ap_client_is_connected(const uint8_t mac[6])` | Coordina `memcmp`, `station_first`, `station_free` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:90`; structural |
| <a id="fn-ap_provisioning_active"></a>`static bool ap_provisioning_active()` | Coordina `radio_mode`, `radio_status` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:550`; structural |
| <a id="fn-append_wifi_credentials_web_result"></a>`static uint8_t append_wifi_credentials_web_result(uint8_t status, const char *ssid, WifiIpv4 ip, uint8_t *out, uint8_t max_len)` | Coordina `bounded_strlen_u8` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1063`; structural |
| <a id="fn-auto_write_float"></a>`static void auto_write_float(uint8_t *p,float f)` | Adapta la llamada a `write_u32_le` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:392`; structural |
| <a id="fn-boot_handoff_name"></a>`static const char *boot_handoff_name(uint8_t handoff)` | Operación boot handoff name de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:84`; inferred-name |
| <a id="fn-boot_profile_name"></a>`static const char *boot_profile_name(uint8_t profile)` | Operación boot profile name de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:96`; inferred-name |
| <a id="fn-bounded_strlen_u8"></a>`static uint8_t bounded_strlen_u8(const char *s, uint8_t max_len)` | Operación bounded strlen u8 de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:635`; inferred-name |
| <a id="fn-broadcast_doc"></a>`static void broadcast_doc(JsonDocument &doc)` | Coordina `send_event_doc_to_client`, `wsm_find_const` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:287`; structural |
| <a id="fn-broadcast_doc_stm_authorized"></a>`static void broadcast_doc_stm_authorized(JsonDocument &doc)` | Coordina `send_event_doc_to_client`, `wsm_find_const` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:299`; structural |
| <a id="fn-canonical_field_allowed"></a>`static bool canonical_field_allowed(const char *key)` | Adapta la llamada a `strcmp` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:862`; structural |
| <a id="fn-canonical_target"></a>`static const char *canonical_target(const char *command)` | Adapta la llamada a `strncmp` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:869`; structural |
| <a id="fn-car_mode_name"></a>`static const char *car_mode_name(uint8_t mode)` | Operación car mode name de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/stm_commands.inc:209`; inferred-name |
| <a id="fn-clear_cleanup_tracking"></a>`static void clear_cleanup_tracking(uint8_t index)` | Operación clear cleanup tracking de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:115`; inferred-name |
| <a id="fn-clear_json_scan_request"></a>`static void clear_json_scan_request()` | Operación clear json scan request de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:8`; inferred-name |
| <a id="fn-clear_json_scan_result"></a>`static void clear_json_scan_result()` | Operación clear json scan result de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:19`; inferred-name |
| <a id="fn-clear_stm_session_and_streams"></a>`static void clear_stm_session_and_streams(WsmSession *session)` | Coordina `now_ms`, `sb_remove_client`, `srw_remove_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:847`; structural |
| <a id="fn-client_by_id"></a>`static AsyncWebSocketClient *client_by_id(uint32_t id)` | Adapta la llamada a `client` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:239`; structural |
| <a id="fn-cmd_ap_client_disconnect"></a>`static void cmd_ap_client_disconnect(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_ap_client_disconnect` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:83`; structural |
| <a id="fn-cmd_ap_clients_list"></a>`static void cmd_ap_clients_list(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_ap_clients_list` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:76`; structural |
| <a id="fn-cmd_ap_credentials_result_get"></a>`static void cmd_ap_credentials_result_get(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_ap_credentials_result_get` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:117`; structural |
| <a id="fn-cmd_auth_pin_change"></a>`static void cmd_auth_pin_change(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_pin_change_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:200`; structural |
| <a id="fn-cmd_auth_pin_login"></a>`static void cmd_auth_pin_login(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_pin_auth_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:190`; structural |
| <a id="fn-cmd_auth_pin_validate_screen"></a>`static void cmd_auth_pin_validate_screen(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_pin_auth_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:195`; structural |
| <a id="fn-cmd_auth_session_get"></a>`static void cmd_auth_session_get(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_auth_session_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:210`; structural |
| <a id="fn-cmd_auth_session_logout"></a>`static void cmd_auth_session_logout(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_auth_session_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:205`; structural |
| <a id="fn-cmd_auto_reconnect"></a>`static void cmd_auto_reconnect(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_auto_reconnect` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:167`; structural |
| <a id="fn-cmd_connect_wifi"></a>`static void cmd_connect_wifi(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_connect_wifi` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:124`; structural |
| <a id="fn-cmd_debug_ap_get"></a>`static void cmd_debug_ap_get(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `current_config_copy`, `make_response_base`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/debug_wifi.inc:83`; structural |
| <a id="fn-cmd_debug_ap_set"></a>`static void cmd_debug_ap_set(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_set_ap` con el contexto de esta integración. | `esp:src/ws_bridge/debug_wifi.inc:77`; structural |
| <a id="fn-cmd_debug_fallback"></a>`static void cmd_debug_fallback(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `isNull`, `make_response_base`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/debug_wifi.inc:124`; structural |
| <a id="fn-cmd_debug_mode_set"></a>`static void cmd_debug_mode_set(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `add_reboot_target_info`, `make_response_base`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/debug_wifi.inc:101`; structural |
| <a id="fn-cmd_debug_wifi_login"></a>`static void cmd_debug_wifi_login(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `debug_wifi_begin`, `make_response_base`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:2`; structural |
| <a id="fn-cmd_debug_wifi_logout"></a>`static void cmd_debug_wifi_logout(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `debug_wifi_release`, `make_response_base`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:34`; structural |
| <a id="fn-cmd_disconnect_wifi"></a>`static void cmd_disconnect_wifi(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_disconnect_wifi` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:149`; structural |
| <a id="fn-cmd_get_current_screen"></a>`static void cmd_get_current_screen(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_get_current_screen` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:177`; structural |
| <a id="fn-cmd_get_mode"></a>`static void cmd_get_mode(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_get_mode` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:64`; structural |
| <a id="fn-cmd_get_scan"></a>`static void cmd_get_scan(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_get_scan` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:95`; structural |
| <a id="fn-cmd_get_status"></a>`static void cmd_get_status(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_get_status` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:70`; structural |
| <a id="fn-cmd_get_wifi_detail"></a>`static void cmd_get_wifi_detail(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_wifi_detail_get` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:107`; structural |
| <a id="fn-cmd_oled_canvas_begin"></a>`static void cmd_oled_canvas_begin(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `OledCanvasAssembly_Begin`, `OledCanvas_ParseCrc32Hex`, `find_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:513`; structural |
| <a id="fn-cmd_oled_canvas_cancel"></a>`static void cmd_oled_canvas_cancel(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `copy_cstr`, `now_ms`, `oled_canvas_mark_cancel_requested` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:647`; structural |
| <a id="fn-cmd_oled_canvas_chunk"></a>`static void cmd_oled_canvas_chunk(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `OledCanvasAssembly_Append`, `OledCanvas_DecodeBase64`, `make_response_base_for_api` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:560`; structural |
| <a id="fn-cmd_oled_canvas_commit"></a>`static void cmd_oled_canvas_commit(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `OledCanvasAssembly_Commit`, `copy_cstr`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:616`; structural |
| <a id="fn-cmd_reboot_esp"></a>`static void cmd_reboot_esp(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_reboot_esp` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:89`; structural |
| <a id="fn-cmd_reboot_stm"></a>`static void cmd_reboot_stm(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:183`; structural |
| <a id="fn-cmd_set_ap"></a>`static void cmd_set_ap(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_set_ap` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:112`; structural |
| <a id="fn-cmd_start_ap"></a>`static void cmd_start_ap(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_start_ap` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:155`; structural |
| <a id="fn-cmd_stm_ap_config_get"></a>`static void cmd_stm_ap_config_get(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_ap_config_get` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:269`; structural |
| <a id="fn-cmd_stm_ap_config_get_alias"></a>`static void cmd_stm_ap_config_get_alias(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_ap_config_get` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:296`; structural |
| <a id="fn-cmd_stm_ap_config_set"></a>`static void cmd_stm_ap_config_set(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_ap_config_set` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:275`; structural |
| <a id="fn-cmd_stm_ap_config_set_alias"></a>`static void cmd_stm_ap_config_set_alias(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `dispatch_stm_ap_config_set`, `isNull`, `remove` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:281`; structural |
| <a id="fn-cmd_stm_auth_remote_result"></a>`static void cmd_stm_auth_remote_result(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_auth_remote_result` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:245`; structural |
| <a id="fn-cmd_stm_auto"></a>`static void cmd_stm_auto(uint32_t client,const char *request,const char *command,JsonObject params)` | Selecciona ID/offsets por nombre JSON y encola la operación AutoMotion. | `esp:src/ws_bridge/dispatch.inc:393`; editorial |
| <a id="fn-cmd_stm_encoder_button"></a>`static void cmd_stm_encoder_button(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_press_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:225`; structural |
| <a id="fn-cmd_stm_get_ap_fallback"></a>`static void cmd_stm_get_ap_fallback(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `start_stm_request` con el contexto de esta integración. | `esp:src/ws_bridge/stm_commands.inc:166`; structural |
| <a id="fn-cmd_stm_get_balance_pid"></a>`static void cmd_stm_get_balance_pid(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:461`; structural |
| <a id="fn-cmd_stm_get_boot_report"></a>`static void cmd_stm_get_boot_report(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:486`; structural |
| <a id="fn-cmd_stm_get_build_info"></a>`static void cmd_stm_get_build_info(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:481`; structural |
| <a id="fn-cmd_stm_get_car_mode"></a>`static void cmd_stm_get_car_mode(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:303`; structural |
| <a id="fn-cmd_stm_get_control_snapshot"></a>`static void cmd_stm_get_control_snapshot(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:471`; structural |
| <a id="fn-cmd_stm_get_extension_profile"></a>`static void cmd_stm_get_extension_profile(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_extension_profile_get` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:250`; structural |
| <a id="fn-cmd_stm_get_firmware_info"></a>`static void cmd_stm_get_firmware_info(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:476`; structural |
| <a id="fn-cmd_stm_get_i2c_policy"></a>`static void cmd_stm_get_i2c_policy(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:319`; structural |
| <a id="fn-cmd_stm_get_ir_emitters"></a>`static void cmd_stm_get_ir_emitters(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `start_stm_request` con el contexto de esta integración. | `esp:src/ws_bridge/stm_commands.inc:191`; structural |
| <a id="fn-cmd_stm_get_ir_snapshot"></a>`static void cmd_stm_get_ir_snapshot(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:491`; structural |
| <a id="fn-cmd_stm_get_mpu_snapshot"></a>`static void cmd_stm_get_mpu_snapshot(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:325`; structural |
| <a id="fn-cmd_stm_get_preferences"></a>`static void cmd_stm_get_preferences(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `dispatch_stm_empty_command`, `isNull`, `read_json_u32` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:350`; structural |
| <a id="fn-cmd_stm_get_telemetry_snapshot"></a>`static void cmd_stm_get_telemetry_snapshot(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_empty_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:368`; structural |
| <a id="fn-cmd_stm_line"></a>`static void cmd_stm_line(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `read_json_u32`, `start_stm_request`, `strcmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:423`; structural |
| <a id="fn-cmd_stm_menu_click"></a>`static void cmd_stm_menu_click(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_menu_click` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:215`; structural |
| <a id="fn-cmd_stm_motion"></a>`static void cmd_stm_motion(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `isNull`, `send_error`, `start_stm_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:435`; structural |
| <a id="fn-cmd_stm_reboot_into_profile"></a>`static void cmd_stm_reboot_into_profile(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_extension_profile_select` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:263`; structural |
| <a id="fn-cmd_stm_rotate_left"></a>`static void cmd_stm_rotate_left(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_screen_code_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:235`; structural |
| <a id="fn-cmd_stm_rotate_right"></a>`static void cmd_stm_rotate_right(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_screen_code_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:240`; structural |
| <a id="fn-cmd_stm_screen_page"></a>`static void cmd_stm_screen_page(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_screen_page` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:220`; structural |
| <a id="fn-cmd_stm_set_ap_fallback"></a>`static void cmd_stm_set_ap_fallback(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `start_stm_request` con el contexto de esta integración. | `esp:src/ws_bridge/stm_commands.inc:183`; structural |
| <a id="fn-cmd_stm_set_balance_pid"></a>`static void cmd_stm_set_balance_pid(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_set_pid` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:466`; structural |
| <a id="fn-cmd_stm_set_car_mode"></a>`static void cmd_stm_set_car_mode(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_set_car_mode` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:309`; structural |
| <a id="fn-cmd_stm_set_extension_profile"></a>`static void cmd_stm_set_extension_profile(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_extension_profile_select` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:257`; structural |
| <a id="fn-cmd_stm_set_i2c_policy"></a>`static void cmd_stm_set_i2c_policy(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_i2c_set_policy` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:314`; structural |
| <a id="fn-cmd_stm_set_ir_emitters"></a>`static void cmd_stm_set_ir_emitters(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `start_stm_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:199`; structural |
| <a id="fn-cmd_stm_set_ir_stream"></a>`static void cmd_stm_set_ir_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_ir_stream` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:496`; structural |
| <a id="fn-cmd_stm_set_mpu_stream"></a>`static void cmd_stm_set_mpu_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_mpu_stream` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:331`; structural |
| <a id="fn-cmd_stm_set_preferences"></a>`static void cmd_stm_set_preferences(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_set_preferences` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:345`; structural |
| <a id="fn-cmd_stm_stop_ir_stream"></a>`static void cmd_stm_stop_ir_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_ir_stream` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:501`; structural |
| <a id="fn-cmd_stm_stop_mpu_stream"></a>`static void cmd_stm_stop_mpu_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_mpu_stream` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:336`; structural |
| <a id="fn-cmd_stm_user_button"></a>`static void cmd_stm_user_button(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stm_press_command` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:230`; structural |
| <a id="fn-cmd_stop_ap"></a>`static void cmd_stop_ap(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stop_ap` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:161`; structural |
| <a id="fn-cmd_stop_scan"></a>`static void cmd_stop_scan(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_stop_scan` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:101`; structural |
| <a id="fn-cmd_telemetry_set_rate"></a>`static void cmd_telemetry_set_rate(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_telemetry_set_rate` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:172`; structural |
| <a id="fn-cmd_web_page_ready"></a>`static void cmd_web_page_ready(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `make_response_base`, `send_doc_to_client`, `send_error` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:47`; structural |
| <a id="fn-cmd_wifi_credentials_cancel"></a>`static void cmd_wifi_credentials_cancel(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_wifi_credentials_cancel` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:144`; structural |
| <a id="fn-cmd_wifi_credentials_get_current"></a>`static void cmd_wifi_credentials_get_current(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_wifi_credentials_get_current` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:130`; structural |
| <a id="fn-cmd_wifi_credentials_submit"></a>`static void cmd_wifi_credentials_submit(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `dispatch_wifi_credentials_submit` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:139`; structural |
| <a id="fn-command_requires_missing_stm_contract"></a>`static bool command_requires_missing_stm_contract(const char *command)` | Coordina `strcmp`, `strncmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:510`; structural |
| <a id="fn-complete_json_scan_driver_cleanup"></a>`static void complete_json_scan_driver_cleanup()` | Coordina `DeleteScanResults`, `RestoreModeAfterScan`, `SetSTAReconnectEnabled` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:36`; structural |
| <a id="fn-complete_response_from_manual_status"></a>`static void complete_response_from_manual_status(const UtmRequest *request, const UNER_Packet *pkt)` | Coordina `F4_ExtensionProfile_IsValid`, `extension_profile_name`, `make_response_base_for_api` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:410`; structural |
| <a id="fn-complete_response_from_payload"></a>`static void complete_response_from_payload(const UtmRequest *request, const UNER_Packet *pkt)` | Coordina `F4_DecodeBalancePidResponse`, `F4_DecodeBuildInfo`, `F4_DecodeControlSnapshot` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:443`; structural |
| <a id="fn-complete_response_from_screen_report"></a>`static void complete_response_from_screen_report(const UtmRequest *request, const UNER_Packet *pkt)` | Coordina `add_screen_report`, `emit_screen_report_event`, `make_response_base_for_api` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:391`; structural |
| <a id="fn-complete_stream_waiters"></a>`static void complete_stream_waiters(SbStream stream, SrwCompletionStatus status)` | Coordina `deliver_stream_completion`, `srw_take_for_stream` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:497`; structural |
| <a id="fn-copy_cstr"></a>`static void copy_cstr(char *dst, size_t dst_len, const char *src)` | Adapta la llamada a `strlen` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:203`; structural |
| <a id="fn-current_config_copy"></a>`static AppConfig current_config_copy()` | Adapta la llamada a `config` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:2`; structural |
| <a id="fn-deadline_reached"></a>`static bool deadline_reached(uint32_t now, uint32_t deadline)` | Operación deadline reached de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:31`; inferred-name |
| <a id="fn-debug_log_ws_send"></a>`static void debug_log_ws_send(const char *destination, const char *type, const char *event_name, const UNER_Packet *pkt)` | Operación debug log ws send de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:194`; inferred-name |
| <a id="fn-debug_log_ws_uner_packet"></a>`static void debug_log_ws_uner_packet(const char *stage, const UNER_Packet *pkt)` | Operación debug log ws uner packet de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:188`; inferred-name |
| <a id="fn-debug_print_hex_payload"></a>`static void debug_print_hex_payload(const uint8_t *payload, uint8_t len)` | Adapta la llamada a `print` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:145`; structural |
| <a id="fn-debug_wifi_begin"></a>`static bool debug_wifi_begin(WsmSession *session)` | Coordina `IsSTAReconnectEnabled`, `SetSessionConfig`, `current_config_copy` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/debug_wifi.inc:2`; structural |
| <a id="fn-debug_wifi_release"></a>`static void debug_wifi_release()` | Coordina `BeginSessionMode`, `DisconnectSTA`, `SetSTAReconnectEnabled` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/debug_wifi.inc:19`; structural |
| <a id="fn-deliver_pending_json_scan_result"></a>`static bool deliver_pending_json_scan_result(uint32_t client_id, uint8_t api_v1, bool resumed_after_disconnect)` | Coordina `WifiScanSnapshot_Count`, `WifiScanSnapshot_Get`, `add_scan_network_summary_fields` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:974`; structural |
| <a id="fn-deliver_stream_completion"></a>`static void deliver_stream_completion(const SrwCompletion *completion)` | Coordina `make_response_base_for_api`, `sb_stream_state`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:449`; structural |
| <a id="fn-dispatch_ap_client_disconnect"></a>`static void dispatch_ap_client_disconnect(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `ap_client_is_connected`, `format_ap_client_mac`, `make_response_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:205`; structural |
| <a id="fn-dispatch_ap_clients_list"></a>`static void dispatch_ap_clients_list(uint32_t client_id, const char *request_id, const char *command)` | Coordina `add_ap_clients`, `make_response_base`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:195`; structural |
| <a id="fn-dispatch_ap_credentials_result_get"></a>`static void dispatch_ap_credentials_result_get(uint32_t client_id, const char *request_id, const char *command)` | Coordina `make_response_base`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:544`; structural |
| <a id="fn-dispatch_api_v1_request"></a>`static void dispatch_api_v1_request(uint32_t client_id, JsonDocument &doc)` | Coordina `c_str`, `canonical_field_allowed`, `canonical_target` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:946`; structural |
| <a id="fn-dispatch_auth_session_command"></a>`static void dispatch_auth_session_command(uint32_t client_id, const char *request_id, const char *command, uint8_t action)` | Operación dispatch auth session command de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:128`; inferred-name |
| <a id="fn-dispatch_auth_session_command"></a>`static void dispatch_auth_session_command(uint32_t client_id, const char *request_id, const char *command, uint8_t action)` | Coordina `sb_remove_client`, `send_error`, `session_for_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:951`; structural |
| <a id="fn-dispatch_auto_reconnect"></a>`static void dispatch_auto_reconnect(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `SetSTAReconnectEnabled`, `make_response_base`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:834`; structural |
| <a id="fn-dispatch_canonical_raw"></a>`static void dispatch_canonical_raw(uint32_t client_id, const char *request_id, const char *command, JsonObject args)` | Coordina `ENABLE_DIAGNOSTIC_RAW_UNER`, `F4_CommandRegistry_IsWireAllowed`, `find_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:878`; structural |
| <a id="fn-dispatch_connect_wifi"></a>`static void dispatch_connect_wifi(uint32_t client_id, const char *request_id, const char *command)` | Coordina `ApplySTAConfig`, `ConnectSTA`, `SetSTAReconnectEnabled` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:566`; structural |
| <a id="fn-dispatch_device_command"></a>`static void dispatch_device_command(uint32_t client_id, JsonDocument &doc)` | Coordina `ap_provisioning_active`, `command_requires_missing_stm_contract`, `ensure_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:684`; structural |
| <a id="fn-dispatch_disconnect_wifi"></a>`static void dispatch_disconnect_wifi(uint32_t client_id, const char *request_id, const char *command)` | Coordina `DisconnectSTA`, `SetSTAReconnectEnabled`, `add_wifi_info` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:801`; structural |
| <a id="fn-dispatch_get_current_screen"></a>`static void dispatch_get_current_screen(uint32_t client_id, const char *request_id, const char *command)` | Adapta la llamada a `start_stm_request` con el contexto de esta integración. | `esp:src/ws_bridge/esp_commands.inc:874`; structural |
| <a id="fn-dispatch_get_mode"></a>`static void dispatch_get_mode(uint32_t client_id, const char *request_id, const char *command)` | Coordina `add_wifi_info`, `make_response_base`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:2`; structural |
| <a id="fn-dispatch_get_scan"></a>`static void dispatch_get_scan(uint32_t client_id, const char *request_id, const char *command)` | Coordina `DeleteScanResults`, `IsSTAReconnectEnabled`, `SetSTAReconnectEnabled` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:323`; structural |
| <a id="fn-dispatch_get_status"></a>`static void dispatch_get_status(uint32_t client_id, const char *request_id, const char *command)` | Coordina `IsSTAReconnectEnabled`, `String`, `add_wifi_info` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:10`; structural |
| <a id="fn-dispatch_pin_auth_command"></a>`static void dispatch_pin_auth_command(uint32_t client_id, const char *request_id, const char *command, JsonObject params, uint8_t scope)` | Coordina `now_ms`, `read_json_u32`, `read_pin_ascii4` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:884`; structural |
| <a id="fn-dispatch_pin_change_command"></a>`static void dispatch_pin_change_command(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `now_ms`, `read_pin_key_ascii4`, `send_error` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:923`; structural |
| <a id="fn-dispatch_reboot_esp"></a>`static void dispatch_reboot_esp(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `BootMode_ApplyRequest`, `add_reboot_target_info`, `current_config_copy` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:251`; structural |
| <a id="fn-dispatch_set_ap"></a>`static void dispatch_set_ap(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `GetAP_IP`, `GetMode`, `add_wifi_info` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:466`; structural |
| <a id="fn-dispatch_start_ap"></a>`static void dispatch_start_ap(uint32_t client_id, const char *request_id, const char *command)` | Coordina `ApplyAPConfig`, `StartAP`, `add_wifi_info` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:811`; structural |
| <a id="fn-dispatch_stm_ap_config_get"></a>`static void dispatch_stm_ap_config_get(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Adapta la llamada a `start_stm_request` con el contexto de esta integración. | `esp:src/ws_bridge/stm_commands.inc:231`; structural |
| <a id="fn-dispatch_stm_ap_config_set"></a>`static void dispatch_stm_ap_config_set(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `bounded_strlen_u8`, `isNull`, `memcpy` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:239`; structural |
| <a id="fn-dispatch_stm_auth_remote_result"></a>`static void dispatch_stm_auth_remote_result(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `read_json_u32`, `send_error`, `start_stm_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:90`; structural |
| <a id="fn-dispatch_stm_empty_command"></a>`static void dispatch_stm_empty_command(uint32_t client_id, const char *request_id, const char *command, uint8_t stm_cmd)` | Adapta la llamada a `start_stm_request` con el contexto de esta integración. | `esp:src/ws_bridge/stm_commands.inc:85`; structural |
| <a id="fn-dispatch_stm_extension_profile_get"></a>`static void dispatch_stm_extension_profile_get(uint32_t client_id, const char *request_id, const char *command)` | Coordina `F4_ExtensionProfile_BuildGetRequest`, `start_stm_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:126`; structural |
| <a id="fn-dispatch_stm_extension_profile_select"></a>`static void dispatch_stm_extension_profile_select(uint32_t client_id, const char *request_id, const char *command, JsonObject params, bool reboot)` | Coordina `F4_ExtensionProfile_BuildRebootRequest`, `F4_ExtensionProfile_BuildSetRequest`, `read_json_u32` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:137`; structural |
| <a id="fn-dispatch_stm_i2c_set_policy"></a>`static void dispatch_stm_i2c_set_policy(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `read_json_u32`, `send_error`, `start_stm_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:111`; structural |
| <a id="fn-dispatch_stm_ir_stream"></a>`static void dispatch_stm_ir_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `dispatch_stream_subscription`, `isNull`, `read_json_u32` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:345`; structural |
| <a id="fn-dispatch_stm_menu_click"></a>`static void dispatch_stm_menu_click(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `read_json_u32`, `send_error`, `start_stm_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:41`; structural |
| <a id="fn-dispatch_stm_mpu_stream"></a>`static void dispatch_stm_mpu_stream(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `dispatch_stream_subscription`, `isNull`, `read_json_u32` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:330`; structural |
| <a id="fn-dispatch_stm_press_command"></a>`static void dispatch_stm_press_command(uint32_t client_id, const char *request_id, const char *command, JsonObject params, uint8_t stm_cmd)` | Coordina `read_json_u32`, `read_press_kind`, `send_error` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:18`; structural |
| <a id="fn-dispatch_stm_screen_code_command"></a>`static void dispatch_stm_screen_code_command(uint32_t client_id, const char *request_id, const char *command, JsonObject params, uint8_t stm_cmd)` | Coordina `read_json_u32`, `send_error`, `start_stm_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:2`; structural |
| <a id="fn-dispatch_stm_screen_page"></a>`static void dispatch_stm_screen_page(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `read_json_u32`, `read_screen_page_direction`, `send_error` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:63`; structural |
| <a id="fn-dispatch_stm_set_car_mode"></a>`static void dispatch_stm_set_car_mode(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `read_json_u32`, `send_error`, `start_stm_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:317`; structural |
| <a id="fn-dispatch_stm_set_pid"></a>`static void dispatch_stm_set_pid(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `isNull`, `start_stm_request`, `write_f32_le` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:375`; structural |
| <a id="fn-dispatch_stm_set_preferences"></a>`static void dispatch_stm_set_preferences(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `read_json_u32`, `start_stm_request`, `write_u32_le` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:355`; structural |
| <a id="fn-dispatch_stop_ap"></a>`static void dispatch_stop_ap(uint32_t client_id, const char *request_id, const char *command)` | Coordina `StopAP`, `add_wifi_info`, `make_response_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:825`; structural |
| <a id="fn-dispatch_stop_scan"></a>`static void dispatch_stop_scan(uint32_t client_id, const char *request_id, const char *command)` | Coordina `find_client`, `finish_json_scan`, `make_response_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:411`; structural |
| <a id="fn-dispatch_stream_subscription"></a>`static bool dispatch_stream_subscription(uint32_t client_id, const char *request_id, const char *command, SbStream stream, bool enable, uint16_t requested_period_ms)` | Coordina `deliver_stream_completion`, `emit_request_accepted`, `find_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/requests.inc:137`; structural |
| <a id="fn-dispatch_telemetry_set_rate"></a>`static void dispatch_telemetry_set_rate(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `copy_cstr`, `dispatch_stream_subscription`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:849`; structural |
| <a id="fn-dispatch_wifi_credentials_cancel"></a>`static void dispatch_wifi_credentials_cancel(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `WifiIpv4`, `bounded_strlen_u8`, `find_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:756`; structural |
| <a id="fn-dispatch_wifi_credentials_get_current"></a>`static void dispatch_wifi_credentials_get_current(uint32_t client_id, const char *request_id, const char *command)` | Coordina `make_response_base`, `now_ms`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:587`; structural |
| <a id="fn-dispatch_wifi_credentials_submit"></a>`static void dispatch_wifi_credentials_submit(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `ApplySTAConfig`, `ConnectSTA`, `IsSTAReconnectEnabled` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:621`; structural |
| <a id="fn-dispatch_wifi_detail_get"></a>`static void dispatch_wifi_detail_get(uint32_t client_id, const char *request_id, const char *command, JsonObject params)` | Coordina `WifiScanSnapshot_FindBySsid`, `add_scan_network_fields`, `bounded_strlen_u8` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/esp_commands.inc:443`; structural |
| <a id="fn-emit_ap_fallback_ready"></a>`static void emit_ap_fallback_ready(const char *reason)` | Coordina `add_wifi_info`, `broadcast_doc`, `make_event_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1124`; structural |
| <a id="fn-emit_auth_session_invalidated"></a>`static void emit_auth_session_invalidated(uint32_t client_id, const char *reason)` | Coordina `createNestedObject`, `find_client`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:430`; structural |
| <a id="fn-emit_boot_report_event"></a>`static void emit_boot_report_event(const UNER_Packet *pkt)` | Coordina `add_boot_report_fields`, `broadcast_doc_stm_authorized`, `make_event_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:1198`; structural |
| <a id="fn-emit_bridge_error"></a>`static void emit_bridge_error(uint32_t client_id, const char *code, const char *message)` | Coordina `createNestedObject`, `find_client`, `make_event_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:518`; structural |
| <a id="fn-emit_car_mode_changed_event"></a>`static void emit_car_mode_changed_event(const UNER_Packet *pkt)` | Coordina `broadcast_doc_stm_authorized`, `car_mode_name`, `make_event_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:1234`; structural |
| <a id="fn-emit_f4_alive_event"></a>`static void emit_f4_alive_event(uint32_t received_at_ms)` | Coordina `createNestedObject`, `send_doc_to_client`, `wsm_find_const` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:407`; structural |
| <a id="fn-emit_generic_stm_event"></a>`static void emit_generic_stm_event(const UNER_Packet *pkt)` | Coordina `add`, `broadcast_doc_stm_authorized`, `createNestedArray` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:1183`; structural |
| <a id="fn-emit_ir_emitters_changed"></a>`static void emit_ir_emitters_changed(const UNER_Packet *pkt)` | Coordina `F4_DecodeIrEmitters`, `add_ir_emitters_fields`, `broadcast_doc_stm_authorized` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:73`; structural |
| <a id="fn-emit_ir_snapshot_event"></a>`static void emit_ir_snapshot_event(const UNER_Packet *pkt, const char *event_name)` | Coordina `add_ir_snapshot_fields`, `createNestedObject`, `legacy_doc` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:144`; structural |
| <a id="fn-emit_mpu_snapshot_event"></a>`static void emit_mpu_snapshot_event(const UNER_Packet *pkt, const char *event_name, const char *request_id)` | Coordina `add_mpu_snapshot_fields`, `createNestedObject`, `make_event_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:121`; structural |
| <a id="fn-emit_qt_alive_event"></a>`static void emit_qt_alive_event(const UNER_Packet *pkt)` | Coordina `broadcast_doc_stm_authorized`, `make_event_base`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:1259`; structural |
| <a id="fn-emit_qt_usb_state_event"></a>`static void emit_qt_usb_state_event(const UNER_Packet *pkt)` | Coordina `broadcast_doc_stm_authorized`, `make_event_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:1247`; structural |
| <a id="fn-emit_request_accepted"></a>`static void emit_request_accepted(uint32_t client_id, const char *request_id, const char *command, uint8_t queue_depth)` | Publica aceptación de cola, sin completar todavía la operación. | `esp:src/ws_bridge/requests.inc:31`; editorial |
| <a id="fn-emit_screen_report_event"></a>`static void emit_screen_report_event(const UNER_Packet *pkt, const char *event_name, const char *request_id)` | Coordina `add_screen_report`, `broadcast_doc_stm_authorized`, `debug_log_ws_send` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:299`; structural |
| <a id="fn-emit_telemetry_data"></a>`static void emit_telemetry_data(const UNER_Packet *pkt)` | Coordina `broadcast_doc`, `createNestedObject`, `make_event_base` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:167`; structural |
| <a id="fn-emit_wifi_credentials_requested"></a>`static void emit_wifi_credentials_requested(const char *ssid)` | Coordina `broadcast_doc`, `make_wifi_credentials_event` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1096`; structural |
| <a id="fn-emit_wifi_credentials_result"></a>`static void emit_wifi_credentials_result(const char *ssid, const char *status, const char *ip, const char *reason)` | Coordina `broadcast_doc`, `make_wifi_credentials_event`, `strcmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1103`; structural |
| <a id="fn-enqueue_internal"></a>`static UtmEnqueueResult enqueue_internal(uint8_t node, uint32_t client_id, uint32_t generation, uint8_t command_id, const uint8_t *payload, uint8_t payload_len, uint8_t internal_kind, const char *name)` | Coordina `F4_CommandRegistry_IsRequestLengthValid`, `F4_CommandRegistry_IsWireAllowed`, `copy_cstr` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:301`; structural |
| <a id="fn-ensure_client"></a>`static WsClientState *ensure_client(uint32_t id)` | Coordina `find_client`, `memset` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:222`; structural |
| <a id="fn-extension_profile_name"></a>`static const char *extension_profile_name(uint8_t profile_id)` | Operación extension profile name de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:322`; inferred-name |
| <a id="fn-extract_legacy_stm_payload"></a>`static bool extract_legacy_stm_payload(JsonObject root_payload, uint8_t *out, uint8_t expected_len, const char **message)` | Coordina `size` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:889`; structural |
| <a id="fn-f4_alive_now"></a>`static bool f4_alive_now()` | Coordina `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:70`; structural |
| <a id="fn-f4_payload_has_status_byte"></a>`static bool f4_payload_has_status_byte(uint8_t command_id)` | Operación f4 payload has status byte de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:375`; inferred-name |
| <a id="fn-find_bridge_command"></a>`static bool find_bridge_command(const char *command, BridgeCommandSpec &out)` | Coordina `memcpy_P`, `strcmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:658`; structural |
| <a id="fn-find_client"></a>`static WsClientState *find_client(uint32_t id)` | Operación find client de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:213`; inferred-name |
| <a id="fn-finish_json_scan"></a>`static void finish_json_scan()` | Coordina `ScanComplete`, `clear_json_scan_request`, `complete_json_scan_driver_cleanup` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:46`; structural |
| <a id="fn-finish_wifi_credentials_flow"></a>`static void finish_wifi_credentials_flow(uint8_t stm_status, const char *web_status, const char *reason, WifiIpv4 ip, bool restore_previous)` | Coordina `ConsumeAPFallbackReady`, `SetSTAReconnectEnabled`, `String` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1170`; structural |
| <a id="fn-format_ap_client_mac"></a>`static void format_ap_client_mac(const uint8_t mac[6], char out[18])` | Adapta la llamada a `snprintf` con el contexto de esta integración. | `esp:src/ws_bridge/esp_commands.inc:64`; structural |
| <a id="fn-format_ap_client_name"></a>`static void format_ap_client_name(const uint8_t mac[6], char out[24])` | Adapta la llamada a `snprintf` con el contexto de esta integración. | `esp:src/ws_bridge/esp_commands.inc:70`; structural |
| <a id="fn-invalidate_stm_session_and_streams"></a>`static void invalidate_stm_session_and_streams(WsmSession *session, const char *reason)` | Coordina `clear_stm_session_and_streams`, `emit_auth_session_invalidated` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:855`; structural |
| <a id="fn-ip_to_string"></a>`static String ip_to_string(WifiIpv4 ip)` | Coordina `String`, `snprintf` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:569`; structural |
| <a id="fn-is_ap_provisioning_command"></a>`static bool is_ap_provisioning_command(const char *command)` | Coordina `strcmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:557`; structural |
| <a id="fn-is_debug_wifi_command"></a>`static bool is_debug_wifi_command(const char *command)` | Coordina `find_bridge_command`, `strncmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:673`; structural |
| <a id="fn-is_extension_profile_selection_command"></a>`static bool is_extension_profile_selection_command(uint8_t command_id)` | Operación is extension profile selection command de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:332`; inferred-name |
| <a id="fn-is_line_command"></a>`static bool is_line_command(uint8_t cmd)` | Operación is line command de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:342`; inferred-name |
| <a id="fn-is_manual_status_stm_command"></a>`static bool is_manual_status_stm_command(uint8_t cmd)` | Operación is manual status stm command de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:771`; inferred-name |
| <a id="fn-is_screen_changed_event"></a>`static bool is_screen_changed_event(const UNER_Packet *pkt)` | Adapta la llamada a `is_screen_report_payload` con el contexto de esta integración. | `esp:src/ws_bridge/reports.inc:315`; structural |
| <a id="fn-is_screen_report_payload"></a>`static bool is_screen_report_payload(const UNER_Packet *pkt)` | Operación is screen report payload de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:310`; inferred-name |
| <a id="fn-line_read_f32"></a>`static float line_read_f32(const uint8_t *p)` | Adapta la llamada a `read_u32_le` con el contexto de esta integración. | `esp:src/ws_bridge/reports.inc:339`; structural |
| <a id="fn-make_event_base"></a>`static JsonObject make_event_base(JsonDocument &doc, const char *event, const char *origin, const char *request_id)` | Adapta la llamada a `createNestedObject` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:372`; structural |
| <a id="fn-make_response_base"></a>`static JsonObject make_response_base(JsonDocument &doc, uint32_t client_id, const char *request_id, const char *target, const char *command, bool ok, const char *code, const char *message)` | Coordina `find_client`, `make_response_base_for_api` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:363`; structural |
| <a id="fn-make_response_base_for_api"></a>`static JsonObject make_response_base_for_api(JsonDocument &doc, uint8_t api_v1, const char *request_id, const char *target, const char *command, bool ok, const char *code, const char *message)` | Coordina `createNestedObject`, `stable_error_code` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:330`; structural |
| <a id="fn-make_wifi_credentials_event"></a>`static JsonObject make_wifi_credentials_event(JsonDocument &doc, const char *event, const char *ssid)` | Coordina `createNestedObject` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1082`; structural |
| <a id="fn-mark_f4_backend_activity"></a>`static void mark_f4_backend_activity(uint32_t received_at_ms)` | Coordina `sb_backend_reconnected`, `stream_broker_has_runtime_state` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:102`; structural |
| <a id="fn-normalize_f4_alive_period"></a>`static uint16_t normalize_f4_alive_period(uint16_t period_ms)` | Operación normalize f4 alive period de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:82`; inferred-name |
| <a id="fn-oled_canvas_backend_lost"></a>`static void oled_canvas_backend_lost()` | Coordina `oled_canvas_reset`, `oled_canvas_send_cancel_error`, `oled_canvas_send_commit_error` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:153`; structural |
| <a id="fn-oled_canvas_deadline_reached"></a>`static bool oled_canvas_deadline_reached(uint32_t now, uint32_t deadline)` | Operación oled canvas deadline reached de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/oled_canvas.inc:2`; inferred-name |
| <a id="fn-oled_canvas_enqueue_operation"></a>`static bool oled_canvas_enqueue_operation(uint8_t operation)` | Coordina `F4_CommandRegistry_IsRequestLengthValid`, `F4_CommandRegistry_IsWireAllowed`, `OledCanvas_BuildBeginPayload` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:164`; structural |
| <a id="fn-oled_canvas_f4_error_code"></a>`static const char *oled_canvas_f4_error_code(uint8_t status)` | Operación oled canvas f4 error code de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/oled_canvas.inc:96`; inferred-name |
| <a id="fn-oled_canvas_f4_error_message"></a>`static const char *oled_canvas_f4_error_message(uint8_t status)` | Operación oled canvas f4 error message de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/oled_canvas.inc:104`; inferred-name |
| <a id="fn-oled_canvas_mark_cancel_requested"></a>`static void oled_canvas_mark_cancel_requested(uint32_t deadline_ms)` | Adapta la llamada a `OledCanvasAssembly_ScrubFramebuffer` con el contexto de esta integración. | `esp:src/ws_bridge/oled_canvas.inc:31`; structural |
| <a id="fn-oled_canvas_on_car_mode"></a>`static void oled_canvas_on_car_mode(uint8_t mode)` | Adapta la llamada a `oled_canvas_request_cancel` con el contexto de esta integración. | `esp:src/ws_bridge/oled_canvas.inc:458`; structural |
| <a id="fn-oled_canvas_on_disconnect"></a>`static void oled_canvas_on_disconnect(uint32_t client_id, uint32_t generation)` | Coordina `now_ms`, `oled_canvas_mark_cancel_requested`, `oled_canvas_reset` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:436`; structural |
| <a id="fn-oled_canvas_on_screen_code"></a>`static void oled_canvas_on_screen_code(uint32_t screen_code)` | Adapta la llamada a `oled_canvas_request_cancel` con el contexto de esta integración. | `esp:src/ws_bridge/oled_canvas.inc:451`; structural |
| <a id="fn-oled_canvas_only_keys"></a>`static bool oled_canvas_only_keys(JsonObject params, const char *const *allowed, size_t allowed_count, const char **message)` | Coordina `c_str`, `key`, `strcmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:458`; structural |
| <a id="fn-oled_canvas_request_cancel"></a>`static void oled_canvas_request_cancel(const char *message, uint8_t status)` | Coordina `now_ms`, `oled_canvas_mark_cancel_requested`, `oled_canvas_reset` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:131`; structural |
| <a id="fn-oled_canvas_require_owner"></a>`static bool oled_canvas_require_owner(uint32_t client_id, uint16_t transfer_id, const char *request_id, const char *command)` | Coordina `find_client`, `now_ms`, `oled_canvas_mark_cancel_requested` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:465`; structural |
| <a id="fn-oled_canvas_reset"></a>`static void oled_canvas_reset()` | Adapta la llamada a `OledCanvasAssembly_Init` con el contexto de esta integración. | `esp:src/ws_bridge/oled_canvas.inc:14`; structural |
| <a id="fn-oled_canvas_send_cancel_error"></a>`static void oled_canvas_send_cancel_error(const char *code, const char *message)` | Adapta la llamada a `send_error_for_api` con el contexto de esta integración. | `esp:src/ws_bridge/oled_canvas.inc:69`; structural |
| <a id="fn-oled_canvas_send_cancel_response"></a>`static void oled_canvas_send_cancel_response(uint32_t client_id, uint8_t api_v1, const char *request_id, uint16_t transfer_id)` | Coordina `make_response_base_for_api`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:54`; structural |
| <a id="fn-oled_canvas_send_commit_error"></a>`static void oled_canvas_send_commit_error(const char *code, const char *message, uint8_t f4_status)` | Coordina `make_response_base_for_api`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:38`; structural |
| <a id="fn-oled_canvas_send_rendered"></a>`static void oled_canvas_send_rendered()` | Coordina `OledCanvas_FormatCrc32Hex`, `make_response_base_for_api`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:78`; structural |
| <a id="fn-oled_canvas_session_valid"></a>`static bool oled_canvas_session_valid()` | Coordina `find_client`, `now_ms`, `oled_canvas_deadline_reached` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:20`; structural |
| <a id="fn-oled_canvas_transaction_complete"></a>`static void oled_canvas_transaction_complete(UtmCompletionStatus status, const UtmFrame *frame)` | Coordina `OledCanvas_ParseF4Response`, `oled_canvas_f4_error_code`, `oled_canvas_f4_error_message` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:242`; structural |
| <a id="fn-oled_canvas_transport_failure"></a>`static void oled_canvas_transport_failure(const char *code, const char *message)` | Coordina `now_ms`, `oled_canvas_mark_cancel_requested`, `oled_canvas_send_commit_error` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:235`; structural |
| <a id="fn-oled_canvas_uart_reserved"></a>`static bool oled_canvas_uart_reserved()` | Operación oled canvas uart reserved de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/oled_canvas.inc:7`; inferred-name |
| <a id="fn-parse_ap_client_mac"></a>`static bool parse_ap_client_mac(const char *text, uint8_t out[6])` | Adapta la llamada a `sscanf` con el contexto de esta integración. | `esp:src/ws_bridge/esp_commands.inc:77`; structural |
| <a id="fn-parse_bssid"></a>`static bool parse_bssid(const char *text, uint8_t out[6])` | Coordina `nibble`, `strlen` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:643`; structural |
| <a id="fn-parse_ipv4"></a>`static bool parse_ipv4(const char *text, WifiIpv4 &out)` | Coordina `WifiIpv4`, `sscanf` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:621`; structural |
| <a id="fn-parse_u32_text"></a>`static bool parse_u32_text(const char *text, uint32_t *out)` | Adapta la llamada a `strtoul` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:670`; structural |
| <a id="fn-process_json_message"></a>`static void process_json_message(uint32_t client_id, char *json)` | Coordina `c_str`, `deserializeJson`, `dispatch_api_v1_request` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:998`; structural |
| <a id="fn-publish_ap_credentials_result"></a>`static void publish_ap_credentials_result(const PendingApCredentialsWeb &transaction, bool success, const char *reason)` | Coordina `WifiScanSnapshot_Init`, `broadcast_doc`, `c_str` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:141`; structural |
| <a id="fn-read_i16_le"></a>`static int16_t read_i16_le(const uint8_t *p)` | Operación read i16 le de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/dispatch.inc:1029`; inferred-name |
| <a id="fn-read_json_u32"></a>`static bool read_json_u32(JsonObject params, const char *key, uint32_t *out)` | Coordina `parse_u32_text` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:682`; structural |
| <a id="fn-read_pin_ascii4"></a>`static bool read_pin_ascii4(JsonObject params, uint8_t ascii4[4])` | Adapta la llamada a `bounded_strlen_u8` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:712`; structural |
| <a id="fn-read_pin_key_ascii4"></a>`static bool read_pin_key_ascii4(JsonObject params, const char *key, uint8_t ascii4[4])` | Adapta la llamada a `bounded_strlen_u8` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:725`; structural |
| <a id="fn-read_press_kind"></a>`static bool read_press_kind(JsonObject params, uint8_t *out)` | Adapta la llamada a `strcmp` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:737`; structural |
| <a id="fn-read_screen_page_direction"></a>`static bool read_screen_page_direction(JsonObject params, uint8_t *out)` | Adapta la llamada a `strcmp` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:754`; structural |
| <a id="fn-read_u16_le"></a>`static uint16_t read_u16_le(const uint8_t *p)` | Operación read u16 le de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:2`; inferred-name |
| <a id="fn-read_u32_le"></a>`static uint32_t read_u32_le(const uint8_t *p)` | Operación read u32 le de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/reports.inc:7`; inferred-name |
| <a id="fn-reject_missing_stm_contract"></a>`static void reject_missing_stm_contract(uint32_t client_id, const char *request_id, const char *target, const char *command)` | Adapta la llamada a `send_error` con el contexto de esta integración. | `esp:src/ws_bridge/dispatch.inc:518`; structural |
| <a id="fn-require_params_object"></a>`static bool require_params_object(JsonObject payload, JsonObject &params)` | Operación require params object de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/validation.inc:2`; inferred-name |
| <a id="fn-resolve_legacy_stm_hex_command"></a>`static bool resolve_legacy_stm_hex_command(const char *command, uint8_t *stm_cmd, uint8_t *payload_len)` | Coordina `strcmp` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:822`; structural |
| <a id="fn-resolve_stream_waiters_if_converged"></a>`static void resolve_stream_waiters_if_converged(SbStream stream)` | Coordina `complete_stream_waiters`, `sb_is_converged` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:504`; structural |
| <a id="fn-restore_previous_ap"></a>`static void restore_previous_ap(const PendingApCredentialsWeb &transaction)` | Coordina `ApplyAPConfig`, `StartAP` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:174`; structural |
| <a id="fn-restore_previous_sta_connection"></a>`static void restore_previous_sta_connection(const PendingWifiCredentialsWeb &transaction)` | Coordina `ApplySTAConfig`, `ConnectSTA`, `DisconnectSTA` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1155`; structural |
| <a id="fn-revoke_all_stm_sessions_and_streams"></a>`static void revoke_all_stm_sessions_and_streams(const char *reason)` | Adapta la llamada a `invalidate_stm_session_and_streams` con el contexto de esta integración. | `esp:src/ws_bridge/reports.inc:863`; structural |
| <a id="fn-schedule_screen_refresh"></a>`static void schedule_screen_refresh(uint8_t reason_cmd, const char *request_id)` | Coordina `copy_cstr`, `memset`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:813`; structural |
| <a id="fn-screen_source_name"></a>`static const char *screen_source_name(uint8_t source)` | Operación screen source name de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:131`; inferred-name |
| <a id="fn-send_doc_to_client"></a>`static void send_doc_to_client(uint32_t client_id, JsonDocument &doc)` | Coordina `client_by_id`, `find_client`, `queueIsFull` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:244`; structural |
| <a id="fn-send_error"></a>`static void send_error(uint32_t client_id, const char *request_id, const char *target, const char *command, const char *code, const char *message)` | Adapta la llamada a `send_response` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:390`; structural |
| <a id="fn-send_error_for_api"></a>`static void send_error_for_api(uint32_t client_id, uint8_t api_v1, const char *request_id, const char *target, const char *command, const char *code, const char *message)` | Coordina `make_response_base_for_api`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:396`; structural |
| <a id="fn-send_event_doc_to_client"></a>`static void send_event_doc_to_client(uint32_t client_id, JsonDocument &doc)` | Coordina `createNestedObject`, `find_client`, `isNull` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:260`; structural |
| <a id="fn-send_response"></a>`static void send_response(uint32_t client_id, const char *request_id, const char *target, const char *command, bool ok, const char *code, const char *message)` | Coordina `make_response_base`, `send_doc_to_client` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:382`; structural |
| <a id="fn-send_wifi_credentials_result_to_stm"></a>`static void send_wifi_credentials_result_to_stm(uint8_t status, const char *ssid, WifiIpv4 ip)` | Coordina `append_wifi_credentials_web_result`, `else`, `printf` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:1134`; structural |
| <a id="fn-service_ap_client_disconnect"></a>`static void service_ap_client_disconnect()` | Coordina `memcpy`, `memset`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:269`; structural |
| <a id="fn-service_ap_credentials_flow"></a>`static void service_ap_credentials_flow()` | Coordina `ApplyAPConfig`, `GetMode`, `StartAP` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:182`; structural |
| <a id="fn-service_ap_fallback_ready"></a>`static void service_ap_fallback_ready()` | Coordina `ConsumeAPFallbackReady`, `emit_ap_fallback_ready`, `notify_ap_fallback` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:256`; structural |
| <a id="fn-service_ap_provisioning_exit"></a>`static void service_ap_provisioning_exit()` | Coordina `StopAP`, `now_ms`, `radio_status` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:130`; structural |
| <a id="fn-service_auto_screen_refresh"></a>`static void service_auto_screen_refresh()` | Coordina `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:281`; structural |
| <a id="fn-service_debug_wifi_scope"></a>`static void service_debug_wifi_scope()` | Coordina `BeginSessionMode`, `DisconnectSTA`, `StopAP` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/debug_wifi.inc:45`; structural |
| <a id="fn-service_f4_alive_timeout"></a>`static void service_f4_alive_timeout()` | Coordina `f4_alive_now`, `now_ms`, `oled_canvas_backend_lost` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:447`; structural |
| <a id="fn-service_json_scan"></a>`static void service_json_scan()` | Coordina `ScanComplete`, `WS_SCAN_LOG`, `ap_provisioning_active` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:2`; structural |
| <a id="fn-service_json_scan_driver_cleanup"></a>`static void service_json_scan_driver_cleanup()` | Coordina `ScanComplete`, `complete_json_scan_driver_cleanup` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:63`; structural |
| <a id="fn-service_oled_canvas"></a>`static void service_oled_canvas()` | Coordina `now_ms`, `oled_canvas_deadline_reached`, `oled_canvas_enqueue_operation` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/oled_canvas.inc:357`; structural |
| <a id="fn-service_session_cleanup"></a>`static void service_session_cleanup()` | Coordina `clear_cleanup_tracking`, `enqueue_internal`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:374`; structural |
| <a id="fn-service_session_expiry"></a>`static void service_session_expiry()` | Coordina `emit_auth_session_invalidated`, `finish_json_scan`, `now_ms` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:424`; structural |
| <a id="fn-service_stm_pending"></a>`static void service_stm_pending()` | Coordina `find_client`, `now_ms`, `sb_unsubscribe` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:291`; structural |
| <a id="fn-service_stream_broker"></a>`static void service_stream_broker()` | Coordina `complete_stream_waiters`, `enqueue_internal`, `resolve_stream_waiters_if_converged` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:326`; structural |
| <a id="fn-service_stream_waiter_timeouts"></a>`static void service_stream_waiter_timeouts()` | Coordina `deliver_stream_completion`, `now_ms`, `srw_take_expired` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:510`; structural |
| <a id="fn-service_wifi_credentials_flow"></a>`static void service_wifi_credentials_flow()` | Coordina `WifiIpv4`, `config`, `equals` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:77`; structural |
| <a id="fn-service_wifi_events"></a>`static void service_wifi_events()` | Coordina `add_wifi_info`, `ap_clients`, `broadcast_doc` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/services.inc:218`; structural |
| <a id="fn-session_for_client"></a>`static WsmSession *session_for_client(uint32_t client_id)` | Operación session for client de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:124`; inferred-name |
| <a id="fn-session_for_client"></a>`static WsmSession *session_for_client(uint32_t client_id)` | Coordina `find_client`, `wsm_find` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/requests.inc:2`; structural |
| <a id="fn-should_refresh_screen_after_status"></a>`static bool should_refresh_screen_after_status(uint8_t cmd, uint8_t status)` | Operación should refresh screen after status de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:793`; inferred-name |
| <a id="fn-stable_error_code"></a>`static const char *stable_error_code(const char *code)` | Coordina `strcmp`, `strstr` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/common.inc:310`; structural |
| <a id="fn-start_stm_request"></a>`static bool start_stm_request(uint32_t client_id, const char *request_id, const char *command, uint8_t stm_cmd, uint8_t response_cmd, const uint8_t *payload, uint8_t payload_len, uint16_t period_ms)` | Construye y encola una transacción F4 con identidad del cliente. | `esp:src/ws_bridge/requests.inc:55`; editorial |
| <a id="fn-stream_broker_has_runtime_state"></a>`static bool stream_broker_has_runtime_state()` | Adapta la llamada a `sb_stream_state` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:91`; structural |
| <a id="fn-stream_name"></a>`static const char *stream_name(SbStream stream)` | Operación stream name de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:444`; inferred-name |
| <a id="fn-transaction_complete"></a>`static void transaction_complete(void *ctx, const UtmRequest *request, UtmCompletionStatus status, const UtmFrame *frame)` | Operación transaction complete de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:126`; inferred-name |
| <a id="fn-transaction_complete"></a>`static void transaction_complete(void *ctx, const UtmRequest *request, UtmCompletionStatus status, const UtmFrame *frame)` | Coordina `F4_CommandRegistry_IsResponseLengthValid`, `F4_ExtensionProfile_DecodeResult`, `clear_cleanup_tracking` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:874`; structural |
| <a id="fn-transaction_owner"></a>`static UtmCommandOwner transaction_owner(F4_CommandOwner owner)` | Operación transaction owner de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/requests.inc:9`; inferred-name |
| <a id="fn-transaction_response_policy"></a>`static UtmResponsePolicy transaction_response_policy(F4_ResponsePolicy policy)` | Operación transaction response policy de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/requests.inc:20`; inferred-name |
| <a id="fn-transaction_send"></a>`static UtmSendResult transaction_send(void *ctx, const UtmRequest *request)` | Operación transaction send de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:125`; inferred-name |
| <a id="fn-transaction_send"></a>`static UtmSendResult transaction_send(void *ctx, const UtmRequest *request)` | Coordina `send_from_node` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:835`; structural |
| <a id="fn-valid_sta_ip"></a>`static bool valid_sta_ip(WifiIpv4 ip)` | Adapta la llamada a `WifiIpv4` con el contexto de esta integración. | `esp:src/ws_bridge/common.inc:665`; structural |
| <a id="fn-validate_ap_client_disconnect_params"></a>`static bool validate_ap_client_disconnect_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `c_str`, `key`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/reports.inc:1208`; structural |
| <a id="fn-validate_ap_credentials_params"></a>`static bool validate_ap_credentials_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `c_str`, `isNull`, `key` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:118`; structural |
| <a id="fn-validate_ap_fallback_params"></a>`static bool validate_ap_fallback_params(JsonObject root_payload, JsonObject &params, const char **message)` | Adapta la llamada a `size` con el contexto de esta integración. | `esp:src/ws_bridge/stm_commands.inc:174`; structural |
| <a id="fn-validate_auth_change_params"></a>`static bool validate_auth_change_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_pin_key_ascii4`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:200`; structural |
| <a id="fn-validate_auth_screen_params"></a>`static bool validate_auth_screen_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_json_u32`, `validate_pin_params` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:188`; structural |
| <a id="fn-validate_auto_params"></a>`static bool validate_auto_params(JsonObject root,JsonObject &params,const char **error)` | Acepta formas de argumentos AutoMotion sin distinguir suficientemente el comando; ver hallazgo. | `esp:src/ws_bridge/dispatch.inc:375`; editorial |
| <a id="fn-validate_empty_params"></a>`static bool validate_empty_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `require_params_object`, `size` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:49`; structural |
| <a id="fn-validate_line_calibration_params"></a>`static bool validate_line_calibration_params(JsonObject root_payload, JsonObject &params, const char **error)` | Coordina `read_json_u32`, `require_params_object`, `size` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:409`; structural |
| <a id="fn-validate_line_follow_params"></a>`static bool validate_line_follow_params(JsonObject root_payload, JsonObject &params, const char **error)` | Coordina `require_params_object`, `size` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/dispatch.inc:418`; structural |
| <a id="fn-validate_motion_percent_params"></a>`static bool validate_motion_percent_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `isNull`, `isfinite`, `read_json_u32` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:299`; structural |
| <a id="fn-validate_motion_switch_params"></a>`static bool validate_motion_switch_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `require_params_object`, `size` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:289`; structural |
| <a id="fn-validate_oled_canvas_begin_params"></a>`static bool validate_oled_canvas_begin_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `OledCanvas_ParseCrc32Hex`, `oled_canvas_only_keys`, `read_json_u32` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:483`; structural |
| <a id="fn-validate_oled_canvas_chunk_params"></a>`static bool validate_oled_canvas_chunk_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `oled_canvas_only_keys`, `read_json_u32`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:512`; structural |
| <a id="fn-validate_oled_canvas_transfer_params"></a>`static bool validate_oled_canvas_transfer_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `oled_canvas_only_keys`, `read_json_u32`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:539`; structural |
| <a id="fn-validate_optional_params"></a>`static bool validate_optional_params(JsonObject root_payload, JsonObject &params, const char **message)` | Operación validate optional params de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/validation.inc:9`; inferred-name |
| <a id="fn-validate_pin_params"></a>`static bool validate_pin_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_pin_ascii4`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:171`; structural |
| <a id="fn-validate_reboot_esp_params"></a>`static bool validate_reboot_esp_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `c_str`, `key`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:23`; structural |
| <a id="fn-validate_required_params"></a>`static bool validate_required_params(JsonObject root_payload, JsonObject &params, const char **message)` | Adapta la llamada a `require_params_object` con el contexto de esta integración. | `esp:src/ws_bridge/validation.inc:16`; structural |
| <a id="fn-validate_stm_ap_config_set_params"></a>`static bool validate_stm_ap_config_set_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `isNull`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/stm_commands.inc:303`; structural |
| <a id="fn-validate_stm_auth_remote_result_params"></a>`static bool validate_stm_auth_remote_result_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_json_u32`, `validate_stm_screen_code_params` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:270`; structural |
| <a id="fn-validate_stm_car_mode_params"></a>`static bool validate_stm_car_mode_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_json_u32`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:369`; structural |
| <a id="fn-validate_stm_extension_profile_params"></a>`static bool validate_stm_extension_profile_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `F4_ExtensionProfile_IsValid`, `read_json_u32`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:332`; structural |
| <a id="fn-validate_stm_i2c_policy_params"></a>`static bool validate_stm_i2c_policy_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_json_u32`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:352`; structural |
| <a id="fn-validate_stm_ir_emitters_params"></a>`static bool validate_stm_ir_emitters_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `require_params_object`, `size` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:320`; structural |
| <a id="fn-validate_stm_menu_click_params"></a>`static bool validate_stm_menu_click_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_json_u32`, `validate_stm_screen_code_params` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:244`; structural |
| <a id="fn-validate_stm_mpu_stream_params"></a>`static bool validate_stm_mpu_stream_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `isNull`, `read_json_u32`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:385`; structural |
| <a id="fn-validate_stm_pid_params"></a>`static bool validate_stm_pid_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `isNull`, `isfinite`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:434`; structural |
| <a id="fn-validate_stm_preferences_params"></a>`static bool validate_stm_preferences_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_json_u32`, `require_params_object`, `size` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:408`; structural |
| <a id="fn-validate_stm_press_params"></a>`static bool validate_stm_press_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_press_kind`, `validate_stm_screen_code_params` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:231`; structural |
| <a id="fn-validate_stm_screen_code_params"></a>`static bool validate_stm_screen_code_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_json_u32`, `require_params_object` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:214`; structural |
| <a id="fn-validate_stm_screen_page_params"></a>`static bool validate_stm_screen_page_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `read_screen_page_direction`, `validate_stm_screen_code_params` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:257`; structural |
| <a id="fn-validate_wifi_credentials_cancel_params"></a>`static bool validate_wifi_credentials_cancel_params(JsonObject root_payload, JsonObject &params, const char **message)` | Operación validate wifi credentials cancel params de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/validation.inc:153`; inferred-name |
| <a id="fn-validate_wifi_credentials_submit_params"></a>`static bool validate_wifi_credentials_submit_params(JsonObject root_payload, JsonObject &params, const char **message)` | Coordina `c_str`, `isNull`, `key` para Bridge JSON, sesiones y traducción de pantallas. | `esp:src/ws_bridge/validation.inc:64`; structural |
| <a id="fn-wifi_mode_string"></a>`static const char *wifi_mode_string(uint8_t mode)` | Operación wifi mode string de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:538`; inferred-name |
| <a id="fn-write_f32_le"></a>`static void write_f32_le(float value, uint8_t out[4])` | Adapta la llamada a `write_u32_le` con el contexto de esta integración. | `esp:src/ws_bridge/stm_commands.inc:368`; structural |
| <a id="fn-write_u32_le"></a>`static void write_u32_le(uint32_t value, uint8_t out[4])` | Operación write u32 le de Bridge JSON, sesiones y traducción de pantallas; semántica fina pendiente de ampliación. | `esp:src/ws_bridge/common.inc:703`; inferred-name |
