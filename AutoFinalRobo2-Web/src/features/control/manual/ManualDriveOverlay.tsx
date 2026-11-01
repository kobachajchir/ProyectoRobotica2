import { ModelAssetProgress } from '../../../components/three/ModelAssetProgress';
import { getModelAssetState, ROBOT_MODEL_URL, subscribeModelAssets } from '../../../components/three/modelAssetLoading';
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { lazy, Suspense, useCallback, useEffect, useId, useMemo, useRef, useState, useSyncExternalStore } from 'react';
import { FloatingModal, type ModalOrigin } from '../../../components/ui/FloatingModal';
import { RenderBoundary } from '../../../components/ui/RenderBoundary';
import { Button } from '../../../components/ui/Button';
import { UiIcon } from '../../../components/ui/UiIcon';
import { clearRobotModelCache } from '../../../components/three/RobotModel';
import { useConnection } from '../../connection/ConnectionContext';
import { Afr2CommandError } from '../../../protocol/afr2Client';
import { inverseMecanumKinematics } from '../sequences/mecanumKinematics';
import { DEFAULT_KINEMATIC_PROFILE } from '../sequences/sequenceTypes';
import { useManualDrivePointers } from './useManualDrivePointers';

const Model = lazy(() => import('./ManualDriveModel').then(module => ({ default: module.ManualDriveModel })));
export interface ManualDirection { id: string; label: string; vx: number; vy: number; angle: number }
const DIRECTIONS: ManualDirection[] = [
  { id: 'n', label: 'adelante', vx: 1, vy: 0, angle: 0 },
  { id: 'ne', label: 'adelante a la derecha', vx: 1, vy: -1, angle: 45 },
  { id: 'e', label: 'la derecha', vx: 0, vy: -1, angle: 90 },
  { id: 'se', label: 'atras a la derecha', vx: -1, vy: -1, angle: 135 },
  { id: 's', label: 'atras', vx: -1, vy: 0, angle: 180 },
  { id: 'sw', label: 'atras a la izquierda', vx: -1, vy: 1, angle: 225 },
  { id: 'w', label: 'la izquierda', vx: 0, vy: 1, angle: 270 },
  { id: 'nw', label: 'adelante a la izquierda', vx: 1, vy: 1, angle: 315 },
];
const KEYS = ['ArrowUp', 'ArrowRight', 'ArrowDown', 'ArrowLeft'];
function Unavailable({ onFailure }: { onFailure: () => void }) {
  useEffect(onFailure, [onFailure]);
  return <p className="afr2-manual-model-message">Modelo 3D no disponible.</p>;
}

/** AFR2-20261025-11. Two-finger rotation is local and never arms physical yaw. */
export function ManualDriveOverlay({ open, origin, onClose, onRelease, onDirectionChange, canDrive, blockedReason, speed, maxSpeed, onSpeedChange, stopSignal, error }: {
  open: boolean; origin: ModalOrigin | null; onClose: () => void; onRelease: () => void;
  onDirectionChange: (direction: ManualDirection) => void; canDrive: boolean; blockedReason: string | null;
  speed: number; maxSpeed: number; onSpeedChange: (speed: number) => void; stopSignal: number | null; error: string | null;
}) {
  const uid = useId();
  const { setOperationActivity, requestCommand, wsState, megaLinkState } = useConnection();
  // AFR2-20261030-08: connection and permission to move are separate states.
  const connected = wsState === 'ready' && megaLinkState === 'ready';
  const [mode, setMode] = useState<'preview' | 'robot'>('preview');
  const modeRef = useRef(mode);
  const wasOpen = useRef(false);
  const wasConnected = useRef(connected);
  const robotSent = useRef(false);
  const [previewSpeed, setPreviewSpeed] = useState(400);
  const [modelReady, setModelReady] = useState(false);
  const modelAsset = useSyncExternalStore(subscribeModelAssets, () => getModelAssetState(ROBOT_MODEL_URL));
  const [modelFailed, setModelFailed] = useState(false);
  const [viewerKey, setViewerKey] = useState(0);
  const [keyboardPressed, setKeyboardPressed] = useState(false);
  const [headingRad, setHeadingRad] = useState(0);
  // AFR2-20261030-12: same 24 heading sectors as the OLED protocol.
  const headingIndex = ((Math.round(headingRad * 12 / Math.PI) % 24) + 24) % 24;
  const viewHeadingRad = connected ? headingIndex * Math.PI / 12 : headingRad;
  const [turnSide, setTurnSide] = useState(0);
  const orientation = useRef({ angle: 0, side: 0, anchor: 0 });
  const turnFrame = useRef(0);
  const [direction, setDirection] = useState<ManualDirection | null>(null);
  const [oledError, setOledError] = useState<string | null>(null);
  const [powerNotice,setPowerNotice]=useState<number|null>(null);
  const powerNoticeTimer=useRef<ReturnType<typeof setTimeout>|null>(null);
  const touchLatest = useRef({ direction: 8, heading: 0, powerPercent:0 });
  touchLatest.current = { direction: direction ? DIRECTIONS.findIndex(d => d.id === direction.id) : 8,
    heading: headingIndex, powerPercent:Math.max(0,Math.min(100,Math.round(speed))) };
  const exitTouch = useRef({onRelease,onClose});
  exitTouch.current={onRelease,onClose};
  const touchFlush = useRef<(() => void) | null>(null);
  useEffect(()=>()=>{if(powerNoticeTimer.current)clearTimeout(powerNoticeTimer.current);},[]);
  const input = useRef<HTMLDivElement>(null);
  useEffect(() => {
    setOledError(null);
    if (!open || !connected) return;
    let cancelled = false, busy = false, last = '', sentAt = 0, activated = false;
    const sync = async () => {
      if (cancelled || busy || document.visibilityState !== 'visible') return;
      const next = touchLatest.current, key = `${next.direction}:${next.heading}:${next.powerPercent}`;
      if (activated && key === last && Date.now() - sentAt < 700) return;
      busy = true;
      try {
        const response = await requestCommand<{powerPercent:number}>('robot.touch.ui', { enabled: true, ...next }, 2000);
        if (!cancelled) {
          if (response.status !== 'completed' || !Number.isInteger(response.payload.powerPercent) || response.payload.powerPercent!==next.powerPercent) throw new Error('Presentacion de potencia sin confirmar; requiere EX01 v10.');
          if(activated && last.split(':')[2]!==String(response.payload.powerPercent)){
            setPowerNotice(response.payload.powerPercent);if(powerNoticeTimer.current)clearTimeout(powerNoticeTimer.current);
            powerNoticeTimer.current=setTimeout(()=>setPowerNotice(null),1200);
          }
          activated = true; last = key; sentAt = Date.now(); setOledError(null);
        }
      } catch (err) { if (!cancelled && activated && err instanceof Afr2CommandError && err.code === 'CAPABILITY_UNAVAILABLE') { cancelled=true; exitTouch.current.onRelease(); exitTouch.current.onClose(); return; } if (!cancelled) setOledError(err instanceof Error ? err.message : 'No se pudo activar la OLED tactil.'); }
      finally { busy = false; }
    };
    const disable = () => { activated = false; last = ''; void requestCommand('robot.touch.ui', { enabled: false, direction: 8, heading: 0, powerPercent:touchLatest.current.powerPercent }, 2000).catch(() => undefined); };
    const visible = () => { if (document.visibilityState !== 'visible') disable(); else void sync(); };
    touchFlush.current = () => { void sync(); };
    void sync(); const timer = window.setInterval(() => void sync(), 100);
    document.addEventListener('visibilitychange', visible);
    return () => { cancelled = true; touchFlush.current = null; clearInterval(timer); document.removeEventListener('visibilitychange', visible); disable(); };
  }, [open, connected, requestCommand]);
  useEffect(() => { touchFlush.current?.(); }, [direction, headingRad, speed]);
  const keys = useRef(new Set<string>());
  const current = useRef<ManualDirection | null>(null);
  const callbacks = useRef({ onRelease, onDirectionChange }); callbacks.current = { onRelease, onDirectionChange };
  // AFR2-20261030-19: share download state with the progress component, then
  // wait for this viewer to report readiness (including its WebGL context).
  const modelLoaded = modelReady && !modelFailed && modelAsset.phase === 'ready';
  const modelUnavailable = modelFailed || modelAsset.phase === 'error';
  const modelBlockedReason = modelUnavailable ? 'No se pudo cargar el modelo 3D. Usa Reintentar modelo 3D.'
    : !modelLoaded ? modelAsset.phase === 'ready' ? 'Preparando la vista del modelo 3D.' : 'Esperando que termine la carga del modelo 3D.'
    : null;
  // A failed motor request cancels its gesture, without becoming a permission latch.
  const physicalReady = canDrive && modelLoaded;
  const enabled = open && modelLoaded && (mode === 'preview' || canDrive);
  const enabledRef = useRef(enabled); enabledRef.current = enabled;
  const robotAvailable = useRef(physicalReady); robotAvailable.current = physicalReady;
  const releaseRobot = useCallback(() => {
    if (!robotSent.current) return;
    robotSent.current = false; callbacks.current.onRelease();
  }, []);
  const publishTurn = useCallback(() => {
    cancelAnimationFrame(turnFrame.current); turnFrame.current = 0;
    const delta = orientation.current.angle - orientation.current.anchor;
    if (Math.abs(delta) >= .01) {
      orientation.current.side = Math.sign(delta); orientation.current.anchor = orientation.current.angle;
    }
    setHeadingRad(orientation.current.angle); setTurnSide(orientation.current.side);
  }, []);
  const rotateBy = useCallback((radians: number) => {
    orientation.current.angle += radians;
    if (!turnFrame.current) turnFrame.current = requestAnimationFrame(publishTurn);
  }, [publishTurn]);
  const resetHeading = useCallback(() => { orientation.current = { angle: 0, side: 0, anchor: 0 }; publishTurn(); }, [publishTurn]);
  useEffect(() => () => cancelAnimationFrame(turnFrame.current), []);
  const clearTranslation = useCallback(() => {
    keys.current.clear(); current.current = null; setDirection(null); setKeyboardPressed(false);
    releaseRobot();
    publishTurn();
  }, [releaseRobot, publishTurn]);
  const startLocalRotation = useCallback(() => {
    clearTranslation(); modeRef.current = 'preview'; setMode('preview');
  }, [clearTranslation]);
  const choose = useCallback((next: ManualDirection | null) => {
    if (current.current?.id === next?.id) return;
    current.current = next; setDirection(next);
    if (next && enabledRef.current && modeRef.current === 'robot' && robotAvailable.current) {
      robotSent.current = true; callbacks.current.onDirectionChange(next);
    } else releaseRobot();
  }, [releaseRobot]);
  const translate = useCallback((dx: number, dy: number) => {
    const length = Math.hypot(dx, dy);
    if (length < (current.current ? 8 : 12)) { choose(null); return; }
    const angle = (Math.atan2(dx, -dy) * 180 / Math.PI + 360) % 360;
    const delta = current.current ? Math.abs(((angle - current.current.angle + 540) % 360) - 180) : 180;
    if (delta >= 30) choose(DIRECTIONS[Math.round(angle / 45) % 8]);
  }, [choose]);
  const { pressed: pointerPressed, twisting, hasPointers, cancel, reset: resetPointers, down, move, end, lost, externalDown } = useManualDrivePointers({
    enabled, keyboardActive: () => keys.current.size > 0, onTranslate: translate, onRelease: clearTranslation,
    onTwistStart: () => { startLocalRotation(); orientation.current.side = 0; orientation.current.anchor = orientation.current.angle; setTurnSide(0); }, onTwist: rotateBy,
  });
  const pressed = pointerPressed || keyboardPressed;
  useEffect(() => {
    if (!open) { wasOpen.current = false; cancel(); return; }
    if (!wasOpen.current) {
      wasOpen.current = true; resetPointers(); resetHeading();
      cancel();
      wasConnected.current = connected;
      modeRef.current = connected ? 'robot' : 'preview'; setMode(modeRef.current);
    } else if (wasConnected.current !== connected) {
      // Cancel held input before changing destination; reconnection never
      // replays an offline gesture against the car.
      cancel(); wasConnected.current = connected;
      modeRef.current = connected ? 'robot' : 'preview'; setMode(modeRef.current);
    } else if (modeRef.current === 'robot' && !canDrive) {
      cancel();
    }
  }, [open, connected, canDrive, cancel, resetPointers, resetHeading]);
  useEffect(() => { if (mode === 'robot' && error) cancel(); }, [error, mode, cancel]);
  useEffect(() => { if (!enabled) cancel(); }, [enabled, cancel]);
  useEffect(() => { cancel(); }, [stopSignal, cancel]);
  useEffect(() => {
    if (current.current && enabled && modeRef.current === 'robot' && robotAvailable.current) {
      robotSent.current = true; callbacks.current.onDirectionChange(current.current);
    }
  }, [speed, enabled]);
  useEffect(() => {
    setOperationActivity('simulation', open && mode === 'preview' && (!!direction || twisting));
    return () => setOperationActivity('simulation', false);
  }, [open, mode, direction, twisting, setOperationActivity]);
  useEffect(() => {
    if (!open) return;
    const hidden = () => { if (document.visibilityState !== 'visible') cancel(); };
    const outsideWindow = (event: globalThis.PointerEvent) => {
      // A null relatedTarget also occurs during capture retargeting. Cancel
      // only at an actual viewport boundary, not when entering the input layer.
      if (hasPointers() && event.pointerType === 'mouse' && event.relatedTarget === null &&
        (event.clientX <= 0 || event.clientY <= 0 || event.clientX >= window.innerWidth - 1 || event.clientY >= window.innerHeight - 1)) cancel();
    };
    window.addEventListener('blur', cancel); window.addEventListener('resize', cancel);
    window.addEventListener('pointerup', end, true); window.addEventListener('pointercancel', end, true);
    window.addEventListener('pointerdown', externalDown, true); document.addEventListener('visibilitychange', hidden);
    window.addEventListener('pointerout', outsideWindow);
    return () => {
      cancel(); window.removeEventListener('blur', cancel); window.removeEventListener('resize', cancel);
      window.removeEventListener('pointerup', end, true); window.removeEventListener('pointercancel', end, true);
      window.removeEventListener('pointerdown', externalDown, true); document.removeEventListener('visibilitychange', hidden);
      window.removeEventListener('pointerout', outsideWindow);
    };
  }, [open, cancel, end, externalDown, hasPointers]);
  const ready = useCallback((value: boolean) => {
    if (!value) {
      enabledRef.current = false; robotAvailable.current = false;
      cancel();
    }
    setModelReady(value);
  }, [cancel]);
  const failed = useCallback(() => { ready(false); setModelFailed(true); }, [ready]);
  useEffect(() => { if (getModelAssetState(ROBOT_MODEL_URL).phase !== 'ready') ready(false); }, [modelAsset.phase, ready]);
  useEffect(() => {
    if (!open) return;
    const key = (event: globalThis.KeyboardEvent) => {
      const modal = input.current?.closest('dialog');
      if (!modal?.open || modal.dataset.phase !== 'open' || input.current?.closest('[inert]')) return;
      if (event.altKey || event.ctrlKey || event.metaKey) { if (keys.current.size) cancel(); return; }
      const turnKey = event.code === 'KeyQ' || event.code === 'KeyE';
      const resetKey = event.code === 'Digit0' || event.code === 'Numpad0';
      if (!KEYS.includes(event.key) && !turnKey && !resetKey) return;
      // The speed field keeps native arrows; other focused buttons must not
      // consume them as navigation while the driving dialog is open.
      if (event.target instanceof Element && event.target.closest('input, select, textarea, [contenteditable="true"]')) return;
      event.preventDefault(); event.stopPropagation();
      document.documentElement.dataset.input = 'keyboard';
      const down = event.type === 'keydown';
      if (!enabledRef.current || hasPointers()) return;
      if (turnKey || resetKey) {
        if (down && !event.repeat) {
          input.current?.focus({ preventScroll: true }); startLocalRotation();
          if (resetKey) resetHeading(); else rotateBy((event.code === 'KeyQ' ? 1 : -1) * Math.PI / 12);
        }
        return;
      }
      if (down && event.repeat) return;
      if (down) { input.current?.focus({ preventScroll: true }); keys.current.add(event.key); }
      else keys.current.delete(event.key);
      const vx = Number(keys.current.has('ArrowUp')) - Number(keys.current.has('ArrowDown'));
      const vy = Number(keys.current.has('ArrowLeft')) - Number(keys.current.has('ArrowRight'));
      setKeyboardPressed(keys.current.size > 0);
      choose(DIRECTIONS.find(item => item.vx === vx && item.vy === vy) ?? null);
    };
    window.addEventListener('keydown', key, true); window.addEventListener('keyup', key, true);
    return () => { window.removeEventListener('keydown', key, true); window.removeEventListener('keyup', key, true); };
  }, [open, cancel, choose, hasPointers, startLocalRotation, resetHeading, rotateBy]);
  const displayedMaxSpeed = mode === 'preview' ? DEFAULT_KINEMATIC_PROFILE.maxWheelSpeedMmS : Math.max(10, maxSpeed);
  const displayedSpeed = mode === 'preview' ? previewSpeed : Math.max(0, Math.min(speed, displayedMaxSpeed));
  const wheelSpeeds = useMemo(() => {
    if (!direction || !enabled) return undefined;
    const c = Math.cos(viewHeadingRad), s = Math.sin(viewHeadingRad);
    const vx = direction.vx * c + direction.vy * s, vy = -direction.vx * s + direction.vy * c;
    const perAxis = Math.floor(displayedSpeed / Math.max(.01, Math.abs(vx) + Math.abs(vy)));
    return inverseMecanumKinematics(vx * perAxis, vy * perAxis, 0, DEFAULT_KINEMATIC_PROFILE.lxPlusLyMm);
  }, [direction, displayedSpeed, enabled, viewHeadingRad]);
  const headingDegrees = connected ? headingIndex * 15 : ((Math.round(headingRad * 1800 / Math.PI) % 3600) + 3600) % 3600 / 10;
  let directionLabel: string;
  if (modelBlockedReason) {
    directionLabel = modelUnavailable ? 'Modelo 3D no disponible' : 'Cargando modelo 3D';
  } else if (connected && mode === 'robot' && !physicalReady) {
    directionLabel = 'Control no disponible';
  } else if (twisting) {
    const turnLabel = connected ? 'Vista' : 'Giro simulado';
    directionLabel = turnSide > 0 ? `${turnLabel} a la izquierda` : turnSide < 0 ? `${turnLabel} a la derecha` : 'Giro con dos dedos';
  } else if (direction) {
    const movementLabel = !connected ? 'Simulando' : mode === 'preview' ? 'Vista local' : 'Moviendo';
    directionLabel = `${movementLabel} hacia ${direction.label}`;
  } else {
    directionLabel = !connected ? 'Simulacion detenida' : mode === 'preview' ? 'Vista local detenida' : 'Desliza y mantene para mover';
  }
  return <FloatingModal open={open} origin={origin} size="fullscreen" title="Conduccion manual" kicker="Control tactil"
    headerStatus={<span className="afr2-manual-connection" data-connected={connected} role="status" aria-live="polite">
      <span className="afr2-manual-connection-dot" aria-hidden="true" />{connected ? 'Conectado' : 'Desconectado'}
    </span>}
    initialFocusRef={input} onBeforeClose={cancel} onBeforeStop={cancel} onClose={onClose}>
    <div className="afr2-manual-drive" data-pressed={pressed} data-mode={mode}>
      <div className="afr2-manual-stage">
        <div className="afr2-manual-shadow" aria-hidden="true" />
        <div className="afr2-manual-status-bar">
          <div className="afr2-manual-mode" data-connected={connected}>{!connected && <span className="afr2-manual-mode-label" role="status"><UiIcon name="cube" />Simulado</span>}
            <span id={`${uid}-heading`} className="afr2-manual-heading-angle" aria-live="off"><span className="sr-only">{connected ? 'Rumbo de la vista: ' : 'Rumbo simulado: '}</span>{headingDegrees.toFixed(1)}°</span></div>
        </div>
        <div className="afr2-manual-model" aria-hidden="true" style={{ transform: `translate(${-8 * (direction?.vy ?? 0)}px, ${-8 * (direction?.vx ?? 0)}px)` }}><RenderBoundary key={viewerKey} fallback={<Unavailable onFailure={failed} />}>
          <Suspense fallback={<p className="afr2-manual-model-message">Cargando modelo 3D…</p>}><Model onReady={ready} wheelSpeeds={wheelSpeeds} headingRad={viewHeadingRad} /></Suspense>
        </RenderBoundary></div>
        <div className="afr2-manual-compass" aria-hidden="true">{DIRECTIONS.map(item => <span key={item.id} className="afr2-manual-stream" data-active={direction?.id === item.id}>
          <svg className="afr2-manual-stream__bearing" width="360" height="240" viewBox="0 0 360 240" preserveAspectRatio="none" fill={`url(#${uid}-trail-${item.id})`} style={{ transform: `rotate(${item.angle - 90}deg)` }}>
            <defs>
              <linearGradient id={`${uid}-trail-${item.id}`} gradientUnits="userSpaceOnUse" x1="0" y1="120" x2="360" y2="120">
                <stop offset="0" stopColor="currentColor" stopOpacity="0" />
                <stop offset=".22" stopColor="currentColor" stopOpacity=".05" />
                <stop offset=".42" stopColor="currentColor" stopOpacity=".28" />
                <stop offset=".66" stopColor="currentColor" stopOpacity=".8" />
                <stop offset=".84" stopColor="currentColor" stopOpacity="1" />
                <stop offset="1" stopColor="currentColor" stopOpacity="1" />
              </linearGradient>
              <linearGradient id={`${uid}-feather-${item.id}`} x1="0" y1="0" x2="0" y2="1">
                <stop offset=".04" stopColor="white" stopOpacity="0" />
                <stop offset=".16" stopColor="white" stopOpacity=".85" />
                <stop offset=".32" stopColor="white" stopOpacity="1" />
                <stop offset=".68" stopColor="white" stopOpacity="1" />
                <stop offset=".84" stopColor="white" stopOpacity=".85" />
                <stop offset=".96" stopColor="white" stopOpacity="0" />
              </linearGradient>
              <mask id={`${uid}-fade-${item.id}`} maskUnits="userSpaceOnUse" x="0" y="0" width="360" height="240">
                <rect width="360" height="240" fill={`url(#${uid}-feather-${item.id})`} />
              </mask>
            </defs>
            <g mask={`url(#${uid}-fade-${item.id})`}>
              <path className="afr2-manual-stream__trace" d="m4 28 34 92-34 92 50 24 54-116L54 4Z" />
              <path className="afr2-manual-stream__trace" d="m124 28 34 92-34 92 50 24 54-116L174 4Z" />
              <path className="afr2-manual-stream__trace" d="m244 28 34 92-34 92 50 24 54-116L294 4Z" />
            </g>
          </svg>
        </span>)}</div>
        <div ref={input} className="afr2-manual-input" role="group" tabIndex={0} aria-label="Un dedo desplaza; gira dos dedos para rotar la vista local"
          aria-describedby={`${uid}-help ${uid}-heading`} aria-keyshortcuts="ArrowUp ArrowDown ArrowLeft ArrowRight Q E 0" aria-disabled={!enabled} onPointerDown={down} onPointerMove={move}
          onPointerUp={end} onPointerCancel={end} onLostPointerCapture={lost}
          onBlur={cancel} onContextMenu={event => { event.preventDefault(); cancel(); }} />
        <span className="afr2-manual-gesture-hint" aria-hidden="true">{twisting ? 'Gira los dos dedos · Levanta ambos para volver al arrastre' : pressed && !direction ? 'Arrastra para elegir una direccion' : ''}</span>
      </div>
      <footer className="afr2-manual-footer">
        <ModelAssetProgress label="Modelo del auto" />
        <p className="afr2-manual-direction" role="status" aria-live="polite">{directionLabel}</p>
        <p id={`${uid}-help`} className="afr2-help">Un dedo desplaza en pantalla · Dos dedos giran la vista local · Solta para detener.
          <span className="afr2-manual-keyboard-help">Flechas: mover · Q/E: girar 15° · 0: centrar rumbo.</span>
          <span>{modelBlockedReason ? modelBlockedReason : !connected ? 'Auto desconectado. Simulacion local sin movimientos fisicos.' : !canDrive ? `Control bloqueado: ${blockedReason || 'PWM manual no disponible.'}` : error ? `Ultima orden sin confirmar: ${error}. Solta y volve a deslizar para reintentar.` : mode === 'preview' ? 'Vista local sin movimientos. Cerra y volve a abrir para controlar el auto.' : 'Vista superior · Direccion y ruedas solicitadas, sin feedback fisico.'}</span></p>
        {mode==='robot' && powerNotice!==null && <div role="status" className="afr2-power-notice"><strong>Velocidad modificada: {Math.round(powerNotice*255/100)} /255</strong><progress max={100} value={powerNotice} aria-label="Potencia representada"/><output>{powerNotice}%</output></div>}
        {oledError && <p className="afr2-help" role="status">OLED: {oledError}</p>}
        {modelUnavailable && <Button onClick={() => { ready(false); clearRobotModelCache(); setModelFailed(false); setViewerKey(value => value + 1); }}>Reintentar modelo 3D</Button>}
        <div className="afr2-manual-speed"><label htmlFor={`${uid}-speed`}>{mode === 'preview' ? connected ? 'Velocidad de la vista local' : 'Velocidad simulada' : 'Potencia de movimiento'}</label><output htmlFor={`${uid}-speed`}>{displayedSpeed} <span>{mode==='preview'?'mm/s':'% del limite PWM'}</span></output>
          <input id={`${uid}-speed`} type="range" min={mode==='preview'?10:0} max={displayedMaxSpeed} step={10} value={displayedSpeed}
            onChange={event => { const value = Math.min(displayedMaxSpeed, Number(event.target.value)); if (modeRef.current === 'preview') setPreviewSpeed(value); else onSpeedChange(value); }} /></div>
      </footer>
    </div>
  </FloatingModal>;
}
