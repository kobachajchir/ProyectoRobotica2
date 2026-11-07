import { useEffect, useMemo } from 'react';
import { Canvas, useThree } from '@react-three/fiber';
import { RobotModel, useRobotModel } from '../../../components/three/RobotModel';
import { MODEL_WHEEL_RADIUS_MM, type RobotWheelValues } from '../../../components/three/robotWheelMotion';
import { DEFAULT_KINEMATIC_PROFILE } from '../sequences/sequenceTypes';

/** AFR2-20261024-13. Shared final asset, framed by camera; no mesh scaling or pose. */
function FloatingRobot({ onReady, wheelSpeeds, headingRad }: { onReady: (ready: boolean) => void; wheelSpeeds?: RobotWheelValues; headingRad: number }) {
  const { size: extent, scene } = useRobotModel();
  const { camera, size, invalidate, gl } = useThree();
  const turnOffsets = useMemo(() => {
    // AFR2-20261106-07: physical CCW turn = left wheels back, right forward (no wiring sign).
    const angle = headingRad * DEFAULT_KINEMATIC_PROFILE.lxPlusLyMm / MODEL_WHEEL_RADIUS_MM;
    return { fl: -angle, fr: angle, rl: -angle, rr: angle };
  }, [headingRad]);
  useEffect(() => { invalidate(); }, [headingRad, invalidate]);
  useEffect(() => {
    // True overhead view: the sensor/front (-Z in the asset) points upward.
    camera.position.set(0, Math.max(1, extent.y * 4), 0);
    camera.up.set(0, 0, -1); camera.lookAt(0, 0, 0);
    // Leave room for the broad ribbons to extend beyond the chassis silhouette.
    camera.zoom = Math.min(size.width * .60 / Math.max(extent.x, .01), size.height * .60 / Math.max(extent.z, .01));
    camera.updateProjectionMatrix(); invalidate();
  }, [camera, size.width, size.height, extent, invalidate]);
  useEffect(() => {
    const lost = () => onReady(false), restored = () => { invalidate(); onReady(true); };
    gl.domElement.addEventListener('webglcontextlost', lost); gl.domElement.addEventListener('webglcontextrestored', restored);
    onReady(true);
    return () => { onReady(false); gl.domElement.removeEventListener('webglcontextlost', lost); gl.domElement.removeEventListener('webglcontextrestored', restored); };
  }, [onReady, gl, invalidate, scene]);
  // AFR2-20261106-33: el chasis queda fijo; headingRad solo hace rodar las ruedas del giro ordenado.
  return <group position={[0, .03, 0]} rotation={[0, 0, 0]}><RobotModel wheelSpeedsMmS={wheelSpeeds} wheelAngleOffsets={turnOffsets} /></group>;
}

export function ManualDriveModel({ onReady, wheelSpeeds, headingRad = 0 }: { onReady: (ready: boolean) => void; wheelSpeeds?: RobotWheelValues; headingRad?: number }) {
  // R3F mounts fallback as native <canvas> children even when WebGL works.
  // It must be passive content; actual failures belong to RenderBoundary.
  return <Canvas orthographic frameloop="demand" dpr={[1, 1.5]} camera={{ near: .001, far: 100 }}
    gl={{ alpha: true }} style={{ pointerEvents: 'none' }} fallback={<p className="afr2-manual-model-message">Modelo 3D no disponible.</p>}>
    <ambientLight intensity={1.7} /><directionalLight position={[2, 4, 3]} intensity={2} />
    <FloatingRobot onReady={onReady} wheelSpeeds={wheelSpeeds} headingRad={headingRad} />
  </Canvas>;
}
