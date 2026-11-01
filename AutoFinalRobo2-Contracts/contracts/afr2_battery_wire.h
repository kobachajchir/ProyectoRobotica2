/* AFR2-20261029-26. Battery divider 10k/10k on ADC5; EX01v9. */
#ifndef AFR2_BATTERY_WIRE_H
#define AFR2_BATTERY_WIRE_H
#include "afr2_wire.h"
#define AFR2_EXT_BATTERY 0x55u
#define AFR2_EXT_F_BATTERY 0x40000UL
#define AFR2_BATTERY_VREF_MV 5000u /* nominal AVCC, not calibrated */
#define AFR2_BATTERY_STALE_MS 2000u
enum { AFR2_BATTERY_OFF, AFR2_BATTERY_PENDING, AFR2_BATTERY_VALID,
       AFR2_BATTERY_RANGE, AFR2_BATTERY_STALE };
/* Request: one byte 0=read cached, 1=start/restart acquisition. Reply 23 bytes. */
static inline uint8_t afr2_battery_reply_valid(const uint8_t *p,uint8_t n) {
    if(n!=23 || !afr2_get_u32(p) || p[12]>AFR2_BATTERY_STALE ||
       afr2_get_u16(p+13)>1023 || afr2_get_u16(p+17)!=AFR2_BATTERY_VREF_MV ||
       p[19]!=2 || p[20]!=1 || p[21]!=0 || p[22]!=5)return 0;
    uint32_t seq=afr2_get_u32(p+4),age=afr2_get_u32(p+8);
    uint16_t raw=afr2_get_u16(p+13),mv=afr2_get_u16(p+15);
    if(p[12]<=AFR2_BATTERY_PENDING)return !seq && age==AFR2_AGE_UNKNOWN && !raw && !mv;
    if(!seq || age==AFR2_AGE_UNKNOWN || mv!=((uint32_t)raw*10000u+512u)/1024u)return 0;
    return p[12]==AFR2_BATTERY_STALE?age>AFR2_BATTERY_STALE_MS:
        age<=AFR2_BATTERY_STALE_MS && (p[12]!=AFR2_BATTERY_VALID || raw<1023);
}
#endif
