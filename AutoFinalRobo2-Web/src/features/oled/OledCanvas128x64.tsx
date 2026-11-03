import { useEffect, useRef } from 'react';
import {
  AFR2_OLED_HEIGHT,
  AFR2_OLED_WIDTH,
  type Afr2OledBuffer,
} from './oledRasterizer';
import { getScreenMetadata } from './screenCatalog';

export interface OledCanvas128x64Props {
  oledBuffer: Afr2OledBuffer;
  screenCode: string;
  sourceMode: 'local-firmware-preview' | 'local-ui-simulation' | 'remote-snapshot';
  showGrid?: boolean;
  compact?: boolean;
  scale?: number;
  /** Replaces the source badge (e.g. a placeholder while waiting for the first capture). */
  badge?: string;
  /** Replaces the screen name when there is no catalogued screen. */
  title?: string;
}

/**
 * Lienzo físico SSD1306 128×64 píxeles (`VIS-006`).
 * Renderiza el framebuffer de 1024 bytes exactamente igual que las 8 páginas verticales del ATmega2560.
 */
export function OledCanvas128x64({
  oledBuffer,
  screenCode,
  sourceMode,
  showGrid = true,
  compact = false,
  scale = 4,
  badge,
  title,
}: OledCanvas128x64Props) {
  const canvasRef = useRef<HTMLCanvasElement | null>(null);
  const meta = getScreenMetadata(screenCode) ?? {
    code: screenCode, name: title ?? 'Pantalla del controlador', renderer: 'Framebuffer del controlador', actions: [],
  };

  useEffect(() => {
    const canvas = canvasRef.current;
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    if (!ctx) return;

    const widthPx = AFR2_OLED_WIDTH * scale;
    const heightPx = AFR2_OLED_HEIGHT * scale;

    ctx.fillStyle = '#000000';
    ctx.fillRect(0, 0, widthPx, heightPx);

    for (let y = 0; y < AFR2_OLED_HEIGHT; y++) {
      for (let x = 0; x < AFR2_OLED_WIDTH; x++) {
        const on = oledBuffer.getPixel(x, y);
        if (on) {
          ctx.fillStyle = '#ffffff';
          ctx.fillRect(
            x * scale,
            y * scale,
            showGrid && scale >= 3 ? scale - 0.5 : scale,
            showGrid && scale >= 3 ? scale - 0.5 : scale
          );
        } else if (showGrid && scale >= 4) {
          ctx.fillStyle = 'rgba(255, 255, 255, 0.05)';
          ctx.fillRect(x * scale, y * scale, 1, 1);
        }
      }
    }
  }, [oledBuffer, screenCode, showGrid, scale]);

  return (
    <div
      className="flex flex-col items-center rounded-2xl border p-4"
      style={{
        backgroundColor: 'var(--oled-bezel)',
        borderColor: 'var(--border-strong)',
        boxShadow: 'inset 0 2px 12px rgba(0, 0, 0, 0.75)',
      }}
    >
      <div className="mb-3 flex w-full flex-wrap items-center justify-between gap-2 text-xs">
        <div className="flex items-center gap-2">
          <span
            className="afr2-tabular rounded px-2 py-0.5 font-semibold"
            style={{
              backgroundColor: 'rgba(255, 255, 255, 0.14)',
              color: '#ffffff',
              border: '1px solid rgba(255, 255, 255, 0.32)',
            }}
          >
            {compact ? meta.name : `${meta.code} · ${meta.name}`}
          </span>
          <span className="afr2-tabular text-[11px]" style={{ color: '#94a3b8' }}>
            {compact ? '128 × 64' : 'SSD1306 128×64 I2C'}
          </span>
        </div>
        <span
          className="afr2-tabular rounded px-2 py-0.5 text-[11px] font-semibold"
          style={{
            backgroundColor: badge ? 'rgba(255, 255, 255, 0.12)' :
              sourceMode === 'remote-snapshot'
                ? 'rgba(74, 222, 128, 0.16)'
                : 'rgba(251, 191, 36, 0.16)',
            color: badge ? '#cbd5e1' : sourceMode === 'remote-snapshot' ? '#4ade80' : '#fbbf24',
          }}
        >
          {badge ?? (sourceMode === 'remote-snapshot'
            ? compact ? 'Captura recibida' : 'SNAPSHOT REMOTO (wire)'
            : sourceMode === 'local-ui-simulation' ? 'SIMULACION UI · DATOS FICTICIOS' : 'VISTA LOCAL · BIBLIOTECA ATmega')}
        </span>
      </div>

      <div className="w-full overflow-x-auto flex justify-center">
        <canvas
          ref={canvasRef}
          width={AFR2_OLED_WIDTH * scale}
          height={AFR2_OLED_HEIGHT * scale}
          aria-label={`Pantalla OLED 128 por 64 píxeles mostrando ${meta.name} (${meta.code})`}
          className="max-w-full rounded-lg border"
          style={{
            borderColor: 'rgba(255, 255, 255, 0.24)',
            imageRendering: 'pixelated',
          }}
        />
      </div>

      {!compact && <div className="mt-3 flex w-full flex-wrap items-center justify-between gap-2 text-[11px]" style={{ color: '#94a3b8' }}>
        <span className="afr2-tabular">Renderer: {meta.renderer}</span>
        <span className="afr2-tabular">Acciones: {meta.actions.join(', ')}</span>
      </div>}
    </div>
  );
}
