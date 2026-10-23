# Fachada de sensores ESP

ID: `esp:module/src/sensors.h` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Provee la superficie local de sensores declarada en la aplicación ESP.

No atribuye al ESP las adquisiciones MPU/IR que pertenecen a F4.

<a id="modularity"></a>
## Modularidad observada

Fachada de aplicación; revisar trabajo efectivo de cada operación antes de describir un sensor físico conectado al ESP.

<a id="state"></a>
## Handle, estado y atributos externos

Estado local que define el módulo; telemetría remota proviene de decoders y streams UNER.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Sensors_Init se invoca en setup.
- Distinguir funciones declaradas de adquisiciones efectivas y de reportes recibidos.

<a id="interactions"></a>
## Interacciones

- F4 sigue siendo productor de las muestras del robot.

<a id="limitations"></a>
## Precauciones y límites

- No inferir hardware por el nombre del módulo ni convertir datos simulados en muestra real.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/sensors.cpp` | c-cpp-structural | `9015786feb18b25683151cd91ebf0d534a44ad71004c9fa92c7129fe286d8697` |
| `src/sensors.h` | c-cpp-structural | `e03e5087b7bab019744e6c4670dc5c7e8e3315d52fb91edde1bcaa5a6c3c2052` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `SensorsOps`

`esp:src/sensors.h:4`

| Atributo / firma | Propósito |
| --- | --- |
| `void *context` | Dato de estado/configuración `context`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool (*initialize)(void *)` | Prepara fuente de sensores de esta fachada de aplicación. |
| `bool (*read_json)(void *, char *out, size_t capacity)` | Rellena representación JSON de sensores según argumentos/capacidad de la firma; no equivale al decoder de telemetría F4. |

### `SensorsHandle`

`esp:src/sensors.h:9`

| Atributo / firma | Propósito |
| --- | --- |
| `SensorsOps ops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `char json[96]{ estructura anidada }` | Dato de estado/configuración `json`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-sensors_getreadingsjson"></a>`const char *Sensors_GetReadingsJSON()` | Consulta Readings JSON dentro de Fachada de sensores ESP. | `esp:src/sensors.cpp:26`; inferred-name |
| <a id="fn-sensors_init"></a>`bool Sensors_Init()` | Inicializa Fachada de sensores ESP dentro de Fachada de sensores ESP. | `esp:src/sensors.cpp:20`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-defaultread"></a>`static bool defaultRead(void *, char *out, size_t capacity)` | Adapta la llamada a `snprintf` con el contexto de esta integración. | `esp:src/sensors.cpp:5`; structural |
