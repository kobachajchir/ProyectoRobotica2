import { ModelAssetProgress } from '../../components/three/ModelAssetProgress';
import { Suspense, lazy, useEffect, useMemo, useState, type ReactNode } from 'react';
import { Disclosure } from '../../components/ui/Disclosure';
import { RenderBoundary } from '../../components/ui/RenderBoundary';
import { Button } from '../../components/ui/Button';
import { SegmentedControl } from '../../components/ui/SegmentedControl';
import { useTheme } from '../../app/providers/ThemeProvider';
import type { RobotPose } from '../../protocol/robotState';
import { TopMap } from './TopMap';
import { clearRobotModelCache } from '../../components/three/RobotModel';
import type { RobotWheelValues } from '../../components/three/robotWheelMotion';
import type { MapView, TrailPoint, MapSceneProps, MapPoint, MapWaypoint } from './controlMapTypes';

const Scene = lazy(() => import('./ControlMapScene').then(module => ({ default: module.ControlMapScene })));
const VIEWS: { value: MapView; label: string }[] = [{ value: 'top', label: 'Superior' }, { value: 'perspective', label: 'Perspectiva' }, { value: 'follow', label: 'Seguir simulación' }];
export interface RequestedMotion { label: string; vxMmS: number; vyMmS: number; wzMradS: number }
export interface SimulationMapFeed {
  pose: RobotPose | null; trail: TrailPoint[]; cursorMs: number; totalDurationMs: number;
  isPlaying: boolean; profileSummary: string; onResetOrigin?: () => void;
  wheelAngles?: RobotWheelValues;
}

/** Simulation only. Commanded motion and physical acknowledgements are never pose. */
export function ControlMap({ requestedMotion, onViewChange, simulationFeed = null, title = 'Simulación de la secuencia', initialView = 'perspective', waypoints, onSelectPoint, pickingDisabled = false, compact = false, routePanel, sidebarPanel }: {
  requestedMotion: RequestedMotion | null; onViewChange: () => void; simulationFeed?: SimulationMapFeed | null;
  title?: string; initialView?: MapView; waypoints?: MapWaypoint[]; onSelectPoint?: (point: MapPoint) => void; pickingDisabled?: boolean;
  compact?: boolean; routePanel?: ReactNode; sidebarPanel?: ReactNode;
}) {
  const { resolvedTheme } = useTheme();
  const [view, setView] = useState<MapView>(initialView);
  const [zoom, setZoom] = useState(onSelectPoint ? .5 : 1);
  const [center, setCenter] = useState({ xMm: 0, yMm: 0 });
  const [showTrail, setShowTrail] = useState(true);
  const [viewerKey, setViewerKey] = useState(0);
  const pose = simulationFeed?.pose ?? null;
  const routeKey = JSON.stringify(waypoints?.map(point => [point.xMm, point.yMm]) ?? []);
  const routeFrame = useMemo(() => {
    const coordinates = JSON.parse(routeKey) as [number, number][];
    if (!coordinates.length) return null;
    const points = [{ xMm: 0, yMm: 0 }, ...coordinates.map(([xMm, yMm]) => ({ xMm, yMm }))];
    const xs = points.map(point => point.xMm), ys = points.map(point => point.yMm);
    const span = Math.max(Math.max(...xs) - Math.min(...xs), Math.max(...ys) - Math.min(...ys), 1200);
    return { center: { xMm: (Math.min(...xs) + Math.max(...xs)) / 2, yMm: (Math.min(...ys) + Math.max(...ys)) / 2 }, zoom: Math.max(.02, Math.min(1, (view === 'top' ? 1200 : 700) / span)) };
  }, [routeKey, view]);
  useEffect(() => { if (routeFrame) { setCenter(routeFrame.center); setZoom(routeFrame.zoom); } }, [routeFrame]);
  useEffect(() => { if (view === 'follow' && pose) setCenter({ xMm: pose.xMm, yMm: pose.yMm }); }, [view, pose]);
  const scene: MapSceneProps = { pose, wheelAngles: simulationFeed?.wheelAngles, trail: showTrail ? simulationFeed?.trail ?? [] : [], view, zoom, center, fresh: !!pose, dark: resolvedTheme === 'dark', waypoints, onSelectPoint, pickingDisabled };
  const fallback = <div className="afr2-map-svg-recovery"><TopMap {...scene} /><div className="afr2-map-recovery-actions"><span>3D no disponible · esquema de simulación</span><Button size="sm" onClick={() => { onViewChange(); clearRobotModelCache(); setViewerKey(v => v + 1); }}>Reintentar modelo 3D</Button></div></div>;
  const minZoom = waypoints ? .02 : .5;
  const changeZoom = (factor: number) => { onViewChange(); setZoom(z => Math.max(minZoom, Math.min(2.5, z * factor))); };
  const fitRoute = () => {
    if (routeFrame) { setCenter(routeFrame.center); setZoom(routeFrame.zoom); } onViewChange();
  };
  const readings = <div className="afr2-map-readings"><div><span>X simulada</span><strong>{pose ? `${Math.round(pose.xMm)} mm` : '—'}</strong></div><div><span>Y simulada</span><strong>{pose ? `${Math.round(pose.yMm)} mm` : '—'}</strong></div><div><span>Rumbo simulado</span><strong>{pose ? `${(pose.thetaMrad * 180 / Math.PI / 1000).toFixed(1)}°` : '—'}</strong></div><div><span>Tiempo</span><strong>{((simulationFeed?.cursorMs ?? 0) / 1000).toFixed(2)} s / {((simulationFeed?.totalDurationMs ?? 0) / 1000).toFixed(2)} s</strong></div></div>;
  const footer = <footer className="afr2-control-map__footer"><p>{simulationFeed?.profileSummary ?? 'La posición es calculada localmente, no informada por el robot.'}</p><div><label><input type="checkbox" checked={showTrail} onChange={e => setShowTrail(e.target.checked)} /> Mostrar recorrido</label><button type="button" onClick={() => { onViewChange(); setCenter({ xMm: 0, yMm: 0 }); simulationFeed?.onResetOrigin?.(); }}>Volver al origen</button></div></footer>;
  return <section className="afr2-control-map" data-compact={compact} aria-label="Simulación del movimiento">
    <header className="afr2-control-map__heading"><div><p className="afr2-kicker">Vista previa · Sin telemetría física</p><h2>{title}</h2></div><span className="afr2-map-status">{simulationFeed?.isPlaying ? 'Reproduciendo simulación' : 'Simulación local'}</span></header>
    <div className="afr2-control-map__toolbar"><SegmentedControl size="sm" ariaLabel="Vista del mapa" items={VIEWS} value={view} onChange={next => { onViewChange(); setView(next); }} />
      <div className="afr2-map-tools"><button type="button" aria-label="Alejar mapa" disabled={zoom <= minZoom} onClick={() => changeZoom(.8)}>−</button><span>{Math.round(zoom * 100)}%</span><button type="button" aria-label="Acercar mapa" disabled={zoom >= 2.5} onClick={() => changeZoom(1.25)}>+</button><button type="button" disabled={!pose} onClick={() => { onViewChange(); if (pose) setCenter({ xMm: pose.xMm, yMm: pose.yMm }); }}>Centrar</button>{waypoints && <button type="button" disabled={!waypoints.length} onClick={fitRoute}>Ver ruta</button>}</div>
    </div>
    <div className="afr2-control-map__viewport" data-picking={!!onSelectPoint && !pickingDisabled}><RenderBoundary key={`${view}-${viewerKey}`} fallback={fallback}><Suspense fallback={<div className="afr2-map-loading" role="status">Cargando modelo…</div>}><Scene {...scene} fallback={fallback} /></Suspense></RenderBoundary><div className="afr2-map-caption"><span>1 unidad = 1 m · Cuadrícula de 0,5 m</span></div></div>
    <ModelAssetProgress label="Modelo del auto" />
    {routePanel && <div className="afr2-map-route">{routePanel}</div>}
    {onSelectPoint && !compact && <p className="afr2-map-command">{pickingDisabled ? 'Pausá la simulación para agregar destinos.' : 'Tocá el suelo para agregar un destino a la lista · Ajuste de 0,1 m. También podés ingresar coordenadas con el teclado.'}</p>}
    {sidebarPanel ? <aside className="afr2-map-sidebar" aria-label="Estado y controles de la simulación">{readings}{sidebarPanel}</aside> : readings}
    {requestedMotion && <p className="afr2-map-command">Consigna solicitada: {requestedMotion.label}. No representa movimiento medido.</p>}
    {compact ? <div className="afr2-map-details"><Disclosure title="Detalles del simulador">{footer}<p className="afr2-help">Tocá el suelo para agregar destinos (ajuste de 0,1 m) o ingresá coordenadas en Recorrido local.</p></Disclosure></div> : footer}
  </section>;
}
