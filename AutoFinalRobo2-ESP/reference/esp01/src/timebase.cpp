#include "timebase.h"
#include "globals.h"
#include "esp_platform.h"

static TimebaseHandle s_timebase;
void Timebase_Init10ms() {
    TimebaseOps ops;
    ops.now_ms = EspPlatform_Millis;
    ops.cooperate = EspPlatform_Yield;
    Timebase_Init(s_timebase, ops, &tm_sta_wait, &tm_sta_retry);
}
void Timebase_Update() { Timebase_Update(s_timebase); }
bool Timebase_ConsumeTick() { return Timebase_ConsumeTick(s_timebase); }
uint32_t Timebase_Ticks() { return Timebase_Ticks(s_timebase); }
