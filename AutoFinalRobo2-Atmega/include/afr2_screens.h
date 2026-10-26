/* AFR2-20261023-19. UI state, pure renderer and explicit service boundary. */
#ifndef AFR2_SCREENS_H
#define AFR2_SCREENS_H
#include "afr2_oled.h"
#include "afr2_encoder_ui.h"
#include "afr2_menu_catalog.h"
#include "afr2_network_wire.h"
#include "afr2_sensor_head.h"
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
    const char *(*reason)(void *,uint8_t action);
} Afr2UiOps;
typedef struct {uint8_t node,selection,first;} Afr2UiPosition;
typedef struct {char title[18],text[18];uint16_t ms;uint8_t kind;} Afr2Notification;
typedef struct {
    uint32_t screen,revision,generation,navigation_revision,clock_ms,notice_started_ms,scroll_started_ms,scroll_render_ms;
    uint8_t node,selection,firstVisible,depth,edit,keyboard,keyboard_field,key,length;
    uint8_t notices,head,tail;
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
    bool line_valid,ultrasonic_valid;
    uint8_t line_bits;
    uint16_t ultrasonic_mm;
    bool oled_ready;
    uint32_t oled_errors;
    char lines[3][19]; /* Prepared in main, never hardware reads in renderer. */
    char labels[3][33]; /* Full dynamic SSID, clipped/marqueed by renderer. */
    uint8_t disabled[3];
    uint8_t sequence_state,sequence_count,sequence_index,sequence_kind;
    uint32_t sequence_duration_ms,sequence_elapsed_ms;
    uint8_t network_valid,network_sta,network_ap,network_clients,network_server;
    /* Borrowed immutable main-loop strings, valid through Render. No ISR writes,
     * allocations or hardware reads. AP passphrase never enters remote capture. */
    const char *network_ssid,*network_ap_ssid,*network_password,*network_ip,*network_ap_ip;
} Afr2UiSnapshot;
void Afr2SensorScreen_Render(Afr2Oled *o,uint8_t node,const Afr2UiSnapshot *s);
void Afr2SequenceScreen_Render(Afr2Oled *o,const Afr2UiSnapshot *s);
void Afr2Ui_Init(Afr2Ui *ui);
void Afr2Ui_Attach(Afr2Ui *ui,const Afr2UiOps *ops);
void Afr2Ui_Apply(Afr2Ui *ui,Afr2UiInput input);
void Afr2Ui_Tick(Afr2Ui *ui,uint32_t now);
void Afr2Ui_Notify(Afr2Ui *ui,uint8_t kind,const char *title,const char *text,uint16_t ms);
void Afr2Ui_Render(Afr2Ui *ui,Afr2Oled *oled,const Afr2UiSnapshot *snapshot);
void Afr2Ui_Open(Afr2Ui *ui,uint8_t node);
uint32_t Afr2Ui_ReportedScreen(const Afr2Ui *ui);
uint16_t Afr2Ui_TextOffset(const char *text,uint8_t width,uint32_t elapsed);
void Afr2NetworkScreen_Render(Afr2Oled *o,const Afr2Ui *ui,const Afr2UiSnapshot *s);
void Afr2NetworkNotice_Render(Afr2Oled *o,const Afr2Ui *ui,const Afr2UiSnapshot *s);
#endif
