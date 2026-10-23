# Balance de inclinación

ID: `f4:module/Core/Inc/balance_control.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Convierte inclinación frontal filtrada y objetivo angular en esfuerzo motor porcentual común.

Configura el PID inyectado, tiempo, límites e inversión de salida. No accede a I2C ni a motores.

<a id="modularity"></a>
## Modularidad observada

Desacoplado de la implementación PID a través de AM_PidOps; comparte sus modelos numéricos.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

BalanceControl_Handle conserva config/status; config.pid_ops y pid_ctx apuntan al PID externo. status expone referencia, medida, contribuciones y enabled. La aplicación es dueña de ambos handles.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init configura el PID externo; SetEnabled y Reset controlan acumulación/historia.
- Update recibe pitch en grados y reloj en ms, calcula dt y consulta salida PID.

## Trabajo en tándem

- MotionControl desplaza target_pitch_deg para marcha y mezcla este esfuerzo común con el giro diferencial.

## Precauciones y límites

- Un BalanceControl enabled no acredita que el robot esté armado o que la muestra sea fresca: esas guardas las aplica MotionControl.
- No compartir pid_ctx entre controles activos sin una política explícita.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/balance_control.h` | `balance_control_types.h` | `39d23b7229adb721baedd4fcb25cb6ea73cb6f35a050d87fd862fb31475abf13` |
| `Core/Inc/balance_control_types.h` | `control_ports.h`, `stdint.h`, `balance_control_data.h` | `4c8fcd62b24e3b891c2cd64c6030aa2d7ef7e7a1db9821c757beb49564d7ca7a` |
| `Core/Inc/balance_control_data.h` | `am_port.h` | `cbe01b2ab9e3250870ffc4f40c067b38f2ec853f51de80a5d91b7c97f37aee84` |
| `Core/Src/balance_control.c` | `balance_control.h`, `stddef.h`, `math.h`, `string.h` | `891e8e66b60033743037bfb2fe2b45aeb38ad56e1918acd412176e218f026673` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `BalanceControl_Handle`

`f4:Core/Inc/balance_control_types.h:20`

| Atributo / firma | Función y precaución |
| --- | --- |
| `BalanceControl_Config_t config` | Configuración copiada por la instancia; los punteros que contiene siguen siendo prestados. |
| `BalanceControl_Status_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |

### `BalanceControl_Config_t`

`f4:Core/Inc/balance_control_data.h:14`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const struct AM_PidOps *pid_ops` | Tabla prestada de operaciones del PID; evita enlazar el balance a una implementación concreta. |
| `void *pid_ctx` | Instancia PID externa emparejada con pid_ops. |
| `float kp` | Dato de estado/configuración `kp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float ki` | Dato de estado/configuración `ki`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float kd` | Dato de estado/configuración `kd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float default_dt_s` | Dato de estado/configuración `default_dt_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float output_limit_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `float integral_limit_deg_s` | Dato de estado/configuración `integral_limit_deg_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float derivative_filter_tau_s` | Dato de estado/configuración `derivative_filter_tau_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float anti_windup_gain` | Dato de estado/configuración `anti_windup_gain`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t output_inverted` | Dato de estado/configuración `output_inverted`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BalanceControl_Status_t`

`f4:Core/Inc/balance_control_data.h:28`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float target_pitch_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float measured_pitch_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float error_deg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `float integral_deg_s` | Dato de estado/configuración `integral_deg_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float derivative_deg_s` | Dato de estado/configuración `derivative_deg_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float output_percent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `uint32_t last_update_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t has_previous` | Dato de estado/configuración `has_previous`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t enabled` | Dato de estado/configuración `enabled`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-balancecontrol_getdefaultconfig"></a>`void BalanceControl_GetDefaultConfig(BalanceControl_Config_t *config)` | Carga ganancias y límites predeterminados del balance. | `f4:Core/Inc/balance_control.h:7`; editorial |
| <a id="fn-balancecontrol_getoutputpercent"></a>`float BalanceControl_GetOutputPercent(const BalanceControl_Handle *balance)` | Consulta esfuerzo porcentual calculado. | `f4:Core/Inc/balance_control.h:21`; editorial |
| <a id="fn-balancecontrol_getstatus"></a>`const BalanceControl_Status_t *BalanceControl_GetStatus(const BalanceControl_Handle *balance)` | Devuelve vista prestada del estado de balance. | `f4:Core/Inc/balance_control.h:22`; editorial |
| <a id="fn-balancecontrol_init"></a>`AM_Result BalanceControl_Init(BalanceControl_Handle *balance, const BalanceControl_Config_t *config)` | Configura el balance y su PID externo a partir del puerto recibido. | `f4:Core/Inc/balance_control.h:8`; editorial |
| <a id="fn-balancecontrol_reset"></a>`void BalanceControl_Reset(BalanceControl_Handle *balance)` | Reinicia memoria de balance y PID. | `f4:Core/Inc/balance_control.h:10`; editorial |
| <a id="fn-balancecontrol_setenabled"></a>`void BalanceControl_SetEnabled(BalanceControl_Handle *balance, uint8_t enabled)` | Habilita o deshabilita cálculo del balance. | `f4:Core/Inc/balance_control.h:11`; editorial |
| <a id="fn-balancecontrol_setpid"></a>`void BalanceControl_SetPid(BalanceControl_Handle *balance, float kp, float ki, float kd)` | Actualiza ganancias PID del balance. | `f4:Core/Inc/balance_control.h:14`; editorial |
| <a id="fn-balancecontrol_settargetpitchdeg"></a>`void BalanceControl_SetTargetPitchDeg(BalanceControl_Handle *balance, float target_pitch_deg)` | Configura inclinación frontal objetivo en grados. | `f4:Core/Inc/balance_control.h:12`; editorial |
| <a id="fn-balancecontrol_update"></a>`AM_Result BalanceControl_Update(BalanceControl_Handle *balance, float measured_pitch_deg, uint32_t now_ms)` | Calcula esfuerzo común a partir de pitch y tiempo actuales. | `f4:Core/Inc/balance_control.h:18`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-balance_clamp"></a>`static float Balance_Clamp(float value, float min_value, float max_value)` | Operación Balance Clamp de Balance de inclinación; semántica fina pendiente de ampliación. | `f4:Core/Src/balance_control.c:14`; inferred-name |
| <a id="fn-balance_abs"></a>`static float Balance_Abs(float value)` | Operación Balance Abs de Balance de inclinación; semántica fina pendiente de ampliación. | `f4:Core/Src/balance_control.c:25`; inferred-name |
