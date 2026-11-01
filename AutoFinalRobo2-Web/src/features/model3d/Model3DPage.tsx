import { Disclosure } from '../../components/ui/Disclosure';
import React, { Component, Suspense, useState } from 'react';
import { clearRobotModelCache } from '../../components/three/RobotModel';
import { GlassSurface } from '../../components/ui/GlassSurface';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { Button } from '../../components/ui/Button';
import { TaskSections } from '../../components/ui/TaskSections';
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
              ? 'El servidor devolvio una pagina HTML en lugar del modelo. Cuando el archivo este disponible, usa Reintentar carga 3D para solicitarlo de nuevo.'
              : this.state.errorMessage ?? 'No se pudo cargar el modelo de AutoFinalRobo2. Reintenta la carga.'}
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

export function Model3DPage() {
  const [selectedModel, setSelectedModel] = useState<'auto' | 'brazo' | 'cubo'>('auto');
  const [referencesVisible, setReferencesVisible] = useState(true);
  const [viewerEnabled, setViewerEnabled] = useState(true);
  const [viewerKey, setViewerKey] = useState(1);
  const retryModel = () => { clearRobotModelCache(); clearInspectionAssetCache(); setViewerKey(k => k + 1); };
  const viewer = (model: 'auto' | 'brazo' | 'cubo') => <div className="afr2-models-viewer">
    {!viewerEnabled ? <div className="afr2-model-inspection__loading"><p>Visor en pausa</p>
      <Button onClick={() => setViewerEnabled(true)}>Reanudar visor</Button></div>
      : <ViewerErrorBoundary key={`${model}-${viewerKey}`} onReset={retryModel}>
        <Suspense fallback={<div className="afr2-model-inspection__loading" role="status">Cargando modelo 3D...</div>}>
          <ModelInspection model={model} referencesVisible={referencesVisible} onToggleReferences={() => setReferencesVisible(v => !v)} />
        </Suspense>
      </ViewerErrorBoundary>}
  </div>;
  return <div className="afr2-route-view afr2-models-page">
    <GlassSurface variant="panel" className="p-5 sm:p-6">
      <SectionHeader title="Modelos 3D" description="Elegi un modelo para explorarlo y conocer sus componentes."
        actions={<><Button size="sm" onClick={() => setViewerEnabled(v => !v)}>{viewerEnabled ? 'Pausar visor' : 'Activar visor'}</Button>
          <Button size="sm" onClick={() => setViewerKey(k => k + 1)}>Restablecer vista</Button></>} />
      <TaskSections label="Elegir modelo 3D" value={selectedModel} onValueChange={value => setSelectedModel(value as 'auto' | 'brazo' | 'cubo')} items={[
        { id: 'auto', label: 'Auto', content: viewer('auto') },
        { id: 'brazo', label: 'Brazo robotico', content: viewer('brazo') },
        { id: 'cubo', label: 'Cubo auxiliar', content: viewer('cubo') },
      ]} />
      <Disclosure title="Acerca del modelo">
        <p className="afr2-help">{selectedModel === 'auto' ? 'Auto Mecanum 4WD con el ensamble actual de Fusion y Blender. Escala: 1 unidad = 1 m, con suelo en la base de las ruedas.' : selectedModel === 'brazo' ? 'Brazo V17 completo en PLA blanco, con sus eslabones y transmisiones. Usa el mismo archivo que el simulador.' : 'Cubo auxiliar de 50 × 50 × 50 mm con su material original. Se utiliza como pieza manipulada en el simulador.'}</p>
      </Disclosure>
    </GlassSurface>
  </div>;
}
