#ifndef LINE_FOLLOW_CONTROL_TYPES_H
#define LINE_FOLLOW_CONTROL_TYPES_H
#include "am_port.h"
#include "line_calibration.h"
#include <stdbool.h>
#include <stdint.h>

#define LINE_FOLLOW_DEFAULT_FORWARD_PERCENT 12.0f
#define LINE_FOLLOW_RECOVER_TIMEOUT_MS 700u
#define LINE_FOLLOW_MAX_TURN_PERCENT 90.0f

/* Existing FSM numbers remain stable in GET_CONTROL_SNAPSHOT. */
typedef enum { LINE_FOLLOW_SEARCH = 0, LINE_FOLLOW_TRACK, LINE_FOLLOW_INTERSECTION,
    LINE_FOLLOW_CROSS_COMMIT, LINE_FOLLOW_RECOVER, LINE_FOLLOW_FAULT } LineFollow_Fsm_t;
typedef enum {
    LINE_FOLLOW_REJECT_NONE = 0, LINE_FOLLOW_REJECT_DISABLED,
    LINE_FOLLOW_REJECT_NOT_READY, LINE_FOLLOW_REJECT_DISARMED,
    LINE_FOLLOW_REJECT_UNSAFE_PITCH, LINE_FOLLOW_REJECT_BAD_WIDTH,
    LINE_FOLLOW_REJECT_CALIBRATION, LINE_FOLLOW_REJECT_IR_STALE,
    LINE_FOLLOW_REJECT_AMBIGUOUS, LINE_FOLLOW_REJECT_LOST,
    LINE_FOLLOW_REJECT_MOTION, LINE_FOLLOW_REJECT_MODE,
    LINE_FOLLOW_REJECT_NOT_SETTLED, LINE_FOLLOW_REJECT_BUSY,
    LINE_FOLLOW_REJECT_NOT_CENTERED, LINE_FOLLOW_REJECT_LIGHTS,
    LINE_FOLLOW_REJECT_UNSTABLE, LINE_FOLLOW_REJECT_QUALITY,
    LINE_FOLLOW_REJECT_STORAGE, LINE_FOLLOW_REJECT_SESSION,
    LINE_FOLLOW_REJECT_TIMEOUT
} LineFollow_RejectReason_t;
typedef struct {
    uint16_t leftRaw, centerRaw, rightRaw, leftNorm, centerNorm, rightNorm;
    float leftDarkness, centerDarkness, rightDarkness;
    float lateralErrorMm, lateralErrorNorm, centeredMarginMm;
    uint16_t lineWidthMm;
    uint8_t pattern, confidence, alignment;
    bool ambiguous, leftBlack, centerBlack, rightBlack, leftWhite, rightWhite;
} LineFollow_Route;
typedef struct {
    uint16_t black_width_mm;
    float sensor_position_mm[3], centered_margin_mm;
} LineFollow_Geometry;
typedef struct {
    LineFollow_Route route;
    uint32_t ir_seq, ir_acquired_ms, imu_acquired_ms;
    uint16_t imu_seq;
    float pitch_deg, roll_deg, yaw_deg, yaw_rate_ccw_dps;
    float pitch_error_deg, roll_deviation_deg;
    uint8_t imu_valid, yaw_valid, ir_valid, illumination_settling;
    uint8_t armed, balance_active, mode_follow, calibrated;
} LineFollow_Input;
typedef struct {
    void *ctx;
    bool (*read)(void *, LineFollow_Input *);
    bool (*command)(void *, float forward, float turn_ccw, uint16_t lease_ms, uint32_t now_ms);
    void (*hold)(void *);
    LineFollow_Geometry line_config;
    float forward_percent, recover_turn_percent, max_turn_percent;
    float kp, ki, kd, yaw_rate_damping;
    uint16_t recover_timeout_ms;
    uint8_t enabled;
} LineFollow_Config_t;
typedef struct {
    LineFollow_Fsm_t fsm;
    LineFollow_RejectReason_t reject_reason;
    LineFollow_Route sample;
    uint8_t stable_pattern, candidate_pattern, candidate_count;
    uint8_t confidence_percent, ambiguous, sensors_valid;
    uint16_t line_width_mm;
    float lateral_error_mm, lateral_error_norm, forward_percent, turn_percent;
    float yaw_rate_dps, last_track_error_mm;
    uint32_t state_started_ms, last_update_ms;
} LineFollow_Status_t;
typedef struct {
    LineFollow_Config_t config;
    LineFollow_Status_t status;
    float integral, previous_error, filtered_derivative;
    uint32_t last_ir_seq, candidate_ms, unstable_ms, ambiguous_ms;
    uint8_t initialized, candidate_valid, unstable, ambiguity_pending;
} LineFollowControl_Handle;

bool LineFollow_Evaluate(const LineCalibration_Store *, const uint16_t raw[3], bool lights,
                        const LineFollow_Geometry *, LineFollow_Route *);
#endif
