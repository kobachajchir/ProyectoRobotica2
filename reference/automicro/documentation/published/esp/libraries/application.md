# Composición y ciclo principal ESP

ID: `esp:module/src/main.cpp` · Capa: **composición** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Construye servicios y adapta Arduino/ESP8266 al servidor, Wi-Fi y protocolo del robot.

setup/loop, referencias de sistema y conexión de tablas; no concentra buffers privados de cada librería.

<a id="modularity"></a>
## Modularidad observada

Capa de aplicación ligada a Arduino y servicios concretos. Las librerías reciben puertos; el sistema no se convierte en una dependencia universal neutral.

<a id="state"></a>
## Handle, estado y atributos externos

g_unerApp pertenece a la composición. EspSystemHandle conserva referencias a flags/timers y referencias opacas a UNER/bridge; no duplica el almacenamiento de los globals existentes.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- setup inicia Serial, timebase, NVS, LED y Wi-Fi; después sensores, bindings, UNER y servidor.
- loop actualiza tiempo, Wi-Fi, LED, servidor y UNER, consume tick y cede con delay(0).

<a id="interactions"></a>
## Interacciones

- AppBindings_Configure conecta servicios antes de UNER_App_Init.
- El modo de boot persistido elige AP-only o AP+STA; arrancar no debe volver a persistir/reiniciar por esa sola selección.

<a id="limitations"></a>
## Precauciones y límites

- El orden observado no demuestra latencia máxima ni ausencia de esperas.
- Código de firmware F4, servidor ESP y bundle Web se publican/cargan por separado.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/app_bindings.cpp` | c-cpp-structural | `30fc5d6cf8cc664de420fa894097cdc968c78c67bfbb55b9bcccfde6c6705e70` |
| `src/app_bindings.h` | c-cpp-structural | `6f279f0e724460ebf5413ee1976125075788c5a653d1a35d883bca3d66c4c74e` |
| `src/esp_system.cpp` | c-cpp-structural | `899d3abdee587159db2ad5c9001f4d5e11461111b078499302907c7f303bc26e` |
| `src/esp_system.h` | c-cpp-structural | `7e1beff1518e10bd59ecfc0c0dba36a0330b8f8cf9aef35033994316fb9791ad` |
| `src/globals.h` | c-cpp-structural | `8a9d87a6cce292b8d793c4a1e52d495cee0810e9451a0b0125338320d3582f6d` |
| `src/main.cpp` | c-cpp-structural | `3e23d3ef2309cb966bcc5af68d8b3608ba700eaf2a098d394574029acff01f0b` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `EspSystemRuntime`

`esp:src/esp_system.h:8`

| Atributo / firma | Propósito |
| --- | --- |
| `volatile t_bitMap &flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `volatile uint16_t &sta_wait` | Dato de estado/configuración `sta_wait`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t &sta_retry` | Dato de estado/configuración `sta_retry`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EspSystemHandle`

`esp:src/esp_system.h:13`

| Atributo / firma | Propósito |
| --- | --- |
| `EspSystemRuntime runtime` | Dato de estado/configuración `runtime`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } preferences` | Dato de estado/configuración `preferences`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } handles` | Dato de estado/configuración `handles`; declaración estructural, interpretación específica pendiente de ampliar. |

### `preferences`

`esp:src/esp_system.h:15`

| Atributo / firma | Propósito |
| --- | --- |
| `const AppConfig *cached` | Dato de estado/configuración `cached`; declaración estructural, interpretación específica pendiente de ampliar. |

### `handles`

`esp:src/esp_system.h:16`

| Atributo / firma | Propósito |
| --- | --- |
| `void *uner_app` | Dato de estado/configuración `uner_app`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *json_bridge` | Dato de estado/configuración `json_bridge`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-appbindings_configure"></a>`void AppBindings_Configure(UNER_App *app)` | Conecta operaciones de aplicación a los handlers UNER sin mover su estado privado. | `esp:src/app_bindings.cpp:183`; editorial |
| <a id="fn-bridge_defaultbackend"></a>`const BridgeBackendOps &Bridge_DefaultBackend()` | Adapta la llamada a `bridgeBackend` con el contexto de esta integración. | `esp:src/app_bindings.cpp:191`; structural |
| <a id="fn-bridge_defaultwifi"></a>`const BridgeWifiPort &Bridge_DefaultWifi()` | Adapta la llamada a `wifiOps` con el contexto de esta integración. | `esp:src/app_bindings.cpp:195`; structural |
| <a id="fn-uner_app_defaultservices"></a>`const UNER_AppServices &UNER_App_DefaultServices()` | Coordina `BridgePlatform_ESP`, `UNER_SerialTransport_InitWithContext`, `delay` para Composición y ciclo principal ESP. | `esp:src/app_bindings.cpp:128`; structural |
| <a id="fn-wsjsonbridge_init"></a>`void WSJsonBridge_Init(AsyncWebSocket *socket, UNER_App *app)` | Coordina `BridgePlatform_ESP`, `WSJsonBridge_InitWithPorts`, `bridgeBackend` para Composición y ciclo principal ESP. | `esp:src/app_bindings.cpp:72`; structural |
| <a id="fn-webserverws_setunerapp"></a>`void WebServerWS_SetUNERApp(void *app)` | Adapta la llamada a `AppBindings_Configure` con el contexto de esta integración. | `esp:src/app_bindings.cpp:189`; structural |
| <a id="fn-webserver_defaultservices"></a>`const WebServerServices &WebServer_DefaultServices()` | Coordina `UNER_App_NotifyApClientJoined`, `UNER_App_NotifyApClientLeft`, `UNER_App_NotifyWebPageClosed` para Composición y ciclo principal ESP. | `esp:src/app_bindings.cpp:161`; structural |
| <a id="fn-g_espsystem"></a>`EspSystemHandle g_espSystem(flagsGlobals,tm_sta_wait,tm_sta_retry)` | Operación g esp System de Composición y ciclo principal ESP; semántica fina pendiente de ampliación. | `esp:src/esp_system.cpp:7`; inferred-name |
| <a id="fn-loop"></a>`void loop()` | Atiende periódicamente servicios y cede tiempo al scheduler/WDT. | `esp:src/main.cpp:70`; editorial |
| <a id="fn-setup"></a>`void setup()` | Inicializa plataforma, servicios, puertos, protocolo y servidor en el orden de composición. | `esp:src/main.cpp:28`; editorial |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-bridgebackend"></a>`static BridgeBackendOps bridgeBackend(UNER_App *app)` | Coordina `NVS_Save`, `NVS_StorageReady`, `UNER_App_NotifyApFallbackReady` para Composición y ciclo principal ESP. | `esp:src/app_bindings.cpp:44`; structural |
| <a id="fn-clamp_scan_rssi"></a>`static int8_t clamp_scan_rssi(int32_t rssi)` | Operación clamp scan rssi de Composición y ciclo principal ESP; semántica fina pendiente de ampliación. | `esp:src/app_bindings.cpp:103`; inferred-name |
| <a id="fn-map_scan_encryption_type"></a>`static uint8_t map_scan_encryption_type(uint8_t raw_type)` | Coordina `defined`, `else` para Composición y ciclo principal ESP. | `esp:src/app_bindings.cpp:77`; structural |
| <a id="fn-neutralip"></a>`static WifiIpv4 neutralIP(IPAddress ip)` | Adapta la llamada a `WifiIpv4` con el contexto de esta integración. | `esp:src/app_bindings.cpp:15`; structural |
| <a id="fn-read_scan_entry"></a>`static bool read_scan_entry(uint8_t index, WifiScanSnapshotEntry *out)` | Coordina `clamp_scan_rssi`, `encryptionType`, `getScanInfoByIndex` para Composición y ciclo principal ESP. | `esp:src/app_bindings.cpp:110`; structural |
| <a id="fn-wifiops"></a>`static const BridgeWifiOps &wifiOps()` | Coordina `WifiUtils_ApplyAPConfig`, `WifiUtils_ApplySTAConfig`, `WifiUtils_Begin` para Composición y ciclo principal ESP. | `esp:src/app_bindings.cpp:17`; structural |
| <a id="fn-wifistatusledmodefor"></a>`static WifiStatusLedMode wifiStatusLedModeFor(WifiServiceState state)` | Operación wifi Status Led Mode For de Composición y ciclo principal ESP; semántica fina pendiente de ampliación. | `esp:src/main.cpp:19`; inferred-name |
