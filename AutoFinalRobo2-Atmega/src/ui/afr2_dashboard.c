/* AFR2-20261102-14. Original header + idle status card; paired TS renderer. */
#include "afr2_screens.h"
#include "afr2_icons.h"
#include "afr2_ui_components.h"
#include <avr/pgmspace.h>
#include <stdio.h>
#include <string.h>
enum { DASH_NONE,DASH_RADAR,DASH_SERVO,DASH_MOTOR_OFF,DASH_MOTOR_ON,DASH_LINE,DASH_TRACE,DASH_FAULT };
static const char mode_names[8][9] PROGMEM={"REPOSO","MANUAL","LINEA","RUEDA","GOTO","FALLO","SECUENC","PWM"};
static const char status_names[8][7] PROGMEM={"--","ON","ON","OFF","RUN","ON","RASTRO","FALLO"};
static const uint8_t status_icons[8] PROGMEM={AFR2_ICON_INFO,AFR2_ICON_RADAR,AFR2_ICON_SERVO,AFR2_ICON_MOTOR,AFR2_ICON_MOTOR,AFR2_ICON_SENSORS,AFR2_ICON_RADAR,AFR2_ICON_WARNING};
static uint16_t offset(const char *text,uint32_t elapsed){
    uint16_t pixels=strlen(text)*7u;if(pixels<=70)return 0;
    uint16_t end=pixels-70;uint32_t travel=(uint32_t)end*40u,phase=elapsed%(1600u+2u*travel);
    if(phase<800)return 0;
    phase-=800;
    if(phase<travel)return phase/40u;
    phase-=travel;
    return phase<800?end:end-(phase-800u)/40u;
}
static uint8_t first(uint8_t mask){for(uint8_t i=1;i<=6;i++)if(mask&(1u<<(i-1)))return i;return DASH_NONE;}
static uint8_t status(Afr2Ui *u,const Afr2UiSnapshot *s,uint8_t full){
    Afr2DashboardState *d=&u->dashboard;uint8_t mask=0;
    if(s->dashboard_valid){
        if(s->quick_flags&1)mask|=1u<<(DASH_RADAR-1);
        if(s->quick_flags&2)mask|=1u<<(DASH_SERVO-1);
        if(!(s->quick_flags&4))mask|=1u<<(DASH_MOTOR_OFF-1);
        else if(s->dashboard_moving)mask|=1u<<(DASH_MOTOR_ON-1);
        if(s->quick_flags&8)mask|=1u<<(DASH_LINE-1);
        if((s->quick_flags&17)==17)mask|=1u<<(DASH_TRACE-1);
    }
    if(full){d->input_ms=u->clock_ms;d->rotate_ms=u->clock_ms;d->armed=0;d->selected=first(mask);}
    if(s->dashboard_valid && s->dashboard_fault){d->selected=DASH_FAULT;d->rotate_ms=u->clock_ms;d->armed=1;}
    else {
        if(d->selected>DASH_TRACE || !d->selected || !(mask&(1u<<(d->selected-1)))){
            d->selected=first(mask);if(!full){d->rotate_ms=u->clock_ms;d->armed=1;}
        }
        if(!full && d->mask!=mask){d->rotate_ms=u->clock_ms;d->armed=1;}
        if(d->held || (uint32_t)(u->clock_ms-d->input_ms)<AFR2_DASHBOARD_IDLE_MS)d->armed=0;
        else if(mask && (mask&(mask-1)) && (!d->armed || (uint32_t)(u->clock_ms-d->rotate_ms)>=AFR2_DASHBOARD_PAGE_MS)){
            for(uint8_t n=1;n<=6;n++){uint8_t next=(d->selected-1+n)%6+1;if(mask&(1u<<(next-1))){d->selected=next;break;}}
            d->rotate_ms=u->clock_ms;d->armed=1;
        }
    }
    d->mask=mask;return d->selected;
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
    uint8_t flags=(!!s->network_valid) | (!!s->network_sta<<1) | (!!s->network_ap<<2) | (!!s->battery_valid<<3);
    uint8_t header=full || d->header_flags!=flags || d->network_boot!=s->network_boot || d->network_revision!=s->network_revision || d->battery_mv!=s->battery_mv;
    if(full)Afr2Oled_Clear(o);
    if(header){
        Afr2Oled_FillRect(o,0,0,128,26,AFR2_BLACK);
        Afr2Icon_Draw(o,!s->network_valid?AFR2_ICON_WIFI:!connected?AFR2_ICON_WIFI_OFF:s->network_sta?AFR2_ICON_WIFI:AFR2_ICON_AP,0,3,18);
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
    uint8_t selected=status(u,s,full);
    if(full){Afr2Component_RoundedPanel(o,62,27,64,16);Afr2Ui_GestureHint(o,0,52,0,PSTR("Menu"));Afr2Oled_TextFlash(o,54,52,PSTR("Rapidas"),AFR2_WHITE);}
    if(full || d->drawn!=selected){
        char name[7];strcpy_P(name,status_names[selected]);uint8_t left=63+(62-(14+4+strlen(name)*7))/2;
        Afr2Oled_FillRect(o,64,28,60,14,AFR2_BLACK);Afr2Oled_FillRect(o,63,29,62,12,AFR2_BLACK);Afr2Icon_Draw(o,pgm_read_byte(&status_icons[selected]),left,28,14);Afr2Oled_Text(o,left+18,30,name,AFR2_WHITE);changed=1;
    }
    d->header_flags=flags;d->network_boot=s->network_boot;d->network_revision=s->network_revision;d->battery_mv=s->battery_mv;
    d->ssid_offset=ss;d->ip_offset=is;d->mode=mode;d->drawn=selected;d->valid=1;
    return changed;
}
