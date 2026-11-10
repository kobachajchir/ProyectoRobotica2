import { useCallback, useEffect, useMemo, useRef, useState, type FormEvent } from 'react';
import { Disclosure } from '../../../components/ui/Disclosure';
import { Button } from '../../../components/ui/Button';
import { SegmentedControl } from '../../../components/ui/SegmentedControl';
import { Chevron } from '../../../components/ui/Chevron';
import { StatusNotice } from '../../../components/ui/StatusNotice';
import { useConnection } from '../../connection/ConnectionContext';
import { ControlMap } from '../ControlMap';
import type { MapPoint, MapWaypoint } from '../controlMapTypes';
import { createSequenceUid } from '../sequences/mecanumKinematics';
import { GotoCommandList } from './GotoCommandList';
import { gotoRouteLegs, useGotoRouteRunner } from './useGotoRouteRunner';
import { formatMetres, GOTO_DIRECTIONS, GOTO_MAX_COMMANDS, GOTO_POINT_STEP_MM, gotoCommandLabel, planGotoCommands, sampleGotoPlan, type GotoCommand, type GotoDirection } from './gotoSimulation';

type MapTool = 'point' | 'arc' | 'curve' | 'edit' | 'erase';
type Handle = { id: string; part: 'end' | 'via' | 'control' };
const TOOLS: { value: MapTool; label: string }[] = [
  { value: 'point', label: 'Recta' }, { value: 'arc', label: 'Arco' },
  { value: 'curve', label: 'Curva' }, { value: 'edit', label: 'Editar' }, { value: 'erase', label: 'Borrador' },
];
const snapPoint = (point: MapPoint): MapPoint => ({ xMm: Math.round(point.xMm / GOTO_POINT_STEP_MM)*GOTO_POINT_STEP_MM || 0, yMm: Math.round(point.yMm / GOTO_POINT_STEP_MM)*GOTO_POINT_STEP_MM || 0 });
function distanceToLine(p: MapPoint, a: MapPoint, b: MapPoint): number {
  const dx = b.xMm-a.xMm, dy = b.yMm-a.yMm;
  const t = Math.max(0, Math.min(1, ((p.xMm-a.xMm)*dx+(p.yMm-a.yMm)*dy)/(dx*dx+dy*dy || 1)));
  return Math.hypot(p.xMm-a.xMm-t*dx, p.yMm-a.yMm-t*dy);
}

/** AFR2-20261110-16: robot = permission to send the whole route (odometry calibrated, link ready). */
export interface GotoRobotTarget { available: boolean; reason: string | null; maxSpeedMmS: number }
export function GotoSimulatorPanel({ commands, onCommandsChange, speedMmS, onSpeedChange, robot }: {
  commands: readonly GotoCommand[]; onCommandsChange: (commands: GotoCommand[]) => void;
  speedMmS: number; onSpeedChange: (value: number) => void; robot?: GotoRobotTarget;
}) {
  const runner = useGotoRouteRunner();
  const { stopRecord, setOperationActivity } = useConnection();
  const [direction, setDirection] = useState<GotoDirection | 'point'>('forward');
  const [metres, setMetres] = useState('2');
  const [xMetres, setXMetres] = useState('0');
  const [yMetres, setYMetres] = useState('0');
  const [cursorMs, setCursorMs] = useState(0);
  const [playing, setPlaying] = useState(false);
  const [started, setStarted] = useState(false);
  const [follow, setFollow] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [tool, setTool] = useState<MapTool>('point');
  const [pendingControl, setPendingControl] = useState<MapPoint | null>(null);
  const [selected, setSelected] = useState<Handle | null>(null);
  const [undo, setUndo] = useState<GotoCommand[][]>([]);
  const [redo, setRedo] = useState<GotoCommand[][]>([]);
  const cursorRef = useRef(0);
  const frameRef = useRef(0);
  const latestStop = useRef(stopRecord.lastIssuedAtMs);
  const plan = useMemo(() => planGotoCommands(commands, speedMmS), [commands, speedMmS]);
  const snapshot = useMemo(() => sampleGotoPlan(plan, cursorMs), [plan, cursorMs]);
  const activeIndex = started ? snapshot.activeIndex : null;
  const completed = started ? snapshot.completed : 0;
  const handles = useMemo(() => {
    let end: MapPoint = { xMm: 0, yMm: 0 };
    return commands.flatMap((command, index) => {
    if (command.kind !== 'move') end = { xMm: command.xMm, yMm: command.yMm };
    else end = { xMm: end.xMm + (command.direction === 'forward' ? command.distanceMm : command.direction === 'backward' ? -command.distanceMm : 0), yMm: end.yMm + (command.direction === 'left' ? command.distanceMm : command.direction === 'right' ? -command.distanceMm : 0) };
    const points: (MapPoint & Handle & { label: string })[] = [{ ...end, id: command.id, part: 'end', label: String(index+1) }];
    if (command.kind === 'arc') points.push({ ...command.via, id: command.id, part: 'via', label: `${index+1} · paso` });
    if (command.kind === 'curve') points.push({ ...command.control, id: command.id, part: 'control', label: `${index+1} · control` });
    return points;
    });
  }, [commands]);
  const waypoints = useMemo<MapWaypoint[]>(() => [
    ...handles.filter(p => p.part === 'end' || tool === 'edit' || tool === 'erase').map(p => ({ ...p, id: `${p.id}-${p.part}`,
      status: (selected?.id === p.id && selected.part === p.part || commands[activeIndex ?? -1]?.id === p.id ? 'active' : commands.findIndex(c => c.id === p.id) < completed ? 'completed' : 'pending') as MapWaypoint['status'] })),
    ...(pendingControl ? [{ ...pendingControl, id: 'draft-control', label: tool === 'arc' ? 'Paso del arco' : 'Control', status: 'active' as const }] : []),
  ], [handles, tool, selected, pendingControl, commands, activeIndex, completed]);
  const plannedRoute = useMemo(() => [{ xMm: 0, yMm: 0 }, ...plan.segments.flatMap(segment => segment.points.slice(1))], [plan]);
  const setCursor = useCallback((value: number) => { cursorRef.current = value; setCursorMs(value); }, []);
  const pause = useCallback(() => { cancelAnimationFrame(frameRef.current); frameRef.current = 0; setPlaying(false); }, []);
  const reset = useCallback(() => { pause(); setCursor(0); setStarted(false); }, [pause, setCursor]);

  useEffect(() => {
    setOperationActivity('simulation', playing);
    return () => setOperationActivity('simulation', false);
  }, [playing, setOperationActivity]);
  useEffect(() => {
    if (latestStop.current !== stopRecord.lastIssuedAtMs) { latestStop.current = stopRecord.lastIssuedAtMs; pause(); }
  }, [stopRecord.lastIssuedAtMs, pause]);
  useEffect(() => {
    const hidden = () => { if (document.visibilityState !== 'visible') pause(); };
    window.addEventListener('blur', pause); document.addEventListener('visibilitychange', hidden);
    return () => { cancelAnimationFrame(frameRef.current); window.removeEventListener('blur', pause); document.removeEventListener('visibilitychange', hidden); };
  }, [pause]);
  useEffect(() => {
    if (!playing || plan.error || !plan.totalDurationMs) return;
    const startWall = performance.now(), startCursor = cursorRef.current;
    let publishedAt = startWall;
    const frame = (now: number) => {
      const next = Math.min(plan.totalDurationMs, startCursor + now - startWall);
      if (next === plan.totalDurationMs || now - publishedAt >= 1000 / 30) { publishedAt = now; setCursor(next); }
      if (next >= plan.totalDurationMs) { frameRef.current = 0; setPlaying(false); }
      else frameRef.current = requestAnimationFrame(frame);
    };
    frameRef.current = requestAnimationFrame(frame);
    return () => { cancelAnimationFrame(frameRef.current); frameRef.current = 0; };
  }, [playing, plan, setCursor]);

  const commit = useCallback((next: GotoCommand[], allowInvalid = false) => {
    if (playing) return false;
    const candidate = planGotoCommands(next, speedMmS);
    if (candidate.error && !allowInvalid) { setError(candidate.error); return false; }
    setUndo(history => [...history.slice(-29), [...commands]]); setRedo([]);
    reset(); onCommandsChange(next); setError(candidate.error); return true;
  }, [playing, reset, onCommandsChange, commands, speedMmS]);
  const append = useCallback((command: GotoCommand) => {
    if (playing) return false;
    if (commands.length >= GOTO_MAX_COMMANDS) { setError(`La lista admite hasta ${GOTO_MAX_COMMANDS} comandos.`); return false; }
    const next = [...commands, command], candidate = planGotoCommands(next, speedMmS);
    if (candidate.error) { setError(candidate.error); return false; }
    return commit(next);
  }, [playing, commands, speedMmS, commit]);
  const updateHandle = (handle: Handle, point: MapPoint) => commit(commands.map(command => {
    if (command.id !== handle.id) return command;
    if (handle.part === 'via' && command.kind === 'arc') return { ...command, via: point };
    if (handle.part === 'control' && command.kind === 'curve') return { ...command, control: point };
    if (handle.part === 'end') return command.kind === 'move' ? { id: command.id, kind: 'point' as const, ...point } : { ...command, ...point };
    return command;
  }));
  const selectPoint = (point: MapPoint, toleranceMm = 250) => {
    if (playing) return;
    const snapped = snapPoint(point);
    if (tool === 'edit' && selected) {
      if (updateHandle(selected, snapped)) setSelected(null);
      return;
    }
    if (tool === 'edit' || tool === 'erase') {
      const nearest = [...handles].sort((a,b) => Math.hypot(point.xMm-a.xMm, point.yMm-a.yMm)-Math.hypot(point.xMm-b.xMm, point.yMm-b.yMm))[0];
      if (nearest && Math.hypot(point.xMm-nearest.xMm, point.yMm-nearest.yMm) <= toleranceMm) {
        if (tool === 'edit') { setSelected({ id: nearest.id, part: nearest.part }); setError(null); }
        else if (nearest.part === 'end') { commit(commands.filter(c => c.id !== nearest.id), true); setSelected(null); }
        else { commit(commands.map(c => c.id === nearest.id && c.kind !== 'move' ? { id: c.id, kind: 'point', xMm: c.xMm, yMm: c.yMm } : c), true); setSelected(null); }
        return;
      }
      const nearestSegment = plan.segments.map(segment => ({ segment, distance: Math.min(...segment.points.slice(1).map((b,i) => distanceToLine(point, segment.points[i], b))) })).sort((a,b) => a.distance-b.distance)[0];
      if (nearestSegment && nearestSegment.distance <= toleranceMm) {
        if (tool === 'erase') commit(commands.filter(c => c.id !== nearestSegment.segment.id), true);
        else setSelected({ id: nearestSegment.segment.id, part: 'end' });
        setError(null); return;
      }
      setError('Tocá un punto o un tramo del recorrido.'); return;
    }
    if (tool === 'arc' || tool === 'curve') {
      if (!pendingControl) { setPendingControl(snapped); setError(null); return; }
      const command: GotoCommand = tool === 'arc'
        ? { id: createSequenceUid('goto'), kind: 'arc', ...snapped, via: pendingControl }
        : { id: createSequenceUid('goto'), kind: 'curve', ...snapped, control: pendingControl };
      if (append(command)) setPendingControl(null);
    } else append({ id: createSequenceUid('goto'), kind: 'point', ...snapped });
  };
  const restoreHistory = (back: boolean) => {
    if (playing) return;
    const history = back ? undo : redo, previous = history[history.length-1];
    if (!previous) return;
    if (back) { setUndo(history.slice(0,-1)); setRedo(h => [...h, [...commands]]); }
    else { setRedo(history.slice(0,-1)); setUndo(h => [...h, [...commands]]); }
    reset(); setSelected(null); setPendingControl(null); setError(null); onCommandsChange(previous);
  };
  const addCommand = (event: FormEvent) => {
    event.preventDefault();
    if (direction === 'point') append({ id: createSequenceUid('goto'), kind: 'point', xMm: Number(xMetres) * 1000, yMm: Number(yMetres) * 1000 });
    else append({ id: createSequenceUid('goto'), kind: 'move', direction, distanceMm: Number(metres) * 1000 });
  };
  const play = () => {
    if (!commands.length || plan.error) return;
    if (cursorRef.current >= plan.totalDurationMs) setCursor(0);
    setSelected(null); setPendingControl(null); setStarted(true); setFollow(true); setPlaying(true);
  };
  const reorder = useCallback((index: number, delta: number) => {
    const next = [...commands], other = index + delta;
    if (other < 0 || other >= next.length) return;
    [next[index], next[other]] = [next[other], next[index]]; commit(next, true);
  }, [commands, commit]);
  const remove = (id: string) => { if (commit(commands.filter(command => command.id !== id), true)) setSelected(null); };
  const selectedPoint = selected ? handles.find(p => p.id === selected.id && p.part === selected.part) : null;
  const changeShape = (kind: 'point' | 'arc' | 'curve') => {
    if (!selected || playing) return;
    const index = commands.findIndex(c => c.id === selected.id);
    const end = handles.find(p => p.id === selected.id && p.part === 'end');
    const start = index > 0 ? handles.find(p => p.id === commands[index-1].id && p.part === 'end') : { xMm: 0, yMm: 0 };
    if (!end || !start) return;
    const control = { xMm: (start.xMm+end.xMm)/2 - (end.yMm-start.yMm)/4, yMm: (start.yMm+end.yMm)/2 + (end.xMm-start.xMm)/4 };
    const base = { id: selected.id, xMm: end.xMm, yMm: end.yMm };
    const replacement: GotoCommand = kind === 'arc' ? { ...base, kind, via: control } : kind === 'curve' ? { ...base, kind, control } : { ...base, kind };
    if (commit(commands.map(c => c.id === selected.id ? replacement : c))) setSelected({ id: selected.id, part: kind === 'arc' ? 'via' : kind === 'curve' ? 'control' : 'end' });
  };
  const hint = playing ? 'Pausá para editar el recorrido.' : tool === 'edit' ? selected ? 'Ahora tocá la nueva ubicación. También podés ajustar las coordenadas de abajo.' : 'Tocá un destino, un control o un tramo para seleccionarlo.'
    : tool === 'erase' ? 'Tocá un destino o tramo para borrarlo. Borrar un control convierte su curva en recta. Podés deshacer.'
    : tool === 'arc' ? pendingControl ? 'Paso 2 de 2: tocá el destino final del arco.' : 'Paso 1 de 2: tocá un punto por donde deba pasar el arco. Parte del último destino.'
    : tool === 'curve' ? pendingControl ? 'Paso 2 de 2: tocá el destino final de la curva.' : 'Paso 1 de 2: tocá el control que define la curvatura. Parte del último destino.'
    : 'Tocá el mapa para añadir un destino en línea recta. Ajuste de 0,1 m.';
  const tools = <div className="afr2-goto-tools">
    <SegmentedControl ariaLabel="Herramienta de dibujo del recorrido" value={tool} items={TOOLS.map(item => ({ ...item, disabled: playing }))} onChange={value => { setTool(value); setSelected(null); setPendingControl(null); setError(null); }} fill="never" />
    <p className="afr2-help" role="status">{hint}</p>
    {(error || plan.error) && <StatusNotice tone="warning" title="Revisá el recorrido">{error || plan.error}</StatusNotice>}
    <div className="afr2-action-row"><Button size="sm" disabled={playing || !undo.length} onClick={() => restoreHistory(true)}>Deshacer</Button><Button size="sm" disabled={playing || !redo.length} onClick={() => restoreHistory(false)}>Rehacer</Button>
      {(pendingControl || selected) && <Button size="sm" disabled={playing} onClick={() => { setPendingControl(null); setSelected(null); }}>Cancelar selección</Button>}</div>
    {selected && <div className="afr2-action-row">{handles.filter(p => p.id === selected.id).map(p => <Button key={p.part} size="sm" disabled={playing} aria-pressed={p.part === selected.part} onClick={() => setSelected({ id: p.id, part: p.part })}>{p.part === 'end' ? 'Destino' : p.part === 'via' ? 'Paso del arco' : 'Control de curva'}</Button>)}<Button size="sm" disabled={playing} onClick={() => remove(selected.id)}>Borrar tramo seleccionado</Button></div>}
    {selected && <div className="afr2-action-row" aria-label="Cambiar forma del tramo"><span className="afr2-help">Convertir a:</span><Button size="sm" disabled={playing} onClick={() => changeShape('point')}>Recta</Button><Button size="sm" disabled={playing} onClick={() => changeShape('arc')}>Arco</Button><Button size="sm" disabled={playing} onClick={() => changeShape('curve')}>Curva</Button></div>}
    {selectedPoint && selected && <div className="afr2-goto-fields" aria-label="Coordenadas del punto seleccionado">{(['xMm','yMm'] as const).map(axis => <label key={axis}>{axis === 'xMm' ? 'X' : 'Y'} (m)<input className="afr2-glass-field" type="number" min={-50} max={50} step="0.1" disabled={playing} value={selectedPoint[axis]/1000} onChange={event => updateHandle(selected, { xMm: selectedPoint.xMm, yMm: selectedPoint.yMm, [axis]: Number(event.target.value)*1000 })} /></label>)}</div>}
  </div>;
  const activeLabel = activeIndex === null ? started && snapshot.completed === commands.length && commands.length > 0 ? 'Recorrido completado' : 'Lista preparada' : `${playing ? 'En curso' : 'En pausa'} · ${activeIndex + 1}/${commands.length} · ${gotoCommandLabel(commands[activeIndex])}`;
  const builder = (
    <section className="afr2-goto-builder" aria-label="Agregar comandos al simulador">
      <header><h2>Agregar movimientos</h2><p className="afr2-help">Adelante es +X; izquierda es +Y. Los pasos parten del punto anterior. Los destinos del mapa usan coordenadas del plano.</p></header>
      <form onSubmit={addCommand} className="afr2-goto-add-form">
        <label>Comando<span className="afr2-select-wrap"><select className="afr2-glass-field" value={direction} disabled={playing} onChange={event => setDirection(event.target.value as GotoDirection | 'point')}>
          {(Object.entries(GOTO_DIRECTIONS) as [GotoDirection, string][]).map(([value, label]) => <option key={value} value={value}>{label}</option>)}
          <option value="point">Ir a coordenadas</option>
        </select><Chevron /></span></label>
        {direction === 'point' ? <div className="afr2-goto-fields"><label>X (m)<input className="afr2-glass-field" type="number" required min={-50} max={50} step="0.1" value={xMetres} disabled={playing} onChange={event => setXMetres(event.target.value)} /></label><label>Y (m)<input className="afr2-glass-field" type="number" required min={-50} max={50} step="0.1" value={yMetres} disabled={playing} onChange={event => setYMetres(event.target.value)} /></label></div>
          : <label>Distancia (m)<input className="afr2-glass-field" type="number" required min="0.1" max="10" step="0.1" value={metres} disabled={playing} onChange={event => setMetres(event.target.value)} /></label>}
        <Button type="submit" disabled={playing || commands.length >= GOTO_MAX_COMMANDS}>Agregar movimiento</Button>
      </form>
      <label className="afr2-goto-speed">Velocidad máxima simulada <strong className="afr2-tabular">{formatMetres(speedMmS)} m/s</strong><input type="range" min={50} max={500} step={50} value={speedMmS} disabled={playing} onChange={event => { reset(); setError(null); onSpeedChange(Number(event.target.value)); }} /></label>
    </section>
  );
  const legs = useMemo(() => plan.error ? [] : gotoRouteLegs(plan), [plan]);
  const robotRunning = runner.state.phase === 'running';
  const sendBlocked = !robot ? 'Envío al auto no disponible en esta vista.'
    : !robot.available ? robot.reason || 'Falta odometría calibrada o enlace con el auto.'
    : !commands.length ? 'Agregá al menos un movimiento.'
    : plan.error ? 'Corregí el recorrido antes de enviarlo.'
    : playing ? 'Pausá la simulación para enviar.'
    : !legs.length ? 'El recorrido no tiene desplazamiento.' : null;
  const sendSpeed = Math.min(speedMmS, robot?.maxSpeedMmS ?? speedMmS);
  const robotStatus = runner.state.phase === 'running' ? `Enviando al auto · tramo ${runner.state.leg}/${runner.state.legs}`
    : runner.state.phase === 'completed' ? `Recorrido completado en el auto (${runner.state.legs} tramos)`
    : runner.state.phase === 'idle' ? null : runner.state.message;
  const sendPanel = <section className="afr2-goto-send" aria-label="Enviar recorrido al auto">
    <h3>Auto</h3>
    <p className="afr2-help">{robotRunning ? 'Tocá STOP o Cancelar para detenerlo.' : sendBlocked ?? `${legs.length} ${legs.length === 1 ? 'tramo' : 'tramos'} · ${formatMetres(sendSpeed)} m/s · rumbo fijo`}</p>
    {robotStatus && <p role="status" aria-live="polite" className="afr2-goto-send__status" data-phase={runner.state.phase}>{robotStatus}</p>}
    {robotRunning
      ? <Button variant="secondary" onClick={() => void runner.cancel()}>Cancelar envío</Button>
      : <Button variant="primary" disabled={!!sendBlocked} title={sendBlocked ?? undefined} onClick={() => { pause(); void runner.run(legs, sendSpeed); }}>Enviar recorrido</Button>}
  </section>;
  const playback = <div className="afr2-goto-playback">
    <Disclosure title="Recorrido local">{builder}</Disclosure>
    <p role="status" aria-live="polite" aria-atomic="true">{activeLabel}</p>
    <p className="afr2-help">Recorrido: {formatMetres(plan.distanceMm)} m</p>
    <div className="afr2-action-row"><Button variant="primary" disabled={!commands.length || !!plan.error} onClick={playing ? pause : play}>{playing ? 'Pausar' : cursorMs >= plan.totalDurationMs && commands.length ? 'Repetir recorrido' : started ? 'Continuar' : 'Simular recorrido'}</Button>
      <Button disabled={playing || !started} onClick={reset}>Reiniciar</Button><Button variant="subtle" disabled={playing || robotRunning || !commands.length} onClick={() => commit([])}>Vaciar lista</Button></div>
    {sendPanel}
  </div>;
  return <div className="afr2-goto-workspace">
    <ControlMap compact title="Simulador GOTO" initialView="top" requestedMotion={null} onViewChange={() => {}}
      toolPanel={tools}
      routePanel={<GotoCommandList commands={commands} activeIndex={activeIndex} completed={completed} playing={playing} follow={follow} onFollow={setFollow} onReorder={reorder} onRemove={remove} onSelect={id => { if (!playing) { setTool('edit'); setSelected({ id, part: 'end' }); setPendingControl(null); } }} selectedId={selected?.id} />}
      sidebarPanel={playback} waypoints={waypoints} plannedRoute={plannedRoute} onSelectPoint={selectPoint} pickingDisabled={playing}
      simulationFeed={{ pose: snapshot.pose, trail: snapshot.trail, wheelAngles: snapshot.wheelAngles, cursorMs, totalDurationMs: plan.totalDurationMs, isPlaying: playing,
        profileSummary: 'Mecanum ideal · Rectas, arcos y curvas a rumbo constante. Vista previa local; sin feedback físico.', onResetOrigin: reset }} />
  </div>;
}
