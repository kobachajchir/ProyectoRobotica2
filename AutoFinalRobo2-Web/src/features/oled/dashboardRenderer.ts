/** AFR2-20261102-14. Paired with afr2_dashboard.c; retained frame, no transport. */
import type { FirmwareSnapshot,FirmwareUiState } from './firmwareRenderer';
import type { Afr2OledBuffer } from './oledRasterizer';
import { componentIcon as icon,componentRoundedPanel } from './oledComponents';
import { gestureHint } from './firmwareHints';
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
const S=L.symbols,IDLE_MS=3000,PAGE_MS=4000;
// AFR2-20261109-05: same geometry and shared-center arithmetic as the AVR.
const CARD_X=54,CARD_Y=27,CARD_W=72,CARD_H=22,CARD_ICON=18,CARD_GAP=4,MODE_W=52;
const modes=['REPOSO','MANUAL','LINEA','RUEDA','GOTO','FALLO','SECUENC','PWM'];
// AFR2-20261108-12: same items, order and texts as afr2_dashboard.c (1 Motores .. 6 Rastro, 7 fallo).
const labels=[['--','--','--'],['OFF','ON','RUN'],['US OFF','US ON','US ON'],['OFF','ON','ON'],['OFF','ON','ON'],['IR OFF','IR ON','IR ON'],['RS OFF','RS ON','RS ON'],['FALLO','FALLO','FALLO']];
const icons=[S.AFR2_ICON_INFO,S.AFR2_ICON_MOTOR,S.AFR2_ICON_SENSORS,S.AFR2_ICON_RADAR,S.AFR2_ICON_SERVO,S.AFR2_ICON_SENSORS,S.AFR2_ICON_RADAR,S.AFR2_ICON_WARNING];
const itemState=(item:number,bits:number)=>item<1 || item>6 || !(bits&(1<<(item-1)))?0:item===1 && (bits&0x40)?2:1;
interface Runtime{inputMs:number;sourceInputMs:number;rotateMs:number;armed:boolean;mask:number;selected:number;drawn:number;mode:number;header:string;ssidOffset:number;ipOffset:number}
const states=new WeakMap<Afr2OledBuffer,Runtime>();
const age=(now:number,then:number)=>(now-then)>>>0;
export function clearDashboard(o:Afr2OledBuffer){states.delete(o);}
function offset(text:string,elapsed:number){
  const end=text.length*7-70;if(end<=0)return 0;const travel=end*40;let phase=(elapsed>>>0)%(1600+2*travel);
  if(phase<800)return 0;phase-=800;if(phase<travel)return Math.trunc(phase/40);phase-=travel;
  return phase<800?end:end-Math.trunc((phase-800)/40);
}
function status(d:Runtime,u:FirmwareUiState,s:FirmwareSnapshot,full:boolean){
  let bits=0;
  if(s.dashboardValid){bits=((s.quickFlags??0)>>1)&0x3f;if((bits&1) && s.dashboardMoving)bits|=0x40;}
  if(full){d.inputMs=u.clockMs;d.rotateMs=u.clockMs;d.armed=false;d.selected=1;}
  const source=u.dashboardInputMs??0;
  if(source!==d.sourceInputMs){d.inputMs=source;d.armed=false;d.sourceInputMs=source;}
  if(u.dashboardHeld){d.inputMs=u.clockMs;d.armed=false;}
  if(!s.dashboardValid)d.selected=0;
  else if(s.dashboardFault){d.selected=7;d.rotateMs=u.clockMs;d.armed=true;}
  else{
    if(d.selected<1 || d.selected>6)d.selected=1;
    let diff=(d.mask^bits)&0x3f;if((d.mask^bits)&0x40)diff|=1;
    if(!full && diff){for(let i=0;i<6;i++)if(diff&(1<<i)){d.selected=i+1;break;}d.rotateMs=u.clockMs;d.armed=true;}
    else if(u.dashboardHeld || age(u.clockMs,d.inputMs)<IDLE_MS)d.armed=false;
    else if(!d.armed || age(u.clockMs,d.rotateMs)>=PAGE_MS){d.selected=d.selected%6+1;d.rotateMs=u.clockMs;d.armed=true;}
  }
  d.mask=bits;return d.selected;
}
function lane(o:Afr2OledBuffer,y:number,text:string,shift:number){o.fillRect(21,y,70,10,'black');o.textWindow(21,91,y,text,shift);o.fillRect(21,y,70,10,'invert');}
export function renderDashboard(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot):boolean{
  let d=states.get(o);const full=!d;
  if(!d){d={inputMs:u.clockMs,sourceInputMs:u.dashboardInputMs??0,rotateMs:u.clockMs,armed:false,mask:0,selected:0,drawn:0,mode:255,header:'',ssidOffset:0,ipOffset:0};states.set(o,d);}
  let changed=full;const connected=s.networkValid && (s.networkSta || s.networkAp);
  const ssid=(connected?(s.networkSta?s.networkSsid:s.networkApSsid):'') || (connected?s.networkSta?'Wi-Fi':'AP':s.networkValid?'Sin Wi-Fi':'Red: --');
  const ip=(connected?(s.networkSta?s.networkIp:s.networkApIp):'') || 'IP: --';
  const ss=offset(ssid,age(u.clockMs,u.scrollStartedMs)),is=offset(ip,age(u.clockMs,u.scrollStartedMs));
  const key=JSON.stringify([s.networkMode,s.networkValid,s.networkSta,s.networkAp,s.batteryValid,s.batteryMv,ssid,ip]);
  const header=full || key!==d.header;
  if(full)o.clear();
  if(header){
    o.fillRect(0,0,128,26,'black');icon(o,!s.networkValid?S.AFR2_ICON_WIFI:s.networkMode===2?S.AFR2_ICON_AP:!connected?S.AFR2_ICON_WIFI_OFF:s.networkSta?S.AFR2_ICON_WIFI:S.AFR2_ICON_AP,0,3,18);
    const mv=s.batteryMv??0,battery=s.batteryValid?S.AFR2_ICON_BATTERY_EMPTY+(mv===0?0:mv<6000?1:mv<7000?2:mv<8000?3:4):S.AFR2_ICON_BATTERY_UNKNOWN;
    const voltage=s.batteryValid?`${Math.trunc(mv/1000)}.${Math.trunc(mv%1000/100)}V`:'--V';
    icon(o,battery,110,0,16);o.drawText(128-voltage.length*7,15,voltage);o.fillRect(0,0,128,26,'invert');changed=true;
  }
  if(header || d.ssidOffset!==ss){lane(o,2,ssid,ss);changed=true;}
  if(header || d.ipOffset!==is){lane(o,14,ip,is);changed=true;}
  const mode=s.dashboardValid && s.dashboardMode!==undefined && s.dashboardMode<8?s.dashboardMode:255;
  if(full || mode!==d.mode){o.fillRect(0,27,MODE_W,23,'black');o.drawText(2,28,'MODO');o.drawText(2,40,modes[mode]??'--');changed=true;}
  const selected=status(d,u,s,full),shown=selected|(itemState(selected,d.mask)<<3);
  // AFR2-20261108-10: same long-press icon and position as afr2_dashboard.c.
  if(full){componentRoundedPanel(o,CARD_X,CARD_Y,CARD_W,CARD_H);gestureHint(o,0,52,false,'Menu');icon(o,L.symbols.AFR2_ICON_ENCODER_LONG,47,51,11);o.drawText(60,52,'Rapidas');}
  if(full || d.drawn!==shown){
    const label=labels[selected][itemState(selected,d.mask)],width=CARD_ICON+CARD_GAP+label.length*7;
    const left=CARD_X+1+Math.trunc((CARD_W-2-width)/2);
    const iconY=CARD_Y+Math.trunc((CARD_H-CARD_ICON)/2),textY=CARD_Y+Math.trunc((CARD_H-10)/2);
    o.fillRect(CARD_X+2,CARD_Y+1,CARD_W-4,CARD_H-2,'black');
    o.fillRect(CARD_X+1,CARD_Y+2,CARD_W-2,CARD_H-4,'black');
    icon(o,icons[selected],left,iconY,CARD_ICON);o.drawText(left+CARD_ICON+CARD_GAP,textY,label);changed=true;
  }
  d.header=key;d.ssidOffset=ss;d.ipOffset=is;d.mode=mode;d.drawn=shown;return changed;
}
