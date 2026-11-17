import { EncoderGlyph } from './EncoderGlyph';
import { UiIcon } from '../../components/ui/UiIcon';
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { useCallback, useEffect, useRef, useState } from 'react';
import { createPortal } from 'react-dom';

export const AFR2_LONG_PRESS_MS = 800;
export const AFR2_ENCODER_LONG_MS = 800;

export interface EncoderControlProps {
  onPressingChange?: (pressed:boolean)=>void;
  longPressMs?: number;
  onRotate: (delta: -1 | 1) => void;
  onPress: (kind: 'short' | 'long') => void;
  onRefresh?: () => void;
  /** Disponibilidad de enlace/capacidad/captura fresca. Si pasa a false, cancela de inmediato cualquier gesto activo. */
  available?: boolean;
  /** Solicitud en vuelo (lectura o acción pendiente). Bloquea iniciar nuevos gestos pero conserva el estado visual largo hasta soltar. */
  busy?: boolean;
  /** Compatibilidad con llamadas que pasen disabled directo. */
  disabled?: boolean;
  /** Motivo explícito mostrado junto al control cuando está deshabilitado o ocupado. */
  disabledReason?: string | null;
  /** Mensaje de estado o confirmación mostrado junto a los controles. */
  statusMessage?: string | null;
  statusTone?: 'neutral' | 'loading' | 'success' | 'error';
  refreshDisabled?: boolean;
  refreshLoading?: boolean;
  lastEventLabel?: string;
  compact?: boolean;
  label?: string;
  /** AFR2-20261103-27. Captura todo el teclado mientras el diálogo que contiene el control está abierto:
   * ← y → giran al soltar, Espacio es SW (mantenido, corto o largo al soltar), Enter no hace nada y Escape queda para cerrar. */
  keyboardCapture?: boolean;
  /** AFR2-20261117-06: nodo bajo la pantalla donde se dibujan los botones circulares (portal). Sin él van dentro de la tarjeta. */
  buttonsHost?: HTMLElement | null;
}

function RefreshSyncIcon({ spinning = false }: { spinning?: boolean }) {
  // AFR2-20261101-07 / VIS-050: icono del registro comun.
  return <UiIcon name="refresh" size={16} className={`shrink-0 ${spinning ? 'animate-spin' : ''}`} />;
}

/**
 * Controles físicos del encoder único KY-040 adaptados de `ScreenStreamWorkspace.tsx` (AFR2-20261023-21 / VIS-018).
 * - Muestra siempre Giro izquierdo, SW y Giro derecho más el botón manual de Actualizar, incluso deshabilitados.
 * - Único botón SW (sin botón USER, Menú ni Atrás).
 * - Umbral configurable (800..3000 ms): corto y largo se emiten solo al soltar; nunca ambos.
 * - Pérdida de disponibilidad, cancelación de puntero, pérdida de foco/captura o cierre del modal cancelan el gesto de inmediato.
 */
export function EncoderControl({
  onRotate,
  longPressMs=800,
  onPress,
  onPressingChange,
  onRefresh,
  available = true,
  busy = false,
  disabled = false,
  disabledReason = null,
  statusMessage = null,
  statusTone = 'neutral',
  refreshDisabled = false,
  refreshLoading = false,
  lastEventLabel,
  compact = false,
  label = 'Controles físicos · Encoder único KY-040',
  keyboardCapture = false,
  buttonsHost = null,
}: EncoderControlProps) {
  const threshold=Math.max(800,Math.min(3000,longPressMs));
  const isAvailable = available && !disabled;
  const controlsDisabled = !isAvailable || busy;

  const [pressing, setPressing] = useState(false);
  const [longVisualActive, setLongVisualActive] = useState(false);
  useEffect(()=>{onPressingChange?.(pressing);},[pressing,onPressingChange]);
  const [pressedRotate, setPressedRotate] = useState<'left' | 'right' | null>(null);

  const gestureThreshold=useRef(threshold);
  const pressStartRef = useRef<number | null>(null);
  const longFiredRef = useRef(false);
  const activeInputSourceRef = useRef<'pointer' | 'keyboard' | null>(null);
  const activeKeyRef = useRef<string | null>(null);
  const timerRef = useRef<ReturnType<typeof setTimeout> | null>(null);
  const rafRef = useRef<number>(0);
  const rotateAnimTimeoutRef = useRef<ReturnType<typeof setTimeout> | null>(null);
  const barRef = useRef<HTMLDivElement | null>(null);

  const setBarScale = (value: number) => {
    if (barRef.current) {
      barRef.current.style.transform = `scaleX(${Math.max(0, Math.min(1, value))})`;
    }
  };

  const clearTimers = useCallback(() => {
    if (timerRef.current) {
      clearTimeout(timerRef.current);
      timerRef.current = null;
    }
    if (rafRef.current) {
      cancelAnimationFrame(rafRef.current);
      rafRef.current = 0;
    }
  }, []);

  const cancelGesture = useCallback(() => {
    clearTimers();
    pressStartRef.current = null;
    longFiredRef.current = false;
    activeInputSourceRef.current = null;
    activeKeyRef.current = null;
    setPressing(false);
    setLongVisualActive(false);
    setBarScale(0);
  }, [clearTimers]);

  // Si se pierde la disponibilidad (enlace caído, captura caducada, modal cerrado), cancelar inmediatamente.
  useEffect(() => {
    if (!isAvailable) {
      cancelGesture();
    }
  }, [isAvailable, cancelGesture]);

  useEffect(() => {
    const onVisibilityOrBlur = () => {
      if (document.visibilityState !== 'visible') {
        cancelGesture();
      }
    };
    const onWindowBlur = () => cancelGesture();
    window.addEventListener('blur', onWindowBlur);
    document.addEventListener('visibilitychange', onVisibilityOrBlur);
    return () => {
      clearTimers();
      if (rotateAnimTimeoutRef.current) {
        clearTimeout(rotateAnimTimeoutRef.current);
      }
      window.removeEventListener('blur', onWindowBlur);
      document.removeEventListener('visibilitychange', onVisibilityOrBlur);
    };
  }, [cancelGesture, clearTimers]);

  const animateRotate = (dir: 'left' | 'right') => {
    setPressedRotate(dir);
    if (rotateAnimTimeoutRef.current) {
      clearTimeout(rotateAnimTimeoutRef.current);
    }
    rotateAnimTimeoutRef.current = setTimeout(() => {
      setPressedRotate((prev) => (prev === dir ? null : prev));
    }, 140);
  };

  const handleRotateClick = (dir: 'left' | 'right') => {
    if (controlsDisabled) return;
    animateRotate(dir);
    onRotate(dir === 'left' ? -1 : 1);
  };

  const beginPress = (source: 'pointer' | 'keyboard', key?: string) => {
    if (controlsDisabled || pressStartRef.current !== null) return;
    clearTimers();
    const start = performance.now();
    gestureThreshold.current=threshold;pressStartRef.current = start;
    longFiredRef.current = false;
    activeInputSourceRef.current = source;
    activeKeyRef.current = key ?? null;
    setPressing(true);
    setLongVisualActive(false);
    setBarScale(0);

    const tick = () => {
      if (pressStartRef.current === null || longFiredRef.current) return;
      const ratio = Math.min(1, (performance.now() - start) / gestureThreshold.current);
      setBarScale(ratio);
      if (ratio < 1) {
        rafRef.current = requestAnimationFrame(tick);
      }
    };
    rafRef.current = requestAnimationFrame(tick);

    timerRef.current = setTimeout(() => {
      timerRef.current = null;
      if (pressStartRef.current === null) return;
      longFiredRef.current = true;
      if (rafRef.current) {
        cancelAnimationFrame(rafRef.current);
        rafRef.current = 0;
      }
      setBarScale(1);
      setLongVisualActive(true);
      // Qualify visually; navigation is emitted only on release.
    }, gestureThreshold.current);
  };

  const finishPress = () => {
    if (pressStartRef.current === null) return;
    const wasLong = longFiredRef.current || performance.now()-pressStartRef.current>=gestureThreshold.current;
    clearTimers();
    pressStartRef.current = null;
    longFiredRef.current = false;
    activeInputSourceRef.current = null;
    activeKeyRef.current = null;
    setPressing(false);
    setLongVisualActive(false);
    setBarScale(0);

    // Soltar despues del umbral largo configurado nunca envia confirmacion corta.
    if (isAvailable && !busy) {
      onPress(wasLong?'long':'short');
    }
  };

  // AFR2-20261103-27: el captor de teclado vive en un efecto estable y lee siempre la última versión de los gestos; todo evento se emite al soltar la tecla.
  const rootRef = useRef<HTMLDivElement | null>(null);
  const armedArrow = useRef<string | null>(null);
  const keysRef = useRef({ rotate: handleRotateClick, begin: beginPress, finish: finishPress, cancel: cancelGesture });
  keysRef.current = { rotate: handleRotateClick, begin: beginPress, finish: finishPress, cancel: cancelGesture };
  useEffect(() => {
    if (!keyboardCapture) return;
    const onKey = (event: KeyboardEvent) => {
      const dialog = rootRef.current?.closest('dialog');
      if (!dialog?.open || dialog.dataset.phase !== 'open' || rootRef.current?.closest('[inert]')) return;
      // Atajos del navegador (Ctrl/Alt/Meta) y teclas de función quedan fuera; Escape cierra el diálogo.
      if (event.ctrlKey || event.altKey || event.metaKey || event.key === 'Escape' || /^F\d+$/.test(event.key)) return;
      event.preventDefault(); event.stopPropagation();
      document.documentElement.dataset.input = 'keyboard';
      const down = event.type === 'keydown';
      if (event.key === ' ' || event.code === 'Space') {
        if (down) { if (!event.repeat) keysRef.current.begin('keyboard', ' '); }
        else if (activeKeyRef.current === ' ') keysRef.current.finish();
      } else if (event.key === 'ArrowLeft' || event.key === 'ArrowRight') {
        // Los giros se emiten al soltar la tecla, igual que el SW; solo si se vio su pulsación.
        if (down) armedArrow.current = event.key;
        else if (armedArrow.current === event.key) { armedArrow.current = null; keysRef.current.rotate(event.key === 'ArrowLeft' ? 'left' : 'right'); }
      }
    };
    window.addEventListener('keydown', onKey, true); window.addEventListener('keyup', onKey, true);
    return () => { window.removeEventListener('keydown', onKey, true); window.removeEventListener('keyup', onKey, true); };
  }, [keyboardCapture]);

  const swButtonDisabled = !isAvailable || (busy && !pressing && !longVisualActive);

  const swStateAttr = !isAvailable
    ? 'disabled'
    : longVisualActive
      ? 'long'
      : pressing
        ? 'pressed'
        : busy
          ? 'disabled'
          : 'idle';

  const reasonToShow =
    disabledReason ??
    (!isAvailable
      ? 'Controles deshabilitados: requiere enlace Web↔ESP y ESP↔ATmega listos, capacidad remota y captura fresca.'
      : busy && !longVisualActive
        ? 'Acción o consulta en curso; aguardando respuesta del controlador.'
        : null);

  // AFR2-20261117-06: botones circulares solo con icono. Giros juntos a la izquierda y SW en el extremo derecho.
  const buttonRow = (
    <div className="afr2-encoder-row" role="group" aria-label="Encoder: giros y pulsador SW">
      <div className="afr2-encoder-row__turns">
        <button
          type="button"
          disabled={controlsDisabled}
          onClick={() => handleRotateClick('left')}
          data-state={controlsDisabled ? 'disabled' : pressedRotate === 'left' ? 'pressed' : 'idle'}
          aria-label="Giro izquierdo del encoder (-1)"
          title="Giro izquierdo"
          className="afr2-encoder-round"
        >
          <EncoderGlyph kind="turn" scale={1.5} mirror />
        </button>
        <button
          type="button"
          disabled={controlsDisabled}
          onClick={() => handleRotateClick('right')}
          data-state={controlsDisabled ? 'disabled' : pressedRotate === 'right' ? 'pressed' : 'idle'}
          aria-label="Giro derecho del encoder (+1)"
          title="Giro derecho"
          className="afr2-encoder-round"
        >
          <EncoderGlyph kind="turn" scale={1.5} />
        </button>
      </div>
      <button
        type="button"
        disabled={swButtonDisabled}
        data-state={swStateAttr}
        title={`Pulsador SW: corto confirma, largo (${threshold} ms al soltar) vuelve`}
        aria-label={
          longVisualActive
            ? 'Largo preparado; soltar para ejecutar el retorno contextual.'
            : 'Pulsador SW del encoder: pulsación corta confirma, mantener y soltar vuelve al contexto anterior'
        }
        onPointerDown={(e) => {
          if (e.button !== 0 || controlsDisabled) return;
          try {
            e.currentTarget.setPointerCapture(e.pointerId);
          } catch {
            // Ignorar si el navegador no soporta captura en este dispositivo
          }
          beginPress('pointer');
        }}
        onPointerUp={(e) => {
          try {
            if (e.currentTarget.hasPointerCapture(e.pointerId)) {
              e.currentTarget.releasePointerCapture(e.pointerId);
            }
          } catch {
            // Ignorar
          }
          if (activeInputSourceRef.current === 'pointer') {
            finishPress();
          }
        }}
        onPointerCancel={(e) => {
          try {
            if (e.currentTarget.hasPointerCapture(e.pointerId)) {
              e.currentTarget.releasePointerCapture(e.pointerId);
            }
          } catch {
            // Ignorar
          }
          cancelGesture();
        }}
        onLostPointerCapture={() => {
          if (activeInputSourceRef.current === 'pointer') {
            cancelGesture();
          }
        }}
        onKeyDown={(e) => {
          if (e.key === ' ' || e.key === 'Enter') {
            e.preventDefault();
            if (e.repeat) return;
            beginPress('keyboard', e.key);
          }
        }}
        onKeyUp={(e) => {
          if (e.key === ' ' || e.key === 'Enter') {
            e.preventDefault();
            if (activeInputSourceRef.current === 'keyboard' && activeKeyRef.current === e.key) {
              finishPress();
            }
          }
        }}
        onBlur={() => {
          cancelGesture();
        }}
        className="afr2-encoder-round afr2-encoder-round--sw select-none"
      >
        <div ref={barRef} className="afr2-oled-sw-progress pointer-events-none" style={{ transform: 'scaleX(0)' }} aria-hidden="true" />
        <span className="afr2-encoder-round__icon">
          <EncoderGlyph kind={longVisualActive ? 'long' : 'short'} scale={2} />
        </span>
      </button>
    </div>
  );

  return (
    <div
      ref={rootRef}
      className={`afr2-oled-controls-card rounded-[var(--radius-md)] border border-[var(--border-default)] bg-[var(--surface-solid)] ${
        compact ? 'p-3.5' : 'p-4'
      } space-y-3.5`}
      aria-label="Controles del encoder OLED"
    >
      <div className="flex flex-wrap items-center justify-between gap-2">
        <div>
          <span className="text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
            {label}
          </span>
          <p className="text-xs text-[var(--text-muted)] mt-0.5">
            Los botones están debajo de la pantalla. Sin botón dedicado: largo al soltar vuelve por contexto, igual que la opción <code>Volver</code>.
          </p>
        </div>

        <div className="flex flex-wrap items-center gap-2">
          {lastEventLabel && (
            <span className="text-xs font-mono px-2.5 py-1 rounded-md bg-[var(--surface-subtle)] border border-[var(--border-default)] text-[var(--text)]">
              Última entrada: {lastEventLabel}
            </span>
          )}
          {onRefresh && (
            <button
              type="button"
              onClick={onRefresh}
              disabled={refreshDisabled || busy}
              aria-label="Actualizar captura de pantalla OLED"
              title="Sincronizar pantalla manualmente con el controlador"
              className="afr2-oled-sync-btn inline-flex min-h-11 items-center gap-2 rounded-[var(--radius-sm)] border border-[var(--border-strong)] bg-[var(--surface-subtle)] px-3.5 py-2 text-xs font-semibold text-[var(--text)] transition-colors hover:bg-[var(--selection)] disabled:cursor-not-allowed disabled:opacity-55"
            >
              <RefreshSyncIcon spinning={refreshLoading} />
              <span>{refreshLoading ? 'Sincronizando…' : 'Actualizar pantalla'}</span>
            </button>
          )}
        </div>
      </div>

      {/* AFR2-20261117-06: la fila de botones circulares va debajo de la pantalla (portal en buttonsHost);
          acá queda la leyenda de referencia con los mismos iconos. */}
      {buttonsHost ? createPortal(buttonRow, buttonsHost) : buttonRow}

      <dl className="afr2-encoder-legend" aria-label="Referencia de los controles del encoder">
        <div data-active={pressedRotate === 'left' || undefined}>
          <dt><EncoderGlyph kind="turn" scale={1} mirror /><span>Giro izquierdo</span>{keyboardCapture && <kbd className="afr2-key-hint">←</kbd>}</dt>
          <dd>Sube o va al anterior; en un valor, lo baja</dd>
        </div>
        <div data-active={pressedRotate === 'right' || undefined}>
          <dt><EncoderGlyph kind="turn" scale={1} /><span>Giro derecho</span>{keyboardCapture && <kbd className="afr2-key-hint">→</kbd>}</dt>
          <dd>Baja o va al siguiente; en un valor, lo sube</dd>
        </div>
        <div data-active={(pressing && !longVisualActive) || undefined}>
          <dt><EncoderGlyph kind="short" scale={1} /><span>Corto</span>{keyboardCapture && <kbd className="afr2-key-hint">Espacio</kbd>}</dt>
          <dd>Inicio: menú · resto: confirma</dd>
        </div>
        <div data-active={longVisualActive || undefined} data-tone="long">
          <dt><EncoderGlyph kind="long" scale={1} /><span>Largo</span></dt>
          <dd>Mantener {threshold} ms y soltar · Inicio: rápidas · resto: vuelve</dd>
        </div>
      </dl>

      <div className="flex flex-col gap-1.5 pt-0.5 empty:hidden">
        {reasonToShow && (
          <p
            role="status"
            className="text-xs rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)] px-3 py-2 text-[var(--text-muted)]"
          >
            <strong className="font-semibold text-[var(--text)]">Estado del control: </strong>
            {reasonToShow}
          </p>
        )}

        {statusMessage && (
          <p
            role="status"
            data-tone={statusTone}
            className="afr2-oled-remote-status text-xs rounded-[var(--radius-sm)] border px-3 py-2"
          >
            {statusMessage}
          </p>
        )}
      </div>
    </div>
  );
}
