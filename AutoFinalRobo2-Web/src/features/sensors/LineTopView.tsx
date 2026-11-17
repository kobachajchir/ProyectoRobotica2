/* AFR2-20261113-02. Línea IR sobre el modelo 3D del auto visto desde arriba (frente hacia arriba).
 * La barra de sensores sale del nodo AFR2_SensorLinea_5IR del CAD: está bajo el chasis, así que los cinco IR y la línea
 * se dibujan encima (sin prueba de profundidad). AFR2-20261113-03: la línea detectada es una cinta negra en el piso, bajo el
 * modelo, de ancho configurable (18 mm por defecto), sobre una pista clara; asoma delante y detrás del auto. */
import { Suspense, useEffect, useMemo, useState } from 'react';
import { useThree } from '@react-three/fiber';
import { Box3, PropertyBinding, Vector3 } from 'three';
import { SimulationViewport } from '../../components/three/SimulationViewport';
import { ModelSpinner } from '../../components/three/ModelSpinner';
import { useRobotModel } from '../../components/three/RobotModel';
import { SimulationRobot } from '../collector/SimulationModel';
import type { LineAnalysis } from './lineAnalysis';

const ZERO_WHEELS = { fl: 0, fr: 0, rl: 0, rr: 0 };
/** AFR2-20261113-03: ancho de cinta por defecto (cinta aisladora negra común). */
export const LINE_TAPE_DEFAULT_MM = 18;
export const LINE_TAPE_MIN_MM = 5;
export const LINE_TAPE_MAX_MM = 60;

function readColors() {
  const css = getComputedStyle(document.documentElement), get = (k: string) => css.getPropertyValue(k).trim();
  return { accent: get('--action') || '#ff5c7a', idle: get('--text-muted') || '#8a94a6', plate: get('--surface-solid') || '#111827', tape: '#0b0b0d', track: '#e9ecf1' };
}

function TopCamera({ z }: { z: number }) {
  const { camera, invalidate } = useThree();
  useEffect(() => {
    camera.position.set(0, 5, z); camera.up.set(0, 0, -1); camera.lookAt(0, 0, z);
    camera.updateProjectionMatrix(); invalidate();
  }, [camera, invalidate, z]);
  return null;
}

/** Barra de sensores en el marco del auto (X izquierda −, Z frente −), con el mismo desplazamiento que SimulationRobot. */
export function useSensorBar() {
  const { scene, offset, size } = useRobotModel();
  return useMemo(() => {
    scene.updateMatrixWorld(true);
    const prefix = PropertyBinding.sanitizeNodeName('AFR2_SensorLinea_5IR:');
    let part: import('three').Object3D | undefined;
    scene.traverse(node => { if (!part && node.name.startsWith(prefix)) part = node; });
    const box = part ? new Box3().setFromObject(part, true) : null;
    const center = box ? box.getCenter(new Vector3()).add(offset) : new Vector3(0, 0, -size.z * .38);
    const width = box ? box.max.x - box.min.x : size.x * .5;
    const result = { x: center.x, z: center.z, spacing: width / 5, top: size.y + .01, length: size.z };
    return result;
  }, [scene, offset, size]);
}

function LineMarks({ line, tapeMm }: { line: LineAnalysis | null; tapeMm: number }) {
  const bar = useSensorBar();
  const [colors] = useState(readColors);
  const { invalidate } = useThree();
  useEffect(() => { invalidate(); }, [line, tapeMm, invalidate]);
  const tapeM = Math.max(LINE_TAPE_MIN_MM, Math.min(LINE_TAPE_MAX_MM, tapeMm)) / 1000;
  const detected = !!line?.detected;
  // Línea: + posición = a la izquierda (−X). Inclinada a la izquierda = gira antihorario (+Y) alrededor de la barra.
  const offsetX = line?.position != null ? -line.position * bar.spacing : 0;
  const tilt = line?.orientation === 'tilt-left' ? .32 : line?.orientation === 'tilt-right' ? -.32 : 0;
  const tapeLength = bar.length * 2.2;
  // AFR2-20261113-03: cinta negra en el piso, debajo del modelo (asoma delante y detrás), sobre una pista clara.
  return <>
    <mesh position={[bar.x, .0004, bar.z * .4]} rotation={[-Math.PI / 2, 0, 0]}>
      <planeGeometry args={[1.2, 1.2]} /><meshBasicMaterial color={colors.track} />
    </mesh>
    {detected && <group position={[line!.horizontal ? bar.x : bar.x + offsetX, .001, bar.z]} rotation={[0, line!.horizontal ? 0 : tilt, 0]}>
      <mesh rotation={[-Math.PI / 2, 0, 0]}>
        <planeGeometry args={line!.horizontal ? [tapeLength, tapeM] : [tapeM, tapeLength]} />
        <meshBasicMaterial color={colors.tape} />
      </mesh>
    </group>}
    {/* Placa de fondo para que los IR se lean sobre el chasis. */}
    <mesh position={[bar.x, bar.top + .001, bar.z]} rotation={[-Math.PI / 2, 0, 0]} renderOrder={5}>
      <planeGeometry args={[bar.spacing * 5.4, bar.spacing * 1.3]} /><meshBasicMaterial color={colors.plate} transparent opacity={.8} depthTest={false} depthWrite={false} />
    </mesh>
    {[0, 1, 2, 3, 4].map(i => {
      const on = !!line && !!(line.bits & (1 << i));
      return <group key={i} position={[bar.x + (i - 2) * bar.spacing, bar.top + .002, bar.z]} rotation={[-Math.PI / 2, 0, 0]}>
        <mesh renderOrder={6}><ringGeometry args={[bar.spacing * .3, bar.spacing * .44, 24]} /><meshBasicMaterial color={on ? colors.accent : colors.idle} depthTest={false} transparent /></mesh>
        {on && <mesh renderOrder={7}><circleGeometry args={[bar.spacing * .28, 20]} /><meshBasicMaterial color={colors.accent} depthTest={false} transparent /></mesh>}
      </group>;
    })}
  </>;
}

function Scene({ line, tapeMm }: { line: LineAnalysis | null; tapeMm: number }) {
  const bar = useSensorBar();
  return <>
    <TopCamera z={bar.z * .4} />
    <SimulationRobot wheelAngles={ZERO_WHEELS} angle={0} />
    <LineMarks line={line} tapeMm={tapeMm} />
  </>;
}

export function LineTopView({ line, label, tapeMm = LINE_TAPE_DEFAULT_MM }: { line: LineAnalysis | null; label: string; tapeMm?: number }) {
  return <div className="afr2-line-top" role="img" aria-label={label}>
    <SimulationViewport floorSize={1.2} camera={{ position: [0, 5, 0], zoom: 660, near: .001, far: 50 }}
      fallback={<p className="afr2-help">Sin 3D en este dispositivo.</p>}
      overlay={<><span className="afr2-line-top__tag" data-side="left">IR1 · izq.</span><span className="afr2-line-top__tag" data-side="right">der. · IR5</span></>}>
      <Suspense fallback={null}><Scene line={line} tapeMm={tapeMm} /></Suspense>
    </SimulationViewport>
    <Suspense fallback={<span className="afr2-line-top__loading"><ModelSpinner size="sm" label="" /></span>}><ModelReady /></Suspense>
  </div>;
}
/** Muestra el spinner hasta que el modelo está descargado (mismo caché que la escena). */
function ModelReady() { useRobotModel(); return null; }
