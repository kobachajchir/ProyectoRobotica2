/*
 * motion_control.h
 *
 * High-level motion arbiter for the balancing robot. Normal modes command
 * movement intent here; this layer shifts the balance setpoint and mixes turn
 * demand with the balance PID output. Raw PWM remains reserved for TEST mode.
 */

#ifndef INC_MOTION_CONTROL_H_DATA
#define INC_MOTION_CONTROL_H_DATA

#include "control_ports.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#include "motion_control_data.h"
typedef struct {
    MotionControl_Config_t config;
    MotionControl_Command_t command;
    MotionControl_Status_t status;
    float current_forward_percent;
    float current_turn_percent;
    uint8_t initialized;
    uint16_t last_sample_seq;
} MotionControl_Handle;


#ifdef __cplusplus
}
#endif
#endif
