/*
 * eventManagers.c
 *
 *  Created on: Jul 23, 2025
 *      Author: kobac
 */
#include "app_wifi_binding.h"
#include "main.h"
#include "menu_favorites.h"
#include "automicro_boot_contract.h"
#include "app_preferences_binding.h"
#include "app_tasks.h"
#include "eventManagers.h"
#include "oled_canvas.h"
#include "screenWrappers.h"
#include "oled_utils.h"
#include "oled_handle.h"
#include "globals.h"
#include "permissions.h"
#include "menu_definitions.h"
#include "screen_lock.h"
#include "stm32f4xx_hal.h"
#include <string.h>

static const char wifiAtPasswordAlphabet[] =
    " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-_=+[]{};:,.?/~"
    "\"\\";




#define BOOTLOADER_RESTART_CONFIRM_REFRESH_MS 50u

// Unused variables removed

// ============================================================================
// CALLBACKS PARA DASHBOARD (CAMBIO DE MODO)
// ============================================================================




typedef enum {
    DASHBOARD_QUICK_ACTION_STABILIZE = 0u,
    DASHBOARD_QUICK_ACTION_MODE,
    DASHBOARD_QUICK_ACTION_COUNT
} DashboardQuickAction_t;


























typedef enum {
    BALANCE_PID_GAIN_KP = 0u,
    BALANCE_PID_GAIN_KI,
    BALANCE_PID_GAIN_KD
} BalancePidGainTarget_t;







#define MOTOR_MIN_PWM_LIVE_TEST_PERCENT 0.1f
#define BALANCE_PID_GAIN_MIN             0.0f
#define BALANCE_PID_GAIN_MAX             80.0f
#define BALANCE_PID_GAIN_STEP            0.05f
#define BALANCE_PID_LOG_DURATION_MIN_SEC 1u
#define BALANCE_PID_LOG_DURATION_MAX_SEC 30u
#define BALANCE_PID_LOG_DURATION_STEP_SEC 1u
#define BALANCE_PID_LOG_SAMPLE_PERIOD_MS 10u
#define BALANCE_PID_LOG_SAMPLE_BYTES     32u
#define OLED_ROTATION_MPU_FACE_UP_AZ_G    0.35f
#define OLED_ROTATION_MPU_FACE_DOWN_AZ_G -0.35f
#define OLED_ROTATION_MPU_LATERAL_TO_180_DEG 105.0f
#define OLED_ROTATION_MPU_LATERAL_TO_0_DEG    75.0f
#define OLED_ROTATION_MPU_Z_NEG_TRIGGER_DEG (-70.0f)
#define OLED_ROTATION_MPU_Z_POS_TRIGGER_DEG   70.0f
#define DASHBOARD_ACTION_TIMEOUT_BAR_DELAY_MS 1000u
#define DASHBOARD_ACTION_TIMEOUT_BAR_STEP_MS   100u


typedef struct {
    char wifiAtPasswordDraft[WIFI_BRIDGE_PASS_MAX_LEN + 1u];
    uint8_t wifiAtPasswordLength;
    uint8_t wifiAtPasswordCharacterIndex;
    bool dashboardModeCanConfirm;
    uint32_t dashboardModeLastInputMs;
    uint32_t dashboardActionTimeoutLastRenderMs;
    uint8_t dashboardQuickActionIndex;
    bool dashboardQuickActionActive;
    uint32_t dashboardQuickActionLastInputMs;
    bool motorMinPwmEditLeft;
    uint16_t motorMinPwmDraft;
    uint16_t motorMinPwmOriginal;
    uint16_t systemActionTimeoutDraft;
    uint16_t systemActionTimeoutOriginal;
    uint16_t usbAlivePeriodDraftMs;
    uint16_t usbAlivePeriodOriginalMs;
    uint16_t lineBlackWidthDraftMm;
    uint16_t lineBlackWidthOriginalMm;
    uint16_t oledRotationDraftDeg;
    uint16_t oledRotationOriginalDeg;
    uint16_t oledRotationMpuTestDeg;
    uint16_t oledRotationMpuTestOriginalDeg;
    int16_t oledRotationMpuTestYawDeg;
    uint8_t oledRotationMpuTestReady;
    Motor_Data_t motorMinPwmSavedData;
    bool motorMinPwmSaved;
    bool motorSingleTestLeft;
    float motorSingleTestSpeed;
    Motor_Direction_t motorSingleTestDirection;
    Motor_Data_t motorSingleTestSavedData;
    bool motorSingleTestSaved;
    BalancePidGainTarget_t balancePidGainTarget;
    float balancePidGainDraft;
    float balancePidGainOriginal;
    uint16_t balancePidLogDurationDraftSec;
    uint16_t balancePidLogDurationOriginalSec;
} AppEventsState;
static AppEventsState appEventsState={
    .wifiAtPasswordCharacterIndex=1u,
    .dashboardModeCanConfirm=false,
    .dashboardModeLastInputMs=0u,
    .dashboardActionTimeoutLastRenderMs=0u,
    .dashboardQuickActionIndex=DASHBOARD_QUICK_ACTION_STABILIZE,
    .dashboardQuickActionActive=false,
    .dashboardQuickActionLastInputMs=0u,
    .motorMinPwmEditLeft=true,
    .motorMinPwmDraft=APP_PREFS_MOTOR_MIN_PWM_DEFAULT,
    .motorMinPwmOriginal=APP_PREFS_MOTOR_MIN_PWM_DEFAULT,
    .systemActionTimeoutDraft=APP_PREFS_SYSTEM_ACTION_TIMEOUT_DEFAULT_SEC,
    .systemActionTimeoutOriginal=APP_PREFS_SYSTEM_ACTION_TIMEOUT_DEFAULT_SEC,
    .usbAlivePeriodDraftMs=APP_PREFS_USB_ALIVE_PERIOD_DEFAULT_MS,
    .usbAlivePeriodOriginalMs=APP_PREFS_USB_ALIVE_PERIOD_DEFAULT_MS,
    .lineBlackWidthDraftMm=APP_PREFS_LINE_BLACK_WIDTH_DEFAULT_MM,
    .lineBlackWidthOriginalMm=APP_PREFS_LINE_BLACK_WIDTH_DEFAULT_MM,
    .oledRotationDraftDeg=APP_PREFS_OLED_ROTATION_DEFAULT_DEG,
    .oledRotationOriginalDeg=APP_PREFS_OLED_ROTATION_DEFAULT_DEG,
    .oledRotationMpuTestDeg=APP_PREFS_OLED_ROTATION_DEFAULT_DEG,
    .oledRotationMpuTestOriginalDeg=APP_PREFS_OLED_ROTATION_DEFAULT_DEG,
    .oledRotationMpuTestYawDeg=0,
    .oledRotationMpuTestReady=0u,
    .motorMinPwmSaved=false,
    .motorSingleTestLeft=true,
    .motorSingleTestSpeed=MOTOR_SPEED_PERCENT_MIN,
    .motorSingleTestDirection=MOTOR_DIR_FORWARD,
    .motorSingleTestSaved=false,
    .balancePidGainTarget=BALANCE_PID_GAIN_KP,
    .balancePidGainDraft=BALANCE_CONTROL_DEFAULT_KP,
    .balancePidGainOriginal=BALANCE_CONTROL_DEFAULT_KP,
    .balancePidLogDurationDraftSec=5u,
    .balancePidLogDurationOriginalSec=5u,
};
#define wifiAtPasswordDraft (appEventsState.wifiAtPasswordDraft)
#define wifiAtPasswordLength (appEventsState.wifiAtPasswordLength)
#define wifiAtPasswordCharacterIndex (appEventsState.wifiAtPasswordCharacterIndex)
#define dashboardModeCanConfirm (appEventsState.dashboardModeCanConfirm)
#define dashboardModeLastInputMs (appEventsState.dashboardModeLastInputMs)
#define dashboardActionTimeoutLastRenderMs (appEventsState.dashboardActionTimeoutLastRenderMs)
#define dashboardQuickActionIndex (appEventsState.dashboardQuickActionIndex)
#define dashboardQuickActionActive (appEventsState.dashboardQuickActionActive)
#define dashboardQuickActionLastInputMs (appEventsState.dashboardQuickActionLastInputMs)
#define motorMinPwmEditLeft (appEventsState.motorMinPwmEditLeft)
#define motorMinPwmDraft (appEventsState.motorMinPwmDraft)
#define motorMinPwmOriginal (appEventsState.motorMinPwmOriginal)
#define systemActionTimeoutDraft (appEventsState.systemActionTimeoutDraft)
#define systemActionTimeoutOriginal (appEventsState.systemActionTimeoutOriginal)
#define usbAlivePeriodDraftMs (appEventsState.usbAlivePeriodDraftMs)
#define usbAlivePeriodOriginalMs (appEventsState.usbAlivePeriodOriginalMs)
#define lineBlackWidthDraftMm (appEventsState.lineBlackWidthDraftMm)
#define lineBlackWidthOriginalMm (appEventsState.lineBlackWidthOriginalMm)
#define oledRotationDraftDeg (appEventsState.oledRotationDraftDeg)
#define oledRotationOriginalDeg (appEventsState.oledRotationOriginalDeg)
#define oledRotationMpuTestDeg (appEventsState.oledRotationMpuTestDeg)
#define oledRotationMpuTestOriginalDeg (appEventsState.oledRotationMpuTestOriginalDeg)
#define oledRotationMpuTestYawDeg (appEventsState.oledRotationMpuTestYawDeg)
#define oledRotationMpuTestReady (appEventsState.oledRotationMpuTestReady)
#define motorMinPwmSavedData (appEventsState.motorMinPwmSavedData)
#define motorMinPwmSaved (appEventsState.motorMinPwmSaved)
#define motorSingleTestLeft (appEventsState.motorSingleTestLeft)
#define motorSingleTestSpeed (appEventsState.motorSingleTestSpeed)
#define motorSingleTestDirection (appEventsState.motorSingleTestDirection)
#define motorSingleTestSavedData (appEventsState.motorSingleTestSavedData)
#define motorSingleTestSaved (appEventsState.motorSingleTestSaved)
#define balancePidGainTarget (appEventsState.balancePidGainTarget)
#define balancePidGainDraft (appEventsState.balancePidGainDraft)
#define balancePidGainOriginal (appEventsState.balancePidGainOriginal)
#define balancePidLogDurationDraftSec (appEventsState.balancePidLogDurationDraftSec)
#define balancePidLogDurationOriginalSec (appEventsState.balancePidLogDurationOriginalSec)

static uint8_t Dashboard_WrapQuickAction(int16_t delta)
{
    int16_t next = (int16_t)dashboardQuickActionIndex + delta;

    while (next < 0) {
        next = (int16_t)(next + DASHBOARD_QUICK_ACTION_COUNT);
    }
    while (next >= DASHBOARD_QUICK_ACTION_COUNT) {
        next = (int16_t)(next - DASHBOARD_QUICK_ACTION_COUNT);
    }

    return (uint8_t)next;
}

static CarMode_t Dashboard_WrapMode(CarMode_t mode, int16_t delta)
{
    int16_t next = (int16_t)mode + delta;

    while (next < 0) {
        next = (int16_t)(next + CAR_MODE_MAX);
    }
    while (next >= CAR_MODE_MAX) {
        next = (int16_t)(next - CAR_MODE_MAX);
    }

    return (CarMode_t)next;
}

static RenderFunction Dashboard_GetQuickActionRender(uint8_t action)
{
    static const RenderFunction renders[DASHBOARD_QUICK_ACTION_COUNT] = {
        OledUtils_RenderStabilizationPrompt_Wrapper,
        OledUtils_RenderModePrompt_Wrapper,
    };

    if (action >= DASHBOARD_QUICK_ACTION_COUNT) {
        action = DASHBOARD_QUICK_ACTION_STABILIZE;
    }

    return renders[action];
}

static void Dashboard_TouchQuickAction(void)
{
    dashboardQuickActionActive = true;
    dashboardQuickActionLastInputMs = HAL_GetTick();
    dashboardActionTimeoutLastRenderMs = 0u;
}

static void Dashboard_OpenQuickAction(uint8_t action)
{
    dashboardModeCanConfirm = false;
    dashboardModeLastInputMs = 0u;
    dashboardQuickActionIndex = (action < DASHBOARD_QUICK_ACTION_COUNT)
        ? action
        : DASHBOARD_QUICK_ACTION_STABILIZE;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);

    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0u;
    }

    menuSystem.renderFn = Dashboard_GetQuickActionRender(dashboardQuickActionIndex);
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
    Dashboard_TouchQuickAction();
}

static void Dashboard_StartModeSelection(void)
{
    dashboardQuickActionActive = false;
    dashboardQuickActionLastInputMs = 0u;
    dashboardActionTimeoutLastRenderMs = 0u;
    autoMicroSystem.runtime.auxCarMode = GET_CAR_MODE();
    SET_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);
    dashboardModeCanConfirm = true;
    dashboardModeLastInputMs = HAL_GetTick();

    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0u;
    }
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }
    menuSystem.userEventManagerFn = dashboardEventManager;
    menuSystem.renderFn = OledUtils_RenderModeChange_Full;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void Dashboard_ApplyPendingMode(void *ctx)
{
    (void)ctx;
    (void)APP_TransitionCarMode(autoMicroSystem.runtime.auxCarMode);
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);
    dashboardModeCanConfirm = false;
    dashboardModeLastInputMs = 0u;
    dashboardQuickActionActive = false;
    dashboardQuickActionLastInputMs = 0u;
    dashboardActionTimeoutLastRenderMs = 0u;
    menuSystem.renderFn = OledUtils_RenderDashboard_Wrapper;
    menuSystem.clearScreen();
    menuSystem.renderFlag = true;
}

static void Dashboard_BackToDashboard(void)
{
    dashboardModeCanConfirm = false;
    dashboardModeLastInputMs = 0u;
    dashboardQuickActionActive = false;
    dashboardQuickActionLastInputMs = 0u;
    dashboardActionTimeoutLastRenderMs = 0u;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);
    menuSystem.renderFn = menuSystem.dashboardRender
        ? menuSystem.dashboardRender
        : OledUtils_RenderDashboard_Wrapper;
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0;
    }
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void Dashboard_OpenStabilizationPrompt(void)
{
    Dashboard_OpenQuickAction(DASHBOARD_QUICK_ACTION_STABILIZE);
}

static void Dashboard_TouchModeSelection(void)
{
    dashboardModeLastInputMs = HAL_GetTick();
    dashboardActionTimeoutLastRenderMs = 0u;
}

static uint32_t Dashboard_GetActionTimeoutMs(void)
{
    return (uint32_t)AppPreferences_GetSystemActionTimeoutSec() * 1000u;
}

static void Dashboard_CancelPendingMode(void)
{
    dashboardModeCanConfirm = false;
    dashboardModeLastInputMs = 0u;
    dashboardActionTimeoutLastRenderMs = 0u;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);
    menuSystem.renderFn = menuSystem.dashboardRender
        ? menuSystem.dashboardRender
        : OledUtils_RenderDashboard_Wrapper;
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void Dashboard_OnRotateCW(void)
{
    if (!IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE)) {
        // Primera rotación
        Dashboard_StartModeSelection();
        menuSystem.renderFn = OledUtils_RenderModeChange_Full;
    } else {
        // Rotaciones subsiguientes
        menuSystem.renderFn = OledUtils_RenderModeChange_ModeOnly;
    }
    autoMicroSystem.runtime.auxCarMode = Dashboard_WrapMode(autoMicroSystem.runtime.auxCarMode, 1);
    Dashboard_TouchModeSelection();
    dashboardModeCanConfirm = true;
    menuSystem.renderFlag = true;
}

static void Dashboard_OnRotateCCW(void)
{
    if (!IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE)) {
        // Primera rotación
        Dashboard_StartModeSelection();
        menuSystem.renderFn = OledUtils_RenderModeChange_Full;
    } else {
        // Rotaciones subsiguientes
        menuSystem.renderFn = OledUtils_RenderModeChange_ModeOnly;
    }
    autoMicroSystem.runtime.auxCarMode = Dashboard_WrapMode(autoMicroSystem.runtime.auxCarMode, -1);
    Dashboard_TouchModeSelection();
    dashboardModeCanConfirm = true;
    menuSystem.renderFlag = true;
}

static void Dashboard_OnShortPress(void)
{
    if (dashboardModeCanConfirm && IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE)) {
        Dashboard_ApplyPendingMode(NULL);
        return;
    }

    Dashboard_OpenStabilizationPrompt();
}

static void Dashboard_OnUserButton(void)
{
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE)) {
        Dashboard_CancelPendingMode();
        return;
    }

    dashboardModeCanConfirm = false;
    dashboardModeLastInputMs = 0u;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);
    MenuSys_NavigateToMain(&menuSystem);
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.clearScreen();
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void Dashboard_OnLongPress(void)
{
    dashboardModeCanConfirm = false;
    dashboardModeLastInputMs = 0u;
    dashboardActionTimeoutLastRenderMs = 0u;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);
    if (menuSystem.dashboardRender) {
        menuSystem.renderFn = menuSystem.dashboardRender;
    }
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0;
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void Dashboard_OnEncLongPress(void)
{
    dashboardModeCanConfirm = false;
    dashboardModeLastInputMs = 0u;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);

    SubMenu *favs = MenuSys_GetFavoritesSubMenu();
    favs->currentItemIndex     = 0;
    favs->firstVisibleItem     = 0;
    favs->lastSelectedItemIndex = -1;
    favs->lastVisibleItem       = -1;

    MenuSys_OpenSubMenu(&menuSystem, favs);
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.clearScreen();
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static const EventCallbacks_t dashboardCallbacks = {
    .onRotateCW     = Dashboard_OnRotateCW,
    .onRotateCCW    = Dashboard_OnRotateCCW,
    .onShortPress   = Dashboard_OnShortPress,
    .onLongPress    = Dashboard_OnLongPress,
    .onUserButton   = Dashboard_OnUserButton,
    .onEncLongPress = Dashboard_OnEncLongPress
};

// ============================================================================
// CALLBACKS PARA MENÚ
// ============================================================================

// ============================================================================
// CALLBACKS PARA PANTALLA DE ESTABILIZACION
// ============================================================================

static void DashboardQuickAction_OnShortPress(void)
{
    dashboardQuickActionActive = false;
    dashboardQuickActionLastInputMs = 0u;

    if (dashboardQuickActionIndex == DASHBOARD_QUICK_ACTION_MODE) {
        Dashboard_StartModeSelection();
        return;
    }

    if (MotionControl_IsArmed(&motionControl)) {
        (void)APP_BalanceSetEnabled(false);
    } else {
        (void)APP_BalanceSetEnabled(true);
    }

    Dashboard_BackToDashboard();
}

static void DashboardQuickAction_OnRotateCW(void)
{
    Dashboard_OpenQuickAction(Dashboard_WrapQuickAction(1));
}

static void DashboardQuickAction_OnRotateCCW(void)
{
    Dashboard_OpenQuickAction(Dashboard_WrapQuickAction(-1));
}

static const EventCallbacks_t stabilizationPromptCallbacks = {
    .onRotateCW     = DashboardQuickAction_OnRotateCW,
    .onRotateCCW    = DashboardQuickAction_OnRotateCCW,
    .onShortPress   = DashboardQuickAction_OnShortPress,
    .onLongPress    = Dashboard_BackToDashboard,
    .onUserButton   = Dashboard_BackToDashboard,
    .onEncLongPress = Dashboard_BackToDashboard
};

static void Menu_OnRotateCW(void)
{
    MenuSys_MoveCursorUp(&menuSystem);
    menuSystem.renderFlag = true;
}

static void Menu_OnRotateCCW(void)
{
    MenuSys_MoveCursorDown(&menuSystem);
    menuSystem.renderFlag = true;
}

static void Menu_OnShortPress(void)
{
    MenuSys_HandleClick(&menuSystem);
}

static void Menu_OnEncLongPress(void)
{
    SubMenu *m = menuSystem.currentMenu;
    if (!m) return;

    int8_t idx = m->currentItemIndex;
    if (idx < 0 || idx >= m->itemCount) return;

    const MenuItem *item = &m->items[idx];
    int8_t result = MenuSys_ToggleFavorite(item);

    if (result == 1) {
        OledUtils_ShowSimpleNotificationMs(
            OLED_SIMPLE_NOTIFICATION_FAVORITE_ADDED, 1500u);
    } else if (result == 0) {
        OledUtils_ShowSimpleNotificationMs(
            OLED_SIMPLE_NOTIFICATION_FAVORITE_REMOVED, 1500u);
    } else {
        /* -1: can't add (full or invalid item like Volver) */
        if (MenuSys_CanBeFavorite(item)) {
            OledUtils_ShowSimpleNotificationMs(
                OLED_SIMPLE_NOTIFICATION_FAVORITE_FULL, 1500u);
        }
    }
}

static void Menu_OnUserButton(void)
{
    if (menuSystem.currentMenu == &mainMenu) {
        Dashboard_BackToDashboard();
    } else {
        MenuSys_NavigateBack(&menuSystem);
    }
}

static void Menu_OnLongPress(void)
{

}

static const EventCallbacks_t menuCallbacks = {
    .onRotateCW     = Menu_OnRotateCW,
    .onRotateCCW    = Menu_OnRotateCCW,
    .onShortPress   = Menu_OnShortPress,
    .onLongPress    = Menu_OnLongPress,
    .onUserButton   = Menu_OnUserButton,
    .onEncLongPress = Menu_OnEncLongPress
};

// ============================================================================
// CALLBACKS PARA ABOUT/PROYECTO
// ============================================================================

static void About_OnRotate(void)
{
    // Toggle de la flag
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN)) {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN);
    } else {
        SET_FLAG(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN);
    }

    // Actualizar render con el wrapper
    menuSystem.clearScreen();
    menuSystem.renderFn = OledUtils_About_Wrapper;
    menuSystem.renderFlag = true;
}

static void About_OnShortPress(void)
{
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN);
    MenuSys_NavigateToMain(&menuSystem);
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.clearScreen();
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void About_OnLongPress(void)
{
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN);
    if (menuSystem.dashboardRender) {
        menuSystem.renderFn = menuSystem.dashboardRender;
    }
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0;
    }
    menuSystem.clearScreen();
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static const EventCallbacks_t aboutCallbacks = {
    .onRotateCW     = About_OnRotate,
    .onRotateCCW    = About_OnRotate,
    .onShortPress   = About_OnShortPress,
    .onLongPress    = About_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = NULL
};

typedef struct {
    uint8_t active;
    uint32_t startedMs;
    uint32_t lastRenderMs;
    uint32_t timeoutMs;
    const char *line1;
    const char *line2;
    GenericConfirmAction_t onConfirm;
    GenericConfirmCancel_t onCancel;
    uint32_t screenCode;
    void (*prevRenderFn)(void);
    void (*prevUserEventManagerFn)(UserEvent_t);
    uint8_t prevInsideMenuFlag;
} GenericConfirmContext_t;

static GenericConfirmContext_t genericConfirmCtx = {0};

static void GenericConfirm_ReturnToPrevious(void)
{
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = genericConfirmCtx.prevInsideMenuFlag;
    }
    menuSystem.renderFn = genericConfirmCtx.prevRenderFn;
    menuSystem.userEventManagerFn = genericConfirmCtx.prevUserEventManagerFn;
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void GenericConfirm_Cancel(void)
{
    genericConfirmCtx.active = 0u;
    if (genericConfirmCtx.onCancel) {
        genericConfirmCtx.onCancel();
    }
    GenericConfirm_ReturnToPrevious();
}

static void GenericConfirm_OnShortPress(void)
{
    if (!genericConfirmCtx.active) {
        return;
    }
    
    genericConfirmCtx.active = 0u;
    if (genericConfirmCtx.onConfirm) {
        genericConfirmCtx.onConfirm();
    }
    
    GenericConfirm_ReturnToPrevious();
}

static const EventCallbacks_t genericConfirmCallbacks = {
    .onRotateCW     = NULL,
    .onRotateCCW    = NULL,
    .onShortPress   = GenericConfirm_OnShortPress,
    .onLongPress    = NULL,
    .onUserButton   = NULL,
    .onEncLongPress = NULL
};

void GenericConfirm_Begin(const char* line1, const char* line2,
                          GenericConfirmAction_t onConfirm,
                          GenericConfirmCancel_t onCancel,
                          uint32_t timeoutMs,
                          uint32_t screenCode)
{
    genericConfirmCtx.active = 1u;
    genericConfirmCtx.line1 = line1;
    genericConfirmCtx.line2 = line2;
    genericConfirmCtx.onConfirm = onConfirm;
    genericConfirmCtx.onCancel = onCancel;
    genericConfirmCtx.timeoutMs = timeoutMs;
    genericConfirmCtx.screenCode = screenCode;
    genericConfirmCtx.startedMs = HAL_GetTick();
    genericConfirmCtx.lastRenderMs = genericConfirmCtx.startedMs;
    
    genericConfirmCtx.prevRenderFn = menuSystem.renderFn;
    genericConfirmCtx.prevUserEventManagerFn = menuSystem.userEventManagerFn;
    if (menuSystem.insideMenuFlag) {
        genericConfirmCtx.prevInsideMenuFlag = *menuSystem.insideMenuFlag;
    } else {
        genericConfirmCtx.prevInsideMenuFlag = 0u;
    }
}

void GenericConfirmEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &genericConfirmCallbacks);
}

void GenericConfirm_Tick(void)
{
    uint32_t now;

    if (!genericConfirmCtx.active) {
        return;
    }

    now = HAL_GetTick();
    if ((uint32_t)(now - genericConfirmCtx.startedMs) >= genericConfirmCtx.timeoutMs) {
        GenericConfirm_Cancel();
        return;
    }

    if ((uint32_t)(now - genericConfirmCtx.lastRenderMs) >= BOOTLOADER_RESTART_CONFIRM_REFRESH_MS) {
        genericConfirmCtx.lastRenderMs = now;
        menuSystem.renderFlag = true;
    }
}

uint8_t GenericConfirm_GetTimeoutContext(uint32_t *startedMs, uint32_t *timeoutMs)
{
    if (!genericConfirmCtx.active || startedMs == NULL || timeoutMs == NULL) {
        return 0u;
    }

    *startedMs = genericConfirmCtx.startedMs;
    *timeoutMs = genericConfirmCtx.timeoutMs;
    return 1u;
}

const char* GenericConfirm_GetLine1(void)
{
    return genericConfirmCtx.line1;
}

const char* GenericConfirm_GetLine2(void)
{
    return genericConfirmCtx.line2;
}

uint32_t GenericConfirm_GetScreenCode(void)
{
    return genericConfirmCtx.screenCode;
}

static void BootV2Restart_Execute(void)
{
    MotionControl_EmergencyStop(&motionControl);
    (void)APP_BalanceSetEnabled(false);
    CLEAR_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    MotorDrive_StopAll(&motorTask);

    if (!AutoMicroBootMailbox_WriteRequest(AUTOMICRO_BOOT_REQUEST_VISUAL_MENU)) {
        MenuSys_GoBack_Wrapper();
        return;
    }

    genericConfirmCtx.active = 0u;
    __DSB();
    __ISB();
    NVIC_SystemReset();

    for (;;) {
        __NOP();
    }
}

void BootV2RestartConfirm_ActionBegin(void)
{
    GenericConfirm_Begin("BootV2", NULL,
                         BootV2Restart_Execute,
                         NULL,
                         OLED_NOTIFICATION_DEFAULT_MS,
                         SCREEN_CODE_SETTINGS_SYSTEM_BOOTLOADER_CONFIRM);
}

static void BootSTRestart_Execute(void)
{
    MotionControl_EmergencyStop(&motionControl);
    (void)APP_BalanceSetEnabled(false);
    CLEAR_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    MotorDrive_StopAll(&motorTask);

    if (!AutoMicroBootMailbox_WriteRequest(AUTOMICRO_BOOT_REQUEST_ROM_BOOTLOADER)) {
        MenuSys_GoBack_Wrapper();
        return;
    }

    genericConfirmCtx.active = 0u;
    __DSB();
    __ISB();
    NVIC_SystemReset();

    for (;;) {
        __NOP();
    }
}

void BootSTRestartConfirm_ActionBegin(void)
{
    GenericConfirm_Begin("BootST", NULL,
                         BootSTRestart_Execute,
                         NULL,
                         OLED_NOTIFICATION_DEFAULT_MS,
                         SCREEN_CODE_SETTINGS_SYSTEM_BOOTLOADER_CONFIRM);
}

// ============================================================================
// CALLBACKS PARA MOTOR TEST
// ============================================================================

static void MotorTest_AdjustSpeed(Motor_Config_t *cfg, float delta)
{
    if (cfg == NULL) {
        return;
    }

    cfg->speedPercent = Motor_ClampSpeedPercent(cfg->speedPercent + delta);
}

static void MotorTest_ToggleDirection(Motor_Config_t *cfg)
{
    if (cfg == NULL) {
        return;
    }

    cfg->motorDirection = (cfg->motorDirection == MOTOR_DIR_FORWARD)
                          ? MOTOR_DIR_BACKWARD
                          : MOTOR_DIR_FORWARD;
}

static void MotorTest_OnRotateCW(void)
{
    uint8_t sel = NIBBLEH_GET_STATE(motorTask.motorData.flags);

    if (sel == MOTORLEFT_SELECTED) {
        MotorTest_AdjustSpeed(&motorTask.motorData.motorLeft, MOTOR_TEST_SPEED_STEP_PERCENT);
    } else if (sel == MOTORRIGHT_SELECTED) {
        MotorTest_AdjustSpeed(&motorTask.motorData.motorRight, MOTOR_TEST_SPEED_STEP_PERCENT);
    } else {
        MotorTest_AdjustSpeed(&motorTask.motorData.motorLeft, MOTOR_TEST_SPEED_STEP_PERCENT);
        MotorTest_AdjustSpeed(&motorTask.motorData.motorRight, MOTOR_TEST_SPEED_STEP_PERCENT);
    }
    menuSystem.renderFlag = true;
}

static void MotorTest_OnRotateCCW(void)
{
    uint8_t sel = NIBBLEH_GET_STATE(motorTask.motorData.flags);

    if (sel == MOTORLEFT_SELECTED) {
        MotorTest_AdjustSpeed(&motorTask.motorData.motorLeft, -MOTOR_TEST_SPEED_STEP_PERCENT);
    } else if (sel == MOTORRIGHT_SELECTED) {
        MotorTest_AdjustSpeed(&motorTask.motorData.motorRight, -MOTOR_TEST_SPEED_STEP_PERCENT);
    } else {
        MotorTest_AdjustSpeed(&motorTask.motorData.motorLeft, -MOTOR_TEST_SPEED_STEP_PERCENT);
        MotorTest_AdjustSpeed(&motorTask.motorData.motorRight, -MOTOR_TEST_SPEED_STEP_PERCENT);
    }
    menuSystem.renderFlag = true;
}

static void MotorTest_OnShortPress(void)
{
    uint8_t sel = NIBBLEH_GET_STATE(motorTask.motorData.flags);

    if (sel == MOTORLEFT_SELECTED) {
        MotorTest_ToggleDirection(&motorTask.motorData.motorLeft);
    } else if (sel == MOTORRIGHT_SELECTED) {
        MotorTest_ToggleDirection(&motorTask.motorData.motorRight);
    } else {
        MotorTest_ToggleDirection(&motorTask.motorData.motorLeft);
        motorTask.motorData.motorRight.motorDirection = motorTask.motorData.motorLeft.motorDirection;
    }
    menuSystem.renderFlag = true;
}

static void MotorTest_OnEncLongPress(void)
{
    uint8_t sel = NIBBLEH_GET_STATE(motorTask.motorData.flags);
    sel = (sel + 1) % 3;
    NIBBLEH_SET_STATE(motorTask.motorData.flags, sel);
    menuSystem.renderFlag = true;
}

static void MotorTest_OnUserButton(void)
{
    if (IS_FLAG_SET(motorTask.motorData.flags, ENABLE_MOVEMENT)) {
        CLEAR_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    } else {
        SET_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    }
    menuSystem.renderFlag = true;
}

static void MotorTest_OnLongPress(void)
{
    if (menuSystem.dashboardRender) {
        menuSystem.renderFn = menuSystem.dashboardRender;
    }
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0;
    }
    CLEAR_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    menuSystem.clearScreen();
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static const EventCallbacks_t motorTestCallbacks = {
    .onRotateCW     = MotorTest_OnRotateCW,
    .onRotateCCW    = MotorTest_OnRotateCCW,
    .onShortPress   = MotorTest_OnShortPress,
    .onLongPress    = MotorTest_OnLongPress,
    .onUserButton   = MotorTest_OnUserButton,
    .onEncLongPress = MotorTest_OnEncLongPress
};

static void MotorSingleTest_SaveState(void)
{
    motorSingleTestSavedData = motorTask.motorData;
    motorSingleTestSaved = true;
}

static void MotorSingleTest_Apply(void)
{
    SET_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    NIBBLEH_SET_STATE(motorTask.motorData.flags,
                      motorSingleTestLeft ? MOTORLEFT_SELECTED : MOTORRIGHT_SELECTED);

    motorTask.motorData.motorLeft.speedPercent = motorSingleTestLeft
        ? motorSingleTestSpeed
        : MOTOR_SPEED_PERCENT_MIN;
    motorTask.motorData.motorRight.speedPercent = motorSingleTestLeft
        ? MOTOR_SPEED_PERCENT_MIN
        : motorSingleTestSpeed;
    motorTask.motorData.motorLeft.motorDirection = motorSingleTestDirection;
    motorTask.motorData.motorRight.motorDirection = motorSingleTestDirection;

    MotorDrive_ApplyTestState(&motorTask);
}

/* Keep context reads on each side of clearScreen: the callback can change it. */
static void EventManagers_RestoreMenu(void)
{
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1u;
    }

    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.userEventManagerFn = menuEventManager;
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void MotorSingleTest_ReturnToMenu(void)
{
    if (motorSingleTestSaved) {
        motorTask.motorData = motorSingleTestSavedData;
        motorSingleTestSaved = false;
        MotorDrive_ApplyTestState(&motorTask);
    } else {
        MotorDrive_StopAll(&motorTask);
    }

    EventManagers_RestoreMenu();
}

static void MotorSingleTest_OnRotateCW(void)
{
    motorSingleTestSpeed = Motor_ClampSpeedPercent(motorSingleTestSpeed + MOTOR_TEST_SPEED_STEP_PERCENT);
    MotorSingleTest_Apply();
    menuSystem.renderFlag = true;
}

static void MotorSingleTest_OnRotateCCW(void)
{
    motorSingleTestSpeed = Motor_ClampSpeedPercent(motorSingleTestSpeed - MOTOR_TEST_SPEED_STEP_PERCENT);
    MotorSingleTest_Apply();
    menuSystem.renderFlag = true;
}

static void MotorSingleTest_OnUserButton(void)
{
    motorSingleTestDirection = (motorSingleTestDirection == MOTOR_DIR_FORWARD)
        ? MOTOR_DIR_BACKWARD
        : MOTOR_DIR_FORWARD;
    MotorSingleTest_Apply();
    menuSystem.renderFlag = true;
}

static const EventCallbacks_t motorSingleTestCallbacks = {
    .onRotateCW     = MotorSingleTest_OnRotateCW,
    .onRotateCCW    = MotorSingleTest_OnRotateCCW,
    .onShortPress   = MotorSingleTest_OnUserButton,
    .onLongPress    = MotorSingleTest_ReturnToMenu,
    .onUserButton   = MotorSingleTest_ReturnToMenu,
    .onEncLongPress = MotorSingleTest_ReturnToMenu
};

void MotorSingleTest_BeginLeft(void)
{
    motorSingleTestLeft = true;
    motorSingleTestSpeed = MOTOR_SPEED_PERCENT_MIN;
    motorSingleTestDirection = MOTOR_DIR_FORWARD;
    MotorSingleTest_SaveState();
    MotorSingleTest_Apply();
    autoMicroSystem.runtime.oled_first_draw = false;
}

void MotorSingleTest_BeginRight(void)
{
    motorSingleTestLeft = false;
    motorSingleTestSpeed = MOTOR_SPEED_PERCENT_MIN;
    motorSingleTestDirection = MOTOR_DIR_FORWARD;
    MotorSingleTest_SaveState();
    MotorSingleTest_Apply();
    autoMicroSystem.runtime.oled_first_draw = false;
}

ScreenCode_t MotorSingleTest_GetScreenCode(void)
{
    return motorSingleTestLeft
        ? SCREEN_CODE_MOTOR_LEFT_TEST
        : SCREEN_CODE_MOTOR_RIGHT_TEST;
}

const char *MotorSingleTest_GetLabel(void)
{
    return motorSingleTestLeft ? "Motor izquierdo" : "Motor derecho";
}

const char *MotorSingleTest_GetDirectionLabel(void)
{
    return (motorSingleTestDirection == MOTOR_DIR_FORWARD) ? "Adelante" : "Atras";
}

uint8_t MotorSingleTest_GetSpeedPercentDisplay(void)
{
    return Motor_SpeedPercentToDisplay(motorSingleTestSpeed);
}

uint16_t MotorSingleTest_GetPwm(void)
{
    const Motor_Handle_t *motor = motorSingleTestLeft
        ? &motorTask.leftMotor
        : &motorTask.rightMotor;

    return Motor_SpeedPercentToPwm(motor, motorSingleTestSpeed, motorSingleTestDirection);
}

uint16_t MotorSingleTest_GetMinPwm(void)
{
    const Motor_Handle_t *motor = motorSingleTestLeft
        ? &motorTask.leftMotor
        : &motorTask.rightMotor;

    return (motorSingleTestDirection == MOTOR_DIR_FORWARD)
        ? motor->min_pwm_forward
        : motor->min_pwm_backward;
}

uint16_t MotorSingleTest_GetMaxPwm(void)
{
    const Motor_Handle_t *motor = motorSingleTestLeft
        ? &motorTask.leftMotor
        : &motorTask.rightMotor;

    return motor->max_pwm;
}

static void MPUCalibration_ReturnToMenu(void)
{
    EventManagers_RestoreMenu();
}

static void MPUCalibration_OnUserButton(void)
{
    if (APP_MPU_CanRecalibrate()) {
        APP_MPU_RequestCalibration();
    }
}

static const EventCallbacks_t mpuCalibrationCallbacks = {
    .onRotateCW     = NULL,
    .onRotateCCW    = NULL,
    .onShortPress   = MPUCalibration_ReturnToMenu,
    .onLongPress    = NULL,
    .onUserButton   = MPUCalibration_OnUserButton,
    .onEncLongPress = MPUCalibration_ReturnToMenu
};

void MPUCalibration_RequestFromMenu(void)
{
    APP_MPU_RequestCalibration();
}

static void WiFiResults_BackToWifiMenu(void)
{
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }

    MenuSys_NavigateBack(&menuSystem);
    WiFiScan_ClearResults();
    menuSystem.clearScreen();
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void WiFiResults_RestartScan(void)
{
    autoMicroSystem.connectivity.wifiSearchingTimeout = WIFIDEFAULTSEARCHTIMEOUT;
    WiFiScan_ClearResults();
    autoMicroSystem.connectivity.wifiScanSessionActive = 1u;
    autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);

    menuSystem.renderFn = OledUtils_RenderWiFiSearching_Wrapper;
    menuSystem.clearScreen();
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = false;
}

// ============================================================================
// CALLBACKS PARA BÚSQUEDA WIFI
// ============================================================================

/**
 * @brief Cancela la búsqueda WiFi y regresa al dashboard.
 */
static void WiFiSearch_Cancel(void)
{
    (void)WifiBridge_StopScan(&wifiBridge);

    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);
    autoMicroSystem.connectivity.wifiSearchingTimeout = WIFIDEFAULTSEARCHTIMEOUT;
    autoMicroSystem.connectivity.wifiScanSessionActive = 0u;
    autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
    WiFiScan_ClearResults();

    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }

    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.clearScreen();
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static void WiFiSearch_OnShortPress(void)
{
    WiFiSearch_Cancel();
}

static void WiFiSearch_OnLongPress(void)
{
    (void)WifiBridge_StopScan(&wifiBridge);
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);
    autoMicroSystem.connectivity.wifiScanSessionActive = 0u;
    autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
    WiFiScan_ClearResults();
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.clearScreen();
    autoMicroSystem.connectivity.wifiSearchingTimeout = WIFIDEFAULTSEARCHTIMEOUT;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static const EventCallbacks_t wifiSearchCallbacks = {
    .onRotateCW     = NULL,  // No hacer nada al rotar durante búsqueda
    .onRotateCCW    = NULL,
    .onShortPress   = WiFiSearch_OnShortPress,
    .onLongPress    = WiFiSearch_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = NULL
};

// ============================================================================
// CALLBACKS PARA RESULTADOS WIFI
// ============================================================================

static void WiFiResults_OnRotateCW(void)
{
    MenuSys_MoveCursorUp(&menuSystem);
    menuSystem.renderFlag = true;
}

static void WiFiResults_OnRotateCCW(void)
{
    MenuSys_MoveCursorDown(&menuSystem);
    menuSystem.renderFlag = true;
}

static void WiFiResults_OnShortPress(void)
{
    SubMenu *menu = menuSystem.currentMenu;
    int8_t idx = (menu != NULL) ? menu->currentItemIndex : -1;

    if (!menu || idx < 0 || idx >= menu->itemCount || menu->itemCount < 2) {
        return;
    }

    if (idx == (menu->itemCount - 2)) {
        WiFiResults_RestartScan();
        return;
    }

    if (idx == (menu->itemCount - 1)) {
        WiFiResults_BackToWifiMenu();
        return;
    }

    if (autoMicroSystem.connectivity.networksFound > 0u && idx < autoMicroSystem.connectivity.networksFound) {
        autoMicroSystem.connectivity.wifiSelectedNetworkIndex = (uint8_t)idx;
        if (WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_WEB) {
            autoMicroSystem.connectivity.wifiNetworkDetailValid[autoMicroSystem.connectivity.wifiSelectedNetworkIndex] = 0u;
            autoMicroSystem.connectivity.wifiNetworkSignalStrengths[autoMicroSystem.connectivity.wifiSelectedNetworkIndex] = 0;
            autoMicroSystem.connectivity.wifiNetworkChannels[autoMicroSystem.connectivity.wifiSelectedNetworkIndex] = 0u;
            autoMicroSystem.connectivity.wifiNetworkEncryptionTypes[autoMicroSystem.connectivity.wifiSelectedNetworkIndex] = 0u;
            autoMicroSystem.connectivity.wifiNetworkEncryptions[autoMicroSystem.connectivity.wifiSelectedNetworkIndex][0] = '\0';
            (void)WifiBridge_RequestWifiNetworkDetail(
                &wifiBridge,
                autoMicroSystem.connectivity.wifiNetworkSsids[autoMicroSystem.connectivity.wifiSelectedNetworkIndex]);
        }
        menuSystem.renderFn = OledUtils_RenderWiFiDetails_Wrapper;
        menuSystem.renderFlag = true;
        autoMicroSystem.runtime.oled_first_draw = true;
    }
}

static void WiFiResults_OnEncLongPress(void)
{
    WiFiResults_BackToWifiMenu();
}

static void WiFiResults_OnUserButton(void)
{
    WiFiScan_ClearResults();
    if (menuSystem.dashboardRender) {
        menuSystem.renderFn = menuSystem.dashboardRender;
    }
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0;
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

static const EventCallbacks_t wifiResultsCallbacks = {
    .onRotateCW     = WiFiResults_OnRotateCW,
    .onRotateCCW    = WiFiResults_OnRotateCCW,
    .onShortPress   = WiFiResults_OnShortPress,
    .onLongPress    = NULL,
    .onUserButton   = WiFiResults_OnUserButton,
    .onEncLongPress = WiFiResults_OnEncLongPress
};

// ============================================================================
// CALLBACKS PARA DETALLE WIFI
// ============================================================================

static void WiFiDetails_BackToResults(void)
{
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    autoMicroSystem.runtime.inside_menu_flag = true;
    menuSystem.currentMenu = &wifiResultsMenu;
    menuSystem.renderFn = OledUtils_RenderWiFiSearchResults_Wrapper;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = false;
}

static void WiFiAtPassword_ShowFailed(void)
{
    OledUtils_SetWiFiCredentialsWebResultStatus(
        WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_FAILED);
    OledUtils_DismissNotification();
    OledUtils_ShowNotificationMsEx(OledUtils_RenderWiFiCredentialsFailedNotification,
                                   OLED_NOTIFICATION_DEFAULT_MS,
                                   NULL,
                                   Dashboard_ReturnAfterConnectivityNotification);
}

static void WiFiAtPassword_ConnectSelected(const char *password)
{
    const char *ssid;

    if (autoMicroSystem.connectivity.wifiSelectedNetworkIndex >= autoMicroSystem.connectivity.networksFound ||
        autoMicroSystem.connectivity.wifiSelectedNetworkIndex >= WIFI_SCAN_MAX_NETWORKS) {
        WiFiAtPassword_ShowFailed();
        return;
    }

    ssid = autoMicroSystem.connectivity.wifiNetworkSsids[autoMicroSystem.connectivity.wifiSelectedNetworkIndex];
    if (WifiBridge_SetCredentials(&wifiBridge, ssid, password) != WIFI_BRIDGE_OK) {
        WiFiAtPassword_ShowFailed();
        return;
    }

    OledUtils_SetWiFiConnectedSsid(ssid);
    APP_ESPWifiConnectRequested();
    menuSystem.renderFn = OledUtils_RenderWiFiConnectionStatus_Wrapper;
    menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0u;
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = false;

    OledUtils_DismissNotification();
    OledUtils_ShowNotificationMsEx(OledUtils_RenderESPWifiConnectingNotification,
                                   20000u,
                                   NULL,
                                   Dashboard_ReturnAfterConnectivityNotification);
}

static void WiFiAtPassword_Begin(void)
{
    memset(wifiAtPasswordDraft, 0, sizeof(wifiAtPasswordDraft));
    wifiAtPasswordLength = 0u;
    wifiAtPasswordCharacterIndex = 1u;
    menuSystem.renderFn = OledUtils_RenderWiFiAtPassword_Wrapper;
    menuSystem.userEventManagerFn = WiFiAtPassword_UserEventManager;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = false;
}

static void WiFiDetails_OnShortPress(void)
{
    if (autoMicroSystem.connectivity.wifiSelectedNetworkIndex >= autoMicroSystem.connectivity.networksFound ||
        autoMicroSystem.connectivity.wifiSelectedNetworkIndex >= WIFI_SCAN_MAX_NETWORKS) {
        return;
    }

    if (WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_AT) {
        if (autoMicroSystem.connectivity.wifiNetworkEncryptionTypes[autoMicroSystem.connectivity.wifiSelectedNetworkIndex] == 0u) {
            WiFiAtPassword_ConnectSelected("");
        } else {
            WiFiAtPassword_Begin();
        }
        return;
    }

    (void)WifiBridge_SendWifiCredentialRequest(
        &wifiBridge,
        autoMicroSystem.connectivity.wifiNetworkSsids[autoMicroSystem.connectivity.wifiSelectedNetworkIndex]);
}

static const EventCallbacks_t wifiDetailsCallbacks = {
    .onRotateCW     = NULL,
    .onRotateCCW    = NULL,
    .onShortPress   = WiFiDetails_OnShortPress,
    .onLongPress    = NULL,
    .onUserButton   = WiFiDetails_BackToResults,
    .onEncLongPress = WiFiDetails_BackToResults
};

static void WiFiAtPassword_OnRotateCW(void)
{
    wifiAtPasswordCharacterIndex++;
    if (wifiAtPasswordCharacterIndex >= (sizeof(wifiAtPasswordAlphabet) - 1u)) {
        wifiAtPasswordCharacterIndex = 0u;
    }
    menuSystem.renderFlag = true;
}

static void WiFiAtPassword_OnRotateCCW(void)
{
    if (wifiAtPasswordCharacterIndex == 0u) {
        wifiAtPasswordCharacterIndex =
            (uint8_t)(sizeof(wifiAtPasswordAlphabet) - 2u);
    } else {
        wifiAtPasswordCharacterIndex--;
    }
    menuSystem.renderFlag = true;
}

static void WiFiAtPassword_OnShortPress(void)
{
    if (wifiAtPasswordLength >= WIFI_BRIDGE_PASS_MAX_LEN) {
        return;
    }

    wifiAtPasswordDraft[wifiAtPasswordLength++] =
        wifiAtPasswordAlphabet[wifiAtPasswordCharacterIndex];
    wifiAtPasswordDraft[wifiAtPasswordLength] = '\0';
    menuSystem.renderFlag = true;
}

static void WiFiAtPassword_OnUserButton(void)
{
    if (wifiAtPasswordLength == 0u) {
        autoMicroSystem.runtime.inside_menu_flag = false;
        menuSystem.renderFn = OledUtils_RenderWiFiDetails_Wrapper;
        menuSystem.userEventManagerFn = WiFiDetails_UserEventManager;
        menuSystem.renderFlag = true;
        autoMicroSystem.runtime.oled_first_draw = false;
        return;
    }

    wifiAtPasswordLength--;
    wifiAtPasswordDraft[wifiAtPasswordLength] = '\0';
    menuSystem.renderFlag = true;
}

static void WiFiAtPassword_OnEncLongPress(void)
{
    WiFiAtPassword_ConnectSelected(wifiAtPasswordDraft);
    memset(wifiAtPasswordDraft, 0, sizeof(wifiAtPasswordDraft));
    wifiAtPasswordLength = 0u;
}

static const EventCallbacks_t wifiAtPasswordCallbacks = {
    .onRotateCW     = WiFiAtPassword_OnRotateCW,
    .onRotateCCW    = WiFiAtPassword_OnRotateCCW,
    .onShortPress   = WiFiAtPassword_OnShortPress,
    .onLongPress    = NULL,
    .onUserButton   = WiFiAtPassword_OnUserButton,
    .onEncLongPress = WiFiAtPassword_OnEncLongPress
};

uint8_t WiFiAtPassword_GetLength(void)
{
    return wifiAtPasswordLength;
}

char WiFiAtPassword_GetSelectedCharacter(void)
{
    return wifiAtPasswordAlphabet[wifiAtPasswordCharacterIndex];
}

// ============================================================================
// CALLBACKS PARA PANTALLAS DE SOLO LECTURA (IR, MPU, Radar)
// ============================================================================

static void ReadOnly_OnShortPress(void)
{
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1;
    }
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.clearScreen();
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void ReadOnly_OnLongPress(void)
{
    if (menuSystem.dashboardRender) {
        menuSystem.renderFn = menuSystem.dashboardRender;
    }
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0;
    }
    menuSystem.clearScreen();
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

const EventCallbacks_t readOnlyCallbacks = {
    .onRotateCW     = NULL,  // No hacer nada al rotar (solo lectura)
    .onRotateCCW    = NULL,
    .onShortPress   = ReadOnly_OnShortPress,
    .onLongPress    = ReadOnly_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = NULL
};

static void IRValues_ToggleView(void)
{
    OledUtils_ToggleValoresIRView();
}

static void IRValues_OnShortPress(void)
{
    OledUtils_EndIRValuesSession();
    ReadOnly_OnShortPress();
}

static void IRValues_OnLongPress(void)
{
    OledUtils_EndIRValuesSession();
    ReadOnly_OnLongPress();
}

static void IRValues_OnEncLongPress(void)
{
    HAL_GPIO_TogglePin(Luces_IR_GPIO_Port, Luces_IR_Pin);
}

static const EventCallbacks_t irValuesCallbacks = {
    .onRotateCW     = IRValues_ToggleView,
    .onRotateCCW    = IRValues_ToggleView,
    .onShortPress   = IRValues_OnShortPress,
    .onLongPress    = IRValues_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = IRValues_OnEncLongPress
};

static void RouteAlignment_ToggleView(void)
{
    OledUtils_ToggleTestCaminoView();
}

static const EventCallbacks_t routeAlignmentCallbacks = {
    .onRotateCW     = RouteAlignment_ToggleView,
    .onRotateCCW    = RouteAlignment_ToggleView,
    .onShortPress   = ReadOnly_OnShortPress,
    .onLongPress    = ReadOnly_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = NULL
};

static void MPUValues_ToggleView(void)
{
    OledUtils_ToggleValoresMPUView();
}

static const EventCallbacks_t mpuValuesCallbacks = {
    .onRotateCW     = MPUValues_ToggleView,
    .onRotateCCW    = MPUValues_ToggleView,
    .onShortPress   = ReadOnly_OnShortPress,
    .onLongPress    = ReadOnly_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = NULL
};

static void UnerRouterTest_ToggleView(void)
{
    OledUtils_NextUnerRouterPacketView();
}

static void UnerRouterTest_PreviousView(void)
{
    OledUtils_PreviousUnerRouterPacketView();
}

static const EventCallbacks_t unerRouterTestCallbacks = {
    .onRotateCW     = UnerRouterTest_ToggleView,
    .onRotateCCW    = UnerRouterTest_PreviousView,
    .onShortPress   = ReadOnly_OnShortPress,
    .onLongPress    = ReadOnly_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = NULL
};

void NotificationDismiss_OnShortPress(void)
{
    /* Cerrar notificación y avanzar a la siguiente en la cola (si la hay) */
    OledUtils_DismissNotification();
    autoMicroSystem.runtime.oled_first_draw = true;
}

void NotificationDismiss_OnLongPress(void)
{
    /* Misma lógica: long press también descarta solo la actual */
    OledUtils_DismissNotification();
    autoMicroSystem.runtime.oled_first_draw = true;
}

/* Callbacks para cuando hay una notificación activa y queremos permitir “dismiss” */
static const EventCallbacks_t notificationDismissCallbacks = {
    .onRotateCW     = NULL,
    .onRotateCCW    = NULL,
    .onShortPress   = NotificationDismiss_OnShortPress,
    .onLongPress    = NULL,
    .onUserButton   = NULL,
    .onEncLongPress = NULL
};

// ============================================================================
// CALLBACKS PARA PREFERENCIAS
// ============================================================================

void Preferences_ToggleEncoderInvert(void)
{
    const bool next = !AppPreferences_IsEncoderInverted();

    (void)AppPreferences_SetEncoderInverted(next);
    ENC_SetDirectionInverted(&encoder, AppPreferences_IsEncoderInverted());
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}


void Preferences_ToggleEspNotifWatchdog(void *param)
{
    (void)param;
    AppPreferencesPatch_t patch = {0};
    patch.fields = APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_ENABLED;
    patch.values.espNotifWatchdogEnabled = !AppPreferences_IsEspNotifWatchdogEnabled();
    AppPreferences_ApplyPatch(&patch, NULL);
}
void Preferences_ToggleScreenLock(void)
{
    const bool next = !AppPreferences_IsScreenLockEnabled();

    (void)AppPreferences_SetScreenLockEnabled(next);
    ScreenLock_OnPreferencesChanged();
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Preferences_ToggleOledAutoRotation(void)
{
    const bool next = !AppPreferences_IsOledAutoRotationEnabled();

    if (AppPreferences_SetOledAutoRotationEnabled(next) == HAL_OK) {
        APP_ApplyPreferenceRuntimeChanges(APP_PREFS_FIELD_OLED_AUTO_ROTATION);
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Preferences_ToggleUnerRouterForwarding(void)
{
    const bool next = !AppPreferences_IsUnerRouterForwardingEnabled();

    if (AppPreferences_SetUnerRouterForwardingEnabled(next) == HAL_OK) {
        APP_ApplyPreferenceRuntimeChanges(APP_PREFS_FIELD_UNER_ROUTER_FORWARDING);
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Preferences_ToggleWifiApFallback(void)
{
    const bool next = !AppPreferences_IsWifiApFallbackEnabled();
    if (AppPreferences_SetWifiApFallbackEnabled(next) == AM_OK) {
        APP_ApplyPreferenceRuntimeChanges(APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED);
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Preferences_TogglePinLock(void)
{
    const bool next = !AppPreferences_IsPinLockEnabled();

    if (AppPreferences_SetPinLockEnabled(next) == HAL_OK) {
        APP_ApplyPreferenceRuntimeChanges(APP_PREFS_FIELD_PIN_LOCK_ENABLED);
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Preferences_ToggleIrObjects(void)
{
    const bool next = !APP_IsIrObjectsEnabled();
    APP_SetIrObjectsEnabled(next);
    OledUtils_ShowSimpleNotificationMs(
        next ? OLED_SIMPLE_NOTIFICATION_IR_OBJECTS_ENABLED
             : OLED_SIMPLE_NOTIFICATION_IR_OBJECTS_DISABLED, 2000u);
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Preferences_ToggleIrLine(void)
{
    const bool next = !APP_IsIrLineEnabled();
    APP_SetIrLineEnabled(next);
    OledUtils_ShowSimpleNotificationMs(
        next ? OLED_SIMPLE_NOTIFICATION_IR_LINE_ENABLED
             : OLED_SIMPLE_NOTIFICATION_IR_LINE_DISABLED, 2000u);
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Preferences_ToggleIrLights(void)
{
    const bool next = !APP_IsIrLightsEnabled();
    APP_SetIrLightsEnabled(next);
    OledUtils_ShowSimpleNotificationMs(
        next ? OLED_SIMPLE_NOTIFICATION_IR_LIGHTS_ENABLED
             : OLED_SIMPLE_NOTIFICATION_IR_LIGHTS_DISABLED, 2000u);
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
}

void Preferences_BeginScreenLockTimeoutEdit(void)
{
    ScreenLock_BeginTimeoutEdit();
    autoMicroSystem.runtime.oled_first_draw = false;
}

void Preferences_BeginSystemActionTimeoutEdit(void)
{
    systemActionTimeoutDraft = AppPreferences_GetSystemActionTimeoutSec();
    systemActionTimeoutOriginal = systemActionTimeoutDraft;
    autoMicroSystem.runtime.oled_first_draw = false;
}

uint16_t Preferences_GetSystemActionTimeoutDraft(void)
{
    return systemActionTimeoutDraft;
}

void Preferences_BeginUsbAlivePeriodEdit(void)
{
    usbAlivePeriodDraftMs = AppPreferences_GetUsbAlivePeriodMs();
    usbAlivePeriodOriginalMs = usbAlivePeriodDraftMs;
    autoMicroSystem.runtime.oled_first_draw = false;
}

uint16_t Preferences_GetUsbAlivePeriodDraftMs(void)
{
    return usbAlivePeriodDraftMs;
}

void Preferences_BeginLineBlackWidthEdit(void)
{
    lineBlackWidthDraftMm = AppPreferences_GetLineBlackWidthMm();
    lineBlackWidthOriginalMm = lineBlackWidthDraftMm;
    autoMicroSystem.runtime.oled_first_draw = false;
}

uint16_t Preferences_GetLineBlackWidthDraftMm(void)
{
    return lineBlackWidthDraftMm;
}

static OledRotation_t Preferences_OledRotationFromDeg(uint16_t rotationDeg)
{
    return (AppPreferences_ClampOledRotationDeg(rotationDeg) ==
            APP_PREFS_OLED_ROTATION_180_DEG)
        ? OLED_ROTATION_180
        : OLED_ROTATION_0;
}

static void Preferences_ApplyOledRotationDraft(void)
{
    (void)OledUtils_SetRotationAndRefresh(
        Preferences_OledRotationFromDeg(oledRotationDraftDeg));
}

static uint16_t Preferences_WrapOledRotationDeg(uint16_t currentDeg, int16_t delta)
{
    int16_t index = (AppPreferences_ClampOledRotationDeg(currentDeg) ==
                     APP_PREFS_OLED_ROTATION_180_DEG) ? 1 : 0;

    index = (int16_t)(index + delta);
    while (index < 0) {
        index = (int16_t)(index + 2);
    }
    while (index >= 2) {
        index = (int16_t)(index - 2);
    }

    return (index == 0)
        ? APP_PREFS_OLED_ROTATION_0_DEG
        : APP_PREFS_OLED_ROTATION_180_DEG;
}

void Preferences_BeginOledRotationTest(void)
{
    oledRotationDraftDeg = AppPreferences_GetOledRotationDeg();
    oledRotationOriginalDeg = oledRotationDraftDeg;
    Preferences_ApplyOledRotationDraft();
    autoMicroSystem.runtime.oled_first_draw = false;
}

uint16_t Preferences_GetOledRotationDraftDeg(void)
{
    return oledRotationDraftDeg;
}

void Preferences_BeginOledRotationMpuTest(void)
{
    oledRotationMpuTestOriginalDeg = AppPreferences_GetOledRotationDeg();
    oledRotationMpuTestDeg = oledRotationMpuTestOriginalDeg;
    oledRotationMpuTestYawDeg = 0;
    oledRotationMpuTestReady = 0u;
    MPU9250_AttitudeResetYaw(&mpuTask.attitude);
    (void)OledUtils_SetRotationAndRefresh(
        Preferences_OledRotationFromDeg(oledRotationMpuTestDeg));
    autoMicroSystem.runtime.oled_first_draw = false;
}

uint16_t Preferences_GetOledRotationMpuTestDeg(void)
{
    return oledRotationMpuTestDeg;
}

int16_t Preferences_GetOledRotationMpuTestYawDeg(void)
{
    return oledRotationMpuTestYawDeg;
}

uint8_t Preferences_IsOledRotationMpuTestReady(void)
{
    return oledRotationMpuTestReady;
}

void Preferences_UpdateOledRotationFromMpuTest(void)
{
    MPU9250_Attitude_t attitude;
    MPU9250_NormalizedData_t normalized;
    uint16_t nextDeg = oledRotationMpuTestDeg;
    float lateralAbs;
    float azAbs;

    oledRotationMpuTestReady = 0u;
    if (MPU9250_GetAttitude(&mpuTask, &attitude) != HAL_OK) {
        return;
    }

    MPU9250_GetNormalizedData(&mpuTask, &normalized);
    lateralAbs = (attitude.roll_deg < 0.0f) ? -attitude.roll_deg : attitude.roll_deg;
    azAbs = (normalized.az_g < 0.0f) ? -normalized.az_g : normalized.az_g;
    oledRotationMpuTestYawDeg = (int16_t)attitude.yaw_deg;
    oledRotationMpuTestReady = 1u;

    if (normalized.az_g <= OLED_ROTATION_MPU_FACE_DOWN_AZ_G) {
        nextDeg = APP_PREFS_OLED_ROTATION_180_DEG;
    } else if (attitude.yaw_deg >= OLED_ROTATION_MPU_Z_POS_TRIGGER_DEG) {
        nextDeg = APP_PREFS_OLED_ROTATION_0_DEG;
    } else if (attitude.yaw_deg <= OLED_ROTATION_MPU_Z_NEG_TRIGGER_DEG) {
        nextDeg = APP_PREFS_OLED_ROTATION_180_DEG;
    } else if (azAbs < OLED_ROTATION_MPU_FACE_UP_AZ_G) {
        if (lateralAbs >= OLED_ROTATION_MPU_LATERAL_TO_180_DEG) {
            nextDeg = APP_PREFS_OLED_ROTATION_180_DEG;
        } else if (lateralAbs <= OLED_ROTATION_MPU_LATERAL_TO_0_DEG) {
            nextDeg = APP_PREFS_OLED_ROTATION_0_DEG;
        }
    }

    if (nextDeg != oledRotationMpuTestDeg) {
        oledRotationMpuTestDeg = nextDeg;
        (void)OledUtils_SetRotationAndRefresh(
            Preferences_OledRotationFromDeg(oledRotationMpuTestDeg));
    }
}

static void MotorMinPwm_SaveLiveStateOnce(void)
{
    if (motorMinPwmSaved) {
        return;
    }

    motorMinPwmSavedData = motorTask.motorData;
    motorMinPwmSaved = true;
}

static void MotorMinPwm_ApplyLiveTest(void)
{
    SET_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    NIBBLEH_SET_STATE(motorTask.motorData.flags,
                      motorMinPwmEditLeft ? MOTORLEFT_SELECTED : MOTORRIGHT_SELECTED);

    motorTask.motorData.motorLeft.speedPercent = motorMinPwmEditLeft
        ? MOTOR_MIN_PWM_LIVE_TEST_PERCENT
        : MOTOR_SPEED_PERCENT_MIN;
    motorTask.motorData.motorRight.speedPercent = motorMinPwmEditLeft
        ? MOTOR_SPEED_PERCENT_MIN
        : MOTOR_MIN_PWM_LIVE_TEST_PERCENT;
    motorTask.motorData.motorLeft.motorDirection = MOTOR_DIR_FORWARD;
    motorTask.motorData.motorRight.motorDirection = MOTOR_DIR_FORWARD;

    MotorDrive_ApplyTestState(&motorTask);
}

static void MotorMinPwm_RestoreLiveState(void)
{
    if (!motorMinPwmSaved) {
        MotorDrive_StopAll(&motorTask);
        return;
    }

    motorTask.motorData = motorMinPwmSavedData;
    motorMinPwmSaved = false;
    MotorDrive_ApplyTestState(&motorTask);
}

void Preferences_BeginMotorLeftMinPwmEdit(void)
{
    motorMinPwmEditLeft = true;
    motorMinPwmDraft = AppPreferences_GetMotorLeftMinPwm();
    motorMinPwmOriginal = motorMinPwmDraft;
    MotorMinPwm_SaveLiveStateOnce();
    MotorDrive_SetLeftMinPwm(&motorTask, motorMinPwmDraft);
    MotorMinPwm_ApplyLiveTest();
    autoMicroSystem.runtime.oled_first_draw = false;
}

void Preferences_BeginMotorRightMinPwmEdit(void)
{
    motorMinPwmEditLeft = false;
    motorMinPwmDraft = AppPreferences_GetMotorRightMinPwm();
    motorMinPwmOriginal = motorMinPwmDraft;
    MotorMinPwm_SaveLiveStateOnce();
    MotorDrive_SetRightMinPwm(&motorTask, motorMinPwmDraft);
    MotorMinPwm_ApplyLiveTest();
    autoMicroSystem.runtime.oled_first_draw = false;
}

uint16_t Preferences_GetMotorMinPwmDraft(void)
{
    return motorMinPwmDraft;
}

const char *Preferences_GetMotorMinPwmEditLabel(void)
{
    return motorMinPwmEditLeft ? "Motor izq" : "Motor der";
}

ScreenCode_t Preferences_GetMotorMinPwmEditScreenCode(void)
{
    return motorMinPwmEditLeft
        ? SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_LEFT
        : SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_RIGHT;
}

static void ScreenLockTimeout_BackToMenu(bool save)
{
    if (save) {
        ScreenLock_SaveTimeoutDraft();
    } else {
        ScreenLock_CancelTimeoutDraft();
    }

    EventManagers_RestoreMenu();
}

static void ScreenLockTimeout_OnRotateCW(void)
{
    ScreenLock_AdjustTimeoutDraft(APP_PREFS_SCREEN_LOCK_TIMEOUT_STEP_SEC);
    menuSystem.renderFlag = true;
}

static void ScreenLockTimeout_OnRotateCCW(void)
{
    ScreenLock_AdjustTimeoutDraft(-(int16_t)APP_PREFS_SCREEN_LOCK_TIMEOUT_STEP_SEC);
    menuSystem.renderFlag = true;
}

static void ScreenLockTimeout_OnShortPress(void)
{
    ScreenLockTimeout_BackToMenu(true);
}

static void ScreenLockTimeout_OnLongPress(void)
{
    ScreenLockTimeout_BackToMenu(false);
}

static const EventCallbacks_t screenLockTimeoutCallbacks = {
    .onRotateCW     = ScreenLockTimeout_OnRotateCW,
    .onRotateCCW    = ScreenLockTimeout_OnRotateCCW,
    .onShortPress   = ScreenLockTimeout_OnShortPress,
    .onLongPress    = ScreenLockTimeout_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = ScreenLockTimeout_OnLongPress
};

static void SystemActionTimeout_BackToMenu(bool save)
{
    if (save) {
        (void)AppPreferences_SetSystemActionTimeoutSec(systemActionTimeoutDraft);
    } else {
        systemActionTimeoutDraft = systemActionTimeoutOriginal;
    }

    EventManagers_RestoreMenu();
}

static void SystemActionTimeout_OnRotateCW(void)
{
    systemActionTimeoutDraft = AppPreferences_ClampSystemActionTimeoutSec(
        (uint16_t)(systemActionTimeoutDraft + APP_PREFS_SYSTEM_ACTION_TIMEOUT_STEP_SEC));
    menuSystem.renderFlag = true;
}

static void SystemActionTimeout_OnRotateCCW(void)
{
    if (systemActionTimeoutDraft > APP_PREFS_SYSTEM_ACTION_TIMEOUT_STEP_SEC) {
        systemActionTimeoutDraft = (uint16_t)(
            systemActionTimeoutDraft - APP_PREFS_SYSTEM_ACTION_TIMEOUT_STEP_SEC);
    } else {
        systemActionTimeoutDraft = 0u;
    }
    systemActionTimeoutDraft =
        AppPreferences_ClampSystemActionTimeoutSec(systemActionTimeoutDraft);
    menuSystem.renderFlag = true;
}

static void SystemActionTimeout_OnShortPress(void)
{
    SystemActionTimeout_BackToMenu(true);
}

static void SystemActionTimeout_OnLongPress(void)
{
    SystemActionTimeout_BackToMenu(false);
}

static const EventCallbacks_t systemActionTimeoutCallbacks = {
    .onRotateCW     = SystemActionTimeout_OnRotateCW,
    .onRotateCCW    = SystemActionTimeout_OnRotateCCW,
    .onShortPress   = SystemActionTimeout_OnShortPress,
    .onLongPress    = SystemActionTimeout_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = SystemActionTimeout_OnLongPress
};

static void UsbAlivePeriod_BackToMenu(bool save)
{
    if (save) {
        (void)AppPreferences_SetUsbAlivePeriodMs(usbAlivePeriodDraftMs);
    } else {
        usbAlivePeriodDraftMs = usbAlivePeriodOriginalMs;
    }

    EventManagers_RestoreMenu();
}

static void UsbAlivePeriod_AdjustDraft(int16_t delta)
{
    int32_t next = (int32_t)usbAlivePeriodDraftMs + (int32_t)delta;

    if (next < (int32_t)APP_PREFS_USB_ALIVE_PERIOD_MIN_MS) {
        next = APP_PREFS_USB_ALIVE_PERIOD_MIN_MS;
    } else if (next > (int32_t)APP_PREFS_USB_ALIVE_PERIOD_MAX_MS) {
        next = APP_PREFS_USB_ALIVE_PERIOD_MAX_MS;
    }

    usbAlivePeriodDraftMs = AppPreferences_ClampUsbAlivePeriodMs((uint16_t)next);
}

static void UsbAlivePeriod_OnRotateCW(void)
{
    UsbAlivePeriod_AdjustDraft(APP_PREFS_USB_ALIVE_PERIOD_STEP_MS);
    menuSystem.renderFlag = true;
}

static void UsbAlivePeriod_OnRotateCCW(void)
{
    UsbAlivePeriod_AdjustDraft(-(int16_t)APP_PREFS_USB_ALIVE_PERIOD_STEP_MS);
    menuSystem.renderFlag = true;
}

static void UsbAlivePeriod_OnShortPress(void)
{
    UsbAlivePeriod_BackToMenu(true);
}

static void UsbAlivePeriod_OnLongPress(void)
{
    UsbAlivePeriod_BackToMenu(false);
}

static const EventCallbacks_t usbAlivePeriodCallbacks = {
    .onRotateCW     = UsbAlivePeriod_OnRotateCW,
    .onRotateCCW    = UsbAlivePeriod_OnRotateCCW,
    .onShortPress   = UsbAlivePeriod_OnShortPress,
    .onLongPress    = UsbAlivePeriod_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = UsbAlivePeriod_OnLongPress
};

static void LineBlackWidth_BackToMenu(bool save)
{
    if (save) {
        if (AppPreferences_SetLineBlackWidthMm(lineBlackWidthDraftMm) == HAL_OK) {
            APP_ApplyPreferenceRuntimeChanges(APP_PREFS_FIELD_LINE_BLACK_WIDTH);
        }
    } else {
        lineBlackWidthDraftMm = lineBlackWidthOriginalMm;
    }

    EventManagers_RestoreMenu();
}

static void LineBlackWidth_AdjustDraft(int16_t delta)
{
    int32_t next = (int32_t)lineBlackWidthDraftMm + (int32_t)delta;

    if (next < (int32_t)APP_PREFS_LINE_BLACK_WIDTH_MIN_MM) {
        next = APP_PREFS_LINE_BLACK_WIDTH_MIN_MM;
    } else if (next > (int32_t)APP_PREFS_LINE_BLACK_WIDTH_MAX_MM) {
        next = APP_PREFS_LINE_BLACK_WIDTH_MAX_MM;
    }

    lineBlackWidthDraftMm = AppPreferences_ClampLineBlackWidthMm((uint16_t)next);
}

static void LineBlackWidth_OnRotateCW(void)
{
    LineBlackWidth_AdjustDraft(APP_PREFS_LINE_BLACK_WIDTH_STEP_MM);
    menuSystem.renderFlag = true;
}

static void LineBlackWidth_OnRotateCCW(void)
{
    LineBlackWidth_AdjustDraft(-(int16_t)APP_PREFS_LINE_BLACK_WIDTH_STEP_MM);
    menuSystem.renderFlag = true;
}

static void LineBlackWidth_OnShortPress(void)
{
    LineBlackWidth_BackToMenu(true);
}

static void LineBlackWidth_OnLongPress(void)
{
    LineBlackWidth_BackToMenu(false);
}

static const EventCallbacks_t lineBlackWidthCallbacks = {
    .onRotateCW     = LineBlackWidth_OnRotateCW,
    .onRotateCCW    = LineBlackWidth_OnRotateCCW,
    .onShortPress   = LineBlackWidth_OnShortPress,
    .onLongPress    = LineBlackWidth_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = LineBlackWidth_OnLongPress
};

static void OledRotationTest_BackToMenu(bool save)
{
    if (save) {
        (void)AppPreferences_SetOledRotationDeg(oledRotationDraftDeg);
    } else {
        oledRotationDraftDeg = oledRotationOriginalDeg;
        (void)OledUtils_SetRotationAndRefresh(
            Preferences_OledRotationFromDeg(oledRotationOriginalDeg));
    }

    EventManagers_RestoreMenu();
}

static void OledRotationTest_Adjust(int16_t delta)
{
    oledRotationDraftDeg = Preferences_WrapOledRotationDeg(oledRotationDraftDeg, delta);
    Preferences_ApplyOledRotationDraft();
}

static void OledRotationTest_OnRotateCW(void)
{
    OledRotationTest_Adjust(1);
}

static void OledRotationTest_OnRotateCCW(void)
{
    OledRotationTest_Adjust(-1);
}

static void OledRotationTest_OnShortPress(void)
{
    OledRotationTest_BackToMenu(true);
}

static void OledRotationTest_OnLongPress(void)
{
    OledRotationTest_BackToMenu(false);
}

static const EventCallbacks_t oledRotationTestCallbacks = {
    .onRotateCW     = OledRotationTest_OnRotateCW,
    .onRotateCCW    = OledRotationTest_OnRotateCCW,
    .onShortPress   = OledRotationTest_OnShortPress,
    .onLongPress    = OledRotationTest_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = OledRotationTest_OnLongPress
};

static void OledRotationMpuTest_BackToMenu(void)
{
    (void)OledUtils_SetRotationAndRefresh(
        Preferences_OledRotationFromDeg(oledRotationMpuTestOriginalDeg));

    EventManagers_RestoreMenu();
}

static const EventCallbacks_t oledRotationMpuTestCallbacks = {
    .onRotateCW     = NULL,
    .onRotateCCW    = NULL,
    .onShortPress   = OledRotationMpuTest_BackToMenu,
    .onLongPress    = OledRotationMpuTest_BackToMenu,
    .onUserButton   = OledRotationMpuTest_BackToMenu,
    .onEncLongPress = OledRotationMpuTest_BackToMenu
};

static void MotorMinPwm_BackToMenu(bool save)
{
    if (save) {
        if (motorMinPwmEditLeft) {
            (void)AppPreferences_SetMotorLeftMinPwm(motorMinPwmDraft);
            MotorDrive_SetLeftMinPwm(&motorTask, AppPreferences_GetMotorLeftMinPwm());
        } else {
            (void)AppPreferences_SetMotorRightMinPwm(motorMinPwmDraft);
            MotorDrive_SetRightMinPwm(&motorTask, AppPreferences_GetMotorRightMinPwm());
        }
    } else {
        motorMinPwmDraft = motorMinPwmOriginal;
        if (motorMinPwmEditLeft) {
            MotorDrive_SetLeftMinPwm(&motorTask, motorMinPwmOriginal);
        } else {
            MotorDrive_SetRightMinPwm(&motorTask, motorMinPwmOriginal);
        }
    }
    MotorMinPwm_RestoreLiveState();

    EventManagers_RestoreMenu();
}

static void MotorMinPwm_AdjustDraft(int16_t delta)
{
    int32_t next = (int32_t)motorMinPwmDraft + (int32_t)delta;

    if (next < (int32_t)APP_PREFS_MOTOR_MIN_PWM_MIN) {
        next = APP_PREFS_MOTOR_MIN_PWM_MIN;
    } else if (next > (int32_t)APP_PREFS_MOTOR_MIN_PWM_MAX) {
        next = APP_PREFS_MOTOR_MIN_PWM_MAX;
    }

    motorMinPwmDraft = AppPreferences_ClampMotorMinPwm((uint16_t)next);
    if (motorMinPwmEditLeft) {
        MotorDrive_SetLeftMinPwm(&motorTask, motorMinPwmDraft);
    } else {
        MotorDrive_SetRightMinPwm(&motorTask, motorMinPwmDraft);
    }
    MotorMinPwm_ApplyLiveTest();
}

static void MotorMinPwm_OnRotateCW(void)
{
    MotorMinPwm_AdjustDraft(APP_PREFS_MOTOR_MIN_PWM_STEP);
    menuSystem.renderFlag = true;
}

static void MotorMinPwm_OnRotateCCW(void)
{
    MotorMinPwm_AdjustDraft(-(int16_t)APP_PREFS_MOTOR_MIN_PWM_STEP);
    menuSystem.renderFlag = true;
}

static void MotorMinPwm_OnShortPress(void)
{
    MotorMinPwm_BackToMenu(true);
}

static void MotorMinPwm_OnLongPress(void)
{
    MotorMinPwm_BackToMenu(false);
}

static const EventCallbacks_t motorMinPwmCallbacks = {
    .onRotateCW     = MotorMinPwm_OnRotateCW,
    .onRotateCCW    = MotorMinPwm_OnRotateCCW,
    .onShortPress   = MotorMinPwm_OnShortPress,
    .onLongPress    = MotorMinPwm_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = MotorMinPwm_OnLongPress
};

static float BalancePid_ClampGain(float value)
{
    if (value < BALANCE_PID_GAIN_MIN) {
        return BALANCE_PID_GAIN_MIN;
    }
    if (value > BALANCE_PID_GAIN_MAX) {
        return BALANCE_PID_GAIN_MAX;
    }
    return value;
}

static uint16_t BalancePid_ClampLogDuration(uint16_t value)
{
    if (value < BALANCE_PID_LOG_DURATION_MIN_SEC) {
        return BALANCE_PID_LOG_DURATION_MIN_SEC;
    }

    uint32_t max_sec_from_ram = BalancePid_GetLogAvailableBytes() / 
        ( (1000u / BALANCE_PID_LOG_SAMPLE_PERIOD_MS) * BALANCE_PID_LOG_SAMPLE_BYTES );
        
    uint16_t max_sec = BALANCE_PID_LOG_DURATION_MAX_SEC;
    if (max_sec_from_ram < max_sec) {
        max_sec = (uint16_t)max_sec_from_ram;
    }

    if (value > max_sec) {
        return max_sec;
    }
    return value;
}

static float BalancePid_ReadSelectedGain(void)
{
    switch (balancePidGainTarget) {
        case BALANCE_PID_GAIN_KI:
            return balanceControl.config.ki;
        case BALANCE_PID_GAIN_KD:
            return balanceControl.config.kd;
        case BALANCE_PID_GAIN_KP:
        default:
            return balanceControl.config.kp;
    }
}

static void BalancePid_ApplySelectedGain(float value)
{
    float kp = balanceControl.config.kp;
    float ki = balanceControl.config.ki;
    float kd = balanceControl.config.kd;

    value = BalancePid_ClampGain(value);

    switch (balancePidGainTarget) {
        case BALANCE_PID_GAIN_KI:
            ki = value;
            break;
        case BALANCE_PID_GAIN_KD:
            kd = value;
            break;
        case BALANCE_PID_GAIN_KP:
        default:
            kp = value;
            break;
    }

    BalanceControl_SetPid(&balanceControl, kp, ki, kd);
}

static void BalancePid_BeginGainEdit(BalancePidGainTarget_t target)
{
    balancePidGainTarget = target;
    balancePidGainDraft = BalancePid_ReadSelectedGain();
    balancePidGainOriginal = balancePidGainDraft;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 1u;
    autoMicroSystem.runtime.oled_first_draw = false;
}

void BalancePid_BeginKpEdit(void)
{
    BalancePid_BeginGainEdit(BALANCE_PID_GAIN_KP);
}

void BalancePid_BeginKiEdit(void)
{
    BalancePid_BeginGainEdit(BALANCE_PID_GAIN_KI);
}

void BalancePid_BeginKdEdit(void)
{
    BalancePid_BeginGainEdit(BALANCE_PID_GAIN_KD);
}

void BalancePid_BeginLogConfig(void)
{
    balancePidLogDurationOriginalSec = balancePidLogDurationDraftSec;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    autoMicroSystem.runtime.oled_first_draw = false;
}

static void BalancePid_ReturnToMenu(void)
{
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;

    EventManagers_RestoreMenu();
}

static void BalancePidGain_Adjust(float delta)
{
    balancePidGainDraft = BalancePid_ClampGain(balancePidGainDraft + delta);
    BalancePid_ApplySelectedGain(balancePidGainDraft);
    menuSystem.renderFlag = true;
}

static void BalancePidGain_OnRotateCW(void)
{
    BalancePidGain_Adjust(BALANCE_PID_GAIN_STEP);
}

static void BalancePidGain_OnRotateCCW(void)
{
    BalancePidGain_Adjust(-BALANCE_PID_GAIN_STEP);
}

static void BalancePidGain_OnShortPress(void)
{
    HAL_StatusTypeDef saveStatus;

    saveStatus = AppPreferences_SetBalancePid(balanceControl.config.kp,
                                              balanceControl.config.ki,
                                              balanceControl.config.kd);
    if (saveStatus == HAL_OK) {
        AppPreferencesBalancePid_t savedPid;

        AppPreferences_GetBalancePid(&savedPid);
        BalanceControl_SetPid(&balanceControl,
                              savedPid.kp,
                              savedPid.ki,
                              savedPid.kd);
    } else {
        balancePidGainDraft = balancePidGainOriginal;
        BalancePid_ApplySelectedGain(balancePidGainOriginal);
    }
    BalanceControl_Reset(&balanceControl);
    BalancePid_ReturnToMenu();
}

static void BalancePidGain_OnLongPress(void)
{
    balancePidGainDraft = balancePidGainOriginal;
    BalancePid_ApplySelectedGain(balancePidGainOriginal);
    BalanceControl_Reset(&balanceControl);
    BalancePid_ReturnToMenu();
}

static const EventCallbacks_t balancePidGainCallbacks = {
    .onRotateCW     = BalancePidGain_OnRotateCW,
    .onRotateCCW    = BalancePidGain_OnRotateCCW,
    .onShortPress   = BalancePidGain_OnShortPress,
    .onLongPress    = BalancePidGain_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = BalancePidGain_OnLongPress
};

static void BalancePidLog_Adjust(int16_t delta)
{
    int32_t next = (int32_t)balancePidLogDurationDraftSec + (int32_t)delta;

    uint32_t max_sec_from_ram = BalancePid_GetLogAvailableBytes() / 
        ( (1000u / BALANCE_PID_LOG_SAMPLE_PERIOD_MS) * BALANCE_PID_LOG_SAMPLE_BYTES );
        
    int32_t max_sec = BALANCE_PID_LOG_DURATION_MAX_SEC;
    if (max_sec_from_ram < (uint32_t)max_sec) {
        max_sec = (int32_t)max_sec_from_ram;
    }

    if (next > max_sec) {
        next = BALANCE_PID_LOG_DURATION_MIN_SEC;
    } else if (next < (int32_t)BALANCE_PID_LOG_DURATION_MIN_SEC) {
        next = max_sec;
    }

    balancePidLogDurationDraftSec = BalancePid_ClampLogDuration((uint16_t)next);
    menuSystem.renderFlag = true;
}

static void BalancePidLog_OnRotateCW(void)
{
    BalancePidLog_Adjust(BALANCE_PID_LOG_DURATION_STEP_SEC);
}

static void BalancePidLog_OnRotateCCW(void)
{
    BalancePidLog_Adjust(-(int16_t)BALANCE_PID_LOG_DURATION_STEP_SEC);
}

static void BalancePidLog_OnShortPress(void)
{
    balancePidLogDurationOriginalSec = balancePidLogDurationDraftSec;
    BalancePid_ReturnToMenu();
}

static void BalancePidLog_OnLongPress(void)
{
    balancePidLogDurationDraftSec = balancePidLogDurationOriginalSec;
    BalancePid_ReturnToMenu();
}

static const EventCallbacks_t balancePidLogCallbacks = {
    .onRotateCW     = BalancePidLog_OnRotateCW,
    .onRotateCCW    = BalancePidLog_OnRotateCCW,
    .onShortPress   = BalancePidLog_OnShortPress,
    .onLongPress    = BalancePidLog_OnLongPress,
    .onUserButton   = NULL,
    .onEncLongPress = BalancePidLog_OnLongPress
};

ScreenCode_t BalancePid_GetCurrentGainScreenCode(void)
{
    switch (balancePidGainTarget) {
        case BALANCE_PID_GAIN_KI:
            return SCREEN_CODE_BALANCE_PID_KI;
        case BALANCE_PID_GAIN_KD:
            return SCREEN_CODE_BALANCE_PID_KD;
        case BALANCE_PID_GAIN_KP:
        default:
            return SCREEN_CODE_BALANCE_PID_KP;
    }
}

const char *BalancePid_GetCurrentGainLabel(void)
{
    switch (balancePidGainTarget) {
        case BALANCE_PID_GAIN_KI:
            return "Ajuste Ki";
        case BALANCE_PID_GAIN_KD:
            return "Ajuste Kd";
        case BALANCE_PID_GAIN_KP:
        default:
            return "Ajuste Kp";
    }
}

float BalancePid_GetCurrentGainDraft(void)
{
    return balancePidGainDraft;
}

uint16_t BalancePid_GetLogDurationSec(void)
{
    return balancePidLogDurationDraftSec;
}

extern char _end;
uint32_t BalancePid_GetLogAvailableBytes(void)
{
    uint32_t sp;
    __asm__ volatile ("mrs %0, msp" : "=r" (sp));
    
    /* Calculate free RAM between end of BSS and current stack pointer.
       Leave an 8KB safety margin for stack growth and interrupts. */
    uint32_t end_addr = (uint32_t)&_end;
    if (sp > end_addr + 8192u) {
        return sp - end_addr - 8192u;
    }
    return 0u;
}

uint32_t BalancePid_GetLogEstimatedBytes(void)
{
    const uint32_t samples =
        ((uint32_t)balancePidLogDurationDraftSec * 1000u) /
        BALANCE_PID_LOG_SAMPLE_PERIOD_MS;

    return samples * BALANCE_PID_LOG_SAMPLE_BYTES;
}

static void ScreenLocked_OnUnlock(void)
{
    ScreenLock_Unlock();
}

static const EventCallbacks_t screenLockedCallbacks = {
    .onRotateCW     = NULL,
    .onRotateCCW    = NULL,
    .onShortPress   = ScreenLocked_OnUnlock,
    .onLongPress    = NULL,
    .onUserButton   = NULL,
    .onEncLongPress = ScreenLocked_OnUnlock
};

// ============================================================================
// MANAGER GENÉRICO DE EVENTOS
// ============================================================================

void GenericEventManager(UserEvent_t ev, const EventCallbacks_t *callbacks)
{
    if (!callbacks) return;

    switch (ev)
    {
        case UE_ROTATE_CW:
            if (callbacks->onRotateCW) {
                callbacks->onRotateCW();
            }
            break;

        case UE_ROTATE_CCW:
            if (callbacks->onRotateCCW) {
                callbacks->onRotateCCW();
            }
            break;

        case UE_ENC_SHORT_PRESS:
            if (callbacks->onShortPress) {
                callbacks->onShortPress();
            }
            break;

        case UE_ENC_LONG_PRESS:
            if (callbacks->onEncLongPress) {
                callbacks->onEncLongPress();
            }
            break;

        case UE_SHORT_PRESS:
            if (callbacks->onUserButton) {
                callbacks->onUserButton();
            }
            break;

        case UE_LONG_PRESS:
            if (callbacks->onLongPress) {
                callbacks->onLongPress();
            }
            break;

        default:
            break;
    }
}

void Dashboard_ResetModeConfirmState(void)
{
    dashboardModeCanConfirm = false;
    dashboardModeLastInputMs = 0u;
    dashboardQuickActionActive = false;
    dashboardQuickActionLastInputMs = 0u;
    dashboardActionTimeoutLastRenderMs = 0u;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE);
}

uint8_t Dashboard_GetActionTimeoutContext(uint32_t *startedMs, uint32_t *timeoutMs)
{
    const uint32_t timeout = Dashboard_GetActionTimeoutMs();

    if (timeout == 0u) {
        return 0u;
    }

    if (dashboardQuickActionActive && dashboardQuickActionLastInputMs != 0u) {
        if (startedMs != NULL) {
            *startedMs = dashboardQuickActionLastInputMs;
        }
        if (timeoutMs != NULL) {
            *timeoutMs = timeout;
        }
        return 1u;
    }

    if (dashboardModeCanConfirm &&
        IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE) &&
        dashboardModeLastInputMs != 0u) {
        if (startedMs != NULL) {
            *startedMs = dashboardModeLastInputMs;
        }
        if (timeoutMs != NULL) {
            *timeoutMs = timeout;
        }
        return 1u;
    }

    return 0u;
}

static void Dashboard_RequestActionTimeoutBarRefresh(uint32_t now,
                                                     uint32_t startedMs,
                                                     uint32_t timeoutMs)
{
    const uint32_t elapsedMs = now - startedMs;

    if (timeoutMs <= DASHBOARD_ACTION_TIMEOUT_BAR_DELAY_MS ||
        elapsedMs < DASHBOARD_ACTION_TIMEOUT_BAR_DELAY_MS ||
        elapsedMs >= timeoutMs) {
        return;
    }

    if (dashboardActionTimeoutLastRenderMs == 0u ||
        (uint32_t)(now - dashboardActionTimeoutLastRenderMs) >=
            DASHBOARD_ACTION_TIMEOUT_BAR_STEP_MS) {
        dashboardActionTimeoutLastRenderMs = now;
        menuSystem.renderFlag = true;
    }
}

void Dashboard_ModeSelectionTick(void)
{
    const uint32_t timeoutMs = Dashboard_GetActionTimeoutMs();
    const uint32_t now = HAL_GetTick();

    if (dashboardQuickActionActive &&
        dashboardQuickActionLastInputMs != 0u &&
        timeoutMs != 0u) {
        const uint32_t elapsedMs = now - dashboardQuickActionLastInputMs;
        if (elapsedMs >= timeoutMs) {
            Dashboard_BackToDashboard();
            return;
        }
        Dashboard_RequestActionTimeoutBarRefresh(now,
                                                 dashboardQuickActionLastInputMs,
                                                 timeoutMs);
    }

    if (!dashboardModeCanConfirm ||
        !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, MODIFYING_CARMODE) ||
        dashboardModeLastInputMs == 0u) {
        return;
    }

    if ((uint32_t)(now - dashboardModeLastInputMs) >= timeoutMs) {
        Dashboard_CancelPendingMode();
        return;
    }

    Dashboard_RequestActionTimeoutBarRefresh(now,
                                             dashboardModeLastInputMs,
                                             timeoutMs);
}

// ============================================================================
// FUNCIONES PÚBLICAS DE EVENT MANAGERS
// ============================================================================

void dashboardEventManager(UserEvent_t ev)
{
    __NOP();
    GenericEventManager(ev, &dashboardCallbacks);
}

void dashboardQuickActionEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &stabilizationPromptCallbacks);
}

void stabilizationPromptEventManager(UserEvent_t ev)
{
    dashboardQuickActionEventManager(ev);
}

void menuEventManager(UserEvent_t ev)
{
    __NOP();
    GenericEventManager(ev, &menuCallbacks);
}

void About_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &aboutCallbacks);
}

void motorTestEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &motorTestCallbacks);
}

void motorSingleTestEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &motorSingleTestCallbacks);
}

void mpuCalibrationEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &mpuCalibrationCallbacks);
}

void WiFiSearch_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &wifiSearchCallbacks);
}

void wifiEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &wifiResultsCallbacks);
}

void WiFiDetails_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &wifiDetailsCallbacks);
}

void WiFiAtPassword_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &wifiAtPasswordCallbacks);
}

void ReadOnly_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &readOnlyCallbacks);
}

void IRValues_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &irValuesCallbacks);
}

void RouteAlignment_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &routeAlignmentCallbacks);
}

void MPUValues_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &mpuValuesCallbacks);
}

void UnerRouterTest_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &unerRouterTestCallbacks);
}

void screenLockedEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &screenLockedCallbacks);
}


void espNotifWatchdogLimitEventManager(UserEvent_t event)
{
    uint8_t value = AppPreferences_GetEspNotifWatchdogLimit();
    bool save = false;

    if (event == UE_ROTATE_CCW) {
        if (value > 1u) value--;
        save = true;
    } else if (event == UE_ROTATE_CW) {
        if (value < 20u) value++;
        save = true;
    } else if (event == UE_ENC_SHORT_PRESS) {
        MenuSys_NavigateBack(&menuSystem);
        return;
    }

    if (save) {
        AppPreferencesPatch_t patch = {0};
        patch.fields = APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_LIMIT;
        patch.values.espNotifWatchdogLimit = value;
        AppPreferences_ApplyPatch(&patch, NULL);
    }
}

static void BalancePidStatus_OnShortPress(void)
{
    if (MotionControl_IsArmed(&motionControl)) {
        (void)APP_BalanceSetEnabled(false);
    } else {
        (void)APP_BalanceSetEnabled(true);
    }
    menuSystem.renderFlag = true;
}

static const EventCallbacks_t balancePidStatusCallbacks = {
    .onShortPress = BalancePidStatus_OnShortPress,
    .onLongPress = NULL,
    .onRotateCW = NULL,
    .onRotateCCW = NULL,
    .onUserButton = Menu_OnUserButton,
    .onEncLongPress = NULL
};

void balancePidStatusEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &balancePidStatusCallbacks);
}
void screenLockTimeoutEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &screenLockTimeoutCallbacks);
}

void systemActionTimeoutEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &systemActionTimeoutCallbacks);
}

void usbAlivePeriodEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &usbAlivePeriodCallbacks);
}

void lineBlackWidthEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &lineBlackWidthCallbacks);
}

void oledRotationTestEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &oledRotationTestCallbacks);
}

void oledRotationMpuTestEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &oledRotationMpuTestCallbacks);
}

void oledCanvasEventManager(UserEvent_t ev)
{
    OledCanvas_HandleLocalEvent(ev);
}

void motorMinPwmEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &motorMinPwmCallbacks);
}

void balancePidGainEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &balancePidGainCallbacks);
}

void balancePidLogEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &balancePidLogCallbacks);
}

void ClickCancelar_UserEventManager(UserEvent_t ev)
{
    GenericEventManager(ev, &notificationDismissCallbacks);
}

// ============================================================================
// LEGACY: ItemEventManager (mantener por compatibilidad si se usa en otro lado)
// ============================================================================

void ItemEventManager(UserEvent_t ev, RenderWrapperFn wrapper)
{
    switch (ev)
    {
        case UE_ROTATE_CW:
        case UE_ROTATE_CCW:
            if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN)) {
                CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN);
            } else {
                SET_FLAG(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN);
            }

            if (wrapper) {
                menuSystem.clearScreen();
                menuSystem.renderFn = wrapper;
                menuSystem.renderFlag = true;
            } else {
                menuSystem.renderFlag = true;
                autoMicroSystem.runtime.oled_first_draw = true;
            }
            break;

        case UE_ENC_SHORT_PRESS:
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN);
            MenuSys_NavigateToMain(&menuSystem);
            menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
            menuSystem.clearScreen();
            if (menuSystem.insideMenuFlag) {
                *menuSystem.insideMenuFlag = 1;
            }
            menuSystem.renderFlag = true;
            autoMicroSystem.runtime.oled_first_draw = true;
            break;

        case UE_ENC_LONG_PRESS:
            break;

        case UE_LONG_PRESS:
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN);
            if (menuSystem.dashboardRender) {
                menuSystem.renderFn = menuSystem.dashboardRender;
            }
            if (menuSystem.insideMenuFlag) {
                *menuSystem.insideMenuFlag = 0;
            }
            menuSystem.clearScreen();
            menuSystem.renderFlag = true;
            autoMicroSystem.runtime.oled_first_draw = true;
            break;

        default:
            break;
    }
}

void APP_EventsBindSystem(void) { autoMicroSystem.handles.events = &appEventsState; }
