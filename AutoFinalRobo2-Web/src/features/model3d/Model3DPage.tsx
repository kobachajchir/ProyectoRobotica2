import { Disclosure } from '../../components/ui/Disclosure';
import React, { Component, Suspense, useState } from 'react';
import { clearRobotModelCache } from '../../components/three/RobotModel';
import { GlassSurface } from '../../components/ui/GlassSurface';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { Button } from '../../components/ui/Button';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { ModelInspection } from './ModelInspection';

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
  const [viewerEnabled, setViewerEnabled] = useState(true);
  const [viewerKey, setViewerKey] = useState(1);
  const retryModel = () => {
    // useLoader caches rejected requests too. A remount alone repeats the old error.
    clearRobotModelCache();
    setViewerKey(k => k + 1);
  };

  return (
    <div className="afr2-route-view flex flex-col gap-6">
      <GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader
          eyebrow="Visor GLB + Draco Local · AutoFinalRobo2-CAD"
          title="Modelo 3D de AutoFinalRobo2"
          description="Explora el modelo del robot: arrastra para girar y usa la rueda o dos dedos para acercarte."
          badge="Modelo final · Mecanum 4WD"
          badgeTone="info"
          actions={
            <>
              <Button
                variant="secondary"
                size="sm"
                onClick={() => setViewerEnabled((v) => !v)}
              >
                {viewerEnabled ? 'Pausar visor' : 'Activar visor'}
              </Button>
              <Button
                variant="secondary"
                size="sm"
                onClick={() => setViewerKey((k) => k + 1)}
              >
                Restablecer vista
              </Button>
            </>
          }
        />

        <Disclosure title="Acerca del modelo">        <StatusNotice
          tone="info"
          title="Ensamble del robot · Fusion v16 y Blender"
          description="Modelo final elegido para la Web, con cuatro ruedas Mecanum, placas y soporte OLED-encoder. Los puntos identifican las piezas del ensamble; las referencias visuales no certifican tolerancias ni montaje fisico."
          className="mt-4"
        /></Disclosure>

        <div className="mt-5">
          {!viewerEnabled ? (
            <div className="afr2-surface-cell flex h-[380px] flex-col items-center justify-center gap-3 p-6 text-center">
              <p className="text-sm font-semibold" style={{ color: 'var(--text-strong)' }}>
                Lienzo WebGL en pausa
              </p>
              <Button variant="primary" size="sm" onClick={() => setViewerEnabled(true)}>
                Reanudar visor 3D
              </Button>
            </div>
          ) : (
            <ViewerErrorBoundary key={viewerKey} onReset={retryModel}>
                <Suspense
                  fallback={
                    <div className="afr2-model-inspection__loading" role="status">
                      Cargando el modelo de AutoFinalRobo2...
                    </div>
                  }
                >
                  <ModelInspection />
                </Suspense>
            </ViewerErrorBoundary>
          )}
        </div>
        <p className="mt-3 text-xs" style={{ color: 'var(--text-muted)' }}>Escala del modelo: 1 unidad = 1 m · Cuadricula de 0,5 m · Suelo en la base de las ruedas</p>
      </GlassSurface>
    </div>
  );
}
