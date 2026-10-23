# GOTO y calibración de motores

ID: `f4:module/Core/Inc/auto_motion.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Planifica desplazamientos GOTO relativos y calibra un perfil de avance del vehículo.

Perfil de cinco puntos PWM/velocidad/rampa y cinco canales IR, estimación de avance y manejo de obstáculos. No implementa odometría de ruedas ni un mapa global.

<a id="modularity"></a>
## Modularidad observada

Sin HAL y sin drivers concretos; callbacks de aplicación conectan sensores y movimiento. Los algoritmos y formatos siguen siendo del dominio AutoMicro.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

AutoMotion_Handle posee copia del puerto, perfil, último input, objetivo, posición estimada, fases, tiempos y métricas. read copia guardas/IMU/PWM/IR; command pide forward/turn con tiempo; hold detiene; save persiste AutoMotion_Profile. La serialización pública define 128 bytes de estado, 88 de perfil y 186 de calibración IR, distintos del sizeof del struct.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init carga perfil persistido; Goto/Calibrate admiten una operación o devuelven razón de rechazo.
- Task usa read, evolución temporal y command para avanzar; Cancel termina y solicita hold.
- SetProfile/SetIr decodifican formatos binarios explícitos; State/GetProfile construyen salidas de protocolo.

## Trabajo en tándem

- APP_AutoCommand traduce a MotionControl; APP_AutoRead reúne IMU, modo, motores e IR.
- APP_AutoSave rechaza guardado en situaciones activas y persiste en preferencias.
- ESP codifica las solicitudes 0x23..0x2A; Web presenta la misma misión y sus requisitos.

## Precauciones y límites

- Posición y velocidad son estimadas por modelo calibrado, no medidas absolutas.
- Comprobar siempre reason y result_valid; recibir una respuesta de 128 bytes no implica éxito.
- La calibración física y la exactitud de trayectoria no se comprobaron en esta entrega.

## Hallazgos vinculados

- [AMDOC-F4-003: La marca RESP permite longitudes fuera del rango de solicitud](../../../review/FINDINGS.md#amdoc-f4-003)
- [AMDOC-ESP-001: El validador AutoMotion acepta formas de argumentos de otros comandos](../../../review/FINDINGS.md#amdoc-esp-001)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/auto_motion.h` | `stdbool.h`, `stdint.h` | `c08909b2b723ceef1738cd050afa1908bd330482bd9de4c4179f2afb952b69a5` |
| `Core/Src/auto_motion.c` | `auto_motion.h`, `math.h`, `string.h` | `bb994b5b177e792e2a1a01da5bbd2a0a69719d545109b07fcec8aaed728e975b` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `AutoMotion_Point`

`f4:Core/Inc/auto_motion.h:9`

| Atributo / firma | Función y precaución |
| --- | --- |
| `float percent, speed_cm_s` | Velocidad calibrada del punto en centímetros por segundo. |
| `uint32_t ramp_ms, runs` | Dato de estado/configuración `runs`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AutoMotion_IrPoint`

`f4:Core/Inc/auto_motion.h:10`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t distance_mm, adc` | Dato de estado/configuración `adc`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AutoMotion_IrChannel`

`f4:Core/Inc/auto_motion.h:11`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t baseline, noise` | Dato de estado/configuración `noise`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t count` | Dato de estado/configuración `count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AutoMotion_IrPoint points[8]` | Dato de estado/configuración `points`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AutoMotion_Profile`

`f4:Core/Inc/auto_motion.h:12`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t revision` | Dato de estado/configuración `revision`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t count, ir_valid` | Dato de estado/configuración `ir_valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AutoMotion_Point points[AUTO_MOTION_POINTS]` | Dato de estado/configuración `points`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AutoMotion_IrChannel ir[5]` | Dato de estado/configuración `ir`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AutoMotion_Input`

`f4:Core/Inc/auto_motion.h:17`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t now_ms, imu_seq` | Dato de estado/configuración `imu_seq`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float yaw_deg, left_duty, right_duty, pwm_hz` | Dato de estado/configuración `pwm_hz`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t objects[5]` | Cinco lecturas raw ADC de sensores de objetos. |
| `uint8_t mode, mpu_ready, mpu_calibrated, armed, balance, settled, safety` | Dato de estado/configuración `safety`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t line_ready, line_fresh, center_black, objects_fresh, objects_enabled, lights` | Dato de estado/configuración `lights`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AutoMotion_Port`

`f4:Core/Inc/auto_motion.h:24`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `bool (*read)(void *, AutoMotion_Input *)` | Copia estado coherente de modo, IMU, actuadores, IR y guardas. |
| `bool (*command)(void *, float, float, uint32_t)` | Solicita avance/giro al árbitro y devuelve si fue aceptado. |
| `void (*hold)(void *)` | Solicita parada segura de la operación al integrador. |
| `bool (*save)(void *, const AutoMotion_Profile *)` | Solicita persistir un perfil de motores; el retorno expresa éxito de almacenamiento. |

### `AutoMotion_Handle`

`f4:Core/Inc/auto_motion.h:31`

| Atributo / firma | Función y precaución |
| --- | --- |
| `AutoMotion_Port port` | Tabla copiada de lectura, orden, hold y persistencia. |
| `AutoMotion_Profile profile` | Copia del modelo calibrado de motores y cinco canales IR. |
| `AutoMotion_Input input` | Último snapshot leído desde la aplicación. |
| `uint8_t kind, state, phase, reason, obstacle_mask, persisted, result_valid, black_candidate` | Dato de estado/configuración `black_candidate`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t id, started_ms, last_ms, measure_ms, measured_ms, ramp_ms, timeout_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t active_ms[2], forward_ms[2], reverse_ms[2], candidate_ms, last_imu_seq` | Memoria de la observación anterior usada por la máquina de estados. |
| `float x, y, goal_x, goal_y, origin_yaw, heading, forward, speed` | Dato de estado/configuración `speed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float duty_seconds[2], max_duty[2], cycles[2], yaw_error_max` | Dato de estado/configuración `yaw_error_max`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float avoid_heading, avoid_x, avoid_y` | Dato de estado/configuración `avoid_y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t avoid_since, last_progress_ms, drive_elapsed` | Dato de estado/configuración `drive_elapsed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float applied_forward` | Dato de estado/configuración `applied_forward`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float best_distance` | Dato de estado/configuración `best_distance`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-automotion_busy"></a>`bool AutoMotion_Busy(const AutoMotion_Handle *)` | Indica si una misión o calibración permanece activa. | `f4:Core/Inc/auto_motion.h:48`; editorial |
| <a id="fn-automotion_calibrate"></a>`uint8_t AutoMotion_Calibrate(AutoMotion_Handle *, uint8_t operation, float percent, uint32_t ramp_ms, uint32_t timeout_ms)` | Procesa operación de calibración con porcentaje, rampa y timeout. | `f4:Core/Inc/auto_motion.h:52`; editorial |
| <a id="fn-automotion_cancel"></a>`void AutoMotion_Cancel(AutoMotion_Handle *)` | Cancela la operación activa y solicita hold. | `f4:Core/Inc/auto_motion.h:49`; editorial |
| <a id="fn-automotion_getprofile"></a>`void AutoMotion_GetProfile(AutoMotion_Handle *, uint8_t *, uint8_t request_reason)` | Serializa perfil en el buffer de salida de 88 bytes. | `f4:Core/Inc/auto_motion.h:56`; editorial |
| <a id="fn-automotion_goto"></a>`uint8_t AutoMotion_Goto(AutoMotion_Handle *, float x_cm, float y_cm)` | Intenta iniciar un objetivo relativo x/y en centímetros y devuelve razón. | `f4:Core/Inc/auto_motion.h:51`; editorial |
| <a id="fn-automotion_init"></a>`void AutoMotion_Init(AutoMotion_Handle *, const AutoMotion_Port *, const AutoMotion_Profile *)` | Inicializa misión, callbacks y perfil de calibración. | `f4:Core/Inc/auto_motion.h:47`; editorial |
| <a id="fn-automotion_setir"></a>`uint8_t AutoMotion_SetIr(AutoMotion_Handle *, const uint8_t *, uint8_t)` | Decodifica y aplica el bloque de calibración IR del protocolo. | `f4:Core/Inc/auto_motion.h:53`; editorial |
| <a id="fn-automotion_setprofile"></a>`uint8_t AutoMotion_SetProfile(AutoMotion_Handle *, const uint8_t *, uint8_t)` | Decodifica y aplica el perfil de motores serializado. | `f4:Core/Inc/auto_motion.h:54`; editorial |
| <a id="fn-automotion_state"></a>`void AutoMotion_State(AutoMotion_Handle *, uint8_t *, uint8_t request_reason)` | Serializa estado de misión/calibración en el buffer de salida de 128 bytes. | `f4:Core/Inc/auto_motion.h:55`; editorial |
| <a id="fn-automotion_task"></a>`void AutoMotion_Task(AutoMotion_Handle *)` | Avanza misión/calibración con sensores, tiempos y guardas actuales. | `f4:Core/Inc/auto_motion.h:50`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-clamp"></a>`static float clamp(float v,float lo,float hi)` | Coordina `fmaxf`, `fminf` para GOTO y calibración de motores. | `f4:Core/Src/auto_motion.c:4`; structural |
| <a id="fn-angle"></a>`static float angle(float a)` | Operación angle de GOTO y calibración de motores; semántica fina pendiente de ampliación. | `f4:Core/Src/auto_motion.c:5`; inferred-name |
| <a id="fn-u32"></a>`static uint32_t u32(const uint8_t *p)` | Operación u32 de GOTO y calibración de motores; semántica fina pendiente de ampliación. | `f4:Core/Src/auto_motion.c:6`; inferred-name |
| <a id="fn-u16"></a>`static uint16_t u16(const uint8_t *p)` | Operación u16 de GOTO y calibración de motores; semántica fina pendiente de ampliación. | `f4:Core/Src/auto_motion.c:7`; inferred-name |
| <a id="fn-f32"></a>`static float f32(const uint8_t *p)` | Adapta la llamada a `u32` con el contexto de esta integración. | `f4:Core/Src/auto_motion.c:8`; structural |
| <a id="fn-put32"></a>`static void put32(uint8_t *p,uint32_t v)` | Operación put32 de GOTO y calibración de motores; semántica fina pendiente de ampliación. | `f4:Core/Src/auto_motion.c:9`; inferred-name |
| <a id="fn-putf"></a>`static void putf(uint8_t *p,float f)` | Adapta la llamada a `put32` con el contexto de esta integración. | `f4:Core/Src/auto_motion.c:10`; structural |
| <a id="fn-end"></a>`static void end(AutoMotion_Handle *h,uint8_t state,uint8_t reason)` | Adapta la llamada a `hold` con el contexto de esta integración. | `f4:Core/Src/auto_motion.c:12`; structural |
| <a id="fn-ready"></a>`static uint8_t ready(AutoMotion_Handle *h,bool calibration)` | Coordina `AutoMotion_Busy`, `read` para GOTO y calibración de motores. | `f4:Core/Src/auto_motion.c:17`; structural |
| <a id="fn-obstacles"></a>`static bool obstacles(AutoMotion_Handle *h)` | Coordina `clamp`, `fabsf`, `fmaxf` para GOTO y calibración de motores. | `f4:Core/Src/auto_motion.c:28`; structural |
| <a id="fn-model"></a>`static float model(const AutoMotion_Handle *h,float percent)` | Operación model de GOTO y calibración de motores; semántica fina pendiente de ampliación. | `f4:Core/Src/auto_motion.c:47`; inferred-name |
| <a id="fn-begin"></a>`static void begin(AutoMotion_Handle *h,uint8_t kind,float percent,uint32_t ramp,uint32_t timeout)` | Coordina `memset` para GOTO y calibración de motores. | `f4:Core/Src/auto_motion.c:59`; structural |
| <a id="fn-save"></a>`static bool save(AutoMotion_Handle *h,const AutoMotion_Profile *next)` | Coordina `AutoMotion_Busy`, `save` para GOTO y calibración de motores. | `f4:Core/Src/auto_motion.c:78`; structural |
