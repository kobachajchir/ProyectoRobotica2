import { useEffect, useState } from 'react';
import { Link, useSearchParams } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { Disclosure } from '../../components/ui/Disclosure';
import { TaskSections } from '../../components/ui/TaskSections';
import { UiIcon } from '../../components/ui/UiIcon';
import { MecanumDrivePad } from '../../components/robot/MecanumDrivePad';
import { RangeNumberField as NumericControl } from '../../components/ui/RangeNumberField';
import { HoldActionButton as HoldButton } from '../../components/ui/HoldActionButton';
import { ROBOT_WHEEL_KEYS } from '../../components/three/robotWheelMotion';
import { armViewFocus, forwardArm, GRIP_MIN_MM, JOINTS, rad, STANDALONE_MOUNT, TRANSPORT, type ArmAngles, type Point3 } from './armKinematics';
import { BIN_SLOTS, collectCube, cubeReach, driveVector, halt, initialCollector, manualGrasp, manualRelease, measure, moveArmTo, moveBase, nextBinPose, positionCube,
  PHASE_LABELS, setJoint, setManualWheels, startCollection, startManualScan } from './collectorSimulation';
import { useCollector, type SimulationKind } from './CollectorContext';
import { SimulationMatrix } from './SimulationMatrix';
import { CollectorScene, type SceneTool, type SceneView } from './CollectorScene';
import { SavedArmPointsPanel } from './SavedArmPointsPanel';
import { robotModel, modelProblems } from './robotModel';
import { useArmPoints } from './ArmPointsContext';
import '../../styles/collector.css';
import '../../styles/arm-points.css';

function RobotControls() {
  const { state, update } = useCollector();
  useEffect(() => () => update(s => ({ ...s, wheels: { fl: 0, fr: 0, rl: 0, rr: 0 } })), [update]);
  const [speed, setSpeed] = useState(100);
  const disabled = state.running || state.scanning || !!state.jointTarget || state.cubes.some(c => c.location === 'gripper');
  const stop = () => update(s => ({ ...s, wheels: { fl: 0, fr: 0, rl: 0, rr: 0 } }));
  const drive = (vx: number, vy: number, wz: number) => update(s => setManualWheels(s, driveVector(vx * speed / 1000, vy * speed / 1000, wz * .6)));
  return <section className="afr2-collector-panel" aria-labelledby="collector-drive-title">
    <div className="afr2-collector-panel__heading"><UiIcon name="control" /><div><h2 id="collector-drive-title">Mover el robot</h2><p>Mantene pulsado para mover. Solta para detener.</p></div></div>
    <div className="afr2-sim-robot-console"><div className="afr2-sim-robot-motion">
    <NumericControl label="Velocidad" value={speed} min={30} max={200} step={10} unit="mm/s" onChange={setSpeed} />
<MecanumDrivePad disabled={disabled} onStart={intent => drive(intent.vx, intent.vy, intent.wz)} onStop={stop} />
    </div><div className="afr2-sim-robot-motors"><h3>Motor por rueda</h3>
    <div className="afr2-collector-wheels">
      {ROBOT_WHEEL_KEYS.map(key => <div key={key} className="afr2-collector-wheel">
        <HoldButton label={`Motor ${key.toUpperCase()} atras`} disabled={disabled} onStart={() => update(s => setManualWheels(s, { fl: 0, fr: 0, rl: 0, rr: 0, [key]: -speed }))} onStop={stop}>− Atras</HoldButton>
        <strong>Motor {key.toUpperCase()}<small>{key === 'fl' ? 'Delantero izq.' : key === 'fr' ? 'Delantero der.' : key === 'rl' ? 'Trasero izq.' : 'Trasero der.'}</small></strong>
        <HoldButton label={`Motor ${key.toUpperCase()} adelante`} disabled={disabled} onStart={() => update(s => setManualWheels(s, { fl: 0, fr: 0, rl: 0, rr: 0, [key]: speed }))} onStop={stop}>Adelante +</HoldButton>
      </div>)}
    </div>
    </div></div>
    <p className="afr2-collector-hint">Para trasladarse, el brazo debe estar en Transporte. Los motores individuales tambien desplazan y giran el chasis.</p>
    <Link className="afr2-collector-link" to="/control">Abrir control del robot real <UiIcon name="arrow" /></Link>
  </section>;
}
function ArmControls() {
  const { state, update } = useCollector();
  const moving = ROBOT_WHEEL_KEYS.some(key => state.wheels[key] !== 0);
  const disabled = !!state.rigIssue || modelProblems(robotModel()).length > 0 || state.running || state.scanning || moving || !!state.jointTarget;
  const preset = (q: ArmAngles) => update(s => moveArmTo(s, q));
  const held = state.cubes.some(c => c.location === 'gripper');
  const selectedJoint = state.selectedJoint;
  return <><div className="afr2-sim-arm-controls"><section className="afr2-collector-panel" aria-labelledby="collector-arm-title">
    <div className="afr2-collector-panel__heading"><UiIcon name="arm" /><div><h2 id="collector-arm-title">Articulaciones</h2><p>Selecciona un eje o arrastralo en el modelo.</p></div></div>
    <div className="afr2-collector-joint-pickers" role="group" aria-label="Seleccionar articulacion">{JOINTS.map((joint, index) =>
      <Button key={joint.label} size="sm" aria-pressed={selectedJoint === index} title={joint.label}
        onClick={() => update(s => ({ ...s, selected: `j${index}`, selectedJoint: index }))}>J{index + 1}</Button>
    )}</div>
    {JOINTS[selectedJoint] && <NumericControl label={JOINTS[selectedJoint].label} value={state.joints[selectedJoint]}
      min={JOINTS[selectedJoint].min} max={JOINTS[selectedJoint].max} unit="°" disabled={disabled}
      onChange={value => update(s => setJoint(s, selectedJoint, value))} />}
    <Button disabled={disabled} onClick={() => preset(TRANSPORT)}>Postura de transporte</Button>
    </section><section className="afr2-collector-panel afr2-sim-gripper" aria-labelledby="collector-gripper-title">
    <h2 id="collector-gripper-title">Gripper</h2>
    <NumericControl label="Apertura" value={state.opening} min={GRIP_MIN_MM} max={robotModel().gripper.max} step={.5} unit="mm"
      disabled={disabled || state.cubes.some(c => c.location === 'gripper')}
      onChange={opening => update(s => ({ ...s, opening }))} />
    <div className="afr2-sim-gripper__actions">
      <Button disabled={disabled || held} onClick={() => update(s => {
        if (s.environment === 'mounted') { const next = manualGrasp(s); if (next.cubes.some(c => c.location === 'gripper')) return next; }
        return { ...s, opening: GRIP_MIN_MM, detail: 'Gripper cerrado sin objeto.' };
      })}>Cerrar</Button>
      <Button disabled={disabled} onClick={() => update(s => held ? manualRelease(s) : { ...s, opening: robotModel().gripper.open, detail: 'Gripper abierto.' })}>Abrir</Button>
    </div>
    {state.environment === 'mounted' && <Button disabled={disabled || !held || state.stored >= BIN_SLOTS.length} onClick={() => update(nextBinPose)}>Llevar al deposito</Button>}
  </section></div><SavedArmPointsPanel /></>;
}

function PositionsPanel() {
  const { state, update } = useCollector();
  const [seed, setSeed] = useState(state.seed), [count, setCount] = useState(state.requestedCount);
  const [spawnX, setSpawnX] = useState(240), [spawnY, setSpawnY] = useState(100);
  const busy = state.running || state.scanning || !!state.jointTarget || ROBOT_WHEEL_KEYS.some(k => state.wheels[k] !== 0);
  const cube = state.cubes.find(c => state.selected === `cube:${c.id}`);
  const reach = cube ? cubeReach(state, cube) : null;
  return <section className="afr2-collector-panel afr2-sim-position-controls"><h2>Posiciones y cubos</h2>
    <Disclosure title="Posicion del robot" meta="mm / grados">
      <NumericControl label="Robot X" value={state.pose.x * 1000} min={-2000} max={2000} step={10} unit="mm" disabled={busy} onChange={x => update(s => moveBase(s, x / 1000, s.pose.y))} />
      <NumericControl label="Robot Y" value={state.pose.y * 1000} min={-2000} max={2000} step={10} unit="mm" disabled={busy} onChange={y => update(s => moveBase(s, s.pose.x, y / 1000))} />
      <NumericControl label="Rumbo" value={state.pose.yaw * 180 / Math.PI} min={-180} max={180} unit="°" disabled={busy} onChange={yaw => update(s => moveBase(s, s.pose.x, s.pose.y, rad(yaw)))} />
    </Disclosure>
    <div className="afr2-collector-objects" role="group" aria-label="Cubos de la escena">{state.cubes.map(item => {
      const status = cubeReach(state, item);
      return <button key={item.id} type="button" aria-pressed={item.id === cube?.id} data-reachable={status.reachable}
        onClick={() => update(s => ({ ...s, selected: `cube:${item.id}` }))}>
        <strong>C{item.id}</strong><span>{(item.point[0] * 1000).toFixed(0)} / {(item.point[1] * 1000).toFixed(0)} mm<small>{status.reason}</small></span>
      </button>;
    })}</div>
    {cube && <div className="afr2-collector-object-editor"><h3>Cubo C{cube.id}</h3>
      <p className="afr2-collector-reach" data-reachable={reach?.reachable}>{reach?.reason}</p>
      <NumericControl label="Cubo X" value={cube.point[0] * 1000} min={-2000} max={2000} step={5} unit="mm" disabled={busy || cube.location !== 'floor'}
        onChange={x => update(s => { const c = s.cubes.find(c => c.id === cube.id)!; return positionCube(s, c.id, [x / 1000, c.point[1], .025]); })} />
      <NumericControl label="Cubo Y" value={cube.point[1] * 1000} min={-2000} max={2000} step={5} unit="mm" disabled={busy || cube.location !== 'floor'}
        onChange={y => update(s => { const c = s.cubes.find(c => c.id === cube.id)!; return positionCube(s, c.id, [c.point[0], y / 1000, .025]); })} />
      <Button variant="primary" disabled={busy || cube.location !== 'floor' || state.stored >= BIN_SLOTS.length} onClick={() => update(s => collectCube(s, cube.id))}>
        {reach?.reachable ? 'Agarrar y guardar este cubo' : 'Acercar robot y recoger'}
      </Button>
    </div>}
    <Disclosure title="Agregar cubo por posicion">
      <NumericControl label="Nuevo X" value={spawnX} min={-1900} max={1900} step={10} unit="mm" disabled={busy} onChange={setSpawnX} />
      <NumericControl label="Nuevo Y" value={spawnY} min={-1900} max={1900} step={10} unit="mm" disabled={busy} onChange={setSpawnY} />
      <Button disabled={busy} onClick={() => update(s => positionCube(s, Math.max(0, ...s.cubes.map(c => c.id)) + 1, [spawnX / 1000, spawnY / 1000, .025]))}>Agregar cubo</Button>
    </Disclosure>
    <Disclosure title="Generar posiciones aleatorias">
      <NumericControl label="Semilla" value={seed} min={0} max={999999} disabled={busy} onChange={v => setSeed(Math.round(v))} />
      <NumericControl label="Cantidad" value={count} min={1} max={12} disabled={busy} onChange={v => setCount(Math.round(v))} />
      <Button disabled={busy} onClick={() => update(s => ({ ...initialCollector(seed, count), references: s.references, workspace: s.workspace }))}>Regenerar y vaciar deposito</Button>
      <p className="afr2-collector-hint">C1 aparece cerca del robot; los demas cubos se distribuyen por el suelo.</p>
    </Disclosure>
  </section>;
}
function MissionPanel() {
  const { state, update } = useCollector();
  const busy = state.running || state.scanning || !!state.jointTarget || ROBOT_WHEEL_KEYS.some(k => state.wheels[k] !== 0);
  return <section className="afr2-collector-panel afr2-sim-mission-controls"><h2>Recoleccion conjunta</h2>
    <p className="afr2-collector-hint">Selecciona un cubo en Posiciones, o inicia el barrido para buscar candidatos con el sensor simulado.</p>
    <Button variant="primary" disabled={busy} onClick={() => update(startCollection)}>Buscar y recolectar</Button>
    <Button disabled={busy} onClick={() => update(startManualScan)}>Solo barrer</Button>
    <div className="afr2-collector-sensor"><span>Eco simulado</span><strong>{state.reading?.distance == null ? '—' : `${(state.reading.distance * 1000).toFixed(0)} mm`}</strong></div>
    <NumericControl label="Servo del sensor" value={state.sensorAngle + 90} min={0} max={180} unit="°" disabled={busy}
      onChange={v => update(s => { const next = { ...s, sensorAngle: v - 90 }; return { ...next, reading: measure(next) }; })} />
    <Disclosure title="Montaje del sensor virtual">
      <NumericControl label="Altura" value={state.sensorHeight * 1000} min={20} max={200} unit="mm" disabled={busy}
        onChange={v => update(s => ({ ...s, sensorHeight: v / 1000, reading: null, echoes: [] }))} />
      <p className="afr2-collector-hint">Montaje bajo propuesto a45 mm. El rayo ideal debe cruzar el cubo para detectar un eco.</p>
    </Disclosure>
  </section>;
}

function SimulationWorkspace() {
  const points = useArmPoints();
  const { state, update, kind } = useCollector();
  const armPanel = kind === 'brazo', carOnly = kind === 'auto';
  const [view, setView] = useState<SceneView>('perspective'), [tool, setTool] = useState<SceneTool>('orbit');
  const [revision, setRevision] = useState(0), [task, setTask] = useState('mission');
  const [matrixExpanded, setMatrixExpanded] = useState(true);
  const [focus, setFocus] = useState<Point3>(() => armPanel ? armViewFocus() : [-state.pose.y, carOnly ? .1 : .2, -state.pose.x]);
  const modelKey = `${robotModel().id}:${robotModel().revision}`;
  useEffect(() => { setFocus(armPanel ? armViewFocus() : [-state.pose.y, carOnly ? .1 : .2, -state.pose.x]); }, [modelKey, armPanel, carOnly]);
  const busy = state.running || state.scanning || !!state.jointTarget || ROBOT_WHEEL_KEYS.some(k => state.wheels[k] !== 0);
  const tcp = forwardArm(state.joints, armPanel ? STANDALONE_MOUNT : undefined).tcp;
  useEffect(() => {
    if (armPanel || carOnly) return;
    if (state.selected.startsWith('j') || state.selected.startsWith('point:') || state.selected === 'gripper' || state.selected === 'arm') setTask('arm');
    else if (state.selected.startsWith('cube:')) setTask('positions');
  }, [state.selected, armPanel, carOnly]);
  return <section className="afr2-collector-card" aria-label="Espacio de simulacion">
    <div className="afr2-collector-layout" data-matrix-open={matrixExpanded}>
    <div className="afr2-collector-main">
      <section className="afr2-collector-stage" aria-label={armPanel ? 'Simulador del brazo' : carOnly ? 'Simulador del auto' : 'Simulador del conjunto'}>
        <div className="afr2-collector-toolbar">
          <div className="afr2-collector-actions" role="group" aria-label="Camara">
            {(['perspective', 'top', 'side'] as SceneView[]).map(v => <Button key={v} size="sm" aria-pressed={view === v} onClick={() => setView(v)}>{v === 'perspective' ? 'Perspectiva' : v === 'top' ? 'Superior' : 'Lateral'}</Button>)}
            <Button size="sm" onClick={() => { setFocus(armPanel ? armViewFocus() : [-state.pose.y, carOnly ? .1 : .2, -state.pose.x]); setRevision(v => v + 1); }}>Centrar</Button>
          </div>
          <div className="afr2-collector-actions">
            {!armPanel && <Button size="sm" disabled={busy} aria-pressed={tool === 'move'} onClick={() => { setTool(t => t === 'move' ? 'orbit' : 'move'); update(s => ({ ...s, selected: 'robot' })); }}>Mover auto</Button>}
            {!armPanel && !carOnly && <Button size="sm" disabled={busy} aria-pressed={tool === 'place'} onClick={() => setTool(t => t === 'place' ? 'orbit' : 'place')}>Agregar cubo</Button>}
            {!carOnly && <Button size="sm" aria-pressed={state.workspace} onClick={() => update(s => ({ ...s, workspace: !s.workspace }))}>{state.workspace ? 'Ocultar alcance' : 'Mostrar alcance'}</Button>}
            {!armPanel && busy && <Button size="sm" onClick={() => update(s => halt(s))}>Pausar</Button>}
            {!carOnly && (points.calculating || state.pointRun) && <Button size="sm" variant="danger" onClick={points.cancel}>Cancelar recorrido</Button>}
          </div>
        </div>
        <CollectorScene view={view} tool={busy || armPanel ? 'orbit' : tool} focus={focus} revision={revision} armPanel={armPanel} carOnly={carOnly} />
        <div className="afr2-collector-scene-footer"><p>{!armPanel && tool === 'place' ? 'Clic en el suelo para agregar un cubo.' : !armPanel && tool === 'move' ? 'Arrastra el auto. Desactiva Mover auto para orbitar.' : armPanel ? 'Arrastra una articulacion con clic izquierdo o derecho para girarla. Arrastra el fondo para orbitar.' : 'Arrastra el fondo para orbitar; selecciona un cubo o una articulacion para operarlo.'}</p><span>Cuadricula100 mm</span></div>
      </section>
      {!armPanel && !carOnly && <div className="afr2-collector-legend"><span className="afr2-collector-legend__reach">Zona de toma</span><span className="afr2-collector-legend__cube">Verde: dentro del alcance</span><span>Ocre: requiere acercarse</span></div>}
      <div className="afr2-collector-readouts">
        {!armPanel && <div><span>Posicion del auto</span><strong>{(state.pose.x * 1000).toFixed(0)} / {(state.pose.y * 1000).toFixed(0)} <small>mm</small></strong></div>}
        {!armPanel && <div><span>Rumbo</span><strong>{(state.pose.yaw * 180 / Math.PI).toFixed(1)}°</strong></div>}
        {!carOnly && <div><span>Herramienta X / Y / Z</span><strong>{tcp.map(v => (v * 1000).toFixed(0)).join(' / ')} <small>mm</small></strong></div>}
        {!carOnly && <div><span>{armPanel ? 'Gripper' : 'Deposito'}</span><strong>{armPanel ? `${state.opening.toFixed(1)} mm` : `${state.stored} / ${BIN_SLOTS.length} cubos`}</strong></div>}
      </div>
    </div>
    <aside className="afr2-sim-math-side" aria-label="Matematica del modelo">
      <SimulationMatrix expanded={matrixExpanded} onToggle={() => setMatrixExpanded(v => !v)} />
    </aside>
    </div>
    <div className="afr2-sim-bottom-controls">
        {!armPanel && !carOnly && state.phase !== 'idle' && <div className="afr2-collector-mission" data-fault={state.phase === 'fault'} role="status" aria-live="polite"><strong>{PHASE_LABELS[state.phase]}</strong><p>{state.detail}</p></div>}
        {armPanel ? <ArmControls /> : carOnly ? <RobotControls /> : <TaskSections label="Controles del conjunto" value={task} onValueChange={setTask} items={[
          { id: 'mission', label: 'Recolectar', content: <MissionPanel /> },
          { id: 'positions', label: 'Posiciones', content: <PositionsPanel /> },
          { id: 'arm', label: 'Brazo', content: <ArmControls /> },
          { id: 'drive', label: 'Auto', content: <RobotControls /> },
        ]} />}
    </div>
  </section>;
}

export function CollectorPage() {
  const { kind } = useCollector();
  const [, setParams] = useSearchParams();
  return <div className="afr2-route-view afr2-collector">
    <SectionHeader title="Simulaciones" description="Explora el movimiento y observa su matematica en tiempo real." />
    <TaskSections label="Elegir simulacion" value={kind} onValueChange={next => setParams({ tipo: next as SimulationKind })} items={[
      { id: 'auto', label: 'Auto', content: <SimulationWorkspace key="auto" /> },
      { id: 'brazo', label: 'Brazo robotico', content: <SimulationWorkspace key="brazo" /> },
      { id: 'conjunta', label: 'Conjunto', content: <SimulationWorkspace key="conjunta" /> },
    ]} />
  </div>;
}
