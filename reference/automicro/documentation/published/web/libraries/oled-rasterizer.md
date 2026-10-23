# Raster binario y protocolo de canvas Web

ID: `web:module/src/features/oledEditor/oledCanvasRasterizer.ts` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Convierte documentos locales a framebuffer page-major LSB y prepara metadata/CRC de canvas.

1024 bytes 128×64 y codificación de operaciones; es distinto del renderer SVG de pantalla en vivo.

<a id="modularity"></a>
## Modularidad observada

Cálculo puro de formato OLED; una implementación independiente puede divergir de primitivas SSD1306 C.

<a id="state"></a>
## Handle, estado y atributos externos

El buffer de salida es copia binaria; preservar su contenido hasta completar el envío de todos los chunks.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Rasterizar documento/asset, calcular CRC y dividir según límites de transferencia.
- El receptor comprueba formato, orden y checksum antes de presentar.

<a id="interactions"></a>
## Interacciones

- ESP reenvasa chunks Web a chunks F4; F4 aplica el framebuffer final.
- Para port exacto del visor conviene unificar la semántica de píxel con el mismo núcleo, conservando dirección del flujo.
- Esquema compartido por operación: published/shared/contracts/oled-canvas-v1.md; actualizar productor, traductor y consumidor al cambiar offsets/chunks/CRC.

<a id="limitations"></a>
## Precauciones y límites

- Compartir formato de buffer no demuestra igualdad de todos los algoritmos de rasterización.

## Hallazgos conectados

- [AMDOC-WEB-003: El renderer SVG actual no constituye el port completo de OLED F4](../../../review/FINDINGS.md#amdoc-web-003)

## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/features/oledEditor/oledCanvasProtocol.ts` | typescript-ast-static | `3bdcfd99c73b29ccf7bf8c75ff94976d44578a8206622e66ffeb58b33f9c6831` |
| `src/features/oledEditor/oledCanvasRasterizer.ts` | typescript-ast-static | `3b64513fc4f099acf026fd8870beebd8cfb4b3adfc244de4480977abc9a51281` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `OledCanvasChunk`

`web:src/features/oledEditor/oledCanvasProtocol.ts:13`

| Atributo / firma | Propósito |
| --- | --- |
| `offset: number` | Dato de estado/configuración `offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bytes: Uint8Array` | Dato de estado/configuración `bytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `dataBase64: string` | Dato de estado/configuración `dataBase64`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledCanvasBeginResponse`

`web:src/features/oledEditor/oledCanvasProtocol.ts:19`

| Atributo / firma | Propósito |
| --- | --- |
| `transferId: number` | Dato de estado/configuración `transferId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `chunkBytes: typeof OLED_CANVAS_WEB_CHUNK_BYTES` | Dato de estado/configuración `chunkBytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `nextOffset: 0` | Dato de estado/configuración `nextOffset`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledCanvasCommitResponse`

`web:src/features/oledEditor/oledCanvasProtocol.ts:25`

| Atributo / firma | Propósito |
| --- | --- |
| `transferId: number` | Dato de estado/configuración `transferId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `state: "rendered"` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bytes: typeof OLED_CANVAS_BYTES` | Dato de estado/configuración `bytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `crc32: string` | Dato de estado/configuración `crc32`; declaración estructural, interpretación específica pendiente de ampliar. |
| `screenCode: typeof OLED_CANVAS_SCREEN_CODE` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledCanvasCancelResponse`

`web:src/features/oledEditor/oledCanvasProtocol.ts:33`

| Atributo / firma | Propósito |
| --- | --- |
| `transferId: number` | Dato de estado/configuración `transferId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `state: "canceled"` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledCanvasRaster`

`web:src/features/oledEditor/oledCanvasRasterizer.ts:11`

| Atributo / firma | Propósito |
| --- | --- |
| `width: typeof OLED_CANVAS_WIDTH` | Dato de estado/configuración `width`; declaración estructural, interpretación específica pendiente de ampliar. |
| `height: typeof OLED_CANVAS_HEIGHT` | Dato de estado/configuración `height`; declaración estructural, interpretación específica pendiente de ampliar. |
| `format: typeof OLED_CANVAS_FORMAT` | Dato de estado/configuración `format`; declaración estructural, interpretación específica pendiente de ampliar. |
| `pixels: Uint8Array` | Dato de estado/configuración `pixels`; declaración estructural, interpretación específica pendiente de ampliar. |
| `framebuffer: Uint8Array` | Imagen binaria retenida durante el ensamblaje/visualización del canvas. |
| `crc32: number` | Dato de estado/configuración `crc32`; declaración estructural, interpretación específica pendiente de ampliar. |
| `crc32Hex: string` | Dato de estado/configuración `crc32Hex`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-buildoledcanvasbeginargs"></a>`buildOledCanvasBeginArgs(raster: OledCanvasRaster): Record<string, unknown>` | Operación build Oled Canvas Begin Args de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasProtocol.ts:38`; inferred-name |
| <a id="fn-buildoledcanvascancelargs"></a>`buildOledCanvasCancelArgs(transferId: number): Record<string, unknown>` | Operación build Oled Canvas Cancel Args de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasProtocol.ts:71`; inferred-name |
| <a id="fn-buildoledcanvaschunkargs"></a>`buildOledCanvasChunkArgs(transferId: number, chunk: OledCanvasChunk): Record<string, unknown>` | Operación build Oled Canvas Chunk Args de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasProtocol.ts:60`; inferred-name |
| <a id="fn-buildoledcanvascommitargs"></a>`buildOledCanvasCommitArgs(transferId: number): Record<string, unknown>` | Operación build Oled Canvas Commit Args de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasProtocol.ts:67`; inferred-name |
| <a id="fn-bytestobase64"></a>`bytesToBase64(bytes: ArrayLike<number>): string` | Operación bytes To Base64 de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasProtocol.ts:144`; inferred-name |
| <a id="fn-colorbit"></a>`colorBit(color: MonoColor): number` | Operación color Bit de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:394`; inferred-name |
| <a id="fn-crc32isohdlc"></a>`crc32IsoHdlc(bytes: ArrayLike<number>): number` | Operación crc32 Iso Hdlc de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:83`; inferred-name |
| <a id="fn-drawasset"></a>`drawAsset(pixels: Uint8Array, asset: ImageAsset, x: number, y: number, width: number, height: number): inferred-by-TypeScript` | Coordina `Math.ceil`, `Math.floor`, `Math.min` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:224`; structural |
| <a id="fn-drawbrush"></a>`drawBrush(pixels: Uint8Array, x: number, y: number, value: number, strokeWidth: number): inferred-by-TypeScript` | Coordina `Math.floor`, `Math.max`, `rounded` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:375`; structural |
| <a id="fn-drawline"></a>`drawLine(pixels: Uint8Array, x0: number, y0: number, x1: number, y1: number, value: number, strokeWidth: number): inferred-by-TypeScript` | Coordina `Math.abs`, `drawBrush` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:350`; structural |
| <a id="fn-drawpixellayer"></a>`drawPixelLayer(pixels: Uint8Array, source: ArrayLike<number>, width: number, height: number, x: number, y: number): inferred-by-TypeScript` | Adapta la llamada a `setPixel` con el contexto de esta integración. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:245`; structural |
| <a id="fn-drawtext"></a>`drawText(pixels: Uint8Array, text: string, font: OledFont, x: number, y: number, align: "left" \| "center" \| "right", value: number): inferred-by-TypeScript` | Coordina `(text \|\| "").split`, `(text \|\| "").split("\n").forEach` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:194`; structural |
| <a id="fn-fillcircle"></a>`fillCircle(pixels: Uint8Array, cx: number, cy: number, radius: number, value: number): inferred-by-TypeScript` | Adapta la llamada a `setPixel` con el contexto de esta integración. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:273`; structural |
| <a id="fn-fillpolygon"></a>`fillPolygon(pixels: Uint8Array, points: Point[], value: number): inferred-by-TypeScript` | Coordina `Math.ceil`, `Math.floor`, `Math.max` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:315`; structural |
| <a id="fn-fillrect"></a>`fillRect(pixels: Uint8Array, x: number, y: number, width: number, height: number, value: number): inferred-by-TypeScript` | Adapta la llamada a `setPixel` con el contexto de esta integración. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:260`; structural |
| <a id="fn-formatcrc32"></a>`formatCrc32(crc32: number): string` | Coordina `(crc32 >>> 0).toString`, `(crc32 >>> 0).toString(16).toUpperCase`, `(crc32 >>> 0).toString(16).toUpperCase().padStart` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:94`; structural |
| <a id="fn-normalizecrc32"></a>`normalizeCrc32(value: unknown): string` | Coordina `/^[0-9A-F]{8}$/.test`, `Number.isInteger`, `formatCrc32` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasProtocol.ts:182`; structural |
| <a id="fn-packpagemajor"></a>`packPageMajor(pixels: ArrayLike<number>): Uint8Array` | Adapta la llamada a `Math.floor` con el contexto de esta integración. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:53`; structural |
| <a id="fn-positivesize"></a>`positiveSize(value: number): number` | Coordina `Math.max`, `rounded` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:396`; structural |
| <a id="fn-rasterizeobject"></a>`rasterizeObject(pixels: Uint8Array, object: CanvasObject, assetsById: Record<string, ImageAsset>): inferred-by-TypeScript` | Coordina `Math.max`, `colorBit`, `drawAsset` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:98`; structural |
| <a id="fn-rasterizeoleddocument"></a>`rasterizeOledDocument(document: EditorDocument): OledCanvasRaster` | Rasteriza el documento local a un framebuffer binario compatible con el formato de canvas. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:22`; editorial |
| <a id="fn-requirerecord"></a>`requireRecord(value: unknown, operation: string): Record<string, unknown>` | Adapta la llamada a `Array.isArray` con el contexto de esta integración. | `web:src/features/oledEditor/oledCanvasProtocol.ts:162`; structural |
| <a id="fn-requiretransferid"></a>`requireTransferId(value: unknown): number` | Coordina `Number`, `Number.isInteger`, `value.trim` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasProtocol.ts:169`; structural |
| <a id="fn-rounded"></a>`rounded(value: number): number` | Coordina `Math.round`, `Number.isFinite` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:395`; structural |
| <a id="fn-setpixel"></a>`setPixel(pixels: Uint8Array, x: number, y: number, value: number): inferred-by-TypeScript` | Operación set Pixel de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:389`; inferred-name |
| <a id="fn-shouldcanceloledcanvasformode"></a>`shouldCancelOledCanvasForMode(isRunning: boolean, mode: number \| null): boolean` | Operación should Cancel Oled Canvas For Mode de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasProtocol.ts:75`; inferred-name |
| <a id="fn-shouldcanceloledcanvasforscreen"></a>`shouldCancelOledCanvasForScreen(isRunning: boolean, screenCode: number \| null \| undefined): boolean` | Operación should Cancel Oled Canvas For Screen de Raster binario y protocolo de canvas Web; semántica fina pendiente de ampliación. | `web:src/features/oledEditor/oledCanvasProtocol.ts:79`; inferred-name |
| <a id="fn-splitoledcanvaschunks"></a>`splitOledCanvasChunks(framebuffer: Uint8Array): OledCanvasChunk[]` | Coordina `bytesToBase64`, `chunks.push`, `framebuffer.slice` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasProtocol.ts:48`; structural |
| <a id="fn-strokecircle"></a>`strokeCircle(pixels: Uint8Array, cx: number, cy: number, radius: number, value: number, strokeWidth: number): inferred-by-TypeScript` | Adapta la llamada a `[ [cx + x, cy + y], [cx + y, cy + x], [cx - y, cy + x], [cx - x, cy + y], [cx - x, cy - y], [cx - y, cy - x], [cx + y, cy - x], [cx + x, cy - y], ].forEach` con el contexto de esta integración. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:290`; structural |
| <a id="fn-strokepolygon"></a>`strokePolygon(pixels: Uint8Array, points: Point[], value: number, strokeWidth: number, close: boolean): inferred-by-TypeScript` | Coordina `Math.max`, `drawLine` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:335`; structural |
| <a id="fn-unpackpagemajor"></a>`unpackPageMajor(framebuffer: ArrayLike<number>): Uint8Array` | Adapta la llamada a `Math.floor` con el contexto de esta integración. | `web:src/features/oledEditor/oledCanvasRasterizer.ts:68`; structural |
| <a id="fn-validateoledcanvasbeginresponse"></a>`validateOledCanvasBeginResponse(data: unknown): OledCanvasBeginResponse` | Coordina `requireRecord`, `requireTransferId` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasProtocol.ts:87`; structural |
| <a id="fn-validateoledcanvascancelresponse"></a>`validateOledCanvasCancelResponse(data: unknown, transferId: number): OledCanvasCancelResponse` | Coordina `requireRecord`, `requireTransferId` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasProtocol.ts:107`; structural |
| <a id="fn-validateoledcanvaschunkresponse"></a>`validateOledCanvasChunkResponse(data: unknown, transferId: number, expectedNextOffset: number): void` | Coordina `requireRecord`, `requireTransferId` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasProtocol.ts:96`; structural |
| <a id="fn-validateoledcanvascommitresponse"></a>`validateOledCanvasCommitResponse(data: unknown, transferId: number, expectedCrc32: number \| string): OledCanvasCommitResponse` | Coordina `normalizeCrc32`, `requireRecord`, `requireTransferId` para Raster binario y protocolo de canvas Web. | `web:src/features/oledEditor/oledCanvasProtocol.ts:118`; structural |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
