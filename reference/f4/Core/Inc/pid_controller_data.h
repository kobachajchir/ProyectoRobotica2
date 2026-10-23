#ifndef PID_CONTROLLER_DATA_H
#define PID_CONTROLLER_DATA_H
#include "am_port.h"
#define PID_CONTROLLER_DEFAULT_DT_S              0.005f
#define PID_CONTROLLER_DEFAULT_OUTPUT_MIN      (-100.0f)
#define PID_CONTROLLER_DEFAULT_OUTPUT_MAX       100.0f
#define PID_CONTROLLER_DEFAULT_INTEGRAL_MIN    (-30.0f)
#define PID_CONTROLLER_DEFAULT_INTEGRAL_MAX      30.0f
#define PID_CONTROLLER_DEFAULT_DERIVATIVE_TAU_S   0.015f
#define PID_CONTROLLER_DEFAULT_ANTI_WINDUP_GAIN   0.25f

typedef struct {
    float kp;
    float ki;
    float kd;
    float default_dt_s;
    float output_min;
    float output_max;
    float integral_min;
    float integral_max;
    float derivative_filter_tau_s;
    float anti_windup_gain;
} PidController_Config_t;

typedef struct {
    float setpoint;
    float measurement;
    float error;
    float proportional;
    float integral;
    float derivative;
    float output_raw;
    float output;
    float last_measurement;
    float last_error;
    float dt_s;
    uint8_t initialized;
    uint8_t saturated;
} PidController_Status_t;


#endif
