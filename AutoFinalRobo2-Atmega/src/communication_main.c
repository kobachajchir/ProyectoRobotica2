/* AFR2-20261023-15. Optional UART-only composition; excludes UI and TWI.
 * Never linked together with src/main.c. Board layer owns Timer0 and GPIO.
 */
#include "afr2_board.h"
#include "afr2_link.h"
#include "afr2_uart1.h"
#include <avr/interrupt.h>
static Afr2Link link;
int main(void) {
    Afr2Board_Init(); Afr2Link_Init(&link,0); Afr2Uart1_Init(); sei();
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
    }
}
