/* AFR2-20261110-16. 90° in-place turn presets closed by Hall odometry (robot.goto.start x=y=0).
 * Positive theta = CCW (left), as in the firmware. The firmware accepts |theta| <= 3140 mrad,
 * so 180° is the largest single turn; 270° is the same final heading as 90° the other way. */
import { useEffect, useLayoutEffect, useRef, useState, type CSSProperties, type ReactNode } from 'react';
import { createPortal } from 'react-dom';
import { UiIcon } from '../ui/UiIcon';

export type TurnSide = 'cw' | 'ccw';
export interface TurnPreset { id: string; side: TurnSide; degrees: number; thetaMrad: number; label: string }
const preset = (side: TurnSide, degrees: number): TurnPreset => ({
  id: `${side}-${degrees}`, side, degrees,
  thetaMrad: (side === 'ccw' ? 1 : -1) * Math.min(3140, Math.round(degrees * Math.PI / 180 * 1000)),
  label: `${degrees}° ${side === 'ccw' ? 'a la izquierda' : 'a la derecha'}`,
});
export const TURN_PRESETS: readonly TurnPreset[] = [preset('ccw', 90), preset('ccw', 180), preset('cw', 90), preset('cw', 180)];

export function TurnPresetList({ side, available, reason, onTurnTo, onDone }: {
  side?: TurnSide; available: boolean; reason: string | null;
  onTurnTo: (thetaMrad: number) => void; onDone?: () => void;
}) {
  const items = TURN_PRESETS.filter(item => !side || item.side === side);
  return <div className="afr2-turn-presets">
    <div className="afr2-turn-presets__grid" role="group" aria-label="Giros sobre el eje">
      {items.map(item => <button key={item.id} type="button" className="afr2-turn-preset" disabled={!available} data-side={item.side}
        onClick={() => { onTurnTo(item.thetaMrad); onDone?.(); }}>
        <UiIcon name={item.side === 'ccw' ? 'rotate-ccw' : 'rotate-cw'} size={22} />
        <strong>{item.degrees}°</strong><small>{item.side === 'ccw' ? 'Izquierda' : 'Derecha'}</small>
      </button>)}
    </div>
    <p className="afr2-help afr2-turn-presets__note" role={available ? undefined : 'status'}>
      {available ? 'Gira sobre su eje hasta el ángulo, medido con los sensores Hall.' : `No disponible: ${reason || 'falta calibrar la odometría Hall.'}`}
    </p>
  </div>;
}

/** Floating panel. variant "sheet": over a blurred scrim (touch-control rail); "popover": a plain
 * floating div with no scrim that closes on any press outside it (drive pad CW/CCW).
 * mode "portal" positions next to an anchor on the page; "contained" stays inside a positioned
 * ancestor (needed inside a modal <dialog>, whose top layer would hide a body portal) and, with
 * placement "side", sits beside the anchor's rail. Escape closes both. */
export function FloatingSheet({ open, onClose, title, anchor = null, mode = 'portal', placement = 'auto', variant = 'sheet', children }: {
  open: boolean; onClose: () => void; title: string; anchor?: HTMLElement | null;
  mode?: 'portal' | 'contained'; placement?: 'auto' | 'side'; variant?: 'sheet' | 'popover'; children: ReactNode;
}) {
  const panel = useRef<HTMLDivElement>(null);
  const [style, setStyle] = useState<CSSProperties>({});
  const [shown, setShown] = useState(false);
  const closeRef = useRef(onClose); closeRef.current = onClose;
  useLayoutEffect(() => {
    if (!open) { setShown(false); return; }
    const element = panel.current, margin = 12, gap = 8;
    if (element && anchor && mode === 'portal') {
      const a = anchor.getBoundingClientRect(), p = element.getBoundingClientRect();
      const below = a.bottom + gap + p.height <= window.innerHeight - margin;
      const top = below ? a.bottom + gap : Math.max(margin, a.top - gap - p.height);
      const left = Math.min(Math.max(margin, a.left + a.width / 2 - p.width / 2), window.innerWidth - margin - p.width);
      setStyle({ top, left });
    } else if (element && anchor && mode === 'contained' && placement === 'side' && window.innerWidth >= 600) {
      // Beside the rail, vertically centred on the pressed button and kept inside the view.
      const root = (element.parentElement ?? element).getBoundingClientRect(), a = anchor.getBoundingClientRect(), p = element.getBoundingClientRect();
      const rail = (anchor.closest('.afr2-manual-rail') ?? anchor).getBoundingClientRect();
      const top = Math.min(Math.max(margin, a.top + a.height / 2 - p.height / 2 - root.top), Math.max(margin, root.height - p.height - margin));
      setStyle({ top, right: Math.max(margin, root.right - rail.left + margin) });
    } else setStyle({});
    const frame = requestAnimationFrame(() => setShown(true));
    return () => cancelAnimationFrame(frame);
  }, [open, anchor, mode, placement]);
  useEffect(() => {
    if (!open) return;
    const previous = document.activeElement as HTMLElement | null;
    panel.current?.querySelector<HTMLElement>('button:not(:disabled), [href], input')?.focus({ preventScroll: true });
    const key = (event: KeyboardEvent) => { if (event.key === 'Escape') { event.preventDefault(); event.stopPropagation(); closeRef.current(); } };
    // Popover: a press anywhere outside closes it (the anchor toggles it itself).
    const outside = (event: PointerEvent) => {
      const target = event.target as Node | null;
      if (target && (panel.current?.contains(target) || anchor?.contains(target))) return;
      closeRef.current();
    };
    window.addEventListener('keydown', key, true);
    if (variant === 'popover') window.addEventListener('pointerdown', outside, true);
    return () => {
      window.removeEventListener('keydown', key, true); window.removeEventListener('pointerdown', outside, true);
      if (previous?.isConnected) previous.focus({ preventScroll: true });
    };
  }, [open, variant, anchor]);
  if (!open) return null;
  const sheet = <div className="afr2-sheet" data-mode={mode} data-placement={placement} data-variant={variant} data-shown={shown}>
    {variant === 'sheet' && <div className="afr2-sheet__scrim" aria-hidden="true" onPointerDown={event => { event.preventDefault(); onClose(); }} />}
    <div ref={panel} className="afr2-sheet__panel" role="dialog" aria-modal={variant === 'sheet'} aria-label={title} style={style}>
      <header className="afr2-sheet__header"><h3>{title}</h3>
        <button type="button" className="afr2-icon-button" aria-label={`Cerrar ${title}`} onClick={onClose}><UiIcon name="close" /></button>
      </header>
      <div className="afr2-sheet__body">{children}</div>
    </div>
  </div>;
  return mode === 'portal' ? createPortal(sheet, document.body) : sheet;
}
