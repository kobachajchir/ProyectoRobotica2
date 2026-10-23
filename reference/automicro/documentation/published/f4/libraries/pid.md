# Controlador PID

ID: `f4:module/Core/Inc/pid_controller.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Calcula control proporcional, integral y derivativo con límites, filtro derivativo y anti-windup.

Cálculo escalar con medida y dt entregados por el llamador; no adquiere sensores ni manda PWM.

<a id="modularity"></a>
## Modularidad observada

Solo tipos estándar, AM_Result y matemáticas; no conoce balance, motores o MPU.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

PidController_Handle contiene config y status. Status conserva error, contribuciones, salida raw/limitada, historia y saturación. GetStatus devuelve una vista const prestada, no una copia inmutable en el tiempo.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- GetDefaultConfig carga límites y dt predeterminados; Init prepara la instancia.
- Update recibe valores/tiempo; Reset limpia memoria del controlador para evitar reutilizar acumulación anterior.

## Trabajo en tándem

- BalanceControl usa AM_PidOps; la aplicación conserva la instancia PID separada de BalanceControl_Handle.

## Precauciones y límites

- dt en segundos; no entregar milisegundos directamente.
- Cambiar ganancias o límites no implica reinicio de historia salvo que la API lo haga expresamente.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/pid_controller.h` | `pid_controller_types.h` | `d43c8c4d826b2b28fa0e2ecbd912ed5f56dfc80d60c54a6822a9667e80f56bc2` |
| `Core/Inc/pid_controller_types.h` | `am_port.h`, `stdint.h`, `pid_controller_data.h` | `a79edcdb8a21158a405bd95e57a08cb92eba17b9c5a08f404d7876ff762548ee` |
| `Core/Inc/pid_controller_data.h` | `am_port.h` | `cc6759dc651cb0f851b1c4e26895e6ad3baa0ce04f72b5de121439e40f231308` |
| `Core/Src/pid_controller.c` | `pid_controller.h`, `stddef.h`, `string.h` | `7b82e742aafd092a7bf74f655d94b8b62a99da20e5b7e2c749470a8ed1ed67ce` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `PidController_Handle`

`f4:Core/Inc/pid_controller_types.h:20`

| Atributo / firma | Función y precaución |
| --- | --- |
| `PidController_Config_t config` | Configuración copiada por la instancia; los punteros que contiene siguen siendo prestados. |
| `PidController_Status_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |

### `PidController_Config_t`

`f4:Core/Inc/pid_controller_data.h:12`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float kp` | Dato de estado/configuración `kp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float ki` | Dato de estado/configuración `ki`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float kd` | Dato de estado/configuración `kd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float default_dt_s` | Dato de estado/configuración `default_dt_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float output_min` | Dato de estado/configuración `output_min`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float output_max` | Dato de estado/configuración `output_max`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float integral_min` | Dato de estado/configuración `integral_min`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float integral_max` | Dato de estado/configuración `integral_max`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float derivative_filter_tau_s` | Dato de estado/configuración `derivative_filter_tau_s`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float anti_windup_gain` | Dato de estado/configuración `anti_windup_gain`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PidController_Status_t`

`f4:Core/Inc/pid_controller_data.h:25`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float setpoint` | Referencia escalar configurada antes del cálculo. |
| `float measurement` | Medida escalar usada en la última actualización. |
| `float error` | Diferencia de control entre referencia y medida. |
| `float proportional` | Contribución proporcional calculada. |
| `float integral` | Contribución integral acumulada y limitada. |
| `float derivative` | Contribución derivativa filtrada. |
| `float output_raw` | Salida antes de limitar al rango configurado. |
| `float output` | Salida final limitada que consumen las capas superiores. |
| `float last_measurement` | Memoria de la observación anterior usada por la máquina de estados. |
| `float last_error` | Memoria de la observación anterior usada por la máquina de estados. |
| `float dt_s` | Intervalo usado por el cálculo en segundos. |
| `uint8_t initialized` | Marca de inicialización local; no acredita disponibilidad de dispositivos externos. |
| `uint8_t saturated` | Indica que la salida alcanzó límites del controlador. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-pidcontroller_getdefaultconfig"></a>`void PidController_GetDefaultConfig(PidController_Config_t *config)` | Carga ganancias, límites, dt y filtro predeterminados. | `f4:Core/Inc/pid_controller.h:7`; editorial |
| <a id="fn-pidcontroller_getoutput"></a>`float PidController_GetOutput(const PidController_Handle *pid)` | Consulta la última salida limitada. | `f4:Core/Inc/pid_controller.h:26`; editorial |
| <a id="fn-pidcontroller_getstatus"></a>`const PidController_Status_t *PidController_GetStatus(const PidController_Handle *pid)` | Devuelve vista const prestada del estado del cálculo. | `f4:Core/Inc/pid_controller.h:27`; editorial |
| <a id="fn-pidcontroller_init"></a>`AM_Result PidController_Init(PidController_Handle *pid, const PidController_Config_t *config)` | Inicializa configuración e historia de la instancia PID. | `f4:Core/Inc/pid_controller.h:8`; editorial |
| <a id="fn-pidcontroller_reset"></a>`void PidController_Reset(PidController_Handle *pid)` | Limpia términos e historia del controlador. | `f4:Core/Inc/pid_controller.h:10`; editorial |
| <a id="fn-pidcontroller_setgains"></a>`void PidController_SetGains(PidController_Handle *pid, float kp, float ki, float kd)` | Actualiza Kp, Ki y Kd. | `f4:Core/Inc/pid_controller.h:11`; editorial |
| <a id="fn-pidcontroller_setintegrallimits"></a>`void PidController_SetIntegralLimits(PidController_Handle *pid, float integral_min, float integral_max)` | Actualiza el intervalo permitido para el término integral. | `f4:Core/Inc/pid_controller.h:18`; editorial |
| <a id="fn-pidcontroller_setoutputlimits"></a>`void PidController_SetOutputLimits(PidController_Handle *pid, float output_min, float output_max)` | Actualiza el intervalo permitido para salida. | `f4:Core/Inc/pid_controller.h:15`; editorial |
| <a id="fn-pidcontroller_setsetpoint"></a>`void PidController_SetSetpoint(PidController_Handle *pid, float setpoint)` | Configura la referencia que comparará la siguiente actualización. | `f4:Core/Inc/pid_controller.h:21`; editorial |
| <a id="fn-pidcontroller_update"></a>`AM_Result PidController_Update(PidController_Handle *pid, float setpoint, float measurement, float dt_s)` | Calcula términos PID y salida limitada con la medida y dt recibidos. | `f4:Core/Inc/pid_controller.h:22`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-pid_clamp"></a>`static float Pid_Clamp(float value, float min_value, float max_value)` | Operación Pid Clamp de Controlador PID; semántica fina pendiente de ampliación. | `f4:Core/Src/pid_controller.c:14`; inferred-name |
| <a id="fn-pid_abs"></a>`static float Pid_Abs(float value)` | Operación Pid Abs de Controlador PID; semántica fina pendiente de ampliación. | `f4:Core/Src/pid_controller.c:25`; inferred-name |
| <a id="fn-pid_normalizeconfig"></a>`static void Pid_NormalizeConfig(PidController_Config_t *config)` | Operación Pid Normalize Config de Controlador PID; semántica fina pendiente de ampliación. | `f4:Core/Src/pid_controller.c:30`; inferred-name |
