import { useEffect, useMemo } from 'react';
import { useFrame, useThree } from '@react-three/fiber';
import { OrbitControls as ThreeOrbitControls } from 'three/addons/controls/OrbitControls.js';

/** Three's installed addon: shared by both reference viewers. */
export function OrbitControls({ targetY = 0 }: { targetY?: number }) {
  const { camera, gl, invalidate } = useThree();
  const controls = useMemo(() => new ThreeOrbitControls(camera), [camera]);
  useFrame(() => controls.update());
  useEffect(() => {
    controls.connect(gl.domElement);
    controls.enableDamping = true;
    controls.target.set(0, targetY, 0);
    controls.update();
    const onChange = () => invalidate();
    controls.addEventListener('change', onChange);
    return () => { controls.removeEventListener('change', onChange); controls.dispose(); };
  }, [controls, gl, invalidate, targetY]);
  return null;
}
