# Tipos y feedback de acciones remotas

ID: `web:module/src/types/RemoteBridgeTypes.ts` · Capa: **contrato** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Define nombres de acciones, modos de compatibilidad y traducción de status a feedback.

Payload/tipos, aceptación/rechazo y acciones de reinicio; efecto físico permanece en firmware.

<a id="modularity"></a>
## Modularidad observada

Contrato compartido representado en TypeScript con adaptación UI.

<a id="state"></a>
## Handle, estado y atributos externos

Estado pending/error pertenece al consumidor; las tablas de comandos/status son datos.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Elegir comando/args, esperar respuesta final y traducir status.
- Ante screen mismatch solicitar estado actual antes de repetir una acción.

<a id="interactions"></a>
## Interacciones

- ESP codifica nombres hacia UNER; F4 devuelve status tras su guarda/router.

<a id="limitations"></a>
## Precauciones y límites

- Status0 de evento consumido no demuestra que el handler produjo un cambio visible.
- No reconstruir acciones especiales solo desde la categoría visual del código.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/SystemResetActions.tsx` | typescript-ast-static | `c3be368edab7c6d11f0dbc9b3d8d8cae62843930d6e8cd649b31351be5067fc3` |
| `src/components/TransportActionButton.tsx` | typescript-ast-static | `8960c90d5ea3466fa6bea2ee88a12aa20f9c18b0eb4263b645a6f594c0dc7e7e` |
| `src/types/RemoteBridgeTypes.ts` | typescript-ast-static | `c373df9ef4a2ec6a4831672deb15654533d970b0bd960fc16a232d0b8a866020` |
| `src/utils/stmRemoteActionFeedback.ts` | typescript-ast-static | `c84330cc9d00856de6cbb0ddc0af14a90578ad7418d95c4d0a48aa8e9d556d49` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `ResetTarget`

`web:src/components/SystemResetActions.tsx:8`

Tipo: `"esp" \| "stm"`.

### `PendingAction`

`web:src/components/SystemResetActions.tsx:9`

Tipo: `ResetTarget \| "profile"`.

### `ExtensionProfileId`

`web:src/components/SystemResetActions.tsx:10`

Tipo: `0 \| 1`.

### `ExtensionProfileResponse`

`web:src/components/SystemResetActions.tsx:12`

| Atributo / firma | Propósito |
| --- | --- |
| `success?: boolean` | Dato de estado/configuración `success`; declaración estructural, interpretación específica pendiente de ampliar. |
| `profileId?: number` | Dato de estado/configuración `profileId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `profile?: string` | Dato de estado/configuración `profile`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rebootRequired?: boolean` | Dato de estado/configuración `rebootRequired`; declaración estructural, interpretación específica pendiente de ampliar. |
| `rebooting?: boolean` | Dato de estado/configuración `rebooting`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reconnectDelayMs?: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `ResetStatus`

`web:src/components/SystemResetActions.tsx:38`

| Atributo / firma | Propósito |
| --- | --- |
| `tone: "idle" \| "loading" \| "success" \| "error"` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `message: string` | Dato de estado/configuración `message`; declaración estructural, interpretación específica pendiente de ampliar. |
| `frame?: string` | Dato de estado/configuración `frame`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TransportActionTone`

`web:src/components/TransportActionButton.tsx:3`

Tipo: `"cyan" \| "emerald" \| "rose" \| "muted"`.

### `TransportActionButtonProps`

`web:src/components/TransportActionButton.tsx:5`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `detail: string` | Dato de estado/configuración `detail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `icon: ReactNode` | Dato de estado/configuración `icon`; declaración estructural, interpretación específica pendiente de ampliar. |
| `tone: TransportActionTone` | Dato de estado/configuración `tone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `active?: boolean` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pressed?: boolean` | Dato de estado/configuración `pressed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `disabled?: boolean` | Dato de estado/configuración `disabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onClick: () => void` | Emite intención de acción al dueño del botón cuando su estado permite activación; el feedback efectivo viene de la respuesta. |

### `RemoteAuthBridgeMode`

`web:src/types/RemoteBridgeTypes.ts:8`

Tipo: `(typeof REMOTE_AUTH_BRIDGE_MODE)[keyof typeof REMOTE_AUTH_BRIDGE_MODE]`.

### `StmRemoteCommandMode`

`web:src/types/RemoteBridgeTypes.ts:16`

Tipo: `(typeof STM_REMOTE_COMMAND_MODE)[keyof typeof STM_REMOTE_COMMAND_MODE]`.

### `EspRemoteAuthParams`

`web:src/types/RemoteBridgeTypes.ts:42`

| Atributo / firma | Propósito |
| --- | --- |
| `pin: string` | Dato de estado/configuración `pin`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StmRemoteGrantParams`

`web:src/types/RemoteBridgeTypes.ts:46`

| Atributo / firma | Propósito |
| --- | --- |
| `screenCode: number` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StmRemoteRotateParams`

`web:src/types/RemoteBridgeTypes.ts:50`

| Atributo / firma | Propósito |
| --- | --- |
| `screenCode: number` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StmRemoteButtonParams`

`web:src/types/RemoteBridgeTypes.ts:54`

| Atributo / firma | Propósito |
| --- | --- |
| `screenCode: number` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pressKind: "short" \| "long"` | Dato de estado/configuración `pressKind`; declaración estructural, interpretación específica pendiente de ampliar. |

### `StmRemoteActionFeedback`

`web:src/utils/stmRemoteActionFeedback.ts:14`

| Atributo / firma | Propósito |
| --- | --- |
| `message: string` | Dato de estado/configuración `message`; declaración estructural, interpretación específica pendiente de ampliar. |
| `status?: number` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `refreshScreen: boolean` | Dato de estado/configuración `refreshScreen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `reauthenticationRequired: boolean` | Dato de estado/configuración `reauthenticationRequired`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-resetstatusnote"></a>`ResetStatusNote({ status }: { status: ResetStatus }): inferred-by-TypeScript` | Restablece Status Note dentro de Tipos y feedback de acciones remotas. | `web:src/components/SystemResetActions.tsx:361`; inferred-name |
| <a id="fn-systemresetactions"></a>`SystemResetActions(): inferred-by-TypeScript` | Coordina `EXTENSION_PROFILES.map`, `profileLabel`, `useEffect` para Tipos y feedback de acciones remotas. | `web:src/components/SystemResetActions.tsx:44`; structural |
| <a id="fn-transportactionbutton"></a>`TransportActionButton({  label,  detail,  icon,  tone,  active = false,  pressed,  disabled = false,  className = "",  onClick, }: TransportActionButtonProps): inferred-by-TypeScript` | Operación Transport Action Button de Tipos y feedback de acciones remotas; semántica fina pendiente de ampliación. | `web:src/components/TransportActionButton.tsx:17`; inferred-name |
| <a id="fn-getremoteauthbridgemode"></a>`getRemoteAuthBridgeMode(): RemoteAuthBridgeMode` | Coordina `import.meta.env.VITE_REMOTE_AUTH_BRIDGE_MODE?.trim`, `import.meta.env.VITE_REMOTE_AUTH_BRIDGE_MODE?.trim().toLowerCase` para Tipos y feedback de acciones remotas. | `web:src/types/RemoteBridgeTypes.ts:59`; structural |
| <a id="fn-getstmremoteactionfeedback"></a>`getStmRemoteActionFeedback(cause: unknown, description: string): StmRemoteActionFeedback` | Adapta la llamada a `readF4Status` con el contexto de esta integración. | `web:src/utils/stmRemoteActionFeedback.ts:21`; structural |
| <a id="fn-getstmremotecommandmode"></a>`getStmRemoteCommandMode(): StmRemoteCommandMode` | Coordina `import.meta.env.VITE_STM_REMOTE_COMMAND_MODE?.trim`, `import.meta.env.VITE_STM_REMOTE_COMMAND_MODE?.trim().toLowerCase` para Tipos y feedback de acciones remotas. | `web:src/types/RemoteBridgeTypes.ts:66`; structural |
| <a id="fn-isextensionprofileid"></a>`isExtensionProfileId(value: unknown): value is ExtensionProfileId` | Operación is Extension Profile Id de Tipos y feedback de acciones remotas; semántica fina pendiente de ampliación. | `web:src/components/SystemResetActions.tsx:30`; inferred-name |
| <a id="fn-isrecord"></a>`isRecord(value: unknown): value is Record<string, unknown>` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/utils/stmRemoteActionFeedback.ts:119`; structural |
| <a id="fn-profilelabel"></a>`profileLabel(profileId: ExtensionProfileId \| null): inferred-by-TypeScript` | Adapta la llamada a `EXTENSION_PROFILES.find` con el contexto de esta integración. | `web:src/components/SystemResetActions.tsx:34`; structural |
| <a id="fn-readf4status"></a>`readF4Status(details: unknown): number \| undefined` | Coordina `Number.isInteger`, `isRecord` para Tipos y feedback de acciones remotas. | `web:src/utils/stmRemoteActionFeedback.ts:108`; structural |
| <a id="fn-resolveespremoteauthcommand"></a>`resolveEspRemoteAuthCommand(action: number): string \| null` | Operación resolve Esp Remote Auth Command de Tipos y feedback de acciones remotas; semántica fina pendiente de ampliación. | `web:src/types/RemoteBridgeTypes.ts:73`; inferred-name |
| <a id="fn-toremotepresskindlabel"></a>`toRemotePressKindLabel(pressKind: number): "short" \| "long"` | Operación to Remote Press Kind Label de Tipos y feedback de acciones remotas; semántica fina pendiente de ampliación. | `web:src/types/RemoteBridgeTypes.ts:85`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-systemresetactions.applyextensionprofile"></a>`SystemResetActions.applyExtensionProfile(): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Number.isFinite` para Tipos y feedback de acciones remotas. | `web:src/components/SystemResetActions.tsx:161`; structural |
| <a id="fn-systemresetactions.sendreset"></a>`SystemResetActions.sendReset(target: ResetTarget): inferred-by-TypeScript` | Coordina `logout`, `request`, `setPendingTarget` para Tipos y feedback de acciones remotas. | `web:src/components/SystemResetActions.tsx:97`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useWebSocket` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:45` |
| `useUser` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:46` |
| `useState` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:47` |
| `useState` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:48` |
| `useState` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:49` |
| `useState` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:50` |
| `useState` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:51` |
| `useState` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:52` |
| `useState` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:53` |
| `useEffect` | `SystemResetActions` | `web:src/components/SystemResetActions.tsx:58` |
