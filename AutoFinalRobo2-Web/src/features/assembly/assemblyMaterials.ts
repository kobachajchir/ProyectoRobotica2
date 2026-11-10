import { Mesh, type Material, type Object3D } from 'three';
import { clamp01 } from './assemblyTimeline';

/** Own only the presentation's materials, keep shared CAD textures/geometry.
 * A separate set per rigid part lets equal source materials fade independently. */
export function revealMaterials(root: Object3D) {
  const copies = new Map<Material, Material>();
  root.traverse(node => {
    if (!(node instanceof Mesh)) return;
    const copy = (source: Material) => {
      let material = copies.get(source);
      if (!material) {
        material = source.clone(); material.transparent = true;
        copies.set(source, material);
      }
      return material;
    };
    node.material = Array.isArray(node.material) ? node.material.map(copy) : copy(node.material);
  });
  return [...copies].map(([source, material]) => ({ source, material }));
}

export function revealOpacity(materials: ReturnType<typeof revealMaterials>, opacity: number) {
  const alpha = clamp01(opacity);
  for (const { source, material } of materials) {
    material.opacity = source.opacity * alpha;
    material.depthWrite = source.depthWrite && alpha >= .99;
  }
}

export function disposeRevealMaterials(materials: ReturnType<typeof revealMaterials>) {
  for (const { material } of materials) material.dispose();
}
