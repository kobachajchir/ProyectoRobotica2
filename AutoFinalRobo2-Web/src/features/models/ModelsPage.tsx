import { Link } from 'react-router-dom';
import { PageHeader } from '../../components/ui/PageHeader';
import { UiIcon } from '../../components/ui/UiIcon';
import { MODEL_BANNERS, MODEL_PAGES } from './modelCatalog';

/**
 * AFR2-20261102-01. Portada de Modelos y simulación: tres banners horizontales con el mismo material
 * e iconografía que las tarjetas de Inicio. Cada uno abre la inspección 3D de su modelo; desde ahí
 * «Ir a simulación» lleva al simulador. Una decisión por pantalla en lugar de dos selectores.
 * Las imágenes son renders CAD, etiquetados como tales.
 */
export function ModelsPage() {
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader title="Modelos y simulación" description="Elegí un modelo para inspeccionarlo en 3D. Desde ahí podés pasar a su simulación." />
    <nav aria-label="Elegir modelo" className="afr2-model-banners">
      {MODEL_BANNERS.map(id => {
        const page = MODEL_PAGES[id];
        return <Link key={id} to={`/modelos/${id}`} className="afr2-model-banner">
          {page.banner && <picture className="afr2-model-banner__media">
            <source media="(min-width: 600px)" srcSet={page.banner.wide} />
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
    <p className="afr2-help afr2-model-banners__more">
      También: <Link to="/modelos/cubo">cubo auxiliar de 50 mm</Link> · <Link to="/simulacion/conjunta">ir directo al simulador</Link> · <Link to="/infraestructura">cómo llegan los modelos al navegador</Link>
    </p>
  </div>;
}
