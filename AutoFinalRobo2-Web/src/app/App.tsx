import { StartupWelcome } from '../components/ui/StartupWelcome';
import { Suspense, lazy } from 'react';
import { HashRouter, Navigate, Outlet, Route, Routes } from 'react-router-dom';
import { ThemeProvider } from './providers/ThemeProvider';
import { ConnectionProvider } from '../features/connection/ConnectionContext';
import { Afr2Toaster } from '../components/notifications/notify';
import { AppShell } from '../components/navigation/RouteNav';
import { HomePage } from '../features/home/HomePage';
import { ControlPage } from '../features/control/ControlPage';
import { SensorsPage } from '../features/sensors/SensorsPage';
import { WifiPage } from '../features/wifi/WifiPage';
import { WifiDraftModal } from '../features/wifi/WifiDraftModal';
import { OledPage } from '../features/oled/OledPage';
import { DocsPage } from '../features/docs/DocsPage';
import { DeveloperPage } from '../features/developer/DeveloperPage';
import { CollectorProvider } from '../features/collector/CollectorContext';

const CollectorPage = lazy(() => import('../features/collector/CollectorPage').then(m => ({ default: m.CollectorPage })));
const UiSimulatorPage = lazy(() => import('../features/developer/UiSimulatorPage').then(m => ({ default: m.UiSimulatorPage })));
const ProtocolStudioPage = lazy(() => import('../features/protocolStudio/ProtocolStudioPage').then(m => ({ default: m.ProtocolStudioPage })));

const Model3DPage = lazy(() =>
  import('../features/model3d/Model3DPage').then((m) => ({ default: m.Model3DPage }))
);

export function App() {
  return (
    <ThemeProvider>
      <ConnectionProvider>
        <StartupWelcome>
          {/* Un unico Toaster raiz fuera de contenedores con transform/overflow (VIS-007) */}
          <Afr2Toaster />
          <WifiDraftModal />
          <HashRouter>
            <AppShell>
              <Routes>
                <Route path="/" element={<HomePage />} />
                <Route path="/control" element={<ControlPage />} />
                <Route element={<CollectorProvider><Outlet /></CollectorProvider>}>
                  <Route path="/simulaciones" element={<Suspense fallback={<p role="status">Cargando simulaciones...</p>}><CollectorPage /></Suspense>} />
                </Route>
                <Route path="/brazo" element={<Navigate to="/simulaciones?tipo=brazo" replace />} />
                <Route path="/simulacion" element={<Navigate to="/simulaciones?tipo=conjunta" replace />} />
                <Route path="/sensores" element={<SensorsPage />} />
                <Route path="/wifi" element={<WifiPage />} />
                <Route path="/oled" element={<OledPage />} />
                <Route
                  path="/modelo-3d"
                  element={
                    <Suspense
                      fallback={
                        <div className="afr2-glass-panel p-8 text-center text-sm font-semibold">
                          Cargando modulo 3D bajo demanda...
                        </div>
                      }
                    >
                      <Model3DPage />
                    </Suspense>
                  }
                />
                <Route path="/documentacion" element={<DocsPage />} />
                <Route path="/desarrollador" element={<DeveloperPage />} />
                <Route path="/desarrollador/protocol-studio" element={<Suspense fallback={<p role="status">Cargando Protocol Studio...</p>}><ProtocolStudioPage /></Suspense>} />
                <Route path="/desarrollador/ui-simulator" element={<Suspense fallback={<p role="status">Cargando Simulador UI...</p>}><UiSimulatorPage /></Suspense>} />
                <Route path="*" element={<Navigate to="/" replace />} />
              </Routes>
            </AppShell>
          </HashRouter>
        </StartupWelcome>
      </ConnectionProvider>
    </ThemeProvider>
  );
}
