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
/** Register future descriptors here. Loading a GLB is not rig inference. */
export const PVC25_V17 = pvc25V17 as RobotModelDefinition;
export const ROBOT_MODELS: RobotModelDefinition[] = [PVC25_V17];
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
  if (!vector(m.renderBasis, 4) || Math.abs(Math.hypot(...m.renderBasis) - 1) > .0001 || !transform(m.cadToRobot)) errors.push('Conversión de coordenadas incompleta.');
  m.joints.forEach((j, i) => {
    if (!j.id || m.joints.findIndex(k => k.id === j.id) !== i || j.parent !== (i ? m.joints[i - 1].id : null)
      || !vector(j.axis, 3) || Math.abs(Math.hypot(...j.axis) - 1) > .0001 || !transform(j.origin)
      || ![j.min, j.max, j.zero, j.radius].every(Number.isFinite) || j.max < j.min || j.radius <= 0 || ![1, -1].includes(j.sign)) errors.push(`Articulacion ${i + 1} incompleta.`);
  });
  if (![m.gripper.min, m.gripper.max, m.gripper.open, m.speedDegS].every(Number.isFinite) || m.gripper.min > m.gripper.max || m.speedDegS <= 0) errors.push('Gripper o velocidad invalidos.');
  if (m.gripper.open < m.gripper.min || m.gripper.open > m.gripper.max || !vector(m.gripper.travelAxis, 3) || Math.abs(Math.hypot(...m.gripper.travelAxis) - 1) > .0001
    || !Number.isFinite(m.gripper.metersPerOpeningMm) || m.gripper.metersPerOpeningMm <= 0) errors.push('Movimiento de mordazas incompleto.');
  if (m.collision.boxes.some(box => !vector(box.center, 3) || !vector(box.size, 3) || box.size.some(v => v <= 0))) errors.push('Geometría de exclusión invalida.');
  if (m.visual.kind === 'gltf' && (!m.visual.url || !m.visual.restToMount || !transform(m.visual.restToMount) || m.joints.some(j => !j.visualNode) || !m.gripper.visualNodes.every(Boolean)
    || !m.visual.restAngles || m.visual.restAngles.length !== m.joints.length || !m.visual.restAngles.every(Number.isFinite) || !Number.isFinite(m.visual.restOpening))) errors.push('Archivo, transformación, postura de exportación o nodos del rig GLB incompletos.');
  if (m.visual.followers?.some(f => !f.visualNode || !Number.isInteger(f.parentIndex) || !Number.isInteger(f.driverIndex)
    || !m.joints[f.parentIndex] || !m.joints[f.driverIndex] || !transform(f.origin) || !vector(f.axis, 3) || Math.abs(Math.hypot(...f.axis) - 1) > .0001
    || !Number.isFinite(f.ratio) || (f.gripperRadiansPerMm !== undefined && !Number.isFinite(f.gripperRadiansPerMm)))) errors.push('Transmisión visual incompleta.');
  if (![m.initial, m.transport, ...m.seeds].every(q => q.length === m.joints.length && q.every((a, i) => Number.isFinite(a) && a >= m.joints[i].min && a <= m.joints[i].max))) errors.push('Posturas fuera de límites.');
  return errors;
}
