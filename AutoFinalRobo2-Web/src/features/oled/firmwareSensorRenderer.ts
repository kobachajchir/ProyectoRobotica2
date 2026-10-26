/** Literal integer-layout port of afr2_sensor_screens.c; prepared data only. */
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
import { Afr2OledBuffer } from './oledRasterizer';
const S=L.symbols,T=Math.trunc;
export interface FirmwareSensorSnapshot {
  nowMs:number;angle:number;pulseUs:number;targetUs:number;enabled:boolean;running:boolean;
  latest:{id:number;ms:number;distanceMm:number;error:number};
  bins:readonly {id:number;distanceMm:number;error:number}[];
}
const sn=(i:number)=>L.sensorSine[i],cs=(i:number)=>i<=18?sn(18-i):-sn(i-18);
function line(o:Afr2OledBuffer,x:number,y:number,ex:number,ey:number){
  const dx=Math.abs(ex-x),dy=Math.abs(ey-y),sx=x<ex?1:-1,sy=y<ey?1:-1;let e=dx-dy;
  for(;;){o.setPixel(x,y);if(x===ex && y===ey)break;const e2=2*e;if(e2>-dy){e-=dy;x+=sx;}if(e2<dx){e+=dx;y+=sy;}}
}
export function renderSensorScreen(o:Afr2OledBuffer,node:number,h:FirmwareSensorSnapshot|undefined,ir:{raw:readonly number[];kind:number;valid:boolean;lineValid:boolean}){
  const txt=(x:number,y:number,text:string)=>o.drawText(x,y,text.slice(0,18));
  if(node===S.AFR2_N_RADAR_VIEW){
    txt(0,0,'Radar 4m');txt(84,0,h?.running?'AUTO':'FIJO');
    for(let r=1;r<=4;r++)for(let i=0;i<36;i++){const radius=T(38*r/4);line(o,64+T(radius*cs(i)/1000),49-T(radius*sn(i)/1000),64+T(radius*cs(i+1)/1000),49-T(radius*sn(i+1)/1000));}
    for(let i=0;i<=36;i+=6)for(let r=2;r<=38;r+=3)o.setPixel(64+T(r*cs(i)/1000),49-T(r*sn(i)/1000));
    txt(110,10,'4m');txt(110,20,'3m');txt(110,30,'2m');txt(110,40,'1m');
    if(h){const i=Math.min(36,T((h.angle+2)/5));if(h.enabled)line(o,64,49,64+T(38*cs(i)/1000),49-T(38*sn(i)/1000));
      h.bins.forEach((b,i)=>{if(i<37 && b.id && b.error===0){const radius=T(b.distanceMm*38/4000);o.fillRect(63+T(radius*cs(i)/1000),48-T(radius*sn(i)/1000),3,3);}});
      txt(0,53,h.latest.id && !h.latest.error?`1/2 ${String(h.angle).padStart(3,'0')} ${String(h.latest.distanceMm).padStart(4,' ')}mm`:`1/2 ${String(h.angle).padStart(3,'0')} -- mm`);
    }else txt(0,53,'1/2 Sin datos');return;
  }
  if(node===S.AFR2_N_US_VIEW){
    txt(0,0,'Distancia 2/2');txt(0,14,h?.latest.id && !h.latest.error?`${h.latest.distanceMm} mm`:'Distancia: --');
    if(h){txt(0,27,h.enabled?`Ang ${String(h.angle).padStart(3,'0')} PWM ${h.pulseUs}`:'Ang -- PWM 0');txt(0,39,`Eco ${h.latest.error} Edad ${h.latest.id?(h.nowMs-h.latest.ms)>>>0:0}ms`);}txt(0,53,'Giro / SW volver');return;
  }
  if(node===S.AFR2_N_SERVO_ANGLE){
    txt(0,0,'Angulo del servo');if(h){txt(0,16,`${String(h.angle).padStart(3,'0')} grados ${h.enabled?'PWM':'OFF'}`);o.drawRect(3,32,122,8);o.fillRect(4+T(h.angle*118/180),30,2,12);txt(0,43,`${h.pulseUs} / ${h.targetUs} us`);}txt(0,54,'Giro / SW volver');return;
  }
  txt(0,0,node===S.AFR2_N_LINE_VIEW?'IR linea 1/2':'IR valores 2/2');
  if(node===S.AFR2_N_LINE_VIEW){for(let i=0;i<5;i++){const x=8+24*i;o.drawRect(x,14,14,28);if(ir.valid){const height=Math.min(24,T(ir.raw[i]*24/(ir.kind===1?1:1023)));o.fillRect(x+2,40-height,10,height);}txt(x+4,43,String(i+1));}}
  else if(ir.valid){for(let row=0;row<3;row++){const i=row*2;txt(0,14+12*row,i<4?`${i+1}:${String(ir.raw[i]).padStart(4,' ')} ${i+2}:${String(ir.raw[i+1]).padStart(4,' ')}`:`5:${String(ir.raw[4]).padStart(4,' ')} ${ir.lineValid?'CAL':'SIN CAL'}`);}}
  else txt(0,25,'Sin lectura IR');txt(0,54,'Giro / SW volver');
}
