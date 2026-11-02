import { componentHeader,componentPanel,componentMini } from './oledComponents';
import { gestureHint } from './firmwareHints';
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
/** Literal integer-layout port of afr2_sensor_screens.c; prepared data only. */
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
import { Afr2OledBuffer } from './oledRasterizer';
const S=L.symbols,T=Math.trunc;
export interface FirmwareSensorSnapshot {
  nowMs:number;angle:number;pulseUs:number;targetUs:number;enabled:boolean;running:boolean;
  latest:{id:number;ms:number;distanceMm:number;error:number;angle?:number;pulseUs?:number};
  bins:readonly {id:number;distanceMm:number;error:number}[];
}
const sn=(i:number)=>L.sensorSine[i],cs=(i:number)=>i<=18?sn(18-i):-sn(i-18);
// AFR2-20261029-16: same half-degree interpolation and integer projection as C.
const snHalf=(a:number)=>{const i=T(a/10),f=a%10,v=sn(i);return i<36?v+T((sn(i+1)-v)*f/10):v;};
const csHalf=(a:number)=>a<=180?snHalf(180-a):-snHalf(a-180);
function line(o:Afr2OledBuffer,x:number,y:number,ex:number,ey:number){
  const dx=Math.abs(ex-x),dy=Math.abs(ey-y),sx=x<ex?1:-1,sy=y<ey?1:-1;let e=dx-dy;
  // AFR2-20261029-15: match AVR's bounded Bresenham. JavaScript already had
  // positive dy and products without AVR's16bit overflow.
  for(let remaining=Math.max(dx,dy)+1;remaining>0;--remaining){o.setPixel(x,y);if(x===ex && y===ey)break;const e2=2*e;if(e2>-dy){e-=dy;x+=sx;}if(e2<dx){e+=dx;y+=sy;}}
}
function hit(o:Afr2OledBuffer,angle:number,mm:number,range:number,radius:number,cy:number){
  if(angle<0 || angle>180 || mm<20 || mm>range)return;
  const r=T(mm*radius/range),x=64+T(r*csHalf(angle*2)/1000),y=cy-T(r*snHalf(angle*2)/1000);
  for(let dy=-2;dy<=2;dy++)for(let dx=-2;dx<=2;dx++)if(dx*dx+dy*dy<=4)o.setPixel(x+dx,y+dy);
}
function backHint(o:Afr2OledBuffer){gestureHint(o,0,53,true,'Volver');}
function smallText(o:Afr2OledBuffer,cx:number,y:number,value:string){
  const left=cx-T((value.length*6-1)/2);
  for(let j=0;j<value.length;j++)for(let row=0;row<8;row++){
    const bits=L.font[(value.charCodeAt(j)-32)*10+row];
    for(let col=0;col<5;col++)if(bits&(0x40>>col))o.setPixel(left+6*j+col,y+row);
  }
}
/** AFR2-20261029-27: literal viewport/range/trace and compact glyph port. */
export function renderSensorScreen(o:Afr2OledBuffer,node:number,h:FirmwareSensorSnapshot|undefined,ir:{raw:readonly number[];kind:number;valid:boolean;lineValid:boolean},view:{rangeIndex:number;trace:boolean}={rangeIndex:0,trace:true}){
  const txt=(x:number,y:number,text:string)=>o.drawText(x,y,text.slice(0,18));
  const rangeIndex=view.rangeIndex>=0 && view.rangeIndex<=3?view.rangeIndex:0,range=2000>>rangeIndex;
  const compact=node===S.AFR2_N_SERVO_ANGLE;
  if(node===S.AFR2_N_RADAR_VIEW || compact){
    const radius=compact?26:38,cy=compact?38:49;
    const active=!!h && h.enabled && h.pulseUs>=1000 && h.pulseUs<=2000 && h.angle>=0 && h.angle<=180;
    const center=active?h!.angle*2:0,low=Math.max(0,center-15),high=Math.min(360,center+15);
    txt(0,0,`${compact?'Servo':'1/2> R'} ${['2m','1m','50cm','25cm'][rangeIndex]}`);
    txt(80,0,h?.running?'AUTO':active?'FIJO':'OFF');if(!compact)txt(114,0,'cm');
    for(let ring=1;ring<=4;ring++){
      const r=T(radius*ring/4),step=T(344/r);
      for(let angle=0;angle<=360;angle+=step)o.setPixel(64+T(r*csHalf(angle)/1000),cy-T(r*snHalf(angle)/1000));
      if(active)for(let angle=low;angle<high;){const end=Math.min(high,(T(angle/10)+1)*10);line(o,64+T(r*csHalf(angle)/1000),cy-T(r*snHalf(angle)/1000),64+T(r*csHalf(end)/1000),cy-T(r*snHalf(end)/1000));angle=end;}
      if(!compact){const mm=T(range*ring/4);smallText(o,115,50-ring*10,mm%10?`${T(mm/10)}.${mm%10}`:String(T(mm/10)));}
    }
    for(let i=0;i<=36;i+=6){
      if(active && i*10>=low && i*10<=high)line(o,64,cy,64+T(radius*cs(i)/1000),cy-T(radius*sn(i)/1000));
      else for(let r=2;r<=radius;r+=3)o.setPixel(64+T(r*cs(i)/1000),cy-T(r*sn(i)/1000));
    }
    if(h){
      if(active){line(o,64,cy,64+T(radius*csHalf(low)/1000),cy-T(radius*snHalf(low)/1000));line(o,64,cy,64+T(radius*csHalf(high)/1000),cy-T(radius*snHalf(high)/1000));}
      if(view.trace)h.bins.forEach((b,i)=>{if(i<37 && b.id && b.error===0)hit(o,i*5,b.distanceMm,range,radius,cy);});
      const sample=h.latest;
      if(!view.trace && sample.id && sample.error===0 && sample.angle!==undefined && sample.pulseUs!==undefined && sample.pulseUs>=1000 && sample.pulseUs<=2000)hit(o,sample.angle,sample.distanceMm,range,radius,cy);
    }
    if(compact){
      o.drawRect(3,44,122,6);
      if(h){o.fillRect(4+T(h.angle*118/180),42,2,10);txt(0,53,`${String(h.angle).padStart(3,'0')} ${h.pulseUs}/${h.targetUs}us`);}else txt(0,53,'Sin datos PWM');
    }else if(h)txt(0,53,h.latest.id && h.latest.error===0?`${String(h.angle).padStart(3,'0')} ${String(h.latest.distanceMm).padStart(4,' ')}mm${h.latest.distanceMm>range?' >esc':''}`:`${String(h.angle).padStart(3,'0')} -- mm`);
    else txt(0,53,'Sin datos');return;
  }
  if(node===S.AFR2_N_US_VIEW){
    componentHeader(o,S.AFR2_ICON_RADAR,'Distancia',2,2);txt(0,16,h?.latest.id && !h.latest.error?`${h.latest.distanceMm} mm`:'Distancia: --');
    if(h){txt(0,28,h.enabled?`Ang ${String(h.angle).padStart(3,'0')} PWM ${h.pulseUs}`:'Ang -- PWM 0');txt(0,40,`Eco ${h.latest.error} Edad ${h.latest.id?(h.nowMs-h.latest.ms)>>>0:0}ms`);}gestureHint(o,0,53,false,'Opciones');return;
  }
  componentHeader(o,S.AFR2_ICON_SENSORS,'Sensores IR',node===S.AFR2_N_LINE_VIEW?1:2,2);
  if(!ir.valid){txt(0,25,ir.kind?'Esperando IR':'IR deshabilitado');backHint(o);return;}
  if(node===S.AFR2_N_LINE_VIEW){componentPanel(o,0,15,128,35);for(let i=0;i<5;i++){
    const center=15+24*i;componentMini(o,center-1,17,i+1);o.drawRect(center-6,24,12,16);
    const height=Math.min(12,T(ir.raw[i]*12/(ir.kind===1?1:1023)));o.fillRect(center-4,38-height,8,height);smallText(o,center,43,String(ir.raw[i]).slice(0,4));
  }}
  else for(let i=0;i<5;i++)txt(i<4?(i%2)*64:43,i<4?16+T(i/2)*12:40,`${i+1}:${String(ir.raw[i]).padStart(4,' ')}`);
  backHint(o);
}
