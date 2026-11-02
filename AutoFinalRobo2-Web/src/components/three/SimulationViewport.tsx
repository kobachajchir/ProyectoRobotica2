import { useEffect, useRef, useState, type ReactNode } from 'react';
import { Canvas, type CanvasProps } from '@react-three/fiber';
import { useTheme } from '../../app/providers/ThemeProvider';
import { UiIcon } from '../ui/UiIcon';
import '../../styles/simulation-viewport.css';

/** Shared simulation renderer. Domain adapters supply models, camera and interactions.
 * Environment, lighting, density and canvas lifecycle are identical in every simulator. */
export function SimulationViewport({ children, overlay, className = '', floorSize = 4,
  floorCenter = [0, 0, 0], fallback, ...canvasProps }: Omit<CanvasProps, 'children' | 'fallback'> & {
  children: ReactNode; overlay?: ReactNode; className?: string; floorSize?: number;
  floorCenter?: [number, number, number]; fallback?: ReactNode;
}) {
  const { resolvedTheme } = useTheme();
  const read = () => {
    const css = getComputedStyle(document.documentElement);
    return ['--sim-scene-sky', '--sim-scene-ground', '--sim-scene-grid'].map(key => css.getPropertyValue(key).trim());
  };
  const [colors, setColors] = useState(read);
  useEffect(() => { const frame = requestAnimationFrame(() => setColors(read())); return () => cancelAnimationFrame(frame); }, [resolvedTheme]);
  return <div className={`afr2-simulation-viewport ${className}`}>
    <Canvas orthographic frameloop="demand" dpr={[1, 1.5]} camera={{ near: .001, far: 400, zoom: 300 }}
      onContextMenu={event => event.preventDefault()} {...canvasProps}
      fallback={fallback ?? <p role="status">WebGL no está disponible en este dispositivo.</p>}>
      <color attach="background" args={[colors[0]]} />
      <ambientLight intensity={1.4} />
      <directionalLight position={[3, 5, 2]} intensity={2} />
      <directionalLight position={[-3, 2, -2]} intensity={.7} />
      <mesh rotation={[-Math.PI / 2, 0, 0]} position={[floorCenter[0], -.003, floorCenter[2]]}>
        <planeGeometry args={[floorSize, floorSize]} /><meshBasicMaterial color={colors[1]} toneMapped={false} />
      </mesh>
      <gridHelper position={floorCenter} args={[floorSize, Math.round(floorSize * 10), colors[2], colors[2]]} />
      {children}
    </Canvas>
    {overlay}
  </div>;
}

/** One accessible zoom control for standalone simulators, sequences and GOTO. */
export function SimulationZoom({ percent, onChange }: {
  percent: number; onChange: (percent: number) => void;
}) {
  const [editing, setEditing] = useState(false);
  const [draft, setDraft] = useState('');
  const previous = useRef(percent);
  const active = useRef(false);
  const begin = () => { previous.current = percent; active.current = true; setDraft(String(percent)); setEditing(true); };
  const finish = (cancel = false) => {
    if (!active.current) return;
    active.current = false;
    const value = Number(draft.trim().replace(',', '.'));
    const valid = draft.trim() !== '' && Number.isFinite(value) && value >= 0 && value <= 100;
    onChange(!cancel && valid ? value : previous.current);
    setEditing(false);
  };
  return <div className="afr2-simulation-overlays">
    <div className="afr2-simulation-info" aria-label="Escala y cuadrícula">1 unidad = 1 m · Cuadrícula de 0,1 m</div>
    <div className="afr2-simulation-zoom" role="group" aria-label="Zoom de la simulación">
      <button type="button" aria-label="Alejar" disabled={percent <= 0} onClick={() => onChange(Math.max(0, percent - 10))}><UiIcon name="minus" size={20} /></button>
      {editing ? <input autoFocus type="text" inputMode="decimal" aria-label="Porcentaje de zoom, de 0 a 100" value={draft}
        onFocus={event => event.currentTarget.select()} onChange={event => setDraft(event.target.value)}
        onBlur={() => finish()} onKeyDown={event => {
          if (event.key === 'Enter' || event.key === 'Escape') { event.preventDefault(); finish(event.key === 'Escape'); }
        }} /> : <button type="button" aria-label={`Zoom ${percent} %. Editar porcentaje`} onClick={begin}>{percent}%</button>}
      <button type="button" aria-label="Acercar" disabled={percent >= 100} onClick={() => onChange(Math.min(100, percent + 10))}><UiIcon name="plus" size={20} /></button>
    </div>
  </div>;
}
