#ifndef AFR2_QR_SCREEN_H
#define AFR2_QR_SCREEN_H
#include "afr2_screens.h"
/* Uses the OLED framebuffer as encoder scratch, then restores a complete frame.
 * Main-loop only. No commands, hardware reads, allocation or persistent secrets. */
void Afr2Qr_Render(Afr2Oled *,const Afr2UiSnapshot *,uint8_t,uint32_t);
/* AFR2-20261108-02: forces the next Render to repaint (entering/leaving the QR screens). */
void Afr2Qr_Invalidate(void);
/* Reusable QRScreen body for caller-chosen RAM bytes (UTF-8 text or binary).
 * Clears the frame, draws the 62x62 QR at (1,1), reserves x67..127 for caller
 * captions/gestures. Returns 0 with a clear frame if the complete payload cannot
 * fit. Caller must enforce stopped-main-loop use and capture privacy. */
uint8_t Afr2Qr_DrawPayload(Afr2Oled *,const uint8_t *,uint16_t);
#endif
