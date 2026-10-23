#ifndef BALANCE_CONTROL_DATA_H
#define BALANCE_CONTROL_DATA_H
#include "am_port.h"
#define BALANCE_CONTROL_DEFAULT_KP                 8.0f
#define BALANCE_CONTROL_DEFAULT_KI                 0.0f
#define BALANCE_CONTROL_DEFAULT_KD                 0.35f
#define BALANCE_CONTROL_DEFAULT_DT_S               0.005f
#define BALANCE_CONTROL_OUTPUT_LIMIT_PERCENT       100.0f
#define BALANCE_CONTROL_INTEGRAL_LIMIT_DEG_S       30.0f
#define BALANCE_CONTROL_DERIVATIVE_FILTER_TAU_S     0.020f
#define BALANCE_CONTROL_ANTI_WINDUP_GAIN            0.25f

struct AM_PidOps;
typedef struct {
    const struct AM_PidOps *pid_ops;
    void *pid_ctx;
    float kp;
    float ki;
    float kd;
    float default_dt_s;
    float output_limit_percent;
    float integral_limit_deg_s;
    float derivative_filter_tau_s;
    float anti_windup_gain;
    uint8_t output_inverted;
} BalanceControl_Config_t;

typedef struct {
    float target_pitch_deg;
    float measured_pitch_deg;
    float error_deg;
    float integral_deg_s;
    float derivative_deg_s;
    float output_percent;
    uint32_t last_update_ms;
    uint8_t has_previous;
    uint8_t enabled;
} BalanceControl_Status_t;


#endif
