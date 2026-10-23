/*
 * oled_utils.h
 *
 *  Created on: Jun 16, 2025
 *      Author: kobac
 */

#ifndef INC_OLED_UTILS_H_
#define INC_OLED_UTILS_H_

#include "types/menu_types.h"
#include "mpu9250_types.h"
#include "oled_handle.h"
#include "ssd1306_types.h"
#include <stdbool.h>
#include <stdint.h>

void OledUtils_RenderMotorCalibration(const uint8_t *,uint8_t,float,uint32_t,uint8_t);
#define OLED_DASHBOARD_SSID_MAX_LEN 32u

typedef struct {
    bool espPresent;
    bool staActive;
    bool apActive;
    bool usbActive;
    bool rfActive;
    uint8_t backend;
    uint8_t carMode;
    uint8_t ip[4];
    char ssid[OLED_DASHBOARD_SSID_MAX_LEN + 1u];
} OledDashboardState;

// ─── Funciones de despliegue ─────────────────────────────────────────────────

void OledUtils_RenderDashboard(void);
bool OledUtils_GetDashboardState(OledDashboardState *state);
void OledUtils_RenderStabilizationPrompt(void);
void OledUtils_RenderModePrompt(void);
void OledUtils_DrawActionTimeoutBar(uint32_t startedMs, uint32_t timeoutMs);

void OledUtils_RenderProyectScreen(void);
void OledUtils_RenderProyectInfoScreen(void);
void OledUtils_RenderBuildInfoScreen(void);
void OledUtils_RenderAboutBootloaderScreen(void);
void OledUtils_RenderGenericConfirm(void);

void OledUtils_RenderModeChange_Full(void);
void OledUtils_RenderModeChange_ModeOnly(void);
void OledUtils_RenderScreenLockTimeout_Full(void);
void OledUtils_RenderScreenLockTimeout_ValueOnly(void);
void OledUtils_RenderSystemActionTimeout_Full(void);
void OledUtils_RenderSystemActionTimeout_ValueOnly(void);
void OledUtils_RenderUsbAlivePeriod_Full(void);
void OledUtils_RenderUsbAlivePeriod_ValueOnly(void);
void OledUtils_RenderLineBlackWidth_Full(void);
void OledUtils_RenderLineBlackWidth_ValueOnly(void);
AM_Result OledUtils_SetRotationAndRefresh(OledRotation_t rotation);
void OledUtils_RenderOledRotationTest_Full(void);
void OledUtils_RenderOledRotationTest_ValueOnly(void);
void OledUtils_RenderOledRotationMpuTest_Full(void);
void OledUtils_RenderOledRotationMpuTest_ValueOnly(void);
void OledUtils_RenderMotorMinPwm_Full(void);
void OledUtils_RenderMotorMinPwm_ValueOnly(void);
void OledUtils_RenderSingleMotorTest_Full(void);
void OledUtils_RenderSingleMotorTest_ValueOnly(void);
void OledUtils_RenderBalancePidStatus(void);
void OledUtils_RenderBalancePidLog_Full(void);
void OledUtils_RenderBalancePidLog_ValueOnly(void);
void OledUtils_RenderBalancePidGain_Full(void);
void OledUtils_RenderBalancePidGain_ValueOnly(void);
void OledUtils_RenderEspNotifWatchdogLimit_Full(void);
void OledUtils_RenderEspNotifWatchdogLimit_ValueOnly(void);
void OledUtils_RenderUnerRouterTest_Full(void);
void OledUtils_RenderUnerRouterTest_ValueOnly(void);
void OledUtils_RenderMPUCalibration_Full(void);
void OledUtils_RenderMPUCalibration_ValueOnly(void);
void OledUtils_RenderScreenLocked(void);

void OledUtils_Clear(void);

void OledUtils_DrawItem(const MenuItem *item, uint8_t y, bool selected);
bool OledUtils_IsMenuScrollActive(void);
#ifndef SCREEN_CONTEXT_H
#define menu_scroll_active OledUtils_IsMenuScrollActive()
#endif
void MenuScroll_Stop(void);
void MenuScroll_Tick(void);
void OledUtils_DashboardScrollTick(void);
void OledUtils_MotorTest_Complete(void);

void OledUtils_MotorTest_Changes(void);

void OledUtils_RenderLineNotice(const char *message, const char *detail);
void OledUtils_RenderLineCalibration(const uint8_t *state, const char *failure);
void OledUtils_EnableContinuousRender(void);
void OledUtils_DisableContinuousRender(void);

// --- Sensores IR ---
void OledUtils_RenderIRGraphScene(bool emittersOn);
void OledUtils_UpdateIRBars(volatile uint16_t *sensorData);
void OledUtils_RenderIRRawScene(bool emittersOn);
void OledUtils_UpdateIRRawValues(volatile uint16_t *sensorData);
void OledUtils_RenderRouteAlignmentScene(void);
void OledUtils_UpdateRouteAlignment(volatile uint16_t *sensorData);
void OledUtils_RenderRouteAlignmentVisualScene(void);
void OledUtils_UpdateRouteAlignmentVisual(volatile uint16_t *sensorData);

// --- Sensor MPU ---
void OledUtils_RenderMPUScene(void);
void OledUtils_UpdateMPUValues(MPU9250_Handle_t *mpu);
void OledUtils_RenderMPUOrientationScene(void);
void OledUtils_UpdateMPUOrientation(MPU9250_Handle_t *mpu);
void OledUtils_RenderMPUCompassScene(void);
void OledUtils_UpdateMPUCompass(MPU9250_Handle_t *mpu);
void OLED_DrawMPUScreen(float angle_z, float angle_front, float angle_lateral);
void OLED_DrawAnglePanel(uint8_t x, uint8_t y, uint8_t w, uint8_t h, const char *title, const char *axis, float angle_deg);
void OLED_DrawRotatedLine(int cx, int cy, int length, float angle_deg);

void OledUtils_RenderWiFiSearchScene(void);
void OledUtils_UpdateWiFiSearchTimer(uint8_t secondsRemaining);
void OledUtils_ShowWifiResults(void);
void OledUtils_RenderWiFiDetails(const char *ssid,
                                 const char *encryption,
                                 int8_t signal_strength,
                                 uint8_t channel,
                                 uint8_t detail_valid);
void OledUtils_RenderWiFiAtPassword(const char *ssid,
                                    uint8_t password_length,
                                    char selected_character);

typedef enum {
    OLED_SIMPLE_NOTIFICATION_NONE = 0u,
    OLED_SIMPLE_NOTIFICATION_WIFI_SEARCH_CANCELED,
    OLED_SIMPLE_NOTIFICATION_COMMAND_RECEIVED,
    OLED_SIMPLE_NOTIFICATION_PING_RECEIVED,
    OLED_SIMPLE_NOTIFICATION_ESP_BOOT_RECEIVED,
    OLED_SIMPLE_NOTIFICATION_ESP_UNRESPONSIVE,
    OLED_SIMPLE_NOTIFICATION_ESP_WATCHDOG_RESET,
    OLED_SIMPLE_NOTIFICATION_ESP_EXCEPTION_RESET,
    OLED_SIMPLE_NOTIFICATION_ESP_MODE_CHANGED,
    OLED_SIMPLE_NOTIFICATION_USB_CONNECTED,
    OLED_SIMPLE_NOTIFICATION_ESP_USB_CONNECTED = OLED_SIMPLE_NOTIFICATION_USB_CONNECTED,
    OLED_SIMPLE_NOTIFICATION_USB_DISCONNECTED,
    OLED_SIMPLE_NOTIFICATION_ESP_WEB_SERVER_UP,
    OLED_SIMPLE_NOTIFICATION_ESP_WEB_CLIENT_CONNECTED,
    OLED_SIMPLE_NOTIFICATION_ESP_WEB_CLIENT_DISCONNECTED,
    OLED_SIMPLE_NOTIFICATION_ESP_AP_DEVICE_CONNECTED,
    OLED_SIMPLE_NOTIFICATION_ESP_AP_DEVICE_DISCONNECTED,
    OLED_SIMPLE_NOTIFICATION_AP_CONFIG_SAVED,
    OLED_SIMPLE_NOTIFICATION_AP_CONFIG_INVALID,
    OLED_SIMPLE_NOTIFICATION_ESP_CHECKING_CONNECTION,
    OLED_SIMPLE_NOTIFICATION_ESP_CHECK_CONNECTION_REQUIRED,
    OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
    OLED_SIMPLE_NOTIFICATION_ESP_WIFI_DISCONNECTED,
    OLED_SIMPLE_NOTIFICATION_PERMISSION_DENIED,
    OLED_SIMPLE_NOTIFICATION_PIN_TIMEOUT,
    OLED_SIMPLE_NOTIFICATION_REMOTE_AUTHENTICATED,
    OLED_SIMPLE_NOTIFICATION_PIN_CORRECT,
    OLED_SIMPLE_NOTIFICATION_PIN_INCORRECT,
    OLED_SIMPLE_NOTIFICATION_PIN_BYPASSED,
    OLED_SIMPLE_NOTIFICATION_PIN_MODIFIED,
    OLED_SIMPLE_NOTIFICATION_CONTROLLER_CONNECTED,
    OLED_SIMPLE_NOTIFICATION_CONTROLLER_DISCONNECTED,
    OLED_SIMPLE_NOTIFICATION_MPU_STREAM_STARTED,
    OLED_SIMPLE_NOTIFICATION_MPU_STREAM_STOPPED,
    OLED_SIMPLE_NOTIFICATION_REMOTE_MODE_CHANGED,
    OLED_SIMPLE_NOTIFICATION_UNER_FORWARDING_ENABLED,
    OLED_SIMPLE_NOTIFICATION_UNER_FORWARDING_DISABLED,
    OLED_SIMPLE_NOTIFICATION_EXT_DEFAULT,
    OLED_SIMPLE_NOTIFICATION_EXT_BEEPER,
    OLED_SIMPLE_NOTIFICATION_EXT_NRF24,
    OLED_SIMPLE_NOTIFICATION_FAVORITE_ADDED,
    OLED_SIMPLE_NOTIFICATION_FAVORITE_REMOVED,
    OLED_SIMPLE_NOTIFICATION_FAVORITE_FULL,
    OLED_SIMPLE_NOTIFICATION_IR_OBJECTS_ENABLED,
    OLED_SIMPLE_NOTIFICATION_IR_OBJECTS_DISABLED,
    OLED_SIMPLE_NOTIFICATION_IR_LINE_ENABLED,
    OLED_SIMPLE_NOTIFICATION_IR_LINE_DISABLED,
    OLED_SIMPLE_NOTIFICATION_IR_LIGHTS_ENABLED,
    OLED_SIMPLE_NOTIFICATION_IR_LIGHTS_DISABLED,
} OledSimpleNotificationId;

void OledUtils_RenderWiFiCredentialsWebNotification(void);
void OledUtils_SetWiFiCredentialsWebResultStatus(uint8_t status);
void OledUtils_RenderWiFiCredentialsSucceededNotification(void);
void OledUtils_RenderWiFiCredentialsFailedNotification(void);
void OledUtils_RenderWiFiNotConnected(void);
void OledUtils_RenderWiFiStatus(void);
void OledUtils_SetWiFiConnectedSsid(const char *ssid);
void OledUtils_RenderESPWifiConnectingNotification(void);
void OledUtils_RenderESPFirmwareRequestNotification(void);
void OledUtils_RenderESPFirmwareScreen(void);
void OledUtils_RenderESPResetSentNotification(void);
void OledUtils_RenderESPResetMode_Wrapper(void);
void OledUtils_RenderESPResetModeNotification(void);
void OledUtils_WifiApSsidEdit_Wrapper(void);
void OledUtils_WifiApPasswordEdit_Wrapper(void);
void OledUtils_WifiApIpv4Edit_Wrapper(void);
void OledUtils_WifiApEnableProtection_Wrapper(void);
void OledUtils_RenderESPWifiConnectedNotification(void);
void OledUtils_RenderESPAPStartedNotification(void);

void OledUtils_ShowNotificationMs(RenderFunction renderFn, uint16_t timeout_ms);
void OledUtils_ShowNotificationMsEx(RenderFunction renderFn, uint16_t timeout_ms, OledNotificationHook onShow, OledNotificationHook onHide);
bool OledUtils_IsNotificationShowing(RenderFunction renderFn);
void OledUtils_ShowSimpleNotificationMsEx(OledSimpleNotificationId notificationId,
                                          uint16_t timeout_ms,
                                          OledNotificationHook onShow,
                                          OledNotificationHook onHide);
void OledUtils_ShowSimpleNotificationMs(OledSimpleNotificationId notificationId, uint16_t timeout_ms);
void OledUtils_ShowNumberedSimpleNotificationMsEx(OledSimpleNotificationId notificationId,
                                                   uint8_t clientNumber,
                                                   uint16_t timeout_ms,
                                                   OledNotificationHook onShow,
                                                   OledNotificationHook onHide);
uint8_t OledUtils_GetNotificationClientNumber(void);
bool OledUtils_IsSimpleNotificationShowing(OledSimpleNotificationId notificationId);
void OledUtils_DismissNotification(void);
void OledUtils_NextNotification(void);
void OledUtils_RemoveNotification(RenderFunction renderFn);
void OledUtils_RemoveSimpleNotification(OledSimpleNotificationId notificationId);
uint8_t OledUtils_GetNotificationCount(void);
void OledUtils_SetNotificationsBlocked(bool blocked);
void OledUtils_NotificationTick10ms(void);
bool OledUtils_GetNotificationProgress(uint16_t *remaining_ms, uint16_t *total_ms);
void OledUtils_ShowRemoteModeChangedNotification(uint8_t mode, uint16_t timeout_ms);

void OledUtils_NotificationRestore(void);

#endif /* INC_OLED_UTILS_H_ */
