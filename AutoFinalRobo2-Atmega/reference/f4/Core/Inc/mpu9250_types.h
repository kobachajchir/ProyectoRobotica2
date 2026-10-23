/*
 * mpu9250.h
 *
 * MPU9250 + AK8963 9-axis driver for the STM32F411 port.
 */

#ifndef INC_MPU9250_H_DATA
#define INC_MPU9250_H_DATA

#include "am_port.h"
#include "i2c_port.h"
#include "types/bitmap_type.h"
#include "utils/macros_utils.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DATA_READY        BIT0_MASK
#define TX_SENT           BIT1_MASK
#define CALIBRATED        BIT2_MASK
#include "mpu9250_data.h"
typedef struct {
    const AM_I2C_DevicePort *bus;
    uint8_t i2c_address;
    uint8_t tx_buffer[1];
    uint8_t rx_buffer[22];
    uint8_t expected_rx_len;
    MPU9250_ReadMode read_mode;
    MPU9250_ReadMode last_read_mode;
    MPU9250_TransferState transfer_state;
    Byte_Flag_Struct flags;
    MPU9250_IntData_t data;
    MPU9250_Snapshot_t latest;
    MPU9250_Callback on_data_ready_cb;
    uint16_t dt_div;
    volatile bool *trigger;
    int32_t gyro_bias_x, gyro_bias_y, gyro_bias_z;
    int32_t calib_sum_x, calib_sum_y, calib_sum_z;
    int32_t calib_accel_sum_x, calib_accel_sum_y, calib_accel_sum_z;
    int32_t angle_x_md, angle_y_md, angle_z_md;
    int32_t rem_x, rem_y, rem_z;
    float gyro_bias_x_dps, gyro_bias_y_dps, gyro_bias_z_dps;
    float angle_x_deg, angle_y_deg, angle_z_deg;
    float gravity_x_mg_f, gravity_y_mg_f, gravity_z_mg_f;
    float roll_ref_deg, pitch_ref_deg;
    int16_t gravity_x_mg, gravity_y_mg, gravity_z_mg;
    int16_t prev_accel_x_mg, prev_accel_y_mg, prev_accel_z_mg;
    int32_t roll_ref_md, pitch_ref_md;
    MPU9250_AttitudeState_t attitude;
    MPU9250_MagCalibration_t mag_calibration;
    uint32_t calib_started_ms;
    uint32_t last_sample_time_us;
    uint32_t last_attitude_ms;
    uint32_t sample_acquired_ms;
    uint16_t calib_count, calib_target;
    uint16_t calib_warmup_ms, calib_window_ms;
    uint8_t prev_accel_valid;
} MPU9250_Handle_t;


#ifdef __cplusplus
}
#endif
#endif
