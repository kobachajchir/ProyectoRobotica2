/* AFR2-20261102-14. Original header + idle status card; paired TS renderer. */
#include "afr2_screens.h"
#include "afr2_icons.h"
#include "afr2_ui_components.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
/* AFR2-20261108-12: the card walks every system in Controles rapidos order, icon + state, one page
 * every 4 s after 3 s idle; a change jumps to that system at once; a fault overrides everything. */
enum { DASH_NONE,DASH_MOTOR,DASH_US,DASH_RADAR,DASH_SERVO,DASH_LINE,DASH_TRACE,DASH_FAULT };
static const char mode_names[8][9] PROGMEM={"REPOSO","MANUAL","LINEA","RUEDA","GOTO","FALLO","SECUENC","PWM"};
/* [item][0 off,1 on,2 run]; max 6 characters (card text area 46 px). US/IR/RS share an icon. */
static const char status_names[8][3][7] PROGMEM={
    {"--","--","--"},{"OFF","ON","RUN"},{"US OFF","US ON","US ON"},{"OFF","ON","ON"},
    {"OFF","ON","ON"},{"IR OFF","IR ON","IR ON"},{"RS OFF","RS ON","RS ON"},{"FALLO","FALLO","FALLO"}};
static const uint8_t status_icons[8] PROGMEM={AFR2_ICON_INFO,AFR2_ICON_MOTOR,AFR2_ICON_SENSORS,AFR2_ICON_RADAR,AFR2_ICON_SERVO,AFR2_ICON_SENSORS,AFR2_ICON_RADAR,AFR2_ICON_WARNING};
/* 0 off, 1 on, 2 run (motors enabled and moving). bits: quick_flags>>1, bit6 = moving. */
static uint8_t item_state(uint8_t item,uint8_t bits){
    if(item<DASH_MOTOR || item>DASH_TRACE)return 0;
    if(!(bits&(1u<<(item-1))))return 0;
    return item==DASH_MOTOR && (bits&0x40u)?2:1;
}
static uint16_t offset(const char *text,uint32_t elapsed){
    uint16_t pixels=strlen(text)*7u;if(pixels<=70)return 0;
    uint16_t end=pixels-70;uint32_t travel=(uint32_t)end*40u,phase=elapsed%(1600u+2u*travel);
    if(phase<800)return 0;
    phase-=800;
    if(phase<travel)return phase/40u;
    phase-=travel;
    return phase<800?end:end-(phase-800u)/40u;
}
static uint8_t status(Afr2Ui *u,const Afr2UiSnapshot *s,uint8_t full){
    Afr2DashboardState *d=&u->dashboard;uint8_t bits=0;
    if(s->dashboard_valid){bits=(uint8_t)((s->quick_flags>>1)&0x3fu);if((bits&1u) && s->dashboard_moving)bits|=0x40u;}
    if(full){d->input_ms=u->clock_ms;d->rotate_ms=u->clock_ms;d->armed=0;d->selected=DASH_MOTOR;}
    if(!s->dashboard_valid)d->selected=DASH_NONE;
    else if(s->dashboard_fault){d->selected=DASH_FAULT;d->rotate_ms=u->clock_ms;d->armed=1;}
    else {
        if(d->selected<DASH_MOTOR || d->selected>DASH_TRACE)d->selected=DASH_MOTOR;
        uint8_t diff=(uint8_t)((d->mask^bits)&0x3fu);if((d->mask^bits)&0x40u)diff|=1u;
        if(!full && diff){   /* show what just changed, then keep walking from there */
            for(uint8_t i=0;i<6;i++)if(diff&(1u<<i)){d->selected=i+1u;break;}
            d->rotate_ms=u->clock_ms;d->armed=1;
        } else if(d->held || (uint32_t)(u->clock_ms-d->input_ms)<AFR2_DASHBOARD_IDLE_MS)d->armed=0;
        else if(!d->armed || (uint32_t)(u->clock_ms-d->rotate_ms)>=AFR2_DASHBOARD_PAGE_MS){
            d->selected=(uint8_t)(d->selected%DASH_TRACE+1u);d->rotate_ms=u->clock_ms;d->armed=1;
        }
    }
    d->mask=bits;return d->selected;
}
static void lane(Afr2Oled *o,uint8_t y,const char *text,uint16_t shift){
    Afr2Oled_FillRect(o,21,y,70,10,AFR2_BLACK);Afr2Oled_TextWindow(o,21,91,y,text,shift,AFR2_WHITE);
    Afr2Oled_FillRect(o,21,y,70,10,AFR2_INVERT);
}
uint8_t Afr2Dashboard_Render(Afr2Ui *u,Afr2Oled *o,const Afr2UiSnapshot *s){
    Afr2DashboardState *d=&u->dashboard;uint8_t full=!d->valid,changed=full;
    uint8_t connected=s->network_valid && (s->network_sta || s->network_ap);
    const char *ssid=connected?(s->network_sta?s->network_ssid:s->network_ap_ssid):NULL;
    const char *ip=connected?(s->network_sta?s->network_ip:s->network_ap_ip):NULL;
    char fallback[12];strcpy_P(fallback,s->network_valid?PSTR("Sin Wi-Fi"):PSTR("Red: --"));
    if(!ssid || !*ssid){if(connected)strcpy_P(fallback,s->network_sta?PSTR("Wi-Fi"):PSTR("AP"));ssid=fallback;}
    if(!ip || !*ip)ip="IP: --";
    uint16_t ss=offset(ssid,u->clock_ms-u->scroll_started_ms),is=offset(ip,u->clock_ms-u->scroll_started_ms);
    uint8_t flags=(!!s->network_valid) | (!!s->network_sta<<1) | (!!s->network_ap<<2) | (!!s->battery_valid<<3) | ((s->network_mode==2)<<4);
    uint8_t header=full || d->header_flags!=flags || d->network_boot!=s->network_boot || d->network_revision!=s->network_revision || d->battery_mv!=s->battery_mv;
    if(full)Afr2Oled_Clear(o);
    if(header){
        Afr2Oled_FillRect(o,0,0,128,26,AFR2_BLACK);
        Afr2Icon_Draw(o,!s->network_valid?AFR2_ICON_WIFI:s->network_mode==2?AFR2_ICON_AP:!connected?AFR2_ICON_WIFI_OFF:s->network_sta?AFR2_ICON_WIFI:AFR2_ICON_AP,0,3,18);
        uint16_t mv=s->battery_mv;uint8_t battery=s->battery_valid?AFR2_ICON_BATTERY_EMPTY+(mv==0?0:mv<6000?1:mv<7000?2:mv<8000?3:4):AFR2_ICON_BATTERY_UNKNOWN;
        char voltage[8];if(s->battery_valid)snprintf_P(voltage,sizeof(voltage),PSTR("%u.%uV"),mv/1000u,mv%1000u/100u);else strcpy_P(voltage,PSTR("--V"));
        /* Compact icon keeps the voltage lane clear, as in the accepted render. */
        Afr2Icon_Draw(o,battery,110,0,16);Afr2Oled_Text(o,128-strlen(voltage)*7,15,voltage,AFR2_WHITE);
        Afr2Oled_FillRect(o,0,0,128,26,AFR2_INVERT);changed=1;
    }
    if(header || d->ssid_offset!=ss){lane(o,2,ssid,ss);changed=1;}
    if(header || d->ip_offset!=is){lane(o,14,ip,is);changed=1;}
    uint8_t mode=s->dashboard_valid && s->dashboard_mode<8?s->dashboard_mode:255;
    if(full || d->mode!=mode){
        char name[9];if(mode<8)strcpy_P(name,mode_names[mode]);else strcpy_P(name,PSTR("--"));
        Afr2Oled_FillRect(o,0,27,60,23,AFR2_BLACK);Afr2Oled_TextFlash(o,2,28,PSTR("MODO"),AFR2_WHITE);Afr2Oled_Text(o,2,40,name,AFR2_WHITE);changed=1;
    }
    uint8_t selected=status(u,s,full),shown=(uint8_t)(selected|(item_state(selected,d->mask)<<3));
    /* AFR2-20261108-10: vuelve el icono de largo junto a "Rapidas" (11 px, como el del progreso). Texto hasta x=109:
     * la esquina 106..127 sigue reservada al anillo de progreso, que solo se dibuja mientras se mantiene. */
    if(full){Afr2Component_RoundedPanel(o,62,27,64,16);Afr2Ui_GestureHint(o,0,52,0,PSTR("Menu"));
        Afr2Icon_Draw(o,AFR2_ICON_ENCODER_LONG,47,51,11);Afr2Oled_TextFlash(o,60,52,PSTR("Rapidas"),AFR2_WHITE);}
    if(full || d->drawn!=shown){
        char name[7];strcpy_P(name,status_names[selected][item_state(selected,d->mask)]);uint8_t left=63+(62-(14+4+strlen(name)*7))/2;
        Afr2Oled_FillRect(o,64,28,60,14,AFR2_BLACK);Afr2Oled_FillRect(o,63,29,62,12,AFR2_BLACK);Afr2Icon_Draw(o,pgm_read_byte(&status_icons[selected]),left,28,14);Afr2Oled_Text(o,left+18,30,name,AFR2_WHITE);changed=1;
    }
    d->header_flags=flags;d->network_boot=s->network_boot;d->network_revision=s->network_revision;d->battery_mv=s->battery_mv;
    d->ssid_offset=ss;d->ip_offset=is;d->mode=mode;d->drawn=shown;d->valid=1;
    return changed;
}
