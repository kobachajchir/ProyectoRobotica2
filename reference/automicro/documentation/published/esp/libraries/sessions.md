# Sesiones WebSocket y autoridad STM

ID: `esp:module/src/ws_session_manager.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Mantiene identidad cliente/generación/nodo y autoridad STM independiente por sesión.

Capacidad, TTL, intentos/rate limit, suspensión, reanudación por token y limpieza.

<a id="modularity"></a>
## Modularidad observada

C sin socket concreto; adopta reglas del producto y nodos UNER.

<a id="state"></a>
## Handle, estado y atributos externos

WsSessionManager posee slots y buckets. WsmSession contiene token privado, nodo, generación y deadlines; esp_debug_wifi es permiso local ESP y no concede autoridad F4.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Open asigna slot/nodo/generación; autenticar establece TTL.
- Touch solo renueva sesión activa y no revive una ya expirada.
- Suspender conserva autoridad temporalmente pero no la usa hasta reanudación por token propio; cambia generación.

<a id="interactions"></a>
## Interacciones

- Bridge usa sesiones para autorizar requests/fanout; UTM y streams conservan la misma generación.

<a id="limitations"></a>
## Precauciones y límites

- No publicar tokens ni PIN en documentación/snapshots.
- Rango de nodos reservado no equivale a capacidad simultánea; WS_MAX_SESSIONS es la capacidad.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/ws_limits.h` | c-cpp-structural | `82e09a65340126531042acf552784466f6ae91516ba80a2688c157e0fd9768c3` |
| `src/ws_session_manager.c` | c-cpp-structural | `16dc59e82d03ba80761afd4b626a384e12e1a7a9ffc7bdac4f6574a6736d66d5` |
| `src/ws_session_manager.h` | c-cpp-structural | `a601065af179c660f263905ae645796fcd7717a9091724259bcda4dcb4b0a81c` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WsmSession`

`esp:src/ws_session_manager.h:38`

| Atributo / firma | Propósito |
| --- | --- |
| `WsmSlotState state` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t client_id` | Dato de estado/configuración `client_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ip_key` | Dato de estado/configuración `ip_key`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t generation` | Dato de estado/configuración `generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t node` | Dato de estado/configuración `node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t stm_authenticated` | Dato de estado/configuración `stm_authenticated`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t esp_debug_used` | Dato de estado/configuración `esp_debug_used`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t esp_debug_wifi` | Dato de estado/configuración `esp_debug_wifi`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stm_ttl_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t stm_expires_at_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t resume_until_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `char resume_token[WSM_RESUME_TOKEN_HEX_LEN + 1u]` | Dato de estado/configuración `resume_token`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WsmRateBucket`

`esp:src/ws_session_manager.h:53`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t used` | Dato de estado/configuración `used`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t key_type` | Dato de estado/configuración `key_type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t key` | Dato de estado/configuración `key`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t window_started_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t attempts` | Dato de estado/configuración `attempts`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WsmMetrics`

`esp:src/ws_session_manager.h:61`

| Atributo / firma | Propósito |
| --- | --- |
| `uint32_t opened` | Dato de estado/configuración `opened`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t capacity_rejections` | Dato de estado/configuración `capacity_rejections`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t cleanup_started` | Dato de estado/configuración `cleanup_started`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t released` | Dato de estado/configuración `released`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stale_generation_rejections` | Dato de estado/configuración `stale_generation_rejections`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t rate_limited` | Dato de estado/configuración `rate_limited`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stm_auth_granted` | Dato de estado/configuración `stm_auth_granted`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stm_auth_revoked` | Dato de estado/configuración `stm_auth_revoked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stm_auth_expired` | Dato de estado/configuración `stm_auth_expired`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t stm_auth_refreshed` | Dato de estado/configuración `stm_auth_refreshed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t reconnect_suspended` | Dato de estado/configuración `reconnect_suspended`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t reconnect_resumed` | Dato de estado/configuración `reconnect_resumed`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WsSessionManager`

`esp:src/ws_session_manager.h:76`

| Atributo / firma | Propósito |
| --- | --- |
| `WsmSession sessions[WSM_MAX_SESSIONS]` | Dato de estado/configuración `sessions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WsmRateBucket rate_buckets[WSM_RATE_BUCKET_COUNT]` | Dato de estado/configuración `rate_buckets`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t next_generation` | Dato de estado/configuración `next_generation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t rate_window_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t rate_max_attempts` | Dato de estado/configuración `rate_max_attempts`; declaración estructural, interpretación específica pendiente de ampliar. |
| `WsmMetrics metrics` | Dato de estado/configuración `metrics`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-wsm_allow_auth_attempt"></a>`uint8_t wsm_allow_auth_attempt(WsSessionManager *manager, uint32_t client_id, uint32_t generation, uint32_t now_ms)` | Coordina `rate_bucket`, `wsm_find` para Sesiones WebSocket y autoridad STM. | `esp:src/ws_session_manager.c:344`; structural |
| <a id="fn-wsm_begin_cleanup"></a>`WsmResult wsm_begin_cleanup(WsSessionManager *manager, uint32_t client_id, uint32_t generation)` | Coordina `wsm_find` para Sesiones WebSocket y autoridad STM. | `esp:src/ws_session_manager.c:208`; structural |
| <a id="fn-wsm_find"></a>`WsmSession *wsm_find(WsSessionManager *manager, uint32_t client_id, uint32_t generation)` | Adapta la llamada a `generation_matches` con el contexto de esta integración. | `esp:src/ws_session_manager.c:117`; structural |
| <a id="fn-wsm_find_by_node"></a>`WsmSession *wsm_find_by_node(WsSessionManager *manager, uint8_t node)` | Operación find by node de Sesiones WebSocket y autoridad STM; semántica fina pendiente de ampliación. | `esp:src/ws_session_manager.c:145`; inferred-name |
| <a id="fn-wsm_find_const"></a>`const WsmSession *wsm_find_const(const WsSessionManager *manager, uint32_t client_id, uint32_t generation)` | Adapta la llamada a `generation_matches` con el contexto de esta integración. | `esp:src/ws_session_manager.c:131`; structural |
| <a id="fn-wsm_init"></a>`void wsm_init(WsSessionManager *manager, uint32_t rate_window_ms, uint16_t rate_max_attempts)` | Coordina `memset` para Sesiones WebSocket y autoridad STM. | `esp:src/ws_session_manager.c:67`; structural |
| <a id="fn-wsm_is_authorized"></a>`uint8_t wsm_is_authorized(const WsSessionManager *manager, uint32_t client_id, uint32_t generation)` | Adapta la llamada a `wsm_find_const` con el contexto de esta integración. | `esp:src/ws_session_manager.c:369`; structural |
| <a id="fn-wsm_metrics"></a>`const WsmMetrics *wsm_metrics(const WsSessionManager *manager)` | Operación metrics de Sesiones WebSocket y autoridad STM; semántica fina pendiente de ampliación. | `esp:src/ws_session_manager.c:398`; inferred-name |
| <a id="fn-wsm_open"></a>`WsmResult wsm_open(WsSessionManager *manager, uint32_t client_id, uint32_t ip_key, WsmSession **out_session)` | Reserva sesión con identidad de cliente y nodo. | `esp:src/ws_session_manager.c:82`; editorial |
| <a id="fn-wsm_release"></a>`WsmResult wsm_release(WsSessionManager *manager, uint32_t client_id, uint32_t generation)` | Libera sesión y su identidad después de limpieza. | `esp:src/ws_session_manager.c:329`; editorial |
| <a id="fn-wsm_resume_by_ip"></a>`WsmResult wsm_resume_by_ip(WsSessionManager *manager, uint32_t new_client_id, uint32_t ip_key, uint32_t now_ms, WsmSession **out_session)` | Coordina `allocate_generation`, `time_reached` para Sesiones WebSocket y autoridad STM. | `esp:src/ws_session_manager.c:241`; structural |
| <a id="fn-wsm_resume_by_token"></a>`WsmResult wsm_resume_by_token(WsSessionManager *manager, uint32_t new_client_id, const char *resume_token, uint32_t now_ms, WsmSession **out_session)` | Reanuda con token propio y asigna generación nueva. | `esp:src/ws_session_manager.c:296`; editorial |
| <a id="fn-wsm_service"></a>`uint8_t wsm_service(WsSessionManager *manager, uint32_t now_ms)` | Coordina `time_reached` para Sesiones WebSocket y autoridad STM. | `esp:src/ws_session_manager.c:378`; structural |
| <a id="fn-wsm_set_stm_auth"></a>`WsmResult wsm_set_stm_auth(WsSessionManager *manager, uint32_t client_id, uint32_t generation, uint8_t authenticated, uint32_t ttl_ms, uint32_t now_ms)` | Coordina `wsm_find` para Sesiones WebSocket y autoridad STM. | `esp:src/ws_session_manager.c:153`; structural |
| <a id="fn-wsm_suspend_for_reconnect"></a>`WsmResult wsm_suspend_for_reconnect(WsSessionManager *manager, uint32_t client_id, uint32_t generation, uint32_t now_ms, uint32_t grace_ms)` | Coordina `wsm_find` para Sesiones WebSocket y autoridad STM. | `esp:src/ws_session_manager.c:224`; structural |
| <a id="fn-wsm_touch_stm_auth"></a>`WsmResult wsm_touch_stm_auth(WsSessionManager *manager, uint32_t client_id, uint32_t generation, uint32_t now_ms)` | Renueva una autorización todavía activa sin revivir una vencida. | `esp:src/ws_session_manager.c:182`; editorial |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-allocate_generation"></a>`static uint32_t allocate_generation(WsSessionManager *manager)` | Operación allocate generation de Sesiones WebSocket y autoridad STM; semántica fina pendiente de ampliación. | `esp:src/ws_session_manager.c:20`; inferred-name |
| <a id="fn-generation_matches"></a>`static uint8_t generation_matches(const WsmSession *session, uint32_t client_id, uint32_t generation)` | Operación generation matches de Sesiones WebSocket y autoridad STM; semántica fina pendiente de ampliación. | `esp:src/ws_session_manager.c:10`; inferred-name |
| <a id="fn-rate_bucket"></a>`static WsmRateBucket *rate_bucket(WsSessionManager *manager, uint8_t key_type, uint32_t key, uint32_t now_ms)` | Coordina `memset`, `time_reached` para Sesiones WebSocket y autoridad STM. | `esp:src/ws_session_manager.c:28`; structural |
| <a id="fn-time_reached"></a>`static uint8_t time_reached(uint32_t now_ms, uint32_t deadline_ms)` | Operación time reached de Sesiones WebSocket y autoridad STM; semántica fina pendiente de ampliación. | `esp:src/ws_session_manager.c:5`; inferred-name |
