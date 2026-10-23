/*
 * motor_control.h
 *
 * DRV8833 motor control layer for the STM32F411 port.
 */

#ifndef INC_MOTOR_CONTROL_H_DATA
#define INC_MOTOR_CONTROL_H_DATA

#include "am_port.h"
#include "types/bitmap_type.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENABLE_MOVEMENT       0x01u

#define MOTORLEFT_SELECTED    0u
#define MOTORRIGHT_SELECTED   1u
#define MOTORNONE_SELECTED    2u

#define MOTOR_PWM_MAX_DEFAULT             255u
#define MOTOR_MIN_PWM_DEFAULT_FORWARD     60u
#define MOTOR_MIN_PWM_DEFAULT_BACKWARD    60u
#define MOTOR_SPEED_PERCENT_MIN           0.0f
#define MOTOR_SPEED_PERCENT_MAX           100.0f
#define MOTOR_TEST_SPEED_STEP_PERCENT     5.0f

typedef enum {
    MOTOR_DIR_FORWARD = 0,
    MOTOR_DIR_BACKWARD = 1
} Motor_Direction_t;

typedef struct {
    float speedPercent;
    Motor_Direction_t motorDirection;
} Motor_Config_t;

typedef struct {
    Byte_Flag_Struct flags;
    Motor_Config_t motorLeft;
    Motor_Config_t motorRight;
} Motor_Data_t;

typedef struct {
    AM_Result (*start)(void *ctx, uint32_t channel);
    void (*set_compare)(void *ctx, uint32_t channel, uint16_t value);
} Motor_Port;

typedef struct {
    void *htim;
    const Motor_Port *port;
    uint32_t forward_channel;
    uint32_t backward_channel;
    uint16_t min_pwm_forward;
    uint16_t min_pwm_backward;
    uint16_t max_pwm;
} Motor_InitConfig_t;

typedef struct {
    void *htim;
    const Motor_Port *port;
    uint32_t forward_channel;
    uint32_t backward_channel;
    uint16_t min_pwm_forward;
    uint16_t min_pwm_backward;
    uint16_t max_pwm;
    float speedPercent;
    Motor_Direction_t direction;
} Motor_Handle_t;

typedef struct {
    Motor_InitConfig_t left;
    Motor_InitConfig_t right;
} MotorDrive_InitConfig_t;

typedef struct MotorControl_Handle {
    Motor_Handle_t leftMotor;
    Motor_Handle_t rightMotor;
    Motor_Data_t motorData;
} MotorControl_Handle;


#ifdef __cplusplus
}
#endif
#endif
