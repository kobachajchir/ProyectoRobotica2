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

static uint32_t boot_cookie __attribute__((section(".noinit")));
static uint32_t boot_serial __attribute__((section(".noinit")));
uint32_t Afr2Boot_Id(void) {
    static uint8_t initialized;
    if(!initialized){
        if((boot_cause&_BV(PORF)) || boot_cookie!=0x41465232UL)boot_serial=0;
        boot_cookie=0x41465232UL;++boot_serial;if(!boot_serial)++boot_serial;initialized=1;
    }
    return boot_serial;
}
void Afr2Watchdog_Reboot(void) { __asm__ __volatile__("cli" ::: "memory");wdt_enable(WDTO_15MS);for(;;){} }
