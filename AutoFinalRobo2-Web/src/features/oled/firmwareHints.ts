/** AFR2-20261030-18. Literal port of Afr2Ui_GestureHint, shared press/turn glyphs. */
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
import { ENCODER_TURN } from './reformResources';
import { ENCODER_LONG } from './encoderLongIcon';
import type { Afr2OledBuffer } from './oledRasterizer';
export function gestureHint(o:Afr2OledBuffer,x:number,y:number,kind:boolean|'turn',text:string){
  const turn=kind==='turn',im=turn?ENCODER_TURN:kind?ENCODER_LONG:L.icons[L.symbols.AFR2_ICON_ENCODER];
  const size=turn?21:13,largest=Math.max(im.width,im.height),stride=Math.ceil(im.width/8);
  const width=Math.trunc(im.width*size/largest),height=Math.trunc(im.height*size/largest);
  const left=x+Math.trunc((size-width)/2),top=y-(turn?4:3)+Math.trunc((size-height)/2);
  for(let py=0;py<height;py++)for(let px=0;px<width;px++){
    const sx=Math.trunc(px*im.width/width),sy=Math.trunc(py*im.height/height);
    if(im.bytes[sy*stride+Math.trunc(sx/8)]&(1<<(sx&7)))o.setPixel(left+px,top+py);
  }
  o.drawText(x+(turn?24:16),y,text);
}
