#include "esp_system.h"
#include "globals.h"

volatile t_bitMap flagsGlobals{};
volatile uint16_t tm_sta_wait = 0;
volatile uint16_t tm_sta_retry = 0;
EspSystemHandle g_espSystem(flagsGlobals,tm_sta_wait,tm_sta_retry);
