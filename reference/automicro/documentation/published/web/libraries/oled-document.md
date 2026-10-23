# Modelo y edición de documentos OLED

ID: `web:module/src/features/oledEditor/document.ts` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Representa y edita documentos gráficos que el usuario puede guardar y enviar a la OLED.

Elementos, geometría, assets, generación de código y persistencia local; separado de captura de pantalla remota.

<a id="modularity"></a>
## Modularidad observada

Modelo/cálculo separables de React, con hooks/almacenamiento como adaptación.

<a id="state"></a>
## Handle, estado y atributos externos

Documento/borrador pertenece al editor; savedScreens conserva su versión local. Referencias a assets deben resolverse al rasterizar.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Crear/cargar documento, editar elementos y serializar/exportar.
- Guardar una copia local no modifica la pantalla del robot sin transferencia.

<a id="interactions"></a>
## Interacciones

- Rasterizer convierte modelo a 1024 bytes; useOledCanvasTransfer coordina envío.

<a id="limitations"></a>
## Precauciones y límites

- Código generado y render físico deben usar la misma geometría y orden de bits.
- Una previsualización del editor no es una muestra de la pantalla activa F4.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/contexts/SavedOledScreensContext.tsx` | typescript-ast-static | `57bf4eecb95e9fa5f4ccef303174d44f56aaff850fee2c51c1915ac88cd8c40c` |
| `src/features/oledEditor/assets.ts` | typescript-ast-static | `45d7516a1575ae70a56dd6e959204c4267b983a425a4326a48b3ad880d1106c4` |
| `src/features/oledEditor/codegen.ts` | typescript-ast-static | `0d035809c10733546649da2ca790f90c8d31e5f0a79f4f714b5e04e7cc99d686` |
| `src/features/oledEditor/document.ts` | typescript-ast-static | `680e5c2bab0cddfb6d296b4c2eb20936930b8ec0c097697c7be67d1074380f9d` |
| `src/features/oledEditor/geometry.ts` | typescript-ast-static | `c7ec1d9bdebe4c7c601793c23393792456745addc450220dc953e59e8c930904` |
| `src/features/oledEditor/types.ts` | typescript-ast-static | `5ac41095010cfa6cd267f583ea3c584e6f3e436a0016fc1534bb73f81c88dabf` |
| `src/features/oledEditor/useOledEditorState.ts` | typescript-ast-static | `6d2cabca6be6cc9ac14a5022c29d0c5a71826ae432bcd4e9e770d0781988af6a` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `PersistedSavedOledScreen`

`web:src/contexts/SavedOledScreensContext.tsx:16`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |
| `document: EditorDocument` | Dato de estado/configuración `document`; declaración estructural, interpretación específica pendiente de ampliar. |
| `createdAt: number` | Dato de estado/configuración `createdAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `updatedAt: number` | Dato de estado/configuración `updatedAt`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SavedOledScreen`

`web:src/contexts/SavedOledScreensContext.tsx:24`

| Atributo / firma | Propósito |
| --- | --- |
| `commands: OledCommand[]` | Dato de estado/configuración `commands`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SaveScreenArgs`

`web:src/contexts/SavedOledScreensContext.tsx:28`

| Atributo / firma | Propósito |
| --- | --- |
| `id?: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `document: EditorDocument` | Dato de estado/configuración `document`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SavedOledScreensContextValue`

`web:src/contexts/SavedOledScreensContext.tsx:33`

| Atributo / firma | Propósito |
| --- | --- |
| `savedScreens: SavedOledScreen[]` | Dato de estado/configuración `savedScreens`; declaración estructural, interpretación específica pendiente de ampliar. |
| `saveScreen: (args: SaveScreenArgs) => string` | Operación save Screen de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. |
| `deleteScreen: (id: string) => void` | Operación delete Screen de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. |
| `getScreen: (id: string) => SavedOledScreen \| undefined` | Operación get Screen de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. |

### `EditorFontPreset`

`web:src/features/oledEditor/assets.ts:5`

| Atributo / firma | Propósito |
| --- | --- |
| `id: OledFont` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `width: number` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: number` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EditorBitmapAssetDefinition`

`web:src/features/oledEditor/assets.ts:12`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `category: string` | Dato de estado/configuración `category`; declaración estructural, interpretación específica pendiente de ampliar. |
| `dataRef: string` | Dato de estado/configuración `dataRef`; declaración estructural, interpretación específica pendiente de ampliar. |
| `description?: string` | Dato de estado/configuración `description`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SharedAssetSeed`

`web:src/features/oledEditor/assets.ts:19`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `width: number` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: number` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bitOrder: BitOrder` | Dato de estado/configuración `bitOrder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `category: string` | Dato de estado/configuración `category`; declaración estructural, interpretación específica pendiente de ampliar. |
| `description?: string` | Dato de estado/configuración `description`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sourceRef?: string` | Dato de estado/configuración `sourceRef`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LocalBitmapDeclaration`

`web:src/features/oledEditor/codegen.ts:17`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `objectId: string` | Dato de estado/configuración `objectId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `symbol: string` | Dato de estado/configuración `symbol`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bytes: number[]` | Dato de estado/configuración `bytes`; declaración estructural, interpretación específica pendiente de ampliar. |

### `HandlePosition`

`web:src/features/oledEditor/geometry.ts:15`

Tipo: `\| "nw" \| "n" \| "ne" \| "e" \| "se" \| "s" \| "sw" \| "w"`.

### `ToolId`

`web:src/features/oledEditor/types.ts:3`

Tipo: `\| "select" \| "image" \| "paint" \| "text" \| "rectangle" \| "circle" \| "line" \| "triangle" \| "polygon"`.

### `MonoColor`

`web:src/features/oledEditor/types.ts:14`

Tipo: `"white" \| "black"`.

### `BitOrder`

`web:src/features/oledEditor/types.ts:15`

Tipo: `"lsb" \| "msb"`.

### `CanvasObjectKind`

`web:src/features/oledEditor/types.ts:16`

Tipo: `\| "rectangle" \| "circle" \| "line" \| "triangle" \| "polygon" \| "text" \| "image" \| "bitmap"`.

### `Point`

`web:src/features/oledEditor/types.ts:26`

| Atributo / firma | Propósito |
| --- | --- |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BoundingBox`

`web:src/features/oledEditor/types.ts:31`

| Atributo / firma | Propósito |
| --- | --- |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `width: number` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: number` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ScreenSpec`

`web:src/features/oledEditor/types.ts:38`

| Atributo / firma | Propósito |
| --- | --- |
| `width: number` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: number` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `background: MonoColor` | Dato de estado/configuración `background`; declaración estructural, interpretación específica pendiente de ampliar. |
| `title: string` | Dato de estado/configuración `title`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BaseObject`

`web:src/features/oledEditor/types.ts:45`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kind: CanvasObjectKind` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `locked: boolean` | Dato de estado/configuración `locked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `hidden: boolean` | Dato de estado/configuración `hidden`; declaración estructural, interpretación específica pendiente de ampliar. |
| `overlay?: boolean` | Dato de estado/configuración `overlay`; declaración estructural, interpretación específica pendiente de ampliar. |
| `zIndex: number` | Dato de estado/configuración `zIndex`; declaración estructural, interpretación específica pendiente de ampliar. |

### `RectangleObject`

`web:src/features/oledEditor/types.ts:55`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "rectangle"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `width: number` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: number` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fill: MonoColor \| null` | Dato de estado/configuración `fill`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stroke: MonoColor \| null` | Dato de estado/configuración `stroke`; declaración estructural, interpretación específica pendiente de ampliar. |
| `strokeWidth: number` | Dato de estado/configuración `strokeWidth`; declaración estructural, interpretación específica pendiente de ampliar. |

### `CircleObject`

`web:src/features/oledEditor/types.ts:66`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "circle"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `cx: number` | Dato de estado/configuración `cx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `cy: number` | Dato de estado/configuración `cy`; declaración estructural, interpretación específica pendiente de ampliar. |
| `radius: number` | Dato de estado/configuración `radius`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fill: MonoColor \| null` | Dato de estado/configuración `fill`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stroke: MonoColor \| null` | Dato de estado/configuración `stroke`; declaración estructural, interpretación específica pendiente de ampliar. |
| `strokeWidth: number` | Dato de estado/configuración `strokeWidth`; declaración estructural, interpretación específica pendiente de ampliar. |

### `LineObject`

`web:src/features/oledEditor/types.ts:76`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "line"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `x1: number` | Dato de estado/configuración `x1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y1: number` | Dato de estado/configuración `y1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `x2: number` | Dato de estado/configuración `x2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y2: number` | Dato de estado/configuración `y2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stroke: MonoColor` | Dato de estado/configuración `stroke`; declaración estructural, interpretación específica pendiente de ampliar. |
| `strokeWidth: number` | Dato de estado/configuración `strokeWidth`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TriangleObject`

`web:src/features/oledEditor/types.ts:86`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "triangle"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `points: [Point, Point, Point]` | Dato de estado/configuración `points`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fill: MonoColor \| null` | Dato de estado/configuración `fill`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stroke: MonoColor \| null` | Dato de estado/configuración `stroke`; declaración estructural, interpretación específica pendiente de ampliar. |
| `strokeWidth: number` | Dato de estado/configuración `strokeWidth`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PolygonObject`

`web:src/features/oledEditor/types.ts:94`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "polygon"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `points: Point[]` | Dato de estado/configuración `points`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fill: MonoColor \| null` | Dato de estado/configuración `fill`; declaración estructural, interpretación específica pendiente de ampliar. |
| `stroke: MonoColor \| null` | Dato de estado/configuración `stroke`; declaración estructural, interpretación específica pendiente de ampliar. |
| `strokeWidth: number` | Dato de estado/configuración `strokeWidth`; declaración estructural, interpretación específica pendiente de ampliar. |

### `TextObject`

`web:src/features/oledEditor/types.ts:102`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "text"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `text: string` | Dato de estado/configuración `text`; declaración estructural, interpretación específica pendiente de ampliar. |
| `font: OledFont` | Dato de estado/configuración `font`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fill: MonoColor` | Dato de estado/configuración `fill`; declaración estructural, interpretación específica pendiente de ampliar. |
| `align?: "left" \| "center" \| "right"` | Dato de estado/configuración `align`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ImageObject`

`web:src/features/oledEditor/types.ts:112`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "image"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `width: number` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: number` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `assetId: string` | Dato de estado/configuración `assetId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bitOrder: BitOrder` | Dato de estado/configuración `bitOrder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `alphaChannel?: boolean` | Dato de estado/configuración `alphaChannel`; declaración estructural, interpretación específica pendiente de ampliar. |

### `BitmapObject`

`web:src/features/oledEditor/types.ts:123`

| Atributo / firma | Propósito |
| --- | --- |
| `kind: "bitmap"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `width: number` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: number` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pixelData: number[]` | Dato de estado/configuración `pixelData`; declaración estructural, interpretación específica pendiente de ampliar. |
| `colorMode: "monochrome"` | Dato de estado/configuración `colorMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `alphaChannel?: boolean` | Dato de estado/configuración `alphaChannel`; declaración estructural, interpretación específica pendiente de ampliar. |

### `CanvasObject`

`web:src/features/oledEditor/types.ts:134`

Tipo: `\| RectangleObject \| CircleObject \| LineObject \| TriangleObject \| PolygonObject \| TextObject \| ImageObject \| BitmapObject`.

### `ImageAsset`

`web:src/features/oledEditor/types.ts:144`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `name: string` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `width: number` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: number` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bytes: number[]` | Dato de estado/configuración `bytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bitOrder: BitOrder` | Dato de estado/configuración `bitOrder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `source: "shared" \| "upload" \| "generated"` | Dato de estado/configuración `source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `draft?: boolean` | Dato de estado/configuración `draft`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EditorDocument`

`web:src/features/oledEditor/types.ts:155`

| Atributo / firma | Propósito |
| --- | --- |
| `screen: ScreenSpec` | Dato de estado/configuración `screen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `objectsById: Record<string, CanvasObject>` | Dato de estado/configuración `objectsById`; declaración estructural, interpretación específica pendiente de ampliar. |
| `zOrder: string[]` | Dato de estado/configuración `zOrder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `assetsById: Record<string, ImageAsset>` | Dato de estado/configuración `assetsById`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SelectionState`

`web:src/features/oledEditor/types.ts:162`

| Atributo / firma | Propósito |
| --- | --- |
| `selectedIds: string[]` | Dato de estado/configuración `selectedIds`; declaración estructural, interpretación específica pendiente de ampliar. |
| `vertexEdit?: \| { objectId: string; kind: "triangle"; activeVertex?: 0 \| 1 \| 2 } \| { objectId: string; kind: "polygon"; activeVertex?: number } \| { objectId: string; kind: "line"; activeVertex?: 0 \| 1 }` | Dato de estado/configuración `vertexEdit`; declaración estructural, interpretación específica pendiente de ampliar. |

### `CodegenSettings`

`web:src/features/oledEditor/types.ts:170`

| Atributo / firma | Propósito |
| --- | --- |
| `wrapperFunction: boolean` | Dato de estado/configuración `wrapperFunction`; declaración estructural, interpretación específica pendiente de ampliar. |
| `includeComments: boolean` | Dato de estado/configuración `includeComments`; declaración estructural, interpretación específica pendiente de ampliar. |
| `declareImages: boolean` | Dato de estado/configuración `declareImages`; declaración estructural, interpretación específica pendiente de ampliar. |
| `declareVariables: boolean` | Dato de estado/configuración `declareVariables`; declaración estructural, interpretación específica pendiente de ampliar. |
| `clearDisplay: boolean` | Dato de estado/configuración `clearDisplay`; declaración estructural, interpretación específica pendiente de ampliar. |
| `prefix: string` | Dato de estado/configuración `prefix`; declaración estructural, interpretación específica pendiente de ampliar. |
| `suffix: string` | Dato de estado/configuración `suffix`; declaración estructural, interpretación específica pendiente de ampliar. |
| `mode: "uner-commands" \| "json-ir"` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |

### `HistoryEditorAction`

`web:src/features/oledEditor/types.ts:181`

| Atributo / firma | Propósito |
| --- | --- |
| `type: "document/replace"` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `document: EditorDocument` | Dato de estado/configuración `document`; declaración estructural, interpretación específica pendiente de ampliar. |

### `HistoryEntry`

`web:src/features/oledEditor/types.ts:186`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `forward: HistoryEditorAction[]` | Dato de estado/configuración `forward`; declaración estructural, interpretación específica pendiente de ampliar. |
| `inverse: HistoryEditorAction[]` | Dato de estado/configuración `inverse`; declaración estructural, interpretación específica pendiente de ampliar. |
| `timestamp: number` | Dato de estado/configuración `timestamp`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EditorState`

`web:src/features/oledEditor/types.ts:193`

| Atributo / firma | Propósito |
| --- | --- |
| `document: EditorDocument` | Dato de estado/configuración `document`; declaración estructural, interpretación específica pendiente de ampliar. |
| `selection: SelectionState` | Dato de estado/configuración `selection`; declaración estructural, interpretación específica pendiente de ampliar. |
| `activeTool: ToolId` | Dato de estado/configuración `activeTool`; declaración estructural, interpretación específica pendiente de ampliar. |
| `hoveredId: string \| null` | Dato de estado/configuración `hoveredId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `viewport: { zoom: number; panX: number; panY: number; }` | Dato de estado/configuración `viewport`; declaración estructural, interpretación específica pendiente de ampliar. |
| `history: { past: HistoryEntry[]; future: HistoryEntry[]; transactionLabel?: string; stagedForward: HistoryEditorAction[]; stagedInverse: HistoryEditorAction[]; }` | Dato de estado/configuración `history`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ui: { rightMouseMode: "none" \| "erase" \| "remove-vertex"; textEditingId?: string; imagePanelOpen: boolean; selectedAssetId?: string; codeSettings: CodegenSettings; }` | Dato de estado/configuración `ui`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DrawOp`

`web:src/features/oledEditor/types.ts:219`

| Atributo / firma | Propósito |
| --- | --- |
| `id: string` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `sourceObjectId: string` | Dato de estado/configuración `sourceObjectId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `kind: "rect" \| "circle" \| "line" \| "polygon" \| "text" \| "bitmap"` | Dato de estado/configuración `kind`; declaración estructural, interpretación específica pendiente de ampliar. |
| `args: Record<string, unknown>` | Dato de estado/configuración `args`; declaración estructural, interpretación específica pendiente de ampliar. |

### `GeneratedCodeResult`

`web:src/features/oledEditor/types.ts:226`

| Atributo / firma | Propósito |
| --- | --- |
| `ir: DrawOp[]` | Dato de estado/configuración `ir`; declaración estructural, interpretación específica pendiente de ampliar. |
| `text: string` | Dato de estado/configuración `text`; declaración estructural, interpretación específica pendiente de ampliar. |
| `lineMap: Record<string, { startLine: number; endLine: number }>` | Dato de estado/configuración `lineMap`; declaración estructural, interpretación específica pendiente de ampliar. |

### `EditorReducerAction`

`web:src/features/oledEditor/useOledEditorState.ts:16`

Tipo: `\| { type: "tool/set"; tool: ToolId } \| { type: "selection/set"; selection: SelectionState } \| { type: "hover/set"; id: string \| null } \| { type: "viewport/set"; patch: Partial<EditorState["viewport"]> } \| { type: "document/set"; document: EditorDocument } \| { type: "screen/set"; patch: Partial<EditorDocument["screen"]> } \| { type: "asset/select"; assetId?: string } \| { type: "ui/image-panel"; open: boolean } \| { type: "ui/right-mouse-mode"; value: EditorState["ui"]["rightMouseMode"] } \| { type: "ui/code-settings"; patch: Partial<CodegenSettings> } \| { type: "history/push"; entry: HistoryEntry; selection?: SelectionState } \| { type: "history/undo" } \| { type: "history/redo" }`.

### `UseOledEditorStateArgs`

`web:src/features/oledEditor/useOledEditorState.ts:31`

| Atributo / firma | Propósito |
| --- | --- |
| `initialDocument?: EditorDocument` | Dato de estado/configuración `initialDocument`; declaración estructural, interpretación específica pendiente de ampliar. |
| `onDocumentChange?: (document: EditorDocument) => void` | Operación on Document Change de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. |
| `onCodeChange?: (result: GeneratedCodeResult) => void` | Operación on Code Change de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. |
| `onDirtyChange?: (dirty: boolean) => void` | Operación on Dirty Change de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-savedoledscreensprovider"></a>`SavedOledScreensProvider({   children, }: { children: ReactNode; }): inferred-by-TypeScript` | Coordina `useEffect`, `useMemo`, `useState` para Modelo y edición de documentos OLED. | `web:src/contexts/SavedOledScreensContext.tsx:44`; structural |
| <a id="fn-applyhistoryaction"></a>`applyHistoryAction(document: EditorDocument, action: HistoryEditorAction): inferred-by-TypeScript` | Adapta la llamada a `cloneDocument` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:296`; structural |
| <a id="fn-bitmapbytestopixels"></a>`bitmapBytesToPixels(bytes: readonly number[], width: number, height: number, bitOrder: BitOrder): inferred-by-TypeScript` | Coordina `Math.ceil`, `buildEmptyPixelData` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:389`; structural |
| <a id="fn-bitmapdeclarationlines"></a>`bitmapDeclarationLines(symbol: string, bytes: number[]): inferred-by-TypeScript` | Coordina `bytes .slice`, `bytes .slice(index, index + 12) .map`, `bytes .slice(index, index + 12) .map((value) => `0x${value.toString(16).padStart(2, "0")}`) .join` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:464`; structural |
| <a id="fn-builddrawops"></a>`buildDrawOps(document: EditorDocument): DrawOp[]` | Coordina `buildEditorAssetIndex`, `orderedRenderableObjects`, `orderedRenderableObjects(document).flatMap` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:53`; structural |
| <a id="fn-buildeditorassetindex"></a>`buildEditorAssetIndex(customAssets: Record<string, ImageAsset>): Record<string, EditorBitmapAssetDefinition>` | Coordina `Object.fromEntries`, `Object.values`, `Object.values(customAssets).map` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/assets.ts:307`; structural |
| <a id="fn-buildemptypixeldata"></a>`buildEmptyPixelData(width: number, height: number): inferred-by-TypeScript` | Adapta la llamada a `Array.from` con el contexto de esta integración. | `web:src/features/oledEditor/document.ts:385`; structural |
| <a id="fn-buildjsoniroutput"></a>`buildJsonIrOutput(document: EditorDocument, settings: CodegenSettings, ir: DrawOp[]): GeneratedCodeResult` | Coordina `JSON.stringify`, `JSON.stringify(document.screen, null, 2).replace`, `ir.forEach` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:179`; structural |
| <a id="fn-buildoledcommands"></a>`buildOledCommands(document: EditorDocument): OledCommand[]` | Coordina `buildEditorAssetIndex`, `commands.push`, `orderedRenderableObjects` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:24`; structural |
| <a id="fn-buildtrianglecommands"></a>`buildTriangleCommands(object: Extract<CanvasObject, { kind: "triangle" }>): inferred-by-TypeScript` | Coordina `commands.push`, `monoToColor` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:612`; structural |
| <a id="fn-buildunercommandsoutput"></a>`buildUnerCommandsOutput(document: EditorDocument, settings: CodegenSettings, ir: DrawOp[]): GeneratedCodeResult` | Coordina `JSON.stringify`, `buildEditorAssetIndex`, `collectLocalBitmapDeclarations` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:225`; structural |
| <a id="fn-centerobjectat"></a>`centerObjectAt(point: Point, width: number, height: number): Point` | Coordina `Math.round`, `clamp` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:72`; structural |
| <a id="fn-clamp"></a>`clamp(value: number, min: number, max: number): inferred-by-TypeScript` | Coordina `Math.max`, `Math.min` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:27`; structural |
| <a id="fn-clamppointtoscreen"></a>`clampPointToScreen(point: Point): Point` | Adapta la llamada a `clamp` con el contexto de esta integración. | `web:src/features/oledEditor/geometry.ts:31`; structural |
| <a id="fn-cloneasset"></a>`cloneAsset(asset: ImageAsset): ImageAsset` | Operación clone Asset de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/document.ts:103`; inferred-name |
| <a id="fn-clonedocument"></a>`cloneDocument(document: EditorDocument): EditorDocument` | Coordina `Object.entries`, `Object.entries(document.assetsById).map`, `Object.entries(document.objectsById).map` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:54`; structural |
| <a id="fn-cloneobject"></a>`cloneObject(object: CanvasObject): CanvasObject` | Adapta la llamada a `object.points.map` con el contexto de esta integración. | `web:src/features/oledEditor/document.ts:73`; structural |
| <a id="fn-collectlocalbitmapdeclarations"></a>`collectLocalBitmapDeclarations(document: EditorDocument): inferred-by-TypeScript` | Coordina `orderedRenderableObjects`, `orderedRenderableObjects(document).flatMap` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:447`; structural |
| <a id="fn-createbitmapobject"></a>`createBitmapObject(document: EditorDocument, point: Point, width: inferred-by-TypeScript, height: inferred-by-TypeScript): BitmapObject` | Coordina `buildEmptyPixelData`, `nextObjectId`, `nextZIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:362`; structural |
| <a id="fn-createcircle"></a>`createCircle(document: EditorDocument, center: Point, radius: number): CircleObject` | Coordina `nextObjectId`, `nextZIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:242`; structural |
| <a id="fn-createemptydocument"></a>`createEmptyDocument(partial?: Partial<EditorDocument>): EditorDocument` | Operación create Empty Document de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/document.ts:40`; inferred-name |
| <a id="fn-createimageobject"></a>`createImageObject(document: EditorDocument, asset: ImageAsset, point: Point): ImageObject` | Coordina `nextObjectId`, `nextZIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:340`; structural |
| <a id="fn-createinitialstate"></a>`createInitialState(initialDocument?: EditorDocument): EditorState` | Coordina `cloneDocument`, `createEmptyDocument` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/useOledEditorState.ts:146`; structural |
| <a id="fn-createline"></a>`createLine(document: EditorDocument, start: Point, end: Point): LineObject` | Coordina `nextObjectId`, `nextZIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:263`; structural |
| <a id="fn-createpolygon"></a>`createPolygon(document: EditorDocument, points: Point[]): PolygonObject` | Coordina `nextObjectId`, `nextZIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:302`; structural |
| <a id="fn-createrectangle"></a>`createRectangle(document: EditorDocument, bounds: { x: number; y: number; width: number; height: number }): RectangleObject` | Coordina `nextObjectId`, `nextZIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:221`; structural |
| <a id="fn-createsavedscreenid"></a>`createSavedScreenId(): inferred-by-TypeScript` | Coordina `Date.now`, `Math.random`, `Math.random().toString` para Modelo y edición de documentos OLED. | `web:src/contexts/SavedOledScreensContext.tsx:163`; structural |
| <a id="fn-createtext"></a>`createText(document: EditorDocument, point: Point): TextObject` | Coordina `nextObjectId`, `nextZIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:320`; structural |
| <a id="fn-createtriangle"></a>`createTriangle(document: EditorDocument, points: [Point, Point, Point]): TriangleObject` | Coordina `nextObjectId`, `nextZIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:284`; structural |
| <a id="fn-createuploadedasset"></a>`createUploadedAsset(name: string, width: number, height: number, bytes: number[], bitOrder: BitOrder): ImageAsset` | Adapta la llamada a `nextAssetId` con el contexto de esta integración. | `web:src/features/oledEditor/document.ts:433`; structural |
| <a id="fn-distancetosegment"></a>`distanceToSegment(point: Point, start: Point, end: Point): inferred-by-TypeScript` | Coordina `Math.hypot`, `clamp` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:162`; structural |
| <a id="fn-editorreducer"></a>`editorReducer(state: EditorState, action: EditorReducerAction): EditorState` | Coordina `applyHistoryAction`, `cloneDocument`, `state.history.past.slice` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/useOledEditorState.ts:172`; structural |
| <a id="fn-fillcirclecommands"></a>`fillCircleCommands(color: MonoColor \| null, x: number, y: number, radius: number): inferred-by-TypeScript` | Adapta la llamada a `monoToColor` con el contexto de esta integración. | `web:src/features/oledEditor/codegen.ts:580`; structural |
| <a id="fn-fillrectcommands"></a>`fillRectCommands(color: MonoColor \| null, x: number, y: number, width: number, height: number): inferred-by-TypeScript` | Adapta la llamada a `monoToColor` con el contexto de esta integración. | `web:src/features/oledEditor/codegen.ts:546`; structural |
| <a id="fn-findlineendpointhandle"></a>`findLineEndpointHandle(object: LineObject, point: Point, tolerance: inferred-by-TypeScript): 0 \| 1 \| null` | Adapta la llamada a `Math.hypot` con el contexto de esta integración. | `web:src/features/oledEditor/geometry.ts:243`; structural |
| <a id="fn-findresizehandle"></a>`findResizeHandle(bounds: BoundingBox, point: Point, tolerance: inferred-by-TypeScript): HandlePosition \| null` | Coordina `Math.hypot`, `Object.entries`, `getResizeHandles` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:279`; structural |
| <a id="fn-findvertexhandle"></a>`findVertexHandle(points: readonly Point[], point: Point, tolerance: inferred-by-TypeScript): number \| null` | Adapta la llamada a `Math.hypot` con el contexto de esta integración. | `web:src/features/oledEditor/geometry.ts:229`; structural |
| <a id="fn-generateeditorcode"></a>`generateEditorCode(document: EditorDocument, settings: CodegenSettings): GeneratedCodeResult` | Coordina `buildDrawOps`, `buildJsonIrOutput`, `buildUnerCommandsOutput` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:43`; structural |
| <a id="fn-getobjectbounds"></a>`getObjectBounds(object: CanvasObject): BoundingBox` | Coordina `Math.abs`, `Math.max`, `Math.min` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:83`; structural |
| <a id="fn-getresizehandles"></a>`getResizeHandles(bounds: BoundingBox): Record<HandlePosition, Point>` | Operación get Resize Handles de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/geometry.ts:259`; inferred-name |
| <a id="fn-getselectionbounds"></a>`getSelectionBounds(objects: CanvasObject[]): BoundingBox \| null` | Coordina `Math.max`, `Math.min`, `bounds.map` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:134`; structural |
| <a id="fn-hittestobject"></a>`hitTestObject(object: CanvasObject, point: Point): inferred-by-TypeScript` | Coordina `Math.hypot`, `Math.max`, `distanceToSegment` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:201`; structural |
| <a id="fn-insertobject"></a>`insertObject(document: EditorDocument, object: CanvasObject): EditorDocument` | Coordina `cloneDocument`, `cloneObject`, `next.zOrder.filter` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:124`; structural |
| <a id="fn-ispersistedsavedoledscreen"></a>`isPersistedSavedOledScreen(value: unknown): value is PersistedSavedOledScreen` | Adapta la llamada a `Boolean` con el contexto de esta integración. | `web:src/contexts/SavedOledScreensContext.tsx:169`; structural |
| <a id="fn-listeditorassets"></a>`listEditorAssets(customAssets: Record<string, ImageAsset>): EditorBitmapAssetDefinition[]` | Coordina `Object.values`, `Object.values(buildEditorAssetIndex(customAssets)).sort`, `buildEditorAssetIndex` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/assets.ts:331`; structural |
| <a id="fn-loadpersistedscreens"></a>`loadPersistedScreens(): PersistedSavedOledScreen[]` | Coordina `Array.isArray`, `JSON.parse`, `parsed .filter` para Modelo y edición de documentos OLED. | `web:src/contexts/SavedOledScreensContext.tsx:136`; structural |
| <a id="fn-monotocolor"></a>`monoToColor(color: MonoColor): inferred-by-TypeScript` | Operación mono To Color de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/codegen.ts:542`; inferred-name |
| <a id="fn-moveobjectdown"></a>`moveObjectDown(document: EditorDocument, id: string): inferred-by-TypeScript` | Coordina `cloneDocument`, `next.zOrder.indexOf`, `syncZIndices` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:195`; structural |
| <a id="fn-moveobjectup"></a>`moveObjectUp(document: EditorDocument, id: string): inferred-by-TypeScript` | Coordina `cloneDocument`, `next.zOrder.indexOf`, `syncZIndices` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:178`; structural |
| <a id="fn-nextassetid"></a>`nextAssetId(prefix: inferred-by-TypeScript): inferred-by-TypeScript` | Operación next Asset Id de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/document.ts:115`; inferred-name |
| <a id="fn-nextobjectid"></a>`nextObjectId(prefix: string): inferred-by-TypeScript` | Operación next Object Id de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/document.ts:110`; inferred-name |
| <a id="fn-nextzindex"></a>`nextZIndex(document: EditorDocument): inferred-by-TypeScript` | Operación next ZIndex de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/document.ts:120`; inferred-name |
| <a id="fn-normalizerect"></a>`normalizeRect(start: Point, end: Point): BoundingBox` | Coordina `Math.abs`, `Math.max`, `Math.min` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:38`; structural |
| <a id="fn-objecttocommandlines"></a>`objectToCommandLines(object: CanvasObject, assetIndex: Record<string, ImageAsset>, declarations: LocalBitmapDeclaration[], includeComments: boolean): inferred-by-TypeScript` | Coordina `commands.forEach`, `lines.push`, `objectToCommandsWithLocalRefs` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:376`; structural |
| <a id="fn-objecttocommands"></a>`objectToCommands(object: CanvasObject, assetIndex: Record<string, ImageAsset>): OledCommand[]` | Coordina `buildTriangleCommands`, `fillCircleCommands`, `fillRectCommands` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:299`; structural |
| <a id="fn-objecttocommandswithlocalrefs"></a>`objectToCommandsWithLocalRefs(object: CanvasObject, assetIndex: Record<string, ImageAsset>, declarations: LocalBitmapDeclaration[]): OledCommand[]` | Coordina `declarations.find`, `objectToCommands`, `sanitizeSymbol` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:396`; structural |
| <a id="fn-orderedrenderableobjects"></a>`orderedRenderableObjects(document: EditorDocument): inferred-by-TypeScript` | Coordina `document.zOrder .map`, `document.zOrder .map((id) => document.objectsById[id]) .filter`, `document.zOrder .map((id) => document.objectsById[id]) .filter((object): object is CanvasObject => Boolean(object)) .filter` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:292`; structural |
| <a id="fn-patchobject"></a>`patchObject(document: EditorDocument, id: string, patch: Partial<T>): EditorDocument` | Adapta la llamada a `cloneDocument` con el contexto de esta integración. | `web:src/features/oledEditor/document.ts:135`; structural |
| <a id="fn-pixelstobitmapbytes"></a>`pixelsToBitmapBytes(pixels: readonly number[], width: number, height: number, bitOrder: BitOrder): inferred-by-TypeScript` | Coordina `Array.from`, `Math.ceil` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:409`; structural |
| <a id="fn-pointinbounds"></a>`pointInBounds(point: Point, bounds: BoundingBox, padding: inferred-by-TypeScript): inferred-by-TypeScript` | Operación point In Bounds de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/geometry.ts:153`; inferred-name |
| <a id="fn-pointinpolygon"></a>`pointInPolygon(point: Point, vertices: readonly Point[]): inferred-by-TypeScript` | Operación point In Polygon de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/geometry.ts:181`; inferred-name |
| <a id="fn-pointsbounds"></a>`pointsBounds(points: readonly Point[]): BoundingBox` | Coordina `Math.max`, `Math.min`, `points.map` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:451`; structural |
| <a id="fn-registerasset"></a>`registerAsset(document: EditorDocument, asset: ImageAsset): EditorDocument` | Coordina `cloneAsset`, `cloneDocument` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:212`; structural |
| <a id="fn-removeobjects"></a>`removeObjects(document: EditorDocument, ids: string[]): EditorDocument` | Coordina `cloneDocument`, `ids.forEach`, `next.zOrder.filter` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:163`; structural |
| <a id="fn-replaceobject"></a>`replaceObject(document: EditorDocument, object: CanvasObject): EditorDocument` | Coordina `cloneDocument`, `cloneObject` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/document.ts:154`; structural |
| <a id="fn-resizebitmapfrombounds"></a>`resizeBitmapFromBounds(object: BitmapObject, bounds: BoundingBox): BitmapObject` | Coordina `Array.from`, `Math.max`, `Math.round` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:382`; structural |
| <a id="fn-resizebounds"></a>`resizeBounds(original: BoundingBox, handle: HandlePosition, dx: number, dy: number): BoundingBox` | Coordina `clamp`, `handle.includes` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:412`; structural |
| <a id="fn-resizecirclefrombounds"></a>`resizeCircleFromBounds(object: CircleObject, bounds: BoundingBox): CircleObject` | Coordina `Math.max`, `Math.round` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:369`; structural |
| <a id="fn-resizeobjectfrombounds"></a>`resizeObjectFromBounds(object: T, bounds: BoundingBox): T` | Coordina `Math.max`, `Math.round`, `clamp` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:356`; structural |
| <a id="fn-samedocument"></a>`sameDocument(left: EditorDocument, right: EditorDocument): inferred-by-TypeScript` | Adapta la llamada a `JSON.stringify` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:304`; structural |
| <a id="fn-sanitizesymbol"></a>`sanitizeSymbol(input: string): inferred-by-TypeScript` | Coordina `input .trim`, `input .trim() .toLowerCase`, `input .trim() .toLowerCase() .replace` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:649`; structural |
| <a id="fn-snapline"></a>`snapLine(start: Point, end: Point, enabled: boolean): Point` | Coordina `Math.abs`, `Math.max`, `Math.sign` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/geometry.ts:47`; structural |
| <a id="fn-stringifycolor"></a>`stringifyColor(color: SSD1306Color): inferred-by-TypeScript` | Operación stringify Color de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/codegen.ts:530`; inferred-name |
| <a id="fn-stringifycommand"></a>`stringifyCommand(command: OledCommand): inferred-by-TypeScript` | Coordina `JSON.stringify`, `String`, `stringifyColor` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/codegen.ts:479`; structural |
| <a id="fn-strokecirclecommands"></a>`strokeCircleCommands(color: MonoColor \| null, x: number, y: number, radius: number): inferred-by-TypeScript` | Adapta la llamada a `monoToColor` con el contexto de esta integración. | `web:src/features/oledEditor/codegen.ts:596`; structural |
| <a id="fn-strokerectcommands"></a>`strokeRectCommands(color: MonoColor \| null, x: number, y: number, width: number, height: number): inferred-by-TypeScript` | Adapta la llamada a `monoToColor` con el contexto de esta integración. | `web:src/features/oledEditor/codegen.ts:563`; structural |
| <a id="fn-synczindices"></a>`syncZIndices(document: EditorDocument): inferred-by-TypeScript` | Adapta la llamada a `document.zOrder.forEach` con el contexto de esta integración. | `web:src/features/oledEditor/document.ts:452`; structural |
| <a id="fn-translateobject"></a>`translateObject(object: T, dx: number, dy: number): T` | Adapta la llamada a `object.points.map` con el contexto de esta integración. | `web:src/features/oledEditor/geometry.ts:295`; structural |
| <a id="fn-updatelineendpoint"></a>`updateLineEndpoint(object: LineObject, endpoint: 0 \| 1, point: Point): LineObject` | Operación update Line Endpoint de Modelo y edición de documentos OLED; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/geometry.ts:346`; inferred-name |
| <a id="fn-updatevertexpoint"></a>`updateVertexPoint(object: T, index: number, point: Point): T` | Adapta la llamada a `object.points.map` con el contexto de esta integración. | `web:src/features/oledEditor/geometry.ts:332`; structural |
| <a id="fn-useolededitorstate"></a>`useOledEditorState({   initialDocument,   onDocumentChange,   onCodeChange,   onDirtyChange, }: UseOledEditorStateArgs): inferred-by-TypeScript` | Coordina `Date.now`, `cloneDocument`, `dispatch` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/useOledEditorState.ts:38`; structural |
| <a id="fn-usesavedoledscreens"></a>`useSavedOledScreens(): inferred-by-TypeScript` | Adapta la llamada a `useContext` con el contexto de esta integración. | `web:src/contexts/SavedOledScreensContext.tsx:124`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-savedoledscreensprovider.deletescreen"></a>`SavedOledScreensProvider.deleteScreen(id: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `setPersistedScreens` con el contexto de esta integración. | `web:src/contexts/SavedOledScreensContext.tsx:105`; structural |
| <a id="fn-savedoledscreensprovider.getscreen"></a>`SavedOledScreensProvider.getScreen(id: inferred-by-TypeScript): inferred-by-TypeScript` | Adapta la llamada a `savedScreens.find` con el contexto de esta integración. | `web:src/contexts/SavedOledScreensContext.tsx:110`; structural |
| <a id="fn-savedoledscreensprovider.savescreen"></a>`SavedOledScreensProvider.saveScreen({ id, document }: inferred-by-TypeScript): inferred-by-TypeScript` | Coordina `Date.now`, `createSavedScreenId`, `document.screen.title.trim` para Modelo y edición de documentos OLED. | `web:src/contexts/SavedOledScreensContext.tsx:79`; structural |
| <a id="fn-useolededitorstate.commitdocument"></a>`useOledEditorState.commitDocument(label: string, previousDocument: EditorDocument, nextDocument: EditorDocument, selection?: SelectionState): inferred-by-TypeScript` | Coordina `Date.now`, `cloneDocument`, `dispatch` para Modelo y edición de documentos OLED. | `web:src/features/oledEditor/useOledEditorState.ts:103`; structural |
| <a id="fn-useolededitorstate.redo"></a>`useOledEditorState.redo(): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:125`; structural |
| <a id="fn-useolededitorstate.selectasset"></a>`useOledEditorState.selectAsset(assetId?: string): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:131`; structural |
| <a id="fn-useolededitorstate.setdocument"></a>`useOledEditorState.setDocument(document: EditorDocument): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:100`; structural |
| <a id="fn-useolededitorstate.sethoveredid"></a>`useOledEditorState.setHoveredId(id: string \| null): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:94`; structural |
| <a id="fn-useolededitorstate.setimagepanelopen"></a>`useOledEditorState.setImagePanelOpen(open: boolean): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:128`; structural |
| <a id="fn-useolededitorstate.setrightmousemode"></a>`useOledEditorState.setRightMouseMode(value: EditorState["ui"]["rightMouseMode"]): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:134`; structural |
| <a id="fn-useolededitorstate.setselection"></a>`useOledEditorState.setSelection(selection: SelectionState): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:91`; structural |
| <a id="fn-useolededitorstate.settool"></a>`useOledEditorState.setTool(tool: ToolId): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:88`; structural |
| <a id="fn-useolededitorstate.setviewport"></a>`useOledEditorState.setViewport(patch: Partial<EditorState["viewport"]>): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:97`; structural |
| <a id="fn-useolededitorstate.undo"></a>`useOledEditorState.undo(): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:122`; structural |
| <a id="fn-useolededitorstate.updatecodesettings"></a>`useOledEditorState.updateCodeSettings(patch: Partial<CodegenSettings>): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:137`; structural |
| <a id="fn-useolededitorstate.updatescreen"></a>`useOledEditorState.updateScreen(patch: Partial<EditorDocument["screen"]>): inferred-by-TypeScript` | Adapta la llamada a `dispatch` con el contexto de esta integración. | `web:src/features/oledEditor/useOledEditorState.ts:140`; structural |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useState` | `SavedOledScreensProvider` | `web:src/contexts/SavedOledScreensContext.tsx:49` |
| `useEffect` | `SavedOledScreensProvider` | `web:src/contexts/SavedOledScreensContext.tsx:53` |
| `useMemo` | `SavedOledScreensProvider` | `web:src/contexts/SavedOledScreensContext.tsx:64` |
| `useMemo` | `SavedOledScreensProvider` | `web:src/contexts/SavedOledScreensContext.tsx:76` |
| `useContext` | `useSavedOledScreens` | `web:src/contexts/SavedOledScreensContext.tsx:125` |
| `useReducer` | `useOledEditorState` | `web:src/features/oledEditor/useOledEditorState.ts:44` |
| `useMemo` | `useOledEditorState` | `web:src/features/oledEditor/useOledEditorState.ts:50` |
| `useMemo` | `useOledEditorState` | `web:src/features/oledEditor/useOledEditorState.ts:54` |
| `useMemo` | `useOledEditorState` | `web:src/features/oledEditor/useOledEditorState.ts:58` |
| `useMemo` | `useOledEditorState` | `web:src/features/oledEditor/useOledEditorState.ts:62` |
| `useEffect` | `useOledEditorState` | `web:src/features/oledEditor/useOledEditorState.ts:70` |
| `useEffect` | `useOledEditorState` | `web:src/features/oledEditor/useOledEditorState.ts:74` |
| `useEffect` | `useOledEditorState` | `web:src/features/oledEditor/useOledEditorState.ts:78` |
