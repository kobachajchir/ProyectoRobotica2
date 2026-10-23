#include "screens_internal.h"
/*
 * screenWrappers.c
 *
 *  Created on: Jul 23, 2025
 *      Author: kobac
 */

#include "screenWrappers.h"
#include "types/menu_types.h"
#include "oled_utils.h"
#include "oled_canvas.h"
#include "fonts.h"
#include "encoder_types.h"
#include "uner_v2.h"
#include <string.h>















#define DASHBOARD_WEB_STATUS_REQUEST_PERIOD_MS        2000u

static void Dashboard_RequestWebStatusIfNeeded(void)
{
    const uint32_t now = Screens_NowMs();

    if (!WifiBridge_IsConnected(&wifiBridge) ||
        WifiBridge_GetActiveBackend(&wifiBridge) != WIFI_BRIDGE_BACKEND_WEB ||
        WifiBridge_IsWaitingValidation(&wifiBridge)) {
        return;
    }

    if (dashboardWebStatusRequestedAt != 0u &&
        ((uint32_t)(now - dashboardWebStatusRequestedAt) <
         DASHBOARD_WEB_STATUS_REQUEST_PERIOD_MS)) {
        return;
    }

    if (WifiBridge_GetStatus(&wifiBridge) == WIFI_BRIDGE_OK) {
        dashboardWebStatusRequestedAt = now;
    }
}

static void WiFiResults_ResetMenuState(void)
{
    memset(wifiResultsItems, 0, (WIFI_RESULTS_MENU_MAX_ITEMS * sizeof(wifiResultsItems[0])));
    wifiResultsMenu.parent = &submenu1;
    wifiResultsMenu.itemCount = 0u;
    wifiResultsMenu.currentItemIndex = 0;
    wifiResultsMenu.firstVisibleItem = 0;
    wifiResultsMenu.lastSelectedItemIndex = -1;
    wifiResultsMenu.lastVisibleItem = -1;
}

void WiFiScan_ClearResults(void)
{
    autoMicroSystem.connectivity.networksFound = 0u;
    autoMicroSystem.connectivity.wifiSelectedNetworkIndex = WIFI_SELECTED_NETWORK_INVALID;
    memset(autoMicroSystem.connectivity.wifiNetworkSsids, 0, sizeof(autoMicroSystem.connectivity.wifiNetworkSsids));
    memset(autoMicroSystem.connectivity.wifiNetworkEncryptions, 0, sizeof(autoMicroSystem.connectivity.wifiNetworkEncryptions));
    memset(autoMicroSystem.connectivity.wifiNetworkSignalStrengths, 0, sizeof(autoMicroSystem.connectivity.wifiNetworkSignalStrengths));
    memset(autoMicroSystem.connectivity.wifiNetworkChannels, 0, sizeof(autoMicroSystem.connectivity.wifiNetworkChannels));
    memset(autoMicroSystem.connectivity.wifiNetworkEncryptionTypes, 0, sizeof(autoMicroSystem.connectivity.wifiNetworkEncryptionTypes));
    memset(autoMicroSystem.connectivity.wifiNetworkDetailValid, 0, sizeof(autoMicroSystem.connectivity.wifiNetworkDetailValid));
    WiFiResults_ResetMenuState();
}

static void WiFiResults_SetDefaultSelection(SubMenu *menu)
{
    if (!menu || menu->itemCount == 0u) {
        return;
    }

    if (menu->currentItemIndex < 0 || menu->currentItemIndex >= menu->itemCount) {
        menu->currentItemIndex = 0;
    }
    if (menu->firstVisibleItem < 0 || menu->firstVisibleItem >= menu->itemCount) {
        menu->firstVisibleItem = 0;
    }
    if (menu->currentItemIndex < menu->firstVisibleItem) {
        menu->firstVisibleItem = menu->currentItemIndex;
    }
    if ((menu->currentItemIndex - menu->firstVisibleItem) >= MENU_VISIBLE_ITEMS) {
        menu->firstVisibleItem = (int8_t)(menu->currentItemIndex - (MENU_VISIBLE_ITEMS - 1));
    }
    if (menu->lastSelectedItemIndex >= menu->itemCount) {
        menu->lastSelectedItemIndex = -1;
    }
    if (menu->lastVisibleItem >= menu->itemCount) {
        menu->lastVisibleItem = -1;
    }
}

static void WiFiResults_BuildMenu(void)
{
    uint8_t itemIndex = 0u;

    memset(wifiResultsItems, 0, (WIFI_RESULTS_MENU_MAX_ITEMS * sizeof(wifiResultsItems[0])));
    wifiResultsMenu.parent = &submenu1;

    if (autoMicroSystem.connectivity.networksFound == 0u) {
        wifiResultsItems[itemIndex].name = "Sin redes";
        wifiResultsItems[itemIndex].icon = Icon_Wifi_bits;
        wifiResultsItems[itemIndex].screen_code = SCREEN_CODE_CONNECTIVITY_WIFI_RESULTS;
        itemIndex++;
    } else {
        for (uint8_t i = 0u; i < autoMicroSystem.connectivity.networksFound && i < WIFI_SCAN_MAX_NETWORKS; ++i) {
            wifiResultsItems[itemIndex].name = autoMicroSystem.connectivity.wifiNetworkSsids[i];
            wifiResultsItems[itemIndex].icon = Icon_Wifi_bits;
            wifiResultsItems[itemIndex].screen_code = SCREEN_CODE_CONNECTIVITY_WIFI_DETAILS;
            itemIndex++;
        }
    }

    wifiResultsItems[itemIndex].name = "Actualizar";
    wifiResultsItems[itemIndex].icon = Icon_Refrescar_bits;
    wifiResultsItems[itemIndex].screen_code = SCREEN_CODE_CONNECTIVITY_WIFI_SEARCHING;
    itemIndex++;

    wifiResultsItems[itemIndex].name = "Volver";
    wifiResultsItems[itemIndex].icon = Icon_Volver_bits;
    wifiResultsItems[itemIndex].screen_code = SCREEN_CODE_CONNECTIVITY_WIFI_MENU;
    itemIndex++;

    wifiResultsMenu.itemCount = itemIndex;
    WiFiResults_SetDefaultSelection(&wifiResultsMenu);
}

static uint8_t MenuSys_VisibleOffset(const SubMenu *menu, int8_t firstVisible, int8_t itemIndex) {
    uint8_t offset = 0;
    if (!menu) return 0;
    if (itemIndex < firstVisible) return 0;
    for (int8_t i = firstVisible; i < itemIndex; i++) {
        if (MenuSys_IsItemVisible(&menu->items[i])) {
            offset++;
        }
    }
    return offset;
}

void OledUtils_DrawItem_Wrapper(const MenuItem *item,
                                int               y,
                                bool              selected)
{
    // llamamos a la versión "completa" pasándole el handle
    OledUtils_DrawItem(item, y, selected);
}

void Dashboard_InvalidateConnectivity(void)
{
    dashboardConnectivityDirty = true;

    if (menuSystem.renderFn == OledUtils_RenderDashboard_Wrapper ||
        menuSystem.renderFn == OledUtils_RenderWiFiStatus) {
        autoMicroSystem.runtime.oled_first_draw = true;
        menuSystem.renderFlag = true;
    }
}

void Dashboard_ReturnAfterConnectivityNotification(void)
{
    if (!dashboardConnectivityDirty) {
        return;
    }
    dashboardConnectivityDirty = 0u;

    if (WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_WEB &&
        !AppPreferences_IsWifiApFallbackEnabled() &&
        !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, WIFI_ACTIVE) &&
        !IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, AP_ACTIVE) &&
        (!autoMicroSystem.runtime.inside_menu_flag || menuSystem.currentMenu == &submenu1 ||
         menuSystem.renderFn == OledUtils_RenderWiFiStatus)) {
        menuSystem.currentMenu = &submenu1;
        menuSystem.renderFn = MenuSys_RenderMenu_Wrapper;
        menuSystem.userEventManagerFn = menuEventManager;
        menuSystem.allowPeriodicRefresh = false;
        autoMicroSystem.runtime.inside_menu_flag = true;
        if (menuSystem.insideMenuFlag) *menuSystem.insideMenuFlag = 1u;
        autoMicroSystem.runtime.oled_first_draw = true;
        menuSystem.renderFlag = true;
        MenuSys_SetCurrentScreenCode(&menuSystem, SCREEN_CODE_CONNECTIVITY_WIFI_MENU,
                                     SCREEN_REPORT_SOURCE_RENDER);
        return;
    }

    if (!autoMicroSystem.runtime.inside_menu_flag) {
        menuSystem.currentMenu = &mainMenu;
        menuSystem.renderFn = OledUtils_RenderDashboard_Wrapper;
        menuSystem.userEventManagerFn = dashboardEventManager;
        menuSystem.allowPeriodicRefresh = false;
        if (menuSystem.insideMenuFlag) {
            *menuSystem.insideMenuFlag = 0u;
        }
        autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
        encoder.allowEncoderInput = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, OLED_ACTIVE);

        autoMicroSystem.runtime.oled_first_draw = true;
        menuSystem.renderFlag = true;
        MenuSys_SetCurrentScreenCode(&menuSystem, SCREEN_CODE_CORE_DASHBOARD, SCREEN_REPORT_SOURCE_RENDER);
    }
}

/**
 * @brief  Wrapper para la pantalla principal (dashboard).
 */
void OledUtils_RenderDashboard_Wrapper(void)
{
    (void)0; // BREAKPOINT: wrapper dashboard
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CORE_DASHBOARD,
                                 SCREEN_REPORT_SOURCE_RENDER);
    // 1) Deshabilito el refresco periódico
    OledUtils_DisableContinuousRender();
    menuSystem.userEventManagerFn = dashboardEventManager;
    Dashboard_ResetModeConfirmState();
    (void)0;
    autoMicroSystem.runtime.inside_menu_flag = false;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;

    // 2) Marco que esta es la primera pasada para full-refresh
    autoMicroSystem.runtime.oled_first_draw = true;

    // 3) Bloqueo / desbloqueo encoder según el flag
    encoder.allowEncoderInput = IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, OLED_ACTIVE);

    // 4) Llamo a la función que pinta TODO en el buffer
    OledUtils_Clear();
    Dashboard_RequestWebStatusIfNeeded();
    OledUtils_RenderDashboard();
    dashboardModeReportPending = true;
    dashboardConnectivityDirty = false;

    // ¡no vuelvo a tocar menuSystem.renderFlag!
}

/**
 * @brief  Wrapper que decide si redibuja todo o sólo mueve cursor.
 */
void OledUtils_RenderStabilizationPrompt_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CORE_STABILIZE_PROMPT,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    menuSystem.userEventManagerFn = dashboardQuickActionEventManager;
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    OledUtils_Clear();
    OledUtils_RenderStabilizationPrompt();
    autoMicroSystem.runtime.oled_first_draw = true;
}

void OledUtils_RenderModePrompt_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CORE_MODE_PROMPT,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    menuSystem.userEventManagerFn = dashboardQuickActionEventManager;
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    OledUtils_Clear();
    OledUtils_RenderModePrompt();
    autoMicroSystem.runtime.oled_first_draw = true;
}

void MenuSys_RenderMenu_Wrapper(void) {
    (void)0; // BREAKPOINT: wrapper menú
    OledUtils_DisableContinuousRender();
    menuSystem.userEventManagerFn = menuEventManager;
    autoMicroSystem.runtime.inside_menu_flag = true;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;

    SubMenu *m = menuSystem.currentMenu;
    if (!m) return;

    // Configuración general
    encoder.allowEncoderInput = true;

    bool firstDraw  = autoMicroSystem.runtime.oled_first_draw;
    bool pageChanged = (m->firstVisibleItem != m->lastVisibleItem);
    bool invalidSelection = (m->lastSelectedItemIndex < 0 || m->lastSelectedItemIndex >= m->itemCount ||
                             m->currentItemIndex < 0 || m->currentItemIndex >= m->itemCount ||
                             !MenuSys_IsItemVisible(&m->items[m->currentItemIndex]) ||
                             !MenuSys_IsItemVisible(&m->items[m->lastSelectedItemIndex]));

    if (firstDraw || pageChanged || invalidSelection) {
        // 👉 full render: limpia + dibuja TODOS los ítems visibles
        MenuSys_RenderMenu(&menuSystem);

        if (MenuSys_IsFavoritesMenu(m)) {
            // Left: [User Btn] [Volver]
            // Right: [Encoder] OK
            ssd1306_SetColor(White);
            ssd1306_DrawBitmap(0, 48, 16, 16, Icon_UserBtn_bits);
            ssd1306_DrawBitmap(18, 48, 16, 16, Icon_Volver_bits);

            ssd1306_DrawBitmap(94, 48, 16, 16, Icon_Encoder_bits);
            ssd1306_SetCursor(112, 51);
            ssd1306_WriteString("OK", Font_7x10);
        }

        autoMicroSystem.runtime.oled_first_draw = false;
    }
    else {
        // 👉 incremental: sólo borrar cursor antiguo y pintar el nuevo
        const uint8_t Y0 = MENU_ITEM_Y0;
        const uint8_t SP = MENU_ITEM_SPACING;

        uint8_t oldVisIdx = MenuSys_VisibleOffset(m, m->firstVisibleItem, m->lastSelectedItemIndex);
        uint8_t newVisIdx = MenuSys_VisibleOffset(m, m->firstVisibleItem, m->currentItemIndex);
        uint8_t oldY      = Y0 + oldVisIdx * SP;
        uint8_t newY      = Y0 + newVisIdx * SP;

        // Borrar cursor anterior
        OledUtils_DrawItem(&m->items[m->lastSelectedItemIndex], oldY, false);

        // Pintar cursor nuevo
        OledUtils_DrawItem(&m->items[m->currentItemIndex], newY, true);

        // Actualizo sólo la selección
        m->lastSelectedItemIndex = m->currentItemIndex;
    }
}

void MenuSys_GoBack_Wrapper(){
    MenuSys_NavigateBack(&menuSystem);
}

void OledUtils_RenderMotorTest_Wrapper(void) {
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_MOTOR_TEST,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    menuSystem.userEventManagerFn = motorTestEventManager;
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    SET_FLAG(motorTask.motorData.flags, ENABLE_MOVEMENT);
    if(!autoMicroSystem.runtime.oled_first_draw){
        OledUtils_Clear();
        OledUtils_MotorTest_Complete();
        autoMicroSystem.runtime.oled_first_draw = true;
    }else{
        (void)0;
        OledUtils_MotorTest_Changes();
    }
}

void OledUtils_ToggleValoresIRView(void)
{
    irValuesRawView = !irValuesRawView;
    autoMicroSystem.runtime.oled_first_draw = false;
    menuSystem.renderFlag = true;
}

void OledUtils_ToggleTestCaminoView(void)
{
    routeAlignmentDetailView = !routeAlignmentDetailView;
    autoMicroSystem.runtime.oled_first_draw = false;
    menuSystem.renderFlag = true;
}

void OledUtils_ToggleValoresMPUView(void)
{
    mpuValuesViewIndex = (mpuValuesViewIndex + 1) % 3;
    autoMicroSystem.runtime.oled_first_draw = false;
    menuSystem.renderFlag = true;
}

static void OledUtils_BeginUnerRouterTransport(uint8_t transport_id)
{
    unerRouterTransportId = transport_id;
    unerRouterTxView = false;
    autoMicroSystem.runtime.oled_first_draw = false;
}

void OledUtils_BeginUnerRouterUsb(void)
{
    OledUtils_BeginUnerRouterTransport(UNER_TR_USB_CDC);
}

void OledUtils_BeginUnerRouterUart(void)
{
    OledUtils_BeginUnerRouterTransport(UNER_TR_UART1_ESP);
}

void OledUtils_BeginUnerRouterNrf(void)
{
    OledUtils_BeginUnerRouterTransport(UNER_TR_NRF24_SPI2);
}

void OledUtils_NextUnerRouterPacketView(void)
{
    unerRouterTxView = !unerRouterTxView;
    autoMicroSystem.runtime.oled_first_draw = false;
    menuSystem.renderFlag = true;
}

void OledUtils_PreviousUnerRouterPacketView(void)
{
    OledUtils_NextUnerRouterPacketView();
}

bool OledUtils_IsUnerRouterTxView(void)
{
    return unerRouterTxView;
}

uint8_t OledUtils_GetUnerRouterTransportId(void)
{
    return unerRouterTransportId;
}

static ScreenCode_t OledUtils_GetUnerRouterScreenCode(void)
{
    switch (unerRouterTransportId) {
        case UNER_TR_UART1_ESP:
            return SCREEN_CODE_SERVICE_UNER_ROUTER_UART;
        case UNER_TR_NRF24_SPI2:
            return SCREEN_CODE_SERVICE_UNER_ROUTER_NRF;
        case UNER_TR_USB_CDC:
        default:
            return SCREEN_CODE_SERVICE_UNER_ROUTER_USB;
    }
}

void OledUtils_About_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN)
                                     ? SCREEN_CODE_SETTINGS_ABOUT_REPO
                                     : SCREEN_CODE_SETTINGS_ABOUT_PROJECT,
                                 SCREEN_REPORT_SOURCE_RENDER);
    // Estado base de esta pantalla
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;

    // Seleccionar cuál pantalla real renderizar según flag
    if (IS_FLAG_SET(autoMicroSystem.runtime.systemFlags2, SHOWSECONDSCREEN)) {
        OledUtils_Clear();
        OledUtils_RenderProyectInfoScreen();
        (void)0;
    } else {
        OledUtils_Clear();
        OledUtils_RenderProyectScreen();
        (void)0;
    }
    (void)0;
    // Activar el manager de eventos para esta pantalla
    menuSystem.userEventManagerFn = About_UserEventManager;
}

void OledUtils_AboutInfo_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SETTINGS_ABOUT_PROJECT,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    OledUtils_Clear();
    OledUtils_RenderProyectScreen();
    menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
}

void OledUtils_AboutRepo_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SETTINGS_ABOUT_REPO,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    OledUtils_Clear();
    OledUtils_RenderProyectInfoScreen();
    menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
}

void OledUtils_AboutBuild_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SETTINGS_ABOUT_BUILD,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    OledUtils_Clear();
    OledUtils_RenderBuildInfoScreen();
    menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
}

void OledUtils_AboutBootloader_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SETTINGS_ABOUT_BOOTLOADER,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    OledUtils_Clear();
    OledUtils_RenderAboutBootloaderScreen();
    menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
}

void OledUtils_RenderGenericConfirm_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 GenericConfirm_GetScreenCode(),
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    OledUtils_Clear();
    OledUtils_RenderGenericConfirm();
    // Event manager is mapped in menu_definition.c
}

/**
 * @brief  Wrapper para la pantalla de Valores IR.
 */
static void OledUtils_BeginIRValuesSession(bool ambientMode)
{
    if (APP_LineCalibrationBusy()) return;
    const int8_t requestedMode = ambientMode ? 1 : 0;

    if (irValuesSessionMode == requestedMode) {
        return;
    }

    if (irValuesSessionMode < 0) {
        irValuesSavedGroup5 = APP_IsIrObjectsEnabled();
        irValuesSavedGroup3 = APP_IsIrLineEnabled();
    }

    irValuesSessionMode = requestedMode;
    irValuesAmbientMode = ambientMode;
    if (ambientMode) {
        TCRT5000_EmittersGroup5Off(&irSensors);
        TCRT5000_EmittersGroup3Off(&irSensors);
    } else {
        TCRT5000_EmittersGroup5On(&irSensors);
        TCRT5000_EmittersGroup3On(&irSensors);
    }
    autoMicroSystem.runtime.oled_first_draw = false;
}

void OledUtils_EndIRValuesSession(void)
{
    if (APP_LineCalibrationBusy()) return;
    if (irValuesSessionMode < 0) {
        return;
    }

    TCRT5000_EmittersGroup5Set(&irSensors, irValuesSavedGroup5);
    TCRT5000_EmittersGroup3Set(&irSensors, irValuesSavedGroup3);
    irValuesSessionMode = -1;
    OledUtils_DisableContinuousRender();
}
static void OledUtils_RenderIRValuesMode(bool ambientMode) {
    const ScreenCode_t screenCode = ambientMode
        ? SCREEN_CODE_SENSORS_IR_AMBIENT
        : SCREEN_CODE_SENSORS_IR_VALUES;

    OledUtils_BeginIRValuesSession(ambientMode);
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 screenCode,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_EnableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;  // Cambiar a true para poder salir
    menuSystem.userEventManagerFn = IRValues_UserEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        if (irValuesRawView) {
            OledUtils_RenderIRRawScene(!irValuesAmbientMode);
            OledUtils_UpdateIRRawValues(autoMicroSystem.runtime.sensor_raw_data);
        } else {
            OledUtils_RenderIRGraphScene(!irValuesAmbientMode);
            OledUtils_UpdateIRBars(autoMicroSystem.runtime.sensor_raw_data);
        }
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        if (irValuesRawView) {
            OledUtils_UpdateIRRawValues(autoMicroSystem.runtime.sensor_raw_data);
        } else {
            OledUtils_UpdateIRBars(autoMicroSystem.runtime.sensor_raw_data);
        }
    }
}

void OledUtils_RenderValoresIR_Wrapper(void)
{
    if (APP_LineCalibrationBusy()) return;
    OledUtils_RenderIRValuesMode(false);
}

void OledUtils_RenderIRAmbiente_Wrapper(void)
{
    if (APP_LineCalibrationBusy()) return;
    OledUtils_RenderIRValuesMode(true);
}

/**
 * @brief  Wrapper para la pantalla Test camino.
 */
void OledUtils_RenderTestCamino_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SENSORS_ROUTE_ALIGNMENT,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_EnableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = RouteAlignment_UserEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        if (routeAlignmentDetailView) {
            OledUtils_RenderRouteAlignmentScene();
            OledUtils_UpdateRouteAlignment(autoMicroSystem.runtime.sensor_raw_data);
        } else {
            OledUtils_RenderRouteAlignmentVisualScene();
            OledUtils_UpdateRouteAlignmentVisual(autoMicroSystem.runtime.sensor_raw_data);
        }
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        if (routeAlignmentDetailView) {
            OledUtils_UpdateRouteAlignment(autoMicroSystem.runtime.sensor_raw_data);
        } else {
            OledUtils_UpdateRouteAlignmentVisual(autoMicroSystem.runtime.sensor_raw_data);
        }
    }
}

/**
 * @brief  Wrapper para la pantalla de Valores MPU.
 */
void OledUtils_RenderValoresMPU_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SENSORS_MPU_VALUES,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_EnableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;  // Cambiar a true
    menuSystem.userEventManagerFn = MPUValues_UserEventManager;
    menuSystem.renderFlag = true;

    if(!autoMicroSystem.runtime.oled_first_draw){
        OledUtils_Clear();
        if (mpuValuesViewIndex == 0) {
            OledUtils_RenderMPUScene();
            OledUtils_UpdateMPUValues(&mpuTask);
        } else if (mpuValuesViewIndex == 1) {
            OledUtils_RenderMPUOrientationScene();
            OledUtils_UpdateMPUOrientation(&mpuTask);
        } else {
            OledUtils_RenderMPUCompassScene();
            OledUtils_UpdateMPUCompass(&mpuTask);
        }
        autoMicroSystem.runtime.oled_first_draw = true;
    }else{
        if (mpuValuesViewIndex == 0) {
            OledUtils_UpdateMPUValues(&mpuTask);
        } else if (mpuValuesViewIndex == 1) {
            OledUtils_UpdateMPUOrientation(&mpuTask);
        } else {
            OledUtils_UpdateMPUCompass(&mpuTask);
        }
    }
}

void OledUtils_RenderMPUCalibration_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SENSORS_MPU_CALIBRATION,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_EnableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = mpuCalibrationEventManager;
    menuSystem.renderFlag = true;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderMPUCalibration_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderMPUCalibration_ValueOnly();
    }
}

void OledUtils_RenderScreenLockTimeout_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SETTINGS_PREFS_DISPLAY_TIMEOUT,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = screenLockTimeoutEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderScreenLockTimeout_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderScreenLockTimeout_ValueOnly();
    }
}

void OledUtils_RenderSystemActionTimeout_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SETTINGS_PREFS_SYSTEM_ACTION_TIMEOUT,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = systemActionTimeoutEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderSystemActionTimeout_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderSystemActionTimeout_ValueOnly();
    }
}

void OledUtils_RenderUsbAlivePeriod_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SETTINGS_PREFS_SYSTEM_USB_ALIVE,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = usbAlivePeriodEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderUsbAlivePeriod_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderUsbAlivePeriod_ValueOnly();
    }
}

void OledUtils_RenderLineBlackWidth_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SETTINGS_PREFS_LINE_WIDTH,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = lineBlackWidthEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderLineBlackWidth_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderLineBlackWidth_ValueOnly();
    }
}

void OledUtils_RenderOledRotationTest_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SENSORS_DISPLAY_ROTATION_TEST,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = oledRotationTestEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderOledRotationTest_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderOledRotationTest_ValueOnly();
    }
}

void OledUtils_RenderOledRotationMpuTest_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SENSORS_DISPLAY_ROTATION_MPU_TEST,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_EnableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = oledRotationMpuTestEventManager;
    menuSystem.renderFlag = true;

    Preferences_UpdateOledRotationFromMpuTest();
    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderOledRotationMpuTest_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderOledRotationMpuTest_ValueOnly();
    }
}

void OledUtils_RenderOledCanvas_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = oledCanvasEventManager;
    OledCanvas_RenderReadyScreen();
}

void OledUtils_RenderMotorMinPwm_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 Preferences_GetMotorMinPwmEditScreenCode(),
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = motorMinPwmEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderMotorMinPwm_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderMotorMinPwm_ValueOnly();
    }
}

void OledUtils_RenderSingleMotorTest_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 MotorSingleTest_GetScreenCode(),
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = motorSingleTestEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderSingleMotorTest_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderSingleMotorTest_ValueOnly();
    }
}

void OledUtils_RenderBalancePidStatus_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_BALANCE_PID_STATUS,
                                 SCREEN_REPORT_SOURCE_RENDER);
    ssd1306_Clear();
    OledUtils_EnableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = balancePidStatusEventManager;
    menuSystem.renderFlag = true;
    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        autoMicroSystem.runtime.oled_first_draw = true;
    }
    OledUtils_RenderBalancePidStatus();
}

void OledUtils_RenderBalancePidLog_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_BALANCE_PID_LOG,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = balancePidLogEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderBalancePidLog_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderBalancePidLog_ValueOnly();
    }
}

void OledUtils_RenderBalancePidGain_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 BalancePid_GetCurrentGainScreenCode(),
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 1u;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = balancePidGainEventManager;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderBalancePidGain_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderBalancePidGain_ValueOnly();
    }
}

void OledUtils_RenderUnerRouterTest_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 OledUtils_GetUnerRouterScreenCode(),
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_EnableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    autoMicroSystem.runtime.encoder_fast_scroll_enabled = 0u;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = UnerRouterTest_UserEventManager;
    menuSystem.renderFlag = true;

    if (!autoMicroSystem.runtime.oled_first_draw) {
        OledUtils_Clear();
        OledUtils_RenderUnerRouterTest_Full();
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_RenderUnerRouterTest_ValueOnly();
    }
}

void OledUtils_RenderScreenLocked_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_WARNING_LOCKED,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = screenLockedEventManager;
    OledUtils_Clear();
    OledUtils_RenderScreenLocked();
    autoMicroSystem.runtime.oled_first_draw = true;
}

/**
 * @brief Wrapper para la pantalla de búsqueda WiFi
 */
void OledUtils_RenderWiFiSearching_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CONNECTIVITY_WIFI_SEARCHING,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = WiFiSearch_UserEventManager;  // AGREGADO
    SET_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);

    if (!autoMicroSystem.runtime.oled_first_draw) {
        autoMicroSystem.connectivity.wifiSearchingTimeout = WIFIDEFAULTSEARCHTIMEOUT;
        WiFiScan_ClearResults();
        autoMicroSystem.connectivity.wifiScanSessionActive = 1u;
        autoMicroSystem.connectivity.wifiScanResultsPending = 0u;
        (void)0;
        (void)WifiBridge_StartScan(&wifiBridge);
        OledUtils_Clear();
        OledUtils_RenderWiFiSearchScene();
        OledUtils_UpdateWiFiSearchTimer((uint8_t)(autoMicroSystem.connectivity.wifiSearchingTimeout / 100U));
        autoMicroSystem.runtime.oled_first_draw = true;
    } else {
        OledUtils_UpdateWiFiSearchTimer((uint8_t)(autoMicroSystem.connectivity.wifiSearchingTimeout / 100U));
    }
}

void OledUtils_RenderWiFiConnectionStatus_Wrapper(){
	autoMicroSystem.runtime.oled_first_draw = true;
	encoder.allowEncoderInput = true;
	menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
	(void)0;
	if (WifiBridge_GetActiveBackend(&wifiBridge) == WIFI_BRIDGE_BACKEND_AT) {
		APP_ESPStatusRefreshRequested();
	} else {
		(void)WifiBridge_GetStatus(&wifiBridge);
	}
	menuSystem.renderFn = OledUtils_RenderWiFiStatus;
	OledUtils_RenderWiFiStatus();
	dashboardConnectivityDirty = false;
	menuSystem.renderFlag = true;
}

void OledUtils_RenderESPFirmwareScreen_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CONNECTIVITY_ESP_FIRMWARE_RECEIVED,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = ReadOnly_UserEventManager;
    OledUtils_RenderESPFirmwareScreen();
    autoMicroSystem.runtime.oled_first_draw = true;
}

void OledUtils_RenderWiFiSearchResults_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CONNECTIVITY_WIFI_RESULTS,
                                 SCREEN_REPORT_SOURCE_MENU);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = true;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = wifiEventManager;
    CLEAR_FLAG(autoMicroSystem.runtime.systemFlags3, WIFI_SEARCHING);

    WiFiResults_BuildMenu();
    menuSystem.currentMenu = &wifiResultsMenu;
    MenuSys_RenderMenu(&menuSystem);
    autoMicroSystem.runtime.oled_first_draw = false;
}

void OledUtils_RenderWiFiDetails_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CONNECTIVITY_WIFI_DETAILS,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = WiFiDetails_UserEventManager;

    if (autoMicroSystem.connectivity.wifiSelectedNetworkIndex >= autoMicroSystem.connectivity.networksFound ||
        autoMicroSystem.connectivity.wifiSelectedNetworkIndex >= WIFI_SCAN_MAX_NETWORKS) {
        autoMicroSystem.connectivity.wifiSelectedNetworkIndex = WIFI_SELECTED_NETWORK_INVALID;
        menuSystem.renderFn = OledUtils_RenderWiFiSearchResults_Wrapper;
        menuSystem.renderFlag = true;
        autoMicroSystem.runtime.oled_first_draw = false;
        return;
    }

    OledUtils_RenderWiFiDetails(autoMicroSystem.connectivity.wifiNetworkSsids[autoMicroSystem.connectivity.wifiSelectedNetworkIndex],
                                autoMicroSystem.connectivity.wifiNetworkEncryptions[autoMicroSystem.connectivity.wifiSelectedNetworkIndex],
                                autoMicroSystem.connectivity.wifiNetworkSignalStrengths[autoMicroSystem.connectivity.wifiSelectedNetworkIndex],
                                autoMicroSystem.connectivity.wifiNetworkChannels[autoMicroSystem.connectivity.wifiSelectedNetworkIndex],
                                autoMicroSystem.connectivity.wifiNetworkDetailValid[autoMicroSystem.connectivity.wifiSelectedNetworkIndex]);
    autoMicroSystem.runtime.oled_first_draw = true;
}

void OledUtils_RenderWiFiAtPassword_Wrapper(void)
{
    MenuSys_SetCurrentScreenCode(&menuSystem,
                                 SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_AT,
                                 SCREEN_REPORT_SOURCE_RENDER);
    OledUtils_DisableContinuousRender();
    autoMicroSystem.runtime.inside_menu_flag = false;
    encoder.allowEncoderInput = true;
    menuSystem.userEventManagerFn = WiFiAtPassword_UserEventManager;

    if (autoMicroSystem.connectivity.wifiSelectedNetworkIndex >= autoMicroSystem.connectivity.networksFound ||
        autoMicroSystem.connectivity.wifiSelectedNetworkIndex >= WIFI_SCAN_MAX_NETWORKS) {
        menuSystem.renderFn = OledUtils_RenderWiFiSearchResults_Wrapper;
        menuSystem.renderFlag = true;
        autoMicroSystem.runtime.oled_first_draw = false;
        return;
    }

    OledUtils_RenderWiFiAtPassword(
        autoMicroSystem.connectivity.wifiNetworkSsids[autoMicroSystem.connectivity.wifiSelectedNetworkIndex],
        WiFiAtPassword_GetLength(),
        WiFiAtPassword_GetSelectedCharacter());
    autoMicroSystem.runtime.oled_first_draw = true;
}

void onRenderComplete(void) {
    (void)0;
    if(IS_FLAG_SET(autoMicroSystem.runtime.systemFlags, OLED_READY)){
        menuSystem.allowPeriodicRefresh = false;
        (void)0;
    }

    if (dashboardModeReportPending) {
        dashboardModeReportPending = false;
        (void)WifiBridge_ReportCarModeChanged(&wifiBridge, (uint8_t)GET_CAR_MODE());
    }
}

void Screens_Init_screenWrappers(void)
{
    irValuesSessionMode = -1;
    unerRouterTransportId = UNER_TR_USB_CDC;
}
