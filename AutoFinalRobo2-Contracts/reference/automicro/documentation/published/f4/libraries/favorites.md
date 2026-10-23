# Favoritos de menú

ID: `f4:module/Core/Inc/menu_favorites.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Mantiene hasta tres accesos favoritos y construye su submenú con referencias a los ítems originales.

Selección, persistencia de screen codes y reconstrucción visual; no duplica el motor de navegación.

<a id="modularity"></a>
## Modularidad observada

Servicios externos evitan llamadas directas a persistencia, pero el handle usa ENC_Handle_t/MenuSystem y globals de contexto. Integración AutoMicro, no biblioteca independiente de tipos del proyecto.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

FavoritesHandle conserva services, encoder, originales, copia de ítems favoritos, submenú y MenuSystem. Favorites_Context es una referencia global activa; el handle posee estado pero la API histórica no recibe instancia explícita.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- FavoritesInit reconstruye favoritos desde preferencias y catálogo.
- ToggleFavorite agrega/quita y solicita persistencia; la capacidad y elegibilidad deben respetarse.

## Trabajo en tándem

- FavoritesServices resuelve screen code → MenuItem y guarda un patch en AppPreferences.
- El menú ordinario muestra tres filas por defecto; favoritos configura dos filas.

## Precauciones y límites

- Los MenuItem originales deben vivir mientras se usen sus referencias.
- El contexto global no garantiza varias instancias activas simultáneas.

## Hallazgos vinculados

- [AMDOC-F4-007: Los contextos activos de UI limitan independencia y reentrada](../../../review/FINDINGS.md#amdoc-f4-007)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/menu_favorites.h` | `stdbool.h`, `stdint.h`, `types/menu_types.h` | `1d13e1317233241fbcff33653382c6d92eb8bc84f9e52f33500777c486c53549` |
| `Core/Inc/favorites_context.h` | `types/menu_types.h`, `app_preferences_types.h`, `fonts.h`, `encoder_types.h` | `6820371fbf07420a085a65cbb45d5fec553d5eebc24920efff6083e4be0f7135` |
| `Core/Src/menu_favorites.c` | `favorites_context.h`, `menu_favorites.h`, `string.h` | `8e558fe5bcea07d6ddef3cf7f7ccccd939776c792b7ab2208e65a6bbc57a1767` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `FavoritesServices`

`f4:Core/Inc/favorites_context.h:10`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t (*now_ms)(void)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |
| `AM_Result (*AppPreferences_ApplyPatch)(const AppPreferencesPatch_t *patch, AppPreferencesFieldMask_t *changedFields)` | Operación App Preferences Apply Patch de Favoritos de menú; semántica fina pendiente de ampliación. |
| `const AppPreferences_t * (*AppPreferences_Get)(void)` | Operación App Preferences Get de Favoritos de menú; semántica fina pendiente de ampliación. |
| `const MenuItem* (*MenuSys_GetItemByScreenCode)(ScreenCode_t code)` | Consulta Item By Screen Code dentro de Favoritos de menú. |
| `void (*MenuSys_GoBack_Wrapper)(void)` | Prepara el contexto de UI y selecciona la vista Go Back. |
| `void (*OledUtils_RenderDashboard_Wrapper)(void)` | Prepara el contexto de UI y selecciona la vista Oled Utils Render Dashboard. |
| `void (*dashboardEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de dashboard Event Manager. |

### `FavoritesHandle`

`f4:Core/Inc/favorites_context.h:20`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const FavoritesServices *services` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `ENC_Handle_t *encoder` | Dato de estado/configuración `encoder`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const MenuItem *g_originalFavs[MAX_FAVORITES]` | Dato de estado/configuración `g_originalFavs`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MenuItem g_favMenuItems[MAX_FAVORITES + 1]` | Dato de estado/configuración `g_favMenuItems`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SubMenu g_submenuFavorites` | Dato de estado/configuración `g_submenuFavorites`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MenuSystem *g_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-menusys_canbefavorite"></a>`bool MenuSys_CanBeFavorite(const MenuItem *item)` | Comprueba si el ítem es elegible para favorito. | `f4:Core/Inc/menu_favorites.h:44`; editorial |
| <a id="fn-menusys_favoritesinit"></a>`void MenuSys_FavoritesInit(MenuSystem *ms)` | Carga favoritos persistidos y prepara el submenú asociado. | `f4:Core/Inc/menu_favorites.h:18`; editorial |
| <a id="fn-menusys_isfavorite"></a>`bool MenuSys_IsFavorite(const MenuItem *item)` | Indica si el ítem está en la lista actual. | `f4:Core/Inc/menu_favorites.h:32`; editorial |
| <a id="fn-menusys_isfavoritesmenu"></a>`bool MenuSys_IsFavoritesMenu(const SubMenu *menu)` | Identifica el submenú de favoritos activo. | `f4:Core/Inc/menu_favorites.h:49`; editorial |
| <a id="fn-menusys_togglefavorite"></a>`int8_t MenuSys_ToggleFavorite(const MenuItem *item)` | Agrega o elimina el ítem favorito y solicita guardar la nueva lista. | `f4:Core/Inc/menu_favorites.h:25`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-savefavoritestopreferences"></a>`static void SaveFavoritesToPreferences(void)` | Adapta la llamada a `AppPreferences_ApplyPatch` con el contexto de esta integración. | `f4:Core/Src/menu_favorites.c:145`; structural |
| <a id="fn-rebuildfavoritesmenu"></a>`static void RebuildFavoritesMenu(void)` | Operación Rebuild Favorites Menu de Favoritos de menú; semántica fina pendiente de ampliación. | `f4:Core/Src/menu_favorites.c:160`; inferred-name |
