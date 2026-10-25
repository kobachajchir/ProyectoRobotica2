import { StartupWelcome } from '../components/ui/StartupWelcome';
import { Suspense, lazy } from 'react';
import { HashRouter, Navigate, Route, Routes } from 'react-router-dom';
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

const Model3DPage = lazy(() =>
  import('../features/model3d/Model3DPage').then((m) => ({ default: m.Model3DPage }))
);

export function App() {
  return (
    <ThemeProvider>
      <ConnectionProvider>
        <StartupWelcome>
          {/* Un único Toaster raíz fuera de contenedores con transform/overflow (VIS-007) */}
          <Afr2Toaster />
          <WifiDraftModal />
          <HashRouter>
            <AppShell>
              <Routes>
                <Route path="/" element={<HomePage />} />
                <Route path="/control" element={<ControlPage />} />
                <Route path="/sensores" element={<SensorsPage />} />
                <Route path="/wifi" element={<WifiPage />} />
                <Route path="/oled" element={<OledPage />} />
                <Route
                  path="/modelo-3d"
                  element={
                    <Suspense
                      fallback={
                        <div className="afr2-glass-panel p-8 text-center text-sm font-semibold">
                          Cargando módulo 3D bajo demanda...
                        </div>
                      }
                    >
                      <Model3DPage />
                    </Suspense>
                  }
                />
                <Route path="/documentacion" element={<DocsPage />} />
                <Route path="/desarrollador" element={<DeveloperPage />} />
                <Route path="*" element={<Navigate to="/" replace />} />
              </Routes>
            </AppShell>
          </HashRouter>
        </StartupWelcome>
      </ConnectionProvider>
    </ThemeProvider>
  );
}
