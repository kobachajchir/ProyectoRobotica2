#ifndef SCREEN_CONTEXT_H
#define SCREEN_CONTEXT_H
#include "am_port.h"
#include "system.h"
#include "system_flags.h"
#include "types/menu_types.h"
#include "fonts.h"
#include "oled_handle.h"
#include "oled_utils.h"
#include "oled_canvas.h"
#include "osk_integration.h"
#include "screen_lock.h"
#include "encoder_types.h"
#include "motor_control_types.h"
#include "mpu9250_types.h"
#include "tcrt5000_types.h"
#include "balance_control_types.h"
#include "motion_control_types.h"
#include "line_follow_control_types.h"
#include "app_preferences_types.h"
#include "permissions_types.h"
#include "uner_handle_types.h"
#include "wifi_bridge_types.h"
#include "automicro_boot_contract.h"
typedef enum {
    OLED_SIMPLE_LAYOUT_LEFT_ICON_LARGE_2 = 0u,
    OLED_SIMPLE_LAYOUT_NO_ICON_LARGE_2,
    OLED_SIMPLE_LAYOUT_CENTER_ICON_LARGE_2,
    OLED_SIMPLE_LAYOUT_CENTER_ICON_SMALL_2,
} OledSimpleNotificationLayout;
typedef struct {
    ScreenCode_t screenCode;
    const uint8_t *icon;
    const char *line1;
    const char *line2;
    uint8_t iconW;
    uint8_t iconH;
    uint8_t layout;
    uint8_t iconFlags;
    uint8_t strikeCount;
} OledSimpleNotificationDef;
typedef struct {
    char text[WIFI_SSID_MAX_LEN + 1u];
    uint32_t startedMs;
    int16_t lastOffset;
    uint8_t x;
    uint8_t right;
} DashboardScrollLane;
typedef struct {
    RenderFunction       prevRenderFn;
    UserEventManagerFn   prevEventManagerFn;
    bool                 prevAllowPeriodicRefresh;
    uint8_t              prevInsideMenu;
    ScreenCode_t         prevScreenCode;
} OSK_SavedContext;
typedef enum {
    OLED_CANVAS_STATE_IDLE = 0u,
    OLED_CANVAS_STATE_RECEIVING,
    OLED_CANVAS_STATE_COMMIT_PENDING,
    OLED_CANVAS_STATE_DISPLAYING,
    OLED_CANVAS_STATE_DISPLAYED,
} OledCanvasState_t;
typedef enum {
    OLED_CANVAS_UI_NONE = 0u,
    OLED_CANVAS_UI_SHOW_READY,
    OLED_CANVAS_UI_EXIT_TO_MENU,
    OLED_CANVAS_UI_EXIT_TO_DASHBOARD,
} OledCanvasUiAction_t;
typedef struct {
    uint8_t valid;
    uint8_t transport_id;
    uint8_t destination_node;
    uint8_t payload[OLED_CANVAS_RESPONSE_BYTES];
} OledCanvasPendingResponse_t;
typedef struct {
    OledCanvasState_t state;
    uint8_t transport_id;
    uint8_t source_node;
    uint16_t transfer_id;
    uint16_t received_len;
    uint32_t expected_crc32;
    uint32_t computed_crc32;
    uint32_t last_activity_ms;
    uint32_t display_error_count_before;
    uint8_t framebuffer[OLED_CANVAS_FRAMEBUFFER_BYTES];

    uint8_t last_result_valid;
    uint8_t last_result_source_node;
    uint16_t last_result_transfer_id;
    uint8_t last_result_status;
    uint32_t last_result_crc32;

    uint8_t cancel_display_status;
    OledCanvasUiAction_t deferred_ui_action;
    OledCanvasPendingResponse_t pending_response;
    OledCanvasSendResponseFn send_response;
    void *send_ctx;
} OledCanvasContext_t;
typedef struct {
    const FontDef * oled_font;
    char  oled_wifi_connected_ssid[WIFI_SSID_MAX_LEN + 1u];
    uint8_t  oled_wifi_credentials_result_status;
    char  oled_remote_mode_notification_line[12];
    bool  oledNotificationsBlocked;
    bool  menu_scroll_active;
    uint32_t  menu_scroll_start_tick;
    const MenuItem * menu_scroll_current_item;
    uint8_t   scroll_text_x;
    uint8_t   scroll_text_y;
    uint8_t   scroll_max_chars;
    const char * scroll_text_ptr;
    uint16_t  scroll_text_len;
    int16_t   scroll_last_offset;
    DashboardScrollLane  dashboardScrollLanes[2];
    bool  dashboardScrollActive;
    uint8_t  appEspNotifWatchdogLimitDraft;
    bool  dashboardModeReportPending;
    bool  dashboardConnectivityDirty;
    bool  irValuesRawView;
    int8_t  irValuesSessionMode;
    bool  irValuesSavedGroup5;
    bool  irValuesSavedGroup3;
    bool  irValuesAmbientMode;
    bool  routeAlignmentDetailView;
    uint8_t  mpuValuesViewIndex;
    uint8_t  unerRouterTransportId;
    bool  unerRouterTxView;
    uint32_t  dashboardWebStatusRequestedAt;
    MenuItem  espResetModeItems[3];
    SubMenu  espResetModeMenu;
    bool  wifiApEnableProtectionAfterPassword;
    OSK_State         s_oskState;
    OSK_SavedContext  s_savedCtx;
    OSK_DrawHandle    s_drawHandle;
    bool              s_initialized;
    OSK_CompleteFn  s_userOnComplete;
    OSK_CancelFn    s_userOnCancel;
    void          * s_userCtx;
    bool  screenLockLocked;
    uint32_t  screenLockIdleTicks10ms;
    uint16_t  screenLockTimeoutDraftSec;
    RenderFunction  screenLockPreviousRenderFn;
    UserEventManagerFn  screenLockPreviousEventManagerFn;
    bool  screenLockPreviousAllowPeriodicRefresh;
    uint8_t  screenLockPreviousInsideMenu;
    OledCanvasContext_t  oled_canvas;
} ScreensState;
typedef struct {
    uint32_t (*now_ms)(void);
    bool (*route_emitters_on)(void);
    bool (*APP_EvaluateRoute)(TCRT5000_RouteSample_t *out);
    bool (*APP_LineCalibrationBusy)(void);
    void (*APP_ESPFirmwareRequestRequested)(void);
    void (*APP_ESPModeChangeCommandAccepted)(void);
    void (*APP_ESPStatusRefreshRequested)(void);
    bool (*APP_IsIrLineEnabled)(void);
    bool (*APP_IsIrObjectsEnabled)(void);
    uint8_t (*APP_MPU_GetCalibrationProgressPercent)(void);
    uint8_t (*APP_MPU_IsCalibrationDoneNoticeActive)(void);
    uint8_t (*APP_MPU_IsCalibrationPromptActive)(void);
    uint8_t (*APP_MPU_IsCalibrationRunning)(void);
    void (*About_UserEventManager)(UserEvent_t ev);
    AppApStatus_t (*AppApConfig_ApplyPatch)(const AppApConfigPatch_t *patch,
                                     uint8_t *changedMask);
    void (*AppApConfig_Get)(AppApConfig_t *out);
    AppApStatus_t (*AppApConfig_ParseIpv4)(const char *text, uint8_t out[4]);
    uint16_t (*AppPreferences_ClampScreenLockTimeoutSec)(uint16_t timeoutSec);
    uint8_t (*AppPreferences_GetEspNotifWatchdogLimit)(void);
    uint16_t (*AppPreferences_GetScreenLockTimeoutSec)(void);
    const char* (*AppPreferences_GetWifiApPassword)(void);
    const char* (*AppPreferences_GetWifiApSsid)(void);
    bool (*AppPreferences_IsScreenLockEnabled)(void);
    bool (*AppPreferences_IsWifiApFallbackEnabled)(void);
    AM_Result (*AppPreferences_SetScreenLockTimeoutSec)(uint16_t timeoutSec);
    const BalanceControl_Status_t * (*BalanceControl_GetStatus)(const BalanceControl_Handle *balance);
    float (*BalancePid_GetCurrentGainDraft)(void);
    const char * (*BalancePid_GetCurrentGainLabel)(void);
    ScreenCode_t (*BalancePid_GetCurrentGainScreenCode)(void);
    uint32_t (*BalancePid_GetLogAvailableBytes)(void);
    uint16_t (*BalancePid_GetLogDurationSec)(void);
    uint32_t (*BalancePid_GetLogEstimatedBytes)(void);
    void (*ClickCancelar_UserEventManager)(UserEvent_t ev);
    uint8_t (*Dashboard_GetActionTimeoutContext)(uint32_t *startedMs, uint32_t *timeoutMs);
    void (*Dashboard_ResetModeConfirmState)(void);
    const char* (*GenericConfirm_GetLine1)(void);
    const char* (*GenericConfirm_GetLine2)(void);
    uint32_t (*GenericConfirm_GetScreenCode)(void);
    void (*IRValues_UserEventManager)(UserEvent_t ev);
    AM_Result (*MPU9250_GetLatest)(MPU9250_Handle_t *hmpu, MPU9250_Snapshot_t *out);
    void (*MPUValues_UserEventManager)(UserEvent_t ev);
    ScreenCode_t (*MenuSys_GetCurrentScreenCode)(const MenuSystem *ms);
    uint8_t (*MenuSys_GetCurrentScreenSource)(const MenuSystem *ms);
    bool (*MenuSys_IsFavorite)(const MenuItem *item);
    bool (*MenuSys_IsFavoritesMenu)(const SubMenu *menu);
    bool (*MenuSys_IsItemVisible)(const MenuItem *item);
    void (*MenuSys_NavigateBack)(MenuSystem *ms);
    void (*MenuSys_RenderMenu)(MenuSystem *ms);
    void (*MenuSys_SetCurrentMenuScreenCode)(MenuSystem *ms);
    void (*MenuSys_SetCurrentScreenCode)(MenuSystem *ms, ScreenCode_t screen_code, ScreenReportSource_t source);
    const MotionControl_Status_t * (*MotionControl_GetStatus)(const MotionControl_Handle *motion);
    uint8_t (*MotionControl_IsArmed)(const MotionControl_Handle *motion);
    const char * (*MotorSingleTest_GetDirectionLabel)(void);
    const char * (*MotorSingleTest_GetLabel)(void);
    uint16_t (*MotorSingleTest_GetMaxPwm)(void);
    uint16_t (*MotorSingleTest_GetMinPwm)(void);
    uint16_t (*MotorSingleTest_GetPwm)(void);
    ScreenCode_t (*MotorSingleTest_GetScreenCode)(void);
    uint8_t (*MotorSingleTest_GetSpeedPercentDisplay)(void);
    uint8_t (*Motor_SpeedPercentToDisplay)(float speed_percent);
    const char * (*OSK_GetText)(const OSK_State *state);
    bool (*OSK_IsActive)(const OSK_State *state);
    void (*OSK_OnBack)(OSK_State *state);
    void (*OSK_OnLeft)(OSK_State *state);
    void (*OSK_OnRight)(OSK_State *state);
    void (*OSK_OnSelect)(OSK_State *state);
    void (*OSK_Open)(OSK_State *state, const OSK_Config *cfg);
    void (*OSK_Render)(OSK_State *state);
    bool (*Permission_IsRemoteNodeAuthorizedNow)(uint8_t source_node);
    bool (*Permission_Request)(PermissionId_t permission,
                        PermissionContinuationFn on_granted,
                        void *ctx);
    uint16_t (*Preferences_GetLineBlackWidthDraftMm)(void);
    uint16_t (*Preferences_GetMotorMinPwmDraft)(void);
    const char * (*Preferences_GetMotorMinPwmEditLabel)(void);
    ScreenCode_t (*Preferences_GetMotorMinPwmEditScreenCode)(void);
    uint16_t (*Preferences_GetOledRotationDraftDeg)(void);
    uint16_t (*Preferences_GetOledRotationMpuTestDeg)(void);
    int16_t (*Preferences_GetOledRotationMpuTestYawDeg)(void);
    uint16_t (*Preferences_GetSystemActionTimeoutDraft)(void);
    uint16_t (*Preferences_GetUsbAlivePeriodDraftMs)(void);
    uint8_t (*Preferences_IsOledRotationMpuTestReady)(void);
    void (*Preferences_UpdateOledRotationFromMpuTest)(void);
    void (*ReadOnly_UserEventManager)(UserEvent_t ev);
    void (*RouteAlignment_UserEventManager)(UserEvent_t ev);
    bool (*TCRT5000_EmittersGroup3IsOn)(TCRT5000_Handle *h);
    void (*TCRT5000_EmittersGroup3Off)(TCRT5000_Handle *h);
    void (*TCRT5000_EmittersGroup3On)(TCRT5000_Handle *h);
    void (*TCRT5000_EmittersGroup3Set)(TCRT5000_Handle *h, bool enabled);
    void (*TCRT5000_EmittersGroup5Off)(TCRT5000_Handle *h);
    void (*TCRT5000_EmittersGroup5On)(TCRT5000_Handle *h);
    void (*TCRT5000_EmittersGroup5Set)(TCRT5000_Handle *h, bool enabled);
    const TCRT5000_SensorDef_t * (*TCRT5000_GetLineSensorDef)(TCRT5000_LineSensor_t sensor);
    const TCRT5000_SensorDef_t * (*TCRT5000_GetObjectSensorDef)(TCRT5000_ObjectSensor_t sensor);
    uint16_t (*TCRT5000_GetRaw)(const volatile uint16_t *sensorData,
                         const TCRT5000_SensorDef_t *sensor);
    uint16_t (*TCRT5000_GetRawById)(const volatile uint16_t *sensorData,
                             TCRT5000_SensorId_t sensorId);
    const char * (*TCRT5000_GetRouteAlignmentText)(TCRT5000_RouteAlignment_t alignment);
    uint16_t (*TCRT5000_NormalizeForDisplay)(uint16_t raw, TCRT5000_Polarity_t polarity);
    void (*TCRT5000_ReadRouteSensorsEx)(const volatile uint16_t *sensorData,
                                 bool irLightsOn,
                                 const TCRT5000_LineConfig_t *config,
                                 TCRT5000_RouteSample_t *sample);
    uint8_t (*UNER_App_GetBootloaderVersion)(uint32_t *version);
    void (*UNER_App_GetRouterPacketSnapshot)(uint8_t tx_view,
                                      uint8_t transport_id,
                                      UNER_RouterSnapshot *out);
    UNER_Status (*UNER_App_ReportNotificationChanged)(uint32_t screen_code,
                                               uint8_t source,
                                               uint16_t remaining_ms,
                                               uint16_t total_ms);
    void (*UnerRouterTest_UserEventManager)(UserEvent_t ev);
    uint8_t (*WiFiAtPassword_GetLength)(void);
    char (*WiFiAtPassword_GetSelectedCharacter)(void);
    void (*WiFiAtPassword_UserEventManager)(UserEvent_t ev);
    void (*WiFiDetails_UserEventManager)(UserEvent_t ev);
    void (*WiFiSearch_UserEventManager)(UserEvent_t ev);
    WifiBridge_Backend (*WifiBridge_GetActiveBackend)(const WifiBridge_Handle *h);
    const char * (*WifiBridge_GetBackendLabel)(const WifiBridge_Handle *h);
    const WifiBridge_Credentials * (*WifiBridge_GetRuntimeCredentials)(void);
    WifiBridge_Result (*WifiBridge_GetStatus)(WifiBridge_Handle *h);
    uint8_t (*WifiBridge_IsConnected)(const WifiBridge_Handle *h);
    uint8_t (*WifiBridge_IsWaitingValidation)(WifiBridge_Handle *h);
    WifiBridge_Result (*WifiBridge_ReportCarModeChanged)(WifiBridge_Handle *h, uint8_t mode);
    WifiBridge_Result (*WifiBridge_SendCommand)(WifiBridge_Handle *h,
                                         uint8_t cmd,
                                         const uint8_t *payload,
                                         uint8_t len);
    WifiBridge_Result (*WifiBridge_SendEspReboot)(WifiBridge_Handle *h, uint8_t boot_mode);
    WifiBridge_Result (*WifiBridge_StartAP)(WifiBridge_Handle *h);
    WifiBridge_Result (*WifiBridge_StartScan)(WifiBridge_Handle *h);
    WifiBridge_Result (*WifiBridge_StopAP)(WifiBridge_Handle *h);
    void (*balancePidGainEventManager)(UserEvent_t ev);
    void (*balancePidLogEventManager)(UserEvent_t ev);
    void (*balancePidStatusEventManager)(UserEvent_t ev);
    void (*dashboardEventManager)(UserEvent_t ev);
    void (*dashboardQuickActionEventManager)(UserEvent_t ev);
    void (*lineBlackWidthEventManager)(UserEvent_t ev);
    void (*menuEventManager)(UserEvent_t ev);
    void (*motorMinPwmEventManager)(UserEvent_t ev);
    void (*motorSingleTestEventManager)(UserEvent_t ev);
    void (*motorTestEventManager)(UserEvent_t ev);
    void (*mpuCalibrationEventManager)(UserEvent_t ev);
    void (*oledCanvasEventManager)(UserEvent_t ev);
    void (*oledRotationMpuTestEventManager)(UserEvent_t ev);
    void (*oledRotationTestEventManager)(UserEvent_t ev);
    void (*screenLockTimeoutEventManager)(UserEvent_t ev);
    void (*screenLockedEventManager)(UserEvent_t ev);
    void (*ssd1306_Clear)(void);
    void (*ssd1306_DrawBitmap)(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP);
    void (*ssd1306_DrawBitmap_MSB)(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP);
    void (*ssd1306_DrawHorizontalLine)(int16_t x, int16_t y, int16_t length);
    void (*ssd1306_DrawLine)(int16_t x0, int16_t y0, int16_t x1, int16_t y1);
    void (*ssd1306_DrawLinearProgress)(uint8_t x,
                                uint8_t y,
                                uint8_t width,
                                uint8_t height,
                                uint32_t elapsed,
                                uint32_t total);
    void (*ssd1306_DrawPixel)(uint8_t x, uint8_t y);
    void (*ssd1306_DrawRect)(int16_t x, int16_t y, int16_t width, int16_t height);
    void (*ssd1306_DrawVerticalLine)(int16_t x, int16_t y, int16_t length);
    void (*ssd1306_FillRect)(int16_t xMove, int16_t yMove, int16_t width, int16_t height);
    uint8_t (*ssd1306_GetDefaultContrast)(void);
    uint32_t (*ssd1306_GetErrorCount)(void);
    AM_Result (*ssd1306_PresentFramebuffer)(const uint8_t *framebuffer,
                                             uint16_t len);
    AM_Result (*ssd1306_RequestPageUpdate)(uint8_t page,
                                            I2C_RequestPrio prio,
                                            uint8_t keep_bus);
    void (*ssd1306_SetColor)(SSD1306_COLOR color);
    AM_Result (*ssd1306_SetContrast)(uint8_t contrast);
    void (*ssd1306_SetCursor)(uint8_t x, uint8_t y);
    AM_Result (*ssd1306_SetRotation)(OledRotation_t rotation);
    void (*ssd1306_UpdateScreen)(void);
    char (*ssd1306_UpdateScreenCompleted)(void);
    char (*ssd1306_WriteChar)(char ch, FontDef Font);
    char (*ssd1306_WriteString)(char* str, FontDef Font);
    void (*systemActionTimeoutEventManager)(UserEvent_t ev);
    void (*usbAlivePeriodEventManager)(UserEvent_t ev);
    void (*wifiEventManager)(UserEvent_t ev);
} ScreensServices;
typedef struct {
    SystemHandle *system;
    const ScreensServices *services;
    ScreensState state;
    MenuSystem *menuSystem;
    ENC_Handle_t *encoder;
    WifiBridge_Handle *wifiBridge;
    TCRT5000_Handle *irSensors;
    MotorControl_Handle *motorTask;
    BalanceControl_Handle *balanceControl;
    MotionControl_Handle *motionControl;
    LineFollowControl_Handle *lineFollowControl;
    MPU9250_Handle_t *mpuTask;
    SubMenu *mainMenu;
    SubMenu *submenu1;
    SubMenu *submenuESP;
    SubMenu *submenuSensorsDisplay;
    SubMenu *wifiResultsMenu;
    MenuItem *wifiResultsItems;
    OledHandle *oledHandle;
    const char *buildDate, *buildTime;
} Screens_Handle;
/* The application supplies the active context for the legacy zero-argument callbacks. */
extern Screens_Handle *Screens_Context;
void Screens_Init_oled_utils(void);
void Screens_Init_screenWrappers(void);
void Screens_Init_notificationWrappers(void);
void Screens_Init_osk_integration(void);
void Screens_Init_screen_lock(void);
void Screens_Init_oled_canvas(void);
#endif
