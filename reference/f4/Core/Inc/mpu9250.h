#ifndef INC_MPU9250_H_
#define INC_MPU9250_H_
#include "mpu9250_types.h"
#ifdef __cplusplus
extern "C" {
#endif
AM_Result MPU9250_Init(MPU9250_Handle_t *hmpu,
                               uint8_t address,
                               const AM_I2C_DevicePort *bus,
                               MPU9250_Callback on_data_ready,
                               volatile bool *trigger);

AM_Result MPU9250_BindPort(MPU9250_Handle_t *hmpu);

AM_Result MPU9250_Configure(MPU9250_Handle_t *hmpu);

void MPU9250_CalibrateGyro(MPU9250_Handle_t *hmpu, uint16_t samples);
void MPU9250_StartCalibration(MPU9250_Handle_t *hmpu, uint16_t warmup_ms, uint16_t window_ms);
void MPU9250_CheckTrigger(MPU9250_Handle_t *hmpu);
void MPU9250_Process(MPU9250_Handle_t *hmpu);

AM_Result MPU9250_RequestAllData(MPU9250_Handle_t *hmpu);
AM_Result MPU9250_RequestAccel(MPU9250_Handle_t *hmpu);
AM_Result MPU9250_RequestGyro(MPU9250_Handle_t *hmpu);
AM_Result MPU9250_RequestTemp(MPU9250_Handle_t *hmpu);
AM_Result MPU9250_RequestOrientation(MPU9250_Handle_t *hmpu);

const MPU9250_IntData_t *MPU9250_GetData(MPU9250_Handle_t *hmpu);
AM_Result MPU9250_GetLatest(MPU9250_Handle_t *hmpu, MPU9250_Snapshot_t *out);
uint8_t MPU9250_IsSampleFresh(const MPU9250_Handle_t *hmpu, uint32_t now_ms);

uint8_t MPU9250_HasNewSample(const MPU9250_Handle_t *hmpu, uint16_t last_seq);
void MPU9250_ClearNewFlag(MPU9250_Handle_t *hmpu);
void MPU9250_SetReferenceNow(MPU9250_Handle_t *hmpu);
void MPU9250_GetOrientation(const MPU9250_Handle_t *hmpu, MPU9250_Orientation_t *out);
void MPU9250_AttitudeInit(MPU9250_AttitudeState_t *state,
                          float kp,
                          float ki,
                          float fixed_dt_s);
void MPU9250_AttitudeSetOffsets(MPU9250_AttitudeState_t *state,
                                const MPU9250_AttitudeOffsets_t *offsets);
void MPU9250_AttitudeResetYaw(MPU9250_AttitudeState_t *state);
void MPU9250_AttitudeUpdate(MPU9250_AttitudeState_t *state,
                            const MPU9250_NormalizedData_t *input,
                            MPU9250_Attitude_t *out);
AM_Result MPU9250_ApplyCalibrationData(MPU9250_Handle_t *hmpu,
                                               const MPU9250_CalibrationData_t *calibration);
AM_Result MPU9250_GetCalibrationData(const MPU9250_Handle_t *hmpu,
                                             MPU9250_CalibrationData_t *out);
void MPU9250_GetNormalizedData(const MPU9250_Handle_t *hmpu, MPU9250_NormalizedData_t *out);
AM_Result MPU9250_GetAttitude(const MPU9250_Handle_t *hmpu, MPU9250_Attitude_t *out);
void MPU9250_GetMagCalibration(const MPU9250_Handle_t *hmpu,
                               MPU9250_MagCalibration_t *out);
AM_Result MPU9250_SetMagCalibration(MPU9250_Handle_t *hmpu,
                                            const MPU9250_MagCalibration_t *calibration);
/* Final robot mount: sensor Y/pitch is robot front/back, sensor X/roll is robot lateral. */
float MPU9250_GetFrontTiltDeg(const MPU9250_Handle_t *hmpu);
float MPU9250_GetLateralTiltDeg(const MPU9250_Handle_t *hmpu);
float MPU9250_GetPitchDeg(const MPU9250_Handle_t *hmpu);
uint8_t MPU9250_IsCalibrating(const MPU9250_Handle_t *hmpu);
uint8_t MPU9250_IsCalibrated(const MPU9250_Handle_t *hmpu);
uint8_t MPU9250_GetCalibrationProgressPercent(const MPU9250_Handle_t *hmpu);
void MPU9250_Convert(MPU9250_Handle_t *hmpu);

AM_Result MPU9250_ReadWhoAmI(MPU9250_Handle_t *hmpu, uint8_t *whoami);

#ifdef __cplusplus
}
#endif

#endif /* INC_MPU9250_H_ */
