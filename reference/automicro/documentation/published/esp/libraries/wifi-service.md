# Servicio de radio Wi-Fi

ID: `esp:module/src/wifi_service.h` · Capa: **servicio** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Centraliza mutaciones de radio y máquina de estado STA/AP/reconexión/scan.

Config IP, SoftAP, conexión STA, fallback y restauración tras scan.

<a id="modularity"></a>
## Modularidad observada

Recibe WifiDriverPort; config/modelos compartidos conservan conocimiento ESP. El acceso global WiFi debe quedar en adaptador, no duplicarse en consumidores.

<a id="state"></a>
## Handle, estado y atributos externos

WifiServiceHandle posee driver/servicios, flags/timers prestados y estado de intento/red. ap_started, keep_ap_with_sta y scan_previous_mode distinguen radio configurada y transición pendiente.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Init conecta puertos/runtime; Begin inicia el modo elegido.
- Loop procesa conexión, timeout, reintento y señales de fallback.
- StartScan conserva modo previo y RestoreModeAfterScan aplica la política de restauración.

<a id="interactions"></a>
## Interacciones

- WifiUtils ofrece fachada histórica; bridge y handlers usan el mismo servicio de radio.

<a id="limitations"></a>
## Precauciones y límites

- AP iniciado no implica STA conectada ni cliente Web autorizado.
- Los puertos requieren operaciones válidas; algunos reenvíos no comprueban null.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/wifi_driver_port.h` | c-cpp-structural | `52f3abad04f4f5c611481592b4211d16aeb1c98243d63642b0f594cec814d222` |
| `src/wifi_service.cpp` | c-cpp-structural | `2ccd38929bd7b5aa9028a2f5f0ce45638c163b45b6c623882da77d087518ef84` |
| `src/wifi_service.h` | c-cpp-structural | `70e2a9236666cc798ea7e20e56952c5ea19f3dcada33716f74e8af0f8d2605a5` |
| `src/wifi_service_types.h` | c-cpp-structural | `ecc78d952db07cf1205f1bf862ff6ce32ce7f32ab03230162d2fb59110ea3884` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WifiDriverOps`

`esp:src/wifi_driver_port.h:4`

| Atributo / firma | Propósito |
| --- | --- |
| `void (*initialize)(void *)` | Inicializa backend radio desde WifiService; debe registrarse antes de Begin/Loop. |
| `bool (*set_mode)(void *, WifiRadioMode)` | Aplica modo SDK elegido por la política del servicio. |
| `WifiRadioMode (*get_mode)(void *)` | Consulta modo SDK para transición/restauración. |
| `WifiLinkStatus (*status)(void *)` | Consulta estado de conexión STA del driver. |
| `WifiLinkStatus (*connect)(void *, const char *, const char *, const uint8_t *)` | Inicia conexión STA con configuración prestada por el servicio. |
| `bool (*disconnect)(void *, bool, bool)` | Desconecta estación a petición de la política del servicio. |
| `bool (*reconnect)(void *)` | Solicita reintento de la conexión existente. |
| `bool (*start_ap)(void *, const char *, const char *, uint8_t, bool, uint8_t)` | Inicia SoftAP en el backend. |
| `bool (*stop_ap)(void *, bool)` | Detiene SoftAP en el backend. |
| `bool (*configure_sta)(void *, WifiIpv4, WifiIpv4, WifiIpv4)` | Aplica parámetros de red de estación antes de conectar. |
| `bool (*configure_ap)(void *, WifiIpv4, WifiIpv4, WifiIpv4)` | Aplica parámetros de red del AP antes de iniciarlo. |
| `void (*auto_reconnect)(void *, bool)` | Configura reconexión automática del SDK según política central. |
| `int (*scan_start)(void *, bool, bool)` | Inicia exploración del driver; su resultado no es inmediato por implicación. |
| `int (*scan_complete)(void *)` | Consulta resultado/avance de scan asíncrono conforme al retorno del SDK. |
| `void (*scan_delete)(void *)` | Libera resultados scan del driver después de su consumo. |
| `WifiIpv4 (*sta_ip)(void *)` | Obtiene dirección de estación del backend. |
| `WifiIpv4 (*ap_ip)(void *)` | Obtiene dirección de AP del backend. |
| `uint8_t (*ap_clients)(void *)` | Consulta estaciones asociadas al AP para estado, no sesiones Web. |

### `WifiDriverPort`

`esp:src/wifi_driver_port.h:24`

| Atributo / firma | Propósito |
| --- | --- |
| `const WifiDriverOps *ops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiServiceOps`

`esp:src/wifi_service.h:6`

| Atributo / firma | Propósito |
| --- | --- |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t (*ticks)(void *)` | Entrega reloj en ms para timeouts del servicio desde la aplicación. |
| `const AppConfig *(*config)(void *)` | Presta configuración del sistema durante uso del servicio; no transferir ni guardar puntero sin lifetime válido. |
| `void (*cooperate)(void *)` | Permite ceder ejecución al runtime en puntos del servicio que lo requieren. |

### `WifiServiceHandle`

`esp:src/wifi_service.h:12`

| Atributo / firma | Propósito |
| --- | --- |
| `WifiDriverPort driver` | Dato de estado/configuración `driver`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiServiceOps services` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `volatile t_bitMap *flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `volatile uint16_t *sta_wait` | Dato de estado/configuración `sta_wait`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t *sta_retry` | Dato de estado/configuración `sta_retry`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sta_start_tick` | Dato de estado/configuración `sta_start_tick`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t last_retry_tick` | Memoria de la observación anterior usada por la máquina de estados. |
| `WifiRunMode mode` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiServiceState state` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool ap_started` | Dato de estado/configuración `ap_started`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool keep_ap_with_sta` | Dato de estado/configuración `keep_ap_with_sta`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool sta_reconnect_enabled` | Dato de estado/configuración `sta_reconnect_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WifiRadioMode scan_previous_mode` | Dato de estado/configuración `scan_previous_mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool scan_mode_restore_pending` | Dato de estado/configuración `scan_mode_restore_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool scan_keep_ap_hybrid` | Dato de estado/configuración `scan_keep_ap_hybrid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool ap_fallback_ready_pending` | Dato de estado/configuración `ap_fallback_ready_pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool sta_attempt_failed_pending` | Dato de estado/configuración `sta_attempt_failed_pending`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiIpv4`

`esp:src/wifi_service_types.h:23`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t octets[4]` | Dato de estado/configuración `octets`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-wifiservice_applyapconfig"></a>`bool WifiService_ApplyAPConfig(WifiServiceHandle &h, const AppConfig &cfg)` | Aplica APConfig dentro de Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:475`; inferred-name |
| <a id="fn-wifiservice_applystaconfig"></a>`bool WifiService_ApplySTAConfig(WifiServiceHandle &h, const AppConfig &cfg)` | Aplica STAConfig dentro de Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:455`; inferred-name |
| <a id="fn-wifiservice_begin"></a>`bool WifiService_Begin(WifiServiceHandle &h, const AppConfig &cfg, WifiRunMode mode)` | Inicia AP/STA según configuración y modo solicitados. | `esp:src/wifi_service.cpp:193`; editorial |
| <a id="fn-wifiservice_configureap"></a>`bool WifiService_ConfigureAP(WifiServiceHandle &h, WifiIpv4 local_ip, WifiIpv4 gateway, WifiIpv4 subnet)` | Adapta la llamada a `softAPConfig` con el contexto de esta integración. | `esp:src/wifi_service.cpp:450`; structural |
| <a id="fn-wifiservice_configuresta"></a>`bool WifiService_ConfigureSTA(WifiServiceHandle &h, WifiIpv4 local_ip, WifiIpv4 gateway, WifiIpv4 subnet)` | Adapta la llamada a `config` con el contexto de esta integración. | `esp:src/wifi_service.cpp:445`; structural |
| <a id="fn-wifiservice_connectsta"></a>`WifiLinkStatus WifiService_ConnectSTA(WifiServiceHandle &h, const char *ssid, const char *password, bool keep_ap, bool disconnect_first, const uint8_t *bssid)` | Coordina `config`, `startStaAttempt` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:411`; structural |
| <a id="fn-wifiservice_consumeapfallbackready"></a>`bool WifiService_ConsumeAPFallbackReady(WifiServiceHandle &h)` | Consume la señal de transición a fallback AP disponible. | `esp:src/wifi_service.cpp:532`; editorial |
| <a id="fn-wifiservice_consumestaattemptfailure"></a>`bool WifiService_ConsumeSTAAttemptFailure(WifiServiceHandle &h)` | Operación Consume STAAttempt Failure de Servicio de radio Wi-Fi; semántica fina pendiente de ampliación. | `esp:src/wifi_service.cpp:539`; inferred-name |
| <a id="fn-wifiservice_deletescanresults"></a>`void WifiService_DeleteScanResults(WifiServiceHandle &h)` | Adapta la llamada a `scanDelete` con el contexto de esta integración. | `esp:src/wifi_service.cpp:387`; structural |
| <a id="fn-wifiservice_disconnectsta"></a>`bool WifiService_DisconnectSTA(WifiServiceHandle &h, bool radio_off, bool erase_sdk_credentials)` | Coordina `WifiService_SetSTAReconnectEnabled`, `disconnect`, `getMode` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:421`; structural |
| <a id="fn-wifiservice_getapclientcount"></a>`uint8_t WifiService_GetAPClientCount(WifiServiceHandle &h)` | Adapta la llamada a `softAPgetStationNum` con el contexto de esta integración. | `esp:src/wifi_service.cpp:530`; structural |
| <a id="fn-wifiservice_getap_ip"></a>`WifiIpv4 WifiService_GetAP_IP(WifiServiceHandle &h)` | Adapta la llamada a `softAPIP` con el contexto de esta integración. | `esp:src/wifi_service.cpp:526`; structural |
| <a id="fn-wifiservice_getmode"></a>`WifiRadioMode WifiService_GetMode(WifiServiceHandle &h)` | Adapta la llamada a `getMode` con el contexto de esta integración. | `esp:src/wifi_service.cpp:528`; structural |
| <a id="fn-wifiservice_getstastatus"></a>`WifiLinkStatus WifiService_GetSTAStatus(WifiServiceHandle &h)` | Adapta la llamada a `status` con el contexto de esta integración. | `esp:src/wifi_service.cpp:529`; structural |
| <a id="fn-wifiservice_getsta_ip"></a>`WifiIpv4 WifiService_GetSTA_IP(WifiServiceHandle &h)` | Adapta la llamada a `localIP` con el contexto de esta integración. | `esp:src/wifi_service.cpp:525`; structural |
| <a id="fn-wifiservice_getstate"></a>`WifiServiceState WifiService_GetState(WifiServiceHandle &h)` | Devuelve el campo `h.state` del estado actual de la instancia. | `esp:src/wifi_service.cpp:524`; structural |
| <a id="fn-wifiservice_init"></a>`void WifiService_Init(WifiServiceHandle &h, const WifiDriverPort &driver, const WifiServiceOps &services, volatile t_bitMap *flags, volatile uint16_t *sta_wait, volatile uint16_t *sta_retry)` | Conecta driver, reloj/config y referencias runtime del servicio. | `esp:src/wifi_service.cpp:7`; editorial |
| <a id="fn-wifiservice_isstareconnectenabled"></a>`bool WifiService_IsSTAReconnectEnabled(WifiServiceHandle &h)` | Devuelve el campo `h.sta_reconnect_enabled` del estado actual de la instancia. | `esp:src/wifi_service.cpp:519`; structural |
| <a id="fn-wifiservice_loop"></a>`void WifiService_Loop(WifiServiceHandle &h)` | Avanza intentos STA, reconexión y fallback. | `esp:src/wifi_service.cpp:234`; editorial |
| <a id="fn-wifiservice_notifystaconfigurationfailure"></a>`void WifiService_NotifySTAConfigurationFailure(WifiServiceHandle &h)` | Notifica STAConfiguration Failure dentro de Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:126`; inferred-name |
| <a id="fn-wifiservice_reconnectsta"></a>`bool WifiService_ReconnectSTA(WifiServiceHandle &h)` | Coordina `WifiService_ApplySTAConfig`, `config`, `startStaAttempt` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:432`; structural |
| <a id="fn-wifiservice_restoremodeafterscan"></a>`void WifiService_RestoreModeAfterScan(WifiServiceHandle &h)` | Restaura modo de radio según el estado conservado. | `esp:src/wifi_service.cpp:392`; editorial |
| <a id="fn-wifiservice_sta_connected"></a>`bool WifiService_STA_Connected(WifiServiceHandle &h)` | Adapta la llamada a `status` con el contexto de esta integración. | `esp:src/wifi_service.cpp:527`; structural |
| <a id="fn-wifiservice_scancomplete"></a>`int WifiService_ScanComplete(WifiServiceHandle &h)` | Adapta la llamada a `scanComplete` con el contexto de esta integración. | `esp:src/wifi_service.cpp:382`; structural |
| <a id="fn-wifiservice_setstareconnectenabled"></a>`void WifiService_SetSTAReconnectEnabled(WifiServiceHandle &h, bool enabled)` | Adapta la llamada a `setAutoReconnect` con el contexto de esta integración. | `esp:src/wifi_service.cpp:513`; structural |
| <a id="fn-wifiservice_startap"></a>`bool WifiService_StartAP(WifiServiceHandle &h, const char *ssid, const char *password, bool keep_sta, uint8_t channel, bool hidden, uint8_t max_clients, bool ap_protected)` | Adapta la llamada a `startAPInternal` con el contexto de esta integración. | `esp:src/wifi_service.cpp:486`; structural |
| <a id="fn-wifiservice_startscan"></a>`int WifiService_StartScan(WifiServiceHandle &h, bool async, bool show_hidden)` | Solicita scan al driver conservando contexto de radio. | `esp:src/wifi_service.cpp:364`; editorial |
| <a id="fn-wifiservice_stopap"></a>`bool WifiService_StopAP(WifiServiceHandle &h, bool radio_off)` | Detiene AP dentro de Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:496`; inferred-name |
| <a id="fn-begin"></a>`WifiLinkStatus begin(const char *s, const char *p, int, const uint8_t *bssid, bool) const` | Adapta la llamada a `connect` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:31`; structural |
| <a id="fn-config"></a>`bool config(WifiIpv4 ip,WifiIpv4 gw,WifiIpv4 mask) const` | Adapta la llamada a `configure_sta` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:41`; structural |
| <a id="fn-disconnect"></a>`bool disconnect(bool off, bool erase) const` | Coordina `disconnect` para Servicio de radio Wi-Fi. | `esp:src/wifi_driver_port.h:35`; structural |
| <a id="fn-getmode"></a>`WifiRadioMode getMode() const` | Adapta la llamada a `get_mode` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:29`; structural |
| <a id="fn-initialize"></a>`void initialize() const` | Coordina `initialize` para Servicio de radio Wi-Fi. | `esp:src/wifi_driver_port.h:27`; structural |
| <a id="fn-localip"></a>`WifiIpv4 localIP() const` | Adapta la llamada a `sta_ip` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:51`; structural |
| <a id="fn-mode"></a>`bool mode(WifiRadioMode m) const` | Adapta la llamada a `set_mode` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:28`; structural |
| <a id="fn-reconnect"></a>`bool reconnect() const` | Coordina `reconnect` para Servicio de radio Wi-Fi. | `esp:src/wifi_driver_port.h:36`; structural |
| <a id="fn-scancomplete"></a>`int scanComplete() const` | Adapta la llamada a `scan_complete` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:49`; structural |
| <a id="fn-scandelete"></a>`void scanDelete() const` | Adapta la llamada a `scan_delete` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:50`; structural |
| <a id="fn-scannetworks"></a>`int scanNetworks(bool async,bool hidden) const` | Adapta la llamada a `scan_start` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:48`; structural |
| <a id="fn-setautoreconnect"></a>`void setAutoReconnect(bool on) const` | Adapta la llamada a `auto_reconnect` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:47`; structural |
| <a id="fn-softap"></a>`bool softAP(const char *s,const char *p,uint8_t ch,bool hidden,uint8_t max) const` | Adapta la llamada a `start_ap` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:37`; structural |
| <a id="fn-softapconfig"></a>`bool softAPConfig(WifiIpv4 ip,WifiIpv4 gw,WifiIpv4 mask) const` | Adapta la llamada a `configure_ap` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:44`; structural |
| <a id="fn-softapip"></a>`WifiIpv4 softAPIP() const` | Adapta la llamada a `ap_ip` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:52`; structural |
| <a id="fn-softapdisconnect"></a>`bool softAPdisconnect(bool off) const` | Adapta la llamada a `stop_ap` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:40`; structural |
| <a id="fn-softapgetstationnum"></a>`uint8_t softAPgetStationNum() const` | Adapta la llamada a `ap_clients` con el contexto de esta integración. | `esp:src/wifi_driver_port.h:53`; structural |
| <a id="fn-status"></a>`WifiLinkStatus status() const` | Coordina `status` para Servicio de radio Wi-Fi. | `esp:src/wifi_driver_port.h:30`; structural |
| <a id="fn-uint32_t"></a>`explicit operator uint32_t() const` | Operación uint32 t de Servicio de radio Wi-Fi; semántica fina pendiente de ampliación. | `esp:src/wifi_service_types.h:27`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-armstaattempt"></a>`static void armStaAttempt(WifiServiceHandle &h)` | Coordina `CLEAR_FLAG`, `SET_FLAG`, `ticks` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:83`; structural |
| <a id="fn-boundedlength"></a>`static size_t boundedLength(const char *value, size_t max_length)` | Operación bounded Length de Servicio de radio Wi-Fi; semántica fina pendiente de ampliación. | `esp:src/wifi_service.cpp:36`; inferred-name |
| <a id="fn-enterapfallbackstate"></a>`static void enterApFallbackState(WifiServiceHandle &h)` | Coordina `CLEAR_FLAG`, `SET_FLAG`, `config` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:98`; structural |
| <a id="fn-modehasap"></a>`static bool modeHasAP(WifiRadioMode mode)` | Operación mode Has AP de Servicio de radio Wi-Fi; semántica fina pendiente de ampliación. | `esp:src/wifi_service.cpp:46`; inferred-name |
| <a id="fn-parseipv4text"></a>`static bool parseIpv4Text(const char *text, WifiIpv4 &out)` | Coordina `WifiIpv4`, `sscanf` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:18`; structural |
| <a id="fn-setaprunning"></a>`static void setApRunning(WifiServiceHandle &h, bool running)` | Coordina `CLEAR_FLAG`, `SET_FLAG` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:51`; structural |
| <a id="fn-setstaconnectedflags"></a>`static void setStaConnectedFlags(WifiServiceHandle &h)` | Coordina `CLEAR_FLAG`, `SET_FLAG` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:71`; structural |
| <a id="fn-setstaidleflags"></a>`static void setStaIdleFlags(WifiServiceHandle &h)` | Adapta la llamada a `CLEAR_FLAG` con el contexto de esta integración. | `esp:src/wifi_service.cpp:61`; structural |
| <a id="fn-startapinternal"></a>`static bool startAPInternal(WifiServiceHandle &h, const char *ssid, const char *password, bool enable_sta_interface, uint8_t channel, bool hidden, uint8_t max_clients, bool ap_protected)` | Coordina `boundedLength`, `mode`, `setApRunning` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:158`; structural |
| <a id="fn-startstaattempt"></a>`static WifiLinkStatus startStaAttempt(WifiServiceHandle &h, const char *ssid, const char *password, bool keep_ap, bool disconnect_first, const uint8_t *bssid)` | Coordina `WifiService_NotifySTAConfigurationFailure`, `armStaAttempt`, `begin` para Servicio de radio Wi-Fi. | `esp:src/wifi_service.cpp:136`; structural |
