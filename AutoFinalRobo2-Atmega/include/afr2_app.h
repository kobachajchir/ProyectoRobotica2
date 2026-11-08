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
    uint8_t line_background,line_foreground,line_samples;
    uint32_t snapshot_revision,snapshot_generation,snapshot_ms;
    uint8_t snapshot_active,line_channel,remote_ui;
    int16_t test_pwm;
    uint16_t servo_us;
    uint16_t raster_ms;
    uint32_t render_ms,sequence_screen_execution,reboot_ms,esp_boot_notified;
    uint8_t reboot_pending;
    uint8_t diag_state,local_power_percent;
    uint16_t diag_timeout;
    uint32_t diag_job,diag_started;
    int32_t hall_live_base[4]; /* AFR2-20261106-07: Prueba Hall zero captured on entry (17 B RAM) */
    uint8_t hall_live_active;
    uint8_t odom_dir;     /* AFR2-20261108-02: prueba de distancia, direccion 0..7 */
    uint8_t odom_cm;      /* 5..100 cm */
    uint8_t odom_state;   /* AFR2-20261108-04: 0 ninguna,1 esperando,2 lanzada,3 descartada,4 no se pudo lanzar,5 vencida */
    uint16_t odom_token;  /* identifica la propuesta de la Web (1..32767) */
    uint32_t odom_ask_ms;
    uint8_t wifi_ask;     /* AFR2-20261108-06: 0 nada, 1 aviso recibido (abrir selector cuando la UI quede libre), 2 selector abierto */
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
