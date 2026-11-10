import { DoubleSide } from 'three';
import { toThree } from './armKinematics';
import type { CollectorState } from './collectorSimulation';

export function CollectionDropZone({ state, color }: { state: CollectorState; color: string }) {
  if (!state.drop.visible) return null;
  const { radius, center } = state.drop;
  return <group position={toThree(center)}>
    <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, .002, 0]}>
      <circleGeometry args={[radius, 64]} />
      <meshBasicMaterial color={color} transparent opacity={.12} depthWrite={false} />
    </mesh>
    <mesh position={[0, .08, 0]}>
      <cylinderGeometry args={[radius, radius, .16, 64, 1, true]} />
      <meshStandardMaterial color={color} emissive={color} emissiveIntensity={1.8} transparent opacity={.17} roughness={.15} side={DoubleSide} depthWrite={false} />
    </mesh>
    {[radius, radius * .77].map((r, i) => <mesh key={i} rotation={[-Math.PI / 2, 0, 0]} position={[0, .004, 0]}>
      <ringGeometry args={[r - .004, r, 64]} />
      <meshBasicMaterial color={color} transparent opacity={i ? .35 : .95} depthWrite={false} />
    </mesh>)}
    <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, .16, 0]}>
      <ringGeometry args={[radius - .002, radius, 64]} /><meshBasicMaterial color={color} transparent opacity={.65} side={DoubleSide} depthWrite={false} />
    </mesh>
    <pointLight color={color} intensity={.18} distance={.7} decay={2} position={[0, .08, 0]} />
  </group>;
}
