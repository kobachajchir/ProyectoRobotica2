#include "wifi_status_led.h"

static void writeLed(WifiStatusLedHandle &h, bool on) {
    if (h.ops.write) h.ops.write(h.ops.context, on);
}
void WifiStatusLed_Begin(WifiStatusLedHandle &h, const WifiStatusLedOps &ops,
                         uint32_t now_ms, uint32_t blink_period_ms) {
    h = WifiStatusLedHandle{};
    h.ops = ops;
    h.blink_period_ms = blink_period_ms;
    if (ops.initialize) ops.initialize(ops.context);
    h.initialized = true;
    h.last_toggle_ms = now_ms;
    writeLed(h, false);
}
void WifiStatusLed_Update(WifiStatusLedHandle &h, WifiStatusLedMode mode, uint32_t now_ms) {
    if (!h.initialized) return;
    if (mode != h.mode) {
        h.mode = mode;
        h.last_toggle_ms = now_ms;
        h.ap_led_on = mode == WIFI_STATUS_LED_AP;
        writeLed(h, mode == WIFI_STATUS_LED_STA || h.ap_led_on);
        return;
    }
    if (mode != WIFI_STATUS_LED_AP) return;
    if ((uint32_t)(now_ms - h.last_toggle_ms) >= h.blink_period_ms) {
        h.last_toggle_ms = now_ms;
        h.ap_led_on = !h.ap_led_on;
        writeLed(h, h.ap_led_on);
    }
}
