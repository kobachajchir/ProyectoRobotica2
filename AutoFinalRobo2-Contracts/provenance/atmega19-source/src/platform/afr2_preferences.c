/* AFR2-20261023-19. Own versioned EEPROM journal, two 128-byte records.
 * Explicit LE encoding; CRC16-CCITT; commit marker is written last.
 * No credentials stored here. One byte is scheduled per main-loop pass.
 */
#include "afr2_product.h"
#include "afr2_wire.h"
#include <avr/eeprom.h>
#include <avr/io.h>
#include <string.h>
static uint8_t image[128], active_slot=1, writing, state;
static uint16_t cursor;
static uint32_t sequence;
static uint16_t crc(const uint8_t *b) {
    uint16_t c=0xffff;
    for(uint8_t i=0;i<127;i++)if(i!=14 && i!=15) {
        c^=(uint16_t)b[i]<<8;
        for(uint8_t bit=0;bit<8;bit++)c=(c&0x8000u)?(uint16_t)((c<<1)^0x1021u):(uint16_t)(c<<1);
    }
    return c;
}
static void encode(const Afr2Config *c,uint8_t *b) {
    uint8_t n=0;
#define B(field) b[n++]=c->field
#define W(field) do{afr2_put_u16(b+n,c->field);n+=2;}while(0)
    B(line_kind);B(line_invert);B(line_calibrated);B(motor_signs);B(motor_ready);
    B(hall_wired);B(hall_signs);B(geometry_ready);B(encoder_steps);B(encoder_invert);B(contrast);B(rotation);B(pwm_limit);B(ramp_step);
    for(uint8_t i=0;i<5;i++){afr2_put_u16(b+n,c->line_threshold[i]);n+=2;}
    W(wheel_mm);W(radius_sum_mm);W(speed_limit);W(test_ms);W(kp);W(ki);W(kd);W(line_kp);W(line_speed);
    for(uint8_t i=0;i<4;i++){afr2_put_u32(b+n,c->counts_per_rev[i]);n+=4;}
#undef B
#undef W
}
static void decode(Afr2Config *c,const uint8_t *b) {
    uint8_t n=0;
#define B(field) c->field=b[n++]
#define W(field) do{c->field=afr2_get_u16(b+n);n+=2;}while(0)
    B(line_kind);B(line_invert);B(line_calibrated);B(motor_signs);B(motor_ready);
    B(hall_wired);B(hall_signs);B(geometry_ready);B(encoder_steps);B(encoder_invert);B(contrast);B(rotation);B(pwm_limit);B(ramp_step);
    for(uint8_t i=0;i<5;i++){c->line_threshold[i]=afr2_get_u16(b+n);n+=2;}
    W(wheel_mm);W(radius_sum_mm);W(speed_limit);W(test_ms);W(kp);W(ki);W(kd);W(line_kp);W(line_speed);
    for(uint8_t i=0;i<4;i++){c->counts_per_rev[i]=afr2_get_u32(b+n);n+=4;}
#undef B
#undef W
}
static uint8_t load_slot(uint8_t slot,Afr2Config *c,uint32_t *seq) {
    eeprom_read_block(image,(const void *)(uintptr_t)((uint16_t)slot*128u),128);
    if(memcmp(image,"AFR2",4) || image[4]!=AFR2_CONFIG_VERSION ||
        afr2_get_u16(image+6)!=58u || image[127]!=0xa5 || crc(image)!=afr2_get_u16(image+14))return 0;
    decode(c,image+16); if(!Afr2Product_ConfigValid(c))return 0;
    *seq=afr2_get_u32(image+8); return 1;
}
void Afr2Prefs_Load(Afr2Config *c,uint8_t *loaded) {
    Afr2Config candidate; uint32_t a=0,b=0;
    uint8_t va=load_slot(0,c,&a), vb=load_slot(1,&candidate,&b);
    if(vb && (!va || (int32_t)(b-a)>0)){*c=candidate;active_slot=1;sequence=b;}
    else if(va){active_slot=0;sequence=a;}
    else {Afr2Product_Defaults(c);sequence=0;active_slot=1;}
    *loaded=va||vb; writing=0;state=0;
}
uint8_t Afr2Prefs_Save(const Afr2Config *c) {
    if(writing || !Afr2Product_ConfigValid(c))return 0;
    memset(image,0,sizeof(image));memcpy(image,"AFR2",4);image[4]=AFR2_CONFIG_VERSION;
    afr2_put_u16(image+6,58);afr2_put_u32(image+8,sequence+1u);encode(c,image+16);
    afr2_put_u16(image+14,crc(image));image[127]=0xa5;
    cursor=0;writing=1;state=1;return 1;
}
uint8_t Afr2Prefs_Task(void) {
    if(!writing || (EECR&_BV(EEPE)))return state;
    uint16_t base=(uint16_t)(active_slot^1u)*128u;
    if(cursor==0)eeprom_write_byte((uint8_t *)(uintptr_t)(base+127u),0);
    else if(cursor<=127)eeprom_update_byte((uint8_t *)(uintptr_t)(base+cursor-1u),image[cursor-1u]);
    else if(cursor==128)eeprom_write_byte((uint8_t *)(uintptr_t)(base+127u),0xa5);
    else {active_slot^=1u;++sequence;writing=0;state=2;}
    ++cursor;return state;
}
