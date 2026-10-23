#ifndef AFR2_SCREENS_H
#define AFR2_SCREENS_H
#include "afr2_oled.h"
#include "afr2_encoder_ui.h"
/* IDs locales reservados; sin reporte wire implementado. */
#define AFR2_SCREEN_HOME 0x010001UL
#define AFR2_SCREEN_MENU 0x010002UL
#define AFR2_SCREEN_SENSORS 0x020001UL
#define AFR2_SCREEN_DISPLAY 0x020002UL
typedef struct {uint32_t screen;uint8_t selection;bool redraw;} Afr2Ui;
typedef struct {
    bool line_valid, ultrasonic_valid;
    uint8_t line_bits; /* bit0=IR1 ... bit4=IR5, polaridad normalizada en sensor */
    uint16_t ultrasonic_mm;
    bool oled_ready;
    uint32_t oled_errors;
} Afr2UiSnapshot;
void Afr2Ui_Init(Afr2Ui *ui);
void Afr2Ui_Apply(Afr2Ui *ui,Afr2UiInput input);
/* Render solo desde main con snapshot coherente. No modifica sensores ni motores. */
void Afr2Ui_Render(Afr2Ui *ui,Afr2Oled *oled,const Afr2UiSnapshot *snapshot);
#endif
