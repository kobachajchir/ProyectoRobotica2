# Broker de transacciones UART

ID: `esp:module/src/uart_transaction_manager.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Relaciona requests Web con solicitudes UART acotadas y una transacción en vuelo.

Cola, cuota por cliente, generación, deadline, reintento idempotente y finalización por ACK/mismo comando.

<a id="modularity"></a>
## Modularidad observada

C sin WebSocket concreto; las identidades de cliente y políticas son parte del contrato del broker.

<a id="state"></a>
## Handle, estado y atributos externos

UartTransactionManager posee queue[8], inflight y métricas; UtmRequest copia payload, request_id y command_name. send_fn/send_ctx y completion_fn/completion_ctx son prestados y deben permanecer válidos.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- utm_enqueue copia y valida admisión; no significa ejecución F4.
- utm_service inicia/reintenta/vence según deadlines y disponibilidad.
- utm_on_frame exige ruta inversa y comando correlacionado; eventos MPU/IR no completan la solicitud.
- Disconnect invalida generación o deja la transacción huérfana sin completarla sobre otro socket.

<a id="interactions"></a>
## Interacciones

- requests.inc traduce política del registro y emite requestAccepted.
- completion entrega resultado al bridge que construye response/error JSON.

<a id="limitations"></a>
## Precauciones y límites

- UTM_MAX_QUEUED_PER_CLIENT=2; cola total 8.
- Reintentar una mutación no idempotente puede duplicar efectos; respetar retry_policy.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/uart_transaction_manager.c` | c-cpp-structural | `8d944ca3a4cdda6adebdb8e59fa243a50f097bfc7cdd356bf4d779b9ce52e445` |
| `src/uart_transaction_manager.h` | c-cpp-structural | `a6044e17ecd6041a082ea00bbb90a866fbd8ed13b32e553f124bbac12b6d3561` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `UtmFrame`

`esp:src/uart_transaction_manager.h:76`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t src` | Dato de estado/configuración `src`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t dst` | Dato de estado/configuración `dst`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t command_id` | Dato de estado/configuración `command_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const uint8_t *payload` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `uint16_t payload_len` | Dato de estado/configuración `payload_len`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UtmRequest`

`esp:src/uart_transaction_manager.h:84`

| Atributo / firma | Propósito |
| --- | --- |
| `uint32_t client_id` | Dato de estado/configuración `client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t generation` | Generación que impide completar sobre una conexión reemplazada. |
| `uint8_t node` | Dato de estado/configuración `node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t dst_node` | Dato de estado/configuración `dst_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t command_id` | Dato de estado/configuración `command_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UtmCommandOwner owner` | Dato de estado/configuración `owner`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UtmResponsePolicy response_policy` | Dato de estado/configuración `response_policy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UtmRetryPolicy retry_policy` | Dato de estado/configuración `retry_policy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t idempotent` | Dato de estado/configuración `idempotent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t max_retries` | Dato de estado/configuración `max_retries`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t retries_done` | Dato de estado/configuración `retries_done`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t payload_len` | Dato de estado/configuración `payload_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t payload[UTM_MAX_PAYLOAD]` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `char request_id[UTM_REQUEST_ID_CAPACITY]` | Dato de estado/configuración `request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char command_name[UTM_REQUEST_ID_CAPACITY]` | Dato de estado/configuración `command_name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t api_v1` | Dato de estado/configuración `api_v1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t internal_kind` | Dato de estado/configuración `internal_kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t timeout_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t deadline_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `UtmMetrics`

`esp:src/uart_transaction_manager.h:106`

| Atributo / firma | Propósito |
| --- | --- |
| `uint32_t enqueued` | Dato de estado/configuración `enqueued`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t rejected_invalid` | Dato de estado/configuración `rejected_invalid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t rejected_full` | Dato de estado/configuración `rejected_full`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t rejected_client_limit` | Dato de estado/configuración `rejected_client_limit`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t rejected_owner` | Dato de estado/configuración `rejected_owner`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t started` | Dato de estado/configuración `started`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t completed` | Dato de estado/configuración `completed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t f4_nacks` | Dato de estado/configuración `f4_nacks`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t timed_out` | Dato de estado/configuración `timed_out`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t uart_errors` | Dato de estado/configuración `uart_errors`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t retries` | Dato de estado/configuración `retries`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t send_busy` | Dato de estado/configuración `send_busy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t canceled_queued` | Dato de estado/configuración `canceled_queued`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t orphaned_inflight` | Dato de estado/configuración `orphaned_inflight`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ignored_stream_events` | Dato de estado/configuración `ignored_stream_events`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t unmatched_frames` | Dato de estado/configuración `unmatched_frames`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t max_queue_depth` | Dato de estado/configuración `max_queue_depth`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UartTransactionManager`

`esp:src/uart_transaction_manager.h:132`

| Atributo / firma | Propósito |
| --- | --- |
| `UtmRequest queue[UTM_QUEUE_CAPACITY]` | Dato de estado/configuración `queue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t queue_count` | Dato de estado/configuración `queue_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UtmRequest inflight` | Dato de estado/configuración `inflight`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t inflight_active` | Dato de estado/configuración `inflight_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t inflight_orphaned` | Dato de estado/configuración `inflight_orphaned`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ack_seen` | Dato de estado/configuración `ack_seen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t response_seen` | Dato de estado/configuración `response_seen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t attempt_deadline_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `UtmSendFn send_fn` | Intenta enviar la solicitud; BUSY mantiene pendiente, OK no es respuesta final. |
| `void *send_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UtmCompletionFn completion_fn` | Notifica resultado final con request y frame prestados durante el callback. |
| `void *completion_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UtmMetrics metrics` | Dato de estado/configuración `metrics`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UtmSendFn`

`esp:src/uart_transaction_manager.h:125`

Callback: `UtmSendResult (*UtmSendFn)(void *ctx, const UtmRequest *request)`. utm_service lo invoca con request prestado y send_ctx: devuelve UTM_SEND_OK/BUSY/ERROR. OK confirma admisión del envío, no respuesta del F4; contexto vive mientras el manager lo use.

### `UtmCompletionFn`

`esp:src/uart_transaction_manager.h:127`

Callback: `void (*UtmCompletionFn)(void *ctx, const UtmRequest *request, UtmCompletionStatus status, const UtmFrame *frame)`. El broker lo invoca al finalizar con request/status y frame cuando existe. Bridge debe correlacionar cliente/generación y copiar lo que retenga; no completa otro socket reutilizado.


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-utm_cancel_all"></a>`void utm_cancel_all(UartTransactionManager *manager)` | Coordina `memset` para Broker de transacciones UART. | `esp:src/uart_transaction_manager.c:383`; structural |
| <a id="fn-utm_client_queued_count"></a>`uint8_t utm_client_queued_count(const UartTransactionManager *manager, uint32_t client_id, uint32_t generation)` | Operación client queued count de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:184`; inferred-name |
| <a id="fn-utm_disconnect_client"></a>`void utm_disconnect_client(UartTransactionManager *manager, uint32_t client_id, uint32_t generation, uint32_t now_ms)` | Cancela pendientes de cliente/generación sin asignarlos a otro socket. | `esp:src/uart_transaction_manager.c:351`; editorial |
| <a id="fn-utm_enqueue"></a>`UtmEnqueueResult utm_enqueue(UartTransactionManager *manager, const UtmRequest *request, uint32_t now_ms)` | Admite y copia una solicitud dentro de límites globales y por cliente. | `esp:src/uart_transaction_manager.c:200`; editorial |
| <a id="fn-utm_inflight"></a>`const UtmRequest *utm_inflight(const UartTransactionManager *manager)` | Operación inflight de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:410`; inferred-name |
| <a id="fn-utm_init"></a>`void utm_init(UartTransactionManager *manager, UtmSendFn send_fn, void *send_ctx, UtmCompletionFn completion_fn, void *completion_ctx)` | Conecta envío/finalización y reinicia cola y métricas. | `esp:src/uart_transaction_manager.c:160`; editorial |
| <a id="fn-utm_metrics"></a>`const UtmMetrics *utm_metrics(const UartTransactionManager *manager)` | Operación metrics de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:415`; inferred-name |
| <a id="fn-utm_on_frame"></a>`UtmFrameResult utm_on_frame(UartTransactionManager *manager, const UtmFrame *frame, uint32_t now_ms)` | Correlaciona ruta/ID/ACK y completa o mantiene pendiente la solicitud. | `esp:src/uart_transaction_manager.c:289`; editorial |
| <a id="fn-utm_queued_count"></a>`uint8_t utm_queued_count(const UartTransactionManager *manager)` | Operación queued count de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:405`; inferred-name |
| <a id="fn-utm_request_init"></a>`void utm_request_init(UtmRequest *request)` | Coordina `memset` para Broker de transacciones UART. | `esp:src/uart_transaction_manager.c:174`; structural |
| <a id="fn-utm_service"></a>`void utm_service(UartTransactionManager *manager, uint32_t now_ms)` | Avanza envío, reintentos y expiración de la transacción activa. | `esp:src/uart_transaction_manager.c:239`; editorial |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-add_ms_saturated"></a>`static uint32_t add_ms_saturated(uint32_t now_ms, uint32_t delta_ms)` | Operación add ms saturated de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:12`; inferred-name |
| <a id="fn-complete_inflight"></a>`static void complete_inflight(UartTransactionManager *manager, UtmCompletionStatus status, const UtmFrame *frame, uint32_t now_ms)` | Coordina `memset`, `notify_completion`, `try_start_next` para Broker de transacciones UART. | `esp:src/uart_transaction_manager.c:133`; structural |
| <a id="fn-default_deadline_span"></a>`static uint32_t default_deadline_span(const UtmRequest *request)` | Operación default deadline span de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:43`; inferred-name |
| <a id="fn-earlier_deadline"></a>`static uint32_t earlier_deadline(uint32_t first, uint32_t second)` | Operación earlier deadline de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:17`; inferred-name |
| <a id="fn-expire_queued"></a>`static void expire_queued(UartTransactionManager *manager, uint32_t now_ms)` | Coordina `notify_completion`, `remove_queue_at`, `time_reached` para Broker de transacciones UART. | `esp:src/uart_transaction_manager.c:70`; structural |
| <a id="fn-notify_completion"></a>`static void notify_completion(UartTransactionManager *manager, const UtmRequest *request, UtmCompletionStatus status, const UtmFrame *frame)` | Adapta la llamada a `completion_fn` con el contexto de esta integración. | `esp:src/uart_transaction_manager.c:22`; structural |
| <a id="fn-remove_queue_at"></a>`static void remove_queue_at(UartTransactionManager *manager, uint8_t index)` | Coordina `memset` para Broker de transacciones UART. | `esp:src/uart_transaction_manager.c:32`; structural |
| <a id="fn-request_is_valid"></a>`static uint8_t request_is_valid(const UtmRequest *request)` | Operación request is valid de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:57`; inferred-name |
| <a id="fn-time_reached"></a>`static uint8_t time_reached(uint32_t now_ms, uint32_t deadline_ms)` | Operación time reached de Broker de transacciones UART; semántica fina pendiente de ampliación. | `esp:src/uart_transaction_manager.c:7`; inferred-name |
| <a id="fn-try_start_next"></a>`static void try_start_next(UartTransactionManager *manager, uint32_t now_ms)` | Coordina `add_ms_saturated`, `earlier_deadline`, `expire_queued` para Broker de transacciones UART. | `esp:src/uart_transaction_manager.c:91`; structural |
