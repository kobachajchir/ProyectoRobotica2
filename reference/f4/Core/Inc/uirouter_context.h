#ifndef UIROUTER_CONTEXT_H
#define UIROUTER_CONTEXT_H
#include "types/menu_types.h"
#include "permissions_types.h"
#include "screen_lock.h"
#include "wifi_bridge_types.h"
#include "system.h"
#include "system_flags.h"
#include "ui_event_router.h"
typedef struct {
    uint32_t (*now_ms)(void);
    ScreenCode_t (*MenuSys_GetCurrentScreenCode)(const MenuSystem *ms);
    void (*MenuSys_NotifyScreenContentChanged)(MenuSystem *ms, ScreenReportSource_t source);
    void (*MenuSys_ResetMenu)(MenuSystem *ms);
    void (*OledUtils_DismissNotification)(void);
    uint8_t (*OledUtils_GetNotificationCount)(void);
    PermissionId_t (*Permission_GetRequestedPermission)(void);
    AuthState_t (*Permission_GetState)(void);
    void (*Permission_HandleUserEvent)(UserEvent_t ev);
    ScreenLock_EventResult_t (*ScreenLock_ProcessEvent)(UserEvent_t ev);
    void (*WiFiScan_ClearResults)(void);
    WifiBridge_Result (*WifiBridge_StopScan)(WifiBridge_Handle *h);
} UiRouterServices;
typedef struct {
    const UiRouterServices *services;
    SystemHandle *system;
    MenuSystem *menuSystem;
    WifiBridge_Handle *wifiBridge;
} UiRouterHandle;
extern UiRouterHandle *UiRouter_Context;
#endif
