# Base temporal y timers

ID: `esp:module/src/timebase.h` · Capa: **servicio** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Produce ticks y actualiza timers/flags de aplicación.

Reloj, tick de 10 ms y adaptación de plataforma; un tick acumulado no es una ejecución paralela de tarea.

<a id="modularity"></a>
## Modularidad observada

Separación de core temporal y adaptación Arduino, con convenciones de flags del proyecto.

<a id="state"></a>
## Handle, estado y atributos externos

Conserva marcas/pendientes y referencias runtime; el consumidor debe usar unidades correctas y diferencias modulares.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Init10ms prepara base en setup; Update contabiliza avance.
- ConsumeTick consume señal/contador según la API para evitar tratar una señal persistente como ticks ilimitados.

<a id="interactions"></a>
## Interacciones

- Wi-Fi y otros servicios usan tiempo/timers; loop decide cuándo atenderlos.

<a id="limitations"></a>
## Precauciones y límites

- No inferir resolución mayor a la fuente de reloj ni latencia máxima del loop.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/timebase.cpp` | c-cpp-structural | `bf2699ea0c8fd82996466d250d49a621f484b994ee7bb1ef7dd197a0323167f5` |
| `src/timebase.h` | c-cpp-structural | `e8c827e2458d0f2274339b6c782fbc4db1ea3a7f7e3475241ad740f6d9a12ef4` |
| `src/timebase_core.cpp` | c-cpp-structural | `431ad2c6f65900f1111cfa7329292fdee05185be63fc67b6e1c823b4ff83d1fc` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `TimebaseOps`

`esp:src/timebase.h:4`

| Atributo / firma | Propósito |
| --- | --- |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t (*now_ms)(void *)` | Devuelve reloj modular en ms; el consumidor decide comparaciones de deadline. |
| `void (*cooperate)(void *)` | Cede ejecución al runtime en esperas/cooperación; no garantiza ausencia de bloqueo. |

### `TimebaseHandle`

`esp:src/timebase.h:9`

| Atributo / firma | Propósito |
| --- | --- |
| `TimebaseOps ops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `volatile uint16_t *sta_wait` | Dato de estado/configuración `sta_wait`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t *sta_retry` | Dato de estado/configuración `sta_retry`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool tick_flag` | Dato de estado/configuración `tick_flag`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ticks` | Dato de estado/configuración `ticks`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t last_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t accumulated_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-timebase_consumetick"></a>`bool Timebase_ConsumeTick(TimebaseHandle &h)` | Consume el tick pendiente según la semántica de la base. | `esp:src/timebase_core.cpp:29`; editorial |
| <a id="fn-timebase_init"></a>`void Timebase_Init(TimebaseHandle &h, const TimebaseOps &ops, volatile uint16_t *sta_wait, volatile uint16_t *sta_retry)` | Adapta la llamada a `now_ms` con el contexto de esta integración. | `esp:src/timebase_core.cpp:3`; structural |
| <a id="fn-timebase_init10ms"></a>`void Timebase_Init10ms()` | Inicializa la base de tick de 10 ms. | `esp:src/timebase.cpp:6`; editorial |
| <a id="fn-timebase_ticks"></a>`uint32_t Timebase_Ticks(const TimebaseHandle &h)` | Devuelve el campo `h.ticks` del estado actual de la instancia. | `esp:src/timebase_core.cpp:34`; structural |
| <a id="fn-timebase_update"></a>`void Timebase_Update(TimebaseHandle &h)` | Actualiza ticks y timers con el tiempo transcurrido. | `esp:src/timebase_core.cpp:11`; editorial |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
