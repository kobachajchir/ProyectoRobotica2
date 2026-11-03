import { NetworkPill,NetworkDetails } from '../../features/connection/NetworkConnection';
import { BatteryPill, BatteryDetails } from '../../features/connection/BatteryStatus';
import { useEffect, useMemo, useRef, useState, type ReactNode } from 'react';
import { NavLink, useLocation } from 'react-router-dom';
import { BrandMark } from '../ui/BrandMark';
import { Button } from '../ui/Button';
import { DismissibleNotice } from '../ui/DismissibleNotice';
import { UiIcon } from '../ui/UiIcon';
import { SegmentedControl, type SegmentedItem } from '../ui/SegmentedControl';
import { FloatingModal, type ModalOrigin } from '../ui/FloatingModal';
import { useTheme, type ThemePreference } from '../../app/providers/ThemeProvider';
import { OverlayContext } from '../../app/providers/OverlayContext';
import { useConnection } from '../../features/connection/ConnectionContext';
import { useRobotMode } from '../../features/connection/useRobotMode';
import { LiveOledViewer } from '../../features/oled/LiveOledViewer';
import { SystemPanel } from '../../features/connection/SystemPanel';
import { OperationalStatusBar } from '../../features/control/OperationalStatusBar';
import { BOTTOM_NAV, DESTINATIONS, HOME, MORE_DESTINATIONS, NAV_GROUPS, belongsTo, destination, titleForPath } from '../../app/navigation';

const SIDEBAR_STORAGE_KEY = 'afr2.sidebar-collapsed';
// AFR2-20261101-09 / VIS-052: Apariencia en el selector segmentado comun.
const THEME_CHOICES: readonly SegmentedItem<ThemePreference>[] = [
  { value: 'system', label: 'Automática' }, { value: 'light', label: 'Clara' }, { value: 'dark', label: 'Oscura' },
];
/** Rutas con barra de estado operativo bajo la barra superior (VIS-053). */
const OPERATIONAL_ROUTES = ['/conducir', '/programas'];

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
      // AFR2-20261101-12: rail plegado entre md y lg; expandida desde lg (1200 px). Bajo md no hay barra lateral.
      return saved === null ? matchMedia('(max-width: 1199.98px)').matches : saved === 'true';
    } catch {
      return true;
    }
  });
  const [modal, setModal] = useState<'oled' | 'settings' | 'network' | 'battery' | 'more' | null>(null);
  const [origin, setOrigin] = useState<ModalOrigin | null>(null);
  const sidebar = useRef<HTMLElement>(null);
  const location = useLocation();
  const mode = useRobotMode();
  const title = titleForPath(location.pathname);
  const operational = OPERATIONAL_ROUTES.includes(location.pathname);
  const moreActive = MORE_DESTINATIONS.some(item => belongsTo(item.to, location.pathname));
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


  const showSettings = (element: HTMLElement, keyboard: boolean) => {
    setOrigin({ element, keyboard });
    setModal('settings');
  };
  const openModal = (next: 'oled' | 'settings' | 'more', element: HTMLElement, keyboard: boolean) => {
    setOrigin({ element, keyboard });
    setModal(next);
  };
  const link = (to: string, onNavigate?: () => void) => {
    const item = destination(to)!;
    return <NavLink key={to} to={to} end={to === '/'} aria-label={item.label} title={sidebarCollapsed ? item.label : undefined}
      onClick={onNavigate} className={() => `afr2-route-link${belongsTo(to, location.pathname) ? ' is-active' : ''}`}>
      <UiIcon name={item.icon} />
      <span className="afr2-sidebar-label">{item.label}</span>
    </NavLink>;
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
              <UiIcon name={sidebarCollapsed ? 'panel-open' : 'panel-close'} />
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
          {/* AFR2-20261101-12 / VIS-053: destinos por tarea en cuatro grupos. */}
          <nav id="afr2-route-nav" aria-label="Navegación principal" className="afr2-route-nav">
            {link(HOME.to)}
            {NAV_GROUPS.map(group => <div key={group.id} className="afr2-nav-group" role="group" aria-labelledby={`afr2-nav-${group.id}`}>
              <p id={`afr2-nav-${group.id}`} className="afr2-nav-caption" data-nav-group={group.id}>{group.label}</p>
              {DESTINATIONS.filter(item => item.group === group.id).map(item => link(item.to))}
            </div>)}
          </nav>
          <button
            type="button"
            className="afr2-route-link afr2-sidebar-settings"
            aria-label="Configuración y apariencia"
            title="Configuración y apariencia"
            aria-haspopup="dialog"
            onClick={(event) => showSettings(event.currentTarget, event.detail === 0)}
          >
            <UiIcon name="settings" />
            <span className="afr2-sidebar-label">Configuración</span>
          </button>
        </aside>
        {/* Celular (bajo 840 px): cuatro destinos al alcance del pulgar y «Más». */}
        <nav className="afr2-bottom-nav" aria-label="Navegación principal">
          {BOTTOM_NAV.map(to => { const item = destination(to)!; return <NavLink key={to} to={to} end={to === '/'}
            className={({ isActive }) => `afr2-bottom-nav__item${isActive ? ' is-active' : ''}`}>
            <UiIcon name={item.icon} /><span>{item.label}</span>
          </NavLink>; })}
          <button type="button" className={`afr2-bottom-nav__item${moreActive ? ' is-active' : ''}`} aria-haspopup="dialog" aria-expanded={modal === 'more'}
            onClick={event => openModal('more', event.currentTarget, event.detail === 0)}>
            <UiIcon name="more" /><span>Más</span>
          </button>
        </nav>
        <div className="afr2-app-content">
          <header className="afr2-topbar">
            <div className="afr2-topbar-context">
              <span className="afr2-location">{title}</span>
              <span className="afr2-mode-label">Modo · {mode}</span>
            </div>
            <div className="afr2-action-row">
              <NetworkPill onOpen={source=>{setOrigin(source);setModal('network');}} />
              <BatteryPill onOpen={source => { setOrigin(source); setModal('battery'); }} />
              {stopAvailable && <Button variant="emergency" icon={<UiIcon name="stop" />} onClick={() => void triggerEmergencyStop('user-stop')}>
                STOP
              </Button>}
            </div>
          </header>
          {operational && <OperationalStatusBar />}
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
          <main id="afr2-main" tabIndex={-1} className="afr2-main" data-visual-area={location.pathname.split('/')[1] || 'inicio'}>
            {children}
          </main>
        </div>
        <FloatingModal
          open={modal === 'oled'}
          origin={origin}
          onClose={() => setModal((value) => (value === 'oled' ? null : value))}
          title="Pantalla en vivo"
          size="lg"
          description="Consultá la OLED sin salir de lo que estás haciendo."
        >
          <LiveOledViewer active={modal === 'oled'} />
        </FloatingModal>
        <FloatingModal open={modal==='network'} origin={origin} onClose={()=>setModal(v=>v==='network'?null:v)} title="Red conectada" description="Wi-Fi y punto de acceso del auto">
          <NetworkDetails onClose={()=>setModal(null)}/>
        </FloatingModal>
        <FloatingModal open={modal === 'battery'} origin={origin} onClose={() => setModal(value => value === 'battery' ? null : value)} title="Batería" description="Voltaje medido por el controlador">
          <BatteryDetails />
        </FloatingModal>
        <FloatingModal
          open={modal === 'settings'}
          origin={origin}
          onClose={() => setModal((value) => (value === 'settings' ? null : value))}
          title="Configuración y apariencia"
        >
          <div className="afr2-theme-field"><span>Apariencia</span><SegmentedControl ariaLabel="Apariencia" items={THEME_CHOICES} value={preference} onChange={setPreference} /></div>
          <SystemPanel />
          {/* AFR2-20261101-12: Desarrollador se encuentra sin conocer el triple toque del logo. */}
          <section className="afr2-settings-about" aria-labelledby="afr2-settings-about-title">
            <h3 id="afr2-settings-about-title" className="afr2-subheading">Acerca de</h3>
            <p className="afr2-help">AutoFinalRobo2 · consola Web del robot Mecanum.</p>
            <NavLink to="/desarrollador" className="afr2-route-link" onClick={() => setModal(null)}><UiIcon name="board" /><span>Desarrollador</span></NavLink>
          </section>
        </FloatingModal>
        <FloatingModal open={modal === 'more'} origin={origin} onClose={() => setModal(value => (value === 'more' ? null : value))} title="Más" description="El resto de las secciones, la pantalla en vivo y la configuración.">
          <nav aria-label="Más secciones" className="afr2-more-list">
            <button type="button" className="afr2-route-link" aria-haspopup="dialog" onClick={event => openModal('oled', event.currentTarget, event.detail === 0)}>
              <UiIcon name="screen" /><span>OLED en vivo</span>
            </button>
            {MORE_DESTINATIONS.map(item => link(item.to, () => setModal(null)))}
            <button type="button" className="afr2-route-link" aria-haspopup="dialog" onClick={event => openModal('settings', event.currentTarget, event.detail === 0)}>
              <UiIcon name="settings" /><span>Configuración</span>
            </button>
          </nav>
        </FloatingModal>
      </div>
    </OverlayContext.Provider>
  );
}
