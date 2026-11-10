import { Group, Matrix4, Mesh, Object3D, PropertyBinding, Vector3 } from 'three';
import { forwardModel, rad, rigid } from '../collector/modelKinematics';
import { PVC25_V17, type ArmAngles, type VisualFollower } from '../collector/robotModel';
import { ARM_ASSEMBLY_STAGES, ease, segment } from './assemblyTimeline';
import { revealMaterials, revealOpacity } from './assemblyMaterials';

const model = PVC25_V17;
export const ARM_TO_SCENE = rigid({ position: [0, 0, 0], quaternion: model.renderBasis }).multiply(rigid(model.mounts.mounted));

/** Same descriptor binding as ArticulatedArmAsset, with presentation offsets.
 * This independent clone never alters the simulator's state, rig or GLB cache. */
function followerFrame(f: VisualFollower, frames: Matrix4[], q: ArmAngles, opening: number) {
  const angle = rad(q[f.driverIndex] * f.ratio) + (opening - model.visual.restOpening!) * (f.gripperRadiansPerMm ?? 0);
  return frames[f.parentIndex].clone().multiply(rigid(f.origin)).multiply(new Matrix4().makeRotationAxis(new Vector3(...f.axis), angle));
}

export function prepareAssemblyArm(source: Object3D) {
  const root = new Group(), scene = source.clone(true); root.add(scene);
  scene.traverse(node => { if (node instanceof Mesh) node.castShadow = true; });
  scene.applyMatrix4(rigid(model.visual.restToMount!)); root.updateMatrixWorld(true);
  const restFrames = forwardModel(model.visual.restAngles!, model).frames;
  const definitions = [
    { name: 'base-fixed', index: -1, side: 0, follower: undefined as VisualFollower | undefined },
    ...model.joints.map((joint, index) => ({ name: joint.visualNode, index, side: 0, follower: undefined as VisualFollower | undefined })),
    ...model.gripper.visualNodes.map((name, i) => ({ name, index: model.joints.length - 1, side: i ? 1 : -1, follower: undefined as VisualFollower | undefined })),
    ...(model.visual.followers ?? []).map(follower => ({ name: follower.visualNode, index: follower.parentIndex, side: 0, follower })),
  ];
  const bindings = definitions.map(definition => {
    const matches: Object3D[] = [];
    scene.traverse(node => { if (node.name === PropertyBinding.sanitizeNodeName(definition.name)) matches.push(node); });
    if (matches.length !== 1) throw new Error(`No se encontró el conjunto ${definition.name} en el brazo V17.`);
    const frame = definition.index < 0 ? new Matrix4() : definition.follower
      ? followerFrame(definition.follower, restFrames, model.visual.restAngles!, model.visual.restOpening!)
      : restFrames[definition.index].clone();
    return { ...definition, node: matches[0], rest: matches[0].matrixWorld.clone(), inverse: frame.invert(), materials: revealMaterials(matches[0]) };
  });
  for (const binding of [...bindings].reverse()) root.attach(binding.node);
  return { root, bindings };
}

export function poseAssemblyArm(rig: ReturnType<typeof prepareAssemblyArm>, q: ArmAngles, opening: number, progress: number, reduced: boolean) {
  const current = forwardModel(q, model);
  for (const binding of rig.bindings) {
    const [from, to] = ARM_ASSEMBLY_STAGES[binding.name];
    const phase = segment(progress, from, to), t = reduced ? (phase >= .5 ? 1 : 0) : ease(phase);
    const frame = binding.index < 0 ? new Matrix4() : binding.follower
      ? followerFrame(binding.follower, current.frames, q, opening) : current.frames[binding.index].clone();
    if (binding.side) frame.multiply(new Matrix4().makeTranslation(...model.gripper.travelAxis.map(v => v * binding.side * (opening - model.visual.restOpening!) * model.gripper.metersPerOpeningMm) as [number, number, number]));
    const translation = new Matrix4().makeTranslation(binding.side * .05 * (1 - t), 0, .25 * (1 - t));
    binding.node.visible = reduced ? phase >= .5 : progress >= from;
    revealOpacity(binding.materials, reduced ? (phase >= .5 ? 1 : 0) : ease(segment(phase, 0, .65)));
    binding.node.matrixAutoUpdate = false;
    binding.node.matrix.copy(translation.multiply(frame).multiply(binding.inverse).multiply(binding.rest));
    binding.node.matrixWorldNeedsUpdate = true;
  }
}

export function toolInScene(q: ArmAngles) {
  return ARM_TO_SCENE.clone().multiply(forwardModel(q, model).tool);
}
