import { robotModel, type ArmAngles, type Point3, type Quaternion4, type RobotModelDefinition } from './robotModel';
import { forwardModel } from './modelKinematics';

export interface SavedArmPoint {
  id: string; name: string; createdAt: string; updatedAt: string;
  position: Point3; quaternion: Quaternion4; opening: number;
  source: { modelId: string; revision: string; frameSignature: string; jointIds: string[]; simulation: string; angles: ArmAngles };
}
export const ARM_POINTS_KEY = 'afr2-arm-points-v1';
export function pointId() {
  return typeof crypto.randomUUID === 'function' ? crypto.randomUUID()
    : `point-${Array.from(crypto.getRandomValues(new Uint32Array(4)), v => v.toString(16)).join('-')}`;
}
export function frameSignature(model = robotModel()) {
  return JSON.stringify({ mount: model.mountFrame, tcp: model.tcpFrame, mounts: model.mounts, tool: model.tcp });
}
export function capturePoint(name: string, angles: ArmAngles, opening: number, simulation: string, model: RobotModelDefinition = robotModel()): SavedArmPoint {
  const pose = forwardModel(angles, model), now = new Date().toISOString();
  return { id: pointId(), name: name.trim().slice(0, 80), createdAt: now, updatedAt: now,
    position: pose.tcp, quaternion: pose.orientation, opening,
    source: { modelId: model.id, revision: model.revision, frameSignature: frameSignature(model),
      jointIds: model.joints.map(j => j.id), angles: [...angles], simulation } };
}
function finiteArray(value: unknown, size?: number): value is number[] {
  return Array.isArray(value) && (size === undefined || value.length === size) && value.every(v => typeof v === 'number' && Number.isFinite(v));
}
function isPoint(value: unknown): value is SavedArmPoint {
  if (!value || typeof value !== 'object') return false;
  const p = value as SavedArmPoint;
  return typeof p.id === 'string' && p.id.length > 0 && typeof p.name === 'string' && p.name.trim().length > 0 && p.name.length <= 80
    && typeof p.createdAt === 'string' && Number.isFinite(Date.parse(p.createdAt)) && typeof p.updatedAt === 'string' && Number.isFinite(Date.parse(p.updatedAt))
    && finiteArray(p.position, 3) && finiteArray(p.quaternion, 4) && Math.abs(Math.hypot(...p.quaternion) - 1) < .0001
    && Number.isFinite(p.opening) && p.opening >= 0 && !!p.source && typeof p.source.modelId === 'string'
    && typeof p.source.revision === 'string' && typeof p.source.frameSignature === 'string' && typeof p.source.simulation === 'string'
    && Array.isArray(p.source.jointIds) && p.source.jointIds.length > 0 && p.source.jointIds.every(id => typeof id === 'string' && id.length > 0)
    && finiteArray(p.source.angles, p.source.jointIds.length);
}
export function parsePointLibrary(raw: string): SavedArmPoint[] {
  const document = JSON.parse(raw) as { format?: string; version?: number; points?: unknown };
  if (!document || document.format !== 'afr2-arm-points' || document.version !== 1 || !Array.isArray(document.points)
    || document.points.length > 1000 || !document.points.every(isPoint)) throw new Error('El archivo no es una biblioteca de puntos v1 valida. Los puntos actuales se conservan.');
  if (new Set(document.points.map(p => p.id)).size !== document.points.length) throw new Error('La biblioteca contiene identificadores repetidos.');
  return document.points;
}
export function serializePoints(points: SavedArmPoint[]) { return JSON.stringify({ format: 'afr2-arm-points', version: 1, points }, null, 2); }
export function loadPoints(): { points: SavedArmPoint[]; issue: string; blocked: boolean; raw: string | null } {
  let raw: string | null = null;
  try {
    raw = window.localStorage.getItem(ARM_POINTS_KEY);
    return { points: raw ? parsePointLibrary(raw) : [], issue: '', blocked: false, raw };
  } catch (error) {
    return { points: [], issue: `${error instanceof Error ? error.message : 'Almacenamiento local no disponible.'} Se trabajara en memoria; exporta un respaldo.`, blocked: true, raw };
  }
}
export function downloadPoints(text: string, filename = 'puntos-brazo.afr2-arm-points.json') {
  const url = URL.createObjectURL(new Blob([text], { type: 'application/json;charset=utf-8' }));
  const anchor = document.createElement('a'); anchor.href = url; anchor.download = filename;
  document.body.appendChild(anchor); anchor.click(); anchor.remove(); URL.revokeObjectURL(url);
}
