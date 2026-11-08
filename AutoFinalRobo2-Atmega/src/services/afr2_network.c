/* AFR2-20261024-08. Own bounded EX01v5 presentation receiver. F4 notification
 * semantics adapted without System globals, HAL, DMA or station-key display. */
#include "afr2_app.h"
#include "afr2_debug.h"
#include "afr2_product_wire.h"
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

static void clear_ap(Afr2Wifi *w) {
    volatile char *p=w->ap_password;for(uint8_t i=0;i<sizeof(w->ap_password);i++)p[i]=0;
    w->ap_ssid[0]=0;
}
uint8_t Afr2App_UiPrivate(const Afr2App *a) {
    const Afr2Ui *u=a->ui;const Afr2Wifi *w=&a->product->wifi;
    uint8_t ap_notice=u->notices && u->notification[u->tail].kind==AFR2_NOTICE_NETWORK+AFR2_NET_AP_READY;
    return u->node==AFR2_N_QR_AP || (u->node==AFR2_N_WIFI_DATA && w->credentials_visible) || u->keyboard || ((u->node==AFR2_N_AP_INFO || ap_notice) && w->display_ready && w->ap_password[0]);
}
uint8_t Afr2App_NetworkCommand(Afr2App *a,uint8_t cmd,const uint8_t *d,uint8_t n,uint8_t *out,uint8_t *length) {
    Afr2Wifi *w=&a->product->wifi;
    if(cmd==AFR2_EXT_NETWORK_STATE) {
        if(n<8 || !afr2_get_u32(d) || !afr2_get_u16(d+4) || d[6]>=AFR2_NETWORK_PAGES || d[7]>32 || n!=8+d[7])return AFR2_RESULT_BAD_REQUEST;
        uint32_t boot=afr2_get_u32(d);uint16_t revision=afr2_get_u16(d+4);uint8_t page=d[6],size=d[7];const uint8_t *text=d+8;
        if(page==0) {
            if(size!=15 || text[0]>3 || text[1]>1 || text[2]>1 || text[4]>1)return AFR2_RESULT_BAD_REQUEST;
            if((text[1] && (!(text[0]&1u) || !(text[7]|text[8]|text[9]|text[10]))) ||
               (text[2] && (!(text[0]&2u) || !(text[11]|text[12]|text[13]|text[14]))) || (!text[2] && text[3]))return AFR2_RESULT_BAD_REQUEST;
            if(boot!=w->display_boot || revision!=w->display_revision) {
                w->display_ready=0;clear_ap(w);
                a->snapshot_active=0;a->ui->redraw=true;
                if(boot!=w->display_boot){if(w->display_boot){w->pending=w->draft_ready=w->network_count=w->scan_total=w->mode_draft=0;w->scan_id=0;memset(w->password,0,sizeof(w->password));w->credentials_visible=0;Afr2Ui_NetworkRestart(a->ui);}w->display_event=0;}
            }
            w->display_boot=boot;w->display_revision=revision;w->display_page=0;
            w->mode=text[0];w->sta_connected=text[1];w->ap_active=text[2];w->ap_clients=text[3];w->web_ready=text[4];
            w->rssi=(int16_t)afr2_get_u16(text+5);
            Afr2Debug_NetState(text[0],text[1],text[2],text[3],text[4],boot); /* logs only changes */
            snprintf_P(w->ip,sizeof(w->ip),PSTR("%u.%u.%u.%u"),text[7],text[8],text[9],text[10]);
            snprintf_P(w->ap_ip,sizeof(w->ap_ip),PSTR("%u.%u.%u.%u"),text[11],text[12],text[13],text[14]);
        } else {
            if(boot!=w->display_boot || revision!=w->display_revision || page!=w->display_page)return AFR2_RESULT_UNAVAILABLE;
            if(memchr(text,0,size))return AFR2_RESULT_BAD_REQUEST;
            if(page==1){memcpy(w->ssid,text,size);w->ssid[size]=0;}
            else if(page==2){memcpy(w->ap_ssid,text,size);w->ap_ssid[size]=0;}
            else if(page==3){memcpy(w->ap_password,text,size);w->ap_password[size]=0;}
            else {
                if(size>31 || (size && strlen(w->ap_password)!=32))return AFR2_RESULT_BAD_REQUEST;
                memcpy(w->ap_password+32,text,size);if(size)w->ap_password[32+size]=0;
            }
        }
        w->display_page=page+1;
        if(w->display_page==AFR2_NETWORK_PAGES) {
            uint8_t password_size=strlen(w->ap_password);
            if(w->ap_active && (!w->ap_ssid[0] || (password_size && password_size<8))) {
                w->display_ready=0;clear_ap(w);return AFR2_RESULT_BAD_REQUEST;
            }
            if(!w->ap_active)clear_ap(w);
            w->display_ready=1;w->available=1;w->status_ms=a->product->now;
            w->display_ms=a->product->now;a->snapshot_active=0;a->ui->redraw=true;
        }
        memcpy(out,d,7);*length=7;return AFR2_RESULT_OK;
    }
    if(cmd!=AFR2_EXT_NETWORK_EVENT || n!=10 || !afr2_get_u32(d+4) || d[8]<1 || d[8]>AFR2_NET_EVENT_MAX)return AFR2_RESULT_BAD_REQUEST;
    if(!w->display_ready || afr2_get_u32(d)!=w->display_boot)return AFR2_RESULT_UNAVAILABLE;
    uint32_t seq=afr2_get_u32(d+4);uint8_t kind=d[8];
    if(seq>w->display_event) {
        uint8_t duplicate_boot=kind==AFR2_NET_BOOT && a->esp_boot_notified==w->display_boot;
        /* AFR2-20261107-48: during the quiet window after the user closed network notices,
         * chatty events are accepted (no ESP retry) but not shown. */
        uint8_t chatty=kind==AFR2_NET_WS_OPEN || kind==AFR2_NET_WS_CLOSE || kind==AFR2_NET_WEB_SERVED || kind==AFR2_NET_CLIENT_JOIN || kind==AFR2_NET_CLIENT_LEAVE;
        uint8_t silent=duplicate_boot || (chatty && Afr2Ui_NetworkQuiet(a->ui));
        if(!silent) {
            if(a->ui->notices>=3)return AFR2_RESULT_UNAVAILABLE;
            char detail[18];detail[0]=0;
            if(kind==AFR2_NET_GOT_IP)strncpy(detail,w->ip,sizeof(detail)-1);
            else if(kind==AFR2_NET_AP_READY)strncpy(detail,w->ap_ip,sizeof(detail)-1);
            else if(kind==AFR2_NET_CLIENT_JOIN || kind==AFR2_NET_CLIENT_LEAVE) {
                if(d[9])snprintf_P(detail,sizeof(detail),PSTR("Cliente %u"),d[9]);else strcpy_P(detail,PSTR("Cliente Wi-Fi"));
            }
            detail[17]=0;
            Afr2Ui_NotifyFlashTitle(a->ui,AFR2_NOTICE_NETWORK+kind,PSTR("Conectividad"),detail,kind==AFR2_NET_AP_READY?20000:4000);
            a->snapshot_active=0;
        }
        if(kind==AFR2_NET_BOOT){
            if(!duplicate_boot){w->pending=w->draft_ready=w->network_count=w->scan_total=w->mode_draft=0;w->scan_id=0;memset(w->password,0,sizeof(w->password));w->credentials_visible=0;Afr2Ui_NetworkRestart(a->ui);}
            a->esp_boot_notified=w->display_boot;
        }
        Afr2Debug_NetEvent(kind,d[9],seq); /* after the notice gate: retries are not logged twice */
        w->display_event=seq;
    }
    afr2_put_u32(out,seq);*length=4;return AFR2_RESULT_OK;
}
