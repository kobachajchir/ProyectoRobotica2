/* AFR2-20261102-05. Runtime AP/Web QR, Nayuki MIT encoder. */
#include "afr2_qr_screen.h"
#include "afr2_icons.h"
#include "qrcodegen.h"
#include <avr/pgmspace.h>
#include <string.h>
#define QR_BYTES qrcodegen_BUFFER_LEN_FOR_VERSION(9)
static uint32_t last_frame,last_boot;
static uint16_t last_network;
static uint8_t last_ap,last_status,cache_valid;
static uint8_t append(uint8_t *data,size_t *len,const char *text,uint8_t escape){
    while(*text){uint8_t c=(uint8_t)*text++;
        if(escape && (c=='\\' || c==';' || c==',' || c==':' || c=='"')){if(*len>=QR_BYTES-1)return 0;data[(*len)++]='\\';}
        if(*len>=QR_BYTES-1)return 0;
        data[(*len)++]=c;
    }return 1;
}
static void labels(Afr2Oled *o,uint8_t ap){
    Afr2Icon_Draw(o,ap?AFR2_ICON_AP:AFR2_ICON_LINK,67,1,14);
    Afr2Oled_TextFlash(o,67,19,ap?PSTR("Conectar"):PSTR("Abrir"),AFR2_WHITE);
    Afr2Oled_TextFlash(o,67,31,ap?PSTR("al AP"):PSTR("Web"),AFR2_WHITE);
    Afr2Ui_GestureHint(o,67,52,1,PSTR("Volver"));
}
/* AFR2-20261106-04: same matrix/raster, borrow ACK scratch after encoding.
 * No main-loop yield here: callbacks cannot modify scratch during this loan. */
static void __attribute__((noinline)) finish(Afr2Oled *o,const uint8_t *encoded){
    uint8_t *matrix=Afr2Oled_BorrowScratch(o,QR_BYTES);
    if(!matrix)return;
    memcpy(matrix,encoded,QR_BYTES);Afr2Oled_Clear(o);
    int size=qrcodegen_getSize(matrix),scale=62/(size+8),side=(size+8)*scale;
    int origin=1+(62-side)/2;Afr2Oled_FillRect(o,1,1,62,62,AFR2_WHITE);
    for(int y=0;y<size;y++)for(int x=0;x<size;x++)if(qrcodegen_getModule(matrix,x,y))
        Afr2Oled_FillRect(o,origin+(x+4)*scale,origin+(y+4)*scale,scale,scale,AFR2_BLACK);
    volatile uint8_t *private_bytes=matrix;for(uint16_t i=0;i<QR_BYTES;i++)private_bytes[i]=0;
}
uint8_t Afr2Qr_DrawPayload(Afr2Oled *o,const uint8_t *payload,uint16_t length){
    cache_valid=0;
    if(!payload || !length || length>=QR_BYTES){Afr2Oled_Clear(o);return 0;}
    memmove(o->buffer,payload,length);
    uint8_t *qr=o->buffer+QR_BYTES;
    if(!qrcodegen_encodeBinary(o->buffer,length,qr,qrcodegen_Ecc_LOW,1,9,qrcodegen_Mask_0,true)){Afr2Oled_Clear(o);return 0;}
    finish(o,qr);return 1;
}
void Afr2Qr_Render(Afr2Oled *o,const Afr2UiSnapshot *s,uint8_t ap,uint32_t revision){
    const char *ip=s->network_sta?s->network_ip:s->network_ap_ip;
    uint8_t status=!s->qr_allowed?1:!s->network_valid?2:ap?(!s->network_ap || !s->network_ap_ssid || !*s->network_ap_ssid?3:0):(!s->network_server || !ip || !*ip || !strcmp(ip,"0.0.0.0") || (!s->network_sta && !s->network_ap)?4:0);
    uint8_t key=ap | (s->network_sta<<1) | (s->network_ap<<2) | (s->network_server<<3);
    uint32_t next=revision+1;if(!next)++next;
    if(cache_valid && last_frame==revision && last_boot==s->network_boot && last_network==s->network_revision && last_ap==key && last_status==status){last_frame=next;return;}
    cache_valid=1;last_frame=next;last_boot=s->network_boot;last_network=s->network_revision;last_ap=key;last_status=status;
    if(!status){
        size_t len=0;uint8_t *data=o->buffer,*qr=o->buffer+QR_BYTES;uint8_t ok;
        if(ap){const char *password=s->network_password?s->network_password:"";
            ok=append(data,&len,*password?"WIFI:T:WPA;S:":"WIFI:T:nopass;S:",0) && append(data,&len,s->network_ap_ssid,1) && append(data,&len,";P:",0) && append(data,&len,password,1) && append(data,&len,";;",0);
        }else ok=append(data,&len,"http://",0) && append(data,&len,ip,0) && append(data,&len,"/",0);
        if(ok && qrcodegen_encodeBinary(data,len,qr,qrcodegen_Ecc_LOW,1,9,qrcodegen_Mask_0,true)){finish(o,qr);labels(o,ap);return;}
        status=5;
    }
    Afr2Oled_Clear(o);
    char message[12];
    Afr2Oled_TextFlash(o,0,3,PSTR("QR"),AFR2_WHITE);
    strcpy_P(message,status==1?PSTR("Detener"):status==5?PSTR("Contenido"):PSTR("Sin datos"));Afr2Oled_TextWindow(o,0,63,20,message,0,AFR2_WHITE);
    strcpy_P(message,status==1?PSTR("auto/radar"):status==3?PSTR("Activar AP"):status==4?PSTR("Web / IP"):status==5?PSTR("muy largo"):PSTR("de red"));Afr2Oled_TextWindow(o,0,63,32,message,0,AFR2_WHITE);
    labels(o,ap);
}
