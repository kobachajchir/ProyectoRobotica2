import { useCallback, useEffect, useRef, useState } from 'react';
import { Afr2CommandError } from '../../protocol/afr2Client';
import { formatAgeLabel } from '../../protocol/wsEnvelope';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { useConnection } from '../connection/ConnectionContext';
import { EncoderControl } from './EncoderControl';
import { OledCanvas128x64 } from './OledCanvas128x64';
import { getScreenMetadata } from './screenCatalog';
import { parseRemoteOled, type RemoteOledSnapshot } from './remoteSnapshot';
import { MenuSideControls } from './MenuSideControls';

const POLL_INTERVAL_MS = 1200;
const STALE_SNAPSHOT_MS = 4000;

export function LiveOledViewer({ active = true }: { active?: boolean }) {
  const { wsState, megaLinkState, capabilities, requestCommand, screenStatus } = useConnection();
  const [snapshot, setSnapshot] = useState<RemoteOledSnapshot | null>(null);
  const [nowMs, setNowMs] = useState(() => Date.now());
  const [loading, setLoading] = useState(false);
  const [actionPending, setActionPending] = useState(false);
  const [unavailable, setUnavailable] = useState(false);
  const [errorMessage, setErrorMessage] = useState<string | null>(null);
  const [actionFeedback, setActionFeedback] = useState<{
    tone: 'neutral' | 'loading' | 'success' | 'error';
    message: string;
  } | null>(null);
  const [lastActionLabel, setLastActionLabel] = useState('ninguna');
  const [pollRevision, setPollRevision] = useState(0);
  const latestSnapshot = useRef<RemoteOledSnapshot | null>(null);
  const inputEpoch = useRef(0);

  const inFlightRef = useRef<Promise<void> | null>(null);
  const actionLockRef = useRef(false);
  const sessionRef = useRef(0);
  const timerRef = useRef<number | null>(null);

  useEffect(()=>{if(!active)return;latestSnapshot.current=null;setSnapshot(null);++inputEpoch.current;setUnavailable(false);setPollRevision(v=>v+1);},[active,screenStatus?.screenCode,screenStatus?.navigationRevision]);

  const linksReady = wsState === 'ready' && megaLinkState === 'ready';
  const oledCap = capabilities.oled_128x64;
  const encoderCap = capabilities.ui_encoder_sw;
  const remoteOledImplemented =
    linksReady && oledCap.present && oledCap.implemented && oledCap.freshness !== 'local-preview';
  const remoteEncoderImplemented =
    remoteOledImplemented &&
    encoderCap.present &&
    encoderCap.implemented &&
    encoderCap.freshness !== 'local-preview';

  const clearTimer = () => {
    if (timerRef.current !== null) {
      window.clearTimeout(timerRef.current);
      timerRef.current = null;
    }
  };

  useEffect(() => {
    sessionRef.current += 1;
    clearTimer();
    actionLockRef.current = false;
    setSnapshot(null);
    latestSnapshot.current = null;
    setLoading(false);
    setActionPending(false);
    setUnavailable(false);
    setErrorMessage(null);
    setActionFeedback(null);
  }, [wsState, megaLinkState, remoteOledImplemented, active]);

  useEffect(() => {
    const cancel = () => { ++inputEpoch.current; };
    window.addEventListener('blur', cancel); document.addEventListener('visibilitychange', cancel);
    return () => { ++inputEpoch.current; window.removeEventListener('blur', cancel); document.removeEventListener('visibilitychange', cancel); };
  }, []);

  useEffect(() => {
    if (!active || !snapshot) return;
    const id = window.setInterval(() => setNowMs(Date.now()), 500);
    return () => window.clearInterval(id);
  }, [active, snapshot]);

  const fetchSnapshot = useCallback(
    async (manual = false, duringAction = false) => {
      if (!active || !remoteOledImplemented || (actionLockRef.current && !duringAction)) return;
      if (typeof document !== 'undefined' && document.visibilityState !== 'visible' && !manual) return;
      if (inFlightRef.current) return inFlightRef.current;

      const currentSession = sessionRef.current;
      latestSnapshot.current = null;
      if (manual) {
        setUnavailable(false);
        setActionFeedback({
          tone: 'loading',
          message: 'Sincronizando pantalla y seleccion actual desde el ATmega2560…',
        });
      }
      setLoading(true);

      const task = (async () => {
        try {
          const res = await requestCommand<Record<string, unknown>>('robot.screen.get', {}, 2500);
          if (currentSession !== sessionRef.current) return;
          if (res.status !== 'completed') {
            setErrorMessage('El controlador acepto la consulta sin entregar captura final.');
            if (manual) {
              setActionFeedback({
                tone: 'error',
                message: 'Consulta aceptada sin captura final.',
              });
            }
            return;
          }
          const parsed = parseRemoteOled(res.payload);
          latestSnapshot.current = parsed;
          setSnapshot(parsed);
          setNowMs(Date.now());
          setErrorMessage(null);
          setUnavailable(false);
          if (manual) {
            const screenTitle = getScreenMetadata(parsed.screenCode)?.name ?? parsed.screenCode;
            setActionFeedback({
              tone: 'success',
              message: `Pantalla ${screenTitle} sincronizada desde el controlador.`,
            });
          }
        } catch (err) {
          if (currentSession !== sessionRef.current) return;
          const code = err instanceof Afr2CommandError ? err.code : null;
          const msg = err instanceof Error ? err.message : 'Fallo al consultar robot.screen.get.';
          setErrorMessage(msg);
          if (code === 'CAPABILITY_UNAVAILABLE') setUnavailable(true);
          if (manual) {
            setActionFeedback({
              tone: 'error',
              message: msg,
            });
          }
        } finally {
          if (currentSession === sessionRef.current) setLoading(false);
          inFlightRef.current = null;
        }
      })();

      inFlightRef.current = task;
      return task;
    },
    [active, remoteOledImplemented, requestCommand]
  );

  useEffect(() => {
    clearTimer();
    if (!active || !remoteOledImplemented || unavailable) return;
    let cancelled = false;
    const loop = async () => {
      await fetchSnapshot(false);
      if (cancelled || !active || !remoteOledImplemented || unavailable) return;
      timerRef.current = window.setTimeout(() => void loop(), POLL_INTERVAL_MS);
    };
    const onVisibility = () => {
      clearTimer();
      if (document.visibilityState === 'visible' && !cancelled) void loop();
    };
    void loop();
    document.addEventListener('visibilitychange', onVisibility);
    return () => {
      cancelled = true;
      clearTimer();
      document.removeEventListener('visibilitychange', onVisibility);
    };
  }, [active, remoteOledImplemented, unavailable, fetchSnapshot, pollRevision]);

  const ageMs = snapshot ? Math.max(0, nowMs - snapshot.receivedAtMs) : null;
  const fresh = Boolean(snapshot && ageMs !== null && ageMs <= STALE_SNAPSHOT_MS && !errorMessage);

  const sendRemoteAction = async (action: 'rotate' | 'select' | 'menu', delta: -1 | 0 | 1, label: string, steps = 1) => {
    if (!active || !remoteEncoderImplemented || !fresh || actionLockRef.current || inFlightRef.current) return;
    const observed = snapshot;
    const currentSession = sessionRef.current;
    const epoch = inputEpoch.current;
    actionLockRef.current = true;
    clearTimer();
    setActionPending(true);
    setErrorMessage(null);
    setLastActionLabel(label);
    setActionFeedback({
      tone: 'loading',
      message: `Enviando al controlador: ${label}…`,
    });

    try {
      const initial = latestSnapshot.current;
      if (!initial || !observed || initial.revision !== observed.revision || initial.screenCode !== observed.screenCode || initial.selection !== observed.selection || initial.firstVisible !== observed.firstVisible) throw new Error('La captura cambio antes de la entrada. Revisala antes de continuar.');
      for (let step = 0; step < Math.min(3, steps); step++) {
        if (currentSession !== sessionRef.current || epoch !== inputEpoch.current || document.visibilityState !== 'visible') return;
        const before = latestSnapshot.current;
        if (!before || before.revision === null || Date.now() - before.receivedAtMs > STALE_SNAPSHOT_MS) throw new Error('Se necesita una captura nueva antes de continuar.');
        const res = await requestCommand<Record<string, unknown>>('robot.ui.action', { action, delta, revision: before.revision }, 2000);
        if (currentSession !== sessionRef.current) return;
        if (res.status !== 'completed') throw new Error('Accion aceptada sin confirmacion final del controlador.');
        await fetchSnapshot(false, true);
        if (currentSession !== sessionRef.current || epoch !== inputEpoch.current) return;
        const after = latestSnapshot.current as RemoteOledSnapshot | null;
        if (!after) throw new Error('No llego una captura posterior a la accion.');
        if (action === 'rotate' && steps > 1) {
          const count = getScreenMetadata(before.screenCode)?.itemCount;
          if (after.screenCode !== before.screenCode || !count || after.selection !== (before.selection + delta + count) % count) throw new Error('Cambio la pantalla o la seleccion durante el desplazamiento. Revisa la captura antes de continuar.');
        }
      }
      setActionFeedback({
        tone: 'success',
        message: `Confirmado por ATmega2560: ${label}. Captura actualizada.`,
      });
    } catch (err) {
      if (currentSession !== sessionRef.current) return;
      const code = err instanceof Afr2CommandError ? err.code : null;
      const msg = err instanceof Error ? err.message : 'Fallo al enviar robot.ui.action.';
      setErrorMessage(msg);
      setActionFeedback({ tone: 'error', message: msg });
      if (code === 'CAPABILITY_UNAVAILABLE') setUnavailable(true);
    } finally {
      if (currentSession === sessionRef.current) {
        actionLockRef.current = false; setActionPending(false); setPollRevision(value => value + 1);
        if (epoch !== inputEpoch.current) setActionFeedback({ tone: 'neutral', message: 'Interaccion interrumpida. Revisa la captura actual antes de continuar.' });
      }
    }
  };

  const blockedReason = !linksReady
    ? `Sin enlace completo (Web↔ESP: ${wsState}, ESP↔ATmega: ${megaLinkState}).`
    : !remoteOledImplemented
      ? oledCap.reason || 'El firmware actual aun no publica capturas remotas (robot.screen.get).'
      : !remoteEncoderImplemented
        ? encoderCap.reason || 'La capacidad remota del encoder (robot.ui.action) no esta habilitada.'
        : !snapshot
          ? 'Esperando primera captura valida del ATmega2560 para habilitar acciones.'
          : !fresh
            ? `Captura retenida caducada (${formatAgeLabel(ageMs)}) o con error; actualiza la pantalla antes de operar.`
            : actionPending
              ? 'Solicitud de accion remota en curso; esperando respuesta del controlador.'
              : loading
                ? 'Actualizando captura desde el controlador…'
                : null;

  const controlsAvailable = active && linksReady && remoteEncoderImplemented && fresh;
  const controlsBusy = loading || actionPending;

  const sourceLabel = snapshot ? 'Framebuffer recibido' : '';

  return (
    <div className="space-y-4">
      <div className="flex flex-wrap items-center justify-between gap-2">
        <div className="flex flex-wrap items-center gap-2 text-xs font-mono">
          <span className="afr2-status-pill" data-state={wsState}>
            <span className="afr2-status-dot" aria-hidden="true" />
            ESP: {wsState}
          </span>
          <span className="afr2-status-pill" data-state={megaLinkState}>
            <span className="afr2-status-dot" aria-hidden="true" />
            Mega: {megaLinkState}
          </span>
          {snapshot && (
            <span className="afr2-status-pill" data-state={fresh ? 'ready' : 'stale'}>
              <span className="afr2-status-dot" aria-hidden="true" />
              {sourceLabel} · {formatAgeLabel(ageMs)}
            </span>
          )}
        </div>
      </div>

      {!linksReady && (
        <StatusNotice tone="warning" title="Sin enlace completo para OLED en vivo">
          Se requiere sesion Web↔ESP (`ready`) y enlace UART ESP↔ATmega2560 (`ready`) para consultar `robot.screen.get`. Los controles permanecen visibles y deshabilitados abajo.
        </StatusNotice>
      )}

      {linksReady && !remoteOledImplemented && (
        <StatusNotice tone="info" title="Servicio OLED remoto pendiente en firmware" badge="Sin captura remota">
          {oledCap.reason || 'El ATmega2560 y el ESP8266 aun no exponen robot.screen.get en vivo.'} No se dibuja una pantalla simulada como si proviniera del auto.
        </StatusNotice>
      )}

      {errorMessage && (
        <StatusNotice tone="error" title="No se pudo actualizar u operar la OLED remota">
          {errorMessage}
        </StatusNotice>
      )}

      {snapshot ? (
        <div className="space-y-3">
          {!fresh && (
            <StatusNotice tone="warning" title="Captura retenida caducada o con fallo posterior">
              Ultima muestra recibida {formatAgeLabel(ageMs)}. Las acciones de encoder quedan bloqueadas hasta recibir una captura fresca.
            </StatusNotice>
          )}
          <div className="afr2-oled-with-side"><div className={fresh ? '' : 'afr2-live-oled__stale'}>
            <OledCanvas128x64
              compact
              oledBuffer={snapshot.buffer}
              screenCode={snapshot.screenCode}
              sourceMode="remote-snapshot"
              showGrid={false}
              scale={4}
            />
          </div><MenuSideControls selection={snapshot.selection} firstVisible={snapshot.firstVisible}
            count={getScreenMetadata(snapshot.screenCode)?.itemCount ?? null} visibleRows={getScreenMetadata(snapshot.screenCode)?.visibleRows ?? 3}
            disabled={!controlsAvailable || controlsBusy}
            onHighlight={index => { const delta = index - snapshot.selection; if (delta) void sendRemoteAction('rotate', delta < 0 ? -1 : 1, `Resaltar opcion ${index + 1}`, Math.abs(delta)); }}
            onScroll={direction => { const count = getScreenMetadata(snapshot.screenCode)?.itemCount ?? 0; const steps = Math.min(3, direction < 0 ? snapshot.selection : count - snapshot.selection - 1); if (steps > 0) void sendRemoteAction('rotate', direction, direction < 0 ? 'Desplazar seleccion hacia arriba' : 'Desplazar seleccion hacia abajo', steps); }} />
          </div>
          <p className="afr2-help">
            {!fresh ? 'Ultima captura caducada · ' : ''}
            {sourceLabel} · Captura {formatAgeLabel(ageMs)} · Seleccion {snapshot.selection} · Ventana{' '}
            {snapshot.firstVisible}
          </p>
        </div>
      ) : (
        <div className="rounded-[var(--radius-md)] border border-[var(--border)] bg-[var(--surface-solid)] p-6 text-center text-sm text-[var(--text-muted)]">
          {loading ? 'Consultando captura inicial a robot.screen.get…' : 'Sin captura remota disponible para representar en el visor 128×64.'}
        </div>
      )}

      <EncoderControl
        available={controlsAvailable}
        busy={controlsBusy}
        disabledReason={blockedReason}
        statusMessage={actionFeedback?.message ?? null}
        statusTone={actionFeedback?.tone ?? 'neutral'}
        onRefresh={() => void fetchSnapshot(true)}
        refreshDisabled={!linksReady || !remoteOledImplemented}
        refreshLoading={loading}
        lastEventLabel={lastActionLabel}
        onRotate={(delta) =>
          void sendRemoteAction('rotate', delta, delta < 0 ? 'Giro izquierdo (-1)' : 'Giro derecho (+1)')
        }
        onPress={(kind) =>
          void sendRemoteAction(
            kind === 'long' ? 'menu' : 'select',
            0,
            kind === 'long' ? 'SW largo (≥ 800 ms · Menu principal)' : 'SW corto (< 800 ms · Confirmar)'
          )
        }
      />
    </div>
  );
}
