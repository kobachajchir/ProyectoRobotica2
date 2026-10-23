# Contexto UNER y superficies históricas

ID: `web:module/src/contexts/UNERProtocolContext.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Conserva integración histórica de eventos/bytes UNER en la UI.

No sustituye automáticamente EspClient/JSON API 1; documentar cuál camino usa cada consumidor.

<a id="modularity"></a>
## Modularidad observada

Adaptación React a un contrato histórico, dependiente de transporte y consumidores.

<a id="state"></a>
## Handle, estado y atributos externos

Provider mantiene sus datos/listeners; sus valores pueden duplicar un modelo nuevo si se monta en paralelo.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Registrar/retirar callbacks al montar y aplicar solo mensajes del contrato esperado.

<a id="interactions"></a>
## Interacciones

- Codecs UNER y WebSocketContext exponen caminos diferentes; los nombres parecidos no prueban equivalencia de formatos.

<a id="limitations"></a>
## Precauciones y límites

- No documentar como camino activo una exportación sin caller localizado.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/contexts/UNERProtocolContext.tsx` | typescript-ast-static | `fd571d1ff96ea72d4a07aa12d6830a8fc67181692c5b328c380b92df5534344f` |
| `src/hooks/useUnerProtocol.tsx` | typescript-ast-static | `0d9dc277be91d865f090bc1e05e40ee0f39a0eb85635a494c046782a479d8a01` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `UNERTransport`

`web:src/contexts/UNERProtocolContext.tsx:18`

| Atributo / firma | Propósito |
| --- | --- |
| `write: (data: Uint8Array) => Promise<void> \| void` | Operación write de Contexto UNER y superficies históricas; semántica fina pendiente de ampliación. |
| `start?: () => Promise<() => void> \| (() => void) \| void` | Operación start de Contexto UNER y superficies históricas; semántica fina pendiente de ampliación. |
| `onBytes: (cb: (chunk: Uint8Array) => void) => (() => void) \| void` | Operación on Bytes de Contexto UNER y superficies históricas; semántica fina pendiente de ampliación. |
| `isConnected?: () => boolean` | Operación is Connected de Contexto UNER y superficies históricas; semántica fina pendiente de ampliación. |

### `UNERCmdHandler`

`web:src/contexts/UNERProtocolContext.tsx:25`

Tipo: `(packet: UNERPacket) => void`.

### `UNERContextValue`

`web:src/contexts/UNERProtocolContext.tsx:27`

| Atributo / firma | Propósito |
| --- | --- |
| `send: (cmd: number, payload?: Uint8Array) => Promise<void>` | Operación send de Contexto UNER y superficies históricas; semántica fina pendiente de ampliación. |
| `subscribe: (cmd: number, handler: UNERCmdHandler) => () => void` | Operación subscribe de Contexto UNER y superficies históricas; semántica fina pendiente de ampliación. |
| `lastPacket?: UNERPacket` | Dato de estado/configuración `lastPacket`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stats: { packetsOK: number; errorsChecksum: number; errorsLength: number; errorsHeader: number; errorsToken: number; errorsOverflow: number; }` | Dato de estado/configuración `stats`; declaración estructural, interpretación específica pendiente de ampliar. |
| `connected: boolean` | Dato de estado/configuración `connected`; declaración estructural, interpretación específica pendiente de ampliar. |

### `UNERProtocolProviderProps`

`web:src/contexts/UNERProtocolContext.tsx:42`

| Atributo / firma | Propósito |
| --- | --- |
| `transport?: UNERTransport` | Dato de estado/configuración `transport`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ringCapacity?: number` | Dato de estado/configuración `ringCapacity`; declaración estructural, interpretación específica pendiente de ampliar. |
| `children: ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-unerprotocolprovider"></a>`UNERProtocolProvider({   transport: transportProp,   ringCapacity = 256,   children, }: UNERProtocolProviderProps): inferred-by-TypeScript` | Coordina `transport.isConnected`, `useAutoWsTransport`, `useCallback` para Contexto UNER y superficies históricas. | `web:src/contexts/UNERProtocolContext.tsx:78`; structural |
| <a id="fn-useautowstransport"></a>`useAutoWsTransport(): UNERTransport` | Coordina `useMemo`, `useWebSocket` para Contexto UNER y superficies históricas. | `web:src/contexts/UNERProtocolContext.tsx:53`; structural |
| <a id="fn-useunerprotocol"></a>`useUNERProtocol(): UNERContextValue` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/hooks/useUnerProtocol.tsx:4`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-unerprotocolprovider.feed"></a>`UNERProtocolProvider.feed(chunk: Uint8Array): inferred-by-TypeScript` | Coordina `console.warn`, `uner.parse`, `uner.pushByte` para Contexto UNER y superficies históricas. | `web:src/contexts/UNERProtocolContext.tsx:117`; structural |
| <a id="fn-useautowstransport.isconnected"></a>`useAutoWsTransport.isConnected(): inferred-by-TypeScript` | Operación is Connected de Contexto UNER y superficies históricas; semántica fina pendiente de ampliación. | `web:src/contexts/UNERProtocolContext.tsx:71`; inferred-name |
| <a id="fn-useautowstransport.onbytes"></a>`useAutoWsTransport.onBytes(cb: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `ws.subscribeRaw` con el contexto de esta integración. | `web:src/contexts/UNERProtocolContext.tsx:61`; structural |
| <a id="fn-useautowstransport.start"></a>`useAutoWsTransport.start(): inferred-by-TypeScript` | Operación start de Contexto UNER y superficies históricas; semántica fina pendiente de ampliación. | `web:src/contexts/UNERProtocolContext.tsx:70`; inferred-name |
| <a id="fn-useautowstransport.write"></a>`useAutoWsTransport.write(data: Uint8Array): inferred-by-TypeScript` | Adapta la llamada a `ws.sendRaw` con el contexto de esta integración. | `web:src/contexts/UNERProtocolContext.tsx:58`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useWebSocket` | `useAutoWsTransport` | `web:src/contexts/UNERProtocolContext.tsx:54` |
| `useMemo` | `useAutoWsTransport` | `web:src/contexts/UNERProtocolContext.tsx:56` |
| `useAutoWsTransport` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:83` |
| `useState` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:86` |
| `useState` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:87` |
| `useRef` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:91` |
| `useCallback` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:93` |
| `useMemo` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:107` |
| `useEffect` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:113` |
| `useCallback` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:163` |
| `useCallback` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:172` |
| `useMemo` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:187` |
| `useMemo` | `UNERProtocolProvider` | `web:src/contexts/UNERProtocolContext.tsx:199` |
| `useContext` | `useUNERProtocol` | `web:src/hooks/useUnerProtocol.tsx:5` |
