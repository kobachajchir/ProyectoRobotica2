#ifndef INC_APP_PREFERENCES_H_
#define INC_APP_PREFERENCES_H_
#include "app_preferences_types.h"
#ifdef __cplusplus
extern "C" {
#endif
AM_Result AppPreferences_SetAutoMotionProfile_H(AppPreferences_Handle *h, const AutoMotion_Profile *profile);
AM_Result AppPreferences_SetLineCalibration_H(AppPreferences_Handle *h, const LineCalibration_Store *store);
void AppPreferences_Init_H(AppPreferences_Handle *h);
const AppPreferences_t *AppPreferences_Get_H(AppPreferences_Handle *h);
void AppPreferences_GetSnapshot_H(AppPreferences_Handle *h, AppPreferences_t *out);
void AppPreferences_GetDefaults(AppPreferences_t *out);
const AppPreferencesDescriptor_t *AppPreferences_GetDescriptors(uint8_t *count);

ext_profile_t AppPreferences_GetExtProfile_H(AppPreferences_Handle *h);
bool AppPreferences_IsEncoderInverted_H(AppPreferences_Handle *h);
bool AppPreferences_IsScreenLockEnabled_H(AppPreferences_Handle *h);
uint16_t AppPreferences_GetScreenLockTimeoutSec_H(AppPreferences_Handle *h);
uint16_t AppPreferences_ClampScreenLockTimeoutSec(uint16_t timeoutSec);
uint16_t AppPreferences_GetMotorLeftMinPwm_H(AppPreferences_Handle *h);
uint16_t AppPreferences_GetMotorRightMinPwm_H(AppPreferences_Handle *h);
uint16_t AppPreferences_ClampMotorMinPwm(uint16_t minPwm);
uint16_t AppPreferences_GetSystemActionTimeoutSec_H(AppPreferences_Handle *h);
uint16_t AppPreferences_ClampSystemActionTimeoutSec(uint16_t timeoutSec);
uint16_t AppPreferences_GetUsbAlivePeriodMs_H(AppPreferences_Handle *h);
uint16_t AppPreferences_ClampUsbAlivePeriodMs(uint16_t periodMs);
uint16_t AppPreferences_GetWebAlivePeriodMs_H(AppPreferences_Handle *h);
uint16_t AppPreferences_ClampWebAlivePeriodMs(uint16_t periodMs);
uint16_t AppPreferences_GetWifiAtProtocol_H(AppPreferences_Handle *h);


const char* AppPreferences_GetWifiApSsid_H(AppPreferences_Handle *h);
const char* AppPreferences_GetWifiApPassword_H(AppPreferences_Handle *h);
bool AppPreferences_IsWifiApPasswordProtected_H(AppPreferences_Handle *h);
AM_Result AppPreferences_SetWifiApSsid_H(AppPreferences_Handle *h, const char *ssid);
AM_Result AppPreferences_SetWifiApPassword_H(AppPreferences_Handle *h, const char *password);
AM_Result AppPreferences_SetWifiApPasswordProtected_H(AppPreferences_Handle *h, bool protected);
void AppPreferences_GetWifiApConfig_H(AppPreferences_Handle *h, AppApConfig_t *out);
AM_Result AppPreferences_SetWifiApConfig_H(AppPreferences_Handle *h, const AppApConfig_t *config, bool applyPending);
AM_Result AppPreferences_SetWifiApApplyPending_H(AppPreferences_Handle *h, bool pending);
bool AppPreferences_IsEspNotifWatchdogEnabled_H(AppPreferences_Handle *h);
uint8_t AppPreferences_GetEspNotifWatchdogLimit_H(AppPreferences_Handle *h);
uint16_t AppPreferences_GetOledRotationDeg_H(AppPreferences_Handle *h);
uint16_t AppPreferences_ClampOledRotationDeg(uint16_t rotationDeg);
uint16_t AppPreferences_GetLineBlackWidthMm_H(AppPreferences_Handle *h);
uint16_t AppPreferences_ClampLineBlackWidthMm(uint16_t widthMm);
bool AppPreferences_IsOledAutoRotationEnabled_H(AppPreferences_Handle *h);
bool AppPreferences_IsUnerRouterForwardingEnabled_H(AppPreferences_Handle *h);
bool AppPreferences_IsPinLockEnabled_H(AppPreferences_Handle *h);
bool AppPreferences_IsWifiApFallbackEnabled_H(AppPreferences_Handle *h);
bool AppPreferences_GetMpuCalibration_H(AppPreferences_Handle *h, AppPreferencesMpuCalibration_t *out);
void AppPreferences_GetBalancePid_H(AppPreferences_Handle *h, AppPreferencesBalancePid_t *out);
bool AppPreferences_VerifyAuthPin_H(AppPreferences_Handle *h, const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS]);
uint32_t AppPreferences_GetRecordSizeBytes(void);
uint32_t AppPreferences_GetRecordCapacity(void);
void AppPreferences_GetStorageInfo_H(AppPreferences_Handle *h, AppPreferencesStorageInfo_t *out);

AM_Result AppPreferences_SetExtProfile_H(AppPreferences_Handle *h, ext_profile_t ext_profile);
AM_Result AppPreferences_SetEncoderInverted_H(AppPreferences_Handle *h, bool inverted);
AM_Result AppPreferences_SetScreenLockEnabled_H(AppPreferences_Handle *h, bool enabled);
AM_Result AppPreferences_SetScreenLockTimeoutSec_H(AppPreferences_Handle *h, uint16_t timeoutSec);
AM_Result AppPreferences_SetMotorLeftMinPwm_H(AppPreferences_Handle *h, uint16_t minPwm);
AM_Result AppPreferences_SetMotorRightMinPwm_H(AppPreferences_Handle *h, uint16_t minPwm);
AM_Result AppPreferences_SetSystemActionTimeoutSec_H(AppPreferences_Handle *h, uint16_t timeoutSec);
AM_Result AppPreferences_SetUsbAlivePeriodMs_H(AppPreferences_Handle *h, uint16_t periodMs);
AM_Result AppPreferences_SetWebAlivePeriodMs_H(AppPreferences_Handle *h, uint16_t periodMs);
AM_Result AppPreferences_SetWifiAtProtocol_H(AppPreferences_Handle *h, uint16_t protocol);
AM_Result AppPreferences_SetOledRotationDeg_H(AppPreferences_Handle *h, uint16_t rotationDeg);
AM_Result AppPreferences_SetLineBlackWidthMm_H(AppPreferences_Handle *h, uint16_t widthMm);
AM_Result AppPreferences_SetOledAutoRotationEnabled_H(AppPreferences_Handle *h, bool enabled);
AM_Result AppPreferences_SetUnerRouterForwardingEnabled_H(AppPreferences_Handle *h, bool enabled);
AM_Result AppPreferences_SetPinLockEnabled_H(AppPreferences_Handle *h, bool enabled);
AM_Result AppPreferences_SetWifiApFallbackEnabled_H(AppPreferences_Handle *h, bool enabled);
AM_Result AppPreferences_SetMpuCalibration_H(AppPreferences_Handle *h, const AppPreferencesMpuCalibration_t *calibration);
AM_Result AppPreferences_SetBalancePid_H(AppPreferences_Handle *h, float kp, float ki, float kd);
AM_Result AppPreferences_ClearMpuCalibration_H(AppPreferences_Handle *h);
AM_Result AppPreferences_ChangeAuthPin_H(AppPreferences_Handle *h, const uint8_t currentDigits[APP_PREFS_AUTH_PIN_DIGITS],
    const uint8_t newDigits[APP_PREFS_AUTH_PIN_DIGITS]);
AM_Result AppPreferences_ApplyPatch_H(AppPreferences_Handle *h, const AppPreferencesPatch_t *patch,
                                            AppPreferencesFieldMask_t *changedFields);
AM_Result AppPreferences_ResetFields_H(AppPreferences_Handle *h, AppPreferencesFieldMask_t fields,
                                             AppPreferencesFieldMask_t *changedFields);

#ifdef __cplusplus
}
#endif

#endif /* INC_APP_PREFERENCES_H_ */
