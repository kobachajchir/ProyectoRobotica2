/*
 * menu_definitions.h
 *
 *  Created on: 31 ene 2026
 *      Author: kobac
 */

#ifndef INC_MENU_DEFINITIONS_H_
#define INC_MENU_DEFINITIONS_H_

#include "app_menu_binding.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Menus */
extern MenuSystem menuSystem;

extern SubMenu mainMenu;
extern SubMenu submenu1;
extern SubMenu submenu2;
extern SubMenu submenu3;
extern SubMenu submenuUSB;
extern SubMenu submenuESP;
extern SubMenu submenuSensorsIr;
extern SubMenu submenuSensorsMpu;
extern SubMenu submenuSensorsDisplay;
extern SubMenu submenuUnerRouter;
extern SubMenu submenuMotors;
extern SubMenu submenuBalance;
extern SubMenu submenuAbout;
extern SubMenu submenuSystem;
extern SubMenu submenuPreferences;
extern SubMenu submenuPreferencesEncoder;
extern SubMenu submenuPreferencesDisplay;
extern SubMenu submenuPreferencesRotation;
extern SubMenu submenuPreferencesSystem;
extern SubMenu submenuPreferencesSecurity;
extern SubMenu submenuPreferencesUnerRouter;
extern SubMenu submenuPreferencesLine;
extern SubMenu submenuPreferencesMotors;
extern SubMenu submenuPreferencesMotorPwm;
extern SubMenu submenuPreferencesIR;

/* Items */
extern MenuItem mainMenuItems[];
extern MenuItem submenu1Items[];
extern MenuItem submenu2Items[];
extern MenuItem submenu3Items[];
extern MenuItem submenuUSBItems[];
extern MenuItem submenuESPItems[];
extern MenuItem submenuSensorsIrItems[];
extern MenuItem submenuSensorsMpuItems[];
extern MenuItem submenuSensorsDisplayItems[];
extern MenuItem submenuUnerRouterItems[];
extern MenuItem submenuMotorsItems[];
extern MenuItem submenuBalanceItems[];
extern MenuItem submenuAboutItems[];
extern MenuItem submenuSystemItems[];
extern MenuItem submenuPreferencesItems[];
extern MenuItem submenuPreferencesEncoderItems[];
extern MenuItem submenuPreferencesDisplayItems[];
extern MenuItem submenuPreferencesRotationItems[];
extern MenuItem submenuPreferencesSystemItems[];
extern MenuItem submenuPreferencesSecurityItems[];
extern MenuItem submenuPreferencesUnerRouterItems[];
extern MenuItem submenuPreferencesLineItems[];
extern MenuItem submenuPreferencesMotorsItems[];
extern MenuItem submenuPreferencesMotorPwmItems[];
extern MenuItem submenuPreferencesIRItems[];

bool MenuItem_IsTestModeVisible(void);
bool MenuItem_IsScreenLockTimeoutVisible(void);
const MenuItem* MenuSys_GetItemByScreenCode(ScreenCode_t code);

#ifdef __cplusplus
}
#endif

#endif /* INC_MENU_DEFINITIONS_H_ */
