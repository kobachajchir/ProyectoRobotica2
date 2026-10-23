#include "timebase.h"

void Timebase_Init(TimebaseHandle &h, const TimebaseOps &ops,
                   volatile uint16_t *sta_wait, volatile uint16_t *sta_retry) {
    h = TimebaseHandle{};
    h.ops = ops;
    h.sta_wait = sta_wait;
    h.sta_retry = sta_retry;
    h.last_ms = ops.now_ms ? ops.now_ms(ops.context) : 0u;
}
void Timebase_Update(TimebaseHandle &h) {
    if (!h.ops.now_ms) return;
    uint32_t now = h.ops.now_ms(h.ops.context);
    uint8_t guard = 0;
    while ((uint32_t)(now - h.last_ms) >= 1u && guard < 10u) {
        ++h.last_ms;
        ++guard;
        // Preserve the legacy 1 ms decrement and bounded catch-up exactly.
        if (h.sta_wait && *h.sta_wait > 0) --*h.sta_wait;
        if (h.sta_retry && *h.sta_retry > 0) --*h.sta_retry;
        if (++h.accumulated_ms >= 10u) {
            h.accumulated_ms = 0;
            ++h.ticks;
            h.tick_flag = true;
        }
    }
    if (guard && h.ops.cooperate) h.ops.cooperate(h.ops.context);
}
bool Timebase_ConsumeTick(TimebaseHandle &h) {
    bool pending = h.tick_flag;
    h.tick_flag = false;
    return pending;
}
uint32_t Timebase_Ticks(const TimebaseHandle &h) { return h.ticks; }
