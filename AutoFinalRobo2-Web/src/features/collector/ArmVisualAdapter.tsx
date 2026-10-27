import { useEffect, useMemo } from 'react';
import { useLoader } from '@react-three/fiber';
import { Group, Matrix4, Object3D, PropertyBinding, Quaternion, Vector3 } from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { DRACOLoader } from 'three/addons/loaders/DRACOLoader.js';
import { forwardModel, rigid } from './modelKinematics';
import { identity, modelProblems, type ArmAngles, type ArmEnvironment, type Point3, type RobotModelDefinition } from './robotModel';

/** A link between arbitrary joint origins, with no hard-coded link length. */
export function ArmLink({ offset, radius, color, ghost = false }: { offset: Point3; radius: number; color: string; ghost?: boolean }) {
  const vector = new Vector3(...offset), length = vector.length();
  if (length < 1e-6) return null;
  const orientation = new Quaternion().setFromUnitVectors(new Vector3(0, 1, 0), vector.clone().normalize());
  return <mesh position={vector.multiplyScalar(.5)} quaternion={orientation} raycast={ghost ? () => undefined : undefined}>
    <cylinderGeometry args={[radius, radius, length, 16]} /><meshStandardMaterial color={color} transparent={ghost} opacity={ghost ? .2 : 1} depthWrite={!ghost} roughness={.6} />
  </mesh>;
}
export function ProceduralGripper({ model, opening, color, ghost = false }: { model: RobotModelDefinition; opening: number; color: string; ghost?: boolean }) {
  const grip = model.gripper;
  const material = { color, transparent: ghost, opacity: ghost ? .2 : 1, depthWrite: !ghost, roughness: .5 };
  return <><mesh position={[0, 0, grip.palmZ]}><boxGeometry args={grip.palmSize} /><meshStandardMaterial {...material} /></mesh>
    {[-1, 1].map(side => <group key={side}>
      <mesh position={grip.travelAxis.map((v, i) => side * v * (opening * grip.metersPerOpeningMm + grip.fingerSize[0] / 2) + (i === 2 ? grip.fingerZ : 0)) as Point3}>
        <boxGeometry args={grip.fingerSize} /><meshStandardMaterial {...material} /></mesh>
      <mesh position={grip.travelAxis.map((v, i) => side * v * (opening * grip.metersPerOpeningMm + grip.padSize[0] / 2) + (i === 2 ? grip.padZ : 0)) as Point3}>
        <boxGeometry args={grip.padSize} /><meshStandardMaterial {...material} /></mesh>
    </group>)}
  </>;
}
const draco = new DRACOLoader().setDecoderPath('/draco/').setDecoderConfig({ type: 'wasm' });
/** GLB nodes bind by descriptor name. Rest geometry is transformed once into the
 * mounting frame; each mapped subtree then follows its own kinematic frame. */
export function ArticulatedArmAsset({ model, q, opening, environment, onIssue }: {
  model: RobotModelDefinition; q: ArmAngles; opening: number; environment: ArmEnvironment; onIssue: (issue: string | null) => void;
}) {
  const asset = useLoader(GLTFLoader, model.visual.url!, loader => loader.setDRACOLoader(draco));
  const prepared = useMemo(() => {
    const root = new Group(), scene = asset.scene.clone(true); root.add(scene);
    scene.applyMatrix4(rigid(model.visual.restToMount ?? identity())); root.updateMatrixWorld(true);
    const rest = forwardModel(model.visual.restAngles ?? model.joints.map(() => 0), model);
    const definitions = [...model.joints.map((j, index) => ({ name: j.visualNode, index, side: 0 })),
      ...model.gripper.visualNodes.map((name, index) => ({ name, index: model.joints.length - 1, side: index ? 1 : -1 }))];
    const bindings: { node: Object3D; rest: Matrix4; index: number; side: number }[] = [];
    let issue: string | null = modelProblems(model).join(' ') || null;
    for (const definition of definitions) {
      if (issue) break;
      const matches: Object3D[] = []; scene.traverse(node => { if (node.name === definition.name || node.name === PropertyBinding.sanitizeNodeName(definition.name)) matches.push(node); });
      if (matches.length !== 1 || bindings.some(b => b.node === matches[0])) { issue = `Nodo del rig ausente o ambiguo: ${definition.name}. Se muestra la malla sin habilitar movimiento.`; break; }
      bindings.push({ node: matches[0], rest: matches[0].matrixWorld.clone(), index: definition.index, side: definition.side });
    }
    if (!issue) for (const binding of [...bindings].reverse()) root.attach(binding.node);
    return { root, bindings, issue, rest };
  }, [asset.scene, model]);
  useEffect(() => { onIssue(prepared.issue); }, [prepared, onIssue]);
  if (!prepared.issue) {
    const current = forwardModel(q, model);
    for (const binding of prepared.bindings) {
      const move = current.frames[binding.index].clone();
      if (binding.side) move.multiply(new Matrix4().makeTranslation(...model.gripper.travelAxis.map(v => v * binding.side * (opening - (model.visual.restOpening ?? model.gripper.open)) * model.gripper.metersPerOpeningMm) as Point3));
      binding.node.matrixAutoUpdate = false;
      binding.node.matrix.copy(move.multiply(prepared.rest.frames[binding.index].clone().invert()).multiply(binding.rest));
      binding.node.matrixWorldNeedsUpdate = true;
    }
  }
  return <group matrix={rigid(model.mounts[environment])} matrixAutoUpdate={false}><primitive object={prepared.root} /></group>;
}
