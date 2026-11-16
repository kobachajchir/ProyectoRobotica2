import { MOTION_REFERENCE } from '../motionReference';
/** AFR2-20261024-10. Local ideal Mecanum translation planner, not robot odometry.
 * X points forward, Y left, in millimetres. AFR2-20261113-01: heading per orientation mode and turn nodes. A map target
 * is absolute in this simulated frame; cardinal steps start at the last endpoint.
 * Arcs pass through a third point; curves use a quadratic Bezier control.
 * Length-indexed samples preserve endpoints and give a common playback clock.
 */
import type { RobotPose } from '../../../protocol/robotState';
import { wheelAnglesFromTravel, type RobotWheelValues } from '../../../components/three/robotWheelMotion';
import type { MapPoint, TrailPoint } from '../controlMapTypes';
import { EDITOR_MAX_BLOCKS_PER_TRACK, EDITOR_MAX_DURATION_MS } from '../sequences/sequenceTypes';

export type GotoDirection = 'forward' | 'backward' | 'left' | 'right';
/** AFR2-20261113-01: «Frontal» mantiene el rumbo (traslación lateral, como antes); «Seguir camino» gira para encarar cada tramo. */
export type GotoOrientation = 'fixed' | 'path';
/** Nodo de giro sobre un tramo: en `at` (fracción del largo, 0..1) el auto se detiene y gira en el lugar hasta `headingMrad`
 * (rumbo absoluto del plano: 0 = frente inicial, + antihorario, ±3142). Ese rumbo se mantiene hasta el fin del comando, también
 * en «Seguir camino»; el comando siguiente vuelve a encarar el camino. */
export interface GotoTurn { id: string; at: number; headingMrad: number }
export type GotoCommand = { id: string; turns?: GotoTurn[] } & (
  | { kind: 'move'; direction: GotoDirection; distanceMm: number }
  | { kind: 'point'; xMm: number; yMm: number }
  | { kind: 'arc'; xMm: number; yMm: number; via: MapPoint }
  | { kind: 'curve'; xMm: number; yMm: number; control: MapPoint }
  /** AFR2-20261112-07: trazo a mano alzada. `points` = vértices intermedios (absolutos); empieza en el destino anterior. */
  | { kind: 'path'; xMm: number; yMm: number; points: MapPoint[] }
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
    : command.kind === 'path' ? `Trazo · ${command.points.length + 1} ${command.points.length ? 'tramos' : 'tramo'} · fin X ${formatMetres(command.xMm)} m · Y ${formatMetres(command.yMm)} m`
    : `${command.kind === 'arc' ? 'Arco' : command.kind === 'curve' ? 'Curva' : 'Ir a'} X ${formatMetres(command.xMm)} m · Y ${formatMetres(command.yMm)} m`;
}
export interface GotoSegment {
  id: string; start: MapPoint; end: MapPoint; startMs: number; endMs: number; distanceMm: number;
  points: MapPoint[]; lengths: number[];
  /** AFR2-20261113-01: poses en el tiempo (posición + rumbo). Entre dos cuadros todo es lineal; mismo punto = giro en el lugar. */
  frames: GotoFrame[];
  /** Nodos de giro ubicados sobre el tramo, con su punto en el plano. */
  turnPoints: (GotoTurn & MapPoint & { distanceMm: number })[];
}
/** tMs absoluto; theta en radianes, continuo (sin envolver), 0 = frente inicial, + antihorario. `wheels` = recorrido acumulado (mm). */
export interface GotoFrame extends MapPoint { tMs: number; theta: number; wheels: RobotWheelValues; turnId?: string }
export interface GotoPlan { segments: GotoSegment[]; totalDurationMs: number; distanceMm: number; error: string | null; orientation: GotoOrientation }
const inWorld = (p: MapPoint) => Number.isFinite(p.xMm) && Number.isFinite(p.yMm) && Math.abs(p.xMm) <= GOTO_WORLD_LIMIT_MM && Math.abs(p.yMm) <= GOTO_WORLD_LIMIT_MM;
const distanceBetween = (a: MapPoint, b: MapPoint) => Math.hypot(a.xMm - b.xMm, a.yMm - b.yMm);
/** Envuelve a (−π, π]. */
export const wrapAngle = (a: number) => { const t = ((a + Math.PI) % (2*Math.PI) + 2*Math.PI) % (2*Math.PI) - Math.PI; return t === -Math.PI ? Math.PI : t; };
/** Radio de giro equivalente del auto (para tiempo de giro y recorrido de ruedas en la simulación). */
const TURN_RADIUS_MM = 200;
/** En «Seguir camino», cambios de dirección mayores giran en el lugar antes de avanzar; los menores giran mientras avanza. */
const CORNER_TURN_RAD = 20 * Math.PI / 180;
/** Source geometry shared by preview, click picking and playback. No physical GOTO command changes. */
function pathPoints(start: MapPoint, end: MapPoint, command: GotoCommand): MapPoint[] | string {
  if (command.kind === 'path') {
    const points = [start, ...command.points, end];
    if (points.some(p => !inWorld(p))) return 'El trazo sale del límite de ±50 m del mapa.';
    if (points.slice(1).some((p, i) => distanceBetween(points[i], p) < 1)) return 'El trazo tiene puntos repetidos. Dibujalo de nuevo.';
    return points;
  }
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
/** Fracción (0..1, por largo) del punto del tramo más cercano a `point`, y su distancia. */
export function nearestOnGotoSegment(segment: GotoSegment, point: MapPoint): { fraction: number; distanceMm: number; point: MapPoint } {
  let best = { fraction: 0, distanceMm: Infinity, point: segment.start };
  for (let i = 1; i < segment.points.length; i++) {
    const a = segment.points[i-1], b = segment.points[i], dx = b.xMm-a.xMm, dy = b.yMm-a.yMm;
    const t = Math.max(0, Math.min(1, ((point.xMm-a.xMm)*dx + (point.yMm-a.yMm)*dy) / (dx*dx + dy*dy || 1)));
    const p = { xMm: a.xMm + t*dx, yMm: a.yMm + t*dy }, d = distanceBetween(p, point);
    if (d < best.distanceMm) best = { fraction: (segment.lengths[i-1] + t*(segment.lengths[i]-segment.lengths[i-1])) / (segment.distanceMm || 1), distanceMm: d, point: p };
  }
  return best;
}
/** Rumbo simulado (rad, envuelto) del auto al pasar por la fracción `fraction` del tramo, antes de un giro ubicado ahí. */
export function headingOnGotoSegment(segment: GotoSegment, fraction: number): number {
  const target = Math.max(0, Math.min(1, fraction)) * segment.distanceMm;
  let travelled = 0;
  for (let i = 1; i < segment.frames.length; i++) {
    const a = segment.frames[i-1], b = segment.frames[i], step = distanceBetween(a, b);
    if (step < .5) continue;
    if (travelled + step >= target - .5) return wrapAngle(a.theta + (b.theta - a.theta) * Math.max(0, Math.min(1, (target - travelled) / step)));
    travelled += step;
  }
  return wrapAngle(segment.frames[segment.frames.length - 1]?.theta ?? 0);
}
/** Recorrido de cada rueda (mm) para un paso dado en el plano (dx, dy) con rumbo medio `heading` y giro `dTheta`. */
function wheelStep(base: RobotWheelValues, dx: number, dy: number, heading: number, dTheta: number): RobotWheelValues {
  const c = Math.cos(heading), s = Math.sin(heading);
  const f = (dx*c + dy*s) * MOTION_REFERENCE.forwardWheelSign, l = (-dx*s + dy*c) * MOTION_REFERENCE.lateralWheelSign;
  const r = dTheta * TURN_RADIUS_MM * MOTION_REFERENCE.yawWheelSign;
  return { fl: base.fl + f - l - r, fr: base.fr + f + l + r, rl: base.rl + f + l - r, rr: base.rr + f - l + r };
}
export function planGotoCommands(commands: readonly GotoCommand[], maxSpeedMmS: number, orientation: GotoOrientation = 'fixed'): GotoPlan {
  const segments: GotoSegment[] = [];
  let start: MapPoint = { xMm: 0, yMm: 0 }, time = 0, distance = 0, heading = 0;
  let wheels: RobotWheelValues = { fl: 0, fr: 0, rl: 0, rr: 0 };
  const fail = (error: string): GotoPlan => ({ segments, totalDurationMs: time, distanceMm: distance, error, orientation });
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
    const turns = [...(command.turns ?? [])].filter(t => Number.isFinite(t.at) && Number.isFinite(t.headingMrad)).sort((a, b) => a.at - b.at);
    if (turns.some(t => t.at < 0 || t.at > 1 || Math.abs(t.headingMrad) > 3142)) return fail('Un giro quedó fuera del tramo o con un ángulo fuera de ±180°.');
    // AFR2-20261113-01: cuadros de pose. Los giros se insertan en su punto exacto; en «Seguir camino» el rumbo encara cada tramo.
    const segmentStart = time;
    const frames: GotoFrame[] = [{ ...start, tMs: time, theta: heading, wheels }];
    const turnPoints: GotoSegment['turnPoints'] = [];
    let locked = false;
    const rotateTo = (target: number, at: MapPoint, turnId?: string) => {
      const delta = target - heading;
      if (Math.abs(delta) < 1e-4) { if (turnId) frames[frames.length - 1].turnId = turnId; return; }
      time += Math.max(10, Math.round(Math.abs(delta) * TURN_RADIUS_MM / maxSpeedMmS * 1000));
      wheels = wheelStep(wheels, 0, 0, heading, delta); heading = target;
      frames.push({ ...at, tMs: time, theta: heading, wheels, turnId });
    };
    const moveTo = (to: MapPoint) => {
      const from = frames[frames.length - 1], dx = to.xMm - from.xMm, dy = to.yMm - from.yMm, step = Math.hypot(dx, dy);
      if (step < .01) return;
      // En «Seguir camino» encara el tramo, salvo después de un nodo de giro: ese rumbo se mantiene hasta el fin del comando.
      if (orientation === 'path' && !locked) {
        const change = wrapAngle(Math.atan2(dy, dx) - heading);
        if (Math.abs(change) > CORNER_TURN_RAD) rotateTo(heading + change, from);
      }
      const next = orientation === 'path' && !locked ? heading + wrapAngle(Math.atan2(dy, dx) - heading) : heading;
      time += step / maxSpeedMmS * 1000;
      wheels = wheelStep(wheels, dx, dy, (heading + next) / 2, next - heading); heading = next;
      frames.push({ ...to, tMs: time, theta: heading, wheels });
    };
    let t = 0;
    for (let i = 1; i < points.length; i++) {
      while (t < turns.length && turns[t].at * length <= lengths[i] + 1e-6) {
        const turn = turns[t++], d = turn.at * length, ratio = (d - lengths[i-1]) / Math.max(1e-6, lengths[i] - lengths[i-1]);
        const a = points[i-1], b = points[i], p = { xMm: a.xMm + (b.xMm - a.xMm) * ratio, yMm: a.yMm + (b.yMm - a.yMm) * ratio };
        moveTo(p);
        turnPoints.push({ ...turn, ...p, distanceMm: d });
        rotateTo(heading + wrapAngle(turn.headingMrad / 1000 - heading), p, turn.id);
        locked = true;
      }
      moveTo(points[i]);
    }
    time = Math.max(segmentStart + 10, Math.ceil(time / 10) * 10);
    frames[frames.length - 1].tMs = time;
    if (time > EDITOR_MAX_DURATION_MS) return fail('La simulación completa admite hasta 10 minutos.');
    segments.push({ id: command.id, start, end, points, lengths, startMs: segmentStart, endMs: time, distanceMm: length, frames, turnPoints });
    start = end; distance += length;
  }
  return { segments, totalDurationMs: time, distanceMm: distance, error: null, orientation };
}
export function sampleGotoPlan(plan: GotoPlan, atMs: number): {
  pose: RobotPose; trail: TrailPoint[]; activeIndex: number | null; completed: number; wheelAngles: RobotWheelValues;
} {
  const cursor = Math.max(0, Math.min(plan.totalDurationMs, Number.isFinite(atMs) ? atMs : 0));
  const pose = (point: MapPoint, sequence: number, theta = 0): RobotPose => ({ xMm: point.xMm, yMm: point.yMm, schema: 'afr2-pose-1', frameId: 'sim-goto-local', sequence, thetaMrad: wrapAngle(theta) * 1000, ageMs: 0 });
  let point: MapPoint = { xMm: 0, yMm: 0 }, theta = 0, completed = 0, activeIndex: number | null = null;
  let wheels: RobotWheelValues = { fl: 0, fr: 0, rl: 0, rr: 0 };
  const trail: TrailPoint[] = [{ ...pose(point, 0), segmentStart: true }];
  for (const [index, segment] of plan.segments.entries()) {
    const last = segment.frames[segment.frames.length - 1];
    if (cursor >= segment.endMs) {
      point = last; theta = last.theta; wheels = last.wheels; completed++;
      for (const p of segment.points.slice(1)) trail.push({ ...pose(p, index + 1, theta), segmentStart: false });
      continue;
    }
    activeIndex = index;
    let k = 1;
    while (k < segment.frames.length - 1 && segment.frames[k].tMs <= cursor) k++;
    const a = segment.frames[k-1], b = segment.frames[k];
    const r = Math.max(0, Math.min(1, (cursor - a.tMs) / Math.max(1e-6, b.tMs - a.tMs)));
    point = { xMm: a.xMm + (b.xMm - a.xMm) * r, yMm: a.yMm + (b.yMm - a.yMm) * r };
    theta = a.theta + (b.theta - a.theta) * r;
    wheels = { fl: a.wheels.fl + (b.wheels.fl - a.wheels.fl) * r, fr: a.wheels.fr + (b.wheels.fr - a.wheels.fr) * r,
      rl: a.wheels.rl + (b.wheels.rl - a.wheels.rl) * r, rr: a.wheels.rr + (b.wheels.rr - a.wheels.rr) * r };
    for (const f of segment.frames.slice(1, k)) trail.push({ ...pose(f, index + 1, f.theta), segmentStart: false });
    if (cursor > segment.startMs) trail.push({ ...pose(point, index + 1, theta), segmentStart: false });
    break;
  }
  // Recorrido de ruedas precalculado por cuadro: determinista al adelantar, repetir o volver al origen.
  return { pose: pose(point, Math.floor(cursor / 10), theta), trail, activeIndex, completed, wheelAngles: wheelAnglesFromTravel(wheels) };
}

/** AFR2-20261112-07: lápiz. Convierte un trazo continuo (muestras del puntero) en un comando `path` que arranca en
 * `start` (el destino anterior), simplificado con Ramer-Douglas-Peucker a `toleranceMm`. Devuelve null si es muy corto. */
export const GOTO_STROKE_MIN_MM = 100;
const STROKE_VERTEX_MIN_MM = 60;
function simplifyPolyline(points: MapPoint[], toleranceMm: number): MapPoint[] {
  if (points.length < 3) return points;
  const keep = new Uint8Array(points.length); keep[0] = keep[points.length - 1] = 1;
  const stack: [number, number][] = [[0, points.length - 1]];
  while (stack.length) {
    const [a, b] = stack.pop()!;
    const pa = points[a], pb = points[b], dx = pb.xMm - pa.xMm, dy = pb.yMm - pa.yMm, len2 = dx*dx + dy*dy;
    let worst = -1, index = -1;
    for (let i = a + 1; i < b; i++) {
      const p = points[i];
      const t = len2 ? Math.max(0, Math.min(1, ((p.xMm-pa.xMm)*dx + (p.yMm-pa.yMm)*dy) / len2)) : 0;
      const d = Math.hypot(p.xMm - pa.xMm - t*dx, p.yMm - pa.yMm - t*dy);
      if (d > worst) { worst = d; index = i; }
    }
    if (worst > toleranceMm) { keep[index] = 1; stack.push([a, index], [index, b]); }
  }
  return points.filter((_, i) => keep[i]);
}
export function gotoPathFromStroke(id: string, start: MapPoint, stroke: readonly MapPoint[], toleranceMm: number): GotoCommand | null {
  const raw = [start, ...stroke.slice(1)];
  let length = 0;
  for (let i = 1; i < raw.length; i++) length += distanceBetween(raw[i-1], raw[i]);
  if (length < GOTO_STROKE_MIN_MM) return null;
  const round = (p: MapPoint): MapPoint => ({ xMm: Math.round(p.xMm / 10) * 10 || 0, yMm: Math.round(p.yMm / 10) * 10 || 0 });
  const simplified = simplifyPolyline(raw, toleranceMm).map((p, i) => i === 0 ? start : round(p));
  // Vértices muy juntos no aportan forma y el auto los descartaría (tramo < 20 mm): se funden con el siguiente.
  const vertices: MapPoint[] = [start];
  for (const p of simplified.slice(1)) if (distanceBetween(vertices[vertices.length - 1], p) >= STROKE_VERTEX_MIN_MM) vertices.push(p);
  if (vertices.length < 2) return null;
  const end = vertices[vertices.length - 1];
  return { id, kind: 'path', xMm: end.xMm, yMm: end.yMm, points: vertices.slice(1, -1) };
}
