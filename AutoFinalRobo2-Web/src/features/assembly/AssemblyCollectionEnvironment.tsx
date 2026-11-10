import { useEffect, useLayoutEffect, useMemo } from 'react';
import { Box3, GridHelper, Group, type Object3D, Vector3 } from 'three';
import { ease, segment } from './assemblyTimeline';
import { disposeRevealMaterials, revealMaterials, revealOpacity } from './assemblyMaterials';

// Reuse the actual 50 mm cube GLB, with deliberate clearances around the robot.
const POSITIONS = [[-.30, -.23, .12], [.29, -.24, -.3], [-.34, .25, -.18], [.31, .28, .45], [.08, .39, -.1]];

export function AssemblyCollectionEnvironment({ source, clock, reduced, floorY, target }: {
  source: Object3D; clock: number; reduced: boolean; floorY: number; target: Vector3;
}) {
  const props = useMemo(() => POSITIONS.map(([x, z, yaw]) => {
    const root = source.clone(true), pivot = new Group();
    const box = new Box3().setFromObject(root, true), center = box.getCenter(new Vector3());
    root.position.sub(center); root.position.y += box.getSize(new Vector3()).y / 2;
    pivot.add(root); pivot.position.set(x, floorY, z); pivot.rotation.y = yaw;
    return { pivot, materials: revealMaterials(root), y: floorY };
  }), [source, floorY]);
  const grid = useMemo(() => {
    const lines = new GridHelper(1.6, 32, '#7794a3', '#526b7a');
    const materials = Array.isArray(lines.material) ? lines.material : [lines.material];
    materials.forEach(material => { material.transparent = true; material.depthWrite = false; });
    return { lines, materials };
  }, []);
  useEffect(() => () => {
    props.forEach(prop => disposeRevealMaterials(prop.materials));
  }, [props]);
  useEffect(() => () => {
    grid.lines.geometry.dispose(); grid.materials.forEach(material => material.dispose());
  }, [grid]);
  const alpha = reduced ? Number(clock >= .87) : ease(segment(clock, .87, .90));
  useLayoutEffect(() => {
    grid.materials.forEach(material => { material.opacity = .24 * alpha; });
    props.forEach((prop, i) => {
      const reveal = reduced ? alpha : ease(segment(clock, .875 + i * .004, .905 + i * .004));
      prop.pivot.visible = reveal > 0;
      prop.pivot.position.y = prop.y + (reduced ? 0 : .025 * (1 - reveal));
      revealOpacity(prop.materials, .7 * reveal);
    });
  }, [clock, reduced, props, grid, alpha]);
  return <group visible={alpha > 0}>
    <primitive object={grid.lines} position={[0, floorY - .001, 0]} />
    <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, floorY - .002, 0]}>
      <planeGeometry args={[1.6, 1.6]} /><meshBasicMaterial color="#526b7a" transparent opacity={.055 * alpha} depthWrite={false} />
    </mesh>
    {props.map((prop, i) => <primitive key={i} object={prop.pivot} />)}
    {/* A quiet ground marker identifies the actual pickup cube, not a second box. */}
    <mesh rotation={[-Math.PI / 2, 0, 0]} position={[target.x, floorY, target.z]}>
      <ringGeometry args={[.038, .0395, 48]} />
      <meshBasicMaterial color="#85bac7" transparent opacity={.5 * alpha * (1 - ease(segment(clock, .969, .982)))} depthWrite={false} />
    </mesh>
  </group>;
}
