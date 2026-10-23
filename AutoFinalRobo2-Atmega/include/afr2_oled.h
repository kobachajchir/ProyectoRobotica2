#ifndef AFR2_OLED_H
#define AFR2_OLED_H
#include "afr2_i2c_manager.h"
#define AFR2_OLED_WIDTH 128u
#define AFR2_OLED_HEIGHT 64u
#define AFR2_OLED_BYTES 1024u
typedef enum { AFR2_BLACK, AFR2_WHITE, AFR2_INVERT } Afr2Color;
typedef struct {
    Afr2I2cManager *manager; /* prestado, debe vivir mas que este handle */
    uint8_t buffer[AFR2_OLED_BYTES];
    uint8_t tx[129]; /* snapshot de pagina, estable hasta fin de transferencia */
    uint8_t command[4];
    uint8_t dirty, page, phase, address7;
    uint32_t retry_ms, clock_ms, errors;
    bool initialized;
} Afr2Oled;
void Afr2Oled_Init(Afr2Oled *o,Afr2I2cManager *manager,uint8_t address7,uint32_t now);
void Afr2Oled_Task(Afr2Oled *o,uint32_t now);
void Afr2Oled_Clear(Afr2Oled *o);
void Afr2Oled_Pixel(Afr2Oled *o,int16_t x,int16_t y,Afr2Color color);
void Afr2Oled_Rect(Afr2Oled *o,int16_t x,int16_t y,uint8_t w,uint8_t h,Afr2Color color);
void Afr2Oled_Char(Afr2Oled *o,int16_t x,int16_t y,char ch,Afr2Color color);
void Afr2Oled_Text(Afr2Oled *o,int16_t x,int16_t y,const char *text,Afr2Color color);
/* Texto PROGMEM en AVR; texto ordinario en host. ASCII 32..126, fallback '?'. */
void Afr2Oled_TextFlash(Afr2Oled *o,int16_t x,int16_t y,const char *text,Afr2Color color);
uint8_t Afr2Font7x10_Row(uint8_t ch,uint8_t row);
#endif
