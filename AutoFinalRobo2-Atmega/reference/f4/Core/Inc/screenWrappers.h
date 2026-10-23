/*
 * screenWrappers.h
 *
 *  Created on: Jul 23, 2025
 *      Author: kobac
 */

#ifndef INC_SCREENWRAPPERS_H_
#define INC_SCREENWRAPPERS_H_

#include <stdbool.h>
#include "types/menu_types.h"

void OledUtils_DrawItem_Wrapper(const MenuItem *item, int y, bool selected);
void OledUtils_RenderDashboard_Wrapper(void);
void Dashboard_InvalidateConnectivity(void);
void Dashboard_ReturnAfterConnectivityNotification(void);
void OledUtils_RenderStabilizationPrompt_Wrapper(void);
void OledUtils_RenderModePrompt_Wrapper(void);
void OledUtils_About_Wrapper(void);
void OledUtils_AboutInfo_Wrapper(void);
void OledUtils_AboutRepo_Wrapper(void);
void OledUtils_AboutBuild_Wrapper(void);
void OledUtils_AboutBootloader_Wrapper(void);
void OledUtils_RenderGenericConfirm_Wrapper(void);
void MenuSys_RenderMenu_Wrapper(void);
void MenuSys_GoBack_Wrapper(void);
void OledUtils_RenderMotorTest_Wrapper(void);
void OledUtils_RenderValoresIR_Wrapper(void);
void OledUtils_RenderIRAmbiente_Wrapper(void);
void OledUtils_EndIRValuesSession(void);
void OledUtils_ToggleValoresIRView(void);
void OledUtils_RenderTestCamino_Wrapper(void);
void OledUtils_ToggleTestCaminoView(void);
void OledUtils_RenderValoresMPU_Wrapper(void);
void OledUtils_ToggleValoresMPUView(void);
void OledUtils_BeginUnerRouterUsb(void);
void OledUtils_BeginUnerRouterUart(void);
void OledUtils_BeginUnerRouterNrf(void);
void OledUtils_NextUnerRouterPacketView(void);
void OledUtils_PreviousUnerRouterPacketView(void);
bool OledUtils_IsUnerRouterTxView(void);
uint8_t OledUtils_GetUnerRouterTransportId(void);
void OledUtils_RenderMPUCalibration_Wrapper(void);
void OledUtils_RenderScreenLockTimeout_Wrapper(void);
void OledUtils_RenderSystemActionTimeout_Wrapper(void);
void OledUtils_RenderUsbAlivePeriod_Wrapper(void);
void OledUtils_RenderLineBlackWidth_Wrapper(void);
void OledUtils_RenderOledRotationTest_Wrapper(void);
void OledUtils_RenderOledRotationMpuTest_Wrapper(void);
void OledUtils_RenderOledCanvas_Wrapper(void);
void OledUtils_RenderMotorMinPwm_Wrapper(void);
void OledUtils_RenderSingleMotorTest_Wrapper(void);
void OledUtils_RenderBalancePidStatus_Wrapper(void);
void OledUtils_RenderBalancePidLog_Wrapper(void);
void OledUtils_RenderBalancePidGain_Wrapper(void);
void OledUtils_RenderUnerRouterTest_Wrapper(void);
void OledUtils_RenderScreenLocked_Wrapper(void);
void OledUtils_RenderWiFiSearching_Wrapper(void);
void OledUtils_RenderWiFiSearchResults_Wrapper(void);
void OledUtils_RenderWiFiDetails_Wrapper(void);
void OledUtils_RenderWiFiAtPassword_Wrapper(void);
void OledUtils_RenderWiFiConnectionStatus_Wrapper(void);
void OledUtils_RenderESPFirmwareScreen_Wrapper(void);
void WiFiScan_ClearResults(void);
void onRenderComplete(void);

#endif /* INC_SCREENWRAPPERS_H_ */
