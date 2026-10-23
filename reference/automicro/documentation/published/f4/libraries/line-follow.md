# Calibración de línea y seguidor

ID: `f4:module/Core/Inc/line_follow_control.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Captura perfiles ópticos blanco/pista y convierte la línea calibrada en consignas de seguimiento.

Dos unidades lógicas comparten line_follow_control.c: LineCalibration_* y LineFollowControl_*. La primera usa fases de iluminación y ventanas DMA; la segunda evalúa ruta y gobierna una FSM SEARCH/TRACK/INTERSECTION/CROSS_COMMIT/RECOVER/FAULT.

<a id="modularity"></a>
## Modularidad observada

Núcleo sin drivers concretos. Tiene conocimiento compartido del esquema de calibración; separación lógica entre calibración y seguidor, pero implementación compilada en el mismo archivo.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

LineCalibration_Handle posee acumuladores volatile, ventanas[32][3], sesión, pose y perfil pendiente; store conserva perfiles blanco/pista y sus revisiones. LineFollowControl_Handle posee config/status, integral/derivada y secuencias. Config.read entrega una instantánea coherente de IR/IMU/guardas; command entrega forward/turn_ccw/lease_ms/now_ms; hold ordena detener.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init carga estado y puertos; Control usa operation y session para arrancar/confirmar/cancelar la calibración.
- PushScan acumula enteros una vez por scan DMA; Task procesa pose, estabilidad, fases, calidad y persistencia fuera de esa acumulación.
- Cada fase captura 32 ventanas de 40 scans útiles; conservar significado de las revisiones blanco/pista.
- LineFollow_Evaluate obtiene oscuridad/error/confianza; Task del seguidor exige entradas válidas y manda órdenes con lease.

## Trabajo en tándem

- La aplicación proporciona pose MPU, scans ADC y conmutación de emisores; AppPreferences persiste LineCalibration_Store.
- El seguidor usa command/hold conectados a MotionControl, sin poseer sus motores o PID concreto.
- UNER 0x1D/0x1E/0x1F controla sesión y publica estado de 168 bytes según el serializador de aplicación.

## Precauciones y límites

- Una pista basada en otra revisión de blanco no es intercambiable sin revisar las guardas.
- La acumulación usa volatile entre DMA y tarea; eso no demuestra atomicidad completa ni portabilidad multihilo.
- No confundir patrón ambiguo, pérdida de línea y datos IR vencidos: sus rechazos son distintos.

## Hallazgos vinculados

- [AMDOC-F4-003: La marca RESP permite longitudes fuera del rango de solicitud](../../../review/FINDINGS.md#amdoc-f4-003)
- [AMDOC-WEB-009: El catálogo de línea no tiene bindings específicos de visor localizados](../../../review/FINDINGS.md#amdoc-web-009)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/line_follow_control.h` | `line_follow_control_types.h` | `8950bf74fc0e12b0c2decc5437a5dc87555fbe2b811894e642b2b3c24e5b90c7` |
| `Core/Inc/line_follow_control_types.h` | `am_port.h`, `line_calibration.h`, `stdbool.h`, `stdint.h` | `e7a92ff809ec9b978602eddbfdf20cc19f2c77cb59bb06d51a081e40c3c92e9d` |
| `Core/Inc/line_calibration.h` | `stdint.h`, `stdbool.h` | `8f821b5921b4a532ec580ec9dc3e60796228e8c9b800caa6f5e2cb7da4ecd065` |
| `Core/Src/line_follow_control.c` | `line_follow_control.h`, `math.h`, `string.h` | `8ad3fb450f6ae0dee5dcd6654ccb765e1175485207e33c946d5776b74aa26434` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `LineFollow_Route`

`f4:Core/Inc/line_follow_control_types.h:28`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t leftRaw, centerRaw, rightRaw, leftNorm, centerNorm, rightNorm` | Dato de estado/configuración `rightNorm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float leftDarkness, centerDarkness, rightDarkness` | Dato de estado/configuración `rightDarkness`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float lateralErrorMm, lateralErrorNorm, centeredMarginMm` | Dato de estado/configuración `centeredMarginMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t lineWidthMm` | Dato de estado/configuración `lineWidthMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t pattern, confidence, alignment` | Dato de estado/configuración `alignment`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool ambiguous, leftBlack, centerBlack, rightBlack, leftWhite, rightWhite` | Dato de estado/configuración `rightWhite`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineFollow_Geometry`

`f4:Core/Inc/line_follow_control_types.h:36`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t black_width_mm` | Dato de estado/configuración `black_width_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float sensor_position_mm[3], centered_margin_mm` | Dato de estado/configuración `centered_margin_mm`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineFollow_Input`

`f4:Core/Inc/line_follow_control_types.h:40`

| Atributo / firma | Función y precaución |
| --- | --- |
| `LineFollow_Route route` | Dato de estado/configuración `route`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ir_seq, ir_acquired_ms, imu_acquired_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t imu_seq` | Dato de estado/configuración `imu_seq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float pitch_deg, roll_deg, yaw_deg, yaw_rate_ccw_dps` | Dato de estado/configuración `yaw_rate_ccw_dps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float pitch_error_deg, roll_deviation_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint8_t imu_valid, yaw_valid, ir_valid, illumination_settling` | Dato de estado/configuración `illumination_settling`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t armed, balance_active, mode_follow, calibrated` | Dato de estado/configuración `calibrated`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineFollow_Config_t`

`f4:Core/Inc/line_follow_control_types.h:49`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto prestado compartido por read, command y hold. |
| `bool (*read)(void *, LineFollow_Input *)` | Copia entradas de sensores, secuencias, tiempos y guardas al snapshot del seguidor. |
| `bool (*command)(void *, float forward, float turn_ccw, uint16_t lease_ms, uint32_t now_ms)` | Solicita marcha/giro CCW con lease y tiempo; false significa que el árbitro no aceptó. |
| `void (*hold)(void *)` | Solicita detener el movimiento a la aplicación. |
| `LineFollow_Geometry line_config` | Dato de estado/configuración `line_config`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float forward_percent, recover_turn_percent, max_turn_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float kp, ki, kd, yaw_rate_damping` | Dato de estado/configuración `yaw_rate_damping`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t recover_timeout_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t enabled` | Dato de estado/configuración `enabled`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineFollow_Status_t`

`f4:Core/Inc/line_follow_control_types.h:60`

| Atributo / firma | Función y precaución |
| --- | --- |
| `LineFollow_Fsm_t fsm` | Estado de seguimiento SEARCH/TRACK/INTERSECTION/CROSS_COMMIT/RECOVER/FAULT. |
| `LineFollow_RejectReason_t reject_reason` | Motivo de rechazo/guarda; no confundir con patrón óptico. |
| `LineFollow_Route sample` | Dato de estado/configuración `sample`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t stable_pattern, candidate_pattern, candidate_count` | Dato de estado/configuración `candidate_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t confidence_percent, ambiguous, sensors_valid` | Dato de estado/configuración `sensors_valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t line_width_mm` | Dato de estado/configuración `line_width_mm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float lateral_error_mm, lateral_error_norm, forward_percent, turn_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float yaw_rate_dps, last_track_error_mm` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t state_started_ms, last_update_ms` | Memoria de la observación anterior usada por la máquina de estados. |

### `LineFollowControl_Handle`

`f4:Core/Inc/line_follow_control_types.h:71`

| Atributo / firma | Función y precaución |
| --- | --- |
| `LineFollow_Config_t config` | Configuración copiada por la instancia; los punteros que contiene siguen siendo prestados. |
| `LineFollow_Status_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `float integral, previous_error, filtered_derivative` | Dato de estado/configuración `filtered_derivative`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t last_ir_seq, candidate_ms, unstable_ms, ambiguous_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t initialized, candidate_valid, unstable, ambiguity_pending` | Dato de estado/configuración `ambiguity_pending`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineCalibration_Profile`

`f4:Core/Inc/line_calibration.h:13`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t mean[LINE_CAL_PHASES][LINE_CAL_SENSORS]` | Dato de estado/configuración `mean`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t noise[LINE_CAL_PHASES][LINE_CAL_SENSORS]` | Dato de estado/configuración `noise`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float pitch_deg, roll_deg, yaw_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint32_t useful_scans` | Dato de estado/configuración `useful_scans`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t valid` | Dato de estado/configuración `valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t reserved[3]` | Dato de estado/configuración `reserved`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineCalibration_Store`

`f4:Core/Inc/line_calibration.h:21`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t white_revision, track_white_revision` | Dato de estado/configuración `track_white_revision`; declaración estructural, interpretación específica pendiente de ampliar. |
| `LineCalibration_Profile white, track` | Dato de estado/configuración `track`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineCalibration_Pose`

`f4:Core/Inc/line_calibration.h:31`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float pitch_deg, roll_deg, yaw_deg, gyro_max_dps` | Dato de estado/configuración `gyro_max_dps`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t acquired_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t seq` | Dato de estado/configuración `seq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t valid` | Dato de estado/configuración `valid`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineCalibration_Port`

`f4:Core/Inc/line_calibration.h:37`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `void (*lights)(void *, bool, bool)` | Selecciona ambos grupos de iluminación para una fase de calibración. |
| `void (*restore)(void *)` | Restaura la iluminación/contexto que existía antes de calibrar. |
| `bool (*save)(void *, const LineCalibration_Store *)` | Persiste el store completo; false mantiene explícito el fallo de almacenamiento. |

### `LineCalibration_Handle`

`f4:Core/Inc/line_calibration.h:43`

| Atributo / firma | Función y precaución |
| --- | --- |
| `LineCalibration_Store store` | Dato de estado/configuración `store`; declaración estructural, interpretación específica pendiente de ampliar. |
| `LineCalibration_Profile pending` | Dato de estado/configuración `pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `LineCalibration_Port port` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `LineCalibration_Pose reference, still_reference` | Dato de estado/configuración `still_reference`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile LineCalibration_State state` | Fase volatile que arbitra captura DMA y procesamiento de tarea. |
| `uint32_t session_id, started_ms, still_ms, phase_ms, last_adc_seq` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint16_t last_pose_seq` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t target, phase, reason, progress` | Dato de estado/configuración `progress`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t scans` | Cantidad de scans útiles de la fase; hasta 1280. |
| `volatile uint32_t sum[3]` | Dato de estado/configuración `sum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint64_t squares[3]` | Acumulación entera de cuadrados para estimar ruido; campos de 64 bits. |
| `volatile uint32_t window_sum[3]` | Dato de estado/configuración `window_sum`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t windows[32][3]` | Medias de 32 ventanas × 3 sensores; cada ventana agrupa 40 scans. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-linecalibration_busy"></a>`bool LineCalibration_Busy(const LineCalibration_Handle *)` | Indica si una fase de calibración sigue activa. | `f4:Core/Inc/line_calibration.h:61`; editorial |
| <a id="fn-linecalibration_control"></a>`uint8_t LineCalibration_Control(LineCalibration_Handle *, uint8_t operation, uint32_t session, uint32_t now_ms)` | Procesa una operación de sesión con su ID y tiempo. | `f4:Core/Inc/line_calibration.h:62`; editorial |
| <a id="fn-linecalibration_init"></a>`void LineCalibration_Init(LineCalibration_Handle *, const LineCalibration_Port *, const LineCalibration_Store *)` | Inicializa calibración con puerto y store previamente persistido. | `f4:Core/Inc/line_calibration.h:59`; editorial |
| <a id="fn-linecalibration_pushscan"></a>`void LineCalibration_PushScan(LineCalibration_Handle *, const uint16_t raw[3], uint32_t seq, uint32_t now_ms)` | Acumula un scan ADC nuevo con secuencia/tiempo sin hacer trabajo de persistencia. | `f4:Core/Inc/line_calibration.h:65`; editorial |
| <a id="fn-linecalibration_ready"></a>`bool LineCalibration_Ready(const LineCalibration_Store *)` | Comprueba disponibilidad y correspondencia de perfiles blanco/pista. | `f4:Core/Inc/line_calibration.h:60`; editorial |
| <a id="fn-linecalibration_task"></a>`void LineCalibration_Task(LineCalibration_Handle *, const LineCalibration_Pose *, uint32_t now_ms)` | Avanza estabilidad, captura, calidad y guardado a partir de pose actual. | `f4:Core/Inc/line_calibration.h:63`; editorial |
| <a id="fn-linefollowcontrol_getdefaultconfig"></a>`void LineFollowControl_GetDefaultConfig(LineFollow_Config_t *config)` | Carga geometría, ganancias y límites del seguidor. | `f4:Core/Inc/line_follow_control.h:7`; editorial |
| <a id="fn-linefollowcontrol_getfsmtext"></a>`const char *LineFollowControl_GetFsmText(LineFollow_Fsm_t fsm)` | Devuelve etiqueta de la fase del seguidor. | `f4:Core/Inc/line_follow_control.h:16`; editorial |
| <a id="fn-linefollowcontrol_getstatus"></a>`const LineFollow_Status_t *LineFollowControl_GetStatus(const LineFollowControl_Handle *line)` | Devuelve vista de FSM, muestra, error y rechazo actuales. | `f4:Core/Inc/line_follow_control.h:15`; editorial |
| <a id="fn-linefollowcontrol_init"></a>`AM_Result LineFollowControl_Init(LineFollowControl_Handle *line, const LineFollow_Config_t *config)` | Inicializa la FSM con sus callbacks de lectura y movimiento. | `f4:Core/Inc/line_follow_control.h:8`; editorial |
| <a id="fn-linefollowcontrol_reset"></a>`void LineFollowControl_Reset(LineFollowControl_Handle *line)` | Restablece historia de seguimiento y estabilidad de patrón. | `f4:Core/Inc/line_follow_control.h:10`; editorial |
| <a id="fn-linefollowcontrol_setenabled"></a>`void LineFollowControl_SetEnabled(LineFollowControl_Handle *line, uint8_t enabled)` | Activa o detiene la producción de consignas del seguidor. | `f4:Core/Inc/line_follow_control.h:11`; editorial |
| <a id="fn-linefollowcontrol_setlineblackwidth"></a>`void LineFollowControl_SetLineBlackWidth(LineFollowControl_Handle *line, uint16_t width_mm)` | Actualiza ancho de línea usado por el modelo geométrico. | `f4:Core/Inc/line_follow_control.h:12`; editorial |
| <a id="fn-linefollowcontrol_task"></a>`void LineFollowControl_Task(LineFollowControl_Handle *line, uint32_t now_ms)` | Evalúa entradas/guardas, avanza FSM y solicita marcha/giro o hold. | `f4:Core/Inc/line_follow_control.h:14`; editorial |
| <a id="fn-linefollow_evaluate"></a>`bool LineFollow_Evaluate(const LineCalibration_Store *, const uint16_t raw[3], bool lights, const LineFollow_Geometry *, LineFollow_Route *)` | Transforma raw de tres sensores y perfiles en patrón, oscuridad y error lateral. | `f4:Core/Inc/line_follow_control_types.h:79`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-clampf"></a>`static float clampf(float x, float lo, float hi)` | Operación clampf de Calibración de línea y seguidor; semántica fina pendiente de ampliación. | `f4:Core/Src/line_follow_control.c:7`; inferred-name |
| <a id="fn-maxf"></a>`static float maxf(float a, float b)` | Operación maxf de Calibración de línea y seguidor; semántica fina pendiente de ampliación. | `f4:Core/Src/line_follow_control.c:8`; inferred-name |
| <a id="fn-median32"></a>`static uint16_t median32(const volatile uint16_t values[32][3], unsigned channel)` | Operación median32 de Calibración de línea y seguidor; semántica fina pendiente de ampliación. | `f4:Core/Src/line_follow_control.c:9`; inferred-name |
| <a id="fn-cal_finish"></a>`static void cal_finish(LineCalibration_Handle *h, LineCalibration_State state, uint8_t reason)` | Adapta la llamada a `restore` con el contexto de esta integración. | `f4:Core/Src/line_follow_control.c:28`; structural |
| <a id="fn-cal_phase"></a>`static void cal_phase(LineCalibration_Handle *h, uint32_t now)` | Coordina `lights`, `memset` para Calibración de línea y seguidor. | `f4:Core/Src/line_follow_control.c:47`; structural |
| <a id="fn-cal_pose_ok"></a>`static bool cal_pose_ok(const LineCalibration_Pose *p, uint32_t now)` | Coordina `fabsf`, `isfinite` para Calibración de línea y seguidor. | `f4:Core/Src/line_follow_control.c:68`; structural |
| <a id="fn-cal_quality"></a>`static bool cal_quality(const LineCalibration_Handle *h, const LineCalibration_Store *next)` | Coordina `fabsf`, `maxf` para Calibración de línea y seguidor. | `f4:Core/Src/line_follow_control.c:73`; structural |
| <a id="fn-line_fault"></a>`static void line_fault(LineFollowControl_Handle *h,uint8_t reason,uint32_t now)` | Adapta la llamada a `LineFollowControl_SetEnabled` con el contexto de esta integración. | `f4:Core/Src/line_follow_control.c:206`; structural |
