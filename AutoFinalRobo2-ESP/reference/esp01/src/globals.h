#pragma once
#include <stdint.h>
#include "bitMap.h"
#include "config.h"

extern volatile t_bitMap flagsGlobals;
extern AppConfig cfg; // Historical declaration; startup uses its local config.
// === Timers (down-counters en ticks de 10 ms) ===
extern volatile uint16_t tm_sta_wait;   // 10 s para STA
extern volatile uint16_t tm_sta_retry;  // p. ej. 5 s entre reintentos

// Tiempos (en ticks de 10 ms)
#define WIFI_CONNECT_TIMEOUT_TICKS  (1000u)  // 10 s
#define STA_RETRY_PERIOD_TICKS      (500u)   // 5 s

#define GF_WIFI_STA_RETRY_DUE  flagsGlobals.bits.b0
#define GF_WIFI_STA_TIMEOUT  flagsGlobals.bits.b1
#define GF_WIFI_STA_CONNECTING  flagsGlobals.bits.b2
#define GF_WIFI_STA_CONNECTED  flagsGlobals.bits.b3
#define GF_WIFI_AP_CREATING  flagsGlobals.bits.b4
#define GF_WIFI_AP_RUNNING  flagsGlobals.bits.b5

#define SSID_MAX_LEN   32
#define PASS_MAX_LEN   63

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
} // extern "C"
#endif
