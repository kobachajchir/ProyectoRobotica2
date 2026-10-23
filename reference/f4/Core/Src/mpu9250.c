/*
 * mpu9250.c
 *
 * STM32F411 HAL + I2C manager MPU9250/AK8963 9-axis driver.
 */

#include "mpu9250.h"

#include <math.h>
#include <string.h>

#define MPU9250_REG_SMPLRT_DIV       0x19u
#define MPU9250_REG_CONFIG           0x1Au
#define MPU9250_REG_GYRO_CFG         0x1Bu
#define MPU9250_REG_ACCEL_CFG        0x1Cu
#define MPU9250_REG_ACCEL_CFG_2      0x1Du
#define MPU9250_REG_I2C_MST_CTRL     0x24u
#define MPU9250_REG_I2C_SLV0_ADDR    0x25u
#define MPU9250_REG_I2C_SLV0_REG     0x26u
#define MPU9250_REG_I2C_SLV0_CTRL    0x27u
#define MPU9250_REG_INT_PIN_CFG      0x37u
#define MPU9250_REG_TEMP_OUT         0x41u
#define MPU9250_REG_GYRO_OUT         0x43u
#define MPU9250_REG_USER_CTRL        0x6Au
#define MPU9250_REG_PWR_MGMT_1       0x6Bu
#define MPU9250_REG_PWR_MGMT_2       0x6Cu
#define MPU9250_REG_WHO_AM_I         0x75u
#define MPU9250_BURST_REG            0x3Bu

#define MPU9250_WHO_AM_I_VALUE       0x71u
#define MPU9250_USER_I2C_MST_EN      0x20u
#define MPU9250_BYPASS_EN            0x02u
#define MPU9250_I2C_MST_400KHZ       0x0Du
#define MPU9250_I2C_SLV_READ         0x80u
#define MPU9250_I2C_SLV_ENABLE       0x80u

#define AK8963_I2C_ADDRESS           0x0Cu
#define AK8963_REG_WIA               0x00u
#define AK8963_REG_ST1               0x02u
#define AK8963_REG_CNTL1             0x0Au
#define AK8963_REG_ASAX              0x10u
#define AK8963_WHO_AM_I_VALUE        0x48u
#define AK8963_MODE_POWER_DOWN       0x00u
#define AK8963_MODE_FUSE_ROM         0x0Fu
#define AK8963_MODE_CONT_100HZ_16BIT 0x16u
#define AK8963_ST1_DRDY              0x01u
#define AK8963_ST2_HOFL              0x08u
#define AK8963_SENSITIVITY_UT_LSB    0.15f

#define MPU9250_BURST_LENGTH             22u
#define MPU9250_MAG_EXT_OFFSET           14u
#define MPU9250_MAG_EXT_LENGTH            8u

#define MPU_DEFAULT_DT_US              5000u
#define MPU_ATTITUDE_FIXED_DT_S     0.0050000f
#define MPU_SMPLRT_DIV_200HZ                 4u
#define MPU_CALIB_DEFAULT_WARMUP_MS         400u
#define MPU_CALIB_MIN_WINDOW_MS            1500u
#define MPU_CALIB_MAX_WINDOW_MS            3000u
#define MPU_CALIB_MIN_SAMPLES                32u
#define MPU_STATIONARY_GYRO_MAX_MDPS       5000
#define MPU_STATIONARY_ACCEL_TOL_MG         150
#define MPU_STATIONARY_ACCEL_DELTA_MG       120
#define MPU_GRAVITY_LPF_SHIFT                 4
#define MPU_DEG_PER_RAD               57.2957795f
#define MPU_RAD_PER_DEG                0.01745329252f
#define MPU_ATTITUDE_DEFAULT_KP        2.0000000f
#define MPU_ATTITUDE_DEFAULT_KI        0.0500000f
#define MPU_VECTOR_NORM_EPSILON        0.0000010f
#define MPU_MAG_FIELD_MIN_UT           1.0000000f
#define MPU_MAG_FIELD_MAX_UT        1000.0000000f

#define MERGE_RAW_BE(i_) ((int16_t)((uint16_t)hmpu->rx_buffer[(i_)] << 8 | hmpu->rx_buffer[(i_) + 1u]))
#define MERGE_RAW_LE(i_) ((int16_t)((uint16_t)hmpu->rx_buffer[(i_) + 1u] << 8 | hmpu->rx_buffer[(i_)]))

static void MPU_I2C_RequestApproved(void *ctx);
static void MPU_I2C_TxComplete(void *ctx);
static void MPU_I2C_RxComplete(void *ctx);
static void MPU_I2C_Error(void *ctx, I2C_ManagerError err);



static int32_t MPU_Abs32(int32_t value)
{
    return (value < 0) ? -value : value;
}

static int16_t MPU_ClampI16(int32_t value)
{
    if (value > 32767) {
        return 32767;
    }
    if (value < -32768) {
        return -32768;
    }
    return (int16_t)value;
}

static int32_t MPU_RoundFloatToI32(float value)
{
    return (int32_t)(value >= 0.0f ? (value + 0.5f) : (value - 0.5f));
}

static int32_t MPU_DegToMd(float deg)
{
    return MPU_RoundFloatToI32(deg * 1000.0f);
}

static float MPU_Normalize180(float angle_deg)
{
    while (angle_deg > 180.0f) {
        angle_deg -= 360.0f;
    }
    while (angle_deg < -180.0f) {
        angle_deg += 360.0f;
    }
    return angle_deg;
}

static float MPU_Normalize360(float angle_deg)
{
    while (angle_deg >= 360.0f) {
        angle_deg -= 360.0f;
    }
    while (angle_deg < 0.0f) {
        angle_deg += 360.0f;
    }
    return angle_deg;
}

static float MPU_ClampFloat(float value, float low, float high)
{
    if (value < low) {
        return low;
    }
    if (value > high) {
        return high;
    }
    return value;
}

static float MPU_AccelRollDegG(float ay_g, float az_g)
{
    return atan2f(ay_g, az_g) * MPU_DEG_PER_RAD;
}

static float MPU_AccelPitchDegG(float ax_g, float ay_g, float az_g)
{
    const float yz_norm = sqrtf((ay_g * ay_g) + (az_g * az_g));
    return atan2f(-ax_g, yz_norm) * MPU_DEG_PER_RAD;
}

static float MPU_AccelRollDeg(int16_t ay, int16_t az)
{
    return MPU_AccelRollDegG((float)ay, (float)az);
}

static float MPU_AccelPitchDeg(int16_t ax, int16_t ay, int16_t az)
{
    return MPU_AccelPitchDegG((float)ax, (float)ay, (float)az);
}

static float MPU_MagHeadingDeg(const MPU9250_NormalizedData_t *input,
                               float roll_deg,
                               float pitch_deg)
{
    const float roll_rad = roll_deg * MPU_RAD_PER_DEG;
    const float pitch_rad = pitch_deg * MPU_RAD_PER_DEG;
    const float sin_roll = sinf(roll_rad);
    const float cos_roll = cosf(roll_rad);
    const float sin_pitch = sinf(pitch_rad);
    const float cos_pitch = cosf(pitch_rad);
    const float horizontal_x = (input->mx_uT * cos_pitch) +
                               (input->mz_uT * sin_pitch);
    const float horizontal_y = (input->mx_uT * sin_roll * sin_pitch) +
                               (input->my_uT * cos_roll) -
                               (input->mz_uT * sin_roll * cos_pitch);

    return MPU_Normalize360(atan2f(-horizontal_y, horizontal_x) * MPU_DEG_PER_RAD);
}

static void MPU_QuaternionFromEuler(float roll_deg,
                                    float pitch_deg,
                                    float yaw_deg,
                                    MPU9250_AttitudeState_t *state)
{
    const float half_roll = 0.5f * roll_deg * MPU_RAD_PER_DEG;
    const float half_pitch = 0.5f * pitch_deg * MPU_RAD_PER_DEG;
    const float half_yaw = 0.5f * yaw_deg * MPU_RAD_PER_DEG;
    const float cr = cosf(half_roll);
    const float sr = sinf(half_roll);
    const float cp = cosf(half_pitch);
    const float sp = sinf(half_pitch);
    const float cy = cosf(half_yaw);
    const float sy = sinf(half_yaw);

    state->q0 = (cr * cp * cy) + (sr * sp * sy);
    state->q1 = (sr * cp * cy) - (cr * sp * sy);
    state->q2 = (cr * sp * cy) + (sr * cp * sy);
    state->q3 = (cr * cp * sy) - (sr * sp * cy);
}

static void MPU_MahonyUpdate(MPU9250_AttitudeState_t *state,
                             const MPU9250_NormalizedData_t *input,
                             float dt_s)
{
    float ax = input->ax_g;
    float ay = input->ay_g;
    float az = input->az_g;
    float mx = input->mx_uT;
    float my = input->my_uT;
    float mz = input->mz_uT;
    float gx = (input->gx_dps - state->offsets.gyro_x_offset_dps) * MPU_RAD_PER_DEG;
    float gy = (input->gy_dps - state->offsets.gyro_y_offset_dps) * MPU_RAD_PER_DEG;
    float gz = (input->gz_dps - state->offsets.gyro_z_offset_dps) * MPU_RAD_PER_DEG;
    float recip_norm;
    float halfex = 0.0f;
    float halfey = 0.0f;
    float halfez = 0.0f;
    uint8_t feedback_valid = 0u;
    const float accel_norm_sq = (ax * ax) + (ay * ay) + (az * az);
    const float mag_norm_sq = (mx * mx) + (my * my) + (mz * mz);

    if (accel_norm_sq > MPU_VECTOR_NORM_EPSILON) {
        float halfvx;
        float halfvy;
        float halfvz;

        recip_norm = 1.0f / sqrtf(accel_norm_sq);
        ax *= recip_norm;
        ay *= recip_norm;
        az *= recip_norm;

        halfvx = (state->q1 * state->q3) - (state->q0 * state->q2);
        halfvy = (state->q0 * state->q1) + (state->q2 * state->q3);
        halfvz = (state->q0 * state->q0) - 0.5f + (state->q3 * state->q3);
        halfex += (ay * halfvz) - (az * halfvy);
        halfey += (az * halfvx) - (ax * halfvz);
        halfez += (ax * halfvy) - (ay * halfvx);
        feedback_valid = 1u;

        if (input->mag_valid && mag_norm_sq > MPU_VECTOR_NORM_EPSILON) {
            float hx;
            float hy;
            float bx;
            float bz;
            float halfwx;
            float halfwy;
            float halfwz;
            const float q0q1 = state->q0 * state->q1;
            const float q0q2 = state->q0 * state->q2;
            const float q0q3 = state->q0 * state->q3;
            const float q1q1 = state->q1 * state->q1;
            const float q1q2 = state->q1 * state->q2;
            const float q1q3 = state->q1 * state->q3;
            const float q2q2 = state->q2 * state->q2;
            const float q2q3 = state->q2 * state->q3;
            const float q3q3 = state->q3 * state->q3;

            recip_norm = 1.0f / sqrtf(mag_norm_sq);
            mx *= recip_norm;
            my *= recip_norm;
            mz *= recip_norm;
            hx = 2.0f * ((mx * (0.5f - q2q2 - q3q3)) +
                         (my * (q1q2 - q0q3)) +
                         (mz * (q1q3 + q0q2)));
            hy = 2.0f * ((mx * (q1q2 + q0q3)) +
                         (my * (0.5f - q1q1 - q3q3)) +
                         (mz * (q2q3 - q0q1)));
            bx = sqrtf((hx * hx) + (hy * hy));
            bz = 2.0f * ((mx * (q1q3 - q0q2)) +
                         (my * (q2q3 + q0q1)) +
                         (mz * (0.5f - q1q1 - q2q2)));
            halfwx = (bx * (0.5f - q2q2 - q3q3)) +
                      (bz * (q1q3 - q0q2));
            halfwy = (bx * (q1q2 - q0q3)) +
                      (bz * (q0q1 + q2q3));
            halfwz = (bx * (q0q2 + q1q3)) +
                      (bz * (0.5f - q1q1 - q2q2));
            halfex += (my * halfwz) - (mz * halfwy);
            halfey += (mz * halfwx) - (mx * halfwz);
            halfez += (mx * halfwy) - (my * halfwx);
        }
    }

    if (feedback_valid) {
        if (state->ki > 0.0f) {
            state->integral_x += state->ki * halfex * dt_s;
            state->integral_y += state->ki * halfey * dt_s;
            state->integral_z += state->ki * halfez * dt_s;
            gx += state->integral_x;
            gy += state->integral_y;
            gz += state->integral_z;
        } else {
            state->integral_x = 0.0f;
            state->integral_y = 0.0f;
            state->integral_z = 0.0f;
        }
        gx += state->kp * halfex;
        gy += state->kp * halfey;
        gz += state->kp * halfez;
    }

    gx *= 0.5f * dt_s;
    gy *= 0.5f * dt_s;
    gz *= 0.5f * dt_s;
    {
        const float qa = state->q0;
        const float qb = state->q1;
        const float qc = state->q2;
        state->q0 += (-qb * gx) - (qc * gy) - (state->q3 * gz);
        state->q1 += (qa * gx) + (qc * gz) - (state->q3 * gy);
        state->q2 += (qa * gy) - (qb * gz) + (state->q3 * gx);
        state->q3 += (qa * gz) + (qb * gy) - (qc * gx);
    }

    recip_norm = 1.0f / sqrtf((state->q0 * state->q0) +
                              (state->q1 * state->q1) +
                              (state->q2 * state->q2) +
                              (state->q3 * state->q3));
    state->q0 *= recip_norm;
    state->q1 *= recip_norm;
    state->q2 *= recip_norm;
    state->q3 *= recip_norm;
}

static uint8_t MPU_IsStationary(MPU9250_Handle_t *hmpu)
{
    const int16_t ax = hmpu->data.accel_x_mg;
    const int16_t ay = hmpu->data.accel_y_mg;
    const int16_t az = hmpu->data.accel_z_mg;
    float accel_norm;
    uint8_t stable_delta = 1u;
    uint8_t stationary;

    accel_norm = sqrtf(((float)ax * (float)ax) +
                       ((float)ay * (float)ay) +
                       ((float)az * (float)az));

    if (hmpu->prev_accel_valid) {
        stable_delta =
            (MPU_Abs32((int32_t)ax - hmpu->prev_accel_x_mg) <= MPU_STATIONARY_ACCEL_DELTA_MG) &&
            (MPU_Abs32((int32_t)ay - hmpu->prev_accel_y_mg) <= MPU_STATIONARY_ACCEL_DELTA_MG) &&
            (MPU_Abs32((int32_t)az - hmpu->prev_accel_z_mg) <= MPU_STATIONARY_ACCEL_DELTA_MG);
    }

    stationary =
        (MPU_Abs32(hmpu->data.gyro_x_mdps) <= MPU_STATIONARY_GYRO_MAX_MDPS) &&
        (MPU_Abs32(hmpu->data.gyro_y_mdps) <= MPU_STATIONARY_GYRO_MAX_MDPS) &&
        (MPU_Abs32(hmpu->data.gyro_z_mdps) <= MPU_STATIONARY_GYRO_MAX_MDPS) &&
        (fabsf(accel_norm - 1000.0f) <= (float)MPU_STATIONARY_ACCEL_TOL_MG) &&
        stable_delta;

    hmpu->prev_accel_x_mg = ax;
    hmpu->prev_accel_y_mg = ay;
    hmpu->prev_accel_z_mg = az;
    hmpu->prev_accel_valid = 1u;

    return stationary;
}

static uint16_t MPU_ClampDtUs(uint32_t dt_us)
{
    if (dt_us == 0u) {
        return MPU_DEFAULT_DT_US;
    }
    if (dt_us > 65535u) {
        return 65535u;
    }
    return (uint16_t)dt_us;
}

void MPU9250_AttitudeInit(MPU9250_AttitudeState_t *state,
                          float kp,
                          float ki,
                          float fixed_dt_s)
{
    MPU9250_AttitudeOffsets_t saved_offsets;
    float saved_yaw_zero;

    if (state == NULL) {
        return;
    }

    saved_offsets = state->offsets;
    saved_yaw_zero = state->yaw_zero_deg;
    memset(state, 0, sizeof(*state));
    state->offsets = saved_offsets;
    state->yaw_zero_deg = saved_yaw_zero;
    state->q0 = 1.0f;
    state->kp = (kp >= 0.0f) ? kp : MPU_ATTITUDE_DEFAULT_KP;
    state->ki = (ki >= 0.0f) ? ki : MPU_ATTITUDE_DEFAULT_KI;
    state->fixed_dt_s = (fixed_dt_s > 0.0f) ? fixed_dt_s : MPU_ATTITUDE_FIXED_DT_S;
}

void MPU9250_AttitudeSetOffsets(MPU9250_AttitudeState_t *state,
                                const MPU9250_AttitudeOffsets_t *offsets)
{
    if (state == NULL || offsets == NULL) {
        return;
    }

    state->offsets = *offsets;
}

void MPU9250_AttitudeResetYaw(MPU9250_AttitudeState_t *state)
{
    if (state == NULL) {
        return;
    }

    state->yaw_zero_deg = state->output.heading_deg;
    state->output.yaw_deg = 0.0f;
}

void MPU9250_AttitudeUpdate(MPU9250_AttitudeState_t *state,
                            const MPU9250_NormalizedData_t *input,
                            MPU9250_Attitude_t *out)
{
    float accel_roll_deg;
    float accel_pitch_deg;
    float magnetic_heading_deg;
    float roll_rad;
    float pitch_sin;
    float yaw_rad;
    float dt_s;

    if (state == NULL || input == NULL) {
        return;
    }

    accel_roll_deg = MPU_AccelRollDegG(input->ay_g, input->az_g);
    accel_pitch_deg = MPU_AccelPitchDegG(input->ax_g, input->ay_g, input->az_g);
    magnetic_heading_deg = input->mag_valid
        ? MPU_MagHeadingDeg(input, accel_roll_deg, accel_pitch_deg)
        : state->output.heading_deg;
    dt_s = (input->sample_dt_s > 0.0f) ? input->sample_dt_s : state->fixed_dt_s;
    dt_s = MPU_ClampFloat(dt_s, 0.0005f, 0.0500f);

    if (!state->initialized) {
        MPU_QuaternionFromEuler(accel_roll_deg,
                                accel_pitch_deg,
                                magnetic_heading_deg,
                                state);
        state->initialized = 1u;
    }

    MPU_MahonyUpdate(state, input, dt_s);

    roll_rad = atan2f(2.0f * ((state->q0 * state->q1) + (state->q2 * state->q3)),
                      1.0f - (2.0f * ((state->q1 * state->q1) + (state->q2 * state->q2))));
    pitch_sin = 2.0f * ((state->q0 * state->q2) - (state->q3 * state->q1));
    pitch_sin = MPU_ClampFloat(pitch_sin, -1.0f, 1.0f);
    yaw_rad = atan2f(2.0f * ((state->q0 * state->q3) + (state->q1 * state->q2)),
                     1.0f - (2.0f * ((state->q2 * state->q2) + (state->q3 * state->q3))));

    state->output.roll_deg = MPU_Normalize180(
        (roll_rad * MPU_DEG_PER_RAD) - state->offsets.accel_roll_offset_deg);
    state->output.pitch_deg = MPU_Normalize180(
        (asinf(pitch_sin) * MPU_DEG_PER_RAD) - state->offsets.accel_pitch_offset_deg);
    state->output.heading_deg = MPU_Normalize360(yaw_rad * MPU_DEG_PER_RAD);
    state->output.yaw_deg = MPU_Normalize180(
        state->output.heading_deg - state->yaw_zero_deg);
    state->output.accel_roll_deg =
        accel_roll_deg - state->offsets.accel_roll_offset_deg;
    state->output.accel_pitch_deg =
        accel_pitch_deg - state->offsets.accel_pitch_offset_deg;
    state->output.mag_heading_deg = magnetic_heading_deg;
    state->output.gyro_roll_rate_dps =
        input->gx_dps - state->offsets.gyro_x_offset_dps;
    state->output.gyro_pitch_rate_dps =
        input->gy_dps - state->offsets.gyro_y_offset_dps;
    state->output.gyro_yaw_rate_dps =
        input->gz_dps - state->offsets.gyro_z_offset_dps;
    state->output.mag_valid = input->mag_valid;

    if (out != NULL) {
        *out = state->output;
    }
}

static void MPU_UpdateSampleTiming(MPU9250_Handle_t *hmpu)
{
    uint32_t now = hmpu->bus->hardware->now_us(hmpu->bus->hardware_ctx);
    uint32_t dt = (hmpu->last_sample_time_us == 0u)
        ? MPU_DEFAULT_DT_US
        : (now - hmpu->last_sample_time_us);

    hmpu->sample_acquired_ms = hmpu->bus->hardware->now_ms(hmpu->bus->hardware_ctx);
    hmpu->last_sample_time_us = now;
    hmpu->latest.sample_dt_us = MPU_ClampDtUs(dt);
}

static void MPU_SetError(MPU9250_Handle_t *hmpu)
{
    SET_FLAG(hmpu->flags, MPU_ERROR);
    CLEAR_FLAG(hmpu->flags, MPU_VALID | MPU_NEW_READING);
    hmpu->latest.flags = hmpu->flags.byte;
}

static void MPU_ResetCalibrationAccum(MPU9250_Handle_t *hmpu)
{
    hmpu->calib_sum_x = 0;
    hmpu->calib_sum_y = 0;
    hmpu->calib_sum_z = 0;
    hmpu->calib_accel_sum_x = 0;
    hmpu->calib_accel_sum_y = 0;
    hmpu->calib_accel_sum_z = 0;
    hmpu->calib_count = 0u;
}

static void MPU_FinishCalibration(MPU9250_Handle_t *hmpu)
{
    int16_t gx;
    int16_t gy;
    int16_t gz;
    MPU9250_AttitudeOffsets_t offsets;

    if (hmpu->calib_count == 0u) {
        MPU_SetError(hmpu);
        return;
    }

    hmpu->gyro_bias_x = hmpu->calib_sum_x / hmpu->calib_count;
    hmpu->gyro_bias_y = hmpu->calib_sum_y / hmpu->calib_count;
    hmpu->gyro_bias_z = hmpu->calib_sum_z / hmpu->calib_count;
    hmpu->gyro_bias_x_dps = (float)hmpu->gyro_bias_x * 0.001f;
    hmpu->gyro_bias_y_dps = (float)hmpu->gyro_bias_y * 0.001f;
    hmpu->gyro_bias_z_dps = (float)hmpu->gyro_bias_z * 0.001f;

    gx = MPU_ClampI16(hmpu->calib_accel_sum_x / hmpu->calib_count);
    gy = MPU_ClampI16(hmpu->calib_accel_sum_y / hmpu->calib_count);
    gz = MPU_ClampI16(hmpu->calib_accel_sum_z / hmpu->calib_count);

    hmpu->gravity_x_mg = gx;
    hmpu->gravity_y_mg = gy;
    hmpu->gravity_z_mg = gz;
    hmpu->gravity_x_mg_f = (float)gx;
    hmpu->gravity_y_mg_f = (float)gy;
    hmpu->gravity_z_mg_f = (float)gz;
    hmpu->roll_ref_deg = MPU_AccelRollDeg(gy, gz);
    hmpu->pitch_ref_deg = MPU_AccelPitchDeg(gx, gy, gz);
    hmpu->roll_ref_md = MPU_DegToMd(hmpu->roll_ref_deg);
    hmpu->pitch_ref_md = MPU_DegToMd(hmpu->pitch_ref_deg);

    offsets.accel_roll_offset_deg = hmpu->roll_ref_deg;
    offsets.accel_pitch_offset_deg = hmpu->pitch_ref_deg;
    offsets.gyro_x_offset_dps = hmpu->gyro_bias_x_dps;
    offsets.gyro_y_offset_dps = hmpu->gyro_bias_y_dps;
    offsets.gyro_z_offset_dps = hmpu->gyro_bias_z_dps;
    MPU9250_AttitudeInit(&hmpu->attitude,
                         MPU_ATTITUDE_DEFAULT_KP,
                         MPU_ATTITUDE_DEFAULT_KI,
                         MPU_ATTITUDE_FIXED_DT_S);
    MPU9250_AttitudeSetOffsets(&hmpu->attitude, &offsets);

    hmpu->angle_x_md = 0;
    hmpu->angle_y_md = 0;
    hmpu->angle_z_md = 0;
    hmpu->angle_x_deg = 0.0f;
    hmpu->angle_y_deg = 0.0f;
    hmpu->angle_z_deg = 0.0f;
    hmpu->rem_x = 0;
    hmpu->rem_y = 0;
    hmpu->rem_z = 0;

    CLEAR_FLAG(hmpu->flags, MPU_CALIBRATING | MPU_ERROR);
    SET_FLAG(hmpu->flags, CALIBRATED | MPU_STATIONARY);
    hmpu->latest.flags = hmpu->flags.byte;
}

static void MPU_ProcessCalibration(MPU9250_Handle_t *hmpu, uint8_t stationary)
{
    uint32_t elapsed_ms = hmpu->bus->hardware->now_ms(hmpu->bus->hardware_ctx) - hmpu->calib_started_ms;

    if (elapsed_ms < hmpu->calib_warmup_ms) {
        return;
    }

    /* Moving samples cannot define the upright reference or gyro bias. */
    if (!stationary) {
        MPU_ResetCalibrationAccum(hmpu);
        hmpu->calib_started_ms = hmpu->bus->hardware->now_ms(hmpu->bus->hardware_ctx);
        return;
    }

    hmpu->calib_sum_x += hmpu->data.gyro_x_mdps;
    hmpu->calib_sum_y += hmpu->data.gyro_y_mdps;
    hmpu->calib_sum_z += hmpu->data.gyro_z_mdps;
    hmpu->calib_accel_sum_x += hmpu->data.accel_x_mg;
    hmpu->calib_accel_sum_y += hmpu->data.accel_y_mg;
    hmpu->calib_accel_sum_z += hmpu->data.accel_z_mg;
    hmpu->calib_count++;

    if (stationary) {
        SET_FLAG(hmpu->flags, MPU_STATIONARY);
    }

    if (elapsed_ms < ((uint32_t)hmpu->calib_warmup_ms + hmpu->calib_window_ms) &&
        hmpu->calib_count < hmpu->calib_target) {
        return;
    }

    if (hmpu->calib_count >= MPU_CALIB_MIN_SAMPLES) {
        MPU_FinishCalibration(hmpu);
        return;
    }
}

static void MPU_UpdateEstimator(MPU9250_Handle_t *hmpu, uint8_t stationary)
{
    MPU9250_NormalizedData_t normalized;
    MPU9250_Attitude_t attitude;
    uint16_t next_seq;

    if (!IS_FLAG_SET(hmpu->flags, CALIBRATED)) {
        return;
    }

    MPU9250_GetNormalizedData(hmpu, &normalized);
    MPU9250_AttitudeUpdate(&hmpu->attitude, &normalized, &attitude);

    hmpu->angle_x_deg = attitude.roll_deg;
    hmpu->angle_y_deg = attitude.pitch_deg;
    hmpu->angle_z_deg = attitude.yaw_deg;
    hmpu->angle_x_md = MPU_DegToMd(hmpu->angle_x_deg);
    hmpu->angle_y_md = MPU_DegToMd(hmpu->angle_y_deg);
    hmpu->angle_z_md = MPU_DegToMd(hmpu->angle_z_deg);

    hmpu->gravity_x_mg_f += ((float)hmpu->data.accel_x_mg - hmpu->gravity_x_mg_f) /
                            (float)(1u << MPU_GRAVITY_LPF_SHIFT);
    hmpu->gravity_y_mg_f += ((float)hmpu->data.accel_y_mg - hmpu->gravity_y_mg_f) /
                            (float)(1u << MPU_GRAVITY_LPF_SHIFT);
    hmpu->gravity_z_mg_f += ((float)hmpu->data.accel_z_mg - hmpu->gravity_z_mg_f) /
                            (float)(1u << MPU_GRAVITY_LPF_SHIFT);
    hmpu->gravity_x_mg = MPU_ClampI16(MPU_RoundFloatToI32(hmpu->gravity_x_mg_f));
    hmpu->gravity_y_mg = MPU_ClampI16(MPU_RoundFloatToI32(hmpu->gravity_y_mg_f));
    hmpu->gravity_z_mg = MPU_ClampI16(MPU_RoundFloatToI32(hmpu->gravity_z_mg_f));

    next_seq = (uint16_t)(hmpu->latest.sample_seq + 1u);
    hmpu->latest.sample_seq = (next_seq == 0u) ? 1u : next_seq;
    hmpu->latest.accel_x_mg = hmpu->data.accel_x_mg;
    hmpu->latest.accel_y_mg = hmpu->data.accel_y_mg;
    hmpu->latest.accel_z_mg = hmpu->data.accel_z_mg;
    hmpu->latest.linear_accel_x_mg = hmpu->data.accel_x_mg - hmpu->gravity_x_mg;
    hmpu->latest.linear_accel_y_mg = hmpu->data.accel_y_mg - hmpu->gravity_y_mg;
    hmpu->latest.linear_accel_z_mg = hmpu->data.accel_z_mg - hmpu->gravity_z_mg;
    hmpu->latest.gyro_x_mdps = MPU_RoundFloatToI32(attitude.gyro_roll_rate_dps * 1000.0f);
    hmpu->latest.gyro_y_mdps = MPU_RoundFloatToI32(attitude.gyro_pitch_rate_dps * 1000.0f);
    hmpu->latest.gyro_z_mdps = MPU_RoundFloatToI32(attitude.gyro_yaw_rate_dps * 1000.0f);
    hmpu->latest.mag_x_nt = hmpu->data.mag_x_nt;
    hmpu->latest.mag_y_nt = hmpu->data.mag_y_nt;
    hmpu->latest.mag_z_nt = hmpu->data.mag_z_nt;
    hmpu->latest.roll_md = hmpu->angle_x_md;
    hmpu->latest.pitch_md = hmpu->angle_y_md;
    hmpu->latest.yaw_md = hmpu->angle_z_md;

    CLEAR_FLAG(hmpu->flags, MPU_ERROR | MPU_STATIONARY);
    SET_FLAG(hmpu->flags, CALIBRATED | MPU_VALID | MPU_NEW_READING);
    if (stationary) {
        SET_FLAG(hmpu->flags, MPU_STATIONARY);
    }
    hmpu->latest.flags = hmpu->flags.byte;
    hmpu->last_attitude_ms = hmpu->sample_acquired_ms;
}

static AM_Result MPU9250_RequestRead(MPU9250_Handle_t *hmpu,
                                             MPU9250_ReadMode mode,
                                             uint8_t start_reg,
                                             uint8_t len)
{
    AM_Result status;

    if (hmpu == NULL || hmpu->bus == NULL) {
        return AM_ERROR;
    }

    if (hmpu->transfer_state != MPU9250_XFER_IDLE) {
        return AM_BUSY;
    }

    hmpu->read_mode = mode;
    hmpu->tx_buffer[0] = start_reg;
    hmpu->expected_rx_len = len;
    hmpu->transfer_state = MPU9250_XFER_WAIT_GRANT;

    status = hmpu->bus->request(hmpu->bus->ctx, I2C_REQ_PRIO_HIGH);
    if (status == AM_ERROR) {
        hmpu->transfer_state = MPU9250_XFER_IDLE;
        MPU_SetError(hmpu);
    }

    return status;
}

AM_Result MPU9250_Init(MPU9250_Handle_t *hmpu,
                               uint8_t address,
                               const AM_I2C_DevicePort *bus,
                               MPU9250_Callback on_data_ready,
                               volatile bool *trigger)
{
    if (hmpu == NULL || bus == NULL || trigger == NULL) {
        return AM_ERROR;
    }

    memset(hmpu, 0, sizeof(*hmpu));
    hmpu->bus = bus;
    hmpu->i2c_address = address;
    hmpu->tx_buffer[0] = MPU9250_BURST_REG;
    hmpu->expected_rx_len = sizeof(hmpu->rx_buffer);
    hmpu->read_mode = MPU9250_READ_ALL;
    hmpu->last_read_mode = MPU9250_READ_ALL;
    hmpu->transfer_state = MPU9250_XFER_IDLE;
    hmpu->on_data_ready_cb = on_data_ready;
    hmpu->trigger = trigger;
    hmpu->dt_div = 200u;
    hmpu->mag_calibration.factory_adjust_x = 1.0f;
    hmpu->mag_calibration.factory_adjust_y = 1.0f;
    hmpu->mag_calibration.factory_adjust_z = 1.0f;
    hmpu->mag_calibration.scale_x = 1.0f;
    hmpu->mag_calibration.scale_y = 1.0f;
    hmpu->mag_calibration.scale_z = 1.0f;
    MPU9250_AttitudeInit(&hmpu->attitude,
                         MPU_ATTITUDE_DEFAULT_KP,
                         MPU_ATTITUDE_DEFAULT_KI,
                         MPU_ATTITUDE_FIXED_DT_S);

    return AM_OK;
}

AM_Result MPU9250_BindPort(MPU9250_Handle_t *hmpu)
{
    if (!hmpu || !hmpu->bus) return AM_ERROR;
    return hmpu->bus->bind(hmpu->bus->ctx,hmpu->i2c_address,1u,hmpu,
                          MPU_I2C_RequestApproved,MPU_I2C_TxComplete,
                          MPU_I2C_RxComplete,MPU_I2C_Error);
}


static AM_Result MPU_WriteRegister(MPU9250_Handle_t *hmpu,
                                                uint8_t reg,
                                                uint8_t value)
{
    return hmpu->bus->hardware->mem_write(hmpu->bus->hardware_ctx,
                             (uint16_t)(hmpu->i2c_address << 1),
                             reg,
                             1u,
                             &value,
                             1u,
                             100u);
}

static AM_Result MPU_ReadRegister(MPU9250_Handle_t *hmpu,
                                               uint8_t reg,
                                               uint8_t *value)
{
    return hmpu->bus->hardware->mem_read(hmpu->bus->hardware_ctx,
                            (uint16_t)(hmpu->i2c_address << 1),
                            reg,
                            1u,
                            value,
                            1u,
                            100u);
}

static AM_Result AK8963_WriteRegister(MPU9250_Handle_t *hmpu,
                                              uint8_t reg,
                                              uint8_t value)
{
    return hmpu->bus->hardware->mem_write(hmpu->bus->hardware_ctx,
                             (uint16_t)(AK8963_I2C_ADDRESS << 1),
                             reg,
                             1u,
                             &value,
                             1u,
                             100u);
}

static AM_Result AK8963_ReadRegisters(MPU9250_Handle_t *hmpu,
                                              uint8_t reg,
                                              uint8_t *data,
                                              uint16_t len)
{
    return hmpu->bus->hardware->mem_read(hmpu->bus->hardware_ctx,
                            (uint16_t)(AK8963_I2C_ADDRESS << 1),
                            reg,
                            1u,
                            data,
                            len,
                            100u);
}

static AM_Result MPU_ConfigureAk8963(MPU9250_Handle_t *hmpu)
{
    uint8_t value;
    uint8_t asa[3];

    if (MPU_WriteRegister(hmpu, MPU9250_REG_USER_CTRL, 0u) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_INT_PIN_CFG, MPU9250_BYPASS_EN) != AM_OK) {
        return AM_ERROR;
    }
    hmpu->bus->hardware->delay_ms(hmpu->bus->hardware_ctx, 10u);

    if (AK8963_ReadRegisters(hmpu, AK8963_REG_WIA, &value, 1u) != AM_OK ||
        value != AK8963_WHO_AM_I_VALUE) {
        return AM_ERROR;
    }
    if (AK8963_WriteRegister(hmpu, AK8963_REG_CNTL1, AK8963_MODE_POWER_DOWN) != AM_OK) {
        return AM_ERROR;
    }
    hmpu->bus->hardware->delay_ms(hmpu->bus->hardware_ctx, 10u);
    if (AK8963_WriteRegister(hmpu, AK8963_REG_CNTL1, AK8963_MODE_FUSE_ROM) != AM_OK) {
        return AM_ERROR;
    }
    hmpu->bus->hardware->delay_ms(hmpu->bus->hardware_ctx, 10u);
    if (AK8963_ReadRegisters(hmpu, AK8963_REG_ASAX, asa, sizeof(asa)) != AM_OK) {
        return AM_ERROR;
    }
    hmpu->mag_calibration.factory_adjust_x =
        (((float)asa[0] - 128.0f) / 256.0f) + 1.0f;
    hmpu->mag_calibration.factory_adjust_y =
        (((float)asa[1] - 128.0f) / 256.0f) + 1.0f;
    hmpu->mag_calibration.factory_adjust_z =
        (((float)asa[2] - 128.0f) / 256.0f) + 1.0f;

    if (AK8963_WriteRegister(hmpu, AK8963_REG_CNTL1, AK8963_MODE_POWER_DOWN) != AM_OK) {
        return AM_ERROR;
    }
    hmpu->bus->hardware->delay_ms(hmpu->bus->hardware_ctx, 10u);
    if (AK8963_WriteRegister(hmpu,
                             AK8963_REG_CNTL1,
                             AK8963_MODE_CONT_100HZ_16BIT) != AM_OK) {
        return AM_ERROR;
    }
    hmpu->bus->hardware->delay_ms(hmpu->bus->hardware_ctx, 10u);

    if (MPU_WriteRegister(hmpu, MPU9250_REG_INT_PIN_CFG, 0u) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_USER_CTRL, MPU9250_USER_I2C_MST_EN) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_I2C_MST_CTRL, MPU9250_I2C_MST_400KHZ) != AM_OK ||
        MPU_WriteRegister(hmpu,
                          MPU9250_REG_I2C_SLV0_ADDR,
                          MPU9250_I2C_SLV_READ | AK8963_I2C_ADDRESS) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_I2C_SLV0_REG, AK8963_REG_ST1) != AM_OK ||
        MPU_WriteRegister(hmpu,
                          MPU9250_REG_I2C_SLV0_CTRL,
                          MPU9250_I2C_SLV_ENABLE | MPU9250_MAG_EXT_LENGTH) != AM_OK) {
        return AM_ERROR;
    }

    return AM_OK;
}

AM_Result MPU9250_Configure(MPU9250_Handle_t *hmpu)
{
    uint8_t value;

    if (hmpu == NULL || hmpu->bus == NULL) {
        return AM_ERROR;
    }

    if (MPU_ReadRegister(hmpu, MPU9250_REG_WHO_AM_I, &value) != AM_OK ||
        value != MPU9250_WHO_AM_I_VALUE) {
        MPU_SetError(hmpu);
        return AM_ERROR;
    }

    if (MPU_WriteRegister(hmpu, MPU9250_REG_PWR_MGMT_1, 0x80u) != AM_OK) {
        MPU_SetError(hmpu);
        return AM_ERROR;
    }
    hmpu->bus->hardware->delay_ms(hmpu->bus->hardware_ctx, 100u);

    if (MPU_WriteRegister(hmpu, MPU9250_REG_PWR_MGMT_1, 0x01u) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_PWR_MGMT_2, 0x00u) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_SMPLRT_DIV, MPU_SMPLRT_DIV_200HZ) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_CONFIG, 0x03u) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_GYRO_CFG, 0x00u) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_ACCEL_CFG, 0x00u) != AM_OK ||
        MPU_WriteRegister(hmpu, MPU9250_REG_ACCEL_CFG_2, 0x03u) != AM_OK ||
        MPU_ConfigureAk8963(hmpu) != AM_OK) {
        MPU_SetError(hmpu);
        return AM_ERROR;
    }

    CLEAR_FLAG(hmpu->flags, MPU_ERROR);
    return AM_OK;
}

void MPU9250_CheckTrigger(MPU9250_Handle_t *hmpu)
{
    if (hmpu == NULL || hmpu->trigger == NULL) {
        return;
    }

    if (*(hmpu->trigger)) {
        *(hmpu->trigger) = false;
        (void)MPU9250_RequestAllData(hmpu);
    }
}

void MPU9250_Process(MPU9250_Handle_t *hmpu)
{
    uint8_t stationary = 0u;

    if (hmpu == NULL) {
        return;
    }

    if (!IS_FLAG_SET(hmpu->flags, DATA_READY)) {
        MPU9250_CheckTrigger(hmpu);
        return;
    }

    CLEAR_FLAG(hmpu->flags, DATA_READY);
    MPU9250_Convert(hmpu);

    if (hmpu->last_read_mode == MPU9250_READ_ALL) {
        stationary = MPU_IsStationary(hmpu);
        if (stationary) {
            SET_FLAG(hmpu->flags, MPU_STATIONARY);
        } else {
            CLEAR_FLAG(hmpu->flags, MPU_STATIONARY);
        }

        if (IS_FLAG_SET(hmpu->flags, MPU_CALIBRATING) ||
            !IS_FLAG_SET(hmpu->flags, CALIBRATED)) {
            MPU_ProcessCalibration(hmpu, stationary);
        } else {
            MPU_UpdateEstimator(hmpu, stationary);
        }
    }

    if (hmpu->on_data_ready_cb != NULL) {
        hmpu->on_data_ready_cb();
    }
    /* Consume the completed buffer before starting another DMA into it. */
    MPU9250_CheckTrigger(hmpu);
}

AM_Result MPU9250_RequestAllData(MPU9250_Handle_t *hmpu)
{
    return MPU9250_RequestRead(hmpu, MPU9250_READ_ALL, MPU9250_BURST_REG, MPU9250_BURST_LENGTH);
}

AM_Result MPU9250_RequestAccel(MPU9250_Handle_t *hmpu)
{
    return MPU9250_RequestRead(hmpu, MPU9250_READ_ACCEL, MPU9250_BURST_REG, 6u);
}

AM_Result MPU9250_RequestGyro(MPU9250_Handle_t *hmpu)
{
    return MPU9250_RequestRead(hmpu, MPU9250_READ_GYRO, MPU9250_REG_GYRO_OUT, 6u);
}

AM_Result MPU9250_RequestTemp(MPU9250_Handle_t *hmpu)
{
    return MPU9250_RequestRead(hmpu, MPU9250_READ_TEMP, MPU9250_REG_TEMP_OUT, 2u);
}

AM_Result MPU9250_RequestOrientation(MPU9250_Handle_t *hmpu)
{
    return MPU9250_RequestAllData(hmpu);
}

const MPU9250_IntData_t *MPU9250_GetData(MPU9250_Handle_t *hmpu)
{
    return (hmpu != NULL) ? &hmpu->data : NULL;
}

AM_Result MPU9250_GetLatest(MPU9250_Handle_t *hmpu, MPU9250_Snapshot_t *out)
{
    if (hmpu == NULL || out == NULL) {
        return AM_ERROR;
    }

    *out = hmpu->latest;
    out->flags = hmpu->flags.byte;
    return MPU9250_IsSampleFresh(hmpu, hmpu->bus->hardware->now_ms(hmpu->bus->hardware_ctx)) ? AM_OK : AM_ERROR;
}

uint8_t MPU9250_IsSampleFresh(const MPU9250_Handle_t *hmpu, uint32_t now_ms)
{
    return (hmpu != NULL && IS_FLAG_SET(hmpu->flags, MPU_VALID) &&
            !IS_FLAG_SET(hmpu->flags, MPU_ERROR | MPU_CALIBRATING) &&
            (uint32_t)(now_ms - hmpu->last_attitude_ms) <= 50u) ? 1u : 0u;
}

uint8_t MPU9250_HasNewSample(const MPU9250_Handle_t *hmpu, uint16_t last_seq)
{
    return (hmpu != NULL &&
            IS_FLAG_SET(hmpu->flags, MPU_VALID) &&
            hmpu->latest.sample_seq != last_seq) ? 1u : 0u;
}

void MPU9250_ClearNewFlag(MPU9250_Handle_t *hmpu)
{
    if (hmpu == NULL) {
        return;
    }

    CLEAR_FLAG(hmpu->flags, MPU_NEW_READING);
    hmpu->latest.flags = hmpu->flags.byte;
}

void MPU9250_SetReferenceNow(MPU9250_Handle_t *hmpu)
{
    MPU9250_AttitudeOffsets_t offsets;
    float yaw_zero_deg;

    if (hmpu == NULL) {
        return;
    }

    yaw_zero_deg = hmpu->attitude.output.heading_deg;
    hmpu->roll_ref_deg = MPU_AccelRollDeg(hmpu->data.accel_y_mg, hmpu->data.accel_z_mg);
    hmpu->pitch_ref_deg = MPU_AccelPitchDeg(hmpu->data.accel_x_mg,
                                            hmpu->data.accel_y_mg,
                                            hmpu->data.accel_z_mg);
    hmpu->roll_ref_md = MPU_DegToMd(hmpu->roll_ref_deg);
    hmpu->pitch_ref_md = MPU_DegToMd(hmpu->pitch_ref_deg);
    offsets = hmpu->attitude.offsets;
    offsets.accel_roll_offset_deg = hmpu->roll_ref_deg;
    offsets.accel_pitch_offset_deg = hmpu->pitch_ref_deg;
    MPU9250_AttitudeInit(&hmpu->attitude,
                         MPU_ATTITUDE_DEFAULT_KP,
                         MPU_ATTITUDE_DEFAULT_KI,
                         MPU_ATTITUDE_FIXED_DT_S);
    hmpu->attitude.yaw_zero_deg = yaw_zero_deg;
    MPU9250_AttitudeSetOffsets(&hmpu->attitude, &offsets);
    hmpu->gravity_x_mg = hmpu->data.accel_x_mg;
    hmpu->gravity_y_mg = hmpu->data.accel_y_mg;
    hmpu->gravity_z_mg = hmpu->data.accel_z_mg;
    hmpu->gravity_x_mg_f = (float)hmpu->data.accel_x_mg;
    hmpu->gravity_y_mg_f = (float)hmpu->data.accel_y_mg;
    hmpu->gravity_z_mg_f = (float)hmpu->data.accel_z_mg;
    hmpu->angle_x_md = 0;
    hmpu->angle_y_md = 0;
    hmpu->angle_z_md = 0;
    hmpu->angle_x_deg = 0.0f;
    hmpu->angle_y_deg = 0.0f;
    hmpu->angle_z_deg = 0.0f;
    hmpu->rem_x = 0;
    hmpu->rem_y = 0;
    hmpu->rem_z = 0;
}

void MPU9250_GetOrientation(const MPU9250_Handle_t *hmpu, MPU9250_Orientation_t *out)
{
    if (hmpu == NULL || out == NULL) {
        return;
    }

    out->angle_x_md = hmpu->angle_x_md;
    out->angle_y_md = hmpu->angle_y_md;
    out->angle_z_md = hmpu->angle_z_md;
}

AM_Result MPU9250_ApplyCalibrationData(MPU9250_Handle_t *hmpu,
                                               const MPU9250_CalibrationData_t *calibration)
{
    if (hmpu == NULL || calibration == NULL) {
        return AM_ERROR;
    }

    hmpu->gyro_bias_x_dps = calibration->attitudeOffsets.gyro_x_offset_dps;
    hmpu->gyro_bias_y_dps = calibration->attitudeOffsets.gyro_y_offset_dps;
    hmpu->gyro_bias_z_dps = calibration->attitudeOffsets.gyro_z_offset_dps;
    hmpu->gyro_bias_x = MPU_RoundFloatToI32(hmpu->gyro_bias_x_dps * 1000.0f);
    hmpu->gyro_bias_y = MPU_RoundFloatToI32(hmpu->gyro_bias_y_dps * 1000.0f);
    hmpu->gyro_bias_z = MPU_RoundFloatToI32(hmpu->gyro_bias_z_dps * 1000.0f);

    hmpu->roll_ref_deg = calibration->attitudeOffsets.accel_roll_offset_deg;
    hmpu->pitch_ref_deg = calibration->attitudeOffsets.accel_pitch_offset_deg;
    hmpu->roll_ref_md = MPU_DegToMd(hmpu->roll_ref_deg);
    hmpu->pitch_ref_md = MPU_DegToMd(hmpu->pitch_ref_deg);

    hmpu->gravity_x_mg = calibration->gravity_x_mg;
    hmpu->gravity_y_mg = calibration->gravity_y_mg;
    hmpu->gravity_z_mg = calibration->gravity_z_mg;
    hmpu->gravity_x_mg_f = (float)calibration->gravity_x_mg;
    hmpu->gravity_y_mg_f = (float)calibration->gravity_y_mg;
    hmpu->gravity_z_mg_f = (float)calibration->gravity_z_mg;

    MPU9250_AttitudeInit(&hmpu->attitude,
                         MPU_ATTITUDE_DEFAULT_KP,
                         MPU_ATTITUDE_DEFAULT_KI,
                         MPU_ATTITUDE_FIXED_DT_S);
    MPU9250_AttitudeSetOffsets(&hmpu->attitude, &calibration->attitudeOffsets);

    hmpu->angle_x_md = 0;
    hmpu->angle_y_md = 0;
    hmpu->angle_z_md = 0;
    hmpu->angle_x_deg = 0.0f;
    hmpu->angle_y_deg = 0.0f;
    hmpu->angle_z_deg = 0.0f;
    hmpu->rem_x = 0;
    hmpu->rem_y = 0;
    hmpu->rem_z = 0;

    CLEAR_FLAG(hmpu->flags, MPU_CALIBRATING | MPU_ERROR);
    SET_FLAG(hmpu->flags, CALIBRATED);
    hmpu->latest.flags = hmpu->flags.byte;

    return AM_OK;
}

AM_Result MPU9250_GetCalibrationData(const MPU9250_Handle_t *hmpu,
                                             MPU9250_CalibrationData_t *out)
{
    if (hmpu == NULL || out == NULL || !IS_FLAG_SET(hmpu->flags, CALIBRATED)) {
        return AM_ERROR;
    }

    out->attitudeOffsets = hmpu->attitude.offsets;
    out->gravity_x_mg = hmpu->gravity_x_mg;
    out->gravity_y_mg = hmpu->gravity_y_mg;
    out->gravity_z_mg = hmpu->gravity_z_mg;

    return AM_OK;
}

void MPU9250_GetNormalizedData(const MPU9250_Handle_t *hmpu, MPU9250_NormalizedData_t *out)
{
    if (hmpu == NULL || out == NULL) {
        return;
    }

    out->ax_g = (float)hmpu->data.accel_x_mg * 0.001f;
    out->ay_g = (float)hmpu->data.accel_y_mg * 0.001f;
    out->az_g = (float)hmpu->data.accel_z_mg * 0.001f;
    out->gx_dps = (float)hmpu->data.gyro_x_mdps * 0.001f;
    out->gy_dps = (float)hmpu->data.gyro_y_mdps * 0.001f;
    out->gz_dps = (float)hmpu->data.gyro_z_mdps * 0.001f;
    out->mx_uT = (float)hmpu->data.mag_x_nt * 0.001f;
    out->my_uT = (float)hmpu->data.mag_y_nt * 0.001f;
    out->mz_uT = (float)hmpu->data.mag_z_nt * 0.001f;
    out->sample_dt_s = (float)hmpu->latest.sample_dt_us * 0.000001f;
    out->mag_valid = hmpu->data.mag_valid;
}

AM_Result MPU9250_GetAttitude(const MPU9250_Handle_t *hmpu, MPU9250_Attitude_t *out)
{
    if (hmpu == NULL || out == NULL || !MPU9250_IsSampleFresh(hmpu, hmpu->bus->hardware->now_ms(hmpu->bus->hardware_ctx))) {
        return AM_ERROR;
    }

    *out = hmpu->attitude.output;
    return AM_OK;
}

void MPU9250_GetMagCalibration(const MPU9250_Handle_t *hmpu,
                               MPU9250_MagCalibration_t *out)
{
    if (hmpu == NULL || out == NULL) {
        return;
    }
    *out = hmpu->mag_calibration;
}

AM_Result MPU9250_SetMagCalibration(MPU9250_Handle_t *hmpu,
                                            const MPU9250_MagCalibration_t *calibration)
{
    if (hmpu == NULL || calibration == NULL ||
        calibration->scale_x <= 0.0f ||
        calibration->scale_y <= 0.0f ||
        calibration->scale_z <= 0.0f) {
        return AM_ERROR;
    }

    hmpu->mag_calibration.bias_x_uT = calibration->bias_x_uT;
    hmpu->mag_calibration.bias_y_uT = calibration->bias_y_uT;
    hmpu->mag_calibration.bias_z_uT = calibration->bias_z_uT;
    hmpu->mag_calibration.scale_x = calibration->scale_x;
    hmpu->mag_calibration.scale_y = calibration->scale_y;
    hmpu->mag_calibration.scale_z = calibration->scale_z;
    return AM_OK;
}

float MPU9250_GetPitchDeg(const MPU9250_Handle_t *hmpu)
{
    return MPU9250_GetFrontTiltDeg(hmpu);
}

float MPU9250_GetFrontTiltDeg(const MPU9250_Handle_t *hmpu)
{
    if (hmpu == NULL || !IS_FLAG_SET(hmpu->flags, MPU_VALID)) {
        return 0.0f;
    }

    return hmpu->attitude.output.pitch_deg;
}

float MPU9250_GetLateralTiltDeg(const MPU9250_Handle_t *hmpu)
{
    if (hmpu == NULL || !IS_FLAG_SET(hmpu->flags, MPU_VALID)) {
        return 0.0f;
    }

    return hmpu->attitude.output.roll_deg;
}

uint8_t MPU9250_IsCalibrating(const MPU9250_Handle_t *hmpu)
{
    return (hmpu != NULL && IS_FLAG_SET(hmpu->flags, MPU_CALIBRATING)) ? 1u : 0u;
}

uint8_t MPU9250_IsCalibrated(const MPU9250_Handle_t *hmpu)
{
    return (hmpu != NULL && IS_FLAG_SET(hmpu->flags, CALIBRATED)) ? 1u : 0u;
}

uint8_t MPU9250_GetCalibrationProgressPercent(const MPU9250_Handle_t *hmpu)
{
    uint32_t pct;
    uint32_t timePct = 0u;
    uint32_t samplePct = 0u;

    if (hmpu == NULL) {
        return 0u;
    }

    if (!IS_FLAG_SET(hmpu->flags, MPU_CALIBRATING)) {
        return IS_FLAG_SET(hmpu->flags, CALIBRATED) ? 100u : 0u;
    }

    if ((uint32_t)hmpu->calib_warmup_ms + hmpu->calib_window_ms > 0u) {
        const uint32_t totalMs = (uint32_t)hmpu->calib_warmup_ms + hmpu->calib_window_ms;
        const uint32_t elapsedMs = hmpu->bus->hardware->now_ms(hmpu->bus->hardware_ctx) - hmpu->calib_started_ms;
        timePct = (elapsedMs * 100u) / totalMs;
    }

    if (hmpu->calib_target > 0u) {
        samplePct = ((uint32_t)hmpu->calib_count * 100u) / hmpu->calib_target;
    }

    pct = (timePct > samplePct) ? timePct : samplePct;
    return (pct > 100u) ? 100u : (uint8_t)pct;
}

void MPU9250_Convert(MPU9250_Handle_t *hmpu)
{
    if (hmpu == NULL) {
        return;
    }

    switch (hmpu->last_read_mode) {
        case MPU9250_READ_ALL: {
            const int16_t ax = MERGE_RAW_BE(0u);
            const int16_t ay = MERGE_RAW_BE(2u);
            const int16_t az = MERGE_RAW_BE(4u);
            const int16_t temperature = MERGE_RAW_BE(6u);
            const int16_t gx = MERGE_RAW_BE(8u);
            const int16_t gy = MERGE_RAW_BE(10u);
            const int16_t gz = MERGE_RAW_BE(12u);
            const uint8_t mag_st1 = hmpu->rx_buffer[MPU9250_MAG_EXT_OFFSET];
            const uint8_t mag_st2 =
                hmpu->rx_buffer[MPU9250_MAG_EXT_OFFSET + MPU9250_MAG_EXT_LENGTH - 1u];

            hmpu->data.accel_x_mg = (int16_t)((int32_t)ax * 1000 / 16384);
            hmpu->data.accel_y_mg = (int16_t)((int32_t)ay * 1000 / 16384);
            hmpu->data.accel_z_mg = (int16_t)((int32_t)az * 1000 / 16384);
            hmpu->data.temperature_celsius_x100 =
                (int16_t)(((int32_t)temperature * 10000 / 33387) + 2100);
            hmpu->data.gyro_x_mdps = (int32_t)gx * 1000 / 131;
            hmpu->data.gyro_y_mdps = (int32_t)gy * 1000 / 131;
            hmpu->data.gyro_z_mdps = (int32_t)gz * 1000 / 131;

            if ((mag_st1 & AK8963_ST1_DRDY) != 0u &&
                (mag_st2 & AK8963_ST2_HOFL) == 0u) {
                const int16_t raw_mx = MERGE_RAW_LE(MPU9250_MAG_EXT_OFFSET + 1u);
                const int16_t raw_my = MERGE_RAW_LE(MPU9250_MAG_EXT_OFFSET + 3u);
                const int16_t raw_mz = MERGE_RAW_LE(MPU9250_MAG_EXT_OFFSET + 5u);
                const float ak_x_uT =
                    ((float)raw_mx * AK8963_SENSITIVITY_UT_LSB *
                     hmpu->mag_calibration.factory_adjust_x);
                const float ak_y_uT =
                    ((float)raw_my * AK8963_SENSITIVITY_UT_LSB *
                     hmpu->mag_calibration.factory_adjust_y);
                const float ak_z_uT =
                    ((float)raw_mz * AK8963_SENSITIVITY_UT_LSB *
                     hmpu->mag_calibration.factory_adjust_z);
                /*
                 * The AK8963 axes inside the package differ from the
                 * accelerometer/gyro axes: X/Y are exchanged and Z is inverted.
                 */
                const float mag_x_uT =
                    (ak_y_uT - hmpu->mag_calibration.bias_x_uT) *
                    hmpu->mag_calibration.scale_x;
                const float mag_y_uT =
                    (ak_x_uT - hmpu->mag_calibration.bias_y_uT) *
                    hmpu->mag_calibration.scale_y;
                const float mag_z_uT =
                    (-ak_z_uT - hmpu->mag_calibration.bias_z_uT) *
                    hmpu->mag_calibration.scale_z;
                const float field_uT = sqrtf((mag_x_uT * mag_x_uT) +
                                             (mag_y_uT * mag_y_uT) +
                                             (mag_z_uT * mag_z_uT));

                if (field_uT >= MPU_MAG_FIELD_MIN_UT &&
                    field_uT <= MPU_MAG_FIELD_MAX_UT) {
                    hmpu->data.mag_x_nt = MPU_RoundFloatToI32(mag_x_uT * 1000.0f);
                    hmpu->data.mag_y_nt = MPU_RoundFloatToI32(mag_y_uT * 1000.0f);
                    hmpu->data.mag_z_nt = MPU_RoundFloatToI32(mag_z_uT * 1000.0f);
                    hmpu->data.mag_valid = 1u;
                } else {
                    hmpu->data.mag_valid = 0u;
                }
            }
            break;
        }
        case MPU9250_READ_ACCEL: {
            const int16_t ax = MERGE_RAW_BE(0u);
            const int16_t ay = MERGE_RAW_BE(2u);
            const int16_t az = MERGE_RAW_BE(4u);
            hmpu->data.accel_x_mg = (int16_t)((int32_t)ax * 1000 / 16384);
            hmpu->data.accel_y_mg = (int16_t)((int32_t)ay * 1000 / 16384);
            hmpu->data.accel_z_mg = (int16_t)((int32_t)az * 1000 / 16384);
            break;
        }
        case MPU9250_READ_TEMP: {
            const int16_t temperature = MERGE_RAW_BE(0u);
            hmpu->data.temperature_celsius_x100 =
                (int16_t)(((int32_t)temperature * 10000 / 33387) + 2100);
            break;
        }
        case MPU9250_READ_GYRO:
        case MPU9250_READ_ORIENTATION: {
            const int16_t gx = MERGE_RAW_BE(0u);
            const int16_t gy = MERGE_RAW_BE(2u);
            const int16_t gz = MERGE_RAW_BE(4u);
            hmpu->data.gyro_x_mdps = (int32_t)gx * 1000 / 131;
            hmpu->data.gyro_y_mdps = (int32_t)gy * 1000 / 131;
            hmpu->data.gyro_z_mdps = (int32_t)gz * 1000 / 131;
            break;
        }
        default:
            break;
    }
}

AM_Result MPU9250_ReadWhoAmI(MPU9250_Handle_t *hmpu, uint8_t *whoami)
{
    if (hmpu == NULL || whoami == NULL) {
        return AM_ERROR;
    }

    return hmpu->bus->hardware->mem_read(hmpu->bus->hardware_ctx,
                            (uint16_t)(hmpu->i2c_address << 1),
                            MPU9250_REG_WHO_AM_I,
                            1u,
                            whoami,
                            1,
                            100);
}

void MPU9250_StartCalibration(MPU9250_Handle_t *hmpu, uint16_t warmup_ms, uint16_t window_ms)
{
    uint32_t expected_samples;

    if (hmpu == NULL) {
        return;
    }

    if (warmup_ms == 0u) {
        warmup_ms = MPU_CALIB_DEFAULT_WARMUP_MS;
    }
    if (window_ms < MPU_CALIB_MIN_WINDOW_MS) {
        window_ms = MPU_CALIB_MIN_WINDOW_MS;
    }
    if (window_ms > MPU_CALIB_MAX_WINDOW_MS) {
        window_ms = MPU_CALIB_MAX_WINDOW_MS;
    }

    expected_samples = ((uint32_t)window_ms * 1000u) / MPU_DEFAULT_DT_US;
    expected_samples = (expected_samples * 3u) / 4u;
    if (expected_samples < MPU_CALIB_MIN_SAMPLES) {
        expected_samples = MPU_CALIB_MIN_SAMPLES;
    }

    MPU_ResetCalibrationAccum(hmpu);
    hmpu->calib_warmup_ms = warmup_ms;
    hmpu->calib_window_ms = window_ms;
    hmpu->calib_target = (uint16_t)expected_samples;
    hmpu->calib_started_ms = hmpu->bus->hardware->now_ms(hmpu->bus->hardware_ctx);
    hmpu->prev_accel_valid = 0u;
    hmpu->last_sample_time_us = 0u;

    CLEAR_FLAG(hmpu->flags, CALIBRATED | MPU_VALID | MPU_NEW_READING | MPU_ERROR);
    SET_FLAG(hmpu->flags, MPU_CALIBRATING);
    hmpu->latest.flags = hmpu->flags.byte;

    if (hmpu->trigger != NULL) {
        *(hmpu->trigger) = true;
    }
}

void MPU9250_CalibrateGyro(MPU9250_Handle_t *hmpu, uint16_t samples)
{
    uint32_t window_ms = (uint32_t)samples * MPU_DEFAULT_DT_US / 1000u;

    if (window_ms < MPU_CALIB_MIN_WINDOW_MS) {
        window_ms = 2000u;
    }

    MPU9250_StartCalibration(hmpu,
                             MPU_CALIB_DEFAULT_WARMUP_MS,
                             (uint16_t)window_ms);
}

static void MPU_I2C_RequestApproved(void *ctx)
{
    MPU9250_Handle_t *hmpu = (MPU9250_Handle_t *)ctx;

    if (hmpu == NULL) {
        return;
    }

    if (hmpu->transfer_state != MPU9250_XFER_WAIT_GRANT) {
        (void)hmpu->bus->release(hmpu->bus->ctx);
        return;
    }

    hmpu->transfer_state = MPU9250_XFER_TX_REG;
    if (hmpu->bus->hardware->tx_dma(hmpu->bus->hardware_ctx,
                                    (uint16_t)(hmpu->i2c_address << 1),
                                    &hmpu->tx_buffer[0],
                                    1) != AM_OK) {
        if (hmpu->bus->transfer_failed) {
            hmpu->bus->transfer_failed(hmpu->bus->ctx);
            return;
        }
        hmpu->transfer_state = MPU9250_XFER_IDLE;
        MPU_SetError(hmpu);
        (void)hmpu->bus->release(hmpu->bus->ctx);
        return;
    }

    SET_FLAG(hmpu->flags, TX_SENT);
}

static void MPU_I2C_TxComplete(void *ctx)
{
    MPU9250_Handle_t *hmpu = (MPU9250_Handle_t *)ctx;

    if (hmpu == NULL || hmpu->transfer_state != MPU9250_XFER_TX_REG) {
        return;
    }

    hmpu->transfer_state = MPU9250_XFER_RX_DATA;
    if (hmpu->bus->hardware->rx_dma(hmpu->bus->hardware_ctx,
                                   (uint16_t)(hmpu->i2c_address << 1),
                                   hmpu->rx_buffer,
                                   hmpu->expected_rx_len) != AM_OK) {
        if (hmpu->bus->transfer_failed) {
            hmpu->bus->transfer_failed(hmpu->bus->ctx);
            return;
        }
        hmpu->transfer_state = MPU9250_XFER_IDLE;
        CLEAR_FLAG(hmpu->flags, TX_SENT);
        MPU_SetError(hmpu);
        (void)hmpu->bus->release(hmpu->bus->ctx);
        return;
    }

    CLEAR_FLAG(hmpu->flags, TX_SENT);
}

static void MPU_I2C_RxComplete(void *ctx)
{
    MPU9250_Handle_t *hmpu = (MPU9250_Handle_t *)ctx;

    if (hmpu == NULL || hmpu->transfer_state != MPU9250_XFER_RX_DATA) {
        return;
    }

    hmpu->transfer_state = MPU9250_XFER_IDLE;
    hmpu->last_read_mode = hmpu->read_mode;
    MPU_UpdateSampleTiming(hmpu);

    (void)hmpu->bus->release(hmpu->bus->ctx);
    CLEAR_FLAG(hmpu->flags, MPU_ERROR);
    SET_FLAG(hmpu->flags, DATA_READY);
}

static void MPU_I2C_Error(void *ctx, I2C_ManagerError err)
{
    MPU9250_Handle_t *hmpu = (MPU9250_Handle_t *)ctx;
    (void)err;

    if (hmpu == NULL) {
        return;
    }

    hmpu->transfer_state = MPU9250_XFER_IDLE;
    hmpu->last_read_mode = hmpu->read_mode;
    CLEAR_FLAG(hmpu->flags, TX_SENT | DATA_READY);
    memset(hmpu->rx_buffer, 0, hmpu->expected_rx_len);
    MPU_SetError(hmpu);
}
