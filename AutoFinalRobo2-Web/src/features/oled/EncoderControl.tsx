import { useCallback, useEffect, useRef, useState } from 'react';

export const AFR2_LONG_PRESS_MS = 800;
export const AFR2_ENCODER_LONG_MS = 800;

export interface EncoderControlProps {
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

const ICON_ENCODER_BITS = new Uint8Array([
  0xf8, 0x03, 0xfc, 0x07, 0x0e, 0x0e, 0xf7, 0x1d, 0xfb, 0x1b, 0xfb, 0x1b, 0xfb,
  0x1b, 0xfb, 0x1b, 0xfb, 0x1b, 0xf7, 0x1d, 0x0e, 0x0e, 0xfc, 0x07, 0xf8, 0x03,
]);

function MonoBitmapEncoderIcon({
  scale = 2,
  color,
}: {
  scale?: number;
  color: string;
}) {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const width = 13;
  const height = 13;

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const bytesPerRow = Math.ceil(width / 8);
    canvas.width = width * scale;
    canvas.height = height * scale;
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.fillStyle = color;

    for (let y = 0; y < height; y += 1) {
      for (let x = 0; x < width; x += 1) {
        const byte = ICON_ENCODER_BITS[y * bytesPerRow + (x >> 3)] ?? 0;
        const bit = 1 << (x & 7);
        if (byte & bit) {
          ctx.fillRect(x * scale, y * scale, scale, scale);
        }
      }
    }
  }, [color, scale]);

  return (
    <canvas
      ref={canvasRef}
      width={width * scale}
      height={height * scale}
      className="block shrink-0"
      aria-hidden="true"
    />
  );
}

function TurnArrowIcon({ direction }: { direction: 'left' | 'right' }) {
  if (direction === 'left') {
    return (
      <svg
        xmlns="http://www.w3.org/2000/svg"
        viewBox="0 0 24 24"
        fill="none"
        stroke="currentColor"
        strokeWidth="2"
        strokeLinecap="round"
        strokeLinejoin="round"
        className="h-4 w-4 shrink-0"
        aria-hidden="true"
      >
        <path d="M9 7H5v4" />
        <path d="M5 11a7 7 0 1 0 2-4.9L5 7" />
      </svg>
    );
  }

  return (
    <svg
      xmlns="http://www.w3.org/2000/svg"
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      strokeWidth="2"
      strokeLinecap="round"
      strokeLinejoin="round"
      className="h-4 w-4 shrink-0"
      aria-hidden="true"
    >
      <path d="M15 7h4v4" />
      <path d="M19 11a7 7 0 1 1-2-4.9L19 7" />
    </svg>
  );
}

function RefreshSyncIcon({ spinning = false }: { spinning?: boolean }) {
  return (
    <svg
      xmlns="http://www.w3.org/2000/svg"
      viewBox="0 0 24 24"
      fill="none"
      stroke="currentColor"
      strokeWidth="2"
      strokeLinecap="round"
      strokeLinejoin="round"
      className={`h-4 w-4 shrink-0 ${spinning ? 'animate-spin' : ''}`}
      aria-hidden="true"
    >
      <path d="M21 12a9 9 0 0 0-9-9 9.75 9.75 0 0 0-6.74 2.74L3 8" />
      <path d="M3 3v5h5" />
      <path d="M3 12a9 9 0 0 0 9 9 9.75 9.75 0 0 0 6.74-2.74L21 16" />
      <path d="M16 16h5v5" />
    </svg>
  );
}

/**
 * Controles físicos del encoder único KY-040 adaptados de `ScreenStreamWorkspace.tsx` (AFR2-20261023-21 / VIS-018).
 * - Muestra siempre Giro izquierdo, SW y Giro derecho más el botón manual de Actualizar, incluso deshabilitados.
 * - Único botón SW (sin botón USER, Menú ni Atrás).
 * - Umbral propio de 800 ms: corto (< 800 ms) confirma al soltar; largo (>= 800 ms) envía menú una vez y
 *   conserva su estado visual de atención hasta soltar, aunque la solicitud ya esté en curso.
 * - Pérdida de disponibilidad, cancelación de puntero, pérdida de foco/captura o cierre del modal cancelan el gesto de inmediato.
 */
export function EncoderControl({
  onRotate,
  onPress,
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
  const isAvailable = available && !disabled;
  const controlsDisabled = !isAvailable || busy;

  const [pressing, setPressing] = useState(false);
  const [longVisualActive, setLongVisualActive] = useState(false);
  const [pressedRotate, setPressedRotate] = useState<'left' | 'right' | null>(null);

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
    pressStartRef.current = start;
    longFiredRef.current = false;
    activeInputSourceRef.current = source;
    activeKeyRef.current = key ?? null;
    setPressing(true);
    setLongVisualActive(false);
    setBarScale(0);

    const tick = () => {
      if (pressStartRef.current === null || longFiredRef.current) return;
      const ratio = Math.min(1, (performance.now() - start) / AFR2_LONG_PRESS_MS);
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
      onPress('long');
    }, AFR2_LONG_PRESS_MS);
  };

  const finishPress = () => {
    if (pressStartRef.current === null) return;
    const wasLong = longFiredRef.current;
    clearTimers();
    pressStartRef.current = null;
    longFiredRef.current = false;
    activeInputSourceRef.current = null;
    activeKeyRef.current = null;
    setPressing(false);
    setLongVisualActive(false);
    setBarScale(0);

    // Soltar después de pulsación larga (>= 800 ms) nunca envía confirmación corta.
    if (!wasLong && isAvailable && !busy) {
      onPress('short');
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
    ? `Menú enviado (≥ ${AFR2_LONG_PRESS_MS} ms) · soltá SW para finalizar el gesto`
    : pressing
      ? `Pulsando SW… soltá antes de ${AFR2_LONG_PRESS_MS} ms para confirmar o mantené para Menú`
      : 'Corto (< 800 ms): confirmar · Largo (≥ 800 ms): abrir Menú principal';

  const reasonToShow =
    disabledReason ??
    (!isAvailable
      ? 'Controles deshabilitados: requiere enlace Web↔ESP y ESP↔ATmega listos, capacidad remota y captura fresca.'
      : busy && !longVisualActive
        ? 'Acción o consulta en curso; aguardando respuesta del controlador.'
        : null);

  return (
    <div
      className={`afr2-oled-controls-card rounded-[var(--radius-md)] border border-[var(--border)] bg-[var(--surface-solid)] ${
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
            Sin botón Menú ni Atrás dedicado: retroceso solo mediante la opción <code>Volver</code> en pantalla.
          </p>
        </div>

        <div className="flex flex-wrap items-center gap-2">
          {lastEventLabel && (
            <span className="text-xs font-mono px-2.5 py-1 rounded-md bg-[var(--surface-subtle)] border border-[var(--border)] text-[var(--text)]">
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
            <TurnArrowIcon direction="left" />
            <span>Giro izquierdo</span>
          </button>
        </div>

        <div className="flex flex-col gap-1">
          <span className="text-[11px] font-semibold uppercase tracking-wider text-[var(--text-muted)]">
            Pulsador SW ({AFR2_LONG_PRESS_MS} ms)
          </span>
          <button
            type="button"
            disabled={swButtonDisabled}
            data-state={swStateAttr}
            aria-label={
              longVisualActive
                ? 'Pulsación larga activa (Menú principal enviado). Soltar para terminar gesto.'
                : 'Pulsador SW del encoder: pulsación corta confirma, mantener 800 ms abre Menú principal'
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
              <MonoBitmapEncoderIcon
                scale={2}
                color={
                  swStateAttr === 'long'
                    ? '#0f172a'
                    : swStateAttr === 'pressed'
                      ? 'var(--on-action)'
                      : 'currentColor'
                }
              />
              <span>
                {longVisualActive
                  ? 'SW · Menú (soltar)'
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
            <TurnArrowIcon direction="right" />
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
          {swStatusCopy}
        </p>

        {reasonToShow && (
          <p
            role="status"
            className="text-xs rounded-[var(--radius-sm)] border border-[var(--border)] bg-[var(--surface-subtle)] px-3 py-2 text-[var(--text-muted)]"
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
