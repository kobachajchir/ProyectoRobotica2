#ifndef MPU9250_DATA_H
#define MPU9250_DATA_H
#include "am_port.h"
#include "types/bitmap_type.h"
#define MPU_NEW_READING   BIT3_MASK
#define MPU_VALID         BIT4_MASK
#define MPU_ERROR         BIT5_MASK
#define MPU_STATIONARY    BIT6_MASK
#define MPU_CALIBRATING   BIT7_MASK

typedef void (*MPU9250_Callback)(void);

typedef struct {
    int16_t accel_x_mg;
    int16_t accel_y_mg;
    int16_t accel_z_mg;
    int16_t temperature_celsius_x100;
    int32_t gyro_x_mdps;
    int32_t gyro_y_mdps;
    int32_t gyro_z_mdps;
    int32_t mag_x_nt;
    int32_t mag_y_nt;
    int32_t mag_z_nt;
    uint8_t mag_valid;
} MPU9250_IntData_t;

typedef struct {
    int32_t angle_x_md;
    int32_t angle_y_md;
    int32_t angle_z_md;
} MPU9250_Orientation_t;

typedef struct {
    float ax_g;
    float ay_g;
    float az_g;
    float gx_dps;
    float gy_dps;
    float gz_dps;
    float mx_uT;
    float my_uT;
    float mz_uT;
    float sample_dt_s;
    uint8_t mag_valid;
} MPU9250_NormalizedData_t;

typedef struct {
    float accel_roll_offset_deg;
    float accel_pitch_offset_deg;
    float gyro_x_offset_dps;
    float gyro_y_offset_dps;
    float gyro_z_offset_dps;
} MPU9250_AttitudeOffsets_t;

typedef struct {
    float roll_deg;
    float pitch_deg;
    /* Relative magnetic yaw in [-180, 180]. Resettable without losing heading. */
    float yaw_deg;
    /* Absolute magnetic heading in [0, 360). */
    float heading_deg;
    float accel_roll_deg;
    float accel_pitch_deg;
    float mag_heading_deg;
    float gyro_roll_rate_dps;
    float gyro_pitch_rate_dps;
    float gyro_yaw_rate_dps;
    uint8_t mag_valid;
} MPU9250_Attitude_t;

typedef struct {
    MPU9250_AttitudeOffsets_t offsets;
    MPU9250_Attitude_t output;
    float q0;
    float q1;
    float q2;
    float q3;
    float integral_x;
    float integral_y;
    float integral_z;
    float kp;
    float ki;
    float fixed_dt_s;
    float yaw_zero_deg;
    uint8_t initialized;
} MPU9250_AttitudeState_t;

typedef struct {
    MPU9250_AttitudeOffsets_t attitudeOffsets;
    int16_t gravity_x_mg;
    int16_t gravity_y_mg;
    int16_t gravity_z_mg;
} MPU9250_CalibrationData_t;

typedef struct {
    float factory_adjust_x;
    float factory_adjust_y;
    float factory_adjust_z;
    float bias_x_uT;
    float bias_y_uT;
    float bias_z_uT;
    float scale_x;
    float scale_y;
    float scale_z;
} MPU9250_MagCalibration_t;

typedef struct {
    uint8_t flags;
    uint16_t sample_seq;
    uint16_t sample_dt_us;
    int16_t accel_x_mg;
    int16_t accel_y_mg;
    int16_t accel_z_mg;
    int16_t linear_accel_x_mg;
    int16_t linear_accel_y_mg;
    int16_t linear_accel_z_mg;
    int32_t gyro_x_mdps;
    int32_t gyro_y_mdps;
    int32_t gyro_z_mdps;
    int32_t mag_x_nt;
    int32_t mag_y_nt;
    int32_t mag_z_nt;
    int32_t roll_md;
    int32_t pitch_md;
    int32_t yaw_md;
} MPU9250_Snapshot_t;

typedef enum {
    MPU9250_READ_ALL = 0,
    MPU9250_READ_ACCEL,
    MPU9250_READ_GYRO,
    MPU9250_READ_TEMP,
    MPU9250_READ_ORIENTATION
} MPU9250_ReadMode;

typedef enum {
    MPU9250_XFER_IDLE = 0,
    MPU9250_XFER_WAIT_GRANT,
    MPU9250_XFER_TX_REG,
    MPU9250_XFER_RX_DATA
} MPU9250_TransferState;


#endif
