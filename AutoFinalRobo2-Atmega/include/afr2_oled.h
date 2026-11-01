#ifndef AFR2_OLED_H
#define AFR2_OLED_H
#include "afr2_i2c_manager.h"
#define AFR2_OLED_WIDTH 128u
#define AFR2_OLED_HEIGHT 64u
#define AFR2_OLED_BYTES 1024u
#define AFR2_OLED_CHUNK 16u
typedef enum { AFR2_BLACK, AFR2_WHITE, AFR2_INVERT } Afr2Color;
typedef struct {
    Afr2I2cManager *manager; /* prestado, debe vivir mas que este handle */
    uint8_t buffer[AFR2_OLED_BYTES];
    /* AFR2-20261029-11: exact acknowledged image; valid bits per16columns.
     * No checksum collisions; unknown chunks are sent after init/recovery. */
    uint8_t acknowledged[AFR2_OLED_BYTES];
    uint8_t valid[8],next_chunk[8];
    uint8_t tx[33]; /* init27bytes or control+<=16data; stable until callback */
    uint8_t command[6];
    uint8_t dirty, page, phase, address7;
    uint8_t contrast,rotation,settings_dirty;
    uint8_t column,count,chunk,next_page;
    uint32_t retry_ms, clock_ms, errors;
    bool initialized;
    bool paused;
    uint8_t last_error,last_failed_phase,consecutive_failures; /* AFR2-20261029-14 */
    bool configured_once;
} Afr2Oled;
void Afr2Oled_Init(Afr2Oled *o,Afr2I2cManager *manager,uint8_t address7,uint32_t now);
void Afr2Oled_Task(Afr2Oled *o,uint32_t now);
/* Cooperative low-priority service: pause stops new submissions, not a byte
 * already on the wire. Resume continues the saved command/data snapshot. */
void Afr2Oled_Pause(Afr2Oled *o,bool paused);
void Afr2Oled_Settings(Afr2Oled *o,uint8_t contrast,uint8_t rotation);
void Afr2Oled_Clear(Afr2Oled *o);
void Afr2Oled_Pixel(Afr2Oled *o,int16_t x,int16_t y,Afr2Color color);
void Afr2Oled_Rect(Afr2Oled *o,int16_t x,int16_t y,uint8_t w,uint8_t h,Afr2Color color);
void Afr2Oled_FillRect(Afr2Oled *o,int16_t x,int16_t y,uint8_t w,uint8_t h,Afr2Color color);
void Afr2Oled_Char(Afr2Oled *o,int16_t x,int16_t y,char ch,Afr2Color color);
void Afr2Oled_Text(Afr2Oled *o,int16_t x,int16_t y,const char *text,Afr2Color color);
/* RAM ASCII text, horizontal pixel clipping [left,right), offset in pixels.
 * Transparent background; caller clears only its own text rectangle. */
void Afr2Oled_TextWindow(Afr2Oled *o,uint8_t left,uint8_t right,int16_t y,const char *text,uint16_t offset,Afr2Color color);
/* Texto PROGMEM en AVR; texto ordinario en host. ASCII 32..126, fallback '?'. */
void Afr2Oled_TextFlash(Afr2Oled *o,int16_t x,int16_t y,const char *text,Afr2Color color);
uint8_t Afr2Font7x10_Row(uint8_t ch,uint8_t row);
#endif
