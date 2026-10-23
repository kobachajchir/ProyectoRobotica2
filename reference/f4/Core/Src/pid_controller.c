/*
 * pid_controller.c
 *
 * PID with derivative-on-measurement, first-order derivative filtering and
 * bounded integral/output. The update path has no allocation and no HAL timing
 * dependency; callers pass the real loop dt.
 */

#include "pid_controller.h"

#include <stddef.h>
#include <string.h>

static float Pid_Clamp(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static float Pid_Abs(float value)
{
    return (value < 0.0f) ? -value : value;
}

static void Pid_NormalizeConfig(PidController_Config_t *config)
{
    float tmp;

    if (config == NULL) {
        return;
    }

    if (config->default_dt_s <= 0.0f) {
        config->default_dt_s = PID_CONTROLLER_DEFAULT_DT_S;
    }
    if (config->output_min > config->output_max) {
        tmp = config->output_min;
        config->output_min = config->output_max;
        config->output_max = tmp;
    }
    if (config->output_min == config->output_max) {
        config->output_min = PID_CONTROLLER_DEFAULT_OUTPUT_MIN;
        config->output_max = PID_CONTROLLER_DEFAULT_OUTPUT_MAX;
    }
    if (config->integral_min > config->integral_max) {
        tmp = config->integral_min;
        config->integral_min = config->integral_max;
        config->integral_max = tmp;
    }
    if (config->integral_min == config->integral_max) {
        config->integral_min = PID_CONTROLLER_DEFAULT_INTEGRAL_MIN;
        config->integral_max = PID_CONTROLLER_DEFAULT_INTEGRAL_MAX;
    }
    if (config->derivative_filter_tau_s < 0.0f) {
        config->derivative_filter_tau_s = 0.0f;
    }
    if (config->anti_windup_gain < 0.0f) {
        config->anti_windup_gain = 0.0f;
    }
}

void PidController_GetDefaultConfig(PidController_Config_t *config)
{
    if (config == NULL) {
        return;
    }

    config->kp = 0.0f;
    config->ki = 0.0f;
    config->kd = 0.0f;
    config->default_dt_s = PID_CONTROLLER_DEFAULT_DT_S;
    config->output_min = PID_CONTROLLER_DEFAULT_OUTPUT_MIN;
    config->output_max = PID_CONTROLLER_DEFAULT_OUTPUT_MAX;
    config->integral_min = PID_CONTROLLER_DEFAULT_INTEGRAL_MIN;
    config->integral_max = PID_CONTROLLER_DEFAULT_INTEGRAL_MAX;
    config->derivative_filter_tau_s = PID_CONTROLLER_DEFAULT_DERIVATIVE_TAU_S;
    config->anti_windup_gain = PID_CONTROLLER_DEFAULT_ANTI_WINDUP_GAIN;
}

AM_Result PidController_Init(PidController_Handle *pid,
                                     const PidController_Config_t *config)
{
    PidController_Config_t default_config;

    if (pid == NULL) {
        return AM_ERROR;
    }

    PidController_GetDefaultConfig(&default_config);
    memset(pid, 0, sizeof(*pid));
    pid->config = (config != NULL) ? *config : default_config;
    Pid_NormalizeConfig(&pid->config);

    return AM_OK;
}

void PidController_Reset(PidController_Handle *pid)
{
    float setpoint;

    if (pid == NULL) {
        return;
    }

    setpoint = pid->status.setpoint;
    memset(&pid->status, 0, sizeof(pid->status));
    pid->status.setpoint = setpoint;
}

void PidController_SetGains(PidController_Handle *pid,
                            float kp,
                            float ki,
                            float kd)
{
    if (pid == NULL) {
        return;
    }

    pid->config.kp = kp;
    pid->config.ki = ki;
    pid->config.kd = kd;
}

void PidController_SetOutputLimits(PidController_Handle *pid,
                                   float output_min,
                                   float output_max)
{
    if (pid == NULL) {
        return;
    }

    pid->config.output_min = output_min;
    pid->config.output_max = output_max;
    Pid_NormalizeConfig(&pid->config);
}

void PidController_SetIntegralLimits(PidController_Handle *pid,
                                     float integral_min,
                                     float integral_max)
{
    if (pid == NULL) {
        return;
    }

    pid->config.integral_min = integral_min;
    pid->config.integral_max = integral_max;
    Pid_NormalizeConfig(&pid->config);
}

void PidController_SetSetpoint(PidController_Handle *pid, float setpoint)
{
    if (pid == NULL) {
        return;
    }

    pid->status.setpoint = setpoint;
}

AM_Result PidController_Update(PidController_Handle *pid,
                                       float setpoint,
                                       float measurement,
                                       float dt_s)
{
    float error;
    float derivative_raw = 0.0f;
    float derivative = 0.0f;
    float alpha = 1.0f;
    float integral;
    float output_raw;
    float output;
    uint8_t saturated;

    if (pid == NULL) {
        return AM_ERROR;
    }

    if (dt_s <= 0.0f) {
        dt_s = pid->config.default_dt_s;
    }
    if (dt_s <= 0.0f) {
        return AM_ERROR;
    }

    error = setpoint - measurement;
    integral = pid->status.integral + (error * dt_s);
    integral = Pid_Clamp(integral, pid->config.integral_min, pid->config.integral_max);

    if (pid->status.initialized) {
        derivative_raw = -(measurement - pid->status.last_measurement) / dt_s;
        if (pid->config.derivative_filter_tau_s > 0.0f) {
            alpha = dt_s / (pid->config.derivative_filter_tau_s + dt_s);
            alpha = Pid_Clamp(alpha, 0.0f, 1.0f);
        }
        derivative = pid->status.derivative +
                     (alpha * (derivative_raw - pid->status.derivative));
    }

    output_raw = (pid->config.kp * error) +
                 (pid->config.ki * integral) +
                 (pid->config.kd * derivative);
    output = Pid_Clamp(output_raw, pid->config.output_min, pid->config.output_max);
    saturated = (Pid_Abs(output - output_raw) > 0.0001f) ? 1u : 0u;

    if (saturated && pid->config.ki != 0.0f && pid->config.anti_windup_gain > 0.0f) {
        integral += (pid->config.anti_windup_gain * (output - output_raw)) /
                    pid->config.ki;
        integral = Pid_Clamp(integral,
                             pid->config.integral_min,
                             pid->config.integral_max);
        output_raw = (pid->config.kp * error) +
                     (pid->config.ki * integral) +
                     (pid->config.kd * derivative);
        output = Pid_Clamp(output_raw, pid->config.output_min, pid->config.output_max);
        saturated = (Pid_Abs(output - output_raw) > 0.0001f) ? 1u : 0u;
    }

    pid->status.setpoint = setpoint;
    pid->status.measurement = measurement;
    pid->status.error = error;
    pid->status.proportional = pid->config.kp * error;
    pid->status.integral = integral;
    pid->status.derivative = derivative;
    pid->status.output_raw = output_raw;
    pid->status.output = output;
    pid->status.last_measurement = measurement;
    pid->status.last_error = error;
    pid->status.dt_s = dt_s;
    pid->status.initialized = 1u;
    pid->status.saturated = saturated;

    return AM_OK;
}

float PidController_GetOutput(const PidController_Handle *pid)
{
    return (pid != NULL) ? pid->status.output : 0.0f;
}

const PidController_Status_t *PidController_GetStatus(const PidController_Handle *pid)
{
    return (pid != NULL) ? &pid->status : NULL;
}
