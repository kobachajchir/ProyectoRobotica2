import { SimulationViewport } from '../../components/three/SimulationViewport';
import { SimulationOrbit, SimulationRobot } from '../collector/SimulationModel';
import { useEffect, useMemo, useRef, useState, type ReactNode } from 'react';
import { useFrame, useThree, type ThreeEvent } from '@react-three/fiber';
import { OrthographicCamera, Vector3 } from 'three';
import { Line2 } from 'three/addons/lines/Line2.js';
import { LineGeometry } from 'three/addons/lines/LineGeometry.js';
import { LineMaterial } from 'three/addons/lines/LineMaterial.js';
import { placeMapOverlay, type MapOverlayAnchor, type MapPoint, type MapSceneProps, type TrailPoint } from './controlMapTypes';

const world = (xMm: number, yMm: number): [number, number, number] => [-yMm / 1000, 0, -xMm / 1000];

function readMapPalette() {
  const css = getComputedStyle(document.documentElement);
  const get = (key: string) => css.getPropertyValue(key).trim();
  return { floor: get('--map-floor'), grid: get('--map-grid'), accent: get('--action'), muted: get('--map-waypoint-muted') };
}

function Polyline({ points, color }: { points: [number, number, number][]; color: string }) {
  const { size } = useThree();
  const line = useMemo(() => {
    const geometry = new LineGeometry().setPositions(points.flat());
    return new Line2(geometry, new LineMaterial({ color, linewidth: 2.5 }));
  }, [points, color]);
  useEffect(() => { line.material.resolution.set(size.width, size.height); }, [line, size]);
  useEffect(() => () => { line.geometry.dispose(); line.material.dispose(); }, [line]);
  return <primitive object={line} />;
}

function useDraftColor() { const [color] = useState(() => readMapPalette().accent || '#4f8cff'); return { color }; }

function CameraRig({ view, pose, center, zoom }: MapSceneProps) {
  const { camera, size, invalidate } = useThree();
  useEffect(() => {
    const target = world(center.xMm, center.yMm);
    const distance = 1.2 / zoom;
    if (camera instanceof OrthographicCamera) {
      const halfHeight = 1 / zoom;
      const halfWidth = halfHeight * size.width / size.height;
      camera.left = -halfWidth; camera.right = halfWidth;
      camera.top = halfHeight; camera.bottom = -halfHeight;
    }
    if (view === 'top') {
      camera.position.set(target[0], 5, target[2]); camera.up.set(0, 0, -1); camera.lookAt(target[0], 0, target[2]);
    } else if (view === 'follow' && pose) {
      const theta = pose.thetaMrad / 1000;
      camera.position.set(target[0] + Math.sin(theta) * distance, distance * .65, target[2] + Math.cos(theta) * distance);
      camera.up.set(0, 1, 0); camera.lookAt(target[0], .08, target[2]);
    } else {
      camera.position.set(target[0] + distance * .8, distance, target[2] + distance * .8);
      camera.up.set(0, 1, 0); camera.lookAt(target[0], .08, target[2]);
    }
    camera.updateProjectionMatrix(); invalidate();
  }, [camera, invalidate, view, pose, center, zoom, size.width, size.height]);
  return null;
}

function Trajectory({ trail, color }: { trail: TrailPoint[]; color: string }) {
  const segments = useMemo(() => {
    const result: [number, number, number][][] = [];
    for (const point of trail) {
      if (!result.length || point.segmentStart) result.push([]);
      const p = world(point.xMm, point.yMm);
      result[result.length - 1].push([p[0], .012, p[2]]);
    }
    return result;
  }, [trail]);
  return <>{segments.filter(segment => segment.length > 1).map((segment, index) => <Polyline key={index} points={segment} color={color} />)}</>;
}

function DestinationPlane({ center, zoom, onSelectPoint, pickingDisabled, drawing, onDrawStroke }: MapSceneProps) {
  const pressed = useRef<{ id: number; x: number; y: number } | null>(null);
  // AFR2-20261112-07: lápiz. Un único puntero capturado; los demás se ignoran, así el trazo es uno y continuo.
  const stroke = useRef<{ id: number; points: MapPoint[] } | null>(null);
  const [draft, setDraft] = useState<MapPoint[]>([]);
  const { color } = useDraftColor();
  const floorPoint = (event: ThreeEvent<PointerEvent>): MapPoint => ({ xMm: -event.point.z * 1000, yMm: -event.point.x * 1000 });
  const stepMm = 12 / zoom;
  const drawDown = (event: ThreeEvent<PointerEvent>) => {
    if (stroke.current || event.button !== 0) return;
    event.stopPropagation();
    try { (event.target as unknown as Element).setPointerCapture?.(event.pointerId); } catch { /* puntero sintético o ya liberado */ }
    stroke.current = { id: event.pointerId, points: [floorPoint(event)] }; setDraft(stroke.current.points);
  };
  const drawMove = (event: ThreeEvent<PointerEvent>) => {
    const current = stroke.current;
    if (!current || current.id !== event.pointerId) return;
    const p = floorPoint(event), last = current.points[current.points.length - 1];
    if (Math.hypot(p.xMm - last.xMm, p.yMm - last.yMm) < stepMm) return;
    current.points = [...current.points, p]; setDraft(current.points);
  };
  const drawUp = (event: ThreeEvent<PointerEvent>) => {
    const current = stroke.current;
    if (!current || current.id !== event.pointerId) return;
    stroke.current = null; setDraft([]);
    const p = floorPoint(event), last = current.points[current.points.length - 1];
    const points = Math.hypot(p.xMm - last.xMm, p.yMm - last.yMm) > 1 ? [...current.points, p] : current.points;
    onDrawStroke?.(points, 90 / zoom);
  };
  const drawCancel = () => { stroke.current = null; setDraft([]); };
  const draftLine = useMemo(() => draft.map(p => { const [x, , z] = world(p.xMm, p.yMm); return [x, .02, z] as [number, number, number]; }), [draft]);
  if (drawing) return <>
    <mesh position={world(center.xMm, center.yMm)} rotation={[-Math.PI / 2, 0, 0]}
      onPointerDown={drawDown} onPointerMove={drawMove} onPointerUp={drawUp} onPointerCancel={drawCancel} onLostPointerCapture={drawCancel}>
      <planeGeometry args={[Math.max(16, 8 / zoom), Math.max(16, 8 / zoom)]} />
      <meshBasicMaterial transparent opacity={0} depthWrite={false} colorWrite={false} />
    </mesh>
    {draftLine.length > 1 && <Polyline points={draftLine} color={color} />}
  </>;
  const down = (event: ThreeEvent<PointerEvent>) => {
    if (pickingDisabled || event.button !== 0) return;
    pressed.current = { id: event.pointerId, x: event.clientX, y: event.clientY };
  };
  const up = (event: ThreeEvent<PointerEvent>) => {
    const initial = pressed.current; pressed.current = null;
    if (!initial || initial.id !== event.pointerId || pickingDisabled || Math.hypot(event.clientX - initial.x, event.clientY - initial.y) > 6) return;
    event.stopPropagation();
    onSelectPoint?.({ xMm: -event.point.z * 1000, yMm: -event.point.x * 1000 }, 90/zoom);
  };
  return <mesh position={world(center.xMm, center.yMm)} rotation={[-Math.PI / 2, 0, 0]}
    onPointerDown={down} onPointerUp={up} onPointerCancel={() => { pressed.current = null; }}>
    <planeGeometry args={[Math.max(16, 8 / zoom), Math.max(16, 8 / zoom)]} />
    <meshBasicMaterial transparent opacity={0} depthWrite={false} colorWrite={false} />
  </mesh>;
}

function Destinations({ waypoints = [], plannedRoute, accent, muted }: Pick<MapSceneProps, 'waypoints' | 'plannedRoute'> & { accent: string; muted: string }) {
  const planned = useMemo(() => (plannedRoute ?? [{ xMm: 0, yMm: 0 }, ...waypoints]).map(point => {
    const [x, , z] = world(point.xMm, point.yMm); return [x, .006, z];
  }) as [number, number, number][], [waypoints, plannedRoute]);
  return <>
    {planned.length > 1 && <Polyline points={planned} color={muted} />}
    {waypoints.map(point => {
      const [x, , z] = world(point.xMm, point.yMm), active = point.status === 'active';
      // AFR2-20261113-01: nodo de giro = círculo con un rombo adentro (círculo de 4 lados).
      if (point.shape === 'turn') return <group key={point.id} position={[x, .016, z]}>
        <mesh rotation={[-Math.PI / 2, 0, 0]}><ringGeometry args={[active ? .07 : .055, active ? .09 : .07, 32]} /><meshBasicMaterial color={active ? accent : muted} /></mesh>
        <mesh rotation={[-Math.PI / 2, 0, 0]}><circleGeometry args={[active ? .045 : .036, 4]} /><meshBasicMaterial color={active ? accent : muted} /></mesh>
      </group>;
      return <group key={point.id} position={[x, .015, z]}>
        <mesh rotation={[-Math.PI / 2, 0, 0]}><ringGeometry args={[active ? .065 : .045, active ? .085 : .06, 24]} /><meshBasicMaterial color={active ? accent : muted} /></mesh>
        <mesh rotation={[-Math.PI / 2, 0, 0]}><circleGeometry args={[.014, 12]} /><meshBasicMaterial color={active ? accent : muted} /></mesh>
      </group>;
    })}
  </>;
}

/** AFR2-20261113-01: reubica la ventana flotante sobre su punto en cada cuadro dibujado (zoom, vista, tamaño). */
function OverlayAnchor({ anchor }: { anchor: MapOverlayAnchor }) {
  const { invalidate } = useThree();
  const projected = useMemo(() => new Vector3(), []);
  useEffect(() => {
    invalidate(); const frame = requestAnimationFrame(() => invalidate());
    // Si la ventana cambia de alto (carga del modelo, texto), se vuelve a ubicar.
    const element = anchor.ref.current, observer = element ? new ResizeObserver(() => invalidate()) : null;
    if (element) observer!.observe(element);
    return () => { cancelAnimationFrame(frame); observer?.disconnect(); };
  }, [anchor, invalidate]);
  useFrame(({ camera, size }) => {
    const element = anchor.ref.current; if (!element) return;
    const [x, , z] = world(anchor.point.xMm, anchor.point.yMm);
    projected.set(x, .02, z).project(camera);
    placeMapOverlay(element, (projected.x + 1) / 2 * size.width, (1 - projected.y) / 2 * size.height, size.width, size.height);
  });
  return null;
}

export function ControlMapScene({ fallback, ...props }: MapSceneProps & { fallback?: ReactNode }) {
  // AFR2-20261101-07: colores desde tokens.css (mismos valores que antes estaban fijos aquí).
  const [color, setColor] = useState(readMapPalette);
  useEffect(() => {
    // ThemeProvider aplica la clase .dark en un efecto; leer después de ese commit.
    const frame = requestAnimationFrame(() => setColor(readMapPalette()));
    return () => cancelAnimationFrame(frame);
  }, [props.dark]);
  const floorCenter = world(Math.round(props.center.xMm / 500) * 500, Math.round(props.center.yMm / 500) * 500);
  const position = props.pose ? world(props.pose.xMm, props.pose.yMm) : [0, 0, 0] as [number, number, number];
  const floorSize = Math.max(16, Math.ceil(8 / props.zoom));
  return <SimulationViewport floorSize={floorSize} floorCenter={floorCenter} camera={{ position: [1, 1, 1], near: .001, far: 400 }}
    fallback={fallback ?? <div className="afr2-map-fallback">WebGL no está disponible.</div>}>
    <CameraRig {...props} />
    <SimulationOrbit enabled={!props.onSelectPoint && props.view !== 'follow'} focus={[floorCenter[0], .08, floorCenter[2]]} />
    {props.onSelectPoint && <DestinationPlane {...props} />}
    <Destinations waypoints={props.waypoints} plannedRoute={props.plannedRoute} accent={color.accent} muted={color.muted} />
    <Trajectory trail={props.trail} color={color.accent} />
    {props.overlayAnchor && <OverlayAnchor anchor={props.overlayAnchor} />}
    <group position={position} rotation={[0, props.pose ? props.pose.thetaMrad / 1000 : 0, 0]}><SimulationRobot wheelAngles={props.wheelAngles ?? { fl: 0, fr: 0, rl: 0, rr: 0 }} angle={0} /></group>
  </SimulationViewport>;
}
