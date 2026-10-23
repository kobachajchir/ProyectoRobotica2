# Árbitro de movimiento

ID: `f4:module/Core/Inc/motion_control.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Arbitra intención de marcha/giro, balance y seguridad antes de aplicar salidas diferenciales a motores.

Armado, desarmado, rechazo, rampas, duración, inclinación segura, frescura de orientación y enclavamiento de seguridad.

<a id="modularity"></a>
## Modularidad observada

Depende de operaciones y datos neutrales, sin llamar drivers concretos. El arbitraje y valores de fuente son semántica del robot.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

MotionControl_Handle posee config, command, status, consignas rampadas y secuencia procesada. config.motors/balance/mpu son contextos opacos de drive_ops/balance_ops/orientation. GetStatus expone guardas, motivo de rechazo, saturación y salida aplicada; command no es prueba de movimiento.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init establece dependencias y estado seguro; Arm exige condiciones del sensor y orientación.
- SetCommand registra intención temporal; Task revisa frescura/seguridad y calcula rampas, balance y mezcla.
- EmergencyStop enclava seguridad; no equivale a una parada ordinaria y no se revierte por enviar otra consigna.

## Trabajo en tándem

- LineFollow, Web, NRF y GOTO producen MotionControl_Command_t; la aplicación decide qué modo puede producirlo.
- El puerto de orientación conserva ready/calibrating/fresh y secuencia por separado.

## Precauciones y límites

- Stop con COAST/BRAKE, Disarm y EmergencyStop tienen efectos distintos.
- No llamar MotorDrive directo en modos normales para eludir guardas.
- duration_ms/ramp_up_ms/ramp_down_ms son uint16_t, con límites de representación propios.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/motion_control.h` | `motion_control_types.h` | `49179cee23b70cdfc3aea2132f3b3660e88da4d09d3a917c501800a0f44f8805` |
| `Core/Inc/motion_control_types.h` | `control_ports.h`, `stdint.h`, `motion_control_data.h` | `81539a15851ee9aee70b91016a963ce41f4f92a0175db175eb01e9e721645264` |
| `Core/Inc/motion_control_data.h` | `am_port.h` | `dce850f1fa3a58717a11c5a3baa20b74b905a306a7e5ae1e5b51c5b1844e44f5` |
| `Core/Src/motion_control.c` | `motion_control.h`, `utils/macros_utils.h`, `stddef.h`, `math.h`, `string.h` | `633d70bb7f3e910cf1f51240b38ba93ae310640957305adeb55060140e6e7a56` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `MotionControl_Handle`

`f4:Core/Inc/motion_control_types.h:20`

| Atributo / firma | Función y precaución |
| --- | --- |
| `MotionControl_Config_t config` | Configuración copiada por la instancia; los punteros que contiene siguen siendo prestados. |
| `MotionControl_Command_t command` | Dato de estado/configuración `command`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MotionControl_Status_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `float current_forward_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float current_turn_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `uint8_t initialized` | Marca de inicialización local; no acredita disponibilidad de dispositivos externos. |
| `uint16_t last_sample_seq` | Memoria de la observación anterior usada por la máquina de estados. |

### `MotionControl_Command_t`

`f4:Core/Inc/motion_control_data.h:40`

| Atributo / firma | Función y precaución |
| --- | --- |
| `MotionControl_Source_t source` | Productor lógico de la orden: balance, follow, Web, NRF, TEST, seguridad o GOTO. |
| `float forward_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float turn_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `uint16_t duration_ms` | Vigencia de la orden en milisegundos, almacenada como uint16_t. |
| `uint16_t ramp_up_ms` | Tiempo de subida de la consigna en milisegundos. |
| `uint16_t ramp_down_ms` | Tiempo de bajada de la consigna en milisegundos. |
| `MotionControl_StopMode_t end_mode` | Parada libre o frenada al terminar la orden. |

### `MotionControl_Config_t`

`f4:Core/Inc/motion_control_data.h:51`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *motors` | Contexto opaco de actuadores que interpreta drive_ops. |
| `void *balance` | Contexto opaco del balance que interpreta balance_ops. |
| `void *mpu` | Contexto opaco de orientación que interpreta orientation. |
| `const struct AM_DriveOps *drive_ops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `const struct AM_BalanceOps *balance_ops` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `const struct AM_OrientationOps *orientation` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `float neutral_pitch_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float max_pitch_offset_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float max_turn_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float safe_pitch_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float turn_headroom_ratio` | Dato de estado/configuración `turn_headroom_ratio`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t enabled` | Dato de estado/configuración `enabled`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MotionControl_Status_t`

`f4:Core/Inc/motion_control_data.h:66`

| Atributo / firma | Función y precaución |
| --- | --- |
| `MotionControl_Source_t source` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MotionControl_RejectReason_t reject_reason` | Motivo actual de rechazo o guarda del árbitro. |
| `float pitch_measured_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float pitch_setpoint_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float balance_output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float forward_applied_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float turn_applied_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float turn_requested_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float turn_limit_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float u_common_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float u_diff_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float left_output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float right_output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `uint32_t command_started_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t command_active` | Dato de estado/configuración `command_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t safety_stop` | Enclavamiento de parada de seguridad, distinto de stop ordinario. |
| `uint8_t balance_active` | Indica que balance participa en el control actual. |
| `uint8_t balance_settled` | Indica que se cumplen criterios de balance estabilizado de la aplicación. |
| `uint8_t armed` | Estado de armado que habilita aplicar movimiento. |
| `uint8_t saturation_limited` | Indica limitación de mezcla diferencial por margen disponible. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-motioncontrol_arm"></a>`void MotionControl_Arm(MotionControl_Handle *motion)` | Intenta armar el movimiento comprobando las guardas vigentes. | `f4:Core/Inc/motion_control.h:13`; editorial |
| <a id="fn-motioncontrol_disarm"></a>`void MotionControl_Disarm(MotionControl_Handle *motion)` | Desarma el movimiento y lleva la tracción a estado detenido. | `f4:Core/Inc/motion_control.h:14`; editorial |
| <a id="fn-motioncontrol_emergencystop"></a>`void MotionControl_EmergencyStop(MotionControl_Handle *motion)` | Aplica parada de seguridad con enclavamiento. | `f4:Core/Inc/motion_control.h:17`; editorial |
| <a id="fn-motioncontrol_getdefaultcommand"></a>`void MotionControl_GetDefaultCommand(MotionControl_Command_t *command)` | Carga una orden de movimiento neutra con valores predeterminados. | `f4:Core/Inc/motion_control.h:7`; editorial |
| <a id="fn-motioncontrol_getstatus"></a>`const MotionControl_Status_t *MotionControl_GetStatus(const MotionControl_Handle *motion)` | Devuelve vista del estado aplicado y del motivo de rechazo. | `f4:Core/Inc/motion_control.h:19`; editorial |
| <a id="fn-motioncontrol_init"></a>`AM_Result MotionControl_Init(MotionControl_Handle *motion, const MotionControl_Config_t *config)` | Conecta operaciones externas e inicializa el árbitro en estado seguro. | `f4:Core/Inc/motion_control.h:8`; editorial |
| <a id="fn-motioncontrol_isarmed"></a>`uint8_t MotionControl_IsArmed(const MotionControl_Handle *motion)` | Consulta el estado de armado del árbitro. | `f4:Core/Inc/motion_control.h:15`; editorial |
| <a id="fn-motioncontrol_setcommand"></a>`AM_Result MotionControl_SetCommand(MotionControl_Handle *motion, const MotionControl_Command_t *command, uint32_t now_ms)` | Evalúa y registra una intención de movimiento y su tiempo de inicio. | `f4:Core/Inc/motion_control.h:10`; editorial |
| <a id="fn-motioncontrol_stop"></a>`void MotionControl_Stop(MotionControl_Handle *motion, MotionControl_StopMode_t mode)` | Detiene la orden con la modalidad de parada recibida. | `f4:Core/Inc/motion_control.h:16`; editorial |
| <a id="fn-motioncontrol_task"></a>`void MotionControl_Task(MotionControl_Handle *motion, uint32_t now_ms)` | Actualiza seguridad, balance, rampas y mezcla diferencial con now_ms. | `f4:Core/Inc/motion_control.h:18`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-motion_clamp"></a>`static float Motion_Clamp(float value, float min_value, float max_value)` | Operación Motion Clamp de Árbitro de movimiento; semántica fina pendiente de ampliación. | `f4:Core/Src/motion_control.c:15`; inferred-name |
| <a id="fn-motion_abs"></a>`static float Motion_Abs(float value)` | Operación Motion Abs de Árbitro de movimiento; semántica fina pendiente de ampliación. | `f4:Core/Src/motion_control.c:26`; inferred-name |
| <a id="fn-motion_profilefactor"></a>`static float Motion_ProfileFactor(const MotionControl_Command_t *command, uint32_t start_ms, uint32_t now_ms, uint8_t *finished)` | Coordina `Motion_Clamp` para Árbitro de movimiento. | `f4:Core/Src/motion_control.c:32`; structural |
| <a id="fn-motion_stopoutputs"></a>`static void Motion_StopOutputs(MotionControl_Handle *motion, MotionControl_StopMode_t mode)` | Coordina `brake`, `stop` para Árbitro de movimiento. | `f4:Core/Src/motion_control.c:81`; structural |
| <a id="fn-motion_clearoutputstatus"></a>`static void Motion_ClearOutputStatus(MotionControl_Handle *motion)` | Operación Motion Clear Output Status de Árbitro de movimiento; semántica fina pendiente de ampliación. | `f4:Core/Src/motion_control.c:94`; inferred-name |
