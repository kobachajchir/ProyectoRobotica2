import type { Point3 } from './robotModel';

export interface GroundPose { x: number; y: number; yaw: number }
const STEP = .10, LIMIT = 19;

/** Local navigation only. Check the entire base footprint along every segment. */
export function groundSegmentClear(a: GroundPose, b: GroundPose, clear: (pose: GroundPose) => boolean) {
  const steps = Math.max(1, Math.ceil(Math.hypot(b.x - a.x, b.y - a.y) / .015));
  for (let i = 0; i <= steps; i++) {
    const t = i / steps;
    if (!clear({ x: a.x + (b.x - a.x) * t, y: a.y + (b.y - a.y) * t, yaw: b.yaw })) return false;
  }
  return true;
}

/** Bounded A*, eight neighbours and collision-checked diagonals, then shortcuts.
 * All waypoints use the docking heading; the base aligns before translating.
 */
export function groundRoute(start: GroundPose, goal: GroundPose, clear: (pose: GroundPose) => boolean): Point3[] | null {
  if (!clear(goal)) return null;
  if (groundSegmentClear(start, goal, clear)) return [[goal.x, goal.y, 0]];
  const key = (x: number, y: number) => `${x},${y}`;
  const pose = (x: number, y: number): GroundPose => ({ x: x * STEP, y: y * STEP, yaw: goal.yaw });
  const open = new Map<string, { x: number; y: number; g: number; f: number }>();
  const costs = new Map<string, number>(), parents = new Map<string, string | null>();
  const closed = new Set<string>();
  const sx = Math.round(start.x / STEP), sy = Math.round(start.y / STEP);
  for (let dx = -1; dx <= 1; dx++) for (let dy = -1; dy <= 1; dy++) {
    const x = sx + dx, y = sy + dy, p = pose(x, y);
    if (Math.abs(x) > LIMIT || Math.abs(y) > LIMIT || !groundSegmentClear(start, p, clear)) continue;
    const id = key(x, y), g = Math.hypot(p.x - start.x, p.y - start.y);
    open.set(id, { x, y, g, f: g + Math.hypot(p.x - goal.x, p.y - goal.y) }); costs.set(id, g); parents.set(id, null);
  }
  while (open.size && closed.size < 1600) {
    let id = '', best = Infinity;
    for (const [candidate, node] of open) if (node.f < best) { id = candidate; best = node.f; }
    const node = open.get(id)!; open.delete(id); closed.add(id);
    const current = pose(node.x, node.y);
    if (Math.hypot(current.x - goal.x, current.y - goal.y) <= .16 && groundSegmentClear(current, goal, clear)) {
      const points: Point3[] = [[goal.x, goal.y, 0]];
      let cursor: string | null = id;
      while (cursor !== null) {
        const [x, y] = cursor.split(',').map(Number); points.unshift([x * STEP, y * STEP, 0]);
        cursor = parents.get(cursor) ?? null;
      }
      const result: Point3[] = [];
      let from = start;
      for (let i = 0; i < points.length;) {
        let next = i;
        for (let j = points.length - 1; j > i; j--) {
          if (groundSegmentClear(from, { x: points[j][0], y: points[j][1], yaw: goal.yaw }, clear)) { next = j; break; }
        }
        result.push(points[next]); from = { x: points[next][0], y: points[next][1], yaw: goal.yaw }; i = next + 1;
      }
      return result;
    }
    for (let dx = -1; dx <= 1; dx++) for (let dy = -1; dy <= 1; dy++) {
      if (!dx && !dy) continue;
      const x = node.x + dx, y = node.y + dy, nextId = key(x, y);
      if (Math.abs(x) > LIMIT || Math.abs(y) > LIMIT || closed.has(nextId)) continue;
      const g = node.g + Math.hypot(dx, dy) * STEP;
      if (g >= (costs.get(nextId) ?? Infinity)) continue;
      const next = pose(x, y);
      if (!groundSegmentClear(current, next, clear)) continue;
      costs.set(nextId, g); parents.set(nextId, id);
      open.set(nextId, { x, y, g, f: g + Math.hypot(next.x - goal.x, next.y - goal.y) });
    }
  }
  return null;
}
