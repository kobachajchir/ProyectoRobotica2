# Sensores IR TCRT5000

ID: `f4:module/Core/Inc/tcrt5000.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Da identidad y polaridad a ocho sensores ADC y controla semánticamente los emisores IR de grupos 5 y 3.

Incluye lecturas raw, normalización de presentación y evaluación geométrica de línea. La adquisición DMA y la calibración por sesión pertenecen a la aplicación y LineCalibration.

<a id="modularity"></a>
## Modularidad observada

Sin HAL, pero el modelo de ocho sensores y dos grupos es específico del robot. initialize y set_emitters encapsulan plataforma.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

TCRT5000_Handle posee puerto copiado y dos booleanos de emisores; el array volatile de ocho lecturas se recibe prestado. Las definiciones SensorDef relacionan identidad lógica, índice ADC y geometría; no reordenar el array por su posición visual.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init llama initialize y establece los estados de emisores.
- Los cambios de iluminación necesitan asentamiento; leer ADC no acredita que corresponda al nuevo estado óptico.
- ReadRouteSensorsEx transforma lecturas con configuración; el seguidor actual añade evaluación calibrada por LineFollow_Evaluate.

## Trabajo en tándem

- app_stm32_ports.h conecta ADC y emisores activos bajos.
- LineCalibration captura con distintas fases de luz; LineFollow consume error y confianza junto a frescura MPU.

## Precauciones y límites

- Distinguir raw ADC, normalizado de presentación y darkness calibrada.
- Los umbrales estáticos de RouteProfile no sustituyen una calibración blanco/pista válida.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/tcrt5000.h` | `tcrt5000_types.h`, `tcrt5000_data.h` | `d2d457d128fabc78d45323547083029b2e5799a6eb4d21a400831e56427aec1b` |
| `Core/Inc/tcrt5000_types.h` | `am_port.h`, `stdbool.h`, `stdint.h` | `77b2c96008c8e7d66d9b69c25bda2e7d47bd8caf6006fe0c966cb67c348c9dac` |
| `Core/Inc/tcrt5000_data.h` | `am_port.h` | `78042a34cc089c561f6466df6f036b2a79284ecd20cce1b368299feb2ad2cb16` |
| `Core/Src/tcrt5000.c` | `tcrt5000.h`, `stddef.h`, `string.h` | `f8e46d820004bd5c7eb63fde515bf38eaefb16d205357187726b702f2f62234d` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `TCRT5000_Port`

`f4:Core/Inc/tcrt5000_types.h:19`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `AM_Result (*initialize)(void *ctx)` | Operación initialize de Sensores IR TCRT5000; semántica fina pendiente de ampliación. |
| `void (*set_emitters)(void *ctx, uint8_t group, bool enabled)` | Ordena encender/apagar un grupo IR; el adaptador resuelve los pines. |

### `TCRT5000_Handle`

`f4:Core/Inc/tcrt5000_types.h:24`

| Atributo / firma | Función y precaución |
| --- | --- |
| `TCRT5000_Port port` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `bool emitters_group5_on, emitters_group3_on` | La declaración agrupa estados lógicos group5/group3; reflejan la orden de iluminación, no una medida óptica. |

### `TCRT5000_SensorDef_t`

`f4:Core/Inc/tcrt5000_data.h:54`

| Atributo / firma | Función y precaución |
| --- | --- |
| `TCRT5000_SensorId_t sensorId` | Dato de estado/configuración `sensorId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *label` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `TCRT5000_Polarity_t polarity` | Dato de estado/configuración `polarity`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TCRT5000_RouteProfile_t`

`f4:Core/Inc/tcrt5000_data.h:60`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t whiteMaxRaw` | Dato de estado/configuración `whiteMaxRaw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t blackMinRaw` | Dato de estado/configuración `blackMinRaw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t sideMarginRaw` | Dato de estado/configuración `sideMarginRaw`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TCRT5000_LineConfig_t`

`f4:Core/Inc/tcrt5000_data.h:66`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t black_width_mm` | Dato de estado/configuración `black_width_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float sensor_position_mm[TCRT5000_LINE_COUNT]` | Dato de estado/configuración `sensor_position_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float centered_margin_mm` | Dato de estado/configuración `centered_margin_mm`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TCRT5000_RouteSample_t`

`f4:Core/Inc/tcrt5000_data.h:72`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t leftRaw` | Dato de estado/configuración `leftRaw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t centerRaw` | Dato de estado/configuración `centerRaw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t rightRaw` | Dato de estado/configuración `rightRaw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t leftNorm` | Dato de estado/configuración `leftNorm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t centerNorm` | Dato de estado/configuración `centerNorm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t rightNorm` | Dato de estado/configuración `rightNorm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float leftDarkness` | Dato de estado/configuración `leftDarkness`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float centerDarkness` | Dato de estado/configuración `centerDarkness`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float rightDarkness` | Dato de estado/configuración `rightDarkness`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float lateralErrorMm` | Dato de estado/configuración `lateralErrorMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float lateralErrorNorm` | Dato de estado/configuración `lateralErrorNorm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float centeredMarginMm` | Dato de estado/configuración `centeredMarginMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t lineWidthMm` | Dato de estado/configuración `lineWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t pattern` | Dato de estado/configuración `pattern`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t confidence` | Dato de estado/configuración `confidence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool ambiguous` | Dato de estado/configuración `ambiguous`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool leftBlack` | Dato de estado/configuración `leftBlack`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool centerBlack` | Dato de estado/configuración `centerBlack`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool rightBlack` | Dato de estado/configuración `rightBlack`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool leftWhite` | Dato de estado/configuración `leftWhite`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool rightWhite` | Dato de estado/configuración `rightWhite`; declaración estructural, interpretación específica pendiente de ampliar. |
| `TCRT5000_RouteAlignment_t alignment` | Dato de estado/configuración `alignment`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-tcrt5000_emittersgroup3ison"></a>`bool TCRT5000_EmittersGroup3IsOn(TCRT5000_Handle *h)` | Consulta el estado lógico ordenado al grupo de tres emisores de línea. | `f4:Core/Inc/tcrt5000.h:17`; editorial |
| <a id="fn-tcrt5000_emittersgroup3off"></a>`void TCRT5000_EmittersGroup3Off(TCRT5000_Handle *h)` | Adapta la llamada a `TCRT5000_EmittersGroup3Set` con el contexto de esta integración. | `f4:Core/Inc/tcrt5000.h:15`; structural |
| <a id="fn-tcrt5000_emittersgroup3on"></a>`void TCRT5000_EmittersGroup3On(TCRT5000_Handle *h)` | Adapta la llamada a `TCRT5000_EmittersGroup3Set` con el contexto de esta integración. | `f4:Core/Inc/tcrt5000.h:14`; structural |
| <a id="fn-tcrt5000_emittersgroup3set"></a>`void TCRT5000_EmittersGroup3Set(TCRT5000_Handle *h, bool enabled)` | Adapta la llamada a `set_emitters` con el contexto de esta integración. | `f4:Core/Inc/tcrt5000.h:16`; structural |
| <a id="fn-tcrt5000_emittersgroup5ison"></a>`bool TCRT5000_EmittersGroup5IsOn(TCRT5000_Handle *h)` | Consulta el estado lógico ordenado al grupo de cinco emisores de objetos. | `f4:Core/Inc/tcrt5000.h:13`; editorial |
| <a id="fn-tcrt5000_emittersgroup5off"></a>`void TCRT5000_EmittersGroup5Off(TCRT5000_Handle *h)` | Adapta la llamada a `TCRT5000_EmittersGroup5Set` con el contexto de esta integración. | `f4:Core/Inc/tcrt5000.h:11`; structural |
| <a id="fn-tcrt5000_emittersgroup5on"></a>`void TCRT5000_EmittersGroup5On(TCRT5000_Handle *h)` | Adapta la llamada a `TCRT5000_EmittersGroup5Set` con el contexto de esta integración. | `f4:Core/Inc/tcrt5000.h:10`; structural |
| <a id="fn-tcrt5000_emittersgroup5set"></a>`void TCRT5000_EmittersGroup5Set(TCRT5000_Handle *h, bool enabled)` | Adapta la llamada a `set_emitters` con el contexto de esta integración. | `f4:Core/Inc/tcrt5000.h:12`; structural |
| <a id="fn-tcrt5000_getdefaultlineconfig"></a>`void TCRT5000_GetDefaultLineConfig(TCRT5000_LineConfig_t *config)` | Carga geometría y anchura de línea predeterminadas. | `f4:Core/Inc/tcrt5000.h:31`; editorial |
| <a id="fn-tcrt5000_getlinesensordef"></a>`const TCRT5000_SensorDef_t *TCRT5000_GetLineSensorDef(TCRT5000_LineSensor_t sensor)` | Devuelve la definición e índice ADC de un sensor de línea. | `f4:Core/Inc/tcrt5000.h:20`; editorial |
| <a id="fn-tcrt5000_getobjectsensordef"></a>`const TCRT5000_SensorDef_t *TCRT5000_GetObjectSensorDef(TCRT5000_ObjectSensor_t sensor)` | Devuelve la definición e índice ADC de un sensor de objetos. | `f4:Core/Inc/tcrt5000.h:21`; editorial |
| <a id="fn-tcrt5000_getraw"></a>`uint16_t TCRT5000_GetRaw(const volatile uint16_t *sensorData, const TCRT5000_SensorDef_t *sensor)` | Lee el canal ADC indicado por una definición de sensor. | `f4:Core/Inc/tcrt5000.h:23`; editorial |
| <a id="fn-tcrt5000_getrawbyid"></a>`uint16_t TCRT5000_GetRawById(const volatile uint16_t *sensorData, TCRT5000_SensorId_t sensorId)` | Resuelve un ID lógico y devuelve su lectura raw. | `f4:Core/Inc/tcrt5000.h:25`; editorial |
| <a id="fn-tcrt5000_getroutealignmenttext"></a>`const char *TCRT5000_GetRouteAlignmentText(TCRT5000_RouteAlignment_t alignment)` | Devuelve el texto de la clasificación de alineación. | `f4:Core/Inc/tcrt5000.h:41`; editorial |
| <a id="fn-tcrt5000_getrouteprofile"></a>`const TCRT5000_RouteProfile_t *TCRT5000_GetRouteProfile(bool irLightsOn)` | Elige umbrales estáticos según estado lógico de iluminación. | `f4:Core/Inc/tcrt5000.h:28`; editorial |
| <a id="fn-tcrt5000_getroutesensorpositionmm"></a>`float TCRT5000_GetRouteSensorPositionMm(TCRT5000_LineSensor_t sensor)` | Devuelve la posición transversal del sensor de línea en milímetros. | `f4:Core/Inc/tcrt5000.h:33`; editorial |
| <a id="fn-tcrt5000_getsensorlabel"></a>`const char *TCRT5000_GetSensorLabel(TCRT5000_SensorId_t sensorId)` | Devuelve la etiqueta del identificador lógico IR. | `f4:Core/Inc/tcrt5000.h:22`; editorial |
| <a id="fn-tcrt5000_init"></a>`AM_Result TCRT5000_Init(TCRT5000_Handle *h, const TCRT5000_Port *port)` | Inicializa el puerto y el estado lógico de los dos grupos de emisores. | `f4:Core/Inc/tcrt5000.h:7`; editorial |
| <a id="fn-tcrt5000_isblack"></a>`bool TCRT5000_IsBlack(uint16_t raw, const TCRT5000_RouteProfile_t *profile)` | Clasifica la lectura como negro usando el perfil recibido. | `f4:Core/Inc/tcrt5000.h:29`; editorial |
| <a id="fn-tcrt5000_iswhite"></a>`bool TCRT5000_IsWhite(uint16_t raw, const TCRT5000_RouteProfile_t *profile)` | Clasifica la lectura como blanco usando el perfil recibido. | `f4:Core/Inc/tcrt5000.h:30`; editorial |
| <a id="fn-tcrt5000_normalizefordisplay"></a>`uint16_t TCRT5000_NormalizeForDisplay(uint16_t raw, TCRT5000_Polarity_t polarity)` | Normaliza el valor ADC según polaridad para su presentación. | `f4:Core/Inc/tcrt5000.h:27`; editorial |
| <a id="fn-tcrt5000_readroutesensors"></a>`void TCRT5000_ReadRouteSensors(const volatile uint16_t *sensorData, bool irLightsOn, TCRT5000_RouteSample_t *sample)` | Evalúa los tres sensores de línea con configuración predeterminada. | `f4:Core/Inc/tcrt5000.h:34`; editorial |
| <a id="fn-tcrt5000_readroutesensorsex"></a>`void TCRT5000_ReadRouteSensorsEx(const volatile uint16_t *sensorData, bool irLightsOn, const TCRT5000_LineConfig_t *config, TCRT5000_RouteSample_t *sample)` | Evalúa patrón, error lateral y alineación con configuración explícita. | `f4:Core/Inc/tcrt5000.h:37`; editorial |
| <a id="fn-tcrt5000_setlineblackwidth"></a>`void TCRT5000_SetLineBlackWidth(TCRT5000_LineConfig_t *config, uint16_t blackWidthMm)` | Actualiza anchura negra y margen geométrico derivado. | `f4:Core/Inc/tcrt5000.h:32`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-tcrt5000_getsensordef"></a>`static const TCRT5000_SensorDef_t *TCRT5000_GetSensorDef(TCRT5000_SensorId_t sensorId)` | Consulta Sensor Def dentro de Sensores IR TCRT5000. | `f4:Core/Src/tcrt5000.c:85`; inferred-name |
| <a id="fn-tcrt5000_evaluateroutealignment"></a>`static TCRT5000_RouteAlignment_t TCRT5000_EvaluateRouteAlignment(const TCRT5000_RouteSample_t *sample, const TCRT5000_RouteProfile_t *profile)` | Operación Evaluate Route Alignment de Sensores IR TCRT5000; semántica fina pendiente de ampliación. | `f4:Core/Src/tcrt5000.c:94`; inferred-name |
| <a id="fn-tcrt5000_normalizeddarkness"></a>`static float TCRT5000_NormalizedDarkness(uint16_t normalized, const TCRT5000_RouteProfile_t *profile)` | Devuelve el campo `0.0f` del estado actual de la instancia. | `f4:Core/Src/tcrt5000.c:195`; structural |
| <a id="fn-tcrt5000_updatelinegeometry"></a>`static void TCRT5000_UpdateLineGeometry(TCRT5000_LineConfig_t *config)` | Actualiza Line Geometry dentro de Sensores IR TCRT5000. | `f4:Core/Src/tcrt5000.c:218`; inferred-name |
