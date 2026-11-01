/* AFR2-20261023-15. Sole AVR TWI master; OLED is the only client. */
#ifndef AFR2_TWI_H
#define AFR2_TWI_H
#include "afr2_i2c_manager.h"
/* Init before sei(). AFR2-20261029-10: weak AVR-VCC pulls enabled by default,
 * matching user-confirmed diagnostic09. AFR2_TWI_INTERNAL_PULLUPS=0 overrides. */
void Afr2Twi_Init(Afr2I2cHardware *hardware);
/* Main loop, before Manager Tick. Nonblocking bus recovery, <=50 ms attempt. */
void Afr2Twi_Task(uint32_t now_ms);
/* AFR2-20261028-19. Main-only, atomic diagnostic snapshot, no bus operation.
 * status=0xff: no ISR status since latest start attempt; otherwise TW_STATUS.
 * recovery: 0=none, 1=in progress, 2=failed. Levels are logic, not volts. */
typedef struct {
    uint8_t status,sda_high,scl_high,recovery;
} Afr2TwiDiagnostics;
void Afr2Twi_Diagnostics(Afr2TwiDiagnostics *out);
#endif
