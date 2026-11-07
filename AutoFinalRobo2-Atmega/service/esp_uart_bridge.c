/* AFR2-20261028-02. Temporary, standalone USART0 <-> USART1 service image.
 * Original AFR2 source, avr-libc only. Not linked into the vehicle firmware.
 * Source candidate: NOT compiled, flashed or exercised on hardware.
 * Fixed baud on BOTH UARTs. Binary transparent: no banners/line edits. The only
 * in-band element is the optional KEY/RST control frame described below.
 * RESET must remain FREE. Disconnect actuator power before installing.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>
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

/* AFR2-20261106-11. Optional KEY/RST control for the ESP, source candidate only.
 * D32/PC5 -> NPN base -> shield KEY (GPIO0); D33/PC4 -> NPN base -> shield RST.
 * Both pins stay INPUT (high impedance) until an explicit control frame arrives
 * and return to INPUT when released, on timeout, and on any Mega reset, so the
 * manual buttons and other firmware are never affected. Drive HIGH = button pressed.
 * Control frame (PC->Mega): A5 'A' 'F' 'R' '2' 'C' 'T' 'L' cmd ~cmd. Any other
 * byte stream is forwarded untouched; a partial prefix is held until resolved
 * (a lone trailing 0xA5 waits for the next byte). Reply (Mega->PC):
 * A5 'A' 'F' 'R' '2' 'R' 'S' 'P' cmd status flags: status 0 = done, 1 = refused
 * (automatic mode not enabled); flags bit0 KEY held, bit1 RST held, bit2 automatic
 * mode enabled. The PC configures the mode: AUTO_ON / AUTO_OFF. The flag lives in
 * RAM and is OFF after every Mega reset, so a freshly opened COM never drives a pin
 * until the PC sets it; while OFF, ENTER_BOOT/RUN/RST_PULSE are refused.
 */
#define KEY_MASK _BV(PC5)
#define RST_MASK _BV(PC4)
#define CMD_QUERY 0x01u      /* identify + report state, touches no pin */
#define CMD_ENTER_BOOT 0x10u /* hold KEY, pulse RST, keep KEY held */
#define CMD_RUN 0x11u        /* release KEY, then pulse RST: boot from flash */
#define CMD_RELEASE 0x12u    /* release KEY and RST, no reset */
#define CMD_RST_PULSE 0x13u  /* pulse RST only, KEY untouched */
#define CMD_BOOT_TEST 0x14u   /* AFR2-20261106-17: capture ROM boot at 74880 */
#define CMD_AUTO_OFF 0x20u   /* PC config: release pins, refuse pin commands */
#define CMD_AUTO_ON 0x21u    /* PC config: allow ENTER_BOOT/RUN/RST_PULSE */
#define KEY_MAX_HOLD_OVERFLOWS 143u /* Timer1 clk/1024 overflow 4.19 s: ~600 s */
static const uint8_t ctl_magic[8] = { 0xA5, 'A', 'F', 'R', '2', 'C', 'T', 'L' };
static const uint8_t rsp_magic[8] = { 0xA5, 'A', 'F', 'R', '2', 'R', 'S', 'P' };
static uint8_t ctl_pos, ctl_stage, ctl_cmd, key_held, rst_held, hold_overflows, auto_enabled;

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
    /* D32/D33 KEY/RST: input without pull-up (button released) until a frame arrives. */
    PORTC &= (uint8_t)~(KEY_MASK|RST_MASK); DDRC &= (uint8_t)~(KEY_MASK|RST_MASK);
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
/* Drive HIGH = transistor on = button pressed. Release: drive low, then input. */
static void line_press(uint8_t mask) { PORTC |= mask; DDRC |= mask; }
static void line_release(uint8_t mask) { PORTC &= (uint8_t)~mask; DDRC &= (uint8_t)~mask; }
static void hold_stop(void) { TCCR1B = 0; key_held = 0; }
static void release_all(void)
{
    line_release(RST_MASK); line_release(KEY_MASK);
    rst_held = 0; hold_stop();
}
static void rst_pulse(void)
{
    line_press(RST_MASK); rst_held = 1; _delay_ms(100);
    line_release(RST_MASK); rst_held = 0;
}
static void send_pc(uint8_t value)
{
    while (!(UCSR0A & _BV(UDRE0))) { }
    UDR0 = value;
}
static void reply(uint8_t cmd, uint8_t status)
{
    uint8_t i;
    for (i = 0; i < 8u; ++i) send_pc(rsp_magic[i]);
    send_pc(cmd); send_pc(status);
    send_pc((uint8_t)((key_held ? 1u : 0u) | (rst_held ? 2u : 0u) | (auto_enabled ? 4u : 0u) | 8u)); /* bit3: framed boot-test support */
}
static void flush_after_control(void)
{
    /* The ESP boot text at 74880 (or an unrelated byte) must not reach esptool. */
    while (UCSR1A & _BV(RXC1)) { (void)UDR1; }
    to_esp.head = to_esp.tail = 0; to_pc.head = to_pc.tail = 0;
}
/* AFR2-20261106-17. Bounded diagnostic, USB baud unchanged; no flash writes.
 * UART1 alone reads ROM boot at nominal 74880 (U2X, UBRR=26 at 16 MHz).
 * Payload: A5 AFR2BOOT, uint16 little-endian byte count, exact captured bytes.
 * Pins are released before any reply, even if the PC vanished during capture.
 */
static void capture_boot(void)
{
    uint8_t bytes[256], saved_a = UCSR1A & _BV(U2X1);
    uint8_t saved_h = UBRR1H, saved_l = UBRR1L, errors = 0;
    uint16_t count = 0, tick;
    static const uint8_t magic[] = {0xA5,'A','F','R','2','B','O','O','T'};
    release_all(); UCSR1B = 0;
    UBRR1H = 0; UBRR1L = 26; UCSR1A = _BV(U2X1);
    UCSR1B = _BV(RXEN1)|_BV(TXEN1);
    while (UCSR1A & _BV(RXC1)) { (void)UDR1; }
    line_press(KEY_MASK); key_held = 1; _delay_ms(20); rst_pulse();
    for (tick = 0; tick < 60000u; ++tick) {
        while (UCSR1A & _BV(RXC1)) {
            uint8_t flags = UCSR1A, value = UDR1;
            if (flags & (_BV(FE1)|_BV(DOR1)|_BV(UPE1))) errors = 1;
            if (count < sizeof(bytes)) bytes[count++] = value; else errors = 1;
        }
        _delay_us(20); /* poll faster than the two-byte USART receive buffer fills */
    }
    release_all(); UCSR1B = 0;
    UBRR1H = saved_h; UBRR1L = saved_l; UCSR1A = saved_a;
    UCSR1B = _BV(RXEN1)|_BV(TXEN1);
    flush_after_control(); reply(CMD_BOOT_TEST, errors ? 3u : 0u);
    for (tick = 0; tick < sizeof(magic); ++tick) send_pc(magic[tick]);
    send_pc((uint8_t)count); send_pc((uint8_t)(count >> 8));
    for (tick = 0; tick < count; ++tick) send_pc(bytes[tick]);
}

static void run_command(uint8_t cmd)
{
    uint8_t status = 0;
    switch (cmd) {
    case CMD_QUERY: break;
    case CMD_AUTO_OFF: release_all(); auto_enabled = 0; break;
    case CMD_AUTO_ON: auto_enabled = 1; break;
    case CMD_ENTER_BOOT:
        if (!auto_enabled) { status = 1; break; } /* not configured by the PC: touch nothing */
        release_all();
        line_press(KEY_MASK); key_held = 1; _delay_ms(20);
        TCNT1 = 0; TIFR1 = _BV(TOV1); hold_overflows = 0; TCCR1B = _BV(CS12)|_BV(CS10);
        rst_pulse(); _delay_ms(200); /* KEY stays held until RUN, RELEASE, timeout or Mega reset */
        break;
    case CMD_BOOT_TEST:
        if (!auto_enabled) { status = 1; break; }
        capture_boot(); return;
    case CMD_RUN:
        if (!auto_enabled) { status = 1; break; }
        line_release(KEY_MASK); hold_stop(); _delay_ms(20); rst_pulse(); _delay_ms(50);
        break;
    case CMD_RELEASE: release_all(); break;
    case CMD_RST_PULSE:
        if (!auto_enabled) { status = 1; break; }
        rst_pulse(); break;
    default: return; /* unknown command: ignored, nothing is driven */
    }
    flush_after_control(); reply(cmd, status);
}
/* PC->ESP byte path: forwards everything except a complete control frame. */
static void from_pc(uint8_t value)
{
    uint8_t i;
    if (ctl_stage == 1u) { ctl_cmd = value; ctl_stage = 2u; return; }
    if (ctl_stage == 2u) {
        ctl_stage = 0; ctl_pos = 0;
        if ((uint8_t)~ctl_cmd == value) run_command(ctl_cmd); /* bad complement: dropped */
        return;
    }
    if (value == ctl_magic[ctl_pos]) { if (++ctl_pos == 8u) ctl_stage = 1u; return; }
    for (i = 0; i < ctl_pos; ++i) push(&to_esp, ctl_magic[i]); /* not a frame: release held prefix */
    ctl_pos = 0;
    if (value == ctl_magic[0]) ctl_pos = 1u; else push(&to_esp, value);
}
int main(void)
{
    quiesce(); serial_init();
    for (;;) {
        if (key_held && (TIFR1 & _BV(TOV1))) {
            TIFR1 = _BV(TOV1);
            if (++hold_overflows >= KEY_MAX_HOLD_OVERFLOWS) release_all(); /* safety timeout */
        }
        if (UCSR0A & _BV(RXC0)) {
            uint8_t flags = UCSR0A, value = UDR0;
            if (flags & (_BV(FE0)|_BV(DOR0)|_BV(UPE0))) ++afr2_bridge_rx0_errors;
            else from_pc(value);
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
