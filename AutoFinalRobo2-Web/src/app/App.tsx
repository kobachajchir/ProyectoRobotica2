import { StartupWelcome } from '../components/ui/StartupWelcome';
import { Suspense, lazy } from 'react';
import { HashRouter, Navigate, Route, Routes, useLocation } from 'react-router-dom';
import { ThemeProvider } from './providers/ThemeProvider';
import { ConnectionProvider } from '../features/connection/ConnectionContext';
import { Afr2Toaster } from '../components/notifications/notify';
import { ModelPrefetcher } from '../components/three/ModelPrefetcher';
import { AppShell } from '../components/navigation/RouteNav';
import { HomePage } from '../features/home/HomePage';
import { ControlPage } from '../features/control/ControlPage';
import { SensorsPage } from '../features/sensors/SensorsPage';
import { WifiPage } from '../features/wifi/WifiPage';
import { WifiDraftModal } from '../features/wifi/WifiDraftModal';
import { DocsPage } from '../features/docs/DocsPage';
import { DeveloperPage } from '../features/developer/DeveloperPage';
import { BusyPage, BusySessionGuard } from '../features/connection/BusyPage';
import { CloudSyncProvider } from '../features/sync/CloudSyncController';
import { LineFollowerPage } from '../features/lineFollower/LineFollowerPage';
import { RemoteSyncPage } from '../features/sync/RemoteSyncPage';

const ModelsPage = lazy(() => import('../features/models/ModelsPage').then(m => ({ default: m.ModelsPage })));
const AssemblyPage = lazy(() => import('../features/assembly/AssemblyPage').then(m => ({ default: m.AssemblyPage })));
const LoaderPage = lazy(() => import('../features/loader/LoaderPage').then(m => ({ default: m.LoaderPage })));
const InfrastructurePage = lazy(() => import('../features/infrastructure/InfrastructurePage').then(m => ({ default: m.InfrastructurePage })));
const Model3DPage = lazy(() => import('../features/model3d/Model3DPage').then(m => ({ default: m.Model3DPage })));
const SimulatorPage = lazy(() => import('../features/collector/SimulatorPage').then(m => ({ default: m.SimulatorPage })));
const ProtocolStudioPage = lazy(() => import('../features/protocolStudio/ProtocolStudioPage').then(m => ({ default: m.ProtocolStudioPage })));

/** AFR2-20261101-12: redirección de rutas anteriores a la estructura por tareas. Conserva los
 * parámetros de búsqueda y agrega los que identifican la vista nueva. */
function Redirect({ to, add = {}, map }: { to: string; add?: Record<string, string>; map?: (params: URLSearchParams) => string | void }) {
  const { search } = useLocation();
  const params = new URLSearchParams(search);
  const target = map?.(params) || to;
  Object.entries(add).forEach(([key, value]) => params.set(key, value));
  const query = params.toString();
  return <Navigate to={query ? `${target}?${query}` : target} replace />;
}
/** /simulaciones?tipo=auto|brazo|conjunta → /simulacion/:tipo. */
function simulatorTarget(params: URLSearchParams) {
  const tipo = params.get('tipo');
  params.delete('tipo');
  return `/simulacion/${tipo === 'auto' || tipo === 'conjunta' ? tipo : 'brazo'}`;
}
/** /control?tarea=… → Conducir, Programas o Puesta a punto. */
function controlTarget(params: URLSearchParams) {
  const task = params.get('tarea');
  if (task === 'wheel') { params.set('tarea', 'ruedas'); return '/puesta-a-punto'; }
  if (task === 'calibrate') { params.set('tarea', 'calibracion'); return '/puesta-a-punto'; }
  if (task === 'goto' || task === 'sequences' || task === 'linea') { params.set('tarea', task === 'goto' ? 'goto' : task === 'linea' ? 'linea' : 'secuencias'); return '/programas'; }
  return '/conducir';
}

export function App() {
  return (
    <ThemeProvider>
      <ConnectionProvider>
        <CloudSyncProvider>
          <StartupWelcome>
            {/* Un único Toaster raíz fuera de contenedores con transform/overflow (VIS-007) */}
            <Afr2Toaster />
            {/* AFR2-20261111-03: descarga los modelos 3D en cuanto la conexión queda estable. */}
            <ModelPrefetcher />
            <WifiDraftModal />
            <HashRouter>
              <BusySessionGuard />
              <AppShell>
                <Routes>
                  <Route path="/" element={<HomePage />} />
                  <Route path="/busy" element={<BusyPage />} />
                  {/* AFR2-20261101-12 / VIS-053: destinos por tarea (Operar, Preparar, Explorar, Sistema). */}
                  <Route path="/conducir" element={<ControlPage area="drive" />} />
                  <Route path="/programas" element={<ControlPage area="programs" />} />
                  {/* AFR2-20261117-01: sección propia del seguidor de línea (acceso rápido de Inicio). */}
                  <Route path="/seguidor-linea" element={<LineFollowerPage />} />
                  {/* AFR2-20261117-05: vista remota (solo lectura) de la copia en Firebase. */}
                  <Route path="/remoto" element={<RemoteSyncPage />} />
                  <Route path="/puesta-a-punto" element={<ControlPage area="setup" />} />
                  <Route path="/sensores" element={<SensorsPage />} />
                  <Route path="/modelos" element={<Suspense fallback={<p role="status" className="afr2-route-loading">Cargando modelos…</p>}><ModelsPage /></Suspense>} />
                  <Route path="/armado" element={<Suspense fallback={<p role="status" className="afr2-route-loading">Cargando recorrido de armado…</p>}><AssemblyPage /></Suspense>} />
                  <Route path="/infraestructura" element={<Suspense fallback={<p role="status" className="afr2-route-loading">Cargando infraestructura…</p>}><InfrastructurePage /></Suspense>} />
                  <Route path="/loader" element={<Suspense fallback={<p role="status" className="afr2-route-loading">Cargando Loader…</p>}><LoaderPage /></Suspense>} />
                  {/* AFR2-20261102-01: inspección 3D y simulador separados. */}
                  <Route path="/modelos/:modelo" element={<Suspense fallback={<p role="status" className="afr2-route-loading">Cargando módulo 3D bajo demanda…</p>}><Model3DPage /></Suspense>} />
                  <Route path="/simulacion/:tipo" element={<Suspense fallback={<p role="status" className="afr2-route-loading">Cargando simulador…</p>}><SimulatorPage /></Suspense>} />
                  <Route path="/conexion" element={<WifiPage />} />
                  <Route path="/ayuda" element={<DocsPage />} />
                  {/* Rutas anteriores. */}
                  <Route path="/control" element={<Redirect to="/conducir" map={controlTarget} />} />
                  <Route path="/simulaciones" element={<Redirect to="/simulacion/brazo" map={simulatorTarget} />} />
                  <Route path="/brazo" element={<Navigate to="/simulacion/brazo" replace />} />
                  <Route path="/simulacion" element={<Navigate to="/simulacion/conjunta" replace />} />
                  <Route path="/modelo-3d" element={<Navigate to="/modelos" replace />} />
                  <Route path="/wifi" element={<Redirect to="/conexion" />} />
                  <Route path="/oled" element={<Redirect to="/ayuda" add={{ seccion: 'oled' }} />} />
                  <Route path="/documentacion" element={<Redirect to="/ayuda" />} />
                  <Route path="/desarrollador" element={<DeveloperPage />} />
                  <Route path="/desarrollador/protocol-studio" element={<Suspense fallback={<p role="status" className="afr2-route-loading">Cargando Protocol Studio...</p>}><ProtocolStudioPage /></Suspense>} />
                  {/* AFR2-20261102-02: el Simulador UI pasó a Ayuda → OLED (vista previa y árbol). */}
                  <Route path="/desarrollador/ui-simulator" element={<Navigate to="/ayuda?seccion=oled&vista=arbol" replace />} />
                  <Route path="*" element={<Navigate to="/" replace />} />
                </Routes>
              </AppShell>
            </HashRouter>
          </StartupWelcome>
        </CloudSyncProvider>
      </ConnectionProvider>
    </ThemeProvider>
  );
}
