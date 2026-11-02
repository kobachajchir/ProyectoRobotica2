import { ModelAssetProgress } from '../../components/three/ModelAssetProgress';
import { Disclosure } from '../../components/ui/Disclosure';
import React, { Component, Suspense, useState } from 'react';
import { clearRobotModelCache } from '../../components/three/RobotModel';
import { PageHeader } from '../../components/ui/PageHeader';
import { UiIcon } from '../../components/ui/UiIcon';
import { Link, Navigate, useParams } from 'react-router-dom';
import { MODEL_PAGES, isModelPageId, type ModelPageId } from '../models/modelCatalog';
import { Button } from '../../components/ui/Button';
import { ModelInspection } from './ModelInspection';
import { clearInspectionAssetCache } from './AssetInspection';

interface ViewerErrorBoundaryState {
  hasError: boolean;
  errorMessage: string | null;
}

class ViewerErrorBoundary extends Component<
  { onReset: () => void; children: React.ReactNode },
  ViewerErrorBoundaryState
> {
  constructor(props: { onReset: () => void; children: React.ReactNode }) {
    super(props);
    this.state = { hasError: false, errorMessage: null };
  }

  static getDerivedStateFromError(error: Error): ViewerErrorBoundaryState {
    return { hasError: true, errorMessage: error.message };
  }

  render() {
    if (this.state.hasError) {
      return (
        <div className="afr2-surface-cell flex h-[380px] flex-col items-center justify-center gap-3 p-6 text-center">
          <p className="text-sm font-bold" style={{ color: 'var(--error)' }}>
            No se pudo inicializar el visor 3D WebGL / GLB
          </p>
          <p className="max-w-md text-xs" style={{ color: 'var(--text-muted)' }}>
            {this.state.errorMessage?.includes("Unexpected token '<'")
              ? 'El servidor devolvió una página HTML en lugar del modelo. Cuando el archivo esté disponible, usá Reintentar carga 3D para solicitarlo de nuevo.'
              : this.state.errorMessage ?? 'No se pudo cargar el modelo de AutoFinalRobo2. Reintentá la carga.'}
          </p>
          <Button
            variant="secondary"
            size="sm"
            onClick={() => {
              this.setState({ hasError: false, errorMessage: null });
              this.props.onReset();
            }}
          >
            Reintentar carga 3D
          </Button>
        </div>
      );
    }
    return this.props.children;
  }
}

/**
 * AFR2-20261102-01. Inspección de un modelo (/modelos/:modelo): visor con referencias visibles de
 * entrada, selector de partes y, abajo a la derecha del visor, «Ir a simulación».
 * Una tarea por pantalla: el simulador vive en /simulacion/:tipo.
 */
export function Model3DPage() {
  const { modelo } = useParams();
  const [referencesVisible, setReferencesVisible] = useState(true);
  const [viewerEnabled, setViewerEnabled] = useState(true);
  const [viewerKey, setViewerKey] = useState(1);
  if (!isModelPageId(modelo)) return <Navigate to="/modelos" replace />;
  const page = MODEL_PAGES[modelo];
  const retryModel = () => { clearRobotModelCache(); clearInspectionAssetCache(); setViewerKey(k => k + 1); };
  const simulate = <Link to={`/simulacion/${page.simulation}`} className="afr2-model-cta afr2-pressable"><UiIcon name="play" />Ir a simulación<UiIcon name="arrow" /></Link>;
  return <div className="afr2-route-view afr2-models-page">
    <PageHeader eyebrow={page.kicker} title={page.title} description={page.description}
      actions={<>
        <Link className="afr2-developer-back" to="/modelos"><UiIcon name="back" />Modelos</Link>
        <Button size="sm" onClick={() => setViewerEnabled(v => !v)}>{viewerEnabled ? 'Pausar visor' : 'Activar visor'}</Button>
        <Button size="sm" onClick={() => setViewerKey(k => k + 1)}>Restablecer vista</Button>
      </>} />
    <div className="afr2-models-viewer">
      {(modelo === 'auto' || modelo === 'conjunto') && viewerEnabled && <ModelAssetProgress label="Modelo del auto" />}
      {!viewerEnabled ? <div className="afr2-model-inspection__loading"><p>Visor en pausa</p>
        <Button onClick={() => setViewerEnabled(true)}>Reanudar visor</Button></div>
        : <ViewerErrorBoundary key={`${modelo}-${viewerKey}`} onReset={retryModel}>
          <Suspense fallback={<div className="afr2-model-inspection__loading" role="status">Cargando modelo 3D…</div>}>
            <ModelInspection model={modelo} referencesVisible={referencesVisible} onToggleReferences={() => setReferencesVisible(v => !v)} action={simulate} />
          </Suspense>
        </ViewerErrorBoundary>}
    </div>
    <Disclosure title="Acerca del modelo">
      <p className="afr2-help">{ABOUT[modelo]}</p>
    </Disclosure>
  </div>;
}

const ABOUT: Record<ModelPageId, string> = {
  auto: 'Auto Mecanum 4WD con el ensamble actual de Fusion y Blender. Escala: 1 unidad = 1 m, con suelo en la base de las ruedas.',
  brazo: 'Brazo V17 completo en PLA blanco, con sus eslabones y transmisiones. Usa el mismo archivo que el simulador.',
  conjunto: 'Auto y brazo V17 con la misma posición de montaje que la simulación conjunta. El brazo se muestra en su pose de reposo; la posición de montaje es la del modelo, no una medición del robot armado.',
  cubo: 'Cubo auxiliar de 50 × 50 × 50 mm con su material original. Se utiliza como pieza manipulada en el simulador.',
};
