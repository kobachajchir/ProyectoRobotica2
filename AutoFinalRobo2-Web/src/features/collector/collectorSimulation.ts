import { forwardMecanumKinematics, inverseMecanumKinematics } from '../control/sequences/mecanumKinematics';
import { ROBOT_WHEEL_KEYS, type RobotWheelValues } from '../../components/three/robotWheelMotion';
import { armPath, armRoute, clamp, CUBE_SIZE, environmentClear, forwardArm, inverseDownward, JOINTS, localToWorld, pickupSolutions, rad,
  TRANSPORT, worldToLocal, type ArmAngles, type ArmEnvironment, type Point3 } from './armKinematics';
import { robotModel, modelProblems } from './robotModel';
import { clearConfiguration, clearOpening, type ArmMotionPlan, type MotionEnvironment } from './armMotion';

export interface Cube { id: number; point: Point3; location: 'floor' | 'gripper' | 'bin'; slot?: number }
export interface Echo { angle: number; distance: number | null; point: Point3 | null; at: number }
export type Phase = 'idle' | 'prepare' | 'scan' | 'approach' | 'remeasure' | 'pregrasp' | 'grasp' | 'lift' | 'deposit' | 'release' | 'stow' | 'complete' | 'fault';
export const PHASE_LABELS: Record<Phase, string> = {
  idle: 'En espera', prepare: 'Preparando el robot', scan: 'Explorando', approach: 'Aproximando', remeasure: 'Midiendo de nuevo',
  pregrasp: 'Preparando toma', grasp: 'Agarrando', lift: 'Levantando', deposit: 'Llevando al deposito',
  release: 'Depositando', stow: 'Recogiendo brazo', complete: 'Recorrido terminado', fault: 'Intervencion necesaria',
};
export const BIN = { center: [.1775, -.13, .13] as Point3, width: .118, length: .118, rim: .19 };
export const BIN_SLOTS: Point3[] = [[.15, -.1025, .155], [.205, -.1025, .155], [.15, -.1575, .155], [.205, -.1575, .155]];
const ZERO = (): RobotWheelValues => ({ fl: 0, fr: 0, rl: 0, rr: 0 });
export interface CollectorState {
  pose: { x: number; y: number; yaw: number };
  joints: ArmAngles; opening: number; wheels: RobotWheelValues; wheelAngles: RobotWheelValues;
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
function syncAttachments(s: CollectorState): CollectorState {
  const tcp = localToWorld(forwardArm(s.joints).tcp, s.pose);
  return { ...s, cubes: s.cubes.map(cube => cube.location === 'gripper' ? { ...cube, point: tcp }
    : cube.location === 'bin' && cube.slot !== undefined ? { ...cube, point: localToWorld(BIN_SLOTS[cube.slot], s.pose) } : cube) };
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
    pose: { x: 0, y: 0, yaw: 0 }, joints: [...robotModel().initial], opening: robotModel().gripper.open,
    wheels: ZERO(), wheelAngles: ZERO(), cubes: generateCubes(seed, count), seed, requestedCount: count,
    sensorAngle: 0, sensorHeight: .045, scanning: false, scanDirection: 1,
    echoes: [], reading: null, clock: 0, sampleDue: 0, scanComplete: false,
    phase: 'idle', running: false, detail: 'Selecciona el robot o el brazo para empezar.', elapsed: 0,
    target: null, jointTarget: null, jointQueue: [], environment: 'mounted', singleTarget: false,
    stored: 0, selected: 'robot', selectedJoint: 0, references: false, workspace: true, pointRun: null, motionEpoch: 0,
    rigIssue: robotModel().visual.kind === 'gltf' ? 'Cargando y vinculando el rig del brazo.' : null,
  };
}
export function halt(s: CollectorState, detail = 'Simulacion detenida. Posiciones conservadas.'): CollectorState {
  return { ...s, wheels: ZERO(), scanning: false, running: false, jointTarget: null, jointQueue: [], pointRun: null, motionEpoch: s.motionEpoch + 1, phase: 'idle', elapsed: 0, detail };
}
function fail(s: CollectorState, detail: string): CollectorState {
  return { ...halt(s, detail), phase: 'fault' };
}
function transition(s: CollectorState, phase: Phase, detail: string): CollectorState {
  return { ...s, phase, elapsed: 0, detail, wheels: ZERO() };
}
function startScan(s: CollectorState): CollectorState {
  return { ...transition(s, 'scan', 'Barrido con la base detenida.'), scanning: true, sensorAngle: -90,
    echoes: [], reading: null, scanDirection: 1, sampleDue: s.clock + .3, scanComplete: false, target: null };
}
export function startCollection(s: CollectorState): CollectorState {
  if (s.rigIssue || s.jointTarget || s.scanning || Object.values(s.wheels).some(v => v !== 0) || modelProblems(robotModel()).length) return s;
  if (s.cubes.some(c => c.location === 'gripper')) return fail(s, 'Hay un cubo sujeto. Depositalo antes de comenzar.');
  if (s.stored >= BIN_SLOTS.length) return fail(s, 'Deposito lleno. Genera otra escena para vaciarlo.');
  if (s.cubes.every(c => c.location !== 'floor')) return { ...s, running: false, phase: 'complete', detail: 'Todos los cubos estan guardados.' };
  return scheduleArm({ ...transition(s, 'stow', 'Preparando la postura de transporte.'), running: true,
    singleTarget: false, environment: 'mounted' }, TRANSPORT);
}

/** Ideal horizontal centre ray vs 50mm cube AABBs. Invalid/no echo stays null.
 * Only this sensor model reads floor geometry; mission planning consumes Echo.
 * Height is an explicit proposed low mount, not the real CAD sensor height. */
export function measure(s: CollectorState): Echo {
  const origin = localToWorld([.103, 0, s.sensorHeight], s.pose);
  const theta = s.pose.yaw + rad(s.sensorAngle), direction = [Math.cos(theta), Math.sin(theta)];
  let nearest = Infinity;
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
    if (near <= far && near >= .02) nearest = Math.min(nearest, near);
  }
  const distance = Number.isFinite(nearest) ? nearest : null;
  return { angle: s.sensorAngle, distance, at: s.clock,
    point: distance === null ? null : [origin[0] + direction[0] * distance, origin[1] + direction[1] * distance, origin[2]] };
}
export function baseClear(s: CollectorState, pose: CollectorState['pose']) {
  if (Math.abs(pose.x) > 2 || Math.abs(pose.y) > 2) return false;
  return s.cubes.filter(c => c.location === 'floor').every(cube => {
    const p = worldToLocal(cube.point, pose);
    return Math.abs(p[0]) > .158 || Math.abs(p[1]) > .135;
  });
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
  return { ...s, wheels, detail: 'Movimiento local de ruedas. Solta el control para detener.' };
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
  if (!armPath(s.joints, joints, s.environment)) return { ...s, detail: 'Limite del entorno: ajusta otro eje o recoge el brazo.' };
  return syncAttachments({ ...s, joints, selectedJoint: index, detail: 'Postura del brazo actualizada en el simulador.' });
}
function alignGripper(q: ArmAngles, yaw: number): ArmAngles {
  const aligned = [...q] as ArmAngles;
  const angle = q[0] + yaw * 180 / Math.PI;
  aligned[5] = ((angle + 45) % 90 + 90) % 90 - 45;
  return aligned;
}
function scheduleArm(s: CollectorState, joints: ArmAngles): CollectorState {
  const route = armRoute(s.joints, joints, s.environment);
  if (!route) return fail(s, 'Trayectoria del brazo bloqueada por el entorno. Ajusta la posicion del robot.');
  return { ...s, jointTarget: route[0], jointQueue: route.slice(1) };
}
export function moveArmTo(s: CollectorState, joints: ArmAngles): CollectorState {
  if (s.rigIssue || modelProblems(robotModel()).length || s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  return scheduleArm({ ...s, detail: 'Moviendo el brazo a la postura elegida.' }, joints);
}
function beginArm(s: CollectorState, point: Point3, phase: Phase, detail: string): CollectorState {
  const solution = inverseDownward(point);
  if (!solution) return fail(s, 'Toma sin solucion en la rama vertical del modelo activo.');
  const joints = alignGripper(solution, s.pose.yaw);
  return scheduleArm(transition(s, phase, detail), joints);
}

export function cubeReach(s: CollectorState, cube: Cube) {
  if (cube.location !== 'floor') return { reachable: false, reason: cube.location === 'bin' ? 'Guardado' : 'En la pinza' };
  const point = worldToLocal(cube.point, s.pose);
  const reachable = !!pickupSolutions(point, s.pose.yaw);
  return { reachable, reason: reachable ? 'Dentro del alcance de toma' : 'Fuera del alcance: acerca el robot' };
}
function beginTarget(s: CollectorState): CollectorState {
  if (!s.target) return fail(s, 'No hay un objetivo seleccionado.');
  const p = worldToLocal(s.target, s.pose);
  if (pickupSolutions(p, s.pose.yaw)) return beginArm({ ...s, opening: robotModel().gripper.open }, [p[0], p[1], .10], 'pregrasp', 'Objeto dentro del alcance: la base queda detenida y el brazo inicia la toma.');
  return { ...transition(s, 'approach', 'Objeto fuera del alcance: acercando el robot.'), sensorAngle: 0, reading: null, sampleDue: s.clock + .3 };
}
export function collectCube(s: CollectorState, id: number): CollectorState {
  if (s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  const cube = s.cubes.find(c => c.id === id && c.location === 'floor');
  if (!cube) return fail(s, 'Selecciona un cubo del suelo.');
  if (s.stored >= BIN_SLOTS.length) return fail(s, 'Deposito lleno.');
  if (s.cubes.some(c => c.location === 'gripper')) return fail(s, 'Primero deposita el cubo sujeto.');
  const next: CollectorState = { ...s, target: [...cube.point], selected: `cube:${id}`, singleTarget: true,
    running: true, environment: 'mounted', wheels: ZERO(), elapsed: 0 };
  if (cubeReach(s, cube).reachable) return beginTarget(next);
  return scheduleArm(transition(next, 'prepare', 'Recogiendo el brazo antes de acercar el auto.'), TRANSPORT);
}
export function positionCube(s: CollectorState, id: number, point: Point3): CollectorState {
  if (s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  const cube = s.cubes.find(c => c.id === id);
  if (cube && cube.location !== 'floor') return s;
  if (!point.every(Number.isFinite) || Math.abs(point[0]) > 2 || Math.abs(point[1]) > 2) return s;
  const p: Point3 = [point[0], point[1], .025];
  if (s.cubes.some(c => c.id !== id && c.location === 'floor' && Math.abs(c.point[0] - p[0]) < .06 && Math.abs(c.point[1] - p[1]) < .06)) {
    return { ...s, detail: 'Esa posicion se superpone con otro cubo.' };
  }
  if (!cube && s.cubes.length >= 24) return { ...s, detail: 'La escena admite hasta 24 cubos.' };
  const cubes: Cube[] = cube ? s.cubes.map(c => c.id === id ? { ...c, point: p } : c) : [...s.cubes, { id, point: p, location: 'floor' }];
  if (!baseClear({ ...s, cubes }, s.pose)) return { ...s, detail: 'La posicion del cubo invade el robot.' };
  return { ...s, cubes, selected: `cube:${id}`, echoes: [], reading: null, detail: 'Posicion del cubo actualizada.' };
}
export function manualGrasp(s: CollectorState): CollectorState {
  if (s.running || s.scanning || s.jointTarget || ROBOT_WHEEL_KEYS.some(k => s.wheels[k] !== 0)) return s;
  if (s.cubes.some(c => c.location === 'gripper')) return s;
  const tcp = localToWorld(forwardArm(s.joints).tcp, s.pose);
  // Scene truth is used here only as contact physics, never as mission input.
  const cube = s.cubes.find(c => c.location === 'floor' && Math.hypot(...c.point.map((v, i) => v - tcp[i])) < .025);
  if (!cube) return { ...s, opening: 50, detail: 'Agarre fallido: no hay un cubo centrado entre las mordazas.' };
  return { ...s, opening: 50, cubes: s.cubes.map(c => c.id === cube.id ? { ...c, location: 'gripper', point: tcp } : c), detail: 'Cubo sujeto en la simulacion.' };
}
export function manualRelease(s: CollectorState): CollectorState {
  if (s.running || s.pointRun) return s;
  const held = s.cubes.find(c => c.location === 'gripper');
  if (!held) return { ...s, opening: robotModel().gripper.open };
  const tcp = forwardArm(s.joints).tcp;
  const slot = BIN_SLOTS[s.stored];
  const inBin = !!slot && Math.hypot(tcp[0] - slot[0], tcp[1] - slot[1]) < .022 && tcp[2] >= BIN.rim + CUBE_SIZE / 2;
  if (!inBin) return { ...s, detail: 'Lleva el cubo sobre la siguiente celda del deposito antes de soltarlo.' };
  return { ...s, opening: robotModel().gripper.open, stored: s.stored + 1,
    cubes: s.cubes.map(c => c.id === held.id ? { ...c, location: 'bin', slot: s.stored, point: localToWorld(slot, s.pose) } : c), detail: 'Cubo guardado en el deposito.' };
}
export function nextBinPose(s: CollectorState) {
  const slot = BIN_SLOTS[s.stored];
  return slot ? beginArm(s, [slot[0], slot[1], .23], 'deposit', 'Llevando el cubo sobre la celda libre.') : fail(s, 'Deposito lleno.');
}
export function tickCollector(previous: CollectorState, dt: number): CollectorState {
  let s = { ...previous, clock: previous.clock + dt, elapsed: previous.elapsed + dt };
  if (s.scanning && s.clock >= s.sampleDue) {
    const reading = measure(s);
    s = { ...s, reading, echoes: [...s.echoes.filter(e => e.angle !== reading.angle), reading].slice(-37), sampleDue: s.clock + .25 };
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
      if (!clearOpening(s.joints, s.opening, opening, s.pointRun.environment)) return fail(s, 'La apertura final invade una restriccion del modelo.');
      s = { ...s, opening, pointRun: null, detail: 'Destino alcanzado en el simulador. Postura y orientacion recuperadas.' };
    }
  }
  if (s.running) {
    switch (s.phase) {
      case 'prepare': if (!s.jointTarget) s = beginTarget(s); break;
      case 'scan': {
        if (!s.scanComplete) break;
        const candidates = s.echoes.filter(e => e.point !== null && e.distance !== null).sort((a, b) => a.distance! - b.distance!);
        const echo = candidates[0];
        if (!echo?.point) return fail(s, 'Objetivo perdido: el barrido no encontro candidatos. Reubica el robot o cambia la escena.');
        const theta = s.pose.yaw + rad(echo.angle);
        const target: Point3 = [echo.point[0] + .025 * Math.cos(theta), echo.point[1] + .025 * Math.sin(theta), .025];
        s = beginTarget({ ...s, target, sensorAngle: 0, reading: null });
        break;
      }
      case 'approach': {
        if (!s.target) return fail(s, 'Objetivo perdido.');
        const dx = s.target[0] - s.pose.x, dy = s.target[1] - s.pose.y;
        const desired = Math.atan2(dy, dx), error = Math.atan2(Math.sin(desired - s.pose.yaw), Math.cos(desired - s.pose.yaw));
        if (Math.abs(error) > .025) {
          s.wheels = driveVector(0, 0, clamp(error * 2, -.5, .5));
          s.reading = null; s.sampleDue = s.clock + .3;
        } else if (s.clock >= s.sampleDue) {
          s.reading = measure(s); s.sampleDue = s.clock + .1;
        }
        if (Math.abs(error) <= .025) {
          if (!s.reading || s.clock - s.reading.at > .2) s.wheels = ZERO();
          else if (s.reading.distance === null) return fail(s, 'Objetivo perdido: no hay eco frontal reciente.');
          else if (Math.hypot(dx, dy) > .238) s.wheels = driveVector(Math.min(.09, Math.max(.015, (Math.hypot(dx, dy) - .235) * 1.5)), 0, 0);
          else s = { ...transition(s, 'remeasure', 'Base detenida; confirmando la toma con otro eco.'), sampleDue: s.clock + .3 };
        }
        break;
      }
      case 'remeasure': {
        if (s.clock < s.sampleDue) break;
        const reading = measure(s);
        if (!reading.point) return fail(s, 'Objetivo perdido al detenerse.');
        const target: Point3 = s.singleTarget && s.target ? s.target : [reading.point[0] + .025 * Math.cos(s.pose.yaw), reading.point[1] + .025 * Math.sin(s.pose.yaw), .025];
        const p = worldToLocal(target, s.pose);
        s = beginArm({ ...s, target, reading }, [p[0], p[1], .10], 'pregrasp', 'Abriendo la pinza y acercando por arriba.');
        s.opening = robotModel().gripper.open;
        break;
      }
      case 'pregrasp':
        if (!s.jointTarget && s.target) s = beginArm(s, worldToLocal(s.target, s.pose), 'grasp', 'Descendiendo sobre el candidato.');
        break;
      case 'grasp':
        if (!s.jointTarget) {
          s = manualGrasp({ ...s, running: false });
          if (!s.cubes.some(c => c.location === 'gripper')) return fail(s, 'Agarre fallido: la estimacion por eco no centro el cubo.');
          const tcp = forwardArm(s.joints).tcp;
          s = beginArm({ ...s, running: true }, [tcp[0], tcp[1], .215], 'lift', 'Levantando el cubo sobre el borde del deposito.');
        }
        break;
      case 'lift': if (!s.jointTarget) s = nextBinPose(s); break;
      case 'deposit': if (!s.jointTarget) s = transition(s, 'release', 'Abriendo la pinza sobre el deposito.'); break;
      case 'release':
        if (s.elapsed >= .4) {
          const released = manualRelease({ ...s, running: false });
          if (released.stored === s.stored) return fail(s, 'No se pudo depositar: la celda no esta alineada.');
          s = scheduleArm({ ...transition(released, 'stow', 'Recogiendo el brazo antes del proximo movimiento.'), running: true }, TRANSPORT);
        }
        break;
      case 'stow':
        if (!s.jointTarget) {
          if (s.singleTarget) return { ...s, running: false, phase: 'complete', detail: 'Cubo guardado. Selecciona el siguiente objetivo.' };
          if (s.cubes.every(c => c.location !== 'floor')) return { ...s, running: false, phase: 'complete', detail: 'Todos los cubos estan guardados.' };
          if (s.stored >= BIN_SLOTS.length) return fail(s, 'Deposito lleno. Se detuvo la recoleccion.');
          s = startScan(s);
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
    if (!baseClear(s, pose)) return fail(s, 'Trayectoria bloqueada: cubo o limite del escenario.');
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
