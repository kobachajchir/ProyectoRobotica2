#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define F4_FIRMWARE_TEXT_MAX 64u
#define F4_BUILD_SHORT_TEXT_MAX 16u
#define F4_COMPILER_TEXT_MAX 96u
#define F4_BOOT_REPORT_SCHEMA 1u
#define F4_BOOT_REPORT_PAYLOAD_SIZE 22u

#define F4_MPU_FLAG_SAMPLE_VALID (1u << 0)
#define F4_MPU_FLAG_CALIBRATED   (1u << 1)
#define F4_MPU_FLAG_MAG_VALID    (1u << 2)
#define F4_MPU_FLAG_STATIONARY   (1u << 3)
#define F4_MPU_FLAG_CALIBRATING  (1u << 4)
#define F4_MPU_FLAG_ERROR        (1u << 5)

typedef enum {
    F4_DECODE_OK = 0,
    F4_DECODE_NULL_ARGUMENT,
    F4_DECODE_LENGTH_MISMATCH,
    F4_DECODE_FORMAT_ERROR,
} F4_DecodeStatus;

typedef struct {
    uint8_t status;
    uint8_t flags;
    uint16_t sample_sequence;
    uint16_t sample_dt_us;
    float roll_deg;
    float pitch_deg;
    float yaw_magnetic_deg;
    float accel_g[3];
    float gyro_deg_s[3];
} F4_MpuSnapshot;

typedef struct {
    uint8_t status;
    uint8_t active;
    uint16_t period_ms;
} F4_StreamStateResponse;

typedef struct {
    uint8_t status;
} F4_StatusResponse;

typedef struct {
    uint8_t status;
    uint32_t changed_mask;
} F4_SetPreferencesResponse;

typedef struct {
    uint8_t status;
    uint8_t mode;
    uint8_t descriptor_count;
    uint32_t field_mask;
    uint8_t encoder_inverted;
    uint8_t screen_lock_enabled;
    uint16_t screen_lock_timeout_sec;
    uint16_t left_min_pwm;
    uint16_t right_min_pwm;
    uint16_t system_action_timeout_sec;
    uint16_t usb_alive_period_ms;
    uint16_t oled_rotation_degrees;
    uint8_t oled_auto_rotation_enabled;
    uint8_t uner_router_forwarding_enabled;
    uint16_t black_line_width_mm;
    uint8_t mpu_calibration_valid;
    uint8_t has_pin_lock_enabled;
    uint8_t pin_lock_enabled;
    uint8_t has_web_alive_period_ms;
    uint16_t web_alive_period_ms;
    uint32_t record_size;
    uint32_t record_capacity;
} F4_PreferencesSnapshot;

typedef struct {
    uint8_t status;
    uint8_t backend;
    uint8_t wifi_mode;
    uint8_t bridge_connected;
    uint32_t capability_mask;
    uint8_t i2c_policy;
    uint8_t motion_armed;
    uint8_t balance_active;
    uint8_t command_active;
    uint8_t safety_stop;
    float pitch_measured_deg;
    float pitch_setpoint_deg;
    float balance_output_percent;
    float left_output_percent;
    float right_output_percent;
    float kp;
    float ki;
    float kd;
    float roll_deg;
    float pitch_deg;
    float yaw_magnetic_deg;
    uint8_t mpu_stream_active;
    uint16_t mpu_stream_period_ms;
    uint16_t left_min_pwm;
    uint16_t right_min_pwm;
    uint16_t raw_adc[8];
    float pid_error_deg;
    float pid_integral_deg_s;
    float pid_derivative_deg_s;
} F4_TelemetrySnapshot;

typedef struct {
    uint8_t status;
    float kp;
    float ki;
    float kd;
} F4_BalancePidResponse;

typedef struct {
    uint8_t status;
    uint8_t changed_mask;
    float kp;
    float ki;
    float kd;
} F4_SetBalancePidResponse;

typedef struct {
    uint8_t status;
    uint8_t car_mode;
    uint8_t line_follow_fsm;
    uint8_t stable_line_pattern;
    uint8_t line_confidence_percent;
    uint8_t line_ambiguous;
    uint8_t line_reject_reason;
    uint16_t line_width_mm;
    float lateral_error_mm;
    float normalized_lateral_error;
    float linear_acceleration_m_s2;
    float estimated_velocity_m_s;
    float yaw_rate_deg_s;
    uint8_t estimator_confidence_percent;
    uint8_t estimator_flags;
    float common_motion_output_percent;
    float differential_motion_output_percent;
    float turn_limit_percent;
    uint8_t saturation_limited;
    uint8_t usb_active;
    uint16_t usb_alive_period_ms;
    uint32_t usb_alive_accepted_count;
    uint32_t usb_rx_overflow_count;
    uint32_t usb_tx_queue_full_count;
    uint8_t usb_queued_tx_count;
    uint32_t usb_tx_drop_count;
} F4_ControlSnapshot;

typedef struct {
    uint8_t status;
    uint8_t name_len;
    uint8_t version_len;
    uint8_t target_len;
    char name[F4_FIRMWARE_TEXT_MAX + 1u];
    char version[F4_FIRMWARE_TEXT_MAX + 1u];
    char target[F4_FIRMWARE_TEXT_MAX + 1u];
} F4_FirmwareInfo;

typedef struct {
    uint8_t status;
    uint8_t date_len;
    uint8_t time_len;
    uint8_t profile_len;
    uint8_t compiler_len;
    char date[F4_BUILD_SHORT_TEXT_MAX + 1u];
    char time[F4_BUILD_SHORT_TEXT_MAX + 1u];
    char profile[F4_BUILD_SHORT_TEXT_MAX + 1u];
    char compiler[F4_COMPILER_TEXT_MAX + 1u];
} F4_BuildInfo;

typedef struct {
    uint8_t schema;
    uint8_t status;
    uint8_t handoff;
    uint8_t reset_flags;
    uint8_t app_validation_flags;
    uint8_t extension_profile;
    uint32_t bootloader_version;
    uint32_t app_version;
    uint32_t app_size;
    uint32_t app_crc32;
} F4_BootReport;

typedef struct {
    uint8_t status;
    uint8_t flags;
    uint16_t sample_sequence;
    uint16_t stream_period_ms;
    uint32_t hal_tick_ms;
    uint16_t raw_adc[8];
    uint16_t normalized[8];
    uint8_t line_pattern;
    uint8_t route_alignment;
    uint8_t confidence_percent;
    uint8_t ambiguous;
    uint16_t line_width_mm;
    float lateral_error_mm;
    float normalized_lateral_error;
} F4_IrSnapshot;

/* 0x60 response and 0x90 event, exactly 42 bytes. */
F4_DecodeStatus F4_DecodeMpuSnapshot(const uint8_t *payload, size_t len, F4_MpuSnapshot *out);
/* 0x61 and 0x6B responses, exactly 4 bytes. */
F4_DecodeStatus F4_DecodeStreamStateResponse(const uint8_t *payload, size_t len, F4_StreamStateResponse *out);
/* 0x62 and 0x6C responses, exactly 1 byte. */
F4_DecodeStatus F4_DecodeStatusResponse(const uint8_t *payload, size_t len, F4_StatusResponse *out);
/* 0x63 response, exactly 5 bytes. */
F4_DecodeStatus F4_DecodeSetPreferencesResponse(const uint8_t *payload, size_t len, F4_SetPreferencesResponse *out);
/* 0x40 mode-0 snapshot. Accepts legacy 34, current 35 and Web-alive 37 byte layouts. */
F4_DecodeStatus F4_DecodePreferencesSnapshot(const uint8_t *payload, size_t len, F4_PreferencesSnapshot *out);
/* 0x64 response, exactly 92 bytes. */
F4_DecodeStatus F4_DecodeTelemetrySnapshot(const uint8_t *payload, size_t len, F4_TelemetrySnapshot *out);
/* 0x65 response, exactly 13 bytes. */
F4_DecodeStatus F4_DecodeBalancePidResponse(const uint8_t *payload, size_t len, F4_BalancePidResponse *out);
/* 0x66 response, exactly 14 bytes. */
F4_DecodeStatus F4_DecodeSetBalancePidResponse(const uint8_t *payload, size_t len, F4_SetBalancePidResponse *out);
/* 0x67 response, exactly 64 bytes. */
F4_DecodeStatus F4_DecodeControlSnapshot(const uint8_t *payload, size_t len, F4_ControlSnapshot *out);
/* 0x68 response. A non-zero status may be represented by a one-byte response. */
F4_DecodeStatus F4_DecodeFirmwareInfo(const uint8_t *payload, size_t len, F4_FirmwareInfo *out);
/* 0x69 response. A non-zero status may be represented by a one-byte response. */
F4_DecodeStatus F4_DecodeBuildInfo(const uint8_t *payload, size_t len, F4_BuildInfo *out);
/* 0x6F response and 0x9F event, exactly 22 bytes. */
F4_DecodeStatus F4_DecodeBootReport(const uint8_t *payload, size_t len, F4_BootReport *out);
/* 0x6A response and 0x91 event, exactly 56 bytes. */
F4_DecodeStatus F4_DecodeIrSnapshot(const uint8_t *payload, size_t len, F4_IrSnapshot *out);

/* 0x77 GET_AP_CONFIG response. Variable length 10..41 bytes:
 * [status, applyState, protected, passwordSet, ssidLen, ssid..., ip[4]] */
typedef struct {
    uint8_t status;
    uint8_t apply_state;     /* AP_APPLY_STATE_* */
    uint8_t ap_protected;    /* 0 or 1 */
    uint8_t password_set;    /* 0 or 1 */
    uint8_t ssid_len;
    char    ssid[33];        /* SSID_MAX_LEN + 1 */
    uint8_t ip[4];
} F4_GetApConfigResponse;

/* 0x78 SET_AP_CONFIG_F4 response. Exactly 3 bytes:
 * [status, changedMask, applyState] */
typedef struct {
    uint8_t status;
    uint8_t changed_mask;    /* AP_FIELD_* bitmask, max 0x0F */
    uint8_t apply_state;     /* AP_APPLY_STATE_* */
} F4_SetApConfigResponse;

/* 0x77 response, variable 10..41 bytes. */
F4_DecodeStatus F4_DecodeGetApConfigResponse(const uint8_t *payload, size_t len, F4_GetApConfigResponse *out);
/* 0x78 response, exactly 3 bytes. */
F4_DecodeStatus F4_DecodeSetApConfigResponse(const uint8_t *payload, size_t len, F4_SetApConfigResponse *out);

/* 0x79/0x98: three state bytes; 0x7A: status then three state bytes. */
typedef struct { uint8_t objects; uint8_t line; uint8_t lights; } F4_IrEmitters;
F4_DecodeStatus F4_DecodeIrEmitters(const uint8_t *payload, size_t len, F4_IrEmitters *out);
F4_DecodeStatus F4_DecodeIrEmittersResult(const uint8_t *payload, size_t len,
                                        uint8_t *status, F4_IrEmitters *out);

#ifdef __cplusplus
}
#endif
