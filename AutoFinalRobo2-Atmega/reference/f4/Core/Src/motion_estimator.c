/*
 * motion_estimator.c
 *
 * Relative velocity estimator based on MPU9250 linear acceleration on the
 * robot longitudinal axis. Leak and confidence gates keep the value bounded
 * because the MPU has no absolute velocity reference.
 */

#include "motion_estimator.h"

#include "utils/macros_utils.h"
#include <stddef.h>
#include <string.h>

static float MotionEstimator_Clamp(float value, float min_value, float max_value)
{
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static float MotionEstimator_Abs(float value)
{
    return (value < 0.0f) ? -value : value;
}


static float MotionEstimator_ApplyLeak(float value, float leak_per_s, float dt_s)
{
    float factor;

    if (leak_per_s <= 0.0f || dt_s <= 0.0f) {
        return value;
    }

    factor = 1.0f - (leak_per_s * dt_s);
    factor = MotionEstimator_Clamp(factor, 0.0f, 1.0f);
    return value * factor;
}

static uint8_t MotionEstimator_Confidence(uint8_t flags,
                                          uint32_t time_without_reference_ms,
                                          uint32_t reference_timeout_ms)
{
    int16_t confidence = 100;

    if ((flags & MOTION_ESTIMATOR_FLAG_MPU_NOT_READY) != 0u) {
        confidence -= 70;
    }
    if ((flags & MOTION_ESTIMATOR_FLAG_STALE) != 0u) {
        confidence -= 45;
    }
    if ((flags & MOTION_ESTIMATOR_FLAG_HIGH_PITCH) != 0u) {
        confidence -= 25;
    }
    if ((flags & MOTION_ESTIMATOR_FLAG_VIBRATION) != 0u) {
        confidence -= 20;
    }
    if ((flags & MOTION_ESTIMATOR_FLAG_SATURATED) != 0u) {
        confidence -= 25;
    }
    if ((flags & MOTION_ESTIMATOR_FLAG_NO_REFERENCE) != 0u &&
        reference_timeout_ms > 0u) {
        const uint32_t over_ms =
            (time_without_reference_ms > reference_timeout_ms)
                ? (time_without_reference_ms - reference_timeout_ms)
                : 0u;
        int16_t penalty = 10;
        penalty = (int16_t)(penalty + (int16_t)(over_ms / 250u));
        if (penalty > 45) {
            penalty = 45;
        }
        confidence = (int16_t)(confidence - penalty);
    }

    if (confidence < 0) {
        confidence = 0;
    } else if (confidence > 100) {
        confidence = 100;
    }

    return (uint8_t)confidence;
}

void MotionEstimator_GetDefaultConfig(MotionEstimator_Config_t *config)
{
    if (config == NULL) {
        return;
    }

    memset(config, 0, sizeof(*config));
    config->accel_deadband_mg = MOTION_ESTIMATOR_DEFAULT_ACCEL_DEADBAND_MG;
    config->accel_filter_alpha = MOTION_ESTIMATOR_DEFAULT_ACCEL_ALPHA;
    config->velocity_leak_per_s = MOTION_ESTIMATOR_DEFAULT_VELOCITY_LEAK_S;
    config->idle_velocity_leak_per_s = MOTION_ESTIMATOR_DEFAULT_IDLE_LEAK_S;
    config->velocity_limit_mps = MOTION_ESTIMATOR_DEFAULT_VELOCITY_LIMIT_MPS;
    config->high_pitch_deg = MOTION_ESTIMATOR_DEFAULT_HIGH_PITCH_DEG;
    config->vibration_mg = MOTION_ESTIMATOR_DEFAULT_VIBRATION_MG;
    config->stale_timeout_ms = MOTION_ESTIMATOR_DEFAULT_STALE_TIMEOUT_MS;
    config->reference_timeout_ms = MOTION_ESTIMATOR_DEFAULT_REFERENCE_TIMEOUT_MS;
}

AM_Result MotionEstimator_Init(MotionEstimator_Handle *estimator,
                                       const MotionEstimator_Config_t *config)
{
    MotionEstimator_Config_t default_config;

    if (estimator == NULL) {
        return AM_ERROR;
    }

    MotionEstimator_GetDefaultConfig(&default_config);
    memset(estimator, 0, sizeof(*estimator));
    estimator->config = (config != NULL) ? *config : default_config;
    if (estimator->config.mpu && !estimator->config.orientation) return AM_ERROR;
    if (estimator->config.accel_filter_alpha < 0.0f) {
        estimator->config.accel_filter_alpha = 0.0f;
    } else if (estimator->config.accel_filter_alpha > 1.0f) {
        estimator->config.accel_filter_alpha = 1.0f;
    }
    if (estimator->config.velocity_limit_mps <= 0.0f) {
        estimator->config.velocity_limit_mps =
            MOTION_ESTIMATOR_DEFAULT_VELOCITY_LIMIT_MPS;
    }
    estimator->status.confidence_percent = 0u;
    estimator->initialized = 1u;

    return AM_OK;
}

void MotionEstimator_Reset(MotionEstimator_Handle *estimator)
{
    void *mpu;
    MotionEstimator_Config_t config;

    if (estimator == NULL) {
        return;
    }

    mpu = estimator->config.mpu;
    config = estimator->config;
    memset(&estimator->status, 0, sizeof(estimator->status));
    estimator->config = config;
    estimator->config.mpu = mpu;
    estimator->last_seq = 0u;
    estimator->status.confidence_percent = 0u;
}

void MotionEstimator_MarkExternalReference(MotionEstimator_Handle *estimator,
                                           float velocity_mps,
                                           uint32_t now_ms)
{
    if (estimator == NULL) {
        return;
    }

    estimator->status.velocity_mps =
        MotionEstimator_Clamp(velocity_mps,
                              -estimator->config.velocity_limit_mps,
                              estimator->config.velocity_limit_mps);
    estimator->status.last_reference_ms = now_ms;
}

void MotionEstimator_Update(MotionEstimator_Handle *estimator,
                            uint32_t now_ms,
                            uint8_t motors_active,
                            uint8_t control_saturated)
{
    MPU9250_Snapshot_t snapshot;
    AM_Result snap_status;
    float dt_s = 0.0f;
    float accel_mg;
    float accel_mps2;
    float alpha;
    float leak;
    uint8_t flags = 0u;
    uint32_t time_without_reference_ms;

    if (estimator == NULL || !estimator->initialized) {
        return;
    }

    memset(&snapshot, 0, sizeof(snapshot));
    if (!estimator->config.orientation || !estimator->config.orientation->ready(estimator->config.mpu)) {
        flags |= MOTION_ESTIMATOR_FLAG_MPU_NOT_READY;
    }

    snap_status = estimator->config.orientation
        ? estimator->config.orientation->snapshot(estimator->config.mpu, &snapshot) : AM_ERROR;
    if (snap_status != AM_OK) {
        flags |= MOTION_ESTIMATOR_FLAG_STALE;
    }

    if (snap_status == AM_OK && snapshot.sample_seq != estimator->last_seq) {
        estimator->last_seq = snapshot.sample_seq;
        estimator->status.last_sample_ms = now_ms;
        estimator->status.has_sample = 1u;
        dt_s = (snapshot.sample_dt_us > 0u)
            ? ((float)snapshot.sample_dt_us * 0.000001f)
            : 0.0f;
    } else {
        flags |= MOTION_ESTIMATOR_FLAG_STALE;
        if (estimator->status.last_update_ms != 0u) {
            dt_s = (float)(now_ms - estimator->status.last_update_ms) * 0.001f;
        }
    }

    if (dt_s <= 0.0f || dt_s > 0.100f) {
        dt_s = 0.005f;
    }

    if (estimator->status.last_sample_ms != 0u &&
        (uint32_t)(now_ms - estimator->status.last_sample_ms) >
            estimator->config.stale_timeout_ms) {
        flags |= MOTION_ESTIMATOR_FLAG_STALE;
    }

    estimator->status.pitch_deg = (float)snapshot.pitch_md * 0.001f;
    if (MotionEstimator_Abs(estimator->status.pitch_deg) >
        estimator->config.high_pitch_deg) {
        flags |= MOTION_ESTIMATOR_FLAG_HIGH_PITCH;
    }

    estimator->status.vibration_mg = MotionEstimator_Abs((float)snapshot.linear_accel_x_mg) +
                                     MotionEstimator_Abs((float)snapshot.linear_accel_z_mg);
    if (estimator->status.vibration_mg > estimator->config.vibration_mg) {
        flags |= MOTION_ESTIMATOR_FLAG_VIBRATION;
    }
    if (control_saturated) {
        flags |= MOTION_ESTIMATOR_FLAG_SATURATED;
    }

    accel_mg = (float)snapshot.linear_accel_y_mg;
    if (MotionEstimator_Abs(accel_mg) < estimator->config.accel_deadband_mg) {
        accel_mg = 0.0f;
    }
    accel_mps2 = accel_mg * (MOTION_ESTIMATOR_G_TO_MPS2 * 0.001f);
    alpha = estimator->config.accel_filter_alpha;
    estimator->status.filtered_accel_mps2 +=
        alpha * (accel_mps2 - estimator->status.filtered_accel_mps2);

    leak = motors_active
        ? estimator->config.velocity_leak_per_s
        : estimator->config.idle_velocity_leak_per_s;
    estimator->status.velocity_mps =
        MotionEstimator_ApplyLeak(estimator->status.velocity_mps, leak, dt_s);
    estimator->status.velocity_mps += estimator->status.filtered_accel_mps2 * dt_s;
    estimator->status.velocity_mps =
        MotionEstimator_Clamp(estimator->status.velocity_mps,
                              -estimator->config.velocity_limit_mps,
                              estimator->config.velocity_limit_mps);

    if (!motors_active && MotionEstimator_Abs(estimator->status.velocity_mps) < 0.01f) {
        estimator->status.velocity_mps = 0.0f;
        estimator->status.last_reference_ms = now_ms;
    }

    if (estimator->status.last_reference_ms == 0u) {
        estimator->status.last_reference_ms = now_ms;
    }

    time_without_reference_ms = now_ms - estimator->status.last_reference_ms;
    if (time_without_reference_ms > estimator->config.reference_timeout_ms) {
        flags |= MOTION_ESTIMATOR_FLAG_NO_REFERENCE;
    }

    estimator->status.sample_seq = snapshot.sample_seq;
    estimator->status.sample_dt_us = snapshot.sample_dt_us;
    estimator->status.linear_accel_y_mg = snapshot.linear_accel_y_mg;
    estimator->status.linear_accel_mps2 = accel_mps2;
    estimator->status.yaw_rate_dps = (float)snapshot.gyro_z_mdps * 0.001f;
    estimator->status.flags = flags;
    estimator->status.confidence_percent =
        MotionEstimator_Confidence(flags,
                                   time_without_reference_ms,
                                   estimator->config.reference_timeout_ms);
    estimator->status.last_update_ms = now_ms;
}

const MotionEstimator_Status_t *MotionEstimator_GetStatus(const MotionEstimator_Handle *estimator)
{
    return (estimator != NULL) ? &estimator->status : NULL;
}
