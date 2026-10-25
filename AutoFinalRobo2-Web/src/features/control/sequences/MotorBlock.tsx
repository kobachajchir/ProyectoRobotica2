import { useEffect, useRef, useState } from 'react';
import { ChassisSpinGraphic } from '../ChassisSpinGraphic';
import type { MovementBlockKind, WheelBlockKind } from './sequenceTypes';

export interface TimelineVisualBlock {
  id: string;
  kind: MovementBlockKind | WheelBlockKind;
  label: string;
  durationMs: number;
  summary: string;
  /** Para bloques de giro ('pivot'): sentido del giro sobre el eje. */
  pivotDirection?: 'ccw' | 'cw';
  /** Para rampas: proporción inicial/final (-1..1) para dibujar el trapecio. */
  rampFromRatio?: number;
  rampToRatio?: number;
  /** Para curva ('curve'): lado izquierdo o derecho. */
  arcSide?: 'left' | 'right';
}

export interface MotorBlockProps {
  block: TimelineVisualBlock;
  selected?: boolean;
  active?: boolean;
  progress?: number;
  readOnly?: boolean;
  canMoveLeft?: boolean;
  canMoveRight?: boolean;
  onClick?: () => void;
  onLabelChange?: (blockId: string, newLabel: string) => void;
  onMoveLeft?: () => void;
  onMoveRight?: () => void;
}

function StopTerminalGlyph() {
  return (
    <span
      className="inline-flex h-5 w-6 flex-col-reverse items-center justify-start gap-0.5 shrink-0"
      aria-hidden="true"
    >
      <i className="block h-1 w-5 rounded-xs border border-current/75 bg-current/35" />
      <i className="block h-1 w-3.5 rounded-xs border border-current/60 bg-current/25" />
      <i className="block h-1 w-2 rounded-xs border border-current/45 bg-current/15" />
    </span>
  );
}

/**
 * Bloque visual de línea de tiempo adaptado de `reference/web/src/components/MotorBlock.tsx` (AFR2-20261023-21 / VIS-018).
 * - Superficie sólida, acento carmesí/rubí en selección y reproducción, sin dependencias de PIN o balance.
 * - Incluye símbolo `ChassisSpinGraphic` para bloques de giro sobre el eje (`pivot`), trapecio SVG para `ramp`
 *   y glifo terminal para `finish`.
 */
export function MotorBlock({
  block,
  selected = false,
  active = false,
  progress = 0,
  readOnly = false,
  canMoveLeft = false,
  canMoveRight = false,
  onClick,
  onLabelChange,
  onMoveLeft,
  onMoveRight,
}: MotorBlockProps) {
  const [isEditing, setIsEditing] = useState(false);
  const [editValue, setEditValue] = useState(block.label);
  const inputRef = useRef<HTMLInputElement>(null);

  useEffect(() => {
    setEditValue(block.label);
  }, [block.label]);

  useEffect(() => {
    if (isEditing && inputRef.current) {
      inputRef.current.focus();
      inputRef.current.select();
    }
  }, [isEditing]);

  const commitLabel = () => {
    const trimmed = editValue.trim();
    if (trimmed && trimmed !== block.label && onLabelChange) {
      onLabelChange(block.id, trimmed);
    } else {
      setEditValue(block.label);
    }
    setIsEditing(false);
  };

  const isFinish = block.kind === 'finish';
  const isWait = block.kind === 'wait';
  const isPivot = block.kind === 'pivot';
  const isRamp = block.kind === 'ramp';
  const isCurve = block.kind === 'curve';

  return (
    <div
      role="button"
      tabIndex={0}
      aria-pressed={selected}
      aria-label={`${block.label} (${block.summary})`}
      data-kind={block.kind}
      data-selected={selected ? 'true' : 'false'}
      data-active={active ? 'true' : 'false'}
      onClick={() => onClick?.()}
      onDoubleClick={(e) => {
        if (readOnly || !onLabelChange) return;
        e.stopPropagation();
        setIsEditing(true);
      }}
      onKeyDown={(e) => {
        if (isEditing) return;
        if (e.key === 'Enter' || e.key === ' ') {
          e.preventDefault();
          onClick?.();
        } else if (!readOnly && e.altKey && e.key === 'ArrowLeft' && canMoveLeft) {
          e.preventDefault();
          onMoveLeft?.();
        } else if (!readOnly && e.altKey && e.key === 'ArrowRight' && canMoveRight) {
          e.preventDefault();
          onMoveRight?.();
        }
      }}
      className="afr2-seq-block relative select-none overflow-hidden rounded-[var(--radius-sm)] px-2.5 py-2 text-left transition-colors"
    >
      {/* Trapecio de rampa de fondo */}
      {isRamp && (
        <svg
          className="pointer-events-none absolute inset-0 h-full w-full opacity-20"
          viewBox="0 0 100 100"
          preserveAspectRatio="none"
          aria-hidden="true"
        >
          {(() => {
            const r0 = Math.max(0.08, Math.min(0.95, Math.abs(block.rampFromRatio ?? 0.2)));
            const r1 = Math.max(0.08, Math.min(0.95, Math.abs(block.rampToRatio ?? 0.85)));
            const y0 = 100 - r0 * 82;
            const y1 = 100 - r1 * 82;
            return (
              <polygon
                points={`0,100 0,${y0} 100,${y1} 100,100`}
                fill="currentColor"
              />
            );
          })()}
        </svg>
      )}

      {/* Barra de progreso en reproducción */}
      {active && !isFinish && (
        <div
          className="pointer-events-none absolute inset-y-0 left-0 bg-[var(--action)]/20 transition-none"
          style={{ width: `${Math.max(0, Math.min(1, progress)) * 100}%` }}
          aria-hidden="true"
        />
      )}

      <div className="relative z-10 flex items-start justify-between gap-1.5">
        <div className="flex items-center gap-1.5 min-w-0 flex-1">
          {isPivot && (
            <ChassisSpinGraphic
              direction={block.pivotDirection ?? 'ccw'}
              active={selected || active}
              size="sm"
              className="text-[var(--action)]"
            />
          )}
          {isFinish && <StopTerminalGlyph />}
          {isCurve && (
            <span className="inline-flex h-5 w-5 items-center justify-center rounded-xs bg-[var(--surface-subtle)] text-xs font-mono font-bold text-[var(--action)] shrink-0" aria-hidden="true">
              {block.arcSide === 'right' ? '↷' : '↶'}
            </span>
          )}
          {isWait && (
            <span className="inline-flex h-5 w-5 items-center justify-center rounded-xs bg-[var(--surface-subtle)] text-[11px] font-mono text-[var(--text-muted)] shrink-0" aria-hidden="true">
              ⏸
            </span>
          )}

          <div className="min-w-0 flex-1">
            {isEditing ? (
              <input
                ref={inputRef}
                type="text"
                value={editValue}
                onChange={(e) => setEditValue(e.target.value)}
                onBlur={commitLabel}
                onClick={(e) => e.stopPropagation()}
                onKeyDown={(e) => {
                  e.stopPropagation();
                  if (e.key === 'Enter') commitLabel();
                  if (e.key === 'Escape') {
                    setEditValue(block.label);
                    setIsEditing(false);
                  }
                }}
                className="w-full rounded-xs border border-[var(--focus)] bg-[var(--surface-solid)] px-1.5 py-0.5 text-xs font-semibold text-[var(--text)] outline-none"
              />
            ) : (
              <span className="block truncate text-xs font-semibold text-[var(--text)]">
                {block.label}
              </span>
            )}
            <span className="block truncate text-[11px] font-mono text-[var(--text-muted)]">
              {block.summary}
            </span>
          </div>
        </div>

        {!readOnly && selected && (canMoveLeft || canMoveRight) && (
          <div
            className="flex items-center gap-0.5 shrink-0"
            onClick={(e) => e.stopPropagation()}
          >
            <button
              type="button"
              disabled={!canMoveLeft}
              onClick={onMoveLeft}
              aria-label={`Mover ${block.label} a la izquierda`}
              title="Mover bloque a la izquierda (Alt+←)"
              className="afr2-seq-reorder-btn"
            >
              ‹
            </button>
            <button
              type="button"
              disabled={!canMoveRight}
              onClick={onMoveRight}
              aria-label={`Mover ${block.label} a la derecha`}
              title="Mover bloque a la derecha (Alt+→)"
              className="afr2-seq-reorder-btn"
            >
              ›
            </button>
          </div>
        )}
      </div>
    </div>
  );
}
