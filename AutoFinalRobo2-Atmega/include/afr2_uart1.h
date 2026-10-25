/* AFR2-20261023-11. USART1 only: D18 TX1 / D19 RX1, candidate 115200 8N1. */
#ifndef AFR2_UART1_H
#define AFR2_UART1_H
#include <stdint.h>
void Afr2Uart1_Init(void);
uint8_t Afr2Uart1_Read(uint8_t *byte);
uint8_t Afr2Uart1_Write(const uint8_t *frame, uint8_t length);
uint8_t Afr2Uart1_TakeFault(void);
uint8_t Afr2Uart1_TxIdle(void);
#endif
