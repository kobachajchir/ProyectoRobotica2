#include "afr2_product_wire.h"
#include "afr2_build_info.h"
#include <string.h>
/* AFR2-20261023-19. Product composition. No code runs on hardware by authoring it.
 * Startup leaves outputs off; movement requires explicit local/remote command.
 */
#include "afr2_board.h"
#include "afr2_uart1.h"
#include "afr2_twi.h"
#include "afr2_app.h"
#include "afr2_debug.h"
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
static uint8_t halt_motion(void *ctx){return Afr2Product_HaltMotion(((Afr2App *)ctx)->product);}
static void snapshot(void *ctx,uint32_t now,Afr2LinkSnapshot *out){Afr2Product_Snapshot(((Afr2App *)ctx)->product,now,out);}
static uint16_t capabilities(void *ctx) {
    Afr2Product *p=((Afr2App *)ctx)->product;
    uint16_t c=AFR2_CAP_UART|AFR2_CAP_STATE|AFR2_CAP_STOP|AFR2_CAP_LINE|AFR2_CAP_ULTRASONIC;
    if(p->sensors.hall_available)c|=AFR2_CAP_ENCODERS;
    return c;
}
/* AFR2-20261108-01. Link diagnostics go to the USART0 debug log (AFR2_DEBUG_LOG). */
static void link_event(void *ctx,const Afr2LinkEvent *e){(void)ctx;Afr2Debug_LinkEvent(e);}
int main(void) {
    Afr2I2cHardware hardware;uint32_t sampled=0,oled_slot_ms=0;
    Afr2Board_Init();Afr2Debug_Init(Afr2Boot_Cause(),Afr2Boot_Id());
    Afr2Product_Init(&product,Afr2Boot_Cause());Afr2Uart1_Init();
    Afr2Twi_Init(&hardware);(void)Afr2I2c_Init(&manager,&hardware);
    Afr2Oled_Init(&oled,&manager,AFR2_OLED_ADDRESS7,0);Afr2Ui_Init(&ui);
    Afr2App_Init(&app,&product,&ui,&oled,&link);
    Afr2LinkOps ops={0};ops.ctx=&app;ops.snapshot=snapshot;ops.stop=stopped;
    ops.current_capabilities=capabilities;ops.command=Afr2App_Command;ops.revoke=Afr2App_Revoke;ops.halt_motion=halt_motion;ops.event=link_event;
    Afr2Link_Init(&link,&ops);Afr2Board_EncoderInit();
    uint8_t levels=Afr2Board_EncoderLevels();
    Afr2EncoderUi_Init(&encoder,(levels&2)!=0,(levels&1)!=0,(levels&4)!=0,0);
    uint8_t boot_pending=1;
    (void)Afr2Boot_Id();Afr2Watchdog_Enable();sei();
    for(;;) {
        uint32_t now=Afr2Board_Millis();uint8_t b;
        const uint32_t loop_started=now;
        Afr2App_Task(&app,now);
        if(boot_pending && !link.reply_length) {
            uint8_t body[14]={0},frame[AFR2_WIRE_OVERHEAD+14];memcpy(body,"BT01",4);body[4]=AFR2_NODE_MEGA;
            afr2_put_u32(body+5,Afr2Boot_Id());body[9]=product.reset_cause;body[10]=AFR2_FIRMWARE_MAJOR;
            body[11]=AFR2_FIRMWARE_MINOR;body[12]=AFR2_FIRMWARE_PATCH;body[13]=AFR2_EXT_VERSION;
            uint8_t length=afr2_wire_build(frame,sizeof(frame),AFR2_ROUTE_RESPONSE,AFR2_EXT_BOOT,body,14);
            if(Afr2Uart1_Write(frame,length))boot_pending=0;
        }
        if(app.reboot_pending && (uint32_t)(now-app.reboot_ms)>=250u && !link.reply_length && Afr2Uart1_TxIdle() &&
           (Afr2Debug_TxIdle() || (uint32_t)(now-app.reboot_ms)>=500u))Afr2Watchdog_Reboot();
        if(Afr2Uart1_TakeFault())Afr2Link_RxFault(&link);
        Afr2Link_Tick(&link,now);
        if(link.reply_length){if(Afr2Uart1_Write(link.reply,link.reply_length))link.reply_length=0;}
        else for(uint8_t budget=64;budget && !link.reply_length && Afr2Uart1_Read(&b);--budget){
            /* ESP crash text arrives on this same UART. Parser states >=6 are a
             * frame past "ROBO2"+length: payload, never text for the log. */
            uint8_t frame_before=link.parser.state;
            Afr2Link_Push(&link,b,now);
            Afr2Debug_PeerByte(b,frame_before>=6u || link.parser.state>=6u);
        }
        Afr2Debug_Task(now,&link);
        /* AFR2-20261024-11. Bound UI work; Hall counters are captured by IRQ
         * first, even at an encoder IRQ entry. Actions remain exclusively here. */
        if(!encoder.raw_pressed && !encoder.stable_pressed)encoder.long_press_ms=product.config.long_press_ms;
        if(encoder.transitions!=product.config.encoder_steps) {
            encoder.transitions=product.config.encoder_steps;encoder.partial=0;
        }
        uint8_t rotary_drained=0;
        for(uint8_t budget=32;budget;--budget) {
            uint8_t ab,status=Afr2Board_EncoderRead(&ab);
            if(!status){rotary_drained=1;break;}
            if(status==2){Afr2EncoderUi_ResetRotation(&encoder,ab);rotary_drained=1;break;}
            Afr2UiInput input={Afr2EncoderUi_Transition(&encoder,ab),false,false};
            if(product.config.encoder_invert)input.rotation=-input.rotation;
            if(input.rotation){
                if(app.snapshot_active)AFR2_DBG("CAP cancelled by local encoder");
                Afr2Debug_Input(input.rotation,false,false,now);
                app.snapshot_active=0;Afr2Ui_Apply(&ui,input);
            }
        }
        /* Do not confirm a selection while older rotary edges remain queued. */
        if(rotary_drained && now!=sampled) {
            sampled=now;levels=Afr2Board_EncoderLevels();
            Afr2UiInput input=Afr2EncoderUi_Button(&encoder,(levels&4)!=0,now);
            Afr2Ui_InputHeld(&ui,encoder.raw_pressed,now);
            if(input.rotation || input.click || input.menu){
                if(app.snapshot_active)AFR2_DBG("CAP cancelled by local button");
                Afr2Debug_Input(input.rotation,input.click,input.menu,now);
                app.snapshot_active=0; /* Local input wins over remote capture. */
            }
            Afr2Ui_Apply(&ui,input);
        }
        Afr2Twi_Task(now);Afr2I2c_Tick(&manager,now);
        /* AFR2-20261106-34: tope de refresco (>=40 ms entre cuadros) para que avisos del ESP o lecturas del
         * ultrasonico no fuerzan redibujos continuos; la bandera redraw se conserva hasta el siguiente cuadro. */
        static uint32_t last_render_ms;
        if((ui.redraw || ui.scroll_redraw) && !app.snapshot_active && (uint32_t)(now-last_render_ms)>=40u) {
            last_render_ms=now;
            uint32_t started=Afr2Board_Millis();Afr2UiSnapshot view;Afr2App_UiSnapshot(&app,&view);Afr2Ui_Render(&ui,&oled,&view);app.raster_ms=(uint16_t)(Afr2Board_Millis()-started);
        }
        Afr2Oled_Settings(&oled,product.config.contrast,product.config.rotation);
        /* AFR2-20261029-11: low-priority bounded sender runs AFTER services,
         * input and render. No periodic forced full-frame retransmission. */
        /* AFR2-20261029-16: low priority is bounded, never starvation. Even
         * persistent input/UART work allows one OLED stage every2ms. Task is
         * nonblocking and any new transfer remains limited to16data bytes. */
        now=Afr2Board_Millis();
        uint8_t oled_slot=rotary_drained && !link.reply_length;
        if((uint32_t)(now-oled_slot_ms)>=2u)oled_slot=1;
        Afr2Oled_Pause(&oled,!oled_slot);
        Afr2Oled_Task(&oled,now);
        if(oled_slot)oled_slot_ms=now;
        Afr2Debug_LoopTime((uint16_t)(Afr2Board_Millis()-loop_started));
        Afr2Watchdog_Kick();
    }
}
