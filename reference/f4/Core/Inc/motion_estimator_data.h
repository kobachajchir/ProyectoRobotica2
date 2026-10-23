#ifndef MOTION_ESTIMATOR_DATA_H
#define MOTION_ESTIMATOR_DATA_H
#include "am_port.h"
#define MOTION_ESTIMATOR_G_TO_MPS2                 9.80665f
#define MOTION_ESTIMATOR_DEFAULT_ACCEL_DEADBAND_MG 18.0f
#define MOTION_ESTIMATOR_DEFAULT_ACCEL_ALPHA        0.35f
#define MOTION_ESTIMATOR_DEFAULT_VELOCITY_LEAK_S    0.45f
#define MOTION_ESTIMATOR_DEFAULT_IDLE_LEAK_S        2.00f
#define MOTION_ESTIMATOR_DEFAULT_VELOCITY_LIMIT_MPS 2.0f
#define MOTION_ESTIMATOR_DEFAULT_STALE_TIMEOUT_MS  60u
#define MOTION_ESTIMATOR_DEFAULT_HIGH_PITCH_DEG    30.0f
#define MOTION_ESTIMATOR_DEFAULT_VIBRATION_MG     650.0f
#define MOTION_ESTIMATOR_DEFAULT_REFERENCE_TIMEOUT_MS 5000u

typedef enum {
    MOTION_ESTIMATOR_FLAG_STALE          = 1u << 0,
    MOTION_ESTIMATOR_FLAG_HIGH_PITCH     = 1u << 1,
    MOTION_ESTIMATOR_FLAG_VIBRATION      = 1u << 2,
    MOTION_ESTIMATOR_FLAG_SATURATED      = 1u << 3,
    MOTION_ESTIMATOR_FLAG_NO_REFERENCE   = 1u << 4,
    MOTION_ESTIMATOR_FLAG_MPU_NOT_READY  = 1u << 5,
} MotionEstimator_Flags_t;

struct AM_OrientationOps;
typedef struct {
    void *mpu;
    const struct AM_OrientationOps *orientation;
    float accel_deadband_mg;
    float accel_filter_alpha;
    float velocity_leak_per_s;
    float idle_velocity_leak_per_s;
    float velocity_limit_mps;
    float high_pitch_deg;
    float vibration_mg;
    uint32_t stale_timeout_ms;
    uint32_t reference_timeout_ms;
} MotionEstimator_Config_t;

typedef struct {
    uint16_t sample_seq;
    uint16_t sample_dt_us;
    uint8_t flags;
    uint8_t confidence_percent;
    int16_t linear_accel_y_mg;
    float linear_accel_mps2;
    float filtered_accel_mps2;
    float velocity_mps;
    float yaw_rate_dps;
    float pitch_deg;
    float vibration_mg;
    uint32_t last_update_ms;
    uint32_t last_sample_ms;
    uint32_t last_reference_ms;
    uint8_t has_sample;
} MotionEstimator_Status_t;


#endif
