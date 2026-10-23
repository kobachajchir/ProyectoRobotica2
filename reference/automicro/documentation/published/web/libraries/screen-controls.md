# Visor de pantalla y controles remotos

ID: `web:module/src/components/ScreenStreamWorkspace.tsx` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Presenta pantalla, acciones de menú/encoder/USER, validación y acceso al canvas guardado.

UI y envío de acciones lógicas con screenCode; no genera la rotación física del encoder.

<a id="modularity"></a>
## Modularidad observada

Integración React entre render, socket y permisos; no es la biblioteca OLED portable.

<a id="state"></a>
## Handle, estado y atributos externos

Refs conservan pantalla/acciones vigentes; pressed/held y timers viven localmente. currentScreen y autorización pertenecen a contextos.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Enviar comando solo con identidad vigente y permiso.
- Teclado captura dentro del visor; decide pulsación al keyup y cancela held al perder foco/visibilidad.
- Después de acciones solicita resincronización; status de aceptación no demuestra cambio de vista.

<a id="interactions"></a>
## Interacciones

- 0x53 click visible 1..3, 0x54/55 pulsación, 0x56 página, 0x57/58 giro lógico.
- F4 contrasta screenCode y aplica prioridad global del router; un código viejo debe resincronizarse.

<a id="limitations"></a>
## Precauciones y límites

- Umbral local de pulsación larga 1500 ms; F4 físico clasifica con otros contadores/umbrales.
- Sin ventana real de F4, una fila dibujada puede diferir de la fila ejecutada por click.
- Un formulario PIN local no debe dibujarse como captura del buffer físico.

## Hallazgos conectados

- [AMDOC-WEB-002: El visor rellena estados que el reporte de pantalla no transporta](../../../review/FINDINGS.md#amdoc-web-002)
- [AMDOC-WEB-004: La ventana del menú reconstruida en Web puede ejecutar otra fila en F4](../../../review/FINDINGS.md#amdoc-web-004)
- [AMDOC-WEB-006: Los builders de sensores no reciben el mismo modelo que la telemetría actual](../../../review/FINDINGS.md#amdoc-web-006)
- [AMDOC-WEB-007: El umbral de pulsación larga del visor difiere del botón físico](../../../review/FINDINGS.md#amdoc-web-007)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/ScreenDashboardPanel.tsx` | typescript-ast-static | `8c46cd9c118328aade5fb96e2ff8e7f1571702e4bed87a487f0ec08430fbd1b9` |
| `src/components/ScreenStreamModal.tsx` | typescript-ast-static | `4a29cbf2fa48454ca950ed3f47f091619367cf8e5dc442a09c515c8c708336fe` |
| `src/components/ScreenStreamWorkspace.tsx` | typescript-ast-static | `1dda55208edda4524b95ca3528a53eb235cbaf6fe412ff9fe9199484fa26318e` |
| `src/contexts/ScreenStreamModalContext.tsx` | typescript-ast-static | `838c1ddbf54090558becad38b464b60f9eee6ca8b123bf3bd54938958aa64b3a` |
| `src/hooks/useScreenStreamKeyboard.ts` | typescript-ast-static | `72f8f9b2acc35082c3a371aeeef15f6f30dc6bad35face796086392194cb0c4d` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `ScreenDashboardPanelProps`

`web:src/components/ScreenDashboardPanel.tsx:5`

| Atributo / firma | Propósito |
| --- | --- |
| `compact?: boolean` | Dato de estado/configuración `compact`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ScreenStreamModalProps`

`web:src/components/ScreenStreamModal.tsx:7`

| Atributo / firma | Propósito |
| --- | --- |
| `isOpen: boolean` | Dato de estado/configuración `isOpen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onClose: () => void` | Pide al dueño cerrar el modal y completar limpieza del montaje; no simula pulsación USER en F4. |

### `ScreenPageDirection`

`web:src/components/ScreenStreamWorkspace.tsx:46`

Tipo: `"up" \| "down"`.

### `AuxButtonKind`

`web:src/components/ScreenStreamWorkspace.tsx:47`

Tipo: `"encoder" \| "user"`.

### `KeyboardCommandDebug`

`web:src/components/ScreenStreamWorkspace.tsx:49`

| Atributo / firma | Propósito |
| --- | --- |
| `key: string` | Dato de estado/configuración `key`; declaración estructural, interpretación específica pendiente de ampliar. |
| `heldMs?: number` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

### `ScreenStreamModalContextValue`

`web:src/contexts/ScreenStreamModalContext.tsx:5`

| Atributo / firma | Propósito |
| --- | --- |
| `isOpen: boolean` | Dato de estado/configuración `isOpen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `open: () => void` | Abre visor en el estado de UI del provider. |
| `close: () => void` | Cierra visor y deja al montaje retirar foco/listeners/gestos. |
| `toggle: () => void` | Alterna apertura local del visor; no cambia pantalla F4 por implicación. |

### `RemoteButtonKind`

`web:src/hooks/useScreenStreamKeyboard.ts:3`

Tipo: `"encoder" \| "user"`.

### `ScreenStreamKeyboardActions`

`web:src/hooks/useScreenStreamKeyboard.ts:5`

| Atributo / firma | Propósito |
| --- | --- |
| `active: boolean` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onRotate: (direction: "left" \| "right") => void` | El hook emite intención left/right desde teclas del visor; el consumidor la traduce al comando y screenCode vigente. |
| `onRefresh: () => void` | Solicita consulta de estado actual desde el ámbito de teclado del visor. |
| `onPressStart: (kind: RemoteButtonKind) => void` | Inicia seguimiento local de gesto USER/encoder y su reloj; no confirma aún el evento remoto. |
| `onPressRelease: (kind: RemoteButtonKind) => void` | Termina gesto, calcula tipo lógico y permite envío del comando. |
| `onPressCancel: (kind: RemoteButtonKind) => void` | Descarta gesto pendiente al perder foco/visibilidad, sin fabricar liberación con acción. |

### `ScreenStreamKeyboardOptions`

`web:src/hooks/useScreenStreamKeyboard.ts:14`

| Atributo / firma | Propósito |
| --- | --- |
| `enabled: boolean` | Dato de estado/configuración `enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `scopeRef: RefObject<HTMLDivElement \| null>` | Dato de estado/configuración `scopeRef`; declaración estructural, interpretación específica pendiente de ampliar. |
| `actionsRef: RefObject<ScreenStreamKeyboardActions \| null>` | Dato de estado/configuración `actionsRef`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-controlblock"></a>`ControlBlock({  title,  children, }: { title: string; children: React.ReactNode; }): inferred-by-TypeScript` | Operación Control Block de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:1594`; inferred-name |
| <a id="fn-labeledlivecontrol"></a>`LabeledLiveControl({  title,  children, }: { title: string; children: React.ReactNode; }): inferred-by-TypeScript` | Operación Labeled Live Control de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:1611`; inferred-name |
| <a id="fn-monobitmapicon"></a>`MonoBitmapIcon({  bitmap,  width,  height,  scale = 2,  color, }: { bitmap: Uint8Array; width: number; height: number; scale?: number; color: string; }): inferred-by-TypeScript` | Coordina `useEffect`, `useRef` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:1626`; structural |
| <a id="fn-pillwithtooltip"></a>`PillWithTooltip({  label,  tooltip,  tone, }: { label: string; tooltip: string; tone: "cyan" \| "emerald"; }): inferred-by-TypeScript` | Operación Pill With Tooltip de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:1715`; inferred-name |
| <a id="fn-screendashboardpanel"></a>`ScreenDashboardPanel({  compact = false, }: ScreenDashboardPanelProps): inferred-by-TypeScript` | Coordina `useEffect`, `useState` para Visor de pantalla y controles remotos. | `web:src/components/ScreenDashboardPanel.tsx:9`; structural |
| <a id="fn-screenfact"></a>`ScreenFact({  label,  value,  wide = false,  styles = "", }: { label: string; value: string; wide?: boolean; styles?: string; }): inferred-by-TypeScript` | Operación Screen Fact de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:1744`; inferred-name |
| <a id="fn-screenstreammodal"></a>`ScreenStreamModal({  isOpen,  onClose, }: ScreenStreamModalProps): inferred-by-TypeScript` | Coordina `useRef`, `useScreenStreamKeyboard` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamModal.tsx:12`; structural |
| <a id="fn-screenstreammodalprovider"></a>`ScreenStreamModalProvider({   children, }: { children: React.ReactNode; }): inferred-by-TypeScript` | Coordina `useMemo`, `useState` para Visor de pantalla y controles remotos. | `web:src/contexts/ScreenStreamModalContext.tsx:15`; structural |
| <a id="fn-screenstreamworkspace"></a>`ScreenStreamWorkspace({  isModal = false,  keyboardActionsRef, }: { isModal?: boolean; keyboardActionsRef?: React.RefObject<ScreenStreamKeyboardActions \| null>; }): inferred-by-TypeScript` | Compone preview, estado, acciones y validación sobre la pantalla remota. | `web:src/components/ScreenStreamWorkspace.tsx:109`; editorial |
| <a id="fn-turnarrowicon"></a>`TurnArrowIcon({ direction }: { direction: "left" \| "right" }): inferred-by-TypeScript` | Operación Turn Arrow Icon de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:1679`; inferred-name |
| <a id="fn-buildauxbuttonpayload"></a>`buildAuxButtonPayload(screenCode: number, pressKind: number): number[]` | Adapta la llamada a `screenCodeToLeBytes` con el contexto de esta integración. | `web:src/components/ScreenStreamWorkspace.tsx:92`; structural |
| <a id="fn-buildencoderrotatepayload"></a>`buildEncoderRotatePayload(screenCode: number): number[]` | Codifica screenCode LE para giro lógico remoto. | `web:src/components/ScreenStreamWorkspace.tsx:99`; editorial |
| <a id="fn-buildmenuitemclickpayload"></a>`buildMenuItemClickPayload(screenCode: number, item: 1 \| 2 \| 3): number[]` | Codifica screenCode LE y posición visible 1-based. | `web:src/components/ScreenStreamWorkspace.tsx:85`; editorial |
| <a id="fn-buildscreenpagepayload"></a>`buildScreenPagePayload(screenCode: number, direction: ScreenPageDirection): number[]` | Adapta la llamada a `screenCodeToLeBytes` con el contexto de esta integración. | `web:src/components/ScreenStreamWorkspace.tsx:75`; structural |
| <a id="fn-buttonforkey"></a>`buttonForKey(key: string): RemoteButtonKind \| null` | Operación button For Key de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/hooks/useScreenStreamKeyboard.ts:20`; inferred-name |
| <a id="fn-createcommandrequestid"></a>`createCommandRequestId(): string` | Coordina `Date.now`, `Math.random`, `Math.random().toString` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:103`; structural |
| <a id="fn-directionforkey"></a>`directionForKey(key: string): "left" \| "right" \| null` | Operación direction For Key de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/hooks/useScreenStreamKeyboard.ts:26`; inferred-name |
| <a id="fn-formatpayload"></a>`formatPayload(payload: number[] \| undefined): inferred-by-TypeScript` | Coordina `payload .map`, `payload .map((byte) => byte.toString(16).toUpperCase().padStart(2, "0")) .join` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:1765`; structural |
| <a id="fn-formatremainingseconds"></a>`formatRemainingSeconds(remainingMs: number \| null): string` | Coordina `Math.max`, `Math.max(0, remainingMs / 1000).toFixed` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:1777`; structural |
| <a id="fn-ismenucontrolscreencode"></a>`isMenuControlScreenCode(screenCode: number \| undefined): boolean` | Operación is Menu Control Screen Code de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:1579`; inferred-name |
| <a id="fn-isstmcommandoptions"></a>`isStmCommandOptions(value: unknown): value is { payload?: number[]; jsonCommand?: string; jsonParams?: Record<string, unknown>; }` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/components/ScreenStreamWorkspace.tsx:1569`; structural |
| <a id="fn-normalizestmcommandoptions"></a>`normalizeStmCommandOptions(payloadOrOptionsOrDescription: \| number[] \| { payload?: number[]; jsonCommand?: string; jsonParams?: Record<string, unknown>; } \| string, descriptionOrOptions?: \| string \| { payload?: number[]; jsonCommand?: string; jsonParams?: Record<string, unknown>; }): inferred-by-TypeScript` | Coordina `Array.isArray`, `isStmCommandOptions` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:1534`; structural |
| <a id="fn-readfinitenumber"></a>`readFiniteNumber(value: unknown): number \| null` | Adapta la llamada a `Number.isFinite` con el contexto de esta integración. | `web:src/components/ScreenStreamWorkspace.tsx:1773`; structural |
| <a id="fn-screencodetolebytes"></a>`screenCodeToLeBytes(screenCode: number): number[]` | Operación screen Code To Le Bytes de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:65`; inferred-name |
| <a id="fn-usescreenstreamkeyboard"></a>`useScreenStreamKeyboard({   enabled,   scopeRef,   actionsRef, }: ScreenStreamKeyboardOptions): inferred-by-TypeScript` | Registra teclado limitado al visor y libera pressed/held/listeners al salir. | `web:src/hooks/useScreenStreamKeyboard.ts:32`; editorial |
| <a id="fn-usescreenstreammodal"></a>`useScreenStreamModal(): inferred-by-TypeScript` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/contexts/ScreenStreamModalContext.tsx:40`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-screenstreammodalprovider.close"></a>`ScreenStreamModalProvider.close(): inferred-by-TypeScript` | Adapta la llamada a `setIsOpen` con el contexto de esta integración. | `web:src/contexts/ScreenStreamModalContext.tsx:26`; structural |
| <a id="fn-screenstreammodalprovider.open"></a>`ScreenStreamModalProvider.open(): inferred-by-TypeScript` | Adapta la llamada a `setIsOpen` con el contexto de esta integración. | `web:src/contexts/ScreenStreamModalContext.tsx:25`; structural |
| <a id="fn-screenstreammodalprovider.toggle"></a>`ScreenStreamModalProvider.toggle(): inferred-by-TypeScript` | Adapta la llamada a `setIsOpen` con el contexto de esta integración. | `web:src/contexts/ScreenStreamModalContext.tsx:27`; structural |
| <a id="fn-screenstreamworkspace.actionbuttonstyle"></a>`ScreenStreamWorkspace.actionButtonStyle(hovered: boolean): React.CSSProperties` | Operación action Button Style de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:707`; inferred-name |
| <a id="fn-screenstreamworkspace.animatemenuitempress"></a>`ScreenStreamWorkspace.animateMenuItemPress(item: number): inferred-by-TypeScript` | Coordina `setPressedMenuItem`, `window.setTimeout` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:471`; structural |
| <a id="fn-screenstreamworkspace.animaterotatepress"></a>`ScreenStreamWorkspace.animateRotatePress(direction: "left" \| "right"): inferred-by-TypeScript` | Coordina `setPressedRotate`, `window.setTimeout` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:485`; structural |
| <a id="fn-screenstreamworkspace.animatescrollpress"></a>`ScreenStreamWorkspace.animateScrollPress(direction: ScreenPageDirection): inferred-by-TypeScript` | Coordina `setPressedScrollDir`, `window.setTimeout` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:478`; structural |
| <a id="fn-screenstreamworkspace.cancelauxpress"></a>`ScreenStreamWorkspace.cancelAuxPress(kind: AuxButtonKind): inferred-by-TypeScript` | Coordina `setLongPressActive`, `setPressedAuxButtons`, `window.clearTimeout` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:567`; structural |
| <a id="fn-screenstreamworkspace.dispatchstmcommand"></a>`ScreenStreamWorkspace.dispatchStmCommand(legacyCommand: string, payloadOrOptionsOrDescription: \| number[] \| { payload?: number[]; jsonCommand?: string; jsonParams?: Record<string, unknown>; } \| string, descriptionOrOptions?: \| string \| { payload?: number[]; jsonCommand?: string; jsonParams?: Record<string, unknown>; }, inputSource: "pointer" \| "keyboard"): inferred-by-TypeScript` | Coordina `Boolean`, `console.log`, `console.warn` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:343`; structural |
| <a id="fn-screenstreamworkspace.getaccentbuttonstyle"></a>`ScreenStreamWorkspace.getAccentButtonStyle(hovered: boolean, pressed: inferred-by-TypeScript, longPressed: inferred-by-TypeScript): React.CSSProperties` | Operación get Accent Button Style de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/components/ScreenStreamWorkspace.tsx:431`; inferred-name |
| <a id="fn-screenstreamworkspace.handleencoderrotate"></a>`ScreenStreamWorkspace.handleEncoderRotate(direction: "left" \| "right", inputSource: "pointer" \| "keyboard"): inferred-by-TypeScript` | Coordina `animateRotatePress`, `buildEncoderRotatePayload`, `console.warn` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:624`; structural |
| <a id="fn-screenstreamworkspace.handlemanualscreensync"></a>`ScreenStreamWorkspace.handleManualScreenSync(inputSource: "pointer" \| "keyboard"): inferred-by-TypeScript` | Coordina `console.log`, `requestCarMode`, `requestCurrentScreen` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:752`; structural |
| <a id="fn-screenstreamworkspace.handlemenuitemclick"></a>`ScreenStreamWorkspace.handleMenuItemClick(item: 1 \| 2 \| 3): inferred-by-TypeScript` | Coordina `animateMenuItemPress`, `buildMenuItemClickPayload`, `console.warn` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:492`; structural |
| <a id="fn-screenstreamworkspace.handlemenuscroll"></a>`ScreenStreamWorkspace.handleMenuScroll(direction: ScreenPageDirection): inferred-by-TypeScript` | Coordina `animateScrollPress`, `buildScreenPagePayload`, `console.warn` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:518`; structural |
| <a id="fn-screenstreamworkspace.handlevalidatepin"></a>`ScreenStreamWorkspace.handleValidatePin(pin: string): inferred-by-TypeScript` | Coordina `scheduleScreenRefresh`, `validatePin` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:665`; structural |
| <a id="fn-screenstreamworkspace.logkeyboardcommand"></a>`ScreenStreamWorkspace.logKeyboardCommand(legacyCommand: string, jsonCommand: string, jsonParams: Record<string, unknown>, payload: number[] \| null, keyboardDebug: KeyboardCommandDebug): inferred-by-TypeScript` | Coordina `console.log`, `payload?.map`, `payload?.map((byte) => byte.toString(16).toUpperCase().padStart(2, "0")).join` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:314`; structural |
| <a id="fn-screenstreamworkspace.onpressrelease"></a>`ScreenStreamWorkspace.onPressRelease(kind: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `releaseAuxPress` con el contexto de esta integración. | `web:src/components/ScreenStreamWorkspace.tsx:794`; structural |
| <a id="fn-screenstreamworkspace.onrefresh"></a>`ScreenStreamWorkspace.onRefresh(): inferred-by-TypeScript` | Adapta la llamada a `handleManualScreenSync` con el contexto de esta integración. | `web:src/components/ScreenStreamWorkspace.tsx:792`; structural |
| <a id="fn-screenstreamworkspace.onrotate"></a>`ScreenStreamWorkspace.onRotate(direction: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `handleEncoderRotate` con el contexto de esta integración. | `web:src/components/ScreenStreamWorkspace.tsx:791`; structural |
| <a id="fn-screenstreamworkspace.releaseauxpress"></a>`ScreenStreamWorkspace.releaseAuxPress(kind: AuxButtonKind, inputSource: "pointer" \| "keyboard"): inferred-by-TypeScript` | Coordina `Date.now`, `buildAuxButtonPayload`, `cancelAuxPress` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:578`; structural |
| <a id="fn-screenstreamworkspace.schedulescreenrefresh"></a>`ScreenStreamWorkspace.scheduleScreenRefresh(): inferred-by-TypeScript` | Coordina `window.clearTimeout`, `window.setTimeout` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:303`; structural |
| <a id="fn-screenstreamworkspace.sendsavedscreen"></a>`ScreenStreamWorkspace.sendSavedScreen(screenId: string): inferred-by-TypeScript` | Coordina `oledCanvasTransfer.send`, `rasterizeOledDocument`, `savedScreens.find` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:719`; structural |
| <a id="fn-screenstreamworkspace.startauxpress"></a>`ScreenStreamWorkspace.startAuxPress(kind: AuxButtonKind): inferred-by-TypeScript` | Coordina `Date.now`, `setLongPressActive`, `setPressedAuxButtons` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:549`; structural |
| <a id="fn-screenstreamworkspace.updateheight"></a>`ScreenStreamWorkspace.updateHeight(): inferred-by-TypeScript` | Coordina `setPreviewHeight`, `target.getBoundingClientRect` para Visor de pantalla y controles remotos. | `web:src/components/ScreenStreamWorkspace.tsx:249`; structural |
| <a id="fn-usescreenstreamkeyboard.cancelheldkeys"></a>`useScreenStreamKeyboard.cancelHeldKeys(): inferred-by-TypeScript` | Coordina `heldKeys.clear`, `heldKeys.forEach` para Visor de pantalla y controles remotos. | `web:src/hooks/useScreenStreamKeyboard.ts:43`; structural |
| <a id="fn-usescreenstreamkeyboard.capturekey"></a>`useScreenStreamKeyboard.captureKey(event: KeyboardEvent): inferred-by-TypeScript` | Coordina `event.preventDefault`, `event.stopImmediatePropagation`, `scope.contains` para Visor de pantalla y controles remotos. | `web:src/hooks/useScreenStreamKeyboard.ts:52`; structural |
| <a id="fn-usescreenstreamkeyboard.onfocusout"></a>`useScreenStreamKeyboard.onFocusOut(event: FocusEvent): inferred-by-TypeScript` | Coordina `cancelHeldKeys`, `scope.contains` para Visor de pantalla y controles remotos. | `web:src/hooks/useScreenStreamKeyboard.ts:98`; structural |
| <a id="fn-usescreenstreamkeyboard.onkeydown"></a>`useScreenStreamKeyboard.onKeyDown(event: KeyboardEvent): inferred-by-TypeScript` | Coordina `actionsRef.current.onPressStart`, `buttonForKey`, `captureKey` para Visor de pantalla y controles remotos. | `web:src/hooks/useScreenStreamKeyboard.ts:61`; structural |
| <a id="fn-usescreenstreamkeyboard.onkeyup"></a>`useScreenStreamKeyboard.onKeyUp(event: KeyboardEvent): inferred-by-TypeScript` | Coordina `actionsRef.current.onPressRelease`, `actionsRef.current.onRefresh`, `actionsRef.current.onRotate` para Visor de pantalla y controles remotos. | `web:src/hooks/useScreenStreamKeyboard.ts:74`; structural |
| <a id="fn-usescreenstreamkeyboard.onvisibilitychange"></a>`useScreenStreamKeyboard.onVisibilityChange(): inferred-by-TypeScript` | Adapta la llamada a `cancelHeldKeys` con el contexto de esta integración. | `web:src/hooks/useScreenStreamKeyboard.ts:95`; structural |
| <a id="fn-usescreenstreamkeyboard.shouldignoreaction"></a>`useScreenStreamKeyboard.shouldIgnoreAction(event: KeyboardEvent): inferred-by-TypeScript` | Operación should Ignore Action de Visor de pantalla y controles remotos; semántica fina pendiente de ampliación. | `web:src/hooks/useScreenStreamKeyboard.ts:50`; inferred-name |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useState` | `ScreenDashboardPanel` | `web:src/components/ScreenDashboardPanel.tsx:12` |
| `useState` | `ScreenDashboardPanel` | `web:src/components/ScreenDashboardPanel.tsx:13` |
| `useState` | `ScreenDashboardPanel` | `web:src/components/ScreenDashboardPanel.tsx:14` |
| `useEffect` | `ScreenDashboardPanel` | `web:src/components/ScreenDashboardPanel.tsx:18` |
| `useRef` | `ScreenStreamModal` | `web:src/components/ScreenStreamModal.tsx:16` |
| `useRef` | `ScreenStreamModal` | `web:src/components/ScreenStreamModal.tsx:17` |
| `useScreenStreamKeyboard` | `ScreenStreamModal` | `web:src/components/ScreenStreamModal.tsx:18` |
| `useUser` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:116` |
| `useIrEmitters` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:117` |
| `useWebSocket` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:118` |
| `useScreen` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:119` |
| `useCarMode` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:120` |
| `useSavedOledScreens` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:121` |
| `useOledCanvasTransfer` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:122` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:125` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:126` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:128` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:129` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:131` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:132` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:134` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:135` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:136` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:138` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:140` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:141` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:142` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:143` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:144` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:145` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:152` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:154` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:155` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:161` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:167` |
| `useRef` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:174` |
| `useRef` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:179` |
| `useRef` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:184` |
| `useRef` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:185` |
| `useRef` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:186` |
| `useRef` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:187` |
| `useRef` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:188` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:189` |
| `useMemo` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:191` |
| `useState` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:229` |
| `useEffect` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:241` |
| `useEffect` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:245` |
| `useEffect` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:269` |
| `useEffect` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:285` |
| `useLayoutEffect` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:787` |
| `useLayoutEffect` | `ScreenStreamWorkspace` | `web:src/components/ScreenStreamWorkspace.tsx:799` |
| `useRef` | `MonoBitmapIcon` | `web:src/components/ScreenStreamWorkspace.tsx:1639` |
| `useEffect` | `MonoBitmapIcon` | `web:src/components/ScreenStreamWorkspace.tsx:1641` |
| `useState` | `ScreenStreamModalProvider` | `web:src/contexts/ScreenStreamModalContext.tsx:20` |
| `useMemo` | `ScreenStreamModalProvider` | `web:src/contexts/ScreenStreamModalContext.tsx:22` |
| `useContext` | `useScreenStreamModal` | `web:src/contexts/ScreenStreamModalContext.tsx:41` |
| `useLayoutEffect` | `useScreenStreamKeyboard` | `web:src/hooks/useScreenStreamKeyboard.ts:37` |
