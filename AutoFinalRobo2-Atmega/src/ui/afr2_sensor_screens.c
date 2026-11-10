/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
/* AFR2-20261025-14. IR graph/numeric interaction adapted from kobac F4
 * oled_utils.c / screenWrappers.c / eventManagers.c. Five AFR2 channels only.
 * Radar/servo layouts are new. Integer raster only; no hardware calls. */
#include "afr2_screens.h"
#include "afr2_ui_components.h"
#include "afr2_icons.h"
#include <avr/pgmspace.h>
#include <stdio.h>
static const int16_t sine[] PROGMEM={0,87,174,259,342,423,500,574,643,707,766,819,866,906,940,966,985,996,1000,996,985,966,940,906,866,819,766,707,643,574,500,423,342,259,174,87,0};
static int16_t sn(uint8_t i){return pgm_read_word(&sine[i]);}
static int16_t cs(uint8_t i){return i<=18?sn(18-i):-sn(i-18);}
/* AFR2-20261029-15: int is16bits on AVR;38*1000 exceeds INT16_MAX. */
static int16_t project(int16_t radius,int16_t trig){return (int32_t)radius*trig/1000;}
static void line(Afr2Oled *o,int16_t x,int16_t y,int16_t ex,int16_t ey){
    /* Positive distances for this Bresenham variant. Negative dy formerly
     * prevented vertical progress: radar stalled main until WDT reset.
     * Bound work to the maximum number of pixels in a valid segment. */
    int16_t dx=ex>x?ex-x:x-ex,dy=ey>y?ey-y:y-ey,sx=x<ex?1:-1,sy=y<ey?1:-1,e=dx-dy;
    uint16_t remaining=(uint16_t)(dx>dy?dx:dy)+1u;
    while(remaining--){Afr2Oled_Pixel(o,x,y,AFR2_WHITE);if(x==ex && y==ey)break;int16_t e2=2*e;if(e2>-dy){e-=dy;x+=sx;}if(e2<dx){e+=dx;y+=sy;}}
}
/* AFR2-20261029-16: half-degree interpolation in the existing flash LUT.
 * Nominal visual cone15degrees total (+/-7.5), not measured beam calibration. */
static int16_t sn_half(uint16_t a){uint8_t i=a/10u,f=a%10u;int16_t v=sn(i);return i<36?v+(sn(i+1)-v)*f/10:v;}
static int16_t cs_half(uint16_t a){return a<=180u?sn_half(180u-a):-sn_half(a-180u);}
/* AFR2-20261029-27: same viewport/range for scan and shared angle editor. */
static uint16_t range_mm(const Afr2UiSnapshot *s){return 2000u>>(s->radar_range<=3?s->radar_range:0);}
static void hit(Afr2Oled *o,uint8_t angle,uint16_t mm,uint16_t range,uint8_t radius,uint8_t cy){
    if(angle>180 || mm<20 || mm>range)return;
    int16_t r=(uint32_t)mm*radius/range;
    int16_t x=64+project(r,cs_half((uint16_t)angle*2u)),y=cy-project(r,sn_half((uint16_t)angle*2u));
    for(int8_t dy=-2;dy<=2;dy++)for(int8_t dx=-2;dx<=2;dx++)
        if(dx*dx+dy*dy<=4)Afr2Oled_Pixel(o,x+dx,y+dy,AFR2_WHITE);
}
#define TXT(x,y,t) Afr2Oled_TextFlash(o,x,y,PSTR(t),AFR2_WHITE)
static void txt(Afr2Oled *o,uint8_t x,uint8_t y,const char *t){Afr2Oled_Text(o,x,y,t,AFR2_WHITE);}
static void small_text(Afr2Oled *o,uint8_t cx,uint8_t y,const char *text);
static void radar(Afr2Oled *o,const Afr2UiSnapshot *s,uint8_t compact){
    const Afr2SensorHead *h=s->head;char text[19],scale[5];
    static const char scales[][5] PROGMEM={"2m","1m","50cm","25cm"};
    uint16_t range=range_mm(s);uint8_t radius=compact?26:38,cy=compact?38:49;
    uint8_t active=h && h->enabled && h->pulse_us>=1000 && h->pulse_us<=2000 && h->angle<=180;
    memcpy_P(scale,scales[s->radar_range<=3?s->radar_range:0],sizeof(scale));
    snprintf_P(text,sizeof(text),compact?PSTR("Servo %s"):PSTR("1/2> R %s"),scale);txt(o,0,0,text);
    if(h && h->running){TXT(80,0,"AUTO");}else if(active){TXT(80,0,"FIJO");}else {TXT(80,0,"OFF");}
    if(!compact){TXT(114,0,"cm");}
    /* Cono nominal de 15 grados, incluso sin PWM; ultimo angulo comandado. */
    uint8_t cone=h && h->angle<=180;
    uint16_t center=cone?(uint16_t)h->angle*2u:180u;
    uint16_t low=center>15u?center-15u:0,high=center+15u;if(high>360u)high=360u;
    for(uint8_t ring=1;ring<=4;ring++){
        int16_t r=(uint16_t)radius*ring/4u;uint16_t step=344u/r;
        for(uint16_t angle=0;angle<=360u;angle+=step)Afr2Oled_Pixel(o,64+project(r,cs_half(angle)),cy-project(r,sn_half(angle)),AFR2_WHITE);
        if(cone)for(uint16_t angle=low;angle<high;){
            uint16_t end=(angle/10u+1u)*10u;if(end>high)end=high;
            line(o,64+project(r,cs_half(angle)),cy-project(r,sn_half(angle)),64+project(r,cs_half(end)),cy-project(r,sn_half(end)));angle=end;
        }
        if(!compact){
            uint16_t mm=(uint32_t)range*ring/4u;
            if(mm%10u)snprintf_P(text,sizeof(text),PSTR("%u.%u"),mm/10u,mm%10u);
            else snprintf_P(text,sizeof(text),PSTR("%u"),mm/10u);
            small_text(o,115,(ring==4?10:45-ring*9),text);
        }
    }
    for(uint8_t i=0;i<=36;i+=6){
        if(cone && (uint16_t)i*10u>=low && (uint16_t)i*10u<=high)line(o,64,cy,64+project(radius,cs(i)),cy-project(radius,sn(i)));
        else for(uint8_t r=2;r<=radius;r+=3)Afr2Oled_Pixel(o,64+project(r,cs(i)),cy-project(r,sn(i)),AFR2_WHITE);
    }
    if(h){
        if(cone){line(o,64,cy,64+project(radius,cs_half(low)),cy-project(radius,sn_half(low)));line(o,64,cy,64+project(radius,cs_half(high)),cy-project(radius,sn_half(high)));}
        if(s->radar_trace)for(uint8_t i=0;i<37;i++)if(h->bins[i].id && h->bins[i].error==AFR2_ECHO_OK)hit(o,i*5u,h->bins[i].distance_mm,range,radius,cy);
        /* AFR2-20261108-02: en Pruebas el servo suele estar apagado (pulse_us 0) y el HC-SR04 igual mide;
         * antes ese eco se descartaba y el radar no graficaba. Se dibuja el ultimo eco valido y reciente
         * (<=1 s) con el angulo registrado (ultimo comando si PWM OFF). En modo rastro los
         * ecos del barrido siguen acumulandose en bins[]; este punto cubre el uso sin barrido. */
        if(h->latest.id && h->latest.error==AFR2_ECHO_OK && (!s->radar_trace || !h->running) && (uint32_t)(s->sensor_now_ms-h->latest.ms)<=1000u){
            uint8_t angle=h->latest.angle<=180?h->latest.angle:(cone?h->angle:90u);
            hit(o,angle,h->latest.distance_mm,range,radius,cy);
        }
    }
}
/* Reuse the existing glyph bytes, dropping empty digit side columns/rows.
 * Four ADC digits occupy23px inside each24px sensor column; no new font RAM. */
static void small_text(Afr2Oled *o,uint8_t cx,uint8_t y,const char *text){
    uint8_t length=0;while(text[length])++length;
    int16_t x=(int16_t)cx-(length*6u-1u)/2u;
    for(uint8_t i=0;i<length;i++)for(uint8_t row=0;row<8;row++){
        uint8_t bits=Afr2Font7x10_Row(text[i],row);
        for(uint8_t col=0;col<5;col++)if(bits&(0x40u>>col))Afr2Oled_Pixel(o,x+6*i+col,y+row,AFR2_WHITE);
    }
}
static void small_number(Afr2Oled *o,uint8_t cx,uint8_t y,uint16_t value){
    char text[5];snprintf_P(text,sizeof(text),PSTR("%u"),value);small_text(o,cx,y,text);
}
void Afr2SensorScreen_Render(Afr2Oled *o,uint8_t node,const Afr2UiSnapshot *s){
    const Afr2SensorHead *h=s->head;char text[19];
    if(node==AFR2_N_RADAR_VIEW || node==AFR2_N_SERVO_ANGLE){
        uint8_t compact=node==AFR2_N_SERVO_ANGLE;radar(o,s,compact);
        if(compact){
            Afr2Oled_Rect(o,3,44,122,6,AFR2_WHITE);
            if(h){Afr2Oled_FillRect(o,4+(uint32_t)h->angle*118u/180u,42,2,10,AFR2_WHITE);snprintf_P(text,sizeof(text),PSTR("%03u %u/%uus"),h->angle,h->pulse_us,h->target_us);txt(o,0,53,text);}
            else {TXT(0,53,"Sin datos PWM");}
        }else if(h){
            /* AFR2-20261109-01: value refreshes with each echo and shows -- when older than 1 s. */
            uint8_t fresh=h->latest.id && h->latest.error==AFR2_ECHO_OK && (uint32_t)(s->sensor_now_ms-h->latest.ms)<=1000u;
            if(fresh)snprintf_P(text,sizeof(text),PSTR("%03u %4umm%s"),h->angle,h->latest.distance_mm,h->latest.distance_mm>range_mm(s)?" >esc":"");
            else snprintf_P(text,sizeof(text),PSTR("%03u -- mm"),h->angle);
            txt(o,0,53,text);
        }else {TXT(0,53,"Sin datos");}return;
    }
    if(node==AFR2_N_US_VIEW){
        Afr2Component_HeaderFlash(o,AFR2_ICON_RADAR,PSTR("Distancia"),2,2);
        if(h && h->latest.id && (uint32_t)(s->sensor_now_ms-h->latest.ms)<=500u && h->latest.error==AFR2_ECHO_OK)snprintf_P(text,sizeof(text),PSTR("%u mm"),h->latest.distance_mm);else snprintf_P(text,sizeof(text),PSTR("Distancia: --"));txt(o,0,16,text);
        if(h){static const char errors[8][18] PROGMEM={"Eco valido","Sin eco (timeout)","ECHO no termina","ECHO ocupado","Fuera de rango","Servo moviendose","Lectura vencida","Sin medicion"};snprintf_P(text,sizeof(text),errors[!h->latest.id?7:(uint32_t)(s->sensor_now_ms-h->latest.ms)>500u?6:h->latest.error<8?h->latest.error:7]);txt(o,0,28,text);
            /* AFR2-20261109-01: where the servo points (it starts centred at 90) and the echo age. */
            uint8_t servo=h->enabled && h->pulse_us>=1000 && h->pulse_us<=2000;
            if(servo && h->latest.id)snprintf_P(text,sizeof(text),PSTR("Servo %03u %lums"),h->angle,(unsigned long)(s->sensor_now_ms-h->latest.ms));
            else if(servo)snprintf_P(text,sizeof(text),PSTR("Servo %03u"),h->angle);
            else if(h->latest.id)snprintf_P(text,sizeof(text),PSTR("Servo OFF %lums"),(unsigned long)(s->sensor_now_ms-h->latest.ms));
            else snprintf_P(text,sizeof(text),PSTR("Servo OFF"));
            txt(o,0,40,text);}
        Afr2Ui_GestureHint(o,0,53,0,PSTR("Opciones"));return;
    }
    Afr2Component_HeaderFlash(o,AFR2_ICON_SENSORS,PSTR("Sensores IR"),node==AFR2_N_LINE_VIEW?1:2,2);
    if(!s->ir_valid){if(s->ir_kind){TXT(0,25,"Esperando IR");}else {TXT(0,25,"IR deshabilitado");}Afr2Ui_GestureHint(o,0,53,1,PSTR("Volver"));return;}
    if(node==AFR2_N_LINE_VIEW){
        Afr2Component_Panel(o,0,15,128,29);
        for(uint8_t i=0;i<5;i++){
            uint8_t center=15+24*i;Afr2Component_Mini(o,center-1,17,i+1);
            Afr2Oled_Rect(o,center-6,23,12,12,AFR2_WHITE);
            uint8_t height=(s->line_bits&(1u<<i))?8:0;
            Afr2Oled_FillRect(o,center-4,33-height,8,height,AFR2_WHITE);small_number(o,center,36,s->ir_raw[i]);
        }
    }else for(uint8_t i=0;i<5;i++){
        snprintf_P(text,sizeof(text),PSTR("%u:%4u"),i+1,s->ir_raw[i]);
        if(i<4)txt(o,(i%2u)*64u,16+(i/2u)*12u,text);
        else txt(o,(128u-6u*7u)/2u,40,text);
    }
    Afr2Ui_GestureHint(o,0,53,1,PSTR("Volver"));
}
