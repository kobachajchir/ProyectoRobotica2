/*
 * balance_control.c
 *
 * Balance PID adapter. Public API stays stable while the implementation uses
 * the reusable pid_controller core.
 */

#include "balance_control.h"

#include <stddef.h>
#include <math.h>
#include <string.h>

static float Balance_Clamp(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static float Balance_Abs(float value)
{
    return (value < 0.0f) ? -value : value;
}

void BalanceControl_GetDefaultConfig(BalanceControl_Config_t *config)
{
    if (config == NULL) {
        return;
    }

    config->pid_ops=NULL; config->pid_ctx=NULL;
    config->kp = BALANCE_CONTROL_DEFAULT_KP;
    config->ki = BALANCE_CONTROL_DEFAULT_KI;
    config->kd = BALANCE_CONTROL_DEFAULT_KD;
    config->default_dt_s = BALANCE_CONTROL_DEFAULT_DT_S;
    config->output_limit_percent = BALANCE_CONTROL_OUTPUT_LIMIT_PERCENT;
    config->integral_limit_deg_s = BALANCE_CONTROL_INTEGRAL_LIMIT_DEG_S;
    config->derivative_filter_tau_s = BALANCE_CONTROL_DERIVATIVE_FILTER_TAU_S;
    config->anti_windup_gain = BALANCE_CONTROL_ANTI_WINDUP_GAIN;
    config->output_inverted = 0u;
}

AM_Result BalanceControl_Init(BalanceControl_Handle *balance,
                                      const BalanceControl_Config_t *config)
{
    BalanceControl_Config_t default_config;
    PidController_Config_t pid_config;

    if (balance == NULL) {
        return AM_ERROR;
    }

    BalanceControl_GetDefaultConfig(&default_config);
    memset(balance, 0, sizeof(*balance));
    balance->config = (config != NULL) ? *config : default_config;

    if (balance->config.default_dt_s <= 0.0f) {
        balance->config.default_dt_s = BALANCE_CONTROL_DEFAULT_DT_S;
    }
    if (balance->config.output_limit_percent <= 0.0f) {
        balance->config.output_limit_percent = BALANCE_CONTROL_OUTPUT_LIMIT_PERCENT;
    }
    if (balance->config.integral_limit_deg_s <= 0.0f) {
        balance->config.integral_limit_deg_s = BALANCE_CONTROL_INTEGRAL_LIMIT_DEG_S;
    }
    if (balance->config.derivative_filter_tau_s < 0.0f) {
        balance->config.derivative_filter_tau_s = BALANCE_CONTROL_DERIVATIVE_FILTER_TAU_S;
    }
    if (balance->config.anti_windup_gain < 0.0f) {
        balance->config.anti_windup_gain = BALANCE_CONTROL_ANTI_WINDUP_GAIN;
    }

    if (!balance->config.pid_ops || !balance->config.pid_ctx) return AM_ERROR;
    balance->config.pid_ops->defaults(&pid_config);
    pid_config.kp = balance->config.kp;
    pid_config.ki = balance->config.ki;
    pid_config.kd = balance->config.kd;
    pid_config.default_dt_s = balance->config.default_dt_s;
    pid_config.output_min = -Balance_Abs(balance->config.output_limit_percent);
    pid_config.output_max = Balance_Abs(balance->config.output_limit_percent);
    pid_config.integral_min = -Balance_Abs(balance->config.integral_limit_deg_s);
    pid_config.integral_max = Balance_Abs(balance->config.integral_limit_deg_s);
    pid_config.derivative_filter_tau_s = balance->config.derivative_filter_tau_s;
    pid_config.anti_windup_gain = balance->config.anti_windup_gain;
    if (balance->config.pid_ops->init(balance->config.pid_ctx, &pid_config) != AM_OK) {
        return AM_ERROR;
    }

    balance->status.enabled = 1u;
    return AM_OK;
}

void BalanceControl_Reset(BalanceControl_Handle *balance)
{
    float target_pitch_deg;
    uint8_t enabled;

    if (balance == NULL) {
        return;
    }

    target_pitch_deg = balance->status.target_pitch_deg;
    enabled = balance->status.enabled;
    memset(&balance->status, 0, sizeof(balance->status));
    balance->status.target_pitch_deg = target_pitch_deg;
    balance->status.enabled = enabled;
    balance->config.pid_ops->reset(balance->config.pid_ctx);
    balance->config.pid_ops->setpoint(balance->config.pid_ctx, target_pitch_deg);
}

void BalanceControl_SetEnabled(BalanceControl_Handle *balance, uint8_t enabled)
{
    if (balance == NULL) {
        return;
    }

    balance->status.enabled = enabled ? 1u : 0u;
    if (!balance->status.enabled) {
        balance->status.output_percent = 0.0f;
    }
}

void BalanceControl_SetTargetPitchDeg(BalanceControl_Handle *balance,
                                      float target_pitch_deg)
{
    if (balance == NULL) {
        return;
    }

    balance->status.target_pitch_deg = target_pitch_deg;
    balance->config.pid_ops->setpoint(balance->config.pid_ctx, target_pitch_deg);
}

void BalanceControl_SetPid(BalanceControl_Handle *balance,
                           float kp,
                           float ki,
                           float kd)
{
    if (balance == NULL) {
        return;
    }

    balance->config.kp = kp;
    balance->config.ki = ki;
    balance->config.kd = kd;
    balance->config.pid_ops->gains(balance->config.pid_ctx, kp, ki, kd);
}

AM_Result BalanceControl_Update(BalanceControl_Handle *balance,
                                        float measured_pitch_deg,
                                        uint32_t now_ms)
{
    float dt_s;
    float output;
    const PidController_Status_t *pid_status;
    const float output_limit = Balance_Abs(balance != NULL
        ? balance->config.output_limit_percent
        : BALANCE_CONTROL_OUTPUT_LIMIT_PERCENT);

    if (balance == NULL || !balance->status.enabled || !isfinite(measured_pitch_deg) ||
        !isfinite(balance->status.target_pitch_deg) || !isfinite(balance->config.kp) ||
        !isfinite(balance->config.ki) || !isfinite(balance->config.kd)) {
        return AM_ERROR;
    }

    if (balance->status.has_previous) {
        dt_s = (float)(now_ms - balance->status.last_update_ms) * 0.001f;
        if (dt_s < 0.001f) {
            dt_s = balance->config.default_dt_s;
        } else if (dt_s > 0.050f) {
            dt_s = 0.050f;
        }
    } else {
        dt_s = balance->config.default_dt_s;
    }

    if (balance->config.pid_ops->update(balance->config.pid_ctx,
                             balance->status.target_pitch_deg,
                             measured_pitch_deg,
                             dt_s) != AM_OK) {
        return AM_ERROR;
    }

    pid_status = balance->config.pid_ops->status(balance->config.pid_ctx);
    output = balance->config.pid_ops->output(balance->config.pid_ctx);

    if (balance->config.output_inverted) {
        output = -output;
    }

    balance->status.measured_pitch_deg = measured_pitch_deg;
    balance->status.error_deg = pid_status ? pid_status->error : 0.0f;
    balance->status.integral_deg_s = pid_status ? pid_status->integral : 0.0f;
    balance->status.derivative_deg_s = pid_status ? pid_status->derivative : 0.0f;
    balance->status.output_percent = Balance_Clamp(output, -output_limit, output_limit);
    balance->status.last_update_ms = now_ms;
    balance->status.has_previous = 1u;

    return AM_OK;
}

float BalanceControl_GetOutputPercent(const BalanceControl_Handle *balance)
{
    return (balance != NULL) ? balance->status.output_percent : 0.0f;
}

const BalanceControl_Status_t *BalanceControl_GetStatus(const BalanceControl_Handle *balance)
{
    return (balance != NULL) ? &balance->status : NULL;
}
