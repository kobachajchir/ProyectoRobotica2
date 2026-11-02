/* AFR2-20261102-05. Shared OLED128-5 components; C/TS paired layouts. */
#include "afr2_ui_components.h"
#include "afr2_icons.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
void Afr2Component_Header(Afr2Oled *o,uint8_t icon,const char *title,uint8_t page,uint8_t count){
    char counter[8];uint8_t right=126;
    Afr2Oled_FillRect(o,0,0,128,14,AFR2_BLACK);Afr2Icon_Draw(o,icon,2,0,14);
    if(count){snprintf_P(counter,sizeof(counter),PSTR("%u/%u"),page,count);right=126-strlen(counter)*7;Afr2Oled_Text(o,right,2,counter,AFR2_WHITE);right-=4;}
    Afr2Oled_TextWindow(o,20,right,2,title,0,AFR2_WHITE);Afr2Oled_FillRect(o,0,0,128,14,AFR2_INVERT);
}
void Afr2Component_Panel(Afr2Oled *o,uint8_t x,uint8_t y,uint8_t w,uint8_t h){
    for(uint8_t r=0;r<h;r++){uint8_t edge=r<h-1-r?r:h-1-r,inset=edge<2?2-edge:0;
        if(!r || r==h-1)Afr2Oled_FillRect(o,x+inset,y+r,w-2*inset,1,AFR2_WHITE);
        else {Afr2Oled_Pixel(o,x+inset,y+r,AFR2_WHITE);Afr2Oled_Pixel(o,x+w-inset-1,y+r,AFR2_WHITE);}}
}
void Afr2Component_Progress(Afr2Oled *o,uint8_t y,uint8_t known,uint8_t percent,uint32_t clock){
    Afr2Oled_FillRect(o,2,y,124,8,AFR2_BLACK);Afr2Oled_Rect(o,2,y,124,8,AFR2_WHITE);
    uint8_t width=known?(uint16_t)(percent>100?100:percent)*120/100:30;
    Afr2Oled_FillRect(o,known?4:4+(clock/40)%91,y+2,width,4,AFR2_WHITE);
}
void Afr2Component_Mini(Afr2Oled *o,uint8_t x,uint8_t y,uint16_t value){
    static const uint8_t digits[10][5] PROGMEM={{7,5,5,5,7},{2,6,2,2,7},{7,1,7,4,7},{7,1,7,1,7},{5,5,7,1,1},{7,4,7,1,7},{7,4,7,5,7},{7,1,1,1,1},{7,5,7,5,7},{7,5,7,1,7}};
    char text[6];snprintf_P(text,sizeof(text),PSTR("%u"),value);
    for(uint8_t i=0;text[i];i++)for(uint8_t r=0;r<5;r++){uint8_t bits=pgm_read_byte(&digits[text[i]-'0'][r]);for(uint8_t c=0;c<3;c++)if(bits&(4>>c))Afr2Oled_Pixel(o,x+4*i+c,y+r,AFR2_WHITE);}
}

void Afr2Component_Metric(Afr2Oled *o,uint8_t x,uint8_t y,const char *text){
    static const uint8_t digits[12][7] PROGMEM={{14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},{2,6,10,18,31,2,2},{31,16,16,30,1,1,30},{14,16,16,30,17,17,14},{31,1,2,4,8,8,8},{14,17,17,14,17,17,14},{14,17,17,15,1,1,14},{0,0,0,31,0,0,0},{0,0,0,0,0,12,12}};
    for(uint8_t i=0;text[i];i++){uint8_t d=text[i]=='-'?10:text[i]=='.'?11:text[i]-'0';if(d>11)continue;
        for(uint8_t r=0;r<7;r++){uint8_t bits=pgm_read_byte(&digits[d][r]);for(uint8_t c=0;c<5;c++)if(bits&(16>>c))Afr2Oled_FillRect(o,x+12*i+2*c,y+2*r,2,2,AFR2_WHITE);}}
}
