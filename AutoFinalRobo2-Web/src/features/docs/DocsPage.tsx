import { useSearchParams } from 'react-router-dom';
import { useOverlays } from '../../app/providers/OverlayContext';
import { AFR2_SCREEN_META } from '../oled/screenCatalog';
import { ScreenCatalogBrowser } from '../oled/ScreenCatalogBrowser';
import { ScreenTreeMap } from '../developer/ScreenTreeMap';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import { PageHeader } from '../../components/ui/PageHeader';
import { TaskSections } from '../../components/ui/TaskSections';
import { Disclosure } from '../../components/ui/Disclosure';
import type { ReactNode } from 'react';
import { HelpRich } from './HelpRich';
import { HELP, type HelpChapter, type HelpModuleId } from './helpContent';

type HelpSection = HelpModuleId;
// AFR2-20261102-23: orden ATmega → ESP → Web → OLED.
const SECTIONS: readonly HelpSection[] = ['atmega', 'esp', 'web', 'oled'];
const SECTION_LABELS: Record<HelpSection, string> = { atmega: 'ATmega', esp: 'ESP', web: 'Web', oled: 'OLED' };

function ChapterBody({ chapter }: { chapter: HelpChapter }) {
  return <div className="afr2-help-chapter">
    <ul className="afr2-help-chapter__text">{chapter.text.map(text => <li key={text}><HelpRich text={text} /></li>)}</ul>
    {chapter.facts && <dl className="afr2-help-facts">{chapter.facts.map(([term, value]) =>
      <div key={term}><dt><HelpRich text={term} /></dt><dd><HelpRich text={value} /></dd></div>)}</dl>}
  </div>;
}

function ModuleGuide({ id, children }: { id: HelpSection; children?: ReactNode }) {
  const module = HELP[id];
  return <div className="afr2-page-stack">
    <section className="afr2-help-hero" aria-label={`${module.name}: ${module.role}`}>
      <div className="afr2-help-hero__layout">
        <div className="afr2-help-hero__main">
          <span className="afr2-help-hero__icon" aria-hidden="true"><UiIcon name={module.icon} size={26} /></span>
          <div className="afr2-help-hero__copy">
            <p className="afr2-help-hero__role">{module.role}</p>
            <h2 className="afr2-help-hero__name">{module.name}</h2>
            <p className="afr2-help-hero__intro">{module.intro}</p>
          </div>
          <dl className="afr2-help-hero__specs">{module.specs.map(([term, value]) => <div key={term}><dt>{term}</dt><dd><HelpRich text={value} /></dd></div>)}</dl>
        </div>
        <aside className="afr2-help-pinout" aria-label={module.pinout.title}>
          <h3 className="afr2-help-pinout__title">{module.pinout.title}</h3>
          {module.pinout.groups.map(group => <div key={group.label} className="afr2-help-pinout__group">
            <p className="afr2-help-pinout__label">{group.label}</p>
            <dl>{group.rows.map(([term, value]) => <div key={term}><dt>{term}</dt><dd><HelpRich text={value} /></dd></div>)}</dl>
          </div>)}
        </aside>
      </div>
    </section>
    {module.groups.map(group => <section key={group.title} className="afr2-help-group" aria-label={group.title}>
      <h2 className="afr2-help-group__title">{group.title}</h2>
      <div className="afr2-help-group__list">{group.chapters.map(chapter =>
        <Disclosure key={chapter.title} title={chapter.title}><ChapterBody chapter={chapter} /></Disclosure>)}</div>
    </section>)}
    {children}
  </div>;
}

/** AFR2-20261101-12 / VIS-053. Ayuda: documentación por módulo, catálogo de pantallas OLED y reglas
 * de supervisión. `?seccion=atmega|esp|web|oled` (AFR2-20261102-23: atmega por defecto). El visor en vivo sigue como modal global. */
export function DocsPage() {
  const [params, setParams] = useSearchParams();
  const requested = params.get('seccion') as HelpSection | null;
  const section: HelpSection = requested && SECTIONS.includes(requested) ? requested : 'atmega';
  const { modalOpen } = useOverlays();
  // AFR2-20261102-02: dentro de OLED, la lista (con vista previa) y el árbol conceptual son dos vistas
  // del mismo catálogo: ?vista=arbol. Elegir una pantalla en el árbol la abre en la lista (?pantalla=).
  const treeView = section === 'oled' && params.get('vista') === 'arbol';
  const showTree = () => setParams({ seccion: 'oled', vista: 'arbol' });
  const openInList = (code: string) => setParams({ seccion: 'oled', pantalla: code });
  return <div className="afr2-route-view afr2-page-stack"><PageHeader title="Ayuda" description="Referencia técnica de cada módulo: hardware, protocolos, límites y pantallas." />
    <TaskSections label="Ayuda por módulo" value={section} onValueChange={id => setParams({ seccion: id }, { replace: true })} items={SECTIONS.map(id => ({ id, label: SECTION_LABELS[id], content: id === 'oled'
      ? <ModuleGuide id="oled"><section className="afr2-help-group" aria-label="Catálogo de pantallas">
          <h2 className="afr2-help-group__title">Catálogo de pantallas</h2>
          <p className="afr2-help">Catálogo compartido con Contracts: {Object.keys(AFR2_SCREEN_META).length} menús, vistas y avisos. Para ver la pantalla del robot en vivo usá «Pantalla en vivo» en la barra lateral o en «Más». La comparación con la pantalla física está pendiente.</p>
          {treeView
            ? <section className="afr2-page-stack" aria-labelledby="afr2-screen-tree-title">
                <div className="afr2-screen-list__heading"><h3 id="afr2-screen-tree-title" className="afr2-subheading">Árbol de pantallas</h3>
                  <Button size="sm" icon={<UiIcon name="back" />} onClick={() => setParams({ seccion: 'oled' })}>Ver lista</Button></div>
                <ScreenTreeMap onOpen={openInList} openLabel="Ver en la lista" />
              </section>
            : <ScreenCatalogBrowser key={params.get('pantalla') ?? 'inicio'} active={!modalOpen} initialCode={params.get('pantalla')} onShowTree={showTree} />}
        </section></ModuleGuide>
      : <ModuleGuide id={id} /> }))} />
  </div>;
}
