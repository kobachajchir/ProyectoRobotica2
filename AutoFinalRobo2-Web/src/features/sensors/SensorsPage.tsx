import { useEffect,useRef,useState } from 'react';
import { useSearchParams } from 'react-router-dom';
import { TaskSections } from '../../components/ui/TaskSections';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { PageHeader } from '../../components/ui/PageHeader';
import { Button } from '../../components/ui/Button';
import { Disclosure } from '../../components/ui/Disclosure';
import { RenderBoundary } from '../../components/ui/RenderBoundary';
import { UiIcon } from '../../components/ui/UiIcon';
import { SegmentedControl, type SegmentedItem } from '../../components/ui/SegmentedControl';
import { useTheme } from '../../app/providers/ThemeProvider';
import { useConnection } from '../connection/ConnectionContext';
import { SENSOR_ERRORS,type IrReading,type RadarConfig,type RadarReply,type RangeSample } from '../../protocol/sensorHead';
import type { Afr2CommandName } from '../../protocol/wsEnvelope';
import { Afr2CommandError } from '../../protocol/afr2Client';
import { SamplingPanel } from './SamplingPanel';
import { useSensorPoll } from './useSensorPoll';
import { RadarScene, formatRadarDistance, type RadarView } from './RadarScene';
import { ScanAngleRange, ScanRangeField } from './ScanControls';

type StoredPoint=RangeSample & {receivedAtMs:number};
/* AFR2-20261106-35: estados separados de una orden de servo. «Recibido» = el ATmega aceptó la orden;
   «aplicado» = la telemetría muestra el PWM objetivo; nunca se infiere desde el envío. */
type ServoPhase={phase:'idle'|'sent'|'received'|'applied'|'error';targetUs?:number;message?:string;at:number};
const SERVO_APPLY_TIMEOUT_MS=1500;
function servoErrorReason(e:unknown){
  if(e instanceof Afr2CommandError){
    if(e.code==='BUSY_IN_FLIGHT')return 'ocupado: otra orden en curso o barrido automático activo';
    if(e.code==='MEGA_LINK_DOWN')return 'sin enlace con el ATmega';
    if(e.code==='BAD_REQUEST')return 'ángulo fuera de rango (0–180°)';
    if(e.code==='CAPABILITY_UNAVAILABLE')return 'el firmware no admite el servo';
    if(e.code==='TIMEOUT')return 'sin respuesta del robot a tiempo';
    return `${e.code}: ${e.message}`;
  }
  return e instanceof Error?e.message:'motivo desconocido';
}
const SERVO_PHASE_TEXT:Record<ServoPhase['phase'],string>={idle:'Sin órdenes',sent:'Enviada',received:'Recibida · esperando PWM',applied:'Aplicada',error:'Error'};
// AFR2-20261101-09 / VIS-052: alcance y vista del radar en el selector segmentado comun.
const RADAR_RANGES:readonly SegmentedItem[]=[{value:'0.25',label:'25 cm'},{value:'0.5',label:'50 cm'},{value:'1',label:'1 m'},{value:'2',label:'2 m'}];
const RADAR_VIEWS:readonly SegmentedItem<RadarView>[]=[{value:'perspective',label:'Perspectiva',icon:'cube'},{value:'top',label:'Superior',icon:'radar'}];
const DEFAULT_CONFIG:RadarConfig={minAngleDeg:0,maxAngleDeg:180,speedDegS:10,settleMs:100,intervalMs:70,inverted:false};
function DistanceWorkspace(){
  const {headReading:head,wsState,megaLinkState,requestCommand,stopRecord,registerHeadViewer}=useConnection();
  useEffect(registerHeadViewer,[registerHeadViewer]);const {resolvedTheme}=useTheme();
  const [points,setPoints]=useState<Record<number,StoredPoint>>({}),[range,setRange]=useState(1),[now,setNow]=useState(Date.now());
  const [view,setView]=useState<RadarView>('top');
  const [config,setConfig]=useState<RadarConfig|null>(null),[draft,setDraft]=useState(DEFAULT_CONFIG),[angle,setAngle]=useState(90);
  const [busy,setBusy]=useState(false),[feedback,setFeedback]=useState<string|null>(null),[lost,setLost]=useState(0),[mapReady,setMapReady]=useState(false);
  const [servo,setServo]=useState<ServoPhase>({phase:'idle',at:0});
  useEffect(()=>{if(servo.phase==='received' && head && servo.targetUs!==undefined && head.pulseUs===servo.targetUs && head.receivedAtMs>=servo.at)setServo(v=>({...v,phase:'applied',at:Date.now()}));},[head,servo]);
  useEffect(()=>{if(servo.phase==='received' && now-servo.at>SERVO_APPLY_TIMEOUT_MS)setServo(v=>v.phase==='received'?{...v,phase:'error',message:'el robot aceptó la orden pero no se observó el PWM aplicado',at:Date.now()}:v);},[now,servo]);
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
      const slot=Math.round(p.angleDeg/5)*5;const old=result[slot];if(!old || ((p.sampleId-old.sampleId)|0)>0)result[slot]={...p,receivedAtMs:Date.now()};
    }return result;});
    if(r.mode==='snapshot'){c.snapshot=r.nextCursor;if(c.snapshot>=37)setMapReady(true);}else c.sequence=r.nextCursor;
  },()=>cursor.current.snapshot<37?{mode:'snapshot',cursor:cursor.current.snapshot}:{mode:'delta',cursor:cursor.current.sequence});
  const action=async(command:Afr2CommandName,payload:Record<string,unknown>,message:string)=>{
    setBusy(true);setFeedback(null);const servoCommand=command==='robot.servo.set';if(servoCommand)setServo({phase:'sent',at:Date.now()});
    try{const r=await requestCommand<RadarConfig>(command,payload);if(servoCommand)setServo({phase:'received',targetUs:(r.payload as unknown as {targetUs?:number}).targetUs,at:Date.now()});if(command==='robot.radar.configure'){setConfig(r.payload);setDraft(r.payload);}if(command==='robot.radar.start'){cursor.current.snapshot=0;cursor.current.sequence=0;setPoints({});setMapReady(false);}setFeedback(message);}
    catch(e){if(servoCommand)setServo({phase:'error',message:servoErrorReason(e),at:Date.now()});setFeedback(e instanceof Error?e.message:'No se confirmó la acción.');}finally{setBusy(false);}
  };
  const move=(value:number)=>{
    setAngle(value);manual.current.target=value;if(manual.current.sending)return;const epoch=manual.current.epoch;
    const drain=async()=>{manual.current.sending=true;
      try{while(manual.current.target!==null && epoch===manual.current.epoch){const target=manual.current.target;manual.current.target=null;setServo({phase:'sent',at:Date.now()});const r=await requestCommand<{targetUs?:number}>('robot.servo.set',{operation:'angle',angleDeg:target});setServo({phase:'received',targetUs:r.payload.targetUs,at:Date.now()});await new Promise(resolve=>setTimeout(resolve,100));}}
      catch(e){setServo({phase:'error',message:servoErrorReason(e),at:Date.now()});setFeedback(e instanceof Error?e.message:'No se confirmó el ángulo.');manual.current.target=null;}
      finally{manual.current.sending=false;}
    };void drain();
  };
  const fresh=online && !!head && now-head.receivedAtMs<1000;
  const sample=head?.sample,age=sample?.ageMs===null || sample?.ageMs===undefined?null:sample.ageMs+Math.max(0,now-(head?.receivedAtMs??now));
  const maxAge=Math.max(200,2*(head?.intervalMs??70)+32,head?.running && config?2*Math.max(5000/config.speedDegS,config.settleMs+32,config.intervalMs):0);
  const sampleFresh=fresh && sample?.valid && age!==null && age<=maxAge;
  const running=head?.running===true,enabled=head?.enabled===true,values=Object.values(points),configured=config??draft;
  const plotPoints=fresh && sample && sample.sampleId && sample.pulseUs>0?[...values.filter(p=>Math.round(p.angleDeg/5)!==Math.round(sample!.angleDeg/5)),sample!]:values;
  const effective=5000/Math.max(5000/configured.speedDegS,configured.settleMs+32,configured.intervalMs);
  const configLocked=running || enabled || busy || !online;
  const pending=!!config && (Object.keys(DEFAULT_CONFIG) as (keyof RadarConfig)[]).some(key=>draft[key]!==config[key]);
  const preview=!config || draft.minAngleDeg!==config.minAngleDeg || draft.maxAngleDeg!==config.maxAngleDeg;
  const echoCount=values.filter(p=>p.valid).length;
  const outOfScale=plotPoints.filter(p=>p.valid && (p.distanceMm??0)>range*1000).length;
  const readingStatus=!online?'Sin conexión':!fresh?'Esperando telemetría':sampleFresh?'Eco valido':sample?.valid?'Medición sin actualizar':sample?SENSOR_ERRORS[sample.error]:'Esperando medición';
  return <section className="afr2-control-map afr2-radar-map" aria-label="Distancia y servo">
    <header className="afr2-control-map__heading">
      <div><p className="afr2-kicker"><UiIcon name="radar"/> Percepción del entorno</p><h2>Radar frontal</h2><p className="afr2-radar-intro">Un mapa de ecos reales, desde el punto de vista del robot.</p></div>
      <div className="afr2-radar-health"><span className={`afr2-map-status${fresh && running?' is-live':''}`}>{!fresh?echoCount?'Mapa histórico':online?'Esperando datos':'Sin conexión':running?'Barrido en curso':'Lectura fija'}</span><span className="afr2-radar-scale-badge">Escala visual · {RADAR_RANGES.find(item=>item.value===String(range))?.label}</span></div>
    </header>
    <div className="afr2-control-map__toolbar">
      <div className="afr2-radar-toolgroup"><span>Alcance</span><SegmentedControl size="sm" ariaLabel="Alcance del radar" items={RADAR_RANGES} value={String(range)} onChange={v=>setRange(Number(v))}/></div>
      <SegmentedControl size="sm" className="afr2-radar-views" ariaLabel="Vista del radar" items={RADAR_VIEWS} value={view} onChange={setView}/>
    </div>
    <div className="afr2-control-map__viewport afr2-radar-viewport">
      <div className="afr2-radar-canvas"><RenderBoundary key={`${range}-${resolvedTheme}`} fallback={<p className="afr2-map-fallback">No se pudo mostrar el radar 3D. Las mediciones y controles siguen disponibles.</p>}>
        <RadarScene range={range} angle={head?.angleDeg??null} enabled={enabled} points={plotPoints} currentId={sampleFresh?sample!.sampleId:0}
          dark={resolvedTheme==='dark'} historical={!fresh || !running} scanMin={draft.minAngleDeg} scanMax={draft.maxAngleDeg} preview={preview} view={view}
          sweeping={fresh && running && enabled} nowMs={now}/>
      </RenderBoundary></div>
      <div className="afr2-radar-zone-label" data-preview={preview}><i aria-hidden="true"/><span>{preview?'Sector en vista previa':'Sector de escaneo'}</span><strong>{draft.minAngleDeg}°<span aria-hidden="true">–</span>{draft.maxAngleDeg}°</strong>{preview && <em>Sin aplicar</em>}</div>
      {!fresh && !echoCount && <div className="afr2-radar-empty"><UiIcon name="sensors"/><div><strong>Esperando al robot</strong><span>Los ecos apareceran al recibir mediciones.</span></div></div>}
      <div className="afr2-map-caption afr2-radar-caption"><span className="afr2-radar-legend" data-kind="echo"><i/> Eco medido</span><span className="afr2-radar-legend" data-kind="current"><i/> Último eco</span><span className="afr2-radar-legend" data-kind="zone"><i/> Sector</span><span className="afr2-radar-legend" data-kind="rings"><i/> Anillos cada {formatRadarDistance(range/4)}</span></div>
    </div>
    <aside className="afr2-map-sidebar afr2-radar-sidebar" aria-label="Mediciones y control del servo">
      <div className="afr2-radar-overview">
        <span className="afr2-radar-eyebrow"><UiIcon name="sensors" size={16}/> Última distancia válida</span>
        <div className="afr2-radar-distance"><strong>{sampleFresh?sample!.distanceMm:'—'}</strong><span>mm</span></div>
        <p className="afr2-radar-reading-state" data-valid={sampleFresh || undefined}><i/>{readingStatus}</p>
        <div className="afr2-radar-secondary">
          <div><span>Ángulo aplicado</span><strong>{fresh && enabled?head!.angleDeg:'—'}<small>°</small></strong></div>
          <div><span>Posiciones con eco</span><strong>{mapReady || echoCount?echoCount:'—'}<small>/ 37</small></strong></div>
        </div>
      </div>
      <div className="afr2-radar-manual">
        <ScanRangeField label="Posición solicitada" value={angle} min={0} max={180} step={1} unit="°" disabled={!fresh || running || busy} onChange={move}/>
        <div className="afr2-action-row">
          <Button size="sm" disabled={!fresh || running || busy} onClick={()=>void action('robot.servo.set',{operation:'center'},'Centrado solicitado; esperando PWM aplicado.')}>Centrar servo</Button>
          <Button size="sm" disabled={!online || busy || running} onClick={()=>void action('robot.servo.set',{operation:'off'},'PWM desactivado.')}>Desactivar</Button>
        </div>
        <p className="afr2-servo-phase" data-phase={servo.phase} role="status" aria-live="polite"><span>Orden de servo</span><strong>{SERVO_PHASE_TEXT[servo.phase]}</strong>{servo.phase==='error' && servo.message && <em>{servo.message}</em>}</p>
      </div>
      <div className="afr2-radar-switch">
        <div><strong><UiIcon name="radar"/> Barrido automático</strong><p className="afr2-help">{pending?'Aplica los parámetros antes de iniciar.':'Recorre el sector y mide en cada paso.'}</p></div>
        <Button variant={running?'secondary':'primary'} disabled={!online || !head || busy || (!running && pending)} onClick={()=>{
          if(!running){cursor.current.snapshot=0;setMapReady(false);}
          void action(running?'robot.radar.stop':'robot.radar.start',{},running?'Radar detenido y PWM desactivado.':'Barrido autonomo iniciado.');
        }}>{running?'Detener barrido':'Iniciar barrido'}</Button>
        {running && <p className="afr2-help">Continúa al salir de esta pantalla o desconectar la Web.</p>}
      </div>
      <div className="afr2-radar-diagnostics"><Disclosure title="Detalles de la medición">
        <dl><div><dt>Ángulo de la muestra</dt><dd>{sampleFresh && sample!.pulseUs>0?`${sample!.angleDeg}°`:'—'}</dd></div>
          <div><dt>Antigüedad</dt><dd>{age!==null?`${Math.round(age)} ms`:'—'}</dd></div>
          <div><dt>PWM aplicado</dt><dd>{fresh?`${head!.pulseUs} µs`:'—'}</dd></div>
          <div><dt>PWM solicitado</dt><dd>{fresh?`${head!.targetUs} µs`:'—'}</dd></div></dl>
        {fresh && head?.frontalEligible && <p className="afr2-help">Apta para protección frontal.</p>}
      </Disclosure></div>
      {feedback && <p className="afr2-sensor-feedback" role="status">{feedback}</p>}
    </aside>
    <div className="afr2-radar-settings"><Disclosure title="Parámetros del escaneo" meta={pending?'Cambios sin aplicar':config?`${configured.minAngleDeg}–${configured.maxAngleDeg}° · ${configured.speedDegS}°/s`:'Sin datos del robot'}>
      <form className="afr2-sensor-fields" onSubmit={e=>{e.preventDefault();void action('robot.radar.configure',{write:true,...draft,inverted:config?.inverted??draft.inverted},'Parámetros aplicados en RAM.');}}>
        <div className="afr2-scan-parameters">
          <ScanAngleRange min={draft.minAngleDeg} max={draft.maxAngleDeg} disabled={configLocked} onChange={(minAngleDeg,maxAngleDeg)=>setDraft(v=>({...v,minAngleDeg,maxAngleDeg}))}/>
          {([['speedDegS','Velocidad angular',1,30,'°/s'],['settleMs','Estabilización',50,500,'ms'],['intervalMs','Intervalo',70,1000,'ms']] as const).map(([key,label,min,max,unit])=>
            <ScanRangeField key={key} label={label} value={draft[key]} min={min} max={max} step={1} unit={unit} disabled={configLocked} onChange={value=>setDraft(v=>({...v,[key]:value}))}/>)}
        </div>
        <div className="afr2-scan-config-footer">
          <p className="afr2-help">Invertir servo se configura desde Sistema → Preferencias.</p>
          <div className="afr2-scan-apply"><p className="afr2-help">{!online?'Conectá el robot para editar.':running?'Detené el radar para editar.':enabled?'Desactiva el servo para editar.':'Enter o salir confirma el número. El sector se previsualiza antes de aplicar al robot.'}</p>
            <Button type="submit" disabled={configLocked || !config}>Aplicar parámetros</Button></div>
        </div>
        <p className="afr2-help afr2-scan-timing">Intervalo entre disparos del HC-SR04 · Estabilización después de cada paso · Velocidad efectiva máxima aplicada: {effective.toFixed(1)}°/s.</p>
      </form>
    </Disclosure></div>
    <SamplingPanel online={online} head={head} running={running}/>
    <footer className="afr2-radar-footer"><span>{!online?'Esperando conexión':mapReady?'Mapa sincronizado':'Recibiendo mapa'}{lost?` · ${lost} muestras perdidas; ${mapReady?'mapa resincronizado':'resincronizando'}`:''}{outOfScale?` · ${outOfScale} ecos fuera de escala`:''}</span><span>Los huecos no indican camino libre.</span></footer>
  </section>;
}
function LineWorkspace(){
  const [reading,setReading]=useState<(IrReading & {receivedAtMs:number})|null>(null),[now,setNow]=useState(Date.now());
  const {wsState,megaLinkState}=useConnection();useSensorPoll<IrReading>('robot.ir.get',100,r=>setReading({...r,receivedAtMs:Date.now()}));
  useEffect(()=>{const id=setInterval(()=>setNow(Date.now()),250);return()=>clearInterval(id);},[]);
  const fresh=wsState==='ready' && megaLinkState==='ready' && reading?.rawValid && reading.ageMs!==null && now-reading.receivedAtMs+reading.ageMs<=500;
  return <section className="afr2-sensor-panel"><SectionHeader title="Línea IR" description="Cinco canales, separados de la distancia ultrasonica."/><div className="afr2-ir-readings">{Array.from({length:5},(_,i)=>{
    const value=fresh?reading!.raw[i]:null,active=fresh && !!(reading!.bits&(1<<i));return <div key={i}><strong>IR{i+1}</strong><div className="afr2-ir-bar"><span style={{height:active?'100%':'0%',backgroundColor:'var(--text-primary)'}}/></div><output>{value??'—'}</output><span>{value===null?'Sin dato':reading?.kind==='digital'?(active?'Línea activa':'Fondo'):'Perfil incompatible'}</span></div>;
  })}</div><p className="afr2-help">{fresh?(reading!.calibrated?'Calibración confirmada':'Lecturas originales disponibles · calibración pendiente'):'Sin lectura actual'} · Mascara procesada: {fresh && reading?.valid?reading.bits.toString(2).padStart(5,'0'):'—'}. Los cuadros muestran actividad; el número es el nivel eléctrico 0/1.</p></section>;
}
interface WheelsReading {wired:boolean;valid:boolean;counts:Record<string,number|null>;ageMs:number|null;receivedAtMs:number}
function WheelsWorkspace(){
  const [reading,setReading]=useState<WheelsReading|null>(null),[now,setNow]=useState(Date.now());
  const {wsState,megaLinkState}=useConnection();useSensorPoll<Omit<WheelsReading,'receivedAtMs'>>('robot.encoders',200,r=>setReading({...r,receivedAtMs:Date.now()}));
  useEffect(()=>{const id=setInterval(()=>setNow(Date.now()),500);return()=>clearInterval(id);},[]);
  const fresh=wsState==='ready' && megaLinkState==='ready' && reading?.valid && reading.ageMs!==null && now-reading.receivedAtMs+reading.ageMs<2000;
  return <section className="afr2-sensor-panel"><SectionHeader title="Ruedas" description="Contadores Hall FL / FR / RL / RR, independientes del encoder de navegación."/><div className="afr2-wheel-readings">{['fl','fr','rl','rr'].map(key=><div key={key}><UiIcon name="wheel"/><strong>{key.toUpperCase()}</strong><output>{fresh?reading!.counts[key]??'—':'—'}</output><span>pulsos</span></div>)}</div><p className="afr2-help">{reading?.wired?'Hall declarado conectado':'Hall sin conexión declarada'} · {fresh?'Lectura actual':'Sin lectura valida'}</p></section>;
}
export function SensorsPage(){
  const [params,setParams]=useSearchParams();
  const requested=params.get('seccion');
  const active=requested && ['distancia','linea','ruedas'].includes(requested)?requested:'distancia';
  return <div className="afr2-route-view afr2-page-stack"><PageHeader title="Sensores" description="Distancia, ángulo y señales del robot en tiempo real."/><TaskSections label="Grupos de sensores" value={active}
    onValueChange={id=>setParams(previous=>{const next=new URLSearchParams(previous);next.set('seccion',id);return next;},{replace:true})} items={[
  {id:'distancia',label:'Distancia y servo',content:<DistanceWorkspace/>},{id:'linea',label:'Línea IR',content:<LineWorkspace/>},{id:'ruedas',label:'Ruedas',content:<WheelsWorkspace/>},
]}/></div>;}
