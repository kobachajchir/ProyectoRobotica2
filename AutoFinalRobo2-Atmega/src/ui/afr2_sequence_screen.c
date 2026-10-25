/* AFR2-20261024-07. Original AFR2 renderer: only prepared snapshot -> pixels.
 * No hardware reads, I2C calls, delays, commands or dynamic allocation. */
#include "afr2_screens.h"
#include "afr2_sequence_labels.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>

static const char names[AFR2_SEQ_LABEL_COUNT][18] PROGMEM={
    "Desplazamiento","Rampa","Giro sobre eje","Curva","Espera","Finalizacion","Cuatro ruedas"
};
static const uint8_t icons[AFR2_SEQ_LABEL_COUNT][8] PROGMEM={
    {0x18,0x3c,0x7e,0x18,0x18,0x18,0x18,0},
    {0x02,0x06,0x0a,0x12,0x22,0x42,0x7e,0},
    {0x3c,0x42,0x81,0x05,0x0f,0x01,0x42,0x3c},
    {0x1e,0x22,0x42,0x40,0x40,0x40,0x40,0},
    {0x3c,0x42,0x91,0x91,0x8d,0x81,0x42,0x3c},
    {0,0x02,0x06,0x8c,0xd8,0x70,0x20,0},
    {0x66,0xff,0x66,0x3c,0x3c,0x66,0xff,0x66}
};
static void outline(Afr2Oled *o,uint8_t x,uint8_t y,uint8_t w,uint8_t h) {
    Afr2Oled_Rect(o,x,y,w,1,AFR2_WHITE);Afr2Oled_Rect(o,x,y+h-1,w,1,AFR2_WHITE);
    Afr2Oled_Rect(o,x,y,1,h,AFR2_WHITE);Afr2Oled_Rect(o,x+w-1,y,1,h,AFR2_WHITE);
}
void Afr2SequenceScreen_Render(Afr2Oled *o,const Afr2UiSnapshot *s) {
    uint8_t count=s->sequence_count, index=s->sequence_index;
    uint8_t completed=s->sequence_state==4?count:index<count?index:0;
    for(uint8_t i=0;i<count;i++) {
        uint8_t x=1+(uint16_t)i*126/count;
        uint8_t w=(uint16_t)(i+1)*126/count-(uint16_t)i*126/count-1;
        if(i<completed)Afr2Oled_FillRect(o,x,0,w,7,AFR2_WHITE);
        else {outline(o,x,0,w,7);if(s->sequence_state==3 && i==index && s->sequence_duration_ms) {
            uint8_t fill=(uint32_t)(w>2?w-2:0)*s->sequence_elapsed_ms/s->sequence_duration_ms;
            if(fill)Afr2Oled_FillRect(o,x+1,2,fill,3,AFR2_WHITE);
        }}
    }
    uint8_t kind=s->sequence_kind<AFR2_SEQ_LABEL_COUNT?s->sequence_kind:AFR2_SEQ_WHEELS;
    if(s->sequence_state==4)kind=AFR2_SEQ_FINISH;
    for(uint8_t y=0;y<8;y++){uint8_t bits=pgm_read_byte(&icons[kind][y]);
        for(uint8_t x=0;x<8;x++)if(bits&(0x80u>>x))Afr2Oled_Rect(o,56+2*x,10+2*y,2,2,AFR2_WHITE);}
    char name[19],time[18];strcpy_P(name,names[kind]);
    if(s->sequence_state==5)strcpy_P(name,PSTR("Cancelada"));
    if(s->sequence_state==6)strcpy_P(name,PSTR("Error de ejecucion"));
    Afr2Oled_Text(o,(128-7*strlen(name))/2,28,name,AFR2_WHITE);
    snprintf_P(time,sizeof(time),PSTR("Tiempo %lu.%02lu s"),(unsigned long)(s->sequence_duration_ms/1000),(unsigned long)((s->sequence_duration_ms%1000)/10));
    Afr2Oled_Text(o,0,42,time,AFR2_WHITE);
    snprintf_P(time,sizeof(time),PSTR("%u/%u  > Volver"),index<count?index+1:count,count);
    Afr2Oled_Text(o,0,54,time,AFR2_WHITE);
}
