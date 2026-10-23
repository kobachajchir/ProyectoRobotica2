#ifndef LINE_CALIBRATION_H
#define LINE_CALIBRATION_H
#include <stdint.h>
#include <stdbool.h>

#define LINE_CAL_PHASES 4u
#define LINE_CAL_SENSORS 3u
#define LINE_CAL_WINDOWS 32u
#define LINE_CAL_WINDOW_SCANS 40u
#define LINE_CAL_USEFUL_SCANS 1280u

/* Independent pose records. Raw ADC values preserve the hardware polarity. */
typedef struct {
    uint16_t mean[LINE_CAL_PHASES][LINE_CAL_SENSORS];
    uint16_t noise[LINE_CAL_PHASES][LINE_CAL_SENSORS];
    float pitch_deg, roll_deg, yaw_deg;
    uint32_t useful_scans;
    uint8_t valid;
    uint8_t reserved[3];
} LineCalibration_Profile;
typedef struct {
    uint32_t white_revision, track_white_revision;
    LineCalibration_Profile white, track;
} LineCalibration_Store;

typedef enum {
    LINE_CAL_IDLE = 0, LINE_CAL_PROMPT, LINE_CAL_STILL,
    LINE_CAL_CAPTURE, LINE_CAL_SAVING, LINE_CAL_DONE,
    LINE_CAL_FAILED, LINE_CAL_CANCELLED
} LineCalibration_State;
typedef struct {
    float pitch_deg, roll_deg, yaw_deg, gyro_max_dps;
    uint32_t acquired_ms;
    uint16_t seq;
    uint8_t valid;
} LineCalibration_Pose;
typedef struct {
    void *ctx;
    void (*lights)(void *, bool, bool);
    void (*restore)(void *);
    bool (*save)(void *, const LineCalibration_Store *);
} LineCalibration_Port;
typedef struct {
    LineCalibration_Store store;
    LineCalibration_Profile pending;
    LineCalibration_Port port;
    LineCalibration_Pose reference, still_reference;
    volatile LineCalibration_State state;
    uint32_t session_id, started_ms, still_ms, phase_ms, last_adc_seq;
    uint16_t last_pose_seq;
    uint8_t target, phase, reason, progress;
    volatile uint16_t scans;
    volatile uint32_t sum[3];
    volatile uint64_t squares[3];
    volatile uint32_t window_sum[3];
    volatile uint16_t windows[32][3];
} LineCalibration_Handle;

void LineCalibration_Init(LineCalibration_Handle *, const LineCalibration_Port *, const LineCalibration_Store *);
bool LineCalibration_Ready(const LineCalibration_Store *);
bool LineCalibration_Busy(const LineCalibration_Handle *);
uint8_t LineCalibration_Control(LineCalibration_Handle *, uint8_t operation, uint32_t session, uint32_t now_ms);
void LineCalibration_Task(LineCalibration_Handle *, const LineCalibration_Pose *, uint32_t now_ms);
/* Called once per completed DMA scan; bounded integer accumulation only. */
void LineCalibration_PushScan(LineCalibration_Handle *, const uint16_t raw[3], uint32_t seq, uint32_t now_ms);

#endif
