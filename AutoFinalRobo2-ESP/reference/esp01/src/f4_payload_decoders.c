#include "f4_payload_decoders.h"

#include <string.h>

typedef char F4_FloatMustBe32Bits[(sizeof(float) == 4u) ? 1 : -1];

static uint16_t read_u16_le(const uint8_t *p)
{
    return (uint16_t)p[0] | ((uint16_t)p[1] << 8);
}

static uint32_t read_u32_le(const uint8_t *p)
{
    return (uint32_t)p[0] |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static float read_f32_le(const uint8_t *p)
{
    uint32_t bits = read_u32_le(p);
    float value;
    memcpy(&value, &bits, sizeof(value));
    return value;
}

static F4_DecodeStatus require_fixed(const uint8_t *payload, size_t len, const void *out, size_t expected)
{
    if (payload == NULL || out == NULL) return F4_DECODE_NULL_ARGUMENT;
    return len == expected ? F4_DECODE_OK : F4_DECODE_LENGTH_MISMATCH;
}

F4_DecodeStatus F4_DecodeMpuSnapshot(const uint8_t *payload, size_t len, F4_MpuSnapshot *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 42u);
    if (status != F4_DECODE_OK) return status;

    memset(out, 0, sizeof(*out));
    out->status = payload[0];
    out->flags = payload[1];
    out->sample_sequence = read_u16_le(&payload[2]);
    out->sample_dt_us = read_u16_le(&payload[4]);
    out->roll_deg = read_f32_le(&payload[6]);
    out->pitch_deg = read_f32_le(&payload[10]);
    out->yaw_magnetic_deg = read_f32_le(&payload[14]);
    out->accel_g[0] = read_f32_le(&payload[18]);
    out->accel_g[1] = read_f32_le(&payload[22]);
    out->accel_g[2] = read_f32_le(&payload[26]);
    out->gyro_deg_s[0] = read_f32_le(&payload[30]);
    out->gyro_deg_s[1] = read_f32_le(&payload[34]);
    out->gyro_deg_s[2] = read_f32_le(&payload[38]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeStreamStateResponse(const uint8_t *payload, size_t len, F4_StreamStateResponse *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 4u);
    if (status != F4_DECODE_OK) return status;
    out->status = payload[0];
    out->active = payload[1];
    out->period_ms = read_u16_le(&payload[2]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeStatusResponse(const uint8_t *payload, size_t len, F4_StatusResponse *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 1u);
    if (status != F4_DECODE_OK) return status;
    out->status = payload[0];
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeSetPreferencesResponse(const uint8_t *payload, size_t len, F4_SetPreferencesResponse *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 5u);
    if (status != F4_DECODE_OK) return status;
    out->status = payload[0];
    out->changed_mask = read_u32_le(&payload[1]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodePreferencesSnapshot(const uint8_t *payload, size_t len, F4_PreferencesSnapshot *out)
{
    size_t metadata_offset;
    if (payload == NULL || out == NULL) return F4_DECODE_NULL_ARGUMENT;
    if (len != 34u && len != 35u && len != 37u)
        return F4_DECODE_LENGTH_MISMATCH;
    if (payload[1] != 0u) return F4_DECODE_FORMAT_ERROR;

    memset(out, 0, sizeof(*out));
    out->status = payload[0];
    out->mode = payload[1];
    out->descriptor_count = payload[2];
    out->field_mask = read_u32_le(&payload[3]);
    out->encoder_inverted = payload[7];
    out->screen_lock_enabled = payload[8];
    out->screen_lock_timeout_sec = read_u16_le(&payload[9]);
    out->left_min_pwm = read_u16_le(&payload[11]);
    out->right_min_pwm = read_u16_le(&payload[13]);
    out->system_action_timeout_sec = read_u16_le(&payload[15]);
    out->usb_alive_period_ms = read_u16_le(&payload[17]);
    out->oled_rotation_degrees = read_u16_le(&payload[19]);
    out->oled_auto_rotation_enabled = payload[21];
    out->uner_router_forwarding_enabled = payload[22];
    out->black_line_width_mm = read_u16_le(&payload[23]);
    out->mpu_calibration_valid = payload[25];

    if (len == 34u) {
        /* Snapshot anterior a pinLockEnabled. No leer payload[26] como bool:
         * ese byte ya pertenece a recordSize. */
        metadata_offset = 26u;
    } else {
        out->has_pin_lock_enabled = 1u;
        out->pin_lock_enabled = payload[26];
        if (len == 37u) {
            out->has_web_alive_period_ms = 1u;
            out->web_alive_period_ms = read_u16_le(&payload[27]);
            metadata_offset = 29u;
        } else {
            metadata_offset = 27u;
        }
    }

    out->record_size = read_u32_le(&payload[metadata_offset]);
    out->record_capacity = read_u32_le(&payload[metadata_offset + 4u]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeTelemetrySnapshot(const uint8_t *payload, size_t len, F4_TelemetrySnapshot *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 92u);
    uint8_t i;
    if (status != F4_DECODE_OK) return status;

    memset(out, 0, sizeof(*out));
    out->status = payload[0];
    out->backend = payload[1];
    out->wifi_mode = payload[2];
    out->bridge_connected = payload[3];
    out->capability_mask = read_u32_le(&payload[4]);
    out->i2c_policy = payload[8];
    out->motion_armed = payload[9];
    out->balance_active = payload[10];
    out->command_active = payload[11];
    out->safety_stop = payload[12];
    out->pitch_measured_deg = read_f32_le(&payload[13]);
    out->pitch_setpoint_deg = read_f32_le(&payload[17]);
    out->balance_output_percent = read_f32_le(&payload[21]);
    out->left_output_percent = read_f32_le(&payload[25]);
    out->right_output_percent = read_f32_le(&payload[29]);
    out->kp = read_f32_le(&payload[33]);
    out->ki = read_f32_le(&payload[37]);
    out->kd = read_f32_le(&payload[41]);
    out->roll_deg = read_f32_le(&payload[45]);
    out->pitch_deg = read_f32_le(&payload[49]);
    out->yaw_magnetic_deg = read_f32_le(&payload[53]);
    out->mpu_stream_active = payload[57];
    out->mpu_stream_period_ms = read_u16_le(&payload[58]);
    out->left_min_pwm = read_u16_le(&payload[60]);
    out->right_min_pwm = read_u16_le(&payload[62]);
    for (i = 0u; i < 8u; ++i) out->raw_adc[i] = read_u16_le(&payload[64u + (2u * i)]);
    out->pid_error_deg = read_f32_le(&payload[80]);
    out->pid_integral_deg_s = read_f32_le(&payload[84]);
    out->pid_derivative_deg_s = read_f32_le(&payload[88]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeBalancePidResponse(const uint8_t *payload, size_t len, F4_BalancePidResponse *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 13u);
    if (status != F4_DECODE_OK) return status;
    out->status = payload[0];
    out->kp = read_f32_le(&payload[1]);
    out->ki = read_f32_le(&payload[5]);
    out->kd = read_f32_le(&payload[9]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeSetBalancePidResponse(const uint8_t *payload, size_t len, F4_SetBalancePidResponse *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 14u);
    if (status != F4_DECODE_OK) return status;
    out->status = payload[0];
    out->changed_mask = payload[1];
    out->kp = read_f32_le(&payload[2]);
    out->ki = read_f32_le(&payload[6]);
    out->kd = read_f32_le(&payload[10]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeControlSnapshot(const uint8_t *payload, size_t len, F4_ControlSnapshot *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 64u);
    if (status != F4_DECODE_OK) return status;

    memset(out, 0, sizeof(*out));
    out->status = payload[0];
    out->car_mode = payload[1];
    out->line_follow_fsm = payload[2];
    out->stable_line_pattern = payload[3];
    out->line_confidence_percent = payload[4];
    out->line_ambiguous = payload[5];
    out->line_reject_reason = payload[6];
    out->line_width_mm = read_u16_le(&payload[7]);
    out->lateral_error_mm = read_f32_le(&payload[9]);
    out->normalized_lateral_error = read_f32_le(&payload[13]);
    out->linear_acceleration_m_s2 = read_f32_le(&payload[17]);
    out->estimated_velocity_m_s = read_f32_le(&payload[21]);
    out->yaw_rate_deg_s = read_f32_le(&payload[25]);
    out->estimator_confidence_percent = payload[29];
    out->estimator_flags = payload[30];
    out->common_motion_output_percent = read_f32_le(&payload[31]);
    out->differential_motion_output_percent = read_f32_le(&payload[35]);
    out->turn_limit_percent = read_f32_le(&payload[39]);
    out->saturation_limited = payload[43];
    out->usb_active = payload[44];
    out->usb_alive_period_ms = read_u16_le(&payload[45]);
    out->usb_alive_accepted_count = read_u32_le(&payload[47]);
    out->usb_rx_overflow_count = read_u32_le(&payload[51]);
    out->usb_tx_queue_full_count = read_u32_le(&payload[55]);
    out->usb_queued_tx_count = payload[59];
    out->usb_tx_drop_count = read_u32_le(&payload[60]);
    return F4_DECODE_OK;
}

static F4_DecodeStatus decode_text_field(const uint8_t *payload,
                                         size_t len,
                                         size_t *offset,
                                         char *dst,
                                         size_t dst_capacity,
                                         uint8_t *decoded_len)
{
    uint8_t field_len;
    if (*offset >= len) return F4_DECODE_FORMAT_ERROR;
    field_len = payload[(*offset)++];
    if ((size_t)field_len + 1u > dst_capacity || *offset + field_len > len) return F4_DECODE_FORMAT_ERROR;
    if (field_len > 0u) memcpy(dst, &payload[*offset], field_len);
    dst[field_len] = '\0';
    *offset += field_len;
    *decoded_len = field_len;
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeFirmwareInfo(const uint8_t *payload, size_t len, F4_FirmwareInfo *out)
{
    F4_DecodeStatus status;
    size_t offset = 1u;
    if (payload == NULL || out == NULL) return F4_DECODE_NULL_ARGUMENT;
    if (len < 1u || len > 196u) return F4_DECODE_LENGTH_MISMATCH;
    memset(out, 0, sizeof(*out));
    out->status = payload[0];
    if (len == 1u) return out->status == 0u ? F4_DECODE_FORMAT_ERROR : F4_DECODE_OK;
    status = decode_text_field(payload, len, &offset, out->name, sizeof(out->name), &out->name_len);
    if (status != F4_DECODE_OK) return status;
    status = decode_text_field(payload, len, &offset, out->version, sizeof(out->version), &out->version_len);
    if (status != F4_DECODE_OK) return status;
    status = decode_text_field(payload, len, &offset, out->target, sizeof(out->target), &out->target_len);
    if (status != F4_DECODE_OK) return status;
    return offset == len ? F4_DECODE_OK : F4_DECODE_FORMAT_ERROR;
}

F4_DecodeStatus F4_DecodeBuildInfo(const uint8_t *payload, size_t len, F4_BuildInfo *out)
{
    F4_DecodeStatus status;
    size_t offset = 1u;
    if (payload == NULL || out == NULL) return F4_DECODE_NULL_ARGUMENT;
    if (len < 1u || len > 149u) return F4_DECODE_LENGTH_MISMATCH;
    memset(out, 0, sizeof(*out));
    out->status = payload[0];
    if (len == 1u) return out->status == 0u ? F4_DECODE_FORMAT_ERROR : F4_DECODE_OK;
    status = decode_text_field(payload, len, &offset, out->date, sizeof(out->date), &out->date_len);
    if (status != F4_DECODE_OK) return status;
    status = decode_text_field(payload, len, &offset, out->time, sizeof(out->time), &out->time_len);
    if (status != F4_DECODE_OK) return status;
    status = decode_text_field(payload, len, &offset, out->profile, sizeof(out->profile), &out->profile_len);
    if (status != F4_DECODE_OK) return status;
    status = decode_text_field(payload, len, &offset, out->compiler, sizeof(out->compiler), &out->compiler_len);
    if (status != F4_DECODE_OK) return status;
    return offset == len ? F4_DECODE_OK : F4_DECODE_FORMAT_ERROR;
}

F4_DecodeStatus F4_DecodeBootReport(const uint8_t *payload, size_t len, F4_BootReport *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, F4_BOOT_REPORT_PAYLOAD_SIZE);
    if (status != F4_DECODE_OK) return status;
    if (payload[0] != F4_BOOT_REPORT_SCHEMA || payload[1] > 1u ||
        payload[2] > 3u || payload[5] > 1u) return F4_DECODE_FORMAT_ERROR;

    memset(out, 0, sizeof(*out));
    out->schema = payload[0];
    out->status = payload[1];
    out->handoff = payload[2];
    out->reset_flags = payload[3];
    out->app_validation_flags = payload[4];
    out->extension_profile = payload[5];
    out->bootloader_version = read_u32_le(&payload[6]);
    out->app_version = read_u32_le(&payload[10]);
    out->app_size = read_u32_le(&payload[14]);
    out->app_crc32 = read_u32_le(&payload[18]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeIrSnapshot(const uint8_t *payload, size_t len, F4_IrSnapshot *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 56u);
    uint8_t i;
    if (status != F4_DECODE_OK) return status;

    memset(out, 0, sizeof(*out));
    out->status = payload[0];
    out->flags = payload[1];
    out->sample_sequence = read_u16_le(&payload[2]);
    out->stream_period_ms = read_u16_le(&payload[4]);
    out->hal_tick_ms = read_u32_le(&payload[6]);
    for (i = 0u; i < 8u; ++i) out->raw_adc[i] = read_u16_le(&payload[10u + (2u * i)]);
    for (i = 0u; i < 8u; ++i) out->normalized[i] = read_u16_le(&payload[26u + (2u * i)]);
    out->line_pattern = payload[42];
    out->route_alignment = payload[43];
    out->confidence_percent = payload[44];
    out->ambiguous = payload[45];
    out->line_width_mm = read_u16_le(&payload[46]);
    out->lateral_error_mm = read_f32_le(&payload[48]);
    out->normalized_lateral_error = read_f32_le(&payload[52]);
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeGetApConfigResponse(const uint8_t *payload, size_t len, F4_GetApConfigResponse *out)
{
    if (payload == NULL || out == NULL) return F4_DECODE_NULL_ARGUMENT;
    if (len < 10u || len > 41u) return F4_DECODE_LENGTH_MISMATCH;

    memset(out, 0, sizeof(*out));
    out->status = payload[0];
    out->apply_state = payload[1];
    out->ap_protected = payload[2];
    out->password_set = payload[3];
    out->ssid_len = payload[4];

    if (out->apply_state > 3u) return F4_DECODE_FORMAT_ERROR;
    if (out->ap_protected > 1u) return F4_DECODE_FORMAT_ERROR;
    if (out->password_set > 1u) return F4_DECODE_FORMAT_ERROR;
    if (out->ssid_len == 0u || out->ssid_len > 32u) return F4_DECODE_FORMAT_ERROR;
    if ((size_t)(out->ssid_len + 9u) != len) return F4_DECODE_LENGTH_MISMATCH;

    memcpy(out->ssid, &payload[5], out->ssid_len);
    out->ssid[out->ssid_len] = '\0';

    /* Validate SSID: only printable ASCII 0x20..0x7E */
    for (uint8_t i = 0u; i < out->ssid_len; ++i) {
        uint8_t ch = (uint8_t)out->ssid[i];
        if (ch < 0x20u || ch > 0x7Eu) return F4_DECODE_FORMAT_ERROR;
    }

    size_t ip_offset = 5u + out->ssid_len;
    out->ip[0] = payload[ip_offset];
    out->ip[1] = payload[ip_offset + 1u];
    out->ip[2] = payload[ip_offset + 2u];
    out->ip[3] = payload[ip_offset + 3u];

    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeSetApConfigResponse(const uint8_t *payload, size_t len, F4_SetApConfigResponse *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 3u);
    if (status != F4_DECODE_OK) return status;

    out->status = payload[0];
    out->changed_mask = payload[1];
    out->apply_state = payload[2];

    if (out->changed_mask > 0x0Fu) return F4_DECODE_FORMAT_ERROR;
    if (out->apply_state > 3u) return F4_DECODE_FORMAT_ERROR;

    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeIrEmitters(const uint8_t *payload, size_t len, F4_IrEmitters *out)
{
    F4_DecodeStatus status = require_fixed(payload, len, out, 3u);
    if (status != F4_DECODE_OK) return status;
    if (payload[0] > 1u || payload[1] > 1u || payload[2] > 1u) return F4_DECODE_FORMAT_ERROR;
    out->objects = payload[0]; out->line = payload[1]; out->lights = payload[2];
    return F4_DECODE_OK;
}

F4_DecodeStatus F4_DecodeIrEmittersResult(const uint8_t *payload, size_t len,
                                        uint8_t *status, F4_IrEmitters *out)
{
    F4_DecodeStatus result = require_fixed(payload, len, out, 4u);
    if (result != F4_DECODE_OK) return result;
    if (status == NULL) return F4_DECODE_NULL_ARGUMENT;
    result = F4_DecodeIrEmitters(payload + 1u, 3u, out);
    if (result == F4_DECODE_OK) *status = payload[0];
    return result;
}
