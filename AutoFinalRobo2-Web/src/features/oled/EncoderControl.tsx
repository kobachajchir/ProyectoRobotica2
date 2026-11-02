import { EncoderGlyph } from './EncoderGlyph';
import { UiIcon } from '../../components/ui/UiIcon';
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { useCallback, useEffect, useRef, useState } from 'react';

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

  const swStatusCopy = longVisualActive
    ? `Largo preparado (≥ ${threshold} ms) · soltá SW para volver`
    : pressing
      ? `Pulsando SW… soltá antes de ${threshold} ms para confirmar o mantené para volver`
      : 'Inicio: corto abre menú; largo abre pantallas rápidas al soltar. En el resto: corto confirma, largo vuelve.';

  const reasonToShow =
    disabledReason ??
    (!isAvailable
      ? 'Controles deshabilitados: requiere enlace Web↔ESP y ESP↔ATmega listos, capacidad remota y captura fresca.'
      : busy && !longVisualActive
        ? 'Acción o consulta en curso; aguardando respuesta del controlador.'
        : null);

  return (
    <div
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
            Sin botón dedicado: largo al soltar vuelve por contexto, igual que la opción <code>Volver</code> en pantalla.
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

      <div className="grid grid-cols-1 sm:grid-cols-3 gap-2.5" role="group" aria-label="Acciones de giro y pulsación SW">
        <div className="flex flex-col gap-1">
          <span className="text-[11px] font-semibold uppercase tracking-wider text-[var(--text-muted)]">
            Giro izquierdo
          </span>
          <button
            type="button"
            disabled={controlsDisabled}
            onClick={() => handleRotateClick('left')}
            data-state={controlsDisabled ? 'disabled' : pressedRotate === 'left' ? 'pressed' : 'idle'}
            aria-label="Giro izquierdo del encoder (-1)"
            className="afr2-oled-live-btn min-h-12 w-full rounded-[var(--radius-sm)] px-3.5 py-2.5 text-sm font-semibold flex items-center justify-center gap-2.5"
          >
            <EncoderGlyph kind="turn" scale={1} mirror />
            <span>Giro izquierdo</span>
          </button>
        </div>

        <div className="flex flex-col gap-1">
          <span className="text-[11px] font-semibold uppercase tracking-wider text-[var(--text-muted)]">
            Pulsador SW ({threshold} ms)
          </span>
          <button
            type="button"
            disabled={swButtonDisabled}
            data-state={swStateAttr}
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
            className="afr2-oled-live-btn afr2-oled-sw-btn relative overflow-hidden min-h-12 w-full rounded-[var(--radius-sm)] px-3.5 py-2.5 text-sm font-semibold flex items-center justify-center gap-2.5 select-none"
          >
            <div
              ref={barRef}
              className="afr2-oled-sw-progress pointer-events-none"
              style={{ transform: 'scaleX(0)' }}
              aria-hidden="true"
            />
            <span className="relative z-10 flex items-center gap-2">
              <EncoderGlyph
                kind={longVisualActive?'long':'short'}
                scale={2}
                color={
                  swStateAttr === 'long'
                    ? 'var(--attention-ink)'
                    : swStateAttr === 'pressed'
                      ? 'var(--text-on-action)'
                      : 'currentColor'
                }
              />
              <span>
                {longVisualActive
                  ? 'SW largo (soltar)'
                  : pressing
                    ? 'SW · Manteniendo…'
                    : 'SW (Corto / Largo)'}
              </span>
            </span>
          </button>
        </div>

        <div className="flex flex-col gap-1">
          <span className="text-[11px] font-semibold uppercase tracking-wider text-[var(--text-muted)]">
            Giro derecho
          </span>
          <button
            type="button"
            disabled={controlsDisabled}
            onClick={() => handleRotateClick('right')}
            data-state={controlsDisabled ? 'disabled' : pressedRotate === 'right' ? 'pressed' : 'idle'}
            aria-label="Giro derecho del encoder (+1)"
            className="afr2-oled-live-btn min-h-12 w-full rounded-[var(--radius-sm)] px-3.5 py-2.5 text-sm font-semibold flex items-center justify-center gap-2.5"
          >
            <EncoderGlyph kind="turn" scale={1} />
            <span>Giro derecho</span>
          </button>
        </div>
      </div>

      <div className="flex flex-col gap-1.5 pt-0.5">
        <p
          className={`text-xs font-medium ${
            longVisualActive
              ? 'text-[var(--warning)] font-semibold'
              : pressing
                ? 'text-[var(--action)]'
                : 'text-[var(--text-muted)]'
          }`}
        >
          <span className="inline-flex items-center gap-2"><EncoderGlyph kind={longVisualActive?'long':'short'} scale={1}/>{swStatusCopy}</span>
        </p>

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
