# Estado normalizado de pantalla remota

ID: `web:module/src/contexts/ScreenContext.tsx` · Capa: **servicio** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Normaliza reportes ESP y construye el modelo de pantalla, selección, menú y PIN que usa la UI.

Identidad y datos reportados/enriquecidos; no dispone de todos los estados privados de los renderers F4.

<a id="modularity"></a>
## Modularidad observada

Depende de transporte, usuario y modo; normalización pura separada de provider. El enriquecimiento mezcla datos confirmados y derivados y debe mantener su procedencia.

<a id="state"></a>
## Handle, estado y atributos externos

ScreenProvider posee currentScreen, requests pendientes y listeners. CurrentScreen conserva rawData, source y tiempo recibido; ScreenViewModel añade conteos/ventana derivados y clasificación de PIN.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Recibir screen.changed/current/device.response y normalizar.
- Enriquecer usando datos reportados y contexto global; preservar solo campos cuyo contexto sigue siendo válido.
- RequestCurrentScreen exige conexión y autoridad remota y lleva timeout propio.

<a id="interactions"></a>
## Interacciones

- ESP add_screen_report serializa identidad/selección/dashboard; renderer consume rawData enriquecido.
- La ventana Web se deriva en bloques de tres, frente al estado firstVisibleItem de F4.

<a id="limitations"></a>
## Precauciones y límites

- Código de página del screenCode no equivale a página actual de scroll.
- GOTO no se interpreta en todos los normalizadores de pantalla.
- Índices 0, cantidad 0 y campo ausente no son equivalentes.

## Hallazgos conectados

- [AMDOC-ESP-002: Los reportes sin selección publican selectedIndex/itemCount como cero](../../../review/FINDINGS.md#amdoc-esp-002)
- [AMDOC-WEB-002: El visor rellena estados que el reporte de pantalla no transporta](../../../review/FINDINGS.md#amdoc-web-002)
- [AMDOC-WEB-004: La ventana del menú reconstruida en Web puede ejecutar otra fila en F4](../../../review/FINDINGS.md#amdoc-web-004)
- [AMDOC-WEB-005: Normalizadores del visor pierden el modo GOTO](../../../review/FINDINGS.md#amdoc-web-005)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/contexts/ScreenContext.tsx` | typescript-ast-static | `051bbf46c10de8f4fce7382cb6c3e36a6a83a65557e6f1bc772ec4c414785165` |
| `src/hooks/useScreen.tsx` | typescript-ast-static | `8abe8789859e9d611a2fb94cb0b62d7977a85a475f236ad7689856d368ef2b44` |
| `src/protocol/screenMenuState.ts` | typescript-ast-static | `7d816c5a1309d7457a5129f4e4b39d3166a69aab5a4cc126c94a33103f7b0aeb` |
| `src/types/ScreenTypes.ts` | typescript-ast-static | `de3cf48411ba56a0a4e906e3d733ce2702f8481b3e29bfaa91c6553bb7eddc70` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `ScreenViewModel`

`web:src/contexts/ScreenContext.tsx:46`

| Atributo / firma | Propósito |
| --- | --- |
| `itemsCount: number` | Dato de estado/configuración `itemsCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `hasMenuItems: boolean` | Dato de estado/configuración `hasMenuItems`; declaración estructural, interpretación específica pendiente de ampliar. |
| `currentMenuPage: number` | Dato de estado/configuración `currentMenuPage`; declaración estructural, interpretación específica pendiente de ampliar. |
| `totalMenuPages: number` | Dato de estado/configuración `totalMenuPages`; declaración estructural, interpretación específica pendiente de ampliar. |
| `selectedIndex: number \| null` | Dato de estado/configuración `selectedIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `visibleStartIndex: number` | Dato de estado/configuración `visibleStartIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `visibleEndIndex: number` | Dato de estado/configuración `visibleEndIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `isValidationScreen: boolean` | Dato de estado/configuración `isValidationScreen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pinDigitsCount: number` | Dato de estado/configuración `pinDigitsCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `screenDefinition: ReturnType<typeof getScreenDefinition>` | Dato de estado/configuración `screenDefinition`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ScreenContextType`

`web:src/contexts/ScreenContext.tsx:59`

| Atributo / firma | Propósito |
| --- | --- |
| `currentScreen: ScreenViewModel \| null` | Dato de estado/configuración `currentScreen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `requestCurrentScreen: () => string \| null` | Solicita refresco de pantalla mediante el cliente; retorna correlación local o null, no un framebuffer ni una autorización. |

### `ScreenProviderProps`

`web:src/contexts/ScreenContext.tsx:68`

| Atributo / firma | Propósito |
| --- | --- |
| `children: ReactNode` | Dato de estado/configuración `children`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ScreenMeta`

`web:src/types/ScreenTypes.ts:19`

| Atributo / firma | Propósito |
| --- | --- |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `menu: number` | Dato de estado/configuración `menu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `submenu: number` | Dato de estado/configuración `submenu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `page: number` | Dato de estado/configuración `page`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source?: number` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sourceName?: string` | Dato de estado/configuración `sourceName`; declaración estructural, interpretación específica pendiente de ampliar. |
| `payload?: number[]` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |

### `ScreenUpdateKind`

`web:src/types/ScreenTypes.ts:142`

Tipo: `\| "screen.changed" \| "screen.current" \| "device.response"`.

### `ScreenReport`

`web:src/types/ScreenTypes.ts:147`

| Atributo / firma | Propósito |
| --- | --- |
| `cmd?: number` | Dato de estado/configuración `cmd`; declaración estructural, interpretación específica pendiente de ampliar. |
| `src?: number` | Dato de estado/configuración `src`; declaración estructural, interpretación específica pendiente de ampliar. |
| `dst?: number` | Dato de estado/configuración `dst`; declaración estructural, interpretación específica pendiente de ampliar. |
| `len?: number` | Dato de estado/configuración `len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `screen_code: number` | Dato de estado/configuración `screen_code`; declaración estructural, interpretación específica pendiente de ampliar. |
| `screenCode: number` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `screenCodeHex: string` | Dato de estado/configuración `screenCodeHex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `menu: number` | Dato de estado/configuración `menu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `submenu: number` | Dato de estado/configuración `submenu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `page: number` | Dato de estado/configuración `page`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source?: number` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sourceName?: string` | Dato de estado/configuración `sourceName`; declaración estructural, interpretación específica pendiente de ampliar. |
| `payload?: number[]` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |
| `rawData?: Record<string, unknown>` | Dato de estado/configuración `rawData`; declaración estructural, interpretación específica pendiente de ampliar. |

### `CurrentScreen`

`web:src/types/ScreenTypes.ts:164`

| Atributo / firma | Propósito |
| --- | --- |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `known: boolean` | Dato de estado/configuración `known`; declaración estructural, interpretación específica pendiente de ampliar. |
| `meta?: ScreenMeta` | Dato de estado/configuración `meta`; declaración estructural, interpretación específica pendiente de ampliar. |
| `updateKind: ScreenUpdateKind` | Dato de estado/configuración `updateKind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `receivedAt: number` | Dato de estado/configuración `receivedAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `requestId?: string` | Dato de estado/configuración `requestId`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-screenprovider"></a>`ScreenProvider({ children }: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `useCallback`, `useCarMode`, `useEffect` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:72`; structural |
| <a id="fn-carmodelabelfrombyte"></a>`carModeLabelFromByte(value: number): CarModeLabel` | Coordina `Math.max`, `Math.min`, `Math.trunc` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:624`; structural |
| <a id="fn-clampbyte"></a>`clampByte(value: number): number` | Coordina `Math.max`, `Math.min`, `Math.trunc` para Estado normalizado de pantalla remota. | `web:src/protocol/screenMenuState.ts:42`; structural |
| <a id="fn-createrequestid"></a>`createRequestId(): string` | Coordina `Date.now`, `Math.random`, `Math.random().toString` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:814`; structural |
| <a id="fn-enrichcurrentscreen"></a>`enrichCurrentScreen(baseScreen: CurrentScreen, rawData: unknown, globalMeta: { carMode: CarModeLabel; sensoresVisible: boolean; }): ScreenViewModel` | Construye el modelo visual con conteos, selección, ventana y clasificación de PIN. | `web:src/contexts/ScreenContext.tsx:295`; editorial |
| <a id="fn-enrichrawscreendata"></a>`enrichRawScreenData(rawData: unknown, meta: { carMode: CarModeLabel; firstVisibleIndex: number; hasMenuItems: boolean; itemsCount: number; sensoresVisible: boolean; selectedIndex: number \| null; }): Record<string, unknown>` | Agrega datos globales/derivados al rawData consumido por renderers. | `web:src/contexts/ScreenContext.tsx:385`; editorial |
| <a id="fn-extractscreenmeta"></a>`extractScreenMeta(data: unknown): { hasMenuItems?: boolean; itemsCount?: number; selectedIndex?: number; }` | Coordina `isRecord`, `normalizeStmMenuIndex`, `readBoolean` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:632`; structural |
| <a id="fn-formatscreencodehex"></a>`formatScreenCodeHex(screenCode: number): string` | Coordina `normalized.toString`, `normalized.toString(16).toUpperCase`, `normalized.toString(16).toUpperCase().padStart` para Estado normalizado de pantalla remota. | `web:src/types/ScreenTypes.ts:232`; structural |
| <a id="fn-getknownmenuitemscount"></a>`getKnownMenuItemsCount(screenCode: number, data: unknown, sensoresVisible: boolean): number \| undefined` | Adapta la llamada a `getWifiResultSsidCount` con el contexto de esta integración. | `web:src/contexts/ScreenContext.tsx:749`; structural |
| <a id="fn-getscreeneventname"></a>`getScreenEventName(payload: unknown): string \| undefined` | Coordina `isRecord`, `readString`, `toRecord` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:832`; structural |
| <a id="fn-getscreenresponsecommand"></a>`getScreenResponseCommand(payload: unknown): string \| undefined` | Coordina `isRecord`, `readString`, `toRecord` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:862`; structural |
| <a id="fn-getscreenresponserequestid"></a>`getScreenResponseRequestId(payload: unknown): string \| undefined` | Coordina `isRecord`, `readString`, `toRecord` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:847`; structural |
| <a id="fn-getwifiresultssidcount"></a>`getWifiResultSsidCount(data: unknown): number` | Coordina `Math.max`, `isRecord`, `readArray` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:774`; structural |
| <a id="fn-hasscreenidentity"></a>`hasScreenIdentity(value: unknown): boolean` | Coordina `hasScreenReportCmd`, `isRecord`, `readNumber` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:906`; structural |
| <a id="fn-hasscreenreportcmd"></a>`hasScreenReportCmd(data: unknown): boolean` | Coordina `isRecord`, `readNumber` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:820`; structural |
| <a id="fn-isrecord"></a>`isRecord(value: unknown): value is Record<string, unknown>` | Operación is Record de Estado normalizado de pantalla remota; semántica fina pendiente de ampliación. | `web:src/types/ScreenTypes.ts:328`; inferred-name |
| <a id="fn-isscreenlikerecord"></a>`isScreenLikeRecord(value: unknown): value is Record<string, unknown>` | Coordina `hasScreenReportCmd`, `isRecord`, `readNumber` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:889`; structural |
| <a id="fn-isshortmenuselectionreport"></a>`isShortMenuSelectionReport(data: unknown): boolean` | Coordina `isRecord`, `readNumber`, `readPayloadBytes` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:880`; structural |
| <a id="fn-iswificredentialsscreencode"></a>`isWifiCredentialsScreenCode(screenCode: number \| null): boolean` | Operación is Wifi Credentials Screen Code de Estado normalizado de pantalla remota; semántica fina pendiente de ampliación. | `web:src/contexts/ScreenContext.tsx:549`; inferred-name |
| <a id="fn-mergewificredentialsintoscreendata"></a>`mergeWifiCredentialsIntoScreenData(data: unknown, credentials: { status: string; ssid: string \| null; reason: string \| null; timeoutMs: number \| null; requestId: string \| null; }): Record<string, unknown>` | Coordina `isRecord`, `isWifiCredentialsScreenCode`, `readScreenCodeValue` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:506`; structural |
| <a id="fn-normalizescreenreport"></a>`normalizeScreenReport(data: unknown): ScreenReport \| null` | Extrae identidad/source y bytes de un reporte JSON admitido. | `web:src/types/ScreenTypes.ts:173`; editorial |
| <a id="fn-normalizestmmenuindex"></a>`normalizeStmMenuIndex(value: number \| undefined, itemsCount?: number): number \| undefined` | Limita el índice reportado sin cambiar su base cero. | `web:src/protocol/screenMenuState.ts:1`; editorial |
| <a id="fn-preservepreviousmenucountforshortselection"></a>`preservePreviousMenuCountForShortSelection(nextScreen: ScreenViewModel, previousScreen: ScreenViewModel \| null): ScreenViewModel` | Coordina `isShortMenuSelectionReport`, `withMenuItemsCount` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:414`; structural |
| <a id="fn-readarray"></a>`readArray(value: unknown): unknown[] \| undefined` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/contexts/ScreenContext.tsx:788`; structural |
| <a id="fn-readboolean"></a>`readBoolean(value: unknown): boolean \| undefined` | Coordina `Number.isFinite`, `value.trim`, `value.trim().toLowerCase` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:947`; structural |
| <a id="fn-readbyte"></a>`readByte(value: unknown): number \| undefined` | Adapta la llamada a `readInteger` con el contexto de esta integración. | `web:src/types/ScreenTypes.ts:297`; structural |
| <a id="fn-readcarmodelabelfromscreendata"></a>`readCarModeLabelFromScreenData(data: unknown): CarModeLabel \| null` | Coordina `Number`, `Number.isFinite`, `Number.parseInt` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:587`; structural |
| <a id="fn-readinteger"></a>`readInteger(value: unknown): number \| undefined` | Coordina `Math.trunc`, `Number`, `Number.isFinite` para Estado normalizado de pantalla remota. | `web:src/types/ScreenTypes.ts:311`; structural |
| <a id="fn-readnumber"></a>`readNumber(value: unknown): number \| undefined` | Coordina `Math.trunc`, `Number`, `Number.isFinite` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:930`; structural |
| <a id="fn-readnumberarray"></a>`readNumberArray(value: unknown): number[] \| undefined` | Coordina `Array.isArray`, `value.map`, `values.every` para Estado normalizado de pantalla remota. | `web:src/types/ScreenTypes.ts:282`; structural |
| <a id="fn-readpayloadbytes"></a>`readPayloadBytes(data: unknown): number[] \| undefined` | Coordina `Array.isArray`, `bytes.push`, `clampByte` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:792`; structural |
| <a id="fn-readpayloadsource"></a>`readPayloadSource(payload: number[] \| undefined, cmd: number \| undefined): number \| undefined` | Operación read Payload Source de Estado normalizado de pantalla remota; semántica fina pendiente de ampliación. | `web:src/types/ScreenTypes.ts:251`; inferred-name |
| <a id="fn-readscreencodefromparts"></a>`readScreenCodeFromParts(data: Record<string, unknown>): number \| null` | Adapta la llamada a `readByte` con el contexto de esta integración. | `web:src/types/ScreenTypes.ts:270`; structural |
| <a id="fn-readscreencodefrompayload"></a>`readScreenCodeFromPayload(payload: number[] \| undefined): number \| null` | Operación read Screen Code From Payload de Estado normalizado de pantalla remota; semántica fina pendiente de ampliación. | `web:src/types/ScreenTypes.ts:238`; inferred-name |
| <a id="fn-readscreencodevalue"></a>`readScreenCodeValue(data: Record<string, unknown>): number \| null` | Coordina `readNumber`, `readPayloadBytes` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:557`; structural |
| <a id="fn-readstring"></a>`readString(value: unknown): string \| undefined` | Operación read String de Estado normalizado de pantalla remota; semántica fina pendiente de ampliación. | `web:src/types/ScreenTypes.ts:293`; inferred-name |
| <a id="fn-readuint32"></a>`readUint32(value: unknown): number \| null` | Adapta la llamada a `readInteger` con el contexto de esta integración. | `web:src/types/ScreenTypes.ts:306`; structural |
| <a id="fn-resolvevisiblemenuwindow"></a>`resolveVisibleMenuWindow(selectedIndex: number \| undefined, itemsCount: number, requestedFirstVisibleIndex?: number, pageSize: inferred-by-TypeScript): { firstVisibleIndex: number; selectedSlot: number \| null }` | Deriva ventana por bloques cuando falta una primera fila explícita. | `web:src/protocol/screenMenuState.ts:14`; editorial |
| <a id="fn-screenmeta"></a>`screenMeta(title: string, screenCode: number, source: number, payload: number[]): ScreenMeta` | Operación screen Meta de Estado normalizado de pantalla remota; semántica fina pendiente de ampliación. | `web:src/types/ScreenTypes.ts:125`; inferred-name |
| <a id="fn-tocurrentscreen"></a>`toCurrentScreen(report: ScreenReport, updateKind: ScreenUpdateKind, requestId?: string): CurrentScreen` | Agrega metadata conocida y tiempo de recepción a un reporte. | `web:src/types/ScreenTypes.ts:214`; editorial |
| <a id="fn-torecord"></a>`toRecord(value: unknown): Record<string, unknown> \| undefined` | Adapta la llamada a `isRecord` con el contexto de esta integración. | `web:src/contexts/ScreenContext.tsx:922`; structural |
| <a id="fn-unwrapscreeneventdata"></a>`unwrapScreenEventData(payload: unknown): unknown` | Coordina `isRecord`, `isScreenLikeRecord`, `toRecord` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:480`; structural |
| <a id="fn-usescreen"></a>`useScreen(): inferred-by-TypeScript` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/hooks/useScreen.tsx:4`; structural |
| <a id="fn-withmenuitemscount"></a>`withMenuItemsCount(screen: ScreenViewModel, itemsCount: number): ScreenViewModel` | Coordina `Math.ceil`, `Math.floor`, `Math.max` para Estado normalizado de pantalla remota. | `web:src/contexts/ScreenContext.tsx:432`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useWebSocket` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:73` |
| `useUser` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:74` |
| `useCarMode` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:78` |
| `useWifiCredentials` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:79` |
| `useState` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:81` |
| `useRef` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:85` |
| `useRef` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:86` |
| `useRef` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:87` |
| `useCallback` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:89` |
| `useCallback` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:122` |
| `useEffect` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:140` |
| `useEffect` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:228` |
| `useEffect` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:243` |
| `useEffect` | `ScreenProvider` | `web:src/contexts/ScreenContext.tsx:259` |
| `useContext` | `useScreen` | `web:src/contexts/ScreenContext.tsx:286` |
| `useContext` | `useScreen` | `web:src/hooks/useScreen.tsx:5` |
