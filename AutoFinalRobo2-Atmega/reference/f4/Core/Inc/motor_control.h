#ifndef INC_MOTOR_CONTROL_H_
#define INC_MOTOR_CONTROL_H_
#include "motor_control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
AM_Result Motor_Init(Motor_Handle_t *motor, const Motor_InitConfig_t *config);
AM_Result Motor_Start(Motor_Handle_t *motor);
AM_Result Motor_SetSpeedPercent(Motor_Handle_t *motor,
                                        float speed_percent,
                                        Motor_Direction_t direction);
AM_Result Motor_SetSignedPercent(Motor_Handle_t *motor,
                                         float signed_percent);
AM_Result Motor_Forward(Motor_Handle_t *motor, float speed_percent);
AM_Result Motor_Backward(Motor_Handle_t *motor, float speed_percent);
void Motor_Stop(Motor_Handle_t *motor);
void Motor_Brake(Motor_Handle_t *motor);
void Motor_SetMinPwm(Motor_Handle_t *motor,
                     uint16_t min_pwm_forward,
                     uint16_t min_pwm_backward);
uint16_t Motor_SpeedPercentToPwm(const Motor_Handle_t *motor,
                                 float speed_percent,
                                 Motor_Direction_t direction);
float Motor_ClampSpeedPercent(float speed_percent);
uint8_t Motor_SpeedPercentToDisplay(float speed_percent);

AM_Result MotorDrive_Init(MotorControl_Handle *drive,
                                  const MotorDrive_InitConfig_t *config);
void MotorDrive_StopAll(MotorControl_Handle *drive);
void MotorDrive_SetLeftPercent(MotorControl_Handle *drive,
                               float speed_percent,
                               Motor_Direction_t direction);
void MotorDrive_SetRightPercent(MotorControl_Handle *drive,
                                float speed_percent,
                                Motor_Direction_t direction);
void MotorDrive_SetBothPercent(MotorControl_Handle *drive,
                               float speed_percent,
                               Motor_Direction_t direction);
void MotorDrive_SetSignedPercent(MotorControl_Handle *drive,
                                 float left_percent,
                                 float right_percent);
void MotorDrive_SetLeftMinPwm(MotorControl_Handle *drive, uint16_t min_pwm);
void MotorDrive_SetRightMinPwm(MotorControl_Handle *drive, uint16_t min_pwm);
void MotorDrive_ApplyTestState(MotorControl_Handle *drive);

#ifdef __cplusplus
}
#endif

#endif /* INC_MOTOR_CONTROL_H_ */
