/* AFR2-20261023-19. Capture reset cause and disable inherited watchdog early. */
#include "afr2_product.h"
#include <avr/io.h>
#include <avr/wdt.h>
static uint8_t boot_cause __attribute__((section(".noinit")));
void Afr2Boot_Early(void) __attribute__((naked,section(".init3"),used));
void Afr2Boot_Early(void) { boot_cause=MCUSR;MCUSR=0;wdt_disable(); }
uint8_t Afr2Boot_Cause(void) { return boot_cause; }
void Afr2Watchdog_Enable(void) { wdt_enable(WDTO_500MS); }
void Afr2Watchdog_Kick(void) { wdt_reset(); }
