/* AFR2-20261024-08. Pure framebuffer layouts; F4 icon/title/detail hierarchy.
 * Strings are borrowed snapshot data, stable for this main-loop render only. */
#include "afr2_screens.h"
#include "afr2_icons.h"
#include "afr2_ui_components.h"
#include <avr/pgmspace.h>
#include <string.h>
#define TXT(o,x,y,s) Afr2Oled_TextFlash(o,x,y,PSTR(s),AFR2_WHITE)
static void line(Afr2Oled *o,uint8_t y,const char *text,uint32_t elapsed) {
    Afr2Oled_FillRect(o,0,y,128,10,AFR2_BLACK);
    uint8_t right=y>=40?104:128;Afr2Oled_TextWindow(o,0,right,y,text,Afr2Ui_TextOffset(text,right,elapsed),AFR2_WHITE);
}
static void line_flash(Afr2Oled *o,uint8_t y,const char *text,uint32_t elapsed) {
    Afr2Oled_FillRect(o,0,y,128,10,AFR2_BLACK);
    uint8_t right=y>=40?104:128;Afr2Oled_TextWindowFlash(o,0,right,y,text,Afr2Ui_TextOffsetFlash(text,right,elapsed),AFR2_WHITE);
}
static void labeled(Afr2Oled *o,uint8_t y,const char *label,uint8_t left,const char *text,uint32_t elapsed) {
    Afr2Oled_FillRect(o,0,y,128,10,AFR2_BLACK);
    Afr2Oled_TextFlash(o,0,y,label,AFR2_WHITE);
    uint8_t right=y>=40?104:128;Afr2Oled_TextWindow(o,left,right,y,text,Afr2Ui_TextOffset(text,right-left,elapsed),AFR2_WHITE);
}
static void back(Afr2Oled *o) {
    Afr2Ui_GestureHint(o,94,52,1,PSTR("Volver"));
}
void Afr2NetworkScreen_Render(Afr2Oled *o,const Afr2Ui *u,const Afr2UiSnapshot *s) {
    if(u->node==AFR2_N_NETWORK){
        Afr2Oled_Clear(o);Afr2Component_HeaderFlash(o,AFR2_ICON_WIFI,PSTR("Conexion"),u->selection+1,2);
        for(uint8_t i=0;i<3;i++)line(o,16+12*i,s->lines[i],u->clock_ms-u->scroll_started_ms);
        Afr2Ui_GestureHint(o,0,53,0,PSTR("Volver"));Afr2Ui_GestureHint(o,66,53,1,PSTR("Volver"));return;
    }
    uint8_t ap=u->node==AFR2_N_AP_INFO;uint32_t elapsed=u->clock_ms-u->scroll_started_ms;
    Afr2Component_HeaderFlash(o,ap?AFR2_ICON_AP:AFR2_ICON_WIFI,ap?PSTR("Datos de red AP"):PSTR("IP de conexion"),0,0);
    if(!s->network_valid){line_flash(o,17,PSTR("Sin datos actuales"),elapsed);line_flash(o,32,PSTR("Actualizar Wi-Fi"),elapsed);}
    else if(ap) {
        if(!s->network_ap){line_flash(o,17,PSTR("AP no esta activo"),elapsed);line_flash(o,32,PSTR("IP: --"),elapsed);}
        else {labeled(o,16,PSTR("SSID:"),35,s->network_ap_ssid,elapsed);labeled(o,28,PSTR("Clave:"),42,s->network_password[0]?s->network_password:"Sin clave",elapsed);labeled(o,40,PSTR("IP:"),21,s->network_ap_ip,elapsed);}
    } else if(s->network_sta) {
        labeled(o,16,PSTR("SSID:"),35,s->network_ssid,elapsed);labeled(o,29,PSTR("IP:"),21,s->network_ip,elapsed);line_flash(o,41,PSTR("STA / abrir en Web"),elapsed);
    } else if(s->network_ap) {
        labeled(o,16,PSTR("SSID:"),35,s->network_ap_ssid,elapsed);labeled(o,29,PSTR("IP:"),21,s->network_ap_ip,elapsed);line_flash(o,41,PSTR("AP / abrir en Web"),elapsed);
    } else {line_flash(o,17,PSTR("Sin IP adquirida"),elapsed);line_flash(o,32,PSTR("Esperando conexion"),elapsed);}
    back(o);
}
void Afr2NetworkNotice_Render(Afr2Oled *o,const Afr2Ui *u,const Afr2UiSnapshot *s) {
    const Afr2Notification *n=&u->notification[u->tail];uint8_t kind=n->kind-AFR2_NOTICE_NETWORK;
    uint32_t elapsed=u->clock_ms-u->notice_started_ms;uint8_t icon=AFR2_ICON_INFO;
    if(kind==AFR2_NET_GOT_IP || kind==AFR2_NET_STA_LOST)icon=AFR2_ICON_WIFI;
    else if(kind==AFR2_NET_AP_READY || kind==AFR2_NET_AP_STOPPED)icon=AFR2_ICON_AP;
    else if(kind==AFR2_NET_CLIENT_JOIN || kind==AFR2_NET_CLIENT_LEAVE || kind==AFR2_NET_WS_OPEN || kind==AFR2_NET_WS_CLOSE)icon=AFR2_ICON_LINK;
    Afr2Icon_Draw(o,icon,0,6,16);
    switch(kind) {
    case AFR2_NET_BOOT:TXT(o,24,9,"ESP iniciada");line_flash(o,31,PSTR("Firmware propio"),elapsed);break;
    case AFR2_NET_WEB_READY:TXT(o,24,9,"Servidor listo");line_flash(o,31,PSTR("HTTP disponible"),elapsed);break;
    case AFR2_NET_GOT_IP:TXT(o,24,9,"IP adquirida");line(o,28,s->network_valid?s->network_ssid:"Wi-Fi",elapsed);line(o,41,n->text,elapsed);break;
    case AFR2_NET_AP_READY:
        TXT(o,24,8,"Red AP creada");
        if(s->network_valid && s->network_ap){labeled(o,23,PSTR("SSID:"),35,s->network_ap_ssid,elapsed);labeled(o,34,PSTR("Clave:"),42,s->network_password[0]?s->network_password:"Sin clave",elapsed);labeled(o,45,PSTR("IP:"),21,s->network_ap_ip,elapsed);}
        else line_flash(o,31,PSTR("Consultar datos AP"),elapsed);
        break;
    case AFR2_NET_CLIENT_JOIN:TXT(o,24,9,"Cliente entro");line(o,32,n->text,elapsed);break;
    case AFR2_NET_CLIENT_LEAVE:TXT(o,24,9,"Cliente salio");line(o,32,n->text,elapsed);break;
    case AFR2_NET_WS_OPEN:TXT(o,24,9,"WebSocket OK");line_flash(o,32,PSTR("Cliente conectado"),elapsed);break;
    case AFR2_NET_WS_CLOSE:TXT(o,24,9,"WebSocket cerro");line_flash(o,32,PSTR("Cliente salio"),elapsed);break;
    case AFR2_NET_WEB_SERVED:TXT(o,24,9,"Web servida");line_flash(o,32,PSTR("HTTP 200 iniciado"),elapsed);break;
    case AFR2_NET_STA_LOST:TXT(o,24,9,"Wi-Fi sin red");line_flash(o,32,PSTR("STA desconectada"),elapsed);break;
    case AFR2_NET_AP_STOPPED:TXT(o,24,9,"Red AP apagada");line_flash(o,32,PSTR("Sin acceso por AP"),elapsed);break;
    }
    if(kind!=AFR2_NET_AP_READY){Afr2Ui_GestureHint(o,0,53,0,PSTR("Cerrar"));Afr2Ui_GestureHint(o,66,53,1,PSTR("Cerrar"));}
}
