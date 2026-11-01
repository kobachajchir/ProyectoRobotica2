/** Robot geometry is configuration, never a property of a saved destination.
 * SI units at the engine boundary; joint UI/records use degrees and gripper mm. */
import pvc25V17 from './models/pvc25-v17-blanco.json';

export type Point3 = [number, number, number];
export type Quaternion4 = [number, number, number, number];
export type ArmAngles = number[];
export type ArmEnvironment = 'standalone' | 'mounted';
export interface RigidTransform { position: Point3; quaternion: Quaternion4 }
export interface JointDefinition {
  id: string; label: string; parent: string | null; axis: Point3;
  origin: RigidTransform; sign: 1 | -1; zero: number; min: number; max: number;
  radius: number; visualNode: string;
}
export interface CollisionBox { id: string; center: Point3; size: Point3 }
export interface VisualFollower {
  visualNode: string; parentIndex: number; driverIndex: number; axis: Point3;
  ratio: number; origin: RigidTransform; gripperRadiansPerMm?: number;
}
export interface RobotModelDefinition {
  id: string; revision: string; label: string; units: 'm';
  /** Stable semantics, not a geometry version: change when redefining these frames. */
  mountFrame: string; tcpFrame: string;
  renderBasis: Quaternion4; cadToRobot: RigidTransform;
  mounts: Record<ArmEnvironment, RigidTransform>;
  joints: JointDefinition[]; tcp: RigidTransform;
  gripper: { min: number; max: number; open: number; palmSize: Point3; palmZ: number;
    fingerSize: Point3; fingerZ: number; padSize: Point3; padZ: number;
    travelAxis: Point3; visualNodes: [string, string]; metersPerOpeningMm: number };
  visual: { kind: 'procedural' | 'gltf'; url?: string; restToMount?: RigidTransform; restAngles?: ArmAngles; restOpening?: number; baseSize: Point3; baseZ: number; followers?: VisualFollower[] };
  collision: { towerRadius: number; boxes: CollisionBox[]; allowedPairs: [string, string][] };
  initial: ArmAngles; transport: ArmAngles; seeds: ArmAngles[]; speedDegS: number;
  /** Optional analytic branch for this serial Z,Y,Y,Z,Y,Z family. */
  downward?: 'zyyzyz';
  source: string;
}
export const identity = (position: Point3 = [0, 0, 0]): RigidTransform => ({ position, quaternion: [0, 0, 0, 1] });
const joint = (id: string, label: string, z: number, axis: Point3, min: number, max: number, parent: string | null, radius: number): JointDefinition =>
  ({ id, label, parent, origin: identity([0, 0, z]), axis, sign: 1, zero: 0, min, max, radius, visualNode: id });
/** v03: base_top112 + tower113 = shoulder225 mm above deck.
 * The CAD deck is94.64 mm above ground; native +Y maps to robot +X,
 * native +X to robot -Y (right-handed). Mount is32 mm behind CAD origin.
 * v04 is deliberately not silently substituted for the approved v03 baseline. */
export const PVC32_V03: RobotModelDefinition = {
  id: 'pvc32-v03', revision: '20261027-v03.1', label: 'PVC32 AGV v03', units: 'm',
  mountFrame: 'afr2-arm-mount-v1', tcpFrame: 'afr2-grasp-center-v1',
  renderBasis: [-.5, .5, .5, .5], cadToRobot: { position: [-.032, 0, .09464], quaternion: [0, 0, -Math.SQRT1_2, Math.SQRT1_2] },
  mounts: { standalone: identity(), mounted: identity([-.032, 0, .09464]) },
  joints: [joint('j1', 'J1 · Base', .112, [0, 0, 1], -90, 90, null, .018),
    joint('j2', 'J2 · Hombro', .113, [0, 1, 0], -10, 125, 'j1', .016),
    joint('j3', 'J3 · Codo', .130, [0, 1, 0], -90, 90, 'j2', .016),
    joint('j4', 'J4 · Giro de muñeca', .120, [0, 0, 1], -90, 90, 'j3', .012),
    joint('j5', 'J5 · Inclinacion', .055, [0, 1, 0], -90, 90, 'j4', .012),
    joint('j6', 'J6 · Herramienta', .065, [0, 0, 1], -90, 90, 'j5', .012)],
  tcp: identity([0, 0, .0785]),
  gripper: { min: 4.00708355144542, max: 79.40530723760045, open: 60.011,
    palmSize: [.118, .06, .016], palmZ: .025, fingerSize: [.012, .03, .05], fingerZ: .056,
    padSize: [.003, .032, .024], padZ: .068, travelAxis: [1, 0, 0],
    visualNodes: ['jaw-left', 'jaw-right'], metersPerOpeningMm: .0005 },
  visual: { kind: 'procedural', baseSize: [.172, .063, .028], baseZ: .014 },
  collision: { towerRadius: .035, boxes: [
    { id: 'chassis', center: [0, 0, .052], size: [.278, .232, .08] },
    { id: 'bin-floor', center: [.1775, -.13, .125], size: [.118, .118, .01] },
    { id: 'bin-left', center: [.1775, -.191, .16], size: [.126, .006, .07] },
    { id: 'bin-right', center: [.1775, -.069, .16], size: [.126, .006, .07] },
    { id: 'bin-front', center: [.2395, -.13, .16], size: [.006, .118, .07] },
    { id: 'bin-back', center: [.1155, -.13, .16], size: [.006, .118, .07] }],
    allowedPairs: [['j4', 'j6'], ['j5', 'tcp']] },
  initial: [0, 15, -40, 0, 25, 0], transport: [0, 15, -40, 0, 25, 0],
  seeds: [[0, 45, -60, 0, 90, 0], [0, 80, 30, 0, 60, 0], [60, 20, -60, 0, 40, 0], [-60, 20, -60, 0, 40, 0]],
  speedDegS: 35, downward: 'zyyzyz',
  source: 'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/parametros_prototipo_v03.csv; revision_v03/modelo_v03.json; tcp_offset. Proxies Web aproximados.',
};
/** Register future descriptors here. Loading a GLB is not rig inference. */
export const PVC25_V17 = pvc25V17 as RobotModelDefinition;
export const ROBOT_MODELS: RobotModelDefinition[] = [PVC25_V17, PVC32_V03];
let active = PVC25_V17;
export function robotModel() { return active; }
export function activateRobotModel(id: string) {
  const model = ROBOT_MODELS.find(item => item.id === id);
  if (!model) throw new Error('Modelo no registrado.');
  active = model;
  return model;
}
export function modelProblems(m: RobotModelDefinition): string[] {
  const errors: string[] = [];
  const vector = (p: number[], n: number) => p.length === n && p.every(Number.isFinite);
  const transform = (t: RigidTransform) => t && vector(t.position, 3) && vector(t.quaternion, 4) && Math.abs(Math.hypot(...t.quaternion) - 1) < .0001;
  if (m.units !== 'm' || !m.id || !m.revision || !m.mountFrame || !m.tcpFrame) errors.push('Faltan unidades, identidad o marcos.');
  if (!m.joints.length || !transform(m.tcp) || !transform(m.mounts.mounted) || !transform(m.mounts.standalone)) errors.push('Cadena, TCP o montaje incompletos.');
  if (!vector(m.renderBasis, 4) || Math.abs(Math.hypot(...m.renderBasis) - 1) > .0001 || !transform(m.cadToRobot)) errors.push('Conversion de coordenadas incompleta.');
  m.joints.forEach((j, i) => {
    if (!j.id || m.joints.findIndex(k => k.id === j.id) !== i || j.parent !== (i ? m.joints[i - 1].id : null)
      || !vector(j.axis, 3) || Math.abs(Math.hypot(...j.axis) - 1) > .0001 || !transform(j.origin)
      || ![j.min, j.max, j.zero, j.radius].every(Number.isFinite) || j.max < j.min || j.radius <= 0 || ![1, -1].includes(j.sign)) errors.push(`Articulacion ${i + 1} incompleta.`);
  });
  if (![m.gripper.min, m.gripper.max, m.gripper.open, m.speedDegS].every(Number.isFinite) || m.gripper.min > m.gripper.max || m.speedDegS <= 0) errors.push('Gripper o velocidad invalidos.');
  if (m.gripper.open < m.gripper.min || m.gripper.open > m.gripper.max || !vector(m.gripper.travelAxis, 3) || Math.abs(Math.hypot(...m.gripper.travelAxis) - 1) > .0001
    || !Number.isFinite(m.gripper.metersPerOpeningMm) || m.gripper.metersPerOpeningMm <= 0) errors.push('Movimiento de mordazas incompleto.');
  if (m.collision.boxes.some(box => !vector(box.center, 3) || !vector(box.size, 3) || box.size.some(v => v <= 0))) errors.push('Geometria de exclusion invalida.');
  if (m.visual.kind === 'gltf' && (!m.visual.url || !m.visual.restToMount || !transform(m.visual.restToMount) || m.joints.some(j => !j.visualNode) || !m.gripper.visualNodes.every(Boolean)
    || !m.visual.restAngles || m.visual.restAngles.length !== m.joints.length || !m.visual.restAngles.every(Number.isFinite) || !Number.isFinite(m.visual.restOpening))) errors.push('Archivo, transformacion, postura de exportacion o nodos del rig GLB incompletos.');
  if (m.visual.followers?.some(f => !f.visualNode || !Number.isInteger(f.parentIndex) || !Number.isInteger(f.driverIndex)
    || !m.joints[f.parentIndex] || !m.joints[f.driverIndex] || !transform(f.origin) || !vector(f.axis, 3) || Math.abs(Math.hypot(...f.axis) - 1) > .0001
    || !Number.isFinite(f.ratio) || (f.gripperRadiansPerMm !== undefined && !Number.isFinite(f.gripperRadiansPerMm)))) errors.push('Transmision visual incompleta.');
  if (![m.initial, m.transport, ...m.seeds].every(q => q.length === m.joints.length && q.every((a, i) => Number.isFinite(a) && a >= m.joints[i].min && a <= m.joints[i].max))) errors.push('Posturas fuera de limites.');
  return errors;
}
