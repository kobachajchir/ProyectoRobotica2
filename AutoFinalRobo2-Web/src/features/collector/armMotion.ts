import { Quaternion, Vector3 } from 'three';
import { forwardModel, jointsWithin, mountFor, rad } from './modelKinematics';
import { modelProblems, robotModel, type ArmAngles, type ArmEnvironment, type CollisionBox, type Point3, type Quaternion4, type RobotModelDefinition } from './robotModel';

export interface MotionEnvironment { environment: ArmEnvironment; boxes: CollisionBox[] }
export interface CartesianPose { position: Point3; quaternion: Quaternion4 }
export interface ArmMotionPlan {
  pointId: string; context: string; modelId: string; revision: string;
  from: ArmAngles; solution: ArmAngles; stages: ArmAngles[]; samples: Point3[];
  opening: number; finishOpening?: number;
  duration: number; positionError: number; orientationError: number;
}
export type MotionResult = { ok: true; plan: ArmMotionPlan } | { ok: false; code: 'model' | 'range' | 'limits' | 'solution' | 'blocked'; message: string };
export interface MotionRequest {
  requestId: number; context: string; pointId: string; model: RobotModelDefinition;
  from: ArmAngles; target: CartesianPose; opening: number; finishOpening?: number; taught?: ArmAngles; environment: MotionEnvironment;
}
const vec = (p: Point3) => new Vector3(...p);
const position = (m: import('three').Matrix4) => new Vector3().setFromMatrixPosition(m);

function segmentBox(a: Vector3, b: Vector3, box: CollisionBox, radius: number) {
  let near = 0, far = 1;
  for (let i = 0; i < 3; i++) {
    const low = box.center[i] - box.size[i] / 2 - radius, high = box.center[i] + box.size[i] / 2 + radius;
    const origin = a.getComponent(i), delta = b.getComponent(i) - origin;
    if (Math.abs(delta) < 1e-10) { if (origin < low || origin > high) return false; }
    else { const t1 = (low - origin) / delta, t2 = (high - origin) / delta; near = Math.max(near, Math.min(t1, t2)); far = Math.min(far, Math.max(t1, t2)); }
    if (near > far) return false;
  }
  return true;
}
function segmentDistance(a: Vector3, b: Vector3, c: Vector3, d: Vector3) {
  const u = b.clone().sub(a), v = d.clone().sub(c), w = a.clone().sub(c);
  const aa = u.dot(u), bb = u.dot(v), cc = v.dot(v), dd = u.dot(w), ee = v.dot(w), denominator = aa * cc - bb * bb;
  let s = denominator > 1e-12 ? Math.max(0, Math.min(1, (bb * ee - cc * dd) / denominator)) : 0;
  let t = cc > 1e-12 ? (bb * s + ee) / cc : 0;
  if (t < 0) { t = 0; s = aa ? Math.max(0, Math.min(1, -dd / aa)) : 0; }
  if (t > 1) { t = 1; s = aa ? Math.max(0, Math.min(1, (bb - dd) / aa)) : 0; }
  return w.addScaledVector(u, s).addScaledVector(v, -t).length();
}
/** Capsule links / AABB environment and sampled gripper volumes. These are
 * approximate Web proxies, not native CAD BRep or continuous collision proof. */
export function clearConfiguration(q: ArmAngles, opening: number, env: MotionEnvironment, model = robotModel()) {
  if (!Number.isFinite(opening) || !jointsWithin(q, model) || opening < model.gripper.min || opening > model.gripper.max) return false;
  const chain = forwardModel(q, model, mountFor(env.environment, model));
  const segments = chain.frames.map((frame, i) => ({ id: model.joints[i].id, a: position(frame),
    b: i + 1 < chain.frames.length ? position(chain.frames[i + 1]) : vec(chain.tcp), radius: i === 0 ? model.collision.towerRadius : model.joints[i].radius }));
  const boxes = [...(env.environment === 'mounted' ? model.collision.boxes : []), ...env.boxes];
  const mount = vec(mountFor(env.environment, model).position), shoulder = position(chain.frames[Math.min(1, chain.frames.length - 1)]);
  for (const segment of segments) {
    if (Math.min(segment.a.z, segment.b.z) < segment.radius || boxes.some(box => segmentBox(segment.a, segment.b, box, segment.radius))) return false;
    for (let n = 0; n <= 8; n++) {
      const p = segment.a.clone().lerp(segment.b, n / 8);
      if (segment.id !== model.joints[0].id && p.z < shoulder.z - segment.radius && Math.hypot(p.x - mount.x, p.y - mount.y) < model.collision.towerRadius + segment.radius) return false;
    }
  }
  for (let i = 0; i < segments.length; i++) for (let j = i + 2; j < segments.length; j++) {
    const a = segments[i], b = segments[j];
    if (model.collision.allowedPairs.some(pair => pair.includes(a.id) && pair.includes(b.id))) continue;
    if (segmentDistance(a.a, a.b, b.a, b.b) < a.radius + b.radius) return false;
  }
  const tool = chain.frames[chain.frames.length - 1];
  const volumes = [{ center: [0, 0, model.gripper.palmZ] as Point3, size: model.gripper.palmSize },
    ...[-1, 1].map(side => ({ center: model.gripper.travelAxis.map((v, i) => side * v * (opening * model.gripper.metersPerOpeningMm + model.gripper.fingerSize[0] / 2) + (i === 2 ? model.gripper.fingerZ : 0)) as Point3, size: model.gripper.fingerSize }))];
  for (const volume of volumes) {
    // Grid through each local OBB, projected to model coordinates. Approximate.
    for (const x of [-.5, 0, .5]) for (const y of [-.5, 0, .5]) for (const z of [-.5, 0, .5]) {
      const p = new Vector3(volume.center[0] + x * volume.size[0], volume.center[1] + y * volume.size[1], volume.center[2] + z * volume.size[2]).applyMatrix4(tool);
      if (p.z < 0 || boxes.some(box => segmentBox(p, p, box, .001))) return false;
    }
  }
  return true;
}
export function samplePath(from: ArmAngles, to: ArmAngles, env: MotionEnvironment, opening: number, model = robotModel()) {
  if (!jointsWithin(from, model) || !jointsWithin(to, model)) return null;
  const count = Math.max(1, Math.ceil(Math.max(...to.map((v, i) => Math.abs(v - from[i]))) / 3));
  const samples = Array.from({ length: count + 1 }, (_, n) => from.map((v, i) => v + (to[i] - v) * n / count));
  return samples.every(q => clearConfiguration(q, opening, env, model)) ? samples : null;
}
export function clearOpening(q: ArmAngles, from: number, to: number, env: MotionEnvironment, model = robotModel()) {
  if (![from, to].every(Number.isFinite) || Math.min(from, to) < model.gripper.min || Math.max(from, to) > model.gripper.max) return false;
  const steps = Math.max(1, Math.ceil(Math.abs(to - from)));
  for (let n = 0; n <= steps; n++) if (!clearConfiguration(q, from + (to - from) * n / steps, env, model)) return false;
  return true;
}
export function routeTo(from: ArmAngles, to: ArmAngles, env: MotionEnvironment, opening: number, model = robotModel()) {
  const candidates = [[to], [model.transport, to], ...model.seeds.map(seed => [seed, to])];
  for (const stages of candidates) {
    let previous = from;
    if (stages.every(q => { const samples = samplePath(previous, q, env, opening, model); previous = q; return samples !== null; })) return stages.map(q => [...q]);
  }
  return null;
}
function orientationError(from: Quaternion, to: Quaternion) {
  const delta = to.clone().multiply(from.clone().invert()).normalize();
  if (delta.w < 0) delta.set(-delta.x, -delta.y, -delta.z, -delta.w);
  const s = Math.hypot(delta.x, delta.y, delta.z);
  return s < 1e-10 ? [0, 0, 0] : [delta.x, delta.y, delta.z].map(v => v * 2 * Math.atan2(s, delta.w) / s);
}
/** Small pivoted Gaussian solve for damped J J^T (6x6). */
function solve(a: number[][], b: number[]) {
  const rows = a.map((row, i) => [...row, b[i]]), n = b.length;
  for (let i = 0; i < n; i++) {
    let best = i; for (let j = i + 1; j < n; j++) if (Math.abs(rows[j][i]) > Math.abs(rows[best][i])) best = j;
    [rows[i], rows[best]] = [rows[best], rows[i]];
    if (Math.abs(rows[i][i]) < 1e-15) return null;
    const scale = rows[i][i]; for (let k = i; k <= n; k++) rows[i][k] /= scale;
    for (let j = 0; j < n; j++) if (j !== i) { const factor = rows[j][i]; for (let k = i; k <= n; k++) rows[j][k] -= factor * rows[i][k]; }
  }
  return rows.map(row => row[n]);
}
export function planMotion(request: MotionRequest): MotionResult {
  const { model, target, from, environment, opening } = request;
  const problems = modelProblems(model);
  if (problems.length) return { ok: false, code: 'model', message: problems.join(' ') };
  if (!jointsWithin(from, model)) return { ok: false, code: 'limits', message: 'La postura inicial no respeta los límites del modelo.' };
  const maxReach = model.joints.reduce((sum, j) => sum + Math.hypot(...j.origin.position), Math.hypot(...model.tcp.position));
  if (Math.hypot(...target.position) > maxReach + .001) return { ok: false, code: 'range', message: 'Destino fuera de la cota máxima de alcance del modelo.' };
  const desired = new Quaternion(...target.quaternion).normalize(), weight = .15;
  const seeds = [from, ...(request.taught ? [request.taught] : []), model.initial, model.transport, ...model.seeds];
  let found = false, atLimit = false;
  for (const seed of seeds) {
    if (!jointsWithin(seed, model)) continue;
    let q = [...seed];
    for (let iteration = 0; iteration < 240; iteration++) {
      const fk = forwardModel(q, model), current = new Quaternion(...fk.orientation);
      const dp = target.position.map((v, i) => v - fk.tcp[i]), dr = orientationError(current, desired);
      const pe = Math.hypot(...dp), re = Math.hypot(...dr);
      if (pe <= .001 && re <= rad(1)) {
        found = true;
        if (request.finishOpening !== undefined && !clearOpening(q, opening, request.finishOpening, environment, model)) break;
        const stages = routeTo(from, q, environment, opening, model);
        if (stages) {
          let previous = from, duration = 0; const samples: Point3[] = [];
          for (const stage of stages) {
            duration += Math.max(...stage.map((v, i) => Math.abs(v - previous[i]))) / model.speedDegS;
            for (const pose of samplePath(previous, stage, environment, opening, model)!) samples.push(forwardModel(pose, model).tcp);
            previous = stage;
          }
          return { ok: true, plan: { pointId: request.pointId, context: request.context, modelId: model.id, revision: model.revision,
            from: [...from], solution: q, stages, samples, opening, finishOpening: request.finishOpening, duration, positionError: pe, orientationError: re } };
        }
        break;
      }
      const error = [...dp, ...dr.map(v => v * weight)], epsilon = .0001;
      const columns = q.map((_, i) => {
        const perturbed = [...q]; perturbed[i] += epsilon * 180 / Math.PI;
        const next = forwardModel(perturbed, model), rotation = orientationError(current, new Quaternion(...next.orientation));
        return [...next.tcp.map((v, k) => (v - fk.tcp[k]) / epsilon), ...rotation.map(v => v * weight / epsilon)];
      });
      const a = Array.from({ length: 6 }, (_, r) => Array.from({ length: 6 }, (_, c) => columns.reduce((sum, column) => sum + column[r] * column[c], 0) + (r === c ? .0001 : 0)));
      const x = solve(a, error); if (!x) break;
      q = q.map((v, i) => {
        const delta = Math.max(-.12, Math.min(.12, columns[i].reduce((sum, j, k) => sum + j * x[k], 0))) * 180 / Math.PI;
        const next = Math.max(model.joints[i].min, Math.min(model.joints[i].max, v + delta));
        if (Math.abs(next - (v + delta)) > .0001) atLimit = true;
        return next;
      });
    }
  }
  return found ? { ok: false, code: 'blocked', message: 'Se encontró postura, pero ningún recorrido admitido por las restricciones aproximadas.' }
    : { ok: false, code: atLimit ? 'limits' : 'solution', message: atLimit ? 'Búsqueda limitada por articulaciones: no se encontró solución. No demuestra imposibilidad geométrica.' : 'No se encontró solución numérica para posición y orientación. El destino se conserva.' };
}
