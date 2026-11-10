import { Link, useNavigate } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { PageHeader } from '../../components/ui/PageHeader';
import { UiIcon } from '../../components/ui/UiIcon';
import { MODEL_BANNERS, MODEL_PAGES } from './modelCatalog';

const actionStyle = {
  backgroundColor: 'var(--model-action-bg)',
  color: 'var(--model-action-ink)',
  borderColor: 'var(--model-action-border)',
};

/**
 * AFR2-20261102-15. Portada de Modelos y simulación: tres tarjetas con render a sangre y texto superpuesto,
 * e iconografía que las tarjetas de Inicio. Cada uno abre la inspección 3D de su modelo; desde ahí
 * «Ir a simulación» lleva al simulador. Una decisión por pantalla en lugar de dos selectores.
 * Las imágenes son renders CAD, etiquetados como tales.
 */
export function ModelsPage() {
  const navigate = useNavigate();
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader title="Modelos y simulación" description="Elegí un modelo para inspeccionarlo en 3D. Desde ahí podés pasar a su simulación." />
    <nav aria-label="Elegir modelo" className="afr2-model-banners">
      {MODEL_BANNERS.map(id => {
        const page = MODEL_PAGES[id];
        return <Link key={id} to={`/modelos/${id}`} className="afr2-model-banner">
          {page.banner && <picture className="afr2-model-banner__media">
            <img src={page.banner.narrow} alt={page.banner.alt} loading="lazy" decoding="async" />
          </picture>}
          <div className="afr2-model-banner__copy">
            <span className="afr2-section-symbol"><UiIcon name={page.icon} /></span>
            <p className="afr2-model-banner__kicker">{page.kicker}</p>
            <h2>{page.title}</h2>
            <p>{page.description}</p>
            <span className="afr2-section-entry" aria-hidden="true">Inspeccionar modelo 3D<UiIcon name="arrow" /></span>
          </div>
          {page.banner && <span className="afr2-model-banner__tag">Render CAD</span>}
        </Link>;
      })}
    </nav>
    <div className="afr2-action-row afr2-model-actions" role="group" aria-label="Explorar más">
      <Button variant="secondary" style={actionStyle} icon={<UiIcon name="arm" />} onClick={() => navigate('/armado')}>Armado del robot<UiIcon name="arrow" /></Button>
      <Button variant="secondary" style={actionStyle} icon={<UiIcon name="control" />} onClick={() => navigate('/simulacion/conjunta')}>Simulador<UiIcon name="arrow" /></Button>
      <Button variant="secondary" style={actionStyle} icon={<UiIcon name="board" />} onClick={() => navigate('/infraestructura')}>Explorar infraestructura<UiIcon name="arrow" /></Button>
    </div>
  </div>;
}
