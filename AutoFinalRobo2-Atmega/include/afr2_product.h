/* AFR2-20261023-19. Own AVR product services. No F4 System/HAL/MPU. */
#ifndef AFR2_PRODUCT_H
#define AFR2_PRODUCT_H
#include <stdint.h>
#include <stdbool.h>
#include "afr2_link.h"
#include "afr2_sequence.h"
#include "afr2_sensor_head.h"
#define AFR2_CONFIG_VERSION 1u
#define AFR2_NOTICE_MS 3000u
enum { AFR2_IDLE, AFR2_MANUAL, AFR2_LINE, AFR2_WHEEL_TEST, AFR2_GOTO, AFR2_FAULT, AFR2_SEQUENCE };
enum { AFR2_SOURCE_NONE, AFR2_SOURCE_LOCAL, AFR2_SOURCE_WEB };
enum { AFR2_OK, AFR2_BAD, AFR2_UNAVAILABLE, AFR2_DENIED, AFR2_BUSY, AFR2_FAILED };
enum { AFR2_WIFI_STATUS=1, AFR2_WIFI_SCAN, AFR2_WIFI_WEB_DRAFT,
       AFR2_WIFI_LOCAL_DRAFT, AFR2_WIFI_CONNECT, AFR2_WIFI_CANCEL, AFR2_WIFI_RETRY, AFR2_WIFI_REBOOT_ESP };
typedef struct {
    uint8_t line_kind, line_invert, line_calibrated, motor_signs, motor_ready;
    uint8_t hall_wired, hall_signs, geometry_ready, encoder_steps, encoder_invert, contrast, rotation;
    uint8_t pwm_limit, ramp_step;
    uint16_t line_threshold[5], wheel_mm, radius_sum_mm, speed_limit, test_ms;
    uint16_t kp, ki, kd; /* Q8 fixed point, common defaults; independent accumulators */
    uint16_t line_kp, line_speed;
    uint32_t counts_per_rev[4];
} Afr2Config;
typedef struct {
    uint16_t line_raw[5], distance_mm;
    uint8_t line_bits, line_valid, ultrasonic_valid, echo_error, hall_available;
    uint32_t line_ms, ultrasonic_ms, hall_ms;
    int32_t counts[4];
} Afr2Sensors;
typedef struct {
    uint32_t id, draft_id, changed_ms,status_ms,job_ms,draft_ms;
    uint8_t op, pending, claimed, available, mode, sta_connected, draft_ready, state;
    uint8_t network_count, selected_network,requested_mode;
    int16_t rssi;
    char ssid[33], ip[16], ap_ip[16], networks[8][33];
    char draft_ssid[33], password[64];
    char esp_version[16],web_version[16];
    uint8_t credential_length;
    uint32_t display_boot,display_event,display_ms;
    uint16_t display_revision;
    uint8_t display_page,display_ready,ap_active,ap_clients,web_ready;
    char ap_ssid[33],ap_password[64];
} Afr2Wifi;
typedef struct Afr2Product {
    Afr2Sequence sequence;
    Afr2Config config;
    Afr2Sensors sensors;
    Afr2Wifi wifi;
    uint8_t mode, origin, fault, stopped, selected_wheel, web_seen;
    uint8_t prefs_state, prefs_loaded, reset_cause, goto_state, hall_measuring, hall_bad;
    uint8_t notice, extension_ready;
    uint16_t local_timeout_ms;
    uint32_t now, control_ms, command_ms, local_started_ms, session, web_ms;
    uint32_t test_started_ms, mission_started_ms;
    int16_t pwm[4], target[4], speed[4], last_error[4];
    int32_t integral[4], previous_counts[4], hall_start, hall_delta;
    uint32_t hall_change_ms[4];
    int32_t x_mm, y_mm, yaw_mrad, target_x, target_y;
    int32_t mission_x, mission_y, mission_yaw;
    uint16_t mission_speed;
    uint8_t pose_valid;
    uint8_t odom_ready;
    float odom_x,odom_y,odom_yaw;
    uint32_t pose_frame,pose_sequence;
} Afr2Product;
void Afr2Product_Init(Afr2Product *p,uint8_t reset_cause);
void Afr2Product_Task(Afr2Product *p,uint32_t now);
uint8_t Afr2Product_Stop(void *ctx);
uint8_t Afr2Product_HaltMotion(void *ctx); /* transport supervision preserves autonomous radar */
void Afr2Product_Snapshot(void *ctx,uint32_t now,Afr2LinkSnapshot *out);
void Afr2Product_WebPulse(Afr2Product *p,uint16_t age);
uint8_t Afr2Product_Drive(Afr2Product *p,int16_t vx,int16_t vy,int16_t wz,uint16_t ttl);
uint8_t Afr2Product_Wheel(Afr2Product *p,uint8_t wheel,int16_t pwm,uint16_t duration);
uint8_t Afr2Product_StartLine(Afr2Product *p);
uint8_t Afr2Product_StartGoto(Afr2Product *p,int32_t x,int32_t y);
uint8_t Afr2Product_StartGotoFull(Afr2Product *p,int32_t x,int32_t y,int16_t theta,uint16_t speed);
void Afr2Product_HallBegin(Afr2Product *p);
uint8_t Afr2Product_HallEnd(Afr2Product *p);
void Afr2Product_HallSave(Afr2Product *p);
void Afr2Product_Save(Afr2Product *p);
void Afr2Product_Defaults(Afr2Config *c);
uint8_t Afr2Product_ConfigValid(const Afr2Config *c);
/* Backend APIs. Only product services access hardware; renderers never do. */
void Afr2Motor_Init(void);
void Afr2Motor_Stop(void);
void Afr2Motor_Write(const int16_t pwm[4],uint8_t signs);
void Afr2Sensors_Init(void);
void Afr2Sensors_Task(Afr2Sensors *s,const Afr2Config *c,uint32_t now);
void Afr2Servo_Set(uint16_t pulse_us); /* 0 disables; 1000..2000 candidate */
void Afr2Prefs_Load(Afr2Config *c,uint8_t *loaded);
uint8_t Afr2Prefs_Save(const Afr2Config *c);
uint8_t Afr2Prefs_Task(void); /* 0 idle,1 writing,2 done,3 failure */
uint8_t Afr2Boot_Cause(void);
uint32_t Afr2Boot_Id(void);
void Afr2Watchdog_Reboot(void);
void Afr2Watchdog_Enable(void);
void Afr2Watchdog_Kick(void);
#endif
