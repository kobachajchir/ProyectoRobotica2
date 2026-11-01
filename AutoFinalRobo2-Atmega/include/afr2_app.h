#ifndef AFR2_APP_H
#define AFR2_APP_H
#include "afr2_product.h"
uint8_t Afr2App_SensorCommand(void *ctx,uint8_t cmd,const uint8_t *d,uint8_t n,uint8_t *out,uint8_t *length);
#include "afr2_screens.h"
typedef struct {
    Afr2Product *product;
    Afr2Ui *ui;
    Afr2Oled *oled;
    Afr2Link *link;
    Afr2Config saved_config;
    uint32_t hall_draft[4]; /* AFR2-20261029-20: per-wheel unsaved measured CPR */
    uint8_t hall_draft_mask,hall_draft_negative,hall_saving;
    uint32_t snapshot_revision,snapshot_generation,snapshot_ms;
    uint8_t snapshot_active,line_channel,remote_ui;
    int16_t test_pwm;
    uint16_t servo_us;
    uint32_t render_ms,sequence_screen_execution,reboot_ms,esp_boot_notified;
    uint8_t reboot_pending;
    uint8_t diag_state,local_power_percent;
    uint16_t diag_timeout;
    uint32_t diag_job,diag_started;
} Afr2App;
void Afr2App_Init(Afr2App *a,Afr2Product *p,Afr2Ui *ui,Afr2Oled *oled,Afr2Link *link);
void Afr2App_Task(Afr2App *a,uint32_t now);
void Afr2App_UiSnapshot(Afr2App *a,Afr2UiSnapshot *s);
uint8_t Afr2App_Command(void *ctx,uint8_t cmd,const uint8_t *data,uint8_t n,
                       uint8_t *body,uint8_t *length);
void Afr2App_Revoke(void *ctx);
uint8_t Afr2App_NetworkCommand(Afr2App *a,uint8_t cmd,const uint8_t *data,uint8_t n,uint8_t *out,uint8_t *length);
uint8_t Afr2App_UiPrivate(const Afr2App *a);
uint8_t Afr2App_RemoteUiAllowed(uint8_t action);
#endif
