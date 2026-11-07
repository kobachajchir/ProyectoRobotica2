/* AFR2-20261106-04. Host regression: exact pixels and ACK lifecycle, no hardware. */
#include "afr2_screens.h"
#include "afr2_qr_screen.h"
#include "afr2_product.h"
#include <avr/pgmspace.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
_Static_assert(sizeof(((Afr2Oled *)0)->buffer)==1024 && sizeof(((Afr2Oled *)0)->acknowledged)==1024,"Both OLED buffers must remain");
static struct {const uint8_t *live;uint8_t sent[33],panel[1024],page,column;uint16_t n;uint8_t fail;} hw;
static Afr2I2cResult start(void *ctx,uint8_t addr,const uint8_t *data,uint16_t n){
 (void)ctx;(void)addr;assert(n<=33);hw.live=data;hw.n=n;memcpy(hw.sent,data,n);return AFR2_I2C_OK;
}
static Afr2I2cResult poll(void *ctx){
 (void)ctx;assert(!memcmp(hw.live,hw.sent,hw.n));
 if(hw.fail){hw.fail=0;return AFR2_I2C_ERROR;}
 if(hw.sent[0]==0){assert(hw.n==4);hw.page=hw.sent[1]&7;hw.column=(hw.sent[2]&15)|((hw.sent[3]&15)<<4);}
 else {assert(hw.sent[0]==0x40);memcpy(hw.panel+hw.page*128+hw.column,hw.sent+1,hw.n-1);}
 return AFR2_I2C_OK;
}
static void abort_tx(void *ctx){(void)ctx;}
static void drain(Afr2Oled *o,Afr2I2cManager *m){
 for(uint32_t t=4;t<4000 && (m->busy || o->dirty);t++){Afr2I2c_Tick(m,t);Afr2Oled_Task(o,t);}
 assert(!m->busy && !o->dirty);assert(!memcmp(o->buffer,o->acknowledged,1024));assert(!memcmp(o->buffer,hw.panel,1024));
}
static void transfer_case(uint8_t fail,uint8_t data_phase){
 Afr2Oled o;Afr2I2cManager m;Afr2I2cHardware hardware={0,start,poll,abort_tx};memset(&hw,0,sizeof(hw));
 assert(Afr2I2c_Init(&m,&hardware));Afr2Oled_Init(&o,&m,0x3c,0);o.initialized=1;o.configured_once=1;o.settings_dirty=0;o.retry_ms=0;o.dirty=0;memset(o.valid,255,8);
 Afr2Oled_Pixel(&o,4,1,AFR2_WHITE);Afr2Oled_Task(&o,0);assert(m.busy);if(data_phase){Afr2I2c_Tick(&m,1);Afr2Oled_Task(&o,2);}assert(m.busy && hw.live==(data_phase?o.tx:o.command));
 uint8_t tx[33],command[6];memcpy(tx,o.tx,33);memcpy(command,o.command,6);
 assert(Afr2Qr_DrawPayload(&o,(const uint8_t *)"http://192.0.2.1/",17));
 assert(!memcmp(tx,o.tx,33) && !memcmp(command,o.command,6));assert(o.valid[0]==0 && o.valid[1]==0 && o.valid[2]==128);
 for(int i=0;i<353;i++)assert(o.acknowledged[i]==0);
 hw.fail=fail;Afr2I2c_Tick(&m,3);
 if(!fail && data_phase){assert(o.acknowledged[4]==2);assert(o.valid[0]&1);}else assert(!(o.valid[0]&1));
 drain(&o,&m);
 uint8_t before[8];memcpy(before,o.valid,8);assert(!Afr2Oled_BorrowScratch(&o,0));assert(!Afr2Oled_BorrowScratch(&o,1025));assert(!memcmp(before,o.valid,8));
}
int main(void){
 Afr2Oled a={0},b={0};const char *text="Texto largo con espacios y 0123456789";
 for(int shift=0;shift<140;shift+=13){Afr2Oled_Clear(&a);Afr2Oled_Clear(&b);Afr2Oled_TextWindow(&a,3,104,47,text,shift,AFR2_WHITE);Afr2Oled_TextWindowFlash(&b,3,104,47,PSTR("Texto largo con espacios y 0123456789"),shift,AFR2_WHITE);assert(!memcmp(a.buffer,b.buffer,1024));}
 Afr2Ui u;Afr2Ui_Init(&u);Afr2Ui_NotifyFlash(&u,1,PSTR("Titulo largo de prueba"),PSTR("Texto largo de prueba"),2500);assert(u.notices==1);Afr2Ui_Notify(&u,1,"Titulo largo de prueba","Texto largo de prueba",2500);assert(u.notices==1);assert(strlen(u.notification[0].title)==17 && strlen(u.notification[0].text)==17);
 char dynamic[]="Mensaje variable";Afr2Ui_NotifyFlashTitle(&u,0,PSTR("RAM"),dynamic,1000);dynamic[0]='X';assert(!strcmp(u.notification[1].text,"Mensaje variable"));
 for(uint32_t t=0;t<10000;t+=37)assert(Afr2Ui_TextOffset(text,104,t)==Afr2Ui_TextOffsetFlash(PSTR("Texto largo con espacios y 0123456789"),104,t));
 transfer_case(0,0);transfer_case(1,0);transfer_case(0,1);transfer_case(1,1);puts("PASS: flash/RAM text and notices; QR scratch during in-flight command/data ACK and error, panel resynchronization");return 0;
}
