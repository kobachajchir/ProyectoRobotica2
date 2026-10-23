# API JSON, Promises y WebSocket

ID: `web:module/src/protocol/wsApi.ts` · Capa: **servicio** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Construye requests, conecta socket, correlaciona respuestas y entrega eventos a consumidores.

API 1, requestId/command, timeout, reconnect y listeners; un evento requestAccepted no resuelve la Promise.

<a id="modularity"></a>
## Modularidad observada

wsApi/EspClient separan sobre y correlación del componente React; provider depende de navegador y transporte WebSocket.

<a id="state"></a>
## Handle, estado y atributos externos

EspClient posee Map de pendientes y listeners; el sender es prestado por el provider. Cada pending conserva resolve/reject/timer y command. Callbacks/closures duran hasta finalización, cancelación o unsubscribe.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Conectar sender al abrir; desconectarlo cancela solicitudes pendientes.
- request registra antes de enviar, con timeout configurable.
- accept entrega eventos o completa respuesta/error correlacionado; cleanup debe retirar listeners/timers.

<a id="interactions"></a>
## Interacciones

- ESP traduce a UNER y mantiene correlación/generación; el navegador no elige su nodo.
- Los contextos consumen eventos y valores JSON, no memoria de structs C.

<a id="limitations"></a>
## Precauciones y límites

- Tipos TypeScript no validan automáticamente contenido remoto.
- Timeout cliente, broker y operación física son plazos distintos.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/contexts/WebSocketContext.tsx` | typescript-ast-static | `5107635387dd61c6edf766f61c9b12aff1cb1b9fb851f75495d1c9ba23704226` |
| `src/hooks/useEspCommand.ts` | typescript-ast-static | `bde836257f8dd9c76eddca2afc29f90830407791ed791e65a34d803d9f42f62a` |
| `src/hooks/useWebSocket.tsx` | typescript-ast-static | `f77eaff6d4ea41f42014dddfbb1253f2538ab5842c908b962e147a3a1aceffe1` |
| `src/protocol/espClient.ts` | typescript-ast-static | `d8189d8c5bb46d1dc74ef976ee56115342bbc82a5119da75fa823e1b6323bd07` |
| `src/protocol/wsApi.ts` | typescript-ast-static | `2afd850fb874b68677249d0c22232876462e5bd3de00b1bbc93d5984e0b72af5` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `WSMessageHandler`

`web:src/contexts/WebSocketContext.tsx:22`

Tipo: `(data: unknown) => void`.

### `WSRawHandler`

`web:src/contexts/WebSocketContext.tsx:23`

Tipo: `(data: ArrayBuffer \| Uint8Array) => void`.

### `WSDataPacketMeta`

`web:src/contexts/WebSocketContext.tsx:24`

Tipo: `Record<string, unknown>`.

### `ConnectionPhase`

`web:src/contexts/WebSocketContext.tsx:26`

Tipo: `\| "idle" \| "connecting" \| "authenticating" \| "ready" \| "retry_wait" \| "failed"`.

### `ConnectionHealth`

`web:src/contexts/WebSocketContext.tsx:34`

Tipo: `"connecting" \| "ready" \| "degraded" \| "offline"`.

### `HeartbeatConfig`

`web:src/contexts/WebSocketContext.tsx:36`

| Atributo / firma | Propósito |
| --- | --- |
| `intervalMs: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `maxRetries: number` | Dato de estado/configuración `maxRetries`; declaración estructural, interpretación específica pendiente de ampliar. |
| `isActive: boolean` | Dato de estado/configuración `isActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `remainingRetries: number` | Dato de estado/configuración `remainingRetries`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WebSocketContextType`

`web:src/contexts/WebSocketContext.tsx:43`

| Atributo / firma | Propósito |
| --- | --- |
| `connected: boolean` | Dato de estado/configuración `connected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `setConnected: (state: boolean) => void` | Operación set Connected de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `connectionPhase: ConnectionPhase` | Dato de estado/configuración `connectionPhase`; declaración estructural, interpretación específica pendiente de ampliar. |
| `connectionHealth: ConnectionHealth` | Dato de estado/configuración `connectionHealth`; declaración estructural, interpretación específica pendiente de ampliar. |
| `hello: EspHello \| null` | Dato de estado/configuración `hello`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastError: EspApiError \| Error \| null` | Dato de estado/configuración `lastError`; declaración estructural, interpretación específica pendiente de ampliar. |
| `mockMode: boolean` | Dato de estado/configuración `mockMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `request: <T>(command: CommandName, args?: Record<string, unknown>, options?: { requestId?: string; timeoutMs?: number }) => Promise<T>` | Solicita propiedad del bus con prioridad; la concesión llega por on_granted. |
| `subscribeEvent: (event: string, handler: (event: WsEvent) => void) => () => void` | Operación subscribe Event de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `reconnect: () => void` | Operación reconnect de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `send: (type: string, payload?: unknown) => void` | Operación send de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `subscribe: (type: string, handler: WSMessageHandler) => () => void` | Registra listener y devuelve una función para retirarlo. |
| `sendRaw: (data: Uint8Array, meta?: WSDataPacketMeta) => void` | Operación send Raw de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `subscribeRaw: (handler: WSRawHandler) => () => void` | Operación subscribe Raw de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `disconnect: () => void` | Operación disconnect de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `mockMessage: (type: string, payload?: unknown) => void` | Operación mock Message de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `mockRaw: (data: Uint8Array) => void` | Operación mock Raw de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `heartbeatConfig: HeartbeatConfig` | Dato de estado/configuración `heartbeatConfig`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lastHeartbeatAt: number \| null` | Dato de estado/configuración `lastHeartbeatAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `setHeartbeatInterval: (ms: number) => void` | Operación set Heartbeat Interval de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `setHeartbeatMaxRetries: (retries: number) => void` | Operación set Heartbeat Max Retries de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `toggleHeartbeatWatchdog: () => void` | Operación toggle Heartbeat Watchdog de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `resetHeartbeatWatchdog: () => void` | Operación reset Heartbeat Watchdog de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `onHeartbeatReceived: () => void` | Operación on Heartbeat Received de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `retrying: boolean` | Dato de estado/configuración `retrying`; declaración estructural, interpretación específica pendiente de ampliar. |
| `setRetrying: (newVal: boolean) => void` | Operación set Retrying de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `setShowRetryModal: (newVal: boolean) => void` | Operación set Show Retry Modal de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `sensorRefreshInterval: number` | Dato de estado/configuración `sensorRefreshInterval`; declaración estructural, interpretación específica pendiente de ampliar. |
| `setSensorRefreshInterval: (newVal: number) => void` | Operación set Sensor Refresh Interval de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |

### `WebSocketProviderProps`

`web:src/contexts/WebSocketContext.tsx:77`

| Atributo / firma | Propósito |
| --- | --- |
| `url: string` | Dato de estado/configuración `url`; declaración estructural, interpretación específica pendiente de ampliar. |
| `children: ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EspApiError`

`web:src/protocol/espClient.ts:10`

| Atributo / firma | Propósito |
| --- | --- |
| `code: string` | Dato de estado/configuración `code`; declaración estructural, interpretación específica pendiente de ampliar. |
| `details?: unknown` | Dato de estado/configuración `details`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PendingRequest`

`web:src/protocol/espClient.ts:26`

| Atributo / firma | Propósito |
| --- | --- |
| `command: CommandName` | Dato de estado/configuración `command`; declaración estructural, interpretación específica pendiente de ampliar. |
| `resolve: (data: unknown) => void` | Operación resolve de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `reject: (error: Error) => void` | Operación reject de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `timer: ReturnType<typeof setTimeout>` | Dato de estado/configuración `timer`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EventHandler`

`web:src/protocol/espClient.ts:33`

Tipo: `(event: WsEvent) => void`.

### `EspClient`

`web:src/protocol/espClient.ts:35`

| Atributo / firma | Propósito |
| --- | --- |
| `pending: inferred-by-TypeScript` | Dato de estado/configuración `pending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `eventListeners: inferred-by-TypeScript` | Dato de estado/configuración `eventListeners`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sender: ((text: string) => void) \| null` | Dato de estado/configuración `sender`; declaración estructural, interpretación específica pendiente de ampliar. |
| `setSender(sender: ((text: string) => void) \| null): void` | Operación set Sender de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. |
| `request(command: CommandName, args: Record<string, unknown>, options: { requestId?: string; timeoutMs?: number }): Promise<T>` | Solicita propiedad del bus con prioridad; la concesión llega por on_granted. |
| `accept(value: unknown): WsEnvelope \| null` | Despacha evento o completa el pending correlacionado por requestId/command. |
| `subscribe(event: string, handler: EventHandler): () => void` | Registra listener y devuelve una función para retirarlo. |
| `cancelPending(code: inferred-by-TypeScript, message: inferred-by-TypeScript): void` | Rechaza pendientes y limpia sus timers al cancelar la conexión. |
| `pendingCount: number` | Dato de estado/configuración `pendingCount`; declaración estructural, interpretación específica pendiente de ampliar. |

### `CommandName`

`web:src/protocol/wsApi.ts:71`

Tipo: `\| (typeof ESP_COMMANDS)[keyof typeof ESP_COMMANDS] \| string`.

### `WsRequest`

`web:src/protocol/wsApi.ts:75`

| Atributo / firma | Propósito |
| --- | --- |
| `api: 1` | Dato de estado/configuración `api`; declaración estructural, interpretación específica pendiente de ampliar. |
| `type: "request"` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `requestId: string` | Dato de estado/configuración `requestId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `command: CommandName` | Dato de estado/configuración `command`; declaración estructural, interpretación específica pendiente de ampliar. |
| `args: Record<string, unknown>` | Dato de estado/configuración `args`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WsResponse`

`web:src/protocol/wsApi.ts:83`

| Atributo / firma | Propósito |
| --- | --- |
| `api: 1` | Dato de estado/configuración `api`; declaración estructural, interpretación específica pendiente de ampliar. |
| `type: "response"` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `requestId: string` | Dato de estado/configuración `requestId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ok: true` | Dato de estado/configuración `ok`; declaración estructural, interpretación específica pendiente de ampliar. |
| `command: CommandName` | Dato de estado/configuración `command`; declaración estructural, interpretación específica pendiente de ampliar. |
| `data: T` | Dato de estado/configuración `data`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WsEvent`

`web:src/protocol/wsApi.ts:92`

| Atributo / firma | Propósito |
| --- | --- |
| `api: 1` | Dato de estado/configuración `api`; declaración estructural, interpretación específica pendiente de ampliar. |
| `type: "event"` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `event: string` | Dato de estado/configuración `event`; declaración estructural, interpretación específica pendiente de ampliar. |
| `data: T` | Dato de estado/configuración `data`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WsError`

`web:src/protocol/wsApi.ts:99`

| Atributo / firma | Propósito |
| --- | --- |
| `api: 1` | Dato de estado/configuración `api`; declaración estructural, interpretación específica pendiente de ampliar. |
| `type: "error"` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `requestId?: string` | Dato de estado/configuración `requestId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `code: WsErrorCode \| string` | Dato de estado/configuración `code`; declaración estructural, interpretación específica pendiente de ampliar. |
| `message: string` | Dato de estado/configuración `message`; declaración estructural, interpretación específica pendiente de ampliar. |
| `details?: unknown` | Dato de estado/configuración `details`; declaración estructural, interpretación específica pendiente de ampliar. |

### `WsEnvelope`

`web:src/protocol/wsApi.ts:108`

Tipo: `WsResponse \| WsEvent \| WsError`.

### `WsErrorCode`

`web:src/protocol/wsApi.ts:110`

Tipo: `\| "invalid_request" \| "unauthorized" \| "forbidden" \| "unsupported_by_f4" \| "busy" \| "timeout" \| "uart_error" \| "f4_nack" \| "offline" \| "connection_lost"`.

### `EspHello`

`web:src/protocol/wsApi.ts:122`

| Atributo / firma | Propósito |
| --- | --- |
| `apiVersion: number` | Dato de estado/configuración `apiVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `espVersion?: string` | Dato de estado/configuración `espVersion`; declaración estructural, interpretación específica pendiente de ampliar. |
| `f4ContractCutoff?: string` | Dato de estado/configuración `f4ContractCutoff`; declaración estructural, interpretación específica pendiente de ampliar. |
| `clientId?: number` | Dato de estado/configuración `clientId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `clientIp?: string` | Dato de estado/configuración `clientIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sessionGeneration?: number` | Dato de estado/configuración `sessionGeneration`; declaración estructural, interpretación específica pendiente de ampliar. |
| `resumeToken?: string` | Dato de estado/configuración `resumeToken`; declaración estructural, interpretación específica pendiente de ampliar. |
| `assignedNode?: string` | Dato de estado/configuración `assignedNode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `maxClients?: number` | Dato de estado/configuración `maxClients`; declaración estructural, interpretación específica pendiente de ampliar. |
| `features?: Record<string, boolean>` | Dato de estado/configuración `features`; declaración estructural, interpretación específica pendiente de ampliar. |
| `backend?: Record<string, unknown>` | Dato de estado/configuración `backend`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sessionResumed?: boolean` | Dato de estado/configuración `sessionResumed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `authenticated?: boolean` | Dato de estado/configuración `authenticated`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-websocketprovider"></a>`WebSocketProvider({ url, children }: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `url.startsWith`, `useCallback`, `useEffect` para API JSON, Promises y WebSocket. | `web:src/contexts/WebSocketContext.tsx:86`; structural |
| <a id="fn-buildwsrequest"></a>`buildWsRequest(command: CommandName, args: Record<string, unknown>, requestId: inferred-by-TypeScript): WsRequest` | Construye sobre API 1 con command, args y requestId válido. | `web:src/protocol/wsApi.ts:146`; editorial |
| <a id="fn-createrequestid"></a>`createRequestId(prefix: inferred-by-TypeScript): string` | Coordina `Date.now`, `Math.random`, `Math.random().toString` para API JSON, Promises y WebSocket. | `web:src/protocol/wsApi.ts:138`; structural |
| <a id="fn-decodewsdatapacket"></a>`decodeWsDataPacket(message: Record<string, unknown>): Uint8Array \| null` | Coordina `Array.isArray`, `String`, `Uint8Array.from` para API JSON, Promises y WebSocket. | `web:src/contexts/WebSocketContext.tsx:460`; structural |
| <a id="fn-isrecord"></a>`isRecord(value: unknown): value is Record<string, unknown>` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/protocol/wsApi.ts:204`; structural |
| <a id="fn-parsewsenvelope"></a>`parseWsEnvelope(value: unknown): WsEnvelope \| null` | Acepta únicamente los sobres reconocidos de respuesta, evento o error. | `web:src/protocol/wsApi.ts:161`; editorial |
| <a id="fn-resolvewebsocketurl"></a>`resolveWebSocketUrl(configuredUrl: string \| undefined, locationLike: Pick<Location, "protocol" \| "host">): string` | Adapta la llamada a `configuredUrl?.trim` con el contexto de esta integración. | `web:src/protocol/wsApi.ts:191`; structural |
| <a id="fn-safeparsejson"></a>`safeParseJson(text: string): unknown` | Adapta la llamada a `JSON.parse` con el contexto de esta integración. | `web:src/protocol/espClient.ts:152`; structural |
| <a id="fn-toespapierror"></a>`toEspApiError(error: WsError): EspApiError` | Operación to Esp Api Error de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. | `web:src/protocol/espClient.ts:160`; inferred-name |
| <a id="fn-useespcommand"></a>`useEspCommand(command: CommandName): inferred-by-TypeScript` | Coordina `useCallback`, `useState`, `useWebSocket` para API JSON, Promises y WebSocket. | `web:src/hooks/useEspCommand.ts:5`; structural |
| <a id="fn-usewebsocket"></a>`useWebSocket(): inferred-by-TypeScript` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/hooks/useWebSocket.tsx:4`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-espclient.accept"></a>`EspClient.accept(value: unknown): WsEnvelope \| null` | Despacha evento o completa el pending correlacionado por requestId/command. | `web:src/protocol/espClient.ts:86`; editorial |
| <a id="fn-espclient.cancelpending"></a>`EspClient.cancelPending(code: inferred-by-TypeScript, message: inferred-by-TypeScript): void` | Rechaza pendientes y limpia sus timers al cancelar la conexión. | `web:src/protocol/espClient.ts:139`; editorial |
| <a id="fn-espclient.pendingcount"></a>`EspClient.pendingCount(): number` | Operación pending Count de API JSON, Promises y WebSocket; semántica fina pendiente de ampliación. | `web:src/protocol/espClient.ts:147`; inferred-name |
| <a id="fn-espclient.request"></a>`EspClient.request(command: CommandName, args: Record<string, unknown>, options: { requestId?: string; timeoutMs?: number }): Promise<T>` | Registra Promise correlacionada, envía JSON y establece timeout. | `web:src/protocol/espClient.ts:47`; editorial |
| <a id="fn-espclient.setsender"></a>`EspClient.setSender(sender: ((text: string) => void) \| null): void` | Adapta la llamada a `this.cancelPending` con el contexto de esta integración. | `web:src/protocol/espClient.ts:40`; structural |
| <a id="fn-espclient.subscribe"></a>`EspClient.subscribe(event: string, handler: EventHandler): () => void` | Registra listener y devuelve una función para retirarlo. | `web:src/protocol/espClient.ts:127`; editorial |
| <a id="fn-websocketprovider.afterload"></a>`WebSocketProvider.afterLoad(): inferred-by-TypeScript` | Adapta la llamada a `window.requestAnimationFrame` con el contexto de esta integración. | `web:src/contexts/WebSocketContext.tsx:300`; structural |
| <a id="fn-websocketprovider.announce"></a>`WebSocketProvider.announce(): inferred-by-TypeScript` | Coordina `clientRef.current.request`, `clientRef.current.request(ESP_COMMANDS.PAGE_READY, {}, { timeoutMs: 3_000 }).catch` para API JSON, Promises y WebSocket. | `web:src/contexts/WebSocketContext.tsx:293`; structural |
| <a id="fn-websocketprovider.onpagehide"></a>`WebSocketProvider.onPageHide(): inferred-by-TypeScript` | Coordina `clearTimer`, `wsRef.current?.close` para API JSON, Promises y WebSocket. | `web:src/contexts/WebSocketContext.tsx:262`; structural |
| <a id="fn-websocketprovider.onpageshow"></a>`WebSocketProvider.onPageShow(event: PageTransitionEvent): inferred-by-TypeScript` | Adapta la llamada a `connectRef.current` con el contexto de esta integración. | `web:src/contexts/WebSocketContext.tsx:267`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:88` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:89` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:90` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:91` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:92` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:93` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:94` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:95` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:96` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:97` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:98` |
| `useState` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:100` |
| `useState` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:101` |
| `useState` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:102` |
| `useState` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:103` |
| `useState` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:104` |
| `useState` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:105` |
| `useState` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:106` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:124` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:129` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:133` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:182` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:198` |
| `useEffect` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:258` |
| `useEffect` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:288` |
| `useRef` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:310` |
| `useEffect` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:311` |
| `useEffect` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:334` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:351` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:356` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:358` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:371` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:381` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:387` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:392` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:404` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:413` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:414` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:415` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:416` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:417` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:422` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:423` |
| `useCallback` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:427` |
| `useMemo` | `WebSocketProvider` | `web:src/contexts/WebSocketContext.tsx:429` |
| `useWebSocket` | `useEspCommand` | `web:src/hooks/useEspCommand.ts:6` |
| `useState` | `useEspCommand` | `web:src/hooks/useEspCommand.ts:7` |
| `useState` | `useEspCommand` | `web:src/hooks/useEspCommand.ts:8` |
| `useCallback` | `useEspCommand` | `web:src/hooks/useEspCommand.ts:10` |
| `useContext` | `useWebSocket` | `web:src/hooks/useWebSocket.tsx:5` |
