import { useState } from 'react';
import { Link } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import type { ModalOrigin } from '../../components/ui/FloatingModal';
import { PageHeader } from '../../components/ui/PageHeader';
import { UiIcon } from '../../components/ui/UiIcon';
import { LineFollowerPanel } from '../control/LineFollowerPanel';
import { LineFollowerLiveView } from '../control/LineFollowerLiveView';
import { LineFollowerOverlay } from './LineFollowerOverlay';

/** AFR2-20261117-01. Sección propia del seguidor de línea: escenario grande (como el control táctil) y, debajo,
 * el mismo panel de marcha y ajuste de Programas sin repetir la vista. */
export function LineFollowerPage() {
  // AFR2-20261117-12: visor de seguimiento a pantalla completa, como el control táctil.
  const [visorOpen, setVisorOpen] = useState(false);
  const [origin, setOrigin] = useState<ModalOrigin | null>(null);
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader eyebrow="Programas" title="Seguidor de línea"
      description="La cinta queda fija al centro con su ancho real y el auto se mueve sobre ella según IR1…IR5."
      actions={<Link className="afr2-developer-back" to="/programas?tarea=linea"><UiIcon name="programs" size={16} />Ver en Programas</Link>} />
    <LineFollowerLiveView tall />
    <div className="afr2-action-row">
      <Button variant="primary" aria-haspopup="dialog" aria-expanded={visorOpen} icon={<UiIcon name="sensors" />}
        onClick={event => { setOrigin({ element: event.currentTarget, keyboard: event.detail === 0 }); setVisorOpen(true); }}>
        Abrir visor de seguimiento
      </Button>
    </div>
    <LineFollowerOverlay open={visorOpen} origin={origin} onClose={() => setVisorOpen(false)} />
    <LineFollowerPanel showView={false} />
  </div>;
}
