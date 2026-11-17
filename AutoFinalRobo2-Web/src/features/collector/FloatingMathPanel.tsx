/* AFR2-20261117-13. Panel flotante y arrastrable para la matemática de los simuladores.
 * Posición fija en pantalla (no se pierde al hacer scroll) y limitada al área de contenido (#afr2-main), nunca sobre la
 * barra lateral ni la superior. Se arrastra desde el encabezado (puntero o flechas con el encabezado enfocado) y recuerda
 * la posición en este navegador. */
import { useCallback, useEffect, useLayoutEffect, useRef, useState, type KeyboardEvent, type PointerEvent, type ReactNode } from 'react';
import { createPortal } from 'react-dom';
import { UiIcon } from '../../components/ui/UiIcon';

const STORAGE_KEY = 'afr2.sim.mathPanel.position.v1';
const MARGIN = 12;
type Pos = { x: number; y: number };

/** Área de contenido visible: #afr2-main recortado a la ventana, debajo de la barra superior fija y encima de la
 * barra de navegación inferior del celular (las dos quedan siempre libres). */
function visibleEdge(selector: string, side: 'bottom' | 'top') {
  const el = document.querySelector(selector);
  if (!el || getComputedStyle(el).display === 'none') return null;
  const r = el.getBoundingClientRect();
  return r.height > 0 ? r[side] : null;
}
function contentBounds() {
  const main = document.getElementById('afr2-main')?.getBoundingClientRect();
  const topbar = visibleEdge('.afr2-topbar', 'bottom'), bottomNav = visibleEdge('.afr2-bottom-nav', 'top');
  const left = Math.max(0, main?.left ?? 0);
  const top = Math.max(0, main?.top ?? 0, topbar ?? 0);
  const right = Math.min(window.innerWidth, main?.right ?? window.innerWidth);
  const bottom = Math.min(window.innerHeight, main?.bottom ?? window.innerHeight, bottomNav ?? window.innerHeight);
  return { left, top, right, bottom };
}

function readStored(): Pos | null {
  try {
    const p = JSON.parse(localStorage.getItem(STORAGE_KEY) ?? 'null') as Pos | null;
    return p && Number.isFinite(p.x) && Number.isFinite(p.y) ? p : null;
  } catch { return null; }
}

export function FloatingMathPanel({ open, onClose, title, subtitle, children }: {
  open: boolean; onClose: () => void; title: string; subtitle?: string; children: ReactNode;
}) {
  const panel = useRef<HTMLDivElement>(null);
  const drag = useRef<{ id: number; dx: number; dy: number } | null>(null);
  const [pos, setPos] = useState<Pos | null>(null);
  const [dragging, setDragging] = useState(false);

  const clamp = useCallback((p: Pos): Pos => {
    const b = contentBounds(), el = panel.current;
    const w = el?.offsetWidth ?? 360, h = el?.offsetHeight ?? 200;
    return {
      x: Math.round(Math.min(Math.max(p.x, b.left + MARGIN), Math.max(b.left + MARGIN, b.right - w - MARGIN))),
      y: Math.round(Math.min(Math.max(p.y, b.top + MARGIN), Math.max(b.top + MARGIN, b.bottom - h - MARGIN))),
    };
  }, []);

  // Al abrir: la última posición guardada o, la primera vez, arriba a la derecha del área de contenido.
  useLayoutEffect(() => {
    if (!open) return;
    const b = contentBounds(), w = panel.current?.offsetWidth ?? 360;
    setPos(clamp(readStored() ?? { x: b.right - w - MARGIN, y: b.top + MARGIN }));
  }, [open, clamp]);

  // Sigue dentro del área si cambia la ventana, el scroll o el tamaño del propio panel.
  useEffect(() => {
    if (!open) return;
    const fit = () => setPos(p => (p ? clamp(p) : p));
    window.addEventListener('resize', fit);
    window.addEventListener('scroll', fit, true);
    const observer = typeof ResizeObserver !== 'undefined' ? new ResizeObserver(fit) : null;
    if (panel.current) observer?.observe(panel.current);
    return () => { window.removeEventListener('resize', fit); window.removeEventListener('scroll', fit, true); observer?.disconnect(); };
  }, [open, clamp]);

  const store = (p: Pos) => { try { localStorage.setItem(STORAGE_KEY, JSON.stringify(p)); } catch { /* sin almacenamiento */ } };

  const onPointerDown = (event: PointerEvent<HTMLElement>) => {
    if (event.button !== 0 || !pos || (event.target as Element).closest('button')) return;
    event.preventDefault();
    event.currentTarget.setPointerCapture(event.pointerId);
    drag.current = { id: event.pointerId, dx: event.clientX - pos.x, dy: event.clientY - pos.y };
    setDragging(true);
  };
  const onPointerMove = (event: PointerEvent<HTMLElement>) => {
    const d = drag.current;
    if (!d || d.id !== event.pointerId) return;
    setPos(clamp({ x: event.clientX - d.dx, y: event.clientY - d.dy }));
  };
  const endDrag = (event: PointerEvent<HTMLElement>) => {
    if (!drag.current || drag.current.id !== event.pointerId) return;
    drag.current = null; setDragging(false);
    if (pos) store(pos);
  };
  const onKeyDown = (event: KeyboardEvent<HTMLElement>) => {
    const step = event.shiftKey ? 48 : 16;
    const delta = event.key === 'ArrowLeft' ? [-step, 0] : event.key === 'ArrowRight' ? [step, 0]
      : event.key === 'ArrowUp' ? [0, -step] : event.key === 'ArrowDown' ? [0, step] : null;
    if (!delta || !pos || (event.target as Element).closest('button')) return;
    event.preventDefault();
    const next = clamp({ x: pos.x + delta[0], y: pos.y + delta[1] });
    setPos(next); store(next);
  };

  if (!open) return null;
  return createPortal(
    <section ref={panel} className="afr2-math-float" role="dialog" aria-modal="false" aria-label={title} data-dragging={dragging || undefined}
      style={pos ? { left: pos.x, top: pos.y } : { visibility: 'hidden' }}>
      <header className="afr2-math-float__header" tabIndex={0} aria-roledescription="Barra para mover el panel"
        title="Arrastrá para mover · flechas con el encabezado enfocado"
        onPointerDown={onPointerDown} onPointerMove={onPointerMove} onPointerUp={endDrag} onPointerCancel={endDrag} onKeyDown={onKeyDown}>
        <span className="afr2-math-float__grip" aria-hidden="true"><UiIcon name="move" size={16} /></span>
        <span className="afr2-math-float__title"><strong>{title}</strong>{subtitle && <small>{subtitle}</small>}</span>
        <button type="button" className="afr2-math-float__close" aria-label="Ocultar matemáticas" title="Ocultar matemáticas" onClick={onClose}>
          <UiIcon name="close" size={18} />
        </button>
      </header>
      <div className="afr2-math-float__body">{children}</div>
    </section>,
    document.body,
  );
}
