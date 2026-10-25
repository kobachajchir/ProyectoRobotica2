/* AFR2-20261023-15. ROBO2 USART1 + OLED/TWI Manager + local encoder UI.
 * No sensor/actuator service is invented: UART only is announced remotely.
 */
#include "afr2_board.h"
#include "afr2_uart1.h"
#include "afr2_link.h"
#include "afr2_twi.h"
#include "afr2_screens.h"
#include <avr/interrupt.h>
#ifndef AFR2_OLED_ADDRESS7
#define AFR2_OLED_ADDRESS7 0x3cu /* SSD1306 candidate, not measured */
#endif
#ifndef AFR2_ENCODER_TRANSITIONS
#define AFR2_ENCODER_TRANSITIONS 4
#endif
#if AFR2_ENCODER_TRANSITIONS != 2 && AFR2_ENCODER_TRANSITIONS != 4
#error "Encoder detent must use 2 or 4 transitions"
#endif
#ifndef AFR2_ENCODER_INVERT
#define AFR2_ENCODER_INVERT 0
#endif
static Afr2Link link;
static Afr2I2cManager manager;
static Afr2Oled oled;
static Afr2EncoderUi encoder;
static Afr2Ui ui;
int main(void) {
    Afr2I2cHardware hardware;
    Afr2UiSnapshot snapshot={0}; /* Sensor values remain invalid until real producers. */
    uint32_t sampled_ms=0;
    Afr2Board_Init(); Afr2Uart1_Init(); Afr2Link_Init(&link,0);
    Afr2Twi_Init(&hardware); (void)Afr2I2c_Init(&manager,&hardware);
    Afr2Oled_Init(&oled,&manager,AFR2_OLED_ADDRESS7,0);
    Afr2Board_EncoderInit();
    uint8_t levels=Afr2Board_EncoderLevels();
    Afr2EncoderUi_Init(&encoder,(levels&2)!=0,(levels&1)!=0,(levels&4)!=0,0);
    encoder.transitions=AFR2_ENCODER_TRANSITIONS;
    Afr2Ui_Init(&ui); sei();
    for (;;) {
        uint8_t b;
        uint32_t now=Afr2Board_Millis();
        if (Afr2Uart1_TakeFault()) Afr2Link_RxFault(&link);
        Afr2Link_Tick(&link,now);
        if (link.reply_length) {
            if (Afr2Uart1_Write(link.reply,link.reply_length)) link.reply_length=0;
        } else {
            for (uint8_t budget=64;budget && !link.reply_length && Afr2Uart1_Read(&b);--budget)
                Afr2Link_Push(&link,b,now);
        }
        /* Never synthesize missed samples by replaying today's GPIO levels. */
        if (now!=sampled_ms) {
            sampled_ms=now; levels=Afr2Board_EncoderLevels();
            Afr2UiInput input=Afr2EncoderUi_Poll(&encoder,(levels&2)!=0,
                (levels&1)!=0,(levels&4)!=0,now);
#if AFR2_ENCODER_INVERT
            input.rotation=-input.rotation;
#endif
            Afr2Ui_Apply(&ui,input);
        }
        Afr2Twi_Task(now); Afr2I2c_Tick(&manager,now);
        if (snapshot.oled_ready!=oled.initialized || snapshot.oled_errors!=oled.errors) {
            snapshot.oled_ready=oled.initialized; snapshot.oled_errors=oled.errors; ui.redraw=true;
        }
        if (ui.redraw) Afr2Ui_Render(&ui,&oled,&snapshot);
        Afr2Oled_Task(&oled,now);
    }
}
