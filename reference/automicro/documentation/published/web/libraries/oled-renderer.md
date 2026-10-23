# Primitivas OLED, fuentes y representación SVG

ID: `web:module/src/components/OledCommandPreview.tsx` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Transforma comandos de dibujo en la representación de una OLED de 128×64 y rasteriza fuentes/bitmaps disponibles.

Puerto gráfico Web actual: unión OledCommand y nodos SVG. El objetivo del proyecto es portar íntegramente semántica y renderers F4; este consumidor no ejecuta directamente la librería C completa.

<a id="modularity"></a>
## Modularidad observada

Helpers de comandos separan geometría de React; OledCommandPreview implementa SVG, con semántica distinta de un framebuffer binario en algunos casos.

<a id="state"></a>
## Handle, estado y atributos externos

renderCommands crea cursor/color/inverted por render; elements conserva orden de dibujo. Assets/fuentes son tablas estáticas, no snapshot físico.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Builders generan comandos; preview recorre en orden y crea elementos.
- Texto usa fuente bitmap disponible; faltantes de asset producen un marcador SVG.
- Clear restablece estado local; el driver F4 tiene sus propias reglas y buffers.

<a id="interactions"></a>
## Interacciones

- F4 SSD1306 rasteriza en RAM; Web debe conservar orden de bits, clipping, XOR, fuente y rotación si busca equivalencia exacta.
- OledCanvasRasterizer es otro rasterizador usado para enviar dibujos a F4; no es automáticamente el motor del visor en vivo.

<a id="limitations"></a>
## Precauciones y límites

- Color Inverse en paint no ejecuta XOR del píxel previo.
- SVG con línea/arco/trazo no acredita igualdad píxel a píxel con algoritmos C.
- Portar dibujo sin todo el estado del renderer no completa una réplica exacta.
- F4 aplica Inverse de forma diferente según primitiva (AMDOC-F4-012); un port literal debe conservar esa observación hasta decidir una corrección explícita.

## Hallazgos conectados

- [AMDOC-WEB-003: El renderer SVG actual no constituye el port completo de OLED F4](../../../review/FINDINGS.md#amdoc-web-003)
- [AMDOC-F4-012: Inverse cambia de semántica entre píxel y líneas rápidas OLED](../../../review/FINDINGS.md#amdoc-f4-012)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/components/OledBitmapText.tsx` | typescript-ast-static | `5452b3961b2bdc72a619211337f90174de4dd2e40f3764b63b1d4ca3742dc47d` |
| `src/components/OledCommandPreview.tsx` | typescript-ast-static | `6144334d642755d296add0f2bddf6fbe5fd47a1fc777a4c92ff45eca36321d4a` |
| `src/screens/bitmapAssets.ts` | typescript-ast-static | `5a49e864b4e24ec46a7154b537378b7fe45d33d0a2e48a4cd0f1adb9b48f5596` |
| `src/screens/helpers.ts` | typescript-ast-static | `803f276eb35092d94b11c8f0b26011097a199f4cc23fecec35b497839cabf029` |
| `src/screens/oledBitmapFont.ts` | typescript-ast-static | `d6990090664076b8a297f67f419200266e8d2be030f5aa1d21749f1ff78bc06b` |
| `src/screens/types.ts` | typescript-ast-static | `beae5e78fa5e7f7f009a118cad426136731ffbacef1af0a72c14e03c1115bf8c` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `OledBitmapTextProps`

`web:src/components/OledBitmapText.tsx:5`

| Atributo / firma | Propósito |
| --- | --- |
| `x: number` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `y: number` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `text: string` | Dato de estado/configuración `text`; declaración estructural, interpretación específica pendiente de ampliar. |
| `font: OledFont` | Dato de estado/configuración `font`; declaración estructural, interpretación específica pendiente de ampliar. |
| `fill: string` | Dato de estado/configuración `fill`; declaración estructural, interpretación específica pendiente de ampliar. |
| `align?: "left" \| "center" \| "right"` | Dato de estado/configuración `align`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledCommandPreviewProps`

`web:src/components/OledCommandPreview.tsx:13`

| Atributo / firma | Propósito |
| --- | --- |
| `commands: OledCommand[]` | Dato de estado/configuración `commands`; declaración estructural, interpretación específica pendiente de ampliar. |
| `className?: string` | Dato de estado/configuración `className`; declaración estructural, interpretación específica pendiente de ampliar. |

### `RenderState`

`web:src/components/OledCommandPreview.tsx:18`

| Atributo / firma | Propósito |
| --- | --- |
| `cursorX: number` | Dato de estado/configuración `cursorX`; declaración estructural, interpretación específica pendiente de ampliar. |
| `cursorY: number` | Dato de estado/configuración `cursorY`; declaración estructural, interpretación específica pendiente de ampliar. |
| `color: SSD1306Color` | Dato de estado/configuración `color`; declaración estructural, interpretación específica pendiente de ampliar. |
| `inverted: boolean` | Dato de estado/configuración `inverted`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledBitmapName`

`web:src/screens/bitmapAssets.ts:1`

Tipo: `string`.

### `OledBitmapAsset`

`web:src/screens/bitmapAssets.ts:3`

| Atributo / firma | Propósito |
| --- | --- |
| `name: OledBitmapName` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bytes: readonly number[]` | Dato de estado/configuración `bytes`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ScreenCode`

`web:src/screens/types.ts:1`

Tipo: `number`.

### `SSD1306Color`

`web:src/screens/types.ts:3`

| Atributo / firma | Propósito |
| --- | --- |
| `Black: inferred-by-TypeScript` | Dato de estado/configuración `Black`; declaración estructural, interpretación específica pendiente de ampliar. |
| `White: inferred-by-TypeScript` | Dato de estado/configuración `White`; declaración estructural, interpretación específica pendiente de ampliar. |
| `Inverse: inferred-by-TypeScript` | Dato de estado/configuración `Inverse`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledFont`

`web:src/screens/types.ts:9`

Tipo: `"Font7x10" \| "Font11x18"`.

### `OledCommand`

`web:src/screens/types.ts:11`

Tipo: `\| { type: "clear" } \| { type: "fill" } \| { type: "setColor"; color: SSD1306Color } \| { type: "setInverted"; value: boolean } \| { type: "setCursor"; x: number; y: number } \| { type: "drawText"; text: string; font: OledFont } \| { type: "drawTextMax"; text: string; maxChars: number; font: OledFont } \| { type: "drawPixel"; x: number; y: number } \| { type: "drawLine"; x0: number; y0: number; x1: number; y1: number } \| { type: "drawHorizontalLine"; x: number; y: number; length: number } \| { type: "drawVerticalLine"; x: number; y: number; length: number } \| { type: "drawRect"; x: number; y: number; w: number; h: number } \| { type: "fillRect"; x: number; y: number; w: number; h: number } \| { type: "drawTriangle"; x1: number; y1: number; x2: number; y2: number; x3: number; y3: number } \| { type: "drawFillTriangle"; x1: number; y1: number; x2: number; y2: number; x3: number; y3: number } \| { type: "drawCircle"; x: number; y: number; radius: number } \| { type: "fillCircle"; x: number; y: number; radius: number } \| { type: "drawCircleQuads"; x: number; y: number; radius: number; quads: number } \| { type: "drawArc"; x: number; y: number; radius: number; startAngle: number; sweep: number } \| { type: "drawProgressBar"; x: number; y: number; barWidth: number; barHeight: number; progress: number } \| { type: "drawBitmap"; x: number; y: number; width: number; height: number; dataRef: string } \| { type: "drawBitmapMSB"; x: number; y: number; width: number; height: number; dataRef: string } \| { type: "polyline"; vertices: Array<{ x: number; y: number }> }`.

### `NotificationProgressArgs`

`web:src/screens/types.ts:36`

| Atributo / firma | Propósito |
| --- | --- |
| `elapsedTicks?: number` | Dato de estado/configuración `elapsedTicks`; declaración estructural, interpretación específica pendiente de ampliar. |
| `totalTicks?: number` | Dato de estado/configuración `totalTicks`; declaración estructural, interpretación específica pendiente de ampliar. |
| `includeProgressOverlay?: boolean` | Dato de estado/configuración `includeProgressOverlay`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MenuScreenItem`

`web:src/screens/types.ts:42`

| Atributo / firma | Propósito |
| --- | --- |
| `label: string` | Dato de estado/configuración `label`; declaración estructural, interpretación específica pendiente de ampliar. |
| `iconRef?: string` | Dato de estado/configuración `iconRef`; declaración estructural, interpretación específica pendiente de ampliar. |
| `visible?: boolean` | Dato de estado/configuración `visible`; declaración estructural, interpretación específica pendiente de ampliar. |
| `screenCode?: ScreenCode` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-oledbitmaptext"></a>`OledBitmapText({   x,   y,   text,   font,   fill,   align = "left", }: OledBitmapTextProps): inferred-by-TypeScript` | Dibuja glifos bitmap según fuente disponible. | `web:src/components/OledBitmapText.tsx:14`; editorial |
| <a id="fn-oledcommandpreview"></a>`OledCommandPreview({   commands,   className = "", }: OledCommandPreviewProps): inferred-by-TypeScript` | Presenta comandos OLED como SVG con clipping 128×64. | `web:src/components/OledCommandPreview.tsx:29`; editorial |
| <a id="fn-barwidthfrombyte"></a>`barWidthFromByte(speed: number, width: number): number` | Coordina `Math.floor`, `Math.max`, `Math.min` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/helpers.ts:89`; structural |
| <a id="fn-baseliney"></a>`baselineY(baseline: number, font: OledFont): number` | Operación baseline Y de Primitivas OLED, fuentes y representación SVG; semántica fina pendiente de ampliación. | `web:src/screens/helpers.ts:80`; inferred-name |
| <a id="fn-bitmapnode"></a>`bitmapNode(command: Extract<OledCommand, { type: "drawBitmap" \| "drawBitmapMSB" }>, state: RenderState, key: number): inferred-by-TypeScript` | Dibuja asset conocido o marcador de referencia cuando falta. | `web:src/components/OledCommandPreview.tsx:258`; editorial |
| <a id="fn-bitmappath"></a>`bitmapPath(command: Extract<OledCommand, { type: "drawBitmap" \| "drawBitmapMSB" }>, bitmap: readonly number[]): inferred-by-TypeScript` | Convierte bits de un bitmap row-major LSB/MSB a trazos de píxel SVG. | `web:src/components/OledCommandPreview.tsx:307`; editorial |
| <a id="fn-buildbitmaptextpath"></a>`buildBitmapTextPath(text: string, font: OledFont, x: number, y: number, align: "left" \| "center" \| "right"): inferred-by-TypeScript` | Coordina `(text \|\| "").split`, `Math.max`, `lines.forEach` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/oledBitmapFont.ts:240`; structural |
| <a id="fn-bytes"></a>`bytes(hex: string): readonly number[]` | Coordina `hex .trim`, `hex .trim() .split`, `hex .trim() .split(/\s+/) .filter` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/bitmapAssets.ts:8`; structural |
| <a id="fn-clear"></a>`clear(): OledCommand` | Operación clear de Primitivas OLED, fuentes y representación SVG; semántica fina pendiente de ampliación. | `web:src/screens/helpers.ts:20`; inferred-name |
| <a id="fn-clearbox"></a>`clearBox(x: number, y: number, w: number, h: number): OledCommand[]` | Coordina `setBlack`, `setWhite` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/helpers.ts:60`; structural |
| <a id="fn-drawbox"></a>`drawBox(x: number, y: number, w: number, h: number): OledCommand[]` | Adapta la llamada a `setWhite` con el contexto de esta integración. | `web:src/screens/helpers.ts:64`; structural |
| <a id="fn-drawframe"></a>`drawFrame(x: number, y: number, w: number, h: number): OledCommand[]` | Adapta la llamada a `setWhite` con el contexto de esta integración. | `web:src/screens/helpers.ts:68`; structural |
| <a id="fn-drawtext"></a>`drawText(text: string, font: OledFont): OledCommand` | Operación draw Text de Primitivas OLED, fuentes y representación SVG; semántica fina pendiente de ampliación. | `web:src/screens/helpers.ts:44`; inferred-name |
| <a id="fn-drawtextmax"></a>`drawTextMax(text: string, maxChars: number, font: OledFont): OledCommand` | Operación draw Text Max de Primitivas OLED, fuentes y representación SVG; semántica fina pendiente de ampliación. | `web:src/screens/helpers.ts:48`; inferred-name |
| <a id="fn-drawxbm"></a>`drawXbm(x: number, y: number, width: number, height: number, dataRef: string): OledCommand[]` | Adapta la llamada a `setWhite` con el contexto de esta integración. | `web:src/screens/helpers.ts:72`; structural |
| <a id="fn-drawxbmmsb"></a>`drawXbmMsb(x: number, y: number, width: number, height: number, dataRef: string): OledCommand[]` | Adapta la llamada a `setWhite` con el contexto de esta integración. | `web:src/screens/helpers.ts:76`; structural |
| <a id="fn-getglyphrows"></a>`getGlyphRows(font: OledFont, char: string): inferred-by-TypeScript` | Coordina `config.data.slice`, `normalizeGlyphChar`, `safeChar.charCodeAt` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/oledBitmapFont.ts:295`; structural |
| <a id="fn-getoledbitmapasset"></a>`getOledBitmapAsset(name: string): OledBitmapAsset \| undefined` | Operación get Oled Bitmap Asset de Primitivas OLED, fuentes y representación SVG; semántica fina pendiente de ampliación. | `web:src/screens/bitmapAssets.ts:271`; inferred-name |
| <a id="fn-irbarheight"></a>`irBarHeight(value: number, maxHeight: number): number` | Coordina `Math.floor`, `Math.max`, `Math.min` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/helpers.ts:93`; structural |
| <a id="fn-linenode"></a>`lineNode(x1: number, y1: number, x2: number, y2: number, state: RenderState, key: number): inferred-by-TypeScript` | Adapta la llamada a `paint` con el contexto de esta integración. | `web:src/components/OledCommandPreview.tsx:245`; structural |
| <a id="fn-normalizeglyphchar"></a>`normalizeGlyphChar(char: string): inferred-by-TypeScript` | Coordina `Array.from`, `candidate.charCodeAt` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/oledBitmapFont.ts:303`; structural |
| <a id="fn-notificationprogressoverlaycommands"></a>`notificationProgressOverlayCommands(args: NotificationProgressArgs): OledCommand[]` | Coordina `Math.floor`, `Math.max`, `Math.min` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/helpers.ts:97`; structural |
| <a id="fn-paint"></a>`paint(state: RenderState): inferred-by-TypeScript` | Selecciona color SVG; actualmente no reproduce XOR de SSD1306_COLOR Inverse. | `web:src/components/OledCommandPreview.tsx:341`; editorial |
| <a id="fn-parsehexnumbers"></a>`parseHexNumbers(source: string): inferred-by-TypeScript` | Coordina `(source.match(/0x[0-9a-fA-F]+/g) ?? []).map`, `source.match` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/oledBitmapFont.ts:310`; structural |
| <a id="fn-percenttextfrombyte"></a>`percentTextFromByte(speed: number): string` | Coordina `Math.floor`, `Math.max`, `Math.min` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/helpers.ts:84`; structural |
| <a id="fn-rendercommands"></a>`renderCommands(commands: OledCommand[]): ReactNode[]` | Interpreta comandos en orden usando cursor, color e inversión locales. | `web:src/components/OledCommandPreview.tsx:55`; editorial |
| <a id="fn-setblack"></a>`setBlack(): OledCommand` | Adapta la llamada a `setColor` con el contexto de esta integración. | `web:src/screens/helpers.ts:32`; structural |
| <a id="fn-setcolor"></a>`setColor(color: SSD1306Color): OledCommand` | Operación set Color de Primitivas OLED, fuentes y representación SVG; semántica fina pendiente de ampliación. | `web:src/screens/helpers.ts:24`; inferred-name |
| <a id="fn-setcursor"></a>`setCursor(x: number, y: number): OledCommand` | Operación set Cursor de Primitivas OLED, fuentes y representación SVG; semántica fina pendiente de ampliación. | `web:src/screens/helpers.ts:40`; inferred-name |
| <a id="fn-setinverse"></a>`setInverse(): OledCommand` | Adapta la llamada a `setColor` con el contexto de esta integración. | `web:src/screens/helpers.ts:36`; structural |
| <a id="fn-setwhite"></a>`setWhite(): OledCommand` | Adapta la llamada a `setColor` con el contexto de esta integración. | `web:src/screens/helpers.ts:28`; structural |
| <a id="fn-textat"></a>`textAt(x: number, y: number, text: string, font: OledFont): OledCommand[]` | Coordina `drawText`, `setCursor`, `setWhite` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/helpers.ts:52`; structural |
| <a id="fn-textmaxat"></a>`textMaxAt(x: number, y: number, text: string, maxChars: number, font: OledFont): OledCommand[]` | Coordina `drawTextMax`, `setCursor`, `setWhite` para Primitivas OLED, fuentes y representación SVG. | `web:src/screens/helpers.ts:56`; structural |
| <a id="fn-textnode"></a>`textNode(text: string, font: OledFont, state: RenderState, key: number): inferred-by-TypeScript` | Adapta la llamada a `paint` con el contexto de esta integración. | `web:src/components/OledCommandPreview.tsx:232`; structural |
| <a id="fn-u8"></a>`u8(value: number): number` | Operación u8 de Primitivas OLED, fuentes y representación SVG; semántica fina pendiente de ampliación. | `web:src/screens/helpers.ts:16`; inferred-name |
| <a id="fn-withnotificationprogress"></a>`withNotificationProgress(commands: OledCommand[], args: NotificationProgressArgs): OledCommand[]` | Adapta la llamada a `notificationProgressOverlayCommands` con el contexto de esta integración. | `web:src/screens/helpers.ts:113`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |

## Estado y efectos React encontrados

| Hook | Función/closure | Evidencia |
| --- | --- | --- |
| `useMemo` | `OledBitmapText` | `web:src/components/OledBitmapText.tsx:22` |
| `useId` | `OledCommandPreview` | `web:src/components/OledCommandPreview.tsx:33` |
