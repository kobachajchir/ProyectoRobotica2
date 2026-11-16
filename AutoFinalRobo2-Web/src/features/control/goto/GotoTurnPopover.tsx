/* AFR2-20261113-01. Ventana flotante de un nodo de giro del GOTO: ángulo (campo + deslizador) y el auto visto desde
 * arriba con el modelo 3D, que gira en vivo. 0° = frente inicial del recorrido; + antihorario (izquierda). */
import { Suspense, useEffect, useId, useState } from 'react';
import { useThree } from '@react-three/fiber';
import { SimulationViewport } from '../../../components/three/SimulationViewport';
import { Button } from '../../../components/ui/Button';
import { UiIcon } from '../../../components/ui/UiIcon';
import { SimulationRobot } from '../../collector/SimulationModel';

const ZERO_WHEELS = { fl: 0, fr: 0, rl: 0, rr: 0 };
const toDeg = (mrad: number) => Math.round(mrad * 180 / Math.PI / 1000);
const clampDeg = (deg: number) => Math.max(-180, Math.min(180, Math.round(deg)));

function TopCamera() {
  const { camera, invalidate } = useThree();
  useEffect(() => {
    camera.position.set(0, 5, 0); camera.up.set(0, 0, -1); camera.lookAt(0, 0, 0);
    camera.updateProjectionMatrix(); invalidate();
  }, [camera, invalidate]);
  return null;
}

/** Auto visto desde arriba: el frente apunta hacia arriba con 0°, igual que la vista superior del mapa. */
export function TurnCarPreview({ headingDeg, pathDeg }: { headingDeg: number; pathDeg: number | null }) {
  return <div className="afr2-goto-turn__view">
    <SimulationViewport floorSize={1} camera={{ position: [0, 5, 0], zoom: 250, near: .001, far: 50 }}
      fallback={<p className="afr2-help">Sin 3D en este dispositivo.</p>}>
      <TopCamera />
      <Suspense fallback={null}>
        <group rotation={[0, headingDeg * Math.PI / 180, 0]}><SimulationRobot wheelAngles={ZERO_WHEELS} angle={0} /></group>
      </Suspense>
    </SimulationViewport>
    <svg className="afr2-goto-turn__compass" viewBox="-50 -50 100 100" aria-hidden="true">
      <circle r="46" fill="none" stroke="var(--border-default)" strokeDasharray="2 4" />
      {pathDeg !== null && <path d="M0 -30 V -46" transform={`rotate(${-pathDeg})`} stroke="var(--text-muted)" strokeWidth="2" strokeDasharray="3 3" />}
      <path d="M0 -34 L-5 -26 M0 -34 L5 -26 M0 -34 V -46" transform={`rotate(${-headingDeg})`} stroke="var(--action)" strokeWidth="2.5" fill="none" strokeLinecap="round" />
    </svg>
  </div>;
}

export function GotoTurnPopover({ title, headingMrad, pathHeadingMrad, disabled, onChange, onDelete, onClose }: {
  title: string; headingMrad: number; pathHeadingMrad: number | null; disabled?: boolean;
  /** `final` = fin del arrastre o valor escrito: solo entonces se guarda un paso de deshacer. */
  onChange: (headingMrad: number, final: boolean) => void; onDelete: () => void; onClose: () => void;
}) {
  const uid = useId();
  const [deg, setDeg] = useState(toDeg(headingMrad));
  const [text, setText] = useState(String(toDeg(headingMrad)));
  useEffect(() => { const value = toDeg(headingMrad); setDeg(value); setText(String(value)); }, [headingMrad]);
  const apply = (value: number, final: boolean) => {
    const next = clampDeg(value); setDeg(next); setText(String(next));
    onChange(Math.round(next * Math.PI / 180 * 1000), final);
  };
  const pathDeg = pathHeadingMrad === null ? null : toDeg(pathHeadingMrad);
  return <div className="afr2-goto-turn" role="dialog" aria-labelledby={`${uid}-title`}
    onKeyDown={event => { if (event.key === 'Escape') { event.stopPropagation(); onClose(); } }}>
    <header><h3 id={`${uid}-title`}><span className="afr2-goto-turn__glyph" aria-hidden="true" />{title}</h3>
      <span className="afr2-goto-turn__actions"><Button size="sm" variant="subtle" aria-label="Quitar giro" title="Quitar giro" disabled={disabled} icon={<UiIcon name="trash" size={16} />} onClick={onDelete} />
        <Button size="sm" variant="subtle" aria-label="Cerrar" title="Cerrar" icon={<UiIcon name="close" size={16} />} onClick={onClose} /></span></header>
    <TurnCarPreview headingDeg={deg} pathDeg={pathDeg} />
    <label className="afr2-goto-turn__angle">Rumbo del auto (°)
      <input className="afr2-glass-field" type="number" min={-180} max={180} step={1} value={text} disabled={disabled}
        onChange={event => { setText(event.target.value); const value = Number(event.target.value); if (event.target.value.trim() !== '' && Number.isFinite(value)) apply(value, true); }}
        onBlur={() => setText(String(deg))} />
    </label>
    <input className="afr2-goto-turn__slider" type="range" min={-180} max={180} step={1} value={deg} disabled={disabled} aria-label="Rumbo del auto en grados"
      onChange={event => apply(Number(event.target.value), false)}
      onPointerUp={event => apply(Number(event.currentTarget.value), true)}
      onKeyUp={event => apply(Number(event.currentTarget.value), true)} />
    <p className="afr2-help" title="El auto se detiene en este punto, gira en el lugar y mantiene ese rumbo hasta el fin del tramo.">0° = frente inicial · + antihorario{pathDeg !== null ? ` · camino a ${pathDeg}°` : ''}</p>
  </div>;
}
