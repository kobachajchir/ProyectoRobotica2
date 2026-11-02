import { memo, useId, useState } from 'react';
import { Chevron } from '../../../components/ui/Chevron';
import { UiIcon } from '../../../components/ui/UiIcon';
import { GOTO_MAX_COMMANDS, gotoCommandLabel, type GotoCommand } from './gotoSimulation';
import { useActiveCommandScroll } from './useActiveCommandScroll';

export const GotoCommandList = memo(function GotoCommandList({ commands, activeIndex, completed, playing, follow, onFollow, onReorder, onRemove }: {
  commands: readonly GotoCommand[]; activeIndex: number | null; completed: number; playing: boolean;
  follow: boolean; onFollow: (value: boolean) => void; onReorder: (index: number, delta: number) => void; onRemove: (id: string) => void;
}) {
  const [expanded, setExpanded] = useState(false);
  const uid = useId();
  const displayIndex = activeIndex ?? (completed > 0 ? Math.min(completed - 1, commands.length - 1) : 0);
  const activeId = commands[displayIndex]?.id ?? null;
  const { viewportRef, listRef, interrupt } = useActiveCommandScroll(activeId, expanded ? follow : true, playing, expanded);
  const manualScroll = () => { interrupt(); onFollow(false); };
  return <>
    <div className="afr2-goto-list-heading"><button type="button" className="afr2-goto-list-toggle" aria-expanded={expanded} aria-controls={uid} onClick={() => { interrupt(); setExpanded(value => !value); }}><span>Recorrido <span className="afr2-tabular">{commands.length}/{GOTO_MAX_COMMANDS}</span></span><Chevron open={expanded} /></button>
      {expanded && <label><input type="checkbox" checked={follow} onChange={event => onFollow(event.target.checked)} /> Seguir activo</label>}
    </div>
    <div id={uid} ref={viewportRef} className="afr2-goto-scroll" data-expanded={expanded} tabIndex={expanded ? 0 : -1} aria-label="Lista de comandos en orden"
      onWheel={() => { if (expanded) manualScroll(); }} onTouchStart={() => { if (expanded) manualScroll(); }}
      onKeyDown={event => { if (expanded && ['ArrowDown', 'ArrowUp', 'PageDown', 'PageUp', 'Home', 'End', ' '].includes(event.key)) manualScroll(); }}>
      <ol ref={listRef} className="afr2-goto-command-list" role="list">
        {commands.map((command, index) => {
          const active = activeIndex === index, label = gotoCommandLabel(command);
          return <li key={command.id} data-command-id={command.id} data-active={active} aria-current={active ? 'step' : undefined} aria-hidden={!expanded && Math.abs(index - displayIndex) > 1 ? true : undefined}>
            <span className="afr2-goto-row-number afr2-tabular" aria-hidden="true">{index < completed ? <UiIcon name="check" size={16} /> : index + 1}</span>
            <div className="afr2-goto-row-content"><span className="afr2-goto-row-label"><span>{label}</span><span className="afr2-goto-row-accent" aria-hidden="true">{label}</span></span>
              <span className="afr2-goto-row-state">{index < completed ? 'Completado' : active ? playing ? 'En curso' : 'En pausa' : 'Pendiente'}</span>
            </div>
            {expanded && <div className="afr2-goto-row-actions">
              <button type="button" disabled={playing || index === 0} aria-label={`Subir comando ${index + 1}`} onClick={() => onReorder(index, -1)}><UiIcon name="arrow-up" /></button>
              <button type="button" disabled={playing || index === commands.length - 1} aria-label={`Bajar comando ${index + 1}`} onClick={() => onReorder(index, 1)}><UiIcon name="arrow-down" /></button>
              <button type="button" disabled={playing} aria-label={`Quitar comando ${index + 1}: ${label}`} onClick={() => onRemove(command.id)}><UiIcon name="close" /></button>
            </div>}
          </li>;
        })}
      </ol>
      {!commands.length && <p className="afr2-goto-empty">Elegí un punto del mapa o agregá el primer movimiento.</p>}
    </div>
    {expanded && <p className="afr2-help">El desplazamiento manual desactiva «Seguir activo». Editar la lista reinicia la vista previa.</p>}
  </>;
});
