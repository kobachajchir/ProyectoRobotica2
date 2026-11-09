/* AFR2-20261030-09. Network state and operations confirmed by ESP observations. */
import { useEffect, useRef, useState, type FormEvent } from 'react';
import { TaskSections } from '../../components/ui/TaskSections';
import { GlassSurface } from '../../components/ui/GlassSurface';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { PageHeader } from '../../components/ui/PageHeader';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { WifiCredentialsFields, validWifiCredentials } from './WifiCredentialsFields';
import { useConnection } from '../connection/ConnectionContext';
import { ConnectionDiagnostics } from '../connection/SystemPanel';
import type { Afr2CommandName, WifiStatusSnapshot } from '../../protocol/wsEnvelope';

function Lock(){return <UiIcon name="lock" size={16} label="Red protegida" />;}
const security=(value?:string)=>value==='open'?'Abierta':value==='unknown' || !value?'Sin dato de seguridad':value;
const modeLabel=(mode:string)=>mode==='AP_STA'?'AP-STA':mode;
type Goal={kind:'connect'|'disconnect'|'ap';started:number};
export function WifiPage(){
  const {wsState,wifiStatus,wifiNetworks,wifiScanState,wifiScanError,wifiScanTruncated,refreshWifiStatus,scanWifiNetworks,requestCommand}=useConnection();
  const [view,setView]=useState('estado'),[configuring,setConfiguring]=useState(false);
  const [ssid,setSsid]=useState(''),[password,setPassword]=useState('');
  const [targetMode,setTargetMode]=useState<'STA'|'AP'|'AP_STA'>('STA');
  const [message,setMessage]=useState(''),[failed,setFailed]=useState(false),[busy,setBusy]=useState(false),[goal,setGoal]=useState<Goal|null>(null);
  const mounted=useRef(true),epoch=useRef(0),scanRequested=useRef(false),modeSeeded=useRef(false),modeEdited=useRef(false);
  const connected=wifiStatus.staConnected,scanning=wifiScanState==='scanning',online=wsState==='ready';
  const fresh=online && typeof wifiStatus.receivedAtMs==='number' && Date.now()-wifiStatus.receivedAtMs<5000;
  const modeKnown=online && typeof wifiStatus.receivedAtMs==='number' && ['STA','AP','AP_STA'].includes(wifiStatus.mode);
  useEffect(()=>{
    if(!online){modeSeeded.current=false;modeEdited.current=false;return;}
    if(!modeKnown)return;
    if(wifiStatus.mode===targetMode)modeEdited.current=false;
    if(!modeEdited.current){modeSeeded.current=true;setTargetMode(wifiStatus.mode as 'STA'|'AP'|'AP_STA');}
  },[online,modeKnown,wifiStatus.mode,targetMode]);
  useEffect(()=>{mounted.current=true;return()=>{mounted.current=false;++epoch.current;};},[]);
  useEffect(()=>{if(scanRequested.current && wifiScanState==='completed'){scanRequested.current=false;setView('redes');setMessage(wifiNetworks.length?'Redes listas.':'No se encontraron redes.');setFailed(false);}},[wifiScanState,wifiNetworks.length]);
  useEffect(()=>{
    if(!goal)return;
    let cancelled=false,timer:ReturnType<typeof setTimeout>|undefined;
    const poll=async()=>{
      if(cancelled)return;
      if(wsState!=='ready'){setMessage('La conexión Web se interrumpio. El resultado Wi-Fi sigue sin confirmar; reconecta y consulta Estado de red.');setGoal(null);return;}
      try{
        const res=await requestCommand<WifiStatusSnapshot>('wifi.status',{},2500);
        if(cancelled)return;
        const status=res.payload,confirmed=res.status==='completed' && (goal.kind==='disconnect'?status.staConnected===false:goal.kind==='ap'?status.mode==='AP' && !!status.apIp && status.apIp!=='0.0.0.0':status.staConnected===true);
        if(confirmed){setMessage(goal.kind==='disconnect'?'Operación exitosa: red desconectada.':goal.kind==='ap'?'Operación exitosa: AP activo.':'Operación exitosa: red conectada.');setFailed(false);setGoal(null);void refreshWifiStatus();return;}
      }catch{if(cancelled)return;}
      if(Date.now()-goal.started>(goal.kind==='disconnect'?5000:25000)){setMessage('No se confirmó el estado esperado dentro del plazo. Consulta Estado de red.');setFailed(true);setGoal(null);return;}
      timer=setTimeout(()=>void poll(),700);
    };
    void poll();return()=>{cancelled=true;if(timer)clearTimeout(timer);};
  },[goal,wsState,requestCommand,refreshWifiStatus]);
  const operate=async(command:Afr2CommandName,payload:Record<string,unknown>,next?:Goal['kind'])=>{
    if(busy || goal)return;const ticket=++epoch.current;setBusy(true);setMessage('Solicitud enviada; esperando confirmación del ESP.');setFailed(false);
    try{
      const res=await requestCommand<Record<string,unknown>>(command,payload,4000);
      if(!mounted.current || ticket!==epoch.current)return;
      if(res.payload.restartScheduled===true){setGoal(null);setConfiguring(false);setMessage('Configuración guardada. ESP reiniciando; volvé a la red elegida y consultá su dirección HTTP.');}
      else if(next){setGoal({kind:next,started:Date.now()});setMessage(next==='disconnect'?'Desconectando…':'Conectando…');}
      else if(res.status==='completed' && res.payload.confirmed===true){setMessage(command==='wifi.ap.stop'?'AP detenido.':command==='wifi.ap.fallback'?'Opción AP-fallback guardada.':'Operación exitosa: modo actualizado.');void refreshWifiStatus();}
      else setMessage('Operación aceptada; consulta el estado para confirmar el resultado.');
    }catch(e){if(mounted.current && ticket===epoch.current){setMessage(e instanceof Error?e.message:'Sin confirmación del ESP.');setFailed(true);if(command==='wifi.mode'){setConfiguring(true);setPassword('');}}}
    finally{if(mounted.current && ticket===epoch.current)setBusy(false);}
  };
  const canScan=modeKnown && wifiStatus.mode!=='AP';
  useEffect(()=>{if(!canScan && view==='redes')setView('estado');},[canScan,view]);
  const scan=()=>{if(!canScan)return;modeEdited.current=true;scanRequested.current=true;setMessage('Buscando redes…');setView('redes');void scanWifiNetworks();};
  const configure=async(e:FormEvent)=>{
    e.preventDefault();if(!validWifiCredentials(ssid,password))return;
    await operate('wifi.configure',{mode:targetMode,ssid,password,persist:true},targetMode==='AP'?'ap':'connect');setPassword('');
  };
  const config=<GlassSurface variant="panel" className="p-5 sm:p-6">
    <SectionHeader title="Modificar credenciales" description="El cambio puede interrumpir esta conexión Web. La aceptación no confirma una conexión nueva."/>
    <form onSubmit={e=>void configure(e)} className="afr2-page-stack">
      <WifiCredentialsFields id="wifi-cfg" mode={targetMode} ssid={ssid} password={password} onSsid={setSsid} onPassword={setPassword}/>
      <div className="afr2-action-row"><Button type="submit" disabled={!online || busy || !!goal || scanning || !validWifiCredentials(ssid,password)}>Guardar y reiniciar</Button><Button type="button" onClick={()=>{setConfiguring(false);setPassword('');}}>Volver</Button></div>
    </form>
  </GlassSurface>;
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader title="Conexión" description="Wi-Fi, punto de acceso y enlace entre el ESP y el ATmega."/>
    <div className="afr2-wifi-mode-card afr2-glass-panel">
      <SectionHeader title="Cambiar modo" description={modeKnown?`Modo actual: ${modeLabel(wifiStatus.mode)}`:'Esperando el modo actual del ESP.'}/>
      <div className="afr2-action-row">
        <label className="afr2-wifi-mode-choice">Opciones de modos <select className="afr2-glass-field" value={modeKnown?targetMode:''} disabled={!modeKnown || busy || !!goal} onChange={e=>{modeEdited.current=true;setTargetMode(e.target.value as typeof targetMode);}}><option value="" disabled>Sin dato actual</option><option value="STA">STA</option><option value="AP">AP</option><option value="AP_STA">AP-STA</option></select></label>
        <Button disabled={!modeKnown || busy || !!goal} onClick={()=>void operate('wifi.mode',{mode:targetMode})}>Aplicar modo</Button>
        <Button disabled={!modeKnown || busy || !!goal} onClick={()=>{setSsid(wifiStatus.staSsid??'');setConfiguring(true);}}>Configurar</Button>
      </div>
    </div>
    {message && <StatusNotice tone={failed?'warning':goal?'info':'neutral'} title={message}/>}
    {configuring?config:<TaskSections label="Secciones de Conexión" value={canScan?view:view==='redes'?'estado':view} onValueChange={next=>{setView(next);if(next==='redes' && wifiScanState==='idle')scan();}} items={[
      {id:'estado',label:'Estado de red',content:<GlassSurface variant="panel" className="p-5 sm:p-6">
        {!fresh?<div className="afr2-wifi-empty" role="status">
          <span className="afr2-wifi-empty__icon" aria-hidden="true"><UiIcon name="wifi" size={56}/></span>
          <p className="afr2-wifi-empty__title">Sin datos</p>
          <Button disabled={!online} onClick={()=>void refreshWifiStatus()}>Actualizar estado</Button>
        </div>:<>
        <SectionHeader title={connected?'Conectado':'Desconectado'} description="Estado de la conexión STA; el punto de acceso AP se informa por separado." actions={<Button disabled={!online} onClick={()=>void refreshWifiStatus()}>Actualizar estado</Button>}/>
        <div className="afr2-action-row">
          <label><input type="checkbox" checked={wifiStatus.askFallback===true} disabled={busy || !!goal || typeof wifiStatus.askFallback!=='boolean'} onChange={e=>void operate('wifi.ap.fallback',{ask:e.target.checked})}/> Preguntar antes de crear AP si falla Wi-Fi</label>
          {wifiStatus.apReady && <Button disabled={busy || !!goal} onClick={()=>void operate('wifi.ap.stop',{})}>Detener AP</Button>}
        </div>
        {wifiStatus.fallbackActive && <StatusNotice tone="info" title="AP / fallback activo"/>}
        {wifiStatus.staNetwork === 'previous' && <StatusNotice tone="info" title={wifiStatus.staConnected ? 'Conectado a la red anterior' : 'Probando la red anterior'}/>}
        {wifiStatus.configError && <StatusNotice tone="warning" title={wifiStatus.configError}/>}
        <div className="afr2-page-stack">
          <section><h3 className="afr2-subheading">Estado de conexión</h3><dl className="afr2-status-list">
            <div><dt>Modo</dt><dd>{fresh?modeLabel(wifiStatus.mode):'—'}</dd></div><div><dt>IP STA</dt><dd>{fresh?wifiStatus.staIp||'—':'—'}</dd></div>
            <div><dt>Mascara STA</dt><dd>{fresh?wifiStatus.staMask||'—':'—'}</dd></div><div><dt>Puerta de enlace STA</dt><dd>{fresh?wifiStatus.staGateway||'—':'—'}</dd></div>
            <div><dt>IP AP</dt><dd>{fresh?wifiStatus.apIp||'—':'—'}</dd></div><div><dt>Mascara AP</dt><dd>{fresh?wifiStatus.apMask||'—':'—'}</dd></div>
            <div><dt>Puerta de enlace AP</dt><dd>{fresh?wifiStatus.apGateway||'—':'—'}</dd></div>
          </dl></section>
          <section><h3 className="afr2-subheading">Datos de red</h3><dl className="afr2-status-list">
            <div><dt>Nombre STA</dt><dd>{fresh?wifiStatus.staSsid||'—':'—'}</dd></div><div><dt>Señal actual</dt><dd>{fresh && wifiStatus.rssiDbm!==null?`${wifiStatus.rssiDbm} dBm`:'—'}</dd></div>
            <div><dt>Seguridad STA</dt><dd>{fresh && wifiStatus.secure===true && <Lock/>}{fresh?security(wifiStatus.security):'—'}</dd></div>
            <div><dt>Nombre AP</dt><dd>{fresh?wifiStatus.apSsid||'—':'—'}</dd></div><div><dt>Seguridad AP</dt><dd>{fresh && wifiStatus.apSecurity && !['open','unknown'].includes(wifiStatus.apSecurity) && <Lock/>}{fresh?security(wifiStatus.apSecurity):'—'}</dd></div>
          </dl></section>
          <div className="afr2-action-row">
            {fresh && connected?<Button disabled={busy || !!goal} onClick={()=>void operate('wifi.disconnect',{},'disconnect')}>Desconectar</Button>:fresh && <><Button disabled={busy || !!goal} onClick={()=>{setSsid(wifiStatus.staSsid??'');setConfiguring(true);}}>Configurar Wi-Fi</Button><Button disabled={busy || !!goal || wifiStatus.mode==='AP'} onClick={()=>void operate('wifi.retry',{},'connect')}>Reintentar</Button></>}
          </div>
        </div>
        </>}
      </GlassSurface>},
      ...(canScan?[{id:'redes',label:'Buscar redes',content:<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader title={scanning?'Buscando redes…':wifiScanState==='completed' && wifiNetworks.length?'Redes encontradas':'Buscar redes'} actions={<Button disabled={!online || busy || !!goal || scanning || wifiStatus.scanRunning===true} onClick={scan} icon={<UiIcon name="refresh" />}>Refrescar</Button>}/>
        {scanning && <StatusNotice tone="info" title="Búsqueda en curso…"/>}{wifiScanError && <StatusNotice tone="warning" title={wifiScanError}/>}
        {wifiScanTruncated && <StatusNotice tone="info" title="Se muestran hasta 16 redes de mayor señal; puede haber otras"/>}
        {!scanning && !wifiNetworks.length && <p>{wifiScanState==='completed'?'No se encontraron redes.':wifiScanState==='failed'?'La búsqueda falló. Podés refrescar.':'Iniciá una búsqueda.'}</p>}
        <div className="afr2-page-stack">{wifiNetworks.map((net,index)=><button type="button" key={`${net.ssid}-${net.channel}-${index}`} className="afr2-pressable afr2-surface-cell flex items-center justify-between p-3 text-left" onClick={()=>{setSsid(net.ssid);setPassword('');modeEdited.current=true;setTargetMode(targetMode==='AP'?'STA':targetMode);setConfiguring(true);}}>
          <span><strong>{net.ssid||'Red oculta'}</strong><br/>{security(net.security??(net.secure?'protected':'open'))} · {net.rssiDbm} dBm</span>{net.secure && <Lock/>}
        </button>)}</div><Button onClick={()=>setView('estado')}>Volver</Button>
      </GlassSurface>}]:[]),
      // AFR2-20261101-12: detalles de la conexión y WebSocket, antes en Configuración → Sistema.
      {id:'diagnostico',label:'Diagnóstico',content:<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader title="Diagnóstico" description="Enlaces Web–ESP–ATmega y sesiones WebSocket observadas."/>
        <ConnectionDiagnostics/>
      </GlassSurface>}
    ]}/>} 
  </div>;
}
