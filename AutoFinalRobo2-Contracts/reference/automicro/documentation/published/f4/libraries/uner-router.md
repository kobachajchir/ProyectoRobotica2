# UNER Handle, rutas y despacho

ID: `f4:module/Core/Inc/uner_handle.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Conecta parser, transportes, rutas, guardas y tabla de comandos para despachar o reenviar paquetes.

Registro de hasta cuatro transportes y seis rutas, control de forwarding, snapshots y callbacks de aplicación.

<a id="modularity"></a>
## Modularidad observada

Sin HAL, con relación interna legítima entre core/router/transport. El ejecutor de aplicación define el significado del comando.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

UNER_Handle posee core, tablas/rutas, tx_buf, contadores y snapshots; presta transportes, command_table, execute_command/ctx y forward_guard/ctx. CommandSpec incluye ID, límites de longitud, flags, timeout y handler; los metadatos no reemplazan el comportamiento real del handler.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init envuelve on_packet; registrar transportes, rutas, comandos y guardas antes de operar.
- Poll obtiene bytes y ProcessPending consume paquetes encolados.
- Paquetes locales pasan por tabla/longitud; otros pueden reenviarse tras guarda y resolución de ruta.

## Trabajo en tándem

- UNER_App conecta handlers y políticas del robot.
- UNER_Send construye en tx_buf y llama try_send; la propiedad del buffer importa si el transporte difiere su uso.

## Precauciones y límites

- Un callback o macro con nombre de función no es una llamada directa resuelta.
- Metadatos de respuesta y handler pueden divergir: ver registro de hallazgos.
- La cola y buffers no ofrecen por sí mismos garantías para varios threads.

## Hallazgos vinculados

- [AMDOC-F4-003: La marca RESP permite longitudes fuera del rango de solicitud](../../../review/FINDINGS.md#amdoc-f4-003)
- [AMDOC-F4-004: BuildFrame acepta payload nulo con longitud positiva](../../../review/FINDINGS.md#amdoc-f4-004)
- [AMDOC-F4-006: 0x74 declara ACK en F4 pero el handler responde por el mismo comando](../../../review/FINDINGS.md#amdoc-f4-006)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/uner_handle.h` | `uner_handle_types.h` | `cc82190fc0b816590d5f2827c106019c2186866454674b9693946bfe91b28aa1` |
| `Core/Inc/uner_handle_types.h` | `stdint.h`, `uner_transport.h` | `fb11a5d2404fc456a4a48f1d43855a01a878c2bbe887dc485c0c9e298376de65` |
| `Core/Inc/uner_transport.h` | `stdint.h`, `uner_core.h` | `b746b135dea65c7daf3462844d69847d45e489623feae46b62557518f65ac975` |
| `Core/Src/uner_handle.c` | `uner_handle.h`, `string.h` | `9f28e9ea0a873f5bddbbce52a1777953e38697787f9720404720cac6b03d2174` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `UNER_CommandSpec`

`f4:Core/Inc/uner_handle_types.h:24`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t id` | ID de comando/evento al que aplica esta entrada. |
| `uint8_t min_args` | Longitud mínima de solicitud declarada; ver excepción RESP en AMDOC-F4-003. |
| `uint8_t max_args` | Longitud máxima de solicitud declarada; no sustituye guardas locales. |
| `uint8_t flags` | Bits de estado del módulo; conservar máscaras y distinguir estado de eventos pendientes. |
| `uint16_t timeout_ms` | Timeout declarado por la tabla F4; no equivale al timeout del cliente Web. |
| `UNER_CommandHandler handler` | Handler específico; si es nulo el handle usa execute_command registrado. |

### `UNER_RouteEntry`

`f4:Core/Inc/uner_handle_types.h:38`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t node` | Dato de estado/configuración `node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |
| `uint8_t enabled` | Dato de estado/configuración `enabled`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_RouterSnapshot`

`f4:Core/Inc/uner_handle_types.h:62`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t valid` | Dato de estado/configuración `valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t forwarding_enabled` | Dato de estado/configuración `forwarding_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t direction` | Dato de estado/configuración `direction`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t src` | Dato de estado/configuración `src`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t dst` | Dato de estado/configuración `dst`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cmd` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t len` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t in_transport` | Dato de estado/configuración `in_transport`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t out_transport` | Dato de estado/configuración `out_transport`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint8_t payload_preview_len` | Dato de estado/configuración `payload_preview_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t payload_preview[UNER_ROUTER_PAYLOAD_PREVIEW_BYTES]` | Dato de estado/configuración `payload_preview`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t sequence` | Dato de estado/configuración `sequence`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t forwarded_frames` | Dato de estado/configuración `forwarded_frames`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t route_miss` | Dato de estado/configuración `route_miss`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t route_tx_busy` | Dato de estado/configuración `route_tx_busy`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_Handle`

`f4:Core/Inc/uner_handle_types.h:81`

| Atributo / firma | Función y precaución |
| --- | --- |
| `UNER_Core core` | Dato de estado/configuración `core`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_Transport *transports[UNER_MAX_TRANSPORTS]` | Dato de estado/configuración `transports`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t transport_count` | Dato de estado/configuración `transport_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_RouteEntry routes[UNER_MAX_ROUTES]` | Dato de estado/configuración `routes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t route_count` | Dato de estado/configuración `route_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_ForwardGuardFn forward_guard` | Dato de estado/configuración `forward_guard`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *forward_guard_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `uint8_t forwarding_enabled` | Dato de estado/configuración `forwarding_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_RouterSnapshot router_snapshot` | Dato de estado/configuración `router_snapshot`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_RouterSnapshot rx_snapshot` | Dato de estado/configuración `rx_snapshot`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_RouterSnapshot tx_snapshot` | Dato de estado/configuración `tx_snapshot`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_RouterSnapshot rx_transport_snapshots[UNER_MAX_TRANSPORTS]` | Dato de estado/configuración `rx_transport_snapshots`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_RouterSnapshot tx_transport_snapshots[UNER_MAX_TRANSPORTS]` | Dato de estado/configuración `tx_transport_snapshots`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t packet_pending` | Señal de que hay trabajo de parser para despachar; no almacena un paquete. |
| `const UNER_CommandSpec *command_table` | Tabla prestada de comandos que debe permanecer válida durante el uso. |
| `uint8_t command_count` | Dato de estado/configuración `command_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_ExecuteCommandFn execute_command` | Ejecutor de aplicación para comandos sin handler individual. |
| `void *execute_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `UNER_OnPacketCb app_on_packet` | Callback del consumidor conservado al instalar el wrapper interno de encolado. |
| `void *app_on_packet_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `uint8_t tx_buf[UNER_OVERHEAD + 255u]` | Buffer de construcción de frames de esta instancia; no conservar una referencia tras el siguiente envío. |
| `uint32_t forwarded_frames` | Dato de estado/configuración `forwarded_frames`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t route_miss` | Dato de estado/configuración `route_miss`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t route_tx_busy` | Dato de estado/configuración `route_tx_busy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t packet_sequence` | Dato de estado/configuración `packet_sequence`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNER_CommandHandler`

`f4:Core/Inc/uner_handle_types.h:17`

Firma: `void (*UNER_CommandHandler)(void *ctx, const UNER_Packet *packet)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `UNER_ExecuteCommandFn`

`f4:Core/Inc/uner_handle_types.h:32`

Firma: `void (*UNER_ExecuteCommandFn)(void *ctx, const UNER_Packet *packet)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `UNER_ForwardGuardFn`

`f4:Core/Inc/uner_handle_types.h:34`

Firma: `uint8_t (*UNER_ForwardGuardFn)(void *ctx, const UNER_Packet *packet, uint8_t out_transport_id)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `UNER_Transport`

`f4:Core/Inc/uner_transport.h:19`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t id` | ID lógico del transporte, distinto del nodo UNER. |
| `uint8_t max_payload` | Límite de payload admitido por este transporte. |
| `UNER_TransportPollRxFn poll_rx` | Dato de estado/configuración `poll_rx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UNER_TransportTrySendFn try_send` | Dato de estado/configuración `try_send`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |

### `UNER_TransportPollRxFn`

`f4:Core/Inc/uner_transport.h:15`

Firma: `UNER_Status (*UNER_TransportPollRxFn)(UNER_Transport *transport, UNER_Core *core)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `UNER_TransportTrySendFn`

`f4:Core/Inc/uner_transport.h:17`

Firma: `UNER_Status (*UNER_TransportTrySendFn)(UNER_Transport *transport, const uint8_t *buf, uint16_t len)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-uner_handle_getrouterpacketsnapshot"></a>`void UNER_Handle_GetRouterPacketSnapshot(const UNER_Handle *handle, uint8_t tx_view, uint8_t transport_id, UNER_RouterSnapshot *out)` | Copia diagnóstico RX/TX del transporte solicitado. | `f4:Core/Inc/uner_handle.h:23`; editorial |
| <a id="fn-uner_handle_getroutersnapshot"></a>`void UNER_Handle_GetRouterSnapshot(const UNER_Handle *handle, UNER_RouterSnapshot *out)` | Copia el último estado resumido del router. | `f4:Core/Inc/uner_handle.h:22`; editorial |
| <a id="fn-uner_handle_init"></a>`UNER_Status UNER_Handle_Init( UNER_Handle *handle, const UNER_CoreConfig *cfg, UNER_Packet *slots, uint8_t slot_count, uint8_t *payload_pool_bytes, uint16_t payload_pool_size)` | Inicializa router/core conservando callback de aplicación. | `f4:Core/Inc/uner_handle.h:7`; editorial |
| <a id="fn-uner_handle_isforwardingenabled"></a>`uint8_t UNER_Handle_IsForwardingEnabled(const UNER_Handle *handle)` | Consulta si el reenvío está habilitado. | `f4:Core/Inc/uner_handle.h:21`; editorial |
| <a id="fn-uner_handle_poll"></a>`void UNER_Handle_Poll(UNER_Handle *handle)` | Atiende transportes y procesa trabajo pendiente. | `f4:Core/Inc/uner_handle.h:33`; editorial |
| <a id="fn-uner_handle_processpending"></a>`void UNER_Handle_ProcessPending(UNER_Handle *handle)` | Despacha o reenvía paquetes consumidos de la cola. | `f4:Core/Inc/uner_handle.h:34`; editorial |
| <a id="fn-uner_handle_registercommands"></a>`UNER_Status UNER_Handle_RegisterCommands( UNER_Handle *handle, const UNER_CommandSpec *table, uint8_t count, UNER_ExecuteCommandFn execute_command, void *execute_ctx)` | Conecta tabla de comandos y ejecutor con contexto. | `f4:Core/Inc/uner_handle.h:27`; editorial |
| <a id="fn-uner_handle_registerroute"></a>`UNER_Status UNER_Handle_RegisterRoute(UNER_Handle *handle, uint8_t node, uint8_t transport_id)` | Asocia un nodo destino a un transporte. | `f4:Core/Inc/uner_handle.h:16`; editorial |
| <a id="fn-uner_handle_registertransport"></a>`UNER_Status UNER_Handle_RegisterTransport(UNER_Handle *handle, UNER_Transport *transport)` | Agrega un transporte con ID, polling y envío. | `f4:Core/Inc/uner_handle.h:15`; editorial |
| <a id="fn-uner_handle_setforwardguard"></a>`void UNER_Handle_SetForwardGuard(UNER_Handle *handle, UNER_ForwardGuardFn guard, void *ctx)` | Registra guarda de forwarding y contexto. | `f4:Core/Inc/uner_handle.h:18`; editorial |
| <a id="fn-uner_handle_setforwardingenabled"></a>`void UNER_Handle_SetForwardingEnabled(UNER_Handle *handle, uint8_t enabled)` | Habilita o bloquea reenvío de paquetes. | `f4:Core/Inc/uner_handle.h:17`; editorial |
| <a id="fn-uner_send"></a>`UNER_Status UNER_Send( UNER_Handle *handle, uint8_t transport_id, uint8_t src, uint8_t dst, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Construye y ofrece un frame al transporte indicado. | `f4:Core/Inc/uner_handle.h:36`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-uner_handle_onpacketqueued"></a>`static void UNER_Handle_OnPacketQueued(void *ctx, const UNER_Packet *p)` | Atiende la notificación de Packet Queued dentro de UNER Handle, rutas y despacho. | `f4:Core/Src/uner_handle.c:8`; inferred-name |
| <a id="fn-uner_handle_israwasyncpush"></a>`static uint8_t UNER_Handle_IsRawAsyncPush(const UNER_Packet *packet)` | Indica si se cumple la condición Raw Async Push dentro de UNER Handle, rutas y despacho. | `f4:Core/Src/uner_handle.c:20`; inferred-name |
| <a id="fn-uner_handle_islengthvalid"></a>`static uint8_t UNER_Handle_IsLengthValid(const UNER_CommandSpec *spec, const UNER_Packet *packet)` | Adapta la llamada a `UNER_Handle_IsRawAsyncPush` con el contexto de esta integración. | `f4:Core/Src/uner_handle.c:33`; structural |
| <a id="fn-uner_handle_findcommandspec"></a>`static uint8_t UNER_Handle_FindCommandSpec( const UNER_CommandSpec *table, uint8_t count, uint8_t cmd, UNER_CommandSpec *out)` | Operación Find Command Spec de UNER Handle, rutas y despacho; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_handle.c:54`; inferred-name |
| <a id="fn-uner_handle_findtransport"></a>`static UNER_Transport *UNER_Handle_FindTransport(UNER_Handle *handle, uint8_t transport_id)` | Operación Find Transport de UNER Handle, rutas y despacho; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_handle.c:73`; inferred-name |
| <a id="fn-uner_handle_findroute"></a>`static uint8_t UNER_Handle_FindRoute(UNER_Handle *handle, uint8_t node, uint8_t *transport_id)` | Operación Find Route de UNER Handle, rutas y despacho; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_handle.c:89`; inferred-name |
| <a id="fn-uner_handle_nextpacketsequence"></a>`static uint32_t UNER_Handle_NextPacketSequence(UNER_Handle *handle)` | Operación Next Packet Sequence de UNER Handle, rutas y despacho; semántica fina pendiente de ampliación. | `f4:Core/Src/uner_handle.c:105`; inferred-name |
| <a id="fn-uner_handle_fillpacketsnapshot"></a>`static void UNER_Handle_FillPacketSnapshot(UNER_Handle *handle, UNER_RouterSnapshot *snapshot, const UNER_Packet *packet, uint8_t direction, uint8_t out_transport, uint8_t status, uint32_t sequence)` | Coordina `memcpy`, `memset` para UNER Handle, rutas y despacho. | `f4:Core/Src/uner_handle.c:119`; structural |
| <a id="fn-uner_handle_recordpacketsnapshot"></a>`static void UNER_Handle_RecordPacketSnapshot(UNER_Handle *handle, const UNER_Packet *packet, uint8_t direction, uint8_t out_transport, uint8_t status)` | Coordina `UNER_Handle_FillPacketSnapshot`, `UNER_Handle_NextPacketSequence` para UNER Handle, rutas y despacho. | `f4:Core/Src/uner_handle.c:159`; structural |
| <a id="fn-uner_handle_recordrouterpacket"></a>`static void UNER_Handle_RecordRouterPacket(UNER_Handle *handle, const UNER_Packet *packet, uint8_t out_transport, uint8_t status)` | Coordina `UNER_Handle_FillPacketSnapshot` para UNER Handle, rutas y despacho. | `f4:Core/Src/uner_handle.c:205`; structural |
| <a id="fn-uner_handle_forwardtotransport"></a>`static UNER_Status UNER_Handle_ForwardToTransport(UNER_Handle *handle, const UNER_Packet *packet, uint8_t transport_id)` | Coordina `UNER_BuildFrame`, `UNER_Handle_FindTransport`, `UNER_Handle_RecordPacketSnapshot` para UNER Handle, rutas y despacho. | `f4:Core/Src/uner_handle.c:223`; structural |
| <a id="fn-uner_handle_forwardpacket"></a>`static void UNER_Handle_ForwardPacket(UNER_Handle *handle, const UNER_Packet *packet)` | Coordina `UNER_Handle_FindRoute`, `UNER_Handle_ForwardToTransport`, `UNER_Handle_RecordRouterPacket` para UNER Handle, rutas y despacho. | `f4:Core/Src/uner_handle.c:331`; structural |
