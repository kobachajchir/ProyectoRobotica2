import type { CollectorState } from './collectorSimulation';
import { worldToLocal } from './armKinematics';
import { robotModel } from './robotModel';
import type { MotionEnvironment } from './armMotion';

export function pointBusy(s: CollectorState) { return s.running || s.scanning || !!s.jointTarget || Object.values(s.wheels).some(v => v !== 0); }
export function motionEnvironment(s: CollectorState): MotionEnvironment {
  const width = .05 * (Math.abs(Math.cos(s.pose.yaw)) + Math.abs(Math.sin(s.pose.yaw)));
  return { environment: s.environment, boxes: s.cubes.filter(c => c.location !== 'gripper').map(c => ({ id: `cube:${c.id}`, center: worldToLocal(c.point, s.pose), size: [width, width, .05] })) };
}
/** Deliberately excludes clock/readings/UI selection; includes every motion input. */
export function pointContext(s: CollectorState, pointId: string, updatedAt: string, restoreOpening: boolean) {
  const m = robotModel();
  return JSON.stringify({ model: m, rigIssue: s.rigIssue, epoch: s.motionEpoch, from: s.joints, opening: s.opening, environment: s.environment, pose: s.pose,
    cubes: s.cubes.map(c => [c.id, c.location, c.point]), pointId, updatedAt, restoreOpening });
}
