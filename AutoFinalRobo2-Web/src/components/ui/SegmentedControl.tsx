import { useCallback, useTransition, useEffect, useLayoutEffect, useRef, useState, type KeyboardEvent, type ReactNode } from 'react';
import { UiIcon, type UiIconName } from './UiIcon';

export interface SegmentedItem<T extends string = string> {
  value: T;
  label: ReactNode;
  icon?: UiIconName;
  disabled?: boolean;
  title?: string;
}

export interface SegmentedControlProps<T extends string = string> {
  items: readonly SegmentedItem<T>[];
  value: T;
  onChange: (value: T) => void;
  ariaLabel: string;
  /** tabs: cambia el panel de la página (tablist). choice: elige vista, rango, tema o modo (radiogroup). */
  variant?: 'tabs' | 'choice';
  /** md 44 px; sm 36 px solo con puntero fino (en táctil vuelve a 44 px). */
  size?: 'md' | 'sm';
  /** auto: las flechas eligen. manual: las flechas solo mueven el foco y Enter/Espacio elige.
   * Usar manual cuando elegir envía una orden al robot. */
  activation?: 'auto' | 'manual';
  /** Con 2 a 4 opciones, en celular se reparten en columnas iguales a todo el ancho. */
  fill?: 'mobile' | 'always' | 'never';
  /** Volver a tocar la opción elegida también llama a onChange (p. ej. reenviar un modo). */
  reselect?: boolean;
  /** tabs: ids `${idPrefix}-tab-${value}` y aria-controls `${idPrefix}-panel-${value}`. */
  idPrefix?: string;
  className?: string;
}

/**
 * AFR2-20261101-09 / VIS-052. Ranura con pill deslizante, única implementación de
 * selector segmentado. La pill es una copia de la fila con estilo activo, recortada con
 * clip-path (receta tab indicator de animate): fondo y texto cambian juntos.
 * 250 ms con --ease-in-out; sin transición en el primer render, al cambiar de tamaño,
 * con teclado ni con movimiento reducido (--duration-segmented vale 0 ms).
 */
export function SegmentedControl<T extends string>({
  items, value, onChange, ariaLabel, variant = 'choice', size = 'md', activation = 'auto',
  fill = 'mobile', reselect = false, idPrefix, className = '',
}: SegmentedControlProps<T>) {
  // La pill sigue al toque de inmediato; el contenido que depende de `value` (a veces una escena 3D)
  // se actualiza después como transición, para que no retrase el movimiento.
  // Si el cambio no se acepta, al terminar la transición vuelve a mandar el valor real.
  const [pending, setPending] = useState<T | null>(null);
  const [transitioning, startChange] = useTransition();
  const shown = transitioning && pending !== null ? pending : value;
  const rootRef = useRef<HTMLDivElement>(null);
  const copyRef = useRef<HTMLDivElement>(null);
  const buttons = useRef(new Map<T, HTMLButtonElement>());
  const animateNext = useRef(true);
  const [ready, setReady] = useState(false);
  const tabs = variant === 'tabs';
  const fillMode = fill === 'mobile' && items.length > 4 ? 'never' : fill;

  const place = useCallback((animate: boolean) => {
    const root = rootRef.current, copy = copyRef.current;
    if (!root || !copy) return;
    const button = buttons.current.get(shown);
    if (!animate) copy.style.transition = 'none';
    if (!button) {
      copy.style.clipPath = 'inset(0 100% 0 0 round 9999px)';
    } else {
      const copyBox = copy.getBoundingClientRect(), box = button.getBoundingClientRect();
      const left = Math.max(0, box.left - copyBox.left);
      const right = Math.max(0, copyBox.right - box.right);
      // Escrito en el elemento recortado, no en una variable del contenedor.
      copy.style.clipPath = `inset(0 ${right.toFixed(2)}px 0 ${left.toFixed(2)}px round 9999px)`;
      // Traer la opción activa a la vista dentro de la ranura, sin desplazar la página.
      const start = button.offsetLeft - 8, end = button.offsetLeft + button.offsetWidth + 8;
      if (start < root.scrollLeft) root.scrollLeft = start;
      else if (end > root.scrollLeft + root.clientWidth) root.scrollLeft = end - root.clientWidth;
    }
    if (!animate) { void copy.offsetWidth; copy.style.transition = ''; }
  }, [shown]);

  useLayoutEffect(() => {
    place(ready && animateNext.current);
    animateNext.current = true;
  }, [place, ready, items]);

  // Primer render sin transición: se habilita después del primer frame.
  useEffect(() => {
    const frame = requestAnimationFrame(() => setReady(true));
    return () => cancelAnimationFrame(frame);
  }, []);

  // Rotación, barra lateral plegada o fuente distinta: reubicar sin animar.
  // Un solo observador por montaje: recrearlo en cada cambio de valor disparaba su aviso
  // inicial y cortaba la transición (hallado en la revisión en navegador, AFR2-20261102-01).
  const placeRef = useRef(place);
  placeRef.current = place;
  useEffect(() => {
    const root = rootRef.current;
    if (!root || typeof ResizeObserver === 'undefined') return;
    let initial = true;
    const observer = new ResizeObserver(() => {
      if (initial) { initial = false; return; }
      placeRef.current(false);
    });
    observer.observe(root);
    return () => observer.disconnect();
  }, []);

  const enabled = items.filter(item => !item.disabled);
  const focusValue = (next: T) => buttons.current.get(next)?.focus();
  const choose = (next: T, fromKeyboard: boolean) => {
    if (next === shown && !reselect) return;
    if (next !== shown) { animateNext.current = !fromKeyboard; setPending(next); }
    startChange(() => onChange(next));
  };
  const onKeyDown = (event: KeyboardEvent<HTMLButtonElement>, current: T) => {
    const index = enabled.findIndex(item => item.value === current);
    let next: number;
    if (event.key === 'ArrowRight' || event.key === 'ArrowDown') next = (index + 1) % enabled.length;
    else if (event.key === 'ArrowLeft' || event.key === 'ArrowUp') next = (index - 1 + enabled.length) % enabled.length;
    else if (event.key === 'Home') next = 0;
    else if (event.key === 'End') next = enabled.length - 1;
    else return;
    event.preventDefault();
    const target = enabled[next];
    if (!target) return;
    if (activation === 'auto') choose(target.value, true);
    focusValue(target.value);
  };
  const focusable = items.some(item => item.value === shown && !item.disabled) ? shown : enabled[0]?.value;

  const content = (item: SegmentedItem<T>) => <span className="afr2-seg__label">
    {item.icon && <UiIcon name={item.icon} size={16} />}<span>{item.label}</span>
  </span>;

  return <div ref={rootRef} className={`afr2-seg ${className}`} data-size={size} data-fill={fillMode} data-ready={ready}>
    <div className="afr2-seg__row" role={tabs ? 'tablist' : 'radiogroup'} aria-label={ariaLabel}>
      {items.map(item => {
        const selected = item.value === shown;
        return <button
          key={item.value}
          ref={node => { if (node) buttons.current.set(item.value, node); else buttons.current.delete(item.value); }}
          type="button"
          className="afr2-seg__item"
          role={tabs ? 'tab' : 'radio'}
          id={tabs && idPrefix ? `${idPrefix}-tab-${item.value}` : undefined}
          aria-controls={tabs && idPrefix ? `${idPrefix}-panel-${item.value}` : undefined}
          aria-selected={tabs ? selected : undefined}
          aria-checked={tabs ? undefined : selected}
          tabIndex={item.value === focusable ? 0 : -1}
          disabled={item.disabled}
          title={item.title}
          onClick={event => choose(item.value, event.detail === 0)}
          onKeyDown={event => onKeyDown(event, item.value)}
        >{content(item)}</button>;
      })}
    </div>
    <div className="afr2-seg__glow" aria-hidden="true">
      <div ref={copyRef} className="afr2-seg__row afr2-seg__active">
        {items.map(item => <span key={item.value} className="afr2-seg__ghost">{content(item)}</span>)}
      </div>
    </div>
  </div>;
}
