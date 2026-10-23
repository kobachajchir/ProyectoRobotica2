# Integración del teclado con UI

ID: `f4:module/Core/Inc/osk_integration.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Conecta el teclado agnóstico con dibujo OLED, eventos del encoder y restauración de navegación AutoMicro.

Adaptación de primitivas, apertura alpha/numérica y callbacks finales del editor.

<a id="modularity"></a>
## Modularidad observada

Es el adaptador explícito entre OSK y Screens; está permitido que conozca ambas APIs.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

ScreensState contiene s_oskState, s_savedCtx, tabla de dibujo y callbacks/contexto del usuario. La integración conserva el renderer, gestor de eventos y flags de la vista anterior.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init instala adaptadores gráficos.
- OpenAlpha/OpenNumeric salvan contexto y abren el teclado.
- EventManager traduce entradas; callbacks internos restauran UI y llaman al consumidor final.

## Trabajo en tándem

- OSK mantiene edición; ScreensServices ejecuta dibujo. Los editores Wi-Fi y PID reciben resultado mediante callbacks.

## Precauciones y límites

- Un único contexto activo de pantallas; no abrir otra edición sobre la primera sin considerar savedCtx.
- Copiar texto confirmado si el consumidor lo conserva después de cerrar/reabrir.
- USER largo global retorna al dashboard antes del handler OSK y sin OnBack/cleanup explícito; ver AMDOC-F4-011 antes de asumir cancelación equivalente.

## Hallazgos vinculados

- [AMDOC-WEB-002: El visor rellena estados que el reporte de pantalla no transporta](../../../review/FINDINGS.md#amdoc-web-002)
- [AMDOC-F4-011: USER largo sale del OSK por el router sin ejecutar su cancelación](../../../review/FINDINGS.md#amdoc-f4-011)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/osk_integration.h` | `osk_types.h`, `types/userEvent_type.h` | `7e7ecdf1c5c7471706540a66b34de30d7afb718182fd07d29df0bbe55b588a67` |
| `Core/Src/osk_integration.c` | `screens_internal.h`, `osk_integration.h`, `fonts.h`, `screen_codes.h`, `oled_utils.h`, `string.h` | `cbf81e3cdff41049a3f7112985bf2837f570c7812a3e4aee9cebf2f5ca77e63f` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-osk_integration_eventmanager"></a>`void OSK_Integration_EventManager(UserEvent_t ev)` | Traduce eventos de usuario a navegación/acción del teclado. | `f4:Core/Inc/osk_integration.h:66`; editorial |
| <a id="fn-osk_integration_init"></a>`void OSK_Integration_Init(void)` | Conecta primitivas gráficas y estado del adaptador OSK. | `f4:Core/Inc/osk_integration.h:28`; editorial |
| <a id="fn-osk_integration_isactive"></a>`bool OSK_Integration_IsActive(void)` | Consulta si el teclado integrado permanece abierto. | `f4:Core/Inc/osk_integration.h:78`; editorial |
| <a id="fn-osk_integration_openalpha"></a>`void OSK_Integration_OpenAlpha(const char *initialText, uint8_t maxLen, const OSK_AlphaConfig *alphaCfg, OSK_CompleteFn onComplete, OSK_CancelFn onCancel, void *ctx)` | Salva la vista anterior y abre edición de texto con sus restricciones. | `f4:Core/Inc/osk_integration.h:42`; editorial |
| <a id="fn-osk_integration_opennumeric"></a>`void OSK_Integration_OpenNumeric(const OSK_NumericConfig *numCfg, OSK_CompleteFn onComplete, OSK_CancelFn onCancel, void *ctx)` | Salva la vista anterior y abre edición numérica configurada. | `f4:Core/Inc/osk_integration.h:57`; editorial |
| <a id="fn-osk_integration_render"></a>`void OSK_Integration_Render(void)` | Invoca render del teclado integrado. | `f4:Core/Inc/osk_integration.h:72`; editorial |
| <a id="fn-screens_init_osk_integration"></a>`void Screens_Init_osk_integration(void)` | Inicializa estado de adaptación de teclado en el contexto activo. | `f4:Core/Src/osk_integration.c:323`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-wrap_clearscreen"></a>`static void wrap_clearScreen(void)` | Adapta la llamada a `ssd1306_Clear` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:44`; structural |
| <a id="fn-wrap_setcolor"></a>`static void wrap_setColor(uint8_t color)` | Adapta la llamada a `ssd1306_SetColor` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:49`; structural |
| <a id="fn-wrap_setcursor"></a>`static void wrap_setCursor(uint8_t x, uint8_t y)` | Adapta la llamada a `ssd1306_SetCursor` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:60`; structural |
| <a id="fn-wrap_writechar"></a>`static void wrap_writeChar(char ch)` | Adapta la llamada a `ssd1306_WriteChar` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:65`; structural |
| <a id="fn-wrap_writestring"></a>`static void wrap_writeString(char *str)` | Adapta la llamada a `ssd1306_WriteString` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:70`; structural |
| <a id="fn-wrap_drawrect"></a>`static void wrap_drawRect(int16_t x, int16_t y, int16_t w, int16_t h)` | Adapta la llamada a `ssd1306_DrawRect` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:75`; structural |
| <a id="fn-wrap_fillrect"></a>`static void wrap_fillRect(int16_t x, int16_t y, int16_t w, int16_t h)` | Adapta la llamada a `ssd1306_FillRect` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:80`; structural |
| <a id="fn-wrap_drawhline"></a>`static void wrap_drawHLine(int16_t x, int16_t y, int16_t len)` | Adapta la llamada a `ssd1306_DrawHorizontalLine` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:85`; structural |
| <a id="fn-wrap_drawvline"></a>`static void wrap_drawVLine(int16_t x, int16_t y, int16_t len)` | Adapta la llamada a `ssd1306_DrawVerticalLine` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:90`; structural |
| <a id="fn-wrap_updatescreen"></a>`static void wrap_updateScreen(void)` | Adapta la llamada a `ssd1306_UpdateScreen` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:95`; structural |
| <a id="fn-wrap_requestpageupdate"></a>`static void wrap_requestPageUpdate(uint8_t page)` | Adapta la llamada a `ssd1306_RequestPageUpdate` con el contexto de esta integración. | `f4:Core/Src/osk_integration.c:100`; structural |
| <a id="fn-osk_saveuicontext"></a>`static void osk_SaveUiContext(void)` | Operación osk Save Ui Context de Integración del teclado con UI; semántica fina pendiente de ampliación. | `f4:Core/Src/osk_integration.c:109`; inferred-name |
| <a id="fn-osk_restoreuicontext"></a>`static void osk_RestoreUiContext(void)` | Coordina `OledUtils_SetNotificationsBlocked`, `clearScreen` para Integración del teclado con UI. | `f4:Core/Src/osk_integration.c:119`; structural |
| <a id="fn-osk_activateui"></a>`static void osk_ActivateUi(ScreenCode_t screenCode)` | Coordina `OledUtils_SetNotificationsBlocked`, `osk_SaveUiContext` para Integración del teclado con UI. | `f4:Core/Src/osk_integration.c:140`; structural |
| <a id="fn-osk_internaloncomplete"></a>`static void osk_InternalOnComplete(OSK_State *state, void *ctx)` | Coordina `osk_RestoreUiContext`, `userCb` para Integración del teclado con UI. | `f4:Core/Src/osk_integration.c:167`; structural |
| <a id="fn-osk_internaloncancel"></a>`static void osk_InternalOnCancel(void *ctx)` | Coordina `osk_RestoreUiContext`, `userCb` para Integración del teclado con UI. | `f4:Core/Src/osk_integration.c:184`; structural |
