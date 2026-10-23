# Arbitraje de exploración Wi-Fi

ID: `esp:module/src/wifi_scan_arbiter.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Asigna dueño de una exploración Wi-Fi para evitar iniciar operaciones competidoras.

Adquisición, liberación y dueño lógico; la radio y resultados pertenecen a otros servicios.

<a id="modularity"></a>
## Modularidad observada

Lógica C sin radio concreta.

<a id="state"></a>
## Handle, estado y atributos externos

Estado de dueño/actividad del árbitro; las identidades del iniciador deben mantenerse al cancelar/liberar.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Adquirir antes de iniciar scan y liberar al completar/cancelar.
- Un iniciador ocupado no debe borrar resultados de otro.

<a id="interactions"></a>
## Interacciones

- Web y UNER comparten el árbitro mediante backend; WifiService ejecuta la radio.

<a id="limitations"></a>
## Precauciones y límites

- La concesión no implica que la radio aceptó o completó scan.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/wifi_scan_arbiter.c` | c-cpp-structural | `c591fdcb94c327327fe6205064d82ae9d18f0c0ea9a3ce60c4c8d63b2a747653` |
| `src/wifi_scan_arbiter.h` | c-cpp-structural | `9a9988f2f0a69df0f30d1606f9209251f0a13fe1311a2f5873352764ecc40cff` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WifiScanArbiter`

`esp:src/wifi_scan_arbiter.h:22`

| Atributo / firma | Propósito |
| --- | --- |
| `WifiScanOwner owner` | Dato de estado/configuración `owner`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t acquisitions` | Dato de estado/configuración `acquisitions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t releases` | Dato de estado/configuración `releases`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t busy_rejections` | Dato de estado/configuración `busy_rejections`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t invalid_releases` | Dato de estado/configuración `invalid_releases`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-wifiscanarbiter_getowner"></a>`WifiScanOwner WifiScanArbiter_GetOwner(const WifiScanArbiter *arbiter)` | Consulta Owner dentro de Arbitraje de exploración Wi-Fi. | `esp:src/wifi_scan_arbiter.c:52`; inferred-name |
| <a id="fn-wifiscanarbiter_init"></a>`void WifiScanArbiter_Init(WifiScanArbiter *arbiter)` | Inicializa Arbitraje de exploración Wi-Fi dentro de Arbitraje de exploración Wi-Fi. | `esp:src/wifi_scan_arbiter.c:11`; inferred-name |
| <a id="fn-wifiscanarbiter_isowner"></a>`uint8_t WifiScanArbiter_IsOwner(const WifiScanArbiter *arbiter, WifiScanOwner owner)` | Adapta la llamada a `owner_valid` con el contexto de esta integración. | `esp:src/wifi_scan_arbiter.c:45`; structural |
| <a id="fn-wifiscanarbiter_release"></a>`uint8_t WifiScanArbiter_Release(WifiScanArbiter *arbiter, WifiScanOwner owner)` | Adapta la llamada a `owner_valid` con el contexto de esta integración. | `esp:src/wifi_scan_arbiter.c:32`; structural |
| <a id="fn-wifiscanarbiter_tryacquire"></a>`WifiScanAcquireResult WifiScanArbiter_TryAcquire(WifiScanArbiter *arbiter, WifiScanOwner owner)` | Adapta la llamada a `owner_valid` con el contexto de esta integración. | `esp:src/wifi_scan_arbiter.c:17`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-owner_valid"></a>`static uint8_t owner_valid(WifiScanOwner owner)` | Operación owner valid de Arbitraje de exploración Wi-Fi; semántica fina pendiente de ampliación. | `esp:src/wifi_scan_arbiter.c:5`; inferred-name |
