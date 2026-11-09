/* AFR2-20261028-02. Temporary, standalone USART0 <-> USART1 service image ("flasher").
 * Original AFR2 source, avr-libc only. Not linked into the vehicle firmware.
 * Source candidate: NOT compiled, flashed or exercised on hardware.
 * Fixed baud on BOTH UARTs. Binary transparent: no banners/line edits. The only
 * in-band elements are the optional control frames described below.
 * RESET must remain FREE. Disconnect actuator power before installing.
 *
 * AFR2-20261108-08: OLED feedback over I2C (same Manager/TWI/OLED stack as the product and the
 * I2C diagnostic). Idle: "AFR2" / "Loader firmware" / version. While esptool talks to the ESP the
 * PC->ESP SLIP stream is only OBSERVED (never changed) to show the phase, the image being written,
 * a progress bar and the block count. Both UARTs now move bytes in interrupts so drawing and I2C
 * never delay the byte stream; rendering runs in the main loop.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "afr2_board.h"
#include "afr2_twi.h"
#include "afr2_i2c_manager.h"
#include "afr2_oled.h"
#include "afr2_build_info.h"

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
 * AFR2-20261108-08: flags bit4 = OLED feedback. CMD_LABEL carries a payload after ~cmd:
 * length N (5..22), offset u32LE, text N-4 bytes (printable ASCII, 1..18). Offset 0xFFFFFFFF is the
 * operation title and starts a new session; any other offset names the image written there.
 * RESULT_OK / RESULT_FAIL report how esptool ended. None of them touches KEY/RST or the ESP.
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
#define CMD_LABEL 0x30u       /* AFR2-20261108-08: title or image name for the OLED */
#define CMD_RESULT_OK 0x31u   /* AFR2-20261108-08: esptool ended without error */
#define CMD_RESULT_FAIL 0x32u /* AFR2-20261108-08: esptool ended with an error */
#define FLAG_DISPLAY 0x10u
#define KEY_MAX_HOLD_OVERFLOWS 143u /* Timer1 clk/1024 overflow 4.19 s: ~600 s */
static const uint8_t ctl_magic[8] = { 0xA5, 'A', 'F', 'R', '2', 'C', 'T', 'L' };
static const uint8_t rsp_magic[8] = { 0xA5, 'A', 'F', 'R', '2', 'R', 'S', 'P' };
static uint8_t key_held, rst_held, hold_overflows, auto_enabled;
/* Frame parser state, owned by the USART0 RX interrupt. */
static uint8_t ctl_pos, ctl_stage, ctl_cmd, ctl_need, ctl_len, ctl_buf[22];
/* Complete frame handed to the main loop (written in the ISR only while ctl_ready == 0). */
static volatile uint8_t ctl_ready;
static uint8_t ready_cmd, ready_len, ready_buf[22];

#define RING_SIZE 512u
#define RING_MASK (RING_SIZE - 1u)
typedef struct { uint8_t data[RING_SIZE]; volatile uint16_t head, tail; } Ring;
static Ring to_esp, to_pc;
static volatile uint8_t esp_hold; /* capture/control in progress: keep PC bytes queued */
/* Debugger-only evidence, never injected into the programming UART stream.
 * Boot text at 74880 may legitimately produce framing errors at another baud.
 */
volatile uint16_t afr2_bridge_rx0_errors, afr2_bridge_rx1_errors;
volatile uint8_t afr2_bridge_overflow;

/* AFR2-20261108-08: what esptool is doing, observed on the PC->ESP SLIP stream. */
enum { PH_IDLE, PH_CONNECT, PH_STUB, PH_ERASE, PH_WRITE, PH_WRITTEN, PH_VERIFY, PH_READ, PH_DONE_OK, PH_DONE_FAIL };
typedef struct { uint8_t phase, images, esp_reset; uint32_t offset, size, total, written; } Progress;
static volatile Progress prog;
static volatile uint8_t prog_seq;
static uint8_t slip_esc, slip_buf[24];
static uint16_t slip_count;

void Afr2Bridge_Early(void) __attribute__((naked,section(".init3"),used));
void Afr2Bridge_Early(void) { MCUSR = 0; wdt_disable(); }

static void quiesce(void)
{
    cli();
    TIMSK0 = TIMSK1 = TIMSK2 = TIMSK3 = TIMSK4 = TIMSK5 = 0;
    TCCR0A = TCCR0B = TCCR1A = TCCR1B = TCCR2A = TCCR2B = 0;
    TCCR3A = TCCR3B = TCCR4A = TCCR4B = TCCR5A = TCCR5B = 0;
    UCSR0B = UCSR1B = UCSR2B = UCSR3B = 0;
    TWCR = 0; ADCSRA = 0; EIMSK = 0; PCICR = 0;
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
#define UART1_RUN (_BV(RXEN1)|_BV(TXEN1)|_BV(RXCIE1))
static void serial_init(void)
{
    UBRR0H = UBRR1H = (uint8_t)(BRIDGE_UBRR >> 8);
    UBRR0L = UBRR1L = (uint8_t)BRIDGE_UBRR;
    UCSR0A = BRIDGE_DOUBLE ? _BV(U2X0) : 0;
    UCSR1A = BRIDGE_DOUBLE ? _BV(U2X1) : 0;
    UCSR0C = _BV(UCSZ01)|_BV(UCSZ00);
    UCSR1C = _BV(UCSZ11)|_BV(UCSZ10);
    UCSR0B = _BV(RXEN0)|_BV(TXEN0)|_BV(RXCIE0);
    UCSR1B = UART1_RUN;
}
/* An incomplete stream must fail esptool, never claim success: both UARTs stop until reset.
 * The OLED keeps running and shows the error. */
static void overflow_stop(void)
{
    afr2_bridge_overflow = 1;
    UCSR0B = UCSR1B = 0;
}
/* Callers run in an ISR or with interrupts disabled. */
static uint8_t push(Ring *ring, uint8_t value)
{
    uint16_t next = (ring->head + 1u) & RING_MASK;
    if (afr2_bridge_overflow) return 0;
    if (next == ring->tail) { overflow_stop(); return 0; }
    ring->data[ring->head] = value; ring->head = next;
    return 1;
}
static uint32_t slip_u32(uint8_t at)
{
    return (uint32_t)slip_buf[at] | ((uint32_t)slip_buf[at + 1u] << 8) | ((uint32_t)slip_buf[at + 2u] << 16) | ((uint32_t)slip_buf[at + 3u] << 24);
}
/* esptool request: 00 cmd sizeLE(2) checksum(4) data. Only the first 24 bytes are kept. */
static void slip_frame(void)
{
    uint8_t cmd = slip_buf[1];
    switch (cmd) {
    case 0x08: /* SYNC */
        if (prog.phase == PH_IDLE || prog.phase >= PH_DONE_OK) { prog.images = 0; prog.total = prog.written = 0; prog.esp_reset = 0; }
        if (prog.phase == PH_IDLE || prog.phase >= PH_DONE_OK) prog.phase = PH_CONNECT;
        break;
    case 0x05: prog.phase = PH_STUB; break; /* MEM_BEGIN: stub upload */
    case 0x02: case 0x10: /* FLASH_BEGIN / FLASH_DEFL_BEGIN: size, packets, packet size, offset */
        if (slip_count < 24u) return;
        prog.size = slip_u32(8); prog.total = slip_u32(12); prog.offset = slip_u32(20);
        prog.written = 0; ++prog.images; prog.phase = PH_WRITE; break;
    case 0x03: case 0x11: /* FLASH_DATA / FLASH_DEFL_DATA: size, sequence, 0, 0, data */
        if (slip_count < 16u) return;
        prog.written = slip_u32(12) + 1u;
        prog.phase = prog.total && prog.written >= prog.total ? PH_WRITTEN : PH_WRITE; break;
    case 0x13: prog.phase = PH_VERIFY; break;            /* SPI_FLASH_MD5 */
    case 0xD0: case 0xD1: prog.phase = PH_ERASE; break;  /* stub ERASE_FLASH / ERASE_REGION */
    case 0xD2: case 0x0E: prog.phase = PH_READ; break;   /* READ_FLASH */
    default: return;
    }
    ++prog_seq;
}
static void slip_byte(uint8_t value)
{
    if (value == 0xC0u) {
        if (slip_count >= 8u && slip_buf[0] == 0x00u) slip_frame();
        slip_count = 0; slip_esc = 0; return;
    }
    if (value == 0xDBu) { slip_esc = 1; return; }
    if (slip_esc) { slip_esc = 0; value = value == 0xDCu ? 0xC0u : value == 0xDDu ? 0xDBu : value; }
    if (slip_count < sizeof(slip_buf)) slip_buf[slip_count] = value;
    if (slip_count < 0xFFFFu) ++slip_count;
}
static void forward(uint8_t value)
{
    if (push(&to_esp, value) && !esp_hold) UCSR1B |= _BV(UDRIE1);
    slip_byte(value);
}
static void frame_complete(void)
{
    ctl_stage = 0; ctl_pos = 0;
    if (ctl_ready) return; /* previous frame not executed yet: dropped, the PC times out */
    ready_cmd = ctl_cmd; ready_len = ctl_len; memcpy(ready_buf, ctl_buf, ctl_len);
    ctl_ready = 1;
}
/* PC->ESP byte path (USART0 RX interrupt): forwards everything except a complete control frame. */
static void from_pc(uint8_t value)
{
    uint8_t i;
    if (ctl_stage == 1u) { ctl_cmd = value; ctl_stage = 2u; return; }
    if (ctl_stage == 2u) {
        if ((uint8_t)~ctl_cmd != value) { ctl_stage = 0; ctl_pos = 0; return; } /* bad complement: dropped */
        ctl_len = 0;
        if (ctl_cmd == CMD_LABEL) { ctl_stage = 3u; return; }
        frame_complete(); return;
    }
    if (ctl_stage == 3u) {
        if (value < 5u || value > sizeof(ctl_buf)) { ctl_stage = 0; ctl_pos = 0; return; }
        ctl_need = value; ctl_stage = 4u; return;
    }
    if (ctl_stage == 4u) { ctl_buf[ctl_len++] = value; if (ctl_len == ctl_need) frame_complete(); return; }
    if (value == ctl_magic[ctl_pos]) { if (++ctl_pos == 8u) ctl_stage = 1u; return; }
    for (i = 0; i < ctl_pos; ++i) forward(ctl_magic[i]); /* not a frame: release held prefix */
    ctl_pos = 0;
    if (value == ctl_magic[0]) ctl_pos = 1u; else forward(value);
}
ISR(USART0_RX_vect)
{
    uint8_t flags = UCSR0A, value = UDR0;
    if (flags & (_BV(FE0)|_BV(DOR0)|_BV(UPE0))) ++afr2_bridge_rx0_errors;
    else from_pc(value);
}
ISR(USART1_RX_vect)
{
    uint8_t flags = UCSR1A, value = UDR1;
    if (flags & (_BV(FE1)|_BV(DOR1)|_BV(UPE1))) ++afr2_bridge_rx1_errors;
    else if (push(&to_pc, value)) UCSR0B |= _BV(UDRIE0);
}
ISR(USART0_UDRE_vect)
{
    if (to_pc.head == to_pc.tail) { UCSR0B &= (uint8_t)~_BV(UDRIE0); return; }
    UDR0 = to_pc.data[to_pc.tail]; to_pc.tail = (to_pc.tail + 1u) & RING_MASK;
}
ISR(USART1_UDRE_vect)
{
    if (esp_hold || to_esp.head == to_esp.tail) { UCSR1B &= (uint8_t)~_BV(UDRIE1); return; }
    UDR1 = to_esp.data[to_esp.tail]; to_esp.tail = (to_esp.tail + 1u) & RING_MASK;
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
    cli();
    if (push(&to_pc, value)) UCSR0B |= _BV(UDRIE0);
    sei();
}
static void reply(uint8_t cmd, uint8_t status)
{
    uint8_t i;
    for (i = 0; i < 8u; ++i) send_pc(rsp_magic[i]);
    send_pc(cmd); send_pc(status);
    /* bit3: framed boot-test support; bit4: OLED feedback (AFR2-20261108-08) */
    send_pc((uint8_t)((key_held ? 1u : 0u) | (rst_held ? 2u : 0u) | (auto_enabled ? 4u : 0u) | 8u | FLAG_DISPLAY));
}
static void flush_after_control(void)
{
    /* The ESP boot text at 74880 (or an unrelated byte) must not reach esptool. */
    cli();
    while (UCSR1A & _BV(RXC1)) { (void)UDR1; }
    to_esp.head = to_esp.tail = 0; to_pc.head = to_pc.tail = 0;
    UCSR0B &= (uint8_t)~_BV(UDRIE0); UCSR1B &= (uint8_t)~_BV(UDRIE1);
    sei();
}
/* AFR2-20261106-17. Bounded diagnostic, USB baud unchanged; no flash writes.
 * UART1 alone reads ROM boot at nominal 74880 (U2X, UBRR=26 at 16 MHz).
 * Payload: A5 AFR2BOOT, uint16 little-endian byte count, exact captured bytes.
 * Pins are released before any reply, even if the PC vanished during capture.
 * AFR2-20261108-08: UART1 interrupts are off during the capture (polled as before);
 * PC bytes that arrive meanwhile stay queued and are flushed afterwards.
 */
static void capture_boot(void)
{
    uint8_t bytes[256], saved_a = UCSR1A & _BV(U2X1);
    uint8_t saved_h = UBRR1H, saved_l = UBRR1L, errors = 0;
    uint16_t count = 0, tick;
    static const uint8_t magic[] = {0xA5,'A','F','R','2','B','O','O','T'};
    release_all(); esp_hold = 1; UCSR1B = 0;
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
    UCSR1B = UART1_RUN;
    flush_after_control(); esp_hold = 0; reply(CMD_BOOT_TEST, errors ? 3u : 0u);
    for (tick = 0; tick < sizeof(magic); ++tick) send_pc(magic[tick]);
    send_pc((uint8_t)count); send_pc((uint8_t)(count >> 8));
    for (tick = 0; tick < count; ++tick) send_pc(bytes[tick]);
}

/* AFR2-20261108-08: labels sent by the Loader for the OLED. */
#define LABEL_TEXT 19u
static char title[LABEL_TEXT], image_name[3][LABEL_TEXT];
static uint32_t image_offset[3];
static uint8_t image_count, ui_dirty = 1;
static void reset_progress(void)
{
    cli(); prog.phase = PH_IDLE; prog.images = 0; prog.total = prog.written = prog.size = prog.offset = 0; prog.esp_reset = 0; ++prog_seq; sei();
}
static void apply_label(const uint8_t *d, uint8_t n)
{
    uint32_t offset = (uint32_t)d[0] | ((uint32_t)d[1] << 8) | ((uint32_t)d[2] << 16) | ((uint32_t)d[3] << 24);
    char text[LABEL_TEXT];uint8_t i, size = (uint8_t)(n - 4u);
    if (size >= LABEL_TEXT) size = LABEL_TEXT - 1u;
    for (i = 0; i < size; ++i) { uint8_t c = d[4u + i]; text[i] = (c >= 32u && c <= 126u) ? (char)c : '?'; }
    text[size] = 0;
    if (offset == 0xFFFFFFFFUL) { strcpy(title, text); image_count = 0; reset_progress(); cli(); prog.phase = PH_CONNECT; sei(); }
    else {
        for (i = 0; i < image_count && image_offset[i] != offset; ++i) { }
        if (i == image_count) { if (image_count == 3u) return; ++image_count; }
        image_offset[i] = offset; strcpy(image_name[i], text);
    }
    ui_dirty = 1;
}
static void run_command(uint8_t cmd, const uint8_t *data, uint8_t n)
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
        cli(); prog.esp_reset = 1; ++prog_seq; sei();
        break;
    case CMD_RELEASE: release_all(); break;
    case CMD_RST_PULSE:
        if (!auto_enabled) { status = 1; break; }
        rst_pulse(); break;
    /* AFR2-20261108-08: display only; no flush, the ESP stream is not touched. */
    case CMD_LABEL: apply_label(data, n); reply(cmd, 0); return;
    case CMD_RESULT_OK: case CMD_RESULT_FAIL:
        cli(); prog.phase = cmd == CMD_RESULT_OK ? PH_DONE_OK : PH_DONE_FAIL; ++prog_seq; sei(); reply(cmd, 0); return;
    default: return; /* unknown command: ignored, nothing is driven */
    }
    flush_after_control(); reply(cmd, status);
}

/* AFR2-20261108-08: OLED. Rotation/contrast are read (never written) from the product EEPROM. */
static Afr2I2cManager manager;
static Afr2Oled oled;
static void display_settings(uint8_t *contrast, uint8_t *rotation)
{
    uint8_t slot, head[16], found = 0;
    uint32_t best = 0;
    *contrast = 0xcf; *rotation = 1;
    for (slot = 0; slot < 2u; ++slot) {
        uint16_t base = (uint16_t)slot * 128u;
        eeprom_read_block(head, (const void *)(uintptr_t)base, sizeof(head));
        if (memcmp(head, "AFR2", 4) || head[4] < 1u || head[4] > 32u || eeprom_read_byte((const uint8_t *)(uintptr_t)(base + 127u)) != 0xa5u) continue;
        uint32_t seq = (uint32_t)head[8] | ((uint32_t)head[9] << 8) | ((uint32_t)head[10] << 16) | ((uint32_t)head[11] << 24);
        if (found && (int32_t)(seq - best) <= 0) continue;
        uint8_t c = eeprom_read_byte((const uint8_t *)(uintptr_t)(base + 26u)), r = eeprom_read_byte((const uint8_t *)(uintptr_t)(base + 27u));
        if (r > 1u) continue;
        found = 1; best = seq; *contrast = c; *rotation = r;
    }
}
static void centered_flash(uint8_t y, const char *text)
{
    uint16_t width = (uint16_t)strlen_P(text) * 7u;
    Afr2Oled_TextFlash(&oled, width < 128u ? (int16_t)((128u - width) / 2u) : 0, y, text, AFR2_WHITE);
}
static void centered(uint8_t y, const char *text)
{
    uint16_t width = (uint16_t)strlen(text) * 7u;
    Afr2Oled_Text(&oled, width < 128u ? (int16_t)((128u - width) / 2u) : 0, y, text, AFR2_WHITE);
}
static void big_text_flash(int16_t x, int16_t y, const char *text)
{
    uint8_t i, row, col;
    for (i = 0; pgm_read_byte(text + i); ++i)
        for (row = 0; row < 10u; ++row) {
            uint8_t bits = Afr2Font7x10_Row(pgm_read_byte(text + i), row);
            for (col = 0; col < 7u; ++col) if (bits & (0x80u >> col)) Afr2Oled_FillRect(&oled, x + i * 14 + col * 2, y + row * 2, 2, 2, AFR2_WHITE);
        }
}
static void render_idle(void)
{
    char line[19];
    big_text_flash(36, 2, PSTR("AFR2"));
    centered_flash(25, PSTR("Loader firmware"));
    centered_flash(37, PSTR("Version " AFR2_FIRMWARE_VERSION));
    if (afr2_bridge_overflow) centered_flash(52, PSTR("Error: desborde"));
    else { snprintf_P(line, sizeof(line), PSTR("USB-ESP %lu"), (unsigned long)AFR2_BRIDGE_BAUD); centered(52, line); }
}
static void render_active(const Progress *p)
{
    char line[19];
    uint8_t i, percent = p->total ? (uint8_t)(((uint32_t)(p->written > p->total ? p->total : p->written) * 100u) / p->total) : 0;
    Afr2Oled_FillRect(&oled, 0, 0, 128, 12, AFR2_WHITE);
    if (title[0]) Afr2Oled_Text(&oled, 2, 1, title, AFR2_BLACK); else Afr2Oled_TextFlash(&oled, 2, 1, PSTR("ESP8266"), AFR2_BLACK);
    /* Image being written: the Loader's name for this offset, else the offset itself. */
    line[0] = 0;
    if (p->images) {
        for (i = 0; i < image_count; ++i) if (image_offset[i] == p->offset) { strcpy(line, image_name[i]); break; }
        if (!line[0]) snprintf_P(line, sizeof(line), p->offset ? PSTR("Datos 0x%06lX") : PSTR("Firmware 0x0"), (unsigned long)p->offset);
        Afr2Oled_Text(&oled, 0, 15, line, AFR2_WHITE);
    } else Afr2Oled_TextFlash(&oled, 0, 15, image_count ? PSTR("Esperando ROM") : PSTR("Sin escritura"), AFR2_WHITE);
    switch (afr2_bridge_overflow ? 0xffu : p->phase) {
    case PH_CONNECT: strcpy_P(line, PSTR("Conectando...")); break;
    case PH_STUB: strcpy_P(line, PSTR("Preparando...")); break;
    case PH_ERASE: strcpy_P(line, PSTR("Borrando...")); break;
    case PH_WRITE: snprintf_P(line, sizeof(line), PSTR("Escribiendo %u%%"), percent); break;
    case PH_WRITTEN: strcpy_P(line, PSTR("Escrito 100%")); break;
    case PH_VERIFY: strcpy_P(line, PSTR("Verificando...")); break;
    case PH_READ: strcpy_P(line, PSTR("Leyendo...")); break;
    case PH_DONE_OK: strcpy_P(line, PSTR("Listo sin error")); break;
    case PH_DONE_FAIL: strcpy_P(line, PSTR("Error: ver Loader")); break;
    case 0xffu: strcpy_P(line, PSTR("Error: desborde")); break;
    default: line[0] = 0; break;
    }
    line[18] = 0; Afr2Oled_Text(&oled, 0, 27, line, AFR2_WHITE);
    Afr2Oled_Rect(&oled, 0, 39, 128, 9, AFR2_WHITE);
    if (p->total) Afr2Oled_FillRect(&oled, 2, 41, (uint8_t)(((uint32_t)percent * 124u) / 100u), 5, AFR2_WHITE);
    if (p->esp_reset) strcpy_P(line, PSTR("ESP reiniciado"));
    else if (p->total && image_count > 1u) snprintf_P(line, sizeof(line), PSTR("Img %u/%u %lu KB"), p->images, image_count, (unsigned long)((p->size + 1023u) / 1024u));
    else if (p->total) snprintf_P(line, sizeof(line), PSTR("%lu/%lu - %lu KB"), (unsigned long)p->written, (unsigned long)p->total, (unsigned long)((p->size + 1023u) / 1024u));
    else line[0] = 0;
    line[18] = 0; Afr2Oled_Text(&oled, 0, 52, line, AFR2_WHITE);
}
static void ui_task(uint32_t now)
{
    static uint8_t seen_seq = 0xffu, seen_overflow;
    static uint32_t render_ms, activity_ms;
    Progress p;
    uint8_t seq;
    cli(); seq = prog_seq; memcpy(&p, (const void *)&prog, sizeof(p)); sei();
    if (seq != seen_seq) { seen_seq = seq; activity_ms = now; ui_dirty = 1; }
    if (afr2_bridge_overflow != seen_overflow) { seen_overflow = afr2_bridge_overflow; ui_dirty = 1; }
    /* A finished session returns to the identity screen after 60 s without esptool traffic. */
    if (p.phase >= PH_DONE_OK && (uint32_t)(now - activity_ms) >= 60000u) { title[0] = 0; image_count = 0; reset_progress(); return; }
    if (!ui_dirty || (uint32_t)(now - render_ms) < 100u) return;
    ui_dirty = 0; render_ms = now;
    Afr2Oled_Clear(&oled);
    if (p.phase == PH_IDLE && !title[0]) render_idle(); else render_active(&p);
}

int main(void)
{
    Afr2I2cHardware hardware;
    uint8_t display, contrast, rotation;
    quiesce();
    Afr2Board_Init(); /* Timer0 millisecond tick + motor latches inactive; no other output */
    serial_init();
    Afr2Twi_Init(&hardware); display = Afr2I2c_Init(&manager, &hardware);
    Afr2Oled_Init(&oled, &manager, 0x3c, 0);
    display_settings(&contrast, &rotation); Afr2Oled_Settings(&oled, contrast, rotation);
    sei();
    for (;;) {
        uint32_t now = Afr2Board_Millis();
        if (key_held && (TIFR1 & _BV(TOV1))) {
            TIFR1 = _BV(TOV1);
            if (++hold_overflows >= KEY_MAX_HOLD_OVERFLOWS) release_all(); /* safety timeout */
        }
        if (ctl_ready) {
            uint8_t cmd = ready_cmd, n = ready_len, data[sizeof(ready_buf)];
            memcpy(data, ready_buf, n); ctl_ready = 0;
            run_command(cmd, data, n);
        }
        ui_task(now);
        Afr2Twi_Task(now); Afr2I2c_Tick(&manager, now);
        if (display && !manager.busy) Afr2Oled_Task(&oled, Afr2Board_Millis());
    }
}
