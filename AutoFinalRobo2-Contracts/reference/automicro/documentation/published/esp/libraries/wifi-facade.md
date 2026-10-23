# Fachada Wi-Fi de aplicación

ID: `esp:module/src/wifi_utils.h` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Conecta el servicio Wi-Fi con el runtime Arduino y conserva su API de aplicación.

Bindings del driver/config/reloj y reenvíos a WifiService.

<a id="modularity"></a>
## Modularidad observada

Fachada específica ESP; no duplicar su estado dentro del bridge.

<a id="state"></a>
## Handle, estado y atributos externos

Instancia del servicio y configuración de sesión; distinguir configuración temporal de NVS persistida.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Begin prepara servicio en setup; Loop lo atiende.
- Cambios temporales pueden aplicar configuración de sesión sin escribir NVS.

<a id="interactions"></a>
## Interacciones

- BridgeWifiPort y UNER_AppServices delegan operaciones aquí.

<a id="limitations"></a>
## Precauciones y límites

- Una configuración en RAM no sobrevive a reinicio salvo guardado explícito.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/wifi_utils.cpp` | c-cpp-structural | `ce09ac8669585eb45ecec245cddd2d8f9d8171e2b94e06b819605a1a67575a44` |
| `src/wifi_utils.h` | c-cpp-structural | `d6ff57046966362c4f93f62a53ea6d3932e8539bf865886352a2a35e19a4ff5e` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-wifiutils_applyapconfig"></a>`bool WifiUtils_ApplyAPConfig(const AppConfig &c)` | Aplica APConfig dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:75`; inferred-name |
| <a id="fn-wifiutils_applystaconfig"></a>`bool WifiUtils_ApplySTAConfig(const AppConfig &c)` | Aplica STAConfig dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:74`; inferred-name |
| <a id="fn-wifiutils_begin"></a>`bool WifiUtils_Begin(const AppConfig &c,WifiRunMode m)` | Coordina `WifiService_Begin`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:61`; structural |
| <a id="fn-wifiutils_configureap"></a>`bool WifiUtils_ConfigureAP(IPAddress ip,IPAddress gw,IPAddress mask)` | Coordina `WifiService_ConfigureAP`, `fromIP`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:73`; structural |
| <a id="fn-wifiutils_configuresta"></a>`bool WifiUtils_ConfigureSTA(IPAddress ip,IPAddress gw,IPAddress mask)` | Coordina `WifiService_ConfigureSTA`, `fromIP`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:72`; structural |
| <a id="fn-wifiutils_connectsta"></a>`wl_status_t WifiUtils_ConnectSTA(const char *s,const char *p,bool ap,bool disc,const uint8_t *b)` | Coordina `WifiService_ConnectSTA`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:67`; structural |
| <a id="fn-wifiutils_consumeapfallbackready"></a>`bool WifiUtils_ConsumeAPFallbackReady()` | Coordina `WifiService_ConsumeAPFallbackReady`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:89`; structural |
| <a id="fn-wifiutils_consumestaattemptfailure"></a>`bool WifiUtils_ConsumeSTAAttemptFailure()` | Coordina `WifiService_ConsumeSTAAttemptFailure`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:91`; structural |
| <a id="fn-wifiutils_deletescanresults"></a>`void WifiUtils_DeleteScanResults()` | Coordina `WifiService_DeleteScanResults`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:65`; structural |
| <a id="fn-wifiutils_disconnectsta"></a>`bool WifiUtils_DisconnectSTA(bool off,bool erase)` | Coordina `WifiService_DisconnectSTA`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:70`; structural |
| <a id="fn-wifiutils_getapclientcount"></a>`uint8_t WifiUtils_GetAPClientCount()` | Consulta APClient Count dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:88`; inferred-name |
| <a id="fn-wifiutils_getap_ip"></a>`IPAddress WifiUtils_GetAP_IP()` | Consulta AP IP dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:84`; inferred-name |
| <a id="fn-wifiutils_getmode"></a>`WiFiMode_t WifiUtils_GetMode()` | Consulta Mode dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:86`; inferred-name |
| <a id="fn-wifiutils_getruntimeconfig"></a>`const AppConfig *WifiUtils_GetRuntimeConfig()` | Adapta la llamada a `NVS_GetCached` con el contexto de esta integración. | `esp:src/wifi_utils.cpp:45`; structural |
| <a id="fn-wifiutils_getstastatus"></a>`wl_status_t WifiUtils_GetSTAStatus()` | Consulta STAStatus dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:87`; inferred-name |
| <a id="fn-wifiutils_getsta_ip"></a>`IPAddress WifiUtils_GetSTA_IP()` | Consulta STA IP dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:83`; inferred-name |
| <a id="fn-wifiutils_getstate"></a>`WifiServiceState WifiUtils_GetState()` | Consulta State dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:82`; inferred-name |
| <a id="fn-wifiutils_isstareconnectenabled"></a>`bool WifiUtils_IsSTAReconnectEnabled()` | Indica si se cumple la condición STAReconnect Enabled dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:81`; inferred-name |
| <a id="fn-wifiutils_loop"></a>`void WifiUtils_Loop()` | Coordina `WifiService_Loop`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:62`; structural |
| <a id="fn-wifiutils_notifystaconfigurationfailure"></a>`void WifiUtils_NotifySTAConfigurationFailure()` | Notifica STAConfiguration Failure dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:93`; inferred-name |
| <a id="fn-wifiutils_reconnectsta"></a>`bool WifiUtils_ReconnectSTA()` | Coordina `WifiService_ReconnectSTA`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:71`; structural |
| <a id="fn-wifiutils_restoremodeafterscan"></a>`void WifiUtils_RestoreModeAfterScan()` | Coordina `WifiService_RestoreModeAfterScan`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:66`; structural |
| <a id="fn-wifiutils_sta_connected"></a>`bool WifiUtils_STA_Connected()` | Coordina `WifiService_STA_Connected`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:85`; structural |
| <a id="fn-wifiutils_scancomplete"></a>`int WifiUtils_ScanComplete()` | Coordina `WifiService_ScanComplete`, `service` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:64`; structural |
| <a id="fn-wifiutils_setstareconnectenabled"></a>`void WifiUtils_SetSTAReconnectEnabled(bool on)` | Configura STAReconnect Enabled dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:80`; inferred-name |
| <a id="fn-wifiutils_setsessionconfig"></a>`void WifiUtils_SetSessionConfig(const AppConfig *config)` | Configura Session Config dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:46`; inferred-name |
| <a id="fn-wifiutils_startap"></a>`bool WifiUtils_StartAP(const char *s,const char *p,bool sta,uint8_t ch,bool hidden,uint8_t max,bool protect)` | Inicia AP dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:76`; inferred-name |
| <a id="fn-wifiutils_startscan"></a>`int WifiUtils_StartScan(bool async,bool hidden)` | Inicia Scan dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:63`; inferred-name |
| <a id="fn-wifiutils_stopap"></a>`bool WifiUtils_StopAP(bool off)` | Detiene AP dentro de Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:79`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-driverops"></a>`static const WifiDriverOps &driverOps()` | Coordina `begin`, `config`, `disconnect` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:10`; structural |
| <a id="fn-fromip"></a>`static WifiIpv4 fromIP(IPAddress ip)` | Adapta la llamada a `WifiIpv4` con el contexto de esta integración. | `esp:src/wifi_utils.cpp:8`; structural |
| <a id="fn-service"></a>`static WifiServiceHandle &service()` | Coordina `Timebase_Ticks`, `WifiService_Init`, `WifiUtils_GetRuntimeConfig` para Fachada Wi-Fi de aplicación. | `esp:src/wifi_utils.cpp:48`; structural |
| <a id="fn-toip"></a>`static IPAddress toIP(WifiIpv4 ip)` | Adapta la llamada a `IPAddress` con el contexto de esta integración. | `esp:src/wifi_utils.cpp:9`; structural |
