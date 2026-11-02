/** AFR2-20261102-05. Paired with afr2_ui_components.c, OLED128-5. */
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
import { ENCODER_LONG } from './encoderLongIcon';
import { ENCODER_TURN, WIFI_LOCK } from './reformResources';
import type { Afr2OledBuffer } from './oledRasterizer';
export function componentIcon(o:Afr2OledBuffer,id:number,x:number,y:number,size:number){
  const im=id===17?ENCODER_LONG:id===24?ENCODER_TURN:id===25?WIFI_LOCK:L.icons[id];if(!im)return;
  const largest=Math.max(im.width,im.height),stride=Math.ceil(im.width/8);
  const w=Math.trunc(im.width*size/largest),h=Math.trunc(im.height*size/largest);
  x+=Math.trunc((size-w)/2);y+=Math.trunc((size-h)/2);
  for(let py=0;py<h;py++)for(let px=0;px<w;px++)if(im.bytes[Math.trunc(py*im.height/h)*stride+Math.trunc(Math.trunc(px*im.width/w)/8)]&(1<<(Math.trunc(px*im.width/w)&7)))o.setPixel(x+px,y+py);
}
export function componentHeader(o:Afr2OledBuffer,icon:number,title:string,page=0,count=0){
  let right=126;o.fillRect(0,0,128,14,'black');componentIcon(o,icon,2,0,14);
  if(count){const text=`${page}/${count}`;right=126-text.length*7;o.drawText(right,2,text);right-=4;}
  o.textWindow(20,right,2,title,0);o.fillRect(0,0,128,14,'invert');
}
export function componentPanel(o:Afr2OledBuffer,x:number,y:number,w:number,h:number){
  for(let r=0;r<h;r++){const inset=Math.max(0,2-Math.min(r,h-r-1));
    if(!r || r===h-1)o.fillRect(x+inset,y+r,w-2*inset,1);
    else {o.setPixel(x+inset,y+r);o.setPixel(x+w-inset-1,y+r);}}
}
export function componentRoundedPanel(o:Afr2OledBuffer,x:number,y:number,w:number,h:number){
  for(let row=0;row<h;row++){const edge=Math.min(row,h-row-1),inset=edge===0?3:edge===1?1:0;
    if(!edge)o.fillRect(x+inset,y+row,w-2*inset,1);else {o.setPixel(x+inset,y+row);o.setPixel(x+w-inset-1,y+row);}}
}
export function componentProgress(o:Afr2OledBuffer,y:number,known:boolean,percent:number,clock:number){
  o.fillRect(2,y,124,8,'black');o.drawRect(2,y,124,8);
  o.fillRect(known?4:4+Math.trunc(clock/40)%91,y+2,known?Math.trunc(Math.min(100,Math.max(0,percent))*120/100):30,4);
}
const mini=[[7,5,5,5,7],[2,6,2,2,7],[7,1,7,4,7],[7,1,7,1,7],[5,5,7,1,1],[7,4,7,1,7],[7,4,7,5,7],[7,1,1,1,1],[7,5,7,5,7],[7,5,7,1,7]];
export function componentMini(o:Afr2OledBuffer,x:number,y:number,value:number){
  for(const [i,ch] of String(value).split('').entries())for(let r=0;r<5;r++)for(let c=0;c<3;c++)if(mini[Number(ch)]?.[r]&(4>>c))o.setPixel(x+4*i+c,y+r);
}
const metric:Record<string,number[]>={'0':[14,17,19,21,25,17,14],'1':[4,12,4,4,4,4,14],'2':[14,17,1,2,4,8,31],'3':[30,1,1,14,1,1,30],'4':[2,6,10,18,31,2,2],'5':[31,16,16,30,1,1,30],'6':[14,16,16,30,17,17,14],'7':[31,1,2,4,8,8,8],'8':[14,17,17,14,17,17,14],'9':[14,17,17,15,1,1,14],'-':[0,0,0,31,0,0,0],'.':[0,0,0,0,0,12,12]};
export function componentMetric(o:Afr2OledBuffer,x:number,y:number,text:string){
  for(const [i,ch] of text.split('').entries())for(let r=0;r<7;r++)for(let c=0;c<5;c++)if(metric[ch]?.[r]&(16>>c))o.fillRect(x+12*i+2*c,y+2*r,2,2);
}
