import { Link } from 'react-router-dom';
import { PageHeader } from '../../components/ui/PageHeader';
import { UiIcon } from '../../components/ui/UiIcon';
import { LineFollowerPanel } from '../control/LineFollowerPanel';
import { LineFollowerLiveView } from '../control/LineFollowerLiveView';

/** AFR2-20261117-01. Sección propia del seguidor de línea: escenario grande (como el control táctil) y, debajo,
 * el mismo panel de marcha y ajuste de Programas sin repetir la vista. */
export function LineFollowerPage() {
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader eyebrow="Programas" title="Seguidor de línea"
      description="La cinta queda fija al centro con su ancho real y el auto se mueve sobre ella según IR1…IR5."
      actions={<Link className="afr2-developer-back" to="/programas?tarea=linea"><UiIcon name="programs" size={16} />Ver en Programas</Link>} />
    <LineFollowerLiveView tall />
    <LineFollowerPanel showView={false} />
  </div>;
}
