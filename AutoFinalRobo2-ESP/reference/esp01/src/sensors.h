#pragma once
#include <stddef.h>

struct SensorsOps {
    void *context = nullptr;
    bool (*initialize)(void *) = nullptr;
    bool (*read_json)(void *, char *out, size_t capacity) = nullptr;
};
struct SensorsHandle {
    SensorsOps ops;
    char json[96]{};
};
bool Sensors_Init(SensorsHandle &h, const SensorsOps &ops);
const char *Sensors_GetReadingsJSON(SensorsHandle &h);
bool Sensors_Init();
const char *Sensors_GetReadingsJSON();
