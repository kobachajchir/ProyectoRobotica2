/* AFR2-20261023-15. Sole AVR TWI master; OLED is the only client. */
#ifndef AFR2_TWI_H
#define AFR2_TWI_H
#include "afr2_i2c_manager.h"
/* Init before sei(). External pull-ups required; no internal 5 V pull-ups. */
void Afr2Twi_Init(Afr2I2cHardware *hardware);
/* Main loop, before Manager Tick. Nonblocking bus recovery, <=50 ms attempt. */
void Afr2Twi_Task(uint32_t now_ms);
#endif
