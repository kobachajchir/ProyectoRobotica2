import { Box3, Group, Quaternion, Vector3, type Object3D } from 'three';
import { ADDITIONS_FROM, ADDITIONS_TO, ease, segment } from './assemblyTimeline';
import { disposeRevealMaterials, revealMaterials, revealOpacity } from './assemblyMaterials';

export const ADDITIONS_MODEL_URL = '/models/AFR2_Sensor_Bateria.glb';
export const ADDITIONS_MODEL_REVISION = '20261109-10';
export const ADDITIONS_ASSET_URL = `${ADDITIONS_MODEL_URL}?v=${ADDITIONS_MODEL_REVISION}`;
export const ADDITIONS_VIEW_DIRECTION = new Vector3(-.62, .65, -1).normalize();
const Y = new Vector3(0, 1, 0), IDENTITY = new Quaternion();
const PANEL_FRONT = new Quaternion().setFromAxisAngle(Y, Math.atan2(-ADDITIONS_VIEW_DIRECTION.z, ADDITIONS_VIEW_DIRECTION.x));
const POWER_TOP = new Quaternion().setFromAxisAngle(new Vector3(1, 0, 0), -Math.PI / 4);
const DIVIDER_FOCUS = new Box3(new Vector3(-.012, 0, .025), new Vector3(.012, .014, .048));
// Browser-only staging translations: voltmeter above holder, sensor below.
// The source auto and its installed transforms are never edited.
const HOLDER_DELTA = new Vector3(0, -.07905133813619614, -.07199974171817303);
const VOLTMETER_DELTA = new Vector3(.05, -.07995112371444702, -.001997555620968344);

function pivotAroundCenter(root: Group, content: Group) {
  root.updateMatrixWorld(true);
  const rest = new Box3().setFromObject(content, true).getCenter(new Vector3());
  const pivot = new Group(); pivot.position.copy(rest); root.add(pivot);
  root.updateMatrixWorld(true); pivot.attach(content);
  return { pivot, content, rest };
}

/** Browser-only extraction. Panel, holder, cells and voltmeter reuse the original
 * auto's cached buffers; the new GLB contains ONLY four components and wires. */
export function prepareAdditions(auto: Object3D, circuitAsset: Object3D) {
  const root = new Group(), panelContent = new Group(), powerContent = new Group(), body = new Group(), battery = new Group(), voltmeter = new Group();
  const circuit = circuitAsset.clone(true);
  for (const original of auto.children) {
    if (/UI_B2/.test(original.name)) panelContent.add(original.clone(true));
    else if (/Voltimetro/.test(original.name)) voltmeter.add(original.clone(true));
    else if (/Portabaterias|Celda_18650/.test(original.name)) battery.add(original.clone(true));
  }
  if (!panelContent.children.length || !battery.children.length) throw new Error('No están disponibles los agregados del modelo del auto.');
  body.add(battery, voltmeter); powerContent.add(body, circuit); root.add(panelContent);
  const panel = pivotAroundCenter(root, panelContent);
  const powerPivot = new Group(); root.add(powerPivot); powerPivot.add(powerContent);
  const power = { pivot: powerPivot, content: powerContent, rest: new Vector3() };
  return { root, panel, power, battery, voltmeter,
    panelMaterials: revealMaterials(panelContent), powerMaterials: revealMaterials(body), circuitMaterials: revealMaterials(circuit) };
}

const interpolateBox = (a: Box3, b: Box3, t: number) => new Box3(a.min.clone().lerp(b.min, t), a.max.clone().lerp(b.max, t));

export function additionsIsolation(progress: number, reduced: boolean) {
  if (reduced) return progress >= ADDITIONS_FROM && progress < ADDITIONS_TO ? 1 : 0;
  return ease(segment(progress, .43, .445)) * (1 - ease(segment(progress, .565, .60)));
}

export function poseAdditions(rig: ReturnType<typeof prepareAdditions>, progress: number, reduced: boolean) {
  const isolated = progress >= ADDITIONS_FROM && progress < ADDITIONS_TO;
  const focus = reduced ? (isolated ? 1 : 0) : ease(segment(progress, .432, .455)) * (1 - ease(segment(progress, .555, .595)));
  const panelAlpha = reduced ? (progress >= .43 ? 1 : 0) : Math.min(1,
    ease(segment(progress, .432, .449)) * (1 - ease(segment(progress, .482, .497))) + ease(segment(progress, .558, .578)));
  const circuitAlpha = reduced ? (isolated ? 1 : 0) : ease(segment(progress, .482, .496)) * (1 - ease(segment(progress, .565, .595)));
  const batteryAlpha = reduced ? (progress >= .43 ? 1 : 0) : ease(segment(progress, .512, .532));
  rig.panel.pivot.position.copy(rig.panel.rest).lerp(new Vector3(reduced && isolated ? -.09 : 0, .15, 0), focus);
  rig.power.pivot.position.copy(rig.power.rest).lerp(new Vector3(reduced && isolated ? .06 : 0, .15, 0), focus);
  rig.power.content.position.set(0, -.010 * focus, 0);
  rig.battery.position.copy(HOLDER_DELTA).multiplyScalar(focus);
  rig.voltmeter.position.copy(VOLTMETER_DELTA).multiplyScalar(focus);
  rig.panel.pivot.quaternion.slerpQuaternions(IDENTITY, PANEL_FRONT, focus);
  rig.power.pivot.quaternion.slerpQuaternions(IDENTITY, POWER_TOP, focus);
  rig.panel.pivot.visible = panelAlpha > .001; rig.power.pivot.visible = circuitAlpha > .001 || batteryAlpha > .001;
  revealOpacity(rig.panelMaterials, panelAlpha); revealOpacity(rig.powerMaterials, batteryAlpha); revealOpacity(rig.circuitMaterials, circuitAlpha);
  rig.root.updateWorldMatrix(true, true);
  const panelBox = new Box3().setFromObject(rig.panel.content, true);
  const dividerBox = DIVIDER_FOCUS.clone().applyMatrix4(rig.power.content.matrixWorld);
  const powerBox = new Box3().setFromObject(rig.power.content, true);
  const macroBounds = reduced && isolated ? panelBox.union(powerBox) : progress < .512
    ? interpolateBox(panelBox, dividerBox, ease(segment(progress, .48, .498)))
    : interpolateBox(dividerBox, powerBox, ease(segment(progress, .512, .537)));
  return { macroBounds, isolation: additionsIsolation(progress, reduced) };
}

export function disposeAdditions(rig: ReturnType<typeof prepareAdditions>) {
  disposeRevealMaterials(rig.panelMaterials); disposeRevealMaterials(rig.powerMaterials); disposeRevealMaterials(rig.circuitMaterials);
}
