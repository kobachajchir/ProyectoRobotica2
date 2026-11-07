/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
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
    B(radar_range);B(radar_trace);W(long_press_ms);B(servo_inverted);B(hall_cw_mask);
#undef B
#undef W
}
static void decode(Afr2Config *c,const uint8_t *b,uint8_t version) {
    uint8_t n=0;
#define B(field) c->field=b[n++]
#define W(field) do{c->field=afr2_get_u16(b+n);n+=2;}while(0)
    B(line_kind);B(line_invert);B(line_calibrated);B(motor_signs);B(motor_ready);
    B(hall_wired);B(hall_signs);B(geometry_ready);B(encoder_steps);B(encoder_invert);B(contrast);B(rotation);B(pwm_limit);B(ramp_step);
    for(uint8_t i=0;i<5;i++){c->line_threshold[i]=afr2_get_u16(b+n);n+=2;}
    W(wheel_mm);W(radius_sum_mm);W(speed_limit);W(test_ms);W(kp);W(ki);W(kd);W(line_kp);W(line_speed);
    for(uint8_t i=0;i<4;i++){c->counts_per_rev[i]=afr2_get_u32(b+n);n+=4;}
    c->radar_range=0;c->radar_trace=1;
    if(version>=5u){B(radar_range);B(radar_trace);}
    c->long_press_ms=800;c->servo_inverted=0;
    if(version>=8u){W(long_press_ms);B(servo_inverted);}
    c->hall_cw_mask=0;if(version>=10u){B(hall_cw_mask);}
    c->wheel_mm=AFR2_FIXED_WHEEL_MM;c->radius_sum_mm=AFR2_FIXED_RADIUS_SUM_MM;c->geometry_ready=1;c->hall_wired=1;
#undef B
#undef W
}
static uint8_t load_slot(uint8_t slot,Afr2Config *c,uint32_t *seq) {
    eeprom_read_block(image,(const void *)(uintptr_t)((uint16_t)slot*128u),128);
    if(memcmp(image,"AFR2",4) || image[4]<1u || image[4]>AFR2_CONFIG_VERSION ||
        afr2_get_u16(image+6)!=(image[4]>=10u?64u:image[4]>=8u?63u:image[4]>=5u?60u:58u) || image[127]!=0xa5 || crc(image)!=afr2_get_u16(image+14))return 0;
    decode(c,image+16,image[4]);
    /* v9 product is digital only; migrate before validating the old enum. */
    if(image[4]<9u && c->line_kind==2u){c->line_kind=1;c->line_calibrated=0;}
    if(!Afr2Product_ConfigValid(c))return 0;
    /* AFR2-20261029-10: same58-byte payload, migrate valid v1 UI fields only.
     * Do not discard wheel/Hall/motor/line calibration or auto-write EEPROM.
     * v2 records retain subsequent intentional user orientation adjustments. */
    if(image[4]==1u) { c->encoder_invert=1;c->rotation=1;c->encoder_steps=4; }
    /* AFR2-20261029-15: old EEPROM may still mark the installed Hall absent.
     * Apply confirmed wiring only; retain CPR, geometry, motor flags and
     * polarities. Future intentional v3 Hall disable remains respected. */
    if(image[4]<3u)c->hall_wired=1;
    /* AFR2-20261106-01: obsolete ADC/default-enable migrations removed.
     * Digital/off selections and every unrelated saved setting survive. */
    /* AFR2-20261030-04: replace legacy default80 with requested255 in RAM.
     * Preserve other configured limits and all calibration. No auto-save;
     * v7 records respect any subsequent explicit user limit, including80. */
    if(image[4]<7u && c->pwm_limit==80u)c->pwm_limit=255u;
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
    afr2_put_u16(image+6,64);afr2_put_u32(image+8,sequence+1u);encode(c,image+16);
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
