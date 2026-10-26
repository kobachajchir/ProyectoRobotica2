/* AFR2-20261023-19. Own AVR adaptation of F4 menusystem, osk and notification
 * concepts. F4 original authors remain in reference/. Renderer is hardware-free.
 */
#include "afr2_screens.h"
#include "afr2_icons.h"
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#define TXT(o,x,y,s) Afr2Oled_TextFlash(o,x,y,PSTR(s),AFR2_WHITE)
static void wipe(void *data,uint16_t n){volatile uint8_t *p=data;while(n--)*p++=0;}
static void sync_code(Afr2Ui *u){Afr2MenuNode n;Afr2Menu_Node(u->node,&n);u->screen=n.code;++u->generation;++u->navigation_revision;u->redraw=true;u->scroll_started_ms=u->clock_ms;u->scroll_redraw=false;}
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
    u->clock_ms=now;
    if(u->notices && (uint32_t)(now-u->notice_started_ms)>=u->notification[u->tail].ms)dismiss(u);
    uint8_t network=u->node==AFR2_N_NETWORK || u->node==AFR2_N_AP_INFO;
    uint8_t interval=(u->notices || network)?100:40;
    if((uint32_t)(now-u->scroll_render_ms)<interval)return;
    u->scroll_render_ms=now;
    if(u->notices || network)u->scroll_redraw=true;
    else if(!u->keyboard && !u->edit) {
        Afr2MenuNode node;Afr2Menu_Node(u->node,&node);
        if(!node.view){Afr2MenuItem item;Afr2Menu_Item(u->node,u->selection,&item);
            if(strlen(item.label)*7u>93u || u->node==AFR2_N_WIFI_NETWORKS)u->scroll_redraw=true;}
    }
}
uint16_t Afr2Ui_TextOffset(const char *text,uint8_t width,uint32_t elapsed) {
    if(!text)return 0;
    uint16_t pixels=(uint16_t)strlen(text)*7u;if(pixels<=width)return 0;
    uint16_t end=pixels-width;
    uint32_t phase=elapsed%(1600UL+(uint32_t)end*40u);
    if(phase<=800u)return 0;
    uint32_t offset=(phase-800u)/40u;return offset>end?end:(uint16_t)offset;
}
uint32_t Afr2Ui_ReportedScreen(const Afr2Ui *u) {
    if(u->notices){uint8_t kind=u->notification[u->tail].kind;
        if(kind>AFR2_NOTICE_NETWORK && kind<=AFR2_NOTICE_NETWORK+AFR2_NET_EVENT_MAX)return 0x040000UL+kind-AFR2_NOTICE_NETWORK;}
    return u->screen;
}
static void result(Afr2Ui *u,uint8_t code) {
    Afr2MenuItem item;Afr2Menu_Item(u->node,u->selection,&item);
    const char *why=u->ops.reason?u->ops.reason(u->ops.ctx,item.action):"Ver requisitos";
    if(!code)Afr2Ui_Notify(u,0,"Operacion","Aceptada",3000);
    else Afr2Ui_Notify(u,1,"No disponible",code==1?"Dato incorrecto":code==3?"Condicion invalida":code==4?"Servicio ocupado":why,3000);
}
static void keyboard(Afr2Ui *u,Afr2UiInput in) {
    if(in.menu){wipe(&u->credentials,sizeof(u->credentials));u->keyboard=0;u->notices=0;u->head=u->tail=0;Afr2Ui_Open(u,AFR2_N_MENU);return;}
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
            if(!r)Afr2Ui_Open(u,AFR2_N_WIFI_REVIEW);
            result(u,r);
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
            if(u->draft<u->editing.min)u->draft=u->editing.min;
            if(u->draft>u->editing.max)u->draft=u->editing.max;
            u->redraw=true;}
        if(in.click){uint8_t r=invoke(u,u->editing.action,u->draft,0);u->edit=0;u->redraw=true;result(u,r);}return;
    }
    if(u->node==AFR2_N_RADAR_VIEW || u->node==AFR2_N_US_VIEW || u->node==AFR2_N_LINE_VIEW || u->node==AFR2_N_IR_VALUES || u->node==AFR2_N_SERVO_ANGLE){
        if(in.click){back(u,u->node==AFR2_N_SERVO_ANGLE?AFR2_N_SERVO:u->node==AFR2_N_LINE_VIEW || u->node==AFR2_N_IR_VALUES?AFR2_N_SENSORS:AFR2_N_DISTANCE_SERVO);return;}
        if(in.rotation){
            if(u->node==AFR2_N_SERVO_ANGLE){int32_t angle=(u->ops.value?u->ops.value(u->ops.ctx,AFR2_A_SERVO_ANGLE):90)+in.rotation;
                if(angle<0)angle=0;
                if(angle>180)angle=180;
                uint8_t r=invoke(u,AFR2_A_SERVO_ANGLE,angle,0);if(r)result(u,r);u->redraw=true;}
            else if(in.rotation%2){u->node=u->node==AFR2_N_RADAR_VIEW?AFR2_N_US_VIEW:u->node==AFR2_N_US_VIEW?AFR2_N_RADAR_VIEW:u->node==AFR2_N_LINE_VIEW?AFR2_N_IR_VALUES:AFR2_N_LINE_VIEW;sync_code(u);}
        }return;
    }
    Afr2MenuNode n;Afr2Menu_Node(u->node,&n);
    if(in.rotation){int16_t selected=(int16_t)u->selection+in.rotation;
        while(selected<0)selected+=n.count;
        u->selection=selected%n.count;
        uint8_t visible=n.view?1:3;
        if(u->selection<u->firstVisible)u->firstVisible=u->selection;
        if(u->selection>=u->firstVisible+visible)u->firstVisible=u->selection-visible+1;
        u->scroll_started_ms=u->clock_ms;
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
    Afr2MenuNode n;Afr2Menu_Node(u->node,&n);
    uint8_t partial=!u->redraw && u->scroll_redraw;
    if(!partial)Afr2Oled_Clear(o);
    if(u->notices) {
        Afr2Notification *a=&u->notification[u->tail];
        uint16_t width=(uint16_t)(((uint32_t)(u->clock_ms-u->notice_started_ms)*128u)/a->ms);if(width>128)width=128;
        Afr2Oled_FillRect(o,0,0,128,2,AFR2_BLACK);Afr2Oled_FillRect(o,0,0,width,2,AFR2_WHITE);
        if(a->kind>AFR2_NOTICE_NETWORK)Afr2NetworkNotice_Render(o,u,s);
        else if(!partial) {
            Afr2Icon_Draw(o,a->kind?AFR2_ICON_WARNING:AFR2_ICON_INFO,56,6,16);
            Afr2Oled_Text(o,0,25,a->title,AFR2_WHITE);Afr2Oled_Text(o,0,39,a->text,AFR2_WHITE);
            TXT(o,0,53,"Click: cerrar");
        }
    } else if(u->keyboard)render_keyboard(u,o);
    else if(u->node==AFR2_N_SEQUENCE)Afr2SequenceScreen_Render(o,s);
    else if(u->node==AFR2_N_NETWORK || u->node==AFR2_N_AP_INFO)Afr2NetworkScreen_Render(o,u,s);
    else if(u->node==AFR2_N_RADAR_VIEW || u->node==AFR2_N_US_VIEW || u->node==AFR2_N_LINE_VIEW || u->node==AFR2_N_IR_VALUES || u->node==AFR2_N_SERVO_ANGLE)Afr2SensorScreen_Render(o,u->node,s);
    else if(u->edit) {
        char value[18];Afr2Oled_Text(o,0,0,u->editing.label,AFR2_WHITE);
        snprintf(value,sizeof(value),"%ld",(long)u->draft);Afr2Oled_Text(o,0,23,value,AFR2_WHITE);
        TXT(o,0,40,"Click: aplicar");TXT(o,0,53,"Largo: descartar");
    } else {
        if(!partial) {
            Afr2Oled_Text(o,0,0,n.title,AFR2_WHITE);
            if(n.view){for(uint8_t j=0;j<3;j++)Afr2Oled_Text(o,0,13+j*12,s->lines[j],AFR2_WHITE);}
        }
        uint8_t count=n.view?1:3;
        for(uint8_t row=0;row<count && u->firstVisible+row<n.count;row++) {
            Afr2MenuItem item;Afr2Menu_Item(u->node,u->firstVisible+row,&item);
            uint8_t selected=u->firstVisible+row==u->selection;
            if(partial && !selected)continue;
            uint8_t y=n.view?52:(16+16*row),left=n.view?14:25,top=n.view?y:y+3;
            if(!partial) {
                Afr2Icon_Draw(o,Afr2Menu_Icon(u->node,item.target,item.action,item.label),n.view?0:3,n.view?54:y,n.view?8:16);
                if(selected && !n.view)Afr2Oled_Rect(o,0,y,128,16,AFR2_WHITE);
                Afr2Oled_Text(o,120,top,selected?(s->disabled[row]?"!":">"):(s->disabled[row]?"!":" "),AFR2_WHITE);
            }
            const char *label=s->labels[row][0]?s->labels[row]:item.label;
            Afr2Oled_FillRect(o,left,top,118-left,10,AFR2_BLACK);
            Afr2Oled_TextWindow(o,left,118,top,label,selected?Afr2Ui_TextOffset(label,118-left,u->clock_ms-u->scroll_started_ms):0,AFR2_WHITE);
        }
    }
    ++u->revision;if(!u->revision)++u->revision;u->redraw=false;u->scroll_redraw=false;
}
