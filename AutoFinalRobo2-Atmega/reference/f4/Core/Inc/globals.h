/*
 * globals.h
 *
 * F411 UI/application globals kept compatible with the original menu layer.
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#include "system.h"
#include "tcrt5000.h"
extern TCRT5000_Handle irSensors;
#include "balance_control.h"
#include "encoder.h"
#include "i2c_manager.h"
#include "line_follow_control.h"
#include "app_menu_binding.h"
#include "motor_control.h"
#include "motion_control.h"
#include "motion_estimator.h"
#include "mpu9250.h"
#include "oled_handle.h"
#include "types/IPStruct_t.h"
#include "types/carmode_type.h"
#include "user_button.h"
#include "utils/macros_utils.h"
#include <stdbool.h>
#include <stdint.h>

#include "system_flags.h"

#define I2C_ADDR_OLED                           0x3Cu
#define I2C_ADDR_MPU                            0x68u

#define TCRT5000_NUM_SENSORS                    8u

#define MENU_VISIBLE_ITEMS                      3
#define MENU_ITEM_Y0                            4
#define MENU_ITEM_SPACING                       21
#define CURSOR_X                                0
#define CURSOR_WIDTH                            8
#define CURSOR_HEIGHT                           16
#define ICON_X                                  8
#define ICON_WIDTH                              16
#define ICON_HEIGHT                             16
#define ICON_Y_OFFSET                           -13

#define WIFIDEFAULTSEARCHTIMEOUT                1000u
#define WIFI_RESULTS_MENU_MAX_ITEMS             (WIFI_SCAN_MAX_NETWORKS + 2u)





extern const char firmwareName[];
extern const char firmwareVersion[];
extern const char firmwareTarget[];
extern const char buildDate[];
extern const char buildTime[];
extern const char buildProfile[];
extern const char buildCompiler[];

extern MotorControl_Handle motorTask;
extern BalanceControl_Handle balanceControl;
extern MotionControl_Handle motionControl;
extern MotionEstimator_Handle motionEstimator;
extern LineFollowControl_Handle lineFollowControl;
extern MPU9250_Handle_t mpuTask;
extern UserButton_Handle_t btnUser;
extern ENC_Handle_t encoder;
extern I2C_ManagerHandle i2cManager;

extern MenuSystem menuSystem;
extern SubMenu mainMenu, submenu1, submenu2, submenu3, submenuESP, submenuAbout;
extern SubMenu wifiResultsMenu;
extern MenuItem wifiResultsItems[WIFI_RESULTS_MENU_MAX_ITEMS];
extern OledHandle oledHandle;

// Macros para leer/escribir el modo en nibble H
#define GET_CAR_MODE()         (SYSTEM_CAR_MODE(autoMicroSystem.runtime.carModeFlags))
#define SET_CAR_MODE(m)        NIBBLEH_SET_STATE(autoMicroSystem.runtime.carModeFlags, (m) & 0x0F)
#define ADVANCE_CAR_MODE()     SET_CAR_MODE((GET_CAR_MODE() + 1) % CAR_MODE_MAX)
#define DECREMENT_CAR_MODE()  SET_CAR_MODE((GET_CAR_MODE() + CAR_MODE_MAX - 1) % CAR_MODE_MAX)
#endif /* GLOBALS_H */
