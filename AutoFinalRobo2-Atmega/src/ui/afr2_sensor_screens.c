/* AFR2-20261025-14. IR graph/numeric interaction adapted from kobac F4
 * oled_utils.c / screenWrappers.c / eventManagers.c. Five AFR2 channels only.
 * Radar/servo layouts are new. Integer raster only; no hardware calls. */
#include "afr2_screens.h"
#include <avr/pgmspace.h>
#include <stdio.h>
static const int16_t sine[] PROGMEM={0,87,174,259,342,423,500,574,643,707,766,819,866,906,940,966,985,996,1000,996,985,966,940,906,866,819,766,707,643,574,500,423,342,259,174,87,0};
static int16_t sn(uint8_t i){return pgm_read_word(&sine[i]);}
static int16_t cs(uint8_t i){return i<=18?sn(18-i):-sn(i-18);}
static void line(Afr2Oled *o,int16_t x,int16_t y,int16_t ex,int16_t ey){
    int16_t dx=ex>x?ex-x:x-ex,dy=ey>y?y-ey:ey-y,sx=x<ex?1:-1,sy=y<ey?1:-1,e=dx-dy;
    for(;;){Afr2Oled_Pixel(o,x,y,AFR2_WHITE);if(x==ex && y==ey)break;int16_t e2=2*e;if(e2>-dy){e-=dy;x+=sx;}if(e2<dx){e+=dx;y+=sy;}}
}
static void txt(Afr2Oled *o,uint8_t x,uint8_t y,const char *t){Afr2Oled_Text(o,x,y,t,AFR2_WHITE);}
void Afr2SensorScreen_Render(Afr2Oled *o,uint8_t node,const Afr2UiSnapshot *s){
    const Afr2SensorHead *h=s->head;char text[19];
    if(node==AFR2_N_RADAR_VIEW){
        txt(o,0,0,"Radar 4m");txt(o,84,0,h && h->running?"AUTO":"FIJO");
        for(uint8_t r=1;r<=4;r++)for(uint8_t i=0;i<36;i++){
            int16_t radius=38*r/4;line(o,64+radius*cs(i)/1000,49-radius*sn(i)/1000,64+radius*cs(i+1)/1000,49-radius*sn(i+1)/1000);
        }
        for(uint8_t i=0;i<=36;i+=6)for(uint8_t r=2;r<=38;r+=3)Afr2Oled_Pixel(o,64+r*cs(i)/1000,49-r*sn(i)/1000,AFR2_WHITE);
        txt(o,110,10,"4m");txt(o,110,20,"3m");txt(o,110,30,"2m");txt(o,110,40,"1m");
        if(h){
            uint8_t i=(h->angle+2)/5;if(i>36)i=36;
            if(h->enabled)line(o,64,49,64+38*cs(i)/1000,49-38*sn(i)/1000);
            for(i=0;i<37;i++)if(h->bins[i].id && h->bins[i].error==AFR2_ECHO_OK){
                int16_t radius=(uint32_t)h->bins[i].distance_mm*38u/4000u;
                Afr2Oled_FillRect(o,63+radius*cs(i)/1000,48-radius*sn(i)/1000,3,3,AFR2_WHITE);
            }
            if(h->latest.id && h->latest.error==AFR2_ECHO_OK)snprintf(text,sizeof(text),"1/2 %03u %4umm",h->angle,h->latest.distance_mm);
            else snprintf(text,sizeof(text),"1/2 %03u -- mm",h->angle);
        }else snprintf(text,sizeof(text),"1/2 Sin datos");txt(o,0,53,text);return;
    }
    if(node==AFR2_N_US_VIEW){
        txt(o,0,0,"Distancia 2/2");
        if(h && h->latest.id && h->latest.error==AFR2_ECHO_OK)snprintf(text,sizeof(text),"%u mm",h->latest.distance_mm);else snprintf(text,sizeof(text),"Distancia: --");txt(o,0,14,text);
        if(h){if(h->enabled)snprintf(text,sizeof(text),"Ang %03u PWM %u",h->angle,h->pulse_us);else snprintf(text,sizeof(text),"Ang -- PWM 0");txt(o,0,27,text);snprintf(text,sizeof(text),"Eco %u Edad %lums",h->latest.error,(unsigned long)(s->head->latest.id?(s->sensor_now_ms-s->head->latest.ms):0));txt(o,0,39,text);}
        txt(o,0,53,"Giro / SW volver");return;
    }
    if(node==AFR2_N_SERVO_ANGLE){
        txt(o,0,0,"Angulo del servo");
        if(h){snprintf(text,sizeof(text),"%03u grados %s",h->angle,h->enabled?"PWM":"OFF");txt(o,0,16,text);
            Afr2Oled_Rect(o,3,32,122,8,AFR2_WHITE);uint8_t x=4+(uint32_t)h->angle*118u/180u;Afr2Oled_FillRect(o,x,30,2,12,AFR2_WHITE);
            snprintf(text,sizeof(text),"%u / %u us",h->pulse_us,h->target_us);txt(o,0,43,text);}
        txt(o,0,54,"Giro / SW volver");return;
    }
    txt(o,0,0,node==AFR2_N_LINE_VIEW?"IR linea 1/2":"IR valores 2/2");
    if(node==AFR2_N_LINE_VIEW){
        for(uint8_t i=0;i<5;i++){
            uint8_t x=8+24*i;Afr2Oled_Rect(o,x,14,14,28,AFR2_WHITE);
            if(s->ir_valid){uint16_t max=s->ir_kind==1?1:1023;uint8_t height=(uint32_t)s->ir_raw[i]*24u/max;if(height>24)height=24;Afr2Oled_FillRect(o,x+2,40-height,10,height,AFR2_WHITE);}
            text[0]='1'+i;text[1]=0;txt(o,x+4,43,text);
        }
    }else if(s->ir_valid){
        for(uint8_t row=0;row<3;row++){
            uint8_t i=row*2;
            if(i<4)snprintf(text,sizeof(text),"%u:%4u %u:%4u",i+1,s->ir_raw[i],i+2,s->ir_raw[i+1]);
            else snprintf(text,sizeof(text),"5:%4u %s",s->ir_raw[4],s->line_valid?"CAL":"SIN CAL");
            txt(o,0,14+12*row,text);
        }
    }else txt(o,0,25,"Sin lectura IR");
    txt(o,0,54,"Giro / SW volver");
}
