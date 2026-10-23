#ifndef MOTION_CONTROL_DATA_H
#define MOTION_CONTROL_DATA_H
#include "am_port.h"
#define MOTION_CONTROL_MAX_PITCH_OFFSET_DEG_DEFAULT    6.0f
#define MOTION_CONTROL_MAX_TURN_PERCENT_DEFAULT        25.0f
#define MOTION_CONTROL_SAFE_PITCH_DEG_DEFAULT          35.0f
#define MOTION_CONTROL_ARM_PITCH_DEG                    10.0f
#define MOTION_CONTROL_TURN_HEADROOM_RATIO_DEFAULT      1.0f
#define MOTION_CONTROL_COMMAND_NONE_DURATION_MS        0u

typedef enum {
    MOTION_SOURCE_NONE = 0,
    MOTION_SOURCE_BALANCE,
    MOTION_SOURCE_FOLLOW,
    MOTION_SOURCE_WEB,
    MOTION_SOURCE_NRF,
    MOTION_SOURCE_UI_TEST,
    MOTION_SOURCE_SAFETY,
    MOTION_SOURCE_GOTO
} MotionControl_Source_t;

typedef enum {
    MOTION_STOP_COAST = 0,
    MOTION_STOP_BRAKE
} MotionControl_StopMode_t;

typedef enum {
    MOTION_REJECT_NONE = 0,
    MOTION_REJECT_NOT_INITIALIZED,
    MOTION_REJECT_MPU_NOT_READY,
    MOTION_REJECT_MPU_CALIBRATING,
    MOTION_REJECT_TILT_LIMIT,
    MOTION_REJECT_DISABLED,
    MOTION_REJECT_DISARMED,
    MOTION_REJECT_MPU_STALE,
    MOTION_REJECT_SAFETY_LATCHED,
    MOTION_REJECT_INVALID_COMMAND
} MotionControl_RejectReason_t;

typedef struct {
    MotionControl_Source_t source;
    float forward_percent;
    float turn_percent;
    uint16_t duration_ms;
    uint16_t ramp_up_ms;
    uint16_t ramp_down_ms;
    MotionControl_StopMode_t end_mode;
} MotionControl_Command_t;

struct AM_DriveOps; struct AM_BalanceOps; struct AM_OrientationOps;
typedef struct {
    void *motors;
    void *balance;
    void *mpu;
    const struct AM_DriveOps *drive_ops;
    const struct AM_BalanceOps *balance_ops;
    const struct AM_OrientationOps *orientation;
    float neutral_pitch_deg;
    float max_pitch_offset_deg;
    float max_turn_percent;
    float safe_pitch_deg;
    float turn_headroom_ratio;
    uint8_t enabled;
} MotionControl_Config_t;

typedef struct {
    MotionControl_Source_t source;
    MotionControl_RejectReason_t reject_reason;
    float pitch_measured_deg;
    float pitch_setpoint_deg;
    float balance_output_percent;
    float forward_applied_percent;
    float turn_applied_percent;
    float turn_requested_percent;
    float turn_limit_percent;
    float u_common_percent;
    float u_diff_percent;
    float left_output_percent;
    float right_output_percent;
    uint32_t command_started_ms;
    uint8_t command_active;
    uint8_t safety_stop;
    uint8_t balance_active;
    uint8_t balance_settled;
    uint8_t armed;
    uint8_t saturation_limited;
} MotionControl_Status_t;


#endif
