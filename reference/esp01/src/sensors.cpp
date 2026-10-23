#include "sensors.h"
#include <stdio.h>

static SensorsHandle s_sensors;
static bool defaultRead(void *, char *out, size_t capacity) {
    // Existing placeholder values are intentionally preserved.
    snprintf(out, capacity, "{\"temperature\":25.1,\"humidity\":45.0,\"pressure\":1013.2}");
    return true;
}
bool Sensors_Init(SensorsHandle &h, const SensorsOps &ops) {
    h = SensorsHandle{};
    h.ops = ops;
    return !ops.initialize || ops.initialize(ops.context);
}
const char *Sensors_GetReadingsJSON(SensorsHandle &h) {
    if (h.ops.read_json) h.ops.read_json(h.ops.context, h.json, sizeof(h.json));
    h.json[sizeof(h.json) - 1u] = '\0';
    return h.json;
}
bool Sensors_Init() {
    SensorsOps ops;
    ops.read_json = defaultRead;
    s_sensors.ops = ops;
    return true;
}
const char *Sensors_GetReadingsJSON() {
    // Legacy callers could read without first calling Sensors_Init().
    if (!s_sensors.ops.read_json) s_sensors.ops.read_json = defaultRead;
    return Sensors_GetReadingsJSON(s_sensors);
}
