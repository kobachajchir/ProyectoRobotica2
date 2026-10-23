#ifndef APP_MENU_SERVICES_H
#define APP_MENU_SERVICES_H
#include "app_menu_binding.h"
#include "app_preferences_binding.h"
#include "app_wifi_binding.h"
static inline bool AppMenuAvailable(void *ctx, const MenuItem *item) {
    (void)ctx;
    return item->required_ext_mask == 0u ||
           (item->required_ext_mask & MENU_EXT_MASK(AppPreferences_GetExtProfile())) != 0u;
}
static inline bool AppMenuReport(void *ctx, uint32_t code, uint8_t source) {
    return WifiBridge_ReportScreenChanged(ctx, code, source) == WIFI_BRIDGE_OK;
}
static inline bool AppMenuSelection(void *ctx, uint32_t code, uint8_t index, uint8_t count, uint8_t source) {
    return WifiBridge_ReportMenuSelectionChanged(ctx, code, index, count, source) == WIFI_BRIDGE_OK;
}
static const MenuServices appMenuServices = { &wifiBridge, AppMenuAvailable, AppMenuReport, AppMenuSelection };
#endif
