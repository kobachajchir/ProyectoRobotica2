import type { RobotPose } from '../../protocol/robotState';
import type { RobotWheelValues } from '../../components/three/robotWheelMotion';

export type MapView = 'top' | 'perspective' | 'follow';
export interface MapPoint { xMm: number; yMm: number }
export interface MapWaypoint extends MapPoint { id: string; status: 'pending' | 'active' | 'completed'; label?: string }
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
}
