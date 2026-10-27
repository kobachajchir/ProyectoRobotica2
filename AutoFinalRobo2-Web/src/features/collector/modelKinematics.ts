import { Matrix4, Quaternion, Vector3 } from 'three';
import { identity, robotModel, type ArmAngles, type ArmEnvironment, type Point3, type Quaternion4, type RigidTransform, type RobotModelDefinition } from './robotModel';

export const rad = (degrees: number) => degrees * Math.PI / 180;
export function rigid(t: RigidTransform) { return new Matrix4().compose(new Vector3(...t.position), new Quaternion(...t.quaternion), new Vector3(1, 1, 1)); }
export function forwardModel(q: ArmAngles, model: RobotModelDefinition = robotModel(), mount: RigidTransform = identity()) {
  const a = rigid(mount), frames: Matrix4[] = [];
  model.joints.forEach((j, i) => {
    a.multiply(rigid(j.origin)).multiply(new Matrix4().makeRotationAxis(new Vector3(...j.axis), rad((q[i] ?? 0) * j.sign + j.zero)));
    frames.push(a.clone());
  });
  const tool = a.clone().multiply(rigid(model.tcp));
  return { frames, tool, tcp: new Vector3().setFromMatrixPosition(tool).toArray() as Point3,
    orientation: new Quaternion().setFromRotationMatrix(tool).normalize().toArray() as Quaternion4 };
}
export function mountFor(environment: ArmEnvironment, model = robotModel()) { return model.mounts[environment]; }
export function jointsWithin(q: ArmAngles, model = robotModel()) {
  return q.length === model.joints.length && q.every((v, i) => Number.isFinite(v) && v >= model.joints[i].min - 1e-7 && v <= model.joints[i].max + 1e-7);
}
