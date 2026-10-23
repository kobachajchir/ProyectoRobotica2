#pragma once
#include <stdint.h>
#include "bitMap.h"
#include "config.h"

// References preserve the existing global symbols without duplicate storage.
// Each library owns its buffers and scheduling state separately.
struct EspSystemRuntime {
    volatile t_bitMap &flags;
    volatile uint16_t &sta_wait;
    volatile uint16_t &sta_retry;
};
struct EspSystemHandle {
    EspSystemRuntime runtime;
    struct { const AppConfig *cached = nullptr; } preferences;
    struct { void *uner_app = nullptr; void *json_bridge = nullptr; } handles;
    EspSystemHandle(volatile t_bitMap &flags, volatile uint16_t &wait,
                    volatile uint16_t &retry) : runtime{flags,wait,retry} {}
};
extern EspSystemHandle g_espSystem;
