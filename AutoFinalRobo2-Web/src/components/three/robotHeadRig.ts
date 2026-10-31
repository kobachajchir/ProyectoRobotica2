import { Group, PropertyBinding, Vector3, type Object3D } from 'three';

export const hasCadPrefix = (node: Object3D, prefix: string) => node.name.startsWith(PropertyBinding.sanitizeNodeName(prefix));

/** CAD: arm=(carX,-carZ-32mm,carY-26mm).
 * Deck mesh local frame is Blender Z-up. Native SG90 output axis in arm CAD:
 * X=-5.15002937mm,Y=42.999972322mm. The Y ordinate along this vertical
 * shaft does not change its rotation line. Never infer it from sensor centre. */
export const HEAD_AXIS_IN_DECK = new Vector3(-.00515002937, .074999972322, .039149956902);

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
  const pivot = new Group(); pivot.name = 'AFR2_Pivote_SG90_CAD';
  pivot.position.copy(model.worldToLocal(deck.localToWorld(HEAD_AXIS_IN_DECK.clone())));
  model.add(pivot); model.updateMatrixWorld(true);
  for (const part of [sensor, bracket, output]) if (part) pivot.attach(part);
  return pivot;
}
