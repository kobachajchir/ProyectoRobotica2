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
 * AFR2-20261106-36: (a) a touch pointercancel on the single drag contact opens a short grace
 * window: the order stays alive and a new touch within it resumes the same drag vector
 * (browsers/system gestures can cancel a finger that is still down). (b) Lifting one finger of
 * a twist keeps the other as a drag from its current position, so the gesture is not lost.
 * Each contact is tracked by pointerId. Order renewal is ControlPage's 180 ms heartbeat.
 */
const CANCEL_GRACE_MS = 250;
export function useManualDrivePointers(options: Options) {
  const latest = useRef(options); latest.current = options;
  const contacts = useRef(new Map<number, Contact>());
  const draining = useRef(new Map<number, string>());
  const twist = useRef<{ angle: number | null; pending: number; active: boolean } | null>(null);
  const grace = useRef<{ dx: number; dy: number; timer: ReturnType<typeof setTimeout> } | null>(null);
  const clearGrace = () => { if (grace.current) { clearTimeout(grace.current.timer); grace.current = null; } };
  const [pressed, setPressed] = useState(false);
  const [twisting, setTwisting] = useState(false);
  const stop = useCallback((endedId?: number, extra?: { id: number; type: string }) => {
    clearGrace();
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
  const hasPointers = useCallback(() => contacts.current.size > 0 || draining.current.size > 0 || grace.current !== null, []);
  const end = useCallback((event: { pointerId: number }) => {
    draining.current.delete(event.pointerId);
    const ended = contacts.current.get(event.pointerId);
    if (!ended) return;
    if (twist.current && contacts.current.size === 2) {
      // The remaining finger becomes a fresh drag from where it is: no jump, no lost gesture.
      contacts.current.delete(event.pointerId); twist.current = null; setTwisting(false);
      try { if (ended.element.hasPointerCapture(ended.id)) ended.element.releasePointerCapture(ended.id); } catch { /* already released */ }
      const rest = contacts.current.values().next().value as Contact | undefined;
      if (rest) { rest.startX = rest.x; rest.startY = rest.y; }
      latest.current.onRelease();
      return;
    }
    stop(event.pointerId);
  }, [stop]);
  const cancelPointer = useCallback((event: { pointerId: number }) => {
    const point = contacts.current.get(event.pointerId);
    if (!point) { draining.current.delete(event.pointerId); return; }
    if (point.type !== 'touch' || contacts.current.size !== 1 || twist.current) { end(event); return; }
    // Possibly spurious: keep the current order for a moment and let a new touch resume it.
    contacts.current.delete(event.pointerId); clearGrace();
    grace.current = { dx: point.x - point.startX, dy: point.y - point.startY, timer: setTimeout(() => { grace.current = null; stop(); }, CANCEL_GRACE_MS) };
  }, [end, stop]);
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
    const resume = grace.current && !contacts.current.size && event.pointerType === 'touch' ? grace.current : null;
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
    if (resume) clearGrace();
    const point: Contact = { id: event.pointerId, type: event.pointerType, x: event.clientX, y: event.clientY,
      startX: event.clientX - (resume?.dx ?? 0), startY: event.clientY - (resume?.dy ?? 0), element: event.currentTarget };
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
  return { pressed, twisting, hasPointers, cancel, reset, down, move, end, cancelPointer, lost, externalDown };
}
