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
/* Poll propuesto 1ms en main; pressed=true significa SW bajo. No sirve para Hall motriz. */
Afr2UiInput Afr2EncoderUi_Poll(Afr2EncoderUi *e,bool clk,bool dt,bool pressed,uint32_t now);
#endif
