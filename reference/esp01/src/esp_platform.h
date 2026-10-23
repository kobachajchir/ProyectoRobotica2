#pragma once
#include <stdint.h>

// Arduino implementation is isolated in esp_platform.cpp.
uint32_t EspPlatform_Millis(void *context);
void EspPlatform_Yield(void *context);
void EspPlatform_LedInit(void *context);
void EspPlatform_LedWrite(void *context, bool on);

void EspPlatform_Delay0(void *context);
