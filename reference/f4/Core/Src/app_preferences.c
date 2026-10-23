#include "app_preferences.h"
#include "balance_control_data.h"

#include <stddef.h>
#include <string.h>

#define APP_PREFS_FLASH_BASE_ADDR (h->storage->base)
#define APP_PREFS_FLASH_END_ADDR (h->storage->end)
#define APP_PREFS_LEGACY_FLASH_BASE_ADDR (h->storage->legacy_base)
#define APP_PREFS_LEGACY_FLASH_END_ADDR (h->storage->legacy_end)
#define APP_PREFS_MAGIC             0x34504641u /* "AFP4" little-endian */
#define APP_PREFS_VERSION_V1        1u
#define APP_PREFS_VERSION_V2        2u
#define APP_PREFS_VERSION_V3        3u
#define APP_PREFS_VERSION_V4        4u
#define APP_PREFS_VERSION_V5        5u
#define APP_PREFS_VERSION_V6        6u
#define APP_PREFS_VERSION_V7        7u
#define APP_PREFS_VERSION_V8        8u
#define APP_PREFS_VERSION_V9        9u
#define APP_PREFS_VERSION_V10      10u
#define APP_PREFS_VERSION_V11      11u
#define APP_PREFS_VERSION_V12      12u
#define APP_PREFS_VERSION          13u
#define APP_PREFS_COMMIT_WORD       0xA5A55A5Au
#define APP_PREFS_ERASED_WORD       0xFFFFFFFFu

#define APP_PREFS_FLAG_ENCODER_INV  (1u << 0)
#define APP_PREFS_FLAG_LOCK_EN      (1u << 1)
#define APP_PREFS_FLAG_MPU_CAL      (1u << 2)
#define APP_PREFS_FLAG_OLED_AUTO_ROT (1u << 3)
#define APP_PREFS_FLAG_ROUTER_FWD_DISABLED (1u << 4)
#define APP_PREFS_FLAG_PIN_LOCK_DISABLED   (1u << 5)
#define APP_PREFS_FLAG_AP_FALLBACK_DISABLED (1u << 6)
#define APP_PREFS_MPU_SENSOR_MPU9250         0x9250u

#define APP_PREFS_MOTOR_LEFT_MIN_PWM_SHIFT   8u
#define APP_PREFS_MOTOR_RIGHT_MIN_PWM_SHIFT 16u
#define APP_PREFS_MOTOR_MIN_PWM_ENCODE_MASK  0xFFu
#define APP_PREFS_SYSTEM_ACTION_TIMEOUT_SHIFT 24u
#define APP_PREFS_SYSTEM_ACTION_TIMEOUT_ENCODE_MASK 0xFFu

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
} AppPreferencesRecordHeader_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV1_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved0;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t reserved1;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV2_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved0;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV3_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved0;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV4_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved0;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV5_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved0;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint16_t webAlivePeriodMs;
    uint16_t reserved1;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV6_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t ext_profile;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved0;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint16_t webAlivePeriodMs;
    uint16_t reserved1;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV7_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t ext_profile;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved0;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint16_t webAlivePeriodMs;
    uint8_t espNotifWatchdogLimit;
    bool espNotifWatchdogEnabled;
    uint16_t balanceKpX20;
    uint16_t balanceKiX20;
    uint16_t balanceKdX20;
    uint16_t mpuSensorId;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV8_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t ext_profile;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t reserved0;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint16_t webAlivePeriodMs;
    uint8_t espNotifWatchdogLimit;
    bool espNotifWatchdogEnabled;
    uint16_t balanceKpX20;
    uint16_t balanceKiX20;
    uint16_t balanceKdX20;
    uint16_t mpuSensorId;
    char wifiApSsid[32u];
    char wifiApPassword[64u];
    bool wifiApPasswordProtected;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV9_t;

typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t ext_profile;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t wifiAtProtocol;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint16_t webAlivePeriodMs;
    uint8_t espNotifWatchdogLimit;
    bool espNotifWatchdogEnabled;
    uint16_t balanceKpX20;
    uint16_t balanceKiX20;
    uint16_t balanceKdX20;
    uint16_t mpuSensorId;
    char wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN];
    char wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN];
    bool wifiApPasswordProtected;
    uint8_t wifiApIpv4[4];
    uint8_t favoriteScreenCodes[3];
    bool wifiApApplyPending;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV11_t;
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t ext_profile;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t wifiAtProtocol;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint16_t webAlivePeriodMs;
    uint8_t espNotifWatchdogLimit;
    bool espNotifWatchdogEnabled;
    uint16_t balanceKpX20;
    uint16_t balanceKiX20;
    uint16_t balanceKdX20;
    uint16_t mpuSensorId;
    char wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN];
    char wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN];
    bool wifiApPasswordProtected;
    uint8_t wifiApIpv4[4];
    uint8_t favoriteScreenCodes[3];
    bool wifiApApplyPending;
    LineCalibration_Store lineCalibration;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecordV12_t;
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint16_t size;
    uint32_t sequence;
    uint32_t ext_profile;
    uint32_t flags;
    uint16_t screenLockTimeoutSec;
    uint16_t wifiAtProtocol;
    float mpuAccelRollOffsetDeg;
    float mpuAccelPitchOffsetDeg;
    float mpuGyroXOffsetDps;
    float mpuGyroYOffsetDps;
    float mpuGyroZOffsetDps;
    int16_t mpuGravityXMg;
    int16_t mpuGravityYMg;
    int16_t mpuGravityZMg;
    uint16_t oledRotationDeg;
    uint16_t lineBlackWidthMm;
    uint16_t usbAlivePeriodMs;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint16_t webAlivePeriodMs;
    uint8_t espNotifWatchdogLimit;
    bool espNotifWatchdogEnabled;
    uint16_t balanceKpX20;
    uint16_t balanceKiX20;
    uint16_t balanceKdX20;
    uint16_t mpuSensorId;
    char wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN];
    char wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN];
    bool wifiApPasswordProtected;
    uint8_t wifiApIpv4[4];
    uint8_t favoriteScreenCodes[3];
    bool wifiApApplyPending;
    LineCalibration_Store lineCalibration;
    AutoMotion_Profile autoMotionProfile;
    uint32_t checksum;
    uint32_t commit;
} AppPreferencesRecord_t;
typedef AppPreferencesRecordV11_t AppPreferencesRecordV10_t;


typedef struct {
    bool found;
    uint16_t formatVersion;
    uint32_t sequence;
    uint32_t firstFreeAddr;
    AppPreferences_t preferences;
    uint8_t authPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
} AppPreferencesScanResult_t;

_Static_assert(sizeof(AppPreferencesRecordV4_t) == 60u,
               "Preferences V4 flash layout changed");
_Static_assert(sizeof(AppPreferencesRecordV5_t) == 64u,
               "Preferences V5 flash layout changed");
_Static_assert(sizeof(AppPreferencesRecordV6_t) == 68u,
               "Preferences V6 flash layout changed");
_Static_assert(sizeof(AppPreferencesRecordV7_t) == 72u,
               "Preferences V7 flash layout changed");
_Static_assert(sizeof(AppPreferencesRecordV8_t) == 80u,
               "Preferences V8 flash layout changed");
_Static_assert(sizeof(AppPreferencesRecordV9_t) == 180u,
               "Preferences V9 flash layout changed");
_Static_assert(sizeof(AppPreferencesRecordV11_t) == 188u,
               "Preferences V11 flash layout changed");
_Static_assert(offsetof(AppPreferencesRecordV4_t, checksum) == 52u,
               "Preferences V4 checksum offset changed");
_Static_assert(offsetof(AppPreferencesRecordV5_t, authPinDigits) == 52u,
               "Preferences V5 PIN offset changed");
_Static_assert(offsetof(AppPreferencesRecordV5_t, checksum) == 56u,
               "Preferences V5 checksum offset changed");
_Static_assert(offsetof(AppPreferencesRecordV6_t, checksum) == 60u,
               "Preferences V6 checksum offset changed");
_Static_assert(offsetof(AppPreferencesRecordV7_t, checksum) == 64u,
               "Preferences V7 Web alive offset changed");
_Static_assert(offsetof(AppPreferencesRecordV8_t, balanceKpX20) == 64u,
               "Preferences V8 PID offset changed");
_Static_assert(offsetof(AppPreferencesRecordV8_t, mpuSensorId) == 70u,
               "Preferences V8 MPU sensor ID offset changed");
_Static_assert(offsetof(AppPreferencesRecordV8_t, checksum) == 72u,
               "Preferences V8 checksum offset changed");
_Static_assert(offsetof(AppPreferencesRecordV9_t, wifiApSsid) == 72u,
               "Preferences V9 WiFi AP SSID offset changed");
_Static_assert(offsetof(AppPreferencesRecordV9_t, wifiApPassword) == 104u,
               "Preferences V9 WiFi AP Password offset changed");
_Static_assert(offsetof(AppPreferencesRecordV9_t, checksum) == 172u,
               "Preferences V9 checksum offset changed");
_Static_assert(offsetof(AppPreferencesRecordV11_t, checksum) == 180u,
               "Preferences V11 checksum offset changed");
static const uint8_t appDefaultAuthPinDigits[APP_PREFS_AUTH_PIN_DIGITS] = {
    1u, 2u, 3u, 4u
};

static bool AppPreferences_MpuCalibrationEquals(const AppPreferencesMpuCalibration_t *a,
                                                const AppPreferencesMpuCalibration_t *b);
static size_t AppPreferences_BoundedStringLength(const char *text, size_t capacity);
static bool AppPreferences_WifiApConfigurationIsValid(const AppPreferences_t *prefs);

static const AppPreferencesDescriptor_t appPreferenceDescriptors[] = {
    {
        .field = APP_PREFS_FIELD_ENCODER_INVERTED,
        .key = "encoderInverted",
        .type = APP_PREFS_VALUE_BOOL,
        .minValue = 0u,
        .maxValue = 1u,
        .stepValue = 1u,
        .defaultValue = 0u,
    },
    {
        .field = APP_PREFS_FIELD_SCREEN_LOCK_ENABLED,
        .key = "screenLockEnabled",
        .type = APP_PREFS_VALUE_BOOL,
        .minValue = 0u,
        .maxValue = 1u,
        .stepValue = 1u,
        .defaultValue = 0u,
    },
    {
        .field = APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT,
        .key = "screenLockTimeoutSec",
        .type = APP_PREFS_VALUE_U16,
        .minValue = APP_PREFS_SCREEN_LOCK_TIMEOUT_MIN_SEC,
        .maxValue = APP_PREFS_SCREEN_LOCK_TIMEOUT_MAX_SEC,
        .stepValue = APP_PREFS_SCREEN_LOCK_TIMEOUT_STEP_SEC,
        .defaultValue = APP_PREFS_SCREEN_LOCK_TIMEOUT_DEFAULT_SEC,
    },
    {
        .field = APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM,
        .key = "motorLeftMinPwm",
        .type = APP_PREFS_VALUE_U16,
        .minValue = APP_PREFS_MOTOR_MIN_PWM_MIN,
        .maxValue = APP_PREFS_MOTOR_MIN_PWM_MAX,
        .stepValue = APP_PREFS_MOTOR_MIN_PWM_STEP,
        .defaultValue = APP_PREFS_MOTOR_MIN_PWM_DEFAULT,
    },
    {
        .field = APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM,
        .key = "motorRightMinPwm",
        .type = APP_PREFS_VALUE_U16,
        .minValue = APP_PREFS_MOTOR_MIN_PWM_MIN,
        .maxValue = APP_PREFS_MOTOR_MIN_PWM_MAX,
        .stepValue = APP_PREFS_MOTOR_MIN_PWM_STEP,
        .defaultValue = APP_PREFS_MOTOR_MIN_PWM_DEFAULT,
    },
    {
        .field = APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT,
        .key = "systemActionTimeoutSec",
        .type = APP_PREFS_VALUE_U16,
        .minValue = APP_PREFS_SYSTEM_ACTION_TIMEOUT_MIN_SEC,
        .maxValue = APP_PREFS_SYSTEM_ACTION_TIMEOUT_MAX_SEC,
        .stepValue = APP_PREFS_SYSTEM_ACTION_TIMEOUT_STEP_SEC,
        .defaultValue = APP_PREFS_SYSTEM_ACTION_TIMEOUT_DEFAULT_SEC,
    },
    {
        .field = APP_PREFS_FIELD_USB_ALIVE_PERIOD,
        .key = "usbAlivePeriodMs",
        .type = APP_PREFS_VALUE_U16,
        .minValue = APP_PREFS_USB_ALIVE_PERIOD_MIN_MS,
        .maxValue = APP_PREFS_USB_ALIVE_PERIOD_MAX_MS,
        .stepValue = APP_PREFS_USB_ALIVE_PERIOD_STEP_MS,
        .defaultValue = APP_PREFS_USB_ALIVE_PERIOD_DEFAULT_MS,
    },
    {
        .field = APP_PREFS_FIELD_OLED_ROTATION,
        .key = "oledRotationDeg",
        .type = APP_PREFS_VALUE_U16,
        .minValue = APP_PREFS_OLED_ROTATION_0_DEG,
        .maxValue = APP_PREFS_OLED_ROTATION_180_DEG,
        .stepValue = APP_PREFS_OLED_ROTATION_180_DEG,
        .defaultValue = APP_PREFS_OLED_ROTATION_DEFAULT_DEG,
    },
    {
        .field = APP_PREFS_FIELD_OLED_AUTO_ROTATION,
        .key = "oledAutoRotationEnabled",
        .type = APP_PREFS_VALUE_BOOL,
        .minValue = 0u,
        .maxValue = 1u,
        .stepValue = 1u,
        .defaultValue = 0u,
    },
    {
        .field = APP_PREFS_FIELD_UNER_ROUTER_FORWARDING,
        .key = "unerRouterForwardingEnabled",
        .type = APP_PREFS_VALUE_BOOL,
        .minValue = 0u,
        .maxValue = 1u,
        .stepValue = 1u,
        .defaultValue = 1u,
    },
    {
        .field = APP_PREFS_FIELD_LINE_BLACK_WIDTH,
        .key = "lineBlackWidthMm",
        .type = APP_PREFS_VALUE_U16,
        .minValue = APP_PREFS_LINE_BLACK_WIDTH_MIN_MM,
        .maxValue = APP_PREFS_LINE_BLACK_WIDTH_MAX_MM,
        .stepValue = APP_PREFS_LINE_BLACK_WIDTH_STEP_MM,
        .defaultValue = APP_PREFS_LINE_BLACK_WIDTH_DEFAULT_MM,
    },
    {
        .field = APP_PREFS_FIELD_MPU_CALIBRATION,
        .key = "mpuCalibration",
        .type = APP_PREFS_VALUE_MPU_CALIBRATION,
        .minValue = 0u,
        .maxValue = 0u,
        .stepValue = 0u,
        .defaultValue = 0u,
    },
    {
        .field = APP_PREFS_FIELD_PIN_LOCK_ENABLED,
        .key = "pinLockEnabled",
        .type = APP_PREFS_VALUE_BOOL,
        .minValue = 0u,
        .maxValue = 1u,
        .stepValue = 1u,
        .defaultValue = 1u,
    },
    {
        .field = APP_PREFS_FIELD_WEB_ALIVE_PERIOD,
        .key = "webAlivePeriodMs",
        .type = APP_PREFS_VALUE_U16,
        .minValue = APP_PREFS_WEB_ALIVE_PERIOD_MIN_MS,
        .maxValue = APP_PREFS_WEB_ALIVE_PERIOD_MAX_MS,
        .stepValue = APP_PREFS_WEB_ALIVE_PERIOD_STEP_MS,
        .defaultValue = APP_PREFS_WEB_ALIVE_PERIOD_DEFAULT_MS,
    },
    {
        .field = APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_ENABLED,
        .key = "espNotifWatchdogEnabled",
        .type = APP_PREFS_VALUE_BOOL,
        .minValue = 0u,
        .maxValue = 1u,
        .stepValue = 1u,
        .defaultValue = 1u,
    },
    {
        .field = APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_LIMIT,
        .key = "espNotifWatchdogLimit",
        .type = APP_PREFS_VALUE_U16,
        .minValue = 0u,
        .maxValue = 255u,
        .stepValue = 1u,
        .defaultValue = 3u,
    },
    {
        .field = APP_PREFS_FIELD_WIFI_AT_PROTOCOL,
        .key = "wifiAtProtocol",
        .type = APP_PREFS_VALUE_U16,
        .minValue = 0u,
        .maxValue = 1u,
        .stepValue = 1u,
        .defaultValue = 0u,
    },
    {
        .field = APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED,
        .key = "wifiApFallbackEnabled",
        .type = APP_PREFS_VALUE_BOOL,
        .minValue = 0u,
        .maxValue = 1u,
        .stepValue = 1u,
        .defaultValue = 1u,
    },
};

static AppPreferences_t AppPreferences_Defaults(void)
{
    AppPreferences_t prefs;

    memset(&prefs, 0, sizeof(prefs));
    prefs.encoderInverted = false;
    prefs.screenLockEnabled = false;
    prefs.screenLockTimeoutSec = APP_PREFS_SCREEN_LOCK_TIMEOUT_DEFAULT_SEC;
    prefs.motorLeftMinPwm = APP_PREFS_MOTOR_MIN_PWM_DEFAULT;
    prefs.motorRightMinPwm = APP_PREFS_MOTOR_MIN_PWM_DEFAULT;
    prefs.systemActionTimeoutSec = APP_PREFS_SYSTEM_ACTION_TIMEOUT_DEFAULT_SEC;
    prefs.usbAlivePeriodMs = APP_PREFS_USB_ALIVE_PERIOD_DEFAULT_MS;
    prefs.webAlivePeriodMs = APP_PREFS_WEB_ALIVE_PERIOD_DEFAULT_MS;
    prefs.oledRotationDeg = APP_PREFS_OLED_ROTATION_DEFAULT_DEG;
    prefs.lineBlackWidthMm = APP_PREFS_LINE_BLACK_WIDTH_DEFAULT_MM;
    prefs.oledAutoRotationEnabled = false;
    prefs.unerRouterForwardingEnabled = true;
    prefs.pinLockEnabled = true;
    prefs.wifiApFallbackEnabled = true;
    prefs.balancePid.kp = BALANCE_CONTROL_DEFAULT_KP;
    prefs.balancePid.ki = BALANCE_CONTROL_DEFAULT_KI;
    prefs.balancePid.kd = BALANCE_CONTROL_DEFAULT_KD;
    prefs.ext_profile = EXT_PROFILE_NRF24;
    prefs.wifiAtProtocol = 0u;
    prefs.espNotifWatchdogEnabled = true;
    prefs.espNotifWatchdogLimit = 3;
    strncpy(prefs.wifiApSsid,
            APP_PREFS_WIFI_AP_SSID_DEFAULT,
            sizeof(prefs.wifiApSsid) - 1u);
    strncpy(prefs.wifiApPassword,
            APP_PREFS_WIFI_AP_PASSWORD_DEFAULT,
            sizeof(prefs.wifiApPassword) - 1u);
    prefs.wifiApPasswordProtected =
        APP_PREFS_WIFI_AP_PASSWORD_PROTECTED_DEFAULT;
    prefs.wifiApApplyPending = true;

    return prefs;
}

static size_t AppPreferences_BoundedStringLength(const char *text, size_t capacity)
{
    size_t length = 0u;

    if (text == NULL) {
        return capacity;
    }

    while (length < capacity && text[length] != '\0') {
        length++;
    }

    return length;
}

static bool AppPreferences_WifiApConfigurationIsValid(const AppPreferences_t *prefs)
{
    size_t ssidLength;
    size_t passwordLength;

    if (prefs == NULL) {
        return false;
    }

    ssidLength = AppPreferences_BoundedStringLength(
        prefs->wifiApSsid, APP_PREFS_WIFI_AP_SSID_MAX_LEN);
    passwordLength = AppPreferences_BoundedStringLength(
        prefs->wifiApPassword, APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN);

    return ssidLength > 0u &&
           ssidLength < APP_PREFS_WIFI_AP_SSID_MAX_LEN &&
           passwordLength < APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN &&
           (passwordLength == 0u ||
            passwordLength >= APP_PREFS_WIFI_AP_PASSWORD_MIN_CHARS) &&
           (!prefs->wifiApPasswordProtected ||
            passwordLength >= APP_PREFS_WIFI_AP_PASSWORD_MIN_CHARS);
}

static void AppPreferences_LoadDefaultAuthPin(uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS])
{
    if (pinDigits != NULL) {
        memcpy(pinDigits, appDefaultAuthPinDigits, APP_PREFS_AUTH_PIN_DIGITS);
    }
}

static bool AppPreferences_AuthPinFormatIsValid(
    const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS])
{
    uint8_t valid = (pinDigits != NULL) ? 1u : 0u;

    if (pinDigits == NULL) {
        return false;
    }

    for (uint8_t i = 0u; i < APP_PREFS_AUTH_PIN_DIGITS; ++i) {
        valid &= (pinDigits[i] <= 9u) ? 1u : 0u;
    }

    return valid != 0u;
}

uint16_t AppPreferences_ClampScreenLockTimeoutSec(uint16_t timeoutSec)
{
    if (timeoutSec < APP_PREFS_SCREEN_LOCK_TIMEOUT_MIN_SEC) {
        return APP_PREFS_SCREEN_LOCK_TIMEOUT_MIN_SEC;
    }

    if (timeoutSec > APP_PREFS_SCREEN_LOCK_TIMEOUT_MAX_SEC) {
        return APP_PREFS_SCREEN_LOCK_TIMEOUT_MAX_SEC;
    }

    return (uint16_t)((timeoutSec / APP_PREFS_SCREEN_LOCK_TIMEOUT_STEP_SEC) *
                      APP_PREFS_SCREEN_LOCK_TIMEOUT_STEP_SEC);
}

uint16_t AppPreferences_ClampMotorMinPwm(uint16_t minPwm)
{
#if APP_PREFS_MOTOR_MIN_PWM_MIN > 0u
    if (minPwm < APP_PREFS_MOTOR_MIN_PWM_MIN) {
        return APP_PREFS_MOTOR_MIN_PWM_MIN;
    }
#endif

    if (minPwm > APP_PREFS_MOTOR_MIN_PWM_MAX) {
        return APP_PREFS_MOTOR_MIN_PWM_MAX;
    }

    return (uint16_t)((minPwm / APP_PREFS_MOTOR_MIN_PWM_STEP) *
                      APP_PREFS_MOTOR_MIN_PWM_STEP);
}

uint16_t AppPreferences_ClampSystemActionTimeoutSec(uint16_t timeoutSec)
{
    if (timeoutSec < APP_PREFS_SYSTEM_ACTION_TIMEOUT_MIN_SEC) {
        return APP_PREFS_SYSTEM_ACTION_TIMEOUT_MIN_SEC;
    }

    if (timeoutSec > APP_PREFS_SYSTEM_ACTION_TIMEOUT_MAX_SEC) {
        return APP_PREFS_SYSTEM_ACTION_TIMEOUT_MAX_SEC;
    }

    return (uint16_t)((timeoutSec / APP_PREFS_SYSTEM_ACTION_TIMEOUT_STEP_SEC) *
                      APP_PREFS_SYSTEM_ACTION_TIMEOUT_STEP_SEC);
}

uint16_t AppPreferences_ClampUsbAlivePeriodMs(uint16_t periodMs)
{
    if (periodMs < APP_PREFS_USB_ALIVE_PERIOD_MIN_MS) {
        return APP_PREFS_USB_ALIVE_PERIOD_MIN_MS;
    }

    if (periodMs > APP_PREFS_USB_ALIVE_PERIOD_MAX_MS) {
        return APP_PREFS_USB_ALIVE_PERIOD_MAX_MS;
    }

    return (uint16_t)((periodMs / APP_PREFS_USB_ALIVE_PERIOD_STEP_MS) *
                      APP_PREFS_USB_ALIVE_PERIOD_STEP_MS);
}

uint16_t AppPreferences_ClampWebAlivePeriodMs(uint16_t periodMs)
{
    if (periodMs < APP_PREFS_WEB_ALIVE_PERIOD_MIN_MS) {
        return APP_PREFS_WEB_ALIVE_PERIOD_MIN_MS;
    }

    if (periodMs > APP_PREFS_WEB_ALIVE_PERIOD_MAX_MS) {
        return APP_PREFS_WEB_ALIVE_PERIOD_MAX_MS;
    }

    return (uint16_t)((periodMs / APP_PREFS_WEB_ALIVE_PERIOD_STEP_MS) *
                      APP_PREFS_WEB_ALIVE_PERIOD_STEP_MS);
}

uint16_t AppPreferences_ClampOledRotationDeg(uint16_t rotationDeg)
{
    return (rotationDeg >= 90u)
        ? APP_PREFS_OLED_ROTATION_180_DEG
        : APP_PREFS_OLED_ROTATION_0_DEG;
}

uint16_t AppPreferences_ClampLineBlackWidthMm(uint16_t widthMm)
{
    if (widthMm < APP_PREFS_LINE_BLACK_WIDTH_MIN_MM) {
        return APP_PREFS_LINE_BLACK_WIDTH_MIN_MM;
    }

    if (widthMm > APP_PREFS_LINE_BLACK_WIDTH_MAX_MM) {
        return APP_PREFS_LINE_BLACK_WIDTH_MAX_MM;
    }

    return (uint16_t)((widthMm / APP_PREFS_LINE_BLACK_WIDTH_STEP_MM) *
                      APP_PREFS_LINE_BLACK_WIDTH_STEP_MM);
}

static uint16_t AppPreferences_DecodeMotorMinPwm(uint32_t flags, uint8_t shift)
{
    uint32_t encoded = (flags >> shift) & APP_PREFS_MOTOR_MIN_PWM_ENCODE_MASK;

    if (encoded == 0u) {
        return APP_PREFS_MOTOR_MIN_PWM_DEFAULT;
    }

    return AppPreferences_ClampMotorMinPwm((uint16_t)(encoded - 1u));
}

static uint32_t AppPreferences_EncodeMotorMinPwm(uint16_t minPwm, uint8_t shift)
{
    uint32_t encoded = (uint32_t)AppPreferences_ClampMotorMinPwm(minPwm) + 1u;

    return (encoded & APP_PREFS_MOTOR_MIN_PWM_ENCODE_MASK) << shift;
}

static uint16_t AppPreferences_DecodeSystemActionTimeoutSec(uint32_t flags)
{
    uint32_t encoded = (flags >> APP_PREFS_SYSTEM_ACTION_TIMEOUT_SHIFT) &
                       APP_PREFS_SYSTEM_ACTION_TIMEOUT_ENCODE_MASK;

    if (encoded == 0u) {
        return APP_PREFS_SYSTEM_ACTION_TIMEOUT_DEFAULT_SEC;
    }

    return AppPreferences_ClampSystemActionTimeoutSec((uint16_t)(encoded - 1u));
}

static uint32_t AppPreferences_EncodeSystemActionTimeoutSec(uint16_t timeoutSec)
{
    uint32_t encoded = (uint32_t)AppPreferences_ClampSystemActionTimeoutSec(timeoutSec) + 1u;

    return (encoded & APP_PREFS_SYSTEM_ACTION_TIMEOUT_ENCODE_MASK) <<
           APP_PREFS_SYSTEM_ACTION_TIMEOUT_SHIFT;
}

static float AppPreferences_ClampBalancePid(float value)
{
    if (value < APP_PREFS_BALANCE_PID_MIN) {
        return APP_PREFS_BALANCE_PID_MIN;
    }
    if (value > APP_PREFS_BALANCE_PID_MAX) {
        return APP_PREFS_BALANCE_PID_MAX;
    }
    return value;
}

static uint16_t AppPreferences_EncodeBalancePid(float value)
{
    value = AppPreferences_ClampBalancePid(value);
    return (uint16_t)((value / APP_PREFS_BALANCE_PID_STEP) + 0.5f);
}

static float AppPreferences_DecodeBalancePid(uint16_t encoded)
{
    return AppPreferences_ClampBalancePid(
        (float)encoded * APP_PREFS_BALANCE_PID_STEP);
}

static uint32_t AppPreferences_ChecksumBytes(const void *record, uint32_t bytes)
{
    const uint32_t *words = (const uint32_t *)record;
    const uint32_t wordCount = bytes / sizeof(uint32_t);
    uint32_t acc = 2166136261u;

    for (uint32_t i = 0u; i < wordCount; i++) {
        acc ^= words[i];
        acc *= 16777619u;
    }

    return acc ^ 0xC001CAFEu;
}

static bool AppPreferences_IsErasedRange(const void *base, uint32_t bytes)
{
    const uint32_t *words = (const uint32_t *)base;
    const uint32_t wordCount = bytes / sizeof(uint32_t);

    for (uint32_t i = 0u; i < wordCount; i++) {
        if (words[i] != APP_PREFS_ERASED_WORD) {
            return false;
        }
    }

    return true;
}

static bool AppPreferences_RecordV1IsValid(const AppPreferencesRecordV1_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V1 ||
        record->size != sizeof(AppPreferencesRecordV1_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV1_t, checksum));
}

static bool AppPreferences_RecordV2IsValid(const AppPreferencesRecordV2_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V2 ||
        record->size != sizeof(AppPreferencesRecordV2_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV2_t, checksum));
}

static bool AppPreferences_RecordV3IsValid(const AppPreferencesRecordV3_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V3 ||
        record->size != sizeof(AppPreferencesRecordV3_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV3_t, checksum));
}

static bool AppPreferences_RecordV4IsValid(const AppPreferencesRecordV4_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V4 ||
        record->size != sizeof(AppPreferencesRecordV4_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV4_t, checksum));
}

static bool AppPreferences_RecordV5IsValid(const AppPreferencesRecordV5_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V5 ||
        record->size != sizeof(AppPreferencesRecordV5_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV5_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits);
}

static bool AppPreferences_RecordV6IsValid(const AppPreferencesRecordV6_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V6 ||
        record->size != sizeof(AppPreferencesRecordV6_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV6_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits);
}

static bool AppPreferences_RecordV7IsValid(const AppPreferencesRecordV7_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V7 ||
        record->size != sizeof(AppPreferencesRecordV7_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV7_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits);
}

static bool AppPreferences_RecordV8IsValid(const AppPreferencesRecordV8_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V8 ||
        record->size != sizeof(AppPreferencesRecordV8_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV8_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits);
}

static bool AppPreferences_RecordV9IsValid(const AppPreferencesRecordV9_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V9 ||
        record->size != sizeof(AppPreferencesRecordV9_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV9_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits);
}
static bool AppPreferences_RecordV10IsValid(const AppPreferencesRecordV10_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V10 ||
        record->size != sizeof(AppPreferencesRecordV10_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV10_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits);
}


static bool AppPreferences_RecordV11IsValid(const AppPreferencesRecordV11_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V11 ||
        record->size != sizeof(AppPreferencesRecordV11_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV11_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits) &&
           record->wifiAtProtocol <= 1u;
}

static bool AppPreferences_RecordV12IsValid(const AppPreferencesRecordV12_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION_V12 ||
        record->size != sizeof(AppPreferencesRecordV12_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecordV12_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits) &&
           record->wifiAtProtocol <= 1u;
}

static bool AppPreferences_RecordIsValid(const AppPreferencesRecord_t *record)
{
    if (record->magic != APP_PREFS_MAGIC ||
        record->version != APP_PREFS_VERSION ||
        record->size != sizeof(AppPreferencesRecord_t) ||
        record->commit != APP_PREFS_COMMIT_WORD) {
        return false;
    }

    return record->checksum ==
           AppPreferences_ChecksumBytes(record, offsetof(AppPreferencesRecord_t, checksum)) &&
           AppPreferences_AuthPinFormatIsValid(record->authPinDigits) &&
           record->wifiAtProtocol <= 1u;
}

static AppPreferences_t AppPreferences_FromRecordV1(const AppPreferencesRecordV1_t *record)
{
    AppPreferences_t prefs = AppPreferences_Defaults();

    prefs.wifiApFallbackEnabled = (record->flags & APP_PREFS_FLAG_AP_FALLBACK_DISABLED) == 0u;
    prefs.encoderInverted = (record->flags & APP_PREFS_FLAG_ENCODER_INV) != 0u;
    prefs.screenLockEnabled = (record->flags & APP_PREFS_FLAG_LOCK_EN) != 0u;
    prefs.screenLockTimeoutSec =
        AppPreferences_ClampScreenLockTimeoutSec(record->screenLockTimeoutSec);
    prefs.motorLeftMinPwm =
        AppPreferences_DecodeMotorMinPwm(record->flags,
                                         APP_PREFS_MOTOR_LEFT_MIN_PWM_SHIFT);
    prefs.motorRightMinPwm =
        AppPreferences_DecodeMotorMinPwm(record->flags,
                                         APP_PREFS_MOTOR_RIGHT_MIN_PWM_SHIFT);
    prefs.systemActionTimeoutSec =
        AppPreferences_DecodeSystemActionTimeoutSec(record->flags);

    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV2(const AppPreferencesRecordV2_t *record)
{
    AppPreferences_t prefs = AppPreferences_FromRecordV1((const AppPreferencesRecordV1_t *)record);

    prefs.mpuCalibration.valid = (record->flags & APP_PREFS_FLAG_MPU_CAL) != 0u;
    prefs.mpuCalibration.accelRollOffsetDeg = record->mpuAccelRollOffsetDeg;
    prefs.mpuCalibration.accelPitchOffsetDeg = record->mpuAccelPitchOffsetDeg;
    prefs.mpuCalibration.gyroXOffsetDps = record->mpuGyroXOffsetDps;
    prefs.mpuCalibration.gyroYOffsetDps = record->mpuGyroYOffsetDps;
    prefs.mpuCalibration.gyroZOffsetDps = record->mpuGyroZOffsetDps;
    prefs.mpuCalibration.gravityXMg = record->mpuGravityXMg;
    prefs.mpuCalibration.gravityYMg = record->mpuGravityYMg;
    prefs.mpuCalibration.gravityZMg = record->mpuGravityZMg;

    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV3(const AppPreferencesRecordV3_t *record)
{
    AppPreferences_t prefs = AppPreferences_FromRecordV2((const AppPreferencesRecordV2_t *)record);

    prefs.oledRotationDeg = AppPreferences_ClampOledRotationDeg(record->oledRotationDeg);
    prefs.oledAutoRotationEnabled = (record->flags & APP_PREFS_FLAG_OLED_AUTO_ROT) != 0u;
    prefs.unerRouterForwardingEnabled =
        (record->flags & APP_PREFS_FLAG_ROUTER_FWD_DISABLED) == 0u;

    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV4(const AppPreferencesRecordV4_t *record)
{
    AppPreferences_t prefs = AppPreferences_FromRecordV3((const AppPreferencesRecordV3_t *)record);

    prefs.lineBlackWidthMm =
        AppPreferences_ClampLineBlackWidthMm(record->lineBlackWidthMm);
    prefs.usbAlivePeriodMs = (record->usbAlivePeriodMs == 0u)
        ? APP_PREFS_USB_ALIVE_PERIOD_DEFAULT_MS
        : AppPreferences_ClampUsbAlivePeriodMs(record->usbAlivePeriodMs);

    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV5(const AppPreferencesRecordV5_t *record)
{
    AppPreferences_t prefs =
        AppPreferences_FromRecordV4((const AppPreferencesRecordV4_t *)record);

    /* V5 records written before this preference used zero in this bit. */
    prefs.pinLockEnabled =
        (record->flags & APP_PREFS_FLAG_PIN_LOCK_DISABLED) == 0u;
    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV6(const AppPreferencesRecordV6_t *record)
{
    AppPreferences_t prefs =
        AppPreferences_FromRecordV5((const AppPreferencesRecordV5_t *)record);

    prefs.webAlivePeriodMs = (record->webAlivePeriodMs == 0u)
        ? APP_PREFS_WEB_ALIVE_PERIOD_DEFAULT_MS
        : AppPreferences_ClampWebAlivePeriodMs(record->webAlivePeriodMs);
    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV7(const AppPreferencesRecordV7_t *record)
{
    AppPreferences_t prefs = AppPreferences_Defaults();

    prefs.ext_profile = (record->ext_profile == (uint32_t)EXT_PROFILE_BEEPER_PB12)
        ? EXT_PROFILE_BEEPER_PB12
        : EXT_PROFILE_NRF24;
    prefs.wifiApFallbackEnabled = (record->flags & APP_PREFS_FLAG_AP_FALLBACK_DISABLED) == 0u;
    prefs.encoderInverted = (record->flags & APP_PREFS_FLAG_ENCODER_INV) != 0u;
    prefs.screenLockEnabled = (record->flags & APP_PREFS_FLAG_LOCK_EN) != 0u;
    prefs.screenLockTimeoutSec = AppPreferences_ClampScreenLockTimeoutSec(record->screenLockTimeoutSec);
    prefs.motorLeftMinPwm = AppPreferences_DecodeMotorMinPwm(record->flags, APP_PREFS_MOTOR_LEFT_MIN_PWM_SHIFT);
    prefs.motorRightMinPwm = AppPreferences_DecodeMotorMinPwm(record->flags, APP_PREFS_MOTOR_RIGHT_MIN_PWM_SHIFT);
    prefs.systemActionTimeoutSec = AppPreferences_DecodeSystemActionTimeoutSec(record->flags);

    prefs.mpuCalibration.valid = (record->flags & APP_PREFS_FLAG_MPU_CAL) != 0u;
    prefs.mpuCalibration.accelRollOffsetDeg = record->mpuAccelRollOffsetDeg;
    prefs.mpuCalibration.accelPitchOffsetDeg = record->mpuAccelPitchOffsetDeg;
    prefs.mpuCalibration.gyroXOffsetDps = record->mpuGyroXOffsetDps;
    prefs.mpuCalibration.gyroYOffsetDps = record->mpuGyroYOffsetDps;
    prefs.mpuCalibration.gyroZOffsetDps = record->mpuGyroZOffsetDps;
    prefs.mpuCalibration.gravityXMg = record->mpuGravityXMg;
    prefs.mpuCalibration.gravityYMg = record->mpuGravityYMg;
    prefs.mpuCalibration.gravityZMg = record->mpuGravityZMg;

    prefs.oledRotationDeg = AppPreferences_ClampOledRotationDeg(record->oledRotationDeg);
    prefs.oledAutoRotationEnabled = (record->flags & APP_PREFS_FLAG_OLED_AUTO_ROT) != 0u;
    prefs.unerRouterForwardingEnabled = (record->flags & APP_PREFS_FLAG_ROUTER_FWD_DISABLED) == 0u;

    prefs.lineBlackWidthMm = AppPreferences_ClampLineBlackWidthMm(record->lineBlackWidthMm);
    prefs.usbAlivePeriodMs = (record->usbAlivePeriodMs == 0u) ? APP_PREFS_USB_ALIVE_PERIOD_DEFAULT_MS : AppPreferences_ClampUsbAlivePeriodMs(record->usbAlivePeriodMs);

    prefs.pinLockEnabled = (record->flags & APP_PREFS_FLAG_PIN_LOCK_DISABLED) == 0u;

    prefs.webAlivePeriodMs = (record->webAlivePeriodMs == 0u)
        ? APP_PREFS_WEB_ALIVE_PERIOD_DEFAULT_MS
        : AppPreferences_ClampWebAlivePeriodMs(record->webAlivePeriodMs);
    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV8(const AppPreferencesRecordV8_t *record)
{
    AppPreferences_t prefs =
        AppPreferences_FromRecordV7((const AppPreferencesRecordV7_t *)record);

    prefs.balancePid.kp = AppPreferences_DecodeBalancePid(record->balanceKpX20);
    prefs.balancePid.ki = AppPreferences_DecodeBalancePid(record->balanceKiX20);
    prefs.balancePid.kd = AppPreferences_DecodeBalancePid(record->balanceKdX20);
    prefs.espNotifWatchdogLimit = record->espNotifWatchdogLimit;
    prefs.espNotifWatchdogEnabled = record->espNotifWatchdogEnabled;

    if (record->mpuSensorId != APP_PREFS_MPU_SENSOR_MPU9250) {
        prefs.mpuCalibration.valid = false;
    }
    return prefs;
}

static bool AppPreferences_RepairLoadedAp(AppPreferences_Handle *h, AppPreferences_t *prefs)
{
    const AppPreferences_t defaults = AppPreferences_Defaults();
    size_t ssidLength;
    size_t passwordLength;
    bool repaired = false;

    prefs->wifiApPasswordProtected =
        prefs->wifiApPasswordProtected ? true : false;
    ssidLength = AppPreferences_BoundedStringLength(
        prefs->wifiApSsid, APP_PREFS_WIFI_AP_SSID_MAX_LEN);
    passwordLength = AppPreferences_BoundedStringLength(
        prefs->wifiApPassword, APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN);

    if (ssidLength == 0u || ssidLength > APP_AP_SSID_MAX_BYTES) {
        memcpy(prefs->wifiApSsid, defaults.wifiApSsid,
               sizeof(prefs->wifiApSsid));
        repaired = true;
    }
    if (passwordLength > APP_AP_PASSWORD_MAX_BYTES ||
        (passwordLength > 0u && passwordLength < APP_AP_PASSWORD_MIN_BYTES)) {
        memcpy(prefs->wifiApPassword, defaults.wifiApPassword,
               sizeof(prefs->wifiApPassword));
        passwordLength = strlen(prefs->wifiApPassword);
        repaired = true;
    }
    if (prefs->wifiApPasswordProtected &&
        passwordLength < APP_AP_PASSWORD_MIN_BYTES) {
        memcpy(prefs->wifiApPassword, defaults.wifiApPassword,
               sizeof(prefs->wifiApPassword));
        repaired = true;
    }
    if (!h->storage->ipv4_valid(prefs->wifiApIpv4)) {
        memset(prefs->wifiApIpv4, 0, sizeof(prefs->wifiApIpv4));
        repaired = true;
    }

    return repaired;
}

static AppPreferences_t AppPreferences_FromRecordV9(AppPreferences_Handle *h, const AppPreferencesRecordV9_t *record)
{
    AppPreferences_t prefs =
        AppPreferences_FromRecordV8((const AppPreferencesRecordV8_t *)record);

    memcpy(prefs.wifiApSsid, record->wifiApSsid, sizeof(record->wifiApSsid));
    prefs.wifiApSsid[sizeof(record->wifiApSsid)] = '\0';
    memcpy(prefs.wifiApPassword, record->wifiApPassword,
           sizeof(record->wifiApPassword));
    prefs.wifiApPassword[APP_AP_PASSWORD_MAX_BYTES] = '\0';
    prefs.wifiApPasswordProtected =
        record->wifiApPasswordProtected ? true : false;
    memset(prefs.wifiApIpv4, 0, sizeof(prefs.wifiApIpv4));
    prefs.wifiApApplyPending = true;
    (void)AppPreferences_RepairLoadedAp(h, &prefs);
    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV11(AppPreferences_Handle *h, const AppPreferencesRecordV11_t *record)
{
    AppPreferences_t prefs =
        AppPreferences_FromRecordV8((const AppPreferencesRecordV8_t *)record);

    memcpy(prefs.wifiApSsid, record->wifiApSsid, sizeof(prefs.wifiApSsid));
    prefs.wifiApSsid[APP_AP_SSID_MAX_BYTES] = '\0';
    memcpy(prefs.wifiApPassword, record->wifiApPassword,
           sizeof(prefs.wifiApPassword));
    prefs.wifiApPassword[APP_AP_PASSWORD_MAX_BYTES] = '\0';
    prefs.wifiApPasswordProtected =
        record->wifiApPasswordProtected ? true : false;
    memcpy(prefs.wifiApIpv4, record->wifiApIpv4, sizeof(prefs.wifiApIpv4));
    prefs.wifiApApplyPending = record->wifiApApplyPending ? true : false;
    prefs.wifiAtProtocol = record->wifiAtProtocol <= 1u ? record->wifiAtProtocol : 0u;
    if (AppPreferences_RepairLoadedAp(h, &prefs)) {
        prefs.wifiApApplyPending = true;
        h->appPreferencesApRepairNeeded = true;
    }
    return prefs;
}

static AppPreferences_t AppPreferences_FromRecordV12(AppPreferences_Handle *h, const AppPreferencesRecordV12_t *record)
{
    AppPreferences_t prefs =
        AppPreferences_FromRecordV8((const AppPreferencesRecordV8_t *)record);

    memcpy(prefs.wifiApSsid, record->wifiApSsid, sizeof(prefs.wifiApSsid));
    prefs.wifiApSsid[APP_AP_SSID_MAX_BYTES] = '\0';
    memcpy(prefs.wifiApPassword, record->wifiApPassword,
           sizeof(prefs.wifiApPassword));
    prefs.wifiApPassword[APP_AP_PASSWORD_MAX_BYTES] = '\0';
    prefs.wifiApPasswordProtected =
        record->wifiApPasswordProtected ? true : false;
    memcpy(prefs.wifiApIpv4, record->wifiApIpv4, sizeof(prefs.wifiApIpv4));
    prefs.wifiApApplyPending = record->wifiApApplyPending ? true : false;
    prefs.wifiAtProtocol = record->wifiAtProtocol <= 1u ? record->wifiAtProtocol : 0u;
    if (AppPreferences_RepairLoadedAp(h, &prefs)) {
        prefs.wifiApApplyPending = true;
        h->appPreferencesApRepairNeeded = true;
    }
    prefs.lineCalibration = record->lineCalibration;
    return prefs;
}
static AppPreferences_t AppPreferences_FromRecord(AppPreferences_Handle *h, const AppPreferencesRecord_t *record)
{
    AppPreferences_t prefs =
        AppPreferences_FromRecordV8((const AppPreferencesRecordV8_t *)record);

    memcpy(prefs.wifiApSsid, record->wifiApSsid, sizeof(prefs.wifiApSsid));
    prefs.wifiApSsid[APP_AP_SSID_MAX_BYTES] = '\0';
    memcpy(prefs.wifiApPassword, record->wifiApPassword,
           sizeof(prefs.wifiApPassword));
    prefs.wifiApPassword[APP_AP_PASSWORD_MAX_BYTES] = '\0';
    prefs.wifiApPasswordProtected =
        record->wifiApPasswordProtected ? true : false;
    memcpy(prefs.wifiApIpv4, record->wifiApIpv4, sizeof(prefs.wifiApIpv4));
    prefs.wifiApApplyPending = record->wifiApApplyPending ? true : false;
    prefs.wifiAtProtocol = record->wifiAtProtocol <= 1u ? record->wifiAtProtocol : 0u;
    if (AppPreferences_RepairLoadedAp(h, &prefs)) {
        prefs.wifiApApplyPending = true;
        h->appPreferencesApRepairNeeded = true;
    }
    prefs.lineCalibration = record->lineCalibration;
    prefs.autoMotionProfile = record->autoMotionProfile;
    return prefs;
}
static AppPreferences_t AppPreferences_FromRecordV10(AppPreferences_Handle *h, const AppPreferencesRecordV10_t *record)
{
    AppPreferences_t prefs =
        AppPreferences_FromRecordV11(h, (const AppPreferencesRecordV11_t *)record);
    prefs.wifiAtProtocol = 0u;
    return prefs;
}


static AppPreferencesRecord_t AppPreferences_ToRecord(AppPreferences_Handle *h, const AppPreferences_t *prefs,
                                                      uint32_t sequence)
{
    AppPreferencesRecord_t record;

    memset(&record, 0, sizeof(record));
    record.magic = APP_PREFS_MAGIC;
    record.version = APP_PREFS_VERSION;
    record.size = sizeof(AppPreferencesRecord_t);
    record.sequence = sequence;
    record.ext_profile = (uint32_t)prefs->ext_profile;
    record.flags = 0u;
    if (prefs->encoderInverted) {
        record.flags |= APP_PREFS_FLAG_ENCODER_INV;
    }
    if (prefs->screenLockEnabled) {
        record.flags |= APP_PREFS_FLAG_LOCK_EN;
    }
    if (prefs->mpuCalibration.valid) {
        record.flags |= APP_PREFS_FLAG_MPU_CAL;
    }
    if (prefs->oledAutoRotationEnabled) {
        record.flags |= APP_PREFS_FLAG_OLED_AUTO_ROT;
    }
    if (!prefs->unerRouterForwardingEnabled) {
        record.flags |= APP_PREFS_FLAG_ROUTER_FWD_DISABLED;
    }
    if (!prefs->pinLockEnabled) {
        record.flags |= APP_PREFS_FLAG_PIN_LOCK_DISABLED;
    }
    if (!prefs->wifiApFallbackEnabled) {
        record.flags |= APP_PREFS_FLAG_AP_FALLBACK_DISABLED;
    }
    record.flags |= AppPreferences_EncodeMotorMinPwm(
        prefs->motorLeftMinPwm,
        APP_PREFS_MOTOR_LEFT_MIN_PWM_SHIFT);
    record.flags |= AppPreferences_EncodeMotorMinPwm(
        prefs->motorRightMinPwm,
        APP_PREFS_MOTOR_RIGHT_MIN_PWM_SHIFT);
    record.flags |= AppPreferences_EncodeSystemActionTimeoutSec(
        prefs->systemActionTimeoutSec);
    record.screenLockTimeoutSec =
        AppPreferences_ClampScreenLockTimeoutSec(prefs->screenLockTimeoutSec);
    record.wifiAtProtocol = prefs->wifiAtProtocol <= 1u ? prefs->wifiAtProtocol : 0u;
    record.mpuAccelRollOffsetDeg = prefs->mpuCalibration.accelRollOffsetDeg;
    record.mpuAccelPitchOffsetDeg = prefs->mpuCalibration.accelPitchOffsetDeg;
    record.mpuGyroXOffsetDps = prefs->mpuCalibration.gyroXOffsetDps;
    record.mpuGyroYOffsetDps = prefs->mpuCalibration.gyroYOffsetDps;
    record.mpuGyroZOffsetDps = prefs->mpuCalibration.gyroZOffsetDps;
    record.mpuGravityXMg = prefs->mpuCalibration.gravityXMg;
    record.mpuGravityYMg = prefs->mpuCalibration.gravityYMg;
    record.mpuGravityZMg = prefs->mpuCalibration.gravityZMg;
    record.oledRotationDeg = AppPreferences_ClampOledRotationDeg(prefs->oledRotationDeg);
    record.lineBlackWidthMm =
        AppPreferences_ClampLineBlackWidthMm(prefs->lineBlackWidthMm);
    record.usbAlivePeriodMs =
        AppPreferences_ClampUsbAlivePeriodMs(prefs->usbAlivePeriodMs);
    memcpy(record.authPinDigits, h->appAuthPinDigits, APP_PREFS_AUTH_PIN_DIGITS);
    record.webAlivePeriodMs =
        AppPreferences_ClampWebAlivePeriodMs(prefs->webAlivePeriodMs);
    record.espNotifWatchdogLimit = prefs->espNotifWatchdogLimit;
    record.espNotifWatchdogEnabled = prefs->espNotifWatchdogEnabled;
    record.balanceKpX20 = AppPreferences_EncodeBalancePid(prefs->balancePid.kp);
    record.balanceKiX20 = AppPreferences_EncodeBalancePid(prefs->balancePid.ki);
    record.balanceKdX20 = AppPreferences_EncodeBalancePid(prefs->balancePid.kd);
    record.mpuSensorId = APP_PREFS_MPU_SENSOR_MPU9250;
    
    memcpy(record.wifiApSsid, prefs->wifiApSsid, sizeof(record.wifiApSsid));
    memcpy(record.wifiApPassword, prefs->wifiApPassword, sizeof(record.wifiApPassword));
    record.wifiApPasswordProtected = prefs->wifiApPasswordProtected;
    memcpy(record.wifiApIpv4, prefs->wifiApIpv4, sizeof(record.wifiApIpv4));
    memcpy(record.favoriteScreenCodes, prefs->favoriteScreenCodes, sizeof(record.favoriteScreenCodes));
    record.wifiApApplyPending = prefs->wifiApApplyPending;
    
    record.lineCalibration = prefs->lineCalibration;
    record.autoMotionProfile = prefs->autoMotionProfile;
    record.checksum = AppPreferences_ChecksumBytes(
        &record, offsetof(AppPreferencesRecord_t, checksum));
    record.commit = APP_PREFS_COMMIT_WORD;

    return record;
}

static AM_Result AppPreferences_EraseSector(AppPreferences_Handle *h)
{
    return h->storage->erase(h->storage->ctx);
}

static AM_Result AppPreferences_WriteRecord(AppPreferences_Handle *h, uint32_t address,
                                                    const AppPreferencesRecord_t *record)
{
    const uint32_t *words = (const uint32_t *)record;

    for (uint8_t i = 0u; i < (uint8_t)(sizeof(*record) / sizeof(uint32_t)); i++) {
        if (h->storage->program(h->storage->ctx,
                              address + ((uint32_t)i * sizeof(uint32_t)),
                              words[i]) != AM_OK) {
            return AM_ERROR;
        }
    }

    return AM_OK;
}

static void AppPreferences_ScanFlash(AppPreferences_Handle *h, uint32_t baseAddr,
                                     uint32_t endAddr,
                                     AppPreferencesScanResult_t *result)
{
    bool found = false;
    uint16_t bestFormatVersion = 0u;
    uint32_t bestSequence = 0u;
    uint32_t firstFreeAddr = endAddr;
    AppPreferences_t bestPrefs = AppPreferences_Defaults();
    uint8_t bestAuthPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    uint32_t addr = baseAddr;

    AppPreferences_LoadDefaultAuthPin(bestAuthPinDigits);

    while (addr + sizeof(AppPreferencesRecordHeader_t) <= endAddr) {
        const AppPreferencesRecordHeader_t *header =
            (const AppPreferencesRecordHeader_t *)h->storage->view(h->storage->ctx,addr,0);
        uint16_t size;

        if (AppPreferences_IsErasedRange((const void *)h->storage->view(h->storage->ctx,addr,0),
                                         sizeof(AppPreferencesRecordHeader_t))) {
            if (firstFreeAddr == endAddr &&
                addr + sizeof(AppPreferencesRecord_t) <= endAddr &&
                AppPreferences_IsErasedRange((const void *)h->storage->view(h->storage->ctx,addr,0),
                                             sizeof(AppPreferencesRecord_t))) {
                firstFreeAddr = addr;
            }
            addr += sizeof(uint32_t);
            continue;
        }

        if (header->magic != APP_PREFS_MAGIC) {
            addr += sizeof(uint32_t);
            continue;
        }

        size = header->size;
        if ((size != sizeof(AppPreferencesRecordV1_t) &&
             size != sizeof(AppPreferencesRecordV2_t) &&
             size != sizeof(AppPreferencesRecordV3_t) &&
             size != sizeof(AppPreferencesRecordV4_t) &&
             size != sizeof(AppPreferencesRecordV5_t) &&
             size != sizeof(AppPreferencesRecordV6_t) &&
             size != sizeof(AppPreferencesRecordV7_t) &&
             size != sizeof(AppPreferencesRecordV8_t) &&
             size != sizeof(AppPreferencesRecordV9_t) &&
             size != sizeof(AppPreferencesRecordV11_t) &&
             size != sizeof(AppPreferencesRecord_t)) ||
            (addr + size > endAddr)) {
            addr += sizeof(uint32_t);
            continue;
        }

        if (header->version == APP_PREFS_VERSION_V1 &&
            size == sizeof(AppPreferencesRecordV1_t)) {
            const AppPreferencesRecordV1_t *record =
                (const AppPreferencesRecordV1_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV1IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV1(record);
                AppPreferences_LoadDefaultAuthPin(bestAuthPinDigits);
            }
        } else if (header->version == APP_PREFS_VERSION_V2 &&
                   size == sizeof(AppPreferencesRecordV2_t)) {
            const AppPreferencesRecordV2_t *record =
                (const AppPreferencesRecordV2_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV2IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV2(record);
                AppPreferences_LoadDefaultAuthPin(bestAuthPinDigits);
            }
        } else if (header->version == APP_PREFS_VERSION_V3 &&
                   size == sizeof(AppPreferencesRecordV3_t)) {
            const AppPreferencesRecordV3_t *record =
                (const AppPreferencesRecordV3_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV3IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV3(record);
                AppPreferences_LoadDefaultAuthPin(bestAuthPinDigits);
            }
        } else if (header->version == APP_PREFS_VERSION_V4 &&
                   size == sizeof(AppPreferencesRecordV4_t)) {
            const AppPreferencesRecordV4_t *record =
                (const AppPreferencesRecordV4_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV4IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV4(record);
                AppPreferences_LoadDefaultAuthPin(bestAuthPinDigits);
            }
        } else if (header->version == APP_PREFS_VERSION_V5 &&
                   size == sizeof(AppPreferencesRecordV5_t)) {
            const AppPreferencesRecordV5_t *record =
                (const AppPreferencesRecordV5_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV5IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV5(record);
                memcpy(bestAuthPinDigits, record->authPinDigits,
                       APP_PREFS_AUTH_PIN_DIGITS);
            }
        } else if (header->version == APP_PREFS_VERSION_V6 &&
                   size == sizeof(AppPreferencesRecordV6_t)) {
            const AppPreferencesRecordV6_t *record =
                (const AppPreferencesRecordV6_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV6IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV6(record);
                memcpy(bestAuthPinDigits, record->authPinDigits,
                       APP_PREFS_AUTH_PIN_DIGITS);
            }
        } else if (header->version == APP_PREFS_VERSION_V7 &&
                   size == sizeof(AppPreferencesRecordV7_t)) {
            const AppPreferencesRecordV7_t *record =
                (const AppPreferencesRecordV7_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV7IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV7(record);
                memcpy(bestAuthPinDigits, record->authPinDigits,
                       APP_PREFS_AUTH_PIN_DIGITS);
            }
        } else if (header->version == APP_PREFS_VERSION_V8 &&
                   size == sizeof(AppPreferencesRecordV8_t)) {
            const AppPreferencesRecordV8_t *record =
                (const AppPreferencesRecordV8_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV8IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV8(record);
                memcpy(bestAuthPinDigits, record->authPinDigits,
                       APP_PREFS_AUTH_PIN_DIGITS);
            }
        } else if (header->version == APP_PREFS_VERSION_V9 &&
                   size == sizeof(AppPreferencesRecordV9_t)) {
            const AppPreferencesRecordV9_t *record =
                (const AppPreferencesRecordV9_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV9IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV9(h, record);
                memcpy(bestAuthPinDigits, record->authPinDigits,
                       APP_PREFS_AUTH_PIN_DIGITS);
            }
        } else if (header->version == APP_PREFS_VERSION_V10 &&
                   size == sizeof(AppPreferencesRecordV10_t)) {
            const AppPreferencesRecordV10_t *record =
                (const AppPreferencesRecordV10_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordV10IsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecordV10(h, record);
                memcpy(bestAuthPinDigits, record->authPinDigits,
                       APP_PREFS_AUTH_PIN_DIGITS);
            }
        } else if (header->version == APP_PREFS_VERSION_V11 && size == sizeof(AppPreferencesRecordV11_t)) {
            const AppPreferencesRecordV11_t *record = (const AppPreferencesRecordV11_t *)h->storage->view(h->storage->ctx,addr,0);
            if (AppPreferences_RecordV11IsValid(record) && (!found || record->sequence > bestSequence)) {
                bestSequence=record->sequence; found=true; bestFormatVersion=header->version;
                bestPrefs=AppPreferences_FromRecordV11(h,record);
                memcpy(bestAuthPinDigits,record->authPinDigits,APP_PREFS_AUTH_PIN_DIGITS);
            }
        } else if (header->version == APP_PREFS_VERSION_V12 && size == sizeof(AppPreferencesRecordV12_t)) {
            const AppPreferencesRecordV12_t *record=(const AppPreferencesRecordV12_t *)h->storage->view(h->storage->ctx,addr,0);
            if(AppPreferences_RecordV12IsValid(record) && (!found || record->sequence>bestSequence)) {
                bestSequence=record->sequence;found=true;bestFormatVersion=header->version;
                bestPrefs=AppPreferences_FromRecordV12(h,record);
                memcpy(bestAuthPinDigits,record->authPinDigits,APP_PREFS_AUTH_PIN_DIGITS);
            }
        } else if (header->version == APP_PREFS_VERSION &&
                   size == sizeof(AppPreferencesRecord_t)) {
            const AppPreferencesRecord_t *record =
                (const AppPreferencesRecord_t *)h->storage->view(h->storage->ctx,addr,0);

            if (AppPreferences_RecordIsValid(record) &&
                (!found || record->sequence > bestSequence)) {
                bestSequence = record->sequence;
                found = true;
                bestFormatVersion = header->version;
                bestPrefs = AppPreferences_FromRecord(h, record);
                memcpy(bestAuthPinDigits,
                       record->authPinDigits,
                       APP_PREFS_AUTH_PIN_DIGITS);
            }
        }

        addr += size;
    }

    result->found = found;
    result->formatVersion = bestFormatVersion;
    result->sequence = bestSequence;
    result->firstFreeAddr = firstFreeAddr;
    result->preferences = bestPrefs;
    memcpy(result->authPinDigits, bestAuthPinDigits,
           APP_PREFS_AUTH_PIN_DIGITS);
}

static void AppPreferences_UseScanResult(AppPreferences_Handle *h, const AppPreferencesScanResult_t *result,
    AppPreferencesStorageSource_t source)
{
    h->appPreferences = result->preferences;
    memcpy(h->appAuthPinDigits, result->authPinDigits,
           APP_PREFS_AUTH_PIN_DIGITS);
    h->appPreferencesNextSequence = result->sequence + 1u;
    h->appPreferencesNextWriteAddr = result->firstFreeAddr;

    h->appPreferencesStorageInfo.source = source;
    h->appPreferencesStorageInfo.status = APP_PREFS_STORAGE_STATUS_READY;
    h->appPreferencesStorageInfo.formatVersion = result->formatVersion;
    h->appPreferencesStorageInfo.sequence = result->sequence;
}

static AM_Result AppPreferences_Save(AppPreferences_Handle *h)
{
    AM_Result status;
    bool writeAttempted = false;
    uint32_t writeAddress = APP_PREFS_FLASH_BASE_ADDR;
    AppPreferencesRecord_t record =
        AppPreferences_ToRecord(h, &h->appPreferences, h->appPreferencesNextSequence);

    status = h->storage->unlock(h->storage->ctx);
    if (status != AM_OK) {
        h->appPreferencesStorageInfo.status = APP_PREFS_STORAGE_STATUS_WRITE_FAILED;
        return status;
    }

    if (h->appPreferencesNextWriteAddr + sizeof(record) > APP_PREFS_FLASH_END_ADDR) {
        status = AppPreferences_EraseSector(h);
        if (status == AM_OK) {
            h->appPreferencesNextWriteAddr = APP_PREFS_FLASH_BASE_ADDR;
        }
    }

    if (status == AM_OK) {
        writeAttempted = true;
        writeAddress = h->appPreferencesNextWriteAddr;
        status = AppPreferences_WriteRecord(h, writeAddress, &record);
    }

    (void)h->storage->lock(h->storage->ctx);
    if (status == AM_OK) {
        const AppPreferencesRecord_t *writtenRecord =
            (const AppPreferencesRecord_t *)h->storage->view(h->storage->ctx,writeAddress,0);

        if (!AppPreferences_RecordIsValid(writtenRecord) ||
            memcmp(writtenRecord, &record, sizeof(record)) != 0) {
            status = AM_ERROR;
        }
    }

    if (status == AM_OK) {
        h->appPreferencesNextWriteAddr = writeAddress + sizeof(record);
        h->appPreferencesNextSequence = record.sequence + 1u;
        if (h->appPreferencesStorageInfo.source ==
            APP_PREFS_STORAGE_SOURCE_LEGACY_SECTOR7) {
            h->appPreferencesStorageInfo.source =
                APP_PREFS_STORAGE_SOURCE_MIGRATED_SECTOR7;
        } else if (h->appPreferencesStorageInfo.source ==
                   APP_PREFS_STORAGE_SOURCE_DEFAULTS) {
            h->appPreferencesStorageInfo.source = APP_PREFS_STORAGE_SOURCE_SECTOR1;
        }
        h->appPreferencesStorageInfo.status = APP_PREFS_STORAGE_STATUS_READY;
        h->appPreferencesStorageInfo.formatVersion = APP_PREFS_VERSION;
        h->appPreferencesStorageInfo.sequence = record.sequence;
    } else {
        h->appPreferencesStorageInfo.status = APP_PREFS_STORAGE_STATUS_WRITE_FAILED;
        if (writeAttempted) {
            /* A partially programmed flash slot cannot safely be reused. */
            h->appPreferencesNextWriteAddr = writeAddress + sizeof(record);
        }
    }

    return status;
}

static AM_Result AppPreferences_SaveTransaction(AppPreferences_Handle *h, const AppPreferences_t *nextPreferences,
    const uint8_t nextAuthPinDigits[APP_PREFS_AUTH_PIN_DIGITS])
{
    AppPreferences_t previousPreferences = h->appPreferences;
    uint8_t previousAuthPinDigits[APP_PREFS_AUTH_PIN_DIGITS];
    AM_Result status;

    memcpy(previousAuthPinDigits, h->appAuthPinDigits,
           APP_PREFS_AUTH_PIN_DIGITS);

    if (nextPreferences != NULL) {
        h->appPreferences = *nextPreferences;
    }
    if (nextAuthPinDigits != NULL) {
        memcpy(h->appAuthPinDigits, nextAuthPinDigits,
               APP_PREFS_AUTH_PIN_DIGITS);
    }

    status = AppPreferences_Save(h);
    if (status != AM_OK) {
        h->appPreferences = previousPreferences;
        memcpy(h->appAuthPinDigits, previousAuthPinDigits,
               APP_PREFS_AUTH_PIN_DIGITS);
    }

    memset(previousAuthPinDigits, 0, sizeof(previousAuthPinDigits));
    return status;
}

void AppPreferences_Init_H(AppPreferences_Handle *h)
{
    AppPreferencesScanResult_t primaryResult;
    AppPreferencesScanResult_t legacyResult;
    bool primaryErased;

    h->appPreferences = AppPreferences_Defaults();
    AppPreferences_LoadDefaultAuthPin(h->appAuthPinDigits);
    h->appPreferencesNextWriteAddr = APP_PREFS_FLASH_BASE_ADDR;
    h->appPreferencesNextSequence = 1u;
    h->appPreferencesApRepairNeeded = false;
    h->appPreferencesStorageInfo.source = APP_PREFS_STORAGE_SOURCE_DEFAULTS;
    h->appPreferencesStorageInfo.status = APP_PREFS_STORAGE_STATUS_DEFAULTS;
    h->appPreferencesStorageInfo.formatVersion = 0u;
    h->appPreferencesStorageInfo.sequence = 0u;

    AppPreferences_ScanFlash(h, APP_PREFS_FLASH_BASE_ADDR,
                             APP_PREFS_FLASH_END_ADDR,
                             &primaryResult);
    if (primaryResult.found) {
        AppPreferences_UseScanResult(h, &primaryResult,
                                     APP_PREFS_STORAGE_SOURCE_SECTOR1);
        if (primaryResult.formatVersion != APP_PREFS_VERSION ||
            h->appPreferencesApRepairNeeded) {
            h->appPreferences.wifiApApplyPending = true;
            if (AppPreferences_Save(h) != AM_OK) {
                h->appPreferencesStorageInfo.status =
                    APP_PREFS_STORAGE_STATUS_MIGRATION_FAILED;
            }
        }
        return;
    }

    primaryErased = AppPreferences_IsErasedRange(
        (const void *)h->storage->view(h->storage->ctx,APP_PREFS_FLASH_BASE_ADDR,0),
        APP_PREFS_FLASH_LOG_SIZE_BYTES);
    h->appPreferencesNextWriteAddr = primaryErased
        ? APP_PREFS_FLASH_BASE_ADDR
        : APP_PREFS_FLASH_END_ADDR;

    /* An empty legacy range means the adapter assigned that sector to code.
     * Never scan address zero or bytes belonging to the application. */
    if (APP_PREFS_LEGACY_FLASH_BASE_ADDR>=APP_PREFS_LEGACY_FLASH_END_ADDR) {
        (void)AppPreferences_Save(h); /* persist current defaults in sector 1 */
        return;
    }
    AppPreferences_ScanFlash(h, APP_PREFS_LEGACY_FLASH_BASE_ADDR,
                             APP_PREFS_LEGACY_FLASH_END_ADDR,
                             &legacyResult);
    if (!legacyResult.found) {
        (void)AppPreferences_Save(h); /* no old records: initialize sector 1 */
        return;
    }

    AppPreferences_UseScanResult(h, &legacyResult, APP_PREFS_STORAGE_SOURCE_LEGACY_SECTOR7);
    h->appPreferencesNextWriteAddr = primaryErased
        ? APP_PREFS_FLASH_BASE_ADDR
        : APP_PREFS_FLASH_END_ADDR;
    if (AppPreferences_Save(h) != AM_OK) {
        h->appPreferencesStorageInfo.status =
            APP_PREFS_STORAGE_STATUS_MIGRATION_FAILED;
    }
}

const AppPreferences_t *AppPreferences_Get_H(AppPreferences_Handle *h)
{
    return &h->appPreferences;
}

void AppPreferences_GetSnapshot_H(AppPreferences_Handle *h, AppPreferences_t *out)
{
    if (out != NULL) {
        *out = h->appPreferences;
    }
}

void AppPreferences_GetDefaults(AppPreferences_t *out)
{
    if (out != NULL) {
        *out = AppPreferences_Defaults();
    }
}

const AppPreferencesDescriptor_t *AppPreferences_GetDescriptors(uint8_t *count)
{
    if (count != NULL) {
        *count = (uint8_t)(sizeof(appPreferenceDescriptors) /
                           sizeof(appPreferenceDescriptors[0]));
    }

    return appPreferenceDescriptors;
}

bool AppPreferences_IsEncoderInverted_H(AppPreferences_Handle *h)
{
    return h->appPreferences.encoderInverted;
}

ext_profile_t AppPreferences_GetExtProfile_H(AppPreferences_Handle *h)
{
    return h->appPreferences.ext_profile;
}

AM_Result AppPreferences_SetExtProfile_H(AppPreferences_Handle *h, ext_profile_t ext_profile)
{
    if (ext_profile != EXT_PROFILE_NRF24 &&
        ext_profile != EXT_PROFILE_BEEPER_PB12) {
        return AM_ERROR;
    }
    AppPreferences_t prefs = h->appPreferences;
    prefs.ext_profile = ext_profile;
    return AppPreferences_SaveTransaction(h, &prefs, NULL);
}

bool AppPreferences_IsScreenLockEnabled_H(AppPreferences_Handle *h)
{
    return h->appPreferences.screenLockEnabled;
}

bool AppPreferences_IsEspNotifWatchdogEnabled_H(AppPreferences_Handle *h)
{
    return h->appPreferences.espNotifWatchdogEnabled;
}

uint8_t AppPreferences_GetEspNotifWatchdogLimit_H(AppPreferences_Handle *h)
{
    return h->appPreferences.espNotifWatchdogLimit;
}

const char* AppPreferences_GetWifiApSsid_H(AppPreferences_Handle *h)
{
    return h->appPreferences.wifiApSsid;
}

const char* AppPreferences_GetWifiApPassword_H(AppPreferences_Handle *h)
{
    return h->appPreferences.wifiApPassword;
}

bool AppPreferences_IsWifiApPasswordProtected_H(AppPreferences_Handle *h)
{
    return h->appPreferences.wifiApPasswordProtected;
}

void AppPreferences_GetWifiApConfig_H(AppPreferences_Handle *h, AppApConfig_t *out)
{
    if (out == NULL) {
        return;
    }

    memset(out, 0, sizeof(*out));
    memcpy(out->ssid, h->appPreferences.wifiApSsid, sizeof(out->ssid));
    memcpy(out->password, h->appPreferences.wifiApPassword, sizeof(out->password));
    out->protected = h->appPreferences.wifiApPasswordProtected;
    memcpy(out->ipv4, h->appPreferences.wifiApIpv4, sizeof(out->ipv4));
    out->applyState = h->appPreferences.wifiApApplyPending
        ? APP_AP_APPLY_PENDING
        : APP_AP_APPLY_SYNCED;
}

uint16_t AppPreferences_GetScreenLockTimeoutSec_H(AppPreferences_Handle *h)
{
    return h->appPreferences.screenLockTimeoutSec;
}

uint16_t AppPreferences_GetMotorLeftMinPwm_H(AppPreferences_Handle *h)
{
    return h->appPreferences.motorLeftMinPwm;
}

uint16_t AppPreferences_GetMotorRightMinPwm_H(AppPreferences_Handle *h)
{
    return h->appPreferences.motorRightMinPwm;
}

uint16_t AppPreferences_GetSystemActionTimeoutSec_H(AppPreferences_Handle *h)
{
    return h->appPreferences.systemActionTimeoutSec;
}

uint16_t AppPreferences_GetUsbAlivePeriodMs_H(AppPreferences_Handle *h)
{
    return h->appPreferences.usbAlivePeriodMs;
}

uint16_t AppPreferences_GetWebAlivePeriodMs_H(AppPreferences_Handle *h)
{
    return h->appPreferences.webAlivePeriodMs;
}
uint16_t AppPreferences_GetWifiAtProtocol_H(AppPreferences_Handle *h)
{
    return h->appPreferences.wifiAtProtocol;
}


uint16_t AppPreferences_GetOledRotationDeg_H(AppPreferences_Handle *h)
{
    return h->appPreferences.oledRotationDeg;
}

uint16_t AppPreferences_GetLineBlackWidthMm_H(AppPreferences_Handle *h)
{
    return h->appPreferences.lineBlackWidthMm;
}

bool AppPreferences_IsOledAutoRotationEnabled_H(AppPreferences_Handle *h)
{
    return h->appPreferences.oledAutoRotationEnabled;
}

bool AppPreferences_IsUnerRouterForwardingEnabled_H(AppPreferences_Handle *h)
{
    return h->appPreferences.unerRouterForwardingEnabled;
}

bool AppPreferences_IsPinLockEnabled_H(AppPreferences_Handle *h)
{
    return h->appPreferences.pinLockEnabled;
}

bool AppPreferences_IsWifiApFallbackEnabled_H(AppPreferences_Handle *h)
{
    return h->appPreferences.wifiApFallbackEnabled;
}

bool AppPreferences_GetMpuCalibration_H(AppPreferences_Handle *h, AppPreferencesMpuCalibration_t *out)
{
    if (out != NULL) {
        *out = h->appPreferences.mpuCalibration;
    }

    return h->appPreferences.mpuCalibration.valid;
}

void AppPreferences_GetBalancePid_H(AppPreferences_Handle *h, AppPreferencesBalancePid_t *out)
{
    if (out != NULL) {
        *out = h->appPreferences.balancePid;
    }
}

bool AppPreferences_VerifyAuthPin_H(AppPreferences_Handle *h, const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS])
{
    uint8_t difference = 0u;

    if (!AppPreferences_AuthPinFormatIsValid(pinDigits)) {
        return false;
    }

    for (uint8_t i = 0u; i < APP_PREFS_AUTH_PIN_DIGITS; ++i) {
        difference |= (uint8_t)(h->appAuthPinDigits[i] ^ pinDigits[i]);
    }

    return difference == 0u;
}

uint32_t AppPreferences_GetRecordSizeBytes(void)
{
    return (uint32_t)sizeof(AppPreferencesRecord_t);
}

uint32_t AppPreferences_GetRecordCapacity(void)
{
    return APP_PREFS_FLASH_LOG_SIZE_BYTES / (uint32_t)sizeof(AppPreferencesRecord_t);
}

void AppPreferences_GetStorageInfo_H(AppPreferences_Handle *h, AppPreferencesStorageInfo_t *out)
{
    if (out != NULL) {
        *out = h->appPreferencesStorageInfo;
    }
}

static void AppPreferences_Normalize(AppPreferences_t *prefs,
                                     AppPreferencesFieldMask_t fields)
{
    if (prefs == NULL) {
        return;
    }

    if ((fields & APP_PREFS_FIELD_ENCODER_INVERTED) != 0u) {
        prefs->encoderInverted = prefs->encoderInverted ? true : false;
    }
    if ((fields & APP_PREFS_FIELD_SCREEN_LOCK_ENABLED) != 0u) {
        prefs->screenLockEnabled = prefs->screenLockEnabled ? true : false;
    }
    if ((fields & APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT) != 0u) {
        prefs->screenLockTimeoutSec =
            AppPreferences_ClampScreenLockTimeoutSec(prefs->screenLockTimeoutSec);
    }
    if ((fields & APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM) != 0u) {
        prefs->motorLeftMinPwm =
            AppPreferences_ClampMotorMinPwm(prefs->motorLeftMinPwm);
    }
    if ((fields & APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM) != 0u) {
        prefs->motorRightMinPwm =
            AppPreferences_ClampMotorMinPwm(prefs->motorRightMinPwm);
    }
    if ((fields & APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT) != 0u) {
        prefs->systemActionTimeoutSec =
            AppPreferences_ClampSystemActionTimeoutSec(prefs->systemActionTimeoutSec);
    }
    if ((fields & APP_PREFS_FIELD_USB_ALIVE_PERIOD) != 0u) {
        prefs->usbAlivePeriodMs =
            AppPreferences_ClampUsbAlivePeriodMs(prefs->usbAlivePeriodMs);
    }
    if ((fields & APP_PREFS_FIELD_WEB_ALIVE_PERIOD) != 0u) {
        prefs->webAlivePeriodMs =
            AppPreferences_ClampWebAlivePeriodMs(prefs->webAlivePeriodMs);
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AT_PROTOCOL) != 0u) {
        prefs->wifiAtProtocol = prefs->wifiAtProtocol <= 1u ? prefs->wifiAtProtocol : 0u;
    }
    if ((fields & APP_PREFS_FIELD_OLED_ROTATION) != 0u) {
        prefs->oledRotationDeg =
            AppPreferences_ClampOledRotationDeg(prefs->oledRotationDeg);
    }
    if ((fields & APP_PREFS_FIELD_LINE_BLACK_WIDTH) != 0u) {
        prefs->lineBlackWidthMm =
            AppPreferences_ClampLineBlackWidthMm(prefs->lineBlackWidthMm);
    }
    if ((fields & APP_PREFS_FIELD_OLED_AUTO_ROTATION) != 0u) {
        prefs->oledAutoRotationEnabled =
            prefs->oledAutoRotationEnabled ? true : false;
    }
    if ((fields & APP_PREFS_FIELD_UNER_ROUTER_FORWARDING) != 0u) {
        prefs->unerRouterForwardingEnabled =
            prefs->unerRouterForwardingEnabled ? true : false;
    }
    if ((fields & APP_PREFS_FIELD_PIN_LOCK_ENABLED) != 0u) {
        prefs->pinLockEnabled = prefs->pinLockEnabled ? true : false;
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED) != 0u) {
        prefs->wifiApFallbackEnabled = prefs->wifiApFallbackEnabled ? true : false;
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED) != 0u) {
        prefs->wifiApPasswordProtected =
            prefs->wifiApPasswordProtected ? true : false;
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AP_APPLY_PENDING) != 0u) {
        prefs->wifiApApplyPending = prefs->wifiApApplyPending ? true : false;
    }
    if (((fields & APP_PREFS_FIELD_MPU_CALIBRATION) != 0u) &&
        !prefs->mpuCalibration.valid) {
        memset(&prefs->mpuCalibration, 0, sizeof(prefs->mpuCalibration));
    }
}

static AppPreferencesFieldMask_t AppPreferences_GetChangedFields(
    const AppPreferences_t *current,
    const AppPreferences_t *next,
    AppPreferencesFieldMask_t fields)
{
    AppPreferencesFieldMask_t changed = 0u;

    if (((fields & APP_PREFS_FIELD_ENCODER_INVERTED) != 0u) &&
        current->encoderInverted != next->encoderInverted) {
        changed |= APP_PREFS_FIELD_ENCODER_INVERTED;
    }
    if (((fields & APP_PREFS_FIELD_SCREEN_LOCK_ENABLED) != 0u) &&
        current->screenLockEnabled != next->screenLockEnabled) {
        changed |= APP_PREFS_FIELD_SCREEN_LOCK_ENABLED;
    }
    if (((fields & APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT) != 0u) &&
        current->screenLockTimeoutSec != next->screenLockTimeoutSec) {
        changed |= APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT;
    }
    if (((fields & APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM) != 0u) &&
        current->motorLeftMinPwm != next->motorLeftMinPwm) {
        changed |= APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM;
    }
    if (((fields & APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM) != 0u) &&
        current->motorRightMinPwm != next->motorRightMinPwm) {
        changed |= APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM;
    }
    if (((fields & APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT) != 0u) &&
        current->systemActionTimeoutSec != next->systemActionTimeoutSec) {
        changed |= APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT;
    }
    if (((fields & APP_PREFS_FIELD_USB_ALIVE_PERIOD) != 0u) &&
        current->usbAlivePeriodMs != next->usbAlivePeriodMs) {
        changed |= APP_PREFS_FIELD_USB_ALIVE_PERIOD;
    }
    if (((fields & APP_PREFS_FIELD_WEB_ALIVE_PERIOD) != 0u) &&
        current->webAlivePeriodMs != next->webAlivePeriodMs) {
        changed |= APP_PREFS_FIELD_WEB_ALIVE_PERIOD;
    }
    if (((fields & APP_PREFS_FIELD_WIFI_AT_PROTOCOL) != 0u) &&
        current->wifiAtProtocol != next->wifiAtProtocol) {
        changed |= APP_PREFS_FIELD_WIFI_AT_PROTOCOL;
    }
    if (((fields & APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_ENABLED) != 0u) &&
        current->espNotifWatchdogEnabled != next->espNotifWatchdogEnabled) {
        changed |= APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_ENABLED;
    }
    if (((fields & APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_LIMIT) != 0u) &&
        current->espNotifWatchdogLimit != next->espNotifWatchdogLimit) {
        changed |= APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_LIMIT;
    }
    if (((fields & APP_PREFS_FIELD_OLED_ROTATION) != 0u) &&
        current->oledRotationDeg != next->oledRotationDeg) {
        changed |= APP_PREFS_FIELD_OLED_ROTATION;
    }
    if (((fields & APP_PREFS_FIELD_LINE_BLACK_WIDTH) != 0u) &&
        current->lineBlackWidthMm != next->lineBlackWidthMm) {
        changed |= APP_PREFS_FIELD_LINE_BLACK_WIDTH;
    }
    if (((fields & APP_PREFS_FIELD_OLED_AUTO_ROTATION) != 0u) &&
        current->oledAutoRotationEnabled != next->oledAutoRotationEnabled) {
        changed |= APP_PREFS_FIELD_OLED_AUTO_ROTATION;
    }
    if (((fields & APP_PREFS_FIELD_UNER_ROUTER_FORWARDING) != 0u) &&
        current->unerRouterForwardingEnabled != next->unerRouterForwardingEnabled) {
        changed |= APP_PREFS_FIELD_UNER_ROUTER_FORWARDING;
    }
    if (((fields & APP_PREFS_FIELD_PIN_LOCK_ENABLED) != 0u) &&
        current->pinLockEnabled != next->pinLockEnabled) {
        changed |= APP_PREFS_FIELD_PIN_LOCK_ENABLED;
    }
    if (((fields & APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED) != 0u) &&
        current->wifiApFallbackEnabled != next->wifiApFallbackEnabled) {
        changed |= APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED;
    }
    if (((fields & APP_PREFS_FIELD_MPU_CALIBRATION) != 0u) &&
        !AppPreferences_MpuCalibrationEquals(&current->mpuCalibration,
                                             &next->mpuCalibration)) {
        changed |= APP_PREFS_FIELD_MPU_CALIBRATION;
    }
    if (((fields & APP_PREFS_FIELD_WIFI_AP_SSID) != 0u) &&
        strncmp(current->wifiApSsid, next->wifiApSsid, APP_PREFS_WIFI_AP_SSID_MAX_LEN) != 0) {
        changed |= APP_PREFS_FIELD_WIFI_AP_SSID;
    }
    if (((fields & APP_PREFS_FIELD_WIFI_AP_PASSWORD) != 0u) &&
        strncmp(current->wifiApPassword, next->wifiApPassword, APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN) != 0) {
        changed |= APP_PREFS_FIELD_WIFI_AP_PASSWORD;
    }
    if (((fields & APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED) != 0u) &&
        current->wifiApPasswordProtected != next->wifiApPasswordProtected) {
        changed |= APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED;
    }
    if (((fields & APP_PREFS_FIELD_WIFI_AP_IPV4) != 0u) &&
        memcmp(current->wifiApIpv4, next->wifiApIpv4,
               sizeof(current->wifiApIpv4)) != 0) {
        changed |= APP_PREFS_FIELD_WIFI_AP_IPV4;
    }
    if (((fields & APP_PREFS_FIELD_WIFI_AP_APPLY_PENDING) != 0u) &&
        current->wifiApApplyPending != next->wifiApApplyPending) {
        changed |= APP_PREFS_FIELD_WIFI_AP_APPLY_PENDING;
    }

    return changed;
}

AM_Result AppPreferences_ApplyPatch_H(AppPreferences_Handle *h, const AppPreferencesPatch_t *patch,
                                            AppPreferencesFieldMask_t *changedFields)
{
    AppPreferences_t next;
    AppPreferencesFieldMask_t fields;
    AppPreferencesFieldMask_t changed;

    if (changedFields != NULL) {
        *changedFields = 0u;
    }
    if (patch == NULL) {
        return AM_ERROR;
    }

    fields = patch->fields & APP_PREFS_FIELD_ALL;
    if (fields == 0u) {
        return AM_OK;
    }

    next = h->appPreferences;

    if ((fields & APP_PREFS_FIELD_ENCODER_INVERTED) != 0u) {
        next.encoderInverted = patch->values.encoderInverted;
    }
    if ((fields & APP_PREFS_FIELD_SCREEN_LOCK_ENABLED) != 0u) {
        next.screenLockEnabled = patch->values.screenLockEnabled;
    }
    if ((fields & APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT) != 0u) {
        next.screenLockTimeoutSec = patch->values.screenLockTimeoutSec;
    }
    if ((fields & APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM) != 0u) {
        next.motorLeftMinPwm = patch->values.motorLeftMinPwm;
    }
    if ((fields & APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM) != 0u) {
        next.motorRightMinPwm = patch->values.motorRightMinPwm;
    }
    if ((fields & APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT) != 0u) {
        next.systemActionTimeoutSec = patch->values.systemActionTimeoutSec;
    }
    if ((fields & APP_PREFS_FIELD_USB_ALIVE_PERIOD) != 0u) {
        next.usbAlivePeriodMs = patch->values.usbAlivePeriodMs;
    }
    if ((fields & APP_PREFS_FIELD_WEB_ALIVE_PERIOD) != 0u) {
        next.webAlivePeriodMs = patch->values.webAlivePeriodMs;
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AT_PROTOCOL) != 0u) {
        next.wifiAtProtocol = patch->values.wifiAtProtocol;
    }
    if ((fields & APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_ENABLED) != 0u) {
        next.espNotifWatchdogEnabled = patch->values.espNotifWatchdogEnabled;
    }
    if ((fields & APP_PREFS_FIELD_ESP_NOTIF_WATCHDOG_LIMIT) != 0u) {
        next.espNotifWatchdogLimit = patch->values.espNotifWatchdogLimit;
    }
    if ((fields & APP_PREFS_FIELD_OLED_ROTATION) != 0u) {
        next.oledRotationDeg = patch->values.oledRotationDeg;
    }
    if ((fields & APP_PREFS_FIELD_LINE_BLACK_WIDTH) != 0u) {
        next.lineBlackWidthMm = patch->values.lineBlackWidthMm;
    }
    if ((fields & APP_PREFS_FIELD_OLED_AUTO_ROTATION) != 0u) {
        next.oledAutoRotationEnabled = patch->values.oledAutoRotationEnabled;
    }
    if ((fields & APP_PREFS_FIELD_UNER_ROUTER_FORWARDING) != 0u) {
        next.unerRouterForwardingEnabled = patch->values.unerRouterForwardingEnabled;
    }
    if ((fields & APP_PREFS_FIELD_PIN_LOCK_ENABLED) != 0u) {
        next.pinLockEnabled = patch->values.pinLockEnabled;
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED) != 0u) {
        next.wifiApFallbackEnabled = patch->values.wifiApFallbackEnabled;
    }
    if ((fields & APP_PREFS_FIELD_MPU_CALIBRATION) != 0u) {
        next.mpuCalibration = patch->values.mpuCalibration;
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AP_SSID) != 0u) {
        strncpy(next.wifiApSsid, patch->values.wifiApSsid, APP_PREFS_WIFI_AP_SSID_MAX_LEN);
        next.wifiApSsid[APP_PREFS_WIFI_AP_SSID_MAX_LEN - 1] = '\0';
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AP_PASSWORD) != 0u) {
        strncpy(next.wifiApPassword, patch->values.wifiApPassword, APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN);
        next.wifiApPassword[APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN - 1] = '\0';
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED) != 0u) {
        next.wifiApPasswordProtected = patch->values.wifiApPasswordProtected;
    }
    if ((fields & APP_PREFS_FIELD_WIFI_AP_IPV4) != 0u) {
        memcpy(next.wifiApIpv4, patch->values.wifiApIpv4,
               sizeof(next.wifiApIpv4));
    }
    if ((fields & APP_PREFS_FIELD_FAVORITES) != 0u) {
        memcpy(next.favoriteScreenCodes, patch->values.favoriteScreenCodes, sizeof(next.favoriteScreenCodes));
    }

    if ((fields & APP_PREFS_FIELD_WIFI_AP_APPLY_PENDING) != 0u) {
        next.wifiApApplyPending = patch->values.wifiApApplyPending;
    }

    AppPreferences_Normalize(&next, fields);
    if ((fields & (APP_PREFS_FIELD_WIFI_AP_SSID |
                   APP_PREFS_FIELD_WIFI_AP_PASSWORD |
                   APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED)) != 0u &&
        !AppPreferences_WifiApConfigurationIsValid(&next)) {
        return AM_ERROR;
    }
    changed = AppPreferences_GetChangedFields(&h->appPreferences, &next, fields);
    if (changedFields != NULL) {
        *changedFields = changed;
    }
    if (changed == 0u) {
        return AM_OK;
    }

    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_ResetFields_H(AppPreferences_Handle *h, AppPreferencesFieldMask_t fields,
                                             AppPreferencesFieldMask_t *changedFields)
{
    AppPreferencesPatch_t patch;
    AppPreferences_t defaults = AppPreferences_Defaults();

    memset(&patch, 0, sizeof(patch));
    patch.fields = fields & APP_PREFS_FIELD_ALL;
    patch.values = h->appPreferences;

    if ((patch.fields & APP_PREFS_FIELD_ENCODER_INVERTED) != 0u) {
        patch.values.encoderInverted = defaults.encoderInverted;
    }
    if ((patch.fields & APP_PREFS_FIELD_SCREEN_LOCK_ENABLED) != 0u) {
        patch.values.screenLockEnabled = defaults.screenLockEnabled;
    }
    if ((patch.fields & APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT) != 0u) {
        patch.values.screenLockTimeoutSec = defaults.screenLockTimeoutSec;
    }
    if ((patch.fields & APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM) != 0u) {
        patch.values.motorLeftMinPwm = defaults.motorLeftMinPwm;
    }
    if ((patch.fields & APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM) != 0u) {
        patch.values.motorRightMinPwm = defaults.motorRightMinPwm;
    }
    if ((patch.fields & APP_PREFS_FIELD_SYSTEM_ACTION_TIMEOUT) != 0u) {
        patch.values.systemActionTimeoutSec = defaults.systemActionTimeoutSec;
    }
    if ((patch.fields & APP_PREFS_FIELD_USB_ALIVE_PERIOD) != 0u) {
        patch.values.usbAlivePeriodMs = defaults.usbAlivePeriodMs;
    }
    if ((patch.fields & APP_PREFS_FIELD_WEB_ALIVE_PERIOD) != 0u) {
        patch.values.webAlivePeriodMs = defaults.webAlivePeriodMs;
    }
    if ((patch.fields & APP_PREFS_FIELD_WIFI_AT_PROTOCOL) != 0u) {
        patch.values.wifiAtProtocol = defaults.wifiAtProtocol;
    }
    if ((patch.fields & APP_PREFS_FIELD_OLED_ROTATION) != 0u) {
        patch.values.oledRotationDeg = defaults.oledRotationDeg;
    }
    if ((patch.fields & APP_PREFS_FIELD_LINE_BLACK_WIDTH) != 0u) {
        patch.values.lineBlackWidthMm = defaults.lineBlackWidthMm;
    }
    if ((patch.fields & APP_PREFS_FIELD_OLED_AUTO_ROTATION) != 0u) {
        patch.values.oledAutoRotationEnabled = defaults.oledAutoRotationEnabled;
    }
    if ((patch.fields & APP_PREFS_FIELD_UNER_ROUTER_FORWARDING) != 0u) {
        patch.values.unerRouterForwardingEnabled = defaults.unerRouterForwardingEnabled;
    }
    if ((patch.fields & APP_PREFS_FIELD_PIN_LOCK_ENABLED) != 0u) {
        patch.values.pinLockEnabled = defaults.pinLockEnabled;
    }
    if ((patch.fields & APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED) != 0u) {
        patch.values.wifiApFallbackEnabled = defaults.wifiApFallbackEnabled;
    }
    if ((patch.fields & APP_PREFS_FIELD_MPU_CALIBRATION) != 0u) {
        patch.values.mpuCalibration = defaults.mpuCalibration;
    }
    if ((patch.fields & APP_PREFS_FIELD_WIFI_AP_SSID) != 0u) {
        strncpy(patch.values.wifiApSsid, defaults.wifiApSsid, APP_PREFS_WIFI_AP_SSID_MAX_LEN);
    }
    if ((patch.fields & APP_PREFS_FIELD_WIFI_AP_PASSWORD) != 0u) {
        strncpy(patch.values.wifiApPassword, defaults.wifiApPassword, APP_PREFS_WIFI_AP_PASSWORD_MAX_LEN);
    }
    if ((patch.fields & APP_PREFS_FIELD_WIFI_AP_PASSWORD_PROTECTED) != 0u) {
        patch.values.wifiApPasswordProtected = defaults.wifiApPasswordProtected;
    }
    if ((patch.fields & APP_PREFS_FIELD_FAVORITES) != 0u) {
        memcpy(patch.values.favoriteScreenCodes, defaults.favoriteScreenCodes, sizeof(defaults.favoriteScreenCodes));
    }

    return AppPreferences_ApplyPatch_H(h, &patch, changedFields);
}

AM_Result AppPreferences_SetEncoderInverted_H(AppPreferences_Handle *h, bool inverted)
{
    AppPreferences_t next;

    if (h->appPreferences.encoderInverted == inverted) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.encoderInverted = inverted;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetScreenLockEnabled_H(AppPreferences_Handle *h, bool enabled)
{
    AppPreferences_t next;

    if (h->appPreferences.screenLockEnabled == enabled) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.screenLockEnabled = enabled;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetScreenLockTimeoutSec_H(AppPreferences_Handle *h, uint16_t timeoutSec)
{
    AppPreferences_t next;

    timeoutSec = AppPreferences_ClampScreenLockTimeoutSec(timeoutSec);

    if (h->appPreferences.screenLockTimeoutSec == timeoutSec) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.screenLockTimeoutSec = timeoutSec;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetMotorLeftMinPwm_H(AppPreferences_Handle *h, uint16_t minPwm)
{
    AppPreferences_t next;

    minPwm = AppPreferences_ClampMotorMinPwm(minPwm);

    if (h->appPreferences.motorLeftMinPwm == minPwm) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.motorLeftMinPwm = minPwm;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetMotorRightMinPwm_H(AppPreferences_Handle *h, uint16_t minPwm)
{
    AppPreferences_t next;

    minPwm = AppPreferences_ClampMotorMinPwm(minPwm);

    if (h->appPreferences.motorRightMinPwm == minPwm) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.motorRightMinPwm = minPwm;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetSystemActionTimeoutSec_H(AppPreferences_Handle *h, uint16_t timeoutSec)
{
    AppPreferences_t next;

    timeoutSec = AppPreferences_ClampSystemActionTimeoutSec(timeoutSec);

    if (h->appPreferences.systemActionTimeoutSec == timeoutSec) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.systemActionTimeoutSec = timeoutSec;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetUsbAlivePeriodMs_H(AppPreferences_Handle *h, uint16_t periodMs)
{
    AppPreferences_t next;

    periodMs = AppPreferences_ClampUsbAlivePeriodMs(periodMs);

    if (h->appPreferences.usbAlivePeriodMs == periodMs) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.usbAlivePeriodMs = periodMs;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetWebAlivePeriodMs_H(AppPreferences_Handle *h, uint16_t periodMs)
{
    AppPreferences_t next;

    periodMs = AppPreferences_ClampWebAlivePeriodMs(periodMs);

    if (h->appPreferences.webAlivePeriodMs == periodMs) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.webAlivePeriodMs = periodMs;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}
AM_Result AppPreferences_SetWifiAtProtocol_H(AppPreferences_Handle *h, uint16_t protocol)
{
    AppPreferences_t next;

    protocol = protocol <= 1u ? protocol : 0u;
    if (h->appPreferences.wifiAtProtocol == protocol) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.wifiAtProtocol = protocol;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}


AM_Result AppPreferences_SetOledRotationDeg_H(AppPreferences_Handle *h, uint16_t rotationDeg)
{
    AppPreferences_t next;

    rotationDeg = AppPreferences_ClampOledRotationDeg(rotationDeg);

    if (h->appPreferences.oledRotationDeg == rotationDeg) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.oledRotationDeg = rotationDeg;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetWifiApSsid_H(AppPreferences_Handle *h, const char *ssid)
{
    AppApConfigPatch_t patch = {0};
    if (ssid == NULL) return AM_ERROR;
    patch.fieldMask = APP_AP_FIELD_SSID;
    strncpy(patch.values.ssid, ssid, APP_AP_SSID_MAX_BYTES);
    return h->storage->apply_ap_patch(&patch, NULL) == APP_AP_STATUS_OK
        ? AM_OK : AM_ERROR;
}

AM_Result AppPreferences_SetWifiApPassword_H(AppPreferences_Handle *h, const char *password)
{
    AppApConfigPatch_t patch = {0};
    if (password == NULL) return AM_ERROR;
    patch.fieldMask = APP_AP_FIELD_PASSWORD;
    strncpy(patch.values.password, password, APP_AP_PASSWORD_MAX_BYTES);
    return h->storage->apply_ap_patch(&patch, NULL) == APP_AP_STATUS_OK
        ? AM_OK : AM_ERROR;
}

AM_Result AppPreferences_SetWifiApPasswordProtected_H(AppPreferences_Handle *h, bool protected)
{
    AppApConfigPatch_t patch = {0};
    patch.fieldMask = APP_AP_FIELD_PROTECTED;
    patch.values.protected = protected;
    return h->storage->apply_ap_patch(&patch, NULL) == APP_AP_STATUS_OK
        ? AM_OK : AM_ERROR;
}
AM_Result AppPreferences_SetWifiApConfig_H(AppPreferences_Handle *h, const AppApConfig_t *config,
                                                 bool applyPending)
{
    AppPreferences_t next;

    if (config == NULL) {
        return AM_ERROR;
    }

    next = h->appPreferences;
    memcpy(next.wifiApSsid, config->ssid, sizeof(next.wifiApSsid));
    next.wifiApSsid[APP_AP_SSID_MAX_BYTES] = '\0';
    memcpy(next.wifiApPassword, config->password, sizeof(next.wifiApPassword));
    next.wifiApPassword[APP_AP_PASSWORD_MAX_BYTES] = '\0';
    next.wifiApPasswordProtected = config->protected ? true : false;
    memcpy(next.wifiApIpv4, config->ipv4, sizeof(next.wifiApIpv4));
    next.wifiApApplyPending = applyPending ? true : false;

    if (!AppPreferences_WifiApConfigurationIsValid(&next) ||
        !h->storage->ipv4_valid(next.wifiApIpv4)) {
        return AM_ERROR;
    }
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetWifiApApplyPending_H(AppPreferences_Handle *h, bool pending)
{
    AppPreferences_t next = h->appPreferences;

    pending = pending ? true : false;
    if (next.wifiApApplyPending == pending) {
        return AM_OK;
    }
    next.wifiApApplyPending = pending;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetLineBlackWidthMm_H(AppPreferences_Handle *h, uint16_t widthMm)
{
    AppPreferences_t next;

    widthMm = AppPreferences_ClampLineBlackWidthMm(widthMm);

    if (h->appPreferences.lineBlackWidthMm == widthMm) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.lineBlackWidthMm = widthMm;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetOledAutoRotationEnabled_H(AppPreferences_Handle *h, bool enabled)
{
    AppPreferences_t next;

    enabled = enabled ? true : false;

    if (h->appPreferences.oledAutoRotationEnabled == enabled) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.oledAutoRotationEnabled = enabled;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetUnerRouterForwardingEnabled_H(AppPreferences_Handle *h, bool enabled)
{
    AppPreferences_t next;

    enabled = enabled ? true : false;

    if (h->appPreferences.unerRouterForwardingEnabled == enabled) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.unerRouterForwardingEnabled = enabled;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

AM_Result AppPreferences_SetWifiApFallbackEnabled_H(AppPreferences_Handle *h, bool enabled)
{
    AppPreferencesPatch_t patch = {0};
    patch.fields = APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED;
    patch.values.wifiApFallbackEnabled = enabled;
    return AppPreferences_ApplyPatch_H(h, &patch, NULL);
}

AM_Result AppPreferences_SetPinLockEnabled_H(AppPreferences_Handle *h, bool enabled)
{
    AppPreferences_t next;

    enabled = enabled ? true : false;

    if (h->appPreferences.pinLockEnabled == enabled) {
        return AM_OK;
    }

    next = h->appPreferences;
    next.pinLockEnabled = enabled;
    return AppPreferences_SaveTransaction(h, &next, NULL);
}

static bool AppPreferences_MpuCalibrationEquals(const AppPreferencesMpuCalibration_t *a,
                                                const AppPreferencesMpuCalibration_t *b)
{
    return a->valid == b->valid &&
           a->accelRollOffsetDeg == b->accelRollOffsetDeg &&
           a->accelPitchOffsetDeg == b->accelPitchOffsetDeg &&
           a->gyroXOffsetDps == b->gyroXOffsetDps &&
           a->gyroYOffsetDps == b->gyroYOffsetDps &&
           a->gyroZOffsetDps == b->gyroZOffsetDps &&
           a->gravityXMg == b->gravityXMg &&
           a->gravityYMg == b->gravityYMg &&
           a->gravityZMg == b->gravityZMg;
}

AM_Result AppPreferences_SetMpuCalibration_H(AppPreferences_Handle *h, const AppPreferencesMpuCalibration_t *calibration)
{
    AppPreferences_t nextPreferences;
    AppPreferencesMpuCalibration_t next;

    if (calibration == NULL) {
        return AM_ERROR;
    }

    next = *calibration;
    next.valid = true;

    if (AppPreferences_MpuCalibrationEquals(&h->appPreferences.mpuCalibration, &next)) {
        return AM_OK;
    }

    nextPreferences = h->appPreferences;
    nextPreferences.mpuCalibration = next;
    nextPreferences.lineCalibration.track.valid = 0;
    nextPreferences.lineCalibration.track_white_revision = 0;
    return AppPreferences_SaveTransaction(h, &nextPreferences, NULL);
}

AM_Result AppPreferences_SetBalancePid_H(AppPreferences_Handle *h, float kp, float ki, float kd)
{
    AppPreferences_t nextPreferences;
    AppPreferencesBalancePid_t next;

    if (kp != kp || ki != ki || kd != kd ||
        kp < APP_PREFS_BALANCE_PID_MIN || kp > APP_PREFS_BALANCE_PID_MAX ||
        ki < APP_PREFS_BALANCE_PID_MIN || ki > APP_PREFS_BALANCE_PID_MAX ||
        kd < APP_PREFS_BALANCE_PID_MIN || kd > APP_PREFS_BALANCE_PID_MAX) {
        return AM_ERROR;
    }

    next.kp = AppPreferences_DecodeBalancePid(
        AppPreferences_EncodeBalancePid(kp));
    next.ki = AppPreferences_DecodeBalancePid(
        AppPreferences_EncodeBalancePid(ki));
    next.kd = AppPreferences_DecodeBalancePid(
        AppPreferences_EncodeBalancePid(kd));
    if (h->appPreferences.balancePid.kp == next.kp &&
        h->appPreferences.balancePid.ki == next.ki &&
        h->appPreferences.balancePid.kd == next.kd) {
        return AM_OK;
    }

    nextPreferences = h->appPreferences;
    nextPreferences.balancePid = next;
    return AppPreferences_SaveTransaction(h, &nextPreferences, NULL);
}

AM_Result AppPreferences_ClearMpuCalibration_H(AppPreferences_Handle *h)
{
    AppPreferences_t nextPreferences;
    AppPreferencesMpuCalibration_t empty;

    memset(&empty, 0, sizeof(empty));
    if (AppPreferences_MpuCalibrationEquals(&h->appPreferences.mpuCalibration, &empty)) {
        return AM_OK;
    }

    nextPreferences = h->appPreferences;
    nextPreferences.mpuCalibration = empty;
    nextPreferences.lineCalibration.track.valid = 0;
    nextPreferences.lineCalibration.track_white_revision = 0;
    return AppPreferences_SaveTransaction(h, &nextPreferences, NULL);
}

AM_Result AppPreferences_ChangeAuthPin_H(AppPreferences_Handle *h, const uint8_t currentDigits[APP_PREFS_AUTH_PIN_DIGITS],
    const uint8_t newDigits[APP_PREFS_AUTH_PIN_DIGITS])
{
    if (!AppPreferences_VerifyAuthPin_H(h, currentDigits) ||
        !AppPreferences_AuthPinFormatIsValid(newDigits)) {
        return AM_ERROR;
    }

    if (memcmp(h->appAuthPinDigits, newDigits, APP_PREFS_AUTH_PIN_DIGITS) == 0) {
        return AM_OK;
    }

    return AppPreferences_SaveTransaction(h, NULL, newDigits);
}

AM_Result AppPreferences_SetLineCalibration_H(AppPreferences_Handle *h, const LineCalibration_Store *store)
{
    if (!h || !store) return AM_ERROR;
    AppPreferences_t next=h->appPreferences;
    next.lineCalibration=*store;
    return AppPreferences_SaveTransaction(h,&next,NULL);
}

AM_Result AppPreferences_SetAutoMotionProfile_H(AppPreferences_Handle *h,const AutoMotion_Profile *profile) {
    if(!h || !profile)return AM_ERROR;
    AppPreferences_t next=h->appPreferences;next.autoMotionProfile=*profile;
    return AppPreferences_SaveTransaction(h,&next,NULL);
}
