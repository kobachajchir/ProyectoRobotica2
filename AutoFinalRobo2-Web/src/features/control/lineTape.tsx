/* AFR2-20261117-11. Ancho de la cinta del seguidor de línea: preferencia de este navegador (solo cambia el dibujo),
 * compartida entre el campo de «Ajuste y calibración» y la vista en vivo, y con Sensores → Línea IR. */
import { useEffect, useState, useSyncExternalStore } from 'react';
import { LINE_TAPE_DEFAULT_MM, LINE_TAPE_MAX_MM, LINE_TAPE_MIN_MM } from '../sensors/LineTopView';

export const LINE_TAPE_STORAGE_KEY = 'afr2.sensors.lineTapeMm';
const LINE_TAPE_EVENT = 'afr2-line-tape-changed';
export const clampTape = (mm: number) => Math.max(LINE_TAPE_MIN_MM, Math.min(LINE_TAPE_MAX_MM, mm));

export function readLineTapeMm(): number {
  try {
    const value = Number(localStorage.getItem(LINE_TAPE_STORAGE_KEY));
    return Number.isFinite(value) && value > 0 ? clampTape(value) : LINE_TAPE_DEFAULT_MM;
  } catch { return LINE_TAPE_DEFAULT_MM; }
}
export function storeLineTapeMm(mm: number) {
  try { localStorage.setItem(LINE_TAPE_STORAGE_KEY, String(clampTape(mm))); } catch { /* sin almacenamiento */ }
  window.dispatchEvent(new Event(LINE_TAPE_EVENT));
}

function subscribe(onChange: () => void) {
  window.addEventListener(LINE_TAPE_EVENT, onChange);
  window.addEventListener('storage', onChange);
  return () => { window.removeEventListener(LINE_TAPE_EVENT, onChange); window.removeEventListener('storage', onChange); };
}
export function useLineTapeMm(): number {
  return useSyncExternalStore(subscribe, readLineTapeMm, () => LINE_TAPE_DEFAULT_MM);
}

/** Campo del ancho de la cinta (mm); se ubica en «Ajuste y calibración de línea». */
export function LineTapeField() {
  const tapeMm = useLineTapeMm();
  const [text, setText] = useState(() => String(tapeMm));
  useEffect(() => { setText(String(tapeMm)); }, [tapeMm]);
  const commit = (value: string) => {
    setText(value);
    const mm = Number(value);
    if (value.trim() !== '' && Number.isFinite(mm) && mm >= LINE_TAPE_MIN_MM && mm <= LINE_TAPE_MAX_MM) storeLineTapeMm(mm);
  };
  return <label className="afr2-line-live__tape">Ancho de la cinta (mm)
    <input className="afr2-glass-field" type="number" inputMode="numeric" min={LINE_TAPE_MIN_MM} max={LINE_TAPE_MAX_MM} step={1}
      value={text} onChange={event => commit(event.target.value)} onBlur={() => setText(String(tapeMm))} />
    <span className="afr2-help">Escala real; se comparte con Sensores → Línea IR. Solo cambia el dibujo.</span>
  </label>;
}
