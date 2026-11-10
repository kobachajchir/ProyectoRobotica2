import React, { useEffect, useMemo, useRef } from 'react';
import {
  DndContext,
  KeyboardSensor,
  PointerSensor,
  closestCenter,
  useSensor,
  useSensors,
  type DragEndEvent,
} from '@dnd-kit/core';
import {
  SortableContext,
  arrayMove,
  horizontalListSortingStrategy,
  sortableKeyboardCoordinates,
  useSortable,
} from '@dnd-kit/sortable';
import { CSS } from '@dnd-kit/utilities';
import { MotorBlock, type TimelineVisualBlock } from './MotorBlock';

export interface TimelineRowProps {
  trackId: string;
  title: string;
  subtitle?: string;
  details?: React.ReactNode;
  blocks: TimelineVisualBlock[];
  /** Duración total de la escala temporal única compartida entre todas las pistas (ms). */
  scaleTotalMs: number;
  /** Posición actual del cursor temporal compartido (ms). */
  cursorMs: number;
  selectedId: string | null;
  readOnly?: boolean;
  trackSelected?: boolean;
  onTrackSelect?: () => void;
  headerAction?: React.ReactNode;
  onSelect: (id: string | null) => void;
  onReorder?: (nextIds: string[]) => void;
  onLabelChange?: (blockId: string, newLabel: string) => void;
  scrollLeft?: number;
  onTimelineScroll?: (left: number) => void;
  onSeekCursor?: (atMs: number) => void;
}

interface SortableBlockCellProps {
  block: TimelineVisualBlock;
  styleWidth: React.CSSProperties;
  selected: boolean;
  active: boolean;
  progress: number;
  readOnly: boolean;
  canMoveLeft: boolean;
  canMoveRight: boolean;
  onSelect: () => void;
  onMoveLeft: () => void;
  onMoveRight: () => void;
  onLabelChange?: (blockId: string, newLabel: string) => void;
}

function SortableBlockCell({
  block,
  styleWidth,
  selected,
  active,
  progress,
  readOnly,
  canMoveLeft,
  canMoveRight,
  onSelect,
  onMoveLeft,
  onMoveRight,
  onLabelChange,
}: SortableBlockCellProps) {
  const {
    attributes,
    listeners,
    setNodeRef,
    transform,
    transition,
    isDragging,
  } = useSortable({
    id: block.id,
    disabled: readOnly || block.kind === 'finish',
  });

  const combinedStyle: React.CSSProperties = {
    ...styleWidth,
    transform: CSS.Transform.toString(transform),
    transition,
    zIndex: isDragging ? 20 : undefined,
    opacity: isDragging ? 0.88 : 1,
  };

  return (
    <div
      ref={setNodeRef}
      style={combinedStyle}
      className="shrink-0"
      {...attributes}
      {...(readOnly ? {} : listeners)}
    >
      <MotorBlock
        block={block}
        selected={selected}
        active={active}
        progress={progress}
        readOnly={readOnly}
        canMoveLeft={canMoveLeft}
        canMoveRight={canMoveRight}
        onClick={onSelect}
        onMoveLeft={onMoveLeft}
        onMoveRight={onMoveRight}
        onLabelChange={onLabelChange}
      />
    </div>
  );
}

/**
 * Fila de línea de tiempo adaptada de `reference/web/src/components/TimelineRow.tsx` (AFR2-20261023-21 / VIS-018).
 * - Soporta reordenamiento por arrastre (`@dnd-kit`), teclado y botones explícitos (`‹`/`›`).
 * - Comparte una única escala temporal (`scaleTotalMs`) y cursor (`cursorMs`).
 * - Cuando una pista de rueda termina antes que `scaleTotalMs`, muestra el tramo restante en consigna 0 mm/s.
 */
export function TimelineRow({
  trackId,
  title,
  subtitle,
  details,
  blocks,
  scaleTotalMs,
  cursorMs,
  selectedId,
  readOnly = false,
  trackSelected = false,
  onTrackSelect,
  headerAction,
  onSelect,
  onReorder,
  onLabelChange,
  scrollLeft = 0,
  onTimelineScroll,
  onSeekCursor,
}: TimelineRowProps) {
  const scroller = useRef<HTMLDivElement>(null);
  useEffect(() => { if (scroller.current && Math.abs(scroller.current.scrollLeft-scrollLeft) > 1) scroller.current.scrollLeft = scrollLeft; }, [scrollLeft]);
  const sensors = useSensors(
    useSensor(PointerSensor, { activationConstraint: { distance: 6 } }),
    useSensor(KeyboardSensor, { coordinateGetter: sortableKeyboardCoordinates })
  );

  // Calcular inicio/fin temporal de cada bloque respetando que 'finish' es terminal
  const timedBlocks = useMemo(() => {
    let elapsed = 0;
    let terminated = false;
    return blocks.map((b) => {
      if (terminated || b.kind === 'finish') {
        terminated = true;
        return { block: b, startMs: elapsed, endMs: elapsed, effectiveDurationMs: 0 };
      }
      const dur = Math.max(0, b.durationMs);
      const startMs = elapsed;
      const endMs = elapsed + dur;
      elapsed = endMs;
      return { block: b, startMs, endMs, effectiveDurationMs: dur };
    });
  }, [blocks]);

  const trackDurationMs = useMemo(
    () => timedBlocks.reduce((acc, item) => acc + item.effectiveDurationMs, 0),
    [timedBlocks]
  );

  const safeScaleMs = Math.max(100, scaleTotalMs, trackDurationMs);
  const idleTailMs = Math.max(0, safeScaleMs - trackDurationMs);
  const cursorRatio = Math.max(0, Math.min(1, cursorMs / safeScaleMs));
  const timelineWidth = Math.max(640, Math.min(12000, safeScaleMs*.1));

  const handleDragEnd = (event: DragEndEvent) => {
    if (readOnly || !onReorder) return;
    const { active, over } = event;
    if (!over || active.id === over.id) return;
    const oldIndex = blocks.findIndex((b) => b.id === String(active.id));
    const newIndex = blocks.findIndex((b) => b.id === String(over.id));
    if (oldIndex < 0 || newIndex < 0) return;
    if (blocks[oldIndex].kind === 'finish') return;
    const next = arrayMove(blocks, oldIndex, blocks[newIndex].kind === 'finish' ? Math.max(0, newIndex - 1) : newIndex);
    onReorder(next.map((b) => b.id));
  };

  const moveIndex = (fromIdx: number, toIdx: number) => {
    if (readOnly || !onReorder) return;
    if (toIdx < 0 || toIdx >= blocks.length) return;
    if (blocks[fromIdx].kind === 'finish' || blocks[toIdx].kind === 'finish') return;
    const next = arrayMove(blocks, fromIdx, toIdx);
    onReorder(next.map((b) => b.id));
  };

  return (
    <div
      className="afr2-timeline-row rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-solid)] p-3 space-y-2"
      data-track={trackId}
      data-readonly={readOnly ? 'true' : 'false'}
      data-selected={trackSelected}
      role="group" aria-label={title} tabIndex={onTrackSelect ? 0 : undefined}
      onClickCapture={onTrackSelect}
      onKeyDown={event => { if (event.target === event.currentTarget && (event.key === 'Enter' || event.key === ' ')) { event.preventDefault(); onTrackSelect?.(); } }}
    >
      <div className="flex flex-wrap items-center justify-between gap-2">
        <div className="flex items-center gap-2">
          <span className="text-xs font-semibold text-[var(--text)]">{title}</span>
          {subtitle && (
            <span className="text-[11px] font-mono text-[var(--text-muted)]">{subtitle}</span>
          )}
          {headerAction && <span className="afr2-track-header-action">{headerAction}</span>}
          {readOnly && (
            <span className="rounded-xs border border-[var(--border-default)] bg-[var(--surface-subtle)] px-1.5 py-0.5 text-[10px] font-mono uppercase text-[var(--text-muted)]">
              Solo lectura
            </span>
          )}
        </div>
        <span className="text-xs font-mono text-[var(--text-muted)]">
          {(trackDurationMs / 1000).toFixed(2)} s / {(safeScaleMs / 1000).toFixed(2)} s · {blocks.length} blq.
        </span>
      </div>

      <div
        ref={scroller}
        onScroll={event => onTimelineScroll?.(event.currentTarget.scrollLeft)}
        className="afr2-timeline-track relative min-h-16 rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)] overflow-x-auto"
        onClick={(e) => {
          if (e.target === e.currentTarget) {
            onSelect(null);
          }
        }}
      >
        <div className="afr2-timeline-strip" style={{ width: timelineWidth+108 }}>
        {/* Cursor temporal sincronizado */}
        <div
          className="pointer-events-none absolute inset-y-0 z-20 w-0.5 bg-[var(--action)]"
          style={{ left: cursorRatio*timelineWidth }}
          aria-hidden="true"
        />
        <div className="afr2-timeline-ruler" style={{ width: timelineWidth }} aria-hidden="true">{[0,.25,.5,.75,1].map(ratio => <span key={ratio} style={{ left: `${ratio*100}%` }}>{(ratio*safeScaleMs/1000).toFixed(1)}s</span>)}</div>

        {blocks.length === 0 ? (
          <div className="flex h-12 items-center justify-center text-xs text-[var(--text-muted)]">
            Pista sin bloques (consigna 0 mm/s).
          </div>
        ) : (
          <DndContext
            sensors={sensors}
            collisionDetection={closestCenter}
            onDragEnd={handleDragEnd}
          >
            <SortableContext
              items={blocks.map((b) => b.id)}
              strategy={horizontalListSortingStrategy}
            >
              <div className="flex items-stretch">
                {timedBlocks.map(({ block, startMs, endMs, effectiveDurationMs }, idx) => {
                  const isFinish = block.kind === 'finish';
                  const styleWidth: React.CSSProperties = isFinish
                    ? { width: 108 }
                    : { width: effectiveDurationMs / safeScaleMs * timelineWidth, minWidth: 0, overflow: 'hidden', borderRight: '1px solid var(--border-default)' };

                  const isActive =
                    !isFinish &&
                    effectiveDurationMs > 0 &&
                    cursorMs > startMs &&
                    cursorMs <= endMs;
                  const progress = isActive
                    ? (cursorMs - startMs) / effectiveDurationMs
                    : 0;

                  return (
                    <SortableBlockCell
                      key={block.id}
                      block={block}
                      styleWidth={styleWidth}
                      selected={selectedId === block.id}
                      active={isActive}
                      progress={progress}
                      readOnly={readOnly}
                      canMoveLeft={!isFinish && idx > 0}
                      canMoveRight={!isFinish && idx < blocks.length - 1 && blocks[idx + 1]?.kind !== 'finish'}
                      onSelect={() => { onSelect(block.id); onSeekCursor?.(Math.min(endMs,startMs+10)); }}
                      onMoveLeft={() => moveIndex(idx, idx - 1)}
                      onMoveRight={() => moveIndex(idx, idx + 1)}
                      onLabelChange={onLabelChange}
                    />
                  );
                })}

                {idleTailMs >= 10 && (
                  <div
                    style={{
                      width: idleTailMs / safeScaleMs * timelineWidth,
                      minWidth: 0, overflow: 'hidden',
                    }}
                    className="flex items-center justify-center rounded-[var(--radius-sm)] border border-dashed border-[var(--border-default)] bg-[var(--surface-solid)]/55 px-2 py-2 text-[11px] font-mono text-[var(--text-muted)] shrink-0"
                    title="Esta pista finaliza antes que el resto del programa; la rueda permanece con consigna 0 mm/s."
                  >
                    0 mm/s (+{idleTailMs} ms)
                  </div>
                )}
              </div>
            </SortableContext>
          </DndContext>
        )}
        </div>
      </div>
      {details}
    </div>
  );
}
