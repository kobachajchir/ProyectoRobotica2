/* AFR2-20261024-08. Pure framebuffer layouts; F4 icon/title/detail hierarchy.
 * Strings are borrowed snapshot data, stable for this main-loop render only. */
#include "afr2_screens.h"
#include "afr2_icons.h"
#include <avr/pgmspace.h>
#include <string.h>
#define TXT(o,x,y,s) Afr2Oled_TextFlash(o,x,y,PSTR(s),AFR2_WHITE)
static void line(Afr2Oled *o,uint8_t y,const char *text,uint32_t elapsed) {
    Afr2Oled_FillRect(o,0,y,128,10,AFR2_BLACK);
    Afr2Oled_TextWindow(o,0,128,y,text,Afr2Ui_TextOffset(text,128,elapsed),AFR2_WHITE);
}
static void labeled(Afr2Oled *o,uint8_t y,const char *label,uint8_t left,const char *text,uint32_t elapsed) {
    Afr2Oled_FillRect(o,0,y,128,10,AFR2_BLACK);
    Afr2Oled_TextFlash(o,0,y,label,AFR2_WHITE);
    Afr2Oled_TextWindow(o,left,128,y,text,Afr2Ui_TextOffset(text,128-left,elapsed),AFR2_WHITE);
}
static void back(Afr2Oled *o) {
    Afr2Icon_Draw(o,AFR2_ICON_BACK,0,54,8);TXT(o,14,53,"Volver");
}
void Afr2NetworkScreen_Render(Afr2Oled *o,const Afr2Ui *u,const Afr2UiSnapshot *s) {
    uint8_t ap=u->node==AFR2_N_AP_INFO;uint32_t elapsed=u->clock_ms-u->scroll_started_ms;
    Afr2Icon_Draw(o,ap?AFR2_ICON_AP:AFR2_ICON_WIFI,0,0,14);
    if(ap)TXT(o,21,1,"Datos de red AP");else TXT(o,21,1,"IP de conexion");
    if(!s->network_valid){line(o,17,"Sin datos actuales",elapsed);line(o,32,"Actualizar Wi-Fi",elapsed);}
    else if(ap) {
        if(!s->network_ap){line(o,17,"AP no esta activo",elapsed);line(o,32,"IP: --",elapsed);}
        else {labeled(o,16,PSTR("SSID:"),35,s->network_ap_ssid,elapsed);labeled(o,28,PSTR("Clave:"),42,s->network_password[0]?s->network_password:"Sin clave",elapsed);labeled(o,40,PSTR("IP:"),21,s->network_ap_ip,elapsed);}
    } else if(s->network_sta) {
        labeled(o,16,PSTR("SSID:"),35,s->network_ssid,elapsed);labeled(o,29,PSTR("IP:"),21,s->network_ip,elapsed);line(o,41,"STA / abrir en Web",elapsed);
    } else if(s->network_ap) {
        labeled(o,16,PSTR("SSID:"),35,s->network_ap_ssid,elapsed);labeled(o,29,PSTR("IP:"),21,s->network_ap_ip,elapsed);line(o,41,"AP / abrir en Web",elapsed);
    } else {line(o,17,"Sin IP adquirida",elapsed);line(o,32,"Esperando conexion",elapsed);}
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
    case AFR2_NET_BOOT:TXT(o,24,9,"ESP iniciada");line(o,31,"Firmware propio",elapsed);break;
    case AFR2_NET_WEB_READY:TXT(o,24,9,"Servidor listo");line(o,31,"HTTP disponible",elapsed);break;
    case AFR2_NET_GOT_IP:TXT(o,24,9,"IP adquirida");line(o,28,s->network_valid?s->network_ssid:"Wi-Fi",elapsed);line(o,41,n->text,elapsed);break;
    case AFR2_NET_AP_READY:
        TXT(o,24,8,"Red AP creada");
        if(s->network_valid && s->network_ap){labeled(o,23,PSTR("SSID:"),35,s->network_ap_ssid,elapsed);labeled(o,34,PSTR("Clave:"),42,s->network_password[0]?s->network_password:"Sin clave",elapsed);labeled(o,45,PSTR("IP:"),21,s->network_ap_ip,elapsed);}
        else line(o,31,"Consultar datos AP",elapsed);
        break;
    case AFR2_NET_CLIENT_JOIN:TXT(o,24,9,"Cliente entro");line(o,32,n->text,elapsed);break;
    case AFR2_NET_CLIENT_LEAVE:TXT(o,24,9,"Cliente salio");line(o,32,n->text,elapsed);break;
    case AFR2_NET_WS_OPEN:TXT(o,24,9,"WebSocket OK");line(o,32,"Cliente conectado",elapsed);break;
    case AFR2_NET_WS_CLOSE:TXT(o,24,9,"WebSocket cerro");line(o,32,"Cliente salio",elapsed);break;
    case AFR2_NET_WEB_SERVED:TXT(o,24,9,"Web servida");line(o,32,"HTTP 200 iniciado",elapsed);break;
    case AFR2_NET_STA_LOST:TXT(o,24,9,"Wi-Fi sin red");line(o,32,"STA desconectada",elapsed);break;
    case AFR2_NET_AP_STOPPED:TXT(o,24,9,"Red AP apagada");line(o,32,"Sin acceso por AP",elapsed);break;
    }
    if(kind!=AFR2_NET_AP_READY)TXT(o,0,54,"Click: cerrar");
}
