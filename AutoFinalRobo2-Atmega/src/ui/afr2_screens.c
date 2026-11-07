/* AFR2-20261030-01: AutoMicro dashboard adaptation and linked icons; prior authors retained. */
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
/* AFR2-20261023-19. Own AVR adaptation of F4 menusystem, osk and notification
 * concepts. F4 original authors remain in reference/. Renderer is hardware-free.
 */
#include "afr2_screens.h"
#include "afr2_icons.h"
#include "afr2_ui_components.h"
#include "afr2_qr_screen.h"
#include "afr2_touch_bitmap.h"
#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#define TXT(o,x,y,s) Afr2Oled_TextFlash(o,x,y,PSTR(s),AFR2_WHITE)
/* Dynamic rows use the same resolver for drawing and activation. */
static uint8_t clean(Afr2Ui *u){return u->ops.value && !u->ops.value(u->ops.ctx,AFR2_A_DIRTY);}
void Afr2Ui_MenuNode(Afr2Ui *u,Afr2MenuNode *n){
    Afr2Menu_Node(u->node,n);
    if(u->node==AFR2_N_WIFI_SEARCH && u->ops.value)n->count=u->ops.value(u->ops.ctx,AFR2_A_WIFI_SCAN_STATE)==1?1:u->ops.value(u->ops.ctx,AFR2_A_WIFI_TOTAL)?3:2;
    if(u->node==AFR2_N_HALL_PICK)n->count=4;
    if(u->node==AFR2_N_HALL_TEST)n->count=3;
    if(u->node==AFR2_N_WIFI_NETWORKS && u->ops.value)n->count=(uint8_t)u->ops.value(u->ops.ctx,AFR2_A_WIFI_COUNT)+2u+(u->ops.value(u->ops.ctx,AFR2_A_WIFI_PAGE_INDEX)>0)+(u->ops.value(u->ops.ctx,AFR2_A_WIFI_TOTAL)>(u->ops.value(u->ops.ctx,AFR2_A_WIFI_PAGE_INDEX)+1)*8);
    uint8_t connection=u->ops.value?(uint8_t)u->ops.value(u->ops.ctx,AFR2_A_WIFI_CONNECTED):2;
    if(u->node==AFR2_N_WIFI_VIEW)n->count-=connection==2?2:connection==1?1:0;
    if(u->node==AFR2_N_WIFI_REVIEW && connection==2)--n->count;
    if(clean(u) && (u->node==AFR2_N_PREFS || u->node==AFR2_N_CALIB))--n->count;
}
void Afr2Ui_MenuItem(Afr2Ui *u,uint8_t index,Afr2MenuItem *item){
    if(u->node==AFR2_N_WIFI_SEARCH && u->ops.value){
        memset(item,0,sizeof(*item));item->target=255;
        if(u->ops.value(u->ops.ctx,AFR2_A_WIFI_SCAN_STATE)==1){strcpy_P(item->label,PSTR("Buscando redes"));return;}
        if(u->ops.value(u->ops.ctx,AFR2_A_WIFI_TOTAL)){if(!index){strcpy_P(item->label,PSTR("Ver redes"));item->target=AFR2_N_WIFI_NETWORKS;return;}--index;}
        if(!index){strcpy_P(item->label,PSTR("Refrescar"));item->action=AFR2_A_WIFI_SCAN;return;}
        strcpy_P(item->label,PSTR("Volver"));item->target=AFR2_N_WIFI;return;
    }
    if(u->node==AFR2_N_WIFI_NETWORKS && u->ops.value && index>=u->ops.value(u->ops.ctx,AFR2_A_WIFI_COUNT)){
        uint8_t extra=index-u->ops.value(u->ops.ctx,AFR2_A_WIFI_COUNT),page=u->ops.value(u->ops.ctx,AFR2_A_WIFI_PAGE_INDEX);
        memset(item,0,sizeof(*item));item->target=255;
        if(page && !extra){strcpy_P(item->label,PSTR("Pagina anterior"));item->action=AFR2_A_WIFI_PAGE;item->arg=page-1;return;}if(page)--extra;
        if(u->ops.value(u->ops.ctx,AFR2_A_WIFI_TOTAL)>(page+1u)*8u){if(!extra){strcpy_P(item->label,PSTR("Pagina siguiente"));item->action=AFR2_A_WIFI_PAGE;item->arg=page+1;return;}--extra;}
        if(!extra){strcpy_P(item->label,PSTR("Refrescar"));item->action=AFR2_A_WIFI_SCAN;return;}
        strcpy_P(item->label,PSTR("Volver"));item->target=AFR2_N_WIFI_SEARCH;return;
    }
    if(u->node==AFR2_N_HALL_TEST){
        memset(item,0,sizeof(*item));item->target=255;
        if(!index){strcpy_P(item->label,PSTR("Estado"));item->target=AFR2_N_HALL_VIEW;}
        else if(index==1){strcpy_P(item->label,PSTR("Calibrar motor"));item->action=AFR2_A_HALL_BEGIN;}
        else {strcpy_P(item->label,PSTR("Volver"));item->target=AFR2_N_TESTS;}return;
    }
    uint8_t connection=u->ops.value?(uint8_t)u->ops.value(u->ops.ctx,AFR2_A_WIFI_CONNECTED):2;
    if(u->node==AFR2_N_WIFI_VIEW){if(connection==2 && index>=3)index+=2;else if(connection==1 && index>=4)++index;}
    if(u->node==AFR2_N_WIFI_REVIEW && connection==2 && index>=1)++index;
    if(clean(u) && ((u->node==AFR2_N_PREFS && index>=3) || (u->node==AFR2_N_CALIB && index>=4)))++index;
    Afr2Menu_Item(u->node,index,item);
    if(item->action==AFR2_A_WIFI_CONNECT && u->ops.value && u->ops.value(u->ops.ctx,AFR2_A_WIFI_CONNECTED)==1){item->action=AFR2_A_WIFI_DISCONNECT;strcpy_P(item->label,PSTR("Desconectar"));}
    if(item->action==AFR2_A_WIFI_CONNECT && u->ops.value && !u->ops.value(u->ops.ctx,AFR2_A_WIFI_CONNECT)){
        item->action=0;item->target=AFR2_N_WIFI_SETUP;strcpy_P(item->label,PSTR("Configurar red"));
    }
    if(item->action==AFR2_A_RADAR_TOGGLE || item->action==AFR2_A_LINE_TOGGLE || item->action==AFR2_A_WHEEL_TOGGLE || item->action==AFR2_A_GOTO_TOGGLE){
        uint8_t on=u->ops.value && u->ops.value(u->ops.ctx,item->action);
        strcpy_P(item->label,on?PSTR("Detener"):PSTR("Iniciar"));
    }
}
static void dismiss(Afr2Ui *u);
void Afr2Ui_GestureHint(Afr2Oled *o,uint8_t x,uint8_t y,uint8_t is_long,const char *text){
    if(is_long==1 && x>=66)x=66;
    uint8_t size=is_long==2?21:13;if(y>52)y=52;
    Afr2Icon_Draw(o,is_long==2?AFR2_ICON_ENCODER_TURN:is_long?AFR2_ICON_ENCODER_LONG:AFR2_ICON_ENCODER,x,y-(size-10)/2,size);
    if(!strcmp_P(text,PSTR("Volver")))Afr2Icon_Draw(o,AFR2_ICON_BACK,x+size+4,y-1,13);
    else Afr2Oled_TextFlash(o,x+size+4,y,text,AFR2_WHITE);
}
/* AFR2-20261030-18: shared selector, with no service calls in its renderer. */
uint8_t Afr2MotorSelector_Step(uint8_t selected,int8_t delta){
    int16_t next=(int16_t)(selected&3u)+delta;while(next<0)next+=4;return (uint8_t)(next%4);
}
void Afr2MotorSelector_Render(Afr2Oled *o,uint8_t selected){
    static const char names[][3] PROGMEM={"FL","FR","RL","RR"};
    Afr2Component_HeaderFlash(o,AFR2_ICON_MOTOR,PSTR("Elegir un motor"),0,0);
    for(uint8_t i=0;i<4;i++)Afr2Oled_TextFlash(o,9+32u*i,22,names[i],AFR2_WHITE);
    Afr2Oled_FillRect(o,9+32u*(selected&3u),34,14,1,AFR2_WHITE);
    Afr2Ui_GestureHint(o,0,40,2,PSTR("Elegir"));
    Afr2Ui_GestureHint(o,0,53,0,PSTR("OK"));Afr2Ui_GestureHint(o,66,53,1,PSTR("Volver"));
}
static void wipe(void *data,uint16_t n){volatile uint8_t *p=data;while(n--)*p++=0;}
static void sync_code(Afr2Ui *u){Afr2MenuNode n;Afr2Ui_MenuNode(u,&n);u->screen=n.code;++u->generation;++u->navigation_revision;u->redraw=true;u->scroll_started_ms=u->clock_ms;u->scroll_redraw=false;}
void Afr2Ui_Init(Afr2Ui *u){memset(u,0,sizeof(*u));u->node=AFR2_N_HOME;sync_code(u);}
void Afr2Ui_Attach(Afr2Ui *u,const Afr2UiOps *ops){u->ops=*ops;}
static uint8_t invoke(Afr2Ui *u,uint8_t action,int32_t value,const void *data) {
    return u->ops.invoke?u->ops.invoke(u->ops.ctx,action,value,data):2;
}
void Afr2Ui_Open(Afr2Ui *u,uint8_t node) {
    if(node>=AFR2_N_COUNT)return;
    if(u->node==AFR2_N_RADAR_CONFIG){u->radar_selection=u->selection;u->radar_first=u->firstVisible;}
    if(u->node==AFR2_N_QUICK)u->quick_selection=u->selection;
    if(node==AFR2_N_HOME || node==AFR2_N_QUICK)u->depth=0;
    else if(u->depth<8)u->history[u->depth++]=(Afr2UiPosition){u->node,u->selection,u->firstVisible};
    u->node=node;u->selection=node==AFR2_N_QUICK?u->quick_selection:0;u->firstVisible=0;u->edit=0;
    if(node==AFR2_N_RADAR_CONFIG){u->selection=u->radar_selection;u->firstVisible=u->radar_first;}
    if(node==AFR2_N_WIFI_MODE_SELECT)u->selection=u->ops.value?(uint8_t)u->ops.value(u->ops.ctx,AFR2_A_WIFI_MODE)-1u:0;
    if(node==AFR2_N_ENCODER_TEST)u->test_short=u->test_long=u->test_back=0;
    sync_code(u);
}
static void back(Afr2Ui *u,uint8_t fallback) {
    if(u->node==AFR2_N_RADAR_CONFIG){u->radar_selection=u->selection;u->radar_first=u->firstVisible;}
    if(u->depth){Afr2UiPosition p=u->history[--u->depth];u->node=p.node;u->selection=p.selection;u->firstVisible=p.first;sync_code(u);}
    else Afr2Ui_Open(u,fallback);
}
/* AFR2-20261029-20: generic mandatory save/discard dialog. No third option.
 * Caller supplies service actions; persistence completion is asynchronous. */
void Afr2Ui_ConfirmSave(Afr2Ui *u,uint8_t save_action,uint8_t discard_action,uint8_t target){
    if(u->node==AFR2_N_RADAR_CONFIG){u->radar_selection=u->selection;u->radar_first=u->firstVisible;}
    u->confirm_save=save_action;u->confirm_discard=discard_action;
    u->confirm_target=target;u->confirm_wait=0;
    u->node=AFR2_N_SAVE_CONFIRM;u->selection=0;u->firstVisible=0;sync_code(u);
}
static void leave_confirm(Afr2Ui *u){
    uint8_t target=u->confirm_target;u->confirm_wait=0;
    if(target==AFR2_N_HOME || target==AFR2_N_QUICK)Afr2Ui_Open(u,target);else back(u,target);
}
void Afr2Ui_SaveCompleted(Afr2Ui *u){
    if(u->node!=AFR2_N_SAVE_CONFIRM || !u->confirm_wait)return;
    leave_confirm(u);Afr2Ui_NotifyFlash(u,0,PSTR("Cambios"),PSTR("Guardados"),2500);
}
void Afr2Ui_NetworkRestart(Afr2Ui *u){
    uint8_t wifi=(u->node>=AFR2_N_WIFI && u->node<=AFR2_N_WIFI_AP) || u->node==AFR2_N_AP_INFO || u->node==AFR2_N_NETWORK || u->node==AFR2_N_WIFI_DATA || u->node==AFR2_N_WIFI_SEARCH || u->node==AFR2_N_WIFI_MODE_SELECT || u->node==AFR2_N_QR_AP || u->node==AFR2_N_QR_WEB;
    if(!wifi)return;
    wipe(&u->credentials,sizeof(u->credentials));u->keyboard=u->edit=0;u->notices=u->head=u->tail=u->notice_target=0;
    u->depth=0;
    if(u->ops.value && u->ops.value(u->ops.ctx,AFR2_A_HALL_SAVE))Afr2Ui_ConfirmSave(u,AFR2_A_HALL_SAVE,AFR2_A_HALL_CANCEL,AFR2_N_MENU);
    else if(!clean(u))Afr2Ui_ConfirmSave(u,AFR2_A_SAVE,AFR2_A_DISCARD,AFR2_N_MENU);
    else Afr2Ui_Open(u,AFR2_N_MENU);
}
static uint8_t hall_dirty(Afr2Ui *u){return u->ops.value && u->ops.value(u->ops.ctx,AFR2_A_HALL_SAVE)!=0;}
/* AFR2-20261029-19. Invoke the actual explicit return item, without changing
 * selection first or replaying synthetic rotations. Physical input unchanged. */
/* AFR2-20261029-27: one contextual return path for physical release and Web Back. */
static void go_back(Afr2Ui *u){
    if(u->notices){dismiss(u);return;}
    if(u->edit){u->edit=0;u->redraw=true;return;}
    if(u->node==AFR2_N_HOME){Afr2Ui_Open(u,AFR2_N_QUICK);return;}
    Afr2MenuNode n;Afr2Ui_MenuNode(u,&n);
    uint8_t target=u->depth?u->history[u->depth-1u].node:n.parent;
    if((u->node==AFR2_N_HALL_TEST || (u->node==AFR2_N_HALL_VIEW && target!=AFR2_N_HALL_TEST)) && hall_dirty(u)){
        Afr2Ui_ConfirmSave(u,AFR2_A_HALL_SAVE,AFR2_A_HALL_CANCEL,target);return;
    }
    if((u->node==AFR2_N_QUICK || u->node==AFR2_N_RADAR_VIEW || u->node==AFR2_N_US_VIEW) &&
       u->ops.value && u->ops.value(u->ops.ctx,AFR2_A_RADAR_DIRTY)){
        if(u->node==AFR2_N_QUICK)u->quick_selection=u->selection;
        Afr2Ui_ConfirmSave(u,AFR2_A_SAVE,AFR2_A_RADAR_DISCARD,target);return;
    }
    if(!clean(u) && (u->node==AFR2_N_PREFS || u->node==AFR2_N_CALIB || u->node==AFR2_N_ENCODER_CONFIG || u->node==AFR2_N_DISPLAY_MENU || u->node==AFR2_N_MOTOR_CALIB || u->node==AFR2_N_LINE_CALIB || u->node==AFR2_N_RADAR_CONFIG)){
        Afr2Ui_ConfirmSave(u,AFR2_A_SAVE,AFR2_A_DISCARD,target);return;
    }
    if(u->node==AFR2_N_SERVO_ANGLE)(void)invoke(u,AFR2_A_SERVO_EDIT_END,0,0);
    if(u->node==AFR2_N_WHEEL || u->node==AFR2_N_MOVE)(void)invoke(u,AFR2_A_HALL_CANCEL,1,0);
    back(u,target);
}
uint8_t Afr2Ui_BackItem(Afr2Ui *u){
    if(u->node==AFR2_N_SAVE_CONFIRM || u->node==AFR2_N_HALL_PICK || u->node==AFR2_N_HALL_COUNT || u->touch_active || u->keyboard)return 0;
    go_back(u);return 1;
}
void Afr2Ui_Touch(Afr2Ui *u,uint8_t active,uint8_t direction,uint8_t heading) {
    if(active)u->touch_ms=u->clock_ms;
    if(u->touch_active!=active) {
        u->touch_active=active;u->touch_started_ms=u->clock_ms;
        u->touch_full=1;u->redraw=true;++u->generation;++u->navigation_revision;
    }
    if(active && (u->touch_direction!=direction || u->touch_heading!=heading)) {
        u->touch_direction=direction;u->touch_heading=heading;
        u->scroll_redraw=true;++u->generation;
    }
}
/* Inverse nearest-neighbour Q8 rotation: identical C/TS integer operations.
 * Only the 64x40 graphic band changes after entry; bus service sends exact
 * differing chunks using the acknowledged framebuffer already in the driver. */
static void render_touch(Afr2Ui *u,Afr2Oled *o) {
    if(u->power_known && (uint32_t)(u->clock_ms-u->power_changed_ms)<1200u){Afr2Power_Render(o,u->power_percent);u->power_visible=1;u->touch_full=1;return;}
    if(u->touch_full) {
        Afr2Oled_Clear(o);Afr2Component_HeaderFlash(o,AFR2_ICON_MOTOR,PSTR("Control tactil"),0,0);
        if((uint32_t)(u->clock_ms-u->touch_started_ms)<1000u) {
            TXT(o,36,26,"Activado");TXT(o,8,53,"Desde la Web");
        }
        u->touch_full=0;
    }
    if((uint32_t)(u->clock_ms-u->touch_started_ms)<1000u)return;
    Afr2Oled_FillRect(o,32,14,64,38,AFR2_BLACK);
    uint8_t h=u->touch_heading;
    /* AFR2-20261030-12: numeric Web heading accompanies the rotated bitmap. */
    char angle[19];snprintf_P(angle,sizeof(angle),PSTR("Angulo: %u grados"),(unsigned int)h*15u);
    Afr2Oled_FillRect(o,0,53,128,10,AFR2_BLACK);Afr2Oled_Text(o,0,53,angle,AFR2_WHITE);
    int16_t sn=(int16_t)pgm_read_word(&afr2_touch_sin[h]);
    int16_t cs=(int16_t)pgm_read_word(&afr2_touch_sin[(h+6u)%24u]);
    /* AFR2-20261030-15: 36x36 rotation window for the supplied24x22 car. */
    for(int16_t y=-18;y<18;y++)for(int16_t x=-18;x<18;x++) {
        int16_t sx=((int32_t)x*cs-(int32_t)y*sn)/256+AFR2_TOUCH_CAR_WIDTH/2;
        int16_t sy=((int32_t)x*sn+(int32_t)y*cs)/256+AFR2_TOUCH_CAR_HEIGHT/2;
        if(sx>=0 && sx<AFR2_TOUCH_CAR_WIDTH && sy>=0 && sy<AFR2_TOUCH_CAR_HEIGHT &&
           (pgm_read_byte(&afr2_touch_car[sy*AFR2_TOUCH_CAR_STRIDE+sx/8])&(1u<<(sx&7))))
            Afr2Oled_Pixel(o,64+x,32+y,AFR2_WHITE);
    }
    if(u->touch_direction<8) {
        uint8_t d=u->touch_direction*3u;
        int16_t dx=(int16_t)pgm_read_word(&afr2_touch_sin[d]);
        int16_t dy=-(int16_t)pgm_read_word(&afr2_touch_sin[(d+6u)%24u]);
        for(uint8_t j=0;j<3;j++)for(int16_t k=-3;k<=3;k++) {
            int16_t along=14+j*3-(k<0?-k:k);
            int16_t x=64+((int32_t)along*dx-(int32_t)k*dy)/256;
            int16_t y=32+((int32_t)along*dy+(int32_t)k*dx)/256;
            if(x>=32 && x<96 && y>=14 && y<52)Afr2Oled_Pixel(o,x,y,AFR2_WHITE);
        }
    }
}
static void notify_text(Afr2Ui *u,uint8_t kind,const char *title,const char *text,uint16_t ms,uint8_t flash) {
    for(uint8_t i=0;i<u->notices;i++) {
        Afr2Notification *n=&u->notification[(u->tail+i)%4];
        if(n->kind==kind && !(flash&1u?strncmp_P(n->title,title,17):strncmp(n->title,title,17)) && !(flash&2u?strncmp_P(n->text,text,17):strncmp(n->text,text,17)))return;
    }
    /* Queue full: replace latest informational item, never unbounded allocation. */
    if(u->notices==4){u->head=(u->head+3u)%4;--u->notices;}
    Afr2Notification *n=&u->notification[u->head];memset(n,0,sizeof(*n));
    if(flash&1u)strncpy_P(n->title,title,17);else strncpy(n->title,title,17);
    if(flash&2u)strncpy_P(n->text,text,17);else strncpy(n->text,text,17);
    n->ms=ms?ms:3000;n->kind=kind;
    u->head=(u->head+1u)%4;if(!u->notices)u->notice_started_ms=u->clock_ms;
    ++u->notices;++u->generation;u->redraw=true;
}
/* AFR2-20261106-04: storage-space explicit, no additional text buffer. */
void Afr2Ui_Notify(Afr2Ui *u,uint8_t k,const char *title,const char *text,uint16_t ms){notify_text(u,k,title,text,ms,0);}
void Afr2Ui_NotifyFlash(Afr2Ui *u,uint8_t k,const char *title,const char *text,uint16_t ms){notify_text(u,k,title,text,ms,3);}
void Afr2Ui_NotifyFlashTitle(Afr2Ui *u,uint8_t k,const char *title,const char *text,uint16_t ms){notify_text(u,k,title,text,ms,1);}
void Afr2Ui_AfterNotice(Afr2Ui *u,uint8_t node){u->notice_target=node+1u;}
static void dismiss(Afr2Ui *u){
    if(!u->notices)return;
    u->tail=(u->tail+1u)%4;--u->notices;++u->generation;u->notice_started_ms=u->clock_ms;u->redraw=true;
    if(!u->notices && u->notice_target){
        uint8_t node=u->notice_target-1u;u->notice_target=0;
        for(uint8_t i=u->depth;i>0;--i)if(u->history[i-1].node==node){Afr2UiPosition p=u->history[i-1];u->depth=i-1;u->node=p.node;u->selection=p.selection;u->firstVisible=p.first;sync_code(u);return;}
        Afr2Ui_Open(u,node);
    }
}
void Afr2Ui_Tick(Afr2Ui *u,uint32_t now) {
    u->clock_ms=now;
    if(u->ops.value){
        uint8_t signature=!!u->ops.value(u->ops.ctx,AFR2_A_DIRTY);
        signature|=!!u->ops.value(u->ops.ctx,AFR2_A_RADAR_TOGGLE)<<1;
        signature|=!!u->ops.value(u->ops.ctx,AFR2_A_LINE_TOGGLE)<<2;
        signature|=!!u->ops.value(u->ops.ctx,AFR2_A_WHEEL_TOGGLE)<<3;
        signature|=!!u->ops.value(u->ops.ctx,AFR2_A_GOTO_TOGGLE)<<4;
        signature|=(u->ops.value(u->ops.ctx,AFR2_A_WIFI_CONNECTED)==1)<<5;
        uint8_t wifi_signature=(uint8_t)u->ops.value(u->ops.ctx,AFR2_A_WIFI_COUNT)*3u+(uint8_t)u->ops.value(u->ops.ctx,AFR2_A_WIFI_CONNECTED);
        if(wifi_signature!=u->wifi_signature){u->wifi_signature=wifi_signature;sync_code(u);}
        if(signature!=u->menu_signature){u->menu_signature=signature;sync_code(u);}
    }
    if(u->node==AFR2_N_QUICK && u->ops.value){
        uint8_t flags=0;static const uint8_t actions[5] PROGMEM={AFR2_A_RADAR_TOGGLE,AFR2_A_SERVO_TOGGLE,AFR2_A_MOTOR_TOGGLE,AFR2_A_LINE_TOGGLE,AFR2_A_RADAR_TRACE};
        for(uint8_t i=0;i<5;i++)if(u->ops.value(u->ops.ctx,pgm_read_byte(actions+i)))flags|=(1u<<i);
        if(flags!=u->quick_signature){u->quick_signature=flags;u->redraw=true;}
    }
    if(u->quick_pending){
        Afr2MenuItem item;Afr2Menu_Item(AFR2_N_QUICK,u->quick_pending-1u,&item);
        if(u->ops.value && !!u->ops.value(u->ops.ctx,item.action)==u->quick_expected){u->quick_pending=0;u->redraw=true;}
        else if((uint32_t)(now-u->quick_started_ms)>500u){u->quick_pending=0;Afr2Ui_NotifyFlash(u,1,PSTR("Sin confirmar"),PSTR("Ver estado real"),2500);}
    }

    if(u->touch_active) {
        if(u->power_visible && (uint32_t)(now-u->power_changed_ms)>=1200u){u->power_visible=0;u->touch_full=1;u->scroll_redraw=true;}
        if((uint32_t)(now-u->touch_started_ms)>=1000u && (uint32_t)(u->scroll_render_ms-u->touch_started_ms)<1000u) {
            u->touch_full=1;u->scroll_redraw=true;
        }
        u->scroll_render_ms=now;return;
    }
    if(u->notices && (uint32_t)(now-u->notice_started_ms)>=u->notification[u->tail].ms)dismiss(u);
    uint8_t network=u->node==AFR2_N_QR_AP || u->node==AFR2_N_QR_WEB || u->node==AFR2_N_WIFI_DATA || u->node==AFR2_N_HOME || u->node==AFR2_N_NETWORK || u->node==AFR2_N_AP_INFO;
    uint8_t interval=(u->notices || (network && u->node!=AFR2_N_HOME))?100:40;
    if((uint32_t)(now-u->scroll_render_ms)<interval)return;
    u->scroll_render_ms=now;
    if(u->notices || network)u->scroll_redraw=true;
    else if(!u->keyboard && !u->edit && u->node!=AFR2_N_QUICK) {
        Afr2MenuNode node;Afr2Ui_MenuNode(u,&node);
        if(!node.view){Afr2MenuItem item;Afr2Ui_MenuItem(u,u->selection,&item);
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
uint16_t Afr2Ui_TextOffsetFlash(const char *text,uint8_t width,uint32_t elapsed) {
    if(!text)return 0;
    uint16_t pixels=(uint16_t)strlen_P(text)*7u;if(pixels<=width)return 0;
    uint16_t end=pixels-width;
    uint32_t phase=elapsed%(1600UL+(uint32_t)end*40u);
    if(phase<=800u)return 0;
    uint32_t offset=(phase-800u)/40u;return offset>end?end:(uint16_t)offset;
}
uint32_t Afr2Ui_ReportedScreen(const Afr2Ui *u) {
    if((u->edit && u->editing.action==AFR2_A_WHEEL) || u->node==AFR2_N_HALL_PICK)return AFR2_SCREEN_MOTOR_SELECTOR;
    if(u->edit && u->editing.action==AFR2_A_POWER)return 0x030040UL;
    if(u->touch_active)return AFR2_SCREEN_TOUCH;
    if(u->notices){uint8_t kind=u->notification[u->tail].kind;
        if(kind>AFR2_NOTICE_NETWORK && kind<=AFR2_NOTICE_NETWORK+AFR2_NET_EVENT_MAX)return 0x040000UL+kind-AFR2_NOTICE_NETWORK;}
    return u->screen;
}
static void action_result(Afr2Ui *u,uint8_t code,uint8_t action){
    const char *why=u->ops.reason_flash?u->ops.reason_flash(u->ops.ctx,action):PSTR("Ver requisitos");
    if(!code)Afr2Ui_NotifyFlash(u,0,PSTR("Operacion"),PSTR("Aceptada"),3000);
    else Afr2Ui_NotifyFlash(u,1,PSTR("No disponible"),code==1?PSTR("Dato incorrecto"):why,3000);
}
static void result(Afr2Ui *u,uint8_t code){
    Afr2MenuItem item;Afr2Ui_MenuItem(u,u->selection,&item);action_result(u,code,item.action);
}
static void keyboard(Afr2Ui *u,Afr2UiInput in) {
    if(in.menu){wipe(&u->credentials,sizeof(u->credentials));u->keyboard=0;u->notices=0;u->head=u->tail=0;u->redraw=true;return;}
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
void Afr2Ui_InputHeld(Afr2Ui *u,uint8_t held,uint32_t now){
    if(held && !u->dashboard.held)u->hold_started_ms=now;
    uint16_t threshold=u->ops.value?(uint16_t)u->ops.value(u->ops.ctx,AFR2_A_LONG_MS):800;if(threshold<800)threshold=800;
    uint32_t amount=held?(uint32_t)(now-u->hold_started_ms)*16u/threshold:0;
    uint8_t progress=amount>16?16:amount;
    if(progress!=u->hold_progress || u->dashboard.held!=held){
        uint8_t completed=u->hold_progress>=16;
        u->hold_progress=progress;
        if(held || !completed)u->redraw=true; /* release: repaint underneath unless long press navigates away */
    }
    if(held || u->dashboard.held!=held){u->dashboard.input_ms=now;u->dashboard.armed=0;}
    u->dashboard.held=held;
}
void Afr2Ui_Apply(Afr2Ui *u,Afr2UiInput in) {
    if(in.rotation || in.click || in.menu){u->dashboard.input_ms=u->clock_ms;u->dashboard.armed=0;}
    if(u->touch_active){if(in.menu){u->touch_cancelled=1;invoke(u,AFR2_A_STOP,0,0);Afr2Ui_Touch(u,0,8,0);}return;}
    if(u->node==AFR2_N_ENCODER_TEST){
        if(in.rotation){u->test_back=1;u->redraw=true;}
        if(in.click){if(u->test_back){go_back(u);return;}u->test_short=1;u->redraw=true;}
        if(in.menu){if(u->test_long){go_back(u);return;}u->test_long=1;u->redraw=true;}
        return;
    }
    if(in.rotation || in.click || in.menu)++u->generation;
    if(in.rotation)u->encoder_turns+=in.rotation;
    if(in.click && u->encoder_clicks!=UINT16_MAX)++u->encoder_clicks;
    if(in.menu && u->encoder_holds!=UINT16_MAX)++u->encoder_holds;
    if(u->node==AFR2_N_SAVE_CONFIRM){
        if(u->notices){if(in.click)dismiss(u);return;}
        if(u->confirm_wait)return;
        if(in.rotation%2){u->selection^=1u;u->redraw=true;}
        /* Long cannot bypass a required Yes/No decision. */
        if(in.click){
            uint8_t r=invoke(u,u->selection?u->confirm_discard:u->confirm_save,(u->selection && u->confirm_save==AFR2_A_HALL_SAVE)?2:0,0);
            if(r){Afr2Ui_NotifyFlash(u,1,PSTR("No completado"),PSTR("Reintentar"),2500);return;}
            if(!u->selection){u->confirm_wait=1;u->redraw=true;}
            else {leave_confirm(u);Afr2Ui_NotifyFlash(u,0,PSTR("Cambios"),PSTR("Descartados"),2500);}
        }return;
    }
    if(u->node==AFR2_N_HALL_PICK || u->node==AFR2_N_HALL_COUNT){
        if(in.menu){
            (void)invoke(u,AFR2_A_HALL_CANCEL,1,0);
            u->notices=u->head=u->tail=0;back(u,AFR2_N_HALL_TEST);
            Afr2Ui_NotifyFlash(u,0,PSTR("Operacion"),PSTR("Cancelada"),2500);return;
        }
        if(u->notices){if(in.click)dismiss(u);return;}
        if(u->node==AFR2_N_HALL_PICK){
            if(in.rotation){u->selection=Afr2MotorSelector_Step(u->selection,in.rotation);u->redraw=true;}
            if(in.click){
                uint8_t r=invoke(u,AFR2_A_WHEEL,u->selection,0);
                if(!r)r=invoke(u,AFR2_A_HALL_BEGIN,0,0);
                if(r){Afr2Ui_NotifyFlash(u,1,PSTR("No disponible"),PSTR("Ver requisitos"),2500);return;}
                u->node=AFR2_N_HALL_COUNT;u->selection=0;sync_code(u);
            }
        }else if(in.click){
            uint8_t r=invoke(u,AFR2_A_HALL_END,0,0);
            if(r){Afr2Ui_NotifyFlash(u,1,PSTR("Operacion"),PSTR("Medicion invalida"),2500);return;}
            Afr2Ui_ConfirmSave(u,AFR2_A_HALL_SAVE,AFR2_A_HALL_CANCEL,AFR2_N_HALL_TEST);
        }return;
    }
    if(u->keyboard){
        if(u->notices && !in.menu){if(in.click)dismiss(u);return;}
        keyboard(u,in);return;
    }
    /* AFR2-20261106-09: on QR, network notices (e.g. the scanning phone joining the AP) kept every
     * press busy dismissing them. Short or long always leaves; queued informative notices are dropped. */
    if((u->node==AFR2_N_QR_AP || u->node==AFR2_N_QR_WEB) && (in.click || in.menu)){
        u->notices=u->head=u->tail=u->notice_target=0;go_back(u);return;
    }
    if(in.menu){go_back(u);return;}
    if(u->notices){if(in.click)dismiss(u);return;}
    /* AFR2-20261029-29: short opens full menu; long-release opens quick cards. */
    if(u->node==AFR2_N_HOME){if(in.click)Afr2Ui_Open(u,AFR2_N_MENU);return;}
    if(u->node==AFR2_N_WIFI_DATA || u->node==AFR2_N_QR_AP || u->node==AFR2_N_QR_WEB){if(in.click)go_back(u);return;}
    if(u->node==AFR2_N_NETWORK){
        if(in.rotation){int16_t page=u->selection+in.rotation;u->selection=page<0?0:page>1?1:page;u->redraw=true;}
        if(in.click){go_back(u);}return;
    }
    if(u->node==AFR2_N_WIFI_MODE_SELECT){
        if(in.rotation){int16_t mode=u->selection+in.rotation;u->selection=mode<0?0:mode>2?2:mode;u->redraw=true;}
        if(in.click){uint8_t rc=invoke(u,AFR2_A_WIFI_CHANGE_MODE,u->selection+1u,0);if(rc)action_result(u,rc,AFR2_A_WIFI_CHANGE_MODE);else Afr2Ui_NotifyFlash(u,0,PSTR("Cambiar modo"),PSTR("Esperando ESP"),1500);}
        return;
    }
    if(u->node==AFR2_N_CAPS){
        if(in.rotation){int16_t page=(int16_t)u->selection+in.rotation;if(page<0)page=0;if(page>5)page=5;u->selection=(uint8_t)page;u->redraw=true;}
        if(in.click)go_back(u);
        return;
    }
    if(u->node==AFR2_N_QUICK){
        if(u->quick_pending)return;
        if(in.rotation){int16_t index=(int16_t)u->selection+in.rotation;if(index<0)index=0;if(index>6)index=6;u->selection=(uint8_t)index;u->quick_selection=u->selection;u->scroll_started_ms=u->clock_ms;u->redraw=true;}
        if(in.click){
            if(u->selection==5){Afr2Ui_Open(u,AFR2_N_MENU);return;}
            if(u->selection==6){Afr2Ui_Open(u,AFR2_N_QUICK_MODES);return;}
            Afr2MenuItem item;Afr2Ui_MenuItem(u,u->selection,&item);
            uint8_t expected=!(u->ops.value && u->ops.value(u->ops.ctx,item.action));
            uint8_t r=invoke(u,item.action,expected,0);
            if(r)result(u,r);
            else {u->quick_pending=u->selection+1u;u->quick_expected=expected;u->quick_started_ms=u->clock_ms;u->redraw=true;}
        }return;
    }
    if(u->edit) {
        if(in.rotation){if(u->editing.action==AFR2_A_WHEEL)u->draft=Afr2MotorSelector_Step((uint8_t)u->draft,in.rotation);else u->draft+=(int32_t)in.rotation*u->editing.step;
            if(u->draft<u->editing.min)u->draft=u->editing.min;
            if(u->draft>u->editing.max)u->draft=u->editing.max;
            u->redraw=true;}
        if(in.click){uint8_t r=invoke(u,u->editing.action,u->draft,0);u->edit=0;u->redraw=true;result(u,r);}return;
    }
    if(u->node==AFR2_N_RADAR_VIEW || u->node==AFR2_N_US_VIEW || u->node==AFR2_N_LINE_VIEW || u->node==AFR2_N_IR_VALUES || u->node==AFR2_N_SERVO_ANGLE){
        if(in.click){if(u->node==AFR2_N_RADAR_VIEW || u->node==AFR2_N_US_VIEW)Afr2Ui_Open(u,AFR2_N_RADAR_OPTIONS);return;}
        if(in.rotation){
            if(u->node==AFR2_N_SERVO_ANGLE){/* AFR2-20261106-35: sentido invertido a pedido del usuario (encoder opuesto al anterior). */int32_t angle=(u->ops.value?u->ops.value(u->ops.ctx,AFR2_A_SERVO_ANGLE):90)-in.rotation;
                if(angle<0)angle=0;
                if(angle>180)angle=180;
                uint8_t r=invoke(u,AFR2_A_SERVO_ANGLE,angle,0);if(r)action_result(u,r,AFR2_A_SERVO_ANGLE);u->redraw=true;}
            else if((in.rotation>0 && (u->node==AFR2_N_RADAR_VIEW || u->node==AFR2_N_LINE_VIEW)) || (in.rotation<0 && (u->node==AFR2_N_US_VIEW || u->node==AFR2_N_IR_VALUES))){u->node=u->node==AFR2_N_RADAR_VIEW?AFR2_N_US_VIEW:u->node==AFR2_N_US_VIEW?AFR2_N_RADAR_VIEW:u->node==AFR2_N_LINE_VIEW?AFR2_N_IR_VALUES:AFR2_N_LINE_VIEW;sync_code(u);}
        }return;
    }
    Afr2MenuNode n;Afr2Ui_MenuNode(u,&n);
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
        Afr2MenuItem i;Afr2Ui_MenuItem(u,u->selection,&i);
        if(i.action && u->ops.available){uint8_t r=u->ops.available(u->ops.ctx,i.action,i.arg);if(r){result(u,r);return;}}
        if(!i.action) {
            if(!strncmp(i.label,"Volver",6)){go_back(u);}
            else {
                if(i.target==AFR2_N_SERVO_ANGLE){uint8_t r=invoke(u,AFR2_A_SERVO_EDIT_BEGIN,0,0);if(r){action_result(u,r,AFR2_A_SERVO_EDIT_BEGIN);return;}}
                Afr2Ui_Open(u,i.target);
                if(i.target==AFR2_N_WIFI_SEARCH){uint8_t rc=invoke(u,AFR2_A_WIFI_SCAN,0,0);if(rc)result(u,rc);}
                if(i.target==AFR2_N_RADAR_RANGE && u->ops.value){u->selection=(uint8_t)u->ops.value(u->ops.ctx,AFR2_A_RADAR_RANGE);u->firstVisible=u->selection>2?u->selection-2:0;}
            }
        } else if(i.action==AFR2_A_WIFI_NETWORK){
            uint8_t rc=invoke(u,i.action,i.arg,0);if(rc)action_result(u,rc,i.action);else Afr2Ui_Open(u,AFR2_N_WIFI_SETUP);
        } else if(i.action==AFR2_A_WIFI_WEB){
            uint8_t rc=invoke(u,i.action,0,0);if(rc)action_result(u,rc,i.action);else u->redraw=true;
        } else if(i.action==AFR2_A_ESP_TEST){
            uint8_t r=invoke(u,i.action,0,0);if(r)action_result(u,r,i.action);else u->redraw=true;
        } else if(i.action==AFR2_A_QUICK_MODE){
            uint8_t r=invoke(u,i.action,i.arg,0);
            if(r)action_result(u,r,i.action);
            else Afr2Ui_NotifyFlash(u,0,PSTR("Modo seleccionado"),i.arg?PSTR("Manual"):PSTR("Reposo"),2500);
        } else if(i.action==AFR2_A_RADAR_RANGE){
            uint8_t r=invoke(u,i.action,i.arg,0);if(r)result(u,r);else back(u,AFR2_N_RADAR_OPTIONS);
        } else if(i.action==AFR2_A_HALL_BEGIN){
            Afr2Ui_Open(u,AFR2_N_HALL_PICK);
            u->selection=u->ops.value?(uint8_t)u->ops.value(u->ops.ctx,AFR2_A_WHEEL):0;
        } else if(i.action==AFR2_A_HALL_SAVE){
            uint8_t r=invoke(u,i.action,0,0);u->redraw=true;
            if(r)result(u,r); /* Completion notice follows actual EEPROM commit. */
        } else if(i.action==AFR2_A_KEYBOARD) {
            wipe(&u->credentials,sizeof(u->credentials));
            if(u->ops.keyboard_seed)u->ops.keyboard_seed(u->ops.ctx,&u->credentials);
            u->keyboard=1;u->keyboard_field=0;u->length=strlen(u->credentials.ssid);u->key=0;u->redraw=true;
        } else if(i.step){u->editing=i;u->edit=1;u->draft=u->ops.value?u->ops.value(u->ops.ctx,i.action):i.min;u->redraw=true;}
        else {uint8_t r=invoke(u,i.action,i.arg,0);u->redraw=true;result(u,r);}
    }
}
static void render_keyboard(Afr2Ui *u,Afr2Oled *o) {
    Afr2Component_HeaderFlash(o,AFR2_ICON_WIFI,u->keyboard_field?PSTR("Clave Wi-Fi"):PSTR("SSID Wi-Fi"),0,0);
    char text[18];uint8_t length=u->length<17?u->length:17;
    if(u->keyboard_field){memset(text,'*',length);text[length]=0;}
    else {memcpy(text,u->credentials.ssid+u->length-length,length);text[length]=0;}
    Afr2Oled_Text(o,0,16,text,AFR2_WHITE);
    uint8_t first=(u->key/10u)/2u*20u;
    for(uint8_t j=0;j<20 && first+j<97;j++) {
        uint8_t k=first+j,x=(j%10u)*10u,y=28u+(j/10u)*12u;
        char ch[2]={(char)(32u+k),0};
        if(k<95)Afr2Oled_Text(o,x,y,ch,AFR2_WHITE);
        else if(k==95)TXT(o,x,y,"<");else TXT(o,x,y,"OK");
        if(k==u->key)Afr2Oled_Rect(o,x-1,y-1,10,12,AFR2_INVERT);
    }
    Afr2Ui_GestureHint(o,0,53,1,PSTR("Cancelar"));
}
static void wheel_name(char *text,uint8_t wheel){
    static const char names[][3] PROGMEM={"FL","FR","RL","RR"};
    memcpy_P(text,names[wheel&3u],3);
}
static void render_hall(Afr2Ui *u,Afr2Oled *o,const Afr2UiSnapshot *s){
    char text[19],name[3];
    if(u->node==AFR2_N_HALL_PICK){
        Afr2MotorSelector_Render(o,u->selection);
    }else if(u->node==AFR2_N_HALL_COUNT){
        Afr2Component_HeaderFlash(o,AFR2_ICON_MOTOR,PSTR("Gire CW 1 vuelta"),0,0);
        Afr2Oled_Text(o,0,16,s->lines[0],AFR2_WHITE);
        Afr2Oled_Text(o,0,28,s->lines[1],AFR2_WHITE);
        Afr2Ui_GestureHint(o,0,40,0,PSTR("Finalizar"));Afr2Ui_GestureHint(o,0,53,1,PSTR("Cancelar"));
    }else {
        TXT(o,0,0,"Estado Hall");
        for(uint8_t i=0;i<4;i++){
            wheel_name(name,i);
            if(s->hall_cpr[i])snprintf_P(text,sizeof(text),PSTR("%s:%lu %s"),name,(unsigned long)s->hall_cpr[i],(s->hall_pending&(1u<<i))?"*":"OK");
            else snprintf_P(text,sizeof(text),PSTR("%s:Sin calibrar"),name);
            Afr2Oled_Text(o,0,12+i*10,text,AFR2_WHITE);
        }
        TXT(o,0,54,"*Nuevo");Afr2Ui_GestureHint(o,94,52,1,PSTR("Volver"));
    }
}
static void render_save_confirm(Afr2Ui *u,Afr2Oled *o,const Afr2UiSnapshot *s){
    if(u->confirm_save==AFR2_A_HALL_SAVE){
        Afr2Component_Header(o,AFR2_ICON_MOTOR,s->lines[0],0,0);
        Afr2Oled_Text(o,0,16,s->lines[1],AFR2_WHITE);
        if(u->confirm_wait){TXT(o,0,34,"Guardando...");return;}
        Afr2Icon_Draw(o,AFR2_ICON_PREFS,0,33,12);TXT(o,16,34,"Confirmar");
        Afr2Icon_Draw(o,AFR2_ICON_BACK,0,48,12);TXT(o,16,49,"Descartar");
        Afr2Oled_FillRect(o,0,u->selection?47:32,128,14,AFR2_INVERT);return;
    }
    Afr2Component_HeaderFlash(o,AFR2_ICON_PREFS,PSTR("Guardar cambios"),0,0);
    if(u->confirm_wait){TXT(o,0,26,"Guardando...");return;}
    TXT(o,0,16,"Desea guardarlos?");
    TXT(o,14,34,"Si");TXT(o,14,49,"No");
    Afr2Oled_FillRect(o,0,u->selection?47:32,128,14,AFR2_INVERT);
}
static void centered(Afr2Oled *o,uint8_t y,const char *text){
    uint16_t width=strlen_P(text)*7u;Afr2Oled_TextFlash(o,width<128?(128-width)/2:0,y,text,AFR2_WHITE);
}
static void render_quick(Afr2Ui *u,Afr2Oled *o,const Afr2UiSnapshot *s){
    Afr2MenuItem item;Afr2Menu_Item(AFR2_N_QUICK,u->selection,&item);Afr2Oled_Clear(o);
    char title[12];static const char titles[7][12] PROGMEM={"Radar","Servo","Motores","Linea","Rastro","Ajustes","Modo"};strcpy_P(title,titles[u->selection]);
    Afr2Component_Header(o,Afr2Menu_Icon(AFR2_N_QUICK,item.target,item.action,item.label),title,u->selection+1,7);Afr2Component_Panel(o,0,16,128,34);
    uint8_t on=!!(s->quick_flags&(1u<<u->selection));
    if(u->selection==5 || u->selection==6){centered(o,20,u->selection==5?PSTR("AJUSTES Y PRUEBAS"):PSTR("MODO DEL ROBOT"));Afr2Ui_GestureHint(o,28,36,0,PSTR("Entrar"));}
    else {
        const char *state=u->selection==0?(on?PSTR("ACTIVO"):PSTR("DETENIDO")):u->selection==2?(on?PSTR("HABILITADOS"):PSTR("BLOQUEADOS")):u->selection==3?(on?PSTR("EN MARCHA"):PSTR("DETENIDO")):u->selection==4?(on?PSTR("VISIBLE"):PSTR("OCULTO")):(on?PSTR("ACTIVADO"):PSTR("DESACTIVADO"));
        centered(o,20,state);
        if(u->quick_pending)centered(o,36,u->quick_expected?PSTR("Activando..."):PSTR("Desactivando..."));else Afr2Ui_GestureHint(o,28,36,0,u->selection==2?(on?PSTR("Bloquear"):PSTR("Habilitar")):u->selection==4?(on?PSTR("Ocultar"):PSTR("Mostrar")):(on?PSTR("Detener"):PSTR("Activar")));
    }
    Afr2Oled_FillRect(o,3,33,122,15,AFR2_INVERT);
    Afr2Icon_Draw(o,AFR2_ICON_ENCODER_TURN,2,50,13);Afr2Ui_GestureHint(o,94,52,1,PSTR("Volver"));
}
static void render_caps(Afr2Ui *u,Afr2Oled *o,const Afr2UiSnapshot *s){
    Afr2Oled_Clear(o);Afr2Component_HeaderFlash(o,AFR2_ICON_INFO,PSTR("Capacidades"),u->selection+1,6);
    for(uint8_t i=0;i<3;i++)Afr2Oled_Text(o,0,16+i*12,s->lines[i],AFR2_WHITE);
    Afr2Ui_GestureHint(o,0,52,0,PSTR("OK"));Afr2Ui_GestureHint(o,94,52,1,PSTR("Volver"));
}
void Afr2Power_Render(Afr2Oled *o,uint8_t percent){
    char text[19];if(percent>100)percent=100;Afr2Oled_Clear(o);
    Afr2Component_HeaderFlash(o,AFR2_ICON_MOTOR,PSTR("Potencia"),0,0);snprintf_P(text,sizeof(text),PSTR("Valor: %u /255"),((uint16_t)percent*255u+50u)/100u);Afr2Oled_Text(o,0,15,text,AFR2_WHITE);
    Afr2Component_Progress(o,30,1,percent,0);
    snprintf_P(text,sizeof(text),PSTR("%u%%"),percent);Afr2Oled_Text(o,52,43,text,AFR2_WHITE);Afr2Icon_Draw(o,AFR2_ICON_ENCODER_TURN,0,47,16);
}
static void render_screen(Afr2Ui *u,Afr2Oled *o,const Afr2UiSnapshot *s) {
    if(u->node==AFR2_N_HOME && !u->notices && !u->touch_active){
        if(Afr2Dashboard_Render(u,o,s)){++u->revision;if(!u->revision)++u->revision;}
        u->redraw=false;u->scroll_redraw=false;return;
    }
    u->dashboard.valid=0;
    if(u->touch_active) {
        render_touch(u,o);++u->revision;if(!u->revision)++u->revision;
        u->redraw=false;u->scroll_redraw=false;return;
    }
    if(!u->notices && (u->node==AFR2_N_QR_AP || u->node==AFR2_N_QR_WEB)){
        Afr2Qr_Render(o,s,u->node==AFR2_N_QR_AP,u->revision);++u->revision;if(!u->revision)++u->revision;u->redraw=false;u->scroll_redraw=false;return;
    }
    Afr2MenuNode n;Afr2Ui_MenuNode(u,&n);
    if(u->selection>=n.count && u->node!=AFR2_N_CAPS && u->node!=AFR2_N_NETWORK){u->selection=n.count-1;u->firstVisible=u->selection>2?u->selection-2:0;sync_code(u);}
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
            Afr2Ui_GestureHint(o,0,53,0,PSTR("Cerrar"));Afr2Ui_GestureHint(o,66,53,1,PSTR("Cerrar"));
        }
    } else if(u->node==AFR2_N_QUICK)render_quick(u,o,s);
    else if(u->node==AFR2_N_ENCODER_TEST){
        TXT(o,0,0,"Prueba encoder");Afr2Ui_GestureHint(o,0,14,2,PSTR("Elegir Volver"));
        Afr2Ui_GestureHint(o,0,27,0,u->test_short?PSTR("Detectado"):PSTR("Pendiente"));
        Afr2Ui_GestureHint(o,0,40,1,u->test_long?PSTR("Salir"):PSTR("Pendiente"));
        Afr2Ui_GestureHint(o,0,53,0,PSTR("Volver"));if(u->test_back)Afr2Oled_Rect(o,0,50,128,14,AFR2_WHITE);
    }
    else if(u->node==AFR2_N_POWER){if(u->power_known)Afr2Power_Render(o,u->power_percent);else {TXT(o,0,0,"Potencia manual");TXT(o,0,25,"Sin valor vigente");}}
    else if(u->node==AFR2_N_WIFI_MODE_SELECT){
        Afr2Component_HeaderFlash(o,AFR2_ICON_WIFI,PSTR("Cambiar modo"),0,0);TXT(o,0,16,"Opciones de modos");TXT(o,3,28,"STA");TXT(o,45,28,"AP");TXT(o,80,28,"AP-STA");
        Afr2Oled_FillRect(o,u->selection==0?0:u->selection==1?40:78,40,u->selection==2?49:31,2,AFR2_WHITE);
        Afr2Ui_GestureHint(o,0,53,0,PSTR("Elegir"));Afr2Ui_GestureHint(o,66,53,1,PSTR("Volver"));
    }
    else if(u->node==AFR2_N_WIFI_DATA){
        Afr2Component_HeaderFlash(o,AFR2_ICON_WIFI,PSTR("Datos de red"),0,0);if(s->labels[0][0])Afr2Oled_TextWindow(o,0,128,16,s->labels[0],Afr2Ui_TextOffset(s->labels[0],128,u->clock_ms-u->scroll_started_ms),AFR2_WHITE);else {TXT(o,0,16,"Sin datos actuales");}
        if(s->labels[1][0]){Afr2Oled_TextWindow(o,0,128,28,s->labels[1],Afr2Ui_TextOffset(s->labels[1],128,u->clock_ms-u->scroll_started_ms),AFR2_WHITE);Afr2Oled_TextWindow(o,0,128,40,s->labels[2],Afr2Ui_TextOffset(s->labels[2],128,u->clock_ms-u->scroll_started_ms),AFR2_WHITE);}else Afr2Oled_Text(o,0,28,s->lines[1],AFR2_WHITE);if(s->row_security[0]>=2)Afr2Icon_Draw(o,AFR2_ICON_LOCK,0,39,12);Afr2Oled_Text(o,s->row_security[0]>=2?14:0,40,s->lines[2],AFR2_WHITE);
        Afr2Ui_GestureHint(o,0,53,0,PSTR("Volver"));Afr2Ui_GestureHint(o,66,53,1,PSTR("Volver"));
    }
    else if(u->node==AFR2_N_ENCODER_VIEW){
        Afr2Component_HeaderFlash(o,AFR2_ICON_ENCODER,PSTR("Estado encoder"),0,0);
        for(uint8_t i=0;i<3;i++){Afr2Icon_Draw(o,i==0?AFR2_ICON_ENCODER_TURN:i==1?AFR2_ICON_ENCODER:AFR2_ICON_ENCODER_LONG,0,16+12*i,10);Afr2Oled_Text(o,14,16+12*i,s->lines[i],AFR2_WHITE);}
        Afr2Ui_GestureHint(o,0,53,1,PSTR("Volver"));
    }
    else if(u->node==AFR2_N_CAPS)render_caps(u,o,s);
    else if(u->node==AFR2_N_SAVE_CONFIRM)render_save_confirm(u,o,s);
    else if(u->node==AFR2_N_HALL_PICK || u->node==AFR2_N_HALL_COUNT || u->node==AFR2_N_HALL_VIEW)render_hall(u,o,s);
    else if(u->keyboard)render_keyboard(u,o);
    else if(u->node==AFR2_N_SEQUENCE)Afr2SequenceScreen_Render(o,s);
    else if(u->node==AFR2_N_NETWORK || u->node==AFR2_N_AP_INFO)Afr2NetworkScreen_Render(o,u,s);
    else if(u->node==AFR2_N_RADAR_VIEW || u->node==AFR2_N_US_VIEW || u->node==AFR2_N_LINE_VIEW || u->node==AFR2_N_IR_VALUES || u->node==AFR2_N_SERVO_ANGLE)Afr2SensorScreen_Render(o,u->node,s);
    else if(u->edit && u->editing.action==AFR2_A_WHEEL)Afr2MotorSelector_Render(o,(uint8_t)u->draft);
    else if(u->edit && u->editing.action==AFR2_A_POWER)Afr2Power_Render(o,(uint8_t)u->draft);
    else if(u->edit) {
        char value[18];Afr2Component_Header(o,AFR2_ICON_CONFIG,u->editing.label,0,0);
        snprintf_P(value,sizeof(value),PSTR("%ld"),(long)u->draft);Afr2Oled_Text(o,0,16,value,AFR2_WHITE);Afr2Ui_GestureHint(o,0,28,2,PSTR("Ajustar"));
        Afr2Ui_GestureHint(o,0,40,0,PSTR("Aplicar"));Afr2Ui_GestureHint(o,0,53,1,PSTR("Descartar"));
    } else {
        if(!partial) {
            char title[18];strncpy(title,n.title,17);title[17]=0;
            if(u->node==AFR2_N_WIFI)snprintf_P(title,sizeof(title),PSTR("Wi-Fi %s"),s->network_mode==1?"STA":s->network_mode==2?"AP":s->network_mode==3?"AP-STA":"--");
            if(u->node==AFR2_N_WIFI_SEARCH)strcpy_P(title,s->network_scan_state==1?PSTR("Buscando redes"):s->network_scan_state==3?PSTR("Busqueda fallo"):s->network_scan_state==0?PSTR("Buscar redes"):s->network_total?PSTR("Redes listas"):PSTR("Sin redes"));
            if(u->node==AFR2_N_WIFI_NETWORKS)snprintf_P(title,sizeof(title),PSTR("Redes %u/%u%s"),s->network_page+1,s->network_total>8?2:1,s->network_truncated?"+":"");
            Afr2Component_Header(o,Afr2Menu_Icon(u->node,u->node,0,title),u->node==AFR2_N_WIFI_VIEW && s->lines[0][0]?s->lines[0]:title,0,0);
            if(n.view){for(uint8_t j=0;j<3;j++)Afr2Oled_TextWindow(o,0,j==2?104:128,16+j*12,s->lines[j],j==2?Afr2Ui_TextOffset(s->lines[j],104,u->clock_ms-u->scroll_started_ms):0,AFR2_WHITE);}
        }
        uint8_t count=n.view?1:3;
        for(uint8_t row=0;row<count && u->firstVisible+row<n.count;row++) {
            Afr2MenuItem item;Afr2Ui_MenuItem(u,u->firstVisible+row,&item);
            uint8_t selected=u->firstVisible+row==u->selection;
            if(partial && !selected)continue;
            if(n.view && !strncmp(item.label,"Volver",6)){if(!partial)Afr2Ui_GestureHint(o,94,52,1,PSTR("Volver"));continue;}
            uint8_t y=n.view?52:(16+16*row),left=n.view?16:23,top=n.view?y:y+3;
            if(!partial) {
                Afr2Icon_Draw(o,Afr2Menu_Icon(u->node,item.target,item.action,item.label),n.view?0:3,n.view?51:y,n.view?12:16);
                
                if(row!=2 && !(u->node==AFR2_N_WIFI_NETWORKS && s->row_security[row]>=2))Afr2Oled_Text(o,120,top,selected?(s->disabled[row]?"!":(!strncmp(item.label,"Volver",6)?" ":">")):(s->disabled[row]?"!":" "),AFR2_WHITE);
            }
            uint8_t right=(u->node==AFR2_N_WIFI_NETWORKS && s->row_security[row]>=2)?108:118;if(row==2 && right>104)right=104;
            if(u->node==AFR2_N_WIFI_NETWORKS && s->row_security[row]>=2){
                uint8_t lockx=row==2?92:111;if(row==2)right=89;
                Afr2Oled_FillRect(o,lockx,top,10,10,AFR2_BLACK);Afr2Icon_Draw(o,AFR2_ICON_LOCK,lockx,top,10);
                if(partial && selected && !n.view)Afr2Oled_FillRect(o,lockx,top,10,10,AFR2_INVERT);
            }
            const char *label=s->labels[row][0]?s->labels[row]:item.label;
            Afr2Oled_FillRect(o,left,top,right-left,10,partial && selected && !n.view?AFR2_WHITE:AFR2_BLACK);
            Afr2Oled_TextWindow(o,left,right,top,label,selected?Afr2Ui_TextOffset(label,right-left,u->clock_ms-u->scroll_started_ms):0,partial && selected && !n.view?AFR2_BLACK:AFR2_WHITE);
            if(!partial && selected && !n.view)Afr2Oled_FillRect(o,0,y,128,16,AFR2_INVERT);
        }
    }
    ++u->revision;if(!u->revision)++u->revision;u->redraw=false;u->scroll_redraw=false;
}

/* AFR2-20261106-01: fixed gesture slot; QR quiet zones remain untouched. */
void Afr2Ui_Render(Afr2Ui *u,Afr2Oled *o,const Afr2UiSnapshot *s){
    render_screen(u,o,s);
    if(u->node==AFR2_N_QR_AP || u->node==AFR2_N_QR_WEB)return;
    static const int8_t ring[16][2] PROGMEM={{0,-9},{3,-8},{6,-6},{8,-3},{9,0},{8,3},{6,6},{3,8},{0,9},{-3,8},{-6,6},{-8,3},{-9,0},{-8,-3},{-6,-6},{-3,-8}};
    /* AFR2-20261106-07: only while the encoder is pressed; no frame. Released -> InputHeld forces a full
     * redraw that wipes this overlay (skipped after a completed long press: the target page repaints). */
    if(!u->dashboard.held)return;
    for(int8_t dy=-11;dy<=11;dy++){ /* disc of ring radius (9) + 2 px cleared */
        int8_t dx=0;while((dx+1)*(dx+1)+dy*dy<=121)++dx;
        Afr2Oled_FillRect(o,117-dx,54+dy,2*dx+1,1,AFR2_BLACK);
    }
    Afr2Icon_Draw(o,AFR2_ICON_ENCODER_LONG,112,49,11);
    for(uint8_t i=0;i<16;i++){int8_t x=pgm_read_byte(&ring[i][0]),y=pgm_read_byte(&ring[i][1]);Afr2Oled_Pixel(o,117+x,54+y,AFR2_WHITE);if(i<u->hold_progress)Afr2Oled_FillRect(o,116+x,53+y,2,2,AFR2_WHITE);}
}
