/* Derivado de conceptos SSD1306 F4: paginas verticales 128x64, wrappers separados.
 * API y transporte nuevos, sin DMA/HAL. No acredita paridad con todos sus renderers.
 */
#include "afr2_oled.h"
#include <string.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#define FLASH PROGMEM
#define READ_BYTE(p) pgm_read_byte(p)
#else
#define FLASH
#define READ_BYTE(p) (*(const uint8_t *)(p))
#endif
enum { IDLE, WAIT_INIT, WAIT_COMMAND, SEND_DATA, WAIT_DATA, WAIT_SETTINGS, WAIT_REFRESH };
/* AFR2-20261029-10: promote the user-confirmed diagnostic's settle time.
 * Page addressing is deliberately retained with this driver's page writes. */
#ifndef AFR2_OLED_STARTUP_MS
#define AFR2_OLED_STARTUP_MS 500u
#endif
/* AFR2-20261105-33: motor noise can corrupt a command byte (orientation,
 * start line, contrast) or a pixel while the panel still ACKs. Resend the
 * configuration periodically and rewrite one chunk per period: 64 chunks
 * x 40 ms = full panel RAM repaired every ~2.6 s, ~0.5 kB/s of bus. */
#ifndef AFR2_OLED_REFRESH_MS
#define AFR2_OLED_REFRESH_MS 1000u
#endif
#ifndef AFR2_OLED_SCRUB_MS
#define AFR2_OLED_SCRUB_MS 40u
#endif
static const uint8_t init_commands[] FLASH={
  0x00,0xae,0xd5,0x80,0xa8,0x3f,0xd3,0x00,0x40,0x8d,0x14,
  0x20,0x02,0xa1,0xc8,0xda,0x12,0x81,0xcf,0xd9,0xf1,0xdb,0x40,0xa4,0xa6,0x2e,0xaf
};
/* Positions in init_commands patched with the current settings. */
enum { INIT_SEGMENT=13, INIT_COM=14, INIT_CONTRAST=18 };
/* AFR2-20261105-33: init/recovery/refresh use the CURRENT rotation and
 * contrast. A fixed 0xA1/0xC8 flipped the image on every recovery while
 * rotation=1 until the separate settings command arrived. */
static uint8_t load_config(Afr2Oled *o,uint8_t skip) {
    uint8_t length=sizeof(init_commands)-skip;o->tx[0]=0;
    for(uint8_t i=1;i<length;i++)o->tx[i]=READ_BYTE(&init_commands[i+skip]);
    o->tx[INIT_SEGMENT-skip]=o->rotation?0xa0:0xa1;
    o->tx[INIT_COM-skip]=o->rotation?0xc0:0xc8;
    o->tx[INIT_CONTRAST-skip]=o->contrast;
    return length;
}
static void completed(void *ctx,Afr2I2cResult result) {
    Afr2Oled *o=ctx;
    if (result!=AFR2_I2C_OK) {
        /* AFR2-20261029-14: ordinary transfer faults must NOT issue DISPLAY
         * OFF and restart all64chunks. TWI Manager already starts bus recovery.
         * Invalidate only the uncertain chunk; retain other ACKed pages. */
        o->errors++;o->last_error=(uint8_t)result;o->last_failed_phase=o->phase;
        if(o->consecutive_failures<3u)++o->consecutive_failures;
        if(o->phase==WAIT_INIT) {
            o->initialized=false;o->dirty=0xff;
            memset(o->valid,0,sizeof(o->valid));
        } else if(o->phase==WAIT_SETTINGS) {
            o->settings_dirty=1;
        } else if(o->phase==WAIT_REFRESH) {
            o->refresh_ms=o->clock_ms; /* retry after the common 50 ms gap */
        } else if(o->phase==WAIT_COMMAND || o->phase==WAIT_DATA) {
            o->valid[o->page]&=(uint8_t)~(1u<<o->chunk);
            o->dirty|=(uint8_t)(1u<<o->page);
        }
        /* A panel power interruption can lose configuration, too. Persistent
         * failure escalates to configuration/RAM resync WITHOUT DISPLAY OFF. */
        if(o->consecutive_failures>=3u) {
            o->initialized=false;o->dirty=0xff;
            memset(o->valid,0,sizeof(o->valid));
        }
        o->phase=IDLE;o->retry_ms=o->clock_ms+50u;return;
    }
    if (o->phase==WAIT_INIT) {
        o->initialized=true;o->configured_once=true;o->consecutive_failures=0;
        o->dirty=0xff; o->settings_dirty=1;
        memset(o->valid,0,sizeof(o->valid)); o->phase=IDLE;
    }
    else if (o->phase==WAIT_COMMAND) o->phase=SEND_DATA;
    else {
        if(o->phase==WAIT_DATA) {
            o->last_transfer_ms=(uint16_t)(o->clock_ms-o->submitted_ms);
            /* Commit only bytes actually ACKed, never the current mutable
             * render buffer. Redraws during TX remain detectable next time. */
            memcpy(&o->acknowledged[(uint16_t)o->page*128u+o->column],&o->tx[1],o->count);
            o->valid[o->page]|=(uint8_t)(1u<<o->chunk);
            o->consecutive_failures=0;
        }
        if(o->phase==WAIT_SETTINGS || o->phase==WAIT_REFRESH)o->consecutive_failures=0;
        o->phase=IDLE;
    }
    /* AFR2-20261029-16: callback records progress only. Main executes services
     * between bounded stages; adding a timed gap needlessly slows every block. */
}
void Afr2Oled_Init(Afr2Oled *o,Afr2I2cManager *m,uint8_t address7,uint32_t now) {
    if (!o) return;
    memset(o,0,sizeof(*o)); o->manager=m; o->address7=address7;
    o->dirty=0xff; o->retry_ms=now+AFR2_OLED_STARTUP_MS; o->clock_ms=now;o->contrast=0xcf;o->settings_dirty=1;
    o->refresh_ms=o->retry_ms+AFR2_OLED_REFRESH_MS;o->scrub_ms=o->retry_ms;
}
static bool send(Afr2Oled *o,const uint8_t *data,uint16_t size,uint8_t next,uint32_t now) {
    /* Publish attempted stage BEFORE Submit so immediate start failures are
     * classified just like deferred failures. BUSY restores the saved phase. */
    uint8_t previous=o->phase;o->phase=next;
    Afr2I2cResult r=Afr2I2c_Submit(o->manager,o->address7,data,size,now,40,completed,o);
    if (r==AFR2_I2C_OK){o->submitted_ms=now;return true;}
    if (r==AFR2_I2C_BUSY)o->phase=previous;
    else completed(o,r);
    return false;
}
void Afr2Oled_Settings(Afr2Oled *o,uint8_t contrast,uint8_t rotation) {
    rotation=rotation!=0;
    if(o->contrast!=contrast || o->rotation!=rotation){
        /* Segment remap only affects later writes: after a rotation change
         * the whole RAM must be rewritten or old columns stay mirrored. */
        if(o->rotation!=rotation){memset(o->valid,0,sizeof(o->valid));o->dirty=0xff;}
        o->contrast=contrast;o->rotation=rotation;o->settings_dirty=1;
    }
}
void Afr2Oled_Pause(Afr2Oled *o,bool paused) { if(o)o->paused=paused; }
void Afr2Oled_Task(Afr2Oled *o,uint32_t now) {
    if (!o || !o->manager) return;
    o->clock_ms=now;
    if (o->paused || (int32_t)(now-o->retry_ms)<0) return;
    if (o->phase==SEND_DATA) { (void)send(o,o->tx,(uint16_t)o->count+1u,WAIT_DATA,now); return; }
    if (o->phase!=IDLE || o->manager->busy) return;
    if (!o->initialized) {
        /* Only cold startup sends0xAE. A recovery reapplies configuration
         * and0xAF without deliberately blanking an already running panel. */
        uint8_t length=load_config(o,o->configured_once?1u:0u);
        (void)send(o,o->tx,length,WAIT_INIT,now); return;
    }
    if(o->settings_dirty) {
        o->command[0]=0;o->command[1]=0x81;o->command[2]=o->contrast;
        o->command[3]=o->rotation?0xa0:0xa1;o->command[4]=o->rotation?0xc0:0xc8;
        if(send(o,o->command,5,WAIT_SETTINGS,now))o->settings_dirty=0;
        return;
    }
    /* AFR2-20261105-33: periodic config refresh, never 0xAE (no blanking).
     * Runs only between page writes, so the page/column pointer is free. */
    if((int32_t)(now-o->refresh_ms)>=0) {
        uint8_t length=load_config(o,1u);
        if(send(o,o->tx,length,WAIT_REFRESH,now))o->refresh_ms=now+AFR2_OLED_REFRESH_MS;
        return;
    }
    /* Scrub: forget one ACKed chunk so it is resent even if unchanged. */
    if((uint32_t)(now-o->scrub_ms)>=AFR2_OLED_SCRUB_MS) {
        uint8_t page=o->scrub>>3;
        o->scrub_ms=now;o->valid[page]&=(uint8_t)~(1u<<(o->scrub&7u));
        o->dirty|=(uint8_t)(1u<<page);o->scrub=(uint8_t)((o->scrub+1u)&63u);
    }
    /* At most one page comparison and one submission per main-loop call.
     * Round-robin pages AND chunks avoid starving the bottom of the screen. */
    for (uint8_t visited=0;visited<8;visited++) {
        uint8_t page=(o->next_page+visited)&7u;
        if (!(o->dirty&(1u<<page))) continue;
        for(uint8_t n=0;n<8;n++) {
            uint8_t chunk=(o->next_chunk[page]+n)&7u;
            uint8_t left=chunk*AFR2_OLED_CHUNK,right=left+AFR2_OLED_CHUNK;
            uint16_t base=(uint16_t)page*128u;
            if(o->valid[page]&(1u<<chunk)) {
                while(left<right && o->buffer[base+left]==o->acknowledged[base+left])++left;
                if(left==right)continue;
                while(right>left && o->buffer[base+right-1u]==o->acknowledged[base+right-1u])--right;
            }
            o->page=page;o->chunk=chunk;o->column=left;o->count=right-left;
            o->command[0]=0;o->command[1]=(uint8_t)(0xb0u+page);
            o->command[2]=left&0x0fu;o->command[3]=0x10u|(left>>4);
            o->tx[0]=0x40;memcpy(&o->tx[1],&o->buffer[base+left],o->count);
            if(send(o,o->command,4,WAIT_COMMAND,now)) {
                o->next_page=(page+1u)&7u;o->next_chunk[page]=(chunk+1u)&7u;
            }
            return;
        }
        /* Clear/redraw with identical final pixels generates no bus traffic. */
        o->dirty&=(uint8_t)~(1u<<page);o->next_page=(page+1u)&7u;
        return;
    }
}
/* AFR2-20261106-04: synchronous main-loop scratch loan from ACK shadow.
 * All intersected chunks are invalidated BEFORE the pointer is returned.
 * Consume/wipe before yielding to I2c_Tick/Oled_Task or another renderer.
 * Does not touch live framebuffer, in-flight tx/command, or add a buffer. */
uint8_t *Afr2Oled_BorrowScratch(Afr2Oled *o,uint16_t bytes) {
    if(!o || !bytes || bytes>AFR2_OLED_BYTES)return NULL;
    uint8_t chunks=(bytes+AFR2_OLED_CHUNK-1u)/AFR2_OLED_CHUNK;
    for(uint8_t i=0;i<chunks;i++) {
        o->valid[i/8u]&=(uint8_t)~(1u<<(i%8u));
        o->dirty|=(uint8_t)(1u<<(i/8u));
    }
    return o->acknowledged;
}
void Afr2Oled_Clear(Afr2Oled *o) {
    if (!o) return;
    for (uint16_t i=0;i<AFR2_OLED_BYTES;i++) if(o->buffer[i]) {
        o->buffer[i]=0; o->dirty|=(uint8_t)(1u<<(i/128u));
    }
}
void Afr2Oled_Pixel(Afr2Oled *o,int16_t x,int16_t y,Afr2Color color) {
    if(!o || x<0 || y<0 || x>=128 || y>=64) return;
    uint16_t index=(uint16_t)x+((uint16_t)y/8u)*128u;
    uint8_t old=o->buffer[index], mask=(uint8_t)(1u<<(y&7));
    uint8_t val=color==AFR2_WHITE?(old|mask):color==AFR2_INVERT?(old^mask):(old&(uint8_t)~mask);
    if(val!=old){o->buffer[index]=val;o->dirty|=(uint8_t)(1u<<(y/8));}
}
void Afr2Oled_Rect(Afr2Oled *o,int16_t x,int16_t y,uint8_t w,uint8_t h,Afr2Color color) {
    if (!w || !h) return;
    for(uint8_t i=0;i<w;i++){Afr2Oled_Pixel(o,x+i,y,color);if(h>1)Afr2Oled_Pixel(o,x+i,y+h-1,color);}
    for(uint8_t i=1;i+1<h;i++){Afr2Oled_Pixel(o,x,y+i,color);if(w>1)Afr2Oled_Pixel(o,x+w-1,y+i,color);}
}
void Afr2Oled_FillRect(Afr2Oled *o,int16_t x,int16_t y,uint8_t w,uint8_t h,Afr2Color color) {
    int16_t left=x<0?0:x,top=y<0?0:y,right=x+w,bottom=y+h;
    if(right>128)right=128;
    if(bottom>64)bottom=64;
    for(int16_t py=top;py<bottom;py++)for(int16_t px=left;px<right;px++)Afr2Oled_Pixel(o,px,py,color);
}
void Afr2Oled_Char(Afr2Oled *o,int16_t x,int16_t y,char ch,Afr2Color color) {
    for(uint8_t row=0;row<10;row++) {
        uint8_t bits=Afr2Font7x10_Row((uint8_t)ch,row);
        for(uint8_t col=0;col<7;col++) if(bits&(0x80u>>col)) Afr2Oled_Pixel(o,x+col,y+row,color);
    }
}
void Afr2Oled_Text(Afr2Oled *o,int16_t x,int16_t y,const char *s,Afr2Color color) {
    if(!s) return;
    while(*s && x<128){Afr2Oled_Char(o,x,y,*s++,color);x+=7;}
}
void Afr2Oled_TextWindow(Afr2Oled *o,uint8_t left,uint8_t right,int16_t y,const char *s,uint16_t offset,Afr2Color color) {
    if(!s || left>=right || right>128)return;
    int16_t x=(int16_t)left-(int16_t)offset;
    while(*s && x<right) {
        char ch=*s++;
        if(x+7>left)for(uint8_t row=0;row<10;row++) {
            uint8_t bits=Afr2Font7x10_Row((uint8_t)ch,row);
            for(uint8_t col=0;col<7;col++)if(x+col>=left && x+col<right && (bits&(0x80u>>col)))
                Afr2Oled_Pixel(o,x+col,y+row,color);
        }
        x+=7;
    }
}
/* AFR2-20261106-04: identical clipping, direct PROGMEM read. */
void Afr2Oled_TextWindowFlash(Afr2Oled *o,uint8_t left,uint8_t right,int16_t y,const char *s,uint16_t offset,Afr2Color color) {
    if(!s || left>=right || right>128)return;
    int16_t x=(int16_t)left-(int16_t)offset;
    uint8_t ch;
    while(x<right && (ch=READ_BYTE(s++))!=0) {
        if(x+7>left)for(uint8_t row=0;row<10;row++) {
            uint8_t bits=Afr2Font7x10_Row((uint8_t)ch,row);
            for(uint8_t col=0;col<7;col++)if(x+col>=left && x+col<right && (bits&(0x80u>>col)))
                Afr2Oled_Pixel(o,x+col,y+row,color);
        }
        x+=7;
    }
}
void Afr2Oled_TextFlash(Afr2Oled *o,int16_t x,int16_t y,const char *s,Afr2Color color) {
    if(!s) return;
    uint8_t ch;
    while(x<128 && (ch=READ_BYTE(s++))!=0){Afr2Oled_Char(o,x,y,(char)ch,color);x+=7;}
}
