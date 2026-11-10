/* AFR2-20261023-15. Hardware composition, C/avr-libc, no Arduino. */
#ifndef AFR2_BOARD_H
#define AFR2_BOARD_H
#include <stdint.h>
#ifndef AFR2_HALL_CONNECTED
#define AFR2_HALL_CONNECTED 1 /* AFR2-20261027-10: user confirms wiring and requests Hall capture. */
#endif
/* Leaves global IRQ disabled; Timer0 CTC 1ms, PWM/direction latches inactive. */
void Afr2Board_Init(void);
uint32_t Afr2Board_Millis(void);
void Afr2Board_EncoderInit(void);
/* bit1 CLK and bit0 DT from one PINE read; bit2 SW pressed (active low). */
uint8_t Afr2Board_EncoderLevels(void);
/* Main only. 0=empty, 1=queued AB, 2=overflow: queue discarded, AB=current.
 * Result2 requires resetting the decoder's partial step, never acting on it. */
uint8_t Afr2Board_EncoderRead(uint8_t *ab);
/* Init before sei(); x1 Hall capture is owned by the board interrupt layer. */
void Afr2Board_HallInit(void);
void Afr2Board_HallSnapshot(int32_t counts[4]);
/* AFR2-20261110-15: 1 ms windows in which the Hall or encoder IRQ budget ran out and the
 * source was masked until the next tick (electrical noise). Saturating counters since boot. */
void Afr2Board_NoiseCounts(uint16_t *hall,uint16_t *encoder);
#endif
