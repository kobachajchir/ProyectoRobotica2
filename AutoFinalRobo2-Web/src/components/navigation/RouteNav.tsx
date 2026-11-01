import { NetworkPill,NetworkDetails } from '../../features/connection/NetworkConnection';
import { BatteryPill, BatteryDetails } from '../../features/connection/BatteryStatus';
import { useEffect, useMemo, useRef, useState, type ReactNode } from 'react';
import { NavLink, useLocation } from 'react-router-dom';
import { BrandMark } from '../ui/BrandMark';
import { Button } from '../ui/Button';
import { DismissibleNotice } from '../ui/DismissibleNotice';
import { UiIcon, type UiIconName } from '../ui/UiIcon';
import { FloatingModal, type ModalOrigin } from '../ui/FloatingModal';
import { useTheme, type ThemePreference } from '../../app/providers/ThemeProvider';
import { OverlayContext } from '../../app/providers/OverlayContext';
import { useConnection } from '../../features/connection/ConnectionContext';
import { useRobotMode } from '../../features/connection/useRobotMode';
import { LiveOledViewer } from '../../features/oled/LiveOledViewer';
import { SystemPanel } from '../../features/connection/SystemPanel';

const SIDEBAR_STORAGE_KEY = 'afr2.sidebar-collapsed';
const NAV_ITEMS: { to: string; label: string; icon: UiIconName }[] = [
  { to: '/', label: 'Inicio', icon: 'home' },
  { to: '/control', label: 'Control', icon: 'control' },
  { to: '/simulaciones', label: 'Simulaciones', icon: 'arm' },
  { to: '/sensores', label: 'Sensores', icon: 'sensors' },
  { to: '/wifi', label: 'Wi-Fi', icon: 'wifi' },
  { to: '/oled', label: 'Pantalla OLED', icon: 'screen' },
  { to: '/modelo-3d', label: 'Modelos 3D', icon: 'cube' },
  { to: '/documentacion', label: 'Documentacion', icon: 'book' },
];

/** AppShell/sidebar adapted from ST2026-Radal 335d3ed; product-specific services
 * and persistent mobile rail replace permissions, commerce and hidden drawer. */
export function AppShell({ children }: { children: ReactNode }) {
  const { preference, setPreference } = useTheme();
  const {
    triggerEmergencyStop,
    stopRecord,
    stopAvailable,
    stopNoticeVisible,
    dismissStopNotice,
  } = useConnection();
  const [isSidebarCollapsed, setIsSidebarCollapsed] = useState(() => {
    try {
      const saved = localStorage.getItem(SIDEBAR_STORAGE_KEY);
      return saved === null ? matchMedia('(max-width: 819px)').matches : saved === 'true';
    } catch {
      return true;
    }
  });
  const [modal, setModal] = useState<'oled' | 'settings' | 'network' | 'battery' | null>(null);
  const [origin, setOrigin] = useState<ModalOrigin | null>(null);
  const sidebar = useRef<HTMLElement>(null);
  const location = useLocation();
  const mode = useRobotMode();
  const title = location.pathname.startsWith('/desarrollador/ui-simulator') ? 'Simulador UI' : location.pathname.startsWith('/desarrollador/protocol-studio') ? 'Protocol Studio' : location.pathname === '/desarrollador' ? 'Desarrollador' : NAV_ITEMS.find((item) => item.to === location.pathname)?.label ?? 'Inicio';
  const sidebarCollapsed = isSidebarCollapsed;
  const overlays = useMemo(
    () => ({
      openOled: (source: ModalOrigin) => {
        setOrigin(source);
        setModal('oled');
      },
      modalOpen: modal !== null,
      oledOpen: modal === 'oled',
    }),
    [modal]
  );

  useEffect(() => {
    try {
      localStorage.setItem(SIDEBAR_STORAGE_KEY, String(isSidebarCollapsed));
    } catch {
      /* Storage is optional. */
    }
  }, [isSidebarCollapsed]);

  useEffect(() => {
    if (isSidebarCollapsed) return;
    const outside = (event: PointerEvent) => {
      if (matchMedia('(max-width: 819px)').matches && !sidebar.current?.contains(event.target as Node)) {
        setIsSidebarCollapsed(true);
      }
    };
    const escape = (event: KeyboardEvent) => {
      if (event.key === 'Escape' && matchMedia('(max-width: 819px)').matches) {
        setIsSidebarCollapsed(true);
      }
    };
    document.addEventListener('pointerdown', outside);
    document.addEventListener('keydown', escape);
    return () => {
      document.removeEventListener('pointerdown', outside);
      document.removeEventListener('keydown', escape);
    };
  }, [isSidebarCollapsed]);

  const showSettings = (element: HTMLElement, keyboard: boolean) => {
    setOrigin({ element, keyboard });
    setModal('settings');
  };



  return (
    <OverlayContext.Provider value={overlays}>
      <div className={`afr2-app-layout${sidebarCollapsed ? ' afr2-app-layout--collapsed' : ''}`}>
        <a
          className="afr2-skip-link"
          href="#afr2-main"
          onClick={(event) => {
            event.preventDefault();
            document.getElementById('afr2-main')?.focus();
          }}
        >
          Ir al contenido
        </a>
        <aside ref={sidebar} className="afr2-sidebar" aria-label="Barra lateral">
          <div className="afr2-sidebar-heading">
            <NavLink to="/" aria-label="AutoFinalRobo2 · Inicio" className="afr2-brand">
              <BrandMark size="header" />
              <span className="afr2-sidebar-label">AutoFinalRobo2</span>
            </NavLink>
            <button
              className="afr2-icon-button afr2-sidebar-collapse"
              aria-controls="afr2-route-nav"
              aria-expanded={!sidebarCollapsed}
              aria-label={sidebarCollapsed ? 'Expandir barra lateral' : 'Colapsar barra lateral'}
              title={sidebarCollapsed ? 'Expandir barra lateral' : 'Colapsar barra lateral'}
              onClick={() => setIsSidebarCollapsed((value) => !value)}
              type="button"
            >
              <UiIcon name={sidebarCollapsed ? 'panel' : 'back'} />
            </button>
          </div>
          <button
            className="afr2-route-link afr2-oled-shortcut"
            type="button"
            aria-label="Abrir visor OLED en vivo"
            title="Visor OLED en vivo"
            aria-haspopup="dialog"
            aria-expanded={modal === 'oled'}
            onClick={(event) => overlays.openOled({ element: event.currentTarget, keyboard: event.detail === 0 })}
          >
            <UiIcon name="screen" />
            <span className="afr2-sidebar-label">
              OLED en vivo<small>Pantalla del robot</small>
            </span>
          </button>
          <p className="afr2-nav-caption afr2-sidebar-label">Espacio de trabajo</p>
          <nav id="afr2-route-nav" aria-label="Navegacion principal" className="afr2-route-nav">
            {NAV_ITEMS.map((item) => (
              <NavLink
                key={item.to}
                to={item.to}
                end={item.to === '/'}
                aria-label={item.label}
                title={sidebarCollapsed ? item.label : undefined}
                onClick={() => {
                  if (matchMedia('(max-width: 819px)').matches) setIsSidebarCollapsed(true);
                }}
                className={({ isActive }) => `afr2-route-link${isActive ? ' is-active' : ''}`}
              >
                <UiIcon name={item.icon} />
                <span className="afr2-sidebar-label">{item.label}</span>
              </NavLink>
            ))}
          </nav>
          <button
            type="button"
            className="afr2-route-link afr2-sidebar-settings"
            aria-label="Configuracion y apariencia"
            title="Configuracion y apariencia"
            aria-haspopup="dialog"
            onClick={(event) => showSettings(event.currentTarget, event.detail === 0)}
          >
            <UiIcon name="settings" />
            <span className="afr2-sidebar-label">Configuracion</span>
          </button>
        </aside>
        <div className="afr2-app-content">
          <header className="afr2-topbar">
            <div className="afr2-topbar-context">
              <span className="afr2-location">{title}</span>
              <span className="afr2-mode-label">Modo · {mode}</span>
            </div>
            <div className="afr2-action-row">
              <NetworkPill onOpen={source=>{setOrigin(source);setModal('network');}} />
              <BatteryPill onOpen={source => { setOrigin(source); setModal('battery'); }} />
              {stopAvailable && <Button variant="emergency" onClick={() => void triggerEmergencyStop('user-stop')}>
                ■ STOP
              </Button>}
            </div>
          </header>
          {stopNoticeVisible && stopRecord.status !== 'idle' && modal === null && stopRecord.detail && (
            <div className="px-4 sm:px-8 pt-3">
              <DismissibleNotice
                id={`header-stop-notice-${stopRecord.noticeId}`}
                tone={stopRecord.status === 'confirmed' ? 'warning' : 'error'}
                title={stopRecord.status === 'confirmed' ? 'STOP confirmado' : 'Aviso de parada STOP'}
                message={stopRecord.detail}
                onDismiss={dismissStopNotice}
                dismissLabel="Cerrar aviso de parada en cabecera"
              />
            </div>
          )}
          <main id="afr2-main" tabIndex={-1} className="afr2-main">
            {children}
          </main>
        </div>
        <FloatingModal
          open={modal === 'oled'}
          origin={origin}
          onClose={() => setModal((value) => (value === 'oled' ? null : value))}
          title="Pantalla en vivo"
          description="Consulta la OLED sin salir de lo que estas haciendo."
        >
          <LiveOledViewer active={modal === 'oled'} />
        </FloatingModal>
        <FloatingModal open={modal==='network'} origin={origin} onClose={()=>setModal(v=>v==='network'?null:v)} title="Red conectada" description="Wi-Fi y punto de acceso del auto">
          <NetworkDetails onClose={()=>setModal(null)}/>
        </FloatingModal>
        <FloatingModal open={modal === 'battery'} origin={origin} onClose={() => setModal(value => value === 'battery' ? null : value)} title="Bateria" description="Voltaje medido por el controlador">
          <BatteryDetails />
        </FloatingModal>
        <FloatingModal
          open={modal === 'settings'}
          origin={origin}
          onClose={() => setModal((value) => (value === 'settings' ? null : value))}
          title="Configuracion y apariencia"
        >
          <div className="afr2-theme-field"><span>Apariencia</span><div className="afr2-theme-choices" role="group" aria-label="Apariencia">{(['system', 'light', 'dark'] as ThemePreference[]).map(value => <button key={value} type="button" aria-pressed={preference === value} onClick={() => setPreference(value)}>{value === 'system' ? 'Automatica' : value === 'light' ? 'Clara' : 'Oscura'}</button>)}</div></div>
          <SystemPanel />
        </FloatingModal>
      </div>
    </OverlayContext.Provider>
  );
}
