# Teclado en pantalla OSK

ID: `f4:module/Core/Inc/osk.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Edita texto y valores numéricos con navegación explícita y primitivas gráficas inyectadas.

QWERTY, mayúsculas/símbolos, teclado numérico y límites de entrada. Geometría de diseño 128×64 y texto máximo 63 caracteres más terminador.

<a id="modularity"></a>
## Modularidad observada

No depende de SSD1306, encoder ni HAL. Sí fija geometría y contrato de dibujo sin contexto por operación; no se promete un backend arbitrario sin adaptación.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

OSK_State conserva configuración, buffer y selección. OSK_Config incorpora copia de OSK_DrawHandle, onComplete/onCancel y callbackCtx; los dibujos no reciben ctx y usan el backend que el integrador deje activo. GetText devuelve memoria del estado; copiar antes de reutilizarlo.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Open configura contenido y layout; los punteros de dibujo necesarios deben estar conectados.
- OnLeft/OnRight/OnSelect/OnBack procesan entradas; Render invoca el backend de dibujo.
- onComplete entrega estado y contexto; onCancel entrega contexto. Close termina la sesión.

## Trabajo en tándem

- osk_integration.c salva/restaura navegación y adapta SSD1306; el núcleo no conoce esa composición.

## Precauciones y límites

- OSK_SUPPORT_FLOAT altera la API disponible por compilación.
- Una referencia de pantalla o selección remota no reconstruye por sí sola cursor/buffer/Shift.

## Hallazgos vinculados

- [AMDOC-F4-011: USER largo sale del OSK por el router sin ejecutar su cancelación](../../../review/FINDINGS.md#amdoc-f4-011)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/osk.h` | `osk_types.h` | `cf09de0323a0ae9e2e2be741ed0be838c8aea3e186b9751134eb4a2f8bf2c17f` |
| `Core/Inc/osk_types.h` | `stdbool.h`, `stdint.h` | `853416e9d708f76753d2d7d5c8262dcc6a11fc4582cb7005d7c4c0d9ccab05a7` |
| `Core/Src/osk.c` | `osk.h`, `string.h`, `stdlib.h` | `2c2e88fc5f0736ae323de2f40ead82684a7de3939205383f7f83d7fcb5020b7d` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `OSK_DrawHandle`

`f4:Core/Inc/osk_types.h:95`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void (*clearScreen)(void)` | Limpia el buffer de dibujo provisto por el integrador. |
| `void (*setColor)(uint8_t color)` | Selecciona color lógico para las primitivas de dibujo siguientes. |
| `void (*setCursor)(uint8_t x, uint8_t y)` | Ubica el cursor de texto en coordenadas de píxel. |
| `void (*writeChar)(char ch)` | Dibuja un carácter con la fuente elegida por el integrador. |
| `void (*writeString)(char *str)` | Dibuja una cadena terminada en cero; no transfiere su propiedad. |
| `void (*drawRect)(int16_t x, int16_t y, int16_t w, int16_t h)` | Dibuja el contorno del rectángulo en píxeles. |
| `void (*fillRect)(int16_t x, int16_t y, int16_t w, int16_t h)` | Rellena el rectángulo en píxeles. |
| `void (*drawHLine)(int16_t x, int16_t y, int16_t len)` | Dibuja un segmento horizontal en píxeles. |
| `void (*drawVLine)(int16_t x, int16_t y, int16_t len)` | Dibuja un segmento vertical en píxeles. |
| `void (*updateScreen)(void)` | Solicita el refresco completo al backend de dibujo. |
| `void (*requestPageUpdate)(uint8_t page)` | Solicita refresco de una página del backend de dibujo. |

### `OSK_NumericConfig`

`f4:Core/Inc/osk_types.h:113`

| Atributo / firma | Función y precaución |
| --- | --- |
| `OSK_NumericMode mode` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t maxIntegerDigits` | Dato de estado/configuración `maxIntegerDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool allowNegative` | Dato de estado/configuración `allowNegative`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool prefillZero` | Dato de estado/configuración `prefillZero`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int32_t minIntValue` | Dato de estado/configuración `minIntValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int32_t maxIntValue` | Dato de estado/configuración `maxIntValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int32_t initialIntValue` | Dato de estado/configuración `initialIntValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t maxDecimalDigits` | Dato de estado/configuración `maxDecimalDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float minFloatValue` | Dato de estado/configuración `minFloatValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float maxFloatValue` | Dato de estado/configuración `maxFloatValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float initialFloatValue` | Dato de estado/configuración `initialFloatValue`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OSK_AlphaConfig`

`f4:Core/Inc/osk_types.h:138`

| Atributo / firma | Función y precaución |
| --- | --- |
| `bool allowLowercase` | Dato de estado/configuración `allowLowercase`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool allowUppercase` | Dato de estado/configuración `allowUppercase`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool allowNumbers` | Dato de estado/configuración `allowNumbers`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool allowSymbols` | Dato de estado/configuración `allowSymbols`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OSK_InitialTextMode initialTextMode` | Dato de estado/configuración `initialTextMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *allowedChars` | Dato de estado/configuración `allowedChars`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OSK_Config`

`f4:Core/Inc/osk_types.h:171`

| Atributo / firma | Función y precaución |
| --- | --- |
| `OSK_Type type` | Dato de estado/configuración `type`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OSK_DrawHandle draw` | Copia de tabla de primitivas de dibujo; sus funciones no reciben contexto por operación. |
| `OSK_CompleteFn onComplete` | Callback de confirmación; recibe OSK_State y callbackCtx. |
| `OSK_CancelFn onCancel` | Callback de cancelación; recibe callbackCtx. |
| `void *callbackCtx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `const char *initialText` | Texto inicial prestado al abrir; no contiene propiedad transferida del buffer externo. |
| `uint8_t maxLen` | Capacidad de edición alfanumérica limitada a 1..63 caracteres. |
| `OSK_NumericConfig numeric` | Dato de estado/configuración `numeric`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OSK_AlphaConfig alpha` | Dato de estado/configuración `alpha`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OSK_State_t`

`f4:Core/Inc/osk_types.h:196`

| Atributo / firma | Función y precaución |
| --- | --- |
| `OSK_Config cfg` | Dato de estado/configuración `cfg`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OSK_Mode mode` | Dato de estado/configuración `mode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char textBuf[OSK_MAX_TEXT_LEN + 1]` | Dato de estado/configuración `textBuf`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t textLen` | Dato de estado/configuración `textLen`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t curRow` | Dato de estado/configuración `curRow`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t curCol` | Dato de estado/configuración `curCol`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool needsFullRedraw` | Dato de estado/configuración `needsFullRedraw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool needsTextRedraw` | Dato de estado/configuración `needsTextRedraw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool needsKeyHighlight` | Dato de estado/configuración `needsKeyHighlight`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t prevRow` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t prevCol` | Memoria de la observación anterior usada por la máquina de estados. |
| `bool hasDecimalPoint` | Dato de estado/configuración `hasDecimalPoint`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool isNegative` | Dato de estado/configuración `isNegative`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool validationError` | Dato de estado/configuración `validationError`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OSK_CompleteFn`

`f4:Core/Inc/osk_types.h:153`

Firma: `void (*OSK_CompleteFn)(OSK_State *state, void *ctx)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `OSK_CancelFn`

`f4:Core/Inc/osk_types.h:160`

Firma: `void (*OSK_CancelFn)(void *ctx)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `RowDesc`

`f4:Core/Src/osk.c:100`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const char *keys` | Dato de estado/configuración `keys`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t cols` | Dato de estado/configuración `cols`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t y` | Dato de estado/configuración `y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t h` | Dato de estado/configuración `h`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-osk_close"></a>`void OSK_Close(OSK_State *state)` | Cierra el teclado con el resultado recibido. | `f4:Core/Inc/osk.h:10`; editorial |
| <a id="fn-osk_getnumericvaluefloat"></a>`float OSK_GetNumericValueFloat(const OSK_State *state)` | Interpreta el contenido numérico como float si está habilitado. | `f4:Core/Inc/osk.h:44`; editorial |
| <a id="fn-osk_getnumericvalueint"></a>`int32_t OSK_GetNumericValueInt(const OSK_State *state)` | Interpreta el contenido numérico como entero. | `f4:Core/Inc/osk.h:37`; editorial |
| <a id="fn-osk_gettext"></a>`const char *OSK_GetText(const OSK_State *state)` | Devuelve una vista del buffer terminado en cero. | `f4:Core/Inc/osk.h:31`; editorial |
| <a id="fn-osk_isactive"></a>`bool OSK_IsActive(const OSK_State *state)` | Consulta si la sesión de teclado permanece abierta. | `f4:Core/Inc/osk.h:30`; editorial |
| <a id="fn-osk_onback"></a>`void OSK_OnBack(OSK_State *state)` | Procesa retroceso/cancelación de la sesión. | `f4:Core/Inc/osk.h:17`; editorial |
| <a id="fn-osk_onleft"></a>`void OSK_OnLeft(OSK_State *state)` | Mueve selección hacia la opción anterior del layout. | `f4:Core/Inc/osk.h:14`; editorial |
| <a id="fn-osk_onright"></a>`void OSK_OnRight(OSK_State *state)` | Mueve selección hacia la opción siguiente del layout. | `f4:Core/Inc/osk.h:15`; editorial |
| <a id="fn-osk_onselect"></a>`void OSK_OnSelect(OSK_State *state)` | Ejecuta la tecla seleccionada, edición o confirmación. | `f4:Core/Inc/osk.h:16`; editorial |
| <a id="fn-osk_open"></a>`void OSK_Open(OSK_State *state, const OSK_Config *cfg)` | Inicializa una sesión de edición alfanumérica o numérica. | `f4:Core/Inc/osk.h:7`; editorial |
| <a id="fn-osk_render"></a>`void OSK_Render(OSK_State *state)` | Dibuja el estado del teclado usando OSK_DrawHandle. | `f4:Core/Inc/osk.h:26`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-osk_getrows"></a>`static void osk_GetRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *buf1, char *buf2)` | Coordina `osk_BuildAlphaRows`, `osk_BuildNumericRows`, `osk_BuildSymbolRows` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:118`; structural |
| <a id="fn-osk_buildalpharows"></a>`static void osk_BuildAlphaRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *row2Buf, char *row3Buf)` | Operación osk Build Alpha Rows de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:135`; inferred-name |
| <a id="fn-osk_buildsymbolrows"></a>`static void osk_BuildSymbolRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *row0Buf, char *row4Buf)` | Operación osk Build Symbol Rows de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:179`; inferred-name |
| <a id="fn-osk_buildnumericrows"></a>`static void osk_BuildNumericRows(const OSK_State *st, RowDesc *rows, uint8_t *numRows, char *numRow2Buf)` | Operación osk Build Numeric Rows de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:230`; inferred-name |
| <a id="fn-osk_getkeyrect"></a>`static void osk_GetKeyRect(const OSK_State *st, const RowDesc *rows, uint8_t numRows, uint8_t row, uint8_t col, int16_t *outX, int16_t *outY, int16_t *outW, int16_t *outH)` | Operación osk Get Key Rect de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:271`; inferred-name |
| <a id="fn-osk_drawkeylabel"></a>`static void osk_DrawKeyLabel(const OSK_State *st, const OSK_DrawHandle *draw, char key, int16_t kx, int16_t ky, int16_t kw, int16_t kh, bool isShiftActive)` | Coordina `drawHLine`, `drawVLine`, `setCursor` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:350`; structural |
| <a id="fn-osk_drawkey"></a>`static void osk_DrawKey(const OSK_State *st, const RowDesc *rows, uint8_t numRows, uint8_t row, uint8_t col, bool highlighted)` | Coordina `drawRect`, `fillRect`, `osk_DrawKeyLabel` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:449`; structural |
| <a id="fn-osk_drawtextbar"></a>`static void osk_DrawTextBar(const OSK_State *st)` | Coordina `drawRect`, `fillRect`, `memcpy` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:488`; structural |
| <a id="fn-osk_drawall"></a>`static void osk_DrawAll(OSK_State *st)` | Coordina `clearScreen`, `osk_DrawKey`, `osk_DrawTextBar` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:526`; structural |
| <a id="fn-osk_updatetextonly"></a>`static void osk_UpdateTextOnly(OSK_State *st)` | Coordina `osk_DrawTextBar`, `requestPageUpdate` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:556`; structural |
| <a id="fn-osk_updatekeyhighlight"></a>`static void osk_UpdateKeyHighlight(OSK_State *st)` | Coordina `osk_DrawKey`, `osk_GetKeyRect`, `osk_GetRows` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:567`; structural |
| <a id="fn-osk_totalkeys"></a>`static uint16_t osk_TotalKeys(const RowDesc *rows, uint8_t numRows)` | Operación osk Total Keys de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:610`; inferred-name |
| <a id="fn-osk_toflat"></a>`static uint16_t osk_ToFlat(const RowDesc *rows, uint8_t numRows, uint8_t row, uint8_t col)` | Operación osk To Flat de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:622`; inferred-name |
| <a id="fn-osk_fromflat"></a>`static void osk_FromFlat(const RowDesc *rows, uint8_t numRows, uint16_t flat, uint8_t *outRow, uint8_t *outCol)` | Operación osk From Flat de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:636`; inferred-name |
| <a id="fn-osk_movecursor"></a>`static void osk_MoveCursor(OSK_State *st, int8_t direction)` | Coordina `osk_FromFlat`, `osk_GetRows`, `osk_ToFlat` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:652`; structural |
| <a id="fn-osk_appendchar"></a>`static void osk_AppendChar(OSK_State *st, char ch)` | Coordina `strchr` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:681`; structural |
| <a id="fn-osk_deletechar"></a>`static void osk_DeleteChar(OSK_State *st)` | Operación osk Delete Char de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:695`; inferred-name |
| <a id="fn-osk_numericcanadddigit"></a>`static bool osk_NumericCanAddDigit(const OSK_State *st)` | Operación osk Numeric Can Add Digit de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:719`; inferred-name |
| <a id="fn-osk_numericcanadddot"></a>`static bool osk_NumericCanAddDot(const OSK_State *st)` | Operación osk Numeric Can Add Dot de Teclado en pantalla OSK; semántica fina pendiente de ampliación. | `f4:Core/Src/osk.c:742`; inferred-name |
| <a id="fn-osk_numericvalidaterange"></a>`static bool osk_NumericValidateRange(const OSK_State *st)` | Coordina `OSK_GetNumericValueFloat`, `OSK_GetNumericValueInt` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:753`; structural |
| <a id="fn-osk_handlekeypress"></a>`static void osk_HandleKeyPress(OSK_State *st)` | Coordina `memcpy`, `onComplete`, `osk_AppendChar` para Teclado en pantalla OSK. | `f4:Core/Src/osk.c:772`; structural |
