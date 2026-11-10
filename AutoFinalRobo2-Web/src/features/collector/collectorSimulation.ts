import { forwardMecanumKinematics, inverseMecanumKinematics } from '../control/sequences/mecanumKinematics';
import { ROBOT_WHEEL_KEYS, type RobotWheelValues } from '../../components/three/robotWheelMotion';
import { armPath, armRoute, clamp, CUBE_SIZE, environmentClear, forwardArm, GRASP_ABOVE_CENTER, JOINTS, localToWorld, pickupSolutions, rad,
  TRANSPORT, worldToLocal, type ArmAngles, type ArmEnvironment, type Point3 } from './armKinematics';
import { robotModel, modelProblems } from './robotModel';
import { groundRoute, type GroundPose } from './collectorNavigation';
import { clearConfiguration, clearOpening, type ArmMotionPlan, type MotionEnvironment } from './armMotion';

export interface Cube { id: number; point: Point3; location: 'floor' | 'gripper' | 'bin'; deliveredAt?: number }
export interface Echo { angle: number; distance: number | null; point: Point3 | null; at: number; objectId?: number; center?: Point3 }
export type Phase = 'idle' | 'prepare' | 'scan' | 'explore' | 'search-turn' | 'align' | 'scout' | 'leave-drop' | 'carry-stow' | 'return' | 'drop-lower' | 'approach' | 'remeasure' | 'pregrasp' | 'grasp' | 'lift' | 'deposit' | 'release' | 'stow' | 'complete' | 'fault';
type NavPurpose = 'pickup' | 'drop' | 'explore' | 'scout' | 'leave';
export const PHASE_LABELS: Record<Phase, string> = {
  scout: 'Acercándose a la caja', 'leave-drop': 'Saliendo del drop',
  explore: 'Avanzando y midiendo', 'search-turn': 'Buscando otro sector', align: 'Alineando de frente', 'carry-stow': 'Asegurando la carga', return: 'Transportando al drop', 'drop-lower': 'Bajando la carga',
  idle: 'En espera', prepare: 'Preparando el robot', scan: 'Explorando', approach: 'Aproximando', remeasure: 'Midiendo de nuevo',
  pregrasp: 'Preparando toma', grasp: 'Agarrando', lift: 'Levantando', deposit: 'Posicionando sobre el drop',
  release: 'Confirmando entrega', stow: 'Recogiendo brazo', complete: 'Recorrido terminado', fault: 'Intervención necesaria',
};
export const DROP_RADIUS = .24;
const ZERO = (): RobotWheelValues => ({ fl: 0, fr: 0, rl: 0, rr: 0 });
export interface CollectorState {
  pose: { x: number; y: number; yaw: number };
  joints: ArmAngles; opening: number; wheels: RobotWheelValues; wheelAngles: RobotWheelValues;
  drop: { center: Point3; radius: number; visible: boolean };
  navigation: { goal: GroundPose; route: Point3[]; purpose: NavPurpose } | null;
  dropTarget: Point3 | null; searchSectors: number; searchHeading: number;
  /** Cyclic patrol: next stop, legs travelled since the last detection, boxes set aside for later. */
  searchStop: number; searchLegs: number; skipIds: number[]; skipRetried: boolean; targetId: number | null;
  cubes: Cube[]; seed: number; requestedCount: number;
  sensorAngle: number; sensorHeight: number; scanning: boolean; scanDirection: number;
  echoes: Echo[]; reading: Echo | null; clock: number; sampleDue: number; scanComplete: boolean;
  phase: Phase; running: boolean; detail: string; elapsed: number;
  target: Point3 | null; jointTarget: ArmAngles | null; stored: number;
  jointQueue: ArmAngles[]; environment: ArmEnvironment; singleTarget: boolean;
  selected: string; references: boolean; workspace: boolean;
  selectedJoint: number;
  motionEpoch: number;
  rigIssue: string | null;
  pointRun: { id: string; modelId: string; revision: string; environment: MotionEnvironment; opening: number; finishOpening: number | null; plan: ArmMotionPlan } | null;
}
/** Centre of a box held by its upper part (TCP sits GRASP_ABOVE_CENTER above it). */
function heldCentre(s: CollectorState): Point3 {
  const tcp = localToWorld(forwardArm(s.joints).tcp, s.pose);
  return [tcp[0], tcp[1], tcp[2] - GRASP_ABOVE_CENTER];
}
function syncAttachments(s: CollectorState): CollectorState {
  if (!s.cubes.some(cube => cube.location === 'gripper')) return s;
  const centre = heldCentre(s);
  return { ...s, cubes: s.cubes.map(cube => cube.location === 'gripper' ? { ...cube, point: centre } : cube) };
}
export function generateCubes(seed: number, count: number): Cube[] {
  let value = seed >>> 0;
  const random = () => { value = (Math.imul(value, 1664525) + 1013904223) >>> 0; return value / 4294967296; };
  const cubes: Cube[] = [];
  for (let attempt = 0; cubes.length < count && attempt < 2000; attempt++) {
    const point: Point3 = cubes.length === 0
      ? [.225 + random() * .01, (random() - .5) * .04, CUBE_SIZE / 2]
      : [.45 + random() * 1.05, (random() - .5) * 1.4, CUBE_SIZE / 2];
    if (cubes.every(cube => Math.hypot(cube.point[0] - point[0], cube.point[1] - point[1]) > .15)) {
      cubes.push({ id: cubes.length + 1, point, location: 'floor' });
    }
  }
  return cubes;
}
export function initialCollector(seed = 42, count = 6): CollectorState {
  return {
    drop: { center: [-.65 - (seed % 5) * .04, -.65 - (seed % 7) * .03, 0], radius: DROP_RADIUS, visible: false },
    navigation: null, dropTarget: null, searchSectors: 0, searchHeading: 0, searchStop: 0, searchLegs: 0, skipIds: [], skipRetried: false, targetId: null,
    pose: { x: 0, y: 0, yaw: 0 }, joints: [...robotModel().initial], opening: robotModel().gripper.open,
    wheels: ZERO(), wheelAngles: ZERO(), cubes: generateCubes(seed, count), seed, requestedCount: count,
    sensorAngle: 0, sensorHeight: .045, scanning: false, scanDirection: 1,
    echoes: [], reading: null, clock: 0, sampleDue: 0, scanComplete: false,
    phase: 'idle', running: false, detail: 'Seleccioná el robot o el brazo para empezar.', elapsed: 0,
    target: null, jointTarget: null, jointQueue: [], environment: 'mounted', singleTarget: false,
    stored: 0, selected: 'robot', selectedJoint: 0, references: false, workspace: true, pointRun: null, motionEpoch: 0,
    rigIssue: robotModel().visual.kind === 'gltf' ? 'Cargando y vinculando el rig del brazo.' : null,
  };
}
export function halt(s: CollectorState, detail = 'Simulación detenida. Posiciones conservadas.'): CollectorState {
  return { ...s, navigation: null, wheels: ZERO(), scanning: false, running: false, jointTarget: null, jointQueue: [], pointRun: null, motionEpoch: s.motionEpoch + 1, phase: 'idle', elapsed: 0, detail };
}
function fail(s: CollectorState, detail: string): CollectorState {
  return { ...halt(s, detail), phase: 'fault' };
}
function transition(s: CollectorState, phase: Phase, detail: string): CollectorState {
  return { ...s, phase, elapsed: 0, detail, wheels: ZERO() };
}
function startScan(s: CollectorState, reset = true): CollectorState {
  return { ...transition(s, 'scan', 'Barrido con la base detenida.'), scanning: true, sensorAngle: -90,
    searchSectors: reset ? 0 : s.searchSectors, navigation: null, echoes: [], reading: null, scanDirection: 1, sampleDue: s.clock + .3, scanComplete: false, target: null };
}
export function startCollection(s: CollectorState): CollectorState {
  if (s.rigIssue || s.jointTarget || s.scanning || Object.values(s.wheels).some(v => v !== 0) || modelProblems(robotModel()).length) return s;
  if (s.cubes.some(c => c.location === 'gripper')) return fail(s, 'Hay un cubo sujeto. Depositalo antes de comenzar.');
  if (s.cubes.every(c => c.location !== 'floor')) return { ...s, running: false, phase: 'complete', detail: 'Todas las cargas se entregaron en el drop.' };
  return scheduleArm({ ...transition(s, 'stow', 'Preparando la postura de transporte.'), running: true,
    singleTarget: false, searchStop: 0, searchLegs: 0, skipIds: [], skipRetried: false, targetId: null, dropTarget: null, drop: { ...s.drop, visible: true }, environment: 'mounted' }, TRANSPORT);
}

/** Ideal horizontal ray vs virtual 50 mm cargo envelopes. Invalid echo is null.
 * The virtual detector also identifies the object and its centre; this is NOT
 * classification/pose estimation from a physical HC-SR04. Planning consumes Echo.
 * Height is an explicit proposed low mount, not the real CAD sensor height. */
export function measure(s: CollectorState): Echo {
  const origin = localToWorld([.103, 0, s.sensorHeight], s.pose);
  const theta = s.pose.yaw + rad(s.sensorAngle), direction = [Math.cos(theta), Math.sin(theta)];
  let nearest = Infinity, detected: Cube | undefined;
  for (const cube of s.cubes) {
    if (cube.location !== 'floor' || Math.abs(cube.point[2] - origin[2]) > CUBE_SIZE / 2) continue;
    let near = 0, far = 4;
    for (let axis = 0; axis < 2; axis++) {
      const low = cube.point[axis] - CUBE_SIZE / 2, high = cube.point[axis] + CUBE_SIZE / 2;
      if (Math.abs(direction[axis]) < 1e-8) {
        if (origin[axis] < low || origin[axis] > high) { far = -1; break; }
      } else {
        const a = (low - origin[axis]) / direction[axis], b = (high - origin[axis]) / direction[axis];
        near = Math.max(near, Math.min(a, b)); far = Math.min(far, Math.max(a, b));
      }
    }
    if (near <= far && near >= .02 && near < nearest) { nearest = near; detected = cube; }
  }
  const distance = Number.isFinite(nearest) ? nearest : null;
  return { angle: s.sensorAngle, distance, at: s.clock, objectId: detected?.id, center: detected ? [...detected.point] : undefined,
    point: distance === null ? null : [origin[0] + direction[0] * distance, origin[1] + direction[1] * distance, origin[2]] };
}
/** `margin` inflates the footprint for route planning, so small wheel drift cannot graze a box. */
export function baseClear(s: CollectorState, pose: CollectorState['pose'], margin = 0) {
  if (Math.abs(pose.x) > 2 || Math.abs(pose.y) > 2) return false;
  return s.cubes.filter(c => c.location === 'floor').every(cube => {
    const p = worldToLocal(cube.point, pose);
    return Math.abs(p[0]) > .158 + margin || Math.abs(p[1]) > .135 + margin;
  });
}
const ROUTE_MARGIN = .01;
/** Planning check with margin; the first few cm next to the start only need the strict footprint. */
function routeClear(s: CollectorState, start: CollectorState['pose']) {
  return (pose: CollectorState['pose']) => baseClear(s, pose, ROUTE_MARGIN) || (Math.hypot(pose.x - start.x, pose.y - start.y) < .03 && baseClear(s, pose));
}
export function moveBase(s: CollectorState, x: number, y: number, yaw = s.pose.yaw) {
  if (s.running || s.jointTarget || s.cubes.some(c => c.location === 'gripper')) return s;
  if (s.joints.some((v, i) => Math.abs(v - TRANSPORT[i]) > 3)) return { ...s, detail: 'Recoge el brazo a Transporte antes de mover la base.' };
  const pose = { yaw, x: clamp(x, -2, 2), y: clamp(y, -2, 2) };
  // Check the drag segment too, so a large pointer jump cannot cross a cube.
  const steps = Math.max(1, Math.ceil(Math.hypot(pose.x - s.pose.x, pose.y - s.pose.y) / .01), Math.ceil(Math.abs(yaw - s.pose.yaw) / .05));
  for (let step = 1; step <= steps; step++) {
    const t = step / steps;
    if (!baseClear(s, { yaw: s.pose.yaw + (yaw - s.pose.yaw) * t, x: s.pose.x + (pose.x - s.pose.x) * t, y: s.pose.y + (pose.y - s.pose.y) * t })) {
      return { ...s, detail: 'Movimiento bloqueado por un cubo o por el borde del escenario.' };
    }
  }
  return syncAttachments({ ...halt(s), pose, echoes: [], reading: null });
}
export function setManualWheels(s: CollectorState, wheels: RobotWheelValues): CollectorState {
  if (s.running || s.scanning || s.jointTarget || s.cubes.some(c => c.location === 'gripper')) return s;
  if (s.joints.some((v, i) => Math.abs(v - TRANSPORT[i]) > 3)) return { ...s, detail: 'Recoge el brazo a Transporte antes de mover la base.' };
  return { ...s, wheels, detail: 'Movimiento local de ruedas. Soltá el control para detener.' };
}
export function driveVector(vx: number, vy: number, wz: number) {
  return inverseMecanumKinematics(vx * 1000, vy * 1000, wz * 1000, 220);
}
export function setJoint(s: CollectorState, index: number, value: number): CollectorState {
  if (s.rigIssue || modelProblems(robotModel()).length) return s;
  if (s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  if (!JOINTS[index] || !Number.isFinite(value)) return s;
  const joints = [...s.joints] as ArmAngles;
  joints[index] = clamp(value, JOINTS[index].min, JOINTS[index].max);
  if (!armPath(s.joints, joints, s.environment)) return { ...s, detail: 'Límite del entorno: ajusta otro eje o recoge el brazo.' };
  return syncAttachments({ ...s, joints, selectedJoint: index, detail: 'Postura del brazo actualizada en el simulador.' });
}
function scheduleArm(s: CollectorState, joints: ArmAngles): CollectorState {
  const route = armRoute(s.joints, joints, s.environment);
  if (!route) return fail(s, 'Trayectoria del brazo bloqueada por el entorno. Ajusta la posición del robot.');
  return { ...s, jointTarget: route[0], jointQueue: route.slice(1) };
}
export function moveArmTo(s: CollectorState, joints: ArmAngles): CollectorState {
  if (s.rigIssue || modelProblems(robotModel()).length || s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  return scheduleArm({ ...s, detail: 'Moviendo el brazo a la postura elegida.' }, joints);
}
/** Stage 0 = 100 mm above, 1 = grasp/release height, 2 = lift. `centre` is the box centre in car coordinates. */
function beginStage(s: CollectorState, centre: Point3, stage: 0 | 1 | 2, phase: Phase, detail: string): CollectorState {
  const solutions = pickupSolutions(centre, s.pose.yaw);
  if (!solutions) return fail(s, 'Toma sin postura libre de choques para el modelo activo.');
  return scheduleArm(transition(s, phase, detail), solutions[stage]);
}

export function cubeReach(s: CollectorState, cube: Cube) {
  if (cube.location !== 'floor') return { reachable: false, reason: cube.location === 'bin' ? 'Guardado' : 'En la pinza' };
  const point = worldToLocal(cube.point, s.pose);
  const reachable = !!pickupSolutions(point, s.pose.yaw);
  return { reachable, reason: reachable ? 'Dentro del alcance de toma' : 'Fuera del alcance: acerca el robot' };
}
/** Car-origin to box-centre distance for a frontal grasp: middle of the feasible window. */
function dockingRange(yaw: number) {
  const feasible: number[] = [];
  for (let distance = .20; distance >= .14; distance -= .0025) if (pickupSolutions([distance, 0, CUBE_SIZE / 2], yaw)) feasible.push(distance);
  return feasible.length ? feasible[Math.floor(feasible.length / 2)] : undefined;
}
const angleGap = (a: number, b: number) => Math.abs(Math.atan2(Math.sin(a - b), Math.cos(a - b)));
/** In-place turn towards `heading` that keeps the base footprint clear: shortest
 * side first, then the long way; if both sweep into a box, back away and retry. */
function turnTowards(s: CollectorState, heading: number): CollectorState {
  const error = Math.atan2(Math.sin(heading - s.pose.yaw), Math.cos(heading - s.pose.yaw));
  for (const direction of [Math.sign(error), -Math.sign(error)]) {
    const sweep = direction === Math.sign(error) ? Math.abs(error) : 2 * Math.PI - Math.abs(error);
    let clear = true;
    for (let a = .02; a < sweep + .02 && clear; a += .02) clear = baseClear(s, { ...s.pose, yaw: s.pose.yaw + direction * Math.min(a, sweep) });
    if (clear) return { ...s, wheels: driveVector(0, 0, direction * clamp(sweep * 2, .05, .5)) };
  }
  // Mecanum escape: back, sideways, then forward, whichever keeps the footprint clear.
  for (const [vx, vy] of [[-1, 0], [0, 1], [0, -1], [1, 0]]) {
    const next = localToWorld([vx * .02, vy * .02, 0], s.pose);
    if (baseClear(s, { ...s.pose, x: next[0], y: next[1] })) return { ...s, wheels: driveVector(vx * .05, vy * .05, 0), detail: 'Despejando el auto para tener espacio de giro.' };
  }
  return fail(s, 'No hay espacio para girar ni desplazarse: reubicá una carga.');
}
const NAV_TEXT: Record<NavPurpose, [align: string, phase: Phase, travel: string]> = {
  pickup: ['Caja elegida: encuadrando el frente del auto con una cara de la caja.', 'approach', 'Acercando de frente y midiendo la distancia a la caja.'],
  scout: ['Caja detectada lejos: orientando el auto hacia ella.', 'scout', 'Acercándose a la caja para medirla de nuevo desde más cerca.'],
  drop: ['Carga asegurada: orientando el auto hacia el drop.', 'return', 'Transportando la carga hacia el círculo de drop.'],
  leave: ['Entrega hecha: orientando el auto para salir del drop.', 'leave-drop', 'Saliendo del drop para volver a buscar.'],
  explore: ['Orientando el auto hacia el siguiente punto de exploración.', 'explore', 'Avanzando con medición frontal; al detenerse realiza otro barrido.'],
};
function navigate(s: CollectorState, goal: GroundPose, purpose: NavPurpose): CollectorState {
  return { ...transition(s, 'align', NAV_TEXT[purpose][0]), navigation: { goal, purpose, route: [] }, scanning: false, sensorAngle: 0, reading: null };
}
/** Set the current box aside (retried after the next delivery) and keep searching. */
function giveUp(s: CollectorState, why: string): CollectorState {
  if (s.singleTarget) return fail(s, why);
  const skipIds = s.targetId !== null && !s.skipIds.includes(s.targetId) ? [...s.skipIds, s.targetId] : s.skipIds;
  const next = startExploration({ ...s, navigation: null, target: null, targetId: null, skipIds });
  return next.phase === 'fault' ? next : { ...next, detail: `${why} Se deja para después y sigue buscando.` };
}
const skipped = (s: CollectorState, echo: Echo) => echo.objectId !== undefined && s.skipIds.includes(echo.objectId);
/** Detected box: far away → get closer and scan again; close → square up and dock. */
const SCOUT_RANGE = .55, SCOUT_STANDOFF = .40;
function chooseTarget(s: CollectorState, echo: Echo): CollectorState {
  if (!echo.center) return s;
  const target: Point3 = [...echo.center], next = { ...s, target, targetId: echo.objectId ?? null, searchLegs: 0, reading: echo };
  const dx = target[0] - s.pose.x, dy = target[1] - s.pose.y, distance = Math.hypot(dx, dy);
  if (distance > SCOUT_RANGE) {
    const yaw = Math.atan2(dy, dx), goal = { x: target[0] - Math.cos(yaw) * SCOUT_STANDOFF, y: target[1] - Math.sin(yaw) * SCOUT_STANDOFF, yaw };
    if (baseClear(s, goal, ROUTE_MARGIN)) return navigate(next, goal, 'scout');
  }
  return beginTarget(next);
}
function beginTarget(s: CollectorState): CollectorState {
  if (!s.target) return giveUp(s, 'No hay un objetivo seleccionado.');
  const target = s.target;
  // Face one side of the box squarely (boxes are axis-aligned): the side closest to the current bearing first.
  const bearing = Math.atan2(target[1] - s.pose.y, target[0] - s.pose.x);
  const faces = [0, 1, 2, 3].map(k => k * Math.PI / 2 - (k > 2 ? 2 * Math.PI : 0)).sort((a, b) => angleGap(a, bearing) - angleGap(b, bearing));
  let reachable = false;
  for (const yaw of faces) {
    const distance = dockingRange(yaw);
    if (distance === undefined) continue;
    reachable = true;
    const goal = { x: target[0] - Math.cos(yaw) * distance, y: target[1] - Math.sin(yaw) * distance, yaw };
    if (baseClear(s, goal, ROUTE_MARGIN)) return navigate(s, goal, 'pickup');
  }
  return reachable ? giveUp(s, 'No hay una cara libre de la caja para tomarla de frente.') : fail(s, 'El modelo activo no tiene una toma frontal válida para esta carga.');
}
/** After a delivery, back out to the map side of the drop before scanning again. */
function leaveDrop(s: CollectorState): CollectorState {
  const [x, y] = s.drop.center, away = Math.atan2(-y, -x), reach = s.drop.radius + .35;
  const goal = { x: x + Math.cos(away) * reach, y: y + Math.sin(away) * reach, yaw: away };
  return baseClear(s, goal, ROUTE_MARGIN) ? navigate(s, goal, 'leave') : startScan(s);
}
function beginDrop(s: CollectorState): CollectorState {
  // Arrive square to the world axes (as at pickup) so the wrist keeps the box clear of the chassis.
  const [x, y] = s.drop.center, bearing = Math.atan2(y - s.pose.y, x - s.pose.x);
  const yaw = Math.round(bearing / (Math.PI / 2)) * Math.PI / 2, distance = dockingRange(yaw);
  if (distance === undefined) return fail(s, 'No hay postura frontal de entrega para este modelo.');
  const point: Point3 = [x - Math.cos(yaw) * s.drop.radius * .65, y - Math.sin(yaw) * s.drop.radius * .65, CUBE_SIZE / 2];
  return navigate({ ...s, dropTarget: point }, { x: point[0] - Math.cos(yaw) * distance, y: point[1] - Math.sin(yaw) * distance, yaw }, 'drop');
}
/** Patrol positions are independent of hidden cargo coordinates. At each stop
 * scan the surroundings; during travel keep taking forward distance samples.
 */
const SEARCH_STOPS: Point3[] = [[.4, 0, 0], [.8, 0, 0], [1.2, .6, 0], [.6, .6, 0], [0, .6, 0], [-.6, .6, 0], [-1.2, 1.2, 0], [0, 1.2, 0], [1.2, 1.2, 0], [1.2, -.6, 0], [.6, -.6, 0], [0, -.6, 0], [-.6, -.6, 0], [-1.2, -1.2, 0], [0, -1.2, 0], [1.2, -1.2, 0]];
function startExploration(s: CollectorState): CollectorState {
  if (s.cubes.every(c => c.location !== 'floor')) return { ...halt(s, 'Todas las cargas se entregaron en el drop.'), phase: 'complete' };
  const front = measure({ ...s, sensorAngle: 0 });
  if (front.center && !skipped(s, front)) return chooseTarget({ ...s, scanning: false }, front);
  // Two full patrol laps without a usable detection: retry set-aside boxes once, then ask for help.
  if (s.searchLegs >= 2 * SEARCH_STOPS.length) {
    if (s.skipIds.length && !s.skipRetried) return startExploration({ ...s, skipIds: [], skipRetried: true, searchLegs: 0 });
    return fail(s, 'Dos vueltas de patrulla sin cajas alcanzables. Reubicá el auto o las cajas.');
  }
  // The patrol is cyclic: keep walking the stops in order, wrapping around.
  for (let n = 0; n < SEARCH_STOPS.length; n++) {
    const i = (s.searchStop + n) % SEARCH_STOPS.length, [x, y] = SEARCH_STOPS[i];
    if (Math.hypot(x - s.pose.x, y - s.pose.y) < .12) continue;
    const goal = { x, y, yaw: Math.atan2(y - s.pose.y, x - s.pose.x) };
    if (baseClear(s, goal, ROUTE_MARGIN) && groundRoute(s.pose, goal, routeClear(s, s.pose)))
      return navigate({ ...s, searchStop: (i + 1) % SEARCH_STOPS.length, searchLegs: s.searchLegs + 1, target: null, targetId: null }, goal, 'explore');
  }
  return fail(s, 'Ningún punto de patrulla tiene ruta libre. Reubicá el auto o las cajas.');
}
function followNavigation(s: CollectorState): CollectorState {
  const nav = s.navigation;
  if (!nav) return fail(s, 'Falta la ruta de navegación.');
  if (s.elapsed > 90) {
    if (nav.purpose === 'drop') return fail(s, 'La navegación no pudo completar el trayecto al drop.');
    return nav.purpose === 'explore' || nav.purpose === 'leave' ? startScan({ ...s, navigation: null }) : giveUp(s, 'No se pudo llegar a la caja.');
  }
  if (nav.purpose !== 'drop' && s.clock >= s.sampleDue) {
    const reading = measure({ ...s, sensorAngle: 0 });
    s = { ...s, sensorAngle: 0, reading, sampleDue: s.clock + .15 };
    const usable = !!reading.center && !skipped(s, reading);
    // While patrolling (or leaving the drop) any frontal echo is a candidate.
    if ((nav.purpose === 'explore' || nav.purpose === 'leave') && usable) return chooseTarget({ ...s, navigation: null }, reading);
    // While getting closer, a different box right in front takes priority.
    if (nav.purpose === 'scout' && usable && reading.objectId !== s.targetId && reading.distance !== null && reading.distance < .08) return chooseTarget({ ...s, navigation: null }, reading);
    if (nav.purpose === 'pickup' && reading.center && s.target) {
      const p = worldToLocal(reading.center, s.pose);
      const expected = Math.hypot(s.target[0] - nav.goal.x, s.target[1] - nav.goal.y);
      const sameTarget = Math.hypot(reading.center[0] - s.target[0], reading.center[1] - s.target[1]) < .04;
      // Front-face echo is measured from the actual virtual sensor origin.
      const frontStop = expected - .103 - CUBE_SIZE / 2;
      if (sameTarget && reading.distance !== null && reading.distance <= frontStop + .004 && Math.abs(p[1]) < .008 && pickupSolutions(p, s.pose.yaw)) {
        return { ...transition(s, 'remeasure', 'Distancia frontal de toma alcanzada: deteniendo y confirmando otro eco.'), navigation: null, sampleDue: s.clock + .3 };
      }
      // A nearer box blocks the way: take that one first instead of stopping.
      if (!sameTarget && reading.distance !== null && reading.distance < .08 && !skipped(s, reading)) return chooseTarget({ ...s, navigation: null }, reading);
    }
  }
  const point = nav.route[0];
  if (!point) {
    if (nav.purpose === 'explore' || nav.purpose === 'leave') return startScan({ ...s, navigation: null });
    if (nav.purpose === 'scout') return { ...startScan({ ...s, navigation: null }), detail: 'Más cerca de la caja: barriendo de nuevo para medirla y elegir.' };
    if (nav.purpose === 'pickup') return { ...transition(s, 'remeasure', 'Frente alineado y base detenida: confirmando de nuevo la caja y el alcance.'), navigation: null, reading: null, sampleDue: s.clock + .3 };
    if (!s.dropTarget) return fail(s, 'No hay punto de entrega.');
    const p = worldToLocal(s.dropTarget, s.pose);
    return beginStage({ ...s, navigation: null }, p, 0, 'deposit', 'Auto detenido frente al drop: colocando la carga sobre la zona.');
  }
  const dx = point[0] - s.pose.x, dy = point[1] - s.pose.y, distance = Math.hypot(dx, dy);
  if (distance < .003) return { ...s, pose: { ...s.pose, x: point[0], y: point[1] }, wheels: ZERO(), navigation: { ...nav, route: nav.route.slice(1) } };
  const speed = Math.min(.12, distance * 2), c = Math.cos(s.pose.yaw), sn = Math.sin(s.pose.yaw);
  return { ...s, wheels: driveVector((c * dx + sn * dy) / distance * speed, (-sn * dx + c * dy) / distance * speed, 0) };
}
export function collectCube(s: CollectorState, id: number): CollectorState {
  if (s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  const cube = s.cubes.find(c => c.id === id && c.location === 'floor');
  if (!cube) return fail(s, 'Seleccioná un cubo del suelo.');
  if (s.cubes.some(c => c.location === 'gripper')) return fail(s, 'Primero deposita el cubo sujeto.');
  const next: CollectorState = { ...s, target: [...cube.point], selected: `cube:${id}`, singleTarget: true,
    running: true, drop: { ...s.drop, visible: true }, environment: 'mounted', wheels: ZERO(), elapsed: 0 };
  return scheduleArm(transition(next, 'prepare', 'Recogiendo el brazo antes de acercar el auto.'), TRANSPORT);
}
export function positionCube(s: CollectorState, id: number, point: Point3): CollectorState {
  if (s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  const cube = s.cubes.find(c => c.id === id);
  if (cube && cube.location !== 'floor') return s;
  if (!point.every(Number.isFinite) || Math.abs(point[0]) > 2 || Math.abs(point[1]) > 2) return s;
  const p: Point3 = [point[0], point[1], .025];
  if (Math.hypot(p[0] - s.drop.center[0], p[1] - s.drop.center[1]) < s.drop.radius + CUBE_SIZE) return { ...s, detail: 'El área de drop queda reservada para la entrega.' };
  if (s.cubes.some(c => c.id !== id && c.location === 'floor' && Math.abs(c.point[0] - p[0]) < .06 && Math.abs(c.point[1] - p[1]) < .06)) {
    return { ...s, detail: 'Esa posición se superpone con otro cubo.' };
  }
  if (!cube && s.cubes.length >= 24) return { ...s, detail: 'La escena admite hasta 24 cubos.' };
  const cubes: Cube[] = cube ? s.cubes.map(c => c.id === id ? { ...c, point: p } : c) : [...s.cubes, { id, point: p, location: 'floor' }];
  if (!baseClear({ ...s, cubes }, s.pose)) return { ...s, detail: 'La posición del cubo invade el robot.' };
  return { ...s, cubes, selected: `cube:${id}`, echoes: [], reading: null, detail: 'Posición del cubo actualizada.' };
}
export function manualGrasp(s: CollectorState): CollectorState {
  if (s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  if (s.cubes.some(c => c.location === 'gripper')) return s;
  const centre = heldCentre(s);
  // Scene truth is used here only as contact physics, never as mission input.
  const cube = s.cubes.find(c => c.location === 'floor' && Math.hypot(...c.point.map((v, i) => v - centre[i])) < .025);
  if (!cube) return { ...s, opening: 50, detail: 'Agarre fallido: no hay un cubo centrado entre las mordazas.' };
  return { ...s, opening: 50, cubes: s.cubes.map(c => c.id === cube.id ? { ...c, location: 'gripper', point: centre } : c), detail: 'Cubo sujeto en la simulación.' };
}
export function manualRelease(s: CollectorState): CollectorState {
  if (s.running || s.pointRun || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  const held = s.cubes.find(c => c.location === 'gripper');
  if (!held) return { ...s, opening: robotModel().gripper.open };
  const tcp = heldCentre(s);
  const inside = s.drop.visible && Math.hypot(tcp[0] - s.drop.center[0], tcp[1] - s.drop.center[1]) <= s.drop.radius - CUBE_SIZE / Math.SQRT2;
  if (!inside || tcp[2] < .015 || tcp[2] > .065) return { ...s, detail: 'La carga debe estar dentro del círculo de drop y cerca del suelo para contabilizarla.' };
  return { ...s, opening: robotModel().gripper.open, stored: s.stored + 1,
    cubes: s.cubes.map(c => c.id === held.id ? { ...c, location: 'bin', deliveredAt: s.clock, point: [tcp[0], tcp[1], CUBE_SIZE / 2] as Point3 } : c),
    detail: `Entrega confirmada: ${s.stored + 1} cajas.` };
}
/** Existing manual action now delivers to the fixed ground drop, not a basket. */
export function nextBinPose(s: CollectorState): CollectorState {
  if (!s.cubes.some(c => c.location === 'gripper')) return { ...s, detail: 'Primero sujetá una carga.' };
  return scheduleArm({ ...transition(s, 'carry-stow', 'Asegurando el brazo antes de transportar la caja.'),
    running: true, singleTarget: s.running ? s.singleTarget : true, drop: { ...s.drop, visible: true } }, TRANSPORT);
}
export function tickCollector(previous: CollectorState, dt: number): CollectorState {
  let s = { ...previous, clock: previous.clock + dt, elapsed: previous.elapsed + dt };
  if (s.scanning && s.clock >= s.sampleDue) {
    const reading = measure(s);
    s = { ...s, reading, echoes: [...s.echoes.filter(e => e.angle !== reading.angle), reading].slice(-37), sampleDue: s.clock + .10 };
    if (s.sensorAngle >= 90) s = { ...s, scanning: false, scanComplete: true };
    else s.sensorAngle += 5;
  }
  if (s.jointTarget) {
    if (s.rigIssue || modelProblems(robotModel()).length) return fail(s, 'Movimiento deshabilitado: descriptor o rig incompleto.');
    if (s.pointRun && (s.pointRun.modelId !== robotModel().id || s.pointRun.revision !== robotModel().revision || s.pointRun.opening !== s.opening)) return fail(s, 'Cambio el modelo o la apertura; recorrido cancelado.');
    const target = s.jointTarget;
    const distance = Math.max(...target.map((v, i) => Math.abs(v - s.joints[i])));
    const fraction = distance === 0 ? 1 : Math.min(1, robotModel().speedDegS * dt / distance);
    const joints = s.joints.map((v, i) => v + (target[i] - v) * fraction) as ArmAngles;
    if (s.pointRun ? !clearConfiguration(joints, s.opening, s.pointRun.environment) : !environmentClear(joints, s.environment)) return fail(s, 'Trayectoria del brazo bloqueada.');
    const done = fraction === 1;
    s = { ...s, joints, jointTarget: done ? s.jointQueue[0] ?? null : target, jointQueue: done ? s.jointQueue.slice(1) : s.jointQueue };
    if (s.pointRun && !s.jointTarget) {
      const opening = s.pointRun.finishOpening ?? s.opening;
      if (!clearOpening(s.joints, s.opening, opening, s.pointRun.environment)) return fail(s, 'La apertura final invade una restricción del modelo.');
      s = { ...s, opening, pointRun: null, detail: 'Destino alcanzado en el simulador. Postura y orientación recuperadas.' };
    }
  }
  if (s.running) {
    switch (s.phase) {
      case 'prepare': if (!s.jointTarget) s = beginTarget(s); break;
      case 'scan': {
        if (!s.scanComplete) break;
        // Choose the nearest usable echo of the sweep (boxes set aside are ignored until the next delivery).
        const candidates = s.echoes.filter(e => e.point !== null && e.distance !== null && !skipped(s, e)).sort((a, b) => a.distance! - b.distance!);
        const echo = candidates[0];
        if (!echo?.point) {
          // Two 180° sweeps (front, then behind) cover the stop; then move on and scan again.
          if (s.searchSectors >= 1) { s = startExploration(s); break; }
          s = { ...transition(s, 'search-turn', 'Sin detección delante: girando para barrer detrás.'), searchSectors: s.searchSectors + 1, searchHeading: s.pose.yaw + Math.PI };
          break;
        }
        const theta = s.pose.yaw + rad(echo.angle);
        const center: Point3 = echo.center ?? [echo.point[0] + .025 * Math.cos(theta), echo.point[1] + .025 * Math.sin(theta), .025];
        s = chooseTarget({ ...s, sensorAngle: 0 }, { ...echo, center });
        break;
      }
      case 'search-turn': {
        const error = Math.atan2(Math.sin(s.searchHeading - s.pose.yaw), Math.cos(s.searchHeading - s.pose.yaw));
        if (Math.abs(error) < .02) s = startScan(s, false);
        else s = turnTowards(s, s.searchHeading);
        break;
      }
      case 'align': {
        const nav = s.navigation;
        if (!nav) return fail(s, 'Falta el destino de navegación.');
        const error = Math.atan2(Math.sin(nav.goal.yaw - s.pose.yaw), Math.cos(nav.goal.yaw - s.pose.yaw));
        if (Math.abs(error) > .015) s = turnTowards(s, nav.goal.yaw);
        else {
          const aligned = { ...s.pose, yaw: nav.goal.yaw }, goal = nav.goal;
          const route = baseClear(s, aligned) ? groundRoute(aligned, goal, routeClear(s, aligned)) : null;
          if (!route) {
            // No free way: drop route is mandatory; otherwise try another stop or another box.
            if (nav.purpose === 'drop') return fail(s, 'No hay ruta libre hasta el drop. Reubicá una carga.');
            if (nav.purpose === 'explore') { s = startExploration({ ...s, navigation: null }); break; }
            if (nav.purpose === 'leave') { s = startScan({ ...s, navigation: null }); break; }
            s = giveUp(s, 'No hay ruta libre hasta la caja.');
            break;
          }
          const [, phase, travel] = NAV_TEXT[nav.purpose];
          s = { ...transition(s, phase, travel), pose: aligned, navigation: { ...nav, goal, route } };
        }
        break;
      }
      case 'approach':
      case 'explore':
      case 'scout':
      case 'leave-drop':
      case 'return': s = followNavigation(s); break;
      case 'carry-stow': if (!s.jointTarget) s = beginDrop(s); break;
      case 'remeasure': {
        if (s.clock < s.sampleDue) break;
        const reading = measure({ ...s, sensorAngle: 0 });
        if (!reading.point || reading.distance === null) { s = giveUp(s, 'Caja perdida al medir de nuevo.'); break; }
        if (!s.target || !reading.center || Math.hypot(reading.center[0] - s.target[0], reading.center[1] - s.target[1]) > .04) { s = giveUp(s, 'La medición frontal cambió.'); break; }
        // Fine lateral centring with Mecanum strafe so the box sits as square to the front as possible.
        const lateral = worldToLocal(reading.center, s.pose)[1];
        if (Math.abs(lateral) > .004) {
          if (s.elapsed > 6) { s = giveUp(s, 'No se pudo centrar el frente del auto con la caja.'); break; }
          s = { ...s, reading, wheels: driveVector(0, clamp(lateral * 3, -.03, .03), 0), sampleDue: s.clock + .1, detail: `Centrando el frente: caja desplazada ${Math.round(lateral * 1000)} mm.` };
          break;
        }
        // Grasp point comes from the forward echo only: sensor origin + distance to the
        // front face + half box, straight ahead of the car (the base is already aligned).
        const p: Point3 = [.103 + reading.distance + CUBE_SIZE / 2, 0, CUBE_SIZE / 2];
        const mm = Math.round(reading.distance * 1000);
        if (!pickupSolutions(p, s.pose.yaw)) {
          // Outside the grasp window: creep forward/back while measuring until the box is at the docking distance.
          const dock = dockingRange(s.pose.yaw);
          if (dock === undefined) return fail(s, 'El modelo activo no tiene una toma frontal válida para esta carga.');
          if (s.elapsed > 6) { s = giveUp(s, `Caja a ${mm} mm: no se pudo ajustar la distancia de toma.`); break; }
          s = { ...s, reading, wheels: driveVector(clamp((p[0] - dock) * 3, -.03, .03), 0, 0), sampleDue: s.clock + .1, detail: `Caja a ${mm} mm al frente: ajustando la distancia de toma.` };
          break;
        }
        const target = localToWorld(p, s.pose);
        s = beginStage({ ...s, target, reading, sensorAngle: 0 }, p, 0, 'pregrasp', `Caja medida a ${mm} mm al frente: abriendo la pinza y acercando por arriba.`);
        s.opening = robotModel().gripper.open;
        break;
      }
      case 'pregrasp':
        if (!s.jointTarget && s.target) s = beginStage(s, worldToLocal(s.target, s.pose), 1, 'grasp', 'Bajando y tomando la caja por su parte superior.');
        break;
      case 'grasp':
        if (!s.jointTarget && s.target) {
          const target = s.target;
          s = manualGrasp({ ...s, running: false });
          if (!s.cubes.some(c => c.location === 'gripper')) return fail(s, 'Agarre fallido: la estimación por eco no centro el cubo.');
          s = beginStage({ ...s, running: true }, worldToLocal(target, s.pose), 2, 'lift', 'Levantando la caja antes del transporte.');
        }
        break;
      case 'lift': if (!s.jointTarget) s = nextBinPose(s); break;
      case 'deposit':
        if (!s.jointTarget && s.dropTarget) s = beginStage(s, worldToLocal(s.dropTarget, s.pose), 1, 'drop-lower', 'Bajando la caja dentro del círculo de drop.');
        break;
      case 'drop-lower': if (!s.jointTarget) s = transition(s, 'release', 'Soltando la carga dentro del drop.'); break;
      case 'release':
        if (s.elapsed >= .4) {
          const released = manualRelease({ ...s, running: false });
          if (released.stored === s.stored) return fail(s, 'Entrega no confirmada: la carga no está dentro del drop o está demasiado alta.');
          s = scheduleArm({ ...transition(released, 'stow', 'Recogiendo el brazo antes del próximo movimiento.'), running: true }, TRANSPORT);
        }
        break;
      case 'stow':
        if (!s.jointTarget) {
          if (s.singleTarget) return { ...s, running: false, phase: 'complete', detail: 'Caja entregada en el drop. Seleccioná la siguiente caja.' };
          if (s.cubes.every(c => c.location !== 'floor')) return { ...s, running: false, phase: 'complete', detail: 'Todas las cargas se entregaron en el drop.' };
          // Cycle: leave the drop, scan, choose, approach... Set-aside boxes get another chance.
          s = s.dropTarget ? leaveDrop({ ...s, dropTarget: null, skipIds: [], skipRetried: false, searchLegs: 0, target: null, targetId: null }) : startExploration(s);
        }
        break;
    }
  }
  const moving = ROBOT_WHEEL_KEYS.some(key => s.wheels[key] !== 0);
  if (moving) {
    const v = forwardMecanumKinematics(s.wheels, 220), yaw = s.pose.yaw;
    const pose = { x: s.pose.x + (Math.cos(yaw) * v.vxMmS - Math.sin(yaw) * v.vyMmS) / 1000 * dt,
      y: s.pose.y + (Math.sin(yaw) * v.vxMmS + Math.cos(yaw) * v.vyMmS) / 1000 * dt,
      yaw: Math.atan2(Math.sin(yaw + v.wzMradS / 1000 * dt), Math.cos(yaw + v.wzMradS / 1000 * dt)) };
    if (!baseClear(s, pose)) return fail(s, 'Trayectoria bloqueada: cubo o límite del escenario.');
    s.pose = pose;
    s.wheelAngles = { ...s.wheelAngles };
    for (const key of ROBOT_WHEEL_KEYS) s.wheelAngles[key] += s.wheels[key] / 40 * dt;
  }
  return syncAttachments(s);
}

export function startManualScan(s: CollectorState): CollectorState {
  if (s.running || s.jointTarget || ROBOT_WHEEL_KEYS.some(key => s.wheels[key] !== 0)) return s;
  return { ...startScan(s), running: false, phase: 'idle', detail: 'Barrido local: 37 muestras, base detenida.' };
}
