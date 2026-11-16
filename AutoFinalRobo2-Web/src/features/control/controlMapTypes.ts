import type { RefObject } from 'react';
import type { RobotPose } from '../../protocol/robotState';
import type { RobotWheelValues } from '../../components/three/robotWheelMotion';

export type MapView = 'top' | 'perspective' | 'follow';
export interface MapPoint { xMm: number; yMm: number }
/** AFR2-20261113-01: `shape: 'turn'` = nodo de giro (círculo con un rombo adentro). */
export interface MapWaypoint extends MapPoint { id: string; status: 'pending' | 'active' | 'completed'; label?: string; shape?: 'turn' }
/** Ventana flotante anclada a un punto del plano; la escena la reubica al mover o hacer zoom. */
export interface MapOverlayAnchor { point: MapPoint; ref: RefObject<HTMLDivElement | null> }
/** Ubica la ventana sobre el punto (en px del visor); si no entra arriba va debajo, y si tampoco, arriba de todo con scroll propio. */
export function placeMapOverlay(element: HTMLDivElement, x: number, y: number, width: number, height: number) {
  const margin = 8, gap = 18;
  element.style.maxHeight = `${Math.max(120, height - margin * 2)}px`;
  const w = element.offsetWidth, h = element.offsetHeight;
  const above = y - h - gap >= margin, below = !above && y + gap + h <= height - margin;
  element.style.left = `${Math.max(w / 2 + margin, Math.min(width - w / 2 - margin, x))}px`;
  element.style.top = `${above ? y - h - gap : below ? y + gap : margin}px`;
  element.dataset.below = String(below); element.dataset.placed = 'true';
}
export interface TrailPoint extends RobotPose {
  segmentStart: boolean;
}
export type PoseSample = TrailPoint;
export interface MapSceneProps {
  pose: RobotPose | null;
  trail: TrailPoint[];
  view: MapView;
  zoom: number;
  center: { xMm: number; yMm: number };
  fresh: boolean;
  dark: boolean;
  wheelAngles?: RobotWheelValues;
  waypoints?: MapWaypoint[];
  plannedRoute?: MapPoint[];
  onSelectPoint?: (point: MapPoint, toleranceMm?: number) => void;
  pickingDisabled?: boolean;
  /** AFR2-20261112-07: modo lápiz. Un solo puntero, sin levantar: al soltar entrega el trazo completo. */
  drawing?: boolean;
  onDrawStroke?: (points: MapPoint[], toleranceMm: number) => void;
  overlayAnchor?: MapOverlayAnchor | null;
}
