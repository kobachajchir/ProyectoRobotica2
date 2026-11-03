/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
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
import { Afr2OledBuffer } from './oledRasterizer';
import { SegmentedControl } from '../../components/ui/SegmentedControl';
import { UiIcon } from '../../components/ui/UiIcon';

const POLL_INTERVAL_MS = 1200;
const STALE_SNAPSHOT_MS = 4000;
/** AFR2-20261102-22: periodic asks for the framebuffer every POLL_INTERVAL_MS; change only when
 * robot.screen.status (already read once per second by ConnectionContext) reports another
 * screen, revision, selection or window. */
type RefreshMode = 'periodic' | 'change';
const REFRESH_MODE_KEY = 'afr2-oled-refresh-mode';
const REFRESH_ITEMS = [
  { value: 'periodic', label: 'Periódica', icon: 'refresh', title: 'Pide la pantalla cada 1,2 s, aunque no cambie.' },
  { value: 'change', label: 'Al cambiar', icon: 'screen', title: 'Pide la pantalla solo cuando el auto informa otra pantalla o selección.' },
] as const;
const BLANK_OLED = new Afr2OledBuffer();
function readRefreshMode(): RefreshMode {
  try { return window.localStorage.getItem(REFRESH_MODE_KEY) === 'change' ? 'change' : 'periodic'; } catch { return 'periodic'; }
}

export function LiveOledViewer({ active = true }: { active?: boolean }) {
  const { robotState, wsState, megaLinkState, capabilities, requestCommand, screenStatus, registerScreenViewer, screenSubscribed } = useConnection();
  useEffect(() => active ? registerScreenViewer() : undefined, [active, registerScreenViewer]);
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
  const [refreshMode, setRefreshModeState] = useState<RefreshMode>(readRefreshMode);
  const setRefreshMode = (mode: RefreshMode) => {
    setRefreshModeState(mode);
    try { window.localStorage.setItem(REFRESH_MODE_KEY, mode); } catch { /* preference only */ }
  };
  const refreshModeRef = useRef(refreshMode); refreshModeRef.current = refreshMode;
  // Status key observed when the shown capture was requested; in change mode a different key means "ask again".
  const statusKey = screenStatus ? `${screenStatus.screenCode}:${screenStatus.navigationRevision}:${screenStatus.selection}:${screenStatus.firstVisible}` : null;
  const statusKeyRef = useRef(statusKey); statusKeyRef.current = statusKey;
  const [fetchedKey, setFetchedKey] = useState<string | null>(null);
  const validatedAtRef = useRef(0);
  const [validatedAtMs, setValidatedAtMs] = useState(0);

  const inFlightRef = useRef<Promise<void> | null>(null);
  const actionLockRef = useRef(false);
  const sessionRef = useRef(0);
  const timerRef = useRef<number | null>(null);

  useEffect(()=>{if(!active)return;latestSnapshot.current=null;++inputEpoch.current;setUnavailable(false);setPollRevision(v=>v+1);},[active,screenStatus?.screenCode,screenStatus?.navigationRevision]);

  const linksReady = wsState === 'ready' && megaLinkState === 'ready';
  const oledCap = capabilities.oled_128x64;
  const encoderCap = capabilities.ui_encoder_sw;
  const remoteOledImplemented =
    linksReady && screenSubscribed && oledCap.present && oledCap.implemented && oledCap.freshness !== 'local-preview';
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
    // The last capture stays drawn (it turns stale by age); only a closed viewer forgets it.
    if (!active) { setSnapshot(null); setFetchedKey(null); }
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
      const keyAtStart = statusKeyRef.current;
      latestSnapshot.current = null;
      if (manual) {
        setUnavailable(false);
        setActionFeedback({
          tone: 'loading',
          message: 'Sincronizando pantalla y selección actual desde el ATmega2560…',
        });
      }
      setLoading(true);

      const task = (async () => {
        try {
          const res = await requestCommand<Record<string, unknown>>('robot.screen.get', {}, 2500);
          if (currentSession !== sessionRef.current) return;
          if (res.status !== 'completed') {
            setErrorMessage('El controlador aceptó la consulta sin entregar captura final.');
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
          setFetchedKey(keyAtStart);
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
      if (cancelled || !active || !remoteOledImplemented || unavailable || refreshMode !== 'periodic') return;
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
  }, [active, remoteOledImplemented, unavailable, fetchSnapshot, pollRevision, refreshMode]);

  const statusChanged = refreshMode === 'change' && statusKey !== null && fetchedKey !== statusKey;
  // Change mode: ask once per reported change (actions fetch on their own).
  useEffect(() => {
    if (!statusChanged || !active || !remoteOledImplemented || unavailable || actionLockRef.current) return;
    void fetchSnapshot(false);
  }, [statusChanged, statusKey, fetchedKey, active, remoteOledImplemented, unavailable, fetchSnapshot]);
  // Change mode: every unchanged status reply confirms that the drawn capture is still current.
  useEffect(() => {
    if (refreshMode !== 'change' || !screenStatus || !snapshot || statusChanged) return;
    const now = Date.now(); validatedAtRef.current = now; setValidatedAtMs(now);
  }, [refreshMode, screenStatus, snapshot, statusChanged]);

  const validFromMs = snapshot ? Math.max(snapshot.receivedAtMs, refreshMode === 'change' ? validatedAtMs : 0) : 0;
  const ageMs = snapshot ? Math.max(0, nowMs - validFromMs) : null;
  const staleView = Boolean(snapshot && ((ageMs ?? 0) > STALE_SNAPSHOT_MS || errorMessage));
  const fresh = Boolean(snapshot && !staleView && !statusChanged);

  const sendRemoteAction = async (action: 'rotate' | 'select' | 'menu' | 'back', delta: -1 | 0 | 1, label: string, steps = 1) => {
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
      if (!initial || !observed || initial.revision !== observed.revision || initial.screenCode !== observed.screenCode || initial.selection !== observed.selection || initial.firstVisible !== observed.firstVisible) throw new Error('La captura cambió antes de la entrada. Revisala antes de continuar.');
      for (let step = 0; step < Math.min(3, steps); step++) {
        if (currentSession !== sessionRef.current || epoch !== inputEpoch.current || document.visibilityState !== 'visible') return;
        const before = latestSnapshot.current;
        const beforeValid = before ? Math.max(before.receivedAtMs, refreshModeRef.current === 'change' ? validatedAtRef.current : 0) : 0;
        if (!before || before.revision === null || Date.now() - beforeValid > STALE_SNAPSHOT_MS) throw new Error('Se necesita una captura nueva antes de continuar.');
        const res = await requestCommand<Record<string, unknown>>('robot.ui.action', { action, delta, revision: before.revision }, 2000);
        if (currentSession !== sessionRef.current) return;
        if (res.status !== 'completed') throw new Error('Acción aceptada sin confirmación final del controlador.');
        await fetchSnapshot(false, true);
        if (currentSession !== sessionRef.current || epoch !== inputEpoch.current) return;
        const after = latestSnapshot.current as RemoteOledSnapshot | null;
        if (!after) throw new Error('No llegó una captura posterior a la acción.');
        if (action === 'rotate' && steps > 1) {
          const count = before.itemCount ?? getScreenMetadata(before.screenCode)?.itemCount;
          if (after.screenCode !== before.screenCode || !count || after.selection !== (before.selection + delta + count) % count) throw new Error('Cambió la pantalla o la selección durante el desplazamiento. Revisá la captura antes de continuar.');
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
        if (epoch !== inputEpoch.current) setActionFeedback({ tone: 'neutral', message: 'Interacción interrumpida. Revisá la captura actual antes de continuar.' });
      }
    }
  };

  const blockedReason = !linksReady
    ? `Sin enlace completo (Web↔ESP: ${wsState}, ESP↔ATmega: ${megaLinkState}).`
    : !remoteOledImplemented
      ? oledCap.reason || 'El firmware actual aún no publica capturas remotas (robot.screen.get).'
      : !remoteEncoderImplemented
        ? encoderCap.reason || 'La capacidad remota del encoder (robot.ui.action) no está habilitada.'
        : !snapshot
          ? 'Esperando primera captura válida del ATmega2560 para habilitar acciones.'
          : statusChanged
            ? 'La pantalla cambió en el auto; actualizando la captura…'
          : !fresh
            ? `Captura retenida caducada (${formatAgeLabel(ageMs)}) o con error; actualizá la pantalla antes de operar.`
            : actionPending
              ? 'Solicitud de acción remota en curso; esperando respuesta del controlador.'
              : loading
                ? 'Actualizando captura desde el controlador…'
                : null;

  const controlsAvailable = active && linksReady && remoteEncoderImplemented && fresh;
  const controlsBusy = loading || actionPending;

  const sourceLabel = snapshot ? (refreshMode === 'change' ? 'Captura vigente' : 'Framebuffer recibido') : '';

  return (
    <div className="afr2-live-oled-layout">
      <div className="afr2-live-oled__topbar">
        <span className="afr2-live-oled__last afr2-tabular">Última entrada: <strong>{lastActionLabel}</strong></span>
        <button type="button" className="afr2-live-oled__sync" onClick={() => void fetchSnapshot(true)}
          disabled={!linksReady || !remoteOledImplemented || loading || actionPending}
          title="Sincronizar pantalla manualmente con el controlador">
          <UiIcon name="refresh" size={16} className={loading ? 'animate-spin' : undefined} />
          <span>{loading ? 'Sincronizando…' : <>Actualizar<span className="afr2-live-oled__sync-long"> pantalla</span></>}</span>
        </button>
      </div>
      <div className="afr2-live-oled__statusbar">
        <div className="flex flex-wrap items-center gap-2 text-xs font-mono afr2-live-oled__links">
          <span className="afr2-status-pill" data-state={wsState}>
            <span className="afr2-status-dot" aria-hidden="true" />
            ESP: {wsState}
          </span>
          <span className="afr2-status-pill" data-state={megaLinkState}>
            <span className="afr2-status-dot" aria-hidden="true" />
            Mega: {megaLinkState}
          </span>
          {snapshot && (
            <span className="afr2-status-pill" data-state={staleView ? 'stale' : 'ready'}>
              <span className="afr2-status-dot" aria-hidden="true" />
              {loading || statusChanged ? 'Actualizando…' : `${sourceLabel} · ${formatAgeLabel(ageMs)}`}
            </span>
          )}
        </div>
        <SegmentedControl items={REFRESH_ITEMS} value={refreshMode} onChange={setRefreshMode}
          ariaLabel="Actualización de la pantalla en vivo" size="sm" fill="never" className="afr2-live-oled__refresh" />
      </div>
      <div className="afr2-live-oled__notices empty:hidden">

      {!linksReady && (
        <StatusNotice tone="warning" title="Sin enlace completo para OLED en vivo">
          Se requiere sesión Web↔ESP (`ready`) y enlace UART ESP↔ATmega2560 (`ready`) para consultar `robot.screen.get`. Los controles permanecen visibles y deshabilitados abajo.
        </StatusNotice>
      )}

      {linksReady && !remoteOledImplemented && (
        <StatusNotice tone="info" title="Servicio OLED remoto pendiente en firmware" badge="Sin captura remota">
          {oledCap.reason || 'El ATmega2560 y el ESP8266 aún no exponen robot.screen.get en vivo.'} No se dibuja una pantalla simulada como si proviniera del auto.
        </StatusNotice>
      )}

      {errorMessage && (
        <StatusNotice tone="error" title="No se pudo actualizar u operar la OLED remota">
          {errorMessage}
        </StatusNotice>
      )}

      </div>
      <div className="afr2-live-oled__body">
      <div className="afr2-live-oled__display">
      {snapshot ? (
        <div className="space-y-3">
          {staleView && (
            <StatusNotice tone="warning" title="Captura retenida caducada o con fallo posterior">
              Última muestra recibida {formatAgeLabel(ageMs)}. Las acciones de encoder quedan bloqueadas hasta recibir una captura fresca.
            </StatusNotice>
          )}
          <div className="afr2-oled-with-side"><div className="afr2-live-oled__screen" data-stale={staleView || undefined} aria-busy={loading || statusChanged}>
            <OledCanvas128x64
              compact
              oledBuffer={snapshot.buffer}
              screenCode={snapshot.screenCode}
              sourceMode="remote-snapshot"
              showGrid={false}
              scale={4}
            />
          </div><MenuSideControls selection={snapshot.selection} firstVisible={snapshot.firstVisible}
            count={snapshot.itemCount ?? getScreenMetadata(snapshot.screenCode)?.itemCount ?? null} visibleRows={getScreenMetadata(snapshot.screenCode)?.visibleRows ?? 3}
            disabled={!controlsAvailable || controlsBusy}
            onBack={getScreenMetadata(snapshot.screenCode)?.role === 'menu' ? () => void sendRemoteAction('back', 0, 'Volver') : undefined}
            onHighlight={index => { const delta = index - snapshot.selection; if (delta) void sendRemoteAction('rotate', delta < 0 ? -1 : 1, `Resaltar opción ${index + 1}`, Math.abs(delta)); }}
            onScroll={direction => { const count = snapshot.itemCount ?? getScreenMetadata(snapshot.screenCode)?.itemCount ?? 0; const steps = Math.min(3, direction < 0 ? snapshot.selection : count - snapshot.selection - 1); if (steps > 0) void sendRemoteAction('rotate', direction, direction < 0 ? 'Desplazar selección hacia arriba' : 'Desplazar selección hacia abajo', steps); }} />
          </div>
          <p className="afr2-help">
            {staleView ? 'Última captura caducada · ' : ''}
            {sourceLabel} · Captura {formatAgeLabel(ageMs)} · Selección {snapshot.selection} · Ventana{' '}
            {snapshot.firstVisible}
          </p>
        </div>
      ) : (
        // Same bezel and size as a real capture, so the first frame does not move the layout.
        <div className="afr2-live-oled__screen afr2-live-oled__placeholder" aria-busy={loading}>
          <OledCanvas128x64 compact oledBuffer={BLANK_OLED} screenCode="" sourceMode="remote-snapshot"
            showGrid={false} scale={4} badge={loading ? 'Consultando…' : 'Sin captura'} title="Pantalla del auto" />
          <p className="afr2-live-oled__placeholder-text" role="status">
            {loading ? 'Consultando la primera captura…' : 'Sin captura remota disponible.'}
          </p>
        </div>
      )}
      </div>
      <div className="afr2-live-oled__controls">
      <EncoderControl longPressMs={robotState?.longPressMs??800}
        available={controlsAvailable}
        busy={controlsBusy}
        disabledReason={blockedReason}
        statusMessage={actionFeedback?.message ?? null}
        statusTone={actionFeedback?.tone ?? 'neutral'}
        onRotate={(delta) =>
          void sendRemoteAction('rotate', delta, delta < 0 ? 'Giro izquierdo (-1)' : 'Giro derecho (+1)')
        }
        onPress={(kind) =>
          void sendRemoteAction(
            kind === 'long' ? 'menu' : 'select',
            0,
            kind === 'long' ? 'SW largo (≥ 800 ms · Retorno al soltar)' : 'SW corto (< 800 ms · Confirmar)'
          )
        }
      />
      </div>
      </div>
    </div>
  );
}
