#pragma once
#include <stdint.h>

struct TimebaseOps {
    void *context = nullptr;
    uint32_t (*now_ms)(void *) = nullptr;
    void (*cooperate)(void *) = nullptr;
};
struct TimebaseHandle {
    TimebaseOps ops;
    volatile uint16_t *sta_wait = nullptr;
    volatile uint16_t *sta_retry = nullptr;
    bool tick_flag = false;
    uint32_t ticks = 0;
    uint32_t last_ms = 0;
    uint8_t accumulated_ms = 0;
};
void Timebase_Init(TimebaseHandle &handle, const TimebaseOps &ops,
                   volatile uint16_t *sta_wait = nullptr,
                   volatile uint16_t *sta_retry = nullptr);
void Timebase_Update(TimebaseHandle &handle);
bool Timebase_ConsumeTick(TimebaseHandle &handle);
uint32_t Timebase_Ticks(const TimebaseHandle &handle);

// Existing singleton API remains available through the ESP adapter.
void Timebase_Init10ms();
void Timebase_Update();
bool Timebase_ConsumeTick();
uint32_t Timebase_Ticks();
