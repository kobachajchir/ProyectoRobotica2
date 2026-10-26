import { useEffect, useMemo, useRef, useState } from 'react';
import { useFrame, useLoader, useThree } from '@react-three/fiber';
import { advanceServo } from './servoMotion';
import { useConnection } from '../../features/connection/ConnectionContext';
import { Box3, Group, Vector3, type Object3D } from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { DRACOLoader } from 'three/addons/loaders/DRACOLoader.js';
import { MODEL_WHEEL_RADIUS_MM, ROBOT_WHEEL_KEYS, type RobotWheelValues } from './robotWheelMotion';

// AFR2-20261024-06. One cached asset for inspection and all Control cameras.
// glTF coordinates are meters. Never normalize the mesh to a display size.
const MODEL_URL = '/models/AutoFinalRobo2.glb';
const draco = new DRACOLoader().setDecoderPath('/draco/').setDecoderConfig({ type: 'wasm' });

const frames = new WeakMap<Object3D, { size: Vector3; offset: Vector3 }>();

export function useRobotModel() {
  const { scene } = useLoader(GLTFLoader, MODEL_URL, loader => loader.setDRACOLoader(draco));
  const framing = useMemo(() => {
    const cached = frames.get(scene);if (cached) return cached;
    // Rotated CAD parts have conservative local boxes. Only actual vertices
    // locate the wheel contact plane; transformed box corners can lie below it.
    const box = new Box3().setFromObject(scene, true);
    const center = box.getCenter(new Vector3());
    const frame = {
      size: box.getSize(new Vector3()),
      // Center only X/Z; the lowest point rests on the ground at Y=0.
      offset: new Vector3(-center.x, -box.min.y, -center.z),
    };
    frames.set(scene, frame);return frame;
  }, [scene]);
  return { scene, ...framing };
}

/** Clone-only rig: rotate each wheel around its motor axle, never its CAD origin.
 * The final export has forward=-Z, left=-X and the four GM25 axes parallel to X.
 * Attaching preserves all original transforms/geometry/materials and scale.
 */
function createWheelRig(model: Object3D) {
  const wheels: Object3D[] = [], motors: Vector3[] = [];
  model.updateMatrixWorld(true);
  model.traverse(node => {
    if (node.name.startsWith('AFR2_Rueda80_Mano')) wheels.push(node);
    if (node.name.startsWith('AFR2_GM25_370')) motors.push(node.getWorldPosition(new Vector3()));
  });
  const pivots: Partial<Record<keyof RobotWheelValues, Group>> = {};
  for (const wheel of wheels) {
    const center = new Box3().setFromObject(wheel, true).getCenter(new Vector3());
    const motor = motors.find(axis => (axis.x < 0) === (center.x < 0) && (axis.z < 0) === (center.z < 0));
    if (!motor) continue;
    const key = `${center.z < 0 ? 'f' : 'r'}${center.x < 0 ? 'l' : 'r'}` as keyof RobotWheelValues;
    const pivot = new Group();
    pivot.position.copy(model.worldToLocal(new Vector3(center.x, motor.y, motor.z)));
    model.add(pivot); model.updateMatrixWorld(true); pivot.attach(wheel);
    pivots[key] = pivot;
  }
  return pivots;
}

/** Clone-only head pivot. SG90 body remains fixed; HC-SR04 and bracket rotate.
 * The shaft X follows the sensor center, Z the supplied SG90 CAD origin.
 * This visual reference is inferred from the current asset, not metrology. */
function createServoRig(model: Object3D) {
  let servo: Object3D|undefined, sensor: Object3D|undefined;const movable:Object3D[]=[];
  model.traverse(node=>{if(node.name.startsWith('AFR2_SG90:'))servo=node;
    if(node.name.startsWith('AFR2_HC_SR04:')){sensor=node;movable.push(node);}
    if(node.name.startsWith('AFR2_Soporte_HCSR04_Referencia:'))movable.push(node);});
  if(!servo || !sensor)return null;
  model.updateMatrixWorld(true);const origin=servo.getWorldPosition(new Vector3()),head=sensor.getWorldPosition(new Vector3());
  const box=new Box3().setFromObject(servo,true),pivot=new Group();
  pivot.position.copy(model.worldToLocal(new Vector3(head.x,box.max.y,origin.z)));model.add(pivot);model.updateMatrixWorld(true);
  for(const part of movable)pivot.attach(part);return pivot;
}
export function RobotModel({ wheelAngles, wheelSpeedsMmS, wheelAngleOffsets, animateServo=true }: {
  wheelAngles?: RobotWheelValues; wheelSpeedsMmS?: RobotWheelValues; wheelAngleOffsets?: RobotWheelValues; animateServo?: boolean;
}) {
  const { scene, offset } = useRobotModel();
  const { headReading }=useConnection();
  const servoTarget=useRef(0),servoVelocity=useRef(0);
  const { model, pivots, servoPivot } = useMemo(() => {
    const instance = scene.clone(true);
    return { model: instance, pivots: createWheelRig(instance), servoPivot:animateServo?createServoRig(instance):null };
  }, [scene,animateServo]);
  const { invalidate } = useThree();
  const appliedOffsets = useMemo<RobotWheelValues>(() => ({ fl: 0, fr: 0, rl: 0, rr: 0 }), [pivots]);
  const firstFrame = useRef(true);
  const [reduceMotion, setReduceMotion] = useState(() => matchMedia('(prefers-reduced-motion: reduce)').matches);
  useEffect(() => {
    const query = matchMedia('(prefers-reduced-motion: reduce)');
    const change = () => setReduceMotion(query.matches);
    query.addEventListener('change', change); return () => query.removeEventListener('change', change);
  }, []);
  useEffect(() => {
    for (const key of ROBOT_WHEEL_KEYS) {
      if (pivots[key] && (reduceMotion || wheelAngles)) pivots[key].rotation.x = reduceMotion ? 0 : -(wheelAngles![key] % (Math.PI * 2));
    }
    invalidate();
  }, [wheelAngles, pivots, reduceMotion, invalidate]);
  useEffect(() => {
    // Add gesture offsets to the existing phase instead of snapping wheels
    // back to zero when switching between translation and local pivoting.
    for (const key of ROBOT_WHEEL_KEYS) {
      const angle = wheelAngleOffsets?.[key] ?? 0;
      const pivot = pivots[key];
      if (pivot && !reduceMotion) pivot.rotation.x -= (angle - appliedOffsets[key]) % (Math.PI * 2);
      appliedOffsets[key] = angle;
    }
    invalidate();
  }, [wheelAngleOffsets, appliedOffsets, pivots, reduceMotion, invalidate]);
  useEffect(()=>{
    if(!animateServo || !headReading || Date.now()-headReading.receivedAtMs>1000)return;
    if(!headReading.enabled || !headReading.pulseUs){if(servoPivot)servoTarget.current=servoPivot.rotation.y;servoVelocity.current=0;return;}
    servoTarget.current=(headReading.angleDeg-90)*Math.PI/180;
    if(servoPivot && reduceMotion)servoPivot.rotation.y=servoTarget.current;invalidate();
  },[animateServo,headReading,servoPivot,reduceMotion,invalidate]);
  useFrame((_,delta)=>{
    if(!animateServo || !servoPivot)return;const difference=servoTarget.current-servoPivot.rotation.y;
    if(Math.abs(difference)<.0001)return;
    if(reduceMotion){servoPivot.rotation.y=servoTarget.current;servoVelocity.current=0;}
    else{const [value,velocity]=advanceServo(servoPivot.rotation.y,servoVelocity.current,servoTarget.current,delta);servoPivot.rotation.y=value;servoVelocity.current=velocity;}
    invalidate();
  });
  const rolling = !reduceMotion && !!wheelSpeedsMmS && ROBOT_WHEEL_KEYS.some(key => wheelSpeedsMmS[key] !== 0);
  useEffect(() => { firstFrame.current = true; if (rolling) invalidate(); }, [wheelSpeedsMmS, rolling, invalidate]);
  useFrame((_, delta) => {
    if (!rolling || !wheelSpeedsMmS) return;
    // Never integrate the idle time before a gesture or catch up after a stall.
    if (firstFrame.current) firstFrame.current = false;
    else for (const key of ROBOT_WHEEL_KEYS) {
      const pivot = pivots[key];
      if (pivot) pivot.rotation.x = (pivot.rotation.x - wheelSpeedsMmS[key] / MODEL_WHEEL_RADIUS_MM * Math.min(delta, .05)) % (Math.PI * 2);
    }
    invalidate();
  });
  // Instances have independent transforms but share cached geometry/materials.
  return <group position={offset} dispose={null}><primitive object={model} /></group>;
}

export function clearRobotModelCache() {
  useLoader.clear(GLTFLoader, MODEL_URL);
}
