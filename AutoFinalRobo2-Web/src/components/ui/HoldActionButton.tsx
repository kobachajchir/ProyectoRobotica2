import { useCallback, useEffect, useRef, useState, type ReactNode } from 'react';
import { Button } from './Button';

/** Common hold semantics. Transport, heartbeat and stop policy belong to the
 * caller; this component only captures/releases pointer and keyboard input. */
export function HoldActionButton({ label, children, disabled = false, active = false, onStart, onStop, className = '' }: {
  label: string; children: ReactNode; disabled?: boolean; active?: boolean;
  onStart: () => void; onStop: () => void; className?: string;
}) {
  const engaged = useRef(false);
  const latest = useRef({ onStart, onStop }); latest.current = { onStart, onStop };
  const [held, setHeld] = useState(false);
  const release = useCallback((render = true) => {
    if (!engaged.current) return;
    engaged.current = false;
    if (render) setHeld(false);
    latest.current.onStop();
  }, []);
  const start = () => {
    if (disabled || engaged.current) return;
    engaged.current = true; setHeld(true); latest.current.onStart();
  };
  useEffect(() => { if (disabled) release(); }, [disabled, release]);
  useEffect(() => () => release(false), [release]);
  return <Button className={`afr2-hold-action ${className}`} aria-label={label} title={label} aria-pressed={active || held} disabled={disabled}
    onPointerDown={event => { if (event.button !== 0 || disabled) return; event.preventDefault(); event.currentTarget.setPointerCapture(event.pointerId); start(); }}
    onPointerUp={event => { release(); if (event.currentTarget.hasPointerCapture(event.pointerId)) event.currentTarget.releasePointerCapture(event.pointerId); }}
    onPointerCancel={() => release()} onLostPointerCapture={() => release()} onBlur={() => release()}
    onPointerLeave={() => release()}
    onKeyDown={event => { if ((event.key === ' ' || event.key === 'Enter') && !event.repeat) { event.preventDefault(); start(); } }}
    onKeyUp={event => { if (event.key === ' ' || event.key === 'Enter') { event.preventDefault(); release(); } }}>
    {children}
  </Button>;
}
