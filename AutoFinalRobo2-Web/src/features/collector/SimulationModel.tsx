import { useEffect, useMemo } from 'react';
import { useFrame, useThree } from '@react-three/fiber';
import { Box3, Group, PropertyBinding, Vector3, type Object3D } from 'three';
import { OrbitControls as ThreeOrbitControls } from 'three/addons/controls/OrbitControls.js';
import { useRobotModel } from '../../components/three/RobotModel';
import { ROBOT_WHEEL_KEYS, type RobotWheelValues } from '../../components/three/robotWheelMotion';
import { createHeadRig } from '../../components/three/robotHeadRig';

/** Simulation wheel poses; sensor shaft binding is shared with Control viewers.
 * Orbit gestures remain local to the simulator. */
function buildRig(model: Object3D) {
  const wheels: Object3D[] = [], motors: Vector3[] = [];
  const starts = (node: Object3D, prefix: string) => node.name.startsWith(PropertyBinding.sanitizeNodeName(prefix));
  model.updateMatrixWorld(true);
  model.traverse(node => {
    if (starts(node, 'AFR2_Rueda80_Mano')) wheels.push(node);
    if (starts(node, 'AFR2_GM25_370')) motors.push(node.getWorldPosition(new Vector3()));
  });
  const pivots: Partial<Record<keyof RobotWheelValues, Group>> = {};
  for (const wheel of wheels) {
    const centre = new Box3().setFromObject(wheel, true).getCenter(new Vector3());
    const axis = motors.find(m => (m.x < 0) === (centre.x < 0) && (m.z < 0) === (centre.z < 0));
    if (!axis) continue;
    const key = `${centre.z < 0 ? 'f' : 'r'}${centre.x < 0 ? 'l' : 'r'}` as keyof RobotWheelValues;
    const pivot = new Group();
    pivot.position.copy(model.worldToLocal(new Vector3(centre.x, axis.y, axis.z)));
    model.add(pivot); model.updateMatrixWorld(true); pivot.attach(wheel); pivots[key] = pivot;
  }
  const headPivot = createHeadRig(model);
  return { model, pivots, headPivot };
}
export function SimulationRobot({ wheelAngles, angle }: { wheelAngles: RobotWheelValues; angle: number }) {
  const { scene, offset } = useRobotModel();
  const rig = useMemo(() => buildRig(scene.clone(true)), [scene]);
  const { invalidate } = useThree();
  useEffect(() => {
    for (const key of ROBOT_WHEEL_KEYS) if (rig.pivots[key]) rig.pivots[key]!.rotation.x = -wheelAngles[key];
    if (rig.headPivot) rig.headPivot.rotation.y = angle * Math.PI / 180;
    invalidate();
  }, [rig, wheelAngles, angle, invalidate]);
  return <group position={offset} dispose={null}><primitive object={rig.model} /></group>;
}
export function SimulationOrbit({ enabled, focus }: { enabled: boolean; focus: [number, number, number] }) {
  const { camera, gl, invalidate } = useThree();
  const controls = useMemo(() => new ThreeOrbitControls(camera), [camera]);
  useFrame(() => { if (controls.enabled) controls.update(); });
  useEffect(() => {
    controls.connect(gl.domElement); controls.enableDamping = true;
    const changed = () => invalidate();
    controls.addEventListener('change', changed);
    return () => { controls.removeEventListener('change', changed); controls.dispose(); };
  }, [controls, gl, invalidate]);
  useEffect(() => { controls.target.set(...focus); controls.update(); invalidate(); }, [controls, focus, invalidate]);
  useEffect(() => { controls.enabled = enabled; invalidate(); }, [controls, enabled, invalidate]);
  return null;
}
