#include "uirouter_context.h"
/*
 * ui_event_router.c
 *
 *  Created on: Sep 22, 2025
 *      Author: kobac
 */

#include "ui_event_router.h"
#include "permissions.h"
#include "screen_lock.h"

#define now_ms() (UiRouter_Context->services->now_ms())
#define MenuSys_ResetMenu (UiRouter_Context->services->MenuSys_ResetMenu)
#define MenuSys_NotifyScreenContentChanged (UiRouter_Context->services->MenuSys_NotifyScreenContentChanged)
#define MenuSys_GetCurrentScreenCode (UiRouter_Context->services->MenuSys_GetCurrentScreenCode)
#define OledUtils_DismissNotification (UiRouter_Context->services->OledUtils_DismissNotification)
#define OledUtils_GetNotificationCount (UiRouter_Context->services->OledUtils_GetNotificationCount)
#define Permission_HandleUserEvent (UiRouter_Context->services->Permission_HandleUserEvent)
#define Permission_GetRequestedPermission (UiRouter_Context->services->Permission_GetRequestedPermission)
#define Permission_GetState (UiRouter_Context->services->Permission_GetState)
#define WiFiScan_ClearResults (UiRouter_Context->services->WiFiScan_ClearResults)
#define ScreenLock_ProcessEvent (UiRouter_Context->services->ScreenLock_ProcessEvent)
#define WifiBridge_StopScan (UiRouter_Context->services->WifiBridge_StopScan)
#define menuSystem (*UiRouter_Context->menuSystem)
#define wifiBridge (*UiRouter_Context->wifiBridge)
#define autoMicroSystem (*UiRouter_Context->system)
#define GET_CAR_MODE() ((CarMode_t)SYSTEM_CAR_MODE(autoMicroSystem.runtime.carModeFlags))
#define WIFIDEFAULTSEARCHTIMEOUT 1000u

UiEventRouterResult_t UiEventRouter_HandleEvent(UserEvent_t ev)
{
    if (ev == UE_NONE) {
        return UI_EVENT_ROUTER_IGNORED;
    }

    if (Permission_GetRequestedPermission() != PERMISSION_NONE &&
        Permission_GetState() != AUTH_IDLE) {
        Permission_HandleUserEvent(ev);
        MenuSys_NotifyScreenContentChanged(&menuSystem, SCREEN_REPORT_SOURCE_RENDER);
        return UI_EVENT_ROUTER_CONSUMED;
    }

    if (OledUtils_GetNotificationCount() > 0) {
        if (ev == UE_ENC_SHORT_PRESS || ev == UE_ENC_LONG_PRESS) {
            OledUtils_DismissNotification();
            return UI_EVENT_ROUTER_CONSUMED;
        }
    }

    if (ScreenLock_ProcessEvent(ev) == SCREEN_LOCK_EVENT_CONSUMED) {
        MenuSys_NotifyScreenContentChanged(&menuSystem, SCREEN_REPORT_SOURCE_RENDER);
        return UI_EVENT_ROUTER_CONSUMED;
    }

    if (ev == UE_LONG_PRESS) {
        //No deberia estar esto aca, hacer chequeo de si esta buscando antes
        const ScreenCode_t screen_code = MenuSys_GetCurrentScreenCode(&menuSystem);
        if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING)) {
            (void)WifiBridge_StopScan(&wifiBridge);
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);
            autoMicroSystem.connectivity.wifiScanSessionActive = 0u;
            autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
            autoMicroSystem.connectivity.wifiSearchingTimeout = WIFIDEFAULTSEARCHTIMEOUT;
        }
        if (screen_code == SCREEN_CODE_CONNECTIVITY_WIFI_SEARCHING ||
            screen_code == SCREEN_CODE_CONNECTIVITY_WIFI_RESULTS ||
            screen_code == SCREEN_CODE_CONNECTIVITY_WIFI_DETAILS ||
            screen_code == SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_AT) {
            WiFiScan_ClearResults();
        }
        MenuSys_ResetMenu(&menuSystem);
        if (menuSystem.dashboardRender) {
            //Aca deberia mandar el comando de reporte de modo
            menuSystem.renderFn = menuSystem.dashboardRender;
        }
        if (menuSystem.insideMenuFlag) {
            *menuSystem.insideMenuFlag = false;
        }
        if (menuSystem.clearScreen) {
            menuSystem.clearScreen();
        }
        menuSystem.renderFlag = true;
        autoMicroSystem.runtime.oled_first_draw = true;
        MenuSys_NotifyScreenContentChanged(&menuSystem, SCREEN_REPORT_SOURCE_RENDER);
        return UI_EVENT_ROUTER_CONSUMED;
    }

    if (menuSystem.userEventManagerFn) {
        menuSystem.userEventManagerFn(ev);
        MenuSys_NotifyScreenContentChanged(&menuSystem, SCREEN_REPORT_SOURCE_RENDER);
        return UI_EVENT_ROUTER_CONSUMED;
    }

    return UI_EVENT_ROUTER_IGNORED;
}
