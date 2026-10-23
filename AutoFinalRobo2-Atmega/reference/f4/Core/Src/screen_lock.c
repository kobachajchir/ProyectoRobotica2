#include "screens_internal.h"
#include "screen_lock.h"

#include <stddef.h>

#include "oled_utils.h"
#include "screenWrappers.h"


#define SCREEN_LOCK_TICKS_PER_SECOND 100u
#define SCREEN_LOCK_CONTRAST_DIM     0x04u









static bool ScreenLock_IsEligibleNow(void)
{
    if (!AppPreferences_IsScreenLockEnabled()) {
        return false;
    }

    if (!IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY) ||
        !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, OLED_ACTIVE)) {
        return false;
    }

    if (menuSystem.allowPeriodicRefresh) {
        return false;
    }

    return true;
}

static void ScreenLock_ResetIdle(void)
{
    screenLockIdleTicks10ms = 0u;
}

static void ScreenLock_Lock(void)
{
    if (screenLockLocked) {
        return;
    }

    OledUtils_DismissNotification();

    screenLockPreviousRenderFn = menuSystem.renderFn;
    screenLockPreviousEventManagerFn = menuSystem.userEventManagerFn;
    screenLockPreviousAllowPeriodicRefresh = menuSystem.allowPeriodicRefresh;
    screenLockPreviousInsideMenu = menuSystem.insideMenuFlag ? *menuSystem.insideMenuFlag : 0u;

    (void)ssd1306_SetContrast(SCREEN_LOCK_CONTRAST_DIM);

    screenLockLocked = true;
    menuSystem.allowPeriodicRefresh = false;
    menuSystem.renderFn = OledUtils_RenderScreenLocked_Wrapper;
    menuSystem.userEventManagerFn = screenLockedEventManager;
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0u;
    }
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void ScreenLock_Init(void)
{
    screenLockLocked = false;
    screenLockIdleTicks10ms = 0u;
    screenLockTimeoutDraftSec = AppPreferences_GetScreenLockTimeoutSec();
    screenLockPreviousRenderFn = NULL;
    screenLockPreviousEventManagerFn = NULL;
    screenLockPreviousAllowPeriodicRefresh = false;
    screenLockPreviousInsideMenu = 0u;
}

void ScreenLock_Task10ms(void)
{
    uint32_t timeoutTicks;

    if (screenLockLocked) {
        return;
    }

    if (!ScreenLock_IsEligibleNow()) {
        ScreenLock_ResetIdle();
        return;
    }

    timeoutTicks = (uint32_t)AppPreferences_GetScreenLockTimeoutSec() *
                   SCREEN_LOCK_TICKS_PER_SECOND;

    if (screenLockIdleTicks10ms >= timeoutTicks) {
        ScreenLock_Lock();
    } else {
        screenLockIdleTicks10ms++;
    }
}

void ScreenLock_OnPreferencesChanged(void)
{
    if (!AppPreferences_IsScreenLockEnabled() && screenLockLocked) {
        ScreenLock_Unlock();
    }

    ScreenLock_ResetIdle();
}

ScreenLock_EventResult_t ScreenLock_ProcessEvent(UserEvent_t ev)
{
    if (ev == UE_NONE) {
        return SCREEN_LOCK_EVENT_CONSUMED;
    }

    if (screenLockLocked) {
        if (ev == UE_ENC_SHORT_PRESS || ev == UE_ENC_LONG_PRESS) {
            return SCREEN_LOCK_EVENT_PASS;
        }

        return SCREEN_LOCK_EVENT_CONSUMED;
    }

    ScreenLock_ResetIdle();
    return SCREEN_LOCK_EVENT_PASS;
}

bool ScreenLock_IsLocked(void)
{
    return screenLockLocked;
}

uint16_t ScreenLock_GetTimeoutDraftSec(void)
{
    return screenLockTimeoutDraftSec;
}

void ScreenLock_BeginTimeoutEdit(void)
{
    screenLockTimeoutDraftSec = AppPreferences_GetScreenLockTimeoutSec();
}

void ScreenLock_AdjustTimeoutDraft(int16_t deltaSec)
{
    int32_t next = (int32_t)screenLockTimeoutDraftSec + (int32_t)deltaSec;

    if (next < APP_PREFS_SCREEN_LOCK_TIMEOUT_MIN_SEC) {
        next = APP_PREFS_SCREEN_LOCK_TIMEOUT_MIN_SEC;
    } else if (next > APP_PREFS_SCREEN_LOCK_TIMEOUT_MAX_SEC) {
        next = APP_PREFS_SCREEN_LOCK_TIMEOUT_MAX_SEC;
    }

    screenLockTimeoutDraftSec =
        AppPreferences_ClampScreenLockTimeoutSec((uint16_t)next);
}

void ScreenLock_SaveTimeoutDraft(void)
{
    (void)AppPreferences_SetScreenLockTimeoutSec(screenLockTimeoutDraftSec);
    ScreenLock_OnPreferencesChanged();
}

void ScreenLock_CancelTimeoutDraft(void)
{
    screenLockTimeoutDraftSec = AppPreferences_GetScreenLockTimeoutSec();
}

void ScreenLock_Unlock(void)
{
    if (!screenLockLocked) {
        return;
    }

    screenLockLocked = false;
    (void)ssd1306_SetContrast(ssd1306_GetDefaultContrast());

    menuSystem.renderFn = screenLockPreviousRenderFn ? screenLockPreviousRenderFn
                                                     : OledUtils_RenderDashboard_Wrapper;
    menuSystem.userEventManagerFn = screenLockPreviousEventManagerFn
        ? screenLockPreviousEventManagerFn
        : dashboardEventManager;
    menuSystem.allowPeriodicRefresh = screenLockPreviousAllowPeriodicRefresh;

    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = screenLockPreviousInsideMenu;
    }

    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }

    ScreenLock_ResetIdle();
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Screens_Init_screen_lock(void)
{

}
