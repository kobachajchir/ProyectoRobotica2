/* AFR2-20261028-19. Original AFR2 diagnostic, Koba Chajchir / Codex.
 * Standalone avr-libc image; never linked into the vehicle or ESP bridge.
 * AFR2-20261029-07: one I2C Manager shared by scanner and active OLED test.
 * Source only: not compiled, flashed or physically verified by authoring it.
 */
#include "afr2_board.h"
#include "afr2_twi.h"
#include "afr2_encoder_ui.h"
#include "afr2_oled.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/atomic.h>
#include <stdio.h>

#if !defined(__AVR_ATmega2560__) || F_CPU != 16000000UL
#error "Diagnostic requires ATmega2560 at 16 MHz"
#endif

static Afr2I2cManager manager;
static Afr2Oled oled;
static uint32_t seen_oled_errors;
static uint8_t oled_ready_reported,oled_error_reported,oled_error_status;
/* AFR2-20261029-08: standalone test only, not a product screen or preference. */
static int32_t test_counter;
static uint8_t test_redraw=1;
static uint8_t address7, found, nacks, faults, pending, completed, scanning, rescan;
static Afr2I2cResult completion;
static uint32_t next_ms, attempt_ms;
static uint16_t tx_dropped;
/* AFR2-20261029-09: serial output never waits on hardware in main. */
static uint8_t tx_queue[512];
static uint16_t tx_head,tx_tail;
/* AFR2-20261028-22: retain each address baseline; scans stay periodic but
 * output is event-driven. Raw bus detail distinguishes changed fault states. */
static uint8_t probe_results[112],probe_details[112],probe_statuses[112];
static uint8_t scan_seen,scan_changed;
static uint32_t scan_started_ms;
static uint16_t reported_tx_dropped;
/* AFR2-20261028-20: same IRQ capture/decode/debounce as the vehicle.
 * Aggregate each direction separately; opposite turns must not cancel in logs. */
static Afr2EncoderUi encoder;
static uint16_t enc_a,enc_b,enc_edges,enc_invalid,enc_overflow;
static uint16_t sw_down,sw_up,sw_short,sw_long;
static uint32_t encoder_sample_ms,encoder_report_ms;
static uint8_t encoder_levels;
/* AFR2-20261029-17: isolated servo test. D13/PB7 is OC1C; ModelY S1
 * needs its separate jumper to D13. Hardware PWM, no product Timer5 ISR.
 * At reset no pulses; first detent enables90+/-5degrees. */
static uint8_t servo_angle=90,servo_report_pending;
static uint16_t servo_us;
/* AFR2-20261029-18: read IR1..IR5 directly, without product EEPROM gates. */
static uint8_t ir_raw_levels,ir_seen,ir_pending;
static uint32_t ir_sample_ms,ir_report_ms;
static void ir_init(void){
    uint8_t jtag=MCUCR|_BV(JTD);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE){MCUCR=jtag;MCUCR=jtag;}
    DDRF&=(uint8_t)~0x1f;PORTF&=(uint8_t)~0x1f;DIDR0&=(uint8_t)~0x1f;
}
static void ir_task(uint32_t now){
    if((uint32_t)(now-ir_sample_ms)<5u)return;
    ir_sample_ms=now;uint8_t pins=PINF,levels=0;
    for(uint8_t i=0;i<5;i++)if(pins&(1u<<(4u-i)))levels|=(uint8_t)(1u<<i);
    if(!ir_seen || levels!=ir_raw_levels){ir_raw_levels=levels;ir_seen=1;ir_pending=1;test_redraw=1;}
}
static void servo_init(void) {
    PRR0&=(uint8_t)~_BV(PRTIM1);
    TCCR1A=0;TCCR1B=0;TCCR1C=0;TIMSK1=0;
    DDRB|=_BV(PB7);PORTB&=(uint8_t)~_BV(PB7);
    /* Disable OC0A: PB7 is shared with OC1C's output modulator. */
    TCCR0A&=(uint8_t)~(_BV(COM0A1)|_BV(COM0A0));
    TCNT1=0;ICR1=39999u;OCR1C=3000u;
    TIFR1=_BV(ICF1)|_BV(OCF1A)|_BV(OCF1B)|_BV(OCF1C)|_BV(TOV1);
    /* Fast PWM14, TOP39999, prescaler8: period20ms. OC1A/B disconnected
     * (motor D11/D12). OC1C remains disconnected until user input. */
    TCCR1A=_BV(WGM11);TCCR1B=_BV(WGM13)|_BV(WGM12)|_BV(CS11);
}
static void servo_set(uint8_t angle) {
    if(angle>180)angle=180;
    uint16_t us=1000u+((uint32_t)angle*1000u+90u)/180u;
    if(servo_us==us)return;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        OCR1C=us*2u; /* buffered by hardware, applied at BOTTOM */
        TCCR1A=_BV(WGM11)|_BV(COM1C1);
    }
    servo_angle=angle;servo_us=us;servo_report_pending=1;test_redraw=1;
}
static void servo_stop(void) {
    if(!servo_us)return;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TCCR1A=_BV(WGM11);PORTB&=(uint8_t)~_BV(PB7);
    }
    servo_us=0;servo_report_pending=1;test_redraw=1;
}

void Afr2I2cDiag_Early(void) __attribute__((naked,section(".init3"),used));
void Afr2I2cDiag_Early(void) { MCUSR=0; wdt_disable(); }

static void uart_init(void) {
    /* USART0: RX D0/PE0, TX D1/PE1. USB bridge uses these same signals.
     * 115200 8N1, U2X=1, UBRR=16 => nominal 117647 baud (+2.12%). */
    UCSR0B=0; UBRR0H=0; UBRR0L=16; UCSR0A=_BV(U2X0);
    UCSR0C=_BV(UCSZ01)|_BV(UCSZ00);
    DDRE&=(uint8_t)~_BV(PE0); PORTE&=(uint8_t)~_BV(PE0);
    UCSR0B=_BV(RXEN0)|_BV(TXEN0);
}
static void put(char c) {
    uint16_t next=(tx_head+1u)&511u;
    if(next==tx_tail) {
        if(tx_dropped!=UINT16_MAX)++tx_dropped;
        return;
    }
    tx_queue[tx_head]=(uint8_t)c; tx_head=next;
}
static void uart_task(void) {
    if(tx_tail!=tx_head && (UCSR0A&_BV(UDRE0))) {
        UDR0=tx_queue[tx_tail]; tx_tail=(tx_tail+1u)&511u;
    }
}
static void text_flash(const char *p) {
    char c; while ((c=(char)pgm_read_byte(p++))!=0) put(c);
}
#define SAY(s) text_flash(PSTR(s))
static void hex(uint8_t n) {
    uint8_t h=n>>4,l=n&15u;
    put(h<10?'0'+h:'A'+h-10); put(l<10?'0'+l:'A'+l-10);
}
static void number(uint16_t n) {
    char digits[5]; uint8_t length=0;
    do { digits[length++]=(char)('0'+n%10u); n/=10u; } while(n);
    while(length) put(digits[--length]);
}
static void increment(uint16_t *n) { if(*n!=UINT16_MAX)++*n; }
static void ir_report(uint32_t now){
    if(!ir_pending || (uint32_t)(now-ir_report_ms)<100u)return;
    ir_report_ms=now;ir_pending=0;
    SAY("IR1..5 A4..A0 RAW=");
    for(uint8_t i=0;i<5;i++)put((ir_raw_levels&(1u<<i))?'1':'0');
    SAY(" BLACK_LOW=");
    for(uint8_t i=0;i<5;i++)put((ir_raw_levels&(1u<<i))?'0':'1');
    SAY("\r\n");
}
static void encoder_task(uint32_t now) {
    for(uint8_t budget=32;budget;--budget) {
        uint8_t ab,status=Afr2Board_EncoderRead(&ab);
        if(!status)break;
        if(status==2) {
            increment(&enc_overflow); Afr2EncoderUi_ResetRotation(&encoder,ab); break;
        }
        increment(&enc_edges);
        if((encoder.ab^ab)==3u)increment(&enc_invalid);
        /* Invert only this diagnostic's direction, including serial labels. */
        int8_t rotation=-Afr2EncoderUi_Transition(&encoder,ab);
        if(rotation){
            int16_t angle=(int16_t)servo_angle+(int16_t)rotation*5;
            if(angle<0)angle=0;
            if(angle>180)angle=180;
            servo_set((uint8_t)angle);
        }
        if(rotation>0) {
            increment(&enc_a);
            if(test_counter<INT32_MAX) { ++test_counter; test_redraw=1; }
        } else if(rotation<0) {
            increment(&enc_b);
            if(test_counter>INT32_MIN) { --test_counter; test_redraw=1; }
        }
    }
    if(now==encoder_sample_ms)return;
    encoder_sample_ms=now; encoder_levels=Afr2Board_EncoderLevels();
    bool was_pressed=encoder.stable_pressed;
    Afr2UiInput event=Afr2EncoderUi_Button(&encoder,(encoder_levels&4u)!=0,now);
    if(was_pressed!=encoder.stable_pressed) {
        test_redraw=1;
        if(encoder.stable_pressed)increment(&sw_down); else increment(&sw_up);
    }
    if(event.click)increment(&sw_short);
    if(event.menu)increment(&sw_long); /* diagnostic event, no menu/action */
}
static void encoder_report(uint32_t now) {
    uint32_t elapsed=now-encoder_report_ms;
    uint8_t activity=enc_edges || enc_overflow || sw_down || sw_up || sw_short || sw_long || servo_report_pending;
    if(!activity || elapsed<100u)return;
    encoder_report_ms=now;
    SAY("ENC A(+)="); number(enc_a); SAY(" B(-)="); number(enc_b);
    SAY(" CLK(D2)="); put((encoder_levels&2u)?'1':'0');
    SAY(" DT(D3)="); put((encoder_levels&1u)?'1':'0');
    SAY(" SW(D42)="); put((encoder_levels&4u)?'0':'1');
    if(encoder.stable_pressed)SAY(" PRESIONADO"); else SAY(" SUELTO");
    SAY(" press="); number(sw_down); SAY(" release="); number(sw_up);
    SAY(" CORTO="); number(sw_short); SAY(" LARGO="); number(sw_long);
    SAY(" edges="); number(enc_edges); SAY(" invalid="); number(enc_invalid);
    SAY(" overflow="); number(enc_overflow);
    SAY(" SERVO D13->S1 angle=");number(servo_angle);SAY(" PWMus=");number(servo_us);
    if(!servo_us){SAY(" OFF");}
    SAY("\r\n");servo_report_pending=0;
    enc_a=enc_b=enc_edges=enc_invalid=enc_overflow=0;
    sw_down=sw_up=sw_short=sw_long=0;
}
static void bus(const Afr2TwiDiagnostics *d) {
    SAY(" SDA(D20)="); put(d->sda_high?'1':'0');
    SAY(" SCL(D21)="); put(d->scl_high?'1':'0');
    SAY(" TWSR=0x"); hex(d->status);
    SAY(" recovery="); number(d->recovery);
}
/* Pure drawing of the test snapshot; no GPIO, I2C or navigation here.
 * Full clear erases old digits and the longer PRESIONADO label on release. */
static void render_counter(Afr2Oled *target,int32_t count,bool pressed) {
    char digits[19];
    Afr2Oled_Clear(target);
    Afr2Oled_TextFlash(target,22,0,PSTR("SERVO + IR"),AFR2_WHITE);
    if(servo_us)snprintf_P(digits,sizeof(digits),PSTR("%3ug %4uus D13"),servo_angle,servo_us);
    else snprintf_P(digits,sizeof(digits),PSTR("SERVO: OFF"));
    Afr2Oled_Text(target,8,13,digits,AFR2_WHITE);
    digits[0]='I';digits[1]='R';digits[2]=':';
    for(uint8_t i=0;i<5;i++)digits[3+i]=ir_seen?((ir_raw_levels&(1u<<i))?'1':'0'):'-';
    digits[8]=0;
    Afr2Oled_Text(target,8,26,digits,AFR2_WHITE);
    snprintf_P(digits,sizeof(digits),PSTR("ENC:%ld"),(long)count);
    Afr2Oled_Text(target,8,39,digits,AFR2_WHITE);
    if(pressed)Afr2Oled_TextFlash(target,29,53,PSTR("PRESIONADO"),AFR2_WHITE);
    else Afr2Oled_TextFlash(target,43,53,PSTR("SUELTO"),AFR2_WHITE);
}
static void oled_status(void) {
    if(oled.initialized && !oled_ready_reported) {
        SAY("OLED 0x3C INIT ACK; servo/encoder/button (180deg). Confirm image visually.\r\n");
        oled_ready_reported=1; oled_error_reported=0;
    }
    if(oled.errors!=seen_oled_errors) {
        Afr2TwiDiagnostics d; Afr2Twi_Diagnostics(&d);
        if(!oled_error_reported || oled_ready_reported || d.status!=oled_error_status) {
            SAY("OLED 0x3C TRANSFER ERROR; bounded retry."); bus(&d); SAY("\r\n");
        }
        oled_error_status=d.status; oled_error_reported=1; oled_ready_reported=0;
        seen_oled_errors=oled.errors;
    }
}
static void done(void *ctx,Afr2I2cResult result) {
    (void)ctx; completion=result; completed=1; /* Manager invokes in main. */
}
static void begin_scan(uint32_t now) {
    address7=0x08; found=nacks=faults=0; scanning=1; rescan=0; next_ms=now;
    scan_started_ms=now; scan_changed=!scan_seen;
}
static void report(uint32_t now) {
    Afr2TwiDiagnostics d; Afr2Twi_Diagnostics(&d);
    uint8_t index=address7-0x08u;
    uint8_t detail=(uint8_t)(d.sda_high|(d.scl_high<<1)|(d.recovery<<2));
    uint8_t changed=!scan_seen || probe_results[index]!=(uint8_t)completion ||
        probe_details[index]!=detail || probe_statuses[index]!=d.status;
    uint8_t lost=scan_seen && probe_results[index]==AFR2_I2C_OK && completion!=AFR2_I2C_OK;
    if(completion==AFR2_I2C_OK)++found;
    else if(completion==AFR2_I2C_NACK)++nacks;
    else ++faults;
    if(changed) {
        scan_changed=1;
        /* First scan: suppress the normal empty-address NACK list. Report
         * later changes including disappearance or recovery to normal NACK. */
        if(scan_seen || completion!=AFR2_I2C_NACK) {
            SAY("ADDR7=0x"); hex(address7); SAY(" WRITE=0x"); hex((uint8_t)(address7<<1));
            if(lost)SAY(" LOST");
            if(completion==AFR2_I2C_OK)SAY(" ACK FOUND");
            else if(completion==AFR2_I2C_NACK)SAY(" NACK");
            else if(completion==AFR2_I2C_TIMEOUT)SAY(" TIMEOUT");
            else SAY(" BUS_ERROR");
            bus(&d); SAY("\r\n");
        }
    }
    probe_results[index]=(uint8_t)completion;
    probe_details[index]=detail; probe_statuses[index]=d.status;
    pending=completed=0;
    if(address7==0x77) {
        scanning=0; next_ms=scan_started_ms+10000u;
        if(scan_changed || reported_tx_dropped!=tx_dropped) {
            SAY("END found="); number(found); SAY(" nack="); number(nacks);
            SAY(" faults="); number(faults); SAY(" tx_dropped="); number(tx_dropped);
            SAY("\r\n");
            if(!found && !faults) SAY("No address ACK. Check power, GND, wiring and pull-ups.\r\n");
            if(faults) SAY("Scan has bus faults; absence of device is NOT established.\r\n");
            reported_tx_dropped=tx_dropped;
        }
        scan_seen=1;
    } else { ++address7; next_ms=now+10u; }
}
static void banner(void) {
    SAY("\r\nAFR2 SERVO+IR+ENC DIAG AFR2-20261029-18 | USART0 115200 8N1\r\n");
    SAY("OLED0x3C D20/D21 rotation180; ENC D2/D3 inverted, SW D42 low=pressed.\r\n");
    SAY("Detent=5deg,0..180; boot PWM OFF. First turn enables from90deg. C=center,S=PWM OFF.\r\n");
    SAY("D13/PB7 OC1C:50Hz,1000..2000us. ModelY S1 needs D13 jumper; servo GND/5V/S1.\r\n");
#if AFR2_TWI_INTERNAL_PULLUPS
    SAY("Weak I2C internal pulls ON (Wire comparison).\r\n");
#else
    SAY("I2C internal pulls OFF.\r\n");
#endif
    SAY("IR1..5=A4..A0 digital RAW,0=black. Changes only,max10/s; R=rescan10s early.\r\n");
}
int main(void) {
    cli();
    /* Board_Init sets wheel motor latches inactive. This image intentionally
     * commands ONLY servo D13 on encoder/C input; no vehicle/EEPROM commands. */
    TIMSK0=TIMSK1=TIMSK2=TIMSK3=TIMSK4=TIMSK5=0;
    TCCR0A=TCCR0B=TCCR1A=TCCR1B=TCCR2A=TCCR2B=0;
    TCCR3A=TCCR3B=TCCR4A=TCCR4B=TCCR5A=TCCR5B=0;
    UCSR0B=UCSR1B=UCSR2B=UCSR3B=0; TWCR=0; ADCSRA=0;
    EIMSK=0; PCICR=0;
    Afr2Board_Init();
    servo_init();ir_init();
    Afr2I2cHardware hw; Afr2Twi_Init(&hw);
    uint8_t configured=Afr2I2c_Init(&manager,&hw);
    Afr2Oled_Init(&oled,&manager,0x3c,0);
    Afr2Oled_Settings(&oled,0xcf,1); /* 180deg, equivalent orientation to setRotation(2). */
    Afr2Board_EncoderInit(); encoder_levels=Afr2Board_EncoderLevels();
    Afr2EncoderUi_Init(&encoder,(encoder_levels&2u)!=0,(encoder_levels&1u)!=0,
        (encoder_levels&4u)!=0,0);
    uart_init(); sei(); wdt_enable(WDTO_500MS);
    banner(); /* Shorter than queue capacity: no startup output wait. */
    if(!configured) SAY("FATAL: I2C Manager not configured.\r\n");
    /* Give power/reset settling and a serial monitor time to attach. */
    next_ms=1000u;
    for(;;) {
        uint32_t now=Afr2Board_Millis();
        uart_task();
        encoder_task(now);
        ir_task(now);ir_report(now);
        if(test_redraw) {
            render_counter(&oled,test_counter,encoder.stable_pressed);
            test_redraw=0;
        }
        if(UCSR0A&_BV(RXC0)) {
            uint8_t flags=UCSR0A,c=UDR0;
            if(!(flags&(_BV(FE0)|_BV(DOR0)|_BV(UPE0)))){
                if(c=='r' || c=='R')rescan=1;
                else if(c=='c' || c=='C')servo_set(90);
                else if(c=='s' || c=='S')servo_stop();
            }
        }
        Afr2Twi_Task(now); Afr2I2c_Tick(&manager,now);
        oled_status();
        if(completed) report(now);
        /* Formatting only queues output; sample again before scheduling bus. */
        now=Afr2Board_Millis(); encoder_task(now); encoder_report(now);
        now=Afr2Board_Millis();
        if(configured && !scanning && !manager.busy && (rescan || (int32_t)(now-next_ms)>=0))begin_scan(now);
        if(configured && scanning && !pending && !manager.busy && (int32_t)(now-next_ms)>=0) {
            Afr2I2cResult r=Afr2I2c_Probe(&manager,address7,now,40,done,0);
            if(r==AFR2_I2C_OK) { pending=1; attempt_ms=now; }
            else if(r!=AFR2_I2C_BUSY) { completion=r; completed=1; }
            /* Only backend recovery BUSY is timed here. A legitimate OLED
             * transfer in progress is skipped above, never counted as fault. */
            else if((uint32_t)(now-next_ms)>=100u) {
                completion=AFR2_I2C_TIMEOUT; completed=1;
            }
        }
        /* Manager bounds accepted transfers; retain explicit evidence of a
         * stalled callback instead of silently hanging diagnostic output. */
        if(pending && (uint32_t)(now-attempt_ms)>=100u) {
            SAY("FATAL: Manager completion overdue; reset diagnostic.\r\n");
            for(;;) {} /* Watchdog restarts; do not re-use an owned transfer. */
        }
        /* One probe gets priority when due; in the gaps OLED keeps progressing.
         * Do not freeze display for the entire112-address scan. Submit BUSY
         * preserves OLED phase and snapshots while a probe owns the Manager. */
        if(configured && !pending && !manager.busy) {
            now=Afr2Board_Millis();
            Afr2Oled_Task(&oled,now);
        }
        uart_task();
        wdt_reset();
    }
}
