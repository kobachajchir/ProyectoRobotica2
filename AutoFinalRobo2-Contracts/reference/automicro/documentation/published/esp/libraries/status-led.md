# Indicador LED de conectividad

ID: `esp:module/src/wifi_status_led.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Expresa el estado AP/STA/inactivo mediante el LED activo bajo del ESP.

Patrón temporal y escritura de pin; no define autoridad de sesión ni disponibilidad F4.

<a id="modularity"></a>
## Modularidad observada

Core de patrón y adaptador GPIO separados; el pin es propio de ESP8266.

<a id="state"></a>
## Handle, estado y atributos externos

Modo, fase y reloj del indicador; la adaptación toma GPIO2 después del arranque para respetar su función de boot.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Begin toma el pin después de setup.
- Update recibe modo derivado de WifiService y tiempo actual.

<a id="interactions"></a>
## Interacciones

- main mapea WifiServiceState a modo del indicador.

<a id="limitations"></a>
## Precauciones y límites

- Un LED indica estado de servicio, no prueba tráfico extremo a extremo.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/wifi_status_led.cpp` | c-cpp-structural | `377525ce1d28c44dfb489da6bc2275d46c723ad3c9c309ee2e112d5fd8577a1e` |
| `src/wifi_status_led.h` | c-cpp-structural | `51c7b1f25cec8f12de88c7d252d92d4593a2e5a5dc89b16887b62edf65cc698b` |
| `src/wifi_status_led_core.cpp` | c-cpp-structural | `7e0d0073d0f5605d21e9a64b65785bf6934d15fb697a46b2feafd8599f952ae5` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WifiStatusLedOps`

`esp:src/wifi_status_led.h:9`

| Atributo / firma | Propósito |
| --- | --- |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void (*initialize)(void *)` | Configura pin/recurso de salida en plataforma antes del primer patrón LED. |
| `void (*write)(void *, bool on)` | Aplica nivel lógico de LED desde la política de estado; el adaptador decide polaridad física. |

### `WifiStatusLedHandle`

`esp:src/wifi_status_led.h:14`

| Atributo / firma | Propósito |
| --- | --- |
| `WifiStatusLedOps ops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `bool initialized` | Marca de inicialización local; no acredita disponibilidad de dispositivos externos. |
| `bool ap_led_on` | Dato de estado/configuración `ap_led_on`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t last_toggle_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t blink_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `WifiStatusLedMode mode` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-wifistatusled_begin"></a>`void WifiStatusLed_Begin(WifiStatusLedHandle &h, const WifiStatusLedOps &ops, uint32_t now_ms, uint32_t blink_period_ms)` | Inicializa salida LED después de la etapa de arranque. | `esp:src/wifi_status_led_core.cpp:6`; editorial |
| <a id="fn-wifistatusled_update"></a>`void WifiStatusLed_Update(WifiStatusLedHandle &h, WifiStatusLedMode mode, uint32_t now_ms)` | Actualiza el patrón de conectividad con modo y tiempo recibidos. | `esp:src/wifi_status_led_core.cpp:16`; editorial |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-writeled"></a>`static void writeLed(WifiStatusLedHandle &h, bool on)` | Adapta la llamada a `write` con el contexto de esta integración. | `esp:src/wifi_status_led_core.cpp:3`; structural |
