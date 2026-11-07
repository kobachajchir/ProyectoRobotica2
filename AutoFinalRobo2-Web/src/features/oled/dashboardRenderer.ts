/** AFR2-20261102-14. Paired with afr2_dashboard.c; retained frame, no transport. */
import type { FirmwareSnapshot,FirmwareUiState } from './firmwareRenderer';
import type { Afr2OledBuffer } from './oledRasterizer';
import { componentIcon as icon,componentRoundedPanel } from './oledComponents';
import { gestureHint } from './firmwareHints';
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
const S=L.symbols,IDLE_MS=3000,PAGE_MS=4000;
const modes=['REPOSO','MANUAL','LINEA','RUEDA','GOTO','FALLO','SECUENC','PWM'];
const labels=['--','ON','ON','OFF','RUN','ON','RASTRO','FALLO'];
const icons=[S.AFR2_ICON_INFO,S.AFR2_ICON_RADAR,S.AFR2_ICON_SERVO,S.AFR2_ICON_MOTOR,S.AFR2_ICON_MOTOR,S.AFR2_ICON_SENSORS,S.AFR2_ICON_RADAR,S.AFR2_ICON_WARNING];
interface Runtime{inputMs:number;sourceInputMs:number;rotateMs:number;armed:boolean;mask:number;selected:number;drawn:number;mode:number;header:string;ssidOffset:number;ipOffset:number}
const states=new WeakMap<Afr2OledBuffer,Runtime>();
const age=(now:number,then:number)=>(now-then)>>>0;
const first=(mask:number)=>{for(let i=1;i<=6;i++)if(mask&(1<<(i-1)))return i;return 0;};
export function clearDashboard(o:Afr2OledBuffer){states.delete(o);}
function offset(text:string,elapsed:number){
  const end=text.length*7-70;if(end<=0)return 0;const travel=end*40;let phase=(elapsed>>>0)%(1600+2*travel);
  if(phase<800)return 0;phase-=800;if(phase<travel)return Math.trunc(phase/40);phase-=travel;
  return phase<800?end:end-Math.trunc((phase-800)/40);
}
function status(d:Runtime,u:FirmwareUiState,s:FirmwareSnapshot,full:boolean){
  let mask=0;const flags=s.quickFlags??0;
  if(s.dashboardValid){if(flags&1)mask|=1;if(flags&2)mask|=2;if(!(flags&4))mask|=4;else if(s.dashboardMoving)mask|=8;if(flags&8)mask|=16;if((flags&17)===17)mask|=32;}
  if(full){d.inputMs=u.clockMs;d.rotateMs=u.clockMs;d.armed=false;d.selected=first(mask);}
  const source=u.dashboardInputMs??0;
  if(source!==d.sourceInputMs){d.inputMs=source;d.armed=false;d.sourceInputMs=source;}
  if(u.dashboardHeld){d.inputMs=u.clockMs;d.armed=false;}
  if(s.dashboardValid && s.dashboardFault){d.selected=7;d.rotateMs=u.clockMs;d.armed=true;}
  else{
    if(d.selected>6 || !d.selected || !(mask&(1<<(d.selected-1)))){d.selected=first(mask);if(!full){d.rotateMs=u.clockMs;d.armed=true;}}
    if(!full && d.mask!==mask){d.rotateMs=u.clockMs;d.armed=true;}
    if(u.dashboardHeld || age(u.clockMs,d.inputMs)<IDLE_MS)d.armed=false;
    else if(mask && (mask&(mask-1)) && (!d.armed || age(u.clockMs,d.rotateMs)>=PAGE_MS)){
      for(let n=1;n<=6;n++){const next=(d.selected-1+n)%6+1;if(mask&(1<<(next-1))){d.selected=next;break;}}
      d.rotateMs=u.clockMs;d.armed=true;
    }
  }
  d.mask=mask;return d.selected;
}
function lane(o:Afr2OledBuffer,y:number,text:string,shift:number){o.fillRect(21,y,70,10,'black');o.textWindow(21,91,y,text,shift);o.fillRect(21,y,70,10,'invert');}
export function renderDashboard(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot):boolean{
  let d=states.get(o);const full=!d;
  if(!d){d={inputMs:u.clockMs,sourceInputMs:u.dashboardInputMs??0,rotateMs:u.clockMs,armed:false,mask:0,selected:0,drawn:0,mode:255,header:'',ssidOffset:0,ipOffset:0};states.set(o,d);}
  let changed=full;const connected=s.networkValid && (s.networkSta || s.networkAp);
  const ssid=(connected?(s.networkSta?s.networkSsid:s.networkApSsid):'') || (connected?s.networkSta?'Wi-Fi':'AP':s.networkValid?'Sin Wi-Fi':'Red: --');
  const ip=(connected?(s.networkSta?s.networkIp:s.networkApIp):'') || 'IP: --';
  const ss=offset(ssid,age(u.clockMs,u.scrollStartedMs)),is=offset(ip,age(u.clockMs,u.scrollStartedMs));
  const key=JSON.stringify([s.networkValid,s.networkSta,s.networkAp,s.batteryValid,s.batteryMv,ssid,ip]);
  const header=full || key!==d.header;
  if(full)o.clear();
  if(header){
    o.fillRect(0,0,128,26,'black');icon(o,!s.networkValid?S.AFR2_ICON_WIFI:!connected?S.AFR2_ICON_WIFI_OFF:s.networkSta?S.AFR2_ICON_WIFI:S.AFR2_ICON_AP,0,3,18);
    const mv=s.batteryMv??0,battery=s.batteryValid?S.AFR2_ICON_BATTERY_EMPTY+(mv===0?0:mv<6000?1:mv<7000?2:mv<8000?3:4):S.AFR2_ICON_BATTERY_UNKNOWN;
    const voltage=s.batteryValid?`${Math.trunc(mv/1000)}.${Math.trunc(mv%1000/100)}V`:'--V';
    icon(o,battery,110,0,16);o.drawText(128-voltage.length*7,15,voltage);o.fillRect(0,0,128,26,'invert');changed=true;
  }
  if(header || d.ssidOffset!==ss){lane(o,2,ssid,ss);changed=true;}
  if(header || d.ipOffset!==is){lane(o,14,ip,is);changed=true;}
  const mode=s.dashboardValid && s.dashboardMode!==undefined && s.dashboardMode<8?s.dashboardMode:255;
  if(full || mode!==d.mode){o.fillRect(0,27,60,23,'black');o.drawText(2,28,'MODO');o.drawText(2,40,modes[mode]??'--');changed=true;}
  const selected=status(d,u,s,full);
  if(full){componentRoundedPanel(o,62,27,64,16);gestureHint(o,0,52,false,'Menu');o.drawText(54,52,'Rapidas');}
  if(full || d.drawn!==selected){const label=labels[selected],left=63+Math.trunc((62-(14+4+label.length*7))/2);o.fillRect(64,28,60,14,'black');o.fillRect(63,29,62,12,'black');icon(o,icons[selected],left,28,14);o.drawText(left+18,30,label);changed=true;}
  d.header=key;d.ssidOffset=ss;d.ipOffset=is;d.mode=mode;d.drawn=selected;return changed;
}
