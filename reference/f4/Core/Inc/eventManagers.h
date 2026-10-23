#ifndef EVENTMANAGERS_H
#define EVENTMANAGERS_H

#include "screen_codes.h"
#include "types/userEvent_type.h"

// Tipo de función para callbacks de render
typedef void (*RenderFn)(void);
typedef void (*RenderWrapperFn)(void);

/**
 * @brief Estructura de callbacks para manejar eventos de usuario
 */
typedef struct {
    RenderFn onRotateCW;      // Callback al rotar sentido horario
    RenderFn onRotateCCW;     // Callback al rotar sentido antihorario
    RenderFn onShortPress;    // Callback al presionar encoder corto
    RenderFn onLongPress;     // Callback al presionar botón usuario largo
    RenderFn onUserButton;    // Callback al presionar botón usuario corto
    RenderFn onEncLongPress;  // Callback al presionar encoder largo
} EventCallbacks_t;

void APP_EventsBindSystem(void);

// Manager genérico
void GenericEventManager(UserEvent_t ev, const EventCallbacks_t *callbacks);

// Event managers públicos
void dashboardEventManager(UserEvent_t ev);
void dashboardQuickActionEventManager(UserEvent_t ev);
void stabilizationPromptEventManager(UserEvent_t ev);
void menuEventManager(UserEvent_t ev);
void About_UserEventManager(UserEvent_t ev);
void motorTestEventManager(UserEvent_t ev);
void WiFiSearch_UserEventManager(UserEvent_t ev);
void wifiEventManager(UserEvent_t ev);
void WiFiDetails_UserEventManager(UserEvent_t ev);
void WiFiAtPassword_UserEventManager(UserEvent_t ev);
uint8_t WiFiAtPassword_GetLength(void);
char WiFiAtPassword_GetSelectedCharacter(void);
void ReadOnly_UserEventManager(UserEvent_t ev);
void IRValues_UserEventManager(UserEvent_t ev);
void RouteAlignment_UserEventManager(UserEvent_t ev);
void MPUValues_UserEventManager(UserEvent_t ev);
void UnerRouterTest_UserEventManager(UserEvent_t ev);
void screenLockedEventManager(UserEvent_t ev);
void espNotifWatchdogLimitEventManager(UserEvent_t event);
void balancePidStatusEventManager(UserEvent_t ev);
void screenLockTimeoutEventManager(UserEvent_t ev);
void systemActionTimeoutEventManager(UserEvent_t ev);
void usbAlivePeriodEventManager(UserEvent_t ev);
void lineBlackWidthEventManager(UserEvent_t ev);
void motorMinPwmEventManager(UserEvent_t ev);
void oledRotationTestEventManager(UserEvent_t ev);
void oledRotationMpuTestEventManager(UserEvent_t ev);
void oledCanvasEventManager(UserEvent_t ev);
void NotificationDismiss_OnShortPress(void);
void NotificationDismiss_OnLongPress(void);
void ClickCancelar_UserEventManager(UserEvent_t ev);
void Dashboard_ResetModeConfirmState(void);
void Dashboard_ModeSelectionTick(void);
uint8_t Dashboard_GetActionTimeoutContext(uint32_t *startedMs, uint32_t *timeoutMs);
typedef void (*GenericConfirmAction_t)(void);
typedef void (*GenericConfirmCancel_t)(void);

void GenericConfirm_Begin(const char* line1, const char* line2,
                          GenericConfirmAction_t onConfirm,
                          GenericConfirmCancel_t onCancel,
                          uint32_t timeoutMs,
                          uint32_t screenCode);
void GenericConfirmEventManager(UserEvent_t ev);
void GenericConfirm_Tick(void);
uint8_t GenericConfirm_GetTimeoutContext(uint32_t *startedMs, uint32_t *timeoutMs);
const char* GenericConfirm_GetLine1(void);
const char* GenericConfirm_GetLine2(void);
uint32_t GenericConfirm_GetScreenCode(void);

void BootV2RestartConfirm_ActionBegin(void);
void BootSTRestartConfirm_ActionBegin(void);
void Preferences_ToggleEncoderInvert(void);
void Preferences_ToggleEspNotifWatchdog(void *param);
void Preferences_ToggleScreenLock(void);
void Preferences_ToggleOledAutoRotation(void);
void Preferences_ToggleUnerRouterForwarding(void);
void Preferences_TogglePinLock(void);
void Preferences_ToggleWifiApFallback(void);
void Preferences_ToggleIrObjects(void);
void Preferences_ToggleIrLine(void);
void Preferences_ToggleIrLights(void);
void Preferences_BeginEspNotifWatchdogLimitEdit(void *param);
void Preferences_BeginScreenLockTimeoutEdit(void);
void Preferences_BeginSystemActionTimeoutEdit(void);
uint16_t Preferences_GetSystemActionTimeoutDraft(void);
void Preferences_BeginUsbAlivePeriodEdit(void);
uint16_t Preferences_GetUsbAlivePeriodDraftMs(void);
void Preferences_BeginLineBlackWidthEdit(void);
uint16_t Preferences_GetLineBlackWidthDraftMm(void);
void Preferences_BeginOledRotationTest(void);
uint16_t Preferences_GetOledRotationDraftDeg(void);
void Preferences_BeginOledRotationMpuTest(void);
void Preferences_UpdateOledRotationFromMpuTest(void);
uint16_t Preferences_GetOledRotationMpuTestDeg(void);
int16_t Preferences_GetOledRotationMpuTestYawDeg(void);
uint8_t Preferences_IsOledRotationMpuTestReady(void);
void Preferences_BeginMotorLeftMinPwmEdit(void);
void Preferences_BeginMotorRightMinPwmEdit(void);
uint16_t Preferences_GetMotorMinPwmDraft(void);
const char *Preferences_GetMotorMinPwmEditLabel(void);
ScreenCode_t Preferences_GetMotorMinPwmEditScreenCode(void);
void MPUCalibration_RequestFromMenu(void);
void mpuCalibrationEventManager(UserEvent_t ev);
void MotorSingleTest_BeginLeft(void);
void MotorSingleTest_BeginRight(void);
void motorSingleTestEventManager(UserEvent_t ev);
ScreenCode_t MotorSingleTest_GetScreenCode(void);
const char *MotorSingleTest_GetLabel(void);
const char *MotorSingleTest_GetDirectionLabel(void);
uint8_t MotorSingleTest_GetSpeedPercentDisplay(void);
uint16_t MotorSingleTest_GetPwm(void);
uint16_t MotorSingleTest_GetMinPwm(void);
uint16_t MotorSingleTest_GetMaxPwm(void);
void BalancePid_BeginKpEdit(void);
void BalancePid_BeginKiEdit(void);
void BalancePid_BeginKdEdit(void);
void BalancePid_BeginLogConfig(void);
void balancePidGainEventManager(UserEvent_t ev);
void balancePidLogEventManager(UserEvent_t ev);
ScreenCode_t BalancePid_GetCurrentGainScreenCode(void);
const char *BalancePid_GetCurrentGainLabel(void);
float BalancePid_GetCurrentGainDraft(void);
uint16_t BalancePid_GetLogDurationSec(void);
uint32_t BalancePid_GetLogAvailableBytes(void);
uint32_t BalancePid_GetLogEstimatedBytes(void);

// Legacy (por compatibilidad)
void ItemEventManager(UserEvent_t ev, RenderWrapperFn wrapper);

#endif /* EVENTMANAGERS_H */
