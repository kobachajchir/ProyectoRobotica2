# Snapshot de resultados Wi-Fi

ID: `esp:module/src/wifi_scan_snapshot.h` · Capa: **datos** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Conserva resultados de scan en un modelo estable para consumidores del SDK y protocolo.

Entradas SSID, señal, canal y seguridad; no iniciar ni cancelar radio.

<a id="modularity"></a>
## Modularidad observada

Datos neutrales del scan y operaciones de lectura, separados de lifetime del SDK.

<a id="state"></a>
## Handle, estado y atributos externos

El snapshot posee copia acotada de resultados; cadenas y contadores respetan capacidades declaradas.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Capturar desde resultados válidos antes de borrar recursos de scan.
- Consumir el snapshot para reportes/páginas sin depender de punteros del SDK liberados.

<a id="interactions"></a>
## Interacciones

- Bridge y handlers UNER comparten el mismo resultado observado.

<a id="limitations"></a>
## Precauciones y límites

- No inventar detalles de red no disponibles; respetar cantidad/capacidad real.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/wifi_scan_snapshot.c` | c-cpp-structural | `7886501167f74ec241ef78c285db9dbe283e0207db72cbbf04174e1f41e6c704` |
| `src/wifi_scan_snapshot.h` | c-cpp-structural | `08d4dee6da614d8a014f4d30fac05cac5f14179b1f11851721fc4c3e7e413472` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WifiScanSnapshotEntry`

`esp:src/wifi_scan_snapshot.h:13`

| Atributo / firma | Propósito |
| --- | --- |
| `char ssid[SSID_MAX_LEN + 1]` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bssid[6]` | Dato de estado/configuración `bssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t signal_strength` | Dato de estado/configuración `signal_strength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t encryption_type` | Dato de estado/configuración `encryption_type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t channel` | Dato de estado/configuración `channel`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WifiScanSnapshot`

`esp:src/wifi_scan_snapshot.h:22`

| Atributo / firma | Propósito |
| --- | --- |
| `WifiScanSnapshotEntry entries[WIFI_SCAN_SNAPSHOT_MAX]` | Dato de estado/configuración `entries`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t count` | Dato de estado/configuración `count`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-wifiscansnapshot_append"></a>`uint8_t WifiScanSnapshot_Append(WifiScanSnapshot *snapshot, const WifiScanSnapshotEntry *entry)` | Operación Append de Snapshot de resultados Wi-Fi; semántica fina pendiente de ampliación. | `esp:src/wifi_scan_snapshot.c:11`; inferred-name |
| <a id="fn-wifiscansnapshot_count"></a>`uint8_t WifiScanSnapshot_Count(const WifiScanSnapshot *snapshot)` | Operación Count de Snapshot de resultados Wi-Fi; semántica fina pendiente de ampliación. | `esp:src/wifi_scan_snapshot.c:20`; inferred-name |
| <a id="fn-wifiscansnapshot_findbyssid"></a>`uint8_t WifiScanSnapshot_FindBySsid(const WifiScanSnapshot *snapshot, const char *ssid, WifiScanSnapshotEntry *out)` | Adapta la llamada a `strcmp` con el contexto de esta integración. | `esp:src/wifi_scan_snapshot.c:33`; structural |
| <a id="fn-wifiscansnapshot_get"></a>`uint8_t WifiScanSnapshot_Get(const WifiScanSnapshot *snapshot, uint8_t index, WifiScanSnapshotEntry *out)` | Consulta Snapshot de resultados Wi-Fi dentro de Snapshot de resultados Wi-Fi. | `esp:src/wifi_scan_snapshot.c:25`; inferred-name |
| <a id="fn-wifiscansnapshot_init"></a>`void WifiScanSnapshot_Init(WifiScanSnapshot *snapshot)` | Inicializa Snapshot de resultados Wi-Fi dentro de Snapshot de resultados Wi-Fi. | `esp:src/wifi_scan_snapshot.c:5`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
