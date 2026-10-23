# Motores y tracción diferencial

ID: `f4:module/Core/Inc/motor_control.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Traduce porcentajes y dirección a los dos canales PWM de cada motor DRV8833.

Control de actuadores y prueba directa; no calcula equilibrio, trayectoria, frescura MPU ni permisos.

<a id="modularity"></a>
## Modularidad observada

Hardware aislado por start y set_compare. Conserva semántica DRV8833 y dos canales por motor.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Motor_Handle_t conserva recurso opaco htim, Motor_Port prestado, canales, mínimos por dirección, máximo y última consigna. MotorControl_Handle agrupa leftMotor/rightMotor y motorData para TEST. Los atributos describen orden aplicada, no velocidad medida.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Motor_Init configura canales y límites; Motor_Start habilita PWM a través del puerto.
- Los setters limitan porcentajes y escriben compares; Stop y Brake representan acciones distintas sobre el puente.

## Trabajo en tándem

- AM_DriveOps conecta el árbitro MotionControl a ambos motores.
- La aplicación reserva control PWM directo para TEST y aplica permisos/guardas antes de invocarlo.

## Precauciones y límites

- Un retorno aceptado no demuestra movimiento físico.
- PWM mínimo compensa zona muerta; porcentaje no equivale a cm/s. AutoMotion usa un perfil aparte.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/motor_control.h` | `motor_control_types.h` | `faf5cd2eade8b7a5ec9bc20606b6debb1aa1829c321286a613be9b2795b6f689` |
| `Core/Inc/motor_control_types.h` | `am_port.h`, `types/bitmap_type.h`, `stdint.h` | `ee595e7e47e784f26ae3edd24eb35a16a88fabfea2637deeb343daae8566551f` |
| `Core/Src/motor_control.c` | `motor_control.h`, `stddef.h`, `math.h` | `dda5288c2aecf76886696fcf983b3be8586a7de3bfa4a53a36afc94a231484c2` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `Motor_Config_t`

`f4:Core/Inc/motor_control_types.h:36`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float speedPercent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `Motor_Direction_t motorDirection` | Dato de estado/configuración `motorDirection`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Motor_Data_t`

`f4:Core/Inc/motor_control_types.h:41`

| Atributo / firma | Función y precaución |
| --- | --- |
| `Byte_Flag_Struct flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `Motor_Config_t motorLeft` | Dato de estado/configuración `motorLeft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `Motor_Config_t motorRight` | Dato de estado/configuración `motorRight`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Motor_Port`

`f4:Core/Inc/motor_control_types.h:47`

| Atributo / firma | Función y precaución |
| --- | --- |
| `AM_Result (*start)(void *ctx, uint32_t channel)` | Operación start de Motores y tracción diferencial; semántica fina pendiente de ampliación. |
| `void (*set_compare)(void *ctx, uint32_t channel, uint16_t value)` | Escribe el compare del canal PWM; no aplica por sí mismo las guardas de movimiento. |

### `Motor_InitConfig_t`

`f4:Core/Inc/motor_control_types.h:52`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *htim` | Recurso opaco del PWM; la aplicación STM32 entrega el timer HAL. |
| `const Motor_Port *port` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `uint32_t forward_channel` | Dato de estado/configuración `forward_channel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t backward_channel` | Dato de estado/configuración `backward_channel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t min_pwm_forward` | Dato de estado/configuración `min_pwm_forward`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t min_pwm_backward` | Dato de estado/configuración `min_pwm_backward`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t max_pwm` | Dato de estado/configuración `max_pwm`; declaración estructural, interpretación específica pendiente de ampliar. |

### `Motor_Handle_t`

`f4:Core/Inc/motor_control_types.h:62`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *htim` | Recurso opaco del PWM; la aplicación STM32 entrega el timer HAL. |
| `const Motor_Port *port` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `uint32_t forward_channel` | Identificador de canal PWM usado para avance. |
| `uint32_t backward_channel` | Identificador de canal PWM usado para retroceso. |
| `uint16_t min_pwm_forward` | Compare mínimo aplicado al avance para compensar zona muerta. |
| `uint16_t min_pwm_backward` | Compare mínimo aplicado al retroceso para compensar zona muerta. |
| `uint16_t max_pwm` | Compare máximo de conversión porcentual; debe corresponder a la configuración del timer. |
| `float speedPercent` | Magnitud porcentual; el rango y signo se fijan en la API de esta librería. |
| `Motor_Direction_t direction` | Sentido lógico de la última orden; no mide dirección física real. |

### `MotorDrive_InitConfig_t`

`f4:Core/Inc/motor_control_types.h:74`

| Atributo / firma | Función y precaución |
| --- | --- |
| `Motor_InitConfig_t left` | Dato de estado/configuración `left`; declaración estructural, interpretación específica pendiente de ampliar. |
| `Motor_InitConfig_t right` | Dato de estado/configuración `right`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MotorControl_Handle`

`f4:Core/Inc/motor_control_types.h:79`

| Atributo / firma | Función y precaución |
| --- | --- |
| `Motor_Handle_t leftMotor` | Handle del actuador izquierdo, con canales y límites propios. |
| `Motor_Handle_t rightMotor` | Handle del actuador derecho, con canales y límites propios. |
| `Motor_Data_t motorData` | Consignas/flags del modo de prueba directa, separadas de medida física. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-motordrive_applyteststate"></a>`void MotorDrive_ApplyTestState(MotorControl_Handle *drive)` | Traslada motorData a PWM directo de ambos motores para TEST. | `f4:Core/Inc/motor_control.h:44`; editorial |
| <a id="fn-motordrive_init"></a>`AM_Result MotorDrive_Init(MotorControl_Handle *drive, const MotorDrive_InitConfig_t *config)` | Configura los motores o la pareja de tracción con canales y límites suministrados. | `f4:Core/Inc/motor_control.h:27`; editorial |
| <a id="fn-motordrive_setbothpercent"></a>`void MotorDrive_SetBothPercent(MotorControl_Handle *drive, float speed_percent, Motor_Direction_t direction)` | Aplica la misma magnitud y dirección a ambos motores. | `f4:Core/Inc/motor_control.h:36`; editorial |
| <a id="fn-motordrive_setleftminpwm"></a>`void MotorDrive_SetLeftMinPwm(MotorControl_Handle *drive, uint16_t min_pwm)` | Actualiza el mínimo PWM del motor izquierdo. | `f4:Core/Inc/motor_control.h:42`; editorial |
| <a id="fn-motordrive_setleftpercent"></a>`void MotorDrive_SetLeftPercent(MotorControl_Handle *drive, float speed_percent, Motor_Direction_t direction)` | Aplica magnitud y dirección al motor izquierdo. | `f4:Core/Inc/motor_control.h:30`; editorial |
| <a id="fn-motordrive_setrightminpwm"></a>`void MotorDrive_SetRightMinPwm(MotorControl_Handle *drive, uint16_t min_pwm)` | Actualiza el mínimo PWM del motor derecho. | `f4:Core/Inc/motor_control.h:43`; editorial |
| <a id="fn-motordrive_setrightpercent"></a>`void MotorDrive_SetRightPercent(MotorControl_Handle *drive, float speed_percent, Motor_Direction_t direction)` | Aplica magnitud y dirección al motor derecho. | `f4:Core/Inc/motor_control.h:33`; editorial |
| <a id="fn-motordrive_setsignedpercent"></a>`void MotorDrive_SetSignedPercent(MotorControl_Handle *drive, float left_percent, float right_percent)` | Aplica porcentajes firmados; el signo selecciona el sentido de cada motor. | `f4:Core/Inc/motor_control.h:39`; editorial |
| <a id="fn-motordrive_stopall"></a>`void MotorDrive_StopAll(MotorControl_Handle *drive)` | Detiene ambos motores de la tracción. | `f4:Core/Inc/motor_control.h:29`; editorial |
| <a id="fn-motor_backward"></a>`AM_Result Motor_Backward(Motor_Handle_t *motor, float speed_percent)` | Aplica retroceso con magnitud porcentual. | `f4:Core/Inc/motor_control.h:15`; editorial |
| <a id="fn-motor_brake"></a>`void Motor_Brake(Motor_Handle_t *motor)` | Aplica la combinación de canales usada para frenado. | `f4:Core/Inc/motor_control.h:17`; editorial |
| <a id="fn-motor_clampspeedpercent"></a>`float Motor_ClampSpeedPercent(float speed_percent)` | Limita la magnitud de velocidad al rango porcentual del driver. | `f4:Core/Inc/motor_control.h:24`; editorial |
| <a id="fn-motor_forward"></a>`AM_Result Motor_Forward(Motor_Handle_t *motor, float speed_percent)` | Aplica avance con magnitud porcentual. | `f4:Core/Inc/motor_control.h:14`; editorial |
| <a id="fn-motor_init"></a>`AM_Result Motor_Init(Motor_Handle_t *motor, const Motor_InitConfig_t *config)` | Configura los motores o la pareja de tracción con canales y límites suministrados. | `f4:Core/Inc/motor_control.h:7`; editorial |
| <a id="fn-motor_setminpwm"></a>`void Motor_SetMinPwm(Motor_Handle_t *motor, uint16_t min_pwm_forward, uint16_t min_pwm_backward)` | Actualiza los mínimos PWM de avance y retroceso. | `f4:Core/Inc/motor_control.h:18`; editorial |
| <a id="fn-motor_setsignedpercent"></a>`AM_Result Motor_SetSignedPercent(Motor_Handle_t *motor, float signed_percent)` | Aplica porcentajes firmados; el signo selecciona el sentido de cada motor. | `f4:Core/Inc/motor_control.h:12`; editorial |
| <a id="fn-motor_setspeedpercent"></a>`AM_Result Motor_SetSpeedPercent(Motor_Handle_t *motor, float speed_percent, Motor_Direction_t direction)` | Aplica magnitud porcentual limitada y dirección explícita. | `f4:Core/Inc/motor_control.h:9`; editorial |
| <a id="fn-motor_speedpercenttodisplay"></a>`uint8_t Motor_SpeedPercentToDisplay(float speed_percent)` | Convierte el porcentaje a un valor entero para presentación. | `f4:Core/Inc/motor_control.h:25`; editorial |
| <a id="fn-motor_speedpercenttopwm"></a>`uint16_t Motor_SpeedPercentToPwm(const Motor_Handle_t *motor, float speed_percent, Motor_Direction_t direction)` | Convierte porcentaje a PWM usando mínimo direccional y máximo configurado. | `f4:Core/Inc/motor_control.h:21`; editorial |
| <a id="fn-motor_start"></a>`AM_Result Motor_Start(Motor_Handle_t *motor)` | Inicia los canales PWM del motor mediante el puerto externo. | `f4:Core/Inc/motor_control.h:8`; editorial |
| <a id="fn-motor_stop"></a>`void Motor_Stop(Motor_Handle_t *motor)` | Lleva ambos canales a parada libre. | `f4:Core/Inc/motor_control.h:16`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-motor_normalizemaxpwm"></a>`static uint16_t Motor_NormalizeMaxPwm(uint16_t max_pwm)` | Operación Normalize Max Pwm de Motores y tracción diferencial; semántica fina pendiente de ampliación. | `f4:Core/Src/motor_control.c:12`; inferred-name |
| <a id="fn-motor_abspercent"></a>`static float Motor_AbsPercent(float signed_percent)` | Operación Abs Percent de Motores y tracción diferencial; semántica fina pendiente de ampliación. | `f4:Core/Src/motor_control.c:17`; inferred-name |
