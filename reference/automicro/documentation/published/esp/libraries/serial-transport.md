# Transporte serial Arduino

ID: `esp:module/src/uner_transport_serial.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Adapta UART/Serial del ESP al contrato de transporte UNER.

Disponibilidad de bytes, recepción y aceptación TX, sin interpretar JSON ni permisos.

<a id="modularity"></a>
## Modularidad observada

Ligado a Arduino/ESP por diseño; mantiene el protocolo separado del acceso físico.

<a id="state"></a>
## Handle, estado y atributos externos

Buffer y recurso serial pertenecen al adaptador; las vistas no deben sobrevivir a su reutilización.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Serial se inicia en setup antes de UNER.
- El loop atiende bytes y envío; flush/cooperate no equivalen a respuesta F4.

<a id="interactions"></a>
## Interacciones

- UNER_Handle utiliza el transporte; TransactionManager serializa solicitudes Web sobre ese canal.

<a id="limitations"></a>
## Precauciones y límites

- No emitir logs arbitrarios sobre la UART que comparte frames del robot.
- Baudrate configurado y entrega de bytes no prueban conectividad física.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/uner_transport_serial.cpp` | c-cpp-structural | `708a04bc2e92eea7890a6fdfdab2dec66e7f47871d2cf8315384033043b6f47a` |
| `src/uner_transport_serial.h` | c-cpp-structural | `b1cc33e2d146e91facdfaadc206fda9c55927ee57142001ac3cc8ba85e3641ee` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `UNER_SerialTransportCtx`

`esp:src/uner_transport_serial.h:5`

| Atributo / firma | Propósito |
| --- | --- |
| `HardwareSerial *serial` | Dato de estado/configuración `serial`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-uner_serialpollrx"></a>`uint16_t UNER_SerialPollRx(void *ctx, uint8_t *out, uint16_t max_len)` | Coordina `available`, `read`, `yield` para Transporte serial Arduino. | `esp:src/uner_transport_serial.cpp:3`; structural |
| <a id="fn-uner_serialtransport_init"></a>`void UNER_SerialTransport_Init(UNER_Transport *transport, uint8_t id, HardwareSerial &serial, uint8_t max_payload)` | Adapta la llamada a `UNER_SerialTransport_InitWithContext` con el contexto de esta integración. | `esp:src/uner_transport_serial.cpp:32`; structural |
| <a id="fn-uner_serialtransport_initwithcontext"></a>`void UNER_SerialTransport_InitWithContext(UNER_Transport *transport, UNER_SerialTransportCtx *ctx, uint8_t id, HardwareSerial &serial, uint8_t max_payload)` | Operación UNER Serial Transport Init With Context de Transporte serial Arduino; semántica fina pendiente de ampliación. | `esp:src/uner_transport_serial.cpp:22`; inferred-name |
| <a id="fn-uner_serialtrysend"></a>`int UNER_SerialTrySend(void *ctx, const uint8_t *data, uint16_t len)` | Coordina `availableForWrite`, `write` para Transporte serial Arduino. | `esp:src/uner_transport_serial.cpp:14`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
