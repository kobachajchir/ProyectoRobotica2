#ifndef AFR2_ENCODER_UI_H
#define AFR2_ENCODER_UI_H
#include <stdbool.h>
#include <stdint.h>
typedef struct { int8_t rotation; bool click, menu; } Afr2UiInput;
typedef struct {
    uint8_t ab, transitions;
    int8_t partial;
    bool raw_pressed, stable_pressed, long_sent, armed;
    uint32_t changed_ms, pressed_ms;
} Afr2EncoderUi;
/* init con niveles actuales. Si arranca pulsado, esperar soltar antes de emitir. */
void Afr2EncoderUi_Init(Afr2EncoderUi *e,bool clk,bool dt,bool pressed,uint32_t now);
/* AFR2-20261024-11. Main only: consume queued AB edges in arrival order.
 * ab: bit1 CLK / bit0 DT. Reset after queue overflow/config discontinuity. */
int8_t Afr2EncoderUi_Transition(Afr2EncoderUi *e,uint8_t ab);
void Afr2EncoderUi_ResetRotation(Afr2EncoderUi *e,uint8_t ab);
/* SW sampled in main, target1ms; pressed=true means active low.
 * Retains20ms debounce,800ms exclusive long press and boot-held suppression. */
Afr2UiInput Afr2EncoderUi_Button(Afr2EncoderUi *e,bool pressed,uint32_t now);
/* Compatibility composition for polling callers. Product uses the IRQ queue
 * plus Button separately, never live AB polling to reconstruct lost edges. */
Afr2UiInput Afr2EncoderUi_Poll(Afr2EncoderUi *e,bool clk,bool dt,bool pressed,uint32_t now);
#endif
