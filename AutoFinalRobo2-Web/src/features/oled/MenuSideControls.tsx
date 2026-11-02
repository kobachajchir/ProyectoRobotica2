/** AFR2-20261024-14. Adapted from AutoMicro ScreenStreamWorkspace's visible
 * row selector and up/down SVG controls. AFR2 uses encoder rotations, not F4 wire. */
import { UiIcon } from '../../components/ui/UiIcon';
export function MenuSideControls({ selection, firstVisible, count, visibleRows, disabled, onHighlight, onScroll, onBack }: {
  selection: number; firstVisible: number; count: number | null; visibleRows: number; disabled: boolean;
  onBack?: () => void;
  onHighlight: (index: number) => void; onScroll: (direction: -1 | 1) => void;
}) {
  if (count === null || count < 2) return onBack ? <aside className="afr2-oled-side" aria-label="Volver al menú anterior"><button type="button" className="afr2-oled-back" disabled={disabled} onClick={onBack}>Volver</button></aside> : null;
  const rows = Math.max(0, Math.min(visibleRows, count - firstVisible));
  return <aside className="afr2-oled-side" aria-label="Selección y desplazamiento del menú">
    <div className="afr2-oled-row-selector" role="group" aria-label="Resaltar opción visible">
      {Array.from({ length: rows }, (_, row) => <button key={row} type="button" disabled={disabled}
        aria-label={`Resaltar opción ${firstVisible + row + 1}`} aria-pressed={selection === firstVisible + row}
        title={`Resaltar opción ${firstVisible + row + 1}; SW corto confirma`}
        onClick={() => onHighlight(firstVisible + row)}>{row + 1}</button>)}
    </div>
    {count > visibleRows && <div className="afr2-oled-page-controls" role="group" aria-label="Desplazar la selección">
      <button type="button" disabled={disabled || firstVisible === 0} aria-label="Subir hasta tres opciones" title="Subir hasta tres opciones" onClick={() => onScroll(-1)}><UiIcon name="chevron-up" /></button>
      <span className="afr2-tabular">{firstVisible + 1}–{firstVisible + rows}<small>de {count}</small></span>
      <button type="button" disabled={disabled || firstVisible + visibleRows >= count} aria-label="Bajar hasta tres opciones" title="Bajar hasta tres opciones" onClick={() => onScroll(1)}><UiIcon name="chevron-down" /></button>
    </div>}
    {onBack && <button type="button" className="afr2-oled-back" disabled={disabled} onClick={onBack}>Volver</button>}
  </aside>;
}
