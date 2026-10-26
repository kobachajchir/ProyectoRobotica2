import { useEffect,useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { useConnection } from './ConnectionContext';
import { UiIcon } from '../../components/ui/UiIcon';
import { Button } from '../../components/ui/Button';
import type { ModalOrigin } from '../../components/ui/FloatingModal';
function Signal({level}:{level:number}){return <svg viewBox="0 0 24 24" width="22" height="22" fill="none" stroke="currentColor" strokeWidth="1.8" strokeLinecap="round" aria-hidden="true">
  <path d="M3 7a15 15 0 0 1 18 0" opacity={level>=3?1:.22}/><path d="M6 11a10 10 0 0 1 12 0" opacity={level>=2?1:.22}/><path d="M9 15a5 5 0 0 1 6 0" opacity={level>=1?1:.22}/><circle cx="12" cy="20" r=".8" fill="currentColor"/>
</svg>;}
export function NetworkPill({onOpen}:{onOpen:(origin:ModalOrigin)=>void}){
  const {wifiStatus:w,wsState}=useConnection();const [now,setNow]=useState(Date.now());
  useEffect(()=>{const id=setInterval(()=>setNow(Date.now()),1000);return()=>clearInterval(id);},[]);
  const fresh=wsState==='ready' && !!w.receivedAtMs && now-w.receivedAtMs<7000;
  const ap=fresh && w.connectionInterface==='AP' && !!w.apIp,sta=fresh && !ap && w.staConnected;
  const rssi=w.connectionRssiDbm??null,level=rssi===null?0:rssi>=-55?3:rssi>=-67?2:rssi>=-75?1:0;
  const label=ap?w.apSsid:sta?w.staSsid:wsState==='ready'?'Red sin datos':wsState==='connecting' || wsState==='handshaking'?'Conectando…':'Sin conexion';
  return <button type="button" className="afr2-network-pill" data-mode={ap?'ap':sta?'sta':'unknown'} aria-haspopup="dialog" title={`${label??'Red'}${sta && rssi!==null?` · ${rssi} dBm`:''}`} aria-label={`Red: ${label??'desconocida'}. Abrir informacion de red`} onClick={e=>onOpen({element:e.currentTarget,keyboard:e.detail===0})}>
    {ap?<UiIcon name="antenna"/>:<Signal level={level}/>}<span>{label??'Red'}</span><small>{ap?'AP':sta && rssi!==null?`${rssi} dBm`:''}</small>
  </button>;
}
export function NetworkDetails({onClose}:{onClose:()=>void}){
  const {wifiStatus:w,wsState,refreshWifiStatus}=useConnection(),navigate=useNavigate();
  const fresh=wsState==='ready' && !!w.receivedAtMs && Date.now()-w.receivedAtMs<7000;
  const ap=w.connectionInterface==='AP',rows=[['Conexion de esta Web',ap?'Punto de acceso del auto':'Wi-Fi / estacion'],['Modo del ESP',w.mode],['SSID Wi-Fi',w.staSsid??'—'],['IP Wi-Fi',w.staIp??'—'],['Señal ESP → router',w.rssiDbm===null?'No disponible':`${w.rssiDbm} dBm`],['SSID del auto',w.apSsid??'—'],['IP del auto',w.apIp??'—'],['Clientes AP',w.apClients===undefined?'—':String(w.apClients)],['Canal',w.channel===undefined?'—':String(w.channel)]];
  return <div className="afr2-page-stack"><p className="afr2-help">{fresh?'Informacion recibida del ESP.':'Sin conexion actual; informacion no disponible.'} En AP, la señal del cliente no se infiere del RSSI del router.</p><dl className="afr2-network-details">{rows.map(([key,value])=><div key={key}><dt>{key}</dt><dd>{fresh?value:'—'}</dd></div>)}</dl><div className="afr2-action-row"><Button disabled={wsState!=='ready'} onClick={()=>void refreshWifiStatus()}>Actualizar</Button><Button variant="primary" onClick={()=>{onClose();navigate('/wifi');}}>Gestionar Wi-Fi</Button></div></div>;
}
