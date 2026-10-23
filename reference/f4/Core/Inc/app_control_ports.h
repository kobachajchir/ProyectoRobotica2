#ifndef APP_CONTROL_PORTS_H
#define APP_CONTROL_PORTS_H
#include "pid_controller.h"
#include "mpu9250.h"
#include "motor_control.h"
#include "balance_control.h"
#include "motion_control.h"
#include "motion_estimator.h"
#include "tcrt5000.h"
static inline AM_Result App_PidInit(void *p,const PidController_Config_t *c) { return PidController_Init(p,c); }
static inline void App_PidReset(void *p) { PidController_Reset(p); }
static inline void App_PidSetpoint(void *p,float v) { PidController_SetSetpoint(p,v); }
static inline void App_PidGains(void *p,float a,float b,float c) { PidController_SetGains(p,a,b,c); }
static inline AM_Result App_PidUpdate(void *p,float a,float b,float c) { return PidController_Update(p,a,b,c); }
static inline float App_PidOutput(const void *p) { return PidController_GetOutput(p); }
static inline const PidController_Status_t *App_PidStatus(const void *p) { return PidController_GetStatus(p); }
static const AM_PidOps appPidOps={PidController_GetDefaultConfig,App_PidInit,App_PidReset,App_PidSetpoint,App_PidGains,App_PidUpdate,App_PidOutput,App_PidStatus};
static inline bool App_MpuReady(const void *p) { const MPU9250_Handle_t *m=p; return m && MPU9250_IsCalibrated(m) && !MPU9250_IsCalibrating(m) && (m->flags.byte&MPU_VALID) && !(m->flags.byte&MPU_ERROR); }
static inline uint8_t App_MpuCalibrating(const void *p) { return MPU9250_IsCalibrating(p); }
static inline uint8_t App_MpuFresh(const void *p,uint32_t ms) { return MPU9250_IsSampleFresh(p,ms); }
static inline float App_MpuFront(const void *p) { return MPU9250_GetFrontTiltDeg(p); }
static inline uint16_t App_MpuSequence(const void *p) { const MPU9250_Handle_t *m=p; return m->latest.sample_seq; }
static inline uint32_t App_MpuAttitudeMs(const void *p) { const MPU9250_Handle_t *m=p; return m->last_attitude_ms; }
static inline AM_Result App_MpuSnapshot(void *p,MPU9250_Snapshot_t *s) { return MPU9250_GetLatest(p,s); }
static inline uint32_t App_MpuNow(const void *p) { const MPU9250_Handle_t *m=p; return m->bus->hardware->now_ms(m->bus->hardware_ctx); }
static const AM_OrientationOps appOrientationOps={App_MpuReady,App_MpuCalibrating,App_MpuFresh,App_MpuFront,App_MpuSequence,App_MpuAttitudeMs,App_MpuSnapshot,App_MpuNow};
static inline void App_BalanceTarget(void *p,float v) { BalanceControl_SetTargetPitchDeg(p,v); }
static inline void App_BalanceEnable(void *p,uint8_t v) { BalanceControl_SetEnabled(p,v); }
static inline void App_BalanceReset(void *p) { BalanceControl_Reset(p); }
static inline AM_Result App_BalanceUpdate(void *p,float v,uint32_t ms) { return BalanceControl_Update(p,v,ms); }
static inline float App_BalanceOutput(const void *p) { return BalanceControl_GetOutputPercent(p); }
static const AM_BalanceOps appBalanceOps={App_BalanceTarget,App_BalanceEnable,App_BalanceReset,App_BalanceUpdate,App_BalanceOutput};
static inline void App_DriveStop(void *p) { MotorDrive_StopAll(p); }
static inline void App_DriveBrake(void *p) { MotorControl_Handle *m=p; Motor_Brake(&m->leftMotor); Motor_Brake(&m->rightMotor); }
static inline void App_DriveApply(void *p,float l,float r) { MotorDrive_SetSignedPercent(p,l,r); }
static const AM_DriveOps appDriveOps={App_DriveStop,App_DriveBrake,App_DriveApply};
static inline AM_Result App_LineCommand(void *p,const MotionControl_Command_t *c,uint32_t ms) { return MotionControl_SetCommand(p,c,ms); }
static inline uint8_t App_LineArmed(const void *p) { return MotionControl_IsArmed(p); }
static inline const MotionEstimator_Status_t *App_LineEstimate(const void *p) { return MotionEstimator_GetStatus(p); }
static inline bool App_LineEmitters(void *p) { return TCRT5000_EmittersGroup3IsOn(p); }
static const AM_LineOps appLineOps={MotionControl_GetDefaultCommand,App_LineCommand,App_LineArmed,App_LineEstimate,TCRT5000_GetDefaultLineConfig,TCRT5000_SetLineBlackWidth,App_LineEmitters,TCRT5000_ReadRouteSensorsEx};
#endif
