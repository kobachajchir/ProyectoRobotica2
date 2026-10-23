#ifndef INC_PID_CONTROLLER_H_
#define INC_PID_CONTROLLER_H_
#include "pid_controller_types.h"
#ifdef __cplusplus
extern "C" {
#endif
void PidController_GetDefaultConfig(PidController_Config_t *config);
AM_Result PidController_Init(PidController_Handle *pid,
                                     const PidController_Config_t *config);
void PidController_Reset(PidController_Handle *pid);
void PidController_SetGains(PidController_Handle *pid,
                            float kp,
                            float ki,
                            float kd);
void PidController_SetOutputLimits(PidController_Handle *pid,
                                   float output_min,
                                   float output_max);
void PidController_SetIntegralLimits(PidController_Handle *pid,
                                     float integral_min,
                                     float integral_max);
void PidController_SetSetpoint(PidController_Handle *pid, float setpoint);
AM_Result PidController_Update(PidController_Handle *pid,
                                       float setpoint,
                                       float measurement,
                                       float dt_s);
float PidController_GetOutput(const PidController_Handle *pid);
const PidController_Status_t *PidController_GetStatus(const PidController_Handle *pid);

#ifdef __cplusplus
}
#endif

#endif /* INC_PID_CONTROLLER_H_ */
