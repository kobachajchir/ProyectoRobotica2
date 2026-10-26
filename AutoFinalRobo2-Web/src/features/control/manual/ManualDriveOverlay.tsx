import { lazy, Suspense, useCallback, useEffect, useId, useMemo, useRef, useState } from 'react';
import { FloatingModal, type ModalOrigin } from '../../../components/ui/FloatingModal';
import { RenderBoundary } from '../../../components/ui/RenderBoundary';
import { Button } from '../../../components/ui/Button';
import { UiIcon } from '../../../components/ui/UiIcon';
import { clearRobotModelCache } from '../../../components/three/RobotModel';
import { useConnection } from '../../connection/ConnectionContext';
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
export function ManualDriveOverlay({ open, origin, onClose, onRelease, onDirectionChange, canDrive, speed, maxSpeed, onSpeedChange, stopSignal, error }: {
  open: boolean; origin: ModalOrigin | null; onClose: () => void; onRelease: () => void;
  onDirectionChange: (direction: ManualDirection) => void; canDrive: boolean;
  speed: number; maxSpeed: number; onSpeedChange: (speed: number) => void; stopSignal: number | null; error: string | null;
}) {
  const uid = useId();
  const { setOperationActivity } = useConnection();
  const [mode, setMode] = useState<'preview' | 'robot'>('preview');
  const modeRef = useRef(mode);
  const wasOpen = useRef(false);
  const robotSent = useRef(false);
  const [previewSpeed, setPreviewSpeed] = useState(400);
  const [modelReady, setModelReady] = useState(false);
  const [modelFailed, setModelFailed] = useState(false);
  const [viewerKey, setViewerKey] = useState(0);
  const [keyboardPressed, setKeyboardPressed] = useState(false);
  const [headingRad, setHeadingRad] = useState(0);
  const [turnSide, setTurnSide] = useState(0);
  const orientation = useRef({ angle: 0, side: 0, anchor: 0 });
  const turnFrame = useRef(0);
  const [direction, setDirection] = useState<ManualDirection | null>(null);
  const input = useRef<HTMLDivElement>(null);
  const keys = useRef(new Set<string>());
  const current = useRef<ManualDirection | null>(null);
  const callbacks = useRef({ onRelease, onDirectionChange }); callbacks.current = { onRelease, onDirectionChange };
  const physicalReady = canDrive && !error && modelReady && !modelFailed;
  const enabled = open && (mode === 'preview' || physicalReady);
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
      modeRef.current = canDrive && !error ? 'robot' : 'preview'; setMode(modeRef.current);
    } else if (modeRef.current === 'robot' && (!canDrive || error)) {
      modeRef.current = 'preview'; setMode('preview'); cancel();
    }
  }, [open, canDrive, error, cancel, resetPointers, resetHeading]);
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
    if (!value && modeRef.current === 'robot') cancel();
    setModelReady(value);
  }, [cancel]);
  const failed = useCallback(() => {
    if (modeRef.current === 'robot') {
      cancel(); modeRef.current = 'preview'; setMode('preview');
    }
    setModelReady(false); setModelFailed(true);
  }, [cancel]);
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
  const displayedSpeed = mode === 'preview' ? previewSpeed : Math.max(10, Math.min(speed, displayedMaxSpeed));
  const wheelSpeeds = useMemo(() => {
    if (!direction || !enabled) return undefined;
    const c = Math.cos(headingRad), s = Math.sin(headingRad);
    const vx = direction.vx * c + direction.vy * s, vy = -direction.vx * s + direction.vy * c;
    const perAxis = Math.floor(displayedSpeed / Math.max(.01, Math.abs(vx) + Math.abs(vy)));
    return inverseMecanumKinematics(vx * perAxis, vy * perAxis, 0, DEFAULT_KINEMATIC_PROFILE.lxPlusLyMm);
  }, [direction, displayedSpeed, enabled, headingRad]);
  const headingDegrees = ((Math.round(headingRad * 1800 / Math.PI) % 3600) + 3600) % 3600 / 10;
  return <FloatingModal open={open} origin={origin} size="fullscreen" title="Conduccion manual" kicker="Control tactil"
    initialFocusRef={input} onBeforeClose={cancel} onBeforeStop={cancel} onClose={onClose}>
    <div className="afr2-manual-drive" data-pressed={pressed} data-mode={mode}>
      <div className="afr2-manual-stage">
        <div className="afr2-manual-shadow" aria-hidden="true" />
        <div className="afr2-manual-mode"><span className="afr2-manual-mode-label" role="status">{mode === 'preview' ? <><UiIcon name="cube" /> Simulacion local</> : <><UiIcon name="wifi" /> Control del robot</>}</span>
          {mode === 'preview' && <span id={`${uid}-heading`} className="afr2-manual-heading-angle" aria-live="off"><span className="sr-only">Rumbo simulado: </span>{headingDegrees.toFixed(1)}°</span>}</div>
        <div className="afr2-manual-model" aria-hidden="true" style={{ transform: `translate(${-8 * (direction?.vy ?? 0)}px, ${-8 * (direction?.vx ?? 0)}px)` }}><RenderBoundary key={viewerKey} fallback={<Unavailable onFailure={failed} />}>
          <Suspense fallback={<p className="afr2-manual-model-message">Cargando modelo 3D…</p>}><Model onReady={ready} wheelSpeeds={wheelSpeeds} headingRad={headingRad} /></Suspense>
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
        <div ref={input} className="afr2-manual-input" role="group" tabIndex={0} aria-label="Un dedo desplaza; gira dos dedos para rotar la simulacion"
          aria-describedby={`${uid}-help ${uid}-heading`} aria-keyshortcuts="ArrowUp ArrowDown ArrowLeft ArrowRight Q E 0" aria-disabled={!enabled} onPointerDown={down} onPointerMove={move}
          onPointerUp={end} onPointerCancel={end} onLostPointerCapture={lost}
          onBlur={cancel} onContextMenu={event => { event.preventDefault(); cancel(); }} />
        <span className="afr2-manual-gesture-hint" aria-hidden="true">{twisting ? 'Gira los dos dedos · Levanta ambos para volver al arrastre' : pressed && !direction ? 'Arrastra para elegir una direccion' : ''}</span>
      </div>
      <footer className="afr2-manual-footer">
        <p className="afr2-manual-direction" role="status" aria-live="polite">{twisting ? turnSide > 0 ? 'Giro simulado a la izquierda' : turnSide < 0 ? 'Giro simulado a la derecha' : 'Giro con dos dedos' : direction ? `${mode === 'preview' ? 'Simulando' : 'Moviendo'} hacia ${direction.label}` : mode === 'preview' ? 'Simulacion detenida' : !modelReady ? 'Preparando modelo 3D…' : 'Desliza y mantene para mover'}</p>
        <p id={`${uid}-help`} className="afr2-help">Un dedo desplaza en pantalla · Dos dedos giran la simulacion · Solta para detener.
          <span className="afr2-manual-keyboard-help">Flechas: mover · Q/E: girar 15° · 0: centrar rumbo.</span>
          <span>{mode === 'preview' ? `${error ? 'Consigna sin confirmar. ' : ''}No envia movimientos. Cerra y volve a abrir para usar el robot cuando este disponible.` : 'Vista superior · Direccion y ruedas solicitadas, sin feedback fisico.'}</span></p>
        {modelFailed && <Button onClick={() => { cancel(); clearRobotModelCache(); setModelReady(false); setModelFailed(false); setViewerKey(value => value + 1); }}>Reintentar modelo 3D</Button>}
        <div className="afr2-manual-speed"><label htmlFor={`${uid}-speed`}>{mode === 'preview' ? 'Velocidad simulada' : 'Velocidad de movimiento'}</label><output htmlFor={`${uid}-speed`}>{displayedSpeed} <span>mm/s</span></output>
          <input id={`${uid}-speed`} type="range" min={10} max={displayedMaxSpeed} step={10} value={displayedSpeed}
            onChange={event => { const value = Math.min(displayedMaxSpeed, Number(event.target.value)); if (modeRef.current === 'preview') setPreviewSpeed(value); else onSpeedChange(value); }} /></div>
      </footer>
    </div>
  </FloatingModal>;
}
