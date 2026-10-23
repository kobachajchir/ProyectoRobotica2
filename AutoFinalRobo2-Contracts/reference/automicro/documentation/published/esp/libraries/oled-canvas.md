# Ensamblaje de framebuffer OLED

ID: `esp:module/src/oled_canvas_transfer.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Documentación de fuente; sin tests, compilación, navegador ni validación física.

## Responsabilidad y alcance

Ensambla un framebuffer enviado por Web y construye operaciones de transferencia F4 con orden/CRC.

1024 bytes, chunks Web hasta 512 y F4 hasta 112, IDs y respuesta de 10 bytes.

<a id="modularity"></a>
## Modularidad observada

C sin HTTP ni pantalla física; la exclusión de UART y el envío se coordinan en el bridge.

<a id="state"></a>
## Handle, estado y atributos externos

OledCanvasAssembly posee bytes, transfer_id, next_offset y CRC esperado/calculado. Las respuestas decodificadas son datos del llamador.

<a id="lifecycle"></a>
## Ciclo de vida y contexto

- Begin establece metadata; Append verifica transferencia/offset y duplicados.
- Finalizar ensamblaje comprueba integridad; los chunks se reenvasan a F4.
- COMMIT espera presentación final, no solo recepción de bytes.

<a id="interactions"></a>
## Interacciones

- Web rasteriza un documento local; ESP lo envía a OledCanvas F4.
- Este camino es Web → OLED; no es una captura OLED F4 → navegador.
- Esquema compartido por operación: published/shared/contracts/oled-canvas-v1.md; actualizar productor, traductor y consumidor al cambiar offsets/chunks/CRC.

<a id="limitations"></a>
## Precauciones y límites

- No confundir esta imagen enviada con la reconstrucción de pantalla en vivo.
- Formato page-major LSB y geometría deben coincidir en los tres extremos.

## Hallazgos conectados


## Archivos y evidencia

| Archivo | Análisis | Hash SHA-256 |
| --- | --- | --- |
| `src/oled_canvas_transfer.c` | c-cpp-structural | `b2409949e48b02d17d9fd99eb8760e590e2078463348632849668993ef134b6c` |
| `src/oled_canvas_transfer.h` | c-cpp-structural | `2ae4253f574115faa6e745af5313817894a3f56b63319988a5905ed8d3563565` |

## Tipos, puertos, props y callbacks

En ESP las referencias y tablas son locales al firmware; en Web props/contextos y closures definen la vida de estado. Una firma no demuestra por sí sola registro, invocación ni transporte de sus datos. Las inferencias permanecen explícitas.

### `OledCanvasAssembly`

`esp:src/oled_canvas_transfer.h:57`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t bytes[OLED_CANVAS_BYTES]` | Dato de estado/configuración `bytes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t transfer_id` | Dato de estado/configuración `transfer_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t next_offset` | Dato de estado/configuración `next_offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t expected_crc32` | Dato de estado/configuración `expected_crc32`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t computed_crc32` | Dato de estado/configuración `computed_crc32`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledCanvasAssemblyState state` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledCanvasF4Response`

`esp:src/oled_canvas_transfer.h:66`

| Atributo / firma | Propósito |
| --- | --- |
| `uint8_t operation` | Dato de estado/configuración `operation`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t status` | Último resultado y guardas del módulo; los getters pueden devolver una vista de este almacenamiento. |
| `uint16_t transfer_id` | Dato de estado/configuración `transfer_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t next_offset` | Dato de estado/configuración `next_offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t computed_crc32` | Dato de estado/configuración `computed_crc32`; declaración estructural, interpretación específica pendiente de ampliar. |


## Funciones públicas / componentes / hooks

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-oledcanvasassembly_append"></a>`OledCanvasResult OledCanvasAssembly_Append(OledCanvasAssembly *assembly, uint16_t transfer_id, uint16_t offset, const uint8_t *data, size_t data_len)` | Copia bytes del chunk respetando transferencia y siguiente offset. | `esp:src/oled_canvas_transfer.c:52`; editorial |
| <a id="fn-oledcanvasassembly_begin"></a>`OledCanvasResult OledCanvasAssembly_Begin(OledCanvasAssembly *assembly, uint16_t transfer_id, uint32_t expected_crc32)` | Abre transferencia con ID y CRC anunciado. | `esp:src/oled_canvas_transfer.c:38`; editorial |
| <a id="fn-oledcanvasassembly_commit"></a>`OledCanvasResult OledCanvasAssembly_Commit(OledCanvasAssembly *assembly, uint16_t transfer_id)` | Coordina `OledCanvas_Crc32` para Ensamblaje de framebuffer OLED. | `esp:src/oled_canvas_transfer.c:82`; structural |
| <a id="fn-oledcanvasassembly_init"></a>`void OledCanvasAssembly_Init(OledCanvasAssembly *assembly)` | Reinicia estado de ensamblaje. | `esp:src/oled_canvas_transfer.c:32`; editorial |
| <a id="fn-oledcanvasassembly_scrubframebuffer"></a>`void OledCanvasAssembly_ScrubFramebuffer(OledCanvasAssembly *assembly)` | Coordina `memset` para Ensamblaje de framebuffer OLED. | `esp:src/oled_canvas_transfer.c:100`; structural |
| <a id="fn-oledcanvas_buildbeginpayload"></a>`size_t OledCanvas_BuildBeginPayload(const OledCanvasAssembly *assembly, uint8_t *output, size_t output_capacity)` | Construye Begin Payload dentro de Ensamblaje de framebuffer OLED. | `esp:src/oled_canvas_transfer.c:205`; inferred-name |
| <a id="fn-oledcanvas_buildchunkpayload"></a>`size_t OledCanvas_BuildChunkPayload(const OledCanvasAssembly *assembly, uint16_t offset, uint8_t *output, size_t output_capacity)` | Construye Chunk Payload dentro de Ensamblaje de framebuffer OLED. | `esp:src/oled_canvas_transfer.c:221`; inferred-name |
| <a id="fn-oledcanvas_buildcontrolpayload"></a>`size_t OledCanvas_BuildControlPayload(uint8_t operation, uint16_t transfer_id, uint8_t *output, size_t output_capacity)` | Adapta la llamada a `write_u16_le` con el contexto de esta integración. | `esp:src/oled_canvas_transfer.c:242`; structural |
| <a id="fn-oledcanvas_crc32"></a>`uint32_t OledCanvas_Crc32(const uint8_t *data, size_t len)` | Operación Crc32 de Ensamblaje de framebuffer OLED; semántica fina pendiente de ampliación. | `esp:src/oled_canvas_transfer.c:107`; inferred-name |
| <a id="fn-oledcanvas_decodebase64"></a>`int OledCanvas_DecodeBase64(const char *input, size_t input_len, uint8_t *output, size_t output_capacity, size_t *output_len)` | Coordina `base64_value` para Ensamblaje de framebuffer OLED. | `esp:src/oled_canvas_transfer.c:166`; structural |
| <a id="fn-oledcanvas_f4responsematchesexpected"></a>`int OledCanvas_F4ResponseMatchesExpected(const uint8_t *payload, size_t payload_len, uint8_t expected_operation, uint16_t expected_transfer_id, uint16_t expected_next_offset)` | Coordina `OledCanvas_ParseF4Response` para Ensamblaje de framebuffer OLED. | `esp:src/oled_canvas_transfer.c:268`; structural |
| <a id="fn-oledcanvas_formatcrc32hex"></a>`void OledCanvas_FormatCrc32Hex(uint32_t value, char output[9])` | Operación Format Crc32 Hex de Ensamblaje de framebuffer OLED; semántica fina pendiente de ampliación. | `esp:src/oled_canvas_transfer.c:143`; inferred-name |
| <a id="fn-oledcanvas_parsecrc32hex"></a>`int OledCanvas_ParseCrc32Hex(const char *text, uint32_t *value)` | Decodifica Crc32 Hex dentro de Ensamblaje de framebuffer OLED. | `esp:src/oled_canvas_transfer.c:125`; inferred-name |
| <a id="fn-oledcanvas_parsef4response"></a>`int OledCanvas_ParseF4Response(const uint8_t *payload, size_t payload_len, OledCanvasF4Response *response)` | Decodifica F4 Response dentro de Ensamblaje de framebuffer OLED. | `esp:src/oled_canvas_transfer.c:253`; inferred-name |

## Funciones y callbacks internos

Descripción: editorial, delegación estructural o inferred-name pendiente de profundizar. En TypeScript los retornos no anotados conservan esa condición; no se ejecuta el typechecker.

| Función / firma | Descripción de una línea | Evidencia / origen |
| --- | --- | --- |
| <a id="fn-base64_value"></a>`static int base64_value(char ch)` | Operación base64 value de Ensamblaje de framebuffer OLED; semántica fina pendiente de ampliación. | `esp:src/oled_canvas_transfer.c:156`; inferred-name |
| <a id="fn-read_u16_le"></a>`static uint16_t read_u16_le(const uint8_t *in)` | Operación read u16 le de Ensamblaje de framebuffer OLED; semántica fina pendiente de ampliación. | `esp:src/oled_canvas_transfer.c:19`; inferred-name |
| <a id="fn-read_u32_le"></a>`static uint32_t read_u32_le(const uint8_t *in)` | Operación read u32 le de Ensamblaje de framebuffer OLED; semántica fina pendiente de ampliación. | `esp:src/oled_canvas_transfer.c:24`; inferred-name |
| <a id="fn-write_u16_le"></a>`static void write_u16_le(uint16_t value, uint8_t *out)` | Operación write u16 le de Ensamblaje de framebuffer OLED; semántica fina pendiente de ampliación. | `esp:src/oled_canvas_transfer.c:5`; inferred-name |
| <a id="fn-write_u32_le"></a>`static void write_u32_le(uint32_t value, uint8_t *out)` | Operación write u32 le de Ensamblaje de framebuffer OLED; semántica fina pendiente de ampliación. | `esp:src/oled_canvas_transfer.c:11`; inferred-name |
