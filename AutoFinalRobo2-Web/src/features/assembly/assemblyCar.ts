import { Box3, BufferGeometry, Float32BufferAttribute, Group, Mesh, PropertyBinding, Quaternion, Vector3, type Object3D } from 'three';
import { ease, segment } from './assemblyTimeline';
import { revealMaterials, revealOpacity } from './assemblyMaterials';

interface CarPart {
  node: Object3D; position: Vector3; quaternion: Quaternion;
  from: number; to: number; offset: Vector3; screw: boolean; axis: Vector3;
  materials: ReturnType<typeof revealMaterials>;
}

/** Split only the clone's merged fasteners by connected CAD surfaces.
 * The asset's buffers and materials remain shared/read-only. Seam vertices are
 * joined by position (1µm); touching head/shank/nut islands stay together.
 * New buffers belong to this presentation and are disposed on unmount. */
function splitFasteners(source: Mesh, geometries: BufferGeometry[]) {
  const geometry = source.geometry, position = geometry.getAttribute('position');
  const index = geometry.getIndex();
  const count = index?.count ?? position.count;
  const parents = new Int32Array(position.count);
  const coordinates = new Map<string, number>();
  for (let i = 0; i < position.count; i++) {
    const key = `${Math.round(position.getX(i) * 1e6)},${Math.round(position.getY(i) * 1e6)},${Math.round(position.getZ(i) * 1e6)}`;
    parents[i] = coordinates.get(key) ?? i; coordinates.set(key, parents[i]);
  }
  const root = (vertex: number): number => {
    let id = vertex;
    while (parents[id] !== id) id = parents[id];
    while (parents[vertex] !== vertex) { const next = parents[vertex]; parents[vertex] = id; vertex = next; }
    return id;
  };
  const vertexAt = (i: number) => index ? index.getX(i) : i;
  for (let i = 0; i < count; i += 3) {
    const a = root(vertexAt(i));
    parents[root(vertexAt(i + 1))] = a; parents[root(vertexAt(i + 2))] = a;
  }
  const islands = new Map<number, { triangles: number[]; bounds: Box3 }>();
  const point = new Vector3();
  for (let i = 0; i < count; i += 3) {
    const key = root(vertexAt(i));
    let island = islands.get(key);
    if (!island) { island = { triangles: [], bounds: new Box3() }; islands.set(key, island); }
    island.triangles.push(i);
    for (let k = 0; k < 3; k++) island.bounds.expandByPoint(point.fromBufferAttribute(position, vertexAt(i + k)));
  }
  const clusters = [...islands.values()];
  // Heads and shafts may be separate watertight shells in the supplied export.
  for (let i = 0; i < clusters.length; i++) {
    for (let j = i + 1; j < clusters.length;) {
      if (clusters[i].bounds.clone().expandByScalar(.0002).intersectsBox(clusters[j].bounds)) {
        clusters[i].triangles.push(...clusters[j].triangles);
        clusters[i].bounds.union(clusters[j].bounds); clusters.splice(j, 1); j = i + 1;
      } else j++;
    }
  }
  const result = new Group(); result.name = `${source.name}_fasteners`;
  result.position.copy(source.position); result.quaternion.copy(source.quaternion); result.scale.copy(source.scale);
  for (const [i, cluster] of clusters.entries()) {
    const output = new BufferGeometry(), center = cluster.bounds.getCenter(new Vector3());
    for (const [name, attribute] of Object.entries(geometry.attributes)) {
      const data: number[] = [];
      for (const triangle of cluster.triangles) for (let k = 0; k < 3; k++) {
        const vertex = vertexAt(triangle + k);
        for (let c = 0; c < attribute.itemSize; c++) data.push(attribute.getComponent(vertex, c) - (name === 'position' ? center.getComponent(c) : 0));
      }
      output.setAttribute(name, new Float32BufferAttribute(data, attribute.itemSize));
    }
    cluster.triangles.forEach((triangle, face) => {
      const material = geometry.groups.find(group => triangle >= group.start && triangle < group.start + group.count)?.materialIndex ?? 0;
      const previous = output.groups[output.groups.length - 1];
      if (previous?.materialIndex === material) previous.count += 3;
      else output.addGroup(face * 3, 3, material);
    });
    output.computeBoundingBox(); output.computeBoundingSphere(); geometries.push(output);
    const mesh = new Mesh(output, source.material); mesh.name = `${source.name}_${i}`; mesh.position.copy(center);
    result.add(mesh);
  }
  return result;
}

export function prepareAssemblyCar(source: Object3D) {
  const model = source.clone(true), parts: CarPart[] = [], geometries: BufferGeometry[] = [];
  const units: { node: Object3D; name: string }[] = [];
  // One CAD occurrence stays rigid even when GLTFLoader represents its several
  // materials as child meshes. Only merged fastener collections are separated.
  for (const component of [...model.children]) {
    // Chapter7 owns these occurrences in its single lightweight GLB. Keep the
    // shared auto asset untouched; avoid showing duplicate panel/battery parts.
    if (/UI_B2|Voltimetro|Portabaterias|Celda_18650/.test(component.name)) { component.visible = false; continue; }
    if (!/Fijaciones_Placas|Prisioneros_Ruedas|Separadores_Montaje/.test(component.name)) {
      units.push({ node: component, name: component.name }); continue;
    }
    const meshes: Mesh[] = [];
    component.traverse(node => { if (node instanceof Mesh) meshes.push(node); });
    let index = 0;
    for (const node of meshes) {
      const replacement = splitFasteners(node, geometries);
      node.parent!.add(replacement); node.removeFromParent();
      for (const island of replacement.children) units.push({ node: island, name: `${component.name}_${index++}` });
    }
  }
  model.updateMatrixWorld(true);
  const lower = model.children.find(node => /CapaInferior/.test(node.name));
  const upper = model.children.find(node => /CapaSuperior/.test(node.name));
  const fixingBoundary = lower && upper
    ? (new Box3().setFromObject(lower, true).getCenter(new Vector3()).y + new Box3().setFromObject(upper, true).getCenter(new Vector3()).y) / 2
    : -Infinity;
  const starts = (name: string, prefix: string) => name.startsWith(PropertyBinding.sanitizeNodeName(`AFR2_${prefix}`));
  const counters = new Map<string, number>();
  for (const { node, name } of units) {
    node.traverse(child => { if (child instanceof Mesh) child.castShadow = true; });
    const center = new Box3().setFromObject(node, true).getCenter(new Vector3());
    const side = center.x < 0 ? -1 : 1;
    let from = .35, to = .425, direction = new Vector3(0, .46, 0), screw = false;
    if (starts(name, 'CapaInferior')) { from = 0; to = .065; direction.set(0, .18, 0); }
    else if (starts(name, 'Separadores')) { from = .07; to = .145; }
    else if (starts(name, 'Fijaciones')) {
      from = center.y < fixingBoundary ? .145 : .315; to = center.y < fixingBoundary ? .185 : .345;
      screw = true; direction.set(0, .18, 0);
    }
    else if (starts(name, 'Soportes_Motores')) { from = .19; to = .235; }
    else if (starts(name, 'GM25')) { from = .225; to = .275; direction.set(side * .2, .16, 0); }
    else if (starts(name, 'CapaSuperior')) { from = .28; to = .315; }
    else if (starts(name, 'Portabaterias')) { from = .35; to = .385; }
    else if (starts(name, 'Celda')) { from = .37; to = .41; }
    else if (starts(name, 'Mega')) { from = .35; to = .39; }
    else if (starts(name, 'OSOYOOMotorDriver')) { from = .37; to = .41; }
    else if (starts(name, 'OSOYOOWifi')) { from = .39; to = .43; }
    else if (starts(name, 'Voltimetro')) { from = .40; to = .435; }
    else if (starts(name, 'SensorLinea')) { from = .44; to = .475; direction.set(0, .18, -.15); }
    else if (starts(name, 'SG90')) { from = .45; to = .485; }
    else if (starts(name, 'Soporte_HCSR04')) { from = .465; to = .495; }
    else if (starts(name, 'HC_SR04')) { from = .48; to = .51; }
    else if (/UI_B2/.test(name)) { from = .49; to = .525; }
    else if (starts(name, 'Casquillos')) { from = .275; to = .2765; direction.set(0, .26, 0); }
    else if (starts(name, 'Rueda')) { from = .276; to = .279; direction.set(side * .35, .025, 0); }
    else if (starts(name, 'Prisioneros')) { from = .279; to = .28; screw = true; direction.set(side * .10, .04, 0); }
    const family = name.replace(/_\d+$/, '');
    const rank = counters.get(family) ?? 0; counters.set(family, rank + 1);
    const stagger = Math.min(rank, 8) * (to - from) * .035;
    const parent = node.parent!;
    // Pivot at the physical part center, preserving the complete CAD transform.
    const pivot = new Group(); pivot.position.copy(parent.worldToLocal(center.clone()));
    parent.add(pivot); model.updateMatrixWorld(true); pivot.attach(node);
    const worldRotation = parent.getWorldQuaternion(new Quaternion());
    const axis = new Vector3(0, 1, 0);
    if (screw && starts(name, 'Prisioneros')) {
      const size = new Box3().setFromObject(node, true).getSize(new Vector3());
      const dimension = size.x > size.y && size.x > size.z ? 0 : size.z > size.y ? 2 : 1;
      axis.set(0, 0, 0).setComponent(dimension, dimension === 0 ? side : 1);
      direction.copy(axis).multiplyScalar(.1);
    }
    axis.applyQuaternion(worldRotation.clone().invert());
    parts.push({ node: pivot, position: pivot.position.clone(), quaternion: pivot.quaternion.clone(),
      from: from + stagger, to: to + stagger, offset: direction.applyQuaternion(worldRotation.clone().invert()), screw, axis,
      materials: revealMaterials(node) });
  }
  return { model, parts, geometries };
}

export function poseAssemblyCar(rig: ReturnType<typeof prepareAssemblyCar>, progress: number, reduced: boolean, visibility = 1) {
  const rotation = new Quaternion();
  for (const part of rig.parts) {
    const phase = segment(progress, part.from, part.to), t = reduced ? (phase >= .5 ? 1 : 0) : ease(phase);
    part.node.visible = visibility > .001 && (reduced ? phase >= .5 : progress >= part.from);
    revealOpacity(part.materials, visibility * (reduced ? (phase >= .5 ? 1 : 0) : ease(segment(phase, 0, .65))));
    part.node.position.copy(part.position).addScaledVector(part.offset, .55 * (1 - t));
    part.node.quaternion.copy(part.quaternion);
    if (part.screw && !reduced) part.node.quaternion.multiply(rotation.setFromAxisAngle(part.axis, -(1 - t) * Math.PI * 4));
  }
}
