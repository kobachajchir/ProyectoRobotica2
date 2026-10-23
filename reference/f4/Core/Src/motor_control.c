/*
 * motor_control.c
 *
 * DRV8833 motor control layer for the STM32F411 port.
 */

#include "motor_control.h"

#include <stddef.h>
#include <math.h>

static uint16_t Motor_NormalizeMaxPwm(uint16_t max_pwm)
{
    return (max_pwm == 0u) ? MOTOR_PWM_MAX_DEFAULT : max_pwm;
}

static float Motor_AbsPercent(float signed_percent)
{
    return (signed_percent < 0.0f) ? -signed_percent : signed_percent;
}

float Motor_ClampSpeedPercent(float speed_percent)
{
    if (!isfinite(speed_percent) || speed_percent <= MOTOR_SPEED_PERCENT_MIN) {
        return MOTOR_SPEED_PERCENT_MIN;
    }
    if (speed_percent >= MOTOR_SPEED_PERCENT_MAX) {
        return MOTOR_SPEED_PERCENT_MAX;
    }
    return speed_percent;
}

uint8_t Motor_SpeedPercentToDisplay(float speed_percent)
{
    float clamped = Motor_ClampSpeedPercent(speed_percent);
    return (uint8_t)(clamped + 0.5f);
}

uint16_t Motor_SpeedPercentToPwm(const Motor_Handle_t *motor,
                                 float speed_percent,
                                 Motor_Direction_t direction)
{
    float clamped;
    uint16_t min_pwm;
    uint16_t max_pwm;
    float pwm;

    if (motor == NULL) {
        return 0u;
    }

    clamped = Motor_ClampSpeedPercent(speed_percent);
    if (clamped <= MOTOR_SPEED_PERCENT_MIN) {
        return 0u;
    }

    max_pwm = Motor_NormalizeMaxPwm(motor->max_pwm);
    min_pwm = (direction == MOTOR_DIR_FORWARD)
        ? motor->min_pwm_forward
        : motor->min_pwm_backward;

    if (min_pwm > max_pwm) {
        min_pwm = max_pwm;
    }

    pwm = (float)min_pwm + ((clamped / 100.0f) * (float)(max_pwm - min_pwm));
    if (pwm >= (float)max_pwm) {
        return max_pwm;
    }

    return (uint16_t)(pwm + 0.5f);
}

AM_Result Motor_Init(Motor_Handle_t *motor, const Motor_InitConfig_t *config)
{
    if ((motor == NULL) || (config == NULL) || (config->htim == NULL) || (config->port == NULL) || !config->port->start || !config->port->set_compare) {
        return AM_ERROR;
    }

    motor->htim = config->htim;
    motor->port = config->port;
    motor->forward_channel = config->forward_channel;
    motor->backward_channel = config->backward_channel;
    motor->min_pwm_forward = config->min_pwm_forward;
    motor->min_pwm_backward = config->min_pwm_backward;
    motor->max_pwm = Motor_NormalizeMaxPwm(config->max_pwm);
    motor->speedPercent = MOTOR_SPEED_PERCENT_MIN;
    motor->direction = MOTOR_DIR_FORWARD;

    Motor_Stop(motor);
    return AM_OK;
}

AM_Result Motor_Start(Motor_Handle_t *motor)
{
    if ((motor == NULL) || (motor->htim == NULL)) {
        return AM_ERROR;
    }

    if (motor->port->start(motor->htim, motor->forward_channel) != AM_OK) {
        return AM_ERROR;
    }
    if (motor->port->start(motor->htim, motor->backward_channel) != AM_OK) {
        return AM_ERROR;
    }

    Motor_Stop(motor);
    return AM_OK;
}

AM_Result Motor_SetSpeedPercent(Motor_Handle_t *motor,
                                        float speed_percent,
                                        Motor_Direction_t direction)
{
    uint16_t pwm;

    if ((motor == NULL) || (motor->htim == NULL)) {
        return AM_ERROR;
    }

    if (!isfinite(speed_percent) ||
        (direction != MOTOR_DIR_FORWARD && direction != MOTOR_DIR_BACKWARD)) {
        Motor_Stop(motor);
        return AM_ERROR;
    }
    motor->speedPercent = Motor_ClampSpeedPercent(speed_percent);
    motor->direction = direction;
    pwm = Motor_SpeedPercentToPwm(motor, motor->speedPercent, direction);

    if (direction == MOTOR_DIR_FORWARD) {
        motor->port->set_compare(motor->htim, motor->backward_channel, 0u);
        motor->port->set_compare(motor->htim, motor->forward_channel, pwm);
    } else {
        motor->port->set_compare(motor->htim, motor->forward_channel, 0u);
        motor->port->set_compare(motor->htim, motor->backward_channel, pwm);
    }

    return AM_OK;
}

AM_Result Motor_SetSignedPercent(Motor_Handle_t *motor,
                                         float signed_percent)
{
    Motor_Direction_t direction = MOTOR_DIR_FORWARD;
    float speed_percent = Motor_AbsPercent(signed_percent);

    if (signed_percent < 0.0f) {
        direction = MOTOR_DIR_BACKWARD;
    }

    return Motor_SetSpeedPercent(motor, speed_percent, direction);
}

AM_Result Motor_Forward(Motor_Handle_t *motor, float speed_percent)
{
    return Motor_SetSpeedPercent(motor, speed_percent, MOTOR_DIR_FORWARD);
}

AM_Result Motor_Backward(Motor_Handle_t *motor, float speed_percent)
{
    return Motor_SetSpeedPercent(motor, speed_percent, MOTOR_DIR_BACKWARD);
}

void Motor_Stop(Motor_Handle_t *motor)
{
    if ((motor == NULL) || (motor->htim == NULL)) {
        return;
    }

    motor->speedPercent = MOTOR_SPEED_PERCENT_MIN;
    motor->port->set_compare(motor->htim, motor->forward_channel, 0u);
    motor->port->set_compare(motor->htim, motor->backward_channel, 0u);
}

void Motor_Brake(Motor_Handle_t *motor)
{
    uint16_t max_pwm;

    if ((motor == NULL) || (motor->htim == NULL)) {
        return;
    }

    max_pwm = Motor_NormalizeMaxPwm(motor->max_pwm);
    motor->speedPercent = MOTOR_SPEED_PERCENT_MIN;
    motor->port->set_compare(motor->htim, motor->forward_channel, max_pwm);
    motor->port->set_compare(motor->htim, motor->backward_channel, max_pwm);
}

void Motor_SetMinPwm(Motor_Handle_t *motor,
                     uint16_t min_pwm_forward,
                     uint16_t min_pwm_backward)
{
    uint16_t max_pwm;

    if (motor == NULL) {
        return;
    }

    max_pwm = Motor_NormalizeMaxPwm(motor->max_pwm);
    motor->min_pwm_forward = (min_pwm_forward > max_pwm) ? max_pwm : min_pwm_forward;
    motor->min_pwm_backward = (min_pwm_backward > max_pwm) ? max_pwm : min_pwm_backward;
}

AM_Result MotorDrive_Init(MotorControl_Handle *drive,
                                  const MotorDrive_InitConfig_t *config)
{
    if ((drive == NULL) || (config == NULL)) {
        return AM_ERROR;
    }

    if (Motor_Init(&drive->leftMotor, &config->left) != AM_OK) {
        return AM_ERROR;
    }
    if (Motor_Init(&drive->rightMotor, &config->right) != AM_OK) {
        return AM_ERROR;
    }
    if (Motor_Start(&drive->leftMotor) != AM_OK) {
        return AM_ERROR;
    }
    if (Motor_Start(&drive->rightMotor) != AM_OK) {
        return AM_ERROR;
    }

    return AM_OK;
}

void MotorDrive_StopAll(MotorControl_Handle *drive)
{
    if (drive == NULL) {
        return;
    }

    Motor_Stop(&drive->leftMotor);
    Motor_Stop(&drive->rightMotor);
}

void MotorDrive_SetLeftPercent(MotorControl_Handle *drive,
                               float speed_percent,
                               Motor_Direction_t direction)
{
    if (drive == NULL) {
        return;
    }

    (void)Motor_SetSpeedPercent(&drive->leftMotor, speed_percent, direction);
}

void MotorDrive_SetRightPercent(MotorControl_Handle *drive,
                                float speed_percent,
                                Motor_Direction_t direction)
{
    if (drive == NULL) {
        return;
    }

    (void)Motor_SetSpeedPercent(&drive->rightMotor, speed_percent, direction);
}

void MotorDrive_SetBothPercent(MotorControl_Handle *drive,
                               float speed_percent,
                               Motor_Direction_t direction)
{
    if (drive == NULL) {
        return;
    }

    MotorDrive_SetLeftPercent(drive, speed_percent, direction);
    MotorDrive_SetRightPercent(drive, speed_percent, direction);
}

void MotorDrive_SetSignedPercent(MotorControl_Handle *drive,
                                 float left_percent,
                                 float right_percent)
{
    if (drive == NULL) {
        return;
    }

    if (!isfinite(left_percent) || !isfinite(right_percent)) {
        MotorDrive_StopAll(drive);
        return;
    }
    (void)Motor_SetSignedPercent(&drive->leftMotor, left_percent);
    (void)Motor_SetSignedPercent(&drive->rightMotor, right_percent);
}

void MotorDrive_SetLeftMinPwm(MotorControl_Handle *drive, uint16_t min_pwm)
{
    if (drive == NULL) {
        return;
    }

    Motor_SetMinPwm(&drive->leftMotor, min_pwm, min_pwm);
}

void MotorDrive_SetRightMinPwm(MotorControl_Handle *drive, uint16_t min_pwm)
{
    if (drive == NULL) {
        return;
    }

    Motor_SetMinPwm(&drive->rightMotor, min_pwm, min_pwm);
}

void MotorDrive_ApplyTestState(MotorControl_Handle *drive)
{
    uint8_t selected;

    if (drive == NULL) {
        return;
    }

    if ((drive->motorData.flags.byte & ENABLE_MOVEMENT) == 0u) {
        MotorDrive_StopAll(drive);
        return;
    }

    selected = (uint8_t)((drive->motorData.flags.byte >> 4u) & 0x0Fu);
    switch (selected) {
    case MOTORLEFT_SELECTED:
        MotorDrive_SetLeftPercent(drive,
                                  drive->motorData.motorLeft.speedPercent,
                                  drive->motorData.motorLeft.motorDirection);
        Motor_Stop(&drive->rightMotor);
        break;

    case MOTORRIGHT_SELECTED:
        Motor_Stop(&drive->leftMotor);
        MotorDrive_SetRightPercent(drive,
                                   drive->motorData.motorRight.speedPercent,
                                   drive->motorData.motorRight.motorDirection);
        break;

    case MOTORNONE_SELECTED:
    default:
        MotorDrive_SetLeftPercent(drive,
                                  drive->motorData.motorLeft.speedPercent,
                                  drive->motorData.motorLeft.motorDirection);
        MotorDrive_SetRightPercent(drive,
                                   drive->motorData.motorRight.speedPercent,
                                   drive->motorData.motorRight.motorDirection);
        break;
    }
}
