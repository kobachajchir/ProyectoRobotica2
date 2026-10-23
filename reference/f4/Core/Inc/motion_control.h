#ifndef INC_MOTION_CONTROL_H_
#define INC_MOTION_CONTROL_H_
#include "motion_control_types.h"
#ifdef __cplusplus
extern "C" {
#endif
void MotionControl_GetDefaultCommand(MotionControl_Command_t *command);
AM_Result MotionControl_Init(MotionControl_Handle *motion,
                                     const MotionControl_Config_t *config);
AM_Result MotionControl_SetCommand(MotionControl_Handle *motion,
                                           const MotionControl_Command_t *command,
                                           uint32_t now_ms);
void MotionControl_Arm(MotionControl_Handle *motion);
void MotionControl_Disarm(MotionControl_Handle *motion);
uint8_t MotionControl_IsArmed(const MotionControl_Handle *motion);
void MotionControl_Stop(MotionControl_Handle *motion, MotionControl_StopMode_t mode);
void MotionControl_EmergencyStop(MotionControl_Handle *motion);
void MotionControl_Task(MotionControl_Handle *motion, uint32_t now_ms);
const MotionControl_Status_t *MotionControl_GetStatus(const MotionControl_Handle *motion);

#ifdef __cplusplus
}
#endif

#endif /* INC_MOTION_CONTROL_H_ */
