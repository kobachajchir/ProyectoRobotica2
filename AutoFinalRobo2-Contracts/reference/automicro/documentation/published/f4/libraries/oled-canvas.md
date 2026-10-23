# Transferencia transaccional OLED Canvas

ID: `f4:module/Core/Inc/oled_canvas.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Recibe un framebuffer por operaciones BEGIN/CHUNK/COMMIT/CANCEL y completa después de su presentación.

Transacción de 1024 bytes 128×64 page-major LSB, correlación por transporte/nodo/transfer_id, orden y CRC32.

<a id="modularity"></a>
## Modularidad observada

Usa paquetes UNER, reglas TEST/pantalla activa y servicios de UI. Es integración de protocolo y presentación, no extensión del core UNER.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

OledCanvasContext_t vive en ScreensState y posee framebuffer, metadatos, respuesta pendiente, estado y callback send_response/send_ctx. No conserva punteros al payload recibido: el ensamblaje pertenece al contexto.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init instala envío; BeginFromMenu prepara pantalla receptora.
- HandlePacket comprueba endpoint, operación y datos y ensambla el framebuffer.
- COMMIT pasa a presentación diferida; Poll resuelve finalización, errores, timeout y reintentos de respuesta.
- HandleLocalEvent puede cancelar la transacción y restaurar navegación.

## Trabajo en tándem

- UNER_App enruta 0x6D; ESP coordina transferencia y Web empaqueta bitmap.
- SSD1306 presenta la imagen; recibir todos los chunks no prueba presentación física.

## Precauciones y límites

- Chunk UART máximo 112 bytes; ensamblaje 3000 ms y presentación 1000 ms según las constantes del header.
- Respuesta de 10 bytes incluye resultado; diferenciar error CRC, orden, contexto y E/S.
- Los reportes de pantalla ordinarios no transportan esta imagen.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/oled_canvas.h` | `stdint.h`, `types/userEvent_type.h`, `uner_core_types.h` | `7ec7d21482c87df73fe27b4a230d90820e0efaedcc3a5947dc424e994c52844f` |
| `Core/Src/oled_canvas.c` | `screens_internal.h`, `oled_canvas.h`, `string.h`, `fonts.h`, `types/menu_types.h`, `oled_utils.h`, `screenWrappers.h`, `screen_codes.h`, `uner_v2.h`, `utils/macros_utils.h` | `10a1d5628c0a2ac35606b1a15e7188c97ae08f3431f3b9d24d1060e99f765157` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `OledCanvasSendResponseFn`

`f4:Core/Inc/oled_canvas.h:47`

Firma: `uint8_t (*OledCanvasSendResponseFn)(uint8_t transport_id, uint8_t destination_node, const uint8_t *payload, uint8_t len, void *ctx)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-oledcanvas_beginfrommenu"></a>`void OledCanvas_BeginFromMenu(void)` | Abre la pantalla local receptora de canvas. | `f4:Core/Inc/oled_canvas.h:55`; editorial |
| <a id="fn-oledcanvas_handlelocalevent"></a>`void OledCanvas_HandleLocalEvent(UserEvent_t event)` | Procesa cancelación/navegación local durante el canvas. | `f4:Core/Inc/oled_canvas.h:57`; editorial |
| <a id="fn-oledcanvas_handlepacket"></a>`void OledCanvas_HandlePacket(const UNER_Packet *packet)` | Procesa una operación de transferencia y sus metadatos. | `f4:Core/Inc/oled_canvas.h:58`; editorial |
| <a id="fn-oledcanvas_init"></a>`void OledCanvas_Init(OledCanvasSendResponseFn send_response, void *ctx)` | Inicializa estado y callback de respuesta de canvas. | `f4:Core/Inc/oled_canvas.h:54`; editorial |
| <a id="fn-oledcanvas_isactivescreen"></a>`uint8_t OledCanvas_IsActiveScreen(void)` | Indica si la pantalla actual acepta la operación canvas. | `f4:Core/Inc/oled_canvas.h:60`; editorial |
| <a id="fn-oledcanvas_poll"></a>`void OledCanvas_Poll(void)` | Avanza presentación y resuelve timeouts/respuestas diferidas. | `f4:Core/Inc/oled_canvas.h:59`; editorial |
| <a id="fn-oledcanvas_renderreadyscreen"></a>`void OledCanvas_RenderReadyScreen(void)` | Dibuja la vista de espera para transferencia. | `f4:Core/Inc/oled_canvas.h:56`; editorial |
| <a id="fn-screens_init_oled_canvas"></a>`void Screens_Init_oled_canvas(void)` | Inicializa el estado canvas del contexto activo de pantallas. | `f4:Core/Src/oled_canvas.c:892`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-oledcanvas_readu16le"></a>`static uint16_t OledCanvas_ReadU16Le(const uint8_t *data)` | Lee U16 Le dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:32`; inferred-name |
| <a id="fn-oledcanvas_readu32le"></a>`static uint32_t OledCanvas_ReadU32Le(const uint8_t *data)` | Lee U32 Le dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:37`; inferred-name |
| <a id="fn-oledcanvas_writeu16le"></a>`static void OledCanvas_WriteU16Le(uint8_t *data, uint16_t value)` | Escribe U16 Le dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:45`; inferred-name |
| <a id="fn-oledcanvas_writeu32le"></a>`static void OledCanvas_WriteU32Le(uint8_t *data, uint32_t value)` | Escribe U32 Le dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:51`; inferred-name |
| <a id="fn-oledcanvas_crc32"></a>`static uint32_t OledCanvas_Crc32(const uint8_t *data, uint16_t len)` | Operación Crc32 de Transferencia transaccional OLED Canvas; semántica fina pendiente de ampliación. | `f4:Core/Src/oled_canvas.c:59`; inferred-name |
| <a id="fn-oledcanvas_guardstatus"></a>`static uint8_t OledCanvas_GuardStatus(uint8_t source_node)` | Coordina `GET_CAR_MODE`, `MenuSys_GetCurrentScreenCode`, `Permission_IsRemoteNodeAuthorizedNow` para Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:75`; structural |
| <a id="fn-oledcanvas_trysendpendingresponse"></a>`static void OledCanvas_TrySendPendingResponse(void)` | Coordina `memset`, `send_response` para Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:96`; structural |
| <a id="fn-oledcanvas_queueresponse"></a>`static void OledCanvas_QueueResponse(uint8_t operation, uint8_t status, uint8_t transport_id, uint8_t destination_node, uint16_t transfer_id, uint16_t next_offset, uint32_t computed_crc32)` | Coordina `OledCanvas_TrySendPendingResponse`, `OledCanvas_WriteU16Le`, `OledCanvas_WriteU32Le` para Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:113`; structural |
| <a id="fn-oledcanvas_rememberresult"></a>`static void OledCanvas_RememberResult(uint8_t status)` | Operación Remember Result de Transferencia transaccional OLED Canvas; semántica fina pendiente de ampliación. | `f4:Core/Src/oled_canvas.c:139`; inferred-name |
| <a id="fn-oledcanvas_resettransferstate"></a>`static void OledCanvas_ResetTransferState(void)` | Restablece Transfer State dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:148`; inferred-name |
| <a id="fn-oledcanvas_showreadyui"></a>`static void OledCanvas_ShowReadyUi(void)` | Coordina `MenuSys_SetCurrentScreenCode`, `clearScreen` para Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:164`; structural |
| <a id="fn-oledcanvas_exittodisplaymenu"></a>`static void OledCanvas_ExitToDisplayMenu(void)` | Coordina `MenuSys_SetCurrentMenuScreenCode`, `clearScreen` para Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:185`; structural |
| <a id="fn-oledcanvas_exittodashboard"></a>`static void OledCanvas_ExitToDashboard(void)` | Coordina `clearScreen` para Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:207`; structural |
| <a id="fn-oledcanvas_applyuiaction"></a>`static void OledCanvas_ApplyUiAction(OledCanvasUiAction_t action)` | Aplica Ui Action dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:228`; inferred-name |
| <a id="fn-oledcanvas_cancelforui"></a>`static void OledCanvas_CancelForUi(uint8_t status, OledCanvasUiAction_t action)` | Coordina `OledCanvas_ApplyUiAction`, `OledCanvas_QueueResponse`, `OledCanvas_RememberResult` para Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:245`; structural |
| <a id="fn-oledcanvas_handlebegin"></a>`static void OledCanvas_HandleBegin(const UNER_Packet *packet)` | Procesa Begin dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:270`; inferred-name |
| <a id="fn-oledcanvas_expectedchunklength"></a>`static uint8_t OledCanvas_ExpectedChunkLength(uint16_t offset)` | Operación Expected Chunk Length de Transferencia transaccional OLED Canvas; semántica fina pendiente de ampliación. | `f4:Core/Src/oled_canvas.c:359`; inferred-name |
| <a id="fn-oledcanvas_handlechunk"></a>`static void OledCanvas_HandleChunk(const UNER_Packet *packet)` | Procesa Chunk dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:367`; inferred-name |
| <a id="fn-oledcanvas_handlecommit"></a>`static void OledCanvas_HandleCommit(const UNER_Packet *packet)` | Procesa Commit dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:477`; inferred-name |
| <a id="fn-oledcanvas_handlecancel"></a>`static void OledCanvas_HandleCancel(const UNER_Packet *packet)` | Procesa Cancel dentro de Transferencia transaccional OLED Canvas. | `f4:Core/Src/oled_canvas.c:567`; inferred-name |
