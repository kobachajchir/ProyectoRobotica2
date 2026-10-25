import { Disclosure } from '../../components/ui/Disclosure';
import React, { Component, Suspense, useEffect, useState } from 'react';
import { Canvas, useThree } from '@react-three/fiber';
import { OrbitControls } from '../../components/three/OrbitControls';
import { RobotModel, useRobotModel, clearRobotModelCache } from '../../components/three/RobotModel';
import { GlassSurface } from '../../components/ui/GlassSurface';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { Button } from '../../components/ui/Button';
import { StatusNotice } from '../../components/ui/StatusNotice';

function InspectionScene() {
  const { size: modelSize } = useRobotModel();
  const { camera, size: viewport, invalidate } = useThree();
  const targetY = modelSize.y / 2;
  useEffect(() => {
    const extent = Math.max(modelSize.x, modelSize.y, modelSize.z, .01);
    const distance = extent * 1.5 / Math.min(1, viewport.width / viewport.height);
    camera.position.set(distance, targetY + distance * .65, distance);
    camera.lookAt(0, targetY, 0);
    camera.updateProjectionMatrix();
    invalidate();
  }, [camera, viewport.width, viewport.height, modelSize, targetY, invalidate]);
  return <><RobotModel /><OrbitControls targetY={targetY} /></>;
}

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
          description="Explorá el modelo del robot: arrastrá para girar y usá la rueda o dos dedos para acercarte."
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
          description="Modelo final elegido para la Web, con cuatro ruedas Mecanum y materiales del CAD. La versión ligera conserva el ensamble; las tolerancias físicas y la futura pieza/PCB OLED-encoder siguen pendientes."
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
              <div
                key={viewerKey}
                className="relative h-[420px] w-full overflow-hidden rounded-2xl border"
                style={{
                  backgroundColor: '#090f1c',
                  borderColor: 'var(--border-default)',
                }}
              >
                <Suspense
                  fallback={
                    <div className="flex h-full w-full items-center justify-center text-xs font-semibold text-slate-300">
                      Cargando el modelo de AutoFinalRobo2...
                    </div>
                  }
                >
                  <Canvas frameloop="demand" dpr={[1, 1.5]} camera={{ position: [.5, .4, .5], fov: 45, near: .005, far: 80 }}
                    fallback={<p className="p-6">WebGL no está disponible. La vista Superior de Control sigue disponible.</p>}>
                    <ambientLight intensity={0.9} />
                    <directionalLight position={[4, 6, 4]} intensity={1.4} />
                    <directionalLight position={[-4, -2, -4]} intensity={0.5} />
                    <gridHelper args={[6, 12, '#b81d33', '#1e293b']} />
                    <InspectionScene />
                  </Canvas>
                </Suspense>
              </div>
            </ViewerErrorBoundary>
          )}
        </div>
        <p className="mt-3 text-xs" style={{ color: 'var(--text-muted)' }}>Escala del modelo: 1 unidad = 1 m · Cuadrícula de 0,5 m · Suelo en la base de las ruedas</p>
      </GlassSurface>
    </div>
  );
}
