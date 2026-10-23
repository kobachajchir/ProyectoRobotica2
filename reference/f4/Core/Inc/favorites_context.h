#ifndef FAVORITES_CONTEXT_H
#define FAVORITES_CONTEXT_H
#include "types/menu_types.h"
#include "app_preferences_types.h"
#include "fonts.h"
#include "encoder_types.h"
#ifndef MAX_FAVORITES
#define MAX_FAVORITES 3
#endif
typedef struct {
    uint32_t (*now_ms)(void);
    AM_Result (*AppPreferences_ApplyPatch)(const AppPreferencesPatch_t *patch,
                                            AppPreferencesFieldMask_t *changedFields);
    const AppPreferences_t * (*AppPreferences_Get)(void);
    const MenuItem* (*MenuSys_GetItemByScreenCode)(ScreenCode_t code);
    void (*MenuSys_GoBack_Wrapper)(void);
    void (*OledUtils_RenderDashboard_Wrapper)(void);
    void (*dashboardEventManager)(UserEvent_t ev);
} FavoritesServices;
typedef struct {
    const FavoritesServices *services;
    ENC_Handle_t *encoder;
    const MenuItem *g_originalFavs[MAX_FAVORITES];
    MenuItem g_favMenuItems[MAX_FAVORITES + 1];
    SubMenu g_submenuFavorites;
    MenuSystem *g_ms;
} FavoritesHandle;
extern FavoritesHandle *Favorites_Context;
#endif
