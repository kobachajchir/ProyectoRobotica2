/* AFR2-20261023-19. Application wrappers: own AVR services, no F4 globals.
 * Only this wrapper prepares presentation snapshots; renderer stays pure.
 */
#include "afr2_app.h"
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
static void wipe(void *data,uint16_t n){volatile uint8_t *p=data;while(n--)*p++=0;}
static int32_t value(void *ctx,uint8_t action) {
    Afr2App *a=ctx;Afr2Product *p=a->product;Afr2Config *c=&p->config;
    switch(action) {
    case AFR2_A_SPEED:return c->speed_limit;case AFR2_A_LINE_SPEED:return c->line_speed;
    case AFR2_A_LINE_KP:return c->line_kp;case AFR2_A_GOTO_X:return p->target_x;case AFR2_A_GOTO_Y:return p->target_y;
    case AFR2_A_WHEEL:return p->selected_wheel;case AFR2_A_TEST_PWM:return a->test_pwm;case AFR2_A_TEST_MS:return c->test_ms;
    case AFR2_A_SERVO_US:return a->servo_us;case AFR2_A_LINE_KIND:return c->line_kind;
    case AFR2_A_LINE_INVERT:return c->line_invert;case AFR2_A_LINE_CHANNEL:return a->line_channel;
    case AFR2_A_LINE_THRESHOLD:return c->line_threshold[a->line_channel];case AFR2_A_MOTOR_SIGNS:return c->motor_signs;
    case AFR2_A_PWM_LIMIT:return c->pwm_limit;case AFR2_A_RAMP:return c->ramp_step;
    case AFR2_A_KP:return c->kp;case AFR2_A_KI:return c->ki;case AFR2_A_KD:return c->kd;
    case AFR2_A_MOTOR_READY:return c->motor_ready;case AFR2_A_WHEEL_MM:return c->wheel_mm;
    case AFR2_A_RADIUS_MM:return c->radius_sum_mm;case AFR2_A_HALL_WIRED:return c->hall_wired;
    case AFR2_A_GEOM_READY:return c->geometry_ready;case AFR2_A_ENC_STEPS:return c->encoder_steps;
    case AFR2_A_ENC_INVERT:return c->encoder_invert;case AFR2_A_CONTRAST:return c->contrast;
    case AFR2_A_ROTATION:return c->rotation;case AFR2_A_WIFI_MODE:return p->wifi.requested_mode?p->wifi.requested_mode:1;
    case AFR2_A_REMOTE_ENABLE:return p->remote_enabled;default:return 0;
    }
}
static uint8_t wifi_request(Afr2Product *p,uint8_t op) {
    Afr2Wifi *w=&p->wifi;
    if(!p->extension_ready || !w->available)return AFR2_UNAVAILABLE;
    if(w->pending)return AFR2_BUSY;
    ++w->id;if(!w->id)++w->id;w->op=op;w->pending=1;w->claimed=0;w->job_ms=p->now;
    return AFR2_OK;
}
static uint8_t local_command(Afr2Product *p) {
    if(p->owner==AFR2_REMOTE)return AFR2_DENIED;
    if(!p->owner)return Afr2Product_Acquire(p,AFR2_LOCAL,1,5000);
    p->lease_started_ms=p->now;return AFR2_OK;
}
static uint8_t numeric_range(uint8_t action,int32_t v) {
    Afr2MenuNode node;Afr2MenuItem item;
    for(uint8_t n=0;n<AFR2_N_COUNT;n++){
        Afr2Menu_Node(n,&node);
        for(uint8_t i=0;i<node.count;i++){
            Afr2Menu_Item(n,i,&item);
            if(item.action==action && item.step)return v>=item.min && v<=item.max &&
                ((action!=AFR2_A_ENC_STEPS) || v==2 || v==4);
        }
    }
    return 1;
}
static uint8_t invoke(void *ctx,uint8_t action,int32_t v,const void *data) {
    Afr2App *a=ctx;Afr2Product *p=a->product;Afr2Config old=p->config,*c=&p->config;
    if(!numeric_range(action,v))return AFR2_BAD;
    if(a->remote_ui && (action==AFR2_A_LOCAL_ACQUIRE || action==AFR2_A_RELEASE ||
       action==AFR2_A_MOVE || action==AFR2_A_LINE_START || action==AFR2_A_GOTO_START ||
       action==AFR2_A_WHEEL_START || action==AFR2_A_SERVO_APPLY || action==AFR2_A_SERVO_OFF ||
       action==AFR2_A_HALL_BEGIN || action==AFR2_A_HALL_END || action==AFR2_A_HALL_SAVE ||
       action==AFR2_A_REMOTE_ENABLE))return AFR2_DENIED;
    /* Configuration/edit changes require stopped outputs and no EEPROM write in flight. */
    uint8_t setting=action==AFR2_A_SPEED || action==AFR2_A_LINE_SPEED || action==AFR2_A_LINE_KP ||
        (action>=AFR2_A_TEST_MS && action<=AFR2_A_TEST_MS) ||
        (action>=AFR2_A_LINE_KIND && action<=AFR2_A_ENC_INVERT) ||
        action==AFR2_A_CONTRAST || action==AFR2_A_ROTATION;
    if(setting && (!p->stopped || p->prefs_state==1))return AFR2_BUSY;
    switch(action) {
    case AFR2_A_STOP:return Afr2Product_Stop(p)?AFR2_OK:AFR2_FAILED;
    case AFR2_A_LOCAL_ACQUIRE:
        if(p->owner==AFR2_REMOTE)Afr2Product_Stop(p);
        return Afr2Product_Acquire(p,AFR2_LOCAL,1,5000);
    case AFR2_A_RELEASE:Afr2Product_Stop(p);return AFR2_OK;
    case AFR2_A_SPEED:c->speed_limit=v;if(c->line_speed>v)c->line_speed=v;break;
    case AFR2_A_MOVE: {
        uint8_t granted=local_command(p);if(granted)return granted;
        int16_t s=c->speed_limit,vx=0,vy=0,wz=0;
        switch(v){case 0:vx=s;break;case 1:vx=-s;break;case 2:vy=s;break;case 3:vy=-s;break;
        case 4:vx=s;vy=s;break;case 5:vx=s;vy=-s;break;case 6:vx=-s;vy=s;break;case 7:vx=-s;vy=-s;break;
        case 8:wz=500;break;case 9:wz=-500;break;default:return AFR2_BAD;}
        return Afr2Product_Drive(p,vx,vy,wz,500);
    }
    case AFR2_A_LINE_SPEED:c->line_speed=v;break;case AFR2_A_LINE_KP:c->line_kp=v;break;
    case AFR2_A_LINE_START:{uint8_t r=local_command(p);if(r)return r;
        r=Afr2Product_StartLine(p);if(!r)p->lease_ms=30000;return r;}
    case AFR2_A_GOTO_X:if(!p->stopped)return AFR2_BUSY;p->target_x=v;return AFR2_OK;
    case AFR2_A_GOTO_Y:if(!p->stopped)return AFR2_BUSY;p->target_y=v;return AFR2_OK;
    case AFR2_A_GOTO_START:{uint8_t r=local_command(p);if(r)return r;
        r=Afr2Product_StartGoto(p,p->target_x,p->target_y);if(!r)p->lease_ms=60000;return r;}
    case AFR2_A_WHEEL:if(v<0 || v>3 || p->hall_measuring || !p->stopped)return AFR2_BAD;p->selected_wheel=v;return AFR2_OK;
    case AFR2_A_TEST_PWM:if(v<-255 || v>255 || !p->stopped)return AFR2_BAD;a->test_pwm=v;return AFR2_OK;
    case AFR2_A_TEST_MS:c->test_ms=v;break;
    case AFR2_A_WHEEL_START:{uint8_t r=local_command(p);if(r)return r;return Afr2Product_Wheel(p,p->selected_wheel,a->test_pwm,c->test_ms);}
    case AFR2_A_HALL_BEGIN:Afr2Product_HallBegin(p);return p->hall_measuring?AFR2_OK:AFR2_UNAVAILABLE;
    case AFR2_A_HALL_END:return Afr2Product_HallEnd(p);
    case AFR2_A_HALL_SAVE:
        if(p->prefs_state==1)return AFR2_BUSY;
        if(p->hall_measuring || p->hall_bad || !p->hall_delta)return AFR2_BAD;
        Afr2Product_HallSave(p);Afr2Product_Save(p);return AFR2_OK;
    case AFR2_A_HALL_CANCEL:
        if(!a->remote_ui && p->owner==AFR2_LOCAL && !p->stopped)Afr2Product_Stop(p);
        p->hall_measuring=0;p->hall_delta=0;p->hall_bad=0;return AFR2_OK;
    case AFR2_A_SERVO_US:if(v<1000 || v>2000)return AFR2_BAD;a->servo_us=v;return AFR2_OK;
    case AFR2_A_SERVO_APPLY:if(!p->stopped)return AFR2_BUSY;Afr2Servo_Set(a->servo_us);return AFR2_OK;
    case AFR2_A_SERVO_OFF:Afr2Servo_Set(0);return AFR2_OK;
    case AFR2_A_SAVE:if(!p->stopped || p->prefs_state==1)return AFR2_BUSY;Afr2Product_Save(p);return AFR2_OK;
    case AFR2_A_DISCARD:if(!p->stopped || p->prefs_state==1)return AFR2_BUSY;*c=a->saved_config;return AFR2_OK;
    case AFR2_A_LINE_KIND:c->line_kind=v;c->line_calibrated=0;break;
    case AFR2_A_LINE_INVERT:c->line_invert=v;c->line_calibrated=0;break;
    case AFR2_A_LINE_CHANNEL:if(v<0 || v>4)return AFR2_BAD;a->line_channel=v;return AFR2_OK;
    case AFR2_A_LINE_THRESHOLD:c->line_threshold[a->line_channel]=v;c->line_calibrated=0;break;
    case AFR2_A_LINE_CONFIRM:if(!c->line_kind)return AFR2_UNAVAILABLE;c->line_calibrated=1;break;
    case AFR2_A_MOTOR_SIGNS:c->motor_signs=v;break;case AFR2_A_PWM_LIMIT:c->pwm_limit=v;break;
    case AFR2_A_RAMP:c->ramp_step=v;break;case AFR2_A_KP:c->kp=v;break;case AFR2_A_KI:c->ki=v;break;case AFR2_A_KD:c->kd=v;break;
    case AFR2_A_MOTOR_READY:c->motor_ready=v;break;case AFR2_A_WHEEL_MM:c->wheel_mm=v;c->geometry_ready=0;p->pose_valid=0;break;
    case AFR2_A_RADIUS_MM:c->radius_sum_mm=v;c->geometry_ready=0;p->pose_valid=0;break;
    case AFR2_A_HALL_WIRED:c->hall_wired=v;break;case AFR2_A_GEOM_READY:c->geometry_ready=v;break;
    case AFR2_A_ENC_STEPS:c->encoder_steps=v;break;case AFR2_A_ENC_INVERT:c->encoder_invert=v;break;
    case AFR2_A_WIFI_STATUS:return wifi_request(p,AFR2_WIFI_STATUS);
    case AFR2_A_WIFI_SCAN:return wifi_request(p,AFR2_WIFI_SCAN);
    case AFR2_A_WIFI_WEB:return wifi_request(p,AFR2_WIFI_WEB_DRAFT);
    case AFR2_A_WIFI_RETRY:return wifi_request(p,AFR2_WIFI_RETRY);
    case AFR2_A_WIFI_MODE:case AFR2_A_WIFI_SET_MODE:
        if(v<1 || v>3 || p->wifi.pending)return AFR2_BAD;
        p->wifi.requested_mode=v;p->wifi.draft_ready=0;return AFR2_OK;
    case AFR2_A_WIFI_NETWORK:
        if(v<0 || v>=p->wifi.network_count)return AFR2_UNAVAILABLE;
        p->wifi.selected_network=v;strcpy(p->wifi.draft_ssid,p->wifi.networks[v]);p->wifi.draft_ready=0;return AFR2_OK;
    case AFR2_A_KEYBOARD: {
        if(!data)return AFR2_BAD;const Afr2UiCredentials *cred=data;
        uint8_t r=wifi_request(p,AFR2_WIFI_LOCAL_DRAFT);if(r)return r;
        strcpy(p->wifi.draft_ssid,cred->ssid);strcpy(p->wifi.password,cred->password);
        p->wifi.credential_length=(uint8_t)strlen(cred->ssid)+1u+(uint8_t)strlen(cred->password);
        p->wifi.draft_ready=0;return AFR2_OK;
    }
    case AFR2_A_WIFI_CONNECT:
        if(!p->wifi.draft_ready || (uint32_t)(p->now-p->wifi.draft_ms)>120000u)return AFR2_UNAVAILABLE;
        return wifi_request(p,AFR2_WIFI_CONNECT);
    case AFR2_A_WIFI_CANCEL:
        wipe(p->wifi.password,sizeof(p->wifi.password));p->wifi.credential_length=0;p->wifi.draft_ready=0;
        /* Keep only a cancellation request, superseding a waiting modal/draft. */
        p->wifi.pending=0;return wifi_request(p,AFR2_WIFI_CANCEL);
    case AFR2_A_CONTRAST:c->contrast=v;break;case AFR2_A_ROTATION:c->rotation=v;break;
    case AFR2_A_CLEAR_FAULT:if(!p->stopped)return AFR2_DENIED;p->fault=0;p->mode=AFR2_IDLE;return AFR2_OK;
    case AFR2_A_REMOTE_ENABLE:if(!p->stopped || v<0 || v>1)return AFR2_DENIED;p->remote_enabled=v;return AFR2_OK;
    default:return AFR2_UNAVAILABLE;
    }
    if(!Afr2Product_ConfigValid(c)){*c=old;return AFR2_BAD;}return AFR2_OK;
}
static void keyboard_seed(void *ctx,Afr2UiCredentials *out) {
    Afr2App *a=ctx;strncpy(out->ssid,a->product->wifi.draft_ssid,32);out->ssid[32]=0;
}
static uint8_t available(void *ctx,uint8_t action,int32_t arg) {
    Afr2App *a=ctx;Afr2Product *p=a->product;
    if(action==AFR2_A_MOVE || action==AFR2_A_WHEEL_START || action==AFR2_A_LINE_START || action==AFR2_A_GOTO_START) {
        if(p->owner==AFR2_REMOTE)return AFR2_DENIED;
        if(action!=AFR2_A_WHEEL_START && (!p->config.motor_ready || !p->pose_valid))return AFR2_UNAVAILABLE;
        if(action==AFR2_A_LINE_START && !p->sensors.line_valid)return AFR2_UNAVAILABLE;
    }
    if(action==AFR2_A_HALL_BEGIN && !p->sensors.hall_available)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_HALL_END && !p->hall_measuring)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_HALL_SAVE && (p->hall_measuring || p->hall_bad || !p->hall_delta))return AFR2_UNAVAILABLE;
    if((action>=AFR2_A_WIFI_STATUS && action<=AFR2_A_WIFI_SET_MODE) &&
        action!=AFR2_A_WIFI_MODE && action!=AFR2_A_WIFI_SET_MODE && action!=AFR2_A_WIFI_CANCEL &&
        (!p->extension_ready || !p->wifi.available))return AFR2_UNAVAILABLE;
    if(action==AFR2_A_WIFI_NETWORK && arg>=p->wifi.network_count)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_WIFI_CONNECT && !p->wifi.draft_ready)return AFR2_UNAVAILABLE;
    return AFR2_OK;
}
static const char *reason(void *ctx,uint8_t action) {
    Afr2Product *p=((Afr2App *)ctx)->product;
    if(action>=AFR2_A_WIFI_STATUS && action<=AFR2_A_WIFI_SET_MODE) {
        if(!p->extension_ready)return "ESP sin extension";
        if(!p->wifi.available)return "ESP sin estado";
        if(action==AFR2_A_WIFI_CONNECT && !p->wifi.draft_ready)return "Falta borrador";
        return "Ver estado Wi-Fi";
    }
    if(action==AFR2_A_HALL_BEGIN || action==AFR2_A_HALL_END || action==AFR2_A_HALL_SAVE ||
       action==AFR2_A_MOVE || action==AFR2_A_LINE_START || action==AFR2_A_GOTO_START) {
        if(!p->sensors.hall_available)return "Conectar Hall";
        for(uint8_t i=0;i<4;i++)if(!p->config.counts_per_rev[i])return "Calibrar vuelta";
        if(!p->config.geometry_ready)return "Config. geometria";
        if(!p->config.motor_ready)return "Habilitar motores";
        if(action==AFR2_A_LINE_START && !p->sensors.line_valid)return "Calibrar linea";
    }
    return "Ver requisitos";
}
void Afr2App_Init(Afr2App *a,Afr2Product *p,Afr2Ui *u,Afr2Oled *o,Afr2Link *l) {
    memset(a,0,sizeof(*a));a->product=p;a->ui=u;a->oled=o;a->link=l;a->saved_config=p->config;
    a->test_pwm=40;a->servo_us=1500;p->wifi.requested_mode=1;
    Afr2UiOps ops={a,invoke,value,keyboard_seed,available,reason};Afr2Ui_Attach(u,&ops);
}
void Afr2App_Revoke(void *ctx) {
    Afr2App *a=ctx;Afr2Product *p=a->product;
    Afr2Product_Stop(p);p->extension_ready=0;p->remote_enabled=0;p->wifi.available=0;
    p->wifi.pending=0;p->wifi.draft_ready=0;p->wifi.credential_length=0;
    wipe(p->wifi.password,sizeof(p->wifi.password));a->snapshot_active=0;
    if(a->ui->keyboard){wipe(&a->ui->credentials,sizeof(a->ui->credentials));a->ui->keyboard=0;a->ui->redraw=true;}
}
void Afr2App_Task(Afr2App *a,uint32_t now) {
    Afr2Product *p=a->product;uint8_t before=p->prefs_state;Afr2Product_Task(p,now);
    if(before==1 && p->prefs_state==2){a->saved_config=p->config;p->prefs_loaded=1;}
    if(a->snapshot_active && (uint32_t)(now-a->snapshot_ms)>600u)a->snapshot_active=0;
    if(p->wifi.available && (uint32_t)(now-p->wifi.changed_ms)>5000u && !p->wifi.pending)p->wifi.available=0;
    if(p->wifi.pending && (uint32_t)(now-p->wifi.job_ms)>(p->wifi.op==AFR2_WIFI_WEB_DRAFT?120000u:30000u)) {
        p->wifi.pending=0;p->wifi.draft_ready=0;wipe(p->wifi.password,sizeof(p->wifi.password));p->wifi.credential_length=0;
        Afr2Ui_Notify(a->ui,1,"Wi-Fi","Tiempo agotado",3000);
    }
    if(p->wifi.draft_ready && (uint32_t)(now-p->wifi.draft_ms)>120000u){p->wifi.draft_ready=0;p->wifi.draft_id=0;}
    if(p->notice){Afr2Ui_Notify(a->ui,p->notice==2?1:0,"Sistema",p->notice==1?"Operacion lista":p->notice==3?"Guardando EEPROM":"Ver requisitos",3000);p->notice=0;}
    Afr2Ui_Tick(a->ui,now);
    if((uint32_t)(now-a->render_ms)>=250u){a->render_ms=now;a->ui->redraw=true;}
}
#define LINE(row,fmt,...) snprintf_P(s->lines[row],sizeof(s->lines[row]),PSTR(fmt),##__VA_ARGS__)
void Afr2App_UiSnapshot(Afr2App *a,Afr2UiSnapshot *s) {
    Afr2Product *p=a->product;Afr2Ui *u=a->ui;memset(s,0,sizeof(*s));
    s->line_valid=p->sensors.line_valid;s->line_bits=p->sensors.line_bits;
    s->ultrasonic_valid=p->sensors.ultrasonic_valid;s->ultrasonic_mm=p->sensors.distance_mm;
    s->oled_ready=a->oled->initialized;s->oled_errors=a->oled->errors;
    switch(u->node) {
    case AFR2_N_HOME:case AFR2_N_ROBOT_VIEW:
        LINE(0,"Modo:%u Mando:%u",p->mode,p->owner);LINE(1,"Salidas OFF:%u",p->stopped);LINE(2,"SW largo: menu");break;
    case AFR2_N_LINK_VIEW:
        LINE(0,"UART:%u Ext:%u",a->link->negotiated,p->extension_ready);LINE(1,"Mando:%u Remoto:%u",p->owner,p->remote_enabled);
        LINE(2,"XOR mal:%u",a->link->parser.stats.checksum);break;
    case AFR2_N_ALERT_VIEW:case AFR2_N_DIAG:
        LINE(0,"Fallo:%u Reset:%u",p->fault,p->reset_cause);LINE(1,"OLED err:%lu",(unsigned long)a->oled->errors);
        LINE(2,"RX gaps:%u",a->link->parser.stats.gap);break;
    case AFR2_N_LINE_VIEW:
        LINE(0,"Raw:%u %u",p->sensors.line_raw[0],p->sensors.line_raw[1]);
        LINE(1,"%u %u %u",p->sensors.line_raw[2],p->sensors.line_raw[3],p->sensors.line_raw[4]);
        LINE(2,"Bits:%02X Val:%u",p->sensors.line_bits,p->sensors.line_valid);break;
    case AFR2_N_US_VIEW:
        if(p->sensors.ultrasonic_valid)LINE(0,"Dist:%u mm",p->sensors.distance_mm);else LINE(0,"Distancia: --");
        LINE(1,"Eco error:%u",p->sensors.echo_error);LINE(2,"Edad:%lu ms",(unsigned long)(p->now-p->sensors.ultrasonic_ms));break;
    case AFR2_N_HALL_VIEW:
        if(!p->sensors.hall_available){LINE(0,"Hall sin conectar");LINE(1,"A8..A15 propuesto");LINE(2,"Captura bloqueada");}
        else {LINE(0,"Rueda:%u C:%ld",p->selected_wheel,(long)p->sensors.counts[p->selected_wheel]);
        LINE(1,"Vuelta:%ld E:%u",(long)p->hall_delta,p->hall_bad);LINE(2,"CPR:%lu",(unsigned long)p->config.counts_per_rev[p->selected_wheel]);}break;
    case AFR2_N_WHEEL_VIEW:
        LINE(0,"FL:%d FR:%d",p->pwm[0],p->pwm[1]);LINE(1,"RL:%d RR:%d",p->pwm[2],p->pwm[3]);LINE(2,"OFF:%u",p->stopped);break;
    case AFR2_N_DISPLAY:
        LINE(0,"OLED listo:%u",s->oled_ready);LINE(1,"Errores:%lu",(unsigned long)s->oled_errors);LINE(2,"TWI IRQ 100kHz");break;
    case AFR2_N_ENCODER_VIEW:
        LINE(0,"Giros:%ld",(long)u->encoder_turns);LINE(1,"Clics:%u",u->encoder_clicks);LINE(2,"Largos:%u",u->encoder_holds);break;
    case AFR2_N_LINE_REQ:
        LINE(0,"Linea valida:%u",p->sensors.line_valid);LINE(1,"Hall:%u Geom:%u",p->sensors.hall_available,p->config.geometry_ready);
        LINE(2,"Motor habilit:%u",p->config.motor_ready);break;
    case AFR2_N_GOTO_REVIEW:LINE(0,"X:%ld mm",(long)p->target_x);LINE(1,"Y:%ld mm",(long)p->target_y);LINE(2,"V max:%u mm/s",p->config.speed_limit);break;
    case AFR2_N_GOTO_RESULT:LINE(0,"Estado GOTO:%u",p->goto_state);LINE(1,"Pose valida:%u",p->pose_valid);LINE(2,"Salidas OFF:%u",p->stopped);break;
    case AFR2_N_POSE:
        if(!p->pose_valid){LINE(0,"Sin odometria");LINE(1,"Conectar Hall");LINE(2,"Calibrar geometria");}
        else {LINE(0,"X:%ld Y:%ld",(long)p->x_mm,(long)p->y_mm);LINE(1,"Yaw:%ld mrad",(long)p->yaw_mrad);LINE(2,"Estimacion ruedas");}break;
    case AFR2_N_WIFI_VIEW:
        if(!p->wifi.available)LINE(0,"ESP: sin servicio");else LINE(0,"Modo:%u STA:%u",p->wifi.mode,p->wifi.sta_connected);
        LINE(1,"%.17s",p->wifi.ssid);LINE(2,"%.17s",p->wifi.ip);break;
    case AFR2_N_WIFI_REVIEW:
        LINE(0,"%.17s",p->wifi.draft_ssid);LINE(1,"Modo:%u Listo:%u",p->wifi.requested_mode,p->wifi.draft_ready);LINE(2,"Giro: opciones");break;
    case AFR2_N_WIFI_NETWORKS:
        for(uint8_t row=0;row<3;row++){uint8_t i=u->firstVisible+row;if(i<8 && i<p->wifi.network_count)strncpy(s->labels[row],p->wifi.networks[i],17);}break;
    case AFR2_N_VERSIONS:LINE(0,"Mega 0.4.0");LINE(1,"ESP:%.13s",p->wifi.esp_version[0]?p->wifi.esp_version:"--");
        LINE(2,"Web:%.13s",p->wifi.web_version[0]?p->wifi.web_version:"--");break;
    case AFR2_N_CAPS:LINE(0,"UI/TWI/UART/STOP");LINE(1,"Linea:%u Hall:%u",p->sensors.line_valid,p->sensors.hall_available);LINE(2,"GOTO req:%u",p->pose_valid);break;
    case AFR2_N_PREFS_VIEW:LINE(0,"EEPROM valida:%u",p->prefs_loaded);LINE(1,"Guardado estado:%u",p->prefs_state);LINE(2,"CRC/version 1");break;
    default:break;
    }
    Afr2MenuNode node;Afr2Menu_Node(u->node,&node);
    for(uint8_t row=0;row<3 && u->firstVisible+row<node.count;row++) {
        Afr2MenuItem item;Afr2Menu_Item(u->node,u->firstVisible+row,&item);
        s->disabled[row]=item.action && available(a,item.action,item.arg)!=AFR2_OK;
    }
}
