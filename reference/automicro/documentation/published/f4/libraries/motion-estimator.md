# Estimador de movimiento relativo

ID: `f4:module/Core/Inc/motion_estimator.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Estima velocidad longitudinal relativa a corto plazo desde aceleración lineal de la IMU.

Filtrado, deadband, integración con fuga, confianza y flags de frescura/vibración/referencia. No es odometría absoluta ni mide ruedas.

<a id="modularity"></a>
## Modularidad observada

Consume AM_OrientationOps y snapshot, sin incluir implementación MPU ni HAL.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

MotionEstimator_Handle conserva config/status y última secuencia; config.orientation/mpu prestan el proveedor. Status combina mg, m/s², m/s, grados y dps: no confundir unidades entre campos.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init configura el modelo; Reset elimina historia.
- Update usa muestra nueva, dt y condiciones de movimiento para actualizar estimación y confianza.
- MarkExternalReference registra una referencia temporal del llamador.

## Trabajo en tándem

- app_tasks.c actualiza estimación dentro de la secuencia de control; los reportes la exponen como estimación.
- GOTO actual usa AutoMotion y perfiles de motor; no asumir que integra directamente velocity_mps de este módulo.

## Precauciones y límites

- La integración acumula deriva aun con fugas y guardas.
- El indicador de confianza es heurístico, no precisión física certificada.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/motion_estimator.h` | `motion_estimator_types.h` | `987a9f61e2b5f33b899eacf457dcbac06f2b716b9fcd0e0c1aaa763c807c4ab1` |
| `Core/Inc/motion_estimator_types.h` | `control_ports.h`, `stdint.h`, `motion_estimator_data.h` | `1e6e9b8755e8020c3d660da8c60140f4c3197123999ef30007c64c1dfbdbabae` |
| `Core/Inc/motion_estimator_data.h` | `am_port.h` | `2ee6988f0ee91b6b49680a8318cafc1d8ce1eaf018ebdf64443354d55dafe6d1` |
| `Core/Src/motion_estimator.c` | `motion_estimator.h`, `utils/macros_utils.h`, `stddef.h`, `string.h` | `be167ff03f9b387cd3b84558f2b18305f1100835dca94b3d3d308cffe3dbe0d9` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `MotionEstimator_Handle`

`f4:Core/Inc/motion_estimator_types.h:20`

| Atributo / firma | Función y precaución |
| --- | --- |
| `MotionEstimator_Config_t config` | Configuración copiada por la instancia; los punteros que contiene siguen siendo prestados. |
| `MotionEstimator_Status_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint16_t last_seq` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t initialized` | Marca de inicialización local; no acredita disponibilidad de dispositivos externos. |

### `MotionEstimator_Config_t`

`f4:Core/Inc/motion_estimator_data.h:25`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *mpu` | Dato de estado/configuración `mpu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const struct AM_OrientationOps *orientation` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `float accel_deadband_mg` | Dato de estado/configuración `accel_deadband_mg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float accel_filter_alpha` | Dato de estado/configuración `accel_filter_alpha`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float velocity_leak_per_s` | Dato de estado/configuración `velocity_leak_per_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float idle_velocity_leak_per_s` | Dato de estado/configuración `idle_velocity_leak_per_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float velocity_limit_mps` | Dato de estado/configuración `velocity_limit_mps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float high_pitch_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float vibration_mg` | Dato de estado/configuración `vibration_mg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stale_timeout_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t reference_timeout_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `MotionEstimator_Status_t`

`f4:Core/Inc/motion_estimator_data.h:39`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t sample_seq` | Secuencia de muestra; cambia al producir nuevos datos, no al volver a leerlos. |
| `uint16_t sample_dt_us` | Tiempo o intervalo en microsegundos; precisión efectiva dependiente del puerto. |
| `uint8_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint8_t confidence_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `int16_t linear_accel_y_mg` | Dato de estado/configuración `linear_accel_y_mg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float linear_accel_mps2` | Dato de estado/configuración `linear_accel_mps2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float filtered_accel_mps2` | Dato de estado/configuración `filtered_accel_mps2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float velocity_mps` | Dato de estado/configuración `velocity_mps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float yaw_rate_dps` | Dato de estado/configuración `yaw_rate_dps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float pitch_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float vibration_mg` | Dato de estado/configuración `vibration_mg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t last_update_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t last_sample_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t last_reference_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t has_sample` | Dato de estado/configuración `has_sample`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-motionestimator_getdefaultconfig"></a>`void MotionEstimator_GetDefaultConfig(MotionEstimator_Config_t *config)` | Carga filtro, fugas, deadband y límites de confianza predeterminados. | `f4:Core/Inc/motion_estimator.h:7`; editorial |
| <a id="fn-motionestimator_getstatus"></a>`const MotionEstimator_Status_t *MotionEstimator_GetStatus(const MotionEstimator_Handle *estimator)` | Devuelve vista de velocidad estimada, unidades y flags de calidad. | `f4:Core/Inc/motion_estimator.h:18`; editorial |
| <a id="fn-motionestimator_init"></a>`AM_Result MotionEstimator_Init(MotionEstimator_Handle *estimator, const MotionEstimator_Config_t *config)` | Inicializa el estimador y conecta proveedor de orientación. | `f4:Core/Inc/motion_estimator.h:8`; editorial |
| <a id="fn-motionestimator_markexternalreference"></a>`void MotionEstimator_MarkExternalReference(MotionEstimator_Handle *estimator, float velocity_mps, uint32_t now_ms)` | Registra una referencia externa con marca temporal. | `f4:Core/Inc/motion_estimator.h:11`; editorial |
| <a id="fn-motionestimator_reset"></a>`void MotionEstimator_Reset(MotionEstimator_Handle *estimator)` | Borra velocidad, tiempos e historia de muestras. | `f4:Core/Inc/motion_estimator.h:10`; editorial |
| <a id="fn-motionestimator_update"></a>`void MotionEstimator_Update(MotionEstimator_Handle *estimator, uint32_t now_ms, uint8_t motors_active, uint8_t control_saturated)` | Integra una muestra nueva y actualiza flags/confianza según el contexto recibido. | `f4:Core/Inc/motion_estimator.h:14`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-motionestimator_clamp"></a>`static float MotionEstimator_Clamp(float value, float min_value, float max_value)` | Operación Clamp de Estimador de movimiento relativo; semántica fina pendiente de ampliación. | `f4:Core/Src/motion_estimator.c:15`; inferred-name |
| <a id="fn-motionestimator_abs"></a>`static float MotionEstimator_Abs(float value)` | Operación Abs de Estimador de movimiento relativo; semántica fina pendiente de ampliación. | `f4:Core/Src/motion_estimator.c:26`; inferred-name |
| <a id="fn-motionestimator_applyleak"></a>`static float MotionEstimator_ApplyLeak(float value, float leak_per_s, float dt_s)` | Adapta la llamada a `MotionEstimator_Clamp` con el contexto de esta integración. | `f4:Core/Src/motion_estimator.c:32`; structural |
| <a id="fn-motionestimator_confidence"></a>`static uint8_t MotionEstimator_Confidence(uint8_t flags, uint32_t time_without_reference_ms, uint32_t reference_timeout_ms)` | Operación Confidence de Estimador de movimiento relativo; semántica fina pendiente de ampliación. | `f4:Core/Src/motion_estimator.c:45`; inferred-name |
