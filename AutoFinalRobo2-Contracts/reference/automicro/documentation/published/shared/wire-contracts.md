# Datos y contratos F4 ↔ ESP ↔ Web

Fotografía por lectura de fuente: 2026-10-20. Este documento describe los caminos
inspeccionados; no certifica interoperabilidad en ejecución. El [catálogo](command-registry.md)
conserva los registros numéricos, límites y políticas por repositorio sin
fusionarlos en una compatibilidad supuesta.

## Productores y consumidores

| Tramo | Productor / consumidor | Representación | Correlación |
| --- | --- | --- | --- |
| Web → ESP | `EspClient.request` → dispatch del bridge | JSON WebSocket `/ws`, API 1 | requestId + command |
| ESP → F4 | broker/transaction manager → UNER_App | Frame UNER v2 por UART | ruta invertida + command / comando original de ACK |
| F4 → ESP → Web | handler/stream → decoder → cliente | Binario traducido a JSON | respuesta pendiente o evento de stream |
| Qt ↔ F4 | cliente de escritorio ↔ transporte USB | UNER v2 por USB CDC | Contrato numérico; internos Qt no reinspeccionados aquí |

El nodo `0x1` es F4, `0x2` es Qt, `0x3` corresponde al broker/identidad histórica
Web y `0x4` al remoto NRF. Sesiones Web usan nodos `0x5..0xC` asignados por ESP;
el rango reservado no implica ocho clientes simultáneos. El servidor actual
expresa un máximo de dos sesiones WebSocket. Broadcast es `0xF`. Transporte
UART1=0, NRF=1 y USB CDC=2; nodo y transporte son identificadores diferentes.

Evidencia: `f4:Core/Inc/uner_v2.h`, `esp:src/webserver_ws.cpp`,
`esp:src/ws_bridge/private_context.h`, `esp:src/ws_bridge/lifecycle.inc`.

## Sobre JSON de la API WebSocket

Ejemplo sintético de solicitud, sin credenciales:

```json
{"api":1,"type":"request","requestId":"doc-001","command":"stm.goto.start","args":{"xCm":25,"yCm":0}}
```

El cliente construye requestId y command no vacíos de hasta 64 caracteres.
`args` es un objeto. El tipo TypeScript `Record<string, unknown>` permite
representar valores, pero el validador C++ del comando impone tipos/campos.

ESP puede anunciar primero:

```json
{"api":1,"type":"event","event":"requestAccepted","data":{"requestId":"doc-001","command":"stm.goto.start","state":"queued","queueDepth":1}}
```

Eso confirma aceptación de cola, no autorización ni ejecución de la misión.
El final usa `type:"response"`, requestId, command, `ok:true` y `data`, o
`type:"error"` con code/message y los datos de correlación disponibles.
`EspClient.accept` entrega eventos a listeners; solo respuesta/error correlacionado
completa la Promise. Una respuesta con otro command genera `invalid_response`.
Al desconectar se cancelan pendientes. Timeout de cliente por defecto: 10.000 ms;
una operación puede solicitar otro. No copiar ese valor a todos los timeouts UART.

Evidencia: `web:src/protocol/wsApi.ts:buildWsRequest/parseWsEnvelope`,
`web:src/protocol/espClient.ts:request/accept`,
`esp:src/ws_bridge/requests.inc:emit_request_accepted/start_stm_request`,
`esp:src/ws_bridge/common.inc:send_response`.

## Frame UNER v2

| Offset | Longitud | Campo | Regla |
| --- | --- | --- | --- |
| 0 | 4 | Cabecera | ASCII `UNER` |
| 4 | 1 | len | Bytes de payload, 0..255 |
| 5 | 1 | token | ASCII `:` |
| 6 | 1 | versión | `0x02` |
| 7 | 1 | ruta | `(src << 4) \| (dst & 0x0F)` |
| 8 | 1 | command | ID numérico |
| 9 | len | payload | Formato particular del comando |
| 9+len | 1 | checksum | XOR de todos los bytes anteriores |

Tamaño total **10+len**. El frame no contiene requestId. El transaction manager
ESP conserva esa relación fuera del wire y mantiene una transacción UART en vuelo
con cola de solicitudes. Verifica ruta de respuesta y comando; los eventos MPU/IR
no completan la transacción pendiente.

Evidencia: `f4:Core/Inc/uner_v2.h`, `f4:Core/Src/uner_core.c:UNER_BuildFrame`,
`esp:src/uart_transaction_manager.c:utm_on_frame`.

<a id="completion"></a>
## ACK, respuesta y finalización

`0xE0` identifica ACK y `0xE1` NACK. Sus payloads identifican primero el comando
original y, cuando existe, status. Otra forma de finalización es un frame con el
mismo ID de comando y payload específico. En algunos flujos hay ACK intermedio
y después respuesta/evento final. El registro ESP distingue las políticas;
la tabla F4 usa flags que no equivalen exactamente a una política de cliente.

La respuesta efectiva de `0x74 SET_EXTENSION_PROFILE` es un byte de status por
el mismo ID: 0 guardado, 2 perfil inválido, 3 fallo de persistencia. La metadata
F4 dice ACK; ESP ya declara SameCommand. Es una contradicción conservada en
[AMDOC-F4-006](../../review/FINDINGS.md#amdoc-f4-006), no una razón para cambiar el
wire implementado. Cambiar perfil no es reiniciar; `0x76` es la operación de
reinicio en perfil y debe correlacionar su propio resultado.

Evidencia: `f4:Core/Src/uner_app.c:UNER_App_HandleSetExtensionProfile`,
`esp:src/f4_command_registry.c:kF4Commands`,
`esp:src/uart_transaction_manager.c:utm_on_frame`.

<a id="length-guards"></a>
## Longitudes y guardas que no deben suponerse

`UNER_Handle_IsLengthValid` permite len>=1 fuera del rango si el comando tiene
RESP. Por eso la metadata no asegura que una solicitud llegue con todos sus
argumentos. Se siguió el camino de GOTO/calibración que lee campos sin otra guarda:
[AMDOC-F4-003](../../review/FINDINGS.md#amdoc-f4-003). Una futura corrección debe
distinguir respuesta legítima, proxy y solicitud local, además de proteger cada
decoder. El parser de frames y la validación semántica son etapas distintas.

También debe rechazarse construir un frame con len>0 y payload nulo; hoy se
conservan bytes previos del buffer: [AMDOC-F4-004](../../review/FINDINGS.md#amdoc-f4-004).

## Representación entre C, C++ y TypeScript

| Concepto | F4 / ESP | Web | Precaución |
| --- | --- | --- | --- |
| uint8/16/32 | Ancho fijo en binario | number JSON | Revisar entero/rango antes de convertir |
| float wire | 4 bytes, representación usada por target, little endian | number JSON o DataView float32 LE | JSON no representa NaN/Infinity; rechazar no finitos |
| bool | Regla explícita 0/1 según mensaje | boolean | No copiar sizeof(bool) ni usar truthiness como validación |
| Texto | Bytes + longitud o terminador según campo | string | No asumir ASCII/UTF-8 ni terminador sin especificación |
| enum/flags | IDs/máscaras serializados expresamente | números y discriminantes | No transportar layout de bitfields C |
| puntero/handle | Memoria local del proceso/MCU | No tiene equivalente wire | Enviar datos, nunca direcciones |
| Tiempo | ms/us y contador modular uint32 | number | Distinguir adquisición, período y tiempo de envío |

Las implementaciones usan helpers LE o memcpy de cuatro bytes para float.
`APP_AutoMotionRequest` asume la representación nativa del F411 para decodificar;
un port a otro target debe revisar ese supuesto. Una unidad en el nombre no
asegura precisión: `now_us` actual proviene de tick ms × 1000
([AMDOC-F4-008](../../review/FINDINGS.md#amdoc-f4-008)).

<a id="auto-motion"></a>
## GOTO y calibración: solicitudes exactas

| JSON | ID | Payload request | Respuesta |
| --- | --- | --- | --- |
| `stm.goto.get` | 0x23 | vacío | estado de 128 bytes |
| `stm.goto.start` | 0x24 | float32 xCm en 0, float32 yCm en 4 | estado de 128 bytes |
| `stm.goto.cancel` | 0x25 | vacío | estado de 128 bytes |
| `stm.goto.irCalibration.set` | 0x26 | 186 bytes descritos abajo | estado de 128 bytes |
| `stm.motorCalibration.get` | 0x27 | vacío | estado de 128 bytes |
| `stm.motorCalibration.control` | 0x28 | operation:u8 en 0; percent:f32 en 1; rampMs:u32 en 5; timeoutMs:u32 en 9 | estado de 128 bytes |
| `stm.motorCalibration.profile.get` | 0x29 | vacío | perfil de 88 bytes |
| `stm.motorCalibration.profile.set` | 0x2A | perfil de 88 bytes | perfil de 88 bytes |
| `stm.mpu.calibration.request` | 0x2B | vacío | estado de 128 bytes |

Cada float/entero multibyte es LE. El codificador ESP usa campos JSON específicos
por comando. Su validador común admite formas que pertenecen a otras operaciones;
esa inconsistencia se registra en [AMDOC-ESP-001](../../review/FINDINGS.md#amdoc-esp-001).
Una documentación de forma correcta no afirma que el servidor ya rechace todas
las incorrectas.

Evidencia: `esp:src/ws_bridge/dispatch.inc:cmd_stm_auto/validate_auto_params`,
`f4:Core/Src/app_tasks.c:APP_AutoMotionRequest`,
`f4:Core/Src/auto_motion.c:AutoMotion_*`,
`web:src/features/autoMotion/GotoPanel.tsx`.

### Estado AutoMotion — 128 bytes

| Offset | Tipo | Significado |
| --- | --- | --- |
| 0 | u8 | Error de la solicitud que construye respuesta: reason recibido distinto de 0 |
| 1 | u8 | Esquema=1 |
| 2 / 3 | u8 / u8 | kind / state de la operación |
| 4 / 5 | u8 / u8 | reason de solicitud o estado / modo del robot |
| 6 | u8 bits | ready MPU, calibrada, perfil motor, perfil IR, balance, estable, seguridad, línea lista en bits 0..7 |
| 7 | u8 | obstacle_mask |
| 8 / 12 | u32 / u32 | ID de operación / tiempo transcurrido ms |
| 16 / 20 | f32 / f32 | posición estimada x/y, cm |
| 24 / 28 | f32 / f32 | objetivo x/y, cm |
| 32 | f32 | heading según convención de misión, grados |
| 36 | f32 | velocidad estimada o medida por calibración temporal, cm/s |
| 40 | f32 | consigna/porcentaje de la operación; interpretación según kind |
| 44 / 48 | u32 / u32 | rampa ms / tiempo medido ms |
| 52 | f32 | frecuencia PWM, Hz |
| 56 / 60 | f32 / f32 | duty medio izquierdo/derecho según cálculo temporal |
| 64 / 68 | f32 / f32 | duty máximo izquierdo/derecho |
| 72 / 76 | f32 / f32 | integral de duty × segundos por motor |
| 80 / 84 | f32 / f32 | ciclos estimados por motor |
| 88 / 92 / 96 / 100 | u32 × 4 | tiempos avance/retroceso izquierdo y derecho, ms |
| 104..107 | u8 × 4 | cantidad de puntos, fase, persistido, resultado válido |
| 108 | f32 | distancia de calibración serializada=100 cm |
| 112 | f32 | máximo error yaw |
| 116 | u32 | tiempo previo al inicio de medición, ms |
| 120..123 | u8 × 4 | objetos habilitados, luces, frescura objetos, frescura línea |
| 124 | u32 | revisión del perfil |

**status de solicitud no es éxito de misión**: byte 0 se construye desde el
argumento reason de la respuesta, mientras byte 4 puede exponer un motivo previo
de la operación. Leer state, reason y result_valid conjuntamente.

### Perfil de motores — 88 bytes

Cabecera: status:u8 en 0, esquema:u8=1 en 1, count:u8 en 2, ir_valid:u8 en 3,
revision:u32 LE en 4. Cinco registros de 16 bytes desde 8: percent:f32,
speed_cm_s:f32, ramp_ms:u32 y runs:u32. En SetProfile se leen count y los puntos;
no se debe asumir que cada campo de una respuesta es editable: el firmware
incrementa su propia revisión y conserva otros datos del perfil.

Puntos aceptados: porcentaje 1..20, velocidad mayor que 0 y hasta 300 cm/s,
rampa hasta 5000 ms, runs 1..1000, porcentajes estrictamente crecientes.

### Calibración IR de objetos — 186 bytes

Byte 0: esquema=1. Cinco canales de 37 bytes desde 1. Cada canal contiene
baseline:u16, noise:u16, count:u8 y ocho pares distance_mm:u16 + adc:u16.
Count debe estar entre 2 y 8; ADC/baseline/noise hasta 4095, distancias útiles
positivas y crecientes. No transportar structs `AutoMotion_IrChannel` por su
tamaño en memoria: el wire tiene offsets explícitos.

## Movimiento: solicitudes y respuesta de ocho bytes

`stm.motion.set` / 0x7D recibe forwardPercent:f32 en 0, turnPercent:f32 en 4 y
durationMs:u16 en 8. `stm.motor.test` / 0x7F usa el mismo layout, pero los floats
son leftPercent y rightPercent. Porcentajes -100..100 y duración 50..2000 ms
según el handler F4. Son contratos distintos aunque tengan la misma longitud.

`stm.balance.setEnabled` / 0x7C recibe enabled:u8; `stm.motion.stop` / 0x7E recibe
disableBalance:u8. Ambos admiten solo 0/1. Stop cancela productores de movimiento;
disableBalance también desarma. Modo y guardas siguen aplicándose en F4.

Respuesta común: status, armed, balance_active, command_active, safety_stop,
reject_reason, carMode y motorTestActive, un byte cada uno. Status 0 no convierte
las salidas solicitadas en una medida de desplazamiento.

Evidencia: `esp:src/ws_bridge/dispatch.inc:cmd_stm_motion`,
`f4:Core/Src/uner_app.c:UNER_App_HandleMotion`.

## Snapshots y sus unidades

### MPU — 42 bytes

| Offset | Tipo | Campo / unidad |
| --- | --- | --- |
| 0 / 1 | u8 / u8 | status / flags wire |
| 2 / 4 | u16 / u16 | sample_seq / sample_dt_us |
| 6 / 10 / 14 | f32 × 3 | roll/pitch/yaw, grados |
| 18 / 22 / 26 | f32 × 3 | aceleración X/Y/Z en g: snapshot mg × 0,001 |
| 30 / 34 / 38 | f32 × 3 | giro X/Y/Z en grados/s: snapshot mdps × 0,001 |

No enviar los mg internos como si ya fueran g. El montaje de balance utiliza
pitch/Y como frontal y roll/X como lateral. Flags de muestra, calibración,
magnetómetro y error se construyen por separado en el serializador.

### IR — 56 bytes

| Offset | Tipo | Campo / unidad |
| --- | --- | --- |
| 0 / 1 | u8 / u8 | status / flags de emisores, luces, modo y ruta válida |
| 2 / 4 / 6 | u16 / u16 / u32 | secuencia de snapshot, período ms, tick ms |
| 10 | u16 × 8 | ADC raw en el orden de canales del contrato |
| 26 | u16 × 8 | valores normalizados para presentación |
| 42..45 | u8 × 4 | patrón, alineación, confianza, ambigüedad |
| 46 | u16 | ancho de línea, mm |
| 48 / 52 | f32 / f32 | error lateral mm / error normalizado |

Raw y ruta se obtienen del mismo scan completo por `APP_ReadIrRouteSnapshot`.
La secuencia del snapshot puntual se incrementa al atender la solicitud; no
confundirla automáticamente con la secuencia física ADC usada por el seguidor.

### Control — 64 bytes

Offsets: 0 status; 1 modo; 2 FSM línea; 3 patrón; 4 confianza; 5 ambigüedad;
6 rechazo línea; 7 ancho:u16 mm; 9 error:f32 mm; 13 error normalizado:f32;
17 aceleración:f32 m/s²; 21 velocidad:f32 m/s; 25 yaw_rate:f32 grados/s;
29 confianza estimador; 30 flags estimador; 31 u_common:f32 %;
35 u_diff:f32 %; 39 límite giro:f32 %; 43 saturación; 44 USB activo;
45 período alive:u16 ms; 47 alive_count:u32; 51 rx_overflow:u32;
55 tx_queue_full:u32; 59 tx_queued:u8; 60 tx_queue_drop:u32.
Los campos sin tipo indicado en esta enumeración ocupan un byte.

Evidencia de snapshots: `f4:Core/Src/uner_app.c:UNER_App_BuildMpuSnapshotPayload`,
`UNER_App_BuildIrSnapshotPayload`, `UNER_App_HandleGetControlSnapshot`;
consumo ESP en `src/ws_bridge/reports.inc` y cliente UNER de ESP.

## UI, permisos y streams

Los comandos de UI transportan screenCode:u32 LE para contrastar el contexto
actual. Click visible y press añaden un byte con fila o tipo de pulsación según
la operación. El reporte `0x95` identifica pantalla/origen y `0x96` añade
selección/cantidad. No contienen un framebuffer ni estado completo de OSK.
Las reglas y diferencias remotas siguen en el documento central
[`SCREEN_BEHAVIOR.md`](../../../docs/SCREEN_BEHAVIOR.md).

Los offsets, variantes y controles de pantalla se detallan en
[SCREEN_SYSTEM](SCREEN_SYSTEM.md#reports). El formato exacto del PIN JSON frente
a dígitos wire, ámbitos y respuesta está en
[auth-pin-v1](contracts/auth-pin-v1.md). La transferencia de framebuffer y sus
dos tamaños de chunk están en [oled-canvas-v1](contracts/oled-canvas-v1.md).

La autorización se aplica en ESP y vuelve a comprobarse en F4 por nodo/transporte.
PIN local en curso, bloqueo por intentos, TTL remoto y permisos de acción son
condiciones distintas. Streams se identifican por transporte+nodo; detener una
suscripción no implica detener todas las interfaces. Recuperar conexión no debe
inventar una sesión autenticada ni una suscripción vigente.

## Cobertura que todavía falta ampliar

El catálogo extrae todas las entradas encontradas, pero no reemplaza un esquema
por campo de cada comando histórico. Quedan para fichas de contrato específicas
los campos completos de preferencias/AP, assets, boot, telemetría extendida,
estado de calibración de línea y la revisión interna Qt. OLED Canvas tiene
frontera, tamaños y ciclo en su [ficha](../f4/libraries/oled-canvas.md) y
[esquema por operación](contracts/oled-canvas-v1.md); mantener productor/decoder
juntos al ampliarlo. La nueva ficha PIN documenta los mensajes, sin agotar la
máquina completa de permisos ni publicar datos de sesión.
