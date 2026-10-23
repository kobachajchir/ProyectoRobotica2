# Streams MPU/IR y solicitudes en espera

ID: `esp:module/src/stream_broker.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Reconcilia suscripciones Web con estado de streams aplicado en F4 y distribuye muestras a destinatarios autorizados.

Desired/applied, dueño del stream, acciones SET/STOP, resultados obsoletos y waiters; no adquiere sensores.

<a id="modularity"></a>
## Modularidad observada

C con datos explícitos y sin renderer; coordinación de protocolo y sesiones del robot.

<a id="state"></a>
## Handle, estado y atributos externos

StreamBroker posee suscriptores por client/generation/node y estado por MPU/IR. SbAction identifica la acción concreta pendiente; desired no equivale a applied.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Subscribe actualiza intención y período entre 20 y 1000 ms.
- TakeAction produce trabajo que el broker UART ejecuta; CompleteAction acepta solo el resultado vigente.
- Al perder backend se reconcilia; retirar un cliente no debe detener el stream requerido por otro.

<a id="interactions"></a>
## Interacciones

- SessionManager autoriza; UTM manda control F4; bridge publica solo a suscriptores correspondientes.

<a id="limitations"></a>
## Precauciones y límites

- Acción bloqueada tras fallo no debe convertirse en bucle ilimitado de reintentos.
- Datos de sensor son muestras, no la respuesta de una mutación pendiente.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/stream_broker.c` | c-cpp-structural | `f2c52f2b9daf98064dafd30b8ed473fbdd3e16516d4761c12fdabd690ee63fcc` |
| `src/stream_broker.h` | c-cpp-structural | `598448a40371530ee3e64adc275a35d80d45f46e22ced1cb601098cb3e88b0a1` |
| `src/stream_request_waiters.c` | c-cpp-structural | `8fe87c147a8bcc01ae32b5f5e378bb583f86fa7711b51a7e3103d8956c26d34d` |
| `src/stream_request_waiters.h` | c-cpp-structural | `e11308125c415253bffccf05c83afef30d1bdfb33464b877c022662b0fbcd8c1` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `SbEndpoint`

`esp:src/stream_broker.h:36`

| Atributo / firma | Propósito |
| --- | --- |
| `uint32_t client_id` | Dato de estado/configuración `client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t generation` | Dato de estado/configuración `generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t node` | Dato de estado/configuración `node`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SbSubscriber`

`esp:src/stream_broker.h:42`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t used` | Dato de estado/configuración `used`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbEndpoint endpoint` | Dato de estado/configuración `endpoint`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t subscribed[SB_STREAM_COUNT]` | Dato de estado/configuración `subscribed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t period_ms[SB_STREAM_COUNT]` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `SbAction`

`esp:src/stream_broker.h:49`

| Atributo / firma | Propósito |
| --- | --- |
| `SbActionType type` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbStream stream` | Dato de estado/configuración `stream`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbEndpoint endpoint` | Dato de estado/configuración `endpoint`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t reconcile` | Dato de estado/configuración `reconcile`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SbStreamState`

`esp:src/stream_broker.h:57`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t desired_active` | Dato de estado/configuración `desired_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t desired_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t desired_owner_valid` | Dato de estado/configuración `desired_owner_valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbEndpoint desired_owner` | Dato de estado/configuración `desired_owner`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t applied_active` | Dato de estado/configuración `applied_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t applied_period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t applied_owner_valid` | Dato de estado/configuración `applied_owner_valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbEndpoint applied_owner` | Dato de estado/configuración `applied_owner`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t needs_known_stop` | Dato de estado/configuración `needs_known_stop`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t pending` | Dato de estado/configuración `pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t action_blocked` | Dato de estado/configuración `action_blocked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbAction pending_action` | Dato de estado/configuración `pending_action`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SbMetrics`

`esp:src/stream_broker.h:72`

| Atributo / firma | Propósito |
| --- | --- |
| `uint32_t subscriptions` | Dato de estado/configuración `subscriptions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t unsubscriptions` | Dato de estado/configuración `unsubscriptions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t client_removals` | Dato de estado/configuración `client_removals`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t desired_period_changes` | Dato de estado/configuración `desired_period_changes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t set_actions` | Dato de estado/configuración `set_actions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stop_actions` | Dato de estado/configuración `stop_actions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t owner_transfers` | Dato de estado/configuración `owner_transfers`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t reconcile_cycles` | Dato de estado/configuración `reconcile_cycles`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t action_failures` | Dato de estado/configuración `action_failures`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stale_action_results` | Dato de estado/configuración `stale_action_results`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t pending_actions_aborted` | Dato de estado/configuración `pending_actions_aborted`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t action_failures_blocked` | Dato de estado/configuración `action_failures_blocked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t explicit_retries` | Dato de estado/configuración `explicit_retries`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t fanout_allowed` | Dato de estado/configuración `fanout_allowed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t fanout_denied` | Dato de estado/configuración `fanout_denied`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StreamBroker`

`esp:src/stream_broker.h:90`

| Atributo / firma | Propósito |
| --- | --- |
| `SbSubscriber subscribers[SB_MAX_CLIENTS]` | Dato de estado/configuración `subscribers`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbStreamState streams[SB_STREAM_COUNT]` | Dato de estado/configuración `streams`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbMetrics metrics` | Dato de estado/configuración `metrics`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SrwWaiter`

`esp:src/stream_request_waiters.h:31`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t used` | Dato de estado/configuración `used`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t client_id` | Dato de estado/configuración `client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t generation` | Dato de estado/configuración `generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t api_v1` | Dato de estado/configuración `api_v1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SbStream stream` | Dato de estado/configuración `stream`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t subscribed` | Dato de estado/configuración `subscribed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t period_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t deadline_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `char request_id[SRW_REQUEST_ID_CAPACITY]` | Dato de estado/configuración `request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char command[SRW_COMMAND_CAPACITY]` | Dato de estado/configuración `command`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SrwCompletion`

`esp:src/stream_request_waiters.h:44`

| Atributo / firma | Propósito |
| --- | --- |
| `SrwWaiter waiter` | Dato de estado/configuración `waiter`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SrwCompletionStatus status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |

### `SrwMetrics`

`esp:src/stream_request_waiters.h:49`

| Atributo / firma | Propósito |
| --- | --- |
| `uint32_t added` | Dato de estado/configuración `added`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t rejected_full` | Dato de estado/configuración `rejected_full`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t rejected_duplicate` | Dato de estado/configuración `rejected_duplicate`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t converged` | Dato de estado/configuración `converged`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t timed_out` | Dato de estado/configuración `timed_out`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t failed` | Dato de estado/configuración `failed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t canceled` | Dato de estado/configuración `canceled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t high_watermark` | Dato de estado/configuración `high_watermark`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StreamRequestWaiters`

`esp:src/stream_request_waiters.h:60`

| Atributo / firma | Propósito |
| --- | --- |
| `SrwWaiter waiters[SRW_CAPACITY]` | Dato de estado/configuración `waiters`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SrwMetrics metrics` | Dato de estado/configuración `metrics`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-sb_backend_reconnected"></a>`void sb_backend_reconnected(StreamBroker *broker)` | Coordina `memset` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_broker.c:346`; structural |
| <a id="fn-sb_clamp_period"></a>`uint16_t sb_clamp_period(uint16_t period_ms)` | Operación clamp period de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_broker.c:163`; inferred-name |
| <a id="fn-sb_complete_action"></a>`SbResult sb_complete_action(StreamBroker *broker, const SbAction *action, uint8_t success)` | Aplica resultado solo si corresponde a la acción todavía vigente. | `esp:src/stream_broker.c:297`; editorial |
| <a id="fn-sb_init"></a>`void sb_init(StreamBroker *broker)` | Coordina `memset` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_broker.c:157`; structural |
| <a id="fn-sb_is_converged"></a>`uint8_t sb_is_converged(const StreamBroker *broker, SbStream stream)` | Coordina `endpoint_equal`, `stream_valid` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_broker.c:333`; structural |
| <a id="fn-sb_metrics"></a>`const SbMetrics *sb_metrics(const StreamBroker *broker)` | Operación metrics de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_broker.c:380`; inferred-name |
| <a id="fn-sb_remove_client"></a>`uint8_t sb_remove_client(StreamBroker *broker, uint32_t client_id, uint32_t generation)` | Coordina `abort_pending_for_endpoint`, `allow_explicit_retry`, `find_subscriber` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_broker.c:220`; structural |
| <a id="fn-sb_should_fanout"></a>`uint8_t sb_should_fanout(StreamBroker *broker, uint32_t client_id, uint32_t generation, SbStream stream, uint8_t authenticated)` | Decide entrega de muestra según suscripción y autorización. | `esp:src/stream_broker.c:359`; editorial |
| <a id="fn-sb_stream_state"></a>`const SbStreamState *sb_stream_state(const StreamBroker *broker, SbStream stream)` | Adapta la llamada a `stream_valid` con el contexto de esta integración. | `esp:src/stream_broker.c:375`; structural |
| <a id="fn-sb_subscribe"></a>`SbResult sb_subscribe(StreamBroker *broker, uint32_t client_id, uint32_t generation, uint8_t node, SbStream stream, uint16_t period_ms)` | Actualiza deseo de suscripción para cliente/generación/nodo. | `esp:src/stream_broker.c:170`; editorial |
| <a id="fn-sb_take_action"></a>`SbAction sb_take_action(StreamBroker *broker, SbStream stream)` | Extrae acción pendiente SET/STOP para reconciliar el stream. | `esp:src/stream_broker.c:248`; editorial |
| <a id="fn-sb_unsubscribe"></a>`SbResult sb_unsubscribe(StreamBroker *broker, uint32_t client_id, uint32_t generation, SbStream stream)` | Coordina `abort_pending_for_stream`, `allow_explicit_retry`, `find_subscriber` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_broker.c:194`; structural |
| <a id="fn-srw_add"></a>`SrwAddResult srw_add(StreamRequestWaiters *manager, uint32_t client_id, uint32_t generation, uint8_t api_v1, SbStream stream, uint8_t subscribed, uint16_t period_ms, const char *request_id, const char *command, uint32_t deadline_ms)` | Coordina `copy_text`, `memset`, `sb_clamp_period` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_request_waiters.c:55`; structural |
| <a id="fn-srw_count"></a>`uint8_t srw_count(const StreamRequestWaiters *manager)` | Operación count de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_request_waiters.c:200`; inferred-name |
| <a id="fn-srw_has_pending"></a>`uint8_t srw_has_pending(const StreamRequestWaiters *manager, uint32_t client_id, uint32_t generation, SbStream stream)` | Coordina `stream_valid` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_request_waiters.c:114`; structural |
| <a id="fn-srw_init"></a>`void srw_init(StreamRequestWaiters *manager)` | Coordina `memset` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_request_waiters.c:49`; structural |
| <a id="fn-srw_metrics"></a>`const SrwMetrics *srw_metrics(const StreamRequestWaiters *manager)` | Operación metrics de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_request_waiters.c:211`; inferred-name |
| <a id="fn-srw_remove_client"></a>`uint8_t srw_remove_client(StreamRequestWaiters *manager, uint32_t client_id, uint32_t generation)` | Coordina `memset` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_request_waiters.c:181`; structural |
| <a id="fn-srw_take_endpoint_stream"></a>`uint8_t srw_take_endpoint_stream(StreamRequestWaiters *manager, uint32_t client_id, uint32_t generation, SbStream stream, SrwCompletionStatus status, SrwCompletion *completion)` | Coordina `stream_valid`, `take_at` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_request_waiters.c:147`; structural |
| <a id="fn-srw_take_expired"></a>`uint8_t srw_take_expired(StreamRequestWaiters *manager, uint32_t now_ms, SrwCompletion *completion)` | Coordina `take_at`, `time_reached` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_request_waiters.c:166`; structural |
| <a id="fn-srw_take_for_stream"></a>`uint8_t srw_take_for_stream(StreamRequestWaiters *manager, SbStream stream, SrwCompletionStatus status, SrwCompletion *completion)` | Coordina `stream_valid`, `take_at` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_request_waiters.c:131`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-abort_pending_for_endpoint"></a>`static void abort_pending_for_endpoint(StreamBroker *broker, const SbEndpoint *endpoint)` | Adapta la llamada a `abort_pending_for_stream` con el contexto de esta integración. | `esp:src/stream_broker.c:132`; structural |
| <a id="fn-abort_pending_for_stream"></a>`static void abort_pending_for_stream(StreamBroker *broker, SbStream stream, const SbEndpoint *endpoint)` | Adapta la llamada a `endpoint_equal` con el contexto de esta integración. | `esp:src/stream_broker.c:120`; structural |
| <a id="fn-action_equal"></a>`static uint8_t action_equal(const SbAction *a, const SbAction *b)` | Adapta la llamada a `endpoint_equal` con el contexto de esta integración. | `esp:src/stream_broker.c:138`; structural |
| <a id="fn-allow_explicit_retry"></a>`static void allow_explicit_retry(StreamBroker *broker, SbStream stream)` | Operación allow explicit retry de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_broker.c:148`; inferred-name |
| <a id="fn-copy_text"></a>`static void copy_text(char *destination, uint8_t capacity, const char *source)` | Adapta la llamada a `strncpy` con el contexto de esta integración. | `esp:src/stream_request_waiters.c:16`; structural |
| <a id="fn-endpoint_equal"></a>`static uint8_t endpoint_equal(const SbEndpoint *a, const SbEndpoint *b)` | Operación endpoint equal de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_broker.c:10`; inferred-name |
| <a id="fn-ensure_subscriber"></a>`static SbSubscriber *ensure_subscriber(StreamBroker *broker, uint32_t client_id, uint32_t generation, uint8_t node)` | Coordina `find_subscriber`, `memset` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_broker.c:98`; structural |
| <a id="fn-find_subscriber"></a>`static SbSubscriber *find_subscriber(StreamBroker *broker, uint32_t client_id, uint32_t generation)` | Operación find subscriber de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_broker.c:18`; inferred-name |
| <a id="fn-owner_is_still_subscribed"></a>`static uint8_t owner_is_still_subscribed(const StreamBroker *broker, SbStream stream, const SbEndpoint *owner)` | Adapta la llamada a `endpoint_equal` con el contexto de esta integración. | `esp:src/stream_broker.c:42`; structural |
| <a id="fn-recompute"></a>`static void recompute(StreamBroker *broker, SbStream stream)` | Coordina `memset`, `owner_is_still_subscribed` para Streams MPU/IR y solicitudes en espera. | `esp:src/stream_broker.c:57`; structural |
| <a id="fn-record_completion"></a>`static void record_completion(StreamRequestWaiters *manager, SrwCompletionStatus status)` | Operación record completion de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_request_waiters.c:24`; inferred-name |
| <a id="fn-stream_valid"></a>`static uint8_t stream_valid(SbStream stream)` | Operación stream valid de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_broker.c:5`; inferred-name |
| <a id="fn-stream_valid"></a>`static uint8_t stream_valid(SbStream stream)` | Operación stream valid de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_request_waiters.c:6`; inferred-name |
| <a id="fn-subscriber_is_empty"></a>`static uint8_t subscriber_is_empty(const SbSubscriber *subscriber)` | Operación subscriber is empty de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_broker.c:35`; inferred-name |
| <a id="fn-take_at"></a>`static uint8_t take_at(StreamRequestWaiters *manager, uint8_t index, SrwCompletionStatus status, SrwCompletion *completion)` | Adapta la llamada a `record_completion` con el contexto de esta integración. | `esp:src/stream_request_waiters.c:33`; structural |
| <a id="fn-time_reached"></a>`static uint8_t time_reached(uint32_t now_ms, uint32_t deadline_ms)` | Operación time reached de Streams MPU/IR y solicitudes en espera; semántica fina pendiente de ampliación. | `esp:src/stream_request_waiters.c:11`; inferred-name |
