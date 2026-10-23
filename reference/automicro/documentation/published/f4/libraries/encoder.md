# Encoder rotativo y pulsador integrado

ID: `f4:module/Core/Inc/encoder.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Convierte diferencias del contador de 16 bits en pasos, dirección, velocidad y aceleración del encoder.

Gestiona wrap, resto de pulsos y pulsador integrado; no conoce menús ni comandos remotos.

<a id="modularity"></a>
## Modularidad observada

No incluye HAL; read_count/reset_count/start/button_level aíslan el timer y GPIO. El sentido base del montaje está codificado dentro del cálculo: portable de plataforma, con convención física del proyecto.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

ENC_Handle_t posee puerto copiado, prevCount, accumCount, pasos actuales/anteriores y btnState. Sus atributos son públicos en C; los getters evitan que el consumidor deba reconstruir el cálculo. calibrateCountPerStep debe representar una cantidad válida de pulsos por paso.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init configura la instancia; Start inicia el contador y restablece su referencia.
- Task_N10ms calcula delta con corrección de wrap, aplica delta=-delta y luego invertDirection; después extrae pasos completos.
- La tarea de muestreo también llama ENC_Button_Task_10ms: no duplicar ese muestreo sin revisar el planificador.

## Trabajo en tándem

- app_stm32_ports.h conecta TIM4; app_tasks.c convierte pasos a eventos. Las acciones remotas llegan al router por otra entrada y no pasan por esta inversión física.

## Precauciones y límites

- GetVelocity/GetAcceleration devuelven uint16_t aunque el cálculo convierte pasos con signo; para dirección numérica usar getters Signed o StepDelta.
- GetScaledSteps devuelve al menos magnitud 1 incluso si lastStepDelta es cero; el consumidor debe aplicar su guarda de actividad.

## Hallazgos vinculados

- [AMDOC-F4-001: Escala cero aceptada por el encoder](../../../review/FINDINGS.md#amdoc-f4-001)
- [AMDOC-F4-002: El acumulador de pulsos reduce un delta de 32 bits a 8 bits](../../../review/FINDINGS.md#amdoc-f4-002)
- [AMDOC-F4-009: GetScaledSteps devuelve 1 con desplazamiento cero](../../../review/FINDINGS.md#amdoc-f4-009)
- [AMDOC-F4-010: sampleTenMs se almacena pero no gobierna el muestreo](../../../review/FINDINGS.md#amdoc-f4-010)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/encoder.h` | `encoder_types.h` | `51509c4ea3c16f78fa048152612da70995f486b9fa5b6a1a1063306815d28051` |
| `Core/Inc/encoder_types.h` | `am_port.h`, `stdbool.h` | `cc9d1b914e37d0c5c13a65aa73b0c985259ec57bcaf172cacc69724631ddd22e` |
| `Core/Src/encoder.c` | `encoder.h` | `a780282d4f007da6c707185062bcf904465ccf122edf8a69cc3ebdaf87abc26e` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `ENC_Byte_Flag_Struct_t`

`f4:Core/Inc/encoder_types.h:18`

| Atributo / firma | Función y precaución |
| --- | --- |
| `struct { estructura anidada } bitmap` | Dato de estado/configuración `bitmap`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } bitPair` | Dato de estado/configuración `bitPair`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } nibbles` | Dato de estado/configuración `nibbles`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t byte` | Dato de estado/configuración `byte`; declaración estructural, interpretación específica pendiente de ampliar. |

### `bitmap`

`f4:Core/Inc/encoder_types.h:19`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t bit0: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit1: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit2: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit3: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit4: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit5: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit6: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bit7: 1` | Dato de estado/configuración `1`; declaración estructural, interpretación específica pendiente de ampliar. |

### `bitPair`

`f4:Core/Inc/encoder_types.h:29`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t bitPairL: 2` | Dato de estado/configuración `2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bitPairCL: 2` | Dato de estado/configuración `2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bitPairCH: 2` | Dato de estado/configuración `2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bitPairH: 2` | Dato de estado/configuración `2`; declaración estructural, interpretación específica pendiente de ampliar. |

### `nibbles`

`f4:Core/Inc/encoder_types.h:35`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t bitL: 4` | Dato de estado/configuración `4`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t bitH: 4` | Dato de estado/configuración `4`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ENC_ButtonState_t`

`f4:Core/Inc/encoder_types.h:86`

| Atributo / firma | Función y precaución |
| --- | --- |
| `ENC_Byte_Flag_Struct_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint8_t counter` | Duración contada en invocaciones del muestreo; con cadencia 10 ms representa centésimas de segundo. |

### `ENC_Data`

`f4:Core/Inc/encoder_types.h:100`

| Atributo / firma | Función y precaución |
| --- | --- |
| `struct { estructura anidada } pair` | Dato de estado/configuración `pair`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t allData` | Dato de estado/configuración `allData`; declaración estructural, interpretación específica pendiente de ampliar. |

### `pair`

`f4:Core/Inc/encoder_types.h:101`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t vel` | Dato de estado/configuración `vel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t acc` | Dato de estado/configuración `acc`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ENC_Port`

`f4:Core/Inc/encoder_types.h:108`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `uint16_t (*read_count)(void *ctx)` | Lee el contador bruto de 16 bits del encoder. |
| `void (*reset_count)(void *ctx)` | Reinicia el contador físico del encoder desde su adaptador. |
| `void (*start)(void *ctx)` | Operación start de Encoder rotativo y pulsador integrado; semántica fina pendiente de ampliación. |
| `bool (*button_level)(void *ctx)` | Lee el pulsador integrado del encoder. |

### `ENC_Handle_t`

`f4:Core/Inc/encoder_types.h:117`

| Atributo / firma | Función y precaución |
| --- | --- |
| `ENC_Port port` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `ENC_ButtonState_t btnState` | Estado y duración del pulsador integrado; el consumidor transforma sus flags en eventos. |
| `uint8_t sampleTenMs` | Período declarado en unidades de 10 ms, almacenado en uint8_t; el planificador actual llama cada 10 ms sin consultar este campo. |
| `ENC_Direction_t dir` | Dirección lógica del último intervalo: NONE=0, CW=1, CCW=-1. |
| `ENC_Data data` | Campos históricos uint16_t de velocidad/aceleración; pueden representar conversiones de valores firmados. |
| `ENC_Data prevData` | Datos del intervalo anterior usados para calcular variación. |
| `ENC_Byte_Flag_Struct_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t prevCount` | Contador bruto anterior de 16 bits, referencia para corrección de wrap. |
| `int16_t prevStepDelta` | Pasos completos del intervalo previo para calcular aceleración. |
| `int16_t lastStepDelta` | Pasos completos firmados del intervalo más reciente. |
| `int16_t lastStepAcc` | Diferencia firmada entre pasos actuales y anteriores. |
| `uint8_t calibrateCountPerStep` | Pulsos raw por paso; cero llega a división/módulo por cero (AMDOC-F4-001). |
| `int8_t accumCount` | Resto/acumulación raw en int8_t; puede perder deltas grandes antes de extraer pasos (AMDOC-F4-002). |
| `bool invertDirection` | Preferencia de inversión aplicada después de la convención física base. |
| `bool allowEncoderInput` | Guarda que consulta la aplicación antes de entregar eventos de encoder a UI. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-enc_button_task_10ms"></a>`void ENC_Button_Task_10ms(ENC_Handle_t *h)` | Actualiza duración y flags del pulsador integrado. | `f4:Core/Inc/encoder.h:21`; editorial |
| <a id="fn-enc_getacceleration"></a>`uint16_t ENC_GetAcceleration(ENC_Handle_t *h)` | Devuelve el campo histórico de aceleración como uint16_t. | `f4:Core/Inc/encoder.h:42`; editorial |
| <a id="fn-enc_getdirection"></a>`ENC_Direction_t ENC_GetDirection(ENC_Handle_t *h)` | Devuelve NONE, CW o CCW según los últimos pasos. | `f4:Core/Inc/encoder.h:32`; editorial |
| <a id="fn-enc_getscaledsteps"></a>`int16_t ENC_GetScaledSteps(ENC_Handle_t *h)` | Selecciona multiplicador firmado 1, 2 o 4 según la magnitud del último desplazamiento. | `f4:Core/Inc/encoder.h:47`; editorial |
| <a id="fn-enc_getsignedacceleration"></a>`int16_t ENC_GetSignedAcceleration(ENC_Handle_t *h)` | Devuelve la variación firmada de pasos respecto del intervalo anterior. | `f4:Core/Inc/encoder.h:46`; editorial |
| <a id="fn-enc_getsignedvelocity"></a>`int16_t ENC_GetSignedVelocity(ENC_Handle_t *h)` | Devuelve lastStepDelta como velocidad firmada por intervalo. | `f4:Core/Inc/encoder.h:45`; editorial |
| <a id="fn-enc_getstepdelta"></a>`int16_t ENC_GetStepDelta(ENC_Handle_t *h)` | Devuelve los últimos pasos completos con signo. | `f4:Core/Inc/encoder.h:44`; editorial |
| <a id="fn-enc_getvelocity"></a>`uint16_t ENC_GetVelocity(ENC_Handle_t *h)` | Devuelve el campo histórico de velocidad como uint16_t. | `f4:Core/Inc/encoder.h:37`; editorial |
| <a id="fn-enc_init"></a>`void ENC_Init(ENC_Handle_t *h, const ENC_Port *port, uint32_t sampleTenMs, uint8_t calibrateCountPerStep)` | Copia el puerto e inicializa período, escala y acumuladores del encoder. | `f4:Core/Inc/encoder.h:7`; editorial |
| <a id="fn-enc_isdirectioninverted"></a>`bool ENC_IsDirectionInverted(const ENC_Handle_t *h)` | Consulta la preferencia de inversión aplicada a esta instancia. | `f4:Core/Inc/encoder.h:14`; editorial |
| <a id="fn-enc_setdirectioninverted"></a>`void ENC_SetDirectionInverted(ENC_Handle_t *h, bool inverted)` | Configura la inversión opcional posterior al sentido base físico. | `f4:Core/Inc/encoder.h:13`; editorial |
| <a id="fn-enc_start"></a>`void ENC_Start(ENC_Handle_t *h)` | Arranca el contador externo y reinicia referencias de muestreo. | `f4:Core/Inc/encoder.h:11`; editorial |
| <a id="fn-enc_task_n10ms"></a>`void ENC_Task_N10ms(ENC_Handle_t *h)` | Calcula pasos y derivadas tras corregir wrap y aplicar las dos convenciones de dirección. | `f4:Core/Inc/encoder.h:27`; editorial |
