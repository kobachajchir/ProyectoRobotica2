/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
#include "afr2_build_info.h"
#include <avr/pgmspace.h>
/* AFR2-20261023-19. New opt-in endpoint, all offsets encoded explicitly LE.
 * ESP integration documented in Contracts; no ESP source edited here.
 */
#include "afr2_app.h"
#include "afr2_debug.h"
#include "afr2_product_wire.h"
#include <stdio.h>
#include <string.h>
static uint8_t wire_result(uint8_t result) {
    return result==AFR2_OK?AFR2_RESULT_OK:result==AFR2_BAD?AFR2_RESULT_BAD_REQUEST:
        result==AFR2_FAILED?AFR2_RESULT_INTERNAL:AFR2_RESULT_UNAVAILABLE;
}
/* Reserved prefix is zero in v7; no token or local enable switch. */
static uint8_t web_command(Afr2Product *p,const uint8_t *d) {
    if(afr2_get_u32(d)!=0 || p->hall_measuring || (!p->stopped && p->origin==AFR2_SOURCE_LOCAL))return 0;
    Afr2Product_WebPulse(p,0);p->origin=AFR2_SOURCE_WEB;return 1;
}
static uint32_t features(const Afr2Product *p) {
    uint32_t f=AFR2_EXT_F_TOUCH_UI|AFR2_EXT_F_UI|AFR2_EXT_F_STOP|AFR2_EXT_F_WIFI|AFR2_EXT_F_PREFS|AFR2_EXT_F_CONFIG|AFR2_EXT_F_SYSTEM|AFR2_EXT_F_NETWORK_UI|AFR2_EXT_F_SENSOR_HEAD|AFR2_EXT_F_RADAR;
    f|=AFR2_EXT_F_BATTERY;
    f|=AFR2_EXT_F_MANUAL_PWM; /* AFR2-20261106: Conducir manual siempre disponible. */
    if(p->config.motor_ready && !p->fault && !p->hall_measuring)f|=AFR2_EXT_F_WHEEL;
    if(p->sensors.line_valid)f|=AFR2_EXT_F_LINE;
    if(p->sensors.hall_available)f|=AFR2_EXT_F_HALL;
    if(p->config.motor_ready && p->pose_valid && !p->fault)f|=AFR2_EXT_F_MOTION|AFR2_EXT_F_GOTO;
    if(p->pose_valid)f|=AFR2_EXT_F_POSE;
    if(Afr2Sequence_Available(p))f|=AFR2_EXT_F_SEQUENCES|AFR2_EXT_F_DISTANCE;
    return f;
}
static uint8_t calibration_action(uint8_t action) {
    if(action==AFR2_A_RADAR_RANGE || action==AFR2_A_RADAR_TRACE)return 1;
    if(action==AFR2_A_ODOM_PROPOSE || action==AFR2_A_ODOM_STATE)return 1; /* AFR2-20261108-04 */
    /* Only numeric editors are writable through CONFIG, no generic arbitrary invoke. */
    Afr2MenuNode node;Afr2MenuItem item;
    for(uint8_t n=0;n<AFR2_N_COUNT;n++) {
        Afr2Menu_Node(n,&node);
        for(uint8_t i=0;i<node.count;i++){
            Afr2Menu_Item(n,i,&item);if(item.action==action && item.step)return 1;
        }
    }
    return 0;
}
uint8_t Afr2App_Command(void *ctx,uint8_t cmd,const uint8_t *d,uint8_t n,uint8_t *out,uint8_t *length) {
    Afr2App *a=ctx;Afr2Product *p=a->product;*length=0;
    if(cmd==AFR2_EXT_HELLO) {
        if(n!=4 || memcmp(d,"EX01",4))return AFR2_RESULT_BAD_REQUEST;
        p->extension_ready=1;p->session=a->link->session;memcpy(out,"EX01",4);afr2_put_u16(out+4,AFR2_EXT_VERSION);
        afr2_put_u32(out+6,features(p));*length=10;return AFR2_RESULT_OK;
    }
    if(!p->extension_ready)return AFR2_RESULT_UNAVAILABLE;
    if(a->reboot_pending && cmd!=AFR2_EXT_INFO)return AFR2_RESULT_UNAVAILABLE;
    if(cmd>=AFR2_EXT_US_GET && cmd<=AFR2_EXT_MOTION_HALT)return Afr2App_SensorCommand(a,cmd,d,n,out,length);
    if(cmd==AFR2_EXT_NETWORK_STATE || cmd==AFR2_EXT_NETWORK_EVENT)return Afr2App_NetworkCommand(a,cmd,d,n,out,length);
    if(cmd>=AFR2_EXT_SEQ_BEGIN && cmd<=AFR2_EXT_SEQ_STATUS) {
        if(cmd==AFR2_EXT_SEQ_START && (a->ui->touch_active || a->ui->keyboard || a->reboot_pending))return AFR2_RESULT_UNAVAILABLE;
        return Afr2Sequence_Command(p,cmd,d,n,out,length);
    }
    if(p->sequence.state>=1 && p->sequence.state<=3 &&
       (cmd==AFR2_EXT_DRIVE || cmd==AFR2_EXT_WHEEL || cmd==AFR2_EXT_MODE || cmd==AFR2_EXT_GOTO ||
        cmd==AFR2_EXT_CONFIG || cmd==AFR2_EXT_PREFS || cmd==AFR2_EXT_HALL_CAL || cmd==AFR2_EXT_LINE_CAL))return AFR2_RESULT_UNAVAILABLE;
    if(a->ui->node==AFR2_N_SERVO_ANGLE && (cmd==AFR2_EXT_DRIVE || cmd==AFR2_EXT_WHEEL || cmd==AFR2_EXT_MODE || cmd==AFR2_EXT_GOTO))return AFR2_RESULT_UNAVAILABLE;
    /* AFR2-20261106-08: touch view may request a GOTO in-place turn (Hall-closed angle). */
    if(a->ui->touch_active && (cmd==AFR2_EXT_WHEEL || cmd==AFR2_EXT_MODE))return AFR2_RESULT_UNAVAILABLE;
    switch(cmd) {
    case AFR2_EXT_BATTERY:{
        if(n!=1 || d[0]>1)return AFR2_RESULT_BAD_REQUEST;
        if(d[0])Afr2Battery_Measure(&p->sensors);
        afr2_put_u32(out,Afr2Boot_Id());afr2_put_u32(out+4,p->sensors.battery_sequence);
        afr2_put_u32(out+8,p->sensors.battery_sequence?p->now-p->sensors.battery_ms:AFR2_AGE_UNKNOWN);
        out[12]=Afr2Battery_Status(&p->sensors,p->now);afr2_put_u16(out+13,p->sensors.battery_raw);
        afr2_put_u16(out+15,Afr2Battery_Millivolts(&p->sensors));afr2_put_u16(out+17,AFR2_BATTERY_VREF_MV);
        out[19]=2;out[20]=1;out[21]=0;out[22]=5;*length=23;break;
    }
    case AFR2_EXT_INFO: {
        if(n!=1 || d[0]>4)return AFR2_RESULT_BAD_REQUEST;
        const char *text=d[0]==0?PSTR(AFR2_FIRMWARE_VERSION):d[0]==1?PSTR(__DATE__ " " __TIME__):
            d[0]==2?PSTR(__VERSION__):d[0]==3?PSTR("ATmega2560 avr-libc 16MHz"):PSTR("revision=" AFR2_BUILD_REVISION);
        uint8_t size=strlen_P(text);if(size>48)size=48;
        out[0]=d[0];afr2_put_u32(out+1,Afr2Boot_Id());out[5]=p->reset_cause;out[6]=size;
        memcpy_P(out+7,text,size);*length=7+size;break;
    }
    case AFR2_EXT_REBOOT:
        if(n!=1 || d[0]!=0xa5)return AFR2_RESULT_BAD_REQUEST;
        if(!p->stopped || p->prefs_state==1 || a->reboot_pending)return AFR2_RESULT_UNAVAILABLE;
        Afr2Product_Stop(p);a->reboot_pending=1;a->reboot_ms=p->now;
        out[0]=1;afr2_put_u32(out+1,Afr2Boot_Id());*length=5;break;
    case AFR2_EXT_STATE:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        out[0]=p->mode;out[1]=p->origin;out[2]=p->stopped;out[3]=p->fault;out[4]=p->config.motor_ready;
        out[5]=p->pose_valid;out[6]=p->goto_state;out[7]=p->prefs_state;
        afr2_put_u32(out+8,features(p));afr2_put_u32(out+12,p->now);out[16]=a->ui->ops.value(a,AFR2_A_DIRTY)!=0;afr2_put_u16(out+17,p->config.long_press_ms);*length=19;break;
    case AFR2_EXT_WEB_PULSE:
        if(n!=2 || afr2_get_u16(d)>1500u)return AFR2_RESULT_BAD_REQUEST;
        Afr2Product_WebPulse(p,afr2_get_u16(d));afr2_put_u16(out,afr2_get_u16(d));*length=2;break;
    case AFR2_EXT_DRIVE_PWM:
        if(n!=12 || afr2_get_u32(d)!=0)return AFR2_RESULT_BAD_REQUEST;
        /* AFR2-20261106: sin web_command; el mando Web puede tomar el control aunque el origen sea local. */
        {uint8_t r=Afr2Product_DrivePwm(p,(int16_t)afr2_get_u16(d+4),(int16_t)afr2_get_u16(d+6),
            (int16_t)afr2_get_u16(d+8),afr2_get_u16(d+10));
         if(r==AFR2_OK){Afr2Product_WebPulse(p,0);p->origin=AFR2_SOURCE_WEB;}
         return wire_result(r);}
    case AFR2_EXT_DRIVE:
        if(n!=12)return AFR2_RESULT_BAD_REQUEST;
        if(!web_command(p,d))return AFR2_RESULT_UNAVAILABLE;
        return wire_result(Afr2Product_Drive(p,(int16_t)afr2_get_u16(d+4),(int16_t)afr2_get_u16(d+6),
                                             (int16_t)afr2_get_u16(d+8),afr2_get_u16(d+10)));
    case AFR2_EXT_WHEEL:
        if(n!=9)return AFR2_RESULT_BAD_REQUEST;
        if(!web_command(p,d))return AFR2_RESULT_UNAVAILABLE;
        return wire_result(Afr2Product_Wheel(p,d[4],(int16_t)afr2_get_u16(d+5),afr2_get_u16(d+7)));
    case AFR2_EXT_MODE:
        if(n!=5)return AFR2_RESULT_BAD_REQUEST;
        if(!web_command(p,d))return AFR2_RESULT_UNAVAILABLE;
        if(d[4]==AFR2_IDLE){Afr2Product_Stop(p);break;}
        if(d[4]==AFR2_MANUAL && p->stopped){p->mode=AFR2_MANUAL;break;}
        if(d[4]!=AFR2_LINE)return AFR2_RESULT_BAD_REQUEST;
        return wire_result(Afr2Product_StartLine(p));
    case AFR2_EXT_GOTO:
        if(n!=12 && n!=16)return AFR2_RESULT_BAD_REQUEST;
        if(!web_command(p,d))return AFR2_RESULT_UNAVAILABLE;
        return wire_result(Afr2Product_StartGotoFull(p,(int32_t)afr2_get_u32(d+4),(int32_t)afr2_get_u32(d+8),
            n==16?(int16_t)afr2_get_u16(d+12):0,n==16?afr2_get_u16(d+14):p->config.speed_limit));
    case AFR2_EXT_LIMITS:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        afr2_put_u16(out,p->config.speed_limit);out[2]=p->config.pwm_limit;out[3]=10;
        afr2_put_u16(out+4,p->config.wheel_mm);afr2_put_u16(out+6,p->config.radius_sum_mm);*length=8;break;
    case AFR2_EXT_CANCEL:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        Afr2Product_Stop(p);out[0]=p->goto_state;*length=1;break;
    case AFR2_EXT_POSE:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        out[0]=p->pose_valid;afr2_put_u32(out+1,(uint32_t)p->x_mm);afr2_put_u32(out+5,(uint32_t)p->y_mm);
        afr2_put_u32(out+9,(uint32_t)p->yaw_mrad);afr2_put_u32(out+13,p->pose_valid?p->now-p->control_ms:AFR2_AGE_UNKNOWN);
        afr2_put_u32(out+17,p->pose_frame);afr2_put_u32(out+21,p->pose_sequence);*length=25;break;
    case AFR2_EXT_CONFIG: {
        if(n!=10 || afr2_get_u32(d)!=0 || d[4]>1 || !calibration_action(d[5]))return AFR2_RESULT_BAD_REQUEST;
        if(d[4]) {
            if(!web_command(p,d) || !p->stopped)return AFR2_RESULT_UNAVAILABLE;
            /* UI callback performs the same validation used by the physical editor. */
            uint8_t r=a->ui->ops.invoke(a,d[5],(int32_t)afr2_get_u32(d+6),0);if(r)return wire_result(r);
        }
        afr2_put_u32(out,(uint32_t)a->ui->ops.value(a,d[5]));*length=4;break;
    }
    case AFR2_EXT_PREFS:
        if(n!=5 || d[4]>1)return AFR2_RESULT_BAD_REQUEST;
        if(!web_command(p,d) || !p->stopped)return AFR2_RESULT_UNAVAILABLE;
        return wire_result(a->ui->ops.invoke(a,d[4]?AFR2_A_DISCARD:AFR2_A_SAVE,0,0));
    case AFR2_EXT_LINE_CAL: {
        if(n!=5 || d[4]>3)return AFR2_RESULT_BAD_REQUEST;
        if(!web_command(p,d) || !p->stopped)return AFR2_RESULT_UNAVAILABLE;
        uint8_t action=d[4]==2?AFR2_A_LINE_CHANNEL:d[4]==3?AFR2_A_LINE_THRESHOLD:d[4]?AFR2_A_LINE_KIND:AFR2_A_LINE_CONFIRM;
        uint8_t r=a->ui->ops.invoke(a,action,p->config.line_kind,0);
        if(r)return wire_result(r);
        out[0]=p->config.line_calibrated;*length=1;break;
    }
    case AFR2_EXT_TOUCH_UI:
        if(n!=4 || d[0]>1 || d[1]>8 || d[2]>23 || d[3]>100)return AFR2_RESULT_BAD_REQUEST;
        if(!d[0])a->ui->touch_cancelled=0;
        if(d[0] && a->ui->touch_cancelled)return AFR2_RESULT_UNAVAILABLE;
        if(d[0] && !a->ui->touch_active) {
            if(a->ui->keyboard || a->ui->edit || p->hall_measuring || a->ui->node==AFR2_N_HALL_PICK || a->ui->node==AFR2_N_SAVE_CONFIRM )return AFR2_RESULT_UNAVAILABLE;
            /* Explicit manual takeover cancels missions and radar. AFR2-20261106-10: not the Web PWM
             * drive that this same touch view is sending (a re-activation after a gap stopped it). */
            if(!(p->mode==AFR2_PWM_MANUAL && p->origin==AFR2_SOURCE_WEB))Afr2Product_Stop(p);
        }
        if(a->ui->touch_active!=d[0] || a->ui->touch_direction!=d[1] || a->ui->touch_heading!=d[2])a->snapshot_active=0;
        if(d[0] && (!a->ui->power_known || a->ui->power_percent!=d[3])){
            a->ui->power_percent=d[3];a->ui->power_known=1;a->ui->power_changed_ms=p->now;a->ui->touch_full=1;a->ui->scroll_redraw=true;++a->ui->generation;a->snapshot_active=0;
        }
        Afr2Ui_Touch(a->ui,d[0],d[1],d[2]);out[0]=d[3];*length=1;break;
    case AFR2_EXT_UI_META:{
        if(n)return AFR2_RESULT_BAD_REQUEST;
        if(a->snapshot_active || Afr2App_UiPrivate(a) || a->ui->edit || a->ui->redraw){
            AFR2_DBG("CAP refused: active=%u private=%u edit=%u redraw=%u",a->snapshot_active,Afr2App_UiPrivate(a),a->ui->edit,a->ui->redraw);
            return AFR2_RESULT_UNAVAILABLE;
        }
        AFR2_DBG_V("CAP start rev=%lu",(unsigned long)a->ui->revision);
        a->snapshot_revision=a->ui->revision;a->snapshot_ms=p->now;a->snapshot_active=1;
        a->snapshot_generation=a->ui->generation;
        afr2_put_u32(out,a->snapshot_revision);afr2_put_u32(out+4,Afr2Ui_ReportedScreen(a->ui));
        out[8]=a->ui->selection;out[9]=a->ui->firstVisible;afr2_put_u16(out+10,1024);
        out[12]=a->ui->notices?1:0;Afr2MenuNode metaNode;Afr2Ui_MenuNode(a->ui,&metaNode);out[13]=metaNode.view || a->ui->notices?0:metaNode.count;*length=14;break;
    }
    case AFR2_EXT_UI_PAGE: {
        if(n!=7)return AFR2_RESULT_BAD_REQUEST;
        if(Afr2App_UiPrivate(a)){AFR2_DBG("CAP page refused: private screen");a->snapshot_active=0;return AFR2_RESULT_UNAVAILABLE;}
        uint16_t offset=afr2_get_u16(d+4);uint8_t count=d[6];
        if(!a->snapshot_active || afr2_get_u32(d)!=a->snapshot_revision){
            AFR2_DBG("CAP page %u refused: capture %S",offset,a->snapshot_active?PSTR("revision changed"):PSTR("no longer active"));
            return AFR2_RESULT_UNAVAILABLE;
        }
        if(!count || count>40 || offset>1024u-count)return AFR2_RESULT_BAD_REQUEST;
        afr2_put_u32(out,a->snapshot_revision);afr2_put_u16(out+4,offset);out[6]=count;
        memcpy(out+7,a->oled->buffer+offset,count);*length=7+count;break;
    }
    case AFR2_EXT_UI_RELEASE:
        if(n!=4)return AFR2_RESULT_BAD_REQUEST;
        if(!a->snapshot_active || afr2_get_u32(d)!=a->snapshot_revision)return AFR2_RESULT_UNAVAILABLE;
        {   /* A slow capture freezes the local render (main.c skips it while active). */
            uint32_t took=p->now-a->snapshot_ms;(void)took;
            if(AFR2_DEBUG_LOG>=2 || took>=300u)AFR2_DBG("CAP done in %lu ms",(unsigned long)took);
        }
        a->snapshot_active=0;break;
    case AFR2_EXT_UI_ACTION: {
        /* Hall wizard and mandatory save choice are local actions. A remote
         * synthetic click must not invoke callbacks hidden behind a view. */
        if(a->ui->node==AFR2_N_ENCODER_TEST || a->ui->node==AFR2_N_ODOM_ASK || a->ui->node==AFR2_N_HALL_PICK || a->ui->node==AFR2_N_HALL_COUNT || a->ui->node==AFR2_N_SERVO_ANGLE ||
           (a->ui->node==AFR2_N_SAVE_CONFIRM && !(a->ui->confirm_save==AFR2_A_SAVE && a->ui->confirm_discard==AFR2_A_RADAR_DISCARD)))return AFR2_RESULT_UNAVAILABLE;
        if(n!=6 || d[4]>3 || (d[4]==0 && (int8_t)d[5]!=1 && (int8_t)d[5]!=-1) ||
           (d[4]!=0 && d[5]!=0))return AFR2_RESULT_BAD_REQUEST;
        if(!a->snapshot_revision || afr2_get_u32(d)!=a->snapshot_revision || a->ui->generation!=a->snapshot_generation ||
           (uint32_t)(p->now-a->snapshot_ms)>4000u || a->snapshot_active || a->ui->keyboard ||
           a->ui->edit || a->ui->touch_active || p->hall_measuring || !p->stopped)return AFR2_RESULT_UNAVAILABLE;
        if(d[4]==0 && a->ui->node==AFR2_N_SERVO_ANGLE)return AFR2_RESULT_UNAVAILABLE;
        if(d[4]==1 && !a->ui->notices && a->ui->node!=AFR2_N_RADAR_VIEW && a->ui->node!=AFR2_N_US_VIEW && a->ui->node!=AFR2_N_LINE_VIEW && a->ui->node!=AFR2_N_IR_VALUES && a->ui->node!=AFR2_N_SERVO_ANGLE) {
            Afr2MenuItem selected;Afr2Ui_MenuItem(a->ui,a->ui->selection,&selected);
            if(selected.step || selected.target==AFR2_N_AP_INFO || selected.target==AFR2_N_QR_AP || !Afr2App_RemoteUiAllowed(selected.action))return AFR2_RESULT_UNAVAILABLE;
        }
        if(d[4]==3) {
            if(a->ui->notices)return AFR2_RESULT_UNAVAILABLE;
            a->remote_ui=1;uint8_t returned=Afr2Ui_BackItem(a->ui);a->remote_ui=0;
            if(!returned)return AFR2_RESULT_UNAVAILABLE;
            afr2_put_u32(out,Afr2Ui_ReportedScreen(a->ui));out[4]=a->ui->selection;*length=5;break;
        }
        AFR2_DBG_V("UI remote action=%u delta=%d",d[4],(int)(int8_t)d[5]);
        a->remote_ui=1;
        Afr2UiInput input={(d[4]==0)?(int8_t)d[5]:0,d[4]==1,d[4]==2};Afr2Ui_Apply(a->ui,input);
        a->remote_ui=0;afr2_put_u32(out,Afr2Ui_ReportedScreen(a->ui));out[4]=a->ui->selection;*length=5;break;
    }
    case AFR2_EXT_DIAG_POLL:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        out[0]=a->diag_state==1;afr2_put_u32(out+1,a->diag_job);afr2_put_u32(out+5,a->diag_job^0xaf203307UL);*length=9;break;
    case AFR2_EXT_DIAG_RESULT:
        if(n!=8)return AFR2_RESULT_BAD_REQUEST;
        if(a->diag_state!=1 || afr2_get_u32(d)!=a->diag_job || (uint32_t)(p->now-a->diag_started)>=a->diag_timeout)return AFR2_RESULT_UNAVAILABLE;
        a->diag_state=afr2_get_u32(d+4)==(a->diag_job^0xaf203307UL)?2:3;
        Afr2Ui_NotifyFlash(a->ui,a->diag_state==2?0:1,PSTR("Prueba ESP"),a->diag_state==2?PSTR("Exitosa"):PSTR("Respuesta invalida"),2500);break;
    case AFR2_EXT_WIFI_POLL: {
        if(n!=1)return AFR2_RESULT_BAD_REQUEST;
        Afr2Wifi *w=&p->wifi;uint8_t offset=d[0],total=0;
        if(w->pending && w->op==AFR2_WIFI_PAGE)total=5;
        else if(w->pending && w->op==AFR2_WIFI_LOCAL_DRAFT)total=w->credential_length;
        else if(w->pending && w->op==AFR2_WIFI_WEB_DRAFT)total=(uint8_t)strlen(w->draft_ssid);
        else if(w->pending && w->op==AFR2_WIFI_WAIT)total=2; /* AFR2-20261108-06: segundos u16LE */
        if(offset>total)return AFR2_RESULT_BAD_REQUEST;
        out[0]=w->pending;afr2_put_u32(out+1,w->id);out[5]=w->op;out[6]=total;out[7]=offset;
        uint8_t count=total-offset;if(count>40)count=40;out[8]=count;out[9]=w->requested_mode;
        afr2_put_u32(out+10,w->draft_id);
        uint8_t ssid_n=(uint8_t)strlen(w->draft_ssid);
        for(uint8_t i=0;i<count;i++) {
            uint8_t pos=offset+i;out[14+i]=(w->op==AFR2_WIFI_WEB_DRAFT || pos<ssid_n)?w->draft_ssid[pos]:pos==ssid_n?0:w->password[pos-ssid_n-1];
        }
        if(w->op==AFR2_WIFI_PAGE && count){uint8_t page[5];afr2_put_u32(page,w->scan_id);page[4]=w->scan_page;memcpy(out+14,page+offset,count);}
        if(w->op==AFR2_WIFI_WAIT && count){uint8_t wait[2];afr2_put_u16(wait,p->config.wifi_retry_s);memcpy(out+14,wait+offset,count);}
        w->claimed=w->pending;*length=14+count;break;
    }
    case AFR2_EXT_WIFI_UPDATE: {
        if(!n)return AFR2_RESULT_BAD_REQUEST;
        Afr2Wifi *w=&p->wifi;
        if(d[0]==0) { /* status: subtype,mode,connected,rssiLE,ssidLen,ssid,ip4,apip4 */
            if(n<14 || d[1]>3 || d[2]>1 || d[5]>32 || n!=14+d[5] || memchr(d+6,0,d[5]))return AFR2_RESULT_BAD_REQUEST;
            if(w->mode!=d[1] || w->sta_connected!=d[2] || strlen(w->ssid)!=d[5] || memcmp(w->ssid,d+6,d[5]))w->details_valid=0;
            if(!w->pending && !w->draft_ready && !w->mode_draft)w->requested_mode=d[1]?d[1]:1;
            w->mode=d[1];w->sta_connected=d[2];w->rssi=(int16_t)afr2_get_u16(d+3);
            memcpy(w->ssid,d+6,d[5]);w->ssid[d[5]]=0;
            const uint8_t *ip=d+6+d[5];snprintf_P(w->ip,sizeof(w->ip),PSTR("%u.%u.%u.%u"),ip[0],ip[1],ip[2],ip[3]);
            snprintf_P(w->ap_ip,sizeof(w->ap_ip),PSTR("%u.%u.%u.%u"),ip[4],ip[5],ip[6],ip[7]);w->available=1;w->status_ms=p->now;
        } else if(d[0]==1) {
            if(n<9 || d[1]>7 || d[2]>32 || d[3]>6 || d[4]>8 || d[1]>=d[4] || n!=9+d[2] || memchr(d+9,0,d[2]))return AFR2_RESULT_BAD_REQUEST;
            if(!w->pending || (w->op!=AFR2_WIFI_SCAN && w->op!=AFR2_WIFI_PAGE) || afr2_get_u32(d+5)!=w->id)return AFR2_RESULT_UNAVAILABLE;
            if(!w->scan_id || d[4]!=w->scan_expected || d[1]>=w->scan_expected)return AFR2_RESULT_UNAVAILABLE;
            if(d[1]==0)w->network_count=0;
            memcpy(w->networks[d[1]],d+9,d[2]);w->networks[d[1]][d[2]]=0;w->network_security[d[1]]=d[3];
            w->scan_received|=(1u<<d[1]);
            if(w->network_count<=d[1])w->network_count=d[1]+1;
        } else if(d[0]==6) {
            if(n!=13 || !w->pending || (w->op!=AFR2_WIFI_SCAN && w->op!=AFR2_WIFI_PAGE) || afr2_get_u32(d+1)!=w->id || d[9]>1 || d[10]>16 || d[11]>1 || d[12]>8 || !afr2_get_u32(d+5) || (d[9] && d[10]<=8) || d[12]!=(d[10]>d[9]*8u+8u?8u:d[10]-d[9]*8u))return AFR2_RESULT_BAD_REQUEST;
            if(w->op==AFR2_WIFI_PAGE && (w->scan_id!=afr2_get_u32(d+5) || w->scan_page!=d[9]))return AFR2_RESULT_UNAVAILABLE;
            w->scan_id=afr2_get_u32(d+5);w->scan_page=d[9];w->scan_total=d[10];w->scan_truncated=d[11];w->scan_expected=d[12];w->scan_received=0;w->network_count=0;
        } else if(d[0]==2) {
            if(n<11 || d[5]>6 || d[10]>32 || n!=11+d[10] || memchr(d+11,0,d[10]))return AFR2_RESULT_BAD_REQUEST;
            if(!w->pending || afr2_get_u32(d+1)!=w->id || !w->claimed)return AFR2_RESULT_UNAVAILABLE;
            if(d[5]==1 && ((w->op!=AFR2_WIFI_WEB_DRAFT && w->op!=AFR2_WIFI_LOCAL_DRAFT) ||
               !afr2_get_u32(d+6) || !d[10]))return AFR2_RESULT_BAD_REQUEST;
            if(d[5]==2 && w->op!=AFR2_WIFI_CONNECT && w->op!=AFR2_WIFI_RETRY && w->op!=AFR2_WIFI_REBOOT_ESP && w->op!=AFR2_WIFI_CHANGE_MODE && w->op!=AFR2_WIFI_DISCONNECT)return AFR2_RESULT_BAD_REQUEST;
            if(d[5]==3 && (w->op==AFR2_WIFI_WEB_DRAFT || w->op==AFR2_WIFI_LOCAL_DRAFT || w->op==AFR2_WIFI_CANCEL))return AFR2_RESULT_BAD_REQUEST;
            if(d[5]==6 && w->op!=AFR2_WIFI_WEB_DRAFT)return AFR2_RESULT_BAD_REQUEST;
            if(d[5]==3 && (w->op==AFR2_WIFI_SCAN || w->op==AFR2_WIFI_PAGE) && (!w->scan_id || w->scan_received!=(uint8_t)((1u<<w->scan_expected)-1u)))return AFR2_RESULT_UNAVAILABLE;
            w->state=d[5];
            if(w->state==1){w->draft_id=afr2_get_u32(d+6);if(!w->draft_id)return AFR2_RESULT_BAD_REQUEST;
                w->draft_ready=1;w->draft_ms=p->now;memcpy(w->draft_ssid,d+11,d[10]);w->draft_ssid[d[10]]=0;
                if(a->ui->node!=AFR2_N_WIFI_REVIEW)Afr2Ui_Open(a->ui,AFR2_N_WIFI_REVIEW);}
            if(w->state==2) {
                if(w->op==AFR2_WIFI_REBOOT_ESP){w->pending=0;Afr2Ui_NotifyFlash(a->ui,0,PSTR("Reinicio ESP"),PSTR("Solicitado"),2000);}
                else Afr2Ui_NotifyFlash(a->ui,0,PSTR("Wi-Fi"),w->op==AFR2_WIFI_DISCONNECT?PSTR("Desconectando..."):(w->op==AFR2_WIFI_CONNECT || w->op==AFR2_WIFI_CHANGE_MODE)?PSTR("Reiniciando ESP"):PSTR("Conectando..."),2000);
            }
            if(w->state==4 && w->op==AFR2_WIFI_CHANGE_MODE)Afr2Ui_Open(a->ui,AFR2_N_WIFI_SETUP);
            if(w->state>=1 && w->state!=2) {
                w->pending=0;
                volatile char *s=w->password;for(uint8_t i=0;i<64;i++)s[i]=0;w->credential_length=0;
                if(w->state>=3)w->draft_ready=0;
                if((w->op==AFR2_WIFI_SCAN || w->op==AFR2_WIFI_PAGE) && w->state>=4)w->network_count=0;
                /* AFR2-20261108-06: la respuesta al selector sin Wi-Fi no agrega aviso propio si salio bien: Iniciar AP
                 * muestra el aviso "Red AP creada" del ESP y Seguir esperando ya aviso al elegir. */
                if((w->op==AFR2_WIFI_AP_START || w->op==AFR2_WIFI_WAIT) && w->state==3){}
                else if(w->state==6){
                    Afr2Ui_NotifyFlash(a->ui,1,PSTR("No hay clientes"),PSTR("Conectados"),3000);Afr2Ui_AfterNotice(a->ui,AFR2_N_WIFI_NETWORKS);
                }else {
                    const char *message=w->state==1?PSTR("Datos listos"):w->state==3?(w->op==AFR2_WIFI_STATUS?PSTR("Estado recibido"):(w->op==AFR2_WIFI_SCAN || w->op==AFR2_WIFI_PAGE)?(w->scan_total?PSTR("Redes listas"):PSTR("Sin redes")):w->op==AFR2_WIFI_DISCONNECT?PSTR("Red desconectada"):w->op==AFR2_WIFI_CHANGE_MODE?PSTR("Modo actualizado"):PSTR("Conectado")):w->state==4?PSTR("Operacion fallida"):PSTR("Cancelado");
                    Afr2Ui_NotifyFlash(a->ui,w->state>=4?1:0,w->state==3?PSTR("Operacion exitosa"):PSTR("Wi-Fi"),message,2500);
                    if(w->state==3 && (w->op==AFR2_WIFI_SCAN || w->op==AFR2_WIFI_PAGE) && w->scan_truncated)Afr2Ui_NotifyFlash(a->ui,1,PSTR("Lista limitada"),PSTR("16 redes maximo"),2500);
                    if(w->state==3 && (w->op==AFR2_WIFI_SCAN || w->op==AFR2_WIFI_PAGE))Afr2Ui_AfterNotice(a->ui,AFR2_N_WIFI_NETWORKS);
                }
            }
        } else if(d[0]==5) { /* Explicit local request only: idLE, offset, total, bytes. */
            if(n<8 || !w->pending || !w->claimed || w->op!=AFR2_WIFI_READ_CREDENTIALS || afr2_get_u32(d+1)!=w->id || d[5]!=w->credential_length || d[6]>96 || d[6]<33 || d[5]+n-7>d[6])return AFR2_RESULT_BAD_REQUEST;
            for(uint8_t i=7;i<n;i++){
                uint8_t pos=w->credential_length++;
                if(pos<33)w->draft_ssid[pos]=d[i];else w->password[pos-33]=d[i];
            }
            if(w->credential_length==d[6]){w->draft_ssid[32]=0;w->password[d[6]-33]=0;w->pending=0;w->credentials_visible=1;w->draft_ms=p->now;Afr2Ui_Open(a->ui,AFR2_N_WIFI_DATA);}
        } else if(d[0]==4) {
            if(n!=18 || d[1]>3 || d[2]>1 || d[3]>6 || d[4]>6)return AFR2_RESULT_BAD_REQUEST;
            char ip[16];snprintf_P(ip,sizeof(ip),PSTR("%u.%u.%u.%u"),d[6],d[7],d[8],d[9]);
            if(w->mode!=d[1] || w->sta_connected!=d[2] || (d[2] && strcmp(ip,w->ip)))return AFR2_RESULT_UNAVAILABLE;
            w->security=d[3];w->ap_security=d[4];w->ws_clients=d[5];memcpy(w->mask,d+10,4);memcpy(w->gateway,d+14,4);w->details_valid=1;w->details_ms=p->now;
        } else if(d[0]==3) {
            if(n<3 || d[1]>1 || d[2]>15 || n!=3+d[2] || memchr(d+3,0,d[2]))return AFR2_RESULT_BAD_REQUEST;
            char *v=d[1]?w->web_version:w->esp_version;memcpy(v,d+3,d[2]);v[d[2]]=0;
        } else return AFR2_RESULT_BAD_REQUEST;
        w->changed_ms=p->now;break;
    }
    default:return AFR2_RESULT_UNAVAILABLE;
    }
    return AFR2_RESULT_OK;
}
