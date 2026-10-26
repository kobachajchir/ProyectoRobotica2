import { useEffect, useId, useRef, useState } from 'react';
import type { MapSceneProps } from './controlMapTypes';

export function TopMap({ pose, trail, zoom, center, fresh, waypoints = [], onSelectPoint, pickingDisabled }: MapSceneProps) {
  const uid = useId().replace(/:/g, '');
  const canvas = useRef<SVGSVGElement>(null);
  const pressed = useRef<{ id: number; x: number; y: number } | null>(null);
  const [size, setSize] = useState({ width: 760, height: 440 });
  useEffect(() => {
    const element = canvas.current;
    if (!element) return;
    const resize = () => { const rect = element.getBoundingClientRect(); setSize({ width: Math.max(180, rect.width), height: Math.max(220, rect.height) }); };
    const observer = new ResizeObserver(resize); observer.observe(element); resize();
    return () => observer.disconnect();
  }, []);
  const cx = size.width / 2, cy = size.height / 2;
  const unit = 96 * zoom; // pixels/metre; this is a view scale, not odometry.
  const point = (x: number, y: number) => ({ x: cx - (y - center.yMm) / 1000 * unit, y: cy - (x - center.xMm) / 1000 * unit });
  const robot = pose ? point(pose.xMm, pose.yMm) : { x: cx, y: cy };
  const tile = unit / 2;
  const scaleLength = size.width < 440 ? unit / 2 : unit;
  const scaleEnd = size.width - 24, scaleY = size.height - 24;
  const lines = trail.map((p, index) => { const v = point(p.xMm, p.yMm); return `${index === 0 || p.segmentStart ? 'M' : 'L'}${v.x},${v.y}`; }).join(' ');
  const route = [{ xMm: 0, yMm: 0 }, ...waypoints].map(p => { const v = point(p.xMm, p.yMm); return `${v.x},${v.y}`; }).join(' ');
  return <svg ref={canvas} className="afr2-control-map__svg" viewBox={`0 0 ${size.width} ${size.height}`} role="img" aria-label={pose ? `Mapa superior de simulacion. X ${pose.xMm} milimetros, Y ${pose.yMm} milimetros.` : 'Mapa superior sin posicion simulada.'}
    onPointerDown={event => { if (onSelectPoint && !pickingDisabled && event.button === 0) pressed.current = { id: event.pointerId, x: event.clientX, y: event.clientY }; }}
    onPointerCancel={() => { pressed.current = null; }}
    onPointerUp={event => {
      const initial = pressed.current; pressed.current = null;
      if (!onSelectPoint || pickingDisabled || !initial || initial.id !== event.pointerId || Math.hypot(event.clientX - initial.x, event.clientY - initial.y) > 6) return;
      const matrix = event.currentTarget.getScreenCTM(); if (!matrix) return;
      const pixel = new DOMPoint(event.clientX, event.clientY).matrixTransform(matrix.inverse());
      onSelectPoint({ xMm: center.xMm - (pixel.y - cy) * 1000 / unit, yMm: center.yMm - (pixel.x - cx) * 1000 / unit });
    }}>
    <defs><pattern id={`${uid}-grid`} width={tile} height={tile} x={cx + center.yMm / 1000 * unit} y={cy + center.xMm / 1000 * unit} patternUnits="userSpaceOnUse"><path d={`M ${tile} 0 L 0 0 0 ${tile}`} fill="none" stroke="var(--map-grid)" strokeWidth="1" /></pattern></defs>
    <rect width={size.width} height={size.height} fill="var(--map-floor)" /><rect width={size.width} height={size.height} fill={`url(#${uid}-grid)`} />
    <path d={`M${cx} ${cy - 16}v32m-16-16h32`} stroke="var(--map-grid-strong)" fill="none" />
    {waypoints.length > 0 && <polyline points={route} fill="none" stroke="var(--text-muted)" strokeDasharray="5 5" strokeWidth="1.5" />}
    {waypoints.map((p, index) => { const v = point(p.xMm, p.yMm); return <g key={p.id} transform={`translate(${v.x} ${v.y})`}><circle r={p.status === 'active' ? 9 : 6} fill="var(--map-floor)" stroke={p.status === 'active' ? 'var(--action)' : 'var(--text-muted)'} strokeWidth="2" /><text y="-13" textAnchor="middle" fill="var(--text-muted)" fontSize="11">{index + 1}</text></g>; })}
    {pose && <path d={lines} fill="none" stroke="var(--action)" strokeWidth="3" strokeLinecap="round" strokeLinejoin="round" opacity={fresh ? 1 : .55} />}
    <g transform={`translate(${robot.x} ${robot.y}) rotate(${pose ? -pose.thetaMrad * 180 / Math.PI / 1000 : 0})`}>
      <circle r="43" fill="var(--action-subtle)" stroke={fresh ? 'var(--action-border)' : 'var(--border-strong)'} strokeDasharray={pose ? undefined : '4 5'} />
      {[-1, 1].map(side => [-1, 1].map(end => <g key={`${side}-${end}`} transform={`translate(${side * 22} ${end * 19})`}><rect x="-6" y="-11" width="12" height="22" rx="4" fill="var(--map-wheel)" /><path d="m-4-6 8 4m-8 2 8 4" stroke="var(--map-wheel-detail)" strokeWidth="1.5" /></g>))}
      <rect x="-17" y="-30" width="34" height="60" rx="9" fill="var(--map-body)" stroke={fresh ? 'var(--action)' : 'var(--text-muted)'} strokeWidth="1.5" />
      <path d="m-7-16 7-8 7 8" fill="none" stroke="var(--action)" strokeWidth="2" strokeLinecap="round" />
      <rect x="-10" y="-7" width="20" height="24" rx="4" fill="var(--action-subtle)" />
      <path d="M0-39v-15m-5 5 5-5 5 5" fill="none" stroke="var(--text-muted)" strokeWidth="1.5" />
    </g>
    {!pose && <text x={cx} y={cy + 80} textAnchor="middle" fill="var(--text-muted)" fontSize="12">Ubicacion desconocida</text>}
    <g transform={`translate(24 ${size.height - 52})`} stroke="var(--text-muted)" strokeWidth="1.5" fill="none"><path d="M0 24V0m-4 5 4-5 4 5M0 24h24m-5-4 5 4-5 4" /><text x="-4" y="-10" fill="var(--text-muted)" stroke="none" fontSize="11">+X</text><text x="30" y="28" fill="var(--text-muted)" stroke="none" fontSize="11">−Y</text></g>
    <path d={`M${scaleEnd - scaleLength} ${scaleY}h${scaleLength}m-${scaleLength} -4v8m${scaleLength} -8v8`} fill="none" stroke="var(--text-muted)" strokeWidth="2" /><text x={scaleEnd - scaleLength / 2} y={scaleY - 15} textAnchor="middle" fill="var(--text-muted)" fontSize="11">{size.width < 440 ? '0,5 m' : '1 m'}</text>
  </svg>;
}
