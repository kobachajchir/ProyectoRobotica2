import { useEffect, useId, useState } from 'react';

/** Compact, synchronized number/range pair shared by operation and simulation. */
export function RangeNumberField({ label, value, min, max, step = 1, unit = '', disabled = false, onChange }: {
  label: string; value: number; min: number; max: number; step?: number; unit?: string;
  disabled?: boolean; onChange: (value: number) => void;
}) {
  const [draft, setDraft] = useState(String(Number(value.toFixed(1))));
  const id = useId(), unavailable = disabled || max < min;
  useEffect(() => { setDraft(String(Number(value.toFixed(1)))); }, [value]);
  const change = (text: string) => {
    setDraft(text);
    if (!unavailable && text.trim() && Number.isFinite(Number(text))) onChange(Math.max(min, Math.min(max, Number(text))));
  };
  const commit = () => { change(draft); setDraft(String(Number(value.toFixed(1)))); };
  return <div className="afr2-range-number"><div className="afr2-range-number__heading"><label htmlFor={id}>{label}</label>
    <span className="afr2-range-number__value"><input className="afr2-compact-number" aria-label={`${label} valor`} type="number"
      min={min} max={max} step={step} value={draft} disabled={unavailable} onChange={event => change(event.target.value)} onBlur={commit}
      onKeyDown={event => { if (event.key === 'Enter') { event.preventDefault(); commit(); } }} /><span>{unit}</span></span>
  </div><input id={id} aria-label={`${label} deslizador`} aria-valuetext={`${value} ${unit}`} type="range" min={min} max={max} step={step}
    value={value} disabled={unavailable} onChange={event => onChange(Number(event.target.value))} /></div>;
}
