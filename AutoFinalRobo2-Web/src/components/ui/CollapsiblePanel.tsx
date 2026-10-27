import { useId, type ReactNode } from 'react';
import { Chevron } from './Chevron';

/** Controlled disclosure card; content unmounts on close so operation-specific
 * cleanup runs just as it does when changing an existing task tab. */
export function CollapsiblePanel({ title, subtitle, meta, expanded, onToggle, children, className = '' }: {
  title: string; subtitle?: string; meta?: string; expanded: boolean; onToggle: () => void; children: ReactNode; className?: string;
}) {
  const id = useId();
  return <section className={`afr2-collapsible-panel ${className}`} data-open={expanded}>
    <button type="button" className="afr2-collapsible-panel__heading" aria-expanded={expanded} aria-controls={id}
      aria-label={`${expanded ? 'Cerrar' : 'Abrir'} ${title}`} title={!expanded ? `Abrir ${title}` : undefined} onClick={onToggle}>
      <span className="afr2-collapsible-panel__copy"><strong>{title}</strong>{subtitle && <small>{subtitle}</small>}</span>
      {meta && <span className="afr2-collapsible-panel__meta">{meta}</span>}<Chevron open={expanded} />
    </button>
    <div id={id} className="afr2-collapsible-panel__body" hidden={!expanded}>{expanded ? children : null}</div>
  </section>;
}
