/* AFR2-20261028-02. Temporary, standalone USART0 <-> USART1 service image.
 * Original AFR2 source, avr-libc only. Not linked into the vehicle firmware.
 * Source candidate: NOT compiled, flashed or exercised on hardware.
 * Fixed baud on BOTH UARTs. Binary transparent: no banners/commands/line edits.
 * RESET must remain FREE. Disconnect actuator power before installing.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdint.h>

#if !defined(__AVR_ATmega2560__) || F_CPU != 16000000UL
#error "Service bridge requires ATmega2560 at 16 MHz"
#endif
#ifndef AFR2_BRIDGE_BAUD
#define AFR2_BRIDGE_BAUD 115200UL
#endif
#if AFR2_BRIDGE_BAUD == 115200UL
#define BRIDGE_UBRR 16u
#define BRIDGE_DOUBLE 1
#elif AFR2_BRIDGE_BAUD == 57600UL
#define BRIDGE_UBRR 34u
#define BRIDGE_DOUBLE 1
#elif AFR2_BRIDGE_BAUD == 38400UL
#define BRIDGE_UBRR 51u
#define BRIDGE_DOUBLE 1
#elif AFR2_BRIDGE_BAUD == 9600UL
#define BRIDGE_UBRR 103u
#define BRIDGE_DOUBLE 0
#else
#error "Supported fixed bridge baud rates: 9600, 38400, 57600, 115200"
#endif

#define RING_SIZE 512u
#define RING_MASK (RING_SIZE - 1u)
typedef struct { uint8_t data[RING_SIZE]; uint16_t head, tail; } Ring;
static Ring to_esp, to_pc;
/* Debugger-only evidence, never injected into the programming UART stream.
 * Boot text at 74880 may legitimately produce framing errors at another baud.
 */
volatile uint16_t afr2_bridge_rx0_errors, afr2_bridge_rx1_errors;
volatile uint8_t afr2_bridge_overflow;

void Afr2Bridge_Early(void) __attribute__((naked,section(".init3"),used));
void Afr2Bridge_Early(void) { MCUSR = 0; wdt_disable(); }

static void quiesce(void)
{
    cli();
    TIMSK0 = TIMSK1 = TIMSK2 = TIMSK3 = TIMSK4 = TIMSK5 = 0;
    TCCR0A = TCCR0B = TCCR1A = TCCR1B = TCCR2A = TCCR2B = 0;
    TCCR3A = TCCR3B = TCCR4A = TCCR4B = TCCR5A = TCCR5B = 0;
    UCSR0B = UCSR1B = UCSR2B = UCSR3B = 0;
    TWCR = 0; ADCSRA = 0;
    /* Documented motor PWM D9,D10,D11,D12 and direction D5..D8/D22..D28.
     * Electrical low requests inactive outputs; does not prove power is absent.
     */
    PORTB &= (uint8_t)~(_BV(PB4)|_BV(PB5)|_BV(PB6));
    PORTH &= (uint8_t)~(_BV(PH3)|_BV(PH4)|_BV(PH5)|_BV(PH6));
    PORTA &= (uint8_t)~0x55u; PORTE &= (uint8_t)~_BV(PE3);
    DDRB |= _BV(PB4)|_BV(PB5)|_BV(PB6);
    DDRH |= _BV(PH3)|_BV(PH4)|_BV(PH5)|_BV(PH6);
    DDRA |= 0x55u; DDRE |= _BV(PE3);
    /* D13 is the radar servo in this car: never use it as a status LED. */
    DDRB &= (uint8_t)~_BV(PB7); PORTB &= (uint8_t)~_BV(PB7);
    DDRE &= (uint8_t)~_BV(PE0); PORTE &= (uint8_t)~_BV(PE0); /* RX0 */
    DDRD &= (uint8_t)~_BV(PD2); PORTD &= (uint8_t)~_BV(PD2); /* RX1 */
}
static void serial_init(void)
{
    UBRR0H = UBRR1H = (uint8_t)(BRIDGE_UBRR >> 8);
    UBRR0L = UBRR1L = (uint8_t)BRIDGE_UBRR;
    UCSR0A = BRIDGE_DOUBLE ? _BV(U2X0) : 0;
    UCSR1A = BRIDGE_DOUBLE ? _BV(U2X1) : 0;
    UCSR0C = _BV(UCSZ01)|_BV(UCSZ00);
    UCSR1C = _BV(UCSZ11)|_BV(UCSZ10);
    UCSR0B = _BV(RXEN0)|_BV(TXEN0);
    UCSR1B = _BV(RXEN1)|_BV(TXEN1);
}
static void overflow_stop(void) __attribute__((noreturn));
static void overflow_stop(void)
{
    afr2_bridge_overflow = 1;
    UCSR0B = UCSR1B = 0;
    for (;;) { /* An incomplete stream must fail esptool, never claim success. */ }
}
static void push(Ring *ring, uint8_t value)
{
    uint16_t next = (ring->head + 1u) & RING_MASK;
    if (next == ring->tail) overflow_stop();
    ring->data[ring->head] = value; ring->head = next;
}
int main(void)
{
    quiesce(); serial_init();
    for (;;) {
        if (UCSR0A & _BV(RXC0)) {
            uint8_t flags = UCSR0A, value = UDR0;
            if (flags & (_BV(FE0)|_BV(DOR0)|_BV(UPE0))) ++afr2_bridge_rx0_errors;
            else push(&to_esp, value);
        }
        if (UCSR1A & _BV(RXC1)) {
            uint8_t flags = UCSR1A, value = UDR1;
            if (flags & (_BV(FE1)|_BV(DOR1)|_BV(UPE1))) ++afr2_bridge_rx1_errors;
            else push(&to_pc, value);
        }
        if (to_esp.head != to_esp.tail && (UCSR1A & _BV(UDRE1))) {
            UDR1 = to_esp.data[to_esp.tail]; to_esp.tail = (to_esp.tail + 1u) & RING_MASK;
        }
        if (to_pc.head != to_pc.tail && (UCSR0A & _BV(UDRE0))) {
            UDR0 = to_pc.data[to_pc.tail]; to_pc.tail = (to_pc.tail + 1u) & RING_MASK;
        }
    }
}
