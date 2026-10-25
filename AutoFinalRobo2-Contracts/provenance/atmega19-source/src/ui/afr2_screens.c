/* AFR2-20261023-19. Own AVR adaptation of F4 menusystem, osk and notification
 * concepts. F4 original authors remain in reference/. Renderer is hardware-free.
 */
#include "afr2_screens.h"
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#define TXT(o,x,y,s) Afr2Oled_TextFlash(o,x,y,PSTR(s),AFR2_WHITE)
static void wipe(void *data,uint16_t n){volatile uint8_t *p=data;while(n--)*p++=0;}
static void sync_code(Afr2Ui *u){Afr2MenuNode n;Afr2Menu_Node(u->node,&n);u->screen=n.code;++u->generation;u->redraw=true;}
void Afr2Ui_Init(Afr2Ui *u){memset(u,0,sizeof(*u));u->node=AFR2_N_HOME;sync_code(u);}
void Afr2Ui_Attach(Afr2Ui *u,const Afr2UiOps *ops){u->ops=*ops;}
static uint8_t invoke(Afr2Ui *u,uint8_t action,int32_t value,const void *data) {
    return u->ops.invoke?u->ops.invoke(u->ops.ctx,action,value,data):2;
}
void Afr2Ui_Open(Afr2Ui *u,uint8_t node) {
    if(node>=AFR2_N_COUNT)return;
    if(node==AFR2_N_HOME || node==AFR2_N_MENU)u->depth=0;
    else if(u->depth<8)u->history[u->depth++]=(Afr2UiPosition){u->node,u->selection,u->firstVisible};
    u->node=node;u->selection=u->firstVisible=0;u->edit=0;sync_code(u);
}
static void back(Afr2Ui *u,uint8_t fallback) {
    if(u->depth){Afr2UiPosition p=u->history[--u->depth];u->node=p.node;u->selection=p.selection;u->firstVisible=p.first;sync_code(u);}
    else Afr2Ui_Open(u,fallback);
}
void Afr2Ui_Notify(Afr2Ui *u,uint8_t kind,const char *title,const char *text,uint16_t ms) {
    for(uint8_t i=0;i<u->notices;i++) {
        Afr2Notification *n=&u->notification[(u->tail+i)%4];
        if(n->kind==kind && !strncmp(n->title,title,17) && !strncmp(n->text,text,17))return;
    }
    /* Queue full: replace latest informational item, never unbounded allocation. */
    if(u->notices==4){u->head=(u->head+3u)%4;--u->notices;}
    Afr2Notification *n=&u->notification[u->head];memset(n,0,sizeof(*n));
    strncpy(n->title,title,17);strncpy(n->text,text,17);n->ms=ms?ms:3000;n->kind=kind;
    u->head=(u->head+1u)%4;if(!u->notices)u->notice_started_ms=u->clock_ms;
    ++u->notices;++u->generation;u->redraw=true;
}
static void dismiss(Afr2Ui *u){if(u->notices){u->tail=(u->tail+1u)%4;--u->notices;++u->generation;u->notice_started_ms=u->clock_ms;u->redraw=true;}}
void Afr2Ui_Tick(Afr2Ui *u,uint32_t now) {
    uint32_t before=u->clock_ms;u->clock_ms=now;
    if(u->notices){if((uint32_t)(now-u->notice_started_ms)>=u->notification[u->tail].ms)dismiss(u);
        else if(now/100u!=before/100u)u->redraw=true;}
}
static void result(Afr2Ui *u,uint8_t code) {
    Afr2MenuItem item;Afr2Menu_Item(u->node,u->selection,&item);
    const char *why=u->ops.reason?u->ops.reason(u->ops.ctx,item.action):"Ver requisitos";
    if(!code)Afr2Ui_Notify(u,0,"Operacion","Aceptada",3000);
    else Afr2Ui_Notify(u,1,"No disponible",code==1?"Dato incorrecto":code==3?"Sin permiso":code==4?"Servicio ocupado":why,3000);
}
static void keyboard(Afr2Ui *u,Afr2UiInput in) {
    if(in.menu){wipe(&u->credentials,sizeof(u->credentials));u->keyboard=0;u->redraw=true;return;}
    if(in.rotation){int16_t k=(int16_t)u->key+in.rotation;while(k<0)k+=97;u->key=k%97;u->redraw=true;}
    if(!in.click)return;
    char *b=u->keyboard_field?u->credentials.password:u->credentials.ssid;
    uint8_t max=u->keyboard_field?63:32;
    if(u->key<95){if(u->length<max){b[u->length++]=(char)(32u+u->key);b[u->length]=0;}}
    else if(u->key==95){if(u->length)b[--u->length]=0;}
    else {
        if(!u->keyboard_field) {
            if(!u->length){result(u,1);return;}
            u->keyboard_field=1;u->length=0;u->key=0;
        } else if(u->length && u->length<8){result(u,1);return;}
        else {
            uint8_t r=invoke(u,AFR2_A_KEYBOARD,0,&u->credentials);
            wipe(&u->credentials,sizeof(u->credentials));u->keyboard=0;
            if(!r)Afr2Ui_Open(u,AFR2_N_WIFI_REVIEW);result(u,r);
        }
    }
    u->redraw=true;
}
void Afr2Ui_Apply(Afr2Ui *u,Afr2UiInput in) {
    if(in.rotation || in.click || in.menu)++u->generation;
    if(in.rotation)u->encoder_turns+=in.rotation;
    if(in.click && u->encoder_clicks!=UINT16_MAX)++u->encoder_clicks;
    if(in.menu && u->encoder_holds!=UINT16_MAX)++u->encoder_holds;
    if(u->keyboard){
        if(u->notices && !in.menu){if(in.click)dismiss(u);return;}
        keyboard(u,in);return;
    }
    if(in.menu) {
        u->notices=0;u->head=u->tail=0;u->edit=0;
        (void)invoke(u,AFR2_A_HALL_CANCEL,0,0); /* Stops local tests/cancels calibration on exit. */
        Afr2Ui_Open(u,AFR2_N_MENU);return;
    }
    if(u->notices){if(in.click)dismiss(u);return;}
    if(u->edit) {
        if(in.rotation){u->draft+=(int32_t)in.rotation*u->editing.step;
            if(u->draft<u->editing.min)u->draft=u->editing.min;if(u->draft>u->editing.max)u->draft=u->editing.max;u->redraw=true;}
        if(in.click){uint8_t r=invoke(u,u->editing.action,u->draft,0);u->edit=0;u->redraw=true;result(u,r);}return;
    }
    Afr2MenuNode n;Afr2Menu_Node(u->node,&n);
    if(in.rotation){int16_t selected=(int16_t)u->selection+in.rotation;
        while(selected<0)selected+=n.count;u->selection=selected%n.count;
        uint8_t visible=n.view?1:3;
        if(u->selection<u->firstVisible)u->firstVisible=u->selection;
        if(u->selection>=u->firstVisible+visible)u->firstVisible=u->selection-visible+1;
        u->redraw=true;
    }
    if(in.click){
        Afr2MenuItem i;Afr2Menu_Item(u->node,u->selection,&i);
        if(i.action && u->ops.available){uint8_t r=u->ops.available(u->ops.ctx,i.action,i.arg);if(r){result(u,r);return;}}
        if(!i.action) {
            if(!strncmp(i.label,"Volver",6)){
                if(u->node==AFR2_N_WHEEL || u->node==AFR2_N_MOVE || u->node==AFR2_N_HALL_TEST)
                    (void)invoke(u,AFR2_A_HALL_CANCEL,0,0);
                back(u,i.target);
            }else Afr2Ui_Open(u,i.target);
        } else if(i.action==AFR2_A_KEYBOARD) {
            wipe(&u->credentials,sizeof(u->credentials));
            if(u->ops.keyboard_seed)u->ops.keyboard_seed(u->ops.ctx,&u->credentials);
            u->keyboard=1;u->keyboard_field=0;u->length=strlen(u->credentials.ssid);u->key=0;u->redraw=true;
        } else if(i.step){u->editing=i;u->edit=1;u->draft=u->ops.value?u->ops.value(u->ops.ctx,i.action):i.min;u->redraw=true;}
        else {uint8_t r=invoke(u,i.action,i.arg,0);u->redraw=true;result(u,r);}
    }
}
static void render_keyboard(Afr2Ui *u,Afr2Oled *o) {
    if(u->keyboard_field)TXT(o,0,0,"Clave Wi-Fi");else TXT(o,0,0,"SSID Wi-Fi");
    char text[18];uint8_t length=u->length<17?u->length:17;
    if(u->keyboard_field){memset(text,'*',length);text[length]=0;}
    else {memcpy(text,u->credentials.ssid+u->length-length,length);text[length]=0;}
    Afr2Oled_Text(o,0,13,text,AFR2_WHITE);
    uint8_t first=(u->key/10u)/2u*20u;
    for(uint8_t j=0;j<20 && first+j<97;j++) {
        uint8_t k=first+j,x=(j%10u)*12u,y=29u+(j/10u)*12u;
        char ch[2]={(char)(32u+k),0};
        if(k<95)Afr2Oled_Text(o,x,y,ch,AFR2_WHITE);
        else if(k==95)TXT(o,x,y,"<");else TXT(o,x,y,"OK");
        if(k==u->key)Afr2Oled_Rect(o,x-1,y-1,12,12,AFR2_INVERT);
    }
    TXT(o,0,53,"Largo: cancelar");
}
void Afr2Ui_Render(Afr2Ui *u,Afr2Oled *o,const Afr2UiSnapshot *s) {
    Afr2MenuNode n;Afr2Menu_Node(u->node,&n);Afr2Oled_Clear(o);
    if(u->notices) {
        Afr2Notification *a=&u->notification[u->tail];
        uint16_t width=(uint16_t)(((uint32_t)(u->clock_ms-u->notice_started_ms)*128u)/a->ms);if(width>128)width=128;
        for(uint8_t x=0;x<width;x++){Afr2Oled_Pixel(o,x,0,AFR2_WHITE);Afr2Oled_Pixel(o,x,1,AFR2_WHITE);}
        Afr2Oled_Text(o,0,10,a->kind?"!":"i",AFR2_WHITE);
        Afr2Oled_Text(o,0,25,a->title,AFR2_WHITE);Afr2Oled_Text(o,0,39,a->text,AFR2_WHITE);
        TXT(o,0,53,"Click: cerrar");
    } else if(u->keyboard)render_keyboard(u,o);
    else if(u->edit) {
        char value[18];Afr2Oled_Text(o,0,0,u->editing.label,AFR2_WHITE);
        snprintf(value,sizeof(value),"%ld",(long)u->draft);Afr2Oled_Text(o,0,23,value,AFR2_WHITE);
        TXT(o,0,40,"Click: aplicar");TXT(o,0,53,"Largo: descartar");
    } else {
        Afr2Oled_Text(o,0,0,n.title,AFR2_WHITE);
        if(n.view){for(uint8_t j=0;j<3;j++)Afr2Oled_Text(o,0,13+j*12,s->lines[j],AFR2_WHITE);}
        uint8_t count=n.view?1:3;
        for(uint8_t row=0;row<count && u->firstVisible+row<n.count;row++) {
            Afr2MenuItem item;Afr2Menu_Item(u->node,u->firstVisible+row,&item);
            uint8_t y=n.view?52:(18+14*row);
            Afr2Oled_Text(o,0,y,u->firstVisible+row==u->selection?(s->disabled[row]?"!":">"):(s->disabled[row]?"-":" "),AFR2_WHITE);
            Afr2Oled_Text(o,9,y,s->labels[row][0]?s->labels[row]:item.label,AFR2_WHITE);
        }
    }
    ++u->revision;if(!u->revision)++u->revision;u->redraw=false;
}
