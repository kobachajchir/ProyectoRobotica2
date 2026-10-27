import { useEffect, useId, useRef, useState } from 'react';
import type { CSSProperties, PointerEvent } from 'react';

function bounded(value: number, min: number, max: number, step: number) {
  return Math.min(max, Math.max(min, min + Math.round((value - min) / step) * step));
}

interface NumberProps {
  label: string;
  value: number;
  min: number;
  max: number;
  step: number;
  unit: string;
  disabled: boolean;
  onChange: (value: number) => void;
}

/** Edits remain local until Enter/blur; the parent always receives a legal value. */
function CommittedNumber({ label, value, min, max, step, unit, disabled, onChange }: NumberProps) {
  const [text, setText] = useState(String(value));
  const [notice, setNotice] = useState('');
  const editing = useRef(false);
  const id = useId();
  useEffect(() => { editing.current = false; setText(String(value)); }, [value, disabled]);
  const commit = () => {
    if (!editing.current) return;
    editing.current = false;
    if (disabled) { setText(String(value)); return; }
    const parsed = text.trim() === '' ? NaN : Number(text.replace(',', '.'));
    const next = Number.isFinite(parsed) ? bounded(parsed, min, max, step) : value;
    setText(String(next));
    setNotice(next !== parsed ? `Valor ajustado a ${next} ${unit}.` : '');
    onChange(next);
  };
  return <span className="afr2-scan-number">
    <input className="afr2-compact-number" type="text" inputMode="decimal" aria-label={label} aria-describedby={`${id}-unit ${id}-bounds ${id}-notice`}
      value={text} disabled={disabled} autoComplete="off" spellCheck={false}
      onFocus={() => { editing.current = true; setNotice(''); }}
      onChange={event => { editing.current = true; setText(event.target.value); }}
      onBlur={commit}
      onKeyDown={event => {
        if (event.nativeEvent.isComposing) return;
        if (event.key === 'Enter') { event.preventDefault(); commit(); }
        if (event.key === 'Escape') { event.preventDefault(); event.stopPropagation(); editing.current = false; setText(String(value)); setNotice(''); }
      }} />
    <span id={`${id}-unit`} className="afr2-scan-number__unit">{unit}</span>
    <span id={`${id}-bounds`} className="afr2-sr-only">Entre {min} y {max}; paso {step}.</span>
    <span id={`${id}-notice`} className="afr2-sr-only" role="status">{notice}</span>
  </span>;
}

type RangeStyle = CSSProperties & { '--range-start'?: string; '--range-end': string };

export function ScanRangeField(props: NumberProps) {
  const { label, value, min, max, step, unit, disabled, onChange } = props;
  const id = useId();
  return <div className="afr2-scan-field" data-disabled={disabled || undefined}>
    <div className="afr2-scan-field__heading"><label htmlFor={id}>{label}</label><CommittedNumber {...props} /></div>
    <div className="afr2-scan-slider" style={{ '--range-end': `${(value - min) / (max - min) * 100}%` } as RangeStyle}>
      <span className="afr2-scan-slider__rail" aria-hidden="true"><span /></span>
      <input id={id} type="range" min={min} max={max} step={step} value={value} disabled={disabled}
        aria-valuetext={`${value} ${unit}`} onChange={event => onChange(Number(event.target.value))} />
    </div>
    <div className="afr2-scan-field__limits" aria-hidden="true"><span>{min} {unit}</span><span>{max} {unit}</span></div>
  </div>;
}

interface AngleProps {
  min: number;
  max: number;
  disabled: boolean;
  onChange: (min: number, max: number) => void;
}

export function ScanAngleRange({ min, max, disabled, onChange }: AngleProps) {
  const id = useId();
  const drag = useRef<'min' | 'max' | null>(null);
  const rail = useRef<HTMLSpanElement>(null);
  const lower = useRef<HTMLInputElement>(null), upper = useRef<HTMLInputElement>(null);
  const latest = useRef({ min, max });
  latest.current = { min, max };
  const setHandle = (handle: 'min' | 'max', value: number) => {
    const current = latest.current;
    const next = handle === 'min'
      ? { min: bounded(value, 0, current.max - 5, 5), max: current.max }
      : { min: current.min, max: bounded(value, current.min + 5, 180, 5) };
    latest.current = next;
    onChange(next.min, next.max);
  };
  const valueAt = (event: PointerEvent<HTMLDivElement>) => {
    const rect = rail.current!.getBoundingClientRect();
    return bounded((event.clientX - rect.left) / rect.width * 180, 0, 180, 5);
  };
  return <fieldset className="afr2-scan-field afr2-scan-zone" disabled={disabled}>
    <legend>Zona de escaneo</legend>
    <div className="afr2-scan-zone__values">
      <div><span>Desde</span><CommittedNumber label="Inicio del escaneo" value={min} min={0} max={max - 5} step={5} unit="°" disabled={disabled} onChange={value => setHandle('min', value)} /></div>
      <span className="afr2-scan-zone__separator" aria-hidden="true">—</span>
      <div><span>Hasta</span><CommittedNumber label="Fin del escaneo" value={max} min={min + 5} max={180} step={5} unit="°" disabled={disabled} onChange={value => setHandle('max', value)} /></div>
    </div>
    <div className="afr2-scan-slider afr2-scan-slider--dual" style={{ '--range-start': `${min / 180 * 100}%`, '--range-end': `${max / 180 * 100}%` } as RangeStyle}>
      <div className="afr2-scan-slider__hit" aria-hidden="true"
        onPointerDown={event => {
          if (disabled || !event.isPrimary || event.button !== 0) return;
          const value = valueAt(event);
          drag.current = Math.abs(value - min) <= Math.abs(value - max) ? 'min' : 'max';
          (drag.current === 'min' ? lower : upper).current?.focus({ preventScroll: true });
          event.currentTarget.setPointerCapture(event.pointerId);
          setHandle(drag.current, value);
        }}
        onPointerMove={event => { if (!disabled && drag.current) setHandle(drag.current, valueAt(event)); }}
        onPointerUp={event => { drag.current = null; if (event.currentTarget.hasPointerCapture(event.pointerId)) event.currentTarget.releasePointerCapture(event.pointerId); }}
        onPointerCancel={() => { drag.current = null; }} onLostPointerCapture={() => { drag.current = null; }} />
      <span ref={rail} className="afr2-scan-slider__rail" aria-hidden="true"><span /></span>
      <input ref={lower} type="range" min={0} max={180} step={5} value={min} disabled={disabled} aria-label="Inicio del escaneo"
        aria-describedby={`${id}-help`} aria-valuetext={`${min} grados; hasta ${max - 5}`} onChange={event => setHandle('min', Number(event.target.value))} />
      <input ref={upper} type="range" min={0} max={180} step={5} value={max} disabled={disabled} aria-label="Fin del escaneo"
        aria-describedby={`${id}-help`} aria-valuetext={`${max} grados; desde ${min + 5}`} onChange={event => setHandle('max', Number(event.target.value))} />
    </div>
    <div className="afr2-scan-field__limits" aria-hidden="true"><span>0°</span><span>90°</span><span>180°</span></div>
    <p id={`${id}-help`} className="afr2-help">Separacion minima 5° · Paso 5°</p>
  </fieldset>;
}
