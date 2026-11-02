import { Chevron } from '../../../components/ui/Chevron';
export function CompletionFields({ completion = 'time', distanceMm = 500, durationMs, estimatedMs, disabled, allowDistance, onChange }: {
  completion?: 'time' | 'distance'; distanceMm?: number; durationMs: number; estimatedMs: number; disabled: boolean; allowDistance: boolean;
  onChange: (patch: { completion?: 'time' | 'distance'; distanceMm?: number; durationMs?: number }) => void;
}) {
  return <div className="afr2-completion-fields">
    <label>Finalizar por<span className="afr2-select-wrap"><select className="afr2-glass-field" disabled={disabled} value={completion} onChange={e => onChange({ completion: e.target.value as 'time' | 'distance', distanceMm })}><option value="time">Tiempo · ms</option>{allowDistance && <option value="distance">Distancia · mm</option>}</select><Chevron /></span></label>
    {completion === 'distance' ? <label>Recorrido de rueda (mm)<input className="afr2-glass-field" type="number" min={1} max={10000} step={1} disabled={disabled} value={distanceMm} onChange={e => onChange({ distanceMm: Math.max(1, Math.min(10000, Math.round(Number(e.target.value) || 1))) })} /><span className="afr2-help">Tiempo simulado: {estimatedMs} ms. En el robot termina por pulsos Hall; π × diámetro / pulsos por vuelta. En chasis, referencia: rueda de mayor recorrido.</span></label>
      : <label>Duración (ms)<input className="afr2-glass-field" type="number" min={10} max={600000} step={10} disabled={disabled} value={durationMs} onChange={e => onChange({ durationMs: Math.max(10, Math.min(600000, Math.round((Number(e.target.value) || 10) / 10) * 10)) })} /></label>}
  </div>;
}
