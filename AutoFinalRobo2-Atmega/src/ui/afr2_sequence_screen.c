/* AFR2-20261024-07. Original AFR2 renderer: only prepared snapshot -> pixels.
 * No hardware reads, I2C calls, delays, commands or dynamic allocation. */
#include "afr2_screens.h"
#include "afr2_sequence_labels.h"
#include "afr2_ui_components.h"
#include "afr2_icons.h"
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
void Afr2SequenceScreen_Render(Afr2Oled *o,const Afr2UiSnapshot *s) {
    uint8_t count=s->sequence_count,index=s->sequence_index;
    char name[19],time[18];strcpy_P(name,PSTR("Secuencia"));
    Afr2Component_Header(o,AFR2_ICON_MOTOR,name,index<count?index+1:count,count);
    uint8_t kind=s->sequence_kind<AFR2_SEQ_LABEL_COUNT?s->sequence_kind:AFR2_SEQ_WHEELS;
    if(s->sequence_state==4)kind=AFR2_SEQ_FINISH;
    /* Keep the canonical step resources, also copied into the Web library. */
    for(uint8_t y=0;y<8;y++)for(uint8_t x=0;x<8;x++)if(pgm_read_byte(&icons[kind][y])&(0x80u>>x))Afr2Oled_Pixel(o,2+x,18+y,AFR2_WHITE);
    strcpy_P(name,names[kind]);if(s->sequence_state==5)strcpy_P(name,PSTR("Cancelada"));if(s->sequence_state==6)strcpy_P(name,PSTR("Error de ejecucion"));
    Afr2Oled_TextWindow(o,14,128,17,name,0,AFR2_WHITE);
    uint32_t percent=s->sequence_state==4?100:s->sequence_duration_ms?(uint32_t)((uint64_t)s->sequence_elapsed_ms*100/s->sequence_duration_ms):0;
    Afr2Component_Progress(o,30,s->sequence_duration_ms>0 || s->sequence_state==4,percent>100?100:percent,s->sensor_now_ms);
    snprintf_P(time,sizeof(time),PSTR("Tiempo %lu.%02lu s"),(unsigned long)(s->sequence_duration_ms/1000),(unsigned long)((s->sequence_duration_ms%1000)/10));
    Afr2Oled_Text(o,0,40,time,AFR2_WHITE);Afr2Ui_GestureHint(o,94,52,1,PSTR("Volver"));
}
