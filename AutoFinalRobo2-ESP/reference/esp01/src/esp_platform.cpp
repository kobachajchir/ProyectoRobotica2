#include "esp_platform.h"
#include <Arduino.h>

uint32_t EspPlatform_Millis(void *) { return millis(); }
void EspPlatform_Yield(void *) { yield(); }
void EspPlatform_LedInit(void *) {
    digitalWrite(2u, HIGH);
    pinMode(2u, OUTPUT);
}
void EspPlatform_LedWrite(void *, bool on) { digitalWrite(2u, on ? LOW : HIGH); }

void EspPlatform_Delay0(void *) { delay(0); }
