# Handlers UNER de aplicación ESP

ID: `esp:module/src/uner_app_handlers.h` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Ejecuta comandos cuyo dueño es ESP y coordina respuestas, red, estado y reinicio diferido.

Operaciones Wi-Fi/AP, credenciales, info de dispositivo, assets y sesiones vistas desde UART.

<a id="modularity"></a>
## Modularidad observada

Aplicación con operaciones inyectadas; depende de esquemas y semántica de ESP/robot, no es parser genérico.

<a id="state"></a>
## Handle, estado y atributos externos

UNER_App conserva estado de sus operaciones; UNER_AppServices presta Wi-Fi/config/server/restart. No emitir configuración privada completa como telemetría.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- AppBindings conecta servicios antes de UNER_App_Init.
- UNER_App_Loop atiende acciones diferidas y emisión de respuestas/eventos.
- Reinicio se programa después de la aceptación/respuesta según el flujo implementado.

<a id="interactions"></a>
## Interacciones

- Web/JSON y UART pueden solicitar servicios del mismo dispositivo; mantener dueño/correlación.
- Wi-Fi, NVS y scan usan sus servicios en vez de duplicar sus máquinas de estados.

<a id="limitations"></a>
## Precauciones y límites

- ACK intermedio y respuesta final no son equivalentes.
- Los guardas de transporte del ESP no sustituyen permisos F4.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/uner_app_handlers.cpp` | c-cpp-structural | `0327378d6e7cf26aeb1d61841fda7bf8319b725966d88a615b0907c84e03db0b` |
| `src/uner_app_handlers.h` | c-cpp-structural | `04cafe686a80f9e7c3129e0dba8a3d6e592cecc0d71590051ba338b1a45c5f1f` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `UnerApplicationState`

`esp:src/uner_app_handlers.cpp:10`

| Atributo / firma | Propósito |
| --- | --- |
| `bool scan_active` | Dato de estado/configuración `scan_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool scan_stopped_by_command` | Dato de estado/configuración `scan_stopped_by_command`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool scan_timed_out` | Dato de estado/configuración `scan_timed_out`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t scan_started_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t scan_notify_dst` | Dato de estado/configuración `scan_notify_dst`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t scan_notify_enabled` | Dato de estado/configuración `scan_notify_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t scan_finalizer_status` | Dato de estado/configuración `scan_finalizer_status`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t scan_last_count` | Dato de estado/configuración `scan_last_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t scan_last_log_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `WifiScanSnapshot scan_cache{ estructura anidada }` | Dato de estado/configuración `scan_cache`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiScanArbiter scan_arbiter{ estructura anidada }` | Dato de estado/configuración `scan_arbiter`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_App`

`esp:src/uner_app_handlers.h:11`

| Atributo / firma | Propósito |
| --- | --- |
| `UNER_Handle handle` | Dato de estado/configuración `handle`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_Transport serial_transport` | Dato de estado/configuración `serial_transport`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t this_node` | Dato de estado/configuración `this_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t peer_node` | Dato de estado/configuración `peer_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t last_sta_status` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t last_wifi_mode` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t last_ap_clients` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t boot_complete_sent` | Dato de estado/configuración `boot_complete_sent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t reboot_pending` | Dato de estado/configuración `reboot_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t reboot_boot_mode` | Dato de estado/configuración `reboot_boot_mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t reboot_not_before_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-uner_app_findscancachebyssid"></a>`uint8_t UNER_App_FindScanCacheBySsid(const char *ssid, UNER_WifiScanCacheEntry *out)` | Adapta la llamada a `WifiScanSnapshot_FindBySsid` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:111`; structural |
| <a id="fn-uner_app_getscancachecount"></a>`uint8_t UNER_App_GetScanCacheCount(void)` | Adapta la llamada a `WifiScanSnapshot_Count` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:101`; structural |
| <a id="fn-uner_app_getscancacheentry"></a>`uint8_t UNER_App_GetScanCacheEntry(uint8_t index, UNER_WifiScanCacheEntry *out)` | Adapta la llamada a `WifiScanSnapshot_Get` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:106`; structural |
| <a id="fn-uner_app_getwifiscanowner"></a>`WifiScanOwner UNER_App_GetWifiScanOwner(void)` | Adapta la llamada a `WifiScanArbiter_GetOwner` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:96`; structural |
| <a id="fn-uner_app_init"></a>`void UNER_App_Init(UNER_App *app, uint8_t this_node, uint8_t peer_node)` | Inicializa handlers, nodo propio y peer para la aplicación ESP. | `esp:src/uner_app_handlers.cpp:1648`; editorial |
| <a id="fn-uner_app_iswifiscanowner"></a>`uint8_t UNER_App_IsWifiScanOwner(WifiScanOwner owner)` | Adapta la llamada a `WifiScanArbiter_IsOwner` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:91`; structural |
| <a id="fn-uner_app_loop"></a>`void UNER_App_Loop(UNER_App *app)` | Atiende protocolo y operaciones diferidas de aplicación. | `esp:src/uner_app_handlers.cpp:1716`; editorial |
| <a id="fn-uner_app_notifyapclientjoined"></a>`void UNER_App_NotifyApClientJoined(UNER_App *app, uint8_t ap_clients, uint8_t client_number)` | Adapta la llamada a `send_evt` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:1788`; structural |
| <a id="fn-uner_app_notifyapclientleft"></a>`void UNER_App_NotifyApClientLeft(UNER_App *app, uint8_t ap_clients, uint8_t client_number)` | Adapta la llamada a `send_evt` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:1796`; structural |
| <a id="fn-uner_app_notifyapfallbackready"></a>`void UNER_App_NotifyApFallbackReady(UNER_App *app)` | Notifica Ap Fallback Ready dentro de Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1804`; inferred-name |
| <a id="fn-uner_app_notifywebpageclosed"></a>`void UNER_App_NotifyWebPageClosed(UNER_App *app, uint32_t client_id, uint8_t client_number)` | Adapta la llamada a `notify_web_page` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:1783`; structural |
| <a id="fn-uner_app_notifywebpageready"></a>`void UNER_App_NotifyWebPageReady(UNER_App *app, uint32_t client_id, uint8_t client_number)` | Adapta la llamada a `notify_web_page` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:1778`; structural |
| <a id="fn-uner_app_notifywebserverup"></a>`void UNER_App_NotifyWebserverUp(UNER_App *app)` | Notifica Webserver Up dentro de Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1749`; inferred-name |
| <a id="fn-uner_app_notifywsclientconnected"></a>`void UNER_App_NotifyWsClientConnected(UNER_App *app, uint32_t client_id)` | Adapta la llamada a `send_evt` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:1759`; structural |
| <a id="fn-uner_app_notifywsclientdisconnected"></a>`void UNER_App_NotifyWsClientDisconnected(UNER_App *app, uint32_t client_id)` | Adapta la llamada a `send_evt` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:1765`; structural |
| <a id="fn-uner_app_oncommandtable"></a>`void UNER_App_OnCommandTable(UNER_App *app)` | Atiende la notificación de Command Table dentro de Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1606`; inferred-name |
| <a id="fn-uner_app_releasewifiscan"></a>`uint8_t UNER_App_ReleaseWifiScan(WifiScanOwner owner)` | Adapta la llamada a `WifiScanArbiter_Release` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:86`; structural |
| <a id="fn-uner_app_sendeventtopeer"></a>`UNER_Status UNER_App_SendEventToPeer(UNER_App *app, uint8_t evt_cmd, const uint8_t *payload, uint8_t len)` | Adapta la llamada a `UNER_Handle_SendRaw` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:283`; structural |
| <a id="fn-uner_app_sendrawfromnode"></a>`UNER_Status UNER_App_SendRawFromNode(UNER_App *app, uint8_t src_node, uint8_t dst_node, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Adapta la llamada a `UNER_Handle_SendRaw` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:277`; structural |
| <a id="fn-uner_app_sendrawtopeer"></a>`UNER_Status UNER_App_SendRawToPeer(UNER_App *app, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Adapta la llamada a `UNER_Handle_SendRaw` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:271`; structural |
| <a id="fn-uner_app_setservices"></a>`void UNER_App_SetServices(const UNER_AppServices &services)` | Configura Services dentro de Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:25`; inferred-name |
| <a id="fn-uner_app_tryacquirewifiscan"></a>`WifiScanAcquireResult UNER_App_TryAcquireWifiScan(WifiScanOwner owner)` | Adapta la llamada a `WifiScanArbiter_TryAcquire` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:81`; structural |
| <a id="fn-uner_app_updatescancachefromdriver"></a>`void UNER_App_UpdateScanCacheFromDriver(int network_count)` | Adapta la llamada a `UNER_App_UpdateScanSnapshotFromDriver` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:76`; structural |
| <a id="fn-uner_app_updatescansnapshotfromdriver"></a>`void UNER_App_UpdateScanSnapshotFromDriver(WifiScanSnapshot *snapshot, int network_count)` | Actualiza Scan Snapshot From Driver dentro de Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:62`; inferred-name |
| <a id="fn-ip"></a>`WifiIpv4 ip(0, 0, 0, 0)` | Operación ip de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:133`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-append_ipv4"></a>`static uint8_t append_ipv4(uint8_t *out, uint8_t max_len, WifiIpv4 ip)` | Operación append ipv4 de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:116`; inferred-name |
| <a id="fn-append_reset_diagnostic"></a>`static uint8_t append_reset_diagnostic(uint8_t *out, uint8_t max_len)` | Coordina `reset_diagnostic`, `webServices` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:358`; structural |
| <a id="fn-append_wifi_detail_payload"></a>`static uint8_t append_wifi_detail_payload(const UNER_WifiScanCacheEntry *entry, uint8_t *out, uint8_t max_len)` | Operación append wifi detail payload de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:385`; inferred-name |
| <a id="fn-append_wifi_info"></a>`static uint8_t append_wifi_info(uint8_t *out, uint8_t max_len, bool prefer_sta)` | Coordina `ap_ip`, `config`, `ip` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:126`; structural |
| <a id="fn-auth_permission_id_supported"></a>`static bool auth_permission_id_supported(uint8_t permission_id)` | Operación auth permission id supported de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:892`; inferred-name |
| <a id="fn-auth_ttl_for_permission"></a>`static uint32_t auth_ttl_for_permission(uint8_t permission_id)` | Operación auth ttl for permission de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:898`; inferred-name |
| <a id="fn-build_scan_results_payload"></a>`static uint8_t build_scan_results_payload(uint8_t *out, uint8_t max_len)` | Coordina `WifiScanSnapshot_Count`, `WifiScanSnapshot_Get` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:332`; structural |
| <a id="fn-build_wifi_credentials_flow_data"></a>`static uint8_t build_wifi_credentials_flow_data(uint8_t action, uint8_t extra_flags, uint8_t *out, uint8_t capacity)` | Coordina `credentials_state`, `memcpy`, `strnlen` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1063`; structural |
| <a id="fn-clear_scan_cache"></a>`static void clear_scan_cache()` | Adapta la llamada a `WifiScanSnapshot_Init` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:57`; structural |
| <a id="fn-current_config_copy"></a>`static AppConfig current_config_copy()` | Coordina `config`, `webServices` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:39`; structural |
| <a id="fn-current_network_interface"></a>`static uint8_t current_network_interface(WifiIpv4 &ip)` | Coordina `WifiIpv4`, `ap_ip`, `radio_mode` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:372`; structural |
| <a id="fn-emit_mode_and_link_events"></a>`static void emit_mode_and_link_events(UNER_App *app)` | Coordina `UNER_Handle_SendRaw`, `ap_clients`, `ap_ip` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:483`; structural |
| <a id="fn-emit_network_ip_evt_and_cmd"></a>`static void emit_network_ip_evt_and_cmd(UNER_App *app, uint8_t net_if, const WifiIpv4 &ip)` | Coordina `UNER_Handle_SendRaw`, `fill_network_ip_payload`, `send_evt` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:304`; structural |
| <a id="fn-fill_interfaces_connected"></a>`static void fill_interfaces_connected(uint8_t out[4])` | Coordina `connected_count`, `webServices` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:323`; structural |
| <a id="fn-fill_network_ip_payload"></a>`static uint8_t fill_network_ip_payload(uint8_t out[5], uint8_t net_if, const WifiIpv4 &ip)` | Operación fill network ip payload de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:294`; inferred-name |
| <a id="fn-flush_uner_tx"></a>`static void flush_uner_tx(UNER_App *app, uint16_t timeout_ms)` | Coordina `UNER_Handle_TxPump`, `cooperate`, `flush_serial` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:175`; structural |
| <a id="fn-h_ap_client_disconnect"></a>`static void h_ap_client_disconnect(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `disconnect_ap_client`, `webServices` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1152`; structural |
| <a id="fn-h_ap_clients_page"></a>`static void h_ap_clients_page(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `ap_clients_page`, `webServices` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1141`; structural |
| <a id="fn-h_app_pin_config"></a>`static void h_app_pin_config(void *ctx, const UNER_Packet *pkt)` | Coordina `is_pin_ascii4_format_valid`, `log_cmd`, `save_test_pin_ascii4` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:974`; structural |
| <a id="fn-h_auth_validate_pin"></a>`static void h_auth_validate_pin(void *ctx, const UNER_Packet *pkt)` | Coordina `auth_permission_id_supported`, `auth_ttl_for_permission`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:904`; structural |
| <a id="fn-h_clear_credentials"></a>`static void h_clear_credentials(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `log_cmd`, `save_config` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:729`; structural |
| <a id="fn-h_connect_wifi"></a>`static void h_connect_wifi(void *ctx, const UNER_Packet *pkt)` | Coordina `ApplySTAConfig`, `ConnectSTA`, `ConsumeSTAAttemptFailure` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1367`; structural |
| <a id="fn-h_disconnect_wifi"></a>`static void h_disconnect_wifi(void *ctx, const UNER_Packet *pkt)` | Coordina `DisconnectSTA`, `SetSTAReconnectEnabled`, `UNER_Handle_SendResponse` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1389`; structural |
| <a id="fn-h_echo"></a>`static void h_echo(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:555`; structural |
| <a id="fn-h_factory_reset"></a>`static void h_factory_reset(void *ctx, const UNER_Packet *pkt)` | Adapta la llamada a `h_clear_credentials` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:754`; structural |
| <a id="fn-h_get_connected_users"></a>`static void h_get_connected_users(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `connected_count`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1160`; structural |
| <a id="fn-h_get_connected_users_mode"></a>`static void h_get_connected_users_mode(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `ap_clients`, `connected_count` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1589`; structural |
| <a id="fn-h_get_credentials"></a>`static void h_get_credentials(void *ctx, const UNER_Packet *pkt)` | Adapta la llamada a `UNER_Handle_SendNack` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:1358`; structural |
| <a id="fn-h_get_interfaces_connected"></a>`static void h_get_interfaces_connected(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `fill_interfaces_connected`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1349`; structural |
| <a id="fn-h_get_preferences"></a>`static void h_get_preferences(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `config`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:607`; structural |
| <a id="fn-h_get_scan_results"></a>`static void h_get_scan_results(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_App_GetWifiScanOwner`, `UNER_Handle_SendResponse`, `UNER_SCAN_LOG` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1256`; structural |
| <a id="fn-h_get_status"></a>`static void h_get_status(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `ap_clients`, `ap_ip` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:562`; structural |
| <a id="fn-h_get_user_info"></a>`static void h_get_user_info(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `client_info`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1167`; structural |
| <a id="fn-h_get_wifi_detail"></a>`static void h_get_wifi_detail(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_App_FindScanCacheBySsid`, `UNER_Handle_SendNack`, `UNER_Handle_SendRaw` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1307`; structural |
| <a id="fn-h_ping"></a>`static void h_ping(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:544`; structural |
| <a id="fn-h_reboot_esp"></a>`static void h_reboot_esp(void *ctx, const UNER_Packet *pkt)` | Coordina `BootMode_ApplyRequest`, `BootMode_IsValid`, `UNER_SCAN_LOG` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:759`; structural |
| <a id="fn-h_request_firmware"></a>`static void h_request_firmware(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:818`; structural |
| <a id="fn-h_set_ap_config"></a>`static void h_set_ap_config(void *ctx, const UNER_Packet *pkt)` | Coordina `ApplyAPConfig`, `StartAP`, `StopAP` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1399`; structural |
| <a id="fn-h_set_ap_fallback"></a>`static void h_set_ap_fallback(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendAck`, `UNER_Handle_SendResponse`, `config` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1554`; structural |
| <a id="fn-h_set_auto_reconnect"></a>`static void h_set_auto_reconnect(void *ctx, const UNER_Packet *pkt)` | Coordina `IsSTAReconnectEnabled`, `SetSTAReconnectEnabled`, `UNER_Handle_SendResponse` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1577`; structural |
| <a id="fn-h_set_credentials"></a>`static void h_set_credentials(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `current_config_copy`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:666`; structural |
| <a id="fn-h_set_encoder_fast"></a>`static void h_set_encoder_fast(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `log_cmd` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1341`; structural |
| <a id="fn-h_set_mode_ap"></a>`static void h_set_mode_ap(void *ctx, const UNER_Packet *pkt)` | Coordina `ApplyAPConfig`, `GetMode`, `StartAP` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:626`; structural |
| <a id="fn-h_set_mode_sta"></a>`static void h_set_mode_sta(void *ctx, const UNER_Packet *pkt)` | Coordina `ApplySTAConfig`, `ConnectSTA`, `SetSTAReconnectEnabled` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:645`; structural |
| <a id="fn-h_start_ap"></a>`static void h_start_ap(void *ctx, const UNER_Packet *pkt)` | Coordina `ApplyAPConfig`, `GetMode`, `StartAP` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1528`; structural |
| <a id="fn-h_start_scan"></a>`static void h_start_scan(void *ctx, const UNER_Packet *pkt)` | Coordina `DeleteScanResults`, `SetSTAReconnectEnabled`, `StartScan` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1179`; structural |
| <a id="fn-h_stop_ap"></a>`static void h_stop_ap(void *ctx, const UNER_Packet *pkt)` | Coordina `StopAP`, `UNER_Handle_SendResponse`, `append_wifi_info` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1545`; structural |
| <a id="fn-h_stop_scan"></a>`static void h_stop_scan(void *ctx, const UNER_Packet *pkt)` | Coordina `DeleteScanResults`, `UNER_App_GetWifiScanOwner`, `UNER_App_ReleaseWifiScan` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1224`; structural |
| <a id="fn-h_wifi_credentials_flow"></a>`static void h_wifi_credentials_flow(void *ctx, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendResponse`, `build_wifi_credentials_flow_data`, `cancel_credentials` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1089`; structural |
| <a id="fn-is_pin_ascii4_format_valid"></a>`static bool is_pin_ascii4_format_valid(const uint8_t *ascii4)` | Operación is pin ascii4 format valid de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:859`; inferred-name |
| <a id="fn-log_cmd"></a>`static void log_cmd(const char* name, const UNER_Packet *pkt)` | Operación log cmd de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:289`; inferred-name |
| <a id="fn-notify_web_page"></a>`static void notify_web_page(UNER_App *app, uint8_t event, uint32_t id, uint8_t number)` | Adapta la llamada a `send_evt` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:1771`; structural |
| <a id="fn-read_uner_text_field"></a>`static bool read_uner_text_field(const UNER_Packet *pkt, uint8_t *offset, char *out, uint8_t capacity, bool allow_empty)` | Coordina `memcpy` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:1047`; structural |
| <a id="fn-save_test_pin_ascii4"></a>`static bool save_test_pin_ascii4(const uint8_t *ascii4)` | Operación save test pin ascii4 de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:870`; inferred-name |
| <a id="fn-send_app_pin_config_response"></a>`static void send_app_pin_config_response(UNER_App *app, const UNER_Packet *pkt, uint8_t status, uint8_t action, uint8_t code)` | Adapta la llamada a `send_response_with_retry` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:964`; structural |
| <a id="fn-send_auth_validate_pin_response"></a>`static void send_auth_validate_pin_response(UNER_App *app, const UNER_Packet *pkt, uint8_t status, uint8_t request_id, uint8_t permission_id, bool granted, uint32_t ttl_ms, uint8_t attempts_left)` | Coordina `send_response_with_retry` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:876`; structural |
| <a id="fn-send_boot_complete_evt_and_cmd"></a>`static void send_boot_complete_evt_and_cmd(UNER_App *app, uint8_t net_if, const WifiIpv4 &ip)` | Coordina `UNER_Handle_SendRaw`, `fill_network_ip_payload`, `send_evt` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:312`; structural |
| <a id="fn-send_evt"></a>`static void send_evt(UNER_App *app, uint8_t evt_cmd, const uint8_t *payload, uint8_t len)` | Adapta la llamada a `UNER_Handle_SendRaw` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:169`; structural |
| <a id="fn-send_reboot_ack_and_flush"></a>`static bool send_reboot_ack_and_flush(UNER_App *app, const UNER_Packet *pkt)` | Coordina `UNER_Handle_SendAck`, `flush_uner_tx` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:188`; structural |
| <a id="fn-send_reboot_nack_and_flush"></a>`static void send_reboot_nack_and_flush(UNER_App *app, const UNER_Packet *pkt, UNER_Status reason)` | Coordina `UNER_Handle_SendNack`, `flush_uner_tx` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:206`; structural |
| <a id="fn-send_response_with_retry"></a>`static bool send_response_with_retry(UNER_App *app, const UNER_Packet *pkt, uint8_t status, const uint8_t *data, uint8_t data_len, uint16_t flush_timeout_ms)` | Coordina `UNER_Handle_SendResponse`, `flush_uner_tx` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:246`; structural |
| <a id="fn-send_scan_results_async"></a>`static void send_scan_results_async(UNER_App *app, uint8_t status)` | Coordina `UNER_Handle_SendRaw`, `UNER_SCAN_LOG` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:401`; structural |
| <a id="fn-service_reboot_state_machine"></a>`static void service_reboot_state_machine(UNER_App *app)` | Coordina `UNER_Handle_TxPump`, `UNER_SCAN_LOG`, `delay_ms` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:222`; structural |
| <a id="fn-service_scan_state_machine"></a>`static void service_scan_state_machine(UNER_App *app)` | Coordina `DeleteScanResults`, `ScanComplete`, `UNER_App_IsWifiScanOwner` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:426`; structural |
| <a id="fn-stored_test_pin_matches"></a>`static bool stored_test_pin_matches(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)` | Operación stored test pin matches de Handlers UNER de aplicación ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_handlers.cpp:829`; inferred-name |
| <a id="fn-validate_test_pin_ascii4"></a>`static bool validate_test_pin_ascii4(const uint8_t *ascii4)` | Coordina `stored_test_pin_matches` para Handlers UNER de aplicación ESP. | `esp:src/uner_app_handlers.cpp:838`; structural |
| <a id="fn-webservices"></a>`static const UNER_AppServices &webServices()` | Adapta la llamada a `UNER_App_DefaultServices` con el contexto de esta integración. | `esp:src/uner_app_handlers.cpp:26`; structural |
