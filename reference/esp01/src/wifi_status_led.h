#pragma once
#include <stdint.h>

typedef enum {
    WIFI_STATUS_LED_INACTIVE = 0,
    WIFI_STATUS_LED_AP,
    WIFI_STATUS_LED_STA
} WifiStatusLedMode;
struct WifiStatusLedOps {
    void *context = nullptr;
    void (*initialize)(void *) = nullptr;
    void (*write)(void *, bool on) = nullptr;
};
struct WifiStatusLedHandle {
    WifiStatusLedOps ops;
    bool initialized = false;
    bool ap_led_on = false;
    uint32_t last_toggle_ms = 0;
    uint32_t blink_period_ms = 500u;
    WifiStatusLedMode mode = WIFI_STATUS_LED_INACTIVE;
};
void WifiStatusLed_Begin(WifiStatusLedHandle &h, const WifiStatusLedOps &ops,
                         uint32_t now_ms, uint32_t blink_period_ms = 500u);
void WifiStatusLed_Update(WifiStatusLedHandle &h, WifiStatusLedMode mode, uint32_t now_ms);
// GPIO2 must be owned only after successful boot, from setup().
void WifiStatusLed_Begin();
void WifiStatusLed_Update(WifiStatusLedMode mode, uint32_t now_ms);
