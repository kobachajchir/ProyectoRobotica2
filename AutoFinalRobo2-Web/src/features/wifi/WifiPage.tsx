/* AFR2-20261030-09. Network state and operations confirmed by ESP observations. */
import { useCallback, useEffect, useRef, useState, type FormEvent } from 'react';
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
import { WifiCredentialsDialog, type WifiCredentialsKind } from './WifiCredentialsDialog';

function Lock(){return <UiIcon name="lock" size={16} label="Red protegida" />;}
const security=(value?:string)=>value==='open'?'Abierta':value==='unknown' || !value?'Sin dato de seguridad':value;
const modeLabel=(mode:string)=>mode==='AP_STA'?'AP-STA':mode;
type Goal={kind:'connect'|'disconnect'|'ap'|'mode';started:number;mode?:string;ssid?:string;previousBootId?:number;restart?:boolean};
export function WifiPage(){
  const {wsState,wifiStatus,wifiNetworks,wifiScanState,wifiScanError,wifiScanTruncated,refreshWifiStatus,scanWifiNetworks,requestCommand}=useConnection();
  const [view,setView]=useState('estado'),[configuring,setConfiguring]=useState(false);
  const [ssid,setSsid]=useState(''),[password,setPassword]=useState('');
  const [targetMode,setTargetMode]=useState<'STA'|'AP'|'AP_STA'>('STA');
  const [configMode,setConfigMode]=useState<'STA'|'AP'|'AP_STA'>('STA');
  const [credentialsKind,setCredentialsKind]=useState<WifiCredentialsKind|null>(null);
  const closeCredentials=useCallback(()=>setCredentialsKind(null),[]);
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
      if(Date.now()-goal.started>35000){setMessage('No se confirmó el cambio. Volvé a conectar a la red elegida y consultá Estado de red.');setFailed(true);setGoal(null);return;}
      if(wsState!=='ready'){setMessage('Esperando reconexión. Volvé al AP del auto o a la nueva red Wi-Fi para confirmar el resultado.');timer=setTimeout(()=>void poll(),1000);return;}
      try{
        const res=await requestCommand<WifiStatusSnapshot>('wifi.status',{},2500);
        if(cancelled)return;
        const status=res.payload;
        const bootConfirmed=!goal.restart || typeof goal.previousBootId==='number' && typeof status.espBootId==='number' && status.espBootId!==goal.previousBootId;
        const modeConfirmed=!goal.mode || status.mode===goal.mode;
        const expected=goal.kind==='disconnect'?status.staConnected===false:goal.kind==='ap'?status.apReady===true && !!status.apIp && (!goal.ssid || status.apSsid===goal.ssid):goal.kind==='mode'?true:status.staConnected===true && (!goal.ssid || status.staSsid===goal.ssid);
        const confirmed=res.status==='completed' && bootConfirmed && modeConfirmed && expected;
        if(confirmed){setMessage(goal.kind==='disconnect'?'Red desconectada.':goal.kind==='ap'?'Reinicio confirmado: AP activo.':goal.kind==='mode'?'Reinicio y modo confirmados por el ESP.':'Conexión a la red elegida confirmada por el ESP.');setFailed(false);setGoal(null);void refreshWifiStatus();return;}
      }catch{if(cancelled)return;}
      if(Date.now()-goal.started>(goal.kind==='disconnect'?5000:35000)){setMessage('No se confirmó el estado esperado dentro del plazo. Consultá Estado de red.');setFailed(true);setGoal(null);return;}
      timer=setTimeout(()=>void poll(),700);
    };
    void poll();return()=>{cancelled=true;if(timer)clearTimeout(timer);};
  },[goal,wsState,requestCommand,refreshWifiStatus]);
  const operate=async(command:Afr2CommandName,payload:Record<string,unknown>,next?:Goal['kind'])=>{
    if(busy || goal)return;const ticket=++epoch.current;setBusy(true);setMessage('Solicitud enviada; esperando confirmación del ESP.');setFailed(false);
    try{
      const res=await requestCommand<Record<string,unknown>>(command,payload,4000);
      if(!mounted.current || ticket!==epoch.current)return;
      if(res.payload.restartScheduled===true){setGoal({kind:next??'mode',started:Date.now(),mode:typeof payload.mode==='string'?payload.mode:undefined,ssid:typeof payload.ssid==='string'?payload.ssid:undefined,previousBootId:wifiStatus.espBootId,restart:true});setConfiguring(false);setPassword('');setMessage('Configuración guardada y reinicio programado. Si cambia el AP, conectate a su nuevo nombre. Esperando confirmar el arranque.');}
      else if(next){setGoal({kind:next,started:Date.now(),ssid:typeof payload.ssid==='string'?payload.ssid:undefined});setMessage(next==='disconnect'?'Desconectando…':'Conectando…');}
      else if(res.status==='completed' && res.payload.confirmed===true){setMessage(command==='wifi.ap.stop'?'AP detenido.':command==='wifi.ap.fallback'?'Opción AP-fallback guardada.':'Operación exitosa: modo actualizado.');void refreshWifiStatus();}
      else setMessage('Operación aceptada; consulta el estado para confirmar el resultado.');
    }catch(e){if(mounted.current && ticket===epoch.current){setMessage(e instanceof Error?e.message:'Sin confirmación del ESP.');setFailed(true);if(command==='wifi.mode'){
      const mode=payload.mode==='AP'?'AP':payload.mode==='AP_STA'?'AP_STA':'STA';
      setConfigMode(mode);setSsid(mode==='AP'?wifiStatus.apSsid??'':wifiStatus.staSsid??'');setConfiguring(true);setPassword('');
    }}}
    finally{if(mounted.current && ticket===epoch.current)setBusy(false);}
  };
  const canScan=modeKnown;
  useEffect(()=>{if(!canScan && view==='redes')setView('estado');},[canScan,view]);
  const scan=()=>{if(!canScan || busy || goal || scanning)return;scanRequested.current=true;setMessage('Buscando redes sin apagar el AP…');setView('redes');void scanWifiNetworks();};
  const openConfig=(mode:'STA'|'AP'|'AP_STA')=>{setConfigMode(mode);setSsid(mode==='AP'?wifiStatus.apSsid??'':wifiStatus.staSsid??'');setPassword('');setConfiguring(true);};
  const configure=async(e:FormEvent)=>{
    e.preventDefault();if(!validWifiCredentials(ssid,password))return;
    await operate('wifi.configure',{mode:configMode,ssid,password,persist:true},configMode==='AP'?'ap':'connect');setPassword('');
  };
  const config=<GlassSurface variant="panel" className="p-5 sm:p-6">
    <SectionHeader title={configMode==='AP'?'Configurar el AP del auto':'Conectar el auto a una red Wi-Fi'} description="Guardar programa un reinicio del ESP. La aceptación no confirma el nuevo arranque ni la conexión."/>
    <form onSubmit={e=>void configure(e)} className="afr2-page-stack">
      {configMode!=='AP' && <label className="afr2-wifi-mode-choice">Después de reiniciar<select className="afr2-glass-field" disabled={busy || !!goal} value={configMode} onChange={e=>setConfigMode(e.target.value as 'STA'|'AP_STA')}><option value="AP_STA">Conectar y mantener el AP (AP-STA)</option><option value="STA">Solo la red del router (STA)</option></select></label>}
      <fieldset disabled={busy || !!goal || scanning} className="contents"><WifiCredentialsFields id="wifi-cfg" mode={configMode} ssid={ssid} password={password} onSsid={setSsid} onPassword={setPassword}/></fieldset>
      {configMode==='AP' && <p className="afr2-help">El nombre y la contraseña corresponden al AP que crea el auto. Si dejás la contraseña vacía se creará una red abierta. La red del router se conserva por separado.</p>}
      <div className="afr2-action-row"><Button type="submit" disabled={!online || busy || !!goal || scanning || !validWifiCredentials(ssid,password)}>Guardar y reiniciar</Button><Button type="button" onClick={()=>{setConfiguring(false);setPassword('');}}>Volver</Button></div>
    </form>
  </GlassSurface>;
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader title="Conexión" description="Wi-Fi, punto de acceso y enlace entre el ESP y el ATmega."/>
    <div className="afr2-wifi-mode-card afr2-glass-panel">
      <SectionHeader title="Cambiar modo" description={modeKnown?`Modo actual: ${modeLabel(wifiStatus.mode)}`:'Esperando el modo actual del ESP.'}/>
      <div className="afr2-action-row">
        <label className="afr2-wifi-mode-choice">Opciones de modos <select className="afr2-glass-field" value={modeKnown?targetMode:''} disabled={!modeKnown || busy || !!goal} onChange={e=>{modeEdited.current=true;setTargetMode(e.target.value as typeof targetMode);}}><option value="" disabled>Sin dato actual</option><option value="STA">STA</option><option value="AP">AP</option><option value="AP_STA">AP-STA</option></select></label>
        <Button disabled={!modeKnown || busy || !!goal || scanning} onClick={()=>void operate('wifi.mode',{mode:targetMode})}>Aplicar modo</Button>
        <Button disabled={!modeKnown || busy || !!goal || scanning} onClick={()=>openConfig(targetMode)}>Configurar modo elegido</Button>
        <Button disabled={!online || busy || !!goal || scanning} onClick={()=>openConfig('AP')}>Configurar AP</Button>
        <Button disabled={!online || busy || !!goal || scanning} onClick={()=>void operate('wifi.mode',{mode:'AP'},'ap')}>Reiniciar en modo AP</Button>
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
            {fresh && connected?<Button disabled={busy || !!goal} onClick={()=>void operate('wifi.disconnect',{},'disconnect')}>Desconectar</Button>:fresh && <><Button disabled={busy || !!goal || scanning} onClick={()=>openConfig(wifiStatus.mode==='STA'?'STA':'AP_STA')}>Configurar Wi-Fi</Button><Button disabled={busy || !!goal || wifiStatus.mode==='AP'} onClick={()=>void operate('wifi.retry',{},'connect')}>Reintentar</Button></>}
          </div>
        </div>
        </>}
      </GlassSurface>},
      ...(canScan?[{id:'redes',label:'Buscar redes',content:<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader title={scanning?'Buscando redes…':wifiScanState==='completed' && wifiNetworks.length?'Redes encontradas':'Buscar redes'} actions={<Button disabled={!online || busy || !!goal || scanning || wifiStatus.scanRunning===true} onClick={scan} icon={<UiIcon name="refresh" />}>Refrescar</Button>}/>
        {wifiStatus.mode==='AP' && <p className="afr2-help">Podés buscar desde el AP del auto. Al elegir una red se propone AP-STA para conservar el punto de acceso después de conectar.</p>}
        {scanning && <StatusNotice tone="info" title="Búsqueda en curso…"/>}{wifiScanError && <StatusNotice tone="warning" title={wifiScanError}/>}
        {wifiScanTruncated && <StatusNotice tone="info" title="Se muestran hasta 16 redes de mayor señal; puede haber otras"/>}
        {!scanning && !wifiNetworks.length && <p>{wifiScanState==='completed'?'No se encontraron redes.':wifiScanState==='failed'?'La búsqueda falló. Podés refrescar.':'Iniciá una búsqueda.'}</p>}
        <div className="afr2-page-stack">{wifiNetworks.map((net,index)=><button type="button" disabled={!online || busy || !!goal || scanning} key={`${net.ssid}-${net.channel}-${index}`} className="afr2-pressable afr2-surface-cell flex items-center justify-between p-3 text-left" onClick={()=>{const mode=wifiStatus.mode==='STA'?'STA':'AP_STA';setSsid(net.ssid);setPassword('');setConfigMode(mode);modeEdited.current=true;setTargetMode(mode);setConfiguring(true);}}>
          <span><strong>{net.ssid||'Red oculta'}</strong><br/>{security(net.security??(net.secure?'protected':'open'))} · {net.rssiDbm} dBm</span>{net.secure && <Lock/>}
        </button>)}</div><Button onClick={()=>setView('estado')}>Volver</Button>
      </GlassSurface>}]:[]),
      // AFR2-20261101-12: detalles de la conexión y WebSocket, antes en Configuración → Sistema.
      {id:'diagnostico',label:'Diagnóstico',content:<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader title="Diagnóstico" description="Enlaces Web–ESP–ATmega y sesiones WebSocket observadas."/>
        <ConnectionDiagnostics/>
      </GlassSurface>}
    ]}/>}
    <GlassSurface variant="panel" className="p-5 sm:p-6"><SectionHeader title="Credenciales guardadas en el auto" description="Consultá la última red que logró conectar o la configuración del punto de acceso."/><div className="afr2-action-row"><Button disabled={!online || busy || !!goal} onClick={()=>setCredentialsKind('last-sta')}>Ver última red Wi-Fi conectada</Button><Button disabled={!online || busy || !!goal} onClick={()=>setCredentialsKind('ap')}>Ver configuración del AP</Button></div></GlassSurface>
    <WifiCredentialsDialog kind={credentialsKind} onClose={closeCredentials} onUse={data=>{if(!data.ssid)return;const mode=data.kind==='ap'?'AP':wifiStatus.mode==='STA'?'STA':'AP_STA';setConfigMode(mode);setSsid(data.ssid);setPassword(data.password??'');setConfiguring(true);}}/>
  </div>;
}
