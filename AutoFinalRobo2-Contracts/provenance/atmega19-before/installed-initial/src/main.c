/* AFR2-20261023-19. Product composition. No code runs on hardware by authoring it.
 * Startup leaves outputs off; movement requires explicit local/remote command.
 */
#include "afr2_board.h"
#include "afr2_uart1.h"
#include "afr2_twi.h"
#include "afr2_app.h"
#include <avr/interrupt.h>
#ifndef AFR2_OLED_ADDRESS7
#define AFR2_OLED_ADDRESS7 0x3c
#endif
static Afr2Product product;
static Afr2Link link;
static Afr2App app;
static Afr2I2cManager manager;
static Afr2Oled oled;
static Afr2EncoderUi encoder;
static Afr2Ui ui;
static uint8_t stopped(void *ctx){return Afr2Product_Stop(((Afr2App *)ctx)->product);}
static void snapshot(void *ctx,uint32_t now,Afr2LinkSnapshot *out){Afr2Product_Snapshot(((Afr2App *)ctx)->product,now,out);}
static uint16_t capabilities(void *ctx) {
    Afr2Product *p=((Afr2App *)ctx)->product;
    uint16_t c=AFR2_CAP_UART|AFR2_CAP_STATE|AFR2_CAP_STOP|AFR2_CAP_LINE|AFR2_CAP_ULTRASONIC;
    if(p->sensors.hall_available)c|=AFR2_CAP_ENCODERS;return c;
}
int main(void) {
    Afr2I2cHardware hardware;uint32_t sampled=0;
    Afr2Board_Init();Afr2Product_Init(&product,Afr2Boot_Cause());Afr2Uart1_Init();
    Afr2Twi_Init(&hardware);(void)Afr2I2c_Init(&manager,&hardware);
    Afr2Oled_Init(&oled,&manager,AFR2_OLED_ADDRESS7,0);Afr2Ui_Init(&ui);
    Afr2App_Init(&app,&product,&ui,&oled,&link);
    Afr2LinkOps ops={0};ops.ctx=&app;ops.snapshot=snapshot;ops.stop=stopped;
    ops.current_capabilities=capabilities;ops.command=Afr2App_Command;ops.revoke=Afr2App_Revoke;
    Afr2Link_Init(&link,&ops);Afr2Board_EncoderInit();
    uint8_t levels=Afr2Board_EncoderLevels();
    Afr2EncoderUi_Init(&encoder,(levels&2)!=0,(levels&1)!=0,(levels&4)!=0,0);
    Afr2Watchdog_Enable();sei();
    for(;;) {
        uint32_t now=Afr2Board_Millis();uint8_t b;
        Afr2App_Task(&app,now);
        if(Afr2Uart1_TakeFault())Afr2Link_RxFault(&link);
        Afr2Link_Tick(&link,now);
        if(link.reply_length){if(Afr2Uart1_Write(link.reply,link.reply_length))link.reply_length=0;}
        else for(uint8_t budget=64;budget && !link.reply_length && Afr2Uart1_Read(&b);--budget)Afr2Link_Push(&link,b,now);
        if(now!=sampled) {
            sampled=now;levels=Afr2Board_EncoderLevels();
            encoder.transitions=product.config.encoder_steps;
            Afr2UiInput input=Afr2EncoderUi_Poll(&encoder,(levels&2)!=0,(levels&1)!=0,(levels&4)!=0,now);
            if(product.config.encoder_invert)input.rotation=-input.rotation;
            Afr2Ui_Apply(&ui,input);
        }
        Afr2Twi_Task(now);Afr2I2c_Tick(&manager,now);
        if(ui.redraw && !app.snapshot_active) {
            Afr2UiSnapshot view;Afr2App_UiSnapshot(&app,&view);Afr2Ui_Render(&ui,&oled,&view);
        }
        Afr2Oled_Settings(&oled,product.config.contrast,product.config.rotation);
        Afr2Oled_Task(&oled,now);Afr2Watchdog_Kick();
    }
}
