#ifndef INC_APP_PREFERENCES_H_DATA
#define INC_APP_PREFERENCES_H_DATA

#include <stdbool.h>
#include <stdint.h>
#include "am_port.h"
#include "line_calibration.h"
#include "auto_motion.h"
#include "app_ap_config_data.h"

#ifdef __cplusplus
extern "C" {
#endif

#define APP_PREFS_SCREEN_LOCK_TIMEOUT_DEFAULT_SEC 30u
#define APP_PREFS_SCREEN_LOCK_TIMEOUT_MIN_SEC      5u
#define APP_PREFS_SCREEN_LOCK_TIMEOUT_MAX_SEC    120u
#define APP_PREFS_SCREEN_LOCK_TIMEOUT_STEP_SEC     5u

#define APP_PREFS_MOTOR_MIN_PWM_DEFAULT          60u
#define APP_PREFS_MOTOR_MIN_PWM_MIN               0u
#define APP_PREFS_MOTOR_MIN_PWM_MAX             200u
#define APP_PREFS_MOTOR_MIN_PWM_STEP              5u

#define APP_PREFS_SYSTEM_ACTION_TIMEOUT_DEFAULT_SEC 5u
#define APP_PREFS_SYSTEM_ACTION_TIMEOUT_MIN_SEC     1u
#define APP_PREFS_SYSTEM_ACTION_TIMEOUT_MAX_SEC    10u
#define APP_PREFS_SYSTEM_ACTION_TIMEOUT_STEP_SEC    1u

#define APP_PREFS_USB_ALIVE_PERIOD_DEFAULT_MS    1000u
#define APP_PREFS_USB_ALIVE_PERIOD_MIN_MS         100u
#define APP_PREFS_USB_ALIVE_PERIOD_MAX_MS       10000u
#define APP_PREFS_USB_ALIVE_PERIOD_STEP_MS        100u

#define APP_PREFS_WEB_ALIVE_PERIOD_DEFAULT_MS    1000u
#define APP_PREFS_WEB_ALIVE_PERIOD_MIN_MS         100u
#define APP_PREFS_WEB_ALIVE_PERIOD_MAX_MS       10000u
#define APP_PREFS_WEB_ALIVE_PERIOD_STEP_MS        100u

#define APP_PREFS_WIFI_AP_SSID_MAX_LEN             32u
#define APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN         64u

#define APP_PREFS_WIFI_AP_SSID_DEFAULT "AutoMicro_AP"
#define APP_PREFS_WIFI_AP_PASSWORD_DEFAULT ""
#define APP_PREFS_WIFI_AP_PASSWORD_PROTECTED_DEFAULT false
#define APP_PREFS_WIFI_AP_PASSWORD_MIN_CHARS 8u


#define APP_PREFS_OLED_ROTATION_DEFAULT_DEG       180u
#define APP_PREFS_OLED_ROTATION_0_DEG               0u
#define APP_PREFS_OLED_ROTATION_180_DEG           180u

#define APP_PREFS_LINE_BLACK_WIDTH_DEFAULT_MM      18u
#define APP_PREFS_LINE_BLACK_WIDTH_MIN_MM          10u
#define APP_PREFS_LINE_BLACK_WIDTH_MAX_MM          30u
#define APP_PREFS_LINE_BLACK_WIDTH_STEP_MM          1u

#define APP_PREFS_AUTH_PIN_DIGITS                    4u

#define APP_PREFS_FLASH_LOG_SIZE_BYTES         (16u * 1024u)

#define APP_PREFS_BALANCE_PID_MIN                   0.0f
#define APP_PREFS_BALANCE_PID_MAX                  80.0f
#define APP_PREFS_BALANCE_PID_STEP                  0.05f

typedef uint32_t AppPreferencesFieldMask_t;

#define APP_PREFS_FIELD_ENCODER_INVERTED          (1u << 0)
#define APP_PREFS_FIELD_SCREEN_LOCK_ENABLED       (1u << 1)
#define APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT       (1u << 2)
#define APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM        (1u << 3)
#define APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM       (1u << 4)
#define APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT     (1u << 5)
#define APP_PREFS_FIELD_MPU_CALIBRATION           (1u << 6)
#define APP_PREFS_FIELD_OLED_ROTATION             (1u << 7)
#define APP_PREFS_FIELD_OLED_AUTO_ROTATION        (1u << 8)
#define APP_PREFS_FIELD_UNER_ROUTER_FORWARDING    (1u << 9)
#define APP_PREFS_FIELD_LINE_BLACK_WIDTH          (1u << 10)
#define APP_PREFS_FIELD_USB_ALIVE_PERIOD          (1u << 11)
#define APP_PREFS_FIELD_PIN_LOCK_ENABLED          (1u << 12)
#define APP_PREFS_FIELD_WEB_ALIVE_PERIOD          (1u << 13)
#define APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_ENABLED (1u << 14)
#define APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_LIMIT   (1u << 15)
#define APP_PREFS_FIELD_WIFI_AP_SSID               (1u << 16)
#define APP_PREFS_FIELD_WIFI_AP_PASSWORD           (1u << 17)
#define APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED (1u << 18)
#define APP_PREFS_FIELD_WIFI_AP_IPV4               (1u << 19)
#define APP_PREFS_FIELD_BALANCE_PID                (1u << 20)
#define APP_PREFS_FIELD_FAVORITES                  (1u << 21)
#define APP_PREFS_FIELD_WIFI_AP_APPLY_PENDING      (1u << 22)
#define APP_PREFS_FIELD_WIFI_AT_PROTOCOL           (1u << 23)
#define APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED   (1u << 24)
#define APP_PREFS_FIELD_ALL                       (APP_PREFS_FIELD_ENCODER_INVERTED | \
                                                   APP_PREFS_FIELD_SCREEN_LOCK_ENABLED | \
                                                   APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT | \
                                                   APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM | \
                                                   APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM | \
                                                   APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT | \
                                                   APP_PREFS_FIELD_MPU_CALIBRATION | \
                                                   APP_PREFS_FIELD_OLED_ROTATION | \
                                                   APP_PREFS_FIELD_OLED_AUTO_ROTATION | \
                                                   APP_PREFS_FIELD_UNER_ROUTER_FORWARDING | \
                                                   APP_PREFS_FIELD_LINE_BLACK_WIDTH | \
                                                   APP_PREFS_FIELD_USB_ALIVE_PERIOD | \
                                                   APP_PREFS_FIELD_PIN_LOCK_ENABLED | \
                                                   APP_PREFS_FIELD_WEB_ALIVE_PERIOD | \
                                                   APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_ENABLED | \
                                                   APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_LIMIT | \
                                                   APP_PREFS_FIELD_WIFI_AP_SSID | \
                                                   APP_PREFS_FIELD_WIFI_AP_PASSWORD | \
                                                   APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED | \
                                                   APP_PREFS_FIELD_WIFI_AP_IPV4 | \
                                                   APP_PREFS_FIELD_BALANCE_PID | \
                                                   APP_PREFS_FIELD_FAVORITES | \
                                                   APP_PREFS_FIELD_WIFI_AP_APPLY_PENDING | APP_PREFS_FIELD_WIFI_AT_PROTOCOL | \
                                                   APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED)

typedef enum {
    APP_PREFS_VALUE_BOOL = 0,
    APP_PREFS_VALUE_U16,
    APP_PREFS_VALUE_MPU_CALIBRATION
} AppPreferencesValueType_t;

typedef enum {
    EXT_PROFILE_NRF24 = 0,
    EXT_PROFILE_BEEPER_PB12
} ext_profile_t;

typedef struct {
    bool valid;
    float accelRollOffsetDeg;
    float accelPitchOffsetDeg;
    float gyroXOffsetDps;
    float gyroYOffsetDps;
    float gyroZOffsetDps;
    int16_t gravityXMg;
    int16_t gravityYMg;
    int16_t gravityZMg;
} AppPreferencesMpuCalibration_t;

typedef struct {
    float kp;
    float ki;
    float kd;
} AppPreferencesBalancePid_t;

typedef struct {
    bool encoderInverted;
    bool screenLockEnabled;
    uint16_t screenLockTimeoutSec;
    uint16_t motorLeftMinPwm;
    uint16_t motorRightMinPwm;
    uint16_t systemActionTimeoutSec;
    uint16_t usbAlivePeriodMs;
    uint16_t webAlivePeriodMs;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    bool oledAutoRotationEnabled;
    bool unerRouterForwardingEnabled;
    bool pinLockEnabled;
    bool espNotifWatchdogEnabled;
    uint8_t espNotifWatchdogLimit;
    AppPreferencesMpuCalibration_t mpuCalibration;
    AppPreferencesBalancePid_t balancePid;
    ext_profile_t ext_profile;
    char wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN];
    char wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN];
    bool wifiApPasswordProtected;
    uint8_t wifiApIpv4[4];
    bool wifiApApplyPending;
    uint8_t favoriteScreenCodes[3];
    uint16_t wifiAtProtocol;
    bool wifiApFallbackEnabled;
    LineCalibration_Store lineCalibration;
    AutoMotion_Profile autoMotionProfile;
} AppPreferences_t;

typedef struct {
    AppPreferencesFieldMask_t fields;
    AppPreferences_t values;
} AppPreferencesPatch_t;

typedef struct {
    AppPreferencesFieldMask_t field;
    const char *key;
    AppPreferencesValueType_t type;
    uint16_t minValue;
    uint16_t maxValue;
    uint16_t stepValue;
    uint16_t defaultValue;
} AppPreferencesDescriptor_t;

typedef enum {
    APP_PREFS_STORAGE_SOURCE_DEFAULTS = 0,
    APP_PREFS_STORAGE_SOURCE_SECTOR1,
    APP_PREFS_STORAGE_SOURCE_MIGRATED_SECTOR7,
    APP_PREFS_STORAGE_SOURCE_LEGACY_SECTOR7
} AppPreferencesStorageSource_t;

typedef enum {
    APP_PREFS_STORAGE_STATUS_DEFAULTS = 0,
    APP_PREFS_STORAGE_STATUS_READY,
    APP_PREFS_STORAGE_STATUS_MIGRATION_FAILED,
    APP_PREFS_STORAGE_STATUS_WRITE_FAILED
} AppPreferencesStorageStatus_t;

typedef struct {
    AppPreferencesStorageSource_t source;
    AppPreferencesStorageStatus_t status;
    uint16_t formatVersion;
    uint32_t sequence;
} AppPreferencesStorageInfo_t;

typedef struct {
    void *ctx;
    /* Empty legacy range disables migration reads; active storage is separate. */
    uint32_t base, end, legacy_base, legacy_end;
    const void *(*view)(void *ctx,uint32_t address,size_t length);
    AM_Result (*unlock)(void *ctx);
    AM_Result (*lock)(void *ctx);
    AM_Result (*erase)(void *ctx);
    AM_Result (*program)(void *ctx,uint32_t address,uint32_t word);
    bool (*ipv4_valid)(const uint8_t *);
    AppApStatus_t (*apply_ap_patch)(const AppApConfigPatch_t *,uint8_t *);
} AppPreferencesStoragePort;
typedef struct {
    const AppPreferencesStoragePort *storage;
    AppPreferences_t appPreferences;
    uint8_t appAuthPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint32_t appPreferencesNextWriteAddr, appPreferencesNextSequence;
    bool appPreferencesApRepairNeeded;
    AppPreferencesStorageInfo_t appPreferencesStorageInfo;
} AppPreferences_Handle;


#ifdef __cplusplus
}
#endif
#endif
