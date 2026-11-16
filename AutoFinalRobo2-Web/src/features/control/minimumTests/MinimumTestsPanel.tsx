import { useEffect, useMemo, useRef, useState } from 'react';
import { useNavigate } from 'react-router-dom';
import { Button } from '../../../components/ui/Button';
import { Chevron } from '../../../components/ui/Chevron';
import { useConnection } from '../../connection/ConnectionContext';
import { parseRobotState, type RobotPose } from '../../../protocol/robotState';
import '../../../styles/minimum-tests.css';
import { version as PRODUCT_VERSION } from '../../../../package.json';
import { createSequenceUid } from '../sequences/mecanumKinematics';
import { GOTO_MAX_COMMANDS, planGotoCommands, type GotoCommand } from '../goto/gotoSimulation';
import { gotoRouteLegs, useGotoRouteRunner, type GotoLeg } from '../goto/useGotoRouteRunner';
import {
  BATCH_LABEL, CAUSE_LABEL, DEFAULT_BOX_DISTANCE_MM, DEFAULT_MISSION_SPEED_MMS, LOAD_GRAMS, MINIMUM_REPETITIONS, MINIMUM_TESTS, MIN_MISSION_SPEED_MMS,
  RECORD_CSV_HEADER, downloadText, formatStat, loadRecords, recordKey, recordLabel, recordsToCsv, recordsToJson, rawPose, relativePose, routePose, saveRecords, singleLeg,
  singleLegSetpoint, summarize, summaryToTable24Csv, withErrors,
  type MinimumBatch, type MinimumSide, type MinimumTestId, type MinimumTestRecord, type Pose2D,
} from './minimumTests';

/**
 * AFR2-20261111-01 / -08. Panel «Pruebas mínimas» (Conducir). Cada envío es UNA repetición; las cinco las toma el usuario
 * una por una, no hay repetición automática. Desde -08 el ensayo (un tramo o la trayectoria completa) se carga entero en el
 * auto con robot.route.* (0x46..0x49) y el ATmega lo corre en local: un corte del WebSocket o de la Wi-Fi no lo detiene, y el
 * informe final (estado, falla, pose en el marco de partida, tiempo) queda en el auto y se lee al reconectar. robot.state
 * aporta la pose de partida. Sin confirmación en el auto: el envío lo autoriza el botón y lo corta STOP.
 */
/** AFR2-20261111-08: qué medir y en qué dirección, siempre desde el frente del auto (lado del HC-SR04) en la pose de partida. */
const MEASURE_AXES = { x: 'x: adelante (+) / atrás (−)', y: 'y: izquierda (+) / derecha (−)', psi: 'ψ: antihorario (+) / horario (−)' };
function measureHint(id: MinimumTestId, side: MinimumSide) {
  if (id === 'advance') return 'Avance: x ≈ 1000 mm hacia el frente (HC-SR04); y es cuánto se corrió de costado (izquierda +); ψ cuánto giró.';
  if (id === 'lateral') return side === 'right'
    ? 'Lateral derecha: y ≈ −1000 mm (hacia la derecha mirando desde atrás hacia el frente); x es cuánto avanzó (+) o retrocedió (−); ψ cuánto giró.'
    : 'Lateral izquierda: y ≈ +1000 mm (hacia la izquierda mirando desde atrás hacia el frente); x es cuánto avanzó (+) o retrocedió (−); ψ cuánto giró.';
  if (id === 'turn') return side === 'cw'
    ? 'Giro horario: ψ ≈ −90°; x e y son cuánto se corrió el centro del auto (ideal 0).'
    : 'Giro antihorario: ψ ≈ +90°; x e y son cuánto se corrió el centro del auto (ideal 0).';
  return 'Trayectoria: x, y y ψ del centro del auto al terminar, respecto de la pose de partida (ideal = la consigna).';
}
interface Readiness { id: string; label: string; state: 'ok' | 'bad' | 'pending' | 'info'; detail: string }
const num = (value: string) => value.trim() === '' ? null : Number(value);
const fmt = (p: Pose2D | null) => p ? `${p.xMm} · ${p.yMm} · ${p.psiDeg}` : '—';

export function MinimumTestsPanel({ gotoCommands, onGotoCommandsChange, maxSpeedMmS }: {
  gotoCommands: GotoCommand[]; onGotoCommandsChange: (commands: GotoCommand[]) => void; maxSpeedMmS: number;
}) {
  const { wsState, megaLinkState, capabilities, requestCommand, triggerEmergencyStop, battery, stopRecord, robotState } = useConnection();
  const navigate = useNavigate();
  const runner = useGotoRouteRunner();
  const [records, setRecords] = useState<MinimumTestRecord[]>(() => loadRecords());
  const [testId, setTestId] = useState<MinimumTestId>('advance');
  const [side, setSide] = useState<MinimumSide>(null);
  const [batch, setBatch] = useState<MinimumBatch>('adjust');
  const [repetition, setRepetition] = useState(1);
  const [speed, setSpeed] = useState(Math.min(DEFAULT_MISSION_SPEED_MMS, maxSpeedMmS));
  const [boxMm, setBoxMm] = useState(DEFAULT_BOX_DISTANCE_MM);
  const [busy, setBusy] = useState(false);
  const [status, setStatus] = useState('');
  const [selectedId, setSelectedId] = useState<string | null>(null);
  const [measure, setMeasure] = useState({ x: '', y: '', psi: '', notes: '' });
  const mounted = useRef(true);
  useEffect(() => { mounted.current = true; return () => { mounted.current = false; }; }, []);
  useEffect(() => { setSpeed(value => Math.min(Math.max(value, MIN_MISSION_SPEED_MMS), maxSpeedMmS)); }, [maxSpeedMmS]);
  useEffect(() => { if (!saveRecords(records) && records.length) setStatus('No se pudo guardar en este navegador: descargá el registro antes de cerrar.'); }, [records]);

  const def = MINIMUM_TESTS.find(t => t.id === testId)!;
  const isTask = testId === 'task' || testId === 'task-loaded';
  useEffect(() => { setSide(def.sides?.[0]?.[0] ?? null); }, [def]);
  const selected = records.find(r => r.id === selectedId) ?? null;
  useEffect(() => {
    setMeasure({ x: selected?.measured ? String(selected.measured.xMm) : '', y: selected?.measured ? String(selected.measured.yMm) : '',
      psi: selected?.measured ? String(selected.measured.psiDeg) : '', notes: selected?.notes ?? '' });
  }, [selected?.id, selected?.measured]);

  const plan = useMemo(() => planGotoCommands(gotoCommands, 100), [gotoCommands]);
  const legs: GotoLeg[] = useMemo(() => isTask ? (plan.error || !gotoCommands.length ? [] : gotoRouteLegs(plan)) : [singleLeg(testId, side)], [isTask, plan, gotoCommands.length, testId, side]);
  const taskEnd = plan.segments.at(-1)?.end;
  const setpoint: Pose2D = isTask ? { xMm: Math.round(taskEnd?.xMm ?? 0), yMm: Math.round(taskEnd?.yMm ?? 0), psiDeg: 0 } : singleLegSetpoint(testId, side);

  const linkReady = wsState === 'ready' && megaLinkState === 'ready' && stopRecord.status !== 'sending' && stopRecord.status !== 'unconfirmed-offline';
  const gotoCapable = capabilities.mecanum_drive.implemented && capabilities.goto_relative.implemented;
  const hallCalibrated = capabilities.wheel_odometry.present && capabilities.wheel_odometry.implemented && capabilities.wheel_odometry.calibrated;
  const stopped = robotState?.stopped === true;
  const speedOk = Number.isFinite(speed) && speed >= MIN_MISSION_SPEED_MMS && speed <= maxSpeedMmS;
  const existing = records.find(r => recordKey(r) === recordKey({ testId, side, batch, repetition }));
  const hallConfigured = capabilities.wheel_odometry.present && capabilities.wheel_odometry.implemented;
  const motorsEnabled = robotState?.motorsEnabled;
  const fault = robotState?.fault ?? null;
  const trajectoryError = isTask ? (!gotoCommands.length ? 'La trayectoria está vacía: cargala o editala en GOTO.' : plan.error) : null;
  // Cada requisito pasa a verde al cumplirse; «Enviar al robot» se habilita con todos los obligatorios en verde.
  const checks: Readiness[] = [
    { id: 'link', label: 'Conexión con el auto', state: linkReady ? 'ok' : 'bad', detail: linkReady ? 'Web, ESP y ATmega enlazados' : 'Conectá el robot o confirmá el STOP pendiente' },
    { id: 'goto', label: 'GOTO disponible', state: gotoCapable ? 'ok' : 'bad', detail: gotoCapable ? 'El auto acepta GOTO relativo' : (capabilities.goto_relative.reason || 'El auto no informa GOTO disponible') },
    { id: 'motors', label: 'Motores habilitados', state: motorsEnabled === true ? 'ok' : motorsEnabled === false ? 'bad' : 'pending',
      detail: motorsEnabled === true ? 'Habilitados en el auto' : motorsEnabled === false ? 'Habilitalos en Controles rápidos del auto (Motores)' : 'Sin confirmar: el auto todavía no informó su estado' },
    { id: 'hall', label: 'Sensores Hall configurados', state: hallConfigured ? 'ok' : 'bad', detail: hallConfigured ? 'Hall conectados y habilitados' : (capabilities.wheel_odometry.reason || 'Habilitá los Hall en Preferencias') },
    { id: 'calibration', label: 'Odometría calibrada', state: hallCalibrated ? 'ok' : batch === 'verify' ? 'bad' : 'info',
      detail: hallCalibrated ? 'Pulsos por vuelta y geometría cargados' : batch === 'verify' ? 'La tanda de verificación exige odometría calibrada' : 'No se exige en la tanda de ajuste' },
    { id: 'stopped', label: 'Auto detenido y sin fallo', state: stopped && !fault ? 'ok' : stopped === false || fault ? 'bad' : 'pending',
      detail: fault ? `Falla F${fault}: limpiala antes de ensayar` : stopped ? 'Salidas apagadas' : robotState ? 'Esperá a que se detenga' : 'Sin confirmar' },
    { id: 'pose', label: 'Pose de partida', state: robotState?.pose ? 'ok' : 'pending', detail: robotState?.pose ? 'El auto informa su pose' : 'Sin confirmar: sin pose válida no se registra la odometría' },
    { id: 'plan', label: isTask ? 'Trayectoria válida' : 'Ensayo y velocidad',
      state: !speedOk || trajectoryError || !legs.length ? 'bad' : 'ok',
      detail: !speedOk ? `La velocidad debe estar entre ${MIN_MISSION_SPEED_MMS} y ${maxSpeedMmS} mm/s` : trajectoryError ?? (legs.length ? `${legs.length} tramo(s) a ${speed} mm/s` : 'No hay tramos para enviar') },
  ];
  const required = checks.filter(check => check.state !== 'info');
  const readyCount = required.filter(check => check.state === 'ok').length;
  const allReady = readyCount === required.length;
  const blocked = allReady ? null : `Faltan requisitos: ${required.filter(check => check.state !== 'ok').map(check => check.label).join(', ')}.`;
  const running = busy || runner.state.phase === 'running' || runner.state.phase === 'uploading';

  const readPose = async (): Promise<RobotPose | null> => {
    try {
      const response = await requestCommand<Record<string, unknown>>('robot.state', {}, 2500);
      return response.status === 'completed' ? parseRobotState(response.payload).pose : null;
    } catch { return null; }
  };
  const send = async () => {
    if (blocked || running) return;
    setBusy(true); setStatus('Leyendo la pose inicial…');
    const started = Date.now();
    try {
      const start = await readPose();
      if (!mounted.current) return;
      if (!start) setStatus('El auto no informa pose válida: se enviará igual y la odometría quedará sin dato.');
      else setStatus(`Enviando ${legs.length > 1 ? `${legs.length} tramos` : 'el ensayo'} al auto…`);
      const result = await runner.run(legs, speed);
      // AFR2-20261111-08: la pose final sale del informe del recorrido (marco de partida); robot.state solo si no llegó.
      const end = result.pose ? null : await readPose();
      const fresh: MinimumTestRecord = {
        id: createSequenceUid('mintest'), testId, testLabel: def.short, side, batch, repetition, createdAt: new Date().toISOString(),
        productVersion: PRODUCT_VERSION, missionSpeedMmS: speed, batteryMv: battery?.valid && battery.millivolts ? battery.millivolts : null,
        loaded: def.loaded, loadGrams: def.loaded ? LOAD_GRAMS : 0, setpoint,
        odometryStart: start ? rawPose(start) : null, odometryFinal: result.pose ? routePose(result.pose) : start && end ? relativePose(start, end) : null, measured: null, finalError: null, drift: null,
        durationMs: result.elapsedMs ?? Date.now() - started, pwmMax: null, cause: result.cause, fault: result.fault, legs: legs.length, message: result.message, notes: '',
      };
      if (!mounted.current) { setRecords(list => [...list.filter(r => recordKey(r) !== recordKey(fresh)), fresh]); return; }
      setRecords(list => [...list.filter(r => recordKey(r) !== recordKey(fresh)), fresh]);
      setSelectedId(fresh.id);
      setStatus(result.cause === 'completed' ? 'Repetición terminada. Cargá lo medido afuera.'
        : `Repetición registrada como «${CAUSE_LABEL[result.cause]}»${result.fault ? ` (F${result.fault})` : ''}. ${result.message ?? ''}`.trim());
    } finally { if (mounted.current) setBusy(false); }
  };
  const saveMeasurement = () => {
    if (!selected) return;
    const x = num(measure.x), y = num(measure.y), psi = num(measure.psi);
    const measured = x !== null && y !== null && psi !== null && [x, y, psi].every(Number.isFinite) ? { xMm: x, yMm: y, psiDeg: psi } : null;
    setRecords(list => list.map(r => r.id === selected.id ? { ...withErrors(r, measured), notes: measure.notes } : r));
    setStatus(measured ? 'Medición guardada.' : 'Faltan x, y o ψ: la repetición queda sin medición y no entra en las medias.');
  };
  const remove = (id: string) => { setRecords(list => list.filter(r => r.id !== id)); if (selectedId === id) setSelectedId(null); };
  const loadDefaultTrajectory = () => onGotoCommandsChange([
    { id: createSequenceUid('goto'), kind: 'move', direction: 'forward', distanceMm: boxMm },
    { id: createSequenceUid('goto'), kind: 'move', direction: 'backward', distanceMm: boxMm },
  ].slice(0, GOTO_MAX_COMMANDS) as GotoCommand[]);

  const ordered = [...records].sort((a, b) => a.createdAt.localeCompare(b.createdAt));
  const summary = useMemo(() => summarize(records), [records]);
  const stamp = new Date().toISOString().slice(0, 19).replace(/[:T]/g, '-');

  return <section className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4" aria-label="Pruebas mínimas">
    <div className="flex items-start justify-between gap-3">
      <div>
        <h2 className="text-base font-semibold text-[var(--text)]">Pruebas mínimas</h2>
        <p className="afr2-help">Cinco ensayos de la cátedra. Cada envío es una repetición (1 a 5): las tomás una por una y cargás lo medido afuera.</p>
      </div>
      <Button type="button" variant="emergency" onClick={() => void triggerEmergencyStop('user-stop')} aria-label="STOP">STOP</Button>
    </div>
    <div className="afr2-readiness" role="group" aria-label="Preparación del ensayo">
      <div className="afr2-readiness__head" data-ready={allReady}>
        <div className="afr2-readiness__title">
          <span className="afr2-readiness__icon" aria-hidden="true">{allReady ? '✓' : '✕'}</span>
          <div><strong>Preparación del ensayo</strong><small>Se actualizan en vivo; «Enviar al robot» se habilita con todos en verde.</small></div>
        </div>
        <span className="afr2-readiness__count" role="status">{readyCount} de {required.length} listos</span>
      </div>
      <div className="afr2-readiness__grid">
        {checks.map(check => <div key={check.id} className="afr2-readiness__card" data-state={check.state}>
          <span className="afr2-readiness__icon" aria-hidden="true">{check.state === 'ok' ? '✓' : check.state === 'bad' ? '✕' : check.state === 'info' ? 'i' : '…'}</span>
          <div><strong>{check.label}</strong><small>{check.detail}</small></div>
        </div>)}
      </div>
    </div>
    <form className="grid grid-cols-1 md:grid-cols-2 gap-4" onSubmit={event => { event.preventDefault(); void send(); }}>
      <label className="block md:col-span-2">Ensayo
        <span className="afr2-select-wrap"><select disabled={running} className="afr2-glass-field w-full min-h-11 px-3" value={testId} onChange={event => setTestId(event.target.value as MinimumTestId)}>
          {MINIMUM_TESTS.map((t, index) => <option key={t.id} value={t.id}>{index + 1}. {t.label}</option>)}
        </select><Chevron /></span>
      </label>
      {def.sides && <label className="block">{testId === 'turn' ? 'Sentido' : 'Lado'}
        <span className="afr2-select-wrap"><select disabled={running} className="afr2-glass-field w-full min-h-11 px-3" value={side ?? ''} onChange={event => setSide(event.target.value as MinimumSide)}>
          {def.sides.map(([value, label]) => <option key={value} value={value ?? ''}>{label}</option>)}
        </select><Chevron /></span>
      </label>}
      <label className="block">Velocidad de misión (mm/s, {MIN_MISSION_SPEED_MMS}…{maxSpeedMmS})
        <input disabled={running} className="afr2-glass-field w-full min-h-11 px-3" type="number" min={MIN_MISSION_SPEED_MMS} max={maxSpeedMmS} step={10} value={speed} onChange={event => setSpeed(Number(event.target.value))} />
      </label>
      <label className="block">Tanda
        <span className="afr2-select-wrap"><select disabled={running} className="afr2-glass-field w-full min-h-11 px-3" value={batch} onChange={event => setBatch(event.target.value as MinimumBatch)}>
          {(Object.keys(BATCH_LABEL) as MinimumBatch[]).map(key => <option key={key} value={key}>{BATCH_LABEL[key]}</option>)}
        </select><Chevron /></span>
      </label>
      <label className="block">Repetición
        <span className="afr2-select-wrap"><select disabled={running} className="afr2-glass-field w-full min-h-11 px-3" value={repetition} onChange={event => setRepetition(Number(event.target.value))}>
          {MINIMUM_REPETITIONS.map(n => <option key={n} value={n}>{n} de 5{records.some(r => recordKey(r) === recordKey({ testId, side, batch, repetition: n })) ? ' (ya registrada)' : ''}</option>)}
        </select><Chevron /></span>
      </label>
      {isTask && <div className="md:col-span-2 space-y-2">
        <p className="afr2-help">Trayectoria: {gotoCommands.length ? `${gotoCommands.length} comando(s), ${legs.length} tramo(s)` : 'vacía'}
          {taskEnd ? ` · termina en x ${Math.round(taskEnd.xMm)} mm, y ${Math.round(taskEnd.yMm)} mm` : ''}. Se edita en la pantalla de GOTO y es la misma lista que allí.
          {def.loaded ? ` El registro marca «con carga» (${LOAD_GRAMS} g).` : ''}</p>
        <div className="afr2-action-row">
          <label className="block">Distancia a la caja X (mm)
            <input disabled={running} className="afr2-glass-field min-h-11 px-3" type="number" min={100} max={5000} step={50} value={boxMm} onChange={event => setBoxMm(Number(event.target.value))} />
          </label>
          <Button type="button" disabled={running || !(boxMm >= 100 && boxMm <= 5000)} onClick={loadDefaultTrajectory}>Cargar ir a la caja y volver al drop</Button>
          <Button type="button" disabled={running} onClick={() => navigate('/programas?tarea=goto')}>Editar en GOTO</Button>
        </div>
      </div>}
      <div className="md:col-span-2 afr2-action-row">
        <Button type="submit" variant="primary" disabled={!!blocked || running} title={blocked ?? undefined}>{existing ? `Enviar al robot (reemplaza la repetición ${repetition})` : 'Enviar al robot'}</Button>
        <span className="afr2-help">Consigna: x {setpoint.xMm} mm · y {setpoint.yMm} mm · ψ {setpoint.psiDeg}°</span>
      </div>
    </form>
    {running && <p role="status" aria-live="polite" className="afr2-help">{runner.state.phase === 'uploading' ? 'Cargando el ensayo en el auto…' : `En el auto · tramo ${runner.state.leg}/${runner.state.legs}`}{runner.state.pose ? ` · x ${Math.round(runner.state.pose.xMm)} mm · y ${Math.round(runner.state.pose.yMm)} mm` : ''}. El auto sigue aunque se corte el WebSocket o la Wi-Fi; STOP corta el ensayo y se registra como «STOP».</p>}
    {status && <p role="status" aria-live="polite" className="afr2-help">{status}</p>}

    <div className="space-y-3">
      <h3 className="text-sm font-semibold">Medición de la repetición</h3>
      {selected ? <>
        <p className="afr2-help">{recordLabel(selected)} · {BATCH_LABEL[selected.batch]} · repetición {selected.repetition} · {CAUSE_LABEL[selected.cause]}
          {selected.fault ? ` (F${selected.fault})` : ''} · odometría (x · y · ψ): {fmt(selected.odometryFinal)}</p>
        <p className="afr2-help"><strong>Medir desde el frente del auto (lado del HC-SR04)</strong>, en la pose de partida. {measureHint(selected.testId, selected.side)}</p>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-3">
          <label className="block"><small className="afr2-help block">{MEASURE_AXES.x}</small>Medido x (mm)<input className="afr2-glass-field w-full min-h-11 px-3" type="number" step="1" value={measure.x} onChange={event => setMeasure({ ...measure, x: event.target.value })} /></label>
          <label className="block"><small className="afr2-help block">{MEASURE_AXES.y}</small>Medido y (mm)<input className="afr2-glass-field w-full min-h-11 px-3" type="number" step="1" value={measure.y} onChange={event => setMeasure({ ...measure, y: event.target.value })} /></label>
          <label className="block"><small className="afr2-help block">{MEASURE_AXES.psi}</small>Medido ψ (°)<input className="afr2-glass-field w-full min-h-11 px-3" type="number" step="0.5" value={measure.psi} onChange={event => setMeasure({ ...measure, psi: event.target.value })} /></label>
          <label className="block"><small className="afr2-help block">Opcional</small>Notas<input className="afr2-glass-field w-full min-h-11 px-3" type="text" maxLength={200} value={measure.notes} onChange={event => setMeasure({ ...measure, notes: event.target.value })} /></label>
        </div>
        <div className="afr2-action-row"><Button type="button" onClick={saveMeasurement}>Guardar medición</Button></div>
      </> : <p className="afr2-help">Terminá una repetición o elegila en la tabla de abajo para cargar x, y y ψ medidos con cinta o cuadrícula (X hacia el frente del auto —lado del HC-SR04—, Y a la izquierda, ψ antihorario, respecto de la pose de partida).</p>}
    </div>

    <div className="space-y-3">
      <h3 className="text-sm font-semibold">Registro ({records.length})</h3>
      <div className="overflow-x-auto"><table className="w-full text-sm">
        <thead><tr className="text-left"><th>Ensayo</th><th>Tanda</th><th>Rep.</th><th>Fin</th><th>Odom. x·y·ψ</th><th>Medido x·y·ψ</th><th>Error x·y·ψ</th><th>Deriva x·y·ψ</th><th /></tr></thead>
        <tbody>{ordered.map(r => <tr key={r.id} aria-selected={r.id === selectedId} className={r.id === selectedId ? 'font-semibold' : undefined}>
          <td><button type="button" className="underline" onClick={() => setSelectedId(r.id)}>{recordLabel(r)}</button></td>
          <td>{r.batch === 'adjust' ? 'Ajuste' : 'Verif.'}</td><td>{r.repetition}</td><td>{CAUSE_LABEL[r.cause]}{r.fault ? ` F${r.fault}` : ''}</td>
          <td>{fmt(r.odometryFinal)}</td><td>{fmt(r.measured)}</td><td>{fmt(r.finalError)}</td><td>{fmt(r.drift)}</td>
          <td><button type="button" className="underline" onClick={() => remove(r.id)} aria-label={`Borrar repetición ${r.repetition}`}>Borrar</button></td>
        </tr>)}{!ordered.length && <tr><td colSpan={9} className="afr2-help">Todavía no hay repeticiones.</td></tr>}</tbody>
      </table></div>
      <div className="afr2-action-row">
        <Button type="button" disabled={!records.length} onClick={() => downloadText(`pruebas-minimas-${stamp}.csv`, recordsToCsv(ordered), 'text/csv;charset=utf-8')}>Descargar CSV</Button>
        <Button type="button" disabled={!records.length} onClick={() => downloadText(`pruebas-minimas-${stamp}.json`, recordsToJson(ordered, PRODUCT_VERSION), 'application/json')}>Descargar JSON</Button>
      </div>
      <p className="afr2-help">Cada CSV lleva {RECORD_CSV_HEADER.length} columnas (ensayo, tanda, repetición, fecha, versión, velocidad, batería, carga, consigna, odometría, medida, error, deriva, duración, PWM, causa). El PWM máximo no lo informa el auto: queda vacío.</p>
    </div>

    <div className="space-y-3">
      <h3 className="text-sm font-semibold">Resumen (media ± desvío muestral de las repeticiones medidas)</h3>
      <div className="overflow-x-auto"><table className="w-full text-sm">
        <thead><tr className="text-left"><th>Ensayo</th><th>Tanda</th><th>Medidas/enviadas</th><th>Error x (mm)</th><th>Error y (mm)</th><th>Error ψ (°)</th><th>Deriva x (mm)</th><th>Deriva y (mm)</th><th>Deriva ψ (°)</th><th>Duración (s)</th></tr></thead>
        <tbody>{summary.map(row => <tr key={row.key}><td>{row.label}{row.loaded ? ` (${LOAD_GRAMS} g)` : ''}</td><td>{row.batch === 'adjust' ? 'Ajuste' : 'Verif.'}</td><td>{row.measured}/{row.runs}</td>
          <td>{formatStat(row.errorX)}</td><td>{formatStat(row.errorY)}</td><td>{formatStat(row.errorPsi)}</td><td>{formatStat(row.driftX)}</td><td>{formatStat(row.driftY)}</td><td>{formatStat(row.driftPsi)}</td><td>{formatStat(row.durationS)}</td></tr>)}
          {!summary.length && <tr><td colSpan={10} className="afr2-help">Sin datos.</td></tr>}</tbody>
      </table></div>
      <div className="afr2-action-row"><Button type="button" disabled={!summary.length} onClick={() => downloadText(`tabla-24-${stamp}.csv`, summaryToTable24Csv(summary), 'text/csv;charset=utf-8')}>Exportar Tabla 24 (CSV)</Button></div>
    </div>
  </section>;
}
