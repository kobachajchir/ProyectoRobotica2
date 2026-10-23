/* Derivado de conceptos SSD1306 F4: paginas verticales 128x64, wrappers separados.
 * API y transporte nuevos, sin DMA/HAL. No acredita paridad con todos sus renderers.
 */
#include "afr2_oled.h"
#include <string.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#define FLASH PROGMEM
#define READ_BYTE(p) pgm_read_byte(p)
#else
#define FLASH
#define READ_BYTE(p) (*(const uint8_t *)(p))
#endif
enum { IDLE, WAIT_INIT, WAIT_COMMAND, SEND_DATA, WAIT_DATA };
static const uint8_t init_commands[] FLASH={
  0x00,0xae,0xd5,0x80,0xa8,0x3f,0xd3,0x00,0x40,0x8d,0x14,
  0x20,0x02,0xa1,0xc8,0xda,0x12,0x81,0xcf,0xd9,0xf1,0xdb,0x40,0xa4,0xa6,0xaf
};
static void completed(void *ctx,Afr2I2cResult result) {
    Afr2Oled *o=ctx;
    if (result!=AFR2_I2C_OK) {
        o->errors++; o->dirty=0xff; o->initialized=false;
        o->phase=IDLE; o->retry_ms=o->clock_ms+1000u; return;
    }
    if (o->phase==WAIT_INIT) { o->initialized=true; o->dirty=0xff; o->phase=IDLE; }
    else if (o->phase==WAIT_COMMAND) o->phase=SEND_DATA;
    else o->phase=IDLE;
}
void Afr2Oled_Init(Afr2Oled *o,Afr2I2cManager *m,uint8_t address7,uint32_t now) {
    if (!o) return;
    memset(o,0,sizeof(*o)); o->manager=m; o->address7=address7;
    o->dirty=0xff; o->retry_ms=now+100u; o->clock_ms=now;
}
static bool send(Afr2Oled *o,const uint8_t *data,uint16_t size,uint8_t next,uint32_t now) {
    Afr2I2cResult r=Afr2I2c_Submit(o->manager,o->address7,data,size,now,40,completed,o);
    if (r==AFR2_I2C_OK) {o->phase=next; return true;}
    if (r!=AFR2_I2C_BUSY) completed(o,r);
    return false;
}
void Afr2Oled_Task(Afr2Oled *o,uint32_t now) {
    if (!o || !o->manager) return;
    o->clock_ms=now;
    if ((int32_t)(now-o->retry_ms)<0) return;
    if (o->phase==SEND_DATA) { (void)send(o,o->tx,129,WAIT_DATA,now); return; }
    if (o->phase!=IDLE || o->manager->busy) return;
    if (!o->initialized) {
        for (uint8_t i=0;i<sizeof(init_commands);i++) o->tx[i]=READ_BYTE(&init_commands[i]);
        (void)send(o,o->tx,sizeof(init_commands),WAIT_INIT,now); return;
    }
    for (uint8_t page=0;page<8;page++) {
        if (!(o->dirty&(1u<<page))) continue;
        o->page=page; o->command[0]=0; o->command[1]=(uint8_t)(0xb0u+page);
        o->command[2]=0; o->command[3]=0x10;
        o->tx[0]=0x40; memcpy(&o->tx[1],&o->buffer[(uint16_t)page*128u],128);
        if (send(o,o->command,4,WAIT_COMMAND,now)) o->dirty&=(uint8_t)~(1u<<page);
        return;
    }
}
void Afr2Oled_Clear(Afr2Oled *o) {
    if (!o) return;
    for (uint16_t i=0;i<AFR2_OLED_BYTES;i++) if(o->buffer[i]) {
        o->buffer[i]=0; o->dirty|=(uint8_t)(1u<<(i/128u));
    }
}
void Afr2Oled_Pixel(Afr2Oled *o,int16_t x,int16_t y,Afr2Color color) {
    if(!o || x<0 || y<0 || x>=128 || y>=64) return;
    uint16_t index=(uint16_t)x+((uint16_t)y/8u)*128u;
    uint8_t old=o->buffer[index], mask=(uint8_t)(1u<<(y&7));
    uint8_t val=color==AFR2_WHITE?(old|mask):color==AFR2_INVERT?(old^mask):(old&(uint8_t)~mask);
    if(val!=old){o->buffer[index]=val;o->dirty|=(uint8_t)(1u<<(y/8));}
}
void Afr2Oled_Rect(Afr2Oled *o,int16_t x,int16_t y,uint8_t w,uint8_t h,Afr2Color color) {
    if (!w || !h) return;
    for(uint8_t i=0;i<w;i++){Afr2Oled_Pixel(o,x+i,y,color);if(h>1)Afr2Oled_Pixel(o,x+i,y+h-1,color);}
    for(uint8_t i=1;i+1<h;i++){Afr2Oled_Pixel(o,x,y+i,color);if(w>1)Afr2Oled_Pixel(o,x+w-1,y+i,color);}
}
void Afr2Oled_Char(Afr2Oled *o,int16_t x,int16_t y,char ch,Afr2Color color) {
    for(uint8_t row=0;row<10;row++) {
        uint8_t bits=Afr2Font7x10_Row((uint8_t)ch,row);
        for(uint8_t col=0;col<7;col++) if(bits&(0x80u>>col)) Afr2Oled_Pixel(o,x+col,y+row,color);
    }
}
void Afr2Oled_Text(Afr2Oled *o,int16_t x,int16_t y,const char *s,Afr2Color color) {
    if(!s) return;
    while(*s && x<128){Afr2Oled_Char(o,x,y,*s++,color);x+=7;}
}
void Afr2Oled_TextFlash(Afr2Oled *o,int16_t x,int16_t y,const char *s,Afr2Color color) {
    if(!s) return;
    uint8_t ch;
    while(x<128 && (ch=READ_BYTE(s++))!=0){Afr2Oled_Char(o,x,y,(char)ch,color);x+=7;}
}
