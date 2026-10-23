#include "screens_internal.h"
/*
 * notificationWrappers.c
 *
 *  Created on: Apr 8, 2026
 */

#include "notificationWrappers.h"
#include "screenWrappers.h"
#include "types/menu_types.h"
#include "oled_utils.h"
#include "oled_handle.h"
#include "fonts.h"
#include "encoder_types.h"
#include "osk_integration.h"

#include <stdio.h>
#include <string.h>


static void OledUtils_ShowESPNotificationReturningTo(RenderFunction renderFn,
                                                    RenderFunction restoreFn,
                                                    OledNotificationHook onShow,
                                                    OledNotificationHook onHide);
static void OledUtils_ShowESPResetModeAfterPermission(void *ctx);
static void OledUtils_OpenESPResetModeMenu(void);
static void OledUtils_ReturnToESPMenu(void);
static void OledUtils_ReturnToWifiMenu(void);
static void OledUtils_SendESPResetWithMode(uint8_t boot_mode);
static void OledUtils_SendESPResetNormal(void);
static void OledUtils_SendESPResetApMode(void);
static void OledUtils_ToggleESPModeAfterPermission(void *ctx);
static void OledUtils_ToggleESPWebModeAfterPermission(void *ctx);





static void OledUtils_SendFirmwareRequest(void)
{
    if (WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_AT) {
        APP_ESPFirmwareRequestRequested();
    } else {
        (void)WifiBridge_SendCommand(
            &wifiBridge, WIFI_BRIDGE_CMD_ID_REQUEST_FIRMWARE, NULL, 0u);
    }
}

static void OledUtils_ReturnToESPMenu(void)
{
    menuSystem.currentMenu = &submenuESP;
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    encoder.allowEncoderInput = true;
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    autoMicroSystem.runtime.inside_menu_flag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void OledUtils_ReturnToWifiMenu(void)
{
    menuSystem.currentMenu = &submenu1;
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    encoder.allowEncoderInput = true;
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    autoMicroSystem.runtime.inside_menu_flag = true;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void OledUtils_SendESPResetWithMode(uint8_t boot_mode)
{
    if (WifiBridge_SendEspReboot(&wifiBridge, boot_mode) != WIFI_BRIDGE_OK) {
        return;
    }

    OledUtils_ReturnToESPMenu();
    OledUtils_ShowNotificationMs(OledUtils_RenderESPResetSentNotification, OLED_NOTIFICATION_DEFAULT_MS);
}

static void OledUtils_SendESPResetNormal(void)
{
    OledUtils_SendESPResetWithMode(WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_NORMAL);
}

static void OledUtils_SendESPResetApMode(void)
{
    OledUtils_SendESPResetWithMode(WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_AP);
}

static void OledUtils_ToggleESPModeAfterPermission(void *ctx)
{
    WifiBridge_Result result;

    (void)ctx;
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)) {
        result = WifiBridge_StopAP(&wifiBridge);
    } else {
        result = WifiBridge_StartAP(&wifiBridge);
    }

    OledUtils_ReturnToWifiMenu();
    if (result == WIFI_BRIDGE_OK) {
        APP_ESPModeChangeCommandAccepted();
        return;
    }

    Dashboard_InvalidateConnectivity();
    OledUtils_DismissNotification();
    OledUtils_ShowSimpleNotificationMsEx(
        OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
        2000u,
        NULL,
        Dashboard_ReturnAfterConnectivityNotification);
}

static void OledUtils_ToggleESPWebModeAfterPermission(void *ctx)
{
    const uint8_t bootMode = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)
        ? WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_NORMAL
        : WIFI_BRIDGE_ESP_REBOOT_BOOT_MODE_AP;

    (void)ctx;
    if (WifiBridge_SendEspReboot(&wifiBridge, bootMode) != WIFI_BRIDGE_OK) {
        return;
    }

    OledUtils_ReturnToWifiMenu();
    OledUtils_ShowNotificationMs(OledUtils_RenderESPResetSentNotification, OLED_NOTIFICATION_DEFAULT_MS);
}

void OledUtils_StartESPAP_Wrapper(void)
{
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT)) {
        (void)Permission_Request(
            PERMISSION_ESP_RESET,
            OledUtils_ToggleESPModeAfterPermission,
            NULL);
        return;
    }

    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.oled_first_draw = true;
    OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_ESP_CHECK_CONNECTION_REQUIRED, OLED_NOTIFICATION_DEFAULT_MS);
}

void OledUtils_ToggleESPWebMode_Wrapper(void)
{
    if (WifiBridge_IsConnected(&wifiBridge) &&
        WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_WEB) {
        (void)Permission_Request(
            PERMISSION_ESP_RESET,
            OledUtils_ToggleESPWebModeAfterPermission,
            NULL);
        return;
    }

    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.oled_first_draw = true;
    OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_ESP_CHECK_CONNECTION_REQUIRED, OLED_NOTIFICATION_DEFAULT_MS);
}

static void OledUtils_OpenESPResetModeMenu(void)
{
    espResetModeMenu.currentItemIndex = 0;
    espResetModeMenu.firstVisibleItem = 0;
    espResetModeMenu.lastSelectedItemIndex = -1;
    espResetModeMenu.lastVisibleItem = -1;
    espResetModeMenu.parent = &submenuESP;

    menuSystem.currentMenu = &espResetModeMenu;
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    encoder.allowEncoderInput = true;
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    autoMicroSystem.runtime.inside_menu_flag = true;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void OledUtils_ShowESPResetModeAfterPermission(void *ctx)
{
    (void)ctx;
    OledUtils_OpenESPResetModeMenu();
}

static void OledUtils_ShowESPNotificationReturningTo(RenderFunction renderFn,
                                                    RenderFunction restoreFn,
                                                    OledNotificationHook onShow,
                                                    OledNotificationHook onHide)
{
    OledUtils_DisableContinuousRender();

    if (restoreFn) {
        menuSystem.renderFn = restoreFn;
    }
    autoMicroSystem.runtime.oled_first_draw = true;
    OledUtils_ShowNotificationMsEx(renderFn, 8500u, onShow, onHide);
}

void OledUtils_RenderStartupNotification_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CORE_STARTUP,
                                 SCREEN_REPORT_SOURCE_SYSTEM);
    OledUtils_DisableContinuousRender();
    menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = false;

    menuSystem.renderFn = OledUtils_RenderDashboard_Wrapper;
    menuSystem.renderFlag = true;
}

void OledUtils_RenderESPCheckConnection_Wrapper(void)
{
    (void)0;
    if (WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_AT) {
        APP_ESPStatusRefreshRequested();
    } else {
        (void)WifiBridge_GetStatus(&wifiBridge);
    }
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.oled_first_draw = true;
    OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_ESP_CHECKING_CONNECTION, 8500u);
}

void OledUtils_RenderESPFirmwareRequest_Wrapper(void)
{
    OledNotificationHook onShowHook = NULL;

    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT)) {
        onShowHook = OledUtils_SendFirmwareRequest;
        OledUtils_ShowESPNotificationReturningTo(OledUtils_RenderESPFirmwareRequestNotification,
                                                OledUtils_RenderESPFirmwareScreen_Wrapper,
                                                onShowHook,
                                                NULL);
        return;
    }

    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.oled_first_draw = true;
    OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_ESP_CHECK_CONNECTION_REQUIRED, OLED_NOTIFICATION_DEFAULT_MS);
}

void OledUtils_RenderESPResetMode_Wrapper(void)
{
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT)) {
        (void)Permission_Request(
            PERMISSION_ESP_RESET,
            OledUtils_ShowESPResetModeAfterPermission,
            NULL);
        return;
    }

    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.oled_first_draw = true;
    OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_ESP_CHECK_CONNECTION_REQUIRED, OLED_NOTIFICATION_DEFAULT_MS);
}

/* WiFi AP Edit Callbacks */


static void WifiAp_ShowResult(AppApStatus_t status)
{
    OledUtils_ShowSimpleNotificationMs(
        status == APP_AP_STATUS_OK
            ? OLED_SIMPLE_NOTIFICATION_AP_CONFIG_SAVED
            : OLED_SIMPLE_NOTIFICATION_AP_CONFIG_INVALID,
        OLED_NOTIFICATION_DEFAULT_MS);
}

static void WifiApSsid_OnComplete(OSK_State *st, void *ctx)
{
    AppApConfigPatch_t patch = {0};
    (void)ctx;
    if (st == NULL) return;
    patch.fieldMask = APP_AP_FIELD_SSID;
    strncpy(patch.values.ssid, OSK_GetText(st), APP_AP_SSID_MAX_BYTES);
    WifiAp_ShowResult(AppApConfig_ApplyPatch(&patch, NULL));
}

static void WifiApPassword_OnComplete(OSK_State *st, void *ctx)
{
    AppApConfigPatch_t patch = {0};
    const char *password = (st != NULL) ? OSK_GetText(st) : NULL;
    (void)ctx;
    if (password == NULL || password[0] == '\0') {
        wifiApEnableProtectionAfterPassword = false;
        WifiAp_ShowResult(APP_AP_STATUS_INVALID_PASSWORD);
        return;
    }
    patch.fieldMask = APP_AP_FIELD_PASSWORD;
    if (wifiApEnableProtectionAfterPassword) {
        patch.fieldMask |= APP_AP_FIELD_PROTECTED;
        patch.values.protected = true;
    }
    strncpy(patch.values.password, password, APP_AP_PASSWORD_MAX_BYTES);
    wifiApEnableProtectionAfterPassword = false;
    WifiAp_ShowResult(AppApConfig_ApplyPatch(&patch, NULL));
}

static void WifiApIpv4_OnComplete(OSK_State *st, void *ctx)
{
    AppApConfigPatch_t patch = {0};
    AppApStatus_t status;
    (void)ctx;
    if (st == NULL) return;
    patch.fieldMask = APP_AP_FIELD_IPV4;
    status = AppApConfig_ParseIpv4(OSK_GetText(st), patch.values.ipv4);
    if (status == APP_AP_STATUS_OK) {
        status = AppApConfig_ApplyPatch(&patch, NULL);
    }
    WifiAp_ShowResult(status);
}

static void WifiApEdit_OnCancel(void *ctx)
{
    (void)ctx;
    wifiApEnableProtectionAfterPassword = false;
}

void OledUtils_WifiApSsidEdit_Wrapper(void)
{
    const OSK_AlphaConfig config = {
        .allowLowercase = true, .allowUppercase = true,
        .allowNumbers = true, .allowSymbols = true,
        .initialTextMode = OSK_INITIAL_TEXT_PREFILL,
        .allowedChars = NULL,
    };
    OSK_Integration_OpenAlpha(AppPreferences_GetWifiApSsid(),
                              APP_AP_SSID_MAX_BYTES, &config,
                              WifiApSsid_OnComplete, WifiApEdit_OnCancel, NULL);
}

void OledUtils_WifiApPasswordEdit_Wrapper(void)
{
    const OSK_AlphaConfig config = {
        .allowLowercase = true, .allowUppercase = true,
        .allowNumbers = true, .allowSymbols = true,
        .initialTextMode = OSK_INITIAL_TEXT_EMPTY,
        .allowedChars = NULL,
    };
    OSK_Integration_OpenAlpha(AppPreferences_GetWifiApPassword(),
                              APP_AP_PASSWORD_MAX_BYTES, &config,
                              WifiApPassword_OnComplete, WifiApEdit_OnCancel, NULL);
}

void OledUtils_WifiApEnableProtection_Wrapper(void)
{
    wifiApEnableProtectionAfterPassword = true;
    OledUtils_WifiApPasswordEdit_Wrapper();
    wifiApEnableProtectionAfterPassword = true;
}

void OledUtils_WifiApIpv4Edit_Wrapper(void)
{
    AppApConfig_t current;
    char initial[16];
    const OSK_AlphaConfig config = {
        .allowLowercase = false, .allowUppercase = false,
        .allowNumbers = true, .allowSymbols = true,
        .initialTextMode = OSK_INITIAL_TEXT_PREFILL,
        .allowedChars = "0123456789.",
    };
    AppApConfig_Get(&current);
    (void)snprintf(initial, sizeof(initial), "%u.%u.%u.%u",
                   current.ipv4[0], current.ipv4[1],
                   current.ipv4[2], current.ipv4[3]);
    OSK_Integration_OpenAlpha(initial, 15u, &config,
                              WifiApIpv4_OnComplete, WifiApEdit_OnCancel, NULL);
}

void Screens_Init_notificationWrappers(void)
{
    memcpy(&espResetModeItems, (MenuItem  [3]){
    { "Normal", OledUtils_SendESPResetNormal, NULL, Icon_Refrescar_bits, NULL, NULL, NULL, SCREEN_CODE_CONNECTIVITY_ESP_REBOOT_MODE },
    { "Modo AP", OledUtils_SendESPResetApMode, NULL, Icon_Wifi_bits, NULL, NULL, NULL, SCREEN_CODE_CONNECTIVITY_ESP_REBOOT_MODE },
    { "Volver", MenuSys_GoBack_Wrapper, &submenuESP, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_ESP_MENU }
}, sizeof(espResetModeItems));
    espResetModeMenu = (SubMenu ){
    .name = "Reinicio ESP",
    .items = espResetModeItems,
    .itemCount = sizeof(espResetModeItems) / sizeof(espResetModeItems[0]),
    .currentItemIndex = 0,
    .lastSelectedItemIndex = -1,
    .firstVisibleItem = 0,
    .lastVisibleItem = -1,
    .parent = &submenuESP,
    .icon = NULL,
    .screen_code = SCREEN_CODE_CONNECTIVITY_ESP_REBOOT_MODE
};
}
