import { Group, PropertyBinding, Quaternion, Vector3, type Object3D } from 'three';

export const hasCadPrefix = (node: Object3D, prefix: string) => node.name.startsWith(PropertyBinding.sanitizeNodeName(prefix));

/** CAD: arm=(carX,-carZ-32mm,carY-26mm).
 * Native SG90 output axis in arm CAD: X=-5.15002937mm,Y=42.999972322mm.
 * CapaSuperior in the exported GLB is Y-up, with identity rotation. Convert
 * Blender (x,y,z) -> glTF (x,z,-y) before applying the deck node matrix.
 * The shaft is parallel to deck-local +Y. Never infer it from sensor centre. */
export const HEAD_AXIS_IN_DECK = new Vector3(-.00515002937, .039149956902, -.074999972322);

export function createHeadRig(model: Object3D) {
  let deck: Object3D | undefined, servo: Object3D | undefined;
  let sensor: Object3D | undefined, bracket: Object3D | undefined, output: Object3D | undefined;
  model.traverse(node => {
    if (hasCadPrefix(node, 'AFR2_CapaSuperior')) deck ??= node;
    if (hasCadPrefix(node, 'AFR2_SG90:')) servo ??= node;
    if (hasCadPrefix(node, 'AFR2_HC_SR04:')) sensor ??= node;
    if (hasCadPrefix(node, 'AFR2_Soporte_HCSR04_Referencia:')) bracket ??= node;
    if (hasCadPrefix(node, 'AFR2_SG90_Salida')) output ??= node;
  });
  if (!deck || !servo || !sensor || !bracket) return null;
  model.updateMatrixWorld(true);
  const shaftFrame = new Group(); shaftFrame.name = 'AFR2_Eje_Fijo_SG90_CAD';
  shaftFrame.position.copy(model.worldToLocal(deck.localToWorld(HEAD_AXIS_IN_DECK.clone())));
  shaftFrame.quaternion.copy(model.getWorldQuaternion(new Quaternion()).invert()
    .multiply(deck.getWorldQuaternion(new Quaternion())));
  model.add(shaftFrame);
  // Consumers drive only this child's Y angle; the shaft position/orientation
  // remain on its parent even when the imported deck frame is rotated.
  const pivot = new Group(); pivot.name = 'AFR2_Pivote_SG90_CAD';
  shaftFrame.add(pivot); model.updateMatrixWorld(true);
  for (const part of [sensor, bracket, output]) if (part) pivot.attach(part);
  return pivot;
}
