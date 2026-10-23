/* Wrappers/contexto/render separados, derivados de arquitectura de screenWrappers.c,
 * screen_context.h y oled_utils.c F4. Navegacion propia: largo=menu, atras=item.
 */
#include "afr2_screens.h"
#include <stdio.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#define LIT(s) PSTR(s)
#else
#define LIT(s) (s)
#endif
#define TEXT(o,x,y,s) Afr2Oled_TextFlash(o,x,y,LIT(s),AFR2_WHITE)
void Afr2Ui_Init(Afr2Ui *ui){if(ui){ui->screen=AFR2_SCREEN_HOME;ui->selection=0;ui->redraw=true;}}
void Afr2Ui_Apply(Afr2Ui *ui,Afr2UiInput input) {
    if(!ui)return;
    if(input.menu){ui->screen=AFR2_SCREEN_MENU;ui->selection=0;ui->redraw=true;return;}
    if(ui->screen==AFR2_SCREEN_MENU) {
        if(input.rotation) {
            int16_t n=(int16_t)ui->selection+input.rotation;
            while(n<0)n+=3;
            ui->selection=(uint8_t)(n%3);ui->redraw=true;
        }
        if(input.click){ui->screen=ui->selection==0?AFR2_SCREEN_SENSORS:
            ui->selection==1?AFR2_SCREEN_DISPLAY:AFR2_SCREEN_HOME;ui->redraw=true;}
    } else if((ui->screen==AFR2_SCREEN_SENSORS || ui->screen==AFR2_SCREEN_DISPLAY) && input.click) {
        /* Unico item seleccionable de estas vistas: Volver. */
        ui->screen=AFR2_SCREEN_MENU;ui->redraw=true;
    }
}
static void render_home(Afr2Oled *o) {
    TEXT(o,0,0,"AutoFinalRobo2");TEXT(o,0,18,"Base de interfaz");
    TEXT(o,0,36,"Sin control activo");TEXT(o,0,52,"SW largo: menu");
}
static void render_menu(Afr2Oled *o,uint8_t selection) {
    TEXT(o,0,0,"Menu");TEXT(o,10,18,"Sensores");TEXT(o,10,32,"Estado OLED");TEXT(o,10,46,"Volver");
    TEXT(o,0,(int16_t)(18+14*selection),">");
}
static void render_sensors(Afr2Oled *o,const Afr2UiSnapshot *s) {
    char line[19];TEXT(o,0,0,"Sensores");
    if(s->line_valid){
        for(uint8_t i=0;i<5;i++)line[i]=(s->line_bits&(1u<<i))?'1':'0';
        line[5]=0;TEXT(o,0,18,"IR:");Afr2Oled_Text(o,28,18,line,AFR2_WHITE);
    }else TEXT(o,0,18,"IR: --");
    if(s->ultrasonic_valid){snprintf(line,sizeof(line),"US: %u mm",(unsigned)s->ultrasonic_mm);Afr2Oled_Text(o,0,32,line,AFR2_WHITE);}
    else TEXT(o,0,32,"US: --");
    TEXT(o,0,50,"> Volver");
}
static void render_display(Afr2Oled *o,const Afr2UiSnapshot *s) {
    char line[19];TEXT(o,0,0,"Estado OLED");
    if(s->oled_ready)TEXT(o,0,18,"Inicializado");else TEXT(o,0,18,"No disponible");
    snprintf(line,sizeof(line),"Errores: %lu",(unsigned long)s->oled_errors);Afr2Oled_Text(o,0,32,line,AFR2_WHITE);
    TEXT(o,0,50,"> Volver");
}
void Afr2Ui_Render(Afr2Ui *ui,Afr2Oled *o,const Afr2UiSnapshot *s) {
    if(!ui || !o || !s)return;
    Afr2Oled_Clear(o);
    switch(ui->screen){
      case AFR2_SCREEN_HOME:render_home(o);break;
      case AFR2_SCREEN_MENU:render_menu(o,ui->selection);break;
      case AFR2_SCREEN_SENSORS:render_sensors(o,s);break;
      case AFR2_SCREEN_DISPLAY:render_display(o,s);break;
      default:TEXT(o,0,0,"Pantalla invalida");TEXT(o,0,50,"SW largo: menu");break;
    }
    ui->redraw=false;
}
