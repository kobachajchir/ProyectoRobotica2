/*
 * motion_control.c
 *
 * Central motion arbiter. Balance is always evaluated first in normal modes;
 * movement commands shift the pitch setpoint and add turn differential.
 */

#include "motion_control.h"

#include "utils/macros_utils.h"
#include <stddef.h>
#include <math.h>
#include <string.h>

static float Motion_Clamp(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static float Motion_Abs(float value)
{
    return (value < 0.0f) ? -value : value;
}


static float Motion_ProfileFactor(const MotionControl_Command_t *command,
                                  uint32_t start_ms,
                                  uint32_t now_ms,
                                  uint8_t *finished)
{
    uint32_t elapsed_ms;
    float factor = 1.0f;

    if (finished != NULL) {
        *finished = 0u;
    }

    if (command == NULL) {
        return 0.0f;
    }

    elapsed_ms = now_ms - start_ms;
    if (command->duration_ms != MOTION_CONTROL_COMMAND_NONE_DURATION_MS &&
        elapsed_ms >= command->duration_ms) {
        if (finished != NULL) {
            *finished = 1u;
        }
        return 0.0f;
    }

    if (command->ramp_up_ms > 0u && elapsed_ms < command->ramp_up_ms) {
        factor = (float)elapsed_ms / (float)command->ramp_up_ms;
    }

    if (command->duration_ms != MOTION_CONTROL_COMMAND_NONE_DURATION_MS &&
        command->ramp_down_ms > 0u) {
        const uint32_t ramp_start_ms =
            (command->duration_ms > command->ramp_down_ms)
                ? (uint32_t)(command->duration_ms - command->ramp_down_ms)
                : 0u;
        if (elapsed_ms < ramp_start_ms) {
            return Motion_Clamp(factor, 0.0f, 1.0f);
        }

        const uint32_t remaining_ms = command->duration_ms - elapsed_ms;
        const float down_factor = (float)remaining_ms / (float)command->ramp_down_ms;
        if (down_factor < factor) {
            factor = down_factor;
        }
    }

    return Motion_Clamp(factor, 0.0f, 1.0f);
}

static void Motion_StopOutputs(MotionControl_Handle *motion, MotionControl_StopMode_t mode)
{
    if (motion == NULL || motion->config.motors == NULL) {
        return;
    }

    if (mode == MOTION_STOP_BRAKE) {
        motion->config.drive_ops->brake(motion->config.motors);
    } else {
        motion->config.drive_ops->stop(motion->config.motors);
    }
}

static void Motion_ClearOutputStatus(MotionControl_Handle *motion)
{
    if (motion == NULL) {
        return;
    }

    motion->status.balance_output_percent = 0.0f;
    motion->status.forward_applied_percent = 0.0f;
    motion->status.turn_applied_percent = 0.0f;
    motion->status.turn_requested_percent = 0.0f;
    motion->status.turn_limit_percent = 0.0f;
    motion->status.u_common_percent = 0.0f;
    motion->status.u_diff_percent = 0.0f;
    motion->status.left_output_percent = 0.0f;
    motion->status.right_output_percent = 0.0f;
    motion->status.saturation_limited = 0u;
}

void MotionControl_GetDefaultCommand(MotionControl_Command_t *command)
{
    if (command == NULL) {
        return;
    }

    command->source = MOTION_SOURCE_BALANCE;
    command->forward_percent = 0.0f;
    command->turn_percent = 0.0f;
    command->duration_ms = MOTION_CONTROL_COMMAND_NONE_DURATION_MS;
    command->ramp_up_ms = 0u;
    command->ramp_down_ms = 0u;
    command->end_mode = MOTION_STOP_COAST;
}

AM_Result MotionControl_Init(MotionControl_Handle *motion,
                                     const MotionControl_Config_t *config)
{
    MotionControl_Command_t default_command;

    if (motion == NULL || config == NULL ||
        config->motors == NULL || config->balance == NULL || config->mpu == NULL ||
        !config->drive_ops || !config->balance_ops || !config->orientation) {
        return AM_ERROR;
    }

    memset(motion, 0, sizeof(*motion));
    motion->config = *config;
    if (motion->config.max_pitch_offset_deg <= 0.0f) {
        motion->config.max_pitch_offset_deg = MOTION_CONTROL_MAX_PITCH_OFFSET_DEG_DEFAULT;
    }
    if (motion->config.max_turn_percent <= 0.0f) {
        motion->config.max_turn_percent = MOTION_CONTROL_MAX_TURN_PERCENT_DEFAULT;
    }
    if (motion->config.safe_pitch_deg <= 0.0f) {
        motion->config.safe_pitch_deg = MOTION_CONTROL_SAFE_PITCH_DEG_DEFAULT;
    }
    if (motion->config.turn_headroom_ratio <= 0.0f) {
        motion->config.turn_headroom_ratio =
            MOTION_CONTROL_TURN_HEADROOM_RATIO_DEFAULT;
    }

    MotionControl_GetDefaultCommand(&default_command);
    motion->command = default_command;
    motion->config.enabled = motion->config.enabled ? 1u : 0u;
    motion->initialized = 1u;
    motion->status.source = MOTION_SOURCE_BALANCE;
    motion->status.reject_reason = MOTION_REJECT_NONE;

    return AM_OK;
}

AM_Result MotionControl_SetCommand(MotionControl_Handle *motion,
                                           const MotionControl_Command_t *command,
                                           uint32_t now_ms)
{
    if (motion == NULL || command == NULL || !motion->initialized) {
        return AM_ERROR;
    }

    if (!isfinite(command->forward_percent) || !isfinite(command->turn_percent)) {
        motion->status.reject_reason = MOTION_REJECT_INVALID_COMMAND;
        return AM_ERROR;
    }
    if (!motion->status.armed || motion->status.safety_stop) {
        motion->status.reject_reason = motion->status.safety_stop
            ? MOTION_REJECT_SAFETY_LATCHED : MOTION_REJECT_DISARMED;
        return AM_ERROR;
    }
    if (!motion->config.orientation->ready(motion->config.mpu) ||
        !motion->config.orientation->fresh(motion->config.mpu, now_ms)) {
        MotionControl_EmergencyStop(motion);
        motion->status.reject_reason = MOTION_REJECT_MPU_NOT_READY;
        return AM_ERROR;
    }
    motion->command = *command;
    motion->command.forward_percent = Motion_Clamp(command->forward_percent, -100.0f, 100.0f);
    motion->command.turn_percent = Motion_Clamp(command->turn_percent, -100.0f, 100.0f);
    motion->status.command_started_ms = now_ms;
    motion->status.command_active = 1u;
    motion->status.source = motion->command.source;

    return AM_OK;
}

void MotionControl_Stop(MotionControl_Handle *motion, MotionControl_StopMode_t mode)
{
    if (motion == NULL) {
        return;
    }

    MotionControl_GetDefaultCommand(&motion->command);
    motion->status.command_active = 0u;
    motion->status.source = MOTION_SOURCE_BALANCE;
    motion->current_forward_percent = 0.0f;
    motion->current_turn_percent = 0.0f;
    motion->status.pitch_setpoint_deg = 0.0f;
    if (motion->config.balance != NULL) {
        motion->config.balance_ops->target(motion->config.balance, 0.0f);
    }
    /* Stopping travel leaves the local balance loop in control of PWM. */
    if (!motion->status.armed || motion->status.safety_stop) {
        Motion_StopOutputs(motion, mode);
        Motion_ClearOutputStatus(motion);
    }
}

void MotionControl_Arm(MotionControl_Handle *motion)
{
    MotionControl_Command_t default_command;

    if (motion == NULL || !motion->initialized) {
        return;
    }

    MotionControl_RejectReason_t reason = MOTION_REJECT_NONE;
    if (!motion->config.enabled) reason = MOTION_REJECT_DISABLED;
    else if (motion->config.orientation->calibrating(motion->config.mpu)) reason = MOTION_REJECT_MPU_CALIBRATING;
    else if (!motion->config.orientation->ready(motion->config.mpu)) reason = MOTION_REJECT_MPU_NOT_READY;
    else if (!motion->config.orientation->fresh(motion->config.mpu, motion->config.orientation->now_ms(motion->config.mpu))) reason = MOTION_REJECT_MPU_STALE;
    else if (!isfinite(motion->config.orientation->front_tilt(motion->config.mpu)) ||
             Motion_Abs(motion->config.orientation->front_tilt(motion->config.mpu)) > MOTION_CONTROL_ARM_PITCH_DEG)
        reason = MOTION_REJECT_TILT_LIMIT;
    if (reason != MOTION_REJECT_NONE) {
        MotionControl_Disarm(motion);
        motion->status.reject_reason = reason;
        return;
    }
    motion->config.balance_ops->enable(motion->config.balance, 1u);
    motion->last_sample_seq = 0u;
    MotionControl_GetDefaultCommand(&default_command);
    motion->command = default_command;
    motion->status.command_active = 0u;
    motion->status.safety_stop = 0u;
    motion->status.armed = 1u;
    motion->status.reject_reason = MOTION_REJECT_NONE;
    motion->status.source = MOTION_SOURCE_BALANCE;
    motion->current_forward_percent = 0.0f;
    motion->current_turn_percent = 0.0f;
    if (motion->config.balance != NULL) {
        motion->config.balance_ops->target(motion->config.balance, 0.0f);
        motion->config.balance_ops->reset(motion->config.balance);
    }
    Motion_ClearOutputStatus(motion);
}

void MotionControl_Disarm(MotionControl_Handle *motion)
{
    if (motion == NULL) {
        return;
    }

    MotionControl_GetDefaultCommand(&motion->command);
    motion->status.armed = 0u;
    motion->status.command_active = 0u;
    motion->status.balance_active = 0u;
    motion->status.reject_reason = MOTION_REJECT_DISARMED;
    motion->current_forward_percent = 0.0f;
    motion->current_turn_percent = 0.0f;
    if (motion->config.balance != NULL) {
        motion->config.balance_ops->target(motion->config.balance, 0.0f);
        motion->config.balance_ops->reset(motion->config.balance);
    }
    Motion_StopOutputs(motion, MOTION_STOP_COAST);
    Motion_ClearOutputStatus(motion);
}

uint8_t MotionControl_IsArmed(const MotionControl_Handle *motion)
{
    return (motion != NULL && motion->status.armed) ? 1u : 0u;
}

void MotionControl_EmergencyStop(MotionControl_Handle *motion)
{
    if (motion == NULL) {
        return;
    }

    MotionControl_GetDefaultCommand(&motion->command);
    motion->status.safety_stop = 1u;
    motion->status.balance_active = 0u;
    motion->status.command_active = 0u;
    motion->status.reject_reason = MOTION_REJECT_TILT_LIMIT;
    motion->current_forward_percent = 0.0f;
    motion->current_turn_percent = 0.0f;
    if (motion->config.balance != NULL) {
        motion->config.balance_ops->reset(motion->config.balance);
    }
    Motion_StopOutputs(motion, MOTION_STOP_COAST);
    Motion_ClearOutputStatus(motion);
}

void MotionControl_Task(MotionControl_Handle *motion, uint32_t now_ms)
{
    float factor = 0.0f;
    uint8_t command_finished = 0u;
    float pitch_deg;
    float pitch_setpoint_deg;
    float balance_output;
    float turn_requested;
    float turn_limit;
    float turn_output;
    float headroom;
    float left_output;
    float right_output;
    const float safe_pitch_deg = (motion != NULL)
        ? Motion_Abs(motion->config.safe_pitch_deg)
        : MOTION_CONTROL_SAFE_PITCH_DEG_DEFAULT;

    if (motion == NULL || !motion->initialized) {
        return;
    }

    if (motion->status.safety_stop) {
        Motion_StopOutputs(motion, MOTION_STOP_COAST);
        return;
    }
    motion->status.balance_active = 0u;
    motion->status.reject_reason = MOTION_REJECT_NONE;
    motion->status.saturation_limited = 0u;

    if (!motion->config.enabled) {
        motion->status.reject_reason = MOTION_REJECT_DISABLED;
        Motion_StopOutputs(motion, MOTION_STOP_COAST);
        Motion_ClearOutputStatus(motion);
        return;
    }

    if (!motion->status.armed) {
        motion->status.reject_reason = MOTION_REJECT_DISARMED;
        Motion_StopOutputs(motion, MOTION_STOP_COAST);
        Motion_ClearOutputStatus(motion);
        return;
    }

    if (motion->config.mpu == NULL || motion->config.balance == NULL || motion->config.motors == NULL) {
        motion->status.reject_reason = MOTION_REJECT_NOT_INITIALIZED;
        return;
    }

    if (motion->config.orientation->calibrating(motion->config.mpu)) {
        MotionControl_EmergencyStop(motion);
        motion->status.reject_reason = MOTION_REJECT_MPU_CALIBRATING;
        return;
    }

    if (!motion->config.orientation->ready(motion->config.mpu)) {
        MotionControl_EmergencyStop(motion);
        motion->status.reject_reason = MOTION_REJECT_MPU_NOT_READY;
        return;
    }

    if (!motion->config.orientation->fresh(motion->config.mpu, now_ms)) {
        MotionControl_EmergencyStop(motion);
        motion->status.reject_reason = MOTION_REJECT_MPU_STALE;
        return;
    }
    pitch_deg = motion->config.orientation->front_tilt(motion->config.mpu);
    motion->status.pitch_measured_deg = pitch_deg;

    if (!isfinite(pitch_deg) || Motion_Abs(pitch_deg) > safe_pitch_deg) {
        MotionControl_EmergencyStop(motion);
        motion->status.pitch_measured_deg = pitch_deg;
        return;
    }

    motion->status.safety_stop = 0u;

    if (motion->status.command_active) {
        factor = Motion_ProfileFactor(&motion->command,
                                      motion->status.command_started_ms,
                                      now_ms,
                                      &command_finished);
        if (command_finished) {
            MotionControl_Stop(motion, motion->command.end_mode);
            MotionControl_GetDefaultCommand(&motion->command);
            motion->status.source = MOTION_SOURCE_BALANCE;
            factor = 0.0f;
        }
    }

    motion->current_forward_percent = Motion_Clamp(motion->command.forward_percent * factor,
                                                   -100.0f,
                                                   100.0f);
    motion->current_turn_percent = Motion_Clamp(motion->command.turn_percent * factor,
                                                -100.0f,
                                                100.0f);

    pitch_setpoint_deg = motion->config.neutral_pitch_deg + (motion->current_forward_percent / 100.0f) *
                         motion->config.max_pitch_offset_deg;
    turn_requested = (motion->current_turn_percent / 100.0f) *
                     motion->config.max_turn_percent;

    motion->config.balance_ops->target(motion->config.balance, pitch_setpoint_deg);
    if (motion->last_sample_seq == motion->config.orientation->sequence(motion->config.mpu)) {
        motion->status.balance_active = 1u;
        return;
    }
    if (motion->config.balance_ops->update(motion->config.balance, pitch_deg,
                              motion->config.orientation->attitude_ms(motion->config.mpu)) != AM_OK) {
        MotionControl_EmergencyStop(motion);
        motion->status.reject_reason = MOTION_REJECT_INVALID_COMMAND;
        return;
    }
    motion->last_sample_seq = motion->config.orientation->sequence(motion->config.mpu);

    balance_output = motion->config.balance_ops->output(motion->config.balance);
    headroom = 100.0f - Motion_Abs(balance_output);
    if (headroom < 0.0f) {
        headroom = 0.0f;
    }
    turn_limit = headroom * motion->config.turn_headroom_ratio;
    turn_limit = Motion_Clamp(turn_limit,
                              0.0f,
                              Motion_Abs(motion->config.max_turn_percent));
    turn_output = Motion_Clamp(turn_requested, -turn_limit, turn_limit);
    left_output = Motion_Clamp(balance_output - turn_output, -100.0f, 100.0f);
    right_output = Motion_Clamp(balance_output + turn_output, -100.0f, 100.0f);

    motion->config.drive_ops->apply(motion->config.motors, left_output, right_output);

    motion->status.pitch_setpoint_deg = pitch_setpoint_deg;
    motion->status.balance_output_percent = balance_output;
    motion->status.forward_applied_percent = motion->current_forward_percent;
    motion->status.turn_applied_percent = motion->current_turn_percent;
    motion->status.turn_requested_percent = turn_requested;
    motion->status.turn_limit_percent = turn_limit;
    motion->status.u_common_percent = balance_output;
    motion->status.u_diff_percent = turn_output;
    motion->status.left_output_percent = left_output;
    motion->status.right_output_percent = right_output;
    motion->status.balance_active = 1u;
    motion->status.saturation_limited =
        (Motion_Abs(turn_output - turn_requested) > 0.0001f ||
         Motion_Abs(left_output - (balance_output - turn_output)) > 0.0001f ||
         Motion_Abs(right_output - (balance_output + turn_output)) > 0.0001f)
            ? 1u
            : 0u;
}

const MotionControl_Status_t *MotionControl_GetStatus(const MotionControl_Handle *motion)
{
    return (motion != NULL) ? &motion->status : NULL;
}
