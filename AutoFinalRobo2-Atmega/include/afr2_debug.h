/* AFR2-20261108-01. Step-by-step debug log on a spare USART (afr2_usart).
 *
 * Default: USART0, TX on D1, 115200 8N1 -> the Mega's own USB serial. The
 * ESP link stays on USART1 (afr2_uart1.c) and is only observed, never driven.
 * Main context only: never call from an ISR. Lines that do not fit in the TX
 * ring are dropped and counted (STAT lost=N); logging never blocks the loop.
 *
 * AFR2_DEBUG_LOG (Makefile): 0 = off (no code, no RAM), 1 = events,
 * 2 = verbose (every remote capture and remote UI action).
 * Overridable: AFR2_DEBUG_PORT (0..3), AFR2_DEBUG_BAUD, AFR2_DEBUG_TX_SIZE (power of two).
 */
#ifndef AFR2_DEBUG_H
#define AFR2_DEBUG_H
#include <stdint.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include "afr2_link.h"

#ifndef AFR2_DEBUG_LOG
#define AFR2_DEBUG_LOG 0
#endif

#if AFR2_DEBUG_LOG
void Afr2Debug_Init(uint8_t avr_reset_cause, uint32_t avr_boot_id);
void Afr2Debug_Printf(PGM_P format, ...);
void Afr2Debug_Task(uint32_t now_ms, const Afr2Link *link);
void Afr2Debug_LoopTime(uint16_t elapsed_ms);
/* Every byte received from the ESP. in_frame: the byte belongs to a ROBO2 frame
 * past its header (payload may hold the AP passphrase): never treated as text. */
void Afr2Debug_PeerByte(uint8_t byte, bool in_frame);
void Afr2Debug_LinkEvent(const Afr2LinkEvent *event);
void Afr2Debug_Input(int8_t rotation, bool click, bool menu, uint32_t now_ms);
void Afr2Debug_NetEvent(uint8_t kind, uint8_t arg, uint32_t sequence);
void Afr2Debug_NetState(uint8_t mode, uint8_t sta, uint8_t ap, uint8_t clients, uint8_t web, uint32_t esp_boot);
uint8_t Afr2Debug_TxIdle(void);
#define AFR2_DBG(format, ...) Afr2Debug_Printf(PSTR(format), ##__VA_ARGS__)
#else
static inline void Afr2Debug_Init(uint8_t c, uint32_t b) { (void)c; (void)b; }
static inline void Afr2Debug_Task(uint32_t n, const Afr2Link *l) { (void)n; (void)l; }
static inline void Afr2Debug_LoopTime(uint16_t e) { (void)e; }
static inline void Afr2Debug_PeerByte(uint8_t b, bool f) { (void)b; (void)f; }
static inline void Afr2Debug_LinkEvent(const Afr2LinkEvent *e) { (void)e; }
static inline void Afr2Debug_Input(int8_t r, bool c, bool m, uint32_t n) { (void)r; (void)c; (void)m; (void)n; }
static inline void Afr2Debug_NetEvent(uint8_t k, uint8_t a, uint32_t s) { (void)k; (void)a; (void)s; }
static inline void Afr2Debug_NetState(uint8_t m, uint8_t s, uint8_t a, uint8_t c, uint8_t w, uint32_t b) {
    (void)m; (void)s; (void)a; (void)c; (void)w; (void)b;
}
static inline uint8_t Afr2Debug_TxIdle(void) { return 1u; }
#define AFR2_DBG(...) ((void)0)
#endif

#if AFR2_DEBUG_LOG >= 2
#define AFR2_DBG_V(format, ...) AFR2_DBG(format, ##__VA_ARGS__)
#else
#define AFR2_DBG_V(...) ((void)0)
#endif
#endif
