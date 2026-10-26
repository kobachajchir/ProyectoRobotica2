/** AFR2-20261024-10. Local ideal Mecanum translation planner, not robot odometry.
 * X points forward, Y left, in millimetres. Heading remains zero. A map target
 * is absolute in this simulated frame; cardinal steps start at the last endpoint.
 * Analytic segments preserve every corner and reach their exact endpoint.
 */
import type { RobotPose } from '../../../protocol/robotState';
import { wheelAnglesFromTravel, type RobotWheelValues } from '../../../components/three/robotWheelMotion';
import type { MapPoint, TrailPoint } from '../controlMapTypes';
import { EDITOR_MAX_BLOCKS_PER_TRACK, EDITOR_MAX_DURATION_MS } from '../sequences/sequenceTypes';

export type GotoDirection = 'forward' | 'backward' | 'left' | 'right';
export type GotoCommand = { id: string } & (
  | { kind: 'move'; direction: GotoDirection; distanceMm: number }
  | { kind: 'point'; xMm: number; yMm: number }
);
export const GOTO_DIRECTIONS: Record<GotoDirection, string> = {
  forward: 'Adelante', backward: 'Atras', left: 'Izquierda', right: 'Derecha',
};
export const GOTO_MAX_COMMANDS = EDITOR_MAX_BLOCKS_PER_TRACK;
export const GOTO_POINT_STEP_MM = 100;
export const GOTO_WORLD_LIMIT_MM = 50_000;
const metreFormat = new Intl.NumberFormat('es-AR', { maximumFractionDigits: 2 });
export const formatMetres = (mm: number) => metreFormat.format(mm / 1000);
export function gotoCommandLabel(command: GotoCommand): string {
  return command.kind === 'move'
    ? `${GOTO_DIRECTIONS[command.direction]} ${formatMetres(command.distanceMm)} m`
    : `Ir a X ${formatMetres(command.xMm)} m · Y ${formatMetres(command.yMm)} m`;
}
export interface GotoSegment {
  id: string; start: MapPoint; end: MapPoint; startMs: number; endMs: number; distanceMm: number;
}
export interface GotoPlan { segments: GotoSegment[]; totalDurationMs: number; distanceMm: number; error: string | null }
export function planGotoCommands(commands: readonly GotoCommand[], maxSpeedMmS: number): GotoPlan {
  const segments: GotoSegment[] = [];
  let start: MapPoint = { xMm: 0, yMm: 0 }, time = 0, distance = 0;
  const fail = (error: string): GotoPlan => ({ segments, totalDurationMs: time, distanceMm: distance, error });
  if (!Number.isFinite(maxSpeedMmS) || maxSpeedMmS < 50 || maxSpeedMmS > 500) return fail('La velocidad simulada debe estar entre 0,05 y 0,5 m/s.');
  if (commands.length > GOTO_MAX_COMMANDS) return fail(`La lista admite hasta ${GOTO_MAX_COMMANDS} comandos.`);
  for (const command of commands) {
    let end: MapPoint;
    if (command.kind === 'point') end = { xMm: command.xMm, yMm: command.yMm };
    else {
      if (!Number.isFinite(command.distanceMm) || command.distanceMm < 100 || command.distanceMm > 10_000) return fail('Cada movimiento debe medir entre 0,1 y 10 m.');
      end = { ...start };
      if (command.direction === 'forward') end.xMm += command.distanceMm;
      if (command.direction === 'backward') end.xMm -= command.distanceMm;
      if (command.direction === 'left') end.yMm += command.distanceMm;
      if (command.direction === 'right') end.yMm -= command.distanceMm;
    }
    if (!Number.isFinite(end.xMm) || !Number.isFinite(end.yMm) || Math.abs(end.xMm) > GOTO_WORLD_LIMIT_MM || Math.abs(end.yMm) > GOTO_WORLD_LIMIT_MM) return fail('El recorrido debe mantenerse entre −50 y 50 m por eje.');
    const length = Math.hypot(end.xMm - start.xMm, end.yMm - start.yMm);
    if (length < 1) return fail('Un destino coincide con el punto anterior. Elegi otro punto o quita ese comando.');
    const duration = Math.max(10, Math.ceil(length / maxSpeedMmS * 100) * 10);
    if (time + duration > EDITOR_MAX_DURATION_MS) return fail('La simulacion completa admite hasta 10 minutos.');
    segments.push({ id: command.id, start, end, startMs: time, endMs: time + duration, distanceMm: length });
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
      trail.push({ ...pose(point, index + 1), segmentStart: false });
    } else {
      activeIndex = index;
      const t = Math.max(0, (cursor - segment.startMs) / (segment.endMs - segment.startMs));
      point = { xMm: segment.start.xMm + (segment.end.xMm - segment.start.xMm) * t, yMm: segment.start.yMm + (segment.end.yMm - segment.start.yMm) * t };
      if (t > 0) trail.push({ ...pose(point, index + 1), segmentStart: false });
      break;
    }
  }
  // Constant heading permits analytic wheel travel from net X/Y displacement.
  // It remains deterministic when scrubbing, replaying or returning to origin.
  const wheelAngles = wheelAnglesFromTravel({ fl: point.xMm - point.yMm, fr: point.xMm + point.yMm,
    rl: point.xMm + point.yMm, rr: point.xMm - point.yMm });
  return { pose: pose(point, Math.floor(cursor / 10)), trail, activeIndex, completed, wheelAngles };
}
