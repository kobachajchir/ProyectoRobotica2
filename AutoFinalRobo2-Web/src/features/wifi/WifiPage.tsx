/* AFR2-20261030-09. Network state and operations confirmed by ESP observations. */
import { useEffect, useRef, useState, type FormEvent } from 'react';
import { TaskSections } from '../../components/ui/TaskSections';
import { GlassSurface } from '../../components/ui/GlassSurface';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { Button } from '../../components/ui/Button';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { WifiCredentialsFields, validWifiCredentials } from './WifiCredentialsFields';
import { useConnection } from '../connection/ConnectionContext';
import type { Afr2CommandName, WifiStatusSnapshot } from '../../protocol/wsEnvelope';

function Lock(){return <svg width="13" height="15" viewBox="0 0 13 15" fill="none" aria-label="Red protegida"><path d="M3 6V4a3.5 3.5 0 0 1 7 0v2M1 6h11v8H1z" stroke="currentColor" strokeWidth="1.5"/><path d="M6.5 9v2" stroke="currentColor" strokeWidth="1.5"/></svg>;}
const security=(value?:string)=>value==='open'?'Abierta':value==='unknown' || !value?'Sin dato de seguridad':value;
const modeLabel=(mode:string)=>mode==='AP_STA'?'AP-STA':mode;
type Goal={kind:'connect'|'disconnect'|'ap';started:number};
export function WifiPage(){
  const {wsState,wifiStatus,wifiNetworks,wifiScanState,wifiScanError,wifiScanTruncated,refreshWifiStatus,scanWifiNetworks,requestCommand}=useConnection();
  const [view,setView]=useState('estado'),[configuring,setConfiguring]=useState(false);
  const [ssid,setSsid]=useState(''),[password,setPassword]=useState(''),[persist,setPersist]=useState(true);
  const [targetMode,setTargetMode]=useState<'STA'|'AP'|'AP_STA'>('AP_STA');
  const [message,setMessage]=useState(''),[failed,setFailed]=useState(false),[busy,setBusy]=useState(false),[goal,setGoal]=useState<Goal|null>(null);
  const mounted=useRef(true),epoch=useRef(0),scanRequested=useRef(false),modeSeeded=useRef(false);
  const connected=wifiStatus.staConnected,scanning=wifiScanState==='scanning',online=wsState==='ready';
  const fresh=online && typeof wifiStatus.receivedAtMs==='number' && Date.now()-wifiStatus.receivedAtMs<5000;
  useEffect(()=>{if(!modeSeeded.current && ['STA','AP','AP_STA'].includes(wifiStatus.mode)){modeSeeded.current=true;setTargetMode(wifiStatus.mode as 'STA'|'AP'|'AP_STA');}},[wifiStatus.mode]);
  useEffect(()=>{mounted.current=true;return()=>{mounted.current=false;++epoch.current;};},[]);
  useEffect(()=>{if(scanRequested.current && wifiScanState==='completed'){scanRequested.current=false;setView('redes');setMessage('Redes listas.');setFailed(false);}},[wifiScanState]);
  useEffect(()=>{
    if(!goal)return;
    let cancelled=false,timer:ReturnType<typeof setTimeout>|undefined;
    const poll=async()=>{
      if(cancelled)return;
      if(wsState!=='ready'){setMessage('La conexion Web se interrumpio. El resultado Wi-Fi sigue sin confirmar; reconecta y consulta Estado de red.');setGoal(null);return;}
      try{
        const res=await requestCommand<WifiStatusSnapshot>('wifi.status',{},2500);
        if(cancelled)return;
        const status=res.payload,confirmed=res.status==='completed' && (goal.kind==='disconnect'?status.staConnected===false:goal.kind==='ap'?status.mode==='AP' && !!status.apIp && status.apIp!=='0.0.0.0':status.staConnected===true);
        if(confirmed){setMessage(goal.kind==='disconnect'?'Operacion exitosa: red desconectada.':goal.kind==='ap'?'Operacion exitosa: AP activo.':'Operacion exitosa: red conectada.');setFailed(false);setGoal(null);void refreshWifiStatus();return;}
      }catch{if(cancelled)return;}
      if(Date.now()-goal.started>(goal.kind==='disconnect'?5000:25000)){setMessage('No se confirmo el estado esperado dentro del plazo. Consulta Estado de red.');setFailed(true);setGoal(null);return;}
      timer=setTimeout(()=>void poll(),700);
    };
    void poll();return()=>{cancelled=true;if(timer)clearTimeout(timer);};
  },[goal,wsState,requestCommand,refreshWifiStatus]);
  const operate=async(command:Afr2CommandName,payload:Record<string,unknown>,next?:Goal['kind'])=>{
    if(busy || goal)return;const ticket=++epoch.current;setBusy(true);setMessage('Solicitud enviada; esperando confirmacion del ESP.');setFailed(false);
    try{
      const res=await requestCommand<Record<string,unknown>>(command,payload,4000);
      if(!mounted.current || ticket!==epoch.current)return;
      if(next){setGoal({kind:next,started:Date.now()});setMessage(next==='disconnect'?'Desconectando…':'Conectando…');}
      else if(res.status==='completed' && res.payload.confirmed===true){setMessage('Operacion exitosa: modo actualizado.');void refreshWifiStatus();}
      else setMessage('Operacion aceptada; consulta el estado para confirmar el resultado.');
    }catch(e){if(mounted.current && ticket===epoch.current){setMessage(e instanceof Error?e.message:'Sin confirmacion del ESP.');setFailed(true);}}
    finally{if(mounted.current && ticket===epoch.current)setBusy(false);}
  };
  const scan=()=>{scanRequested.current=true;setMessage('Buscando redes…');setView('redes');void scanWifiNetworks();};
  const configure=async(e:FormEvent)=>{
    e.preventDefault();if(!validWifiCredentials(ssid,password))return;
    await operate('wifi.configure',{mode:targetMode,ssid,password,persist},targetMode==='AP'?'ap':'connect');setPassword('');
  };
  const config=<GlassSurface variant="panel" className="p-5 sm:p-6">
    <SectionHeader title="Modificar credenciales" description="El cambio puede interrumpir esta conexion Web. La aceptacion no confirma una conexion nueva."/>
    <form onSubmit={e=>void configure(e)} className="afr2-page-stack">
      <WifiCredentialsFields id="wifi-cfg" mode={targetMode} ssid={ssid} password={password} persist={persist} onSsid={setSsid} onPassword={setPassword} onPersist={setPersist}/>
      <div className="afr2-action-row"><Button type="submit" disabled={!online || busy || !!goal || scanning || !validWifiCredentials(ssid,password)}>Conectar</Button><Button type="button" onClick={()=>{setConfiguring(false);setPassword('');}}>Volver</Button></div>
    </form>
  </GlassSurface>;
  return <div className="afr2-route-view afr2-page-stack">
    <SectionHeader title="Wi-Fi" description="Estado de red, modos y credenciales del robot."/>
    {message && <StatusNotice tone={failed?'warning':goal?'info':'neutral'} title={message}/>}
    {configuring?config:<TaskSections label="Secciones Wi-Fi" value={view} onValueChange={setView} items={[
      {id:'estado',label:'Estado de red',content:<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader title={!fresh?'Sin datos actuales':connected?'Conectado':'Desconectado'} description="Estado de la conexion STA; el punto de acceso AP se informa por separado." actions={<Button disabled={!online} onClick={()=>void refreshWifiStatus()}>Actualizar estado</Button>}/>
        {wifiStatus.fallbackActive && <StatusNotice tone="info" title="AP / fallback activo"/>}
        {wifiStatus.configError && <StatusNotice tone="warning" title={wifiStatus.configError}/>}
        <div className="afr2-page-stack">
          <section><h2>Estado de conexion</h2><dl className="afr2-status-list">
            <div><dt>Modo</dt><dd>{fresh?modeLabel(wifiStatus.mode):'—'}</dd></div><div><dt>IP STA</dt><dd>{fresh?wifiStatus.staIp||'—':'—'}</dd></div>
            <div><dt>Mascara STA</dt><dd>{fresh?wifiStatus.staMask||'—':'—'}</dd></div><div><dt>Puerta de enlace STA</dt><dd>{fresh?wifiStatus.staGateway||'—':'—'}</dd></div>
            <div><dt>IP AP</dt><dd>{fresh?wifiStatus.apIp||'—':'—'}</dd></div><div><dt>Mascara AP</dt><dd>{fresh?wifiStatus.apMask||'—':'—'}</dd></div>
            <div><dt>Puerta de enlace AP</dt><dd>{fresh?wifiStatus.apGateway||'—':'—'}</dd></div>
          </dl></section>
          <section><h2>Datos de red</h2><dl className="afr2-status-list">
            <div><dt>Nombre STA</dt><dd>{fresh?wifiStatus.staSsid||'—':'—'}</dd></div><div><dt>Senal actual</dt><dd>{fresh && wifiStatus.rssiDbm!==null?`${wifiStatus.rssiDbm} dBm`:'—'}</dd></div>
            <div><dt>Seguridad STA</dt><dd>{fresh && wifiStatus.secure===true && <Lock/>}{fresh?security(wifiStatus.security):'—'}</dd></div>
            <div><dt>Nombre AP</dt><dd>{fresh?wifiStatus.apSsid||'—':'—'}</dd></div><div><dt>Seguridad AP</dt><dd>{fresh && wifiStatus.apSecurity && !['open','unknown'].includes(wifiStatus.apSecurity) && <Lock/>}{fresh?security(wifiStatus.apSecurity):'—'}</dd></div>
          </dl></section>
          <section><h2>Cambiar modo</h2><label>Opciones de modos <select value={targetMode} disabled={busy || !!goal} onChange={e=>setTargetMode(e.target.value as typeof targetMode)}><option value="STA">STA</option><option value="AP">AP</option><option value="AP_STA">AP-STA</option></select></label><Button disabled={!online || busy || !!goal} onClick={()=>void operate('wifi.mode',{mode:targetMode})}>Aplicar modo</Button></section>
          <div className="afr2-action-row">
            <Button disabled={!online || busy || !!goal} onClick={()=>{setSsid(wifiStatus.staSsid??'');setConfiguring(true);}}>Configurar</Button>
            {fresh && connected?<Button disabled={busy || !!goal} onClick={()=>void operate('wifi.disconnect',{},'disconnect')}>Desconectar</Button>:fresh && <><Button disabled={busy || !!goal} onClick={()=>{setSsid(wifiStatus.staSsid??'');setConfiguring(true);}}>Conectar</Button><Button disabled={busy || !!goal || wifiStatus.mode==='AP'} onClick={()=>void operate('wifi.retry',{},'connect')}>Reintentar</Button></>}
          </div>
        </div>
      </GlassSurface>},
      {id:'redes',label:'Buscar redes',content:<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader title="Redes encontradas" actions={<Button disabled={!online || busy || !!goal || scanning || wifiStatus.scanRunning===true} onClick={scan}><span aria-hidden="true">↻</span> Refrescar</Button>}/>
        {scanning && <StatusNotice tone="info" title="Busqueda en curso…"/>}{wifiScanError && <StatusNotice tone="warning" title={wifiScanError}/>}
        {wifiScanTruncated && <StatusNotice tone="info" title="Se muestran las primeras 16 redes"/>}
        {!scanning && !wifiNetworks.length && <p>Sin redes disponibles en esta sesion.</p>}
        <div className="afr2-page-stack">{wifiNetworks.map((net,index)=><button type="button" key={`${net.ssid}-${net.channel}-${index}`} className="afr2-pressable afr2-surface-cell flex items-center justify-between p-3 text-left" onClick={()=>{setSsid(net.ssid);setPassword('');setTargetMode(wifiStatus.mode==='AP'?'AP_STA':wifiStatus.mode==='STA'?'STA':'AP_STA');setConfiguring(true);}}>
          <span><strong>{net.ssid||'Red oculta'}</strong><br/>{security(net.security??(net.secure?'protected':'open'))} · {net.rssiDbm} dBm</span>{net.secure && <Lock/>}
        </button>)}</div><Button onClick={()=>setView('estado')}>Volver</Button>
      </GlassSurface>}
    ]}/>} 
  </div>;
}
