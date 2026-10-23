#include "app_tasks.h"
#include "app_stm32_ports.h"
#include "app_i2c_ports.h"
#include "app_control_ports.h"
#include "app_menu_services.h"
#include "app_wifi_ports.h"
#include "app_screens_ports.h"
#include "app_auxiliary_ports.h"
extern Screens_Handle appScreens;

#include <math.h>
#include <string.h>

#include "app_preferences_binding.h"
#include "eventManagers.h"
#include "globals.h"
#include "line_follow_control.h"
#include "main.h"
#include "menu_definitions.h"
#include "app_menu_binding.h"
#include "motion_estimator.h"
#include "mpu9250.h"
#include "menu_favorites.h"
#include "oled_canvas.h"
#include "oled_utils.h"
#include "permissions.h"
#include "screen_codes.h"
#include "screen_lock.h"
#include "screenWrappers.h"
#include "app_display.h"
#include "tcrt5000.h"
#include "ui_event_router.h"
#include "uner_app.h"
#include "usbd_cdc_if.h"
#include "usb_mode_manager.h"
#include "utils/macros_utils.h"
#include "app_wifi_binding.h"

#define APP_TIM3_TICKS_10MS     40U
#define APP_MPU_TICKS_5MS       20U
#define APP_OLED_REFRESH_TICKS  10U
#define APP_MPU_CAL_PROMPT_MS   2500U
#define APP_MPU_CAL_DONE_MS     1800U
#define APP_UNER_FORWARDING_NOTIFICATION_MS 2000U
#define APP_ESP_POST_INIT_REBOOT_RETRY_MS         50U
#define APP_ESP_POST_INIT_REBOOT_GRACE_MS      10000U
#define APP_ESP_STARTUP_GRACE_MS                5000U
#define APP_OLED_AUTO_ROTATION_TICKS          10U
#define APP_OLED_AUTO_ROTATION_LATERAL_ENTER_DEG 75.0f
#define APP_OLED_AUTO_ROTATION_LATERAL_EXIT_DEG  60.0f
#define APP_OLED_AUTO_ROTATION_Z_NEG_TRIGGER_DEG (-70.0f)
#define APP_OLED_AUTO_ROTATION_Z_POS_TRIGGER_DEG   70.0f

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart1;

UserButton_Handle_t btnUser;
ENC_Handle_t encoder;
I2C_ManagerHandle i2cManager;









































typedef enum {
    APP_MPU_CAL_UI_IDLE = 0,
    APP_MPU_CAL_UI_WAIT_PROMPT,
    APP_MPU_CAL_UI_RUNNING,
    APP_MPU_CAL_UI_DONE
} AppMpuCalibrationUiState;




typedef struct {
    PidController_Handle balancePid;
    PidController_Handle linePid;
    AppI2cBinding oledBusBinding;
    AppI2cBinding mpuBusBinding;
    volatile uint8_t appInputReady;
    volatile uint8_t appTim3Tick10ms;
    volatile uint8_t appMpuTick250us;
    volatile uint8_t appUiTick10msPending;
    volatile uint8_t appMotionTick5msPending;
    uint8_t appRemoteMotorTestActive;
    uint32_t appRemoteMotorTestUntilMs;
    volatile uint8_t appLineTick10msPending;
    uint8_t appDisplayReady;
    uint8_t appMpuReady;
    uint8_t appMpuCalibrationSavePending;
    uint8_t appOled10msCounter;
    uint8_t appOledAutoRotation10msCounter;
    uint8_t appOledAutoRotationInTiltZone;
    uint8_t appOledAutoRotationLastEnabled;
    uint8_t appEspPostInitRebootDisconnectObserved;
    uint32_t appLastEspAliveMs;
    WifiBridge_Backend appLastEspAliveBackend;
    uint8_t appEspAliveBackendInitialized;
    uint8_t appEspNotifWatchdogCounter;
    uint32_t appEspPostInitRebootLastAttemptMs;
    uint32_t appEspPostInitRebootGraceUntilMs;
    uint32_t appUsbAliveLastMs;
    uint32_t appUsbAliveCount;
    uint32_t appMpuCalibrationPromptStartedMs;
    uint32_t appMpuCalibrationDoneStartedMs;
    uint8_t appLineFollowWasActive;
    uint8_t appUsbDtrLast;
    uint8_t appStmStartupComplete;
    uint8_t appEspPostInitRebootPending;
    uint8_t appAtModeChangePending;
    uint8_t appAtConnectCommandPending;
    uint8_t appWifiApFallbackSyncConfirmed;
    uint8_t appWifiApFallbackSyncedValue;
    uint32_t appWifiApFallbackNextSyncMs;
    uint8_t appAtConnectPending;
    uint8_t appWifiStatusRefreshPending;
    uint8_t appAtFirmwareRequestPending;
    uint32_t appEspStartupGraceUntilMs;
    AppMpuCalibrationUiState appMpuCalibrationUiState;
} AppTasksState;
static AppTasksState appTasksState={
    .appEspPostInitRebootDisconnectObserved=0u,
    .appLastEspAliveMs=0u,
    .appLastEspAliveBackend=WIFI_BRIDGE_BACKEND_DETECTING,
    .appEspAliveBackendInitialized=0u,
    .appEspNotifWatchdogCounter=0u,
};
#define balancePid (appTasksState.balancePid)
#define linePid (appTasksState.linePid)
#define oledBusBinding (appTasksState.oledBusBinding)
#define mpuBusBinding (appTasksState.mpuBusBinding)
#define appInputReady (appTasksState.appInputReady)
#define appTim3Tick10ms (appTasksState.appTim3Tick10ms)
#define appMpuTick250us (appTasksState.appMpuTick250us)
#define appUiTick10msPending (appTasksState.appUiTick10msPending)
#define appMotionTick5msPending (appTasksState.appMotionTick5msPending)
#define appRemoteMotorTestActive (appTasksState.appRemoteMotorTestActive)
#define appRemoteMotorTestUntilMs (appTasksState.appRemoteMotorTestUntilMs)
#define appLineTick10msPending (appTasksState.appLineTick10msPending)
#define appDisplayReady (appTasksState.appDisplayReady)
#define appMpuReady (appTasksState.appMpuReady)
#define appMpuCalibrationSavePending (appTasksState.appMpuCalibrationSavePending)
#define appOled10msCounter (appTasksState.appOled10msCounter)
#define appOledAutoRotation10msCounter (appTasksState.appOledAutoRotation10msCounter)
#define appOledAutoRotationInTiltZone (appTasksState.appOledAutoRotationInTiltZone)
#define appOledAutoRotationLastEnabled (appTasksState.appOledAutoRotationLastEnabled)
#define appEspPostInitRebootDisconnectObserved (appTasksState.appEspPostInitRebootDisconnectObserved)
#define appLastEspAliveMs (appTasksState.appLastEspAliveMs)
#define appLastEspAliveBackend (appTasksState.appLastEspAliveBackend)
#define appEspAliveBackendInitialized (appTasksState.appEspAliveBackendInitialized)
#define appEspNotifWatchdogCounter (appTasksState.appEspNotifWatchdogCounter)
#define appEspPostInitRebootLastAttemptMs (appTasksState.appEspPostInitRebootLastAttemptMs)
#define appEspPostInitRebootGraceUntilMs (appTasksState.appEspPostInitRebootGraceUntilMs)
#define appUsbAliveLastMs (appTasksState.appUsbAliveLastMs)
#define appUsbAliveCount (appTasksState.appUsbAliveCount)
#define appMpuCalibrationPromptStartedMs (appTasksState.appMpuCalibrationPromptStartedMs)
#define appMpuCalibrationDoneStartedMs (appTasksState.appMpuCalibrationDoneStartedMs)
#define appLineFollowWasActive (appTasksState.appLineFollowWasActive)
#define appUsbDtrLast (appTasksState.appUsbDtrLast)
#define appStmStartupComplete (appTasksState.appStmStartupComplete)
#define appEspPostInitRebootPending (appTasksState.appEspPostInitRebootPending)
#define appAtModeChangePending (appTasksState.appAtModeChangePending)
#define appAtConnectCommandPending (appTasksState.appAtConnectCommandPending)
#define appWifiApFallbackSyncConfirmed (appTasksState.appWifiApFallbackSyncConfirmed)
#define appWifiApFallbackSyncedValue (appTasksState.appWifiApFallbackSyncedValue)
#define appWifiApFallbackNextSyncMs (appTasksState.appWifiApFallbackNextSyncMs)
#define appAtConnectPending (appTasksState.appAtConnectPending)
#define appWifiStatusRefreshPending (appTasksState.appWifiStatusRefreshPending)
#define appAtFirmwareRequestPending (appTasksState.appAtFirmwareRequestPending)
#define appEspStartupGraceUntilMs (appTasksState.appEspStartupGraceUntilMs)
#define appMpuCalibrationUiState (appTasksState.appMpuCalibrationUiState)

static void APP_InitMenuSystem(void);
static void APP_InitMotorDrive(void);
static void APP_InitBalanceMotion(void);
static void APP_InitMPU(void);
static void APP_MPU_DataReady(void);
static uint8_t APP_MPU_LoadSavedCalibration(void);
static void APP_MPU_SaveCalibrationIfPending(void);
static void APP_MPUCalibration_Task(void);
static void APP_ESPRebootAfterStmStartup_Task(void);
static void APP_ESPAlive_Task(void);
static void APP_USB_CDC_Task(void);
static void APP_WifiBridgeEventCallback(const WifiBridge_Event *evt);
static void APP_WifiConnectRequest_Task(void);
static void APP_WifiApFallbackSync_Task(void);
static void APP_WifiStatusRefresh_Task(void);
static void APP_WifiFirmwareRequest_Task(void);
static void APP_ShowConnectivitySimpleNotification(OledSimpleNotificationId notificationId,
                                                   uint16_t timeoutMs);
static void APP_ClearStaIp(void);
static void APP_ClearApIp(void);
static uint8_t APP_ModeFromConnectivityFlags(void);
static void APP_UpdateAtConnectivityFlags(void);
static void APP_FinishAtWifiConnectionIfReady(void);
static void APP_UICapabilityGuardTask(void);
static void APP_UI10ms_Task(void);
static void APP_OLED_Task10ms(void);
static void APP_OLED_AutoRotationTask10ms(void);
static OledRotation_t APP_OledRotationFromDeg(uint16_t rotationDeg);
static OledRotation_t APP_GetPreferenceOledRotation(void);
static uint8_t APP_CurrentScreenRequiresRemoteUi(void);
static void APP_ReturnToWifiMenuAfterCapabilityLoss(void);

static OledRotation_t APP_OledRotationFromDeg(uint16_t rotationDeg)
{
    return (AppPreferences_ClampOledRotationDeg(rotationDeg) ==
            APP_PREFS_OLED_ROTATION_180_DEG)
        ? OLED_ROTATION_180
        : OLED_ROTATION_0;
}

static OledRotation_t APP_GetPreferenceOledRotation(void)
{
    return APP_OledRotationFromDeg(AppPreferences_GetOledRotationDeg());
}


/* DMA owns two complete scans. Only a completed half is published. */
static volatile uint16_t appAdcDma[APP_ADC_CHANNEL_COUNT*2u];
static volatile uint32_t appAdcSeq, appAdcMs;
static LineCalibration_Handle appLineCalibration;
static uint8_t appFollowReason;
static uint32_t appRouteCenteredMs, appLightChangedMs, appSettledSince, appSettledLastMs;
static uint16_t appSettledSeq;
static float appSettledPitch;
static uint8_t appRouteCentered, appSettling, appSettledCandidate;
static bool appSavedIrLine, appSavedIrObjects, appSavedWhiteLights;

static void APP_AdcPublish(unsigned offset)
{
    for (unsigned i=0;i<8u;++i) autoMicroSystem.runtime.sensor_raw_data[i]=appAdcDma[offset+i];
    appAdcMs=HAL_GetTick(); ++appAdcSeq;
    uint16_t line[3]={appAdcDma[offset+3u],appAdcDma[offset],appAdcDma[offset+1u]};
    LineCalibration_PushScan(&appLineCalibration,line,appAdcSeq,appAdcMs);
}
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *adc) { if (adc==&hadc1) APP_AdcPublish(0); }
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc) { if (adc==&hadc1) APP_AdcPublish(8); }
static void APP_CopyAdc(uint16_t raw[8],uint32_t *seq,uint32_t *ms)
{
    uint32_t irq=__get_PRIMASK(); __disable_irq();
    for (unsigned i=0;i<8u;++i) raw[i]=autoMicroSystem.runtime.sensor_raw_data[i];
    if (seq) *seq=appAdcSeq;
    if (ms) *ms=appAdcMs;
    if (!irq) __enable_irq();
}
void APP_CopyAdcSnapshot(uint16_t raw[8]) { APP_CopyAdc(raw,NULL,NULL); }
static AutoMotion_Handle appAutoMotion;
uint8_t APP_BalanceSetEnabled(bool on)
{
    if (on && APP_LineCalibrationBusy()) return LINE_FOLLOW_REJECT_BUSY;
    appSettledCandidate=0; motionControl.status.balance_settled=0;
    if (!on) { APP_AutoMotionCancel(); APP_LineFollowStop(); APP_MotorTestStop(); MotionControl_Disarm(&motionControl); return 0; }
    if (GET_CAR_MODE()==TEST_MODE) return LINE_FOLLOW_REJECT_MODE;
    MotionControl_Arm(&motionControl);
    return MotionControl_IsArmed(&motionControl)?0:LINE_FOLLOW_REJECT_NOT_READY;
}
bool APP_LineCalibrationBusy(void) { return LineCalibration_Busy(&appLineCalibration); }
bool APP_LineFollowEnabled(void) { return lineFollowControl.config.enabled!=0u; }
void APP_LineFollowStop(void)
{
    APP_AutoMotionCancel();
    LineFollowControl_SetEnabled(&lineFollowControl,0);
    MotionControl_Stop(&motionControl,MOTION_STOP_COAST);
    appLineFollowWasActive=0;
}
static LineCalibration_Pose APP_LinePose(void)
{
    MPU9250_Snapshot_t s; LineCalibration_Pose p={0};
    if (MPU9250_GetLatest(&mpuTask,&s)!=AM_OK) return p;
    p.seq=s.sample_seq; p.acquired_ms=mpuTask.last_attitude_ms;
    p.pitch_deg=MPU9250_GetFrontTiltDeg(&mpuTask); p.roll_deg=MPU9250_GetLateralTiltDeg(&mpuTask);
    p.yaw_deg=s.yaw_md*0.001f;
    p.gyro_max_dps=fmaxf(fabsf(s.gyro_x_mdps*0.001f),fmaxf(fabsf(s.gyro_y_mdps*0.001f),fabsf(s.gyro_z_mdps*0.001f)));
    p.valid=App_MpuReady(&mpuTask) && MPU9250_IsSampleFresh(&mpuTask,HAL_GetTick());
    return p;
}
static bool APP_LineReadSnapshot(void *ctx,LineFollow_Input *in,uint16_t snapshot[8])
{
    (void)ctx; memset(in,0,sizeof(*in));
    uint16_t raw[8]; APP_CopyAdc(raw,&in->ir_seq,&in->ir_acquired_ms);
    if (snapshot) memcpy(snapshot,raw,sizeof(raw));
    uint16_t lower[3]={raw[3],raw[0],raw[1]};
    bool lights=HAL_GPIO_ReadPin(Luces_IR_GPIO_Port,Luces_IR_Pin)==GPIO_PIN_SET;
    in->calibrated=LineCalibration_Ready(&appLineCalibration.store);
    in->ir_valid=LineFollow_Evaluate(&appLineCalibration.store,lower,lights,&lineFollowControl.config.line_config,&in->route) &&
        TCRT5000_EmittersGroup3IsOn(&irSensors) && !APP_LineCalibrationBusy();
    in->illumination_settling=appSettling && (uint32_t)(HAL_GetTick()-appLightChangedMs)<50u;
    if (!in->illumination_settling) appSettling=0;
    LineCalibration_Pose pose=APP_LinePose();
    in->pitch_deg=pose.pitch_deg; in->roll_deg=pose.roll_deg; in->yaw_deg=pose.yaw_deg;
    in->imu_acquired_ms=pose.acquired_ms; in->imu_seq=pose.seq; in->imu_valid=pose.valid;
    MPU9250_Snapshot_t s;
    if (MPU9250_GetLatest(&mpuTask,&s)==AM_OK) {
        /* Same body-Z convention as the existing MPU motion estimator. */
        in->yaw_rate_ccw_dps=s.gyro_z_mdps*0.001f;
        in->yaw_valid=pose.valid && isfinite(in->yaw_rate_ccw_dps);
    }
    in->armed=MotionControl_IsArmed(&motionControl);
    in->balance_active=motionControl.status.balance_active && !motionControl.status.safety_stop;
    in->mode_follow=GET_CAR_MODE()==FOLLOW_MODE;
    in->pitch_error_deg=in->pitch_deg-motionControl.status.pitch_setpoint_deg;
    in->roll_deviation_deg=in->roll_deg-(in->calibrated?appLineCalibration.store.track.roll_deg:0);
    return true;
}
static bool APP_LineRead(void *ctx,LineFollow_Input *in) { return APP_LineReadSnapshot(ctx,in,NULL); }
static bool APP_LineCommand(void *ctx,float forward,float turn,uint16_t lease,uint32_t now)
{
    (void)ctx; MotionControl_Command_t command; MotionControl_GetDefaultCommand(&command);
    command.source=MOTION_SOURCE_FOLLOW; command.forward_percent=forward;
    command.turn_percent=turn; command.duration_ms=lease;
    return MotionControl_SetCommand(&motionControl,&command,now)==AM_OK;
}
static void APP_LineHold(void *ctx) { (void)ctx; MotionControl_Stop(&motionControl,MOTION_STOP_COAST); }
static void APP_CalLights(void *ctx,bool ir,bool white)
{
    (void)ctx; TCRT5000_EmittersGroup3Set(&irSensors,ir);
    HAL_GPIO_WritePin(Luces_IR_GPIO_Port,Luces_IR_Pin,white?GPIO_PIN_SET:GPIO_PIN_RESET);
}
static void APP_CalRestore(void *ctx)
{
    (void)ctx; TCRT5000_EmittersGroup3Set(&irSensors,appSavedIrLine);
    TCRT5000_EmittersGroup5Set(&irSensors,appSavedIrObjects);
    HAL_GPIO_WritePin(Luces_IR_GPIO_Port,Luces_IR_Pin,appSavedWhiteLights?GPIO_PIN_SET:GPIO_PIN_RESET);
    appLightChangedMs=HAL_GetTick(); appSettling=1; appRouteCentered=0;
}
static bool APP_CalSave(void *ctx,const LineCalibration_Store *store)
{
    (void)ctx;
    if (MotionControl_IsArmed(&motionControl) || APP_MotorTestIsActive()) return false;
    if (AppPreferences_SetLineCalibration(store)!=AM_OK) return false;
    motionControl.config.neutral_pitch_deg=LineCalibration_Ready(store)?store->track.pitch_deg:0;
    motionControl.status.balance_settled=0; appSettledCandidate=0;
    return true;
}
uint8_t APP_LineCalibrationControl(uint8_t op,uint32_t session)
{
    if (op<=1u) {
        if (APP_LineCalibrationBusy()) return LINE_FOLLOW_REJECT_BUSY;
        if (MPU9250_IsCalibrating(&mpuTask) || APP_MPU_IsCalibrationPromptActive()) return LINE_FOLLOW_REJECT_NOT_READY;
        if (op==1u && !appLineCalibration.store.white.valid) return LINE_FOLLOW_REJECT_CALIBRATION;
        APP_AutoMotionCancel(); APP_LineFollowStop(); APP_MotorTestStop(); MotionControl_Disarm(&motionControl);
        OledUtils_EndIRValuesSession();
        appSavedIrLine=TCRT5000_EmittersGroup3IsOn(&irSensors);
        appSavedIrObjects=TCRT5000_EmittersGroup5IsOn(&irSensors);
        appSavedWhiteLights=HAL_GPIO_ReadPin(Luces_IR_GPIO_Port,Luces_IR_Pin)==GPIO_PIN_SET;
        /* Other emitters cannot contaminate the lower-array calibration. */
        TCRT5000_EmittersGroup5Set(&irSensors,false);
    }
    return LineCalibration_Control(&appLineCalibration,op,session,HAL_GetTick());
}
uint8_t APP_LineFollowSetEnabled(bool on)
{
    if (!on) { APP_LineFollowStop(); appFollowReason=0; return 0; }
    if (APP_LineFollowEnabled()) return 0;
    LineFollow_Input in; APP_LineRead(NULL,&in); uint32_t now=HAL_GetTick();
    uint8_t reason=!in.calibrated ? LINE_FOLLOW_REJECT_CALIBRATION :
        (APP_LineCalibrationBusy()?LINE_FOLLOW_REJECT_BUSY:
        (!in.mode_follow?LINE_FOLLOW_REJECT_MODE:
        (!in.armed || !in.balance_active?LINE_FOLLOW_REJECT_DISARMED:
        (!motionControl.status.balance_settled?LINE_FOLLOW_REJECT_NOT_SETTLED:
        (!in.ir_valid || (uint32_t)(now-in.ir_acquired_ms)>30u?LINE_FOLLOW_REJECT_IR_STALE:
        (!appRouteCentered || (uint32_t)(now-appRouteCenteredMs)<20u?LINE_FOLLOW_REJECT_NOT_CENTERED:0))))));
    appFollowReason=reason;
    if (reason) return reason;
    if (in.illumination_settling || !in.imu_valid || (uint32_t)(now-in.imu_acquired_ms)>50u) { appFollowReason=LINE_FOLLOW_REJECT_NOT_READY; return appFollowReason; }
    LineFollowControl_Reset(&lineFollowControl); LineFollowControl_SetEnabled(&lineFollowControl,1);
    appLineFollowWasActive=1; return 0;
}
bool APP_ReadIrRouteSnapshot(uint16_t raw[8],TCRT5000_RouteSample_t *out)
{
    LineFollow_Input in; APP_LineReadSnapshot(NULL,&in,raw); memset(out,0,sizeof(*out));
    out->alignment=TCRT5000_ROUTE_SEARCH;
    #define COPY_ROUTE(field) out->field=in.route.field
    COPY_ROUTE(leftRaw); COPY_ROUTE(centerRaw); COPY_ROUTE(rightRaw);
    COPY_ROUTE(leftNorm); COPY_ROUTE(centerNorm); COPY_ROUTE(rightNorm);
    COPY_ROUTE(leftDarkness); COPY_ROUTE(centerDarkness); COPY_ROUTE(rightDarkness);
    COPY_ROUTE(lateralErrorMm); COPY_ROUTE(lateralErrorNorm); COPY_ROUTE(centeredMarginMm);
    COPY_ROUTE(lineWidthMm); COPY_ROUTE(pattern); COPY_ROUTE(confidence); COPY_ROUTE(ambiguous);
    COPY_ROUTE(leftBlack); COPY_ROUTE(centerBlack); COPY_ROUTE(rightBlack); COPY_ROUTE(leftWhite); COPY_ROUTE(rightWhite);
    #undef COPY_ROUTE
    out->alignment=(TCRT5000_RouteAlignment_t)in.route.alignment;
    return in.ir_valid && !in.illumination_settling && (uint32_t)(HAL_GetTick()-in.ir_acquired_ms)<=30u;
}
bool APP_EvaluateRoute(TCRT5000_RouteSample_t *out) { return APP_ReadIrRouteSnapshot(NULL,out); }
static void APP_LineServiceTask(uint32_t now)
{
    LineCalibration_Pose pose=APP_LinePose();
    LineCalibration_Task(&appLineCalibration,&pose,now);
    if (APP_LineCalibrationBusy() && MotionControl_IsArmed(&motionControl)) {
        APP_LineFollowStop(); MotionControl_Disarm(&motionControl);
    }
    LineFollow_Input in; APP_LineRead(NULL,&in);
    bool centered=in.ir_valid && !in.illumination_settling && !in.route.ambiguous && in.route.pattern==2u;
    if (!centered) appRouteCentered=0;
    else if (!appRouteCentered) { appRouteCentered=1; appRouteCenteredMs=now; }
    bool eligible=in.armed && in.balance_active && in.imu_valid && !APP_LineCalibrationBusy();
    if (!eligible || (uint32_t)(now-in.imu_acquired_ms)>50u) {
        appSettledCandidate=0; motionControl.status.balance_settled=0; return;
    }
    if (pose.seq==appSettledSeq) return;
    uint32_t dt=pose.acquired_ms-appSettledLastMs;
    float rate=dt && dt<=50u ? fabsf(pose.pitch_deg-appSettledPitch)*1000.0f/dt : 1000.0f;
    appSettledSeq=pose.seq; appSettledLastMs=pose.acquired_ms; appSettledPitch=pose.pitch_deg;
    bool quiet=fabsf(in.pitch_error_deg)<=2.0f && rate<=5.0f && fabsf(in.roll_deviation_deg)<=3.0f;
    if (!quiet) { appSettledCandidate=0; motionControl.status.balance_settled=0; }
    else {
        if (!appSettledCandidate) { appSettledCandidate=1; appSettledSince=pose.acquired_ms; }
        motionControl.status.balance_settled=(uint32_t)(pose.acquired_ms-appSettledSince)>=500u;
    }
}
static void app_line_u32(uint8_t *p,uint32_t n) { for(unsigned i=0;i<4;++i) p[i]=(uint8_t)(n>>(8u*i)); }
static void app_line_f32(uint8_t *p,float f) { uint32_t n; memcpy(&n,&f,4); app_line_u32(p,n); }
void APP_GetLineState(uint8_t p[168],uint8_t reason)
{
    memset(p,0,168); LineCalibration_Handle *h=&appLineCalibration;
    LineFollow_Input in; APP_LineRead(NULL,&in);
    p[0]=reason?1:0; p[1]=1; p[2]=(uint8_t)h->state; p[3]=h->target; p[4]=h->phase; p[5]=h->progress;
    p[6]=reason?reason:h->reason; p[7]=reason?reason:appFollowReason;
    p[8]=(h->store.white.valid?1u:0u)|(LineCalibration_Ready(&h->store)?2u:0u)|
        (motionControl.status.balance_settled?4u:0u)|(APP_LineFollowEnabled()?8u:0u)|
        (in.armed?16u:0u)|(in.balance_active?32u:0u)|(motionControl.status.safety_stop?64u:0u)|(in.ir_valid?128u:0u);
    p[9]=(uint8_t)GET_CAR_MODE(); p[10]=in.route.pattern; p[11]=in.route.confidence;
    app_line_u32(p+12,h->session_id); app_line_u32(p+16,h->store.white_revision);
    app_line_u32(p+20,h->store.track_white_revision);
    app_line_u32(p+24,h->state==LINE_CAL_CAPTURE?h->phase*1280u+h->scans:(h->state==LINE_CAL_DONE?5120u:0u));
    app_line_u32(p+28,h->store.white.useful_scans); app_line_u32(p+32,h->store.track.useful_scans);
    app_line_f32(p+36,motionControl.config.neutral_pitch_deg);
    app_line_f32(p+40,h->store.track.roll_deg); app_line_f32(p+44,h->store.track.yaw_deg);
    const LineCalibration_Profile *profiles[2]={&h->store.white,&h->store.track};
    for(unsigned t=0;t<2;++t) {
        for(unsigned phase=0;phase<4;++phase) for(unsigned c=0;c<3;++c) {
            unsigned pos=t*48u+phase*6u+c*2u;
            uint16_t mean=profiles[t]->mean[phase][c], noise=profiles[t]->noise[phase][c];
            p[48u+pos]=(uint8_t)mean; p[49u+pos]=(uint8_t)(mean>>8);
            p[72u+pos]=(uint8_t)noise; p[73u+pos]=(uint8_t)(noise>>8);
        }
        app_line_f32(p+144u+t*12u,profiles[t]->pitch_deg);
        app_line_f32(p+148u+t*12u,profiles[t]->roll_deg);
        app_line_f32(p+152u+t*12u,profiles[t]->yaw_deg);
    }
}


/* Autonomous services own intentions; balance remains the final PWM arbiter. */
static bool APP_AutoRead(void *ctx,AutoMotion_Input *out) {
 (void)ctx;memset(out,0,sizeof(*out));uint16_t raw[8];uint32_t seq,ms;APP_CopyAdc(raw,&seq,&ms);
 LineFollow_Input line;APP_LineRead(NULL,&line);LineCalibration_Pose pose=APP_LinePose();
 out->now_ms=HAL_GetTick();out->imu_seq=pose.seq;out->yaw_deg=pose.yaw_deg;out->mpu_ready=pose.valid;
 out->mpu_calibrated=AppPreferences_Get()->mpuCalibration.valid && !MPU9250_IsCalibrating(&mpuTask);
 out->mode=GET_CAR_MODE();out->armed=MotionControl_IsArmed(&motionControl);
 out->balance=motionControl.status.balance_active;out->settled=motionControl.status.balance_settled;out->safety=motionControl.status.safety_stop;
 out->line_ready=line.calibrated;out->line_fresh=line.ir_valid && out->now_ms-ms<=30u && !line.illumination_settling;
 out->center_black=line.route.centerBlack;
 out->objects_enabled=TCRT5000_EmittersGroup5IsOn(&irSensors);
 out->objects_fresh=seq && out->now_ms-ms<=30u && !line.illumination_settling;
 /* Raw DMA order follows the existing eight-channel IR payload. */
 const uint8_t index[5]={6,4,2,5,7};
 for(unsigned c=0;c<5;++c)out->objects[c]=raw[index[c]];
 out->lights=APP_IsIrLightsEnabled();
 float top=(float)(htim2.Instance->ARR+1u);
 out->left_duty=100.0f*((float)__HAL_TIM_GET_COMPARE(&htim2,TIM_CHANNEL_1)-(float)__HAL_TIM_GET_COMPARE(&htim2,TIM_CHANNEL_2))/top;
 out->right_duty=100.0f*((float)__HAL_TIM_GET_COMPARE(&htim2,TIM_CHANNEL_3)-(float)__HAL_TIM_GET_COMPARE(&htim2,TIM_CHANNEL_4))/top;
 uint32_t clock=HAL_RCC_GetPCLK1Freq();
 if((RCC->CFGR&RCC_CFGR_PPRE1)!=0u)clock*=2u;
 out->pwm_hz=(float)clock/(htim2.Instance->PSC+1u)/top;return true;
}
static bool APP_AutoCommand(void *ctx,float forward,float turn,uint32_t now) {
 (void)ctx;MotionControl_Command_t cmd;MotionControl_GetDefaultCommand(&cmd);
 cmd.source=MOTION_SOURCE_GOTO;cmd.forward_percent=forward;cmd.turn_percent=turn;cmd.duration_ms=50;
 return MotionControl_SetCommand(&motionControl,&cmd,now)==AM_OK;
}
static bool APP_AutoSave(void *ctx,const AutoMotion_Profile *profile) {
 (void)ctx;if(MotionControl_IsArmed(&motionControl)||APP_MotorTestIsActive()||APP_LineCalibrationBusy())return false;
 return AppPreferences_SetAutoMotionProfile(profile)==AM_OK;
}
bool APP_AutoMotionBusy(void) { return AutoMotion_Busy(&appAutoMotion); }
void APP_AutoMotionCancel(void) { AutoMotion_Cancel(&appAutoMotion); }
uint8_t APP_AutoMotionRequest(uint8_t cmd,const uint8_t *request,uint8_t length,uint8_t *response) {
 uint8_t reason=0;
 if(APP_LineCalibrationBusy() && cmd!=0x23u && cmd!=0x27u && cmd!=0x29u && cmd!=0x25u)reason=AUTO_BUSY;
 else if(cmd==0x24u) { float x,y;memcpy(&x,request,4);memcpy(&y,request+4,4);reason=AutoMotion_Goto(&appAutoMotion,x,y); }
 else if(cmd==0x25u)AutoMotion_Cancel(&appAutoMotion);
 else if(cmd==0x2Bu) { if(APP_AutoMotionBusy() || APP_LineCalibrationBusy() || APP_MPU_IsCalibrationRunning() || APP_MPU_IsCalibrationPromptActive())reason=AUTO_BUSY;else APP_MPU_RequestCalibration(); }
 else if(cmd==0x26u)reason=AutoMotion_SetIr(&appAutoMotion,request,length);
 else if(cmd==0x28u) {
  float percent;memcpy(&percent,request+1,4);uint32_t ramp,timeout;memcpy(&ramp,request+5,4);memcpy(&timeout,request+9,4);
  reason=AutoMotion_Calibrate(&appAutoMotion,request[0],percent,ramp,timeout);
 } else if(cmd==0x2Au)reason=AutoMotion_SetProfile(&appAutoMotion,request,length);
 if(cmd==0x29u||cmd==0x2Au){AutoMotion_GetProfile(&appAutoMotion,response,reason);return AUTO_MOTION_PROFILE_BYTES;}
 AutoMotion_State(&appAutoMotion,response,reason);return AUTO_MOTION_STATE_BYTES;
}

void APP_Init(void)
{
    AppScreensBind(&appScreens);
    AppAuxiliaryBind();
    APP_EventsBindSystem();
    autoMicroSystem.handles.tasks = &appTasksState;
    autoMicroSystem.handles.screens = &appScreens;
    autoMicroSystem.handles.ir = &irSensors;
    autoMicroSystem.handles.preferences = &appPreferencesHandle;
    autoMicroSystem.metadata.name = firmwareName; autoMicroSystem.metadata.version = firmwareVersion;
    autoMicroSystem.metadata.target = firmwareTarget; autoMicroSystem.metadata.build_date = buildDate;
    autoMicroSystem.metadata.build_time = buildTime; autoMicroSystem.metadata.build_profile = buildProfile;
    autoMicroSystem.metadata.build_compiler = buildCompiler;
    autoMicroSystem.handles.encoder = &encoder;
    autoMicroSystem.handles.button = &btnUser;
    autoMicroSystem.handles.motors = &motorTask;
    autoMicroSystem.handles.mpu = &mpuTask;
    autoMicroSystem.handles.i2c = &i2cManager;
    autoMicroSystem.handles.menu = &menuSystem;
    autoMicroSystem.handles.display = &appDisplay;
    autoMicroSystem.handles.oled_ui = &oledHandle;
    autoMicroSystem.handles.balance = &balanceControl;
    autoMicroSystem.handles.motion = &motionControl;
    autoMicroSystem.handles.estimator = &motionEstimator;
    autoMicroSystem.handles.line = &lineFollowControl;
    autoMicroSystem.handles.wifi = &wifiBridge;
    autoMicroSystem.configuration.preferences = AppPreferences_Get();
    appStmStartupComplete = 0U;
    appEspPostInitRebootPending = 0U;
    appWifiApFallbackSyncConfirmed = 0u;
    appWifiApFallbackNextSyncMs = 0u;
    AppPreferences_Init();
    Permission_Init();
    ScreenLock_Init();
    static AppIrContext irContext;
    irContext.objects = (AppGpioContext){IR_Objects_GPIO_Port,IR_Objects_Pin};
    irContext.line = (AppGpioContext){IR_Line_GPIO_Port,IR_Line_Pin};
    irContext.adc=&hadc1; irContext.samples=appAdcDma; irContext.count=APP_ADC_CHANNEL_COUNT*2u;
    const TCRT5000_Port irPort={&irContext,App_IrInitialize,App_IrEmitters};
    if (TCRT5000_Init(&irSensors,&irPort)!=AM_OK) Error_Handler();
    autoMicroSystem.handles.ir=&irSensors;
    initCarMode();
    APP_InitMotorDrive();
    const AutoMotion_Port autoPort={NULL,APP_AutoRead,APP_AutoCommand,APP_LineHold,APP_AutoSave};
    AutoMotion_Init(&appAutoMotion,&autoPort,&AppPreferences_Get()->autoMotionProfile);

    if (App_IrStartSampling(&irContext) != HAL_OK) {
        Error_Handler();
    }

    if (I2C_Manager_Init(&i2cManager, &appI2cHardware, &hi2c1, I2C_MANAGER_DEFAULT_LEASE_TIMEOUT_MS) != HAL_OK) {
        Error_Handler();
    }
    i2cManager.recover_cb = App_I2cRecover;

    App_I2cConnect(&oledBusBinding,&i2cManager,DEVICE_ID_OLED);
    App_I2cConnect(&mpuBusBinding,&i2cManager,DEVICE_ID_MPU);
    if (ssd1306_BindPort(&oledBusBinding.port) != HAL_OK) {
        Error_Handler();
    }

    APP_InitMPU();
    APP_InitBalanceMotion();

    appDisplayReady = ssd1306_Init();
    if (appDisplayReady) {
        SET_FLAG(autoMicroSystem.runtime.systemFlags, OLED_READY);
        SET_FLAG(autoMicroSystem.runtime.systemFlags2, OLED_ACTIVE);
        (void)ssd1306_SetRotation(APP_GetPreferenceOledRotation());
    }

    if (WifiBridge_Init(&wifiBridge, &huart1, &appWifiHardware, &appWifiServices, NULL, &autoMicroSystem, &wifiBridgeCredentials) != WIFI_BRIDGE_OK) {
        Error_Handler();
    }
    WifiBridge_SetEventCallback(&wifiBridge, APP_WifiBridgeEventCallback);
    if (WifiBridge_Start(&wifiBridge) != WIFI_BRIDGE_OK) {
        Error_Handler();
    }
    /* Reports need the bridge, and the boot notification needs initialized UI callbacks. */
    APP_InitMenuSystem();
    if (appDisplayReady) {
        ext_profile_t profile = AppPreferences_GetExtProfile();
        if (profile == EXT_PROFILE_BEEPER_PB12) {
            OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_EXT_BEEPER, OLED_NOTIFICATION_DEFAULT_MS);
        } else if (profile == EXT_PROFILE_NRF24) {
            OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_EXT_NRF24, OLED_NOTIFICATION_DEFAULT_MS);
        } else {
            OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_EXT_DEFAULT, OLED_NOTIFICATION_DEFAULT_MS);
        }
    }
    UNER_App_SetRouterForwardingEnabled(
        AppPreferences_IsUnerRouterForwardingEnabled() ? 1u : 0u);
    APP_ApplyIrState();

    appInputReady = 1U;
    SET_FLAG(autoMicroSystem.runtime.systemFlags, INIT_CAR);

    __HAL_TIM_SET_COUNTER(&htim3, 0U);
    if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK) {
        Error_Handler();
    }

    /*
     * Arm the raw WEB/UNER reboot only after every STM subsystem is ready.
     * The first APP_Loop iteration may transmit immediately; an ESP running
     * AT firmware will ignore this binary frame.
     */
    appEspPostInitRebootLastAttemptMs =
        HAL_GetTick() - APP_ESP_POST_INIT_REBOOT_RETRY_MS;
    appEspPostInitRebootPending = 1U;
    appEspStartupGraceUntilMs = HAL_GetTick() + APP_ESP_STARTUP_GRACE_MS;
    if (appEspStartupGraceUntilMs == 0u) appEspStartupGraceUntilMs = 1u;
    appStmStartupComplete = 1U;
}

void APP_Loop(void)
{
    USB_ModeManager_Task();
    Permission_Task(HAL_GetTick());
    i2cManager_MainTask();
    UserBtn_MainTask(&btnUser);
    Encoder_MainTask(&encoder);
    if (appMpuReady) {
        MPU9250_Process(&mpuTask);
    }
    Motor_MainTask();
    APP_MPUCalibration_Task();
    WifiBridge_Process(&wifiBridge);
    APP_WifiApFallbackSync_Task();
    APP_WifiConnectRequest_Task();
    APP_WifiStatusRefresh_Task();
    APP_WifiFirmwareRequest_Task();
    APP_ESPRebootAfterStmStartup_Task();
    APP_ESPAlive_Task();
    APP_UICapabilityGuardTask();
    APP_USB_CDC_Task();
    APP_LineServiceTask(HAL_GetTick());
    Dashboard_ModeSelectionTick();
    GenericConfirm_Tick();
    APP_UI10ms_Task();
    OLED_MainTask();
}

void APP_ESPModeChangeCommandAccepted(void)
{
    appAtModeChangePending = 1u;
}

void APP_ESPWifiConnectRequested(void)
{
    appAtConnectCommandPending = 1u;
}

void APP_ESPWifiApFallbackResponse(uint8_t status, uint8_t enabled)
{
    const uint8_t desired = AppPreferences_IsWifiApFallbackEnabled() ? 1u : 0u;
    if (status == 0u && enabled == desired) {
        appWifiApFallbackSyncedValue = enabled;
        appWifiApFallbackSyncConfirmed = 1u;
        appWifiApFallbackNextSyncMs = HAL_GetTick() + 30000u;
    } else {
        appWifiApFallbackSyncConfirmed = 0u;
    }
}

void APP_ESPStatusRefreshRequested(void)
{
    appWifiStatusRefreshPending = 1u;
}

void APP_ESPFirmwareRequestRequested(void)
{
    appAtFirmwareRequestPending = 1u;
}

static void APP_ClearStaIp(void)
{
    autoMicroSystem.connectivity.espStaIp.block1 = 0u;
    autoMicroSystem.connectivity.espStaIp.block2 = 0u;
    autoMicroSystem.connectivity.espStaIp.block3 = 0u;
    autoMicroSystem.connectivity.espStaIp.block4 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.staIp.block1 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.staIp.block2 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.staIp.block3 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.staIp.block4 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.staIpValid = 0u;
}

static void APP_ClearApIp(void)
{
    autoMicroSystem.connectivity.espApIp.block1 = 0u;
    autoMicroSystem.connectivity.espApIp.block2 = 0u;
    autoMicroSystem.connectivity.espApIp.block3 = 0u;
    autoMicroSystem.connectivity.espApIp.block4 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.apIp.block1 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.apIp.block2 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.apIp.block3 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.apIp.block4 = 0u;
    autoMicroSystem.connectivity.espWifiConnection.apIpValid = 0u;
}

static uint8_t APP_ModeFromConnectivityFlags(void)
{
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE) &&
        IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE)) {
        return WIFI_BRIDGE_MODE_AP_STA;
    }
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)) {
        return WIFI_BRIDGE_MODE_AP;
    }
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE)) {
        return WIFI_BRIDGE_MODE_STA;
    }
    return WIFI_BRIDGE_MODE_UNKNOWN;
}

static bool APP_IsBootNotificationShowing(void)
{
    return OledUtils_IsSimpleNotificationShowing(OLED_SIMPLE_NOTIFICATION_EXT_DEFAULT) ||
           OledUtils_IsSimpleNotificationShowing(OLED_SIMPLE_NOTIFICATION_EXT_BEEPER) ||
           OledUtils_IsSimpleNotificationShowing(OLED_SIMPLE_NOTIFICATION_EXT_NRF24);
}

static void APP_ShowConnectivitySimpleNotification(OledSimpleNotificationId notificationId,
                                                   uint16_t timeoutMs)
{
    if (APP_IsBootNotificationShowing()) {
        return;
    }
    /* Suppress ESP failure notifications during startup grace period */
    if (appEspStartupGraceUntilMs != 0u) {
        if ((int32_t)(appEspStartupGraceUntilMs - HAL_GetTick()) > 0) {
            return;
        }
        appEspStartupGraceUntilMs = 0u;
    }

    Dashboard_InvalidateConnectivity();
    if (!appDisplayReady || !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
        return;
    }

    OledUtils_ShowSimpleNotificationMsEx(notificationId,
                                         timeoutMs,
                                         NULL,
                                         Dashboard_ReturnAfterConnectivityNotification);
}

static void APP_UpdateAtConnectivityFlags(void)
{
    const uint8_t mode = wifiBridge.mode;

    SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);

    if (mode == WIFI_BRIDGE_MODE_AP || mode == WIFI_BRIDGE_MODE_AP_STA) {
        SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    } else {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
    }

    if ((mode == WIFI_BRIDGE_MODE_STA || mode == WIFI_BRIDGE_MODE_AP_STA) &&
        autoMicroSystem.connectivity.espWifiConnection.staIpValid) {
        SET_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
    } else {
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
    }
}

static void APP_FinishAtWifiConnectionIfReady(void)
{
    if (!appAtConnectPending || !autoMicroSystem.connectivity.espWifiConnection.staIpValid) {
        return;
    }

    appAtConnectPending = 0u;
    OledUtils_SetWiFiConnectedSsid(autoMicroSystem.connectivity.espWifiConnection.staSsid);
    if (OledUtils_IsNotificationShowing(OledUtils_RenderESPWifiConnectingNotification)) {
        OledUtils_ShowNotificationMs(OledUtils_RenderESPWifiConnectedNotification, OLED_NOTIFICATION_DEFAULT_MS);
    } else {
        OledUtils_ShowNotificationMsEx(OledUtils_RenderESPWifiConnectedNotification,
                                       2500u,
                                       NULL,
                                       Dashboard_ReturnAfterConnectivityNotification);
    }
}

static void APP_WifiBridgeEventCallback(const WifiBridge_Event *evt)
{
    if (evt == NULL ||
        WifiBridge_GetActiveBackend(&wifiBridge) != WIFI_BRIDGE_BACKEND_AT) {
        return;
    }

    UNER_App_OnAtBridgeEvent(evt);
    switch (evt->id) {
        case WIFI_BRIDGE_EVT_BOOT:
            appWifiStatusRefreshPending = 1u;
            Dashboard_InvalidateConnectivity();
            break;

        case WIFI_BRIDGE_EVT_MODE_CHANGED:
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
            if (evt->mode == WIFI_BRIDGE_MODE_AP) {
                SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
                CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
                APP_ClearStaIp();
                APP_ClearApIp();
                appWifiStatusRefreshPending = 1u;
            } else if (evt->mode == WIFI_BRIDGE_MODE_STA) {
                CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
                CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
                APP_ClearApIp();
                APP_ClearStaIp();
                appWifiStatusRefreshPending = 1u;
            } else if (evt->mode == WIFI_BRIDGE_MODE_AP_STA) {
                SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
                if (evt->got_ip) {
                    SET_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
                }
                appWifiStatusRefreshPending = 1u;
            }
            Dashboard_InvalidateConnectivity();
            break;

        case WIFI_BRIDGE_EVT_AP_STARTED:
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
            APP_ClearStaIp();
            APP_ClearApIp();
            appWifiStatusRefreshPending = 1u;
            if (appAtModeChangePending) {
                appAtModeChangePending = 0u;
                APP_ShowConnectivitySimpleNotification(
                    OLED_SIMPLE_NOTIFICATION_ESP_MODE_CHANGED,
                    1800u);
            } else {
                Dashboard_InvalidateConnectivity();
            }
            break;

        case WIFI_BRIDGE_EVT_AP_STOPPED:
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
            APP_ClearApIp();
            APP_ClearStaIp();
            appWifiStatusRefreshPending = 1u;
            if (appAtModeChangePending) {
                appAtModeChangePending = 0u;
                APP_ShowConnectivitySimpleNotification(
                    OLED_SIMPLE_NOTIFICATION_ESP_MODE_CHANGED,
                    1800u);
            } else {
                Dashboard_InvalidateConnectivity();
            }
            break;

        case WIFI_BRIDGE_EVT_STA_CONNECTED:
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
            if (evt->mode == WIFI_BRIDGE_MODE_STA) {
                CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
            }
            APP_ClearStaIp();
            Dashboard_InvalidateConnectivity();
            break;

        case WIFI_BRIDGE_EVT_STA_DISCONNECTED:
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
            APP_ClearStaIp();
            Dashboard_InvalidateConnectivity();
            break;

        case WIFI_BRIDGE_EVT_GOT_IP:
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, ESP_PRESENT);
            if (evt->mode == WIFI_BRIDGE_MODE_AP) {
                SET_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
                CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
                if (!autoMicroSystem.connectivity.espWifiConnection.apIpValid) {
                    appWifiStatusRefreshPending = 1u;
                }
            } else {
                SET_FLAG(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
                if (evt->mode == WIFI_BRIDGE_MODE_STA) {
                    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
                }
                if (!autoMicroSystem.connectivity.espWifiConnection.staIpValid) {
                    appWifiStatusRefreshPending = 1u;
                }
            }
            APP_FinishAtWifiConnectionIfReady();
            Dashboard_InvalidateConnectivity();
            break;

        case WIFI_BRIDGE_EVT_SCAN_DONE:
            if (autoMicroSystem.connectivity.wifiScanSessionActive || IS_FLAG_SET(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING)) {
                autoMicroSystem.connectivity.wifiSearchingTimeout = 0u;
                CLEAR_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);
                autoMicroSystem.connectivity.wifiScanSessionActive = 0u;
                autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
                menuSystem.currentMenu = &wifiResultsMenu;
                menuSystem.renderFn = OledUtils_RenderWiFiSearchResults_Wrapper;
                menuSystem.renderFlag = true;
                autoMicroSystem.runtime.oled_first_draw = false;
                OledUtils_ShowNotificationMs(OledUtils_ShowWifiResults, OLED_NOTIFICATION_DEFAULT_MS);
            }
            break;

        case WIFI_BRIDGE_EVT_STATUS_REFRESHED:
            APP_UpdateAtConnectivityFlags();
            APP_FinishAtWifiConnectionIfReady();
            if (OledUtils_IsSimpleNotificationShowing(
                    OLED_SIMPLE_NOTIFICATION_ESP_CHECKING_CONNECTION)) {
                OledUtils_RemoveSimpleNotification(OLED_SIMPLE_NOTIFICATION_ESP_CHECKING_CONNECTION);
                if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE)) {
                    OledUtils_ShowNotificationMs(
                        OledUtils_RenderESPWifiConnectedNotification,
                        2500u);
                } else if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE) &&
                           autoMicroSystem.connectivity.espWifiConnection.apIpValid) {
                    OledUtils_ShowNotificationMs(
                        OledUtils_RenderESPAPStartedNotification,
                        2500u);
                } else {
                    OledUtils_ShowNotificationMs(OledUtils_RenderWiFiNotConnected,
                                                    2500u);
                }
            }
            Dashboard_InvalidateConnectivity();
            break;

        case WIFI_BRIDGE_EVT_FIRMWARE_RECEIVED:
            appAtFirmwareRequestPending = 0u;
            OledUtils_NotificationRestore();
            menuSystem.renderFn = OledUtils_RenderESPFirmwareScreen_Wrapper;
            menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
            menuSystem.renderFlag = true;
            autoMicroSystem.runtime.oled_first_draw = false;
            break;

        case WIFI_BRIDGE_EVT_ERROR:
            if (appAtModeChangePending) {
                appAtModeChangePending = 0u;
                appWifiStatusRefreshPending = 0u;
                wifiBridge.mode = APP_ModeFromConnectivityFlags();
                APP_ShowConnectivitySimpleNotification(
                    OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
                    2000u);
            }
            if (appAtConnectPending) {
                appAtConnectPending = 0u;
                OledUtils_SetWiFiCredentialsWebResultStatus(
                    (evt->status == WIFI_BRIDGE_TIMEOUT)
                        ? WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_TIMEOUT
                        : WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_FAILED);
                OledUtils_ShowNotificationMs(
                    OledUtils_RenderWiFiCredentialsFailedNotification,
                    2500u);
            }
            if (autoMicroSystem.connectivity.wifiScanSessionActive || IS_FLAG_SET(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING)) {
                CLEAR_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);
                autoMicroSystem.connectivity.wifiScanSessionActive = 0u;
                autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
                autoMicroSystem.connectivity.wifiSearchingTimeout = WIFIDEFAULTSEARCHTIMEOUT;
                OledUtils_NotificationRestore();
                menuSystem.currentMenu = &submenu1;
                menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
                menuSystem.userEventManagerFn = menuEventManager;
                menuSystem.renderFlag = true;
                autoMicroSystem.runtime.oled_first_draw = true;
                APP_ShowConnectivitySimpleNotification(
                    OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
                    2000u);
            }
            if (OledUtils_IsNotificationShowing(
                    OledUtils_RenderESPFirmwareRequestNotification)) {
                APP_ShowConnectivitySimpleNotification(
                    OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
                    2000u);
            }
            break;

        default:
            break;
    }
}

uint8_t APP_GetWifiApFallbackSyncState(void)
{
    const WifiBridge_Backend backend = WifiBridge_GetActiveBackend(&wifiBridge);
    /* Factory AT policy is owned and applied by F4, not mirrored in the modem. */
    if (backend == WIFI_BRIDGE_BACKEND_AT) return 2u;
    if (backend != WIFI_BRIDGE_BACKEND_WEB) return 0u;
    return appWifiApFallbackSyncConfirmed &&
        appWifiApFallbackSyncedValue == (AppPreferences_IsWifiApFallbackEnabled() ? 1u : 0u) ? 2u : 1u;
}

static void APP_WifiApFallbackSync_Task(void)
{
    const uint32_t now = HAL_GetTick();
    const uint8_t desired = AppPreferences_IsWifiApFallbackEnabled() ? 1u : 0u;
    if (!appStmStartupComplete || appEspPostInitRebootPending ||
        WifiBridge_GetActiveBackend(&wifiBridge) != WIFI_BRIDGE_BACKEND_WEB ||
        UNER_App_IsMcuResetPending()) return;

    if (appWifiApFallbackSyncConfirmed && appWifiApFallbackSyncedValue != desired) {
        appWifiApFallbackSyncConfirmed = 0u;
        appWifiApFallbackNextSyncMs = 0u;
    }
    if (appWifiApFallbackNextSyncMs != 0u &&
        (int32_t)(now - appWifiApFallbackNextSyncMs) < 0) return;
    if (WifiBridge_IsWaitingValidation(&wifiBridge)) return;

    if (WifiBridge_SendCommand(&wifiBridge, WIFI_BRIDGE_CMD_ID_SET_AP_FALLBACK,
                               &desired, 1u) == WIFI_BRIDGE_OK) {
        // The response, rather than the enqueue or ACK, confirms persistence.
        appWifiApFallbackSyncConfirmed = 0u;
        appWifiApFallbackNextSyncMs = now + 2000u;
    }
}

static void APP_WifiConnectRequest_Task(void)
{
    const WifiBridge_Backend backend = WifiBridge_GetActiveBackend(&wifiBridge);
    WifiBridge_Result result;

    if (!appAtConnectCommandPending) {
        return;
    }
    if ((backend != WIFI_BRIDGE_BACKEND_WEB && backend != WIFI_BRIDGE_BACKEND_AT) ||
        WifiBridge_IsWaitingValidation(&wifiBridge)) {
        return;
    }
    if (backend == WIFI_BRIDGE_BACKEND_WEB &&
        (!appWifiApFallbackSyncConfirmed ||
         appWifiApFallbackSyncedValue != (AppPreferences_IsWifiApFallbackEnabled() ? 1u : 0u))) {
        return;
    }

    result = WifiBridge_Connect(&wifiBridge);
    if (result == WIFI_BRIDGE_OK) {
        appAtConnectCommandPending = 0u;
        appAtConnectPending = backend == WIFI_BRIDGE_BACKEND_AT ? 1u : 0u;
    } else if (result != WIFI_BRIDGE_BUSY) {
        appAtConnectCommandPending = 0u;
        appAtConnectPending = 0u;
        OledUtils_SetWiFiCredentialsWebResultStatus(
            WIFI_BRIDGE_WIFI_WEB_CREDENTIALS_STATUS_FAILED);
        OledUtils_ShowNotificationMs(
            OledUtils_RenderWiFiCredentialsFailedNotification,
            2500u);
    }
}

static void APP_WifiStatusRefresh_Task(void)
{
    const WifiBridge_Backend backend = WifiBridge_GetActiveBackend(&wifiBridge);

    if (!appWifiStatusRefreshPending) {
        return;
    }
    if (backend == WIFI_BRIDGE_BACKEND_WEB) {
        appWifiStatusRefreshPending = 0u;
        return;
    }
    if (backend != WIFI_BRIDGE_BACKEND_AT ||
        WifiBridge_IsWaitingValidation(&wifiBridge)) {
        return;
    }

    if (WifiBridge_GetStatus(&wifiBridge) == WIFI_BRIDGE_OK) {
        appWifiStatusRefreshPending = 0u;
    }
}

static void APP_WifiFirmwareRequest_Task(void)
{
    const WifiBridge_Backend backend = WifiBridge_GetActiveBackend(&wifiBridge);

    if (!appAtFirmwareRequestPending) {
        return;
    }
    if (backend == WIFI_BRIDGE_BACKEND_WEB) {
        appAtFirmwareRequestPending = 0u;
        return;
    }
    if (backend != WIFI_BRIDGE_BACKEND_AT ||
        WifiBridge_IsWaitingValidation(&wifiBridge)) {
        return;
    }

    if (WifiBridge_SendCommand(&wifiBridge,
                               WIFI_BRIDGE_CMD_ID_REQUEST_FIRMWARE,
                               NULL,
                               0u) == WIFI_BRIDGE_OK) {
        appAtFirmwareRequestPending = 0u;
    }
}

void initCarMode(void)
{
    SET_CAR_MODE(IDLE_MODE);
    static AppGpioContext userGpio = {User_BTN_GPIO_Port, User_BTN_Pin};
    static AppEncoderContext encoderHw = {&htim4, {EncoderSW_GPIO_Port, EncoderSW_Pin}};
    const UserButton_Port buttonPort = {&userGpio, App_ReadLevel};
    const ENC_Port encoderPort = {&encoderHw, App_EncoderCount, App_EncoderReset,
                                  App_EncoderStart, App_EncoderButton};
    UserButton_Init(&btnUser, &buttonPort, USER_BUTTON_ACTIVE_HIGH);
    ENC_Init(&encoder, &encoderPort, 1U, 4U);
    ENC_SetDirectionInverted(&encoder, AppPreferences_IsEncoderInverted());
    ENC_Start(&encoder);

    /* Temporary until the original screenWrappers/menu layer owns this flag again. */
    encoder.allowEncoderInput = true;
}

bool APP_TransitionCarMode(CarMode_t mode)
{
    if (mode > GOTO_MODE || APP_LineCalibrationBusy()) {
        return false;
    }

    APP_AutoMotionCancel();
    LineFollowControl_SetEnabled(&lineFollowControl, 0u);
    appLineFollowWasActive = 0u;
    APP_ApplyIrState();
    MotionControl_Stop(&motionControl, MOTION_STOP_COAST);
    APP_MotorTestStop();
    if (mode == TEST_MODE || GET_CAR_MODE() == TEST_MODE) {
        MotionControl_Disarm(&motionControl);
    }
    SET_CAR_MODE(mode);
    return true;
}

void APP_ApplyIrState(void)
{
    if (APP_LineCalibrationBusy()) return;
    appLightChangedMs=HAL_GetTick(); appSettling=1; appRouteCentered=0;
    TCRT5000_EmittersGroup5Set(&irSensors, autoMicroSystem.configuration.ir_objects_enabled);
    TCRT5000_EmittersGroup3Set(&irSensors, autoMicroSystem.configuration.ir_line_enabled);
    HAL_GPIO_WritePin(Luces_IR_GPIO_Port, Luces_IR_Pin,
                      autoMicroSystem.configuration.ir_lights_enabled ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool APP_IsIrObjectsEnabled(void) { return autoMicroSystem.configuration.ir_objects_enabled; }
bool APP_IsIrLineEnabled(void) { return autoMicroSystem.configuration.ir_line_enabled; }
bool APP_IsIrLightsEnabled(void) { return autoMicroSystem.configuration.ir_lights_enabled; }

void APP_SetIrObjectsEnabled(bool enabled) {
    if (APP_LineCalibrationBusy()) return;
    autoMicroSystem.configuration.ir_objects_enabled = enabled;
    APP_ApplyIrState();
}
void APP_SetIrLineEnabled(bool enabled) {
    if (APP_LineCalibrationBusy()) return;
    if (!enabled) APP_LineFollowStop();
    autoMicroSystem.configuration.ir_line_enabled = enabled;
    APP_ApplyIrState();
}
void APP_SetIrLightsEnabled(bool enabled) {
    if (APP_LineCalibrationBusy()) return;
    autoMicroSystem.configuration.ir_lights_enabled = enabled;
    APP_ApplyIrState();
}

void APP_ApplyPreferenceRuntimeChanges(AppPreferencesFieldMask_t changedFields)
{
    changedFields &= APP_PREFS_FIELD_ALL;
    if (changedFields == 0u) {
        return;
    }

    if ((changedFields & APP_PREFS_FIELD_WIFI_AP_FALLBACK_ENABLED) != 0u) {
        appWifiApFallbackSyncConfirmed = 0u;
        appWifiApFallbackNextSyncMs = 0u;
    }

    if ((changedFields & APP_PREFS_FIELD_ENCODER_INVERTED) != 0u) {
        ENC_SetDirectionInverted(&encoder, AppPreferences_IsEncoderInverted());
    }

    if ((changedFields & (APP_PREFS_FIELD_SCREEN_LOCK_ENABLED |
                          APP_PREFS_FIELD_SCREEN_LOCK_TIMEOUT)) != 0u) {
        ScreenLock_OnPreferencesChanged();
    }

    if ((changedFields & APP_PREFS_FIELD_MOTOR_LEFT_MIN_PWM) != 0u) {
        MotorDrive_SetLeftMinPwm(&motorTask, AppPreferences_GetMotorLeftMinPwm());
    }

    if ((changedFields & APP_PREFS_FIELD_MOTOR_RIGHT_MIN_PWM) != 0u) {
        MotorDrive_SetRightMinPwm(&motorTask, AppPreferences_GetMotorRightMinPwm());
    }

    if (((changedFields & APP_PREFS_FIELD_MPU_CALIBRATION) != 0u) && appMpuReady) {
        (void)APP_MPU_LoadSavedCalibration();
    }

    if (((changedFields & APP_PREFS_FIELD_OLED_ROTATION) != 0u) &&
        appDisplayReady &&
        IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
        (void)OledUtils_SetRotationAndRefresh(APP_GetPreferenceOledRotation());
    }

    if (((changedFields & APP_PREFS_FIELD_OLED_AUTO_ROTATION) != 0u) &&
        appDisplayReady &&
        IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
        appOledAutoRotationInTiltZone = 0u;
        appOledAutoRotation10msCounter = 0u;
        appOledAutoRotationLastEnabled =
            AppPreferences_IsOledAutoRotationEnabled() ? 1u : 0u;
        if (appMpuReady) {
            MPU9250_AttitudeResetYaw(&mpuTask.attitude);
        }
        if (!AppPreferences_IsOledAutoRotationEnabled()) {
            (void)OledUtils_SetRotationAndRefresh(APP_GetPreferenceOledRotation());
        }
    }

    if ((changedFields & APP_PREFS_FIELD_UNER_ROUTER_FORWARDING) != 0u) {
        const uint8_t forwarding_enabled =
            AppPreferences_IsUnerRouterForwardingEnabled() ? 1u : 0u;

        UNER_App_SetRouterForwardingEnabled(forwarding_enabled);
        if (appDisplayReady && IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
            OledUtils_ShowSimpleNotificationMs(
                forwarding_enabled
                    ? OLED_SIMPLE_NOTIFICATION_UNER_FORWARDING_ENABLED
                    : OLED_SIMPLE_NOTIFICATION_UNER_FORWARDING_DISABLED,
                APP_UNER_FORWARDING_NOTIFICATION_MS);
        }
    }

    if (((changedFields & APP_PREFS_FIELD_WEB_ALIVE_PERIOD) != 0u) &&
        WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_WEB) {
        appLastEspAliveMs =
            HAL_GetTick() - AppPreferences_GetWebAlivePeriodMs();
    }

    if ((changedFields & APP_PREFS_FIELD_PIN_LOCK_ENABLED) != 0u) {
        Permission_CancelLocalRequest();
        Permission_RevokeAll();
        Permission_RevokeAllRemote();
    }

    if ((changedFields & APP_PREFS_FIELD_LINE_BLACK_WIDTH) != 0u) {
        APP_LineFollowStop();
        LineFollowControl_SetLineBlackWidth(&lineFollowControl,
                                            AppPreferences_GetLineBlackWidthMm());
    }
}

uint32_t APP_USB_GetAliveCount(void)
{
    return appUsbAliveCount;
}

void UserBtn_MainTask(UserButton_Handle_t *btn)
{
    UserEvent_t local_ev = UE_NONE;

    if (IS_FLAG_SET(btn->state.flags, BTN_USER_SHORT_PRESS)) {
        CLEAR_FLAG(btn->state.flags, BTN_USER_SHORT_PRESS);
        local_ev = UE_SHORT_PRESS;
    } else if (IS_FLAG_SET(btn->state.flags, BTN_USER_LONG_PRESS)) {
        CLEAR_FLAG(btn->state.flags, BTN_USER_LONG_PRESS);
        local_ev = UE_LONG_PRESS;
    }

    UiEventRouter_HandleEvent(local_ev);
}

void Encoder_MainTask(ENC_Handle_t *enc)
{
    UserEvent_t local_ev = UE_NONE;
    int16_t scaled_steps = 0;

    if (IS_FLAG_SET(enc->flags, ENC_FLAG_UPDATED)) {
        CLEAR_FLAG(enc->flags, ENC_FLAG_UPDATED);
        local_ev = (enc->dir == ENC_DIR_CW) ? UE_ROTATE_CW : UE_ROTATE_CCW;
        scaled_steps = ENC_GetScaledSteps(enc);
    } else if (IS_FLAG_SET(enc->btnState.flags, ENC_BTN_SHORT_PRESS)) {
        CLEAR_FLAG(enc->btnState.flags, ENC_BTN_SHORT_PRESS);
        local_ev = UE_ENC_SHORT_PRESS;
    } else if (IS_FLAG_SET(enc->btnState.flags, ENC_BTN_LONG_PRESS)) {
        CLEAR_FLAG(enc->btnState.flags, ENC_BTN_LONG_PRESS);
        local_ev = UE_ENC_LONG_PRESS;
    }

    if (local_ev != UE_NONE && enc->allowEncoderInput) {
        if (local_ev == UE_ROTATE_CW || local_ev == UE_ROTATE_CCW) {
            int16_t repeats = 1;

            if (!autoMicroSystem.runtime.inside_menu_flag && autoMicroSystem.runtime.encoder_fast_scroll_enabled) {
                repeats = scaled_steps;
                if (repeats < 0) {
                    repeats = (int16_t)(-repeats);
                }
                if (repeats < 1) {
                    repeats = 1;
                }
            }

            for (int16_t i = 0; i < repeats; i++) {
                UiEventRouter_HandleEvent(local_ev);
            }
        } else {
            UiEventRouter_HandleEvent(local_ev);
        }
    }
}

void i2cManager_MainTask(void)
{
    static uint32_t lastOledErrorCount;
    I2C_Manager_Tick(&i2cManager);
    const uint32_t errorCount = ssd1306_GetErrorCount();
    if (errorCount != lastOledErrorCount) {
        lastOledErrorCount = errorCount;
        /* Repaint even if the interrupted frame was the last scheduled draw. */
        if (oledHandle.notification.active && oledHandle.notification.count > 0u) {
            oledHandle.notification.queue[oledHandle.notification.head].needsFullRender = true;
        }
        autoMicroSystem.runtime.oled_first_draw = true;
        menuSystem.renderFlag = true;
    }
}

uint8_t APP_MotorTestIsActive(void) { return appRemoteMotorTestActive; }

void APP_MotorTestStop(void)
{
    appRemoteMotorTestActive = 0u;
    motorTask.motorData.flags.byte &= (uint8_t)~ENABLE_MOVEMENT;
    MotorDrive_StopAll(&motorTask);
}

AM_Result APP_MotorTestSet(float left, float right, uint16_t duration_ms)
{
    if (APP_LineCalibrationBusy() || GET_CAR_MODE() != TEST_MODE || MotionControl_IsArmed(&motionControl) ||
        !isfinite(left) || !isfinite(right) || left < -100.0f || left > 100.0f ||
        right < -100.0f || right > 100.0f || duration_ms < 50u || duration_ms > 2000u) return HAL_ERROR;
    motorTask.motorData.motorLeft.speedPercent = (left < 0.0f) ? -left : left;
    motorTask.motorData.motorRight.speedPercent = (right < 0.0f) ? -right : right;
    motorTask.motorData.motorLeft.motorDirection = left < 0.0f ? MOTOR_DIR_BACKWARD : MOTOR_DIR_FORWARD;
    motorTask.motorData.motorRight.motorDirection = right < 0.0f ? MOTOR_DIR_BACKWARD : MOTOR_DIR_FORWARD;
    motorTask.motorData.flags.byte = (uint8_t)((MOTORNONE_SELECTED << 4u) | ENABLE_MOVEMENT);
    appRemoteMotorTestUntilMs = HAL_GetTick() + duration_ms;
    appRemoteMotorTestActive = 1u;
    MotorDrive_ApplyTestState(&motorTask);
    return HAL_OK;
}

void Motor_MainTask(void)
{
    uint8_t motionPending;
    uint8_t linePending;
    const uint32_t now = HAL_GetTick();
    const CarMode_t mode = GET_CAR_MODE();
    const MotionControl_Status_t *motionStatus;

    if (GET_CAR_MODE() == TEST_MODE) {
        if (MotionControl_IsArmed(&motionControl)) MotionControl_Disarm(&motionControl);
        if (appRemoteMotorTestActive && (int32_t)(now - appRemoteMotorTestUntilMs) >= 0)
            APP_MotorTestStop();
        appLineFollowWasActive = 0u;
        LineFollowControl_SetEnabled(&lineFollowControl, 0u);
        MotionEstimator_Reset(&motionEstimator);
        MotorDrive_ApplyTestState(&motorTask);
        return;
    }

    __disable_irq();
    motionPending = appMotionTick5msPending;
    appMotionTick5msPending = 0u;
    linePending = appLineTick10msPending;
    appLineTick10msPending = 0u;
    __enable_irq();

    if (motionPending == 0u && linePending == 0u) {
        return;
    }

    if (mode == FOLLOW_MODE && APP_LineFollowEnabled()) {
        if (linePending > 0u) {
            LineFollowControl_Task(&lineFollowControl,now);
            if (!APP_LineFollowEnabled()) { appFollowReason=(uint8_t)lineFollowControl.status.reject_reason; appLineFollowWasActive=0; }
        }
    } else {
        if (appLineFollowWasActive) {
            LineFollowControl_SetEnabled(&lineFollowControl, 0u);
            MotionControl_Stop(&motionControl, MOTION_STOP_COAST);
            appLineFollowWasActive = 0u;
        }
    }

    if (motionPending > 0u) {
        AutoMotion_Task(&appAutoMotion);
        motionStatus = MotionControl_GetStatus(&motionControl);
        MotionEstimator_Update(&motionEstimator,
                               now,
                               (motionStatus != NULL && motionStatus->balance_active) ? 1u : 0u,
                               (motionStatus != NULL && motionStatus->saturation_limited) ? 1u : 0u);
        MotionControl_Task(&motionControl, now);
        motionPending--;
    }
}

void OLED_MainTask(void)
{
    if (!appDisplayReady || !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
        return;
    }

    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_REFRESH) && menuSystem.allowPeriodicRefresh) {
        if (menuSystem.renderFn) {
            menuSystem.renderFlag = true;
        }
        CLEAR_FLAG(autoMicroSystem.runtime.systemFlags, OLED_REFRESH);
    }
    
    if (OledUtils_IsMenuScrollActive()) {
        static uint32_t last_scroll_tick = 0;
        uint32_t now = HAL_GetTick();
        if (now - last_scroll_tick >= 40) {
            last_scroll_tick = now;
            MenuScroll_Tick();
        }
    }

    OledUtils_DashboardScrollTick();

    if (menuSystem.renderFlag && menuSystem.renderFn) {
        if (ssd1306_UpdateScreenCompleted()) {
            menuSystem.renderFlag = false;
            menuSystem.renderFn();
            if (!ssd1306_UpdateScreenCompleted()) {
                return;
            }
            ssd1306_UpdateScreen();
        }
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        if (appMpuReady) {
            appMpuTick250us++;
            if (appMpuTick250us >= APP_MPU_TICKS_5MS) {
                appMpuTick250us = 0U;
                if (appMotionTick5msPending < 10u) {
                    appMotionTick5msPending++;
                }
                /* Retry after I2C errors too; the driver rejects overlapping DMA. */
                autoMicroSystem.runtime.mpu_trigger = true;
            }
        }

        appTim3Tick10ms++;
        if (appTim3Tick10ms >= APP_TIM3_TICKS_10MS) {
            appTim3Tick10ms = 0U;
            if (appInputReady) {
                UserButton_Task10ms(&btnUser);
                ENC_Task_N10ms(&encoder);
            }
            if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
                if (appUiTick10msPending < 10u) {
                    appUiTick10msPending++;
                }
            }
            if (appLineTick10msPending < 10u) {
                appLineTick10msPending++;
            }
        }
    }
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1) {
        I2C_Manager_OnTxCplt(&i2cManager, hi2c);
    }
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1) {
        I2C_Manager_OnTxCplt(&i2cManager, hi2c);
    }
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1) {
        I2C_Manager_OnRxCplt(&i2cManager, hi2c);
    }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1) {
        I2C_Manager_OnRxCplt(&i2cManager, hi2c);
    }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    if (hi2c == &hi2c1) {
        I2C_Manager_OnError(&i2cManager, hi2c);
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        WifiBridge_OnTxComplete(&wifiBridge);
    }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        WifiBridge_OnRxHalfComplete(&wifiBridge);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        WifiBridge_OnRxComplete(&wifiBridge);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1) {
        WifiBridge_OnError(&wifiBridge);
        (void)WifiBridge_Start(&wifiBridge);
    }
}

static void APP_InitMenuSystem(void)
{
    RenderFunction initialRender = menuSystem.renderFn ? menuSystem.renderFn
                                                       : OledUtils_RenderDashboard_Wrapper;

    menuSystem.root = &mainMenu;
    menuSystem.services = &appMenuServices;
    menuSystem.fast_scroll_enabled = &autoMicroSystem.runtime.encoder_fast_scroll_enabled;
    MenuSys_Init(&menuSystem);
    MenuSys_FavoritesInit(&menuSystem);
    MenuSys_SetCallbacks(&menuSystem,
                         OledUtils_Clear,
                         OledUtils_DrawItem_Wrapper,
                         initialRender,
                         &autoMicroSystem.runtime.inside_menu_flag,
                         OledUtils_RenderDashboard_Wrapper);
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = true;
    encoder.allowEncoderInput = true;
}

static void APP_InitMotorDrive(void)
{
    const uint16_t leftMinPwm = AppPreferences_GetMotorLeftMinPwm();
    const uint16_t rightMinPwm = AppPreferences_GetMotorRightMinPwm();
    const MotorDrive_InitConfig_t motorConfig = {
        .left = {
            .htim = &htim2,
            .port = &appMotorPort,
            .forward_channel = TIM_CHANNEL_1,
            .backward_channel = TIM_CHANNEL_2,
            .min_pwm_forward = leftMinPwm,
            .min_pwm_backward = leftMinPwm,
            .max_pwm = MOTOR_PWM_MAX_DEFAULT,
        },
        .right = {
            .htim = &htim2,
            .port = &appMotorPort,
            .forward_channel = TIM_CHANNEL_3,
            .backward_channel = TIM_CHANNEL_4,
            .min_pwm_forward = rightMinPwm,
            .min_pwm_backward = rightMinPwm,
            .max_pwm = MOTOR_PWM_MAX_DEFAULT,
        },
    };

    if (MotorDrive_Init(&motorTask, &motorConfig) != HAL_OK) {
        Error_Handler();
    }
}

static void APP_InitBalanceMotion(void)
{
    BalanceControl_Config_t balanceConfig;
    AppPreferencesBalancePid_t savedPid;
    MotionEstimator_Config_t estimatorConfig;
    LineFollow_Config_t lineConfig;
    const MotionControl_Config_t motionConfig = {
        .drive_ops=&appDriveOps, .balance_ops=&appBalanceOps, .orientation=&appOrientationOps,
        .motors = &motorTask,
        .balance = &balanceControl,
        .mpu = &mpuTask,
        .max_pitch_offset_deg = MOTION_CONTROL_MAX_PITCH_OFFSET_DEG_DEFAULT,
        .max_turn_percent = MOTION_CONTROL_MAX_TURN_PERCENT_DEFAULT,
        .safe_pitch_deg = MOTION_CONTROL_SAFE_PITCH_DEG_DEFAULT,
        .turn_headroom_ratio = MOTION_CONTROL_TURN_HEADROOM_RATIO_DEFAULT,
        .enabled = 1u,
    };

    BalanceControl_GetDefaultConfig(&balanceConfig);
    balanceConfig.pid_ops=&appPidOps; balanceConfig.pid_ctx=&balancePid;
    AppPreferences_GetBalancePid(&savedPid);
    balanceConfig.kp = savedPid.kp;
    balanceConfig.ki = savedPid.ki;
    balanceConfig.kd = savedPid.kd;
    if (BalanceControl_Init(&balanceControl, &balanceConfig) != HAL_OK) {
        Error_Handler();
    }

    if (MotionControl_Init(&motionControl, &motionConfig) != HAL_OK) {
        Error_Handler();
    }

    MotionEstimator_GetDefaultConfig(&estimatorConfig);
    estimatorConfig.mpu = &mpuTask;
    estimatorConfig.orientation=&appOrientationOps;
    if (MotionEstimator_Init(&motionEstimator, &estimatorConfig) != HAL_OK) {
        Error_Handler();
    }

    LineFollowControl_GetDefaultConfig(&lineConfig);
    lineConfig.read=APP_LineRead; lineConfig.command=APP_LineCommand; lineConfig.hold=APP_LineHold;
    lineConfig.line_config.black_width_mm=AppPreferences_GetLineBlackWidthMm();
    lineConfig.line_config.centered_margin_mm=fmaxf(0,16.0f-lineConfig.line_config.black_width_mm*0.5f);
    const LineCalibration_Port calPort={NULL,APP_CalLights,APP_CalRestore,APP_CalSave};
    LineCalibration_Init(&appLineCalibration,&calPort,&AppPreferences_Get()->lineCalibration);
    motionControl.config.neutral_pitch_deg=LineCalibration_Ready(&appLineCalibration.store)?appLineCalibration.store.track.pitch_deg:0;
    if (LineFollowControl_Init(&lineFollowControl, &lineConfig) != HAL_OK) {
        Error_Handler();
    }
    LineFollowControl_SetEnabled(&lineFollowControl, 0u);
}

static void APP_InitMPU(void)
{
    appMpuReady = 0U;
    appMpuTick250us = 0U;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags, MPU_GET_DATA);
    autoMicroSystem.runtime.mpu_trigger = false;

    if (I2C_Manager_IsAddressReady(&i2cManager, I2C_ADDR_MPU) != HAL_OK) {
        return;
    }

    if (MPU9250_Init(&mpuTask,
                     I2C_ADDR_MPU,
                     &mpuBusBinding.port,
                     APP_MPU_DataReady,
                     &autoMicroSystem.runtime.mpu_trigger) != HAL_OK) {
        return;
    }

    if (MPU9250_BindPort(&mpuTask) != HAL_OK) {
        return;
    }

    if (MPU9250_Configure(&mpuTask) != HAL_OK) {
        return;
    }

    if (!APP_MPU_LoadSavedCalibration()) {
        MPU9250_StartCalibration(&mpuTask, 400U, 1600U);
        appMpuCalibrationSavePending = 1U;
    }
    SET_FLAG(autoMicroSystem.runtime.systemFlags, MPU_GET_DATA);
    appMpuReady = 1U;
}

static void APP_MPU_DataReady(void)
{
    SET_FLAG(autoMicroSystem.runtime.systemFlags, MPU_GET_DATA);
}

static uint8_t APP_MPU_LoadSavedCalibration(void)
{
    AppPreferencesMpuCalibration_t prefCal;
    MPU9250_CalibrationData_t mpuCal;

    if (!AppPreferences_GetMpuCalibration(&prefCal)) {
        return 0U;
    }

    mpuCal.attitudeOffsets.accel_roll_offset_deg = prefCal.accelRollOffsetDeg;
    mpuCal.attitudeOffsets.accel_pitch_offset_deg = prefCal.accelPitchOffsetDeg;
    mpuCal.attitudeOffsets.gyro_x_offset_dps = prefCal.gyroXOffsetDps;
    mpuCal.attitudeOffsets.gyro_y_offset_dps = prefCal.gyroYOffsetDps;
    mpuCal.attitudeOffsets.gyro_z_offset_dps = prefCal.gyroZOffsetDps;
    mpuCal.gravity_x_mg = prefCal.gravityXMg;
    mpuCal.gravity_y_mg = prefCal.gravityYMg;
    mpuCal.gravity_z_mg = prefCal.gravityZMg;

    return (MPU9250_ApplyCalibrationData(&mpuTask, &mpuCal) == HAL_OK) ? 1U : 0U;
}

static void APP_MPU_SaveCalibrationIfPending(void)
{
    MPU9250_CalibrationData_t mpuCal;
    AppPreferencesMpuCalibration_t prefCal;

    if (!appMpuCalibrationSavePending || !appMpuReady) {
        return;
    }

    if (MPU9250_IsCalibrating(&mpuTask)) {
        return;
    }

    if (MPU9250_GetCalibrationData(&mpuTask, &mpuCal) != HAL_OK) {
        appMpuCalibrationSavePending = 0U;
        return;
    }

    prefCal.valid = true;
    prefCal.accelRollOffsetDeg = mpuCal.attitudeOffsets.accel_roll_offset_deg;
    prefCal.accelPitchOffsetDeg = mpuCal.attitudeOffsets.accel_pitch_offset_deg;
    prefCal.gyroXOffsetDps = mpuCal.attitudeOffsets.gyro_x_offset_dps;
    prefCal.gyroYOffsetDps = mpuCal.attitudeOffsets.gyro_y_offset_dps;
    prefCal.gyroZOffsetDps = mpuCal.attitudeOffsets.gyro_z_offset_dps;
    prefCal.gravityXMg = mpuCal.gravity_x_mg;
    prefCal.gravityYMg = mpuCal.gravity_y_mg;
    prefCal.gravityZMg = mpuCal.gravity_z_mg;

    /* Persist MPU offsets and dependent track invalidation in one transaction.
     * RAM must also invalidate the old pose if Flash fails after MPU calibration. */
    (void)AppPreferences_SetMpuCalibration(&prefCal);
    appLineCalibration.store.track.valid=0;
    appLineCalibration.store.track_white_revision=0;
    motionControl.config.neutral_pitch_deg=0;
    motionControl.status.balance_settled=0; appSettledCandidate=0;
    appMpuCalibrationSavePending = 0U;
}

void APP_MPU_RequestCalibration(void)
{
    if (APP_LineCalibrationBusy()) return;
    APP_AutoMotionCancel(); APP_LineFollowStop(); APP_MotorTestStop(); MotionControl_Disarm(&motionControl);
    appMpuCalibrationUiState = APP_MPU_CAL_UI_WAIT_PROMPT;
    appMpuCalibrationPromptStartedMs = HAL_GetTick();
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 0u;
    }
    menuSystem.renderFn = OledUtils_RenderMPUCalibration_Wrapper;
    menuSystem.userEventManagerFn = mpuCalibrationEventManager;
    if (menuSystem.clearScreen) {
        menuSystem.clearScreen();
    }
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = false;
}

uint8_t APP_MPU_IsCalibrationPromptActive(void)
{
    return (appMpuCalibrationUiState == APP_MPU_CAL_UI_WAIT_PROMPT) ? 1u : 0u;
}

uint8_t APP_MPU_IsCalibrationRunning(void)
{
    return (appMpuReady &&
            appMpuCalibrationUiState == APP_MPU_CAL_UI_RUNNING &&
            MPU9250_IsCalibrating(&mpuTask)) ? 1u : 0u;
}

uint8_t APP_MPU_GetCalibrationProgressPercent(void)
{
    if (!appMpuReady) {
        return 0u;
    }

    return MPU9250_GetCalibrationProgressPercent(&mpuTask);
}

uint8_t APP_MPU_CanRecalibrate(void)
{
    return (appMpuCalibrationUiState != APP_MPU_CAL_UI_WAIT_PROMPT &&
            APP_MPU_IsCalibrationRunning() == 0u) ? 1u : 0u;
}

uint8_t APP_MPU_IsCalibrationDoneNoticeActive(void)
{
    return (appMpuCalibrationUiState == APP_MPU_CAL_UI_DONE &&
            (uint32_t)(HAL_GetTick() - appMpuCalibrationDoneStartedMs) < APP_MPU_CAL_DONE_MS) ? 1u : 0u;
}

static void APP_MPUCalibration_Task(void)
{
    APP_MPU_SaveCalibrationIfPending();

    if (appMpuCalibrationUiState == APP_MPU_CAL_UI_WAIT_PROMPT) {
        if ((uint32_t)(HAL_GetTick() - appMpuCalibrationPromptStartedMs) < APP_MPU_CAL_PROMPT_MS) {
            return;
        }

        appMpuCalibrationUiState = APP_MPU_CAL_UI_RUNNING;

        if (appMpuReady) {
            MPU9250_StartCalibration(&mpuTask, 400U, 1600U);
            appMpuCalibrationSavePending = 1U;
            SET_FLAG(autoMicroSystem.runtime.systemFlags, MPU_GET_DATA);
        }

        if (menuSystem.insideMenuFlag) {
            *menuSystem.insideMenuFlag = 0u;
        }
        menuSystem.renderFn = OledUtils_RenderMPUCalibration_Wrapper;
        menuSystem.userEventManagerFn = mpuCalibrationEventManager;
        if (menuSystem.clearScreen) {
            menuSystem.clearScreen();
        }
        menuSystem.renderFlag = true;
        autoMicroSystem.runtime.oled_first_draw = false;
        return;
    }

    if (appMpuCalibrationUiState == APP_MPU_CAL_UI_RUNNING) {
        if (!appMpuReady || !MPU9250_IsCalibrating(&mpuTask)) {
            APP_MPU_SaveCalibrationIfPending();
            appMpuCalibrationUiState = APP_MPU_CAL_UI_DONE;
            appMpuCalibrationDoneStartedMs = HAL_GetTick();
            menuSystem.renderFlag = true;
            autoMicroSystem.runtime.oled_first_draw = false;
        }
    }
}

bool APP_DeferEspStartupRebootFailure(void)
{
    if (appEspPostInitRebootGraceUntilMs == 0u ||
        (int32_t)(appEspPostInitRebootGraceUntilMs - HAL_GetTick()) <= 0) {
        return false;
    }
    /* Includes a missing/rejected startup reboot ACK, even before first alive. */
    appEspPostInitRebootDisconnectObserved = 1u;
    return true;
}

static void APP_ESPAlive_Task(void)
{
    const uint32_t now = HAL_GetTick();
    const WifiBridge_Backend backend = WifiBridge_GetActiveBackend(&wifiBridge);
    const uint16_t webAlivePeriodMs = AppPreferences_GetWebAlivePeriodMs();
    const uint16_t alivePeriodMs = (backend == WIFI_BRIDGE_BACKEND_WEB)
        ? webAlivePeriodMs
        : ((backend == WIFI_BRIDGE_BACKEND_AT)
            ? WIFI_BRIDGE_AT_ALIVE_PERIOD_MS
            : WIFI_BRIDGE_ALIVE_PERIOD_MS);
    WifiBridge_Result aliveResult;
    uint8_t connectionLost;

    if (!appEspAliveBackendInitialized || appLastEspAliveBackend != backend) {
        appLastEspAliveBackend = backend;
        appEspAliveBackendInitialized = 1u;
        appLastEspAliveMs = now - alivePeriodMs;
        Dashboard_InvalidateConnectivity();
    }

    if ((uint32_t)(now - appLastEspAliveMs) < alivePeriodMs) {
        return;
    }
    aliveResult = WifiBridge_SendAliveWithWebPeriod(&wifiBridge, webAlivePeriodMs);
    if (aliveResult == WIFI_BRIDGE_OK) {
        appLastEspAliveMs = now;
        appEspNotifWatchdogCounter = 0u;
    }
    connectionLost = WifiBridge_ConsumeConnectionLostEvent(&wifiBridge);
    if (connectionLost) {
        Permission_RevokeAllRemote();
        Dashboard_InvalidateConnectivity();
    }

    if (appEspPostInitRebootGraceUntilMs != 0u) {
        if ((int32_t)(appEspPostInitRebootGraceUntilMs - now) > 0) {
            if (connectionLost) {
                appEspPostInitRebootDisconnectObserved = 1u;
            }
            return;
        }

        appEspPostInitRebootGraceUntilMs = 0u;
        if (appEspPostInitRebootDisconnectObserved &&
            !WifiBridge_IsConnected(&wifiBridge)) {
            
            bool showNotif = true;
            if (AppPreferences_IsEspNotifWatchdogEnabled()) {
                if (appEspNotifWatchdogCounter >= AppPreferences_GetEspNotifWatchdogLimit()) {
                    showNotif = false;
                } else {
                    appEspNotifWatchdogCounter++;
                }
            }
            if (showNotif) {
                APP_ShowConnectivitySimpleNotification(
                    OLED_SIMPLE_NOTIFICATION_ESP_CONNECTION_FAILED,
                    2000u);
            }
        }
        appEspPostInitRebootDisconnectObserved = 0u;
        return;
    }

    if (connectionLost) {
        bool showNotif = true;
        if (AppPreferences_IsEspNotifWatchdogEnabled()) {
            if (appEspNotifWatchdogCounter >= AppPreferences_GetEspNotifWatchdogLimit()) {
                showNotif = false;
            } else {
                appEspNotifWatchdogCounter++;
            }
        }
        if (showNotif) {
            APP_ShowConnectivitySimpleNotification(
                OLED_SIMPLE_NOTIFICATION_ESP_UNRESPONSIVE,
                2000u);
        }
    }
}

static void APP_ESPRebootAfterStmStartup_Task(void)
{
    const uint32_t now = HAL_GetTick();

    if (!appStmStartupComplete ||
        !appEspPostInitRebootPending ||
        UNER_App_IsMcuResetPending()) {
        return;
    }

    if ((uint32_t)(now - appEspPostInitRebootLastAttemptMs) <
        APP_ESP_POST_INIT_REBOOT_RETRY_MS) {
        return;
    }
    appEspPostInitRebootLastAttemptMs = now;

    /*
     * Send through the UNER transport without waiting for backend detection.
     * WEB firmware recognizes REBOOT_ESP; AT firmware does not recognize the
     * frame and continues with the normal AT/WEB probing that follows.
     */
    if (UNER_App_SendStartupEspReboot() == UNER_OK) {
        appEspPostInitRebootPending = 0U;
        appWifiApFallbackSyncConfirmed = 0u;
        appWifiApFallbackNextSyncMs = 0u;
        appEspPostInitRebootGraceUntilMs = now + APP_ESP_POST_INIT_REBOOT_GRACE_MS;
        if (appEspPostInitRebootGraceUntilMs == 0u) {
            appEspPostInitRebootGraceUntilMs = 1u;
        }
        appEspPostInitRebootDisconnectObserved = 0u;
    }
}

static void APP_USB_CDC_Task(void)
{
    const uint8_t dtr_active = CDC_IsDtrActive_FS();
    const uint32_t now = HAL_GetTick();
    const uint16_t alivePeriodMs = AppPreferences_GetUsbAlivePeriodMs();

    if (dtr_active != appUsbDtrLast) {
        appUsbDtrLast = dtr_active;

        if (dtr_active) {
            appUsbAliveCount = 0u;
            appUsbAliveLastMs = now - (uint32_t)alivePeriodMs;
            SET_FLAG(autoMicroSystem.runtime.systemFlags2, USB_ACTIVE);
            UNER_App_OnUsbDtrChanged(1u);
            (void)UNER_App_ReportUsbQtState(1u);
            if (appDisplayReady && IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
                OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_USB_CONNECTED, OLED_NOTIFICATION_DEFAULT_MS);
            }
        } else {
            CLEAR_FLAG(autoMicroSystem.runtime.systemFlags2, USB_ACTIVE);
            UNER_App_OnUsbDtrChanged(0u);
            (void)UNER_App_ReportUsbQtState(0u);
            if (appDisplayReady && IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
                OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_USB_DISCONNECTED, OLED_NOTIFICATION_DEFAULT_MS);
            }
        }

        menuSystem.renderFlag = true;
    }

    if (!dtr_active) {
        return;
    }

    if ((uint32_t)(now - appUsbAliveLastMs) < alivePeriodMs) {
        return;
    }

    appUsbAliveLastMs = now;
    if (UNER_App_SendUsbAlive(appUsbAliveCount + 1u) == UNER_OK) {
        appUsbAliveCount++;
    }
}

static uint8_t APP_CurrentScreenRequiresRemoteUi(void)
{
    const ScreenCode_t code = MenuSys_GetCurrentScreenCode(&menuSystem);

    return (code == SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS) ? 1u : 0u;
}

static void APP_ReturnToWifiMenuAfterCapabilityLoss(void)
{
    OledUtils_NotificationRestore();
    OledUtils_DisableContinuousRender();
    menuSystem.currentMenu = &submenu1;
    submenu1.currentItemIndex = 0;
    submenu1.firstVisibleItem = 0;
    submenu1.lastSelectedItemIndex = -1;
    submenu1.lastVisibleItem = -1;
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.allowPeriodicRefresh = false;
    if (menuSystem.insideMenuFlag) {
        *menuSystem.insideMenuFlag = 1u;
    }
    encoder.allowEncoderInput = true;
    autoMicroSystem.runtime.oled_first_draw = true;
    menuSystem.renderFlag = true;
    MenuSys_SetCurrentMenuScreenCode(&menuSystem);
}

static void APP_UICapabilityGuardTask(void)
{
    if (WifiBridge_HasCapability(&wifiBridge, WIFI_BRIDGE_CAP_REMOTE_UI)) {
        return;
    }

    if (APP_CurrentScreenRequiresRemoteUi()) {
        APP_ReturnToWifiMenuAfterCapabilityLoss();
    }
}

static void APP_UI10ms_Task(void)
{
    uint8_t pending;

    __disable_irq();
    pending = appUiTick10msPending;
    appUiTick10msPending = 0u;
    __enable_irq();

    while (pending > 0u) {
        APP_OLED_Task10ms();
        ScreenLock_Task10ms();
        pending--;
    }
}

static float APP_AbsFloat(float value)
{
    return (value < 0.0f) ? -value : value;
}

static uint8_t APP_OLED_ShouldSkipAutoRotation(void)
{
    const ScreenCode_t code = menuSystem.current_screen_code;

    return (code == SCREEN_CODE_SENSORS_DISPLAY_ROTATION_TEST ||
            code == SCREEN_CODE_SENSORS_DISPLAY_ROTATION_MPU_TEST ||
            code == SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS ||
            code == SCREEN_CODE_SETTINGS_PREFS_ROTATION_TEST ||
            code == SCREEN_CODE_SETTINGS_PREFS_ROTATION_MPU_TEST) ? 1u : 0u;
}

static void APP_OLED_ApplyRotationDegIfNeeded(uint16_t rotationDeg)
{
    const OledRotation_t rotation = APP_OledRotationFromDeg(rotationDeg);

    if (ssd1306_GetRotation() != rotation) {
        (void)OledUtils_SetRotationAndRefresh(rotation);
    }
}

static void APP_OLED_AutoRotationTask10ms(void)
{
    MPU9250_Attitude_t attitude;
    float lateralAbs;
    uint16_t nextDeg;

    if (!appDisplayReady || !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)) {
        return;
    }

    if (!AppPreferences_IsOledAutoRotationEnabled()) {
        if (appOledAutoRotationLastEnabled) {
            appOledAutoRotationLastEnabled = 0u;
            appOledAutoRotationInTiltZone = 0u;
            APP_OLED_ApplyRotationDegIfNeeded(AppPreferences_GetOledRotationDeg());
        }
        return;
    }

    if (!appOledAutoRotationLastEnabled) {
        appOledAutoRotationLastEnabled = 1u;
        appOledAutoRotationInTiltZone = 0u;
        appOledAutoRotation10msCounter = 0u;
        if (appMpuReady) {
            MPU9250_AttitudeResetYaw(&mpuTask.attitude);
        }
        APP_OLED_ApplyRotationDegIfNeeded(AppPreferences_GetOledRotationDeg());
    }

    if (APP_OLED_ShouldSkipAutoRotation()) {
        return;
    }

    appOledAutoRotation10msCounter++;
    if (appOledAutoRotation10msCounter < APP_OLED_AUTO_ROTATION_TICKS) {
        return;
    }
    appOledAutoRotation10msCounter = 0u;

    if (!appMpuReady || MPU9250_IsCalibrating(&mpuTask)) {
        return;
    }

    if (MPU9250_GetAttitude(&mpuTask, &attitude) != HAL_OK) {
        return;
    }

    lateralAbs = APP_AbsFloat(attitude.roll_deg);
    if (!appOledAutoRotationInTiltZone) {
        if (lateralAbs < APP_OLED_AUTO_ROTATION_LATERAL_ENTER_DEG) {
            APP_OLED_ApplyRotationDegIfNeeded(AppPreferences_GetOledRotationDeg());
            return;
        }

        appOledAutoRotationInTiltZone = 1u;
        MPU9250_AttitudeResetYaw(&mpuTask.attitude);
        return;
    }

    if (lateralAbs <= APP_OLED_AUTO_ROTATION_LATERAL_EXIT_DEG) {
        appOledAutoRotationInTiltZone = 0u;
        MPU9250_AttitudeResetYaw(&mpuTask.attitude);
        APP_OLED_ApplyRotationDegIfNeeded(AppPreferences_GetOledRotationDeg());
        return;
    }

    if (attitude.yaw_deg >= APP_OLED_AUTO_ROTATION_Z_POS_TRIGGER_DEG) {
        nextDeg = APP_PREFS_OLED_ROTATION_0_DEG;
    } else if (attitude.yaw_deg <= APP_OLED_AUTO_ROTATION_Z_NEG_TRIGGER_DEG) {
        nextDeg = APP_PREFS_OLED_ROTATION_180_DEG;
    } else {
        return;
    }

    APP_OLED_ApplyRotationDegIfNeeded(nextDeg);
}

static void APP_OLED_Task10ms(void)
{
    if (appOled10msCounter > APP_OLED_REFRESH_TICKS) {
        appOled10msCounter = 0U;
        SET_FLAG(autoMicroSystem.runtime.systemFlags, OLED_REFRESH);
    } else {
        appOled10msCounter++;
    }

    OledUtils_NotificationTick10ms();
    APP_OLED_AutoRotationTask10ms();
}
