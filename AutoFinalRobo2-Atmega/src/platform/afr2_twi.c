/* AFR2-20261023-15. New avr-libc TWI backend, no HAL/DMA or busy waits.
 * Register/status semantics: Microchip ATmega2560 datasheet, section 24.
 * Manager owns transfers and callbacks. ISR advances at most one wire byte.
 */
#include "afr2_twi.h"
#include "board_pinmap.h"
#include <avr/interrupt.h>
#include <util/atomic.h>
#include <util/twi.h>
#if F_CPU != 16000000UL
#error "Recalculate TWI bitrate for this clock"
#endif
#define SDA _BV(AFR2_OLED_SDA_BIT)
#define SCL _BV(AFR2_OLED_SCL_BIT)
#define BUS (SDA|SCL)
enum { IDLE, ACTIVE, STOPPING, DONE, REC_BEGIN, REC_WAIT, REC_LOW,
       REC_HIGH, REC_STOP_LOW, REC_STOP_HIGH, REC_RELEASE, FAILED };
static volatile uint8_t phase, result, address;
static const uint8_t * volatile bytes;
static volatile uint16_t size, position;
static uint8_t pulses;
static uint32_t recovery_ms, step_ms;
/* Open drain: only drive low or release. Never drive either line high. */
static void release(uint8_t pins) { DDRD&=(uint8_t)~pins; }
static void low(uint8_t pins) { PORTD&=(uint8_t)~pins; DDRD|=pins; }
static void ready(void) {
    release(BUS); PORTD&=(uint8_t)~BUS;
    TWSR=0; TWBR=72; /* 16 MHz / (16+2*72) = 100 kHz, prescaler 1. */
    TWCR=_BV(TWEN); phase=IDLE;
}
static void fail_recovery(void) { TWCR=0; release(BUS); phase=FAILED; }
static void stop_transfer(Afr2I2cResult status) {
    result=(uint8_t)status;
    /* STOP has no interrupt. poll() waits for TWSTO to clear. */
    phase=STOPPING; TWCR=_BV(TWINT)|_BV(TWEN)|_BV(TWSTO);
}
static Afr2I2cResult start(void *ctx,uint8_t addr,const uint8_t *data,uint16_t n) {
    Afr2I2cResult r=AFR2_I2C_BUSY; (void)ctx;
    if (!data || !n || addr>0x7f) return AFR2_I2C_ERROR;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (phase==FAILED) { phase=REC_BEGIN; r=AFR2_I2C_ERROR; }
        else if (phase==IDLE) {
            if ((PIND&BUS)!=BUS) { TWCR=0; phase=REC_BEGIN; r=AFR2_I2C_ERROR; }
            else {
                bytes=data; size=n; position=0; address=(uint8_t)(addr<<1);
                result=AFR2_I2C_BUSY; phase=ACTIVE;
                TWCR=_BV(TWINT)|_BV(TWEN)|_BV(TWIE)|_BV(TWSTA);
                r=AFR2_I2C_OK;
            }
        }
    }
    return r;
}
static Afr2I2cResult poll(void *ctx) {
    Afr2I2cResult r=AFR2_I2C_BUSY; (void)ctx;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        if (phase==STOPPING && !(TWCR&_BV(TWSTO))) phase=DONE;
        if (phase==DONE) {
            r=(Afr2I2cResult)result; bytes=0; size=position=0; phase=IDLE;
        }
    }
    return r;
}
static void abort_transfer(void *ctx) {
    (void)ctx;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
        TWCR=0; /* No ISR/hardware can dereference the borrowed buffer now. */
        bytes=0; size=position=0; release(BUS); phase=REC_BEGIN;
    }
}
void Afr2Twi_Init(Afr2I2cHardware *hardware) {
    TWCR=0; bytes=0; size=position=0; result=AFR2_I2C_OK;
    ready();
    hardware->ctx=0; hardware->start=start; hardware->poll=poll; hardware->abort=abort_transfer;
}
ISR(TWI_vect) {
    if (phase!=ACTIVE) { TWCR=0; return; }
    switch (TW_STATUS) {
    case TW_START: case TW_REP_START:
        TWDR=address; TWCR=_BV(TWINT)|_BV(TWEN)|_BV(TWIE); break;
    case TW_MT_SLA_ACK: case TW_MT_DATA_ACK:
        if (position<size) {
            TWDR=bytes[position++]; TWCR=_BV(TWINT)|_BV(TWEN)|_BV(TWIE);
        } else stop_transfer(AFR2_I2C_OK);
        break;
    case TW_MT_ARB_LOST:
        /* No STOP after arbitration loss: the bus is no longer ours. */
        result=AFR2_I2C_ERROR; phase=DONE; TWCR=_BV(TWINT)|_BV(TWEN); break;
    case TW_MT_SLA_NACK: case TW_MT_DATA_NACK: case TW_BUS_ERROR:
    default: stop_transfer(AFR2_I2C_ERROR); break;
    }
}
void Afr2Twi_Task(uint32_t now) {
    if (phase<REC_BEGIN || phase==FAILED) return;
    if (phase==REC_BEGIN) {
        TWCR=0; release(BUS); PORTD&=(uint8_t)~BUS;
        recovery_ms=step_ms=now; pulses=0; phase=REC_WAIT;
    }
    if ((uint32_t)(now-recovery_ms)>=50u) { fail_recovery(); return; }
    if ((uint32_t)(now-step_ms)<1u) return;
    switch (phase) {
    case REC_WAIT:
        if (!(PIND&SCL)) break;
        if (PIND&SDA) { ready(); return; }
        low(SCL); phase=REC_LOW; step_ms=now; break;
    case REC_LOW:
        release(SCL); phase=REC_HIGH; step_ms=now; break;
    case REC_HIGH:
        if (!(PIND&SCL)) break; /* clock stretching bounded by total deadline */
        if (++pulses>=9u || (PIND&SDA)) {
            low(SCL); low(SDA); phase=REC_STOP_LOW;
        } else { low(SCL); phase=REC_LOW; }
        step_ms=now; break;
    case REC_STOP_LOW:
        release(SCL); phase=REC_STOP_HIGH; step_ms=now; break;
    case REC_STOP_HIGH:
        if (!(PIND&SCL)) break;
        release(SDA); phase=REC_RELEASE; step_ms=now; break;
    case REC_RELEASE:
        if ((PIND&BUS)==BUS) ready(); else fail_recovery();
        break;
    default: fail_recovery(); break;
    }
}
