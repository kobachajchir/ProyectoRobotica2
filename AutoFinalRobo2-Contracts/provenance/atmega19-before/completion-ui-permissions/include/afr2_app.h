#ifndef AFR2_APP_H
#define AFR2_APP_H
#include "afr2_product.h"
#include "afr2_screens.h"
typedef struct {
    Afr2Product *product;
    Afr2Ui *ui;
    Afr2Oled *oled;
    Afr2Link *link;
    Afr2Config saved_config;
    uint32_t snapshot_revision,snapshot_generation,snapshot_ms;
    uint8_t snapshot_active,line_channel,remote_ui;
    int16_t test_pwm;
    uint16_t servo_us;
    uint32_t render_ms;
} Afr2App;
void Afr2App_Init(Afr2App *a,Afr2Product *p,Afr2Ui *ui,Afr2Oled *oled,Afr2Link *link);
void Afr2App_Task(Afr2App *a,uint32_t now);
void Afr2App_UiSnapshot(Afr2App *a,Afr2UiSnapshot *s);
uint8_t Afr2App_Command(void *ctx,uint8_t cmd,const uint8_t *data,uint8_t n,
                       uint8_t *body,uint8_t *length);
void Afr2App_Revoke(void *ctx);
#endif
