#include "globals.h"
#include "uirouter_context.h"
UiRouterHandle appUiRouter;
UiRouterHandle *UiRouter_Context;
#include "ap_context.h"
ApHandle appAp;
ApHandle *Ap_Context;
#include "favorites_context.h"
FavoritesHandle appFavorites;
FavoritesHandle *Favorites_Context;
#include "permission_context.h"
PermissionHandle appPermission;
PermissionHandle *Permission_Context;
#include "screen_context.h"
Screens_Handle appScreens;
Screens_Handle *Screens_Context;
#include "app_wifi_binding.h"
WifiBridge_Handle wifiBridge;
WifiBridge_Credentials wifiBridgeCredentials = {
    .flags.raw = ((sizeof(WIFI_BRIDGE_AT_DEFAULT_PASSWORD) > 1u) ? 0x02u : 0x00u),
    .ssid = "",
    .password = WIFI_BRIDGE_AT_DEFAULT_PASSWORD,
};


#include "app_preferences_binding.h"
AppPreferences_Handle appPreferencesHandle;
#include "app_display.h"
SSD1306_Handle appDisplay;
TCRT5000_Handle irSensors;

SystemHandle autoMicroSystem = {
    .configuration = { .ir_lights_enabled = true },
    .connectivity = {
        .wifiSearchingTimeout = WIFIDEFAULTSEARCHTIMEOUT,
        .wifiSelectedNetworkIndex = WIFI_SELECTED_NETWORK_INVALID,
        .espWifiConnection = {
            .staSsid = "WiFi", .apSsid = "AutoMicro_AP", .apPassword = ""
        }
    }
};
const char firmwareName[] = "AutoMicro F4";
#include "app_version.h"
const char firmwareVersion[] = APP_FIRMWARE_VERSION;
const char firmwareTarget[] = "STM32F411CEU6";
const char buildDate[] = __DATE__;
const char buildTime[] = __TIME__;
#if defined(DEBUG)
const char buildProfile[] = "DEBUG";
#else
const char buildProfile[] = "RELEASE";
#endif
const char buildCompiler[] = "GCC " __VERSION__;

MotorControl_Handle motorTask = {
    .motorData = {
        .motorLeft = {
            .speedPercent = 40.0f,
            .motorDirection = MOTOR_DIR_FORWARD,
        },
        .motorRight = {
            .speedPercent = 40.0f,
            .motorDirection = MOTOR_DIR_FORWARD,
        },
    },
};

BalanceControl_Handle balanceControl;
MotionControl_Handle motionControl;
MotionEstimator_Handle motionEstimator;
LineFollowControl_Handle lineFollowControl;
MPU9250_Handle_t mpuTask;

OledHandle oledHandle;

MenuItem wifiResultsItems[WIFI_RESULTS_MENU_MAX_ITEMS] = {0};
SubMenu wifiResultsMenu = {
    .name = "Redes WiFi",
    .items = wifiResultsItems,
    .itemCount = 0,
    .currentItemIndex = 0,
    .lastSelectedItemIndex = -1,
    .firstVisibleItem = 0,
    .lastVisibleItem = -1,
    .parent = &submenu1,
    .icon = NULL,
    .screen_code = SCREEN_CODE_CONNECTIVITY_WIFI_RESULTS
};
