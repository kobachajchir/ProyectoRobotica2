#ifndef AUTO_MOTION_H
#define AUTO_MOTION_H
#include <stdbool.h>
#include <stdint.h>
#define AUTO_MOTION_STATE_BYTES 128u
#define AUTO_MOTION_PROFILE_BYTES 88u
#define AUTO_MOTION_IR_BYTES 186u
#define AUTO_MOTION_POINTS 5u
typedef struct { float percent, speed_cm_s; uint32_t ramp_ms, runs; } AutoMotion_Point;
typedef struct { uint16_t distance_mm, adc; } AutoMotion_IrPoint;
typedef struct { uint16_t baseline, noise; uint8_t count; AutoMotion_IrPoint points[8]; } AutoMotion_IrChannel;
typedef struct {
    uint32_t revision; uint8_t count, ir_valid;
    AutoMotion_Point points[AUTO_MOTION_POINTS];
    AutoMotion_IrChannel ir[5];
} AutoMotion_Profile;
typedef struct {
    uint32_t now_ms, imu_seq;
    float yaw_deg, left_duty, right_duty, pwm_hz;
    uint16_t objects[5];
    uint8_t mode, mpu_ready, mpu_calibrated, armed, balance, settled, safety;
    uint8_t line_ready, line_fresh, center_black, objects_fresh, objects_enabled, lights;
} AutoMotion_Input;
typedef struct {
    void *ctx;
    bool (*read)(void *, AutoMotion_Input *);
    bool (*command)(void *, float, float, uint32_t);
    void (*hold)(void *);
    bool (*save)(void *, const AutoMotion_Profile *);
} AutoMotion_Port;
typedef struct {
    AutoMotion_Port port; AutoMotion_Profile profile;
    AutoMotion_Input input;
    uint8_t kind, state, phase, reason, obstacle_mask, persisted, result_valid, black_candidate;
    uint32_t id, started_ms, last_ms, measure_ms, measured_ms, ramp_ms, timeout_ms;
    uint32_t active_ms[2], forward_ms[2], reverse_ms[2], candidate_ms, last_imu_seq;
    float x, y, goal_x, goal_y, origin_yaw, heading, forward, speed;
    float duty_seconds[2], max_duty[2], cycles[2], yaw_error_max;
    float avoid_heading, avoid_x, avoid_y;
    uint32_t avoid_since, last_progress_ms, drive_elapsed;
    float applied_forward;
    float best_distance;
} AutoMotion_Handle;
enum { AUTO_OK, AUTO_MODE, AUTO_MPU, AUTO_BALANCE, AUTO_MOTOR_PROFILE, AUTO_IR_PROFILE,
       AUTO_LINE, AUTO_BUSY, AUTO_ARGUMENT, AUTO_OBSTACLE, AUTO_TIMEOUT, AUTO_SAVE,
       AUTO_CANCELLED, AUTO_MOTION_REJECTED, AUTO_HEADING };
void AutoMotion_Init(AutoMotion_Handle *, const AutoMotion_Port *, const AutoMotion_Profile *);
bool AutoMotion_Busy(const AutoMotion_Handle *);
void AutoMotion_Cancel(AutoMotion_Handle *);
void AutoMotion_Task(AutoMotion_Handle *);
uint8_t AutoMotion_Goto(AutoMotion_Handle *, float x_cm, float y_cm);
uint8_t AutoMotion_Calibrate(AutoMotion_Handle *, uint8_t operation, float percent, uint32_t ramp_ms, uint32_t timeout_ms);
uint8_t AutoMotion_SetIr(AutoMotion_Handle *, const uint8_t *, uint8_t);
uint8_t AutoMotion_SetProfile(AutoMotion_Handle *, const uint8_t *, uint8_t);
void AutoMotion_State(AutoMotion_Handle *, uint8_t *, uint8_t request_reason);
void AutoMotion_GetProfile(AutoMotion_Handle *, uint8_t *, uint8_t request_reason);
#endif
