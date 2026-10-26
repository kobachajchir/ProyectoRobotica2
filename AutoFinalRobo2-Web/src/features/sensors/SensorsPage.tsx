import { useEffect,useRef,useState } from 'react';
import { useSearchParams } from 'react-router-dom';
import { TaskSections } from '../../components/ui/TaskSections';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { Button } from '../../components/ui/Button';
import { Disclosure } from '../../components/ui/Disclosure';
import { RenderBoundary } from '../../components/ui/RenderBoundary';
import { UiIcon } from '../../components/ui/UiIcon';
import { useTheme } from '../../app/providers/ThemeProvider';
import { useConnection } from '../connection/ConnectionContext';
import { SENSOR_ERRORS,type IrReading,type RadarConfig,type RadarReply,type RangeSample } from '../../protocol/sensorHead';
import type { Afr2CommandName } from '../../protocol/wsEnvelope';
import { useSensorPoll } from './useSensorPoll';
import { RadarScene, type RadarView } from './RadarScene';
import { ScanAngleRange, ScanRangeField } from './ScanControls';

type StoredPoint=RangeSample & {receivedAtMs:number};
const DEFAULT_CONFIG:RadarConfig={minAngleDeg:0,maxAngleDeg:180,speedDegS:10,settleMs:100,intervalMs:70,inverted:false};
function DistanceWorkspace(){
  const {headReading:head,wsState,megaLinkState,requestCommand,stopRecord}=useConnection();const {resolvedTheme}=useTheme();
  const [points,setPoints]=useState<Record<number,StoredPoint>>({}),[range,setRange]=useState(1),[now,setNow]=useState(Date.now());
  const [view,setView]=useState<RadarView>('perspective');
  const [config,setConfig]=useState<RadarConfig|null>(null),[draft,setDraft]=useState(DEFAULT_CONFIG),[angle,setAngle]=useState(90);
  const [busy,setBusy]=useState(false),[feedback,setFeedback]=useState<string|null>(null),[lost,setLost]=useState(0),[mapReady,setMapReady]=useState(false);
  const cursor=useRef({boot:0,scan:0,snapshot:0,sequence:0}),manual=useRef({sending:false,target:null as number|null,epoch:0});
  const online=wsState==='ready' && megaLinkState==='ready';
  useEffect(()=>{const timer=setInterval(()=>setNow(Date.now()),250);return()=>clearInterval(timer);},[]);
  useEffect(()=>{if(head)setAngle(v=>manual.current.sending || manual.current.target!==null?v:head.angleDeg);},[head]);
  useEffect(()=>{
    if(!online)return;let cancelled=false;
    void requestCommand<RadarConfig>('robot.radar.configure',{write:false}).then(r=>{if(!cancelled){setConfig(r.payload);setDraft(r.payload);}}).catch(()=>{});
    return()=>{cancelled=true;};
  },[online,requestCommand,head?.bootId]);
  useEffect(()=>{if(head && cursor.current.boot!==head.bootId){cursor.current={boot:head.bootId,scan:head.scanRevision,snapshot:0,sequence:0};setPoints({});setMapReady(false);setLost(0);}},[head?.bootId]);
  useEffect(()=>{cursor.current.snapshot=0;setMapReady(false);},[online]);
  useEffect(()=>{if(head && head.scanRevision!==cursor.current.scan){cursor.current.scan=head.scanRevision;cursor.current.snapshot=0;cursor.current.sequence=0;setPoints({});setMapReady(false);}},[head?.scanRevision]);
  useEffect(()=>{manual.current.epoch++;manual.current.target=null;setBusy(false);},[stopRecord.lastIssuedAtMs,online]);
  useEffect(()=>{const cancel=()=>{manual.current.epoch++;manual.current.target=null;};window.addEventListener('blur',cancel);return()=>{cancel();window.removeEventListener('blur',cancel);};},[]);
  useSensorPoll<RadarReply>('robot.radar.get',100,r=>{
    const c=cursor.current;if(r.bootId!==c.boot){cursor.current={boot:r.bootId,scan:r.scanRevision,snapshot:0,sequence:0};setPoints({});setMapReady(false);return;}
    if(r.scanRevision!==c.scan){if(((r.scanRevision-c.scan)&255)<128){c.scan=r.scanRevision;c.snapshot=0;c.sequence=0;setPoints({});setMapReady(false);}return;}
    if(r.dropped){setLost(n=>n+r.dropped);c.snapshot=0;setMapReady(false);}
    for(const p of r.samples)if(p.sampleId && (!c.sequence || ((p.sampleId-c.sequence)|0)>0))c.sequence=p.sampleId;
    setPoints(previous=>{const result={...previous};for(const p of r.samples)if(p.sampleId){
      const old=result[p.angleDeg];if(!old || ((p.sampleId-old.sampleId)|0)>0)result[p.angleDeg]={...p,receivedAtMs:Date.now()};
    }return result;});
    if(r.mode==='snapshot'){c.snapshot=r.nextCursor;if(c.snapshot>=37)setMapReady(true);}else c.sequence=r.nextCursor;
  },()=>cursor.current.snapshot<37?{mode:'snapshot',cursor:cursor.current.snapshot}:{mode:'delta',cursor:cursor.current.sequence});
  const action=async(command:Afr2CommandName,payload:Record<string,unknown>,message:string)=>{
    setBusy(true);setFeedback(null);try{const r=await requestCommand<RadarConfig>(command,payload);if(command==='robot.radar.configure'){setConfig(r.payload);setDraft(r.payload);}if(command==='robot.radar.start'){cursor.current.snapshot=0;cursor.current.sequence=0;setPoints({});setMapReady(false);}setFeedback(message);}
    catch(e){setFeedback(e instanceof Error?e.message:'No se confirmo la accion.');}finally{setBusy(false);}
  };
  const move=(value:number)=>{
    setAngle(value);manual.current.target=value;if(manual.current.sending)return;const epoch=manual.current.epoch;
    const drain=async()=>{manual.current.sending=true;
      try{while(manual.current.target!==null && epoch===manual.current.epoch){const target=manual.current.target;manual.current.target=null;await requestCommand('robot.servo.set',{operation:'angle',angleDeg:target});await new Promise(resolve=>setTimeout(resolve,100));}}
      catch(e){setFeedback(e instanceof Error?e.message:'No se confirmo el angulo.');manual.current.target=null;}
      finally{manual.current.sending=false;}
    };void drain();
  };
  const fresh=online && !!head && now-head.receivedAtMs<1000;
  const sample=head?.sample,age=sample?.ageMs===null || sample?.ageMs===undefined?null:sample.ageMs+Math.max(0,now-(head?.receivedAtMs??now));
  const maxAge=Math.max(200,2*(head?.intervalMs??70)+32,head?.running && config?2*Math.max(5000/config.speedDegS,config.settleMs+32,config.intervalMs):0);
  const sampleFresh=fresh && sample?.valid && age!==null && age<=maxAge;
  const running=head?.running===true,enabled=head?.enabled===true,values=Object.values(points),configured=config??draft;
  const plotPoints=sampleFresh && sample!.pulseUs>0?[...values.filter(p=>p.angleDeg!==sample!.angleDeg),sample!]:values;
  const effective=5000/Math.max(5000/configured.speedDegS,configured.settleMs+32,configured.intervalMs);
  const configLocked=running || enabled || busy || !online;
  const pending=!!config && (Object.keys(DEFAULT_CONFIG) as (keyof RadarConfig)[]).some(key=>draft[key]!==config[key]);
  const preview=!config || draft.minAngleDeg!==config.minAngleDeg || draft.maxAngleDeg!==config.maxAngleDeg;
  const echoCount=values.filter(p=>p.valid).length;
  const outOfScale=plotPoints.filter(p=>p.valid && (p.distanceMm??0)>range*1000).length;
  const readingStatus=!online?'Sin conexion':!fresh?'Esperando telemetria':sampleFresh?'Eco valido':sample?.valid?'Medicion sin actualizar':sample?SENSOR_ERRORS[sample.error]:'Esperando medicion';
  return <section className="afr2-control-map afr2-radar-map" aria-label="Distancia y servo">
    <header className="afr2-control-map__heading">
      <div><p className="afr2-kicker"><UiIcon name="radar"/> Distancia y servo</p><h2>Radar frontal</h2><p className="afr2-radar-intro">Explora el espacio delante del auto.</p></div>
      <span className={`afr2-map-status${fresh && running?' is-live':''}`}>{!fresh?echoCount?'Mapa historico':online?'Esperando datos':'Sin conexion':running?'Barrido en curso':'Lectura fija'}</span>
    </header>
    <div className="afr2-control-map__toolbar">
      <div className="afr2-radar-toolgroup"><span>Alcance</span><div className="afr2-map-views" role="group" aria-label="Alcance del radar">
        {[.5,1,2,4].map(v=><button key={v} type="button" aria-pressed={range===v} onClick={()=>setRange(v)}>{v} m</button>)}
      </div></div>
      <div className="afr2-map-views" role="group" aria-label="Vista del radar">
        <button type="button" aria-pressed={view==='perspective'} onClick={()=>setView('perspective')}><UiIcon name="cube"/> Perspectiva</button>
        <button type="button" aria-pressed={view==='top'} onClick={()=>setView('top')}><UiIcon name="radar"/> Superior</button>
      </div>
    </div>
    <div className="afr2-control-map__viewport afr2-radar-viewport">
      <div className="afr2-radar-canvas"><RenderBoundary key={`${range}-${resolvedTheme}`} fallback={<p className="afr2-map-fallback">No se pudo mostrar el radar 3D. Las mediciones y controles siguen disponibles.</p>}>
        <RadarScene range={range} angle={head?.angleDeg??null} enabled={enabled} points={plotPoints} currentId={sampleFresh?sample!.sampleId:0}
          dark={resolvedTheme==='dark'} historical={!fresh || !running} scanMin={draft.minAngleDeg} scanMax={draft.maxAngleDeg} preview={preview} view={view}/>
      </RenderBoundary></div>
      <div className="afr2-radar-zone-label"><span>{preview?'Vista previa · sin aplicar':'Sector de escaneo'}</span><strong>{draft.minAngleDeg}° <span aria-hidden="true">—</span> {draft.maxAngleDeg}°</strong></div>
      <div className="afr2-radar-bearing" aria-hidden="true"><UiIcon name="arrow"/><span>Frente<small>90°</small></span></div>
      {!fresh && !echoCount && <div className="afr2-radar-empty"><UiIcon name="sensors"/><div><strong>Esperando al robot</strong><span>Los ecos apareceran al recibir mediciones.</span></div></div>}
      <div className="afr2-map-caption"><span className="afr2-radar-legend"><i/> Eco medido</span><span>Anillos de {range/4} m</span></div>
    </div>
    <aside className="afr2-map-sidebar afr2-radar-sidebar" aria-label="Mediciones y control del servo">
      <div className="afr2-radar-overview">
        <span className="afr2-radar-eyebrow">Distancia medida</span>
        <div className="afr2-radar-distance"><strong>{sampleFresh?sample!.distanceMm:'—'}</strong><span>mm</span></div>
        <p className="afr2-radar-reading-state" data-valid={sampleFresh || undefined}><i/>{readingStatus}</p>
        <div className="afr2-radar-secondary">
          <div><span>Angulo aplicado</span><strong>{fresh && enabled?head!.angleDeg:'—'}<small>°</small></strong></div>
          <div><span>Posiciones con eco</span><strong>{mapReady || echoCount?echoCount:'—'}<small>/ 37</small></strong></div>
        </div>
      </div>
      <div className="afr2-radar-manual">
        <ScanRangeField label="Posicion solicitada" value={angle} min={0} max={180} step={1} unit="°" disabled={!fresh || running || busy} onChange={move}/>
        <div className="afr2-action-row">
          <Button size="sm" disabled={!fresh || running || busy} onClick={()=>void action('robot.servo.set',{operation:'center'},'Centrado solicitado; esperando PWM aplicado.')}>Centrar servo</Button>
          <Button size="sm" disabled={!online || busy || running} onClick={()=>void action('robot.servo.set',{operation:'off'},'PWM desactivado.')}>Desactivar</Button>
        </div>
      </div>
      <div className="afr2-radar-switch">
        <div><strong><UiIcon name="radar"/> Barrido automatico</strong><p className="afr2-help">{pending?'Aplica los parametros antes de iniciar.':'Recorre el sector y mide en cada paso.'}</p></div>
        <Button variant={running?'secondary':'primary'} disabled={!online || !head || busy || (!running && pending)} onClick={()=>{
          if(!running){cursor.current.snapshot=0;setMapReady(false);}
          void action(running?'robot.radar.stop':'robot.radar.start',{},running?'Radar detenido y PWM desactivado.':'Barrido autonomo iniciado.');
        }}>{running?'Detener barrido':'Iniciar barrido'}</Button>
        {running && <p className="afr2-help">Continua al salir de esta pantalla o desconectar la Web.</p>}
      </div>
      <div className="afr2-radar-diagnostics"><Disclosure title="Detalles de la medicion">
        <dl><div><dt>Angulo de la muestra</dt><dd>{sampleFresh && sample!.pulseUs>0?`${sample!.angleDeg}°`:'—'}</dd></div>
          <div><dt>Antigüedad</dt><dd>{age!==null?`${Math.round(age)} ms`:'—'}</dd></div>
          <div><dt>PWM aplicado</dt><dd>{fresh?`${head!.pulseUs} µs`:'—'}</dd></div>
          <div><dt>PWM solicitado</dt><dd>{fresh?`${head!.targetUs} µs`:'—'}</dd></div></dl>
        {fresh && head?.frontalEligible && <p className="afr2-help">Apta para proteccion frontal.</p>}
      </Disclosure></div>
      {feedback && <p className="afr2-sensor-feedback" role="status">{feedback}</p>}
    </aside>
    <div className="afr2-radar-settings"><Disclosure title="Parametros del escaneo" meta={pending?'Cambios sin aplicar':config?`${configured.minAngleDeg}–${configured.maxAngleDeg}° · ${configured.speedDegS}°/s`:'Sin datos del robot'}>
      <form className="afr2-sensor-fields" onSubmit={e=>{e.preventDefault();void action('robot.radar.configure',{write:true,...draft},'Parametros aplicados en RAM.');}}>
        <div className="afr2-scan-parameters">
          <ScanAngleRange min={draft.minAngleDeg} max={draft.maxAngleDeg} disabled={configLocked} onChange={(minAngleDeg,maxAngleDeg)=>setDraft(v=>({...v,minAngleDeg,maxAngleDeg}))}/>
          {([['speedDegS','Velocidad angular',1,30,'°/s'],['settleMs','Estabilizacion',50,500,'ms'],['intervalMs','Intervalo',70,1000,'ms']] as const).map(([key,label,min,max,unit])=>
            <ScanRangeField key={key} label={label} value={draft[key]} min={min} max={max} step={1} unit={unit} disabled={configLocked} onChange={value=>setDraft(v=>({...v,[key]:value}))}/>)}
        </div>
        <div className="afr2-scan-config-footer">
          <div className="afr2-scan-inversion"><label className="afr2-checkbox"><input type="checkbox" checked={draft.inverted} disabled={configLocked} aria-describedby="afr2-servo-inversion-help" onChange={e=>setDraft(v=>({...v,inverted:e.target.checked}))}/> Corregir giro invertido</label>
            <p id="afr2-servo-inversion-help" className="afr2-help">Para montajes que giran al reves. Intercambia 0° y 180°; mantiene el centro de 90°.</p></div>
          <div className="afr2-scan-apply"><p className="afr2-help">{!online?'Conecta el robot para editar.':running?'Detene el radar para editar.':enabled?'Desactiva el servo para editar.':'Enter o salir confirma el numero. El sector se previsualiza antes de aplicar al robot.'}</p>
            <Button type="submit" disabled={configLocked || !config}>Aplicar parametros</Button></div>
        </div>
        <p className="afr2-help afr2-scan-timing">Intervalo entre disparos del HC-SR04 · Estabilizacion despues de cada paso · Velocidad efectiva maxima aplicada: {effective.toFixed(1)}°/s.</p>
      </form>
    </Disclosure></div>
    <footer className="afr2-radar-footer"><span>{!online?'Esperando conexion':mapReady?'Mapa sincronizado':'Recibiendo mapa'}{lost?` · ${lost} muestras perdidas; ${mapReady?'mapa resincronizado':'resincronizando'}`:''}{outOfScale?` · ${outOfScale} ecos fuera de escala`:''}</span><span>Los huecos no indican camino libre.</span></footer>
  </section>;
}
function LineWorkspace(){
  const [reading,setReading]=useState<(IrReading & {receivedAtMs:number})|null>(null),[now,setNow]=useState(Date.now());
  const {wsState,megaLinkState}=useConnection();useSensorPoll<IrReading>('robot.ir.get',100,r=>setReading({...r,receivedAtMs:Date.now()}));
  useEffect(()=>{const id=setInterval(()=>setNow(Date.now()),250);return()=>clearInterval(id);},[]);
  const fresh=wsState==='ready' && megaLinkState==='ready' && reading?.rawValid && reading.ageMs!==null && now-reading.receivedAtMs+reading.ageMs<=500;
  return <section className="afr2-sensor-panel"><SectionHeader title="Linea IR" description="Cinco canales, separados de la distancia ultrasonica."/><div className="afr2-ir-readings">{Array.from({length:5},(_,i)=>{
    const value=fresh?reading!.raw[i]:null,max=reading?.kind==='digital'?1:1023;return <div key={i}><strong>IR{i+1}</strong><div className="afr2-ir-bar"><span style={{height:value!==null?`${value/max*100}%`:'0%'}}/></div><output>{value??'—'}</output><span>{value===null?'Sin dato':reading?.kind==='digital'?'Digital':'ADC / 1023'}</span></div>;
  })}</div><p className="afr2-help">{fresh?(reading!.calibrated?'Calibracion confirmada':'Lecturas originales disponibles · calibracion pendiente'):'Sin lectura actual'} · Mascara procesada: {fresh && reading?.valid?reading.bits.toString(2).padStart(5,'0'):'—'}. Las barras representan señal, sin unidad de distancia.</p></section>;
}
interface WheelsReading {wired:boolean;valid:boolean;counts:Record<string,number|null>;ageMs:number|null;receivedAtMs:number}
function WheelsWorkspace(){
  const [reading,setReading]=useState<WheelsReading|null>(null),[now,setNow]=useState(Date.now());
  const {wsState,megaLinkState}=useConnection();useSensorPoll<Omit<WheelsReading,'receivedAtMs'>>('robot.encoders',200,r=>setReading({...r,receivedAtMs:Date.now()}));
  useEffect(()=>{const id=setInterval(()=>setNow(Date.now()),500);return()=>clearInterval(id);},[]);
  const fresh=wsState==='ready' && megaLinkState==='ready' && reading?.valid && reading.ageMs!==null && now-reading.receivedAtMs+reading.ageMs<2000;
  return <section className="afr2-sensor-panel"><SectionHeader title="Ruedas" description="Contadores Hall FL / FR / RL / RR, independientes del encoder de navegacion."/><div className="afr2-wheel-readings">{['fl','fr','rl','rr'].map(key=><div key={key}><UiIcon name="wheel"/><strong>{key.toUpperCase()}</strong><output>{fresh?reading!.counts[key]??'—':'—'}</output><span>pulsos</span></div>)}</div><p className="afr2-help">{reading?.wired?'Hall declarado conectado':'Hall sin conexion declarada'} · {fresh?'Lectura actual':'Sin lectura valida'}</p></section>;
}
export function SensorsPage(){
  const [params,setParams]=useSearchParams();
  const requested=params.get('seccion');
  const active=requested && ['distancia','linea','ruedas'].includes(requested)?requested:'distancia';
  return <div className="afr2-route-view afr2-page-stack"><SectionHeader title="Sensores" description="Distancia, angulo y señales del robot en tiempo real."/><TaskSections label="Grupos de sensores" value={active}
    onValueChange={id=>setParams(previous=>{const next=new URLSearchParams(previous);next.set('seccion',id);return next;},{replace:true})} items={[
  {id:'distancia',label:'Distancia y servo',content:<DistanceWorkspace/>},{id:'linea',label:'Linea IR',content:<LineWorkspace/>},{id:'ruedas',label:'Ruedas',content:<WheelsWorkspace/>},
]}/></div>;}
