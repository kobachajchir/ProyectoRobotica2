/** Runtime counterpart of afr2_qr_screen.c. No network or robot calls. */
import { qrcodegen } from './vendor/qrcodegen';
import type { FirmwareSnapshot } from './firmwareRenderer';
import type { Afr2OledBuffer } from './oledRasterizer';
import { componentIcon } from './oledComponents';
import { gestureHint } from './firmwareHints';
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
const escapeWifi=(value:string)=>value.replace(/[\\;,:\"]/g,'\\$&');
const cache=new WeakMap<Afr2OledBuffer,{payload:string;qr:qrcodegen.QrCode}>();
export function clearQrScreenCache(o:Afr2OledBuffer){cache.delete(o);}
/** Caller-chosen UTF-8 content; same 62x62 body and limits as Afr2Qr_DrawPayload. */
export function renderQrPayload(o:Afr2OledBuffer,payload:string):boolean{
  o.clear();
  if(!payload){cache.delete(o);return false;}
  try{
    const previous=cache.get(o);
    const qr=previous?.payload===payload?previous.qr:qrcodegen.QrCode.encodeSegments([qrcodegen.QrSegment.makeBytes(Array.from(new TextEncoder().encode(payload)))],qrcodegen.QrCode.Ecc.LOW,1,9,0,true);
    cache.set(o,{payload,qr});
    const scale=Math.trunc(62/(qr.size+8)),origin=1+Math.trunc((62-(qr.size+8)*scale)/2);
    o.fillRect(1,1,62,62);
    for(let y=0;y<qr.size;y++)for(let x=0;x<qr.size;x++)if(qr.getModule(x,y))o.fillRect(origin+(x+4)*scale,origin+(y+4)*scale,scale,scale,'black');
    return true;
  }catch{cache.delete(o);o.clear();return false;}
}
export function renderQrScreen(o:Afr2OledBuffer,s:FirmwareSnapshot,ap:boolean){
  const ip=s.networkSta?s.networkIp:s.networkApIp;
  let status=s.qrAllowed===false?1:!s.networkValid?2:ap?(!s.networkAp || !s.networkApSsid?3:0):(!s.networkServer || !ip || ip==='0.0.0.0' || (!s.networkSta && !s.networkAp)?4:0);
  o.clear();
  if(!status){
    const payload=ap?`WIFI:T:${s.networkPassword?'WPA':'nopass'};S:${escapeWifi(s.networkApSsid)};P:${escapeWifi(s.networkPassword)};;`:`http://${ip}/`;
    if(!renderQrPayload(o,payload))status=5;
  }
  if(status){cache.delete(o);o.textWindow(0,63,3,'QR',0);o.textWindow(0,63,20,status===1?'Detener':status===5?'Contenido':'Sin datos',0);o.textWindow(0,63,32,status===1?'auto/radar':status===3?'Activar AP':status===4?'Web / IP':status===5?'muy largo':'de red',0);}
  componentIcon(o,ap?L.symbols.AFR2_ICON_AP:L.symbols.AFR2_ICON_LINK,67,1,14);
  o.drawText(67,19,ap?'Conectar':'Abrir');o.drawText(67,31,ap?'al AP':'Web');gestureHint(o,67,52,true,'Volver');
}
