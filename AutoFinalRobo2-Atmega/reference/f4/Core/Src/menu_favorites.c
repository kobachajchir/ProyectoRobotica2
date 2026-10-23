#include "favorites_context.h"
/**
 * @file menu_favorites.c
 * @brief Sistema de favoritos para el menú.
 *
 * Permite al usuario marcar hasta MAX_FAVORITES ítems como favoritos y
 * acceder a ellos rápidamente desde el Dashboard con una pulsación larga
 * del encoder.
 *
 * Diseño:
 * - Se almacenan punteros a los MenuItem originales (g_originalFavs[]).
 * - Al abrir el menú de favoritos, se copia cada ítem original a un
 *   arreglo local (g_favMenuItems[]) y se añade un "Volver" al final.
 * - La comparación de favoritos se hace por puntero al MenuItem original,
 *   lo cual funciona porque los ítems del menú son estáticos.
 */

#include "menu_favorites.h"
#include <string.h>

#define now_ms() (Favorites_Context->services->now_ms())
#define dashboardEventManager (Favorites_Context->services->dashboardEventManager)
#define MenuSys_GetItemByScreenCode (Favorites_Context->services->MenuSys_GetItemByScreenCode)
#define OledUtils_RenderDashboard_Wrapper (Favorites_Context->services->OledUtils_RenderDashboard_Wrapper)
#define MenuSys_GoBack_Wrapper (Favorites_Context->services->MenuSys_GoBack_Wrapper)
#define AppPreferences_Get (Favorites_Context->services->AppPreferences_Get)
#define AppPreferences_ApplyPatch (Favorites_Context->services->AppPreferences_ApplyPatch)
#define encoder (*Favorites_Context->encoder)
#define g_originalFavs (Favorites_Context->g_originalFavs)
#define g_favMenuItems (Favorites_Context->g_favMenuItems)
#define g_submenuFavorites (Favorites_Context->g_submenuFavorites)
#define g_ms (Favorites_Context->g_ms)

/* ── Estado interno ───────────────────────────────────────────── */


 /* +1 para "Volver" */



/* ── Helpers forward ──────────────────────────────────────────── */
static void RebuildFavoritesMenu(void);
static void SaveFavoritesToPreferences(void);

/* ── Inicialización ───────────────────────────────────────────── */

void MenuSys_FavoritesInit(MenuSystem *ms)
{
    g_ms = ms;

    memset(g_originalFavs, 0, sizeof(g_originalFavs));
    memset(g_favMenuItems, 0, sizeof(g_favMenuItems));

    const AppPreferences_t* prefs = AppPreferences_Get();
    for (uint8_t i = 0; i < MAX_FAVORITES; i++) {
        ScreenCode_t code = prefs->favoriteScreenCodes[i];
        if (code != SCREEN_CODE_NONE) {
            const MenuItem* item = MenuSys_GetItemByScreenCode(code);
            if (item && MenuSys_CanBeFavorite(item)) {
                g_originalFavs[i] = item;
            }
        }
    }

    g_submenuFavorites.max_visible_items = 2u;
    g_submenuFavorites.name              = "Favoritos";
    g_submenuFavorites.items             = g_favMenuItems;
    g_submenuFavorites.itemCount         = 1;
    g_submenuFavorites.currentItemIndex  = 0;
    g_submenuFavorites.firstVisibleItem  = 0;
    g_submenuFavorites.lastSelectedItemIndex = -1;
    g_submenuFavorites.lastVisibleItem   = -1;
    g_submenuFavorites.parent            = NULL;  /* NULL => NavigateBack va al dashboard */
    g_submenuFavorites.icon              = NULL;
    g_submenuFavorites.screen_code       = SCREEN_CODE_CORE_FAVORITES;

    RebuildFavoritesMenu();
}

/* ── API pública ──────────────────────────────────────────────── */

SubMenu* MenuSys_GetFavoritesSubMenu(void)
{
    return &g_submenuFavorites;
}

bool MenuSys_CanBeFavorite(const MenuItem *item)
{
    if (!item) return false;

    /* Los ítems "Volver" usan MenuSys_GoBack_Wrapper como action */
    if (item->action == MenuSys_GoBack_Wrapper) {
        return false;
    }

    return true;
}

bool MenuSys_IsFavorite(const MenuItem *item)
{
    if (!item) return false;

    for (uint8_t i = 0; i < MAX_FAVORITES; i++) {
        if (g_originalFavs[i] == item) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Alterna un ítem entre favorito y no favorito.
 * @return  1 si se agregó, 0 si se removió, -1 si no se pudo (lleno o inválido).
 */
int8_t MenuSys_ToggleFavorite(const MenuItem *item)
{
    if (!MenuSys_CanBeFavorite(item)) {
        return -1;
    }

    /* ¿Ya es favorito? => Remover */
    for (uint8_t i = 0; i < MAX_FAVORITES; i++) {
        if (g_originalFavs[i] == item) {
            g_originalFavs[i] = NULL;
            RebuildFavoritesMenu();
            SaveFavoritesToPreferences();
            return 0; /* Removido */
        }
    }

    /* Agregar al primer slot libre */
    for (uint8_t i = 0; i < MAX_FAVORITES; i++) {
        if (g_originalFavs[i] == NULL) {
            g_originalFavs[i] = item;
            RebuildFavoritesMenu();
            SaveFavoritesToPreferences();
            return 1; /* Agregado */
        }
    }

    /* Todos los slots ocupados */
    return -1;
}

static void SaveFavoritesToPreferences(void)
{
    AppPreferencesPatch_t patch;
    memset(&patch, 0, sizeof(patch));
    
    for (uint8_t i = 0; i < MAX_FAVORITES; i++) {
        patch.values.favoriteScreenCodes[i] = g_originalFavs[i] ? g_originalFavs[i]->screen_code : SCREEN_CODE_NONE;
    }
    
    patch.fields = APP_PREFS_FIELD_FAVORITES;
    AppPreferences_ApplyPatch(&patch, NULL);
}

/* ── Reconstrucción del submenú ───────────────────────────────── */

static void RebuildFavoritesMenu(void)
{
    uint8_t count = 0;

    for (uint8_t i = 0; i < MAX_FAVORITES; i++) {
        if (g_originalFavs[i] != NULL) {
            /* Copiamos el MenuItem original completo */
            g_favMenuItems[count] = *(g_originalFavs[i]);
            count++;
        }
    }

    /* Último ítem: "Volver" */
    MenuItem *volver = &g_favMenuItems[count];
    volver->name            = "Volver";
    volver->action          = MenuSys_GoBack_Wrapper;
    volver->submenu         = NULL;
    volver->icon            = Icon_Volver_bits;
    volver->screenRenderFn  = OledUtils_RenderDashboard_Wrapper;
    volver->eventManagerFn  = dashboardEventManager;
    volver->visibilityFn    = NULL;
    volver->screen_code     = SCREEN_CODE_CORE_DASHBOARD;
    volver->dynamicIconFn   = NULL;
    volver->dynamicNameFn   = NULL;

    g_submenuFavorites.itemCount = count + 1;

    /* Asegurar que el cursor no quede fuera de rango */
    if (g_submenuFavorites.currentItemIndex >= g_submenuFavorites.itemCount) {
        g_submenuFavorites.currentItemIndex = g_submenuFavorites.itemCount - 1;
    }
}

bool MenuSys_IsFavoritesMenu(const SubMenu *menu)
{
    return (menu == &g_submenuFavorites);
}
