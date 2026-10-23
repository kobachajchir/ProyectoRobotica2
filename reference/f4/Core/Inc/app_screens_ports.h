#ifndef APP_SCREENS_PORTS_H
#define APP_SCREENS_PORTS_H
#include "globals.h"
#include "main.h"
#include "screen_context.h"
#include "osk.h"
#include "menu_definitions.h"
#include <string.h>
#include "eventManagers.h"
#include "app_tasks.h"
#include "app_wifi_binding.h"
#include "app_display.h"
#include "app_preferences_binding.h"
#include "app_ap_config.h"
#include "permissions.h"
#include "menu_favorites.h"
#include "uner_app.h"
static inline bool AppScreensRouteLights(void) { return HAL_GPIO_ReadPin(Luces_IR_GPIO_Port,Luces_IR_Pin)==GPIO_PIN_SET; }
static inline uint16_t AppScreens_AppPreferences_ClampScreenLockTimeoutSec(uint16_t timeoutSec) { return AppPreferences_ClampScreenLockTimeoutSec(timeoutSec); }
static inline uint8_t AppScreens_AppPreferences_GetEspNotifWatchdogLimit(void) { return AppPreferences_GetEspNotifWatchdogLimit(); }
static inline uint16_t AppScreens_AppPreferences_GetScreenLockTimeoutSec(void) { return AppPreferences_GetScreenLockTimeoutSec(); }
static inline const char* AppScreens_AppPreferences_GetWifiApPassword(void) { return AppPreferences_GetWifiApPassword(); }
static inline const char* AppScreens_AppPreferences_GetWifiApSsid(void) { return AppPreferences_GetWifiApSsid(); }
static inline bool AppScreens_AppPreferences_IsScreenLockEnabled(void) { return AppPreferences_IsScreenLockEnabled(); }
static inline bool AppScreens_AppPreferences_IsWifiApFallbackEnabled(void) { return AppPreferences_IsWifiApFallbackEnabled(); }
static inline AM_Result AppScreens_AppPreferences_SetScreenLockTimeoutSec(uint16_t timeoutSec) { return AppPreferences_SetScreenLockTimeoutSec(timeoutSec); }
static inline const WifiBridge_Credentials * AppScreens_WifiBridge_GetRuntimeCredentials(void) { return WifiBridge_GetRuntimeCredentials(); }
static inline void AppScreens_ssd1306_Clear(void) { ssd1306_Clear(); }
static inline void AppScreens_ssd1306_DrawBitmap(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP) { ssd1306_DrawBitmap(X, Y, W, H, pBMP); }
static inline void AppScreens_ssd1306_DrawBitmap_MSB(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP) { ssd1306_DrawBitmap_MSB(X, Y, W, H, pBMP); }
static inline void AppScreens_ssd1306_DrawHorizontalLine(int16_t x, int16_t y, int16_t length) { ssd1306_DrawHorizontalLine(x, y, length); }
static inline void AppScreens_ssd1306_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1) { ssd1306_DrawLine(x0, y0, x1, y1); }
static inline void AppScreens_ssd1306_DrawLinearProgress(uint8_t x,
                                uint8_t y,
                                uint8_t width,
                                uint8_t height,
                                uint32_t elapsed,
                                uint32_t total) { ssd1306_DrawLinearProgress(x, y, width, height, elapsed, total); }
static inline void AppScreens_ssd1306_DrawPixel(uint8_t x, uint8_t y) { ssd1306_DrawPixel(x, y); }
static inline void AppScreens_ssd1306_DrawRect(int16_t x, int16_t y, int16_t width, int16_t height) { ssd1306_DrawRect(x, y, width, height); }
static inline void AppScreens_ssd1306_DrawVerticalLine(int16_t x, int16_t y, int16_t length) { ssd1306_DrawVerticalLine(x, y, length); }
static inline void AppScreens_ssd1306_FillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height) { ssd1306_FillRect(xMove, yMove, width, height); }
static inline uint8_t AppScreens_ssd1306_GetDefaultContrast(void) { return ssd1306_GetDefaultContrast(); }
static inline uint32_t AppScreens_ssd1306_GetErrorCount(void) { return ssd1306_GetErrorCount(); }
static inline AM_Result AppScreens_ssd1306_PresentFramebuffer(const uint8_t *framebuffer,
                                             uint16_t len) { return ssd1306_PresentFramebuffer(framebuffer, len); }
static inline AM_Result AppScreens_ssd1306_RequestPageUpdate(uint8_t page,
                                            I2C_RequestPrio prio,
                                            uint8_t keep_bus) { return ssd1306_RequestPageUpdate(page, prio, keep_bus); }
static inline void AppScreens_ssd1306_SetColor(SSD1306_COLOR color) { ssd1306_SetColor(color); }
static inline AM_Result AppScreens_ssd1306_SetContrast(uint8_t contrast) { return ssd1306_SetContrast(contrast); }
static inline void AppScreens_ssd1306_SetCursor(uint8_t x, uint8_t y) { ssd1306_SetCursor(x, y); }
static inline AM_Result AppScreens_ssd1306_SetRotation(OledRotation_t rotation) { return ssd1306_SetRotation(rotation); }
static inline void AppScreens_ssd1306_UpdateScreen(void) { ssd1306_UpdateScreen(); }
static inline char AppScreens_ssd1306_UpdateScreenCompleted(void) { return ssd1306_UpdateScreenCompleted(); }
static inline char AppScreens_ssd1306_WriteChar(char ch, FontDef Font) { return ssd1306_WriteChar(ch, Font); }
static inline char AppScreens_ssd1306_WriteString(char* str, FontDef Font) { return ssd1306_WriteString(str, Font); }
static const ScreensServices appScreensServices={
 .now_ms=HAL_GetTick,
 .route_emitters_on=AppScreensRouteLights,
 .APP_EvaluateRoute=APP_EvaluateRoute,
 .APP_LineCalibrationBusy=APP_LineCalibrationBusy,
 .APP_ESPFirmwareRequestRequested=APP_ESPFirmwareRequestRequested,
 .APP_ESPModeChangeCommandAccepted=APP_ESPModeChangeCommandAccepted,
 .APP_ESPStatusRefreshRequested=APP_ESPStatusRefreshRequested,
 .APP_IsIrLineEnabled=APP_IsIrLineEnabled,
 .APP_IsIrObjectsEnabled=APP_IsIrObjectsEnabled,
 .APP_MPU_GetCalibrationProgressPercent=APP_MPU_GetCalibrationProgressPercent,
 .APP_MPU_IsCalibrationDoneNoticeActive=APP_MPU_IsCalibrationDoneNoticeActive,
 .APP_MPU_IsCalibrationPromptActive=APP_MPU_IsCalibrationPromptActive,
 .APP_MPU_IsCalibrationRunning=APP_MPU_IsCalibrationRunning,
 .About_UserEventManager=About_UserEventManager,
 .AppApConfig_ApplyPatch=AppApConfig_ApplyPatch,
 .AppApConfig_Get=AppApConfig_Get,
 .AppApConfig_ParseIpv4=AppApConfig_ParseIpv4,
 .AppPreferences_ClampScreenLockTimeoutSec=AppScreens_AppPreferences_ClampScreenLockTimeoutSec,
 .AppPreferences_GetEspNotifWatchdogLimit=AppScreens_AppPreferences_GetEspNotifWatchdogLimit,
 .AppPreferences_GetScreenLockTimeoutSec=AppScreens_AppPreferences_GetScreenLockTimeoutSec,
 .AppPreferences_GetWifiApPassword=AppScreens_AppPreferences_GetWifiApPassword,
 .AppPreferences_GetWifiApSsid=AppScreens_AppPreferences_GetWifiApSsid,
 .AppPreferences_IsScreenLockEnabled=AppScreens_AppPreferences_IsScreenLockEnabled,
 .AppPreferences_IsWifiApFallbackEnabled=AppScreens_AppPreferences_IsWifiApFallbackEnabled,
 .AppPreferences_SetScreenLockTimeoutSec=AppScreens_AppPreferences_SetScreenLockTimeoutSec,
 .BalanceControl_GetStatus=BalanceControl_GetStatus,
 .BalancePid_GetCurrentGainDraft=BalancePid_GetCurrentGainDraft,
 .BalancePid_GetCurrentGainLabel=BalancePid_GetCurrentGainLabel,
 .BalancePid_GetCurrentGainScreenCode=BalancePid_GetCurrentGainScreenCode,
 .BalancePid_GetLogAvailableBytes=BalancePid_GetLogAvailableBytes,
 .BalancePid_GetLogDurationSec=BalancePid_GetLogDurationSec,
 .BalancePid_GetLogEstimatedBytes=BalancePid_GetLogEstimatedBytes,
 .ClickCancelar_UserEventManager=ClickCancelar_UserEventManager,
 .Dashboard_GetActionTimeoutContext=Dashboard_GetActionTimeoutContext,
 .Dashboard_ResetModeConfirmState=Dashboard_ResetModeConfirmState,
 .GenericConfirm_GetLine1=GenericConfirm_GetLine1,
 .GenericConfirm_GetLine2=GenericConfirm_GetLine2,
 .GenericConfirm_GetScreenCode=GenericConfirm_GetScreenCode,
 .IRValues_UserEventManager=IRValues_UserEventManager,
 .MPU9250_GetLatest=MPU9250_GetLatest,
 .MPUValues_UserEventManager=MPUValues_UserEventManager,
 .MenuSys_GetCurrentScreenCode=MenuSys_GetCurrentScreenCode,
 .MenuSys_GetCurrentScreenSource=MenuSys_GetCurrentScreenSource,
 .MenuSys_IsFavorite=MenuSys_IsFavorite,
 .MenuSys_IsFavoritesMenu=MenuSys_IsFavoritesMenu,
 .MenuSys_IsItemVisible=MenuSys_IsItemVisible,
 .MenuSys_NavigateBack=MenuSys_NavigateBack,
 .MenuSys_RenderMenu=MenuSys_RenderMenu,
 .MenuSys_SetCurrentMenuScreenCode=MenuSys_SetCurrentMenuScreenCode,
 .MenuSys_SetCurrentScreenCode=MenuSys_SetCurrentScreenCode,
 .MotionControl_GetStatus=MotionControl_GetStatus,
 .MotionControl_IsArmed=MotionControl_IsArmed,
 .MotorSingleTest_GetDirectionLabel=MotorSingleTest_GetDirectionLabel,
 .MotorSingleTest_GetLabel=MotorSingleTest_GetLabel,
 .MotorSingleTest_GetMaxPwm=MotorSingleTest_GetMaxPwm,
 .MotorSingleTest_GetMinPwm=MotorSingleTest_GetMinPwm,
 .MotorSingleTest_GetPwm=MotorSingleTest_GetPwm,
 .MotorSingleTest_GetScreenCode=MotorSingleTest_GetScreenCode,
 .MotorSingleTest_GetSpeedPercentDisplay=MotorSingleTest_GetSpeedPercentDisplay,
 .Motor_SpeedPercentToDisplay=Motor_SpeedPercentToDisplay,
 .OSK_GetText=OSK_GetText,
 .OSK_IsActive=OSK_IsActive,
 .OSK_OnBack=OSK_OnBack,
 .OSK_OnLeft=OSK_OnLeft,
 .OSK_OnRight=OSK_OnRight,
 .OSK_OnSelect=OSK_OnSelect,
 .OSK_Open=OSK_Open,
 .OSK_Render=OSK_Render,
 .Permission_IsRemoteNodeAuthorizedNow=Permission_IsRemoteNodeAuthorizedNow,
 .Permission_Request=Permission_Request,
 .Preferences_GetLineBlackWidthDraftMm=Preferences_GetLineBlackWidthDraftMm,
 .Preferences_GetMotorMinPwmDraft=Preferences_GetMotorMinPwmDraft,
 .Preferences_GetMotorMinPwmEditLabel=Preferences_GetMotorMinPwmEditLabel,
 .Preferences_GetMotorMinPwmEditScreenCode=Preferences_GetMotorMinPwmEditScreenCode,
 .Preferences_GetOledRotationDraftDeg=Preferences_GetOledRotationDraftDeg,
 .Preferences_GetOledRotationMpuTestDeg=Preferences_GetOledRotationMpuTestDeg,
 .Preferences_GetOledRotationMpuTestYawDeg=Preferences_GetOledRotationMpuTestYawDeg,
 .Preferences_GetSystemActionTimeoutDraft=Preferences_GetSystemActionTimeoutDraft,
 .Preferences_GetUsbAlivePeriodDraftMs=Preferences_GetUsbAlivePeriodDraftMs,
 .Preferences_IsOledRotationMpuTestReady=Preferences_IsOledRotationMpuTestReady,
 .Preferences_UpdateOledRotationFromMpuTest=Preferences_UpdateOledRotationFromMpuTest,
 .ReadOnly_UserEventManager=ReadOnly_UserEventManager,
 .RouteAlignment_UserEventManager=RouteAlignment_UserEventManager,
 .TCRT5000_EmittersGroup3IsOn=TCRT5000_EmittersGroup3IsOn,
 .TCRT5000_EmittersGroup3Off=TCRT5000_EmittersGroup3Off,
 .TCRT5000_EmittersGroup3On=TCRT5000_EmittersGroup3On,
 .TCRT5000_EmittersGroup3Set=TCRT5000_EmittersGroup3Set,
 .TCRT5000_EmittersGroup5Off=TCRT5000_EmittersGroup5Off,
 .TCRT5000_EmittersGroup5On=TCRT5000_EmittersGroup5On,
 .TCRT5000_EmittersGroup5Set=TCRT5000_EmittersGroup5Set,
 .TCRT5000_GetLineSensorDef=TCRT5000_GetLineSensorDef,
 .TCRT5000_GetObjectSensorDef=TCRT5000_GetObjectSensorDef,
 .TCRT5000_GetRaw=TCRT5000_GetRaw,
 .TCRT5000_GetRawById=TCRT5000_GetRawById,
 .TCRT5000_GetRouteAlignmentText=TCRT5000_GetRouteAlignmentText,
 .TCRT5000_NormalizeForDisplay=TCRT5000_NormalizeForDisplay,
 .TCRT5000_ReadRouteSensorsEx=TCRT5000_ReadRouteSensorsEx,
 .UNER_App_GetBootloaderVersion=UNER_App_GetBootloaderVersion,
 .UNER_App_GetRouterPacketSnapshot=UNER_App_GetRouterPacketSnapshot,
 .UNER_App_ReportNotificationChanged=UNER_App_ReportNotificationChanged,
 .UnerRouterTest_UserEventManager=UnerRouterTest_UserEventManager,
 .WiFiAtPassword_GetLength=WiFiAtPassword_GetLength,
 .WiFiAtPassword_GetSelectedCharacter=WiFiAtPassword_GetSelectedCharacter,
 .WiFiAtPassword_UserEventManager=WiFiAtPassword_UserEventManager,
 .WiFiDetails_UserEventManager=WiFiDetails_UserEventManager,
 .WiFiSearch_UserEventManager=WiFiSearch_UserEventManager,
 .WifiBridge_GetActiveBackend=WifiBridge_GetActiveBackend,
 .WifiBridge_GetBackendLabel=WifiBridge_GetBackendLabel,
 .WifiBridge_GetRuntimeCredentials=AppScreens_WifiBridge_GetRuntimeCredentials,
 .WifiBridge_GetStatus=WifiBridge_GetStatus,
 .WifiBridge_IsConnected=WifiBridge_IsConnected,
 .WifiBridge_IsWaitingValidation=WifiBridge_IsWaitingValidation,
 .WifiBridge_ReportCarModeChanged=WifiBridge_ReportCarModeChanged,
 .WifiBridge_SendCommand=WifiBridge_SendCommand,
 .WifiBridge_SendEspReboot=WifiBridge_SendEspReboot,
 .WifiBridge_StartAP=WifiBridge_StartAP,
 .WifiBridge_StartScan=WifiBridge_StartScan,
 .WifiBridge_StopAP=WifiBridge_StopAP,
 .balancePidGainEventManager=balancePidGainEventManager,
 .balancePidLogEventManager=balancePidLogEventManager,
 .balancePidStatusEventManager=balancePidStatusEventManager,
 .dashboardEventManager=dashboardEventManager,
 .dashboardQuickActionEventManager=dashboardQuickActionEventManager,
 .lineBlackWidthEventManager=lineBlackWidthEventManager,
 .menuEventManager=menuEventManager,
 .motorMinPwmEventManager=motorMinPwmEventManager,
 .motorSingleTestEventManager=motorSingleTestEventManager,
 .motorTestEventManager=motorTestEventManager,
 .mpuCalibrationEventManager=mpuCalibrationEventManager,
 .oledCanvasEventManager=oledCanvasEventManager,
 .oledRotationMpuTestEventManager=oledRotationMpuTestEventManager,
 .oledRotationTestEventManager=oledRotationTestEventManager,
 .screenLockTimeoutEventManager=screenLockTimeoutEventManager,
 .screenLockedEventManager=screenLockedEventManager,
 .ssd1306_Clear=AppScreens_ssd1306_Clear,
 .ssd1306_DrawBitmap=AppScreens_ssd1306_DrawBitmap,
 .ssd1306_DrawBitmap_MSB=AppScreens_ssd1306_DrawBitmap_MSB,
 .ssd1306_DrawHorizontalLine=AppScreens_ssd1306_DrawHorizontalLine,
 .ssd1306_DrawLine=AppScreens_ssd1306_DrawLine,
 .ssd1306_DrawLinearProgress=AppScreens_ssd1306_DrawLinearProgress,
 .ssd1306_DrawPixel=AppScreens_ssd1306_DrawPixel,
 .ssd1306_DrawRect=AppScreens_ssd1306_DrawRect,
 .ssd1306_DrawVerticalLine=AppScreens_ssd1306_DrawVerticalLine,
 .ssd1306_FillRect=AppScreens_ssd1306_FillRect,
 .ssd1306_GetDefaultContrast=AppScreens_ssd1306_GetDefaultContrast,
 .ssd1306_GetErrorCount=AppScreens_ssd1306_GetErrorCount,
 .ssd1306_PresentFramebuffer=AppScreens_ssd1306_PresentFramebuffer,
 .ssd1306_RequestPageUpdate=AppScreens_ssd1306_RequestPageUpdate,
 .ssd1306_SetColor=AppScreens_ssd1306_SetColor,
 .ssd1306_SetContrast=AppScreens_ssd1306_SetContrast,
 .ssd1306_SetCursor=AppScreens_ssd1306_SetCursor,
 .ssd1306_SetRotation=AppScreens_ssd1306_SetRotation,
 .ssd1306_UpdateScreen=AppScreens_ssd1306_UpdateScreen,
 .ssd1306_UpdateScreenCompleted=AppScreens_ssd1306_UpdateScreenCompleted,
 .ssd1306_WriteChar=AppScreens_ssd1306_WriteChar,
 .ssd1306_WriteString=AppScreens_ssd1306_WriteString,
 .systemActionTimeoutEventManager=systemActionTimeoutEventManager,
 .usbAlivePeriodEventManager=usbAlivePeriodEventManager,
 .wifiEventManager=wifiEventManager,
};
static inline void AppScreensBind(Screens_Handle *h) {
    memset(h,0,sizeof(*h));
    h->system=&autoMicroSystem; h->services=&appScreensServices;
    h->menuSystem=&menuSystem;
    h->encoder=&encoder;
    h->wifiBridge=&wifiBridge;
    h->irSensors=&irSensors;
    h->motorTask=&motorTask;
    h->balanceControl=&balanceControl;
    h->motionControl=&motionControl;
    h->lineFollowControl=&lineFollowControl;
    h->mpuTask=&mpuTask;
    h->mainMenu=&mainMenu;
    h->submenu1=&submenu1;
    h->submenuESP=&submenuESP;
    h->submenuSensorsDisplay=&submenuSensorsDisplay;
    h->wifiResultsMenu=&wifiResultsMenu;
    h->wifiResultsItems=wifiResultsItems;
    h->oledHandle=&oledHandle;
    h->buildDate=buildDate; h->buildTime=buildTime; Screens_Context=h;
    Screens_Init_oled_utils();
    Screens_Init_screenWrappers();
    Screens_Init_notificationWrappers();
    Screens_Init_osk_integration();
    Screens_Init_screen_lock();
    Screens_Init_oled_canvas();
}
#endif
