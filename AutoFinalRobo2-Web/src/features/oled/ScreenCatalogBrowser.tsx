import { useEffect, useRef, useState, type ReactNode } from 'react';
import { Chevron } from '../../components/ui/Chevron';
import { AFR2_SCREEN_META, type Afr2ScreenCode, type Afr2ScreenMetadata } from './screenCatalog';
import { FirmwareLibraryPreview } from './FirmwareLibraryPreview';

const screens = Object.values(AFR2_SCREEN_META);
const childrenOf = (code: Afr2ScreenCode) => screens.filter(screen => screen.parent === code);

/** Catalogue hierarchy only; branches never introduce a second firmware navigation. */
export function ScreenCatalogBrowser({ active = true }: { active?: boolean }) {
  const [selected, setSelected] = useState<Afr2ScreenCode>('0x010002');
  const [previewSession, setPreviewSession] = useState(0);
  const tree = useRef<HTMLElement>(null);
  const [expanded, setExpanded] = useState<Set<string>>(() => new Set(['0x010001', '0x010002', 'notices']));
  useEffect(() => {
    const ancestors: string[] = []; let parent = AFR2_SCREEN_META[selected]?.parent;
    while (parent && !ancestors.includes(parent)) { ancestors.push(parent); parent = AFR2_SCREEN_META[parent]?.parent; }
    setExpanded(previous => new Set([...previous, ...ancestors]));
  }, [selected]);
  useEffect(() => {
    const viewport = tree.current, row = viewport?.querySelector<HTMLElement>(`[data-code="${selected}"]`);
    if (!viewport || !row) return;
    const viewRect = viewport.getBoundingClientRect(), rowRect = row.getBoundingClientRect();
    if (rowRect.top < viewRect.top) viewport.scrollTop += rowRect.top - viewRect.top;
    else if (rowRect.bottom > viewRect.bottom) viewport.scrollTop += rowRect.bottom - viewRect.bottom;
  }, [selected, expanded]);
  const toggle = (code: string) => setExpanded(previous => { const next = new Set(previous); if (next.has(code)) next.delete(code); else next.add(code); return next; });
  const branch = (screen: Afr2ScreenMetadata): ReactNode => {
    const children = childrenOf(screen.code), open = expanded.has(screen.code);
    return <li key={screen.code}><div className="afr2-screen-tree-row" data-selected={selected === screen.code}>
      {children.length > 0 ? <button className="afr2-screen-tree-toggle" type="button" aria-label={`${open ? 'Plegar' : 'Desplegar'} ${screen.name}`} aria-expanded={open} aria-controls={`branch-${screen.code}`} onClick={() => toggle(screen.code)}><Chevron open={open} /></button> : <span className="afr2-screen-tree-leaf" aria-hidden="true" />}
      <button type="button" className="afr2-screen-tree-label" data-code={screen.code} aria-pressed={selected === screen.code} onClick={() => { setSelected(screen.code); setPreviewSession(value => value + 1); }}><span>{screen.name}</span><small>{screen.code}</small></button>
    </div>{children.length > 0 && open && <ul id={`branch-${screen.code}`}>{children.map(branch)}</ul>}</li>;
  };
  const current = AFR2_SCREEN_META[selected];
  return <div className="afr2-screen-catalogue">
    <nav ref={tree} className="afr2-screen-tree" aria-label="Árbol de pantallas OLED"><ul>{screens.filter(screen => !screen.parent && !screen.returnsToContext).map(branch)}
      <li><button className="afr2-screen-tree-notices" type="button" aria-expanded={expanded.has('notices')} aria-controls="screen-notices" onClick={() => toggle('notices')}><Chevron open={expanded.has('notices')} /><span>Avisos · vuelven al contexto</span></button>
        {expanded.has('notices') && <ul id="screen-notices">{screens.filter(screen => screen.returnsToContext).map(branch)}</ul>}</li>
    </ul></nav>
    <section className="afr2-screen-library" aria-label="Pantalla seleccionada"><header><p className="afr2-kicker">Biblioteca del ATmega · vista local</p><h2>{current.name}</h2><code>{current.code}</code></header>
      <FirmwareLibraryPreview key={previewSession} active={active} screen={current} onNavigate={setSelected} />
      <p className="afr2-help">{current.returnsToContext ? 'El aviso regresa al contexto anterior.' : current.parent ? `Padre: ${AFR2_SCREEN_META[current.parent]?.name ?? current.parent}.` : 'Pantalla inicial.'} Acciones: {current.actions.join(' · ')}.</p>
      {current.localCredentials && <p className="afr2-help">La clave AP se muestra solo en la OLED física; su captura remota permanece protegida.</p>}
    </section>
  </div>;
}
