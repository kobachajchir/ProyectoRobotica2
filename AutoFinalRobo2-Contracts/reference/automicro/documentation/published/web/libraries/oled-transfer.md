# Transacción de transferencia canvas Web

ID: `web:module/src/features/oledEditor/useOledCanvasTransfer.ts` · Capa: **integración** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Coordina envío de un documento al canvas físico con estado, cancelación y respuesta final.

BEGIN/CHUNK/COMMIT/CANCEL y UI de estudio, con contexto TEST/pantalla de canvas.

<a id="modularity"></a>
## Modularidad observada

Adaptador entre modelo/raster, API y UI; no modifica el firmware desde el cliente.

<a id="state"></a>
## Handle, estado y atributos externos

Hook conserva transacción/offset/pending; el buffer y transferId pertenecen a esa operación.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Comprobar modo/pantalla y comenzar metadata.
- Enviar chunks en orden, commit y esperar presentación; cancelar/limpiar cuando corresponde.

<a id="interactions"></a>
## Interacciones

- ESP reserva/reconcilia canal y F4 posee el framebuffer de recepción.
- El mismo usuario puede guardar un documento sin enviarlo.
- Esquema compartido por operación: published/shared/contracts/oled-canvas-v1.md; actualizar productor, traductor y consumidor al cambiar offsets/chunks/CRC.

<a id="limitations"></a>
## Precauciones y límites

- Recepción del último chunk no es confirmación de que el panel presentó la imagen.
- El canal no devuelve automáticamente imágenes de otras pantallas físicas.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/features/oledEditor/LopakaLikeEditor.tsx` | typescript-ast-static | `2f08168943628bfd5b9fc251e6e5670727fd8bf9903f4bf06f9c9e4618fccff5` |
| `src/features/oledEditor/OledCanvasTransferPanel.tsx` | typescript-ast-static | `6c27b52ea4959426925310513d4d47bef493ab14f80210c2c52e71f6ec0d2fa8` |
| `src/features/oledEditor/OledStudioWorkspace.tsx` | typescript-ast-static | `193ec488fa6248aab24ee6c7a2f148a7ba2e8159bee8345d98efc67fd258ecf0` |
| `src/features/oledEditor/useOledCanvasTransfer.ts` | typescript-ast-static | `91998fee17ac2ffe36254449032ac1d059123fcadc29ea93bab26bff5479e23e` |
| `src/pages/OledEditorSection.tsx` | typescript-ast-static | `37680a6f4544127f9555df1020b9197048b0575e6ea7e7e312157f677c6bc660` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `DraftShape`

`web:src/features/oledEditor/LopakaLikeEditor.tsx:73`

Tipo: `\| { tool: "rectangle"; start: Point; current: Point } \| { tool: "circle"; start: Point; current: Point } \| { tool: "line"; start: Point; current: Point; snapped: boolean } \| { tool: "triangle"; points: Point[]; current: Point \| null } \| { tool: "polygon"; points: Point[]; current: Point \| null } \| { tool: "marquee"; start: Point; current: Point }`.

### `InteractionSession`

`web:src/features/oledEditor/LopakaLikeEditor.tsx:81`

Tipo: `\| { kind: "move"; beforeDocument: EditorDocument; selectedIds: string[]; start: Point; } \| { kind: "resize"; beforeDocument: EditorDocument; objectId: string; handle: HandlePosition; start: Point; bounds: ReturnType<typeof getObjectBounds>; } \| { kind: "line-endpoint"; beforeDocument: EditorDocument; objectId: string; endpoint: 0 \| 1; } \| { kind: "vertex"; beforeDocument: EditorDocument; objectId: string; index: number; } \| { kind: "paint"; beforeDocument: EditorDocument; objectId: string; lastPoint: Point; erase: boolean; } \| null`.

### `LopakaLikeEditorProps`

`web:src/features/oledEditor/LopakaLikeEditor.tsx:117`

| Atributo / firma | Propósito |
| --- | --- |
| `initialDocument?: EditorDocument` | Dato de estado/configuración `initialDocument`; declaración estructural, interpretación específica pendiente de ampliar. |
| `readOnly?: boolean` | Dato de estado/configuración `readOnly`; declaración estructural, interpretación específica pendiente de ampliar. |
| `activeView?: "visualization" \| "code"` | Dato de estado/configuración `activeView`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onDocumentChange?: (document: EditorDocument) => void` | Entrega documento editado al dueño; el objeto pertenece al modelo Web, no a memoria F4. |
| `onCodeChange?: (result: GeneratedCodeResult) => void` | Entrega código generado al panel consumidor; generación no compila ni envía firmware. |
| `onDirtyChange?: (dirty: boolean) => void` | Informa si el editor tiene cambios locales pendientes de guardar. |
| `onSaveDocument?: (document: EditorDocument) => void` | Pide persistir documento Web por la política del dueño; no es commit OLED Canvas físico. |
| `onClearDocument?: () => void` | Pide limpiar documento/selección del editor; no borra pantalla F4 sin transferencia explícita. |

### `EditorIconProps`

`web:src/features/oledEditor/LopakaLikeEditor.tsx:144`

| Atributo / firma | Propósito |
| --- | --- |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledStudioTab`

`web:src/features/oledEditor/OledStudioWorkspace.tsx:9`

Tipo: `"visualization" \| "code" \| "stream"`.

### `OledCanvasTransferPhase`

`web:src/features/oledEditor/useOledCanvasTransfer.ts:25`

Tipo: `\| "idle" \| "preparing" \| "uploading" \| "waiting_f4" \| "rendered" \| "canceling" \| "canceled" \| "error"`.

### `OledCanvasTransferState`

`web:src/features/oledEditor/useOledCanvasTransfer.ts:35`

| Atributo / firma | Propósito |
| --- | --- |
| `phase: OledCanvasTransferPhase` | Dato de estado/configuración `phase`; declaración estructural, interpretación específica pendiente de ampliar. |
| `progressBytes: number` | Dato de estado/configuración `progressBytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `message: string` | Dato de estado/configuración `message`; declaración estructural, interpretación específica pendiente de ampliar. |
| `error: string \| null` | Dato de estado/configuración `error`; declaración estructural, interpretación específica pendiente de ampliar. |
| `transferId: number \| null` | Dato de estado/configuración `transferId`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LocalCancellation`

`web:src/features/oledEditor/useOledCanvasTransfer.ts:51`


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-bitmapminipreview"></a>`BitmapMiniPreview({  width,  height,  pixels, }: { width: number; height: number; pixels: number[]; }): inferred-by-TypeScript` | Coordina `Math.floor`, `Math.max`, `pixels.map` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2317`; structural |
| <a id="fn-boundsfields"></a>`BoundsFields({  x,  y,  width,  height,  onChange, }: { x: number; y: number; width: number; height: number; onChange: (patch: { x: number; y: number; width: number; height: number }) => void; }): inferred-by-TypeScript` | Operación Bounds Fields de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2223`; inferred-name |
| <a id="fn-chevrondownicon"></a>`ChevronDownIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Chevron Down Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2157`; inferred-name |
| <a id="fn-chevrontoggleicon"></a>`ChevronToggleIcon({ open }: { open: boolean }): inferred-by-TypeScript` | Operación Chevron Toggle Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/OledStudioWorkspace.tsx:247`; inferred-name |
| <a id="fn-chevronupicon"></a>`ChevronUpIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Chevron Up Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2149`; inferred-name |
| <a id="fn-circletoolicon"></a>`CircleToolIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Circle Tool Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2048`; inferred-name |
| <a id="fn-clearicon"></a>`ClearIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Limpia Icon dentro de Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2214`; inferred-name |
| <a id="fn-cursorselecticon"></a>`CursorSelectIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Cursor Select Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2014`; inferred-name |
| <a id="fn-eyeclosedicon"></a>`EyeClosedIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Eye Closed Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2121`; inferred-name |
| <a id="fn-eyeopenicon"></a>`EyeOpenIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Eye Open Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2112`; inferred-name |
| <a id="fn-framebufferpreview"></a>`FramebufferPreview({  title,  framebuffer,  confirmed = false, }: { title: string; framebuffer: Uint8Array; confirmed?: boolean; }): inferred-by-TypeScript` | Adapta la llamada a `useMemo` con el contexto de esta integración. | `web:src/features/oledEditor/OledCanvasTransferPanel.tsx:169`; structural |
| <a id="fn-iconbase"></a>`IconBase({  className,  children,  viewBox = "0 0 24 24",  fill = "none", }: { className?: string; children: ReactNode; viewBox?: string; fill?: string; }): inferred-by-TypeScript` | Operación Icon Base de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1973`; inferred-name |
| <a id="fn-imagetoolicon"></a>`ImageToolIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Image Tool Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2092`; inferred-name |
| <a id="fn-layeractioniconbutton"></a>`LayerActionIconButton({  label,  icon,  onClick,  disabled = false, }: { label: string; icon: ReactNode; onClick?: (event: ReactMouseEvent<HTMLButtonElement>) => void; disabled?: boolean; }): inferred-by-TypeScript` | Operación Layer Action Icon Button de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1948`; inferred-name |
| <a id="fn-linetoolicon"></a>`LineToolIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Line Tool Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2056`; inferred-name |
| <a id="fn-lockclosedicon"></a>`LockClosedIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Lock Closed Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2131`; inferred-name |
| <a id="fn-lockopenicon"></a>`LockOpenIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Lock Open Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2140`; inferred-name |
| <a id="fn-lopakalikeeditor"></a>`LopakaLikeEditor({  initialDocument,  readOnly = false,  activeView = "visualization",  onDocumentChange,  onCodeChange,  onDirtyChange,  onSaveDocument,  onClearDocument, }: LopakaLikeEditorProps): inferred-by-TypeScript` | Coordina `( [ ["wrapperFunction", "Función contenedora"], ["includeComments", "Comentarios por capa"], ["declareImages", "Declarar bitmaps locales"], ["declareVariables", "Declarar variables de pantalla"], ["clearDisplay", "Limpiar pantalla al inicio"], ] as Array<[keyof typeof editor.state.ui.codeSettings, string]> ).map`, `TOOL_DEFINITIONS.map`, `[...editor.state.document.zOrder].reverse` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:148`; structural |
| <a id="fn-monocolorfields"></a>`MonoColorFields({  fill,  stroke,  onFillChange,  onStrokeChange, }: { fill: "white" \| "black" \| null; stroke: "white" \| "black" \| null; onFillChange: (value: "white" \| "black" \| null) => void; onStrokeChange: (value: "white" \| "black" \| null) => void; }): inferred-by-TypeScript` | Operación Mono Color Fields de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2246`; inferred-name |
| <a id="fn-monoselect"></a>`MonoSelect({  label,  value,  onChange,  allowNull = false, }: { label: string; value: "white" \| "black" \| null; onChange: (value: "white" \| "black" \| null) => void; allowNull?: boolean; }): inferred-by-TypeScript` | Operación Mono Select de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2265`; inferred-name |
| <a id="fn-numberfield"></a>`NumberField({  label,  value,  onChange, }: { label: string; value: number; onChange: (value: number) => void; }): inferred-by-TypeScript` | Operación Number Field de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2295`; inferred-name |
| <a id="fn-objectinspector"></a>`ObjectInspector({  object,  onRename,  onDelete,  onToggleLocked,  onToggleHidden,  onPatch,  onAssetChange,  assetList, }: { object: CanvasObject; onRename: (name: string) => void; onDelete: () => void; onToggleLocked: () => void; onToggleHidden: () => void; onPatch: (object: CanvasObject, label: string) => void; onAssetChange: (assetId: string) => void; assetList: ReturnType<typeof listEditorAssets>; }): inferred-by-TypeScript` | Adapta la llamada a `renderInspectorFields` con el contexto de esta integración. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1637`; structural |
| <a id="fn-oledcanvastransferpanel"></a>`OledCanvasTransferPanel({ document }: { document: EditorDocument }): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min`, `Math.round` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/OledCanvasTransferPanel.tsx:15`; structural |
| <a id="fn-olededitorsection"></a>`OledEditorSection(): inferred-by-TypeScript` | Operación Oled Editor Section de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/pages/OledEditorSection.tsx:3`; inferred-name |
| <a id="fn-oledstudioworkspace"></a>`OledStudioWorkspace(): inferred-by-TypeScript` | Coordina `OLED_STUDIO_TABS.map`, `savedScreens.map`, `useCallback` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/OledStudioWorkspace.tsx:20`; structural |
| <a id="fn-paintbrushtoolicon"></a>`PaintBrushToolIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Paint Brush Tool Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2102`; inferred-name |
| <a id="fn-polygontoolicon"></a>`PolygonToolIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Polygon Tool Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2074`; inferred-name |
| <a id="fn-rectangletoolicon"></a>`RectangleToolIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Rectangle Tool Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2040`; inferred-name |
| <a id="fn-redoicon"></a>`RedoIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Redo Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2195`; inferred-name |
| <a id="fn-saveicon"></a>`SaveIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Save Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2204`; inferred-name |
| <a id="fn-texttoolicon"></a>`TextToolIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Text Tool Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2082`; inferred-name |
| <a id="fn-toolbariconbutton"></a>`ToolbarIconButton({  label,  active = false,  disabled = false,  onClick,  children, }: { label: string; active?: boolean; disabled?: boolean; onClick?: (event: ReactMouseEvent<HTMLButtonElement>) => void; children: ReactNode; }): inferred-by-TypeScript` | Operación Toolbar Icon Button de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1907`; inferred-name |
| <a id="fn-triangletoolicon"></a>`TriangleToolIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Triangle Tool Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2066`; inferred-name |
| <a id="fn-undoicon"></a>`UndoIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Undo Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2186`; inferred-name |
| <a id="fn-zoominicon"></a>`ZoomInIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Zoom In Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2165`; inferred-name |
| <a id="fn-zoomouticon"></a>`ZoomOutIcon({ className }: EditorIconProps): inferred-by-TypeScript` | Operación Zoom Out Icon de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2176`; inferred-name |
| <a id="fn-applypaintstroke"></a>`applyPaintStroke(document: EditorDocument, objectId: string, from: Point, to: Point, erase: boolean): inferred-by-TypeScript` | Coordina `cloneDocument`, `linePoints`, `points.forEach` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2463`; structural |
| <a id="fn-canvasnavigationmessage"></a>`canvasNavigationMessage(): string` | Operación canvas Navigation Message de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/useOledCanvasTransfer.ts:312`; inferred-name |
| <a id="fn-decodeimagefile"></a>`decodeImageFile(file: File): inferred-by-TypeScript` | Coordina `Array.from`, `Math.ceil`, `Math.max` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2523`; structural |
| <a id="fn-drawpackedbitmap"></a>`drawPackedBitmap(context: CanvasRenderingContext2D, bytes: readonly number[], sourceWidth: number, sourceHeight: number, object: ImageObject, bitOrder: ImageAsset["bitOrder"]): inferred-by-TypeScript` | Coordina `Math.max`, `bitmapBytesToPixels`, `context.fillRect` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2395`; structural |
| <a id="fn-drawpixels"></a>`drawPixels(context: CanvasRenderingContext2D, object: BitmapObject): inferred-by-TypeScript` | Adapta la llamada a `context.fillRect` con el contexto de esta integración. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2424`; structural |
| <a id="fn-drawrasterobjects"></a>`drawRasterObjects(canvas: HTMLCanvasElement \| null, document: EditorDocument, assetIndex: Record<string, ImageAsset>): inferred-by-TypeScript` | Coordina `canvas.getContext`, `context.clearRect`, `context.fillRect` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2355`; structural |
| <a id="fn-formatcarmodevalue"></a>`formatCarModeValue(value: number): string` | Coordina `value.toString`, `value.toString(16).toUpperCase`, `value.toString(16).toUpperCase().padStart` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/OledCanvasTransferPanel.tsx:162`; structural |
| <a id="fn-framebufferpath"></a>`framebufferPath(framebuffer: Uint8Array): string` | Coordina `segments.join`, `segments.push`, `unpackPageMajor` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/OledCanvasTransferPanel.tsx:195`; structural |
| <a id="fn-gettoollabel"></a>`getToolLabel(tool: ToolId): inferred-by-TypeScript` | Adapta la llamada a `TOOL_DEFINITIONS.find` con el contexto de esta integración. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1903`; structural |
| <a id="fn-isactivephase"></a>`isActivePhase(phase: OledCanvasTransferPhase): boolean` | Operación is Active Phase de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/useOledCanvasTransfer.ts:316`; inferred-name |
| <a id="fn-isarrowkey"></a>`isArrowKey(key: string): inferred-by-TypeScript` | Operación is Arrow Key de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2459`; inferred-name |
| <a id="fn-iseditableelement"></a>`isEditableElement(element: Element \| null): inferred-by-TypeScript` | Operación is Editable Element de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2001`; inferred-name |
| <a id="fn-linepoints"></a>`linePoints(start: Point, end: Point): inferred-by-TypeScript` | Coordina `Math.abs`, `points.push` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2494`; structural |
| <a id="fn-monotopaint"></a>`monoToPaint(color: "white" \| "black" \| null): inferred-by-TypeScript` | Operación mono To Paint de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2440`; inferred-name |
| <a id="fn-parsemode"></a>`parseMode(value: unknown): number \| null` | Coordina `Math.trunc`, `Number`, `Number.isFinite` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/useOledCanvasTransfer.ts:301`; structural |
| <a id="fn-readcarmode"></a>`readCarMode(value: unknown): number \| null` | Coordina `Array.isArray`, `parseMode` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/useOledCanvasTransfer.ts:293`; structural |
| <a id="fn-rectsoverlap"></a>`rectsOverlap(left: ReturnType<typeof normalizeRect>, right: ReturnType<typeof getObjectBounds>): inferred-by-TypeScript` | Operación rects Overlap de Transacción de transferencia canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:2450`; inferred-name |
| <a id="fn-renderinspectorfields"></a>`renderInspectorFields(object: CanvasObject, onPatch: (object: CanvasObject, label: string) => void, onAssetChange: (assetId: string) => void, assetList: ReturnType<typeof listEditorAssets>): inferred-by-TypeScript` | Coordina `OLED_FONT_PRESETS.map`, `assetList.map`, `object.points.map` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1697`; structural |
| <a id="fn-useoledcanvastransfer"></a>`useOledCanvasTransfer(): inferred-by-TypeScript` | Gestiona transacción de canvas y resultado final remoto. | `web:src/features/oledEditor/useOledCanvasTransfer.ts:53`; editorial |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-lopakalikeeditor.copygeneratedcode"></a>`LopakaLikeEditor.copyGeneratedCode(): inferred-by-TypeScript` | Coordina `navigator.clipboard.writeText`, `setCodeCopied` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:875`; structural |
| <a id="fn-lopakalikeeditor.ensurepaintlayer"></a>`LopakaLikeEditor.ensurePaintLayer(): inferred-by-TypeScript` | Coordina `createBitmapObject`, `editor.setSelection`, `editor.state.document.zOrder .map` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:807`; structural |
| <a id="fn-lopakalikeeditor.finalizepolygondraft"></a>`LopakaLikeEditor.finalizePolygonDraft(points: Point[]): inferred-by-TypeScript` | Coordina `cloneDocument`, `createPolygon`, `editor.commitDocument` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:847`; structural |
| <a id="fn-lopakalikeeditor.finalizetriangledraft"></a>`LopakaLikeEditor.finalizeTriangleDraft(points: [Point, Point, Point]): inferred-by-TypeScript` | Coordina `cloneDocument`, `createTriangle`, `editor.commitDocument` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:836`; structural |
| <a id="fn-lopakalikeeditor.findtopobjectatpoint"></a>`LopakaLikeEditor.findTopObjectAtPoint(point: Point): inferred-by-TypeScript` | Adapta la llamada a `hitTestObject` con el contexto de esta integración. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:322`; structural |
| <a id="fn-lopakalikeeditor.focuseditor"></a>`LopakaLikeEditor.focusEditor(): inferred-by-TypeScript` | Adapta la llamada a `rootRef.current?.focus` con el contexto de esta integración. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:297`; structural |
| <a id="fn-lopakalikeeditor.getcanvaspointfrompointer"></a>`LopakaLikeEditor.getCanvasPointFromPointer(event: ReactPointerEvent<HTMLDivElement>): Point` | Coordina `Math.floor`, `clamp`, `stageRef.current?.getBoundingClientRect` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:301`; structural |
| <a id="fn-lopakalikeeditor.handlecleardocument"></a>`LopakaLikeEditor.handleClearDocument(): inferred-by-TypeScript` | Adapta la llamada a `onClearDocument` con el contexto de esta integración. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:888`; structural |
| <a id="fn-lopakalikeeditor.handlesavedocument"></a>`LopakaLikeEditor.handleSaveDocument(): inferred-by-TypeScript` | Coordina `cloneDocument`, `onSaveDocument` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:884`; structural |
| <a id="fn-lopakalikeeditor.handleselectionpointerdown"></a>`LopakaLikeEditor.handleSelectionPointerDown(event: ReactPointerEvent<HTMLDivElement>, point: Point): inferred-by-TypeScript` | Coordina `cloneDocument`, `editor.setSelection`, `editor.state.selection.selectedIds .map` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:448`; structural |
| <a id="fn-lopakalikeeditor.handlestagecontextmenu"></a>`LopakaLikeEditor.handleStageContextMenu(event: React.MouseEvent<HTMLDivElement>): inferred-by-TypeScript` | Coordina `cloneDocument`, `editor.commitDocument`, `event.preventDefault` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:779`; structural |
| <a id="fn-lopakalikeeditor.handlestagedoubleclick"></a>`LopakaLikeEditor.handleStageDoubleClick(): inferred-by-TypeScript` | Adapta la llamada a `finalizePolygonDraft` con el contexto de esta integración. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:773`; structural |
| <a id="fn-lopakalikeeditor.handlestagepointerdown"></a>`LopakaLikeEditor.handleStagePointerDown(event: ReactPointerEvent<HTMLDivElement>): inferred-by-TypeScript` | Coordina `applyPaintStroke`, `centerObjectAt`, `cloneDocument` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:333`; structural |
| <a id="fn-lopakalikeeditor.handlestagepointermove"></a>`LopakaLikeEditor.handleStagePointerMove(event: ReactPointerEvent<HTMLDivElement>): inferred-by-TypeScript` | Coordina `applyPaintStroke`, `cloneDocument`, `editor.setDocument` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:557`; structural |
| <a id="fn-lopakalikeeditor.handlestagepointerup"></a>`LopakaLikeEditor.handleStagePointerUp(): inferred-by-TypeScript` | Coordina `Math.max`, `Math.round`, `cloneDocument` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:664`; structural |
| <a id="fn-lopakalikeeditor.handleuploadchange"></a>`LopakaLikeEditor.handleUploadChange(event: ChangeEvent<HTMLInputElement>): inferred-by-TypeScript` | Coordina `cloneDocument`, `createUploadedAsset`, `decodeImageFile` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:858`; structural |
| <a id="fn-lopakalikeeditor.listener"></a>`LopakaLikeEditor.listener(event: KeyboardEvent): inferred-by-TypeScript` | Coordina `cloneDocument`, `editor.commitDocument`, `editor.redo` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:195`; structural |
| <a id="fn-lopakalikeeditor.renderdraftshape"></a>`LopakaLikeEditor.renderDraftShape(shape: DraftShape \| null): inferred-by-TypeScript` | Coordina `Math.max`, `normalizeRect`, `points.map` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1513`; structural |
| <a id="fn-lopakalikeeditor.renderselectionoverlay"></a>`LopakaLikeEditor.renderSelectionOverlay(bounds: ReturnType<typeof getSelectionBounds>, object: CanvasObject \| null): inferred-by-TypeScript` | Coordina `Object.entries`, `Object.entries(handles).map`, `getResizeHandles` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1579`; structural |
| <a id="fn-lopakalikeeditor.rendervectorobject"></a>`LopakaLikeEditor.renderVectorObject(object: CanvasObject): inferred-by-TypeScript` | Coordina `monoToPaint`, `object.points.map`, `object.points.map((point) => `${point.x},${point.y}`).join` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:1433`; structural |
| <a id="fn-lopakalikeeditor.updatesingleobject"></a>`LopakaLikeEditor.updateSingleObject(nextObject: CanvasObject, label: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `cloneDocument`, `editor.commitDocument`, `replaceObject` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/LopakaLikeEditor.tsx:892`; structural |
| <a id="fn-oledstudioworkspace.handlecleardocument"></a>`OledStudioWorkspace.handleClearDocument(): inferred-by-TypeScript` | Coordina `cloneDocument`, `createEmptyDocument`, `setEditorKey` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/OledStudioWorkspace.tsx:64`; structural |
| <a id="fn-oledstudioworkspace.handledeletescreen"></a>`OledStudioWorkspace.handleDeleteScreen(screenId: string): inferred-by-TypeScript` | Coordina `deleteScreen`, `setLoadedScreenId` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/OledStudioWorkspace.tsx:57`; structural |
| <a id="fn-oledstudioworkspace.handleloadscreen"></a>`OledStudioWorkspace.handleLoadScreen(screenId: string): inferred-by-TypeScript` | Coordina `cloneDocument`, `savedScreens.find`, `setEditorKey` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/OledStudioWorkspace.tsx:45`; structural |
| <a id="fn-oledstudioworkspace.handlesavedocument"></a>`OledStudioWorkspace.handleSaveDocument(document: EditorDocument): inferred-by-TypeScript` | Coordina `saveScreen`, `setLoadedScreenId` para Transacción de transferencia canvas Web. | `web:src/features/oledEditor/OledStudioWorkspace.tsx:37`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useOledEditorState` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:158` |
| `useRef` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:164` |
| `useRef` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:165` |
| `useRef` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:166` |
| `useRef` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:167` |
| `useRef` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:168` |
| `useState` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:170` |
| `useRef` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:171` |
| `useState` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:172` |
| `useMemo` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:174` |
| `useMemo` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:185` |
| `useEffect` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:190` |
| `useEffect` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:194` |
| `useEffect` | `LopakaLikeEditor` | `web:src/features/oledEditor/LopakaLikeEditor.tsx:288` |
| `useMemo` | `OledCanvasTransferPanel` | `web:src/features/oledEditor/OledCanvasTransferPanel.tsx:16` |
| `useOledCanvasTransfer` | `OledCanvasTransferPanel` | `web:src/features/oledEditor/OledCanvasTransferPanel.tsx:17` |
| `useMemo` | `FramebufferPreview` | `web:src/features/oledEditor/OledCanvasTransferPanel.tsx:178` |
| `useSavedOledScreens` | `OledStudioWorkspace` | `web:src/features/oledEditor/OledStudioWorkspace.tsx:21` |
| `useState` | `OledStudioWorkspace` | `web:src/features/oledEditor/OledStudioWorkspace.tsx:22` |
| `useState` | `OledStudioWorkspace` | `web:src/features/oledEditor/OledStudioWorkspace.tsx:23` |
| `useState` | `OledStudioWorkspace` | `web:src/features/oledEditor/OledStudioWorkspace.tsx:24` |
| `useState` | `OledStudioWorkspace` | `web:src/features/oledEditor/OledStudioWorkspace.tsx:27` |
| `useState` | `OledStudioWorkspace` | `web:src/features/oledEditor/OledStudioWorkspace.tsx:30` |
| `useState` | `OledStudioWorkspace` | `web:src/features/oledEditor/OledStudioWorkspace.tsx:31` |
| `useCallback` | `OledStudioWorkspace` | `web:src/features/oledEditor/OledStudioWorkspace.tsx:33` |
| `useWebSocket` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:54` |
| `useUser` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:55` |
| `useCarMode` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:56` |
| `useScreen` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:57` |
| `useState` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:58` |
| `useState` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:59` |
| `useState` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:60` |
| `useRef` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:61` |
| `useRef` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:62` |
| `useRef` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:63` |
| `useRef` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:64` |
| `useRef` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:65` |
| `useCallback` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:72` |
| `useCallback` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:87` |
| `useCallback` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:102` |
| `useEffect` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:230` |
| `useEffect` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:236` |
| `useEffect` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:243` |
| `useEffect` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:249` |
| `useEffect` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:257` |
| `useMemo` | `useOledCanvasTransfer` | `web:src/features/oledEditor/useOledCanvasTransfer.ts:266` |
