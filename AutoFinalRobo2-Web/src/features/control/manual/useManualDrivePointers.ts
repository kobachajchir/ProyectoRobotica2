import { useCallback, useRef, useState, type PointerEvent } from 'react';

interface Contact { id: number; type: string; x: number; y: number; startX: number; startY: number; element: HTMLDivElement }
interface Options {
  enabled: boolean;
  keyboardActive: () => boolean;
  onTranslate: (dx: number, dy: number) => void;
  onRelease: () => void;
  onTwistStart: () => void;
  onTwist: (radians: number) => void;
}

/** AFR2-20261025-11. One pointer translates; two touch contacts twist locally.
 * Leaving a multi-touch gesture drains the remaining contacts before rearming.
 */
export function useManualDrivePointers(options: Options) {
  const latest = useRef(options); latest.current = options;
  const contacts = useRef(new Map<number, Contact>());
  const draining = useRef(new Map<number, string>());
  const twist = useRef<{ angle: number | null; pending: number; active: boolean } | null>(null);
  const [pressed, setPressed] = useState(false);
  const [twisting, setTwisting] = useState(false);
  const stop = useCallback((endedId?: number, extra?: { id: number; type: string }) => {
    const captured = [...contacts.current.values()]; contacts.current.clear(); twist.current = null;
    for (const point of captured) if (point.id !== endedId) draining.current.set(point.id, point.type);
    if (extra && extra.id !== endedId) draining.current.set(extra.id, extra.type);
    if (endedId !== undefined) draining.current.delete(endedId);
    setPressed(false); setTwisting(false); latest.current.onRelease();
    for (const point of captured) {
      try { if (point.element.hasPointerCapture(point.id)) point.element.releasePointerCapture(point.id); }
      catch { /* Capture can already have ended; contacts/output are cleared. */ }
    }
  }, []);
  const cancel = useCallback(() => stop(), [stop]);
  const reset = useCallback(() => { stop(); draining.current.clear(); }, [stop]);
  const hasPointers = useCallback(() => contacts.current.size > 0 || draining.current.size > 0, []);
  const end = useCallback((event: { pointerId: number }) => {
    draining.current.delete(event.pointerId);
    if (contacts.current.has(event.pointerId)) stop(event.pointerId);
  }, [stop]);
  const lost = useCallback((event: { pointerId: number }) => {
    if (contacts.current.has(event.pointerId)) stop();
  }, [stop]);
  const externalDown = useCallback((event: globalThis.PointerEvent) => {
    const first = contacts.current.values().next().value as Contact | undefined;
    if (!first) {
      if (draining.current.size) draining.current.set(event.pointerId, event.pointerType);
      return;
    }
    if (contacts.current.has(event.pointerId)) return;
    const validPair = contacts.current.size === 1 && first.type === 'touch' && event.pointerType === 'touch' &&
      event.target instanceof Node && first.element.contains(event.target);
    if (!validPair) stop(undefined, { id: event.pointerId, type: event.pointerType });
  }, [stop]);
  const down = useCallback((event: PointerEvent<HTMLDivElement>) => {
    if (!latest.current.enabled || event.button !== 0) return;
    // A new primary contact is a fresh native contact cycle, also recovering
    // from an up event missed while the window was blurred.
    if (!contacts.current.size && event.isPrimary && [...draining.current.values()].every(type => type === event.pointerType)) draining.current.clear();
    if (draining.current.size) { draining.current.set(event.pointerId, event.pointerType); return; }
    const first = contacts.current.values().next().value as Contact | undefined;
    const validPair = contacts.current.size === 1 && first?.type === 'touch' && event.pointerType === 'touch';
    if (latest.current.keyboardActive() || (first && !validPair) || (!first && !event.isPrimary)) {
      stop(undefined, { id: event.pointerId, type: event.pointerType }); return;
    }
    event.preventDefault(); event.stopPropagation();
    document.documentElement.dataset.input = 'pointer'; event.currentTarget.focus({ preventScroll: true });
    try { event.currentTarget.setPointerCapture(event.pointerId); }
    catch { stop(undefined, { id: event.pointerId, type: event.pointerType }); return; }
    const point: Contact = { id: event.pointerId, type: event.pointerType, x: event.clientX, y: event.clientY,
      startX: event.clientX, startY: event.clientY, element: event.currentTarget };
    contacts.current.set(point.id, point); setPressed(true);
    if (first) {
      latest.current.onTwistStart();
      const distance = Math.hypot(point.x - first.x, point.y - first.y);
      twist.current = { angle: distance >= 40 ? Math.atan2(point.y - first.y, point.x - first.x) : null, pending: 0, active: false };
      setTwisting(true);
    }
  }, [stop]);
  const move = useCallback((event: PointerEvent<HTMLDivElement>) => {
    const point = contacts.current.get(event.pointerId);
    if (!point || !latest.current.enabled) return;
    if (point.type === 'mouse' && !(event.buttons & 1)) { stop(); return; }
    const rect = point.element.getBoundingClientRect();
    if (event.clientX < rect.left || event.clientX > rect.right || event.clientY < rect.top || event.clientY > rect.bottom) { stop(); return; }
    point.x = event.clientX; point.y = event.clientY;
    const turn = twist.current;
    if (!turn) { latest.current.onTranslate(point.x - point.startX, point.y - point.startY); return; }
    const [a, b] = [...contacts.current.values()];
    if (!a || !b) return;
    if (Math.hypot(b.x - a.x, b.y - a.y) < 40) { turn.angle = null; turn.pending = 0; return; }
    const angle = Math.atan2(b.y - a.y, b.x - a.x);
    const previous = turn.angle; turn.angle = angle;
    if (previous === null) return;
    // Screen angles grow clockwise; Three.js +Y is counterclockwise overhead.
    const delta = -Math.atan2(Math.sin(angle - previous), Math.cos(angle - previous));
    if (!turn.active) {
      turn.pending += delta;
      if (Math.abs(turn.pending) < Math.PI / 60) return; // 3 degree twist slop.
      turn.active = true; latest.current.onTwist(turn.pending); turn.pending = 0;
    } else latest.current.onTwist(delta);
  }, [stop]);
  return { pressed, twisting, hasPointers, cancel, reset, down, move, end, lost, externalDown };
}
