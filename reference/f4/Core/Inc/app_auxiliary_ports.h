#ifndef APP_AUXILIARY_PORTS_H
#define APP_AUXILIARY_PORTS_H
#include "globals.h"
#include "main.h"
#include "permissions.h"
#include "menu_favorites.h"
#include "menu_definitions.h"
#include "screenWrappers.h"
#include "oled_utils.h"
#include "app_display.h"
#include "app_preferences_binding.h"
#include "app_ap_config.h"
#include "app_wifi_binding.h"
#include "eventManagers.h"
#include "ui_event_router.h"
#include <string.h>
#include "permission_context.h"
static inline void AppPermission_ssd1306_DrawBitmap(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP) { ssd1306_DrawBitmap(X, Y, W, H, pBMP); }
static inline void AppPermission_ssd1306_DrawRect(int16_t x, int16_t y, int16_t width, int16_t height) { ssd1306_DrawRect(x, y, width, height); }
static inline void AppPermission_ssd1306_SetCursor(uint8_t x, uint8_t y) { ssd1306_SetCursor(x, y); }
static inline char AppPermission_ssd1306_WriteString(char* str, FontDef Font) { return ssd1306_WriteString(str, Font); }
static inline void AppPermission_ssd1306_SetColor(SSD1306_COLOR color) { ssd1306_SetColor(color); }
static inline bool AppPermission_AppPreferences_IsPinLockEnabled(void) { return AppPreferences_IsPinLockEnabled(); }
static inline void AppPermission_ssd1306_FillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height) { ssd1306_FillRect(xMove, yMove, width, height); }
static inline bool AppPermission_AppPreferences_VerifyAuthPin(const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS]) { return AppPreferences_VerifyAuthPin(pinDigits); }
static inline void AppPermission_ssd1306_Clear(void) { ssd1306_Clear(); }
static inline bool AppPermissionNodeValid(uint8_t node) { return node==UNER_NODE_PC_QT || node==UNER_NODE_WEB_APP || (node>=UNER_NODE_WEB_CLIENT_FIRST && node<=UNER_NODE_WEB_CLIENT_LAST); }
static const PermissionServices appPermissionServices={
 .now_ms=HAL_GetTick,
 .remote_node_valid=AppPermissionNodeValid,
 .AppPreferences_IsPinLockEnabled=AppPermission_AppPreferences_IsPinLockEnabled,
 .AppPreferences_VerifyAuthPin=AppPermission_AppPreferences_VerifyAuthPin,
 .MenuSys_SetCurrentScreenCode=MenuSys_SetCurrentScreenCode,
 .OledUtils_DismissNotification=OledUtils_DismissNotification,
 .OledUtils_SetNotificationsBlocked=OledUtils_SetNotificationsBlocked,
 .ssd1306_Clear=AppPermission_ssd1306_Clear,
 .ssd1306_DrawBitmap=AppPermission_ssd1306_DrawBitmap,
 .ssd1306_DrawRect=AppPermission_ssd1306_DrawRect,
 .ssd1306_FillRect=AppPermission_ssd1306_FillRect,
 .ssd1306_SetColor=AppPermission_ssd1306_SetColor,
 .ssd1306_SetCursor=AppPermission_ssd1306_SetCursor,
 .ssd1306_WriteString=AppPermission_ssd1306_WriteString,
};
extern PermissionHandle appPermission;
#include "favorites_context.h"
static inline const AppPreferences_t * AppFavorites_AppPreferences_Get(void) { return AppPreferences_Get(); }
static inline AM_Result AppFavorites_AppPreferences_ApplyPatch(const AppPreferencesPatch_t *patch,
                                            AppPreferencesFieldMask_t *changedFields) { return AppPreferences_ApplyPatch(patch, changedFields); }
static const FavoritesServices appFavoritesServices={
 .now_ms=HAL_GetTick,
 .AppPreferences_ApplyPatch=AppFavorites_AppPreferences_ApplyPatch,
 .AppPreferences_Get=AppFavorites_AppPreferences_Get,
 .MenuSys_GetItemByScreenCode=MenuSys_GetItemByScreenCode,
 .MenuSys_GoBack_Wrapper=MenuSys_GoBack_Wrapper,
 .OledUtils_RenderDashboard_Wrapper=OledUtils_RenderDashboard_Wrapper,
 .dashboardEventManager=dashboardEventManager,
};
extern FavoritesHandle appFavorites;
#include "ap_context.h"
static inline const AppPreferences_t * AppAp_AppPreferences_Get(void) { return AppPreferences_Get(); }
static inline AM_Result AppAp_AppPreferences_SetWifiApApplyPending(bool pending) { return AppPreferences_SetWifiApApplyPending(pending); }
static inline AM_Result AppAp_AppPreferences_SetWifiApConfig(const AppApConfig_t *config, bool applyPending) { return AppPreferences_SetWifiApConfig(config, applyPending); }
static inline void AppAp_AppPreferences_GetWifiApConfig(AppApConfig_t *out) { AppPreferences_GetWifiApConfig(out); }
static const ApServices appApServices={
 .now_ms=HAL_GetTick,
 .AppPreferences_Get=AppAp_AppPreferences_Get,
 .AppPreferences_GetWifiApConfig=AppAp_AppPreferences_GetWifiApConfig,
 .AppPreferences_SetWifiApApplyPending=AppAp_AppPreferences_SetWifiApApplyPending,
 .AppPreferences_SetWifiApConfig=AppAp_AppPreferences_SetWifiApConfig,
 .WifiBridge_GetActiveBackend=WifiBridge_GetActiveBackend,
 .WifiBridge_IsWaitingValidation=WifiBridge_IsWaitingValidation,
 .WifiBridge_SendCommand=WifiBridge_SendCommand,
};
extern ApHandle appAp;
#include "uirouter_context.h"
static const UiRouterServices appUiRouterServices={
 .now_ms=HAL_GetTick,
 .MenuSys_GetCurrentScreenCode=MenuSys_GetCurrentScreenCode,
 .MenuSys_NotifyScreenContentChanged=MenuSys_NotifyScreenContentChanged,
 .MenuSys_ResetMenu=MenuSys_ResetMenu,
 .OledUtils_DismissNotification=OledUtils_DismissNotification,
 .OledUtils_GetNotificationCount=OledUtils_GetNotificationCount,
 .Permission_GetRequestedPermission=Permission_GetRequestedPermission,
 .Permission_GetState=Permission_GetState,
 .Permission_HandleUserEvent=Permission_HandleUserEvent,
 .ScreenLock_ProcessEvent=ScreenLock_ProcessEvent,
 .WiFiScan_ClearResults=WiFiScan_ClearResults,
 .WifiBridge_StopScan=WifiBridge_StopScan,
};
extern UiRouterHandle appUiRouter;
static inline void AppAuxiliaryBind(void) {
    Permission_Context=&appPermission; appPermission.services=&appPermissionServices;
    appPermission.system=&autoMicroSystem;
    appPermission.menuSystem=&menuSystem;
    Favorites_Context=&appFavorites; appFavorites.services=&appFavoritesServices;
    appFavorites.encoder=&encoder;
    Ap_Context=&appAp; appAp.services=&appApServices;
    appAp.system=&autoMicroSystem;
    appAp.wifiBridge=&wifiBridge;
    UiRouter_Context=&appUiRouter; appUiRouter.services=&appUiRouterServices;
    appUiRouter.system=&autoMicroSystem;
    appUiRouter.menuSystem=&menuSystem;
    appUiRouter.wifiBridge=&wifiBridge;
    autoMicroSystem.security.permissions=&appPermission;
    autoMicroSystem.handles.favorites=&appFavorites;
    autoMicroSystem.handles.ap_config=&appAp;
    autoMicroSystem.handles.ui_router=&appUiRouter;
}
#endif
