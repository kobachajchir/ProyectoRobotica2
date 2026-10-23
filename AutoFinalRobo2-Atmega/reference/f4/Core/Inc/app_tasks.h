#include "system.h"
#ifndef INC_APP_TASKS_H_
#define INC_APP_TASKS_H_

#include <stdint.h>
#include "app_preferences_binding.h"
#include "user_button.h"
#include "encoder.h"
#include "i2c_manager.h"
#include "types/carmode_type.h"
#include "tcrt5000_data.h"
#include "line_calibration.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APP_ADC_CHANNEL_COUNT             8U
#define APP_ADC_A0_PULLUP_BUFFER_INDEX    3U
#define APP_ADC_A4_PULLDOWN_BUFFER_INDEX  4U

extern UserButton_Handle_t btnUser;
extern ENC_Handle_t encoder;
extern I2C_ManagerHandle i2cManager;

bool APP_AutoMotionBusy(void);
void APP_AutoMotionCancel(void);
uint8_t APP_AutoMotionRequest(uint8_t cmd,const uint8_t *request,uint8_t length,uint8_t *response);
#define APP_LINE_STATE_BYTES 168u
bool APP_LineCalibrationBusy(void);
uint8_t APP_BalanceSetEnabled(bool enabled);
uint8_t APP_LineCalibrationControl(uint8_t operation, uint32_t session);
uint8_t APP_LineFollowSetEnabled(bool enabled);
void APP_LineFollowStop(void);
bool APP_LineFollowEnabled(void);
void APP_GetLineState(uint8_t payload[APP_LINE_STATE_BYTES], uint8_t request_reason);
bool APP_EvaluateRoute(TCRT5000_RouteSample_t *out);
bool APP_ReadIrRouteSnapshot(uint16_t raw[8],TCRT5000_RouteSample_t *out);
void APP_CopyAdcSnapshot(uint16_t raw[8]);

void APP_Init(void);
void APP_Loop(void);
void APP_ESPModeChangeCommandAccepted(void);
void APP_ESPWifiConnectRequested(void);
void APP_ESPWifiApFallbackResponse(uint8_t status, uint8_t enabled);
uint8_t APP_GetWifiApFallbackSyncState(void);
void APP_ESPStatusRefreshRequested(void);
void APP_ESPFirmwareRequestRequested(void);
bool APP_DeferEspStartupRebootFailure(void);

void initCarMode(void);
void UserBtn_MainTask(UserButton_Handle_t *btn);
void Encoder_MainTask(ENC_Handle_t *enc);
void i2cManager_MainTask(void);
void Motor_MainTask(void);
uint8_t APP_MotorTestIsActive(void);
AM_Result APP_MotorTestSet(float left, float right, uint16_t duration_ms);
void APP_MotorTestStop(void);
void OLED_MainTask(void);
void APP_MPU_RequestCalibration(void);
void APP_ApplyPreferenceRuntimeChanges(AppPreferencesFieldMask_t changedFields);
void APP_ApplyIrState(void);

bool APP_IsIrObjectsEnabled(void);
bool APP_IsIrLineEnabled(void);
bool APP_IsIrLightsEnabled(void);
void APP_SetIrObjectsEnabled(bool enabled);
void APP_SetIrLineEnabled(bool enabled);
void APP_SetIrLightsEnabled(bool enabled);

bool APP_TransitionCarMode(CarMode_t mode);
uint32_t APP_USB_GetAliveCount(void);
uint8_t APP_MPU_IsCalibrationPromptActive(void);
uint8_t APP_MPU_IsCalibrationRunning(void);
uint8_t APP_MPU_GetCalibrationProgressPercent(void);
uint8_t APP_MPU_CanRecalibrate(void);
uint8_t APP_MPU_IsCalibrationDoneNoticeActive(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_APP_TASKS_H_ */
