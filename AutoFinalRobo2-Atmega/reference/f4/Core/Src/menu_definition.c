#include "main.h"
#include "menu_definitions.h"
#include "screenWrappers.h"
#include "notificationWrappers.h"
#include "app_preferences_binding.h"
#include "app_ap_config.h"
#include "eventManagers.h"
#include "app_tasks.h"
#include "oled_utils.h"
#include "oled_canvas.h"
#include "fonts.h"
#include "globals.h"
#include "app_wifi_binding.h"
#include "wifi_bridge_at.h"
#include "usb_mode_manager.h"
#include "osk_integration.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

static const uint8_t *MenuIcon_EncoderInvert(void);
static const uint8_t *MenuIcon_WifiApFallback(void);
static void MenuAction_RetryWifi(void);
static bool MenuItem_IsWifiRetryVisible(void);
static const uint8_t *MenuIcon_ScreenLock(void);
static const uint8_t *MenuIcon_EspNotifWatchdog(void);
static bool MenuItem_IsEspNotifWatchdogLimitVisible(void);
static void MenuAction_ToggleStabilization(void);
static const uint8_t *MenuIcon_Stabilization(void);
static const char *MenuName_Stabilization(void);

static const uint8_t *MenuIcon_IrObjects(void);
static const uint8_t *MenuIcon_IrLine(void);
static const uint8_t *MenuIcon_IrLights(void);
static const char *MenuName_IrObjects(void);
static const char *MenuName_IrLine(void);
static const char *MenuName_IrLights(void);

static const uint8_t *MenuIcon_EspNotifWatchdog(void)
{
    return AppPreferences_IsEspNotifWatchdogEnabled() ? Icon_ChoiceBulletOn_bits : Icon_ChoiceBulletOff_bits;
}

static const uint8_t *MenuIcon_IrObjects(void)
{
    return APP_IsIrObjectsEnabled()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const uint8_t *MenuIcon_IrLine(void)
{
    return APP_IsIrLineEnabled()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const uint8_t *MenuIcon_IrLights(void)
{
    return APP_IsIrLightsEnabled()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const char *MenuName_IrObjects(void)
{
    return APP_IsIrObjectsEnabled() ? "Objetos IR: Encendidos" : "Objetos IR: Apagados";
}

static const char *MenuName_IrLine(void)
{
    return APP_IsIrLineEnabled() ? "Linea IR: Encendidos" : "Linea IR: Apagados";
}

static const char *MenuName_IrLights(void)
{
    return APP_IsIrLightsEnabled() ? "Luz IR: Encendidos" : "Luz IR: Apagados";
}

static bool MenuItem_IsEspNotifWatchdogLimitVisible(void)
{
    return AppPreferences_IsEspNotifWatchdogEnabled();
}
static const uint8_t *MenuIcon_OledAutoRotation(void);
static const uint8_t *MenuIcon_UnerRouterForwarding(void);
static const uint8_t *MenuIcon_PinLock(void);
static const uint8_t *MenuIcon_WifiModeToggle(void);
static const uint8_t *MenuIcon_WebModeToggle(void);
static const char *MenuName_WifiInfo(void);
static const char *MenuName_WifiModeToggle(void);
static const char *MenuName_WebModeToggle(void);
static const char *MenuName_WifiConnectionToggle(void);
static const uint8_t *MenuIcon_WifiConnectionToggle(void);
static void MenuAction_WifiConnectionToggle(void);
static bool MenuItem_IsWifiWebStaVisible(void);
static bool MenuItem_IsWifiWebApVisible(void);
static void MenuAction_ToggleWifiApPasswordProtected(void);
static const uint8_t *MenuIcon_WifiApPasswordProtected(void);
static bool MenuItem_IsWifiApPasswordProtectedVisible(void);
static bool MenuItem_IsAtModeToggleVisible(void);
static bool MenuItem_IsAtWifiSearchVisible(void);
static bool MenuItem_IsWebBackendVisible(void);
static void MenuAction_ToggleAtTransport(void);
static const char *MenuName_AtTransport(void);
static bool MenuItem_IsUsbInstallerAvailable(void);
static void MenuAction_SelectUsbCdc(void);
static void MenuAction_SelectUsbInstaller(void);
static const uint8_t *MenuIcon_UsbCdc(void);
static const uint8_t *MenuIcon_UsbInstaller(void);

/* ---------- submenu ESP ---------- */

MenuItem submenuESPItems[] = {
    {"Chequear ESP", NULL, NULL, Icon_Link_bits, OledUtils_RenderESPCheckConnection_Wrapper, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_ESP_CHECKING},
    {"Firmware",    NULL, NULL, Icon_Info_bits,  OledUtils_RenderESPFirmwareRequest_Wrapper, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_ESP_FIRMWARE_REQUEST},
    {"Reset ESP",   NULL, NULL, Icon_Refrescar_bits, OledUtils_RenderESPResetMode_Wrapper, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_ESP_REBOOT_MODE},
    {"Volver", MenuSys_GoBack_Wrapper, &mainMenu, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CORE_MAIN_MENU}
};

SubMenu submenuESP = {
    .name = "Enlace ESP",
    .items = submenuESPItems,
    .itemCount = sizeof(submenuESPItems)/sizeof(submenuESPItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu1,
    .icon = NULL,
    .screen_code = SCREEN_CODE_CONNECTIVITY_ESP_MENU
};

/* ---------- Submenu USB ---------- */

MenuItem submenuUSBItems[] = {
    {"Comunicacion Qt", MenuAction_SelectUsbCdc, NULL, Icon_Link_bits, NULL, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_USB_CONNECTED, MenuIcon_UsbCdc},
    {"Instalador Win", MenuAction_SelectUsbInstaller, NULL, Icon_Tool_bits, NULL, menuEventManager, MenuItem_IsUsbInstallerAvailable, SCREEN_CODE_CONNECTIVITY_USB_DISCONNECTED, MenuIcon_UsbInstaller},
    {"Volver", MenuSys_GoBack_Wrapper, &mainMenu, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CORE_MAIN_MENU}
};

SubMenu submenuUSB = {
    .name = "USB",
    .items = submenuUSBItems,
    .itemCount = sizeof(submenuUSBItems) / sizeof(submenuUSBItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &mainMenu,
    .icon = NULL,
    .screen_code = SCREEN_CODE_CONNECTIVITY_USB_CONNECTED
};

/* ---------- Menu System ---------- */

MenuSystem menuSystem = {
    .currentMenu = &mainMenu,
    .clearScreen = OledUtils_Clear,
    .drawItem = OledUtils_DrawItem_Wrapper,
    .renderFn = OledUtils_RenderStartupNotification_Wrapper,
    .insideMenuFlag = &autoMicroSystem.runtime.inside_menu_flag,
    .renderFlag = false,
    .current_screen_code = SCREEN_CODE_CORE_STARTUP,
    .last_reported_screen_code = SCREEN_CODE_NONE,
    .current_screen_source = SCREEN_REPORT_SOURCE_SYSTEM,
    .screen_report_pending = false,
    .current_selected_index = MENU_SELECTION_INVALID_INDEX,
    .last_reported_selected_index = MENU_SELECTION_INVALID_INDEX,
    .current_item_count = 0u,
    .last_reported_item_count = 0u,
    .current_selection_source = SCREEN_REPORT_SOURCE_UNKNOWN,
    .selection_report_pending = false
};

/* ---------- Main menu ---------- */

static void MenuAction_EnterWifiAp(void)
{
    if (!WifiBridge_IsConnected(&wifiBridge)) {
        OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_ESP_WIFI_DISCONNECTED, OLED_NOTIFICATION_DEFAULT_MS);
    } else {
        MenuSys_OpenSubMenu(&menuSystem, &submenu1);
    }
}


static SubMenu submenuLineCalibration;
static uint32_t localLineCalSession;
static uint8_t localLineNotice;
static bool MenuItem_IsFollowModeVisible(void) { return GET_CAR_MODE()==FOLLOW_MODE; }
static const char *LineNoticeText(uint8_t reason) {
    switch(reason) {
        case LINE_FOLLOW_REJECT_CALIBRATION:return "Calibracion pendiente";
        case LINE_FOLLOW_REJECT_DISARMED:return "Activar balance";
        case LINE_FOLLOW_REJECT_NOT_SETTLED:return "Esperar equilibrio";
        case LINE_FOLLOW_REJECT_NOT_CENTERED:return "Alinear sobre pista";
        case LINE_FOLLOW_REJECT_BUSY:return "Calibracion en curso";
        case LINE_FOLLOW_REJECT_QUALITY:return "Contraste insuficiente";
        case LINE_FOLLOW_REJECT_UNSTABLE:return "Auto no esta quieto";
        case LINE_FOLLOW_REJECT_STORAGE:return "Error al guardar";
        case LINE_FOLLOW_REJECT_TIMEOUT:return "Captura vencida";
        default:return "Lectura no disponible";
    }
}
static void MenuLineBack(void) {
    OledUtils_DisableContinuousRender();
    menuSystem.renderFn=MenuSys_RenderMenu_Wrapper; menuSystem.userEventManagerFn=menuEventManager;
    MenuSys_OpenSubMenu(&menuSystem,&submenuSensorsIr);
    autoMicroSystem.runtime.inside_menu_flag=true; menuSystem.renderFlag=true;
}
static void MenuLineNoticeEvents(UserEvent_t ev) {
    if (ev==UE_SHORT_PRESS || ev==UE_ENC_SHORT_PRESS || ev==UE_ENC_LONG_PRESS) {
        menuSystem.renderFn=MenuSys_RenderMenu_Wrapper; menuSystem.userEventManagerFn=menuEventManager;
        MenuSys_OpenSubMenu(&menuSystem,&mainMenu); menuSystem.renderFlag=true;
    }
}
static void MenuRender_LineNotice(void) {
    OledUtils_Clear(); MenuSys_SetCurrentScreenCode(&menuSystem,SCREEN_CODE_LINE_NOTICE,SCREEN_REPORT_SOURCE_NOTIFICATION);
    OledUtils_RenderLineNotice(LineNoticeText(localLineNotice),
        localLineNotice==LINE_FOLLOW_REJECT_CALIBRATION?"Blanco y pista":"Seguir detenido");
}
static void MenuAction_LineFollow(void) {
    localLineNotice=APP_LineFollowSetEnabled(!APP_LineFollowEnabled());
    if (localLineNotice) {
        menuSystem.renderFn=MenuRender_LineNotice; menuSystem.userEventManagerFn=MenuLineNoticeEvents;
        autoMicroSystem.runtime.inside_menu_flag=false;
    }
    menuSystem.renderFlag=true;
}
static const char *MenuName_LineFollow(void) { return APP_LineFollowEnabled()?"Detener seguimiento":"Seguir pista"; }
static void MenuLineCalEvents(UserEvent_t ev) {
    uint8_t state[APP_LINE_STATE_BYTES]; APP_GetLineState(state,0);
    if (ev==UE_ENC_LONG_PRESS) {
        if (state[12] || localLineCalSession) (void)APP_LineCalibrationControl(3,localLineCalSession);
        MenuLineBack(); return;
    }
    if (ev==UE_ENC_SHORT_PRESS || ev==UE_SHORT_PRESS) {
        if (state[2]==LINE_CAL_PROMPT) localLineNotice=APP_LineCalibrationControl(2,localLineCalSession);
        else if (!APP_LineCalibrationBusy()) MenuLineBack();
        menuSystem.renderFlag=true;
    }
}
static void MenuRender_LineCalibration(void) {
    uint8_t s[APP_LINE_STATE_BYTES]; APP_GetLineState(s,0);
    MenuSys_SetCurrentScreenCode(&menuSystem,s[3]?SCREEN_CODE_LINE_CAL_TRACK:SCREEN_CODE_LINE_CAL_WHITE,SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_RenderLineCalibration(s,LineNoticeText(s[6]));
}
static void MenuAction_LineCal(uint8_t target) {
    localLineNotice=APP_LineCalibrationControl(target,0);
    if (localLineNotice) {
        menuSystem.renderFn=MenuRender_LineNotice; menuSystem.userEventManagerFn=MenuLineNoticeEvents;
    } else {
        uint8_t s[APP_LINE_STATE_BYTES]; APP_GetLineState(s,0);
        localLineCalSession=(uint32_t)s[12]|((uint32_t)s[13]<<8)|((uint32_t)s[14]<<16)|((uint32_t)s[15]<<24);
        menuSystem.renderFn=MenuRender_LineCalibration; menuSystem.userEventManagerFn=MenuLineCalEvents;
        OledUtils_EnableContinuousRender();
    }
    autoMicroSystem.runtime.inside_menu_flag=false; menuSystem.renderFlag=true;
}
static void MenuAction_LineWhite(void) { MenuAction_LineCal(0); }
static void MenuAction_LineTrack(void) { MenuAction_LineCal(1); }
static void MenuAction_EnterIr(void) { submenuSensorsIr.parent=&mainMenu; MenuSys_OpenSubMenu(&menuSystem,&submenuSensorsIr); }
static void MenuAction_IrBack(void) { MenuSys_OpenSubMenu(&menuSystem,GET_CAR_MODE()==FOLLOW_MODE?&mainMenu:&submenu2); }
static MenuItem submenuLineCalibrationItems[] = {
    {"Calibrar blanco",MenuAction_LineWhite,NULL,Icon_Tool_bits,NULL,NULL,NULL,SCREEN_CODE_LINE_CAL_WHITE},
    {"Calibrar sobre pista",MenuAction_LineTrack,NULL,Icon_Tool_bits,NULL,NULL,NULL,SCREEN_CODE_LINE_CAL_TRACK},
    {"Volver",MenuSys_GoBack_Wrapper,&submenuSensorsIr,Icon_Volver_bits,MenuSys_RenderMenu_Wrapper,menuEventManager,NULL,SCREEN_CODE_SENSORS_IR_MENU}
};
static SubMenu submenuLineCalibration = {
    .name="Calibrar linea",.items=submenuLineCalibrationItems,.itemCount=3,
    .parent=&submenuSensorsIr,.screen_code=SCREEN_CODE_LINE_CAL_MENU
};

static void MenuAction_Motors(void);
static bool MenuItem_MotorCalibrationVisible(void);
MenuItem mainMenuItems[] = {
    {"Motores",MenuAction_Motors,NULL,Icon_Motor_bits,MenuSys_RenderMenu_Wrapper,menuEventManager,MenuItem_MotorCalibrationVisible,SCREEN_CODE_MOTOR_MENU},
    {"Seguir pista",MenuAction_LineFollow,NULL,Icon_Tool_bits,NULL,NULL,MenuItem_IsFollowModeVisible,SCREEN_CODE_LINE_FOLLOW,NULL,MenuName_LineFollow},
    {"Infrarrojos",MenuAction_EnterIr,NULL,Icon_Sensors_bits,MenuSys_RenderMenu_Wrapper,menuEventManager,MenuItem_IsFollowModeVisible,SCREEN_CODE_SENSORS_IR_MENU},
    {"WiFi", MenuAction_EnterWifiAp, NULL, Icon_Wifi_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_WIFI_MENU},
    {"Testeo", NULL, &submenu2, Icon_Tool_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, MenuItem_IsTestModeVisible, SCREEN_CODE_SENSORS_MENU},
    {"Balance", NULL, &submenuBalance, Icon_Balance_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_BALANCE_MENU},
    {"Configuracion", NULL, &submenu3, Icon_Config_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU},
    { "Volver", MenuSys_GoBack_Wrapper, NULL, Icon_Volver_bits, OledUtils_RenderDashboard_Wrapper, dashboardEventManager, NULL, SCREEN_CODE_CORE_DASHBOARD },
};

SubMenu mainMenu = {
    .name = "Principal",
    .items = mainMenuItems,
    .itemCount = sizeof(mainMenuItems)/sizeof(mainMenuItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = NULL,
    .icon = NULL,
    .screen_code = SCREEN_CODE_CORE_MAIN_MENU
};

/* ---------- Submenu WIFI ---------- */

MenuItem submenuConfigApItems[] = {
    {"Proteger c/pass", MenuAction_ToggleWifiApPasswordProtected, NULL, NULL, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_WIFI_MENU, MenuIcon_WifiApPasswordProtected},
    {"Ingresar SSID", OledUtils_WifiApSsidEdit_Wrapper, NULL, Icon_Tool_bits, OSK_Integration_Render, NULL, NULL, SCREEN_CODE_CONNECTIVITY_OSK_WIFI_SSID},
    {"Ingresar contraseÃ±a", OledUtils_WifiApPasswordEdit_Wrapper, NULL, Icon_Tool_bits, OSK_Integration_Render, NULL, MenuItem_IsWifiApPasswordProtectedVisible, SCREEN_CODE_CONNECTIVITY_OSK_WIFI_PASSWORD},
    {"Ingresar IPv4", OledUtils_WifiApIpv4Edit_Wrapper, NULL, Icon_Tool_bits, OSK_Integration_Render, NULL, NULL, SCREEN_CODE_CONNECTIVITY_OSK_WIFI_IPV4},
    {"Volver", MenuSys_GoBack_Wrapper, &submenu1, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_WIFI_MENU}
};

SubMenu submenuConfigAp = {
    .name = "Config AP",
    .items = submenuConfigApItems,
    .itemCount = sizeof(submenuConfigApItems)/sizeof(submenuConfigApItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu1,
    .icon = NULL,
    .screen_code = SCREEN_CODE_CONNECTIVITY_WIFI_MENU
};

MenuItem submenu1Items[] = {
    {"", MenuAction_WifiConnectionToggle, NULL, NULL, MenuSys_RenderMenu_Wrapper, menuEventManager, MenuItem_IsWebBackendVisible, SCREEN_CODE_CONNECTIVITY_WIFI_MENU, MenuIcon_WifiConnectionToggle, MenuName_WifiConnectionToggle},
    {"", MenuAction_ToggleAtTransport, NULL, Icon_Link_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, MenuItem_IsAtModeToggleVisible, SCREEN_CODE_CONNECTIVITY_WIFI_MENU, NULL, MenuName_AtTransport},
    {"Info", NULL, NULL, Icon_Info_bits, OledUtils_RenderWiFiConnectionStatus_Wrapper, menuEventManager, NULL, SCREEN_CODE_CONNECTIVITY_WIFI_STATUS, NULL, MenuName_WifiInfo},
    {"Buscar red", NULL, NULL, Icon_Refrescar_bits, OledUtils_RenderWiFiSearching_Wrapper, WiFiSearch_UserEventManager, MenuItem_IsWifiWebStaVisible, SCREEN_CODE_CONNECTIVITY_WIFI_SEARCHING},
    {"Pasar a modo AP", OledUtils_ToggleESPWebMode_Wrapper, NULL, NULL, NULL, NULL, MenuItem_IsWebBackendVisible, SCREEN_CODE_CONNECTIVITY_ESP_REBOOT_MODE, MenuIcon_WebModeToggle, MenuName_WebModeToggle},
    {"Modo AP", OledUtils_StartESPAP_Wrapper, NULL, Icon_APWifi_bits, NULL, NULL, MenuItem_IsAtModeToggleVisible, SCREEN_CODE_CONNECTIVITY_ESP_REBOOT_MODE, MenuIcon_WifiModeToggle, MenuName_WifiModeToggle},
    {"Buscar red", NULL, NULL, Icon_Refrescar_bits, OledUtils_RenderWiFiSearching_Wrapper, WiFiSearch_UserEventManager, MenuItem_IsAtWifiSearchVisible, SCREEN_CODE_CONNECTIVITY_WIFI_SEARCHING},
    {"Configurar AP", NULL, &submenuConfigAp, Icon_Tool_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, MenuItem_IsWifiWebApVisible, SCREEN_CODE_CONNECTIVITY_WIFI_MENU},
    {"Conexion ESP", NULL, &submenuESP, Icon_Link_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, MenuItem_IsWifiWebStaVisible, SCREEN_CODE_CONNECTIVITY_ESP_MENU},
    {"Volver", MenuSys_GoBack_Wrapper, &mainMenu, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CORE_MAIN_MENU},
    {"Reintentar WiFi", MenuAction_RetryWifi, NULL, Icon_Refrescar_bits, NULL, menuEventManager, MenuItem_IsWifiRetryVisible, SCREEN_CODE_CONNECTIVITY_WIFI_RETRY}
};

SubMenu submenu1 = {
    .name = "Wifi",
    .items = submenu1Items,
    .itemCount = sizeof(submenu1Items)/sizeof(submenu1Items[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &mainMenu,
    .icon = NULL,
    .screen_code = SCREEN_CODE_CONNECTIVITY_WIFI_MENU
};

/* ---------- Submenu Sensores ---------- */

MenuItem submenuSensorsIrItems[] = {
    {"Calibrar",NULL,&submenuLineCalibration,Icon_Tool_bits,MenuSys_RenderMenu_Wrapper,menuEventManager,NULL,SCREEN_CODE_LINE_CAL_MENU},
    {"Valores IR", NULL, NULL, Icon_Tool_bits, OledUtils_RenderValoresIR_Wrapper, IRValues_UserEventManager, NULL, SCREEN_CODE_SENSORS_IR_VALUES},
    {"IR ambiente", NULL, NULL, Icon_Tool_bits, OledUtils_RenderIRAmbiente_Wrapper, IRValues_UserEventManager, NULL, SCREEN_CODE_SENSORS_IR_AMBIENT},
    {"Test ruta", NULL, NULL, Icon_Tool_bits, OledUtils_RenderTestCamino_Wrapper, RouteAlignment_UserEventManager, NULL, SCREEN_CODE_SENSORS_ROUTE_ALIGNMENT},
    {"Volver", MenuAction_IrBack, NULL, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SENSORS_MENU}
};

SubMenu submenuSensorsIr = {
    .name = "Infrarrojos",
    .items = submenuSensorsIrItems,
    .itemCount = sizeof(submenuSensorsIrItems)/sizeof(submenuSensorsIrItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu2,
    .icon = Icon_Sensors_bits,
    .screen_code = SCREEN_CODE_SENSORS_IR_MENU
};

MenuItem submenuSensorsMpuItems[] = {
    {"Valores MPU", NULL, NULL, Icon_Tool_bits, OledUtils_RenderValoresMPU_Wrapper, ReadOnly_UserEventManager, NULL, SCREEN_CODE_SENSORS_MPU_VALUES},
    {"Recalibrar", MPUCalibration_RequestFromMenu, NULL, Icon_Refrescar_bits, OledUtils_RenderMPUCalibration_Wrapper, mpuCalibrationEventManager, NULL, SCREEN_CODE_SENSORS_MPU_CALIBRATION},
    {"Volver", MenuSys_GoBack_Wrapper, &submenu2, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SENSORS_MENU}
};

SubMenu submenuSensorsMpu = {
    .name = "MPU",
    .items = submenuSensorsMpuItems,
    .itemCount = sizeof(submenuSensorsMpuItems)/sizeof(submenuSensorsMpuItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu2,
    .icon = Icon_Tool_bits,
    .screen_code = SCREEN_CODE_SENSORS_MPU_MENU
};

MenuItem submenuSensorsDisplayItems[] = {
    {"Test orient.", Preferences_BeginOledRotationTest, NULL, Icon_Smartphone_bits, OledUtils_RenderOledRotationTest_Wrapper, oledRotationTestEventManager, NULL, SCREEN_CODE_SENSORS_DISPLAY_ROTATION_TEST},
    {"Auto MPU", Preferences_BeginOledRotationMpuTest, NULL, Icon_Smartphone_bits, OledUtils_RenderOledRotationMpuTest_Wrapper, oledRotationMpuTestEventManager, NULL, SCREEN_CODE_SENSORS_DISPLAY_ROTATION_MPU_TEST},
    {"OLED Canvas", OledCanvas_BeginFromMenu, NULL, Icon_Smartphone_bits, OledUtils_RenderOledCanvas_Wrapper, oledCanvasEventManager, NULL, SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS},
    {"Volver", MenuSys_GoBack_Wrapper, &submenu2, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SENSORS_MENU}
};

SubMenu submenuSensorsDisplay = {
    .name = "Pantalla",
    .items = submenuSensorsDisplayItems,
    .itemCount = sizeof(submenuSensorsDisplayItems)/sizeof(submenuSensorsDisplayItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu2,
    .icon = Icon_Smartphone_bits,
    .screen_code = SCREEN_CODE_SENSORS_DISPLAY_MENU
};

MenuItem submenuUnerRouterItems[] = {
    {"USB CDC", OledUtils_BeginUnerRouterUsb, NULL, Icon_USB_bits, OledUtils_RenderUnerRouterTest_Wrapper, UnerRouterTest_UserEventManager, NULL, SCREEN_CODE_SERVICE_UNER_ROUTER_USB},
    {"UART ESP", OledUtils_BeginUnerRouterUart, NULL, Icon_Link_bits, OledUtils_RenderUnerRouterTest_Wrapper, UnerRouterTest_UserEventManager, NULL, SCREEN_CODE_SERVICE_UNER_ROUTER_UART},
    {"NRF24", OledUtils_BeginUnerRouterNrf, NULL, Icon_RF_bits, OledUtils_RenderUnerRouterTest_Wrapper, UnerRouterTest_UserEventManager, NULL, SCREEN_CODE_SERVICE_UNER_ROUTER_NRF, NULL, NULL, MENU_EXT_MASK(EXT_PROFILE_NRF24)},
    {"Volver", MenuSys_GoBack_Wrapper, &submenu2, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SENSORS_MENU}
};

SubMenu submenuUnerRouter = {
    .name = "UNER Router",
    .items = submenuUnerRouterItems,
    .itemCount = sizeof(submenuUnerRouterItems)/sizeof(submenuUnerRouterItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu2,
    .icon = Icon_Link_bits,
    .screen_code = SCREEN_CODE_SERVICE_UNER_ROUTER_MENU
};

static void OskTest_OnComplete(OSK_State *st, void *ctx)
{
    (void)st;
    (void)ctx;
    MenuSys_SetCurrentScreenCode(&menuSystem, SCREEN_CODE_SENSORS_KEYBOARD_MENU, SCREEN_REPORT_SOURCE_RENDER);
    menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
    menuSystem.userEventManagerFn = menuEventManager;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.inside_menu_flag = true;
}

static void OskTest_OnCancel(void *ctx)
{
    OskTest_OnComplete(NULL, ctx);
}

static void MenuSys_OskTestAlpha_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem, SCREEN_CODE_TEST_KEYBOARD_ASCII, SCREEN_REPORT_SOURCE_RENDER);
    OSK_Integration_OpenAlpha("", 20, NULL, OskTest_OnComplete, OskTest_OnCancel, NULL);
}

static void MenuSys_OskTestNumeric_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem, SCREEN_CODE_TEST_KEYBOARD_NUMERIC, SCREEN_REPORT_SOURCE_RENDER);
    OSK_NumericConfig numCfg = {
        .mode = OSK_NUMERIC_MODE_FLOAT,
        .maxIntegerDigits = 5,
        .allowNegative = true,
        .prefillZero = true,
        .minIntValue = -99999,
        .maxIntValue = 99999,
        .initialIntValue = 0,
#if OSK_SUPPORT_FLOAT
        .maxDecimalDigits = 3,
        .minFloatValue = -99999.0f,
        .maxFloatValue = 99999.0f,
        .initialFloatValue = 0.0f
#endif
    };
    OSK_Integration_OpenNumeric(&numCfg, OskTest_OnComplete, OskTest_OnCancel, NULL);
}

MenuItem submenuTestKeyboardItems[] = {
    {"Teclado ASCII", MenuSys_OskTestAlpha_Wrapper, NULL, keyboardIcon, NULL, NULL, NULL, SCREEN_CODE_TEST_KEYBOARD_ASCII},
    {"Teclado numerico", MenuSys_OskTestNumeric_Wrapper, NULL, Icon_Numpad_bits, NULL, NULL, NULL, SCREEN_CODE_TEST_KEYBOARD_NUMERIC},
    {"Volver", MenuSys_GoBack_Wrapper, &submenu2, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SENSORS_MENU}
};

SubMenu submenuTestKeyboard = {
    .name = "Test Teclado",
    .items = submenuTestKeyboardItems,
    .itemCount = sizeof(submenuTestKeyboardItems)/sizeof(submenuTestKeyboardItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu2,
    .icon = Icon_KeyboardsTypes_bits,
    .screen_code = SCREEN_CODE_SENSORS_KEYBOARD_MENU
};

MenuItem submenu2Items[] = {
    {"Test Teclado", NULL, &submenuTestKeyboard, Icon_KeyboardsTypes_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SENSORS_KEYBOARD_MENU},
    {"Infrarrojos", NULL, &submenuSensorsIr, Icon_Sensors_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SENSORS_IR_MENU},
    {"MPU", NULL, &submenuSensorsMpu, Icon_Compass_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SENSORS_MPU_MENU},
    {"UNER Router", NULL, &submenuUnerRouter, Icon_Router_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SERVICE_UNER_ROUTER_MENU},
    {"Motores", NULL, &submenuMotors, Icon_Motor_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_MOTOR_MENU},
    {"Volver", MenuSys_GoBack_Wrapper, &mainMenu, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CORE_MAIN_MENU}
};

SubMenu submenu2 = {
    .name = "Testeo",
    .items = submenu2Items,
    .itemCount = sizeof(submenu2Items)/sizeof(submenu2Items[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &mainMenu,
    .icon = NULL,
    .screen_code = SCREEN_CODE_SENSORS_MENU
};

/* ---------- Submenu Motores ---------- */


static uint8_t localMotorCalReason,localMotorCalField;
static float localMotorCalPercent=12.0f;
static uint32_t localMotorCalRamp=500u;
static bool MenuItem_MotorCalibrationVisible(void){return GET_CAR_MODE()==IDLE_MODE||GET_CAR_MODE()==GOTO_MODE;}
static void MenuMotorBack(void){
 OledUtils_DisableContinuousRender();menuSystem.renderFn=MenuSys_RenderMenu_Wrapper;menuSystem.userEventManagerFn=menuEventManager;
 MenuSys_OpenSubMenu(&menuSystem,&submenuMotors);autoMicroSystem.runtime.inside_menu_flag=true;menuSystem.renderFlag=true;
}
static void MenuMotorCalRender(void){
 uint8_t s[AUTO_MOTION_STATE_BYTES];APP_AutoMotionRequest(0x27u,NULL,0,s);
 MenuSys_SetCurrentScreenCode(&menuSystem,SCREEN_CODE_MOTOR_CALIBRATION,SCREEN_REPORT_SOURCE_RENDER);
 OledUtils_RenderMotorCalibration(s,localMotorCalReason,localMotorCalPercent,localMotorCalRamp,localMotorCalField);
}
static void MenuMotorCalEvents(UserEvent_t ev){
 if(ev==UE_ENC_LONG_PRESS){APP_AutoMotionCancel();MenuMotorBack();return;}
 if(ev==UE_ENC_SHORT_PRESS){localMotorCalField=(localMotorCalField+1u)%2u;menuSystem.renderFlag=true;return;}
 if(!APP_AutoMotionBusy()&&(ev==UE_ROTATE_CW||ev==UE_ROTATE_CCW)){
  int direction=ev==UE_ROTATE_CW?1:-1;localMotorCalReason=0;
  if(localMotorCalField==0)localMotorCalPercent=fminf(20,fmaxf(1,localMotorCalPercent+direction));
  else localMotorCalRamp=(uint32_t)fminf(5000,fmaxf(0,(float)localMotorCalRamp+direction*100));
  menuSystem.renderFlag=true;return;
 }
 if(ev==UE_SHORT_PRESS){
  uint8_t state[AUTO_MOTION_STATE_BYTES],request[13]={0};APP_AutoMotionRequest(0x27u,NULL,0,state);
  if(APP_AutoMotionBusy()){APP_AutoMotionCancel();return;}
  if(state[2]==1&&state[3]==3&&state[107]){(void)APP_BalanceSetEnabled(false);request[0]=3;}
  memcpy(request+1,&localMotorCalPercent,4);memcpy(request+5,&localMotorCalRamp,4);
  uint32_t timeout=30000;memcpy(request+9,&timeout,4);APP_AutoMotionRequest(0x28u,request,sizeof(request),state);localMotorCalReason=state[4];menuSystem.renderFlag=true;
 }
}
static void MenuMotorCalBegin(void){
 localMotorCalReason=0;localMotorCalField=0;menuSystem.renderFn=MenuMotorCalRender;menuSystem.userEventManagerFn=MenuMotorCalEvents;
 autoMicroSystem.runtime.inside_menu_flag=false;menuSystem.renderFlag=true;OledUtils_EnableContinuousRender();
}
static void MenuAction_Motors(void){submenuMotors.parent=GET_CAR_MODE()==TEST_MODE?&submenu2:&mainMenu;MenuSys_OpenSubMenu(&menuSystem,&submenuMotors);}
static void MenuAction_MotorsBack(void){MenuSys_OpenSubMenu(&menuSystem,GET_CAR_MODE()==TEST_MODE?&submenu2:&mainMenu);}

MenuItem submenuMotorsItems[] = {
    {"Calibracion 1m",MenuMotorCalBegin,NULL,Icon_Tool_bits,NULL,NULL,MenuItem_MotorCalibrationVisible,SCREEN_CODE_MOTOR_CALIBRATION},
    {"Motor izquierdo", MotorSingleTest_BeginLeft, NULL, Icon_Motor_bits, OledUtils_RenderSingleMotorTest_Wrapper, motorSingleTestEventManager, MenuItem_IsTestModeVisible, SCREEN_CODE_MOTOR_LEFT_TEST},
    {"Motor derecho", MotorSingleTest_BeginRight, NULL, Icon_Motor_bits, OledUtils_RenderSingleMotorTest_Wrapper, motorSingleTestEventManager, MenuItem_IsTestModeVisible, SCREEN_CODE_MOTOR_RIGHT_TEST},
    {"Volver", MenuAction_MotorsBack, NULL, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CORE_MAIN_MENU}
};

SubMenu submenuMotors = {
    .name = "Motores",
    .items = submenuMotorsItems,
    .itemCount = sizeof(submenuMotorsItems)/sizeof(submenuMotorsItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu2,
    .icon = Icon_Motor_bits,
    .screen_code = SCREEN_CODE_MOTOR_MENU
};

/* ---------- Submenu Balance ---------- */

MenuItem submenuBalanceItems[] = {
    {"PID: Desactivado", MenuAction_ToggleStabilization, NULL, Icon_ChoiceBulletOff_bits, NULL, menuEventManager, NULL, SCREEN_CODE_BALANCE_MENU, MenuIcon_Stabilization, MenuName_Stabilization},
    {"Estado PID", NULL, NULL, Icon_Info_bits, OledUtils_RenderBalancePidStatus_Wrapper, ReadOnly_UserEventManager, NULL, SCREEN_CODE_BALANCE_PID_STATUS},
    {"Log PID", BalancePid_BeginLogConfig, NULL, Icon_Config_bits, OledUtils_RenderBalancePidLog_Wrapper, balancePidLogEventManager, NULL, SCREEN_CODE_BALANCE_PID_LOG},
    {"Kp", BalancePid_BeginKpEdit, NULL, Icon_Tool_bits, OledUtils_RenderBalancePidGain_Wrapper, balancePidGainEventManager, NULL, SCREEN_CODE_BALANCE_PID_KP},
    {"Ki", BalancePid_BeginKiEdit, NULL, Icon_Tool_bits, OledUtils_RenderBalancePidGain_Wrapper, balancePidGainEventManager, NULL, SCREEN_CODE_BALANCE_PID_KI},
    {"Kd", BalancePid_BeginKdEdit, NULL, Icon_Tool_bits, OledUtils_RenderBalancePidGain_Wrapper, balancePidGainEventManager, NULL, SCREEN_CODE_BALANCE_PID_KD},
    {"Volver", MenuSys_GoBack_Wrapper, &mainMenu, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CORE_MAIN_MENU}
};

SubMenu submenuBalance = {
    .name = "Balance",
    .items = submenuBalanceItems,
    .itemCount = sizeof(submenuBalanceItems)/sizeof(submenuBalanceItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &mainMenu,
    .icon = Icon_Balance_bits,
    .screen_code = SCREEN_CODE_BALANCE_MENU
};

/* ---------- Submenu Acerca de ---------- */

MenuItem submenuAboutItems[] = {
    {"Info", NULL, NULL, Icon_Info_bits, OledUtils_AboutInfo_Wrapper, ReadOnly_UserEventManager, NULL, SCREEN_CODE_SETTINGS_ABOUT_PROJECT},
    {"Repositorio", NULL, NULL, Icon_Info_bits, OledUtils_AboutRepo_Wrapper, ReadOnly_UserEventManager, NULL, SCREEN_CODE_SETTINGS_ABOUT_REPO},
    {"Build", NULL, NULL, Icon_Info_bits, OledUtils_AboutBuild_Wrapper, ReadOnly_UserEventManager, NULL, SCREEN_CODE_SETTINGS_ABOUT_BUILD},
    {"Bootloader", NULL, NULL, Icon_Info_bits, OledUtils_AboutBootloader_Wrapper, ReadOnly_UserEventManager, NULL, SCREEN_CODE_SETTINGS_ABOUT_BOOTLOADER},
    {"Volver", MenuSys_GoBack_Wrapper, &submenu3, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_MENU}
};

SubMenu submenuAbout = {
    .name = "Acerca de",
    .items = submenuAboutItems,
    .itemCount = sizeof(submenuAboutItems)/sizeof(submenuAboutItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu3,
    .icon = NULL,
    .screen_code = SCREEN_CODE_SETTINGS_ABOUT_MENU
};

/* ---------- Submenu Preferencias ---------- */

MenuItem submenuPreferencesEncoderItems[] = {
    {"Invertir giro", Preferences_ToggleEncoderInvert, NULL, Icon_ChoiceBulletOff_bits, NULL, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_ENCODER_INVERT, MenuIcon_EncoderInvert},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferences, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU}
};

SubMenu submenuPreferencesEncoder = {
    .name = "Encoder",
    .items = submenuPreferencesEncoderItems,
    .itemCount = sizeof(submenuPreferencesEncoderItems)/sizeof(submenuPreferencesEncoderItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferences,
    .icon = Icon_Encoder_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_ENCODER_MENU
};

MenuItem submenuPreferencesDisplayItems[] = {
    {"Rot. automatica", Preferences_ToggleOledAutoRotation, NULL, Icon_ChoiceBulletOff_bits, NULL, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_ROTATION_AUTO, MenuIcon_OledAutoRotation},
    {"Test orient.", Preferences_BeginOledRotationTest, NULL, Icon_Smartphone_bits, OledUtils_RenderOledRotationTest_Wrapper, oledRotationTestEventManager, NULL, SCREEN_CODE_SENSORS_DISPLAY_ROTATION_TEST},
    {"Auto MPU", Preferences_BeginOledRotationMpuTest, NULL, Icon_Smartphone_bits, OledUtils_RenderOledRotationMpuTest_Wrapper, oledRotationMpuTestEventManager, NULL, SCREEN_CODE_SENSORS_DISPLAY_ROTATION_MPU_TEST},
    {"OLED Canvas", OledCanvas_BeginFromMenu, NULL, Icon_Smartphone_bits, OledUtils_RenderOledCanvas_Wrapper, oledCanvasEventManager, NULL, SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferences, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU}
};

SubMenu submenuPreferencesDisplay = {
    .name = "Pantalla",
    .items = submenuPreferencesDisplayItems,
    .itemCount = sizeof(submenuPreferencesDisplayItems)/sizeof(submenuPreferencesDisplayItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferences,
    .icon = Icon_Smartphone_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_DISPLAY_MENU
};

MenuItem submenuPreferencesRotationItems[] = {
    {"Rot. automatica", Preferences_ToggleOledAutoRotation, NULL, Icon_ChoiceBulletOff_bits, NULL, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_ROTATION_AUTO, MenuIcon_OledAutoRotation},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferences, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU}
};

SubMenu submenuPreferencesRotation = {
    .name = "Rotacion",
    .items = submenuPreferencesRotationItems,
    .itemCount = sizeof(submenuPreferencesRotationItems)/sizeof(submenuPreferencesRotationItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferences,
    .icon = Icon_Smartphone_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_ROTATION_MENU
};

MenuItem submenuPreferencesIRItems[] = {
    {NULL, Preferences_ToggleIrObjects, NULL, Icon_ChoiceBulletOff_bits,
     NULL, menuEventManager, NULL,
     SCREEN_CODE_SETTINGS_PREFS_IR_OBJECTS,
     MenuIcon_IrObjects, MenuName_IrObjects, 0u},
    {NULL, Preferences_ToggleIrLine, NULL, Icon_ChoiceBulletOff_bits,
     NULL, menuEventManager, NULL,
     SCREEN_CODE_SETTINGS_PREFS_IR_LINE,
     MenuIcon_IrLine, MenuName_IrLine, 0u},
    {NULL, Preferences_ToggleIrLights, NULL, Icon_ChoiceBulletOff_bits,
     NULL, menuEventManager, NULL,
     SCREEN_CODE_SETTINGS_PREFS_IR_LIGHTS,
     MenuIcon_IrLights, MenuName_IrLights, 0u},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferencesSystem, Icon_Volver_bits,
     MenuSys_RenderMenu_Wrapper, menuEventManager, NULL,
     SCREEN_CODE_SETTINGS_PREFS_SYSTEM_MENU,
     NULL, NULL, 0u},
};

SubMenu submenuPreferencesIR = {
    .name = "Estado IR",
    .items = submenuPreferencesIRItems,
    .itemCount = sizeof(submenuPreferencesIRItems) / sizeof(submenuPreferencesIRItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferencesSystem,
    .icon = Icon_Sensors_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_IR_MENU
};

MenuItem submenuPreferencesSystemItems[] = {
    {"T.s accion", Preferences_BeginSystemActionTimeoutEdit, NULL, Icon_Config_bits, OledUtils_RenderSystemActionTimeout_Wrapper, systemActionTimeoutEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_SYSTEM_ACTION_TIMEOUT},
    {"Alive USB", Preferences_BeginUsbAlivePeriodEdit, NULL, Icon_USB_bits, OledUtils_RenderUsbAlivePeriod_Wrapper, usbAlivePeriodEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_SYSTEM_USB_ALIVE},
    {"Mutear ESP", Preferences_ToggleEspNotifWatchdog, NULL, Icon_ChoiceBulletOff_bits, NULL, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_SYSTEM_MENU, MenuIcon_EspNotifWatchdog},
    {"Limite Mutear", Preferences_BeginEspNotifWatchdogLimitEdit, NULL, Icon_Link_bits, OledUtils_RenderEspNotifWatchdogLimit_Full, espNotifWatchdogLimitEventManager, MenuItem_IsEspNotifWatchdogLimitVisible, SCREEN_CODE_SETTINGS_PREFS_SYSTEM_MENU},
    {"Estado IR", NULL, &submenuPreferencesIR, Icon_Sensors_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_IR_MENU, NULL, NULL, 0u},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferences, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU},
    {"AP fallback", Preferences_ToggleWifiApFallback, NULL, Icon_ChoiceBulletOn_bits, NULL, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_WIFI_AP_FALLBACK, MenuIcon_WifiApFallback}
};

SubMenu submenuPreferencesSystem = {
    .name = "Sistema",
    .items = submenuPreferencesSystemItems,
    .itemCount = sizeof(submenuPreferencesSystemItems)/sizeof(submenuPreferencesSystemItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferences,
    .icon = Icon_Config_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_SYSTEM_MENU
};

MenuItem submenuPreferencesSecurityItems[] = {
    {"Bloq pantalla", Preferences_ToggleScreenLock, NULL, Icon_ChoiceBulletOff_bits, NULL, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_DISPLAY_LOCK, MenuIcon_ScreenLock},
    {"T.s bloqueo", Preferences_BeginScreenLockTimeoutEdit, NULL, Icon_Smartphone_bits, OledUtils_RenderScreenLockTimeout_Wrapper, screenLockTimeoutEventManager, MenuItem_IsScreenLockTimeoutVisible, SCREEN_CODE_SETTINGS_PREFS_DISPLAY_TIMEOUT},
    {"Bloqueo PIN", Preferences_TogglePinLock, NULL, Icon_ChoiceBulletOn_bits, NULL, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_SECURITY_PIN_LOCK, MenuIcon_PinLock},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferences, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU}
};

SubMenu submenuPreferencesSecurity = {
    .name = "Seguridad",
    .items = submenuPreferencesSecurityItems,
    .itemCount = sizeof(submenuPreferencesSecurityItems)/sizeof(submenuPreferencesSecurityItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferences,
    .icon = Icon_Config_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_SECURITY_MENU
};

MenuItem submenuPreferencesUnerRouterItems[] = {
    {"Forwarding", Preferences_ToggleUnerRouterForwarding, NULL, Icon_ChoiceBulletOn_bits, NULL, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_UNER_ROUTER_FORWARDING, MenuIcon_UnerRouterForwarding},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferences, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU}
};

SubMenu submenuPreferencesUnerRouter = {
    .name = "UNER Router",
    .items = submenuPreferencesUnerRouterItems,
    .itemCount = sizeof(submenuPreferencesUnerRouterItems)/sizeof(submenuPreferencesUnerRouterItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferences,
    .icon = Icon_Link_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_UNER_ROUTER_MENU
};

MenuItem submenuPreferencesLineItems[] = {
    {"Ancho cinta", Preferences_BeginLineBlackWidthEdit, NULL, Icon_Sensors_bits, OledUtils_RenderLineBlackWidth_Wrapper, lineBlackWidthEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_LINE_WIDTH},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferences, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU}
};

SubMenu submenuPreferencesLine = {
    .name = "Linea",
    .items = submenuPreferencesLineItems,
    .itemCount = sizeof(submenuPreferencesLineItems)/sizeof(submenuPreferencesLineItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferences,
    .icon = Icon_Sensors_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_LINE_MENU
};

MenuItem submenuPreferencesMotorPwmItems[] = {
    {"Motor izq", Preferences_BeginMotorLeftMinPwmEdit, NULL, Icon_Tool_bits, OledUtils_RenderMotorMinPwm_Wrapper, motorMinPwmEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_LEFT},
    {"Motor der", Preferences_BeginMotorRightMinPwmEdit, NULL, Icon_Tool_bits, OledUtils_RenderMotorMinPwm_Wrapper, motorMinPwmEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_RIGHT},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferencesMotors, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MOTORS_MENU}
};

SubMenu submenuPreferencesMotorPwm = {
    .name = "PWM Minimo",
    .items = submenuPreferencesMotorPwmItems,
    .itemCount = sizeof(submenuPreferencesMotorPwmItems)/sizeof(submenuPreferencesMotorPwmItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferencesMotors,
    .icon = Icon_Tool_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_MENU
};

MenuItem submenuPreferencesMotorsItems[] = {
    {"PWM Minimo", NULL, &submenuPreferencesMotorPwm, Icon_Tool_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_MENU},
    {"Volver", MenuSys_GoBack_Wrapper, &submenuPreferences, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU}
};

SubMenu submenuPreferencesMotors = {
    .name = "Motores",
    .items = submenuPreferencesMotorsItems,
    .itemCount = sizeof(submenuPreferencesMotorsItems)/sizeof(submenuPreferencesMotorsItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenuPreferences,
    .icon = Icon_Tool_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_MOTORS_MENU
};

MenuItem submenuPreferencesItems[] = {
    {"Encoder", NULL, &submenuPreferencesEncoder, Icon_Encoder_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_ENCODER_MENU},
    {"Pantalla", NULL, &submenuPreferencesDisplay, Icon_Smartphone_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_DISPLAY_MENU},
    {"Seguridad", NULL, &submenuPreferencesSecurity, Icon_Lock_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_SECURITY_MENU},
    {"Sistema", NULL, &submenuPreferencesSystem, Icon_Config_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_SYSTEM_MENU},
    {"UNER Router", NULL, &submenuPreferencesUnerRouter, Icon_Router_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_UNER_ROUTER_MENU},
    {"Motores", NULL, &submenuPreferencesMotors, Icon_Motor_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MOTORS_MENU},
    {"Linea", NULL, &submenuPreferencesLine, Icon_Sensors_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_LINE_MENU},
    {"Volver", MenuSys_GoBack_Wrapper, &mainMenu, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_MENU}
};

SubMenu submenuPreferences = {
    .name = "Preferencias",
    .items = submenuPreferencesItems,
    .itemCount = sizeof(submenuPreferencesItems)/sizeof(submenuPreferencesItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu3,
    .icon = Icon_Prefs_bits,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_MENU
};

/* ---------- Submenu Config ---------- */

extern SubMenu submenu3;

MenuItem submenuSystemItems[] = {
    {"Reiniciar a BootV2", BootV2RestartConfirm_ActionBegin, NULL, Icon_Refrescar_bits, OledUtils_RenderGenericConfirm_Wrapper, GenericConfirmEventManager, NULL, SCREEN_CODE_SETTINGS_SYSTEM_BOOTLOADER_CONFIRM},
    {"Reiniciar a BootST", BootSTRestartConfirm_ActionBegin, NULL, Icon_Refrescar_bits, OledUtils_RenderGenericConfirm_Wrapper, GenericConfirmEventManager, NULL, SCREEN_CODE_SETTINGS_SYSTEM_BOOTLOADER_CONFIRM},
    {"Volver", MenuSys_GoBack_Wrapper, &submenu3, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_MENU}
};

SubMenu submenuSystem = {
    .name = "Sistema",
    .items = submenuSystemItems,
    .itemCount = sizeof(submenuSystemItems)/sizeof(submenuSystemItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu3,
    .icon = Icon_Config_bits,
    .screen_code = SCREEN_CODE_SETTINGS_SYSTEM_MENU
};

static void MenuAction_RebootExtNRF24(void)
{
    AppPreferences_SetExtProfile(EXT_PROFILE_NRF24);
    NVIC_SystemReset();
}

static void MenuAction_RebootExtBeeper(void)
{
    AppPreferences_SetExtProfile(EXT_PROFILE_BEEPER_PB12);
    NVIC_SystemReset();
}

MenuItem submenuExtensionsItems[] = {
    {"Modo NRF24", MenuAction_RebootExtNRF24, NULL, Icon_Puzzle_bits, NULL, NULL, NULL, SCREEN_CODE_NONE},
    {"Modo Beeper", MenuAction_RebootExtBeeper, NULL, Icon_Puzzle_bits, NULL, NULL, NULL, SCREEN_CODE_NONE},
    {"Volver", MenuSys_GoBack_Wrapper, &submenu3, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_MENU}
};

SubMenu submenuExtensions = {
    .name = "Extensiones",
    .items = submenuExtensionsItems,
    .itemCount = sizeof(submenuExtensionsItems)/sizeof(submenuExtensionsItems[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &submenu3,
    .icon = Icon_Puzzle_bits,
    .screen_code = SCREEN_CODE_NONE
};

MenuItem submenu3Items[] = {
    {"Preferencias", NULL, &submenuPreferences, Icon_Prefs_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_PREFS_MENU},
    {"Sistema", NULL, &submenuSystem, Icon_Config_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_SYSTEM_MENU},
    {"Extensiones", NULL, &submenuExtensions, Icon_Puzzle_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_NONE},
    {"Acerca de", NULL, &submenuAbout, Icon_Info_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_SETTINGS_ABOUT_MENU},
    {"Volver", MenuSys_GoBack_Wrapper, &mainMenu, Icon_Volver_bits, MenuSys_RenderMenu_Wrapper, menuEventManager, NULL, SCREEN_CODE_CORE_MAIN_MENU}
};

SubMenu submenu3 = {
    .name = "Configuracion",
    .items = submenu3Items,
    .itemCount = sizeof(submenu3Items)/sizeof(submenu3Items[0]),
    .currentItemIndex = 0,
    .firstVisibleItem = 0,
    .parent = &mainMenu,
    .icon = NULL,
    .screen_code = SCREEN_CODE_SETTINGS_PREFS_MENU
};

static bool MenuItem_IsUsbInstallerAvailable(void)
{
    const MotionControl_Status_t *status = MotionControl_GetStatus(&motionControl);

    if (motorTask.leftMotor.speedPercent > 0.0f ||
        motorTask.rightMotor.speedPercent > 0.0f) {
        return false;
    }
    if (status != NULL && (status->command_active || status->balance_active)) {
        return false;
    }
    return true;
}

static void MenuAction_SelectUsbCdc(void)
{
    USB_ModeManager_RequestCdc();
}

static void MenuAction_SelectUsbInstaller(void)
{
    if (MenuItem_IsUsbInstallerAvailable()) {
        USB_ModeManager_RequestInstaller();
    }
}

static const uint8_t *MenuIcon_UsbCdc(void)
{
    return (USB_ModeManager_GetActiveMode() == USB_MODE_CDC)
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const uint8_t *MenuIcon_UsbInstaller(void)
{
    return (USB_ModeManager_GetActiveMode() == USB_MODE_MSC_INSTALLER)
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

bool MenuItem_IsTestModeVisible(void) {
    return GET_CAR_MODE() == TEST_MODE;
}

bool MenuItem_IsScreenLockTimeoutVisible(void) {
    return AppPreferences_IsScreenLockEnabled();
}

static const uint8_t *MenuIcon_WifiApFallback(void)
{
    return AppPreferences_IsWifiApFallbackEnabled()
        ? Icon_ChoiceBulletOn_bits : Icon_ChoiceBulletOff_bits;
}

static void MenuAction_RetryWifi(void)
{
    APP_ESPWifiConnectRequested();
}

static bool MenuItem_IsWifiRetryVisible(void)
{
    return MenuItem_IsWifiWebStaVisible() &&
           !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE);
}

static const uint8_t *MenuIcon_EncoderInvert(void)
{
    return AppPreferences_IsEncoderInverted()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const uint8_t *MenuIcon_ScreenLock(void)
{
    return AppPreferences_IsScreenLockEnabled()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}



static const uint8_t *MenuIcon_OledAutoRotation(void)
{
    return AppPreferences_IsOledAutoRotationEnabled()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const uint8_t *MenuIcon_UnerRouterForwarding(void)
{
    return AppPreferences_IsUnerRouterForwardingEnabled()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const uint8_t *MenuIcon_PinLock(void)
{
    return AppPreferences_IsPinLockEnabled()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const uint8_t *MenuIcon_WifiModeToggle(void)
{
    return IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)
        ? Icon_Wifi_bits
        : Icon_APWifi_bits;
}

static const uint8_t *MenuIcon_WebModeToggle(void)
{
    return IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)
        ? Icon_Wifi_bits
        : NULL;
}

static const char *MenuName_WifiInfo(void)
{
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)) {
        return "Ver info de AP";
    }
    return "Ver info de red";
}

static const char *MenuName_WifiModeToggle(void)
{
    return IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)
        ? "Modo red"
        : "Modo AP";
}

static const char *MenuName_WebModeToggle(void)
{
    return IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)
        ? "Pasar a modo red"
        : "Pasar a modo AP";
}

static bool MenuItem_IsWebBackendVisible(void)
{
    return WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_WEB;
}

static bool MenuItem_IsAtModeToggleVisible(void)
{
    return WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_AT;
}

static bool MenuItem_IsAtWifiSearchVisible(void)
{
    return MenuItem_IsAtModeToggleVisible() &&
           !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
}
static const char *MenuName_AtTransport(void)
{
    return AppPreferences_GetWifiAtProtocol() == WIFI_BRIDGE_NETWORK_UDP
        ? "Transporte: UDP"
        : "Transporte: TCP";
}

static void MenuAction_ToggleAtTransport(void)
{
    const WifiBridgeNetworkProtocol next =
        AppPreferences_GetWifiAtProtocol() == WIFI_BRIDGE_NETWORK_UDP
            ? WIFI_BRIDGE_NETWORK_TCP
            : WIFI_BRIDGE_NETWORK_UDP;

    if (AppPreferences_SetWifiAtProtocol((uint16_t)next) == HAL_OK) {
        (void)WifiBridge_AT_SetNetworkProtocol(&wifiBridge, next);
        menuSystem.suppressClearOnNextRender = true;
        menuSystem.renderFlag = true;
        autoMicroSystem.runtime.oled_first_draw = false;
    }
}


static void MenuAction_ToggleStabilization(void)
{
    if (MotionControl_IsArmed(&motionControl)) {
        (void)APP_BalanceSetEnabled(false);
    } else {
        (void)APP_BalanceSetEnabled(true);
    }
    menuSystem.suppressClearOnNextRender = true;
    menuSystem.renderFlag = true;
    autoMicroSystem.runtime.oled_first_draw = false;
}

static const uint8_t *MenuIcon_Stabilization(void)
{
    return MotionControl_IsArmed(&motionControl)
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const char *MenuName_Stabilization(void)
{
    return MotionControl_IsArmed(&motionControl)
        ? "PID: Activado"
        : "PID: Desactivado";
}

static void MenuAction_WifiConnectionToggle(void)
{
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)) {
        if (WifiBridge_IsConnected(&wifiBridge)) {
            WifiBridge_StopAP(&wifiBridge);
        } else {
            WifiBridge_StartAP(&wifiBridge);
        }
    } else {
        if (WifiBridge_IsConnected(&wifiBridge)) {
            WifiBridge_Disconnect(&wifiBridge);
        } else {
            WifiBridge_Connect(&wifiBridge);
        }
    }
}

static const uint8_t *MenuIcon_WifiConnectionToggle(void)
{
    return WifiBridge_IsConnected(&wifiBridge)
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static const char *MenuName_WifiConnectionToggle(void)
{
    static char buf[64];
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE)) {
        if (WifiBridge_IsConnected(&wifiBridge)) {
            return "Red AP: Activa";
        } else {
            return "Red AP: Desactivada";
        }
    } else {
        if (WifiBridge_IsConnected(&wifiBridge)) {
            snprintf(buf, sizeof(buf), "Conectado a %s", wifiBridge.ssid[0] != '\0' ? wifiBridge.ssid : "Red");
            return buf;
        } else {
            return "Conectar red";
        }
    }
}

static bool MenuItem_IsWifiWebStaVisible(void)
{
    return MenuItem_IsWebBackendVisible() && !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE);
}

static bool MenuItem_IsWifiWebApVisible(void)
{
    const WifiBridge_Backend backend = WifiBridge_GetActiveBackend(&wifiBridge);
    return backend == WIFI_BRIDGE_BACKEND_WEB || backend == WIFI_BRIDGE_BACKEND_AT;
}

static void MenuAction_ToggleWifiApPasswordProtected(void)
{
    AppApConfig_t current;
    AppApConfigPatch_t patch = {0};

    AppApConfig_Get(&current);
    if (!current.protected && strlen(current.password) < APP_AP_PASSWORD_MIN_BYTES) {
        OledUtils_WifiApEnableProtection_Wrapper();
        return;
    }
    patch.fieldMask = APP_AP_FIELD_PROTECTED;
    patch.values.protected = !current.protected;
    if (AppApConfig_ApplyPatch(&patch, NULL) == APP_AP_STATUS_OK) {
        OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_AP_CONFIG_SAVED,
                                           OLED_NOTIFICATION_DEFAULT_MS);
    } else {
        OledUtils_ShowSimpleNotificationMs(OLED_SIMPLE_NOTIFICATION_AP_CONFIG_INVALID,
                                           OLED_NOTIFICATION_DEFAULT_MS);
    }
}

static const uint8_t *MenuIcon_WifiApPasswordProtected(void)
{
    return AppPreferences_IsWifiApPasswordProtected()
        ? Icon_ChoiceBulletOn_bits
        : Icon_ChoiceBulletOff_bits;
}

static bool MenuItem_IsWifiApPasswordProtectedVisible(void)
{
    return AppPreferences_IsWifiApPasswordProtected();
}


const MenuItem* MenuSys_GetItemByScreenCode(ScreenCode_t code) {
    if (code == SCREEN_CODE_NONE) return NULL;
    const MenuItem* arrays[] = {
        submenuESPItems, submenuUSBItems, mainMenuItems, submenuConfigApItems,
        submenu1Items, submenuSensorsIrItems, submenuLineCalibrationItems, submenuSensorsMpuItems,
        submenuSensorsDisplayItems, submenuUnerRouterItems, submenuTestKeyboardItems,
        submenu2Items, submenuMotorsItems, submenuBalanceItems, submenuAboutItems,
        submenuPreferencesEncoderItems, submenuPreferencesDisplayItems,
        submenuPreferencesRotationItems, submenuPreferencesSystemItems,
        submenuPreferencesSecurityItems, submenuPreferencesUnerRouterItems,
        submenuPreferencesLineItems, submenuPreferencesMotorPwmItems,
        submenuPreferencesMotorsItems, submenuPreferencesItems, submenuSystemItems,
        submenuExtensionsItems, submenu3Items,
        submenuPreferencesIRItems
    };
    const uint8_t sizes[] = {
        sizeof(submenuESPItems)/sizeof(MenuItem), sizeof(submenuUSBItems)/sizeof(MenuItem),
        sizeof(mainMenuItems)/sizeof(MenuItem), sizeof(submenuConfigApItems)/sizeof(MenuItem),
        sizeof(submenu1Items)/sizeof(MenuItem), sizeof(submenuSensorsIrItems)/sizeof(MenuItem), sizeof(submenuLineCalibrationItems)/sizeof(MenuItem),
        sizeof(submenuSensorsMpuItems)/sizeof(MenuItem), sizeof(submenuSensorsDisplayItems)/sizeof(MenuItem),
        sizeof(submenuUnerRouterItems)/sizeof(MenuItem), sizeof(submenuTestKeyboardItems)/sizeof(MenuItem),
        sizeof(submenu2Items)/sizeof(MenuItem), sizeof(submenuMotorsItems)/sizeof(MenuItem),
        sizeof(submenuBalanceItems)/sizeof(MenuItem), sizeof(submenuAboutItems)/sizeof(MenuItem),
        sizeof(submenuPreferencesEncoderItems)/sizeof(MenuItem), sizeof(submenuPreferencesDisplayItems)/sizeof(MenuItem),
        sizeof(submenuPreferencesRotationItems)/sizeof(MenuItem), sizeof(submenuPreferencesSystemItems)/sizeof(MenuItem),
        sizeof(submenuPreferencesSecurityItems)/sizeof(MenuItem), sizeof(submenuPreferencesUnerRouterItems)/sizeof(MenuItem),
        sizeof(submenuPreferencesLineItems)/sizeof(MenuItem), sizeof(submenuPreferencesMotorPwmItems)/sizeof(MenuItem),
        sizeof(submenuPreferencesMotorsItems)/sizeof(MenuItem), sizeof(submenuPreferencesItems)/sizeof(MenuItem),
        sizeof(submenuSystemItems)/sizeof(MenuItem), sizeof(submenuExtensionsItems)/sizeof(MenuItem),
        sizeof(submenu3Items)/sizeof(MenuItem),
        sizeof(submenuPreferencesIRItems)/sizeof(MenuItem)
    };
    
    for (size_t a = 0; a < sizeof(arrays)/sizeof(arrays[0]); a++) {
        for (size_t i = 0; i < sizes[a]; i++) {
            if (arrays[a][i].screen_code == code) {
                return &arrays[a][i];
            }
        }
    }
    return NULL;
}
