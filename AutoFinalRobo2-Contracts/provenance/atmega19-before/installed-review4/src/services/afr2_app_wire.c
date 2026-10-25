/* AFR2-20261023-19. New opt-in endpoint, all offsets encoded explicitly LE.
 * ESP integration documented in Contracts; no ESP source edited here.
 */
#include "afr2_app.h"
#include "afr2_product_wire.h"
#include <stdio.h>
#include <string.h>
static uint8_t wire_result(uint8_t result) {
    return result==AFR2_OK?AFR2_RESULT_OK:result==AFR2_BAD?AFR2_RESULT_BAD_REQUEST:
        result==AFR2_FAILED?AFR2_RESULT_INTERNAL:AFR2_RESULT_UNAVAILABLE;
}
static uint8_t owner(const Afr2Product *p,const uint8_t *d) {
    return p->remote_enabled && p->owner==AFR2_REMOTE && afr2_get_u32(d)==p->owner_token;
}
static uint32_t features(const Afr2Product *p) {
    uint32_t f=AFR2_EXT_F_UI|AFR2_EXT_F_STOP|AFR2_EXT_F_WIFI|AFR2_EXT_F_PREFS;
    if(p->sensors.line_valid)f|=AFR2_EXT_F_LINE;
    if(p->sensors.hall_available)f|=AFR2_EXT_F_HALL;
    if(p->config.motor_ready && p->pose_valid)f|=AFR2_EXT_F_MOTION;
    if(p->pose_valid)f|=AFR2_EXT_F_POSE;
    return f;
}
static uint8_t calibration_action(uint8_t action) {
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
        p->extension_ready=1;memcpy(out,"EX01",4);afr2_put_u16(out+4,AFR2_EXT_VERSION);
        afr2_put_u32(out+6,features(p));*length=10;return AFR2_RESULT_OK;
    }
    if(!p->extension_ready)return AFR2_RESULT_UNAVAILABLE;
    switch(cmd) {
    case AFR2_EXT_STATE:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        out[0]=p->mode;out[1]=p->owner;out[2]=p->stopped;out[3]=p->fault;out[4]=p->remote_enabled;
        out[5]=p->pose_valid;out[6]=p->goto_state;out[7]=p->prefs_state;
        afr2_put_u32(out+8,features(p));afr2_put_u32(out+12,p->now);*length=16;break;
    case AFR2_EXT_LEASE:
        if(n!=7 || d[0]>1)return AFR2_RESULT_BAD_REQUEST;
        if(!p->remote_enabled)return AFR2_RESULT_UNAVAILABLE;
        if(d[0]==1){if(!owner(p,d+1))return AFR2_RESULT_UNAVAILABLE;Afr2Product_Stop(p);}
        else {uint8_t r=Afr2Product_Acquire(p,AFR2_REMOTE,afr2_get_u32(d+1),afr2_get_u16(d+5));if(r)return wire_result(r);}
        out[0]=p->owner;afr2_put_u16(out+1,p->lease_ms);*length=3;break;
    case AFR2_EXT_DRIVE:
        if(n!=12)return AFR2_RESULT_BAD_REQUEST;
        if(!owner(p,d))return AFR2_RESULT_UNAVAILABLE;
        return wire_result(Afr2Product_Drive(p,(int16_t)afr2_get_u16(d+4),(int16_t)afr2_get_u16(d+6),
                                             (int16_t)afr2_get_u16(d+8),afr2_get_u16(d+10)));
    case AFR2_EXT_WHEEL:
        if(n!=9)return AFR2_RESULT_BAD_REQUEST;
        if(!owner(p,d))return AFR2_RESULT_UNAVAILABLE;
        return wire_result(Afr2Product_Wheel(p,d[4],(int16_t)afr2_get_u16(d+5),afr2_get_u16(d+7)));
    case AFR2_EXT_MODE:
        if(n!=5)return AFR2_RESULT_BAD_REQUEST;
        if(!owner(p,d))return AFR2_RESULT_UNAVAILABLE;
        if(d[4]==AFR2_IDLE){Afr2Product_Stop(p);break;}
        if(d[4]!=AFR2_LINE)return AFR2_RESULT_BAD_REQUEST;
        return wire_result(Afr2Product_StartLine(p));
    case AFR2_EXT_GOTO:
        if(n!=12)return AFR2_RESULT_BAD_REQUEST;
        if(!owner(p,d))return AFR2_RESULT_UNAVAILABLE;
        return wire_result(Afr2Product_StartGoto(p,(int32_t)afr2_get_u32(d+4),(int32_t)afr2_get_u32(d+8)));
    case AFR2_EXT_CANCEL:
        if(n)return AFR2_RESULT_BAD_REQUEST;Afr2Product_Stop(p);out[0]=p->goto_state;*length=1;break;
    case AFR2_EXT_POSE:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        out[0]=p->pose_valid;afr2_put_u32(out+1,(uint32_t)p->x_mm);afr2_put_u32(out+5,(uint32_t)p->y_mm);
        afr2_put_u32(out+9,(uint32_t)p->yaw_mrad);afr2_put_u32(out+13,p->pose_valid?p->now-p->control_ms:AFR2_AGE_UNKNOWN);
        afr2_put_u32(out+17,p->pose_frame);afr2_put_u32(out+21,p->pose_sequence);*length=25;break;
    case AFR2_EXT_HALL_CAL:
        if(n!=6 || d[4]>3 || d[5]>3)return AFR2_RESULT_BAD_REQUEST;
        if(!owner(p,d))return AFR2_RESULT_UNAVAILABLE;
        if(d[4]!=3 && !p->sensors.hall_available)return AFR2_RESULT_UNAVAILABLE;
        if(p->hall_measuring && p->selected_wheel!=d[5])return AFR2_RESULT_UNAVAILABLE;
        p->selected_wheel=d[5];
        if(d[4]==0)Afr2Product_HallBegin(p);
        else if(d[4]==1){uint8_t r=Afr2Product_HallEnd(p);if(r)return wire_result(r);}
        else if(d[4]==2){if(p->prefs_state==1)return AFR2_RESULT_UNAVAILABLE;
            if(!p->hall_delta || p->hall_bad || p->hall_measuring)return AFR2_RESULT_BAD_REQUEST;Afr2Product_HallSave(p);Afr2Product_Save(p);}
        else {p->hall_measuring=0;p->hall_delta=0;}
        out[0]=p->hall_measuring;out[1]=p->hall_bad;afr2_put_u32(out+2,(uint32_t)p->hall_delta);*length=6;break;
    case AFR2_EXT_CONFIG: {
        if(n!=10 || d[4]>1 || !calibration_action(d[5]))return AFR2_RESULT_BAD_REQUEST;
        if(d[4]) {
            if(!owner(p,d) || !p->stopped)return AFR2_RESULT_UNAVAILABLE;
            /* UI callback performs the same validation used by the physical editor. */
            uint8_t r=a->ui->ops.invoke(a,d[5],(int32_t)afr2_get_u32(d+6),0);if(r)return wire_result(r);
        }
        afr2_put_u32(out,(uint32_t)a->ui->ops.value(a,d[5]));*length=4;break;
    }
    case AFR2_EXT_PREFS:
        if(n!=5 || d[4]>1)return AFR2_RESULT_BAD_REQUEST;
        if(!owner(p,d) || !p->stopped)return AFR2_RESULT_UNAVAILABLE;
        return wire_result(a->ui->ops.invoke(a,d[4]?AFR2_A_DISCARD:AFR2_A_SAVE,0,0));
    case AFR2_EXT_UI_META:
        if(n)return AFR2_RESULT_BAD_REQUEST;
        if(a->snapshot_active || a->ui->keyboard || a->ui->edit || a->ui->redraw)return AFR2_RESULT_UNAVAILABLE;
        a->snapshot_revision=a->ui->revision;a->snapshot_ms=p->now;a->snapshot_active=1;
        a->snapshot_generation=a->ui->generation;
        afr2_put_u32(out,a->snapshot_revision);afr2_put_u32(out+4,a->ui->screen);
        out[8]=a->ui->selection;out[9]=a->ui->firstVisible;afr2_put_u16(out+10,1024);
        out[12]=a->ui->notices?1:0;*length=13;break;
    case AFR2_EXT_UI_PAGE: {
        if(n!=7)return AFR2_RESULT_BAD_REQUEST;
        uint16_t offset=afr2_get_u16(d+4);uint8_t count=d[6];
        if(!a->snapshot_active || afr2_get_u32(d)!=a->snapshot_revision)return AFR2_RESULT_UNAVAILABLE;
        if(!count || count>40 || offset>1024u-count)return AFR2_RESULT_BAD_REQUEST;
        afr2_put_u32(out,a->snapshot_revision);afr2_put_u16(out+4,offset);out[6]=count;
        memcpy(out+7,a->oled->buffer+offset,count);*length=7+count;break;
    }
    case AFR2_EXT_UI_RELEASE:
        if(n!=4)return AFR2_RESULT_BAD_REQUEST;
        if(!a->snapshot_active || afr2_get_u32(d)!=a->snapshot_revision)return AFR2_RESULT_UNAVAILABLE;
        a->snapshot_active=0;break;
    case AFR2_EXT_UI_ACTION: {
        if(n!=6 || d[4]>2 || (d[4]==0 && (int8_t)d[5]!=1 && (int8_t)d[5]!=-1) ||
           (d[4]!=0 && d[5]!=0))return AFR2_RESULT_BAD_REQUEST;
        if(afr2_get_u32(d)!=a->snapshot_revision || a->ui->generation!=a->snapshot_generation ||
           (uint32_t)(p->now-a->snapshot_ms)>4000u || a->snapshot_active || a->ui->keyboard ||
           a->ui->edit || p->owner==AFR2_LOCAL || p->hall_measuring || !p->stopped)return AFR2_RESULT_UNAVAILABLE;
        if(d[4]==1 && !a->ui->notices) {
            Afr2MenuItem selected;Afr2Menu_Item(a->ui->node,a->ui->selection,&selected);
            if(selected.step || selected.action==AFR2_A_KEYBOARD)return AFR2_RESULT_UNAVAILABLE;
        }
        a->remote_ui=1;
        Afr2UiInput input={(d[4]==0)?(int8_t)d[5]:0,d[4]==1,d[4]==2};Afr2Ui_Apply(a->ui,input);
        a->remote_ui=0;afr2_put_u32(out,a->ui->screen);out[4]=a->ui->selection;*length=5;break;
    }
    case AFR2_EXT_WIFI_POLL: {
        if(n!=1)return AFR2_RESULT_BAD_REQUEST;
        Afr2Wifi *w=&p->wifi;uint8_t offset=d[0],total=0;
        if(w->pending && w->op==AFR2_WIFI_LOCAL_DRAFT)total=w->credential_length;
        else if(w->pending && w->op==AFR2_WIFI_WEB_DRAFT)total=(uint8_t)strlen(w->draft_ssid);
        if(offset>total)return AFR2_RESULT_BAD_REQUEST;
        out[0]=w->pending;afr2_put_u32(out+1,w->id);out[5]=w->op;out[6]=total;out[7]=offset;
        uint8_t count=total-offset;if(count>40)count=40;out[8]=count;out[9]=w->requested_mode;
        afr2_put_u32(out+10,w->draft_id);
        uint8_t ssid_n=(uint8_t)strlen(w->draft_ssid);
        for(uint8_t i=0;i<count;i++) {
            uint8_t pos=offset+i;out[14+i]=(w->op==AFR2_WIFI_WEB_DRAFT || pos<ssid_n)?w->draft_ssid[pos]:pos==ssid_n?0:w->password[pos-ssid_n-1];
        }
        w->claimed=w->pending;*length=14+count;break;
    }
    case AFR2_EXT_WIFI_UPDATE: {
        if(!n)return AFR2_RESULT_BAD_REQUEST;
        Afr2Wifi *w=&p->wifi;
        if(d[0]==0) { /* status: subtype,mode,connected,rssiLE,ssidLen,ssid,ip4,apip4 */
            if(n<14 || d[1]>3 || d[2]>1 || d[5]>32 || n!=14+d[5] || memchr(d+6,0,d[5]))return AFR2_RESULT_BAD_REQUEST;
            w->mode=d[1];w->sta_connected=d[2];w->rssi=(int16_t)afr2_get_u16(d+3);
            memcpy(w->ssid,d+6,d[5]);w->ssid[d[5]]=0;
            const uint8_t *ip=d+6+d[5];snprintf(w->ip,sizeof(w->ip),"%u.%u.%u.%u",ip[0],ip[1],ip[2],ip[3]);
            snprintf(w->ap_ip,sizeof(w->ap_ip),"%u.%u.%u.%u",ip[4],ip[5],ip[6],ip[7]);w->available=1;
        } else if(d[0]==1) {
            if(n<3 || d[1]>7 || d[2]>32 || n!=3+d[2] || memchr(d+3,0,d[2]))return AFR2_RESULT_BAD_REQUEST;
            if(d[1]==0)w->network_count=0;memcpy(w->networks[d[1]],d+3,d[2]);w->networks[d[1]][d[2]]=0;
            if(w->network_count<=d[1])w->network_count=d[1]+1;
        } else if(d[0]==2) {
            if(n<11 || d[5]>5 || d[10]>32 || n!=11+d[10] || memchr(d+11,0,d[10]))return AFR2_RESULT_BAD_REQUEST;
            if(!w->pending || afr2_get_u32(d+1)!=w->id || !w->claimed)return AFR2_RESULT_UNAVAILABLE;
            if(d[5]==1 && ((w->op!=AFR2_WIFI_WEB_DRAFT && w->op!=AFR2_WIFI_LOCAL_DRAFT) ||
               !afr2_get_u32(d+6) || !d[10]))return AFR2_RESULT_BAD_REQUEST;
            if(d[5]==2 && w->op!=AFR2_WIFI_CONNECT && w->op!=AFR2_WIFI_RETRY)return AFR2_RESULT_BAD_REQUEST;
            if(d[5]==3 && (w->op==AFR2_WIFI_WEB_DRAFT || w->op==AFR2_WIFI_LOCAL_DRAFT || w->op==AFR2_WIFI_CANCEL))return AFR2_RESULT_BAD_REQUEST;
            w->state=d[5];
            if(w->state==1){w->draft_id=afr2_get_u32(d+6);if(!w->draft_id)return AFR2_RESULT_BAD_REQUEST;
                w->draft_ready=1;w->draft_ms=p->now;memcpy(w->draft_ssid,d+11,d[10]);w->draft_ssid[d[10]]=0;
                if(a->ui->node!=AFR2_N_WIFI_REVIEW)Afr2Ui_Open(a->ui,AFR2_N_WIFI_REVIEW);}
            if(w->state==2)Afr2Ui_Notify(a->ui,0,"Wi-Fi","Conectando...",3000);
            if(w->state>=1 && w->state!=2) {
                w->pending=0;
                volatile char *s=w->password;for(uint8_t i=0;i<64;i++)s[i]=0;w->credential_length=0;
                if(w->state>=3)w->draft_ready=0;
                Afr2Ui_Notify(a->ui,w->state>=4?1:0,"Wi-Fi",w->state==1?"Datos listos":
                    w->state==3?(w->op==AFR2_WIFI_STATUS?"Estado recibido":w->op==AFR2_WIFI_SCAN?"Redes listas":"Conectado"):
                    w->state==4?"Fallo conexion":"Cancelado",3000);
            }
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
