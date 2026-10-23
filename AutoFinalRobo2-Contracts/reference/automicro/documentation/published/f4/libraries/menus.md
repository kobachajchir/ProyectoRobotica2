# Sistema de menús

ID: `f4:module/Core/Inc/menusystem.h` · Capa: **núcleo** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Navega el árbol de menús, selecciona ítems y mantiene reportes de pantalla y selección.

Modelo MenuItem/SubMenu, visibilidad, disponibilidad, cursor, páginas y callbacks de render/acción. El catálogo concreto vive en menu_definition.c.

<a id="modularity"></a>
## Modularidad observada

No incluye HAL ni bridge concreto. Comparte screen_codes y UserEvent_t. Varias funciones de render/visibilidad son callbacks sin ctx, por lo que el contexto se obtiene externamente.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

MenuSystem presta root/currentMenu, MenuServices, fast_scroll_enabled e insideMenuFlag; posee flags de render y reportes pendientes. MenuItem presta texto, bitmap, acción, submenú y proveedores dinámicos. SubMenu guarda índices y padre: cambiarlo afecta la navegación de todas las referencias al mismo objeto.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- La aplicación conecta raíz, servicios y callbacks antes de navegar.
- Se resuelve visibilidad y disponibilidad antes de seleccionar; no son la misma condición.
- FlushScreenReport precede a FlushMenuSelectionReport; la selección espera mientras quede una pantalla pendiente.

## Trabajo en tándem

- MenuServices.report_screen/report_selection delega a WiFi/UNER desde app_menu_services.h.
- Favoritos conserva referencias a MenuItem y reconstruye un submenú; Screens dibuja usando los callbacks instalados.

## Precauciones y límites

- MAX_VISIBLE_ITEMS del header tiene valor histórico 2, pero la implementación usa max_visible_items o 3; consumidores no deben inferir filas desde esa macro.
- 0x96 transmite selección/cantidad, no buffer de teclado ni imagen física.

## Hallazgos vinculados

- [AMDOC-WEB-004: La ventana del menú reconstruida en Web puede ejecutar otra fila en F4](../../../review/FINDINGS.md#amdoc-web-004)
- [AMDOC-WEB-008: Dos identidades de acción F4 no están declaradas en screenCodes Web](../../../review/FINDINGS.md#amdoc-web-008)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/menusystem.h` | `types/menu_types.h` | `8ff3d559f3a91228bc3b7ebd76eb21261bbd2b4da6d7742c5d0cd9d97b0cffc9` |
| `Core/Inc/types/menu_types.h` | `stdbool.h`, `stdint.h`, `stddef.h`, `screen_codes.h`, `userEvent_type.h` | `132cfeb31787cd870f4c0f0d8746062af3ab10c0c292e5a1aeb72042642626ca` |
| `Core/Src/menusystem.c` | `stdbool.h`, `stdint.h`, `stddef.h`, `stdio.h`, `string.h`, `types/menu_types.h`, `menusystem.h` | `305d434e463bd82700d50f6f2abff868e58facbc0a7b59bcc645b924ae7c1a96` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `MenuItem`

`f4:Core/Inc/types/menu_types.h:35`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const char *name` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MenuFunction action` | Acción invocada al seleccionar; callback histórico sin contexto explícito. |
| `struct SubMenu *submenu` | Submenú al que puede navegar este ítem. |
| `const uint8_t *icon` | Dato de estado/configuración `icon`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderScreenFunction screenRenderFn` | Renderer asociado a la pantalla destino del ítem. |
| `UserEventManagerFn eventManagerFn` | Gestor de eventos instalado para la pantalla del ítem. |
| `MenuVisibilityFn visibilityFn` | Condición dinámica de visibilidad; nula implica visible. |
| `ScreenCode_t screen_code` | Dato de estado/configuración `screen_code`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MenuIconProviderFn dynamicIconFn` | Proveedor opcional de bitmap según estado runtime. |
| `MenuNameProviderFn dynamicNameFn` | Proveedor opcional de etiqueta según estado runtime. |
| `uint32_t required_ext_mask` | Máscara de perfiles/extensiones requeridos para disponibilidad. |

### `SubMenu`

`f4:Core/Inc/types/menu_types.h:54`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const char *name` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MenuItem *items` | Dato de estado/configuración `items`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t itemCount` | Dato de estado/configuración `itemCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t currentItemIndex` | Dato de estado/configuración `currentItemIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t lastSelectedItemIndex` | Dato de estado/configuración `lastSelectedItemIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t firstVisibleItem` | Dato de estado/configuración `firstVisibleItem`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t lastVisibleItem` | Dato de estado/configuración `lastVisibleItem`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct SubMenu *parent` | Dato de estado/configuración `parent`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const uint8_t *icon` | Dato de estado/configuración `icon`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ScreenCode_t screen_code` | Dato de estado/configuración `screen_code`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t max_visible_items` | Dato de estado/configuración `max_visible_items`; declaración estructural, interpretación específica pendiente de ampliar. |

### `MenuServices`

`f4:Core/Inc/types/menu_types.h:68`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |
| `bool (*item_available)(void *ctx, const MenuItem *item)` | Decide disponibilidad del ítem según política externa y contexto. |
| `bool (*report_screen)(void *ctx, uint32_t code, uint8_t source)` | Intenta publicar identidad de pantalla y origen; false conserva el reporte pendiente. |
| `bool (*report_selection)(void *ctx, uint32_t code, uint8_t index, uint8_t count, uint8_t source)` | Intenta publicar selección y cantidad; no contiene el framebuffer. |

### `MenuSystem`

`f4:Core/Inc/types/menu_types.h:75`

| Atributo / firma | Función y precaución |
| --- | --- |
| `SubMenu *root` | Dato de estado/configuración `root`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const MenuServices *services` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `volatile uint8_t *fast_scroll_enabled` | Dato de estado/configuración `fast_scroll_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SubMenu *currentMenu` | Dato de estado/configuración `currentMenu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ClearFunction clearScreen` | Callback de limpieza de dibujo. |
| `DrawItemFunction drawItem` | Callback que dibuja ítem, coordenada Y y selección. |
| `RenderFunction renderFn` | Renderer activo, no necesariamente el renderer de lista de menú. |
| `RenderFunction dashboardRender` | Dato de estado/configuración `dashboardRender`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UserEventManagerFn userEventManagerFn` | Gestor de eventos de la vista activa. |
| `volatile uint8_t *insideMenuFlag` | Bandera externa prestada que diferencia navegación de menú de otra vista. |
| `volatile bool renderFlag` | Dato de estado/configuración `renderFlag`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool suppressClearOnNextRender` | Evita limpiar al próximo render para conservar contenido en actualización parcial. |
| `bool allowPeriodicRefresh` | Dato de estado/configuración `allowPeriodicRefresh`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ScreenCode_t current_screen_code` | Dato de estado/configuración `current_screen_code`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ScreenCode_t last_reported_screen_code` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t current_screen_source` | Dato de estado/configuración `current_screen_source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool screen_report_pending` | Queda pendiente publicar identidad de pantalla. |
| `uint8_t current_selected_index` | Dato de estado/configuración `current_selected_index`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t last_reported_selected_index` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t current_item_count` | Dato de estado/configuración `current_item_count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t last_reported_item_count` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t current_selection_source` | Dato de estado/configuración `current_selection_source`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool selection_report_pending` | Queda pendiente publicar selección; espera que finalice reporte de pantalla. |

### `MenuFunction`

`f4:Core/Inc/types/menu_types.h:20`

Firma: `void (*MenuFunction)()`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `ClearFunction`

`f4:Core/Inc/types/menu_types.h:22`

Firma: `void (*ClearFunction)()`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `RenderFunction`

`f4:Core/Inc/types/menu_types.h:23`

Firma: `void (*RenderFunction)()`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `RenderWrapperFn`

`f4:Core/Inc/types/menu_types.h:24`

Firma: `void (*RenderWrapperFn)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `RenderScreenFunction`

`f4:Core/Inc/types/menu_types.h:25`

Firma: `void (*RenderScreenFunction)()`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `UserEventManagerFn`

`f4:Core/Inc/types/menu_types.h:26`

Firma: `void (*UserEventManagerFn)(UserEvent_t ev)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `MenuVisibilityFn`

`f4:Core/Inc/types/menu_types.h:27`

Firma: `bool (*MenuVisibilityFn)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `MenuIconProviderFn`

`f4:Core/Inc/types/menu_types.h:28`

Firma: `const uint8_t *(*MenuIconProviderFn)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `MenuNameProviderFn`

`f4:Core/Inc/types/menu_types.h:29`

Firma: `const char *(*MenuNameProviderFn)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `DrawItemFunction`

`f4:Core/Inc/types/menu_types.h:51`

Firma: `void (*DrawItemFunction)(const MenuItem *item, int y, bool selected)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-menusys_clearcurrentmenuselection"></a>`void MenuSys_ClearCurrentMenuSelection(MenuSystem *ms, ScreenReportSource_t source)` | Invalida selección de menú y solicita el reporte correspondiente. | `f4:Core/Inc/menusystem.h:65`; editorial |
| <a id="fn-menusys_flushmenuselectionreport"></a>`void MenuSys_FlushMenuSelectionReport(MenuSystem *ms)` | Envía selección pendiente solo después de completar el reporte de pantalla. | `f4:Core/Inc/menusystem.h:66`; editorial |
| <a id="fn-menusys_flushscreenreport"></a>`void MenuSys_FlushScreenReport(MenuSystem *ms)` | Intenta enviar pantalla pendiente y conserva el pendiente si falla. | `f4:Core/Inc/menusystem.h:58`; editorial |
| <a id="fn-menusys_getcurrentscreencode"></a>`ScreenCode_t MenuSys_GetCurrentScreenCode(const MenuSystem *ms)` | Consulta identidad de pantalla del MenuSystem recibido. | `f4:Core/Inc/menusystem.h:56`; editorial |
| <a id="fn-menusys_getcurrentscreensource"></a>`uint8_t MenuSys_GetCurrentScreenSource(const MenuSystem *ms)` | Consulta el origen registrado de la pantalla actual. | `f4:Core/Inc/menusystem.h:57`; editorial |
| <a id="fn-menusys_handleclick"></a>`void MenuSys_HandleClick(MenuSystem *ms)` | Ejecuta acción, pantalla o entrada a submenú del ítem seleccionado. | `f4:Core/Inc/menusystem.h:52`; editorial |
| <a id="fn-menusys_init"></a>`void MenuSys_Init(MenuSystem *system)` | Inicializa navegación y estado de reportes a partir de la raíz suministrada. | `f4:Core/Inc/menusystem.h:25`; editorial |
| <a id="fn-menusys_isitemavailable_h"></a>`bool MenuSys_IsItemAvailable_H(const MenuSystem *ms, const MenuItem *item)` | Consulta política item_available del servicio externo. | `f4:Core/Inc/menusystem.h:69`; editorial |
| <a id="fn-menusys_isitemvisible"></a>`bool MenuSys_IsItemVisible(const MenuItem *item)` | Evalúa visibilityFn o considera visible al ítem sin condición. | `f4:Core/Inc/menusystem.h:68`; editorial |
| <a id="fn-menusys_movecursordown"></a>`void MenuSys_MoveCursorDown(MenuSystem *ms)` | Avanza a la siguiente selección navegable y ajusta ventana visible. | `f4:Core/Inc/menusystem.h:42`; editorial |
| <a id="fn-menusys_movecursorup"></a>`void MenuSys_MoveCursorUp(MenuSystem *ms)` | Retrocede a la selección navegable anterior y ajusta ventana. | `f4:Core/Inc/menusystem.h:43`; editorial |
| <a id="fn-menusys_movevisiblepage"></a>`bool MenuSys_MoveVisiblePage(MenuSystem *ms, int8_t direction)` | Desplaza una página visible según dirección. | `f4:Core/Inc/menusystem.h:45`; editorial |
| <a id="fn-menusys_navigateback"></a>`void MenuSys_NavigateBack(MenuSystem *ms)` | Vuelve al padre o al dashboard según la posición actual. | `f4:Core/Inc/menusystem.h:48`; editorial |
| <a id="fn-menusys_navigatetomain"></a>`void MenuSys_NavigateToMain(MenuSystem *ms)` | Navega al menú raíz. | `f4:Core/Inc/menusystem.h:49`; editorial |
| <a id="fn-menusys_notifyscreencontentchanged"></a>`void MenuSys_NotifyScreenContentChanged(MenuSystem *ms, ScreenReportSource_t source)` | Solicita reportar cambio de contenido sin inventar un framebuffer remoto. | `f4:Core/Inc/menusystem.h:54`; editorial |
| <a id="fn-menusys_opensubmenu"></a>`void MenuSys_OpenSubMenu(MenuSystem *ms, SubMenu *submenu)` | Abre el submenú recibido y actualiza identidad de pantalla. | `f4:Core/Inc/menusystem.h:51`; editorial |
| <a id="fn-menusys_rendermenu"></a>`void MenuSys_RenderMenu(MenuSystem *ms)` | Dibuja los ítems visibles con los callbacks del menú. | `f4:Core/Inc/menusystem.h:47`; editorial |
| <a id="fn-menusys_reportcurrentmenuselection"></a>`void MenuSys_ReportCurrentMenuSelection(MenuSystem *ms, ScreenReportSource_t source)` | Adapta la llamada a `MenuSys_UpdateSelectionFromCurrentMenu` con el contexto de esta integración. | `f4:Core/Inc/menusystem.h:63`; structural |
| <a id="fn-menusys_requestcurrentmenuselectionsnapshot"></a>`bool MenuSys_RequestCurrentMenuSelectionSnapshot(MenuSystem *ms)` | Solicita publicar la selección visible actual como snapshot. | `f4:Core/Inc/menusystem.h:64`; editorial |
| <a id="fn-menusys_resetmenu"></a>`void MenuSys_ResetMenu(MenuSystem *ms)` | Restablece contexto de navegación del menú. | `f4:Core/Inc/menusystem.h:50`; editorial |
| <a id="fn-menusys_selectvisibleitem"></a>`bool MenuSys_SelectVisibleItem(MenuSystem *ms, uint8_t visible_position_one_based)` | Selecciona una fila visible numerada desde uno. | `f4:Core/Inc/menusystem.h:44`; editorial |
| <a id="fn-menusys_setcallbacks"></a>`void MenuSys_SetCallbacks(MenuSystem *ms, ClearFunction clear, DrawItemFunction draw, RenderFunction render, volatile uint8_t *insideFlag, RenderFunction dashboardRender)` | Conecta primitivas de dibujo, renderers y bandera externa de menú. | `f4:Core/Inc/menusystem.h:35`; editorial |
| <a id="fn-menusys_setcurrentmenuscreencode"></a>`void MenuSys_SetCurrentMenuScreenCode(MenuSystem *ms)` | Adapta la llamada a `MenuSys_SetCurrentScreenCode` con el contexto de esta integración. | `f4:Core/Inc/menusystem.h:55`; structural |
| <a id="fn-menusys_setcurrentmenuselection"></a>`void MenuSys_SetCurrentMenuSelection(MenuSystem *ms, uint8_t selected_index, uint8_t item_count, ScreenReportSource_t source)` | Configura índice, cantidad y origen de selección y marca reporte pendiente. | `f4:Core/Inc/menusystem.h:59`; editorial |
| <a id="fn-menusys_setcurrentscreencode"></a>`void MenuSys_SetCurrentScreenCode(MenuSystem *ms, ScreenCode_t screen_code, ScreenReportSource_t source)` | Actualiza identidad/origen de pantalla y marca reporte pendiente. | `f4:Core/Inc/menusystem.h:53`; editorial |
| <a id="fn-menu_iscurrentitem"></a>`static inline bool Menu_IsCurrentItem(const MenuSystem *sys, const MenuItem *item)` | Compara el ítem con la selección actual; requiere menú e índices válidos. | `f4:Core/Inc/menusystem.h:71`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-menusys_hasnavigableselection"></a>`static bool MenuSys_HasNavigableSelection(const MenuSystem *ms)` | Adapta la llamada a `MenuSys_IsCurrentScreenMenu` con el contexto de esta integración. | `f4:Core/Src/menusystem.c:159`; structural |
| <a id="fn-menusys_iscurrentscreenmenu"></a>`static bool MenuSys_IsCurrentScreenMenu(const MenuSystem *ms)` | Indica si se cumple la condición Current Screen Menu dentro de Sistema de menús. | `f4:Core/Src/menusystem.c:168`; inferred-name |
| <a id="fn-menusys_getvisibleselection"></a>`static bool MenuSys_GetVisibleSelection(const SubMenu *menu, uint8_t *selected_index, uint8_t *item_count)` | Consulta Visible Selection dentro de Sistema de menús. | `f4:Core/Src/menusystem.c:175`; inferred-name |
| <a id="fn-menusys_resolvecurrentmenuselection"></a>`static bool MenuSys_ResolveCurrentMenuSelection(const MenuSystem *ms, uint8_t *selected_index, uint8_t *item_count)` | Coordina `MenuSys_GetVisibleSelection`, `MenuSys_IsCurrentScreenMenu` para Sistema de menús. | `f4:Core/Src/menusystem.c:220`; structural |
| <a id="fn-menusys_updateselectionfromcurrentmenu"></a>`static void MenuSys_UpdateSelectionFromCurrentMenu(MenuSystem *ms, ScreenReportSource_t source)` | Actualiza Selection From Current Menu dentro de Sistema de menús. | `f4:Core/Src/menusystem.c:236`; inferred-name |
| <a id="fn-menusys_selectparentitemforchild"></a>`static bool MenuSys_SelectParentItemForChild(SubMenu *parent, const SubMenu *child)` | Adapta la llamada a `MenuSys_IsItemVisible` con el contexto de esta integración. | `f4:Core/Src/menusystem.c:272`; structural |
| <a id="fn-menusys_findfirstvisibleindexfrom"></a>`static int8_t MenuSys_FindFirstVisibleIndexFrom(const SubMenu *menu, int8_t start)` | Adapta la llamada a `MenuSys_IsItemVisible` con el contexto de esta integración. | `f4:Core/Src/menusystem.c:298`; structural |
| <a id="fn-menusys_findlastvisibleindexbefore"></a>`static int8_t MenuSys_FindLastVisibleIndexBefore(const SubMenu *menu, int8_t start)` | Adapta la llamada a `MenuSys_IsItemVisible` con el contexto de esta integración. | `f4:Core/Src/menusystem.c:309`; structural |
| <a id="fn-menusys_findnextvisibleindex"></a>`static int8_t MenuSys_FindNextVisibleIndex(const SubMenu *menu, int8_t start, int8_t direction)` | Adapta la llamada a `MenuSys_IsItemVisible` con el contexto de esta integración. | `f4:Core/Src/menusystem.c:320`; structural |
| <a id="fn-menusys_computelastvisibleindex"></a>`static int8_t MenuSys_ComputeLastVisibleIndex(const SubMenu *menu, int8_t firstVisibleIndex)` | Coordina `GET_MAX_VISIBLE`, `MenuSys_IsItemVisible` para Sistema de menús. | `f4:Core/Src/menusystem.c:332`; structural |
| <a id="fn-menusys_findwrappedvisibleindex"></a>`static int8_t MenuSys_FindWrappedVisibleIndex(const SubMenu *menu, int8_t start, int8_t direction)` | Coordina `MenuSys_FindFirstVisibleIndexFrom`, `MenuSys_FindLastVisibleIndexBefore`, `MenuSys_FindNextVisibleIndex` para Sistema de menús. | `f4:Core/Src/menusystem.c:349`; structural |
| <a id="fn-menusys_ensurecurrentselectionvisible"></a>`static void MenuSys_EnsureCurrentSelectionVisible(SubMenu *menu)` | Coordina `MenuSys_ComputeLastVisibleIndex`, `MenuSys_FindFirstVisibleIndexFrom`, `MenuSys_FindLastVisibleIndexBefore` para Sistema de menús. | `f4:Core/Src/menusystem.c:364`; structural |
