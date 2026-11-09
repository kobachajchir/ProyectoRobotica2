/* AFR2-20261030-01: AutoMicro dashboard adaptation and linked icons; prior authors retained. */
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
#include "afr2_build_info.h"
/* AFR2-20261023-19. Application wrappers: own AVR services, no F4 globals.
 * Only this wrapper prepares presentation snapshots; renderer stays pure.
 */
#include "afr2_app.h"
#include "afr2_debug.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <avr/pgmspace.h>
static void wipe(void *data,uint16_t n){volatile uint8_t *p=data;while(n--)*p++=0;}
static uint8_t config_dirty(const Afr2Config *a,const Afr2Config *b){
    if(a->line_kind!=b->line_kind || a->line_invert!=b->line_invert || a->line_calibrated!=b->line_calibrated || a->motor_signs!=b->motor_signs || a->motor_ready!=b->motor_ready || a->hall_wired!=b->hall_wired || a->hall_signs!=b->hall_signs || a->hall_cw_mask!=b->hall_cw_mask || a->geometry_ready!=b->geometry_ready || a->encoder_steps!=b->encoder_steps || a->encoder_invert!=b->encoder_invert || a->contrast!=b->contrast || a->rotation!=b->rotation || a->pwm_limit!=b->pwm_limit || a->ramp_step!=b->ramp_step || a->radar_range!=b->radar_range || a->radar_trace!=b->radar_trace || a->long_press_ms!=b->long_press_ms || a->wifi_retry_s!=b->wifi_retry_s || a->servo_inverted!=b->servo_inverted || a->wheel_mm!=b->wheel_mm || a->radius_sum_mm!=b->radius_sum_mm || a->speed_limit!=b->speed_limit || a->test_ms!=b->test_ms || a->kp!=b->kp || a->ki!=b->ki || a->kd!=b->kd || a->line_kp!=b->line_kp || a->line_speed!=b->line_speed)return 1;
    for(uint8_t i=0;i<5;i++)if(a->line_threshold[i]!=b->line_threshold[i])return 1;
    for(uint8_t i=0;i<4;i++)if(a->counts_per_rev[i]!=b->counts_per_rev[i])return 1;
    return 0;
}
static uint8_t available(void *ctx,uint8_t action,int32_t arg);
/* AFR2-20261108-04: arranque comun de la prueba de distancia (menu Iniciar y confirmacion del encoder). */
static uint8_t odom_start(Afr2App *a) {
    Afr2Product *p=a->product;
    int32_t d=(int32_t)a->odom_cm*10,e=d*707/1000,x=0,y=0;
    switch(a->odom_dir){case 0:x=d;break;case 1:x=-d;break;case 2:y=d;break;case 3:y=-d;break;
    case 4:x=e;y=e;break;case 5:x=e;y=-e;break;case 6:x=-e;y=e;break;default:x=-e;y=-e;break;}
    uint8_t rc=AFR2_OK;
    if(Afr2Head_State()->running)rc=AFR2_BUSY;
    else {if(p->origin==AFR2_SOURCE_WEB)Afr2Product_HaltMotion(p);p->origin=AFR2_SOURCE_LOCAL;p->local_timeout_ms=5000;p->local_started_ms=p->now;}
    if(rc)return rc;
    rc=Afr2Product_StartGoto(p,x,y);if(!rc)p->local_timeout_ms=60000;return rc;
}
static int32_t value(void *ctx,uint8_t action) {
    Afr2App *a=ctx;Afr2Product *p=a->product;Afr2Config *c=&p->config;
    const Afr2SensorHead *h=Afr2Head_State();
    switch(action) {
    case AFR2_A_WIFI_FALLBACK:return p->wifi.policy_valid?p->wifi.ask_fallback:2;
    case AFR2_A_WIFI_AP_TOGGLE:return p->wifi.ap_active;
    case AFR2_A_WIFI_CONNECT:return p->wifi.draft_ready;
    case AFR2_A_WIFI_CONNECTED:return p->wifi.available?(p->wifi.sta_connected?1:0):2;
    case AFR2_A_WIFI_COUNT:return p->wifi.pending?0:p->wifi.network_count;
    case AFR2_A_WIFI_PAGE_INDEX:return p->wifi.scan_page;
    case AFR2_A_WIFI_TOTAL:return p->wifi.scan_total;
    case AFR2_A_WIFI_BUSY:return p->wifi.pending;
    case AFR2_A_WIFI_SCAN_STATE:return (p->wifi.op==AFR2_WIFI_SCAN || p->wifi.op==AFR2_WIFI_PAGE) && p->wifi.pending?1:(p->wifi.op==AFR2_WIFI_SCAN || p->wifi.op==AFR2_WIFI_PAGE) && p->wifi.state>=4?3:p->wifi.scan_id?2:0;
    case AFR2_A_WIFI_ROW_AUTH:return p->wifi.network_security[p->wifi.selected_network];
    case AFR2_A_DIRTY:return config_dirty(c,&a->saved_config);
    case AFR2_A_LONG_MS:return c->long_press_ms;
    case AFR2_A_WIFI_RETRY_S:return c->wifi_retry_s; /* AFR2-20261108-06 */
    case AFR2_A_POWER:return a->local_power_percent;
    case AFR2_A_ESP_TIMEOUT:return a->diag_timeout;
    case AFR2_A_WHEEL_TOGGLE:return p->mode==AFR2_WHEEL_TEST && !p->stopped;
    case AFR2_A_GOTO_TOGGLE:case AFR2_A_ODOM_TOGGLE:return p->mode==AFR2_GOTO && !p->stopped;
    case AFR2_A_ODOM_PROPOSE:case AFR2_A_ODOM_STATE:return ((int32_t)a->odom_token<<8)|a->odom_state;
    case AFR2_A_ODOM_DIR:return a->odom_dir;
    case AFR2_A_ODOM_CM:return a->odom_cm;
    case AFR2_A_RADAR_RANGE:return c->radar_range;
    case AFR2_A_RADAR_TRACE:return c->radar_trace;
    case AFR2_A_RADAR_DIRTY:return c->radar_range!=a->saved_config.radar_range || c->radar_trace!=a->saved_config.radar_trace;
    case AFR2_A_RADAR_TOGGLE:return h->running;
    case AFR2_A_US_TOGGLE:return h->sampling;
    case AFR2_A_IR_TOGGLE:return h->ir_enabled;
    case AFR2_A_SERVO_TOGGLE:return h->enabled;
    case AFR2_A_MOTOR_TOGGLE:return c->motor_ready;
    case AFR2_A_LINE_TOGGLE:return p->mode==AFR2_LINE && !p->stopped;
    case AFR2_A_SERVO_ANGLE:return h->target_us?(h->inverted?180-((int32_t)(h->target_us-1000)*180+500)/1000:((int32_t)(h->target_us-1000)*180+500)/1000):h->angle;
    case AFR2_A_RADAR_MIN:return h->min_angle;case AFR2_A_RADAR_MAX:return h->max_angle;case AFR2_A_RADAR_SPEED:return h->speed;
    case AFR2_A_RADAR_SETTLE:return h->settle_ms;case AFR2_A_US_INTERVAL:return h->interval_ms;case AFR2_A_SERVO_INVERT:return c->servo_inverted;
    case AFR2_A_SPEED:return c->speed_limit;case AFR2_A_LINE_SPEED:return c->line_speed;
    case AFR2_A_LINE_KP:return c->line_kp;case AFR2_A_GOTO_X:return p->target_x;case AFR2_A_GOTO_Y:return p->target_y;
    case AFR2_A_WHEEL:return p->selected_wheel;case AFR2_A_TEST_PWM:return a->test_pwm;case AFR2_A_TEST_MS:return c->test_ms;
    case AFR2_A_SERVO_US:return a->servo_us;case AFR2_A_LINE_KIND:return c->line_kind;
    case AFR2_A_LINE_INVERT:return c->line_invert;case AFR2_A_LINE_CHANNEL:return a->line_channel;
    case AFR2_A_LINE_THRESHOLD:return c->line_threshold[a->line_channel];case AFR2_A_MOTOR_SIGNS:return c->motor_signs;
    case AFR2_A_PWM_LIMIT:return c->pwm_limit;case AFR2_A_RAMP:return c->ramp_step;
    case AFR2_A_KP:return c->kp;case AFR2_A_KI:return c->ki;case AFR2_A_KD:return c->kd;
    case AFR2_A_MOTOR_READY:return c->motor_ready;case AFR2_A_WHEEL_MM:return c->wheel_mm;
    case AFR2_A_HALL_SAVE:return a->hall_draft_mask;
    case AFR2_A_HALL_END:return p->hall_delta;
    case AFR2_A_SAVE:return p->prefs_state;
    case AFR2_A_RADIUS_MM:return c->radius_sum_mm;case AFR2_A_HALL_WIRED:return c->hall_wired;
    case AFR2_A_GEOM_READY:return c->geometry_ready;case AFR2_A_ENC_STEPS:return c->encoder_steps;
    case AFR2_A_ENC_INVERT:return c->encoder_invert;case AFR2_A_CONTRAST:return c->contrast;
    case AFR2_A_ROTATION:return c->rotation;case AFR2_A_WIFI_MODE:return p->wifi.mode;
    default:return 0;
    }
}
static uint8_t wifi_request(Afr2Product *p,uint8_t op) {
    Afr2Wifi *w=&p->wifi;
    if(!p->extension_ready || (!w->available && op!=AFR2_WIFI_CANCEL && op!=AFR2_WIFI_STATUS))return AFR2_UNAVAILABLE;
    if(w->pending)return AFR2_BUSY;
    if(op==AFR2_WIFI_SCAN){memset(w->network_security,0,sizeof(w->network_security));w->network_count=0;w->selected_network=0;memset(w->networks,0,sizeof(w->networks));}
    ++w->id;if(!w->id)++w->id;w->op=op;w->pending=1;w->claimed=0;w->job_ms=p->now;
    return AFR2_OK;
}
static uint8_t local_command(Afr2Product *p) {
    if(Afr2Head_State()->running)return AFR2_BUSY;
    if(p->origin==AFR2_SOURCE_WEB)Afr2Product_HaltMotion(p);
    p->origin=AFR2_SOURCE_LOCAL;p->local_timeout_ms=5000;p->local_started_ms=p->now;return AFR2_OK;
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
uint8_t Afr2App_RemoteUiAllowed(uint8_t action) {
    switch(action) {
    case AFR2_A_WIFI_FALLBACK:case AFR2_A_WIFI_AP_TOGGLE:case AFR2_A_WIFI_DISCONNECT:case AFR2_A_WIFI_CHANGE_MODE:case AFR2_A_WIFI_SET_MODE:case AFR2_A_ESP_TEST:case AFR2_A_WHEEL_TOGGLE:case AFR2_A_GOTO_TOGGLE:case AFR2_A_ODOM_TOGGLE:case AFR2_A_ODOM_PROPOSE:case AFR2_A_ODOM_ANSWER:case AFR2_A_POWER:case AFR2_A_QUICK_MODE:case AFR2_A_RADAR_TOGGLE:case AFR2_A_US_TOGGLE:case AFR2_A_IR_TOGGLE:case AFR2_A_SERVO_TOGGLE:case AFR2_A_MOTOR_TOGGLE:case AFR2_A_LINE_TOGGLE:
    case AFR2_A_SERVO_EDIT_BEGIN:case AFR2_A_SERVO_EDIT_END:case AFR2_A_RADAR_DISCARD:
    case AFR2_A_MOVE:case AFR2_A_LINE_START:
    case AFR2_A_GOTO_START:case AFR2_A_WHEEL_START:case AFR2_A_SERVO_APPLY:case AFR2_A_SERVO_OFF:
    case AFR2_A_HALL_BEGIN:case AFR2_A_HALL_END:case AFR2_A_HALL_SAVE:case AFR2_A_HALL_CANCEL:
    case AFR2_A_LINE_CONFIRM:case AFR2_A_SAVE:case AFR2_A_DISCARD:case AFR2_A_CLEAR_FAULT:
    case AFR2_A_WIFI_READ_CREDENTIALS:case AFR2_A_WIFI_CONNECT:case AFR2_A_KEYBOARD:return 0;
    case AFR2_A_REBOOT_MEGA:case AFR2_A_REBOOT_ESP:return 0;
    case AFR2_A_SERVO_ANGLE:case AFR2_A_SERVO_CENTER:case AFR2_A_RADAR_START:case AFR2_A_RADAR_STOP:
    case AFR2_A_RADAR_MIN:case AFR2_A_RADAR_MAX:case AFR2_A_RADAR_SPEED:case AFR2_A_RADAR_SETTLE:case AFR2_A_US_INTERVAL:case AFR2_A_SERVO_INVERT:return 0;
    default:return 1;
    }
}
static uint8_t invoke(void *ctx,uint8_t action,int32_t v,const void *data) {
    Afr2App *a=ctx;Afr2Product *p=a->product;Afr2Config old=p->config,*c=&p->config;
    if(!numeric_range(action,v))return AFR2_BAD;
    uint8_t visual_choice=a->ui->node==AFR2_N_SAVE_CONFIRM && a->ui->confirm_save==AFR2_A_SAVE && a->ui->confirm_discard==AFR2_A_RADAR_DISCARD && (action==AFR2_A_SAVE || action==AFR2_A_RADAR_DISCARD);
    if(a->remote_ui && !Afr2App_RemoteUiAllowed(action) && !visual_choice)return AFR2_DENIED;
    /* Configuration/edit changes require stopped outputs and no EEPROM write in flight. */
    uint8_t setting=action==AFR2_A_SPEED || action==AFR2_A_LINE_SPEED || action==AFR2_A_LINE_KP ||
        (action>=AFR2_A_TEST_MS && action<=AFR2_A_TEST_MS) ||
        (action>=AFR2_A_LINE_KIND && action<=AFR2_A_ENC_INVERT) ||
        action==AFR2_A_LONG_MS || action==AFR2_A_WIFI_RETRY_S || action==AFR2_A_CONTRAST || action==AFR2_A_ROTATION || action==AFR2_A_RADAR_RANGE || action==AFR2_A_RADAR_TRACE;
    if(setting && ((!p->stopped && !(action==AFR2_A_MOTOR_READY && !v)) || p->prefs_state==1 || p->hall_measuring))return AFR2_BUSY;
    const Afr2SensorHead *h=Afr2Head_State();
    if(action==AFR2_A_SERVO_INVERT){
        if(v<0 || v>1 || !p->stopped || h->enabled || h->target_us || h->running || p->prefs_state==1)return AFR2_BUSY;
        uint8_t rc=Afr2Head_Configure(h->min_angle,h->max_angle,h->speed,h->settle_ms,h->interval_ms,v);
        if(!rc){c->servo_inverted=v;}return rc;
    }
    if(action>=AFR2_A_SERVO_ANGLE && action<=AFR2_A_SERVO_INVERT){
        if(!p->stopped || p->fault || p->hall_measuring)return AFR2_BUSY;
        switch(action){
        case AFR2_A_SERVO_ANGLE:return Afr2Head_Servo(1,v);case AFR2_A_SERVO_CENTER:return Afr2Head_Servo(2,90);
        case AFR2_A_RADAR_START:if(p->sequence.state>=1 && p->sequence.state<=3)return AFR2_BUSY;return Afr2Head_Start();case AFR2_A_RADAR_STOP:Afr2Head_Stop();return AFR2_OK;
        default:return Afr2Head_Configure(action==AFR2_A_RADAR_MIN?v:h->min_angle,action==AFR2_A_RADAR_MAX?v:h->max_angle,
            action==AFR2_A_RADAR_SPEED?v:h->speed,action==AFR2_A_RADAR_SETTLE?v:h->settle_ms,action==AFR2_A_US_INTERVAL?v:h->interval_ms,action==AFR2_A_SERVO_INVERT?v:h->inverted);
        }
    }
    switch(action) {
    case AFR2_A_LONG_MS:if(v<800 || v>3000 || v%100)return AFR2_BAD;c->long_press_ms=v;break;
    case AFR2_A_WIFI_RETRY_S:if(v<10 || v>300 || v%10)return AFR2_BAD;c->wifi_retry_s=v;break; /* AFR2-20261108-06 */
    case AFR2_A_WIFI_ASK: /* AFR2-20261108-06: 1 Iniciar AP, 2 Seguir esperando (el ESP recibe los segundos de la preferencia) */
        if(v!=1 && v!=2)return AFR2_BAD;
        {uint8_t r=wifi_request(p,v==1?AFR2_WIFI_AP_START:AFR2_WIFI_WAIT);
         if(!r && v==1)Afr2Ui_NotifyFlash(a->ui,0,PSTR("Wi-Fi"),PSTR("Iniciando AP"),2500); /* AFR2-20261109-01 */
         return r;}
    case AFR2_A_POWER:if(v<0 || v>100)return AFR2_BAD;if(!p->stopped)return AFR2_BUSY;a->local_power_percent=v;a->ui->power_percent=v;a->ui->power_known=1;a->ui->power_changed_ms=p->now;return AFR2_OK;
    case AFR2_A_ESP_TIMEOUT:if(v<1000 || v>5000 || v%250)return AFR2_BAD;if(a->diag_state==1)return AFR2_BUSY;a->diag_timeout=v;return AFR2_OK;
    case AFR2_A_ESP_TEST:
        if(a->diag_state==1)return AFR2_BUSY;
        if(!p->extension_ready)return AFR2_UNAVAILABLE;
        if(!++a->diag_job){++a->diag_job;}a->diag_state=1;a->diag_started=p->now;return AFR2_OK;
    case AFR2_A_WHEEL_TOGGLE:
        if(p->mode==AFR2_WHEEL_TEST && !p->stopped)return Afr2Product_Stop(p)?AFR2_OK:AFR2_FAILED;
        {uint8_t rc=local_command(p);if(rc)return rc;return Afr2Product_Wheel(p,p->selected_wheel,a->test_pwm,c->test_ms);}
    case AFR2_A_ODOM_DIR:if(v<0 || v>7)return AFR2_BAD;if(!p->stopped)return AFR2_BUSY;a->odom_dir=v;return AFR2_OK;
    case AFR2_A_ODOM_CM:if(v<5 || v>100 || v%5)return AFR2_BAD;if(!p->stopped)return AFR2_BUSY;a->odom_cm=v;return AFR2_OK;
    case AFR2_A_ODOM_TOGGLE:
        /* AFR2-20261108-03: prueba de distancia por GOTO relativo (ver odom_start). */
        if(p->mode==AFR2_GOTO && !p->stopped)return Afr2Product_Stop(p)?AFR2_OK:AFR2_FAILED;
        return odom_start(a);
    case AFR2_A_ODOM_PROPOSE: {
        /* AFR2-20261108-04: v = (token<<16)|(dir<<8)|cm. Abre la pantalla de confirmacion en el auto. */
        uint16_t token=(uint16_t)((uint32_t)v>>16);uint8_t dir=(uint8_t)(v>>8),cm=(uint8_t)v;
        if(v<0 || !token || dir>7 || cm<5 || cm>100 || cm%5)return AFR2_BAD;
        uint8_t node=a->ui->node;
        if(!p->stopped || p->fault || p->prefs_state==1 || p->hall_measuring || a->ui->touch_active || a->ui->keyboard || a->ui->edit ||
           node==AFR2_N_SAVE_CONFIRM || node==AFR2_N_HALL_PICK || node==AFR2_N_HALL_COUNT || node==AFR2_N_ENCODER_TEST || node==AFR2_N_ODOM_RUN || node==AFR2_N_WIFI_ASK)return AFR2_BUSY;
        {uint8_t rc=available(a,AFR2_A_ODOM_TOGGLE,0);if(rc)return rc;}
        a->odom_dir=dir;a->odom_cm=cm;a->odom_token=token;a->odom_state=1;a->odom_ask_ms=p->now;
        if(node!=AFR2_N_ODOM_ASK)Afr2Ui_Open(a->ui,AFR2_N_ODOM_ASK);else a->ui->redraw=true;
        return AFR2_OK;
    }
    case AFR2_A_ODOM_ANSWER: {
        if(a->odom_state!=1)return AFR2_UNAVAILABLE;
        if(!v){a->odom_state=3;return AFR2_OK;}
        {uint8_t rc=odom_start(a);a->odom_state=rc?4:2;return rc;}
    }
    case AFR2_A_GOTO_TOGGLE:
        if(p->mode==AFR2_GOTO && !p->stopped)return Afr2Product_Stop(p)?AFR2_OK:AFR2_FAILED;
        {uint8_t rc=local_command(p);if(rc)return rc;rc=Afr2Product_StartGoto(p,p->target_x,p->target_y);if(!rc)p->local_timeout_ms=60000;return rc;}
    case AFR2_A_RADAR_RANGE:if(v<0 || v>3)return AFR2_BAD;c->radar_range=v;break;
    case AFR2_A_RADAR_TRACE:if(v<0 || v>1)return AFR2_BAD;c->radar_trace=v;break;
    case AFR2_A_RADAR_DISCARD:
        if(p->prefs_state==1)return AFR2_BUSY;
        c->radar_range=a->saved_config.radar_range;c->radar_trace=a->saved_config.radar_trace;return AFR2_OK;
    case AFR2_A_US_TOGGLE:Afr2Head_SetSampling(!h->sampling);return AFR2_OK; /* AFR2-20261108-10; off also stops the radar (-13) */
    case AFR2_A_IR_TOGGLE: /* AFR2-20261108-13: no motors needed; off stops a running line follow */
        if(h->ir_enabled && p->mode==AFR2_LINE && !p->stopped)Afr2Product_HaltMotion(p);
        Afr2Head_SetIr(!h->ir_enabled);return AFR2_OK;
    case AFR2_A_RADAR_TOGGLE:
        if(h->running){Afr2Head_Stop();return AFR2_OK;}
        if(!p->stopped || p->fault || p->hall_measuring || (p->sequence.state>=1 && p->sequence.state<=3))return AFR2_BUSY;
        return Afr2Head_Start();
    case AFR2_A_SERVO_TOGGLE:
        if(h->enabled || h->target_us){Afr2Head_Stop();return AFR2_OK;}
        if(!p->stopped || p->fault || p->hall_measuring)return AFR2_BUSY;
        return Afr2Head_Servo(1,h->angle);
    case AFR2_A_MOTOR_TOGGLE:
        if(p->prefs_state==1 || p->hall_measuring)return AFR2_BUSY;
        if(c->motor_ready){Afr2Product_HaltMotion(p);c->motor_ready=0;}
        else {if(!p->stopped || p->fault)return AFR2_DENIED;c->motor_ready=1;}
        return AFR2_OK;
    case AFR2_A_LINE_TOGGLE:
        if(p->mode==AFR2_LINE && !p->stopped){Afr2Product_HaltMotion(p);return AFR2_OK;}
        {uint8_t r=local_command(p);if(r)return r;r=Afr2Product_StartLine(p);if(!r)p->local_timeout_ms=30000;return r;}
    case AFR2_A_SERVO_EDIT_BEGIN:
        if(!p->stopped || p->fault || p->hall_measuring || (p->sequence.state>=1 && p->sequence.state<=3))return AFR2_BUSY;
        Afr2Head_Pause();
        if(!h->target_us)return Afr2Head_Servo(1,h->angle);
        return AFR2_OK;
    case AFR2_A_SERVO_EDIT_END:
        if(!p->stopped || p->fault || p->hall_measuring){Afr2Head_CancelResume();return AFR2_OK;}
        Afr2Head_Resume();return AFR2_OK;
    case AFR2_A_QUICK_MODE:
        if(v!=AFR2_IDLE && v!=AFR2_MANUAL)return AFR2_BAD;
        if(p->fault)return AFR2_DENIED;
        if(v==AFR2_IDLE)return Afr2Product_Stop(p) && p->mode==AFR2_IDLE?AFR2_OK:AFR2_FAILED;
        if(!p->stopped || p->hall_measuring || p->prefs_state==1 || p->origin==AFR2_SOURCE_WEB || (p->sequence.state>=1 && p->sequence.state<=3))return AFR2_BUSY;
        p->mode=AFR2_MANUAL;return AFR2_OK;
    case AFR2_A_STOP:return Afr2Product_Stop(p)?AFR2_OK:AFR2_FAILED;
    case AFR2_A_BATTERY_MEASURE:Afr2Battery_Measure(&p->sensors);return AFR2_OK;
    case AFR2_A_SPEED:c->speed_limit=v;if(c->line_speed>v)c->line_speed=v;break;
    case AFR2_A_MOVE: {
        uint8_t permission=Afr2Product_PwmStatus(p);if(permission)return permission;
        uint8_t granted=local_command(p);if(granted)return granted;
        int16_t s=a->local_power_percent,vx=0,vy=0,wz=0;
        switch(v){case 0:vx=s;break;case 1:vx=-s;break;case 2:vy=s;break;case 3:vy=-s;break;
        case 4:vx=s;vy=s;break;case 5:vx=s;vy=-s;break;case 6:vx=-s;vy=s;break;case 7:vx=-s;vy=-s;break;
        case 8:wz=s;break;case 9:wz=-s;break;default:return AFR2_BAD;}
        return Afr2Product_DrivePwm(p,vx,vy,wz,500);
    }
    case AFR2_A_LINE_SPEED:c->line_speed=v;break;case AFR2_A_LINE_KP:c->line_kp=v;break;
    case AFR2_A_LINE_START:{uint8_t r=local_command(p);if(r)return r;
        r=Afr2Product_StartLine(p);if(!r)p->local_timeout_ms=30000;return r;}
    case AFR2_A_GOTO_X:if(!p->stopped)return AFR2_BUSY;p->target_x=v;return AFR2_OK;
    case AFR2_A_GOTO_Y:if(!p->stopped)return AFR2_BUSY;p->target_y=v;return AFR2_OK;
    case AFR2_A_GOTO_START:{uint8_t r=local_command(p);if(r)return r;
        r=Afr2Product_StartGoto(p,p->target_x,p->target_y);if(!r)p->local_timeout_ms=60000;return r;}
    case AFR2_A_WHEEL:
        if(v<0 || v>3 || p->hall_measuring || !p->stopped)return AFR2_BAD;
        if(p->selected_wheel!=v){p->hall_delta=0;p->hall_bad=0;}p->selected_wheel=v;return AFR2_OK;
    case AFR2_A_TEST_PWM:if(v<-255 || v>255 || !p->stopped)return AFR2_BAD;a->test_pwm=v;return AFR2_OK;
    case AFR2_A_TEST_MS:c->test_ms=v;break;
    case AFR2_A_WHEEL_START:{uint8_t r=local_command(p);if(r)return r;return Afr2Product_Wheel(p,p->selected_wheel,a->test_pwm,c->test_ms);}
    case AFR2_A_HALL_BEGIN:
        if(Afr2Head_State()->running || p->prefs_state==1 || a->hall_saving)return AFR2_BUSY;
        Afr2Product_HallBegin(p);if(!p->hall_measuring)return AFR2_UNAVAILABLE;
        /* Invalidate only this wheel, durably. Other wheels retain CPR/sign. */
        c->counts_per_rev[p->selected_wheel]=0;
        if(!Afr2Prefs_Save(c)){*c=old;p->hall_measuring=0;return AFR2_FAILED;}
        p->prefs_state=1;p->pose_valid=p->odom_ready=0;
        a->hall_draft_mask&=(uint8_t)~(1u<<p->selected_wheel);
        a->hall_draft_negative&=(uint8_t)~(1u<<p->selected_wheel);return AFR2_OK;
    case AFR2_A_HALL_END: {
        uint8_t r=Afr2Product_HallEnd(p);if(r)return r;
        uint8_t bit=(uint8_t)(1u<<p->selected_wheel);
        a->hall_draft[p->selected_wheel]=(uint32_t)(p->hall_delta<0?-p->hall_delta:p->hall_delta);
        a->hall_draft_mask|=bit;
        if(p->hall_delta<0)a->hall_draft_negative|=bit;else a->hall_draft_negative&=(uint8_t)~bit;
        return AFR2_OK;
    }
    case AFR2_A_HALL_SAVE:
        if(p->prefs_state==1 || a->hall_saving || !p->stopped)return AFR2_BUSY;
        if(p->hall_measuring || !a->hall_draft_mask)return AFR2_BAD;
        for(uint8_t i=0;i<4;i++)if(a->hall_draft_mask&(1u<<i)) {
            c->counts_per_rev[i]=a->hall_draft[i];c->hall_cw_mask|=(1u<<i);
            if(a->hall_draft_negative&(1u<<i))c->hall_signs^=(1u<<i);
        }
        if(!Afr2Prefs_Save(c)){*c=old;return AFR2_FAILED;}
        a->hall_saving=a->hall_draft_mask;p->prefs_state=1;
        p->pose_valid=0;p->odom_ready=0;p->notice=0;return AFR2_OK;
    case AFR2_A_HALL_CANCEL:
        if(a->hall_saving)return AFR2_BUSY;
        if(!a->remote_ui && p->origin==AFR2_SOURCE_LOCAL && !p->stopped)Afr2Product_Stop(p);
        /* v=1 cancels only the current attempt; completed wheel drafts survive. */
        if(v==2){a->hall_draft_mask&=(uint8_t)~(1u<<p->selected_wheel);a->hall_draft_negative&=(uint8_t)~(1u<<p->selected_wheel);}
        else if(v!=1){a->hall_draft_mask=0;a->hall_draft_negative=0;}
        p->hall_measuring=0;p->hall_delta=0;p->hall_bad=0;return AFR2_OK;
    case AFR2_A_SERVO_US:if(v<1000 || v>2000)return AFR2_BAD;a->servo_us=v;return AFR2_OK;
    case AFR2_A_SERVO_APPLY:if(h->running || !p->stopped)return AFR2_BUSY;Afr2Servo_Set(a->servo_us);return AFR2_OK;
    case AFR2_A_SERVO_OFF:Afr2Head_Stop();return AFR2_OK;
    case AFR2_A_SAVE:if(!p->stopped || p->prefs_state==1)return AFR2_BUSY;Afr2Product_Save(p);return p->prefs_state==1?AFR2_OK:AFR2_FAILED;
    case AFR2_A_DISCARD:if(!p->stopped || p->prefs_state==1 || p->hall_measuring)return AFR2_BUSY;
        if(c->servo_inverted!=a->saved_config.servo_inverted){uint8_t rc=Afr2Head_Configure(h->min_angle,h->max_angle,h->speed,h->settle_ms,h->interval_ms,a->saved_config.servo_inverted);if(rc)return rc;}
        *c=a->saved_config;p->odom_ready=0;p->pose_valid=0;return AFR2_OK;
    case AFR2_A_LINE_KIND:if(v<0 || v>1)return AFR2_BAD;c->line_kind=v;c->line_calibrated=0;a->line_samples=0;break;
    case AFR2_A_LINE_INVERT:c->line_invert=v;c->line_calibrated=0;a->line_samples=0;break;
    case AFR2_A_LINE_CHANNEL:case AFR2_A_LINE_THRESHOLD: {
        if(!c->line_kind || !Afr2Head_State()->ir_valid)return AFR2_UNAVAILABLE;
        uint8_t bits=0;for(uint8_t i=0;i<5;i++)if(p->sensors.line_raw[i])bits|=(1u<<i);
        if(action==AFR2_A_LINE_CHANNEL){a->line_background=bits;a->line_samples=1;}
        else {if(!(a->line_samples&1u))return AFR2_UNAVAILABLE;a->line_foreground=bits;a->line_samples|=2;}
        c->line_calibrated=0;return AFR2_OK;
    }
    case AFR2_A_LINE_CONFIRM:
        /* AFR2-20261107-43: antes exigia que los 5 canales cambiaran entre fondo y linea, imposible
         * con una linea angosta bajo 1-2 sensores. Basta la muestra de fondo (blanco); si hay muestra
         * de linea, al menos un canal debe cambiar. Un canal trabado sobre blanco queda inactivo. */
        if(!c->line_kind || !(a->line_samples&1u) || ((a->line_samples&2u) && !(a->line_background^a->line_foreground)))return AFR2_UNAVAILABLE;
        c->line_invert=a->line_background;c->line_calibrated=1;break;
    case AFR2_A_MOTOR_SIGNS:c->motor_signs=v;break;case AFR2_A_PWM_LIMIT:c->pwm_limit=v;break;
    case AFR2_A_RAMP:c->ramp_step=v;break;case AFR2_A_KP:c->kp=v;break;case AFR2_A_KI:c->ki=v;break;case AFR2_A_KD:c->kd=v;break;
    case AFR2_A_MOTOR_READY:if(!v)Afr2Product_HaltMotion(p);c->motor_ready=v;break;case AFR2_A_WHEEL_MM:return AFR2_DENIED;
    case AFR2_A_RADIUS_MM:return AFR2_DENIED;
    case AFR2_A_HALL_WIRED:case AFR2_A_GEOM_READY:return AFR2_DENIED;
    case AFR2_A_ENC_STEPS:c->encoder_steps=v;break;case AFR2_A_ENC_INVERT:c->encoder_invert=v;break;
    case AFR2_A_WIFI_READ_CREDENTIALS:
        p->wifi.credentials_visible=0;p->wifi.credential_length=0;wipe(p->wifi.password,sizeof(p->wifi.password));return wifi_request(p,AFR2_WIFI_READ_CREDENTIALS);
    case AFR2_A_WIFI_STATUS:return wifi_request(p,AFR2_WIFI_STATUS);
    case AFR2_A_WIFI_FALLBACK:
        if(!p->wifi.policy_valid || p->wifi.pending)return AFR2_UNAVAILABLE;
        p->wifi.policy_requested=!p->wifi.ask_fallback;return wifi_request(p,AFR2_WIFI_ASK_FALLBACK);
    case AFR2_A_WIFI_AP_TOGGLE:return wifi_request(p,p->wifi.ap_active?AFR2_WIFI_AP_STOP:AFR2_WIFI_AP_START);
    case AFR2_A_WIFI_SCAN:if(!(p->wifi.mode&1u))return AFR2_UNAVAILABLE;if(!p->wifi.mode_draft){p->wifi.requested_mode=p->wifi.mode?p->wifi.mode:1;p->wifi.mode_draft=1;}p->wifi.scan_page=0;p->wifi.scan_total=0;p->wifi.scan_received=0;p->wifi.scan_id=0;return wifi_request(p,AFR2_WIFI_SCAN);
    case AFR2_A_WIFI_PAGE:
        if(v<0 || v>1 || !p->wifi.scan_id || p->wifi.pending)return AFR2_UNAVAILABLE;
        p->wifi.scan_page=v;p->wifi.network_count=0;p->wifi.scan_received=0;return wifi_request(p,AFR2_WIFI_PAGE);
    case AFR2_A_WIFI_WEB:return wifi_request(p,AFR2_WIFI_WEB_DRAFT);
    case AFR2_A_WIFI_RETRY:if(!p->stopped || p->wifi.sta_connected)return AFR2_BUSY;return wifi_request(p,AFR2_WIFI_RETRY);
    /* AFR2-20261108-14: choosing a mode (or Desconectar) on the OLED is the user's decision: stop motion and
     * replace any pending Wi-Fi job instead of refusing with "Detener / ver red". The ESP's late update
     * for the replaced job carries the old id and is refused (UNAVAILABLE); it then polls the new job. */
    case AFR2_A_WIFI_DISCONNECT:
        if(!p->stopped)Afr2Product_Stop(p);
        p->wifi.pending=0;return wifi_request(p,AFR2_WIFI_DISCONNECT);
    case AFR2_A_WIFI_CHANGE_MODE:case AFR2_A_WIFI_SET_MODE:
        if(v<1 || v>3)return AFR2_BAD;
        if(!p->stopped)Afr2Product_Stop(p);
        p->wifi.pending=0;
        p->wifi.requested_mode=v;p->wifi.mode_draft=1;p->wifi.draft_ready=0;
        {uint8_t r=wifi_request(p,AFR2_WIFI_CHANGE_MODE);
         if(!r)Afr2Ui_NotifyFlash(a->ui,0,PSTR("Cambiando Wi-Fi"),v==1?PSTR("a STA, reinicia"):v==2?PSTR("a AP, reinicia"):PSTR("a AP+STA reinicia"),2500); /* AFR2-20261109-01 */
         return r;}
    case AFR2_A_WIFI_NETWORK:
        if(v<0 || v>=p->wifi.network_count)return AFR2_UNAVAILABLE;
        p->wifi.requested_mode=p->wifi.requested_mode==3?3:1;p->wifi.mode_draft=1;
        p->wifi.selected_network=v;strcpy(p->wifi.draft_ssid,p->wifi.networks[v]);p->wifi.draft_ready=0;return AFR2_OK;
    case AFR2_A_KEYBOARD: {
        if(!data)return AFR2_BAD;
        const Afr2UiCredentials *cred=data;
        uint8_t r=wifi_request(p,AFR2_WIFI_LOCAL_DRAFT);if(r)return r;
        strcpy(p->wifi.draft_ssid,cred->ssid);strcpy(p->wifi.password,cred->password);
        p->wifi.credential_length=(uint8_t)strlen(cred->ssid)+1u+(uint8_t)strlen(cred->password);
        p->wifi.draft_ready=0;return AFR2_OK;
    }
    case AFR2_A_WIFI_CONNECT:
        if(!p->stopped)return AFR2_BUSY;
        if(!p->wifi.draft_ready || (uint32_t)(p->now-p->wifi.draft_ms)>120000u)return AFR2_UNAVAILABLE;
        return wifi_request(p,AFR2_WIFI_CONNECT);
    case AFR2_A_WIFI_CANCEL:
        p->wifi.mode_draft=0;wipe(p->wifi.password,sizeof(p->wifi.password));p->wifi.credential_length=0;p->wifi.draft_ready=0;
        /* Keep only a cancellation request, superseding a waiting modal/draft. */
        p->wifi.pending=0;return wifi_request(p,AFR2_WIFI_CANCEL);
    case AFR2_A_CONTRAST:c->contrast=v;break;case AFR2_A_ROTATION:c->rotation=v;break;
    case AFR2_A_CLEAR_FAULT:if(!p->stopped)return AFR2_DENIED;p->fault=0;memset(p->hp_score,0,sizeof(p->hp_score));p->mode=AFR2_IDLE;return AFR2_OK;
    case AFR2_A_REBOOT_MEGA:
        if(!p->stopped || p->prefs_state==1 || a->reboot_pending)return AFR2_BUSY;
        Afr2Product_Stop(p);a->reboot_pending=1;a->reboot_ms=p->now;return AFR2_OK;
    case AFR2_A_REBOOT_ESP:
        if(!p->stopped || p->prefs_state==1 || a->reboot_pending)return AFR2_BUSY;
        return wifi_request(p,AFR2_WIFI_REBOOT_ESP);
    default:return AFR2_UNAVAILABLE;
    }
    if(!Afr2Product_ConfigValid(c)){*c=old;return AFR2_BAD;}
    if(action==AFR2_A_WHEEL_MM || action==AFR2_A_RADIUS_MM || action==AFR2_A_HALL_WIRED || action==AFR2_A_GEOM_READY) {
        p->odom_ready=0;p->pose_valid=0;
    }
    return AFR2_OK;
}
static void keyboard_seed(void *ctx,Afr2UiCredentials *out) {
    Afr2App *a=ctx;strncpy(out->ssid,a->product->wifi.draft_ssid,32);out->ssid[32]=0;
}
static uint8_t available(void *ctx,uint8_t action,int32_t arg) {
    Afr2App *a=ctx;Afr2Product *p=a->product;
    if(action==AFR2_A_MOVE){uint8_t permission=Afr2Product_PwmStatus(p);if(permission)return permission;}
    if(action==AFR2_A_WIFI_SCAN && !(p->wifi.mode&1u))return AFR2_UNAVAILABLE;
    if(action==AFR2_A_WIFI_FALLBACK && (!p->wifi.policy_valid || p->wifi.pending))return AFR2_UNAVAILABLE;
    if(action==AFR2_A_WIFI_CONNECT && p->wifi.sta_connected)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_WIFI_RETRY && p->wifi.mode==2)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_WIFI_RETRY && p->wifi.sta_connected)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_ESP_TEST && (a->diag_state==1 || !p->extension_ready))return AFR2_UNAVAILABLE;
    if(action==AFR2_A_DISCARD && !config_dirty(&p->config,&a->saved_config))return AFR2_UNAVAILABLE;
    if(action==AFR2_A_REBOOT_MEGA || action==AFR2_A_REBOOT_ESP) {
        if(!p->stopped || p->prefs_state==1 || a->reboot_pending)return AFR2_BUSY;
        if(action==AFR2_A_REBOOT_ESP && (!p->extension_ready || !p->wifi.available))return AFR2_UNAVAILABLE;
        if(action==AFR2_A_REBOOT_ESP && p->wifi.pending)return AFR2_BUSY;
    }
    if(action==AFR2_A_MOVE || action==AFR2_A_WHEEL_START || action==AFR2_A_LINE_START || action==AFR2_A_GOTO_START) {
        if(!p->config.motor_ready)return AFR2_UNAVAILABLE;
        if(action!=AFR2_A_WHEEL_START && action!=AFR2_A_MOVE && !p->pose_valid)return AFR2_UNAVAILABLE;
        if(action==AFR2_A_LINE_START && !p->sensors.line_valid_raw)return AFR2_UNAVAILABLE;
    }
    if((action==AFR2_A_WHEEL_TOGGLE && p->mode!=AFR2_WHEEL_TEST) || ((action==AFR2_A_GOTO_TOGGLE || action==AFR2_A_ODOM_TOGGLE) && p->mode!=AFR2_GOTO) || (action==AFR2_A_LINE_TOGGLE && p->mode!=AFR2_LINE)){
        if(!p->config.motor_ready || p->fault)return AFR2_UNAVAILABLE;
        if((action==AFR2_A_GOTO_TOGGLE || action==AFR2_A_ODOM_TOGGLE) && !p->pose_valid)return AFR2_UNAVAILABLE;
        if(action==AFR2_A_LINE_TOGGLE && !p->sensors.line_valid_raw)return AFR2_UNAVAILABLE;
    }
    if(action==AFR2_A_HALL_BEGIN && !p->sensors.hall_available)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_HALL_END && !p->hall_measuring)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_HALL_SAVE && (p->hall_measuring || !a->hall_draft_mask || a->hall_saving))return AFR2_UNAVAILABLE;
    if((action>=AFR2_A_WIFI_STATUS && action<=AFR2_A_WIFI_SET_MODE) &&
        action!=AFR2_A_WIFI_MODE && action!=AFR2_A_WIFI_SET_MODE && action!=AFR2_A_WIFI_CANCEL && action!=AFR2_A_WIFI_STATUS &&
        (!p->extension_ready || !p->wifi.available))return AFR2_UNAVAILABLE;
    if(action==AFR2_A_WIFI_NETWORK && arg>=p->wifi.network_count)return AFR2_UNAVAILABLE;
    if(action==AFR2_A_WIFI_CONNECT && !p->wifi.draft_ready)return AFR2_UNAVAILABLE;
    return AFR2_OK;
}
static const char *reason(void *ctx,uint8_t action) {
    Afr2App *a=ctx;Afr2Product *p=a->product;
    if(action==AFR2_A_WIFI_FALLBACK)return !p->wifi.policy_valid?PSTR("ESP sin politica"):PSTR("Wi-Fi ocupado");
    if(action==AFR2_A_WIFI_SCAN && !(p->wifi.mode&1u))return PSTR("Cambiar modo STA");
    if(action==AFR2_A_WIFI_RETRY && p->wifi.mode==2)return PSTR("Cambiar modo STA");
    if(action==AFR2_A_WIFI_ASK)return !p->extension_ready?PSTR("ESP sin extension"):!p->wifi.available?PSTR("ESP sin estado"):p->wifi.pending?PSTR("Wi-Fi ocupado"):PSTR("Ver estado Wi-Fi"); /* AFR2-20261108-06 */
    if(action==AFR2_A_WIFI_DISCONNECT || action==AFR2_A_WIFI_CHANGE_MODE || action==AFR2_A_WIFI_SET_MODE)return PSTR("Detener / ver red");
    if(action==AFR2_A_ESP_TEST)return PSTR("Enlace / ocupado");
    if(action==AFR2_A_QUICK_MODE)return PSTR("Detener / ver fallo");
    if(action==AFR2_A_SERVO_INVERT)return PSTR("Desactivar servo");
    if(action==AFR2_A_REBOOT_MEGA || action==AFR2_A_REBOOT_ESP)return PSTR("Detener / ver ESP");
    if(p->prefs_state==1)return PSTR("Guardando EEPROM");
    if(p->fault)return PSTR("Fallo: ver estado");
    if(action==AFR2_A_MOVE || action==AFR2_A_WHEEL_START || action==AFR2_A_LINE_START || action==AFR2_A_LINE_TOGGLE || action==AFR2_A_GOTO_START){
        if(!p->config.motor_ready)return PSTR("Habilitar motores");
        if(Afr2Head_State()->running)return PSTR("Detener radar");
        if(action==AFR2_A_MOVE || action==AFR2_A_WHEEL_START)return PSTR("Ver PWM / estado");
    }
    if(action==AFR2_A_RADAR_TOGGLE || action==AFR2_A_SERVO_TOGGLE || action==AFR2_A_SERVO_ANGLE || action==AFR2_A_SERVO_EDIT_BEGIN)return PSTR("Detener / ver Hall");
    if(action==AFR2_A_RADAR_TRACE || action==AFR2_A_RADAR_RANGE)return PSTR("Detener motores");
    if(action>=AFR2_A_WIFI_STATUS && action<=AFR2_A_WIFI_SET_MODE) {
        if(!p->extension_ready)return PSTR("ESP sin extension");
        if(!p->wifi.available)return PSTR("ESP sin estado");
        if(action==AFR2_A_WIFI_CONNECT && !p->wifi.draft_ready)return PSTR("Falta borrador");
        return PSTR("Ver estado Wi-Fi");
    }
    if(action==AFR2_A_HALL_BEGIN || action==AFR2_A_HALL_END || action==AFR2_A_HALL_SAVE ||
       action==AFR2_A_MOVE || action==AFR2_A_LINE_START || action==AFR2_A_LINE_TOGGLE || action==AFR2_A_GOTO_START) {
        if(!p->sensors.hall_available)return PSTR("Habilitar Hall");
        if(action==AFR2_A_HALL_END && !p->hall_measuring)return PSTR("Iniciar medicion");
        if(action==AFR2_A_HALL_SAVE && !a->hall_draft_mask)return PSTR("Completar vuelta");
        if(action==AFR2_A_HALL_SAVE && a->hall_saving)return PSTR("Guardando EEPROM");
        for(uint8_t i=0;i<4;i++)if(!p->config.counts_per_rev[i])return PSTR("Calibrar vuelta");
        if(!p->config.geometry_ready)return PSTR("Config. geometria");
        if(!p->config.motor_ready)return PSTR("Habilitar motores");
        if((action==AFR2_A_LINE_START || action==AFR2_A_LINE_TOGGLE) && !p->sensors.line_valid_raw)return PSTR("Calibrar linea");
    }
    return PSTR("Ver requisitos");
}
void Afr2App_Init(Afr2App *a,Afr2Product *p,Afr2Ui *u,Afr2Oled *o,Afr2Link *l) {
    memset(a,0,sizeof(*a));a->product=p;a->ui=u;a->oled=o;a->link=l;a->saved_config=p->config;
    a->diag_timeout=2000;a->local_power_percent=100;
    const Afr2SensorHead *head=Afr2Head_State();
    (void)Afr2Head_Configure(head->min_angle,head->max_angle,head->speed,head->settle_ms,head->interval_ms,p->config.servo_inverted);
    (void)Afr2Head_Servo(2,90);   /* AFR2-20261109-01: servo always starts centred, a known reference */
    a->test_pwm=40;a->servo_us=1500;a->odom_dir=0;a->odom_cm=50; /* AFR2-20261108-02 */
    p->wifi.requested_mode=1;
    Afr2UiOps ops={a,invoke,value,keyboard_seed,available,reason};Afr2Ui_Attach(u,&ops);
}
void Afr2App_Revoke(void *ctx) {
    Afr2App *a=ctx;Afr2Product *p=a->product;
    Afr2Ui_Touch(a->ui,0,8,0);
    if(a->odom_state==1){a->odom_state=3;if(a->ui->node==AFR2_N_ODOM_ASK)Afr2Ui_Leave(a->ui,AFR2_N_CALIB);} /* AFR2-20261108-04 */
    /* AFR2-20261108-06: sin enlace con el ESP la pregunta ya no tiene a quien responder. */
    if(a->wifi_ask){a->wifi_ask=0;if(a->ui->node==AFR2_N_WIFI_ASK)Afr2Ui_Leave(a->ui,AFR2_N_WIFI);}
    Afr2Head_CancelResume();Afr2Product_HaltMotion(p);p->extension_ready=0;p->session=0;p->wifi.available=0;
    p->wifi.pending=0;p->wifi.draft_ready=0;p->wifi.credential_length=0;p->wifi.network_count=0;p->wifi.details_valid=0;a->ui->notice_target=0;
    p->wifi.esp_version[0]=p->wifi.web_version[0]=0;
    wipe(p->wifi.password,sizeof(p->wifi.password));a->snapshot_active=0;
    wipe(p->wifi.ap_password,sizeof(p->wifi.ap_password));p->wifi.ap_ssid[0]=0;
    p->wifi.display_ready=0;p->wifi.ap_active=0;p->wifi.web_ready=0;a->ui->redraw=true;
    if(a->ui->keyboard){wipe(&a->ui->credentials,sizeof(a->ui->credentials));a->ui->keyboard=0;a->ui->redraw=true;}
}
void Afr2App_Task(Afr2App *a,uint32_t now) {
    Afr2Product *p=a->product;if(a->reboot_pending){p->now=now;Afr2Product_Stop(p);return;}
    /* AFR2-20261108-10: the distance screen measures while it is open, even with the quick toggle off. */
    /* AFR2-20261109-01: radar and servo-angle screens plot live echoes too (they stopped updating in 1.5.0). */
    Afr2Head_SetView(a->ui->node==AFR2_N_US_VIEW || a->ui->node==AFR2_N_RADAR_VIEW || a->ui->node==AFR2_N_SERVO_ANGLE);
    /* AFR2-20261108-13: line screens and calibration read the IR while open. */
    Afr2Head_SetIrView(a->ui->node==AFR2_N_LINE_VIEW || a->ui->node==AFR2_N_IR_VALUES || a->ui->node==AFR2_N_LINE_CALIB);
    if(a->diag_state==1 && (!p->extension_ready || (uint32_t)(now-a->diag_started)>=a->diag_timeout)){
        a->diag_state=p->extension_ready?4:5;Afr2Ui_NotifyFlash(a->ui,1,PSTR("Prueba ESP"),p->extension_ready?PSTR("Tiempo agotado"):PSTR("Enlace perdido"),2500);
    }
    uint8_t before=p->prefs_state;Afr2Product_Task(p,now);
    if(before==1 && p->prefs_state==2){
        a->saved_config=p->config;p->prefs_loaded=1;
        if(p->hall_measuring)p->notice=0; /* Invalidation commit must not interrupt pulse counting. */
        if(a->hall_saving){
            a->hall_draft_mask&=(uint8_t)~a->hall_saving;
            a->hall_draft_negative&=(uint8_t)~a->hall_saving;
            a->hall_saving=0;p->hall_delta=0;p->notice=0;
            if(!a->ui->confirm_wait)Afr2Ui_NotifyFlash(a->ui,0,PSTR("Mediciones"),PSTR("Guardadas"),2500);
        }
        if(a->ui->confirm_wait){p->notice=0;Afr2Ui_SaveCompleted(a->ui);}
    }
    if(before==1 && p->prefs_state==3){a->hall_saving=0;a->ui->confirm_wait=0;Afr2Ui_NotifyFlash(a->ui,1,PSTR("EEPROM"),PSTR("No guardado"),3000);}
    if(a->snapshot_active && (uint32_t)(now-a->snapshot_ms)>600u){
        AFR2_DBG("CAP expired after %lu ms (ESP did not finish the 26 pages)",(unsigned long)(now-a->snapshot_ms));
        a->snapshot_active=0;
    }
    if(p->wifi.available && (uint32_t)(now-p->wifi.status_ms)>5000u) {
        p->wifi.available=0;p->wifi.display_ready=0;wipe(p->wifi.ap_password,sizeof(p->wifi.ap_password));
        a->snapshot_active=0;a->ui->redraw=true;
    }
    if(p->wifi.display_ready && (uint32_t)(now-p->wifi.display_ms)>5000u) {
        p->wifi.display_ready=0;wipe(p->wifi.ap_password,sizeof(p->wifi.ap_password));
        a->snapshot_active=0;a->ui->redraw=true;
    }
    if(p->wifi.pending && (uint32_t)(now-p->wifi.job_ms)>(p->wifi.op==AFR2_WIFI_WEB_DRAFT?120000u:30000u)) {
        p->wifi.pending=0;p->wifi.state=4;p->wifi.draft_ready=0;wipe(p->wifi.password,sizeof(p->wifi.password));p->wifi.credential_length=0;
        Afr2Ui_NotifyFlash(a->ui,1,PSTR("Wi-Fi"),PSTR("Tiempo agotado"),3000);
    }
    if(p->wifi.credentials_visible && (a->ui->node!=AFR2_N_WIFI_DATA || (uint32_t)(now-p->wifi.draft_ms)>30000u)){p->wifi.credentials_visible=0;wipe(p->wifi.password,sizeof(p->wifi.password));a->ui->redraw=true;}
    if(p->wifi.draft_ready && (uint32_t)(now-p->wifi.draft_ms)>120000u){p->wifi.draft_ready=0;p->wifi.draft_id=0;}
    if(p->sequence.state==3 && p->sequence.execution!=a->sequence_screen_execution) {
        a->sequence_screen_execution=p->sequence.execution;
        a->ui->notices=a->ui->head=a->ui->tail=0;
        Afr2Ui_Open(a->ui,AFR2_N_SEQUENCE);
    }
    if(a->ui->node==AFR2_N_SEQUENCE && p->notice==1)p->notice=0;
    /* AFR2-20261108-04: propuesta de la Web sin respuesta: vence a los 60 s; si se abandono la pantalla cuenta como descartada. */
    if(a->odom_state==1){
        if(a->ui->node!=AFR2_N_ODOM_ASK)a->odom_state=3;
        else if((uint32_t)(now-a->odom_ask_ms)>60000u){
            a->odom_state=5;Afr2Ui_Leave(a->ui,AFR2_N_CALIB);Afr2Ui_NotifyFlash(a->ui,1,PSTR("Odometria"),PSTR("Prueba vencida"),3000);
        }
    }
    /* AFR2-20261108-06: selector sin Wi-Fi. Se abre despues del aviso 0x04000c, solo cuando la UI no esta en un
     * dialogo obligatorio, edicion, teclado, prueba en marcha o tactil. Se cierra solo si la STA conecta o hay AP. */
    if(a->wifi_ask){
        Afr2Ui *u=a->ui;uint8_t node=u->node;
        if(p->wifi.sta_connected || p->wifi.ap_active || p->wifi.mode==2 || (p->wifi.policy_valid && !p->wifi.ask_fallback)){a->wifi_ask=0;if(node==AFR2_N_WIFI_ASK)Afr2Ui_Leave(u,AFR2_N_WIFI);}
        else if(a->wifi_ask==1){
            if(!u->notices && !u->touch_active && !u->keyboard && !u->edit && node!=AFR2_N_SAVE_CONFIRM && node!=AFR2_N_HALL_PICK &&
               node!=AFR2_N_HALL_COUNT && node!=AFR2_N_ENCODER_TEST && node!=AFR2_N_ODOM_RUN && node!=AFR2_N_ODOM_ASK && node!=AFR2_N_SEQUENCE){
                if(node!=AFR2_N_WIFI_ASK)Afr2Ui_Open(u,AFR2_N_WIFI_ASK);
                a->wifi_ask=2;
            }
        }
        else if(node!=AFR2_N_WIFI_ASK){
            /* Respondido (corto, largo o 5 s). Si otra pantalla la desplazo sin respuesta, se sigue esperando
             * para que el ESP no quede sin decision. */
            if(!u->wifi_ask_answered)(void)wifi_request(p,AFR2_WIFI_WAIT);
            a->wifi_ask=0;
        }
    }
    /* AFR2-20261108-03: la prueba de distancia termina sola: sale de la vista y avisa el resultado. */
    if(a->ui->node==AFR2_N_ODOM_RUN && !a->ui->notices && (p->stopped || p->mode!=AFR2_GOTO)){
        char text[18];uint8_t state=p->goto_state;
        Afr2Ui_Leave(a->ui,AFR2_N_ODOM_MENU);
        if(state==2){
            snprintf_P(text,sizeof(text),PSTR("X%ld Y%ld"),(long)p->x_mm,(long)p->y_mm);
            Afr2Ui_NotifyFlashTitle(a->ui,0,PSTR("Llego al objetivo"),text,4000);
        }else if(p->fault){
            /* AFR2-20261108-05: la causa real (antes decia solo "Interrumpido"). 1 lazo lento, 2 odometria/geometria, 3 Hall sin pulsos, 4 signo PWM/Hall. */
            snprintf_P(text,sizeof(text),p->fault==4?PSTR("F4 signo PWM/Hall"):p->fault==3?PSTR("F3 sin pulsos"):p->fault==2?PSTR("F2 odometria"):PSTR("F%u lazo/otro"),(unsigned)p->fault);
            Afr2Ui_NotifyFlashTitle(a->ui,1,PSTR("Fallo de marcha"),text,5000);
            AFR2_DBG("ODOM interrupted fault=%u hp=%d,%d,%d,%d",(unsigned)p->fault,p->hp_score[0],p->hp_score[1],p->hp_score[2],p->hp_score[3]);
        }else Afr2Ui_NotifyFlash(a->ui,1,PSTR("Odometria"),state==4?PSTR("No llego (60 s)"):PSTR("Interrumpido"),3000);
        p->notice=0; /* reemplaza el aviso generico de GOTO */
    }
    if(p->notice){Afr2Ui_NotifyFlash(a->ui,p->notice==2?1:0,PSTR("Sistema"),p->notice==1?PSTR("Operacion lista"):p->notice==3?PSTR("Guardando EEPROM"):PSTR("Ver requisitos"),3000);p->notice=0;}
    if(a->ui->touch_active && (uint32_t)(now-a->ui->touch_ms)>2500u)Afr2Ui_Touch(a->ui,0,8,0);
    /* AFR2-20261106-07: Prueba Hall zero taken on each entry, outside the renderer. */
    if(a->ui->node==AFR2_N_HALL_LIVE){
        if(!a->hall_live_active){memcpy(a->hall_live_base,p->sensors.counts,sizeof(a->hall_live_base));a->hall_live_active=1;}
    }else a->hall_live_active=0;
    Afr2Ui_Tick(a->ui,now);
    /* Text scroll updates only its clipped band. Do not schedule a full OLED
     * redraw every40ms; notices/network views have their own100ms redraw. */
    if(!a->ui->touch_active && !a->ui->notices && a->ui->node!=AFR2_N_NETWORK && a->ui->node!=AFR2_N_AP_INFO && a->ui->node!=AFR2_N_QUICK && (uint32_t)(now-a->render_ms)>=(a->ui->node==AFR2_N_RADAR_VIEW || a->ui->node==AFR2_N_US_VIEW || a->ui->node==AFR2_N_LINE_VIEW || a->ui->node==AFR2_N_IR_VALUES || a->ui->node==AFR2_N_SERVO_ANGLE || a->ui->node==AFR2_N_HALL_LIVE || a->ui->node==AFR2_N_ODOM_RUN?100u:250u)) {
        Afr2MenuNode node;Afr2Menu_Node(a->ui->node,&node);
        if(node.view || a->ui->edit || a->ui->keyboard || (uint32_t)(now-a->render_ms)>=1000u) {
            a->render_ms=now;a->ui->redraw=true;
        }
    }
}
#define LINE(row,fmt,...) snprintf_P(s->lines[row],sizeof(s->lines[row]),PSTR(fmt),##__VA_ARGS__)
void Afr2App_UiSnapshot(Afr2App *a,Afr2UiSnapshot *s) {
    Afr2Product *p=a->product;Afr2Ui *u=a->ui;memset(s,0,sizeof(*s));
    s->sensor_now_ms=p->now;s->head=Afr2Head_State();s->ir_kind=p->config.line_kind;s->ir_valid=s->head->ir_valid;memcpy(s->ir_raw,p->sensors.line_raw,sizeof(s->ir_raw));
    s->radar_range=p->config.radar_range;s->radar_trace=p->config.radar_trace;
    s->dashboard_valid=1;s->dashboard_mode=p->mode;s->dashboard_moving=!p->stopped;
    s->dashboard_fault=p->fault || p->mode==AFR2_FAULT;
    /* AFR2-20261108-12: bit = pagina de Controles rapidos: 1 motores, 2 ultrasonido, 3 radar, 4 servo, 5 linea, 6 rastro. */
    s->quick_flags=(p->config.motor_ready?2u:0u)|(s->head->sampling?4u:0u)|(s->head->running?8u:0u)|(s->head->enabled?16u:0u)|(s->head->ir_enabled?32u:0u)|(p->config.radar_trace?64u:0u);
    s->line_valid=p->sensors.line_valid;s->line_bits=p->sensors.line_bits;
    s->ultrasonic_valid=p->sensors.ultrasonic_valid;s->ultrasonic_mm=p->sensors.distance_mm;
    s->oled_ready=a->oled->initialized;s->oled_errors=a->oled->errors;
    s->hall_pending=a->hall_draft_mask;
    for(uint8_t i=0;i<4;i++)s->hall_cpr[i]=(a->hall_draft_mask&(1u<<i))?a->hall_draft[i]:p->config.counts_per_rev[i];
    s->battery_valid=Afr2Battery_Status(&p->sensors,p->now)==AFR2_BATTERY_VALID;
    s->battery_mv=s->battery_valid?Afr2Battery_Millivolts(&p->sensors):0;
    s->network_ask_fallback=p->wifi.ask_fallback;s->network_policy_valid=p->wifi.policy_valid;
    s->network_mode=p->wifi.mode;s->network_busy=p->wifi.pending;s->network_page=p->wifi.scan_page;s->network_total=p->wifi.scan_total;s->network_truncated=p->wifi.scan_truncated;s->network_scan_state=(uint8_t)value(a,AFR2_A_WIFI_SCAN_STATE);
    s->network_valid=p->wifi.available && p->wifi.display_ready;
    s->network_boot=p->wifi.display_boot;s->network_revision=p->wifi.display_revision;
    s->qr_allowed=p->stopped && !s->head->running;
    s->network_rssi=p->wifi.rssi;s->network_rssi_valid=p->wifi.available && p->wifi.sta_connected && (uint32_t)(p->now-p->wifi.status_ms)<=5000u && p->wifi.rssi>-127 && p->wifi.rssi<0;
    s->network_sta=p->wifi.sta_connected;s->network_ap=p->wifi.ap_active;
    s->network_clients=p->wifi.ap_clients;s->network_server=p->wifi.web_ready;
    s->network_ssid=p->wifi.ssid;s->network_ap_ssid=p->wifi.ap_ssid;s->network_password=p->wifi.ap_password;
    s->network_ip=p->wifi.ip;s->network_ap_ip=p->wifi.ap_ip;
    s->sequence_state=p->sequence.state;s->sequence_count=p->sequence.count;
    s->sequence_index=p->sequence.index;
    if(p->sequence.state==4)s->sequence_duration_ms=p->sequence.total;
    #if AFR2_SEQUENCE_MEMORY_APPROVED
    if(p->sequence.mode) {
        s->sequence_kind=6;s->sequence_duration_ms=p->sequence.total;s->sequence_elapsed_ms=p->sequence.elapsed;
    } else if(p->sequence.index<p->sequence.count) {
        uint8_t i=p->sequence.index;s->sequence_kind=(p->sequence.kinds[i/2]>>((i&1u)*4u))&15u;
        s->sequence_duration_ms=p->sequence.segments[i].ms;
        uint32_t offset=0;for(uint8_t j=0;j<i;j++)offset+=p->sequence.segments[j].ms;
        s->sequence_elapsed_ms=p->sequence.elapsed>=offset?p->sequence.elapsed-offset:0;
        if(s->sequence_elapsed_ms>s->sequence_duration_ms)s->sequence_elapsed_ms=s->sequence_duration_ms;
    }
    #endif
    switch(u->node) {
    case AFR2_N_BATTERY_VIEW:{
        uint8_t state=Afr2Battery_Status(&p->sensors,p->now);
        uint16_t mv=Afr2Battery_Millivolts(&p->sensors);
        if(state==AFR2_BATTERY_VALID)LINE(0,"Bateria:%u.%02u V",mv/1000u,(mv%1000u)/10u);
        else LINE(0,"Bateria: -- V");
        if(state==AFR2_BATTERY_OFF){LINE(1,"Realizar medicion");LINE(2,"Desde Bateria");}
        else if(state==AFR2_BATTERY_PENDING){LINE(1,"Midiendo...");LINE(2,"AVCC nominal 5V");}
        else if(state==AFR2_BATTERY_RANGE){LINE(1,"Fuera de rango");LINE(2,"Revisar divisor");}
        else if(state==AFR2_BATTERY_STALE){LINE(1,"Lectura caducada");LINE(2,"Volver a medir");}
        else {LINE(1,"Edad:%lu ms",(unsigned long)(p->now-p->sensors.battery_ms));LINE(2,"Sin calibrar");}
        break;
    }
    case AFR2_N_SAVE_CONFIRM:
        if(a->ui->confirm_save==AFR2_A_HALL_SAVE){
            static const char names[][3] PROGMEM={"FL","FR","RL","RR"};char name[3];memcpy_P(name,names[p->selected_wheel],3);
            LINE(0,"Sensor Hall %s",name);LINE(1,"Pulsos:%lu",(unsigned long)a->hall_draft[p->selected_wheel]);
        }break;
    case AFR2_N_HALL_COUNT:
        LINE(0,"Rueda %u",p->selected_wheel+1u);
        LINE(1,"Pulsos:%ld",(long)p->hall_delta);break;
    case AFR2_N_HOME:
        switch(p->mode) {
        case AFR2_IDLE:LINE(0,"Reposo");break;
        case AFR2_MANUAL:LINE(0,"MANUAL");break;
        case AFR2_WHEEL_TEST:LINE(0,"PRUEBA DE RUEDA");break;
        case AFR2_FAULT:LINE(0,"FALLO");break;
        case AFR2_SEQUENCE:LINE(0,"SECUENCIA");break;
        case AFR2_LINE:LINE(0,"SEGUIR LINEA");break;
        case AFR2_GOTO:LINE(0,"GOTO");break;
        case AFR2_PWM_MANUAL:LINE(0,"PWM MANUAL");break;
        default:LINE(0,"Modo: %u",p->mode);break;
        }
        break;
    case AFR2_N_GEOMETRY:LINE(0,"Rueda:%u mm",p->config.wheel_mm);LINE(1,"Lx+Ly:%u mm",p->config.radius_sum_mm);LINE(2,"CAD nominal");break;
    case AFR2_N_ROBOT_VIEW:
        LINE(0,"Modo:%u Ctrl:%u",p->mode,p->origin);LINE(1,"Salidas OFF:%u",p->stopped);LINE(2,"Estado de salidas");break;
    case AFR2_N_LINK_VIEW:
        LINE(0,"UART:%u Ext:%u",a->link->negotiated,p->extension_ready);LINE(1,"Origen:%u Web:%u",p->origin,p->web_seen && (uint32_t)(p->now-p->web_ms)<1500u);
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
    case AFR2_N_ODOM_ASK:{
        char name[14];Afr2Ui_OdomDirName(name,a->odom_dir);
        LINE(0,"%s",name);LINE(1,"%u cm",a->odom_cm);break;
    }
    case AFR2_N_ODOM_RUN:{
        /* AFR2-20261108-03: pose estimada, objetivo absoluto de la mision y distancia restante. */
        float fx=(float)(p->mission_x-p->x_mm),fy=(float)(p->mission_y-p->y_mm);
        LINE(0,"Act X%ld Y%ld",(long)p->x_mm,(long)p->y_mm);
        LINE(1,"Obj X%ld Y%ld",(long)p->mission_x,(long)p->mission_y);
        LINE(2,"Falta %ld mm",(long)sqrtf(fx*fx+fy*fy));
        break;
    }
    case AFR2_N_HALL_LIVE:{
        /* AFR2-20261106-07: hand-turn test; counts relative to entry, + one way, - the other. */
        const int32_t *c=p->sensors.counts,*b=a->hall_live_base;
        LINE(0,"FL%6ld FR%6ld",(long)(c[0]-b[0]),(long)(c[1]-b[1]));
        LINE(1,"RL%6ld RR%6ld",(long)(c[2]-b[2]),(long)(c[3]-b[3]));
        if(p->sensors.hall_available)LINE(2,"Gire a mano");else LINE(2,"Hall deshabilitado");
        break;
    }
    case AFR2_N_HALL_VIEW:
        if(!p->sensors.hall_available){LINE(0,"Hall deshabilitado");LINE(1,"Revisar ajustes");LINE(2,"Captura bloqueada");}
        else {LINE(0,"Rueda:%u C:%ld",p->selected_wheel,(long)p->sensors.counts[p->selected_wheel]);
        LINE(1,"Vuelta:%ld E:%u",(long)p->hall_delta,p->hall_bad);LINE(2,"CPR:%lu",(unsigned long)p->config.counts_per_rev[p->selected_wheel]);}break;
    case AFR2_N_WHEEL_VIEW:
        LINE(0,"FL:%d FR:%d",p->pwm[0],p->pwm[1]);LINE(1,"RL:%d RR:%d",p->pwm[2],p->pwm[3]);{static const char marks[3] PROGMEM={'?','+','!'};char hp[5];for(uint8_t i=0;i<4;i++)hp[i]=pgm_read_byte(&marks[Afr2Product_HallPwmState(p,i)]);hp[4]=0;
        /* AFR2-20261107-39: HP = PWM/Hall por rueda FL FR RL RR: + coincide, ! cruzado, ? sin datos. */
        LINE(2,"Inv:%02X HP:%s",p->config.motor_signs,hp);}break;
    case AFR2_N_DISPLAY:
        LINE(0,"OLED listo:%u",s->oled_ready);LINE(1,"Errores:%lu",(unsigned long)s->oled_errors);LINE(2,"R%ums TX%ums",a->raster_ms,a->oled->last_transfer_ms);break;
    case AFR2_N_ENCODER_VIEW:
        LINE(0,"Giros:%ld",(long)u->encoder_turns);LINE(1,"Clics:%u",u->encoder_clicks);LINE(2,"Largos:%u",u->encoder_holds);break;
    case AFR2_N_LINE_REQ:
        LINE(0,"Linea valida:%u",p->sensors.line_valid);LINE(1,"Hall:%u Geom:%u",p->sensors.hall_available,p->config.geometry_ready);
        LINE(2,"Motor habilit:%u",p->config.motor_ready);break;
    case AFR2_N_GOTO_REVIEW:LINE(0,"X:%ld mm",(long)p->target_x);LINE(1,"Y:%ld mm",(long)p->target_y);LINE(2,"V max:%u mm/s",p->config.speed_limit);break;
    case AFR2_N_GOTO_RESULT:LINE(0,"Estado GOTO:%u",p->goto_state);LINE(1,"Pose valida:%u",p->pose_valid);LINE(2,"Salidas OFF:%u",p->stopped);break;
    case AFR2_N_POSE:
        if(!p->pose_valid){LINE(0,"Sin odometria");if(p->sensors.hall_available)LINE(1,"Hall habilitado");else LINE(1,"Habilitar Hall");LINE(2,"Calibrar CPR Hall");}
        else {LINE(0,"X:%ld Y:%ld",(long)p->x_mm,(long)p->y_mm);LINE(1,"Yaw:%ld mrad",(long)p->yaw_mrad);LINE(2,"Estimacion ruedas");}break;
    case AFR2_N_WIFI_VIEW:
        if(!p->wifi.available)LINE(0,"Sin datos actuales");else LINE(0,"%s",p->wifi.sta_connected?"Conectado":"Desconectado");break;
    case AFR2_N_NETWORK:
        if(!p->wifi.available){LINE(0,"Sin datos actuales");break;}
        if(!u->selection){
            LINE(0,"Modo: %s",p->wifi.mode==1?"STA":p->wifi.mode==2?"AP":p->wifi.mode==3?"AP-STA":"--");
            LINE(1,"IP %s:",p->wifi.sta_connected?"STA":"AP");LINE(2,"%.18s",p->wifi.sta_connected?p->wifi.ip:p->wifi.ap_active?p->wifi.ap_ip:"--");
        }else if(p->wifi.details_valid && (uint32_t)(p->now-p->wifi.details_ms)<=5000u){
            const uint8_t *m=p->wifi.mask,*g=p->wifi.gateway;
            LINE(0,"Mascara:");if(m[0]|m[1]|m[2]|m[3])LINE(1,"%u.%u.%u.%u",m[0],m[1],m[2],m[3]);else LINE(1,"--");
            if(g[0]|g[1]|g[2]|g[3])LINE(2,"GW:%u.%u.%u.%u",g[0],g[1],g[2],g[3]);else LINE(2,"Puerta enlace: --");
        }else {LINE(0,"Mascara: --");LINE(1,"Puerta enlace: --");LINE(2,"Sin datos actuales");}break;
    case AFR2_N_WIFI_DATA:{
        if(p->wifi.credentials_visible){strncpy(s->labels[0],p->wifi.draft_ssid,32);strncpy(s->labels[1],p->wifi.password,32);strncpy(s->labels[2],p->wifi.password+32,31);break;}
        uint8_t sta=p->wifi.sta_connected,auth=sta?p->wifi.security:p->wifi.ap_security;
        if(!p->wifi.available){LINE(0,"Sin datos actuales");break;}
        strncpy(s->labels[0],sta?p->wifi.ssid:p->wifi.ap_active?p->wifi.ap_ssid:"Sin red",32);
        if(sta && s->network_rssi_valid)LINE(1,"Senal: %d dBm",p->wifi.rssi);else LINE(1,"Senal: --");
        if(!p->wifi.details_valid || (uint32_t)(p->now-p->wifi.details_ms)>5000u)auth=0;
        LINE(2,"%s",auth==1?"Abierta":auth==2?"WEP":auth==3?"WPA":auth==4?"WPA2":auth==5?"WPA/WPA2":auth==6?"Protegida":"Seguridad: --");s->row_security[0]=auth;break;
    }
    case AFR2_N_WIFI_REVIEW:
        LINE(0,"%.17s",p->wifi.draft_ssid);LINE(1,"Modo:%u Listo:%u",p->wifi.requested_mode,p->wifi.draft_ready);LINE(2,"Opciones");break;
    case AFR2_N_WIFI_NETWORKS:
        for(uint8_t row=0;row<3;row++){uint8_t i=u->firstVisible+row;if(!p->wifi.pending && i<8 && i<p->wifi.network_count){strncpy(s->labels[row],p->wifi.networks[i][0]?p->wifi.networks[i]:"Red oculta",32);s->row_security[row]=p->wifi.network_security[i];}}break;
    case AFR2_N_VERSIONS:LINE(0,"Mega " AFR2_FIRMWARE_VERSION);LINE(1,"ESP:%.13s",p->wifi.esp_version[0]?p->wifi.esp_version:"--");
        LINE(2,"Web:%.13s",p->wifi.web_version[0]?p->wifi.web_version:"--");break;
    case AFR2_N_CAPS:
        switch(u->selection){
        case 0:LINE(0,"UI: encoder/OLED");LINE(1,"Dibujo por TWI");LINE(2,"OLED listo:%u",s->oled_ready);break;
        case 1:LINE(0,"UART: puente ESP");LINE(1,"Wi-Fi/HTTP/WS");LINE(2,"Enlace EX01:%u",p->extension_ready);break;
        case 2:LINE(0,"Sensores: IR/HC");LINE(1,"Linea:%u Eco:%u",p->sensors.line_valid,p->sensors.ultrasonic_valid);LINE(2,"Hall disponible:%u",p->sensors.hall_available);break;
        case 3:LINE(0,"PWM: sin odometria");LINE(1,"STOP corta salidas");LINE(2,"Motor:%u Fallo:%u",p->config.motor_ready,p->fault);break;
        case 4:LINE(0,"GOTO requiere pose");LINE(1,"CPR y CAD nominal");LINE(2,"Pose valida:%u",p->pose_valid);break;
        default:LINE(0,"Secuencias: pasos");LINE(1,"Sec:%u EEPROM:%u",Afr2Sequence_Available(p),p->prefs_loaded);LINE(2,"Guardar explicito");break;
        }break;
    case AFR2_N_PREFS_VIEW:LINE(0,"EEPROM valida:%u",p->prefs_loaded);LINE(1,"Guardado estado:%u",p->prefs_state);LINE(2,"CRC/version %u",AFR2_CONFIG_VERSION);break;
    default:break;
    }
    Afr2MenuNode node;Afr2Ui_MenuNode(u,&node);
    for(uint8_t row=0;row<3 && u->firstVisible+row<node.count;row++) {
        Afr2MenuItem item;Afr2Ui_MenuItem(u,u->firstVisible+row,&item);
        s->disabled[row]=item.action && available(a,item.action,item.arg)!=AFR2_OK;
        if(item.action==AFR2_A_RADAR_TOGGLE || item.action==AFR2_A_LINE_TOGGLE || item.action==AFR2_A_WHEEL_TOGGLE || item.action==AFR2_A_GOTO_TOGGLE || item.action==AFR2_A_ODOM_TOGGLE)strncpy(s->labels[row],item.label,32);
        if(item.action==AFR2_A_ESP_TEST && a->diag_state==1)strcpy(s->labels[row],"Enviando...");
    }
}
