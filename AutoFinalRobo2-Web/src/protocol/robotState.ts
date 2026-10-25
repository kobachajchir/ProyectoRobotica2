/** AFR2-20261024-15. Optional pose from the compiled EX01 AVR/ESP producers.
 * Hall/geometry calibration and physical acceptance remain pending.
 * Never integrate drive commands into measured position. */
export const ROBOT_MODES = ['idle', 'manual', 'line-follow', 'wheel-test', 'goto', 'sequence', 'fault', 'unknown'] as const;
export type RobotMode = typeof ROBOT_MODES[number];
export interface RobotPose {
  schema: 'afr2-pose-1';
  frameId: string;
  sequence: number;
  xMm: number;
  yMm: number;
  thetaMrad: number;
  ageMs: number;
}
export interface RobotStateSnapshot {
  mode: RobotMode;
  stopped: boolean | null;
  ageMs: number | null;
  receivedAtMs: number;
  pose: RobotPose | null;
  gotoState: string;
  preferencesState: number | null;
  origin: 'none' | 'local' | 'web' | 'unknown';
  fault: number | null;
}
function finite(value: unknown, bound: number): value is number { return typeof value === 'number' && Number.isFinite(value) && Math.abs(value) <= bound; }
function uint32(value: unknown): value is number { return typeof value === 'number' && Number.isInteger(value) && value >= 0 && value <= 0xffffffff; }

export function parseRobotState(value: Record<string, unknown>): RobotStateSnapshot {
  const candidate = value.pose;
  let pose: RobotPose | null = null;
  if (candidate && typeof candidate === 'object' && !Array.isArray(candidate)) {
    const p = candidate as Record<string, unknown>;
    if (p.schema === 'afr2-pose-1' && p.valid === true && typeof p.frameId === 'string' && p.frameId.length > 0 && p.frameId.length <= 64 && uint32(p.sequence) && uint32(p.ageMs) && finite(p.xMm, 0x7fffffff) && finite(p.yMm, 0x7fffffff) && finite(p.thetaMrad, 3142)) {
      pose = { schema: 'afr2-pose-1', frameId: p.frameId, sequence: p.sequence, xMm: p.xMm, yMm: p.yMm, thetaMrad: p.thetaMrad, ageMs: p.ageMs };
    }
  }
  return {
    mode: typeof value.mode === 'string' && ROBOT_MODES.some(mode => mode === value.mode) ? value.mode as RobotMode : 'unknown',
    stopped: typeof value.stopped === 'boolean' ? value.stopped : null,
    ageMs: uint32(value.ageMs) ? value.ageMs : null,
    receivedAtMs: Date.now(), pose,
    gotoState: typeof value.gotoState === 'string' ? value.gotoState : 'unknown',
    preferencesState: uint32(value.preferencesState) ? value.preferencesState : null,
    origin: value.origin === 'none' || value.origin === 'local' || value.origin === 'web' ? value.origin : 'unknown',
    fault: uint32(value.fault) ? value.fault : null,
  };
}

export function stateAge(snapshot: RobotStateSnapshot | null, now: number): number | null {
  if (!snapshot || snapshot.ageMs === null) return null;
  return snapshot.ageMs + Math.max(0, now - snapshot.receivedAtMs);
}
export function poseAge(snapshot: RobotStateSnapshot | null, now: number): number | null {
  return snapshot?.pose ? snapshot.pose.ageMs + Math.max(0, now - snapshot.receivedAtMs) : null;
}
