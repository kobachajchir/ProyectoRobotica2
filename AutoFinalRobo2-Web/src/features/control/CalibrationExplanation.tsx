import { CALIBRATION_GUIDE, explainEditorValue } from './calibrationGuide';

export function CalibrationExplanation({ selected, value, received }: {
  selected: { id: number; symbol: string; min?: number; max?: number; label?: string };
  value: number; received: number | null;
}) {
  const guide = CALIBRATION_GUIDE[selected.id];
  if (!guide) return <aside className="afr2-calibration-explanation">No hay una ficha disponible para este parámetro.</aside>;
  const valid = Number.isInteger(value) && value >= (selected.min ?? -Infinity) && value <= (selected.max ?? Infinity)
    && (selected.id !== 41 || value === 2 || value === 4);
  return <aside className="afr2-calibration-explanation" aria-labelledby="afr2-calibration-help-title">
    <header><span>{guide.group}</span><h2 id="afr2-calibration-help-title">{guide.title}</h2><p>{guide.meaning}</p></header>
    <dl className="afr2-calibration-facts">
      <div><dt>Unidad</dt><dd>{guide.unit}</dd></div>
      <div><dt>Rango admitido</dt><dd>{selected.min} … {selected.max}{selected.id === 41 ? ' · solo 2 o 4' : ''}</dd></div>
      <div><dt>Valor consultado</dt><dd>{received === null ? 'Sin consulta vigente' : received}</dd></div>
      <div><dt>Valor en el editor</dt><dd>{Number.isFinite(value) ? value : '—'}</dd></div>
    </dl>
    <section className="afr2-calibration-interpretation"><h3>Como se interpreta el valor</h3><p>{valid ? explainEditorValue(selected.id, value) : 'El editor necesita un entero admitido por el rango de este parámetro.'}</p></section>
    <section><h3>Qué cambia al ajustarlo</h3><p>{guide.effect}</p></section>
    <section><h3>Ejemplo de lectura</h3><p>{guide.example}</p></section>
    <section><h3>Relación con otros ajustes</h3><p>{guide.detail}</p></section>
  </aside>;
}
