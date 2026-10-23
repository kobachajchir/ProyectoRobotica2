# Puertos y adaptadores de plataforma ESP

ID: `esp:module/src/bridge_ports.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Define operaciones de radio, red, plataforma, almacenamiento y backend que consume la integración JSON/UNER.

Tablas BridgeWifiOps, BridgeBackendOps, BridgePlatformOps, UNER_AppServices y fachadas que prestan recursos.

<a id="modularity"></a>
## Modularidad observada

Abstrae llamadas concretas, pero bridge_ports.h incluye Arduino, config, UNER y tipos Wi-Fi; los String y AsyncWebSocketClient mantienen dependencia de plataforma/modelo.

<a id="state"></a>
## Handle, estado y atributos externos

Los puertos contienen ops/context, funciones y referencias prestadas a estado de reinicio. No transfieren propiedad de credenciales, AppConfig, cliente AsyncWebSocket ni recursos SDK.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- La composición suministra tablas completas antes de consumidores.
- Cada método de fachada reenvía context; algunos callbacks opcionales comprueban null y otros exigen un puerto completo.

<a id="interactions"></a>
## Interacciones

- BridgeWifiPort concentra mutaciones de radio en WifiService.
- BridgeBackendOps conecta envío/scan/config con UNER sin que el bridge conozca buffers UART.

<a id="limitations"></a>
## Precauciones y límites

- Un void* requiere emparejamiento correcto con su tabla.
- La inyección no vuelve no bloqueantes restart, flush, filesystem ni operaciones de radio.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/bridge_platform_esp.cpp` | c-cpp-structural | `1a36cc8b22c014968772624ae4048e9823fab582a1d78645f28527c4a12da80d` |
| `src/bridge_ports.h` | c-cpp-structural | `552e5a00a21472b80b4d30e3893aeba34042507cba1e5716c19cb24744abe1df` |
| `src/esp_platform.cpp` | c-cpp-structural | `c84d5f92724ebe3f9bbcdadb7a56c55102771a59e48aa73c89103f5470894129` |
| `src/esp_platform.h` | c-cpp-structural | `3c5f3e2e9c94f41b4616dc09c9d9139f2c1abb6a1193bd0d1253bfc7b1bc6e7c` |
| `src/uner_app_ports.h` | c-cpp-structural | `856b7057900e6a6d42374fbd426dc778520f7a14e0d4a07c093d259abaaf80b8` |
| `src/uner_platform_esp.h` | c-cpp-structural | `c1b58c5fa3692b335172a5cc3f69a5e2af1cfd282274eafcd3f8186d2e050596` |
| `src/webserver_ports.h` | c-cpp-structural | `8a4509bf609a1e28ff471771498f1666b88414a211b8890ff5185b88bde2dee7` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `BridgeWifiOps`

`esp:src/bridge_ports.h:9`

| Atributo / firma | Propósito |
| --- | --- |
| `void (*SetSessionConfig)(void *, const AppConfig *)` | Presta la configuración al servicio Wi-Fi durante la sesión; la fachada de bridge la invoca con el contexto registrado. |
| `bool (*BeginSessionMode)(void *, const AppConfig &, WifiRunMode)` | Inicia en el servicio el modo de radio elegido para esta sesión; no confirma conectividad por sí solo. |
| `int (*StartScan)(void *, bool async, bool show_hidden)` | Solicita exploración a la única autoridad de radio; consumir resultado por ScanComplete/snapshot. |
| `int (*ScanComplete)(void *)` | Consulta avance o cantidad de resultados de la exploración del driver; distinguir pendiente y error según retorno. |
| `void (*DeleteScanResults)(void *)` | Libera resultados temporales del driver después de copiarlos o terminar su consumo. |
| `void (*RestoreModeAfterScan)(void *)` | Pide restaurar la política de radio guardada antes del scan; no abrir otro propietario de Wi-Fi. |
| `WifiLinkStatus (*ConnectSTA)(void *, const char* ssid, const char* password, bool keep_ap, bool disconnect_first, const uint8_t* bssid)` | Solicita conexión STA con los argumentos prestados del llamador; el estado se observa posteriormente. |
| `bool (*DisconnectSTA)(void *, bool radio_off, bool erase_sdk_credentials)` | Desconecta STA mediante el servicio de radio y su contexto, no mediante una segunda instancia de WiFi. |
| `bool (*ApplySTAConfig)(void *, const AppConfig& cfg)` | Aplica direccionamiento/configuración STA antes de la conexión; retorno sigue la firma del puerto. |
| `bool (*ApplyAPConfig)(void *, const AppConfig& cfg)` | Aplica configuración IP del AP mediante el driver compartido. |
| `bool (*StartAP)(void *, const char* ssid, const char* password, bool keep_sta, uint8_t channel, bool hidden, uint8_t max_clients, bool ap_protected)` | Solicita inicio SoftAP y conserva separada la observación de STA conectada. |
| `bool (*StopAP)(void *, bool radio_off)` | Detiene SoftAP mediante la autoridad común de radio. |
| `void (*SetSTAReconnectEnabled)(void *, bool enabled)` | Configura la política de reconexión automática STA en el servicio. |
| `bool (*IsSTAReconnectEnabled)(void *)` | Consulta la política de reconexión configurada, no el éxito de un intento actual. |
| `WifiServiceState (*GetState)(void *)` | Obtiene la vista de estado del servicio Wi-Fi; no transfiere propiedad del handle. |
| `WifiIpv4 (*GetAP_IP)(void *)` | Obtiene dirección AP del servicio para reportarla al bridge. |
| `WifiRadioMode (*GetMode)(void *)` | Consulta modo de radio actual para decidir/restaurar transiciones. |
| `bool (*ConsumeAPFallbackReady)(void *)` | Consume la señal pendiente de fallback AP listo para notificar una sola vez. |
| `bool (*ConsumeSTAAttemptFailure)(void *)` | Consume resultado pendiente de fallo del intento STA. |
| `void (*NotifySTAConfigurationFailure)(void *)` | Informa al servicio que la configuración STA falló y debe reflejarlo en su política/estado. |

### `BridgeWifiPort`

`esp:src/bridge_ports.h:31`

| Atributo / firma | Propósito |
| --- | --- |
| `const BridgeWifiOps *ops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BridgeBackendOps`

`esp:src/bridge_ports.h:55`

| Atributo / firma | Propósito |
| --- | --- |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t *reboot_pending` | Dato de estado/configuración `reboot_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t *reboot_boot_mode` | Dato de estado/configuración `reboot_boot_mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t *reboot_not_before_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `UNER_Status (*send_peer)(void *, uint8_t, const uint8_t *, uint8_t)` | Entrega comando/payload al backend UNER para el peer elegido; admisión del envío no es respuesta del peer. |
| `UNER_Status (*send_from_node)(void *, uint8_t, uint8_t, uint8_t, const uint8_t *, uint8_t)` | Envía desde una identidad de nodo explícita, preservando correlación de sesión/ruta. |
| `uint8_t (*web_page_ready)(void *, uint32_t)` | Notifica al backend la presencia/disponibilidad de página Web. |
| `void (*notify_ap_fallback)(void *)` | Propaga al backend el resultado de fallback AP para actualizar estado F4. |
| `void (*intercept_packets)(void *, UNER_PacketInterceptorFn, void *)` | Activa/desactiva o instala intercepción de paquetes según firma del puerto; no sustituye el parser. |
| `WifiScanAcquireResult (*acquire_scan)(void *, WifiScanOwner)` | Adquiere la exploración compartida en el árbitro para el consumidor. |
| `uint8_t (*release_scan)(void *, WifiScanOwner)` | Libera la demanda/propiedad de scan del consumidor sin asumir que no quedan otros interesados. |
| `void (*scan_snapshot)(void *, WifiScanSnapshot *, int)` | Entrega vista del resultado retenido de scan; respetar lifetime del almacenamiento dueño. |
| `const AppConfig *(*config)(void *)` | Presta configuración vigente al bridge; un puntero retornado no concede propiedad ni persistencia implícita. |
| `bool (*save_config)(void *, const AppConfig &)` | Solicita persistencia explícita de la configuración; conservar resultado de error. |
| `bool (*storage_ready)(void *)` | Consulta si el backend de almacenamiento está disponible antes de usarlo. |

### `BridgePlatformOps`

`esp:src/bridge_ports.h:74`

| Atributo / firma | Propósito |
| --- | --- |
| `uint32_t (*now_ms)()` | Consulta reloj de plataforma en ms para deadlines; no usar como reloj de adquisición en us. |
| `uint32_t (*random_u32)()` | Obtiene una palabra aleatoria de plataforma; su origen/garantías pertenecen al adaptador. |
| `uint32_t (*free_heap)()` | Obtiene memoria heap libre para diagnóstico. |
| `uint32_t (*max_free_block)()` | Obtiene el bloque contiguo máximo libre; no equivale a heap total. |
| `uint8_t (*heap_fragmentation)()` | Obtiene indicador de fragmentación del heap del backend. |
| `String (*core_version)()` | Presta/entrega versión del core de plataforma para diagnóstico. |
| `const char *(*sdk_version)()` | Presta/entrega versión SDK; no es versión de contrato UNER. |
| `bool (*asset_exists)(const char *)` | Consulta existencia del recurso solicitado en almacenamiento. |
| `WifiRadioMode (*radio_mode)()` | Consulta modo Wi-Fi desde el SDK para diagnóstico. |
| `WifiLinkStatus (*radio_status)()` | Consulta estado STA del SDK; requiere mapear su enum al modelo del servicio. |
| `WifiIpv4 (*sta_ip)()` | Obtiene IP de estación actual. |
| `WifiIpv4 (*ap_ip)()` | Obtiene IP SoftAP actual. |
| `String (*sta_ssid)()` | Obtiene SSID observado de la conexión STA, no una credencial de contraseña. |
| `int32_t (*sta_rssi)()` | Obtiene RSSI STA según unidad del SDK para diagnóstico de conexión. |
| `uint8_t (*ap_clients)()` | Consulta cantidad de estaciones asociadas al AP; no equivale a sesiones WebSocket. |
| `WifiIpv4 (*client_ip)(AsyncWebSocketClient *)` | Obtiene IP del cliente WebSocket prestado al puerto. |
| `void *(*station_first)()` | Obtiene primer elemento de la enumeración de estaciones AP; liberar con station_free según contrato del backend. |
| `void *(*station_next)(void *)` | Avanza por la enumeración de estaciones AP sin transferir propiedad del nodo. |
| `const uint8_t *(*station_mac)(void *)` | Extrae identidad MAC de una entrada de estación. |
| `uint32_t (*station_ip)(void *)` | Extrae IP de una entrada de estación. |
| `void (*station_free)()` | Libera recursos de enumeración de estaciones en el backend que los creó. |
| `bool (*station_deauth)(uint8_t *)` | Solicita desasociación de la estación AP identificada; no concede permisos de control F4. |

### `UNER_AppServices`

`esp:src/uner_app_ports.h:7`

| Atributo / firma | Propósito |
| --- | --- |
| `BridgeWifiPort wifi` | Dato de estado/configuración `wifi`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const BridgePlatformOps *platform` | Dato de estado/configuración `platform`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const AppConfig *(*config)()` | Presta configuración de aplicación al handler UNER. |
| `bool (*save_config)(const AppConfig &)` | Persiste cambios de configuración desde la capa de aplicación. |
| `bool (*scan_entry)(uint8_t, WifiScanSnapshotEntry *)` | Obtiene entrada de resultados de scan para codificar una respuesta UNER. |
| `void (*reset_diagnostic)(uint8_t *reason, uint8_t *exception)` | Obtiene o aplica la operación diagnóstica de reset según firma del servicio inyectado. |
| `void (*flush_serial)()` | Solicita vaciado del serial; puede bloquear según adaptador, no cambia la semántica ACK. |
| `void (*cooperate)()` | Cede ejecución al runtime para cooperar con Wi-Fi/watchdog; no sustituye una máquina de estados. |
| `void (*delay_ms)(uint32_t)` | Espera solicitada en ms mediante plataforma; callback no vuelve asíncrona esa espera. |
| `void (*restart)()` | Solicita reinicio físico ESP; considerar orden de respuesta/flush y no seguir usando estado previo. |
| `void (*initialize_serial_transport)(UNER_Transport *)` | Conecta/inicializa transporte serial de UNER desde la composición. |
| `uint8_t (*connected_count)()` | Consulta clientes conectados según servicio de servidor, separado de estaciones AP. |
| `bool (*client_info)(uint8_t, uint32_t *, uint8_t *)` | Entrega información del cliente identificado para reportes. |
| `void (*request_credentials)(const char *)` | Inicia petición de credenciales al flujo dueño; no devuelve una contraseña por inferencia. |
| `uint8_t (*credentials_state)(uint8_t *, char *, size_t, char *, size_t)` | Consulta fase/resultado de la petición de credenciales. |
| `uint8_t (*submit_credentials)(const char *, const char *, bool, const char *, const char *, const char *, const char *)` | Entrega datos recibidos al flujo de credenciales con el contexto registrado. |
| `uint8_t (*cancel_credentials)(const char *)` | Cancela petición de credenciales pendiente y su contexto de aplicación. |
| `uint8_t (*ap_clients_page)(uint8_t, uint8_t, uint8_t *, uint8_t, uint8_t *)` | Codifica/consulta página de estaciones AP según argumentos de la operación. |
| `uint8_t (*disconnect_ap_client)(const uint8_t *)` | Solicita desconexión de estación AP; no confundir con cerrar una sesión de autenticación F4. |

### `WebServerServices`

`esp:src/webserver_ports.h:6`

| Atributo / firma | Propósito |
| --- | --- |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool (*storage_ready)()` | Consulta disponibilidad de filesystem antes de servir recursos. |
| `void (*bridge_initialize)(void *, AsyncWebSocket *)` | Inicializa bridge con servicios registrados antes de recibir solicitudes. |
| `bool (*bridge_connect)(AsyncWebSocketClient *, const char *)` | Informa al bridge apertura del cliente para crear sesión/generación. |
| `bool (*bridge_disconnect)(uint32_t)` | Informa al bridge cierre del cliente para limpiar pendientes, demanda y autoridad. |
| `void (*bridge_data)(AsyncWebSocketClient *, AwsFrameInfo *, uint8_t *, size_t)` | Entrega bytes/mensaje recibido al bridge; copiar lo que deba sobrevivir al callback del servidor. |
| `void (*bridge_loop)()` | Presta el turno periódico de ejecución al bridge para deadlines/colas. |
| `void (*notify_connected)(void *, uint32_t)` | Propaga evento de cliente Web conectado al resto del sistema. |
| `void (*notify_disconnected)(void *, uint32_t)` | Propaga evento de cliente Web desconectado después de gestionar su identidad. |
| `void (*notify_page_ready)(void *, uint32_t, uint8_t)` | Propaga que una página está lista; distinto de tener socket o radio asociados. |
| `void (*notify_page_closed)(void *, uint32_t, uint8_t)` | Propaga cierre de página y su ciclo de vida. |
| `void (*notify_ap_join)(void *, uint8_t, uint8_t)` | Propaga asociación de estación al AP; no implica login Web. |
| `void (*notify_ap_leave)(void *, uint8_t, uint8_t)` | Propaga salida de estación del AP; no inventar un resultado de comando F4. |
| `void (*notify_server_up)(void *)` | Informa servidor disponible para sincronizar estado con el peer. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-applyapconfig"></a>`bool ApplyAPConfig(const AppConfig& cfg) const` | Aplica APConfig dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:43`; inferred-name |
| <a id="fn-applystaconfig"></a>`bool ApplySTAConfig(const AppConfig& cfg) const` | Aplica STAConfig dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:42`; inferred-name |
| <a id="fn-beginsessionmode"></a>`bool BeginSessionMode(const AppConfig &config, WifiRunMode mode) const` | Coordina `BeginSessionMode` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:35`; structural |
| <a id="fn-bridgeplatform_esp"></a>`const BridgePlatformOps &BridgePlatform_ESP()` | Coordina `RSSI`, `SSID`, `STAILQ_NEXT` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_platform_esp.cpp:11`; structural |
| <a id="fn-bridge_defaultbackend"></a>`const BridgeBackendOps &Bridge_DefaultBackend()` | Carga los valores predeterminados de Backend dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:100`; inferred-name |
| <a id="fn-bridge_defaultwifi"></a>`const BridgeWifiPort &Bridge_DefaultWifi()` | Carga los valores predeterminados de Wifi dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:101`; inferred-name |
| <a id="fn-connectsta"></a>`WifiLinkStatus ConnectSTA(const char* ssid, const char* password, bool keep_ap, bool disconnect_first, const uint8_t* bssid) const` | Coordina `ConnectSTA` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:40`; structural |
| <a id="fn-consumeapfallbackready"></a>`bool ConsumeAPFallbackReady() const` | Coordina `ConsumeAPFallbackReady` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:51`; structural |
| <a id="fn-consumestaattemptfailure"></a>`bool ConsumeSTAAttemptFailure() const` | Coordina `ConsumeSTAAttemptFailure` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:52`; structural |
| <a id="fn-deletescanresults"></a>`void DeleteScanResults() const` | Coordina `DeleteScanResults` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:38`; structural |
| <a id="fn-disconnectsta"></a>`bool DisconnectSTA(bool radio_off, bool erase_sdk_credentials) const` | Coordina `DisconnectSTA` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:41`; structural |
| <a id="fn-espplatform_delay0"></a>`void EspPlatform_Delay0(void *)` | Adapta la llamada a `delay` con el contexto de esta integración. | `esp:src/esp_platform.cpp:12`; structural |
| <a id="fn-espplatform_ledinit"></a>`void EspPlatform_LedInit(void *)` | Coordina `digitalWrite`, `pinMode` para Puertos y adaptadores de plataforma ESP. | `esp:src/esp_platform.cpp:6`; structural |
| <a id="fn-espplatform_ledwrite"></a>`void EspPlatform_LedWrite(void *, bool on)` | Adapta la llamada a `digitalWrite` con el contexto de esta integración. | `esp:src/esp_platform.cpp:10`; structural |
| <a id="fn-espplatform_logunerpacket"></a>`void EspPlatform_LogUNERPacket(void *context, const UNER_Packet *packet)` | Operación Log UNERPacket de Puertos y adaptadores de plataforma ESP; semántica fina pendiente de ampliación. | `esp:src/uner_platform_esp.h:4`; inferred-name |
| <a id="fn-espplatform_millis"></a>`uint32_t EspPlatform_Millis(void *)` | Adapta la llamada a `millis` con el contexto de esta integración. | `esp:src/esp_platform.cpp:4`; structural |
| <a id="fn-espplatform_yield"></a>`void EspPlatform_Yield(void *)` | Adapta la llamada a `yield` con el contexto de esta integración. | `esp:src/esp_platform.cpp:5`; structural |
| <a id="fn-getap_ip"></a>`WifiIpv4 GetAP_IP() const` | Consulta AP IP dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:49`; inferred-name |
| <a id="fn-getmode"></a>`WifiRadioMode GetMode() const` | Consulta Mode dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:50`; inferred-name |
| <a id="fn-getstate"></a>`WifiServiceState GetState() const` | Consulta State dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:48`; inferred-name |
| <a id="fn-isstareconnectenabled"></a>`bool IsSTAReconnectEnabled() const` | Indica si se cumple la condición STAReconnect Enabled dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:47`; inferred-name |
| <a id="fn-notifystaconfigurationfailure"></a>`void NotifySTAConfigurationFailure() const` | Notifica STAConfiguration Failure dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:53`; inferred-name |
| <a id="fn-restoremodeafterscan"></a>`void RestoreModeAfterScan() const` | Coordina `RestoreModeAfterScan` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:39`; structural |
| <a id="fn-scancomplete"></a>`int ScanComplete() const` | Coordina `ScanComplete` para Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:37`; structural |
| <a id="fn-setstareconnectenabled"></a>`void SetSTAReconnectEnabled(bool enabled) const` | Configura STAReconnect Enabled dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:46`; inferred-name |
| <a id="fn-setsessionconfig"></a>`void SetSessionConfig(const AppConfig *config) const` | Configura Session Config dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:34`; inferred-name |
| <a id="fn-startap"></a>`bool StartAP(const char* ssid, const char* password, bool keep_sta, uint8_t channel, bool hidden, uint8_t max_clients, bool ap_protected) const` | Inicia AP dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:44`; inferred-name |
| <a id="fn-startscan"></a>`int StartScan(bool async, bool show_hidden) const` | Inicia Scan dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:36`; inferred-name |
| <a id="fn-stopap"></a>`bool StopAP(bool radio_off) const` | Detiene AP dentro de Puertos y adaptadores de plataforma ESP. | `esp:src/bridge_ports.h:45`; inferred-name |
| <a id="fn-uner_app_defaultservices"></a>`const UNER_AppServices &UNER_App_DefaultServices()` | Operación UNER App Default Services de Puertos y adaptadores de plataforma ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_ports.h:30`; inferred-name |
| <a id="fn-uner_app_setservices"></a>`void UNER_App_SetServices(const UNER_AppServices &services)` | Operación UNER App Set Services de Puertos y adaptadores de plataforma ESP; semántica fina pendiente de ampliación. | `esp:src/uner_app_ports.h:29`; inferred-name |
| <a id="fn-webserverws_setservices"></a>`void WebServerWS_SetServices(const WebServerServices &services)` | Operación Web Server WS Set Services de Puertos y adaptadores de plataforma ESP; semántica fina pendiente de ampliación. | `esp:src/webserver_ports.h:22`; inferred-name |
| <a id="fn-webserver_defaultservices"></a>`const WebServerServices &WebServer_DefaultServices()` | Operación Web Server Default Services de Puertos y adaptadores de plataforma ESP; semántica fina pendiente de ampliación. | `esp:src/webserver_ports.h:23`; inferred-name |
| <a id="fn-wifi_softap_deauth"></a>`bool wifi_softap_deauth(uint8 *mac)` | Operación wifi softap deauth de Puertos y adaptadores de plataforma ESP; semántica fina pendiente de ampliación. | `esp:src/bridge_platform_esp.cpp:7`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-neutralip"></a>`static WifiIpv4 neutralIP(IPAddress ip)` | Adapta la llamada a `WifiIpv4` con el contexto de esta integración. | `esp:src/bridge_platform_esp.cpp:10`; structural |
