import { MOTION_REFERENCE } from '../motionReference';
/** AFR2-20261024-10. Local ideal Mecanum translation planner, not robot odometry.
 * X points forward, Y left, in millimetres. Heading remains zero. A map target
 * is absolute in this simulated frame; cardinal steps start at the last endpoint.
 * Arcs pass through a third point; curves use a quadratic Bezier control.
 * Length-indexed samples preserve endpoints and give a common playback clock.
 */
import type { RobotPose } from '../../../protocol/robotState';
import { wheelAnglesFromTravel, type RobotWheelValues } from '../../../components/three/robotWheelMotion';
import type { MapPoint, TrailPoint } from '../controlMapTypes';
import { EDITOR_MAX_BLOCKS_PER_TRACK, EDITOR_MAX_DURATION_MS } from '../sequences/sequenceTypes';

export type GotoDirection = 'forward' | 'backward' | 'left' | 'right';
export type GotoCommand = { id: string } & (
  | { kind: 'move'; direction: GotoDirection; distanceMm: number }
  | { kind: 'point'; xMm: number; yMm: number }
  | { kind: 'arc'; xMm: number; yMm: number; via: MapPoint }
  | { kind: 'curve'; xMm: number; yMm: number; control: MapPoint }
);
export const GOTO_DIRECTIONS: Record<GotoDirection, string> = {
  forward: 'Adelante', backward: 'Atrás', left: 'Izquierda', right: 'Derecha',
};
export const GOTO_MAX_COMMANDS = EDITOR_MAX_BLOCKS_PER_TRACK;
export const GOTO_POINT_STEP_MM = 100;
export const GOTO_WORLD_LIMIT_MM = 50_000;
const metreFormat = new Intl.NumberFormat('es-AR', { maximumFractionDigits: 2 });
export const formatMetres = (mm: number) => metreFormat.format(mm / 1000);
export function gotoCommandLabel(command: GotoCommand): string {
  return command.kind === 'move'
    ? `${GOTO_DIRECTIONS[command.direction]} ${formatMetres(command.distanceMm)} m`
    : `${command.kind === 'arc' ? 'Arco' : command.kind === 'curve' ? 'Curva' : 'Ir a'} X ${formatMetres(command.xMm)} m · Y ${formatMetres(command.yMm)} m`;
}
export interface GotoSegment {
  id: string; start: MapPoint; end: MapPoint; startMs: number; endMs: number; distanceMm: number;
  points: MapPoint[]; lengths: number[];
}
export interface GotoPlan { segments: GotoSegment[]; totalDurationMs: number; distanceMm: number; error: string | null }
const inWorld = (p: MapPoint) => Number.isFinite(p.xMm) && Number.isFinite(p.yMm) && Math.abs(p.xMm) <= GOTO_WORLD_LIMIT_MM && Math.abs(p.yMm) <= GOTO_WORLD_LIMIT_MM;
const distanceBetween = (a: MapPoint, b: MapPoint) => Math.hypot(a.xMm - b.xMm, a.yMm - b.yMm);
/** Source geometry shared by preview, click picking and playback. No physical GOTO command changes. */
function pathPoints(start: MapPoint, end: MapPoint, command: GotoCommand): MapPoint[] | string {
  if (command.kind === 'curve') {
    const c = command.control;
    if (!inWorld(c)) return 'El control de la curva debe estar entre −50 y 50 m por eje.';
    const count = Math.min(256, Math.max(32, Math.ceil((distanceBetween(start, c) + distanceBetween(c, end)) / 50)));
    return Array.from({ length: count + 1 }, (_, i) => {
      const t = i / count, u = 1 - t;
      return { xMm: u*u*start.xMm + 2*u*t*c.xMm + t*t*end.xMm, yMm: u*u*start.yMm + 2*u*t*c.yMm + t*t*end.yMm };
    });
  }
  if (command.kind === 'arc') {
    const b = { xMm: command.via.xMm - start.xMm, yMm: command.via.yMm - start.yMm };
    const c = { xMm: end.xMm - start.xMm, yMm: end.yMm - start.yMm };
    const d = 2*(b.xMm*c.yMm - b.yMm*c.xMm);
    if (!inWorld(command.via) || distanceBetween(start, command.via) < 1 || distanceBetween(end, command.via) < 1 || Math.abs(d) < 1) return 'Para el arco elegí tres puntos distintos que no estén alineados.';
    const b2 = b.xMm*b.xMm + b.yMm*b.yMm, c2 = c.xMm*c.xMm + c.yMm*c.yMm;
    const center = { xMm: start.xMm + (b2*c.yMm - c2*b.yMm)/d, yMm: start.yMm + (b.xMm*c2 - c.xMm*b2)/d };
    const angle = (p: MapPoint) => Math.atan2(p.yMm - center.yMm, p.xMm - center.xMm);
    const positive = (a: number) => (a % (2*Math.PI) + 2*Math.PI) % (2*Math.PI);
    const a0 = angle(start), ccw = positive(angle(end) - a0);
    const sweep = positive(angle(command.via) - a0) < ccw ? ccw : ccw - 2*Math.PI;
    const radius = distanceBetween(start, center);
    const count = Math.min(256, Math.max(32, Math.ceil(Math.abs(sweep)*radius/50), Math.ceil(Math.abs(sweep)/(Math.PI/60))));
    return Array.from({ length: count + 1 }, (_, i) => i === 0 ? start : i === count ? end : ({ xMm: center.xMm + radius*Math.cos(a0+sweep*i/count), yMm: center.yMm + radius*Math.sin(a0+sweep*i/count) }));
  }
  return [start, end];
}
export function pointOnGotoSegment(segment: GotoSegment, fraction: number): { point: MapPoint; before: MapPoint[] } {
  const target = Math.max(0, Math.min(1, fraction))*segment.distanceMm;
  let i = 1;
  while (i < segment.lengths.length - 1 && segment.lengths[i] < target) i++;
  const a = segment.points[i-1], b = segment.points[i];
  const ratio = (target - segment.lengths[i-1]) / Math.max(.000001, segment.lengths[i] - segment.lengths[i-1]);
  return { point: { xMm: a.xMm + (b.xMm-a.xMm)*ratio, yMm: a.yMm + (b.yMm-a.yMm)*ratio }, before: segment.points.slice(1, i) };
}
export function planGotoCommands(commands: readonly GotoCommand[], maxSpeedMmS: number): GotoPlan {
  const segments: GotoSegment[] = [];
  let start: MapPoint = { xMm: 0, yMm: 0 }, time = 0, distance = 0;
  const fail = (error: string): GotoPlan => ({ segments, totalDurationMs: time, distanceMm: distance, error });
  if (!Number.isFinite(maxSpeedMmS) || maxSpeedMmS < 50 || maxSpeedMmS > 500) return fail('La velocidad simulada debe estar entre 0,05 y 0,5 m/s.');
  if (commands.length > GOTO_MAX_COMMANDS) return fail(`La lista admite hasta ${GOTO_MAX_COMMANDS} comandos.`);
  for (const command of commands) {
    let end: MapPoint;
    if (command.kind !== 'move') end = { xMm: command.xMm, yMm: command.yMm };
    else {
      if (!Number.isFinite(command.distanceMm) || command.distanceMm < 100 || command.distanceMm > 10_000) return fail('Cada movimiento debe medir entre 0,1 y 10 m.');
      end = { ...start };
      if (command.direction === 'forward') end.xMm += command.distanceMm;
      if (command.direction === 'backward') end.xMm -= command.distanceMm;
      if (command.direction === 'left') end.yMm += command.distanceMm;
      if (command.direction === 'right') end.yMm -= command.distanceMm;
    }
    if (!Number.isFinite(end.xMm) || !Number.isFinite(end.yMm) || Math.abs(end.xMm) > GOTO_WORLD_LIMIT_MM || Math.abs(end.yMm) > GOTO_WORLD_LIMIT_MM) return fail('El recorrido debe mantenerse entre −50 y 50 m por eje.');
    if (distanceBetween(start, end) < 1) return fail('Un destino coincide con el punto anterior. Elegí otro punto.');
    const points = pathPoints(start, end, command);
    if (typeof points === 'string') return fail(points);
    if (points.some(p => !inWorld(p))) return fail('El arco o curva sale del límite de ±50 m del mapa.');
    const lengths = [0];
    for (let i = 1; i < points.length; i++) lengths.push(lengths[i-1] + distanceBetween(points[i-1], points[i]));
    const length = lengths[lengths.length-1];
    if (length < 1) return fail('Un destino coincide con el punto anterior. Elegí otro punto o quitá ese comando.');
    const duration = Math.max(10, Math.ceil(length / maxSpeedMmS * 100) * 10);
    if (time + duration > EDITOR_MAX_DURATION_MS) return fail('La simulación completa admite hasta 10 minutos.');
    segments.push({ id: command.id, start, end, points, lengths, startMs: time, endMs: time + duration, distanceMm: length });
    start = end; time += duration; distance += length;
  }
  return { segments, totalDurationMs: time, distanceMm: distance, error: null };
}
export function sampleGotoPlan(plan: GotoPlan, atMs: number): {
  pose: RobotPose; trail: TrailPoint[]; activeIndex: number | null; completed: number; wheelAngles: RobotWheelValues;
} {
  const cursor = Math.max(0, Math.min(plan.totalDurationMs, Number.isFinite(atMs) ? atMs : 0));
  const pose = (point: MapPoint, sequence: number): RobotPose => ({ ...point, schema: 'afr2-pose-1', frameId: 'sim-goto-local', sequence, thetaMrad: 0, ageMs: 0 });
  let point: MapPoint = { xMm: 0, yMm: 0 }, completed = 0, activeIndex: number | null = null;
  const trail: TrailPoint[] = [{ ...pose(point, 0), segmentStart: true }];
  for (const [index, segment] of plan.segments.entries()) {
    if (cursor >= segment.endMs) {
      point = segment.end; completed++;
      for (const p of segment.points.slice(1)) trail.push({ ...pose(p, index + 1), segmentStart: false });
    } else {
      activeIndex = index;
      const t = Math.max(0, (cursor - segment.startMs) / (segment.endMs - segment.startMs));
      const sample = pointOnGotoSegment(segment, t);
      point = sample.point;
      for (const p of sample.before) trail.push({ ...pose(p, index + 1), segmentStart: false });
      if (t > 0) trail.push({ ...pose(point, index + 1), segmentStart: false });
      break;
    }
  }
  // Constant heading permits analytic wheel travel from net X/Y displacement.
  // It remains deterministic when scrubbing, replaying or returning to origin.
  const forwardTravelMm = point.xMm * MOTION_REFERENCE.forwardWheelSign;
  const lateralTravelMm = point.yMm * MOTION_REFERENCE.lateralWheelSign;
  const wheelAngles = wheelAnglesFromTravel({ fl: forwardTravelMm - lateralTravelMm, fr: forwardTravelMm + lateralTravelMm,
    rl: forwardTravelMm + lateralTravelMm, rr: forwardTravelMm - lateralTravelMm });
  return { pose: pose(point, Math.floor(cursor / 10)), trail, activeIndex, completed, wheelAngles };
}
