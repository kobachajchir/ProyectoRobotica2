/* AFR2-20261030-01: AutoMicro dashboard adaptation and linked icons; prior authors retained. */
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
/* AFR2-20261023-19. UI state, pure renderer and explicit service boundary. */
#ifndef AFR2_SCREENS_H
#define AFR2_SCREENS_H
#include "afr2_oled.h"
#include "afr2_dashboard.h"
#include "afr2_encoder_ui.h"
#include "afr2_menu_catalog.h"
#include "afr2_network_wire.h"
#include "afr2_sensor_head.h"
#define AFR2_SCREEN_MOTOR_SELECTOR 0x030044UL
#define AFR2_SCREEN_TOUCH 0x030035UL
#define AFR2_SCREEN_SEQUENCE 0x03002cUL
#define AFR2_SCREEN_HOME 0x010001UL
#define AFR2_SCREEN_MENU 0x010002UL
#define AFR2_SCREEN_SENSORS 0x020001UL
#define AFR2_SCREEN_DISPLAY 0x020002UL
typedef struct { char ssid[33],password[64]; } Afr2UiCredentials;
typedef struct {
    void *ctx;
    uint8_t (*invoke)(void *,uint8_t action,int32_t value,const void *data);
    int32_t (*value)(void *,uint8_t action);
    void (*keyboard_seed)(void *,Afr2UiCredentials *);
    uint8_t (*available)(void *,uint8_t action,int32_t arg);
    const char *(*reason_flash)(void *,uint8_t action); /* PROGMEM on AVR */
} Afr2UiOps;
typedef struct {uint8_t node,selection,first;} Afr2UiPosition;
typedef struct {char title[18],text[18];uint16_t ms;uint8_t kind;} Afr2Notification;
typedef struct {
    uint32_t screen,revision,generation,navigation_revision,clock_ms,notice_started_ms,scroll_started_ms,scroll_render_ms;
    Afr2DashboardState dashboard;
    uint32_t touch_ms,touch_started_ms;
    uint8_t touch_active,touch_cancelled,touch_direction,touch_heading,touch_full;
    uint8_t node,selection,firstVisible,depth,edit,keyboard,keyboard_field,key,length;
    uint8_t notices,head,tail;
    /* AFR2-20261029-20: reusable save/discard choice, action-based service. */
    uint8_t confirm_save,confirm_discard,confirm_target,confirm_wait;
    uint8_t quick_selection,quick_pending,quick_expected;
    uint8_t radar_selection,radar_first,test_short,test_long,test_back,notice_target;
    uint8_t power_percent,power_known,power_visible,menu_signature,wifi_signature;
    uint32_t power_changed_ms;
    uint32_t quick_started_ms,hold_started_ms;
    uint8_t hold_progress,quick_signature;
    /* AFR2-20261107-48: after the user closes a network notice, chatty network
     * events (WebSocket, clients, Web served) are accepted but not shown until then. */
    uint32_t network_quiet_until_ms;
    bool redraw,scroll_redraw;
    Afr2UiPosition history[8];
    Afr2MenuItem editing;
    int32_t draft;
    Afr2UiCredentials credentials;
    Afr2Notification notification[4];
    Afr2UiOps ops;
    int32_t encoder_turns;
    uint16_t encoder_clicks,encoder_holds;
} Afr2Ui;
typedef struct {
    uint32_t sensor_now_ms;
    const Afr2SensorHead *head; /* stable borrowed main-loop state during render */
    uint16_t ir_raw[5];
    uint8_t ir_kind,ir_valid;
    uint8_t radar_range,radar_trace,quick_flags;
    /* AFR2-20261030-01: voltage validity, never a charge percentage. */
    uint16_t battery_mv;
    uint8_t battery_valid;
    uint8_t dashboard_valid,dashboard_mode,dashboard_moving,dashboard_fault;
    bool line_valid,ultrasonic_valid;
    uint8_t line_bits;
    uint16_t ultrasonic_mm;
    bool oled_ready;
    uint32_t oled_errors;
    char lines[3][19]; /* Prepared in main, never hardware reads in renderer. */
    char labels[3][33]; /* Full dynamic SSID, clipped/marqueed by renderer. */
    uint8_t disabled[3];
    uint32_t hall_cpr[4]; /* pending measurement when present, saved CPR otherwise */
    uint8_t hall_pending;
    uint8_t sequence_state,sequence_count,sequence_index,sequence_kind;
    uint32_t sequence_duration_ms,sequence_elapsed_ms;
    uint8_t network_valid,network_sta,network_ap,network_clients,network_server;
    uint8_t network_mode,network_busy,network_page,network_total,network_truncated,network_scan_state;
    int16_t network_rssi;
    uint8_t network_rssi_valid;
    uint32_t network_boot;
    uint16_t network_revision;
    uint8_t qr_allowed;
    uint8_t row_security[3];
    /* Borrowed immutable main-loop strings, valid through Render. No ISR writes,
     * allocations or hardware reads. AP passphrase never enters remote capture. */
    const char *network_ssid,*network_ap_ssid,*network_password,*network_ip,*network_ap_ip;
} Afr2UiSnapshot;
void Afr2Ui_MenuNode(Afr2Ui *u,Afr2MenuNode *n);
void Afr2Ui_MenuItem(Afr2Ui *u,uint8_t index,Afr2MenuItem *item);
void Afr2Ui_NetworkRestart(Afr2Ui *u);
/* title/text are PROGMEM; FlashTitle keeps text in RAM. */
void Afr2Ui_NotifyFlash(Afr2Ui *,uint8_t,const char *,const char *,uint16_t);
void Afr2Ui_NotifyFlashTitle(Afr2Ui *,uint8_t,const char *,const char *,uint16_t);
void Afr2Ui_AfterNotice(Afr2Ui *u,uint8_t node);
/* AFR2-20261107-48: true while chatty network notices stay silent. */
uint8_t Afr2Ui_NetworkQuiet(const Afr2Ui *u);
void Afr2Ui_GestureHint(Afr2Oled *o,uint8_t x,uint8_t y,uint8_t is_long,const char *text);
uint8_t Afr2MotorSelector_Step(uint8_t selected,int8_t delta);
void Afr2MotorSelector_Render(Afr2Oled *o,uint8_t selected);
void Afr2Power_Render(Afr2Oled *o,uint8_t percent);
void Afr2SensorScreen_Render(Afr2Oled *o,uint8_t node,const Afr2UiSnapshot *s);
void Afr2SequenceScreen_Render(Afr2Oled *o,const Afr2UiSnapshot *s);
uint8_t Afr2Ui_BackItem(Afr2Ui *ui);
void Afr2Ui_Touch(Afr2Ui *ui,uint8_t active,uint8_t direction,uint8_t heading);
void Afr2Ui_Init(Afr2Ui *ui);
void Afr2Ui_Attach(Afr2Ui *ui,const Afr2UiOps *ops);
void Afr2Ui_Apply(Afr2Ui *ui,Afr2UiInput input);
void Afr2Ui_InputHeld(Afr2Ui *ui,uint8_t held,uint32_t now);
uint8_t Afr2Dashboard_Render(Afr2Ui *,Afr2Oled *,const Afr2UiSnapshot *);
void Afr2Ui_Tick(Afr2Ui *ui,uint32_t now);
void Afr2Ui_Notify(Afr2Ui *ui,uint8_t kind,const char *title,const char *text,uint16_t ms);
void Afr2Ui_Render(Afr2Ui *ui,Afr2Oled *oled,const Afr2UiSnapshot *snapshot);
void Afr2Ui_Open(Afr2Ui *ui,uint8_t node);
void Afr2Ui_ConfirmSave(Afr2Ui *ui,uint8_t save_action,uint8_t discard_action,uint8_t target);
void Afr2Ui_SaveCompleted(Afr2Ui *ui);
uint32_t Afr2Ui_ReportedScreen(const Afr2Ui *ui);
uint16_t Afr2Ui_TextOffsetFlash(const char *,uint8_t,uint32_t);
uint16_t Afr2Ui_TextOffset(const char *text,uint8_t width,uint32_t elapsed);
void Afr2NetworkScreen_Render(Afr2Oled *o,const Afr2Ui *ui,const Afr2UiSnapshot *s);
void Afr2NetworkNotice_Render(Afr2Oled *o,const Afr2Ui *ui,const Afr2UiSnapshot *s);
#endif
