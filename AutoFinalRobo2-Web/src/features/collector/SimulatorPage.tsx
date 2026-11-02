import { useId } from 'react';
import { Link, Navigate, useNavigate, useParams } from 'react-router-dom';
import { PageHeader } from '../../components/ui/PageHeader';
import { SegmentedControl, type SegmentedItem } from '../../components/ui/SegmentedControl';
import { UiIcon } from '../../components/ui/UiIcon';
import { CollectorProvider, type SimulationKind } from './CollectorContext';
import { SimulationWorkspace } from './CollectorPage';
import { MODEL_PAGES } from '../models/modelCatalog';

const SIMULATORS: readonly SegmentedItem<SimulationKind>[] = [
  { value: 'auto', label: 'Auto', icon: 'control' },
  { value: 'brazo', label: 'Brazo robótico', icon: 'arm' },
  { value: 'conjunta', label: 'Conjunto', icon: 'cube' },
];
const MODEL_FOR: Record<SimulationKind, keyof typeof MODEL_PAGES> = { auto: 'auto', brazo: 'brazo', conjunta: 'conjunto' };
const isKind = (value: string | undefined): value is SimulationKind => value === 'auto' || value === 'brazo' || value === 'conjunta';

/**
 * AFR2-20261102-01. Simulador separado de la inspección 3D: una tarea por pantalla.
 * El selector cambia entre los tres simuladores (cada uno conserva su estado en memoria mientras
 * la página está abierta); «Ver modelo 3D» vuelve a la inspección del mismo modelo.
 */
export function SimulatorPage() {
  const { tipo } = useParams();
  const navigate = useNavigate();
  const uid = useId();
  if (!isKind(tipo)) return <Navigate to="/simulacion/conjunta" replace />;
  const model = MODEL_PAGES[MODEL_FOR[tipo]];
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader eyebrow="Modelos y simulación" title="Simulador" description="Simulación local con datos ficticios: no mueve el robot real."
      actions={<Link className="afr2-developer-back" to={`/modelos/${model.id}`}><UiIcon name="cube" />Ver modelo 3D</Link>}>
      <SegmentedControl variant="tabs" idPrefix={uid} ariaLabel="Elegir simulador" items={SIMULATORS} value={tipo}
        onChange={next => navigate(`/simulacion/${next}`)} />
    </PageHeader>
    <div role="tabpanel" id={`${uid}-panel-${tipo}`} aria-labelledby={`${uid}-tab-${tipo}`} className="afr2-collector">
      <CollectorProvider kind={tipo}><SimulationWorkspace key={tipo} /></CollectorProvider>
    </div>
  </div>;
}
