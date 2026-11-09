/* AFR2-20261108-01. Product debug log over afr2_usart (default USART0/D1, 115200).
 *
 * Line format: "<ms> <TAG> text". Tags:
 *   AVR   own boot and reset cause         ESP   ESP boot frame and its reset reason
 *   ESP>  text the ESP printed on its UART (crash dump); ESP+ continues a long line
 *   LINK  session, supervision loss, UART faults, refused requests
 *   NET   Wi-Fi/WS/HTTP events and state forwarded by the ESP
 *   CAP   remote OLED capture (robot.screen.get)   ENC  local encoder
 *   STAT  every 5 s: link counters, slowest loop, untouched stack, dropped lines
 * Never logs secrets: no SSID passwords, no AP passphrase, no frame payloads.
 */
#include "afr2_debug.h"
#if AFR2_DEBUG_LOG
#include "afr2_usart.h"
#include "afr2_board.h"
#include "afr2_build_info.h"
#include "afr2_product_wire.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>

#ifndef AFR2_DEBUG_PORT
#define AFR2_DEBUG_PORT 0
#endif
#ifndef AFR2_DEBUG_TAP
#define AFR2_DEBUG_TAP 0
#endif
/* AFR2-20261108-10: log and single-direction TAP modes at 115200; only mode 3 (both directions,
 * 115200 each way on the link) switches to AFR2_DEBUG_FAST_BAUD after confirming, and back. */
#ifndef AFR2_DEBUG_BAUD
#define AFR2_DEBUG_BAUD 115200UL
#endif
#ifndef AFR2_DEBUG_FAST_BAUD
#define AFR2_DEBUG_FAST_BAUD 250000UL   /* exact at 16 MHz */
#endif
#ifndef AFR2_DEBUG_TX_SIZE
#if AFR2_DEBUG_TAP
#define AFR2_DEBUG_TX_SIZE 256u
#else
#define AFR2_DEBUG_TX_SIZE 128u
#endif
#endif
#define ROM_QUIET_MS 50u       /* RX fault after this much ESP silence: probably its ROM at 74880 */
#define ROM_REPEAT_MS 2000u
#define DBG_LINE_MAX 80u           /* including CR LF */
#define DBG_PEER_MAX 48u           /* ESP text chunk; longer lines continue as "ESP+" */
#define DBG_PEER_MIN 8u            /* shorter text runs are treated as binary noise */
#define DBG_PEER_LINES 16u         /* per DBG_PEER_WINDOW_MS, the rest is counted */
#define DBG_PEER_WINDOW_MS 2000u
#define STAT_MS 5000u
#define REJECT_REPEAT_MS 2000u
#define FAULT_REPEAT_MS 500u
#define SLOW_LOOP_MS 100u      /* watchdog is 500 ms */
#define STACK_PAINT 0xC5u

/* Owns the debug port's vectors; USART1 remains with afr2_uart1.c. */
AFR2_USART_BIND_ISR(AFR2_DEBUG_PORT)

static Afr2Usart usart;
static uint8_t tx_ring[AFR2_DEBUG_TX_SIZE];
static char line[DBG_LINE_MAX];
static char peer[DBG_PEER_MAX];
static struct {
    uint8_t ready, peer_len, peer_cont, peer_skip, peer_armed, peer_lines;
    uint8_t reject_cmd, reject_res, fault_burst, net_valid;
    uint8_t net[5];
    int8_t rotation;
    uint16_t lost, peer_quiet, peer_hex, reject_repeats, loop_max;
    uint16_t seen_requests, seen_rejects, seen_faults;
    uint16_t esp_resets;   /* AFR2-20261108-09: ESP boot frames since this AVR boot */
    uint32_t rotation_ms, stat_ms, reject_ms, fault_ms, peer_window_ms, esp_boot, net_boot;
    uint32_t peer_ms, rom_ms;
} d;

#if AFR2_DEBUG_TAP
/* AFR2-20261108-09. TAP state. Control line arrives on RX0 from the PC (USB). */
#define TAP_SYNC 0x1Eu
#define TAP_F_REDACTED 0x01u
#define TAP_F_LOST 0x02u
#define TAP_RAW_MAX 32u
#define TAP_RAW_IDLE_MS 20u
#define TAP_LINE_MAX 24u
#define TAP_LINE_DISCARD 0xFFu
static uint8_t rx_ring[16];
static char tap_line[TAP_LINE_MAX];
static uint8_t tap_raw[TAP_RAW_MAX];
static struct {
    uint8_t mode, line_len, raw_len, raw_flags, raw_lost, report, switching;
    uint16_t lost;
    uint32_t raw_ms, baud, switch_ms;   /* baud: current port speed */
} t;
#endif
static uint8_t debug_open(uint32_t baud);

/* Stack low-water mark: paint free RAM once, later count bytes never touched.
 * Valid because the product uses no heap (no malloc). */
extern uint8_t __heap_start;
static void paint_stack(void) {
    uint8_t *p = &__heap_start, *limit = (uint8_t *)SP - 32;
    while (p < limit) *p++ = STACK_PAINT;
}
static uint16_t stack_untouched(void) {
    const uint8_t *p = &__heap_start, *limit = (const uint8_t *)SP;
    uint16_t n = 0;
    while (p < limit && *p == STACK_PAINT) { ++p; ++n; }
    return n;
}

static uint8_t vlog(PGM_P format, va_list args) {
    if (!d.ready) return 0;
#if AFR2_DEBUG_TAP
    if (t.switching) { if (d.lost != UINT16_MAX) ++d.lost; return 0; }   /* speed change in progress */
#endif
    int head = snprintf_P(line, DBG_LINE_MAX - 2u, PSTR("%lu "), (unsigned long)Afr2Board_Millis());
    if (head < 0) head = 0;
    int body = vsnprintf_P(line + head, DBG_LINE_MAX - 2u - (unsigned)head, format, args);
    if (body < 0) body = 0;
    uint16_t length = (uint16_t)(head + body);
    if (length > DBG_LINE_MAX - 3u) length = DBG_LINE_MAX - 3u;   /* truncated, still one line */
    line[length] = '\r'; line[length + 1u] = '\n';
    if (!Afr2Usart_Write(&usart, line, length + 2u)) { if (d.lost != UINT16_MAX) ++d.lost; return 0; }
    return 1;
}
static uint8_t log_p(PGM_P format, ...) {
    va_list args; va_start(args, format);
    uint8_t ok = vlog(format, args);
    va_end(args);
    return ok;
}
#if AFR2_DEBUG_TAP
/* In TAP modes only reset/link-loss lines stay in the log; STAT/ESP text use log_p directly. */
static uint8_t starts_P(PGM_P text, PGM_P prefix) {
    for (;;) {
        char p = (char)pgm_read_byte(prefix++);
        if (!p) return 1;
        if ((char)pgm_read_byte(text++) != p) return 0;
    }
}
static uint8_t tap_allows(PGM_P format) {
    return !t.mode || starts_P(format, PSTR("AVR")) || starts_P(format, PSTR("ESP")) ||
        starts_P(format, PSTR("LINK lost")) || starts_P(format, PSTR("LINK hello")) || starts_P(format, PSTR("TAP"));
}
#else
#define tap_allows(format) 1
#endif
void Afr2Debug_Printf(PGM_P format, ...) {
    if (!tap_allows(format)) return;
    va_list args; va_start(args, format);
    (void)vlog(format, args);
    va_end(args);
}

static PGM_P esp_reason(uint8_t r) {
    switch (r) {   /* ESP8266 rst_info.reason (SDK user_interface.h, core postmortem) */
    case 0: return PSTR("power-on");
    case 1: return PSTR("hw-watchdog");
    case 2: return PSTR("exception");
    case 3: return PSTR("soft-watchdog");
    case 4: return PSTR("soft-restart(restart/abort)");
    case 5: return PSTR("deep-sleep");
    case 6: return PSTR("external-reset");
    case 253: return PSTR("stack-smash");
    case 254: return PSTR("user-exception");
    default: return PSTR("unknown");
    }
}
static PGM_P result_name(uint8_t r) {
    switch (r) {
    case AFR2_RESULT_BAD_REQUEST: return PSTR("bad-request");
    case AFR2_RESULT_UNAVAILABLE: return PSTR("unavailable");
    case AFR2_RESULT_SESSION: return PSTR("no-session");
    case AFR2_RESULT_INTERNAL: return PSTR("internal");
    default: return PSTR("?");
    }
}
static PGM_P net_name(uint8_t kind) {
    switch (kind) {
    case AFR2_NET_BOOT: return PSTR("esp-boot");
    case AFR2_NET_WEB_READY: return PSTR("web-ready");
    case AFR2_NET_GOT_IP: return PSTR("sta-got-ip");
    case AFR2_NET_AP_READY: return PSTR("ap-ready");
    case AFR2_NET_CLIENT_JOIN: return PSTR("ap-client-join");
    case AFR2_NET_CLIENT_LEAVE: return PSTR("ap-client-leave");
    case AFR2_NET_WS_OPEN: return PSTR("ws-open");
    case AFR2_NET_WS_CLOSE: return PSTR("ws-close");
    case AFR2_NET_WEB_SERVED: return PSTR("index-served");
    case AFR2_NET_STA_LOST: return PSTR("sta-lost");
    case AFR2_NET_AP_STOPPED: return PSTR("ap-stopped");
    case AFR2_NET_STA_NOT_FOUND: return PSTR("sta-not-found"); /* AFR2-20261108-06 */
    default: return PSTR("?");
    }
}
static PGM_P mode_name(uint8_t mode) {
    return mode == 1u ? PSTR("STA") : mode == 2u ? PSTR("AP") : mode == 3u ? PSTR("AP_STA") : PSTR("OFF");
}

static uint8_t debug_open(uint32_t baud) {
    Afr2UsartConfig config = AFR2_USART_CONFIG_INIT(AFR2_DEBUG_PORT, baud);
    config.tx_buffer = tx_ring; config.tx_size = sizeof(tx_ring);
#if AFR2_DEBUG_TAP
    config.rx_buffer = rx_ring; config.rx_size = sizeof(rx_ring);
#endif
    if (Afr2Usart_Init(&usart, &config) != AFR2_USART_OK) return 0;
#if AFR2_DEBUG_TAP
    t.baud = baud;
#endif
    return 1;
}
void Afr2Debug_Init(uint8_t cause, uint32_t boot_id) {
    paint_stack();
    memset(&d, 0, sizeof(d));
#if AFR2_DEBUG_TAP
    memset(&t, 0, sizeof(t));   /* mode 0 (log) at 115200 after every reset; never stored */
#endif
    if (!debug_open(AFR2_DEBUG_BAUD)) return;   /* stays silent */
    d.ready = 1;
    /* MCUSR bits: PORF 0, EXTRF 1 (reset pin, also USB auto-reset), BORF 2, WDRF 3, JTRF 4. */
    AFR2_DBG("AVR BOOT id=%lu cause=0x%02x%S%S%S%S fw=" AFR2_FIRMWARE_VERSION " ext=%u",
        (unsigned long)boot_id, cause,
        (cause & 0x01u) ? PSTR(" power-on") : PSTR(""), (cause & 0x02u) ? PSTR(" reset-pin") : PSTR(""),
        (cause & 0x04u) ? PSTR(" brown-out") : PSTR(""), (cause & 0x08u) ? PSTR(" watchdog") : PSTR(""),
        (unsigned)AFR2_EXT_VERSION);
    AFR2_DBG("AVR log USART%u %lu bd (real %lu), level %u", (unsigned)AFR2_DEBUG_PORT,
        (unsigned long)AFR2_DEBUG_BAUD, (unsigned long)Afr2Usart_ActualBaud(&usart), (unsigned)AFR2_DEBUG_LOG);
}

uint8_t Afr2Debug_TxIdle(void) { return !d.ready || Afr2Usart_TxIdle(&usart); }

/* ---- AFR2-20261108-09: TAP (frames/raw bytes to the USB, selected from the PC) ---- */
#if AFR2_DEBUG_TAP
static uint8_t sensitive_command(uint8_t command) {
    /* Keep in sync with "sensitive": true in Contracts/contracts/wire_catalog.json. */
    return command == AFR2_EXT_WIFI_POLL || command == AFR2_EXT_WIFI_UPDATE || command == AFR2_EXT_NETWORK_STATE;
}
static PGM_P tap_name(uint8_t mode) {
    switch (mode) {
    case 1: return PSTR("rx-atmega");
    case 2: return PSTR("rx-esp");
    case 3: return PSTR("link");
    case 4: return PSTR("raw");
    default: return PSTR("log");
    }
}
/* All-or-nothing: the record goes out whole or is counted as lost. Main context only. */
static uint8_t tap_write(uint8_t type, uint8_t flags, const uint8_t *a, uint8_t na, const uint8_t *b, uint8_t nb) {
    uint8_t head[9], n = (uint8_t)(na + nb), x = 0, i;
    if (t.switching || Afr2Usart_TxFree(&usart) < (uint16_t)(10u + n)) { if (t.lost != UINT16_MAX) ++t.lost; return 0; }
    head[0] = TAP_SYNC; head[1] = 'T'; head[2] = type; head[3] = flags;
    afr2_put_u32(head + 4, Afr2Board_Millis()); head[8] = n;
    for (i = 1; i < 9u; ++i) x ^= head[i];
    for (i = 0; i < na; ++i) x ^= a[i];
    for (i = 0; i < nb; ++i) x ^= b[i];
    (void)Afr2Usart_Write(&usart, head, 9u);
    if (na) (void)Afr2Usart_Write(&usart, a, na);
    if (nb) (void)Afr2Usart_Write(&usart, b, nb);
    (void)Afr2Usart_Write(&usart, &x, 1u);
    return 1;
}
static void tap_frame(uint8_t type, uint8_t route, uint8_t command, const uint8_t *payload, uint8_t length) {
    if (!d.ready || !(t.mode == type || t.mode == 3u)) return;
    uint8_t head[3] = { route, command, length };
    if (sensitive_command(command)) { (void)tap_write(type, TAP_F_REDACTED, head, 3u, 0, 0); return; }
    (void)tap_write(type, 0, head, 2u, payload, length);
}
static void tap_flush_raw(void) {
    if (!t.raw_len) return;
    uint8_t sent = tap_write(3u, (uint8_t)(t.raw_flags | (t.raw_lost ? TAP_F_LOST : 0u)), tap_raw, t.raw_len, 0, 0);
    t.raw_lost = !sent; t.raw_len = 0; t.raw_flags = 0;
}
static uint32_t tap_baud_for(uint8_t mode) { return mode == 3u ? AFR2_DEBUG_FAST_BAUD : AFR2_DEBUG_BAUD; }
static void tap_set_mode(uint8_t mode) {
    t.raw_len = 0; t.raw_flags = 0; t.raw_lost = 0;
    t.mode = mode; t.report = 1;
}
/* AFR2-20261108-10: after the confirmation (sent at the OLD speed) leaves the pin, stay silent
 * TAP_SWITCH_MS so the PC can change its own speed first, then reopen the port at the mode's speed. */
#define TAP_SWITCH_MS 500u
static void tap_switch_baud(uint32_t now) {
    if (!t.switching || !Afr2Usart_TxIdle(&usart) || (uint32_t)(now - t.switch_ms) < TAP_SWITCH_MS) return;
    uint32_t target = tap_baud_for(t.mode);
    t.switching = 0;
    Afr2Usart_Deinit(&usart);
    if (!debug_open(target) && !debug_open(AFR2_DEBUG_BAUD)) { d.ready = 0; return; }
    (void)log_p(PSTR("TAP baud=%lu"), (unsigned long)t.baud);
}
static void tap_command(void) {
    /* Only "#AFR2 TAP n" / "#AFR2 TAP ?"; anything else is ignored. Never echoes the input. */
    if (strncmp_P(tap_line, PSTR("#AFR2 TAP "), 10) != 0) return;
    char arg = tap_line[10];
    if (tap_line[11] != 0) { AFR2_DBG("TAP error: use #AFR2 TAP 0..4 or ?"); return; }
    if (arg == '?') { t.report = 1; return; }
    if (arg >= '0' && arg <= '4') { tap_set_mode((uint8_t)(arg - '0')); return; }
    AFR2_DBG("TAP error: use #AFR2 TAP 0..4 or ?");
}
static void tap_control(uint32_t now) {
    uint8_t c, budget = 32;
    while (budget-- && Afr2Usart_Read(&usart, &c)) {
        if (c == '\r' || c == '\n') {
            if (t.line_len && t.line_len != TAP_LINE_DISCARD) { tap_line[t.line_len] = 0; tap_command(); }
            t.line_len = 0;
            continue;
        }
        if (t.line_len == TAP_LINE_DISCARD) continue;
        if (c < 0x20u || c > 0x7eu || t.line_len >= TAP_LINE_MAX - 1u) { t.line_len = TAP_LINE_DISCARD; continue; }
        tap_line[t.line_len++] = (char)c;
    }
    /* The confirmation is retried until it fits: the PC waits for it before switching views. It names
     * the speed the port will use for this mode; the PC changes its own speed when it reads it. */
    if (t.report && !t.switching) {
        uint32_t target = tap_baud_for(t.mode);
        if (log_p(PSTR("TAP mode=%u %S baud=%lu"), t.mode, tap_name(t.mode), (unsigned long)target)) {
            t.report = 0;
            if (target != t.baud) { t.switching = 1; t.switch_ms = now; }
        }
    }
    tap_switch_baud(now);
    if (t.raw_len && (uint32_t)(now - t.raw_ms) >= TAP_RAW_IDLE_MS) tap_flush_raw();
}
#endif

void Afr2Debug_PeerRaw(uint8_t b, bool payload_byte, uint8_t command) {
#if AFR2_DEBUG_TAP
    if (!d.ready || t.mode != 4u) return;
    if (payload_byte && sensitive_command(command)) { b = 0; t.raw_flags |= TAP_F_REDACTED; }
    if (!t.raw_len) t.raw_ms = Afr2Board_Millis();
    tap_raw[t.raw_len++] = b;
    if (t.raw_len >= TAP_RAW_MAX) tap_flush_raw();
#else
    (void)b; (void)payload_byte; (void)command;
#endif
}

void Afr2Debug_LoopTime(uint16_t ms) {
    if (ms > d.loop_max) d.loop_max = ms;
    if (ms >= SLOW_LOOP_MS) AFR2_DBG("LOOP slow %u ms (watchdog 500 ms)", ms);
}

static void flush_rotation(void) {
    if (d.rotation) { AFR2_DBG("ENC rot %+d", (int)d.rotation); d.rotation = 0; }
}
void Afr2Debug_Input(int8_t rotation, bool click, bool menu, uint32_t now) {
    if (rotation) {
        if (!d.rotation) d.rotation_ms = now;
        int16_t sum = (int16_t)d.rotation + rotation;
        d.rotation = (int8_t)(sum > 100 ? 100 : sum < -100 ? -100 : sum);
    }
    if (click || menu) flush_rotation();
    if (click) AFR2_DBG("ENC click");
    if (menu) AFR2_DBG("ENC long (released >= long press)");
}

static void flush_rejects(void) {
    if (d.reject_repeats) {
        AFR2_DBG("LINK reject cmd=0x%02x %S repeated x%u", d.reject_cmd, result_name(d.reject_res), d.reject_repeats);
        d.reject_repeats = 0;
    }
}
static void flush_faults(uint32_t now) {
    if (d.fault_burst) {
        AFR2_DBG("LINK rx-fault x%u (framing/overrun/ring full)", d.fault_burst);
        d.fault_burst = 0; d.fault_ms = now;
    }
}

void Afr2Debug_LinkEvent(const Afr2LinkEvent *e) {
    if (!e || !d.ready) return;
    uint32_t now = Afr2Board_Millis();
    switch (e->kind) {
    case AFR2_LINK_EVT_PEER_BOOT: {
        if (!e->data || e->length < 14u) break;
        const uint8_t *b = e->data;   /* "BT01", node, bootId u32, reason, major, minor, patch, ext */
        d.esp_boot = afr2_get_u32(b + 5);
        if (d.esp_resets != UINT16_MAX) ++d.esp_resets;
        AFR2_DBG("ESP BOOT id=%08lx rst=%u %S fw=%u.%u.%u ext=%u n=%u",
            (unsigned long)d.esp_boot, b[9], esp_reason(b[9]), b[10], b[11], b[12], b[13], d.esp_resets);
        break;
    }
#if AFR2_DEBUG_TAP
    case AFR2_LINK_EVT_FRAME_RX: tap_frame(1u, e->result, e->command, e->data, e->length); break;
    case AFR2_LINK_EVT_FRAME_TX: tap_frame(2u, e->result, e->command, e->data, e->length); break;
#endif
    case AFR2_LINK_EVT_SESSION:
        AFR2_DBG("LINK hello, new session %08lx", (unsigned long)e->value);
        break;
    case AFR2_LINK_EVT_SUPERVISION:
        AFR2_DBG("LINK lost: no ESP request for %lu ms, motion halted", (unsigned long)e->value);
        break;
    case AFR2_LINK_EVT_RX_FAULT:
        /* AFR2-20261108-09: the ESP ROM prints its reset line at 74880 baud; on this 115200 UART it
         * arrives as framing errors right after the ESP went quiet. Only a hint; the cause comes
         * later in the ESP BOOT line (rst=). */
        if ((uint32_t)(now - d.peer_ms) >= ROM_QUIET_MS && (!d.rom_ms || (uint32_t)(now - d.rom_ms) >= ROM_REPEAT_MS)) {
            d.rom_ms = now ? now : 1u;
            AFR2_DBG("ESP ROM probable: noise after %lu ms quiet (ESP reset?)", (unsigned long)(now - d.peer_ms));
        }
        ++d.fault_burst;
        if (d.fault_burst == 1u || (uint32_t)(now - d.fault_ms) >= FAULT_REPEAT_MS) flush_faults(now);
        break;
    case AFR2_LINK_EVT_REJECT:
        if (d.reject_ms && e->command == d.reject_cmd && e->result == d.reject_res &&
            (uint32_t)(now - d.reject_ms) < 60000UL) {
            ++d.reject_repeats;
            if ((uint32_t)(now - d.reject_ms) >= REJECT_REPEAT_MS) { flush_rejects(); d.reject_ms = now; }
        } else {
            flush_rejects();
            d.reject_cmd = e->command; d.reject_res = e->result; d.reject_ms = now ? now : 1u;
            AFR2_DBG("LINK reject cmd=0x%02x %S", e->command, result_name(e->result));
        }
        break;
    default: break;
    }
}

void Afr2Debug_NetEvent(uint8_t kind, uint8_t arg, uint32_t sequence) {
    AFR2_DBG("NET %S arg=%u seq=%lu", net_name(kind), arg, (unsigned long)sequence);
}
void Afr2Debug_NetState(uint8_t mode, uint8_t sta, uint8_t ap, uint8_t clients, uint8_t web, uint32_t esp_boot) {
    uint8_t now[5] = { mode, sta, ap, clients, web };
    if (d.net_valid && d.net_boot == esp_boot && !memcmp(now, d.net, sizeof(now))) return;
    memcpy(d.net, now, sizeof(now)); d.net_valid = 1; d.net_boot = esp_boot;
    AFR2_DBG("NET state mode=%S sta=%u ap=%u clients=%u web=%u", mode_name(mode), sta, ap, clients, web);
}

/* ESP text sniffer. The ESP8266 core prints its crash report on UART0, which is
 * the Mega link: forward printable runs that start right after a newline and
 * end in one. Frame bytes and any binary byte reset and disarm the run, so
 * frame payloads (which carry the AP passphrase) are never printed. Stack-dump
 * rows ("xxxxxxxx:") are counted, not printed. */
static uint8_t peer_is_stack_row(void) {
    if (d.peer_len < 9u) return 0;
    for (uint8_t i = 0; i < 8u; ++i) {
        char c = peer[i];
        if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))) return 0;
    }
    return peer[8] == ':';
}
static void peer_emit(void) {
    uint32_t now = Afr2Board_Millis();
    if ((uint32_t)(now - d.peer_window_ms) >= DBG_PEER_WINDOW_MS) {
        if (d.peer_quiet) { AFR2_DBG("ESP> (%u lines not shown)", d.peer_quiet); d.peer_quiet = 0; }
        d.peer_window_ms = now; d.peer_lines = 0;
    }
    if (d.peer_lines >= DBG_PEER_LINES) { ++d.peer_quiet; return; }
    if (d.peer_hex) { AFR2_DBG("ESP> (%u stack rows omitted)", d.peer_hex); d.peer_hex = 0; }
    ++d.peer_lines; peer[d.peer_len] = 0;
    (void)log_p(PSTR("%S%s"), d.peer_cont ? PSTR("ESP+ ") : PSTR("ESP> "), peer);   /* kept in TAP modes */
}
void Afr2Debug_PeerByte(uint8_t b, bool in_frame) {
    if (!d.ready) return;
    d.peer_ms = Afr2Board_Millis();
    if (in_frame) { d.peer_len = 0; d.peer_cont = 0; d.peer_skip = 0; d.peer_armed = 0; return; }
    if (b >= 0x20u && b < 0x7fu) {
        if (d.peer_skip || !d.peer_armed) return;
        peer[d.peer_len++] = (char)b;
        if (d.peer_len >= DBG_PEER_MAX - 1u) {
            if (!d.peer_cont && peer_is_stack_row()) d.peer_skip = 1;
            else { peer_emit(); d.peer_cont = 1; }
            d.peer_len = 0;
        }
        return;
    }
    uint8_t newline = b == '\n' || b == '\r';
    if (newline && d.peer_armed) {
        if (d.peer_skip || (!d.peer_cont && peer_is_stack_row())) ++d.peer_hex;
        else if (d.peer_cont ? d.peer_len > 0u : d.peer_len >= DBG_PEER_MIN) peer_emit();
    }
    d.peer_len = 0; d.peer_cont = 0; d.peer_skip = 0; d.peer_armed = newline;
}

void Afr2Debug_Task(uint32_t now, const Afr2Link *link) {
    if (!d.ready) return;
    Afr2Usart_Poll(&usart);
#if AFR2_DEBUG_TAP
    tap_control(now);
#endif
    if (d.rotation && (uint32_t)(now - d.rotation_ms) >= 250u) flush_rotation();
    if (d.fault_burst && (uint32_t)(now - d.fault_ms) >= FAULT_REPEAT_MS) flush_faults(now);
    if (d.reject_repeats && (uint32_t)(now - d.reject_ms) >= REJECT_REPEAT_MS) { flush_rejects(); d.reject_ms = now; }
    if ((uint32_t)(now - d.stat_ms) < STAT_MS || !link) return;
    d.stat_ms = now;
    uint16_t requests = (uint16_t)(link->requests - d.seen_requests);
    uint16_t rejects = (uint16_t)(link->rejects - d.seen_rejects);
    uint16_t faults = (uint16_t)(link->faults - d.seen_faults);
    if (log_p(PSTR("STAT neg=%u req=%u rej=%u flt=%u loop<=%ums stack>=%uB lost=%u esp=%08lx"),
            link->negotiated, requests, rejects, faults, d.loop_max, stack_untouched(), d.lost,
            (unsigned long)d.esp_boot)) {
        d.lost = 0;
    }
    d.seen_requests = link->requests; d.seen_rejects = link->rejects; d.seen_faults = link->faults;
    d.loop_max = 0;
    /* AFR2-20261108-09: second line keeps the first under 80 characters. */
#if AFR2_DEBUG_TAP
    if (log_p(PSTR("STAT2 espRst=%u tap=%S tapLost=%u"), d.esp_resets, tap_name(t.mode), t.lost)) t.lost = 0;
#else
    (void)log_p(PSTR("STAT2 espRst=%u"), d.esp_resets);
#endif
}
#else
typedef int afr2_debug_disabled;   /* keeps the translation unit non-empty */
#endif
