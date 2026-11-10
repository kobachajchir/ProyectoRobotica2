import { Vector3 } from 'three';
import { activateRobotModel, robotModel, type ArmAngles, type ArmEnvironment, type Point3 } from './robotModel';
import { forwardModel, rigid } from './modelKinematics';
import { clearConfiguration, routeTo, samplePath } from './armMotion';
import { CUBE_MODEL } from './auxiliaryModels';
export type { ArmAngles, ArmEnvironment, Point3 } from './robotModel';

/** Compatibility facade for collection, now backed by the active descriptor.
 * X forward, Y left, Z up. No visual file defines its own link dimensions. */
export let STANDALONE_MOUNT = robotModel().mounts.standalone.position;
export let JOINTS = robotModel().joints;
export let TRANSPORT = robotModel().transport;
export let ARM_MOUNT = robotModel().mounts.mounted.position;
export const CUBE_SIZE = CUBE_MODEL.size[0];
// V17's downward wrist branch cannot keep the floor pickup radius at 215 mm
// TCP height. Use a lower lift before folding into the transport posture.
export const PICKUP_LIFT_HEIGHT = .18;
export let GRIP_MIN_MM = robotModel().gripper.min;
export const rad = (deg: number) => deg * Math.PI / 180;
export const clamp = (value: number, min: number, max: number) => Math.max(min, Math.min(max, value));
export let CAD_TO_THREE = rigid({ position: [0, 0, 0], quaternion: robotModel().renderBasis });
export const toThree = (p: Point3): Point3 => new Vector3(...p).applyMatrix4(CAD_TO_THREE).toArray() as Point3;
/** Single activation boundary; provider resets both simulated workspaces. */
export function setArmModel(id: string) {
  const model = activateRobotModel(id);
  STANDALONE_MOUNT = model.mounts.standalone.position; ARM_MOUNT = model.mounts.mounted.position;
  JOINTS = model.joints; TRANSPORT = model.transport; GRIP_MIN_MM = model.gripper.min;
  CAD_TO_THREE = rigid({ position: [0, 0, 0], quaternion: model.renderBasis });
  return model;
}

export function forwardArm(q: ArmAngles, mount: Point3 = ARM_MOUNT) {
  const model = robotModel(), environment = mount === STANDALONE_MOUNT ? 'standalone' : 'mounted';
  return forwardModel(q, model, { ...model.mounts[environment], position: mount });
}
export function armViewFocus(): Point3 {
  const model = robotModel(), reach = model.joints.reduce((sum, j) => sum + Math.hypot(...j.origin.position), Math.hypot(...model.tcp.position));
  return toThree([STANDALONE_MOUNT[0], STANDALONE_MOUNT[1], STANDALONE_MOUNT[2] + reach / 2]);
}

/** A bounded top-down grasp branch (J4=J6=0); deliberately no claim of full IK.
 * 130mm upper arm, 120+55mm to J5, 65+78.5mm tool pointing down. */
export function inverseDownward(target: Point3, mount: Point3 = ARM_MOUNT): ArmAngles | null {
  const model = robotModel();
  if (!model.downward || model.joints.length !== 6) return null;
  if (model.tcp.position[0] !== 0 || model.tcp.position[1] !== 0 || model.tcp.quaternion.some((a, k) => a !== (k === 3 ? 1 : 0))) return null;
  const axes = [[0, 0, 1], [0, 1, 0], [0, 1, 0], [0, 0, 1], [0, 1, 0], [0, 0, 1]];
  if (model.joints.some((j, i) => j.sign !== 1 || j.zero !== 0 || j.axis.some((a, k) => a !== axes[i][k])
    || j.origin.position[0] !== 0 || j.origin.position[1] !== 0 || j.origin.quaternion.some((a, k) => a !== (k === 3 ? 1 : 0)))) return null;
  const geometry = { shoulder: model.joints[0].origin.position[2] + model.joints[1].origin.position[2],
    upper: model.joints[2].origin.position[2], forearm: model.joints[3].origin.position[2] + model.joints[4].origin.position[2],
    tool: model.joints[5].origin.position[2] + model.tcp.position[2] };
  const environment = mount === STANDALONE_MOUNT ? 'standalone' : 'mounted';
  const local = new Vector3(...target).applyMatrix4(rigid({ ...model.mounts[environment], position: mount }).invert());
  const { x, y } = local;
  const radius = Math.hypot(x, y), z = local.z + geometry.tool - geometry.shoulder;
  const { upper, forearm } = geometry;
  const cosine = (radius * radius + z * z - upper ** 2 - forearm ** 2) / (2 * upper * forearm);
  if (cosine < -1 || cosine > 1) return null;
  for (const elbow of [Math.acos(cosine), -Math.acos(cosine)]) {
    const shoulder = Math.atan2(radius, z) - Math.atan2(forearm * Math.sin(elbow), upper + forearm * Math.cos(elbow));
    const q: ArmAngles = [Math.atan2(y, x) * 180 / Math.PI, shoulder * 180 / Math.PI,
      elbow * 180 / Math.PI, 0, 180 - (shoulder + elbow) * 180 / Math.PI, 0];
    // A frontal V17 grasp lies at J1=90°. Discard only real limit violations,
    // not the floating-point residue of the mounted quarter-turn quaternion.
    if (q.every((value, i) => value >= JOINTS[i].min - 1e-7 && value <= JOINTS[i].max + 1e-7)) return q.map((value, i) => clamp(value, JOINTS[i].min, JOINTS[i].max));
  }
  return null;
}

export function localToWorld(p: Point3, pose: { x: number; y: number; yaw: number }): Point3 {
  const c = Math.cos(pose.yaw), s = Math.sin(pose.yaw);
  return [pose.x + c * p[0] - s * p[1], pose.y + s * p[0] + c * p[1], p[2]];
}
export function worldToLocal(p: Point3, pose: { x: number; y: number; yaw: number }): Point3 {
  const x = p[0] - pose.x, y = p[1] - pose.y, c = Math.cos(pose.yaw), s = Math.sin(pose.yaw);
  return [c * x + s * y, -s * x + c * y, p[2]];
}

/** Conservative capsules for proposed rig/environment clearance. No full BRep
 * self-collision check: yellow sampled workspace must not be called certified. */
export function environmentClear(q: ArmAngles, environment: ArmEnvironment = 'mounted') {
  return clearConfiguration(q, robotModel().gripper.open, { environment, boxes: [] });
}

export function armPath(from: ArmAngles, to: ArmAngles, environment: ArmEnvironment = 'mounted'): ArmAngles[] | null {
  return samplePath(from, to, { environment, boxes: [] }, robotModel().gripper.open);
}

/** Safe, finite waypoint alternatives; every leg uses the same path filter. */
export function armRoute(from: ArmAngles, to: ArmAngles, environment: ArmEnvironment = 'mounted'): ArmAngles[] | null {
  return routeTo(from, to, { environment, boxes: [] }, robotModel().gripper.open);
}

export function workspacePoints(environment: ArmEnvironment = 'mounted') {
  const reachable: number[] = [], excluded: number[] = [];
  const mount = environment === 'standalone' ? STANDALONE_MOUNT : ARM_MOUNT;
  // Discrete downward-grasp samples only, not a solid sphere or a certificate.
  const model = robotModel(), reach = model.joints.reduce((sum, j) => sum + Math.hypot(...j.origin.position), Math.hypot(...model.tcp.position));
  const step = Math.max(.025, reach / 12);
  for (let x = mount[0] - reach; x <= mount[0] + reach; x += step) for (let y = mount[1] - reach; y <= mount[1] + reach; y += step) for (let z = .025; z <= mount[2] + reach; z += step) {
    const q = inverseDownward([x, y, z], mount);
    if (q) (environmentClear(q, environment) ? reachable : excluded).push(...toThree([x, y, z]));
  }
  return { reachable: new Float32Array(reachable), excluded: new Float32Array(excluded) };
}

/** V17 cannot put the TCP at a floor box centre in front of the car: below
 * ~40 mm the palm hits the chassis envelope. Take the box by its upper part. */
export const GRASP_ABOVE_CENTER = .02;
/** Wrist angles that keep the jaws square to a world-axis-aligned box. */
function wristCandidates(q: ArmAngles, yaw: number) {
  const base = ((q[0] + yaw * 180 / Math.PI + 45) % 90 + 90) % 90 - 45;
  return [base, base + 90, base - 90].filter(w => w >= JOINTS[5].min && w <= JOINTS[5].max);
}
/** Approach (100 mm), grasp and lift postures sharing one clear wrist angle,
 * so the jaws never rotate next to the box or the chassis. `point` is the box centre. */
export function pickupSolutions(point: Point3, yaw = 0) {
  const targets: Point3[] = [[point[0], point[1], .10], [point[0], point[1], point[2] + GRASP_ABOVE_CENTER], [point[0], point[1], PICKUP_LIFT_HEIGHT]];
  const raw = targets.map(target => inverseDownward(target));
  if (raw.some(q => !q)) return null;
  for (const wrist of wristCandidates(raw[0]!, yaw)) {
    const solutions = raw.map(q => { const next = [...q!] as ArmAngles; next[5] = wrist; return next; });
    if (solutions.every(q => environmentClear(q))) return solutions;
  }
  return null;
}

export function groundWorkspacePoints() {
  const points: number[] = [];
  const model = robotModel(), reach = model.joints.reduce((sum, j) => sum + Math.hypot(...j.origin.position), Math.hypot(...model.tcp.position)), step = Math.max(.0125, reach / 24);
  for (let x = ARM_MOUNT[0] - reach; x <= ARM_MOUNT[0] + reach; x += step) for (let y = ARM_MOUNT[1] - reach; y <= ARM_MOUNT[1] + reach; y += step) {
    if (pickupSolutions([x, y, .025])) points.push(...toThree([x, y, .004]));
  }
  return new Float32Array(points);
}
