/** AFR2-20261024-09. Copy of current Afr2Oled drawing primitives.
 * Source: Atmega/src/display/afr2_oled.c; glyph bytes generated from its font.
 * No I2C service in a browser. Live snapshots retain all 1024 original bytes.
 */
import { afr2Font7x10Row } from './font7x10';
export const AFR2_OLED_WIDTH = 128;
export const AFR2_OLED_HEIGHT = 64;
export const AFR2_OLED_BYTES = 1024;
export type Afr2Color = 'black' | 'white' | 'invert';
export class Afr2OledBuffer {
  public readonly buffer = new Uint8Array(AFR2_OLED_BYTES);
  public clear(): void { this.buffer.fill(0); }
  public setPixel(x: number, y: number, color: Afr2Color = 'white'): void {
    x = Math.trunc(x); y = Math.trunc(y);
    if (x < 0 || y < 0 || x >= 128 || y >= 64) return;
    const index = x + Math.floor(y / 8) * 128, old = this.buffer[index], mask = 1 << (y & 7);
    this.buffer[index] = color === 'white' ? old | mask : color === 'invert' ? old ^ mask : old & ~mask;
  }
  public getPixel(x: number, y: number): boolean {
    if (x < 0 || y < 0 || x >= 128 || y >= 64) return false;
    return (this.buffer[x + Math.floor(y / 8) * 128] & (1 << (y & 7))) !== 0;
  }
  public drawRect(x: number, y: number, w: number, h: number, color: Afr2Color = 'white'): void {
    if (!w || !h) return;
    for (let i = 0; i < w; i++) { this.setPixel(x + i, y, color); if (h > 1) this.setPixel(x + i, y + h - 1, color); }
    for (let i = 1; i + 1 < h; i++) { this.setPixel(x, y + i, color); if (w > 1) this.setPixel(x + w - 1, y + i, color); }
  }
  public fillRect(x: number, y: number, w: number, h: number, color: Afr2Color = 'white'): void {
    x=Math.trunc(x);y=Math.trunc(y);w=Math.trunc(w);h=Math.trunc(h);
    const left=Math.max(0,x),top=Math.max(0,y),right=Math.min(128,x+w),bottom=Math.min(64,y+h);
    if(w<=0 || h<=0 || left>=right || top>=bottom)return;
    const firstPage=Math.trunc(top/8),lastPage=Math.trunc((bottom-1)/8);
    for(let page=firstPage;page<=lastPage;page++){
      const first=page===firstPage?top&7:0,last=page===lastPage?(bottom-1)&7:7;
      const mask=((0xff<<first)&(0xff>>>(7-last)))&0xff;
      for(let px=left;px<right;px++){
        const index=page*128+px,old=this.buffer[index];
        this.buffer[index]=color==='white'?old|mask:color==='invert'?old^mask:old&~mask;
      }
    }
  }
  public drawChar(x: number, y: number, ch: string, color: Afr2Color = 'white'): void {
    for (let row = 0; row < 10; row++) {
      const bits = afr2Font7x10Row(ch.charCodeAt(0) || 0, row);
      for (let col = 0; col < 7; col++) if (bits & (0x80 >> col)) this.setPixel(x + col, y + row, color);
    }
  }
  public drawText(x: number, y: number, text: string, color: Afr2Color = 'white'): void {
    for (const ch of text) { if (x >= 128 || ch === '\0') break; this.drawChar(x, y, ch, color); x += 7; }
  }
  /** AFR2-20261106-04: also the AVR TextWindowFlash port; JS strings share one address space. */
  public textWindow(left: number, right: number, y: number, text: string, offset: number, color: Afr2Color = 'white'): void {
    if (left >= right || right > 128) return;
    let x = left - offset;
    for (const ch of text) {
      if (x >= right || ch === '\0') break;
      if (x + 7 > left) for (let row = 0; row < 10; row++) {
        const bits = afr2Font7x10Row(ch.charCodeAt(0), row);
        for (let col = 0; col < 7; col++) if (x + col >= left && x + col < right && (bits & (0x80 >> col))) this.setPixel(x + col, y + row, color);
      }
      x += 7;
    }
  }
}
