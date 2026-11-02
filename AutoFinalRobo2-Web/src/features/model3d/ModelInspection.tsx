import { frameModel } from '../../components/three/frameModel';
import { InspectionEnvironment, useInspectionPalette } from '../../components/three/InspectionEnvironment';
import { useEffect, useMemo, useRef, useState, type ReactNode, type RefObject } from 'react';
import { Canvas, useFrame, useThree } from '@react-three/fiber';
import { Link } from 'react-router-dom';
import { Box3, Vector3 } from 'three';
import { OrbitControls } from '../../components/three/OrbitControls';
import { RobotModel, useRobotModel } from '../../components/three/RobotModel';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import { getComponentAnchors, MODEL_COMPONENTS } from './modelComponents';
import '../../styles/model-inspection.css';
import { AssetInspection } from './AssetInspection';
import { CombinedInspection } from './CombinedInspection';

interface PointElements {
  button: HTMLButtonElement | null;
  line: SVGLineElement | null;
  dot: SVGCircleElement | null;
}

function InspectionScene({ modelSize, anchors, elements, referencesVisible }: {
  modelSize: Vector3;
  anchors: Map<string, Vector3>;
  elements: RefObject<Map<string, PointElements>>;
  referencesVisible: boolean;
}) {
  const { camera, size: viewport, invalidate } = useThree();
  const targetY = modelSize.y / 2;
  useEffect(() => {
    frameModel(camera, new Box3(new Vector3(-modelSize.x / 2, 0, -modelSize.z / 2), new Vector3(modelSize.x / 2, modelSize.y, modelSize.z / 2)), viewport);
    invalidate();
  }, [camera, viewport.width, viewport.height, modelSize, targetY, invalidate]);
  return <><RobotModel animateServo={false} /><OrbitControls targetY={targetY} />
    {referencesVisible && <ProjectedPoints anchors={anchors} elements={elements} />}
  </>;
}

function ProjectedPoints({ anchors, elements }: {
  anchors: Map<string, Vector3>;
  elements: RefObject<Map<string, PointElements>>;
}) {
  const { invalidate } = useThree();
  useEffect(() => { invalidate(); }, [invalidate]);
  const points = useMemo(() => Array.from(anchors, ([id, anchor]) => ({
    id, anchor, projected: new Vector3(), x: 0, y: 0, visible: false,
  })), [anchors]);
  // Registered after OrbitControls: project the camera's current frame. Updating
  // DOM refs avoids React renders on every orbit frame and keeps demand rendering.
  useFrame(({ camera, size }) => {
    camera.updateMatrixWorld();
    for (const point of points) {
      const p = point.projected.copy(point.anchor).project(camera);
      point.x = (p.x + 1) * size.width / 2;
      point.y = (1 - p.y) * size.height / 2;
      point.visible = p.z > -1 && p.z < 1 && point.x >= 0 && point.x <= size.width && point.y >= 0 && point.y <= size.height;
    }
    // Small leaders keep the exact attachment visible while separating nearby
    // 44px selectors (especially the OLED/encoder and stacked boards).
    const positions = points.filter(p => p.visible).map(point => ({ point, x: point.x, y: point.y }));
    for (let pass = 0; pass < 10; pass++) {
      for (let i = 0; i < positions.length; i++) for (let j = i + 1; j < positions.length; j++) {
        const a = positions[i], b = positions[j];
        let dx = b.x - a.x, dy = b.y - a.y;
        const distance = Math.hypot(dx, dy);
        if (distance >= 46) continue;
        if (distance < .01) { dx = 1; dy = 0; }
        const push = (46 - distance) / 2, length = Math.hypot(dx, dy);
        a.x -= dx / length * push; a.y -= dy / length * push;
        b.x += dx / length * push; b.y += dy / length * push;
      }
      for (const p of positions) {
        p.x = Math.max(24, Math.min(size.width - 24, p.x));
        p.y = Math.max(24, Math.min(size.height - 24, p.y));
      }
    }
    for (const point of points) {
      const nodes = elements.current.get(point.id);
      if (!nodes) continue;
      if (nodes.button) nodes.button.hidden = !point.visible;
      if (nodes.line) nodes.line.style.display = point.visible ? '' : 'none';
      if (nodes.dot) nodes.dot.style.display = point.visible ? '' : 'none';
    }
    for (const { point, x, y } of positions) {
      const nodes = elements.current.get(point.id);
      if (nodes?.button) nodes.button.style.transform = `translate(${x}px, ${y}px) translate(-50%, -50%)`;
      if (nodes?.line) {
        nodes.line.setAttribute('x1', String(point.x)); nodes.line.setAttribute('y1', String(point.y));
        nodes.line.setAttribute('x2', String(x)); nodes.line.setAttribute('y2', String(y));
      }
      if (nodes?.dot) { nodes.dot.setAttribute('cx', String(point.x)); nodes.dot.setAttribute('cy', String(point.y)); }
    }
  });
  return null;
}

function CarInspection({ referencesVisible, onToggleReferences, action }: { referencesVisible: boolean; onToggleReferences: () => void; action?: ReactNode }) {
  const { scene, size, offset } = useRobotModel();
  const palette = useInspectionPalette();
  const anchors = useMemo(() => getComponentAnchors(scene, offset), [scene, offset]);
  const [selected, setSelected] = useState<string | null>(null);
  const elements = useRef(new Map<string, PointElements>());
  const panel = useRef<HTMLElement>(null);
  const picker = useRef<HTMLSelectElement>(null);
  const closeButton = useRef<HTMLButtonElement>(null);
  const opener = useRef<HTMLElement | null>(null);
  const components = MODEL_COMPONENTS.filter(component => anchors.has(component.id));
  const component = components.find(item => item.id === selected);
  const refsFor = (id: string) => {
    let refs = elements.current.get(id);
    if (!refs) { refs = { button: null, line: null, dot: null }; elements.current.set(id, refs); }
    return refs;
  };
  const select = (id: string, trigger: HTMLElement) => { opener.current = trigger; setSelected(id); };
  const restoreFocus = () => {
    const target = opener.current;
    (target && !target.hidden && target.isConnected ? target : picker.current)?.focus({ preventScroll: true });
  };
  const close = () => { setSelected(null); restoreFocus(); };
  useEffect(() => { if (selected) closeButton.current?.focus({ preventScroll: true }); }, [selected]);
  useEffect(() => {
    if (!selected) return;
    const pointers = new Set<number>();
    let gesture: { id: number; x: number; y: number; moved: boolean; outside: boolean } | null = null;
    const isOutside = (target: EventTarget | null) => target instanceof Element
      && !panel.current?.contains(target) && !target.closest('[data-model-selector]');
    const down = (event: PointerEvent) => {
      pointers.add(event.pointerId);
      if (pointers.size > 1) { if (gesture) gesture.moved = true; return; }
      gesture = { id: event.pointerId, x: event.clientX, y: event.clientY, moved: false, outside: isOutside(event.target) && event.button === 0 };
    };
    const move = (event: PointerEvent) => {
      if (gesture?.id === event.pointerId && Math.hypot(event.clientX - gesture.x, event.clientY - gesture.y) > 6) gesture.moved = true;
    };
    const up = (event: PointerEvent) => {
      move(event); pointers.delete(event.pointerId);
      if (gesture?.id === event.pointerId) {
        if (gesture.outside && !gesture.moved && isOutside(event.target)) setSelected(null);
        gesture = null;
      }
    };
    const cancel = (event: PointerEvent) => { pointers.delete(event.pointerId); gesture = null; };
    const key = (event: KeyboardEvent) => {
      if (event.key === 'Escape' && !document.querySelector('dialog[open]')) {
        event.preventDefault(); setSelected(null); restoreFocus();
      }
    };
    const wheel = () => { if (gesture) gesture.moved = true; };
    document.addEventListener('pointerdown', down, true);
    document.addEventListener('pointermove', move, true);
    document.addEventListener('pointerup', up, true);
    document.addEventListener('pointercancel', cancel, true);
    document.addEventListener('keydown', key);
    document.addEventListener('wheel', wheel, { passive: true });
    return () => {
      document.removeEventListener('pointerdown', down, true);
      document.removeEventListener('pointermove', move, true);
      document.removeEventListener('pointerup', up, true);
      document.removeEventListener('pointercancel', cancel, true);
      document.removeEventListener('keydown', key);
      document.removeEventListener('wheel', wheel);
    };
  }, [selected]);

  return <div className="afr2-model-inspection">
    <div className="afr2-model-inspection__toolbar" data-model-selector>
      <div className="afr2-model-inspection__picker">
      <label htmlFor="afr2-model-component"><UiIcon name="cube" />Explorar componentes</label>
      <span className="afr2-select-wrap"><select ref={picker} id="afr2-model-component" value={selected ?? ''} disabled={!referencesVisible}
        onChange={event => event.target.value ? select(event.target.value, event.currentTarget) : close()}>
        <option value="">Seleccionar componente</option>
        {components.map((item, index) => <option key={item.id} value={item.id}>{String(index + 1).padStart(2, '0')} · {item.label}</option>)}
      </select></span>
      </div>
      <Button size="sm" className="afr2-model-inspection__toggle" aria-pressed={referencesVisible} aria-controls="afr2-model-references"
        onClick={() => { setSelected(null); onToggleReferences(); }}>
        {referencesVisible ? 'Ocultar referencias' : 'Mostrar referencias'}
      </Button>
    </div>
    <div className="afr2-model-inspection__viewport">
      <Canvas frameloop="demand" dpr={[1, 1.5]} camera={{ position: [.5, .4, .5], fov: 45, near: .005, far: 80 }}
        fallback={<p className="afr2-model-inspection__fallback">WebGL no está disponible. La vista Superior de Control sigue disponible.</p>}>
        <InspectionEnvironment palette={palette} extent={Math.max(size.x, size.y, size.z)} />
        <InspectionScene modelSize={size} anchors={anchors} elements={elements} referencesVisible={referencesVisible} />
      </Canvas>
      <div id="afr2-model-references" className="afr2-model-inspection__points" hidden={!referencesVisible} role="group" aria-label="Componentes del modelo 3D">
        <svg className="afr2-model-inspection__leaders" aria-hidden="true">
          {components.map(item => <g key={item.id} data-selected={selected === item.id}>
            <line ref={node => { refsFor(item.id).line = node; }} />
            <circle r="3" ref={node => { refsFor(item.id).dot = node; }} />
          </g>)}
        </svg>
        {components.map((item, index) => <button key={item.id} type="button" hidden
          ref={node => { refsFor(item.id).button = node; }}
          className="afr2-model-point" data-model-selector data-selected={selected === item.id}
          aria-label={`Ver ${item.label} · ${item.model}`} aria-expanded={selected === item.id}
          aria-controls={selected === item.id ? 'afr2-model-info' : undefined}
          onClick={event => select(item.id, event.currentTarget)}>
          <span className="afr2-model-point__number">{String(index + 1).padStart(2, '0')}</span>
          <span className="afr2-model-point__label">{item.label}</span>
        </button>)}
      </div>
      {referencesVisible && component && <aside id="afr2-model-info" ref={panel} className="afr2-model-info" aria-labelledby="afr2-model-info-title">
        <header className="afr2-model-info__heading">
          <span className="afr2-model-info__icon"><UiIcon name={component.icon} /></span>
          <div><p className="afr2-kicker">Componente del robot</p><h2 id="afr2-model-info-title">{component.label}</h2></div>
          <button ref={closeButton} type="button" className="afr2-model-info__close afr2-pressable" aria-label="Cerrar ficha del componente"
            onClick={close}>
            <UiIcon name="close" />
          </button>
        </header>
        <div className="afr2-model-info__body">
          <p className="afr2-model-info__model">{component.model}</p>
          <p>{component.description}</p>
          <dl>{component.facts.map(([label, value]) => <div key={label}><dt>{label}</dt><dd>{value}</dd></div>)}</dl>
        </div>
        <footer><Link to={component.to} className="afr2-model-info__action afr2-pressable"><UiIcon name="control" />Ir a controlar<UiIcon name="arrow" /></Link>
          <p className="afr2-help">{component.destination}</p>
        </footer>
      </aside>}
      {action && <div className="afr2-model-inspection__cta">{action}</div>}
    </div>
    <p className="afr2-help afr2-model-inspection__help">{referencesVisible
      ? 'Elegí un punto para conocer la pieza. Podés girar o acercar el modelo con la ficha abierta; cerrala con la X, Escape o un clic fuera.'
      : 'Referencias ocultas. Podés girar y acercar el modelo; usa Mostrar referencias para explorar sus componentes.'}</p>
  </div>;
}

/** Car keeps its detailed references; arm and cube use their exported assets. */
export function ModelInspection({ model = 'auto', referencesVisible, onToggleReferences, action }: {
  model?: 'auto' | 'brazo' | 'cubo' | 'conjunto'; referencesVisible: boolean; onToggleReferences: () => void;
  /** AFR2-20261102-01: acción abajo a la derecha del visor (p. ej. «Ir a simulación»). */
  action?: ReactNode;
}) {
  if (model === 'auto') return <CarInspection referencesVisible={referencesVisible} onToggleReferences={onToggleReferences} action={action} />;
  if (model === 'conjunto') return <CombinedInspection referencesVisible={referencesVisible} onToggleReferences={onToggleReferences} action={action} />;
  return <AssetInspection model={model} referencesVisible={referencesVisible} onToggleReferences={onToggleReferences} action={action} />;
}
