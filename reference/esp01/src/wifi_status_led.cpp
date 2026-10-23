#include "wifi_status_led.h"
#include "esp_platform.h"

static WifiStatusLedHandle s_led;
void WifiStatusLed_Begin() {
    WifiStatusLedOps ops;
    ops.initialize = EspPlatform_LedInit;
    ops.write = EspPlatform_LedWrite;
    // Preserve GPIO initialization before sampling the initial clock.
    ops.initialize(ops.context);
    ops.initialize = nullptr;
    WifiStatusLed_Begin(s_led, ops, EspPlatform_Millis(nullptr));
}
void WifiStatusLed_Update(WifiStatusLedMode mode, uint32_t now_ms) {
    WifiStatusLed_Update(s_led, mode, now_ms);
}
