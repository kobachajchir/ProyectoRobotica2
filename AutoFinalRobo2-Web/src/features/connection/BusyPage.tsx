import { useCallback, useEffect, useState } from 'react';
import { useLocation, useNavigate } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { PageHeader } from '../../components/ui/PageHeader';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { UiIcon } from '../../components/ui/UiIcon';
import { useConnection } from './ConnectionContext';
import { ROBOT_MODE_LABELS } from './useRobotMode';

export interface BusyEndpointSnapshot {
  busy: boolean;
  busyForRequester: boolean;
  maxWifiClients: number;
  maxWebClients: number;
  apClients: number;
  wsClients: number;
  robotMode: string;
  robotStopped: boolean;
  uptimeMs: number;
}

/**
 * Redirecciona automáticamente a `/busy` cuando el broker ESP rechaza el WebSocket con
 * código 1013 («Session limit») o `/api/busy` informa que otro cliente Web está activo.
 */
export function BusySessionGuard() {
  const { lastSocketClose, wsState } = useConnection();
  const location = useLocation();
  const navigate = useNavigate();

  useEffect(() => {
    if (
      wsState !== 'ready' &&
      lastSocketClose?.code === 1013 &&
      lastSocketClose.reason === 'Session limit' &&
      location.pathname !== '/busy'
    ) {
      navigate('/busy', { replace: true });
    }
  }, [lastSocketClose, location.pathname, navigate, wsState]);

  return null;
}

/**
 * Página `/busy` (en variante embebida y Web) que informa el estado de ocupado actual del auto
 * cuando otro cliente Web está conectado, y libera/redirige a `/` cuando el cliente se desconecta.
 */
export function BusyPage() {
  const { wsState, lastSocketClose, wifiStatus, robotState, connectToEndpoint } = useConnection();
  const navigate = useNavigate();
  const [remoteBusy, setRemoteBusy] = useState<BusyEndpointSnapshot | null>(null);
  const [checking, setChecking] = useState(false);

  const pollBusyStatus = useCallback(async () => {
    setChecking(true);
    try {
      const response = await fetch('/api/busy', { cache: 'no-store' });
      if (response.ok) {
        const data = (await response.json()) as BusyEndpointSnapshot;
        setRemoteBusy(data);
        if (!data.busyForRequester && !data.busy) {
          connectToEndpoint();
          navigate('/', { replace: true });
        }
      }
    } catch {
      /* En variante online sin ESP directo se usa el estado del contexto WebSocket */
    } finally {
      setChecking(false);
    }
  }, [connectToEndpoint, navigate]);

  useEffect(() => {
    void pollBusyStatus();
    const id = window.setInterval(() => {
      void pollBusyStatus();
    }, 2000);
    return () => window.clearInterval(id);
  }, [pollBusyStatus]);

  useEffect(() => {
    if (wsState === 'ready' && (!remoteBusy || !remoteBusy.busyForRequester)) {
      navigate('/', { replace: true });
    }
  }, [navigate, remoteBusy, wsState]);

  const isBusy =
    remoteBusy?.busyForRequester ??
    remoteBusy?.busy ??
    (lastSocketClose?.code === 1013 && lastSocketClose.reason === 'Session limit');

  const apClients = remoteBusy?.apClients ?? wifiStatus.apClients ?? 0;
  const maxWifi = remoteBusy?.maxWifiClients ?? 4;
  const wsClients = remoteBusy?.wsClients ?? wifiStatus.wsClients ?? (isBusy ? 1 : 0);
  const maxWeb = remoteBusy?.maxWebClients ?? 1;
  const rawMode = remoteBusy?.robotMode ?? robotState?.mode ?? '—';
  const modeLabel = (ROBOT_MODE_LABELS as Record<string, string>)[rawMode] ?? rawMode;
  const stoppedLabel =
    remoteBusy !== null
      ? remoteBusy.robotStopped
        ? 'Detenido'
        : 'En movimiento'
      : robotState?.stopped === false
        ? 'En movimiento'
        : 'Detenido';

  return (
    <div className="afr2-route-view afr2-page-stack">
      <PageHeader
        eyebrow="Protección de sesión única"
        title="Estado Busy del auto"
        description="El punto de acceso Wi-Fi admite hasta 4 dispositivos conectados, pero el servidor embebido atiende datos de aplicación a 1 único cliente Web por vez."
      />

      <StatusNotice
        tone={isBusy ? 'warning' : 'info'}
        title={
          isBusy
            ? 'Auto ocupado por otro cliente Web activo'
            : 'Auto libre — listo para conectar'
        }
      >
        {isBusy
          ? 'Otro navegador mantiene la sesión Web activa con el auto. Cualquier petición a la raíz u otra ruta es redirigida a esta vista (/busy) sin entregar datos de control hasta que el cliente actual se desconecte.'
          : 'No hay otro cliente bloqueando la sesión. Podés volver al inicio y tomar el control.'}
      </StatusNotice>

      <section className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
        <h2 className="text-base font-semibold">Estado actual reportado por el auto</h2>
        <dl className="grid grid-cols-2 sm:grid-cols-4 gap-3 text-sm">
          <div className="p-3 rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)]">
            <dt className="text-xs text-[var(--text-muted)]">Cliente Web activo</dt>
            <dd className="text-base font-mono font-semibold mt-1">
              {wsClients} / {maxWeb}
            </dd>
          </div>
          <div className="p-3 rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)]">
            <dt className="text-xs text-[var(--text-muted)]">Dispositivos en Wi-Fi AP</dt>
            <dd className="text-base font-mono font-semibold mt-1">
              {apClients} / {maxWifi}
            </dd>
          </div>
          <div className="p-3 rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)]">
            <dt className="text-xs text-[var(--text-muted)]">Modo actual del auto</dt>
            <dd className="text-base font-mono font-semibold mt-1">{modeLabel}</dd>
          </div>
          <div className="p-3 rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)]">
            <dt className="text-xs text-[var(--text-muted)]">Salidas de tracción</dt>
            <dd className="text-base font-semibold mt-1">{stoppedLabel}</dd>
          </div>
        </dl>

        <div className="afr2-action-row">
          <Button
            variant="primary"
            icon={<UiIcon name="refresh" size={16} />}
            disabled={checking}
            onClick={() => {
              connectToEndpoint();
              void pollBusyStatus();
            }}
          >
            Comprobar disponibilidad y conectar
          </Button>
          {!isBusy && (
            <Button variant="secondary" onClick={() => navigate('/')}>
              Ir al inicio
            </Button>
          )}
        </div>
      </section>
    </div>
  );
}
