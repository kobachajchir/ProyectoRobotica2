import { useCallback, useEffect, useMemo, useRef, useState, type FormEvent } from 'react';
import { Disclosure } from '../../../components/ui/Disclosure';
import { Button } from '../../../components/ui/Button';
import { Chevron } from '../../../components/ui/Chevron';
import { StatusNotice } from '../../../components/ui/StatusNotice';
import { useConnection } from '../../connection/ConnectionContext';
import { ControlMap } from '../ControlMap';
import type { MapPoint, MapWaypoint } from '../controlMapTypes';
import { createSequenceUid } from '../sequences/mecanumKinematics';
import { GotoCommandList } from './GotoCommandList';
import { formatMetres, GOTO_DIRECTIONS, GOTO_MAX_COMMANDS, GOTO_POINT_STEP_MM, gotoCommandLabel, planGotoCommands, sampleGotoPlan, type GotoCommand, type GotoDirection } from './gotoSimulation';

export function GotoSimulatorPanel({ commands, onCommandsChange, speedMmS, onSpeedChange }: {
  commands: readonly GotoCommand[]; onCommandsChange: (commands: GotoCommand[]) => void;
  speedMmS: number; onSpeedChange: (value: number) => void;
}) {
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
  const cursorRef = useRef(0);
  const frameRef = useRef(0);
  const latestStop = useRef(stopRecord.lastIssuedAtMs);
  const plan = useMemo(() => planGotoCommands(commands, speedMmS), [commands, speedMmS]);
  const snapshot = useMemo(() => sampleGotoPlan(plan, cursorMs), [plan, cursorMs]);
  const activeIndex = started ? snapshot.activeIndex : null;
  const completed = started ? snapshot.completed : 0;
  const waypoints = useMemo<MapWaypoint[]>(() => plan.segments.map((segment, index) => ({ ...segment.end, id: segment.id,
    status: index < completed ? 'completed' : index === activeIndex ? 'active' : 'pending' })), [plan, activeIndex, completed]);
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

  const commit = useCallback((next: GotoCommand[]) => {
    if (playing) return;
    reset(); onCommandsChange(next); setError(null);
  }, [playing, reset, onCommandsChange]);
  const append = useCallback((command: GotoCommand) => {
    if (playing) return;
    if (commands.length >= GOTO_MAX_COMMANDS) { setError(`La lista admite hasta ${GOTO_MAX_COMMANDS} comandos.`); return; }
    const next = [...commands, command], candidate = planGotoCommands(next, speedMmS);
    if (candidate.error) { setError(candidate.error); return; }
    commit(next);
  }, [playing, commands, speedMmS, commit]);
  const selectPoint = useCallback((point: MapPoint) => {
    append({ id: createSequenceUid('goto'), kind: 'point',
      xMm: Math.round(point.xMm / GOTO_POINT_STEP_MM) * GOTO_POINT_STEP_MM || 0,
      yMm: Math.round(point.yMm / GOTO_POINT_STEP_MM) * GOTO_POINT_STEP_MM || 0 });
  }, [append]);
  const addCommand = (event: FormEvent) => {
    event.preventDefault();
    if (direction === 'point') append({ id: createSequenceUid('goto'), kind: 'point', xMm: Number(xMetres) * 1000, yMm: Number(yMetres) * 1000 });
    else append({ id: createSequenceUid('goto'), kind: 'move', direction, distanceMm: Number(metres) * 1000 });
  };
  const play = () => {
    if (!commands.length || plan.error) return;
    if (cursorRef.current >= plan.totalDurationMs) setCursor(0);
    setStarted(true); setFollow(true); setPlaying(true);
  };
  const reorder = useCallback((index: number, delta: number) => {
    const next = [...commands], other = index + delta;
    if (other < 0 || other >= next.length) return;
    [next[index], next[other]] = [next[other], next[index]]; commit(next);
  }, [commands, commit]);
  const remove = useCallback((id: string) => commit(commands.filter(command => command.id !== id)), [commands, commit]);
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
      {(error || plan.error) && <StatusNotice tone="warning" title="Revisá el recorrido">{error || plan.error}</StatusNotice>}
      <label className="afr2-goto-speed">Velocidad máxima simulada <strong className="afr2-tabular">{formatMetres(speedMmS)} m/s</strong><input type="range" min={50} max={500} step={50} value={speedMmS} disabled={playing} onChange={event => { reset(); setError(null); onSpeedChange(Number(event.target.value)); }} /></label>
    </section>
  );
  const playback = <div className="afr2-goto-playback">
    <Disclosure title="Recorrido local">{builder}</Disclosure>
    <p role="status" aria-live="polite" aria-atomic="true">{activeLabel}</p>
    <p className="afr2-help">Recorrido: {formatMetres(plan.distanceMm)} m</p>
    <div className="afr2-action-row"><Button variant="primary" disabled={!commands.length || !!plan.error} onClick={playing ? pause : play}>{playing ? 'Pausar' : cursorMs >= plan.totalDurationMs && commands.length ? 'Repetir recorrido' : started ? 'Continuar' : 'Simular recorrido'}</Button>
      <Button disabled={playing || !started} onClick={reset}>Reiniciar</Button><Button variant="subtle" disabled={playing || !commands.length} onClick={() => commit([])}>Vaciar lista</Button></div>
  </div>;
  return <div className="afr2-goto-workspace">
    <ControlMap compact title="Simulador GOTO" initialView="top" requestedMotion={null} onViewChange={() => {}}
      routePanel={<GotoCommandList commands={commands} activeIndex={activeIndex} completed={completed} playing={playing} follow={follow} onFollow={setFollow} onReorder={reorder} onRemove={remove} />}
      sidebarPanel={playback} waypoints={waypoints} onSelectPoint={selectPoint} pickingDisabled={playing}
      simulationFeed={{ pose: snapshot.pose, trail: snapshot.trail, wheelAngles: snapshot.wheelAngles, cursorMs, totalDurationMs: plan.totalDurationMs, isPlaying: playing,
        profileSummary: 'Traslación Mecanum ideal, rumbo constante. Cada destino se recorre en orden; sin feedback físico.', onResetOrigin: reset }} />
  </div>;
}
