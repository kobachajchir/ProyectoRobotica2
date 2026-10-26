import { useState } from 'react';
import { Chevron } from '../../components/ui/Chevron';
import { useOverlays } from '../../app/providers/OverlayContext';
import { AFR2_SCREEN_META } from './screenCatalog';
import { LiveOledViewer } from './LiveOledViewer';
import { ScreenCatalogBrowser } from './ScreenCatalogBrowser';

export function OledPage() {
  const { modalOpen } = useOverlays();
  const [catalogueOpen, setCatalogueOpen] = useState(false);
  return <div className="afr2-route-view afr2-page-stack">
    <header><p className="afr2-kicker">Interfaz del robot</p><h1 className="afr2-page-title">Pantalla OLED</h1><p className="afr2-help">El mismo visor esta siempre a mano en la parte superior de la barra lateral.</p></header>
    <LiveOledViewer active={!modalOpen} />
    <details className="afr2-disclosure" onToggle={event => setCatalogueOpen(event.currentTarget.open)}><summary><span>Pantallas y navegacion · {Object.keys(AFR2_SCREEN_META).length}</span><Chevron /></summary><div className="afr2-disclosure-body">{catalogueOpen && <ScreenCatalogBrowser active={!modalOpen} />}
      <p className="afr2-help">Catalogo compartido con Contracts: menus, vistas y avisos. El visor muestra los pixeles recibidos del ATmega, incluidos iconos y texto desplazado. La comparacion con la pantalla fisica esta pendiente.</p>
    </div></details>
  </div>;
}
