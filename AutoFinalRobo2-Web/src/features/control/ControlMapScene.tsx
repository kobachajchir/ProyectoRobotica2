import { useEffect, useMemo, useRef, useState, type ReactNode } from 'react';
import { Canvas, useThree, type ThreeEvent } from '@react-three/fiber';
import { OrthographicCamera } from 'three';
import { Line2 } from 'three/addons/lines/Line2.js';
import { LineGeometry } from 'three/addons/lines/LineGeometry.js';
import { LineMaterial } from 'three/addons/lines/LineMaterial.js';
import { RobotModel } from '../../components/three/RobotModel';
import type { MapSceneProps, TrailPoint } from './controlMapTypes';

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
      camera.position.set(target[0], 5, target[2]);
      camera.up.set(0, 0, -1);
      camera.lookAt(target[0], 0, target[2]);
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

function DestinationPlane({ center, zoom, onSelectPoint, pickingDisabled }: MapSceneProps) {
  const pressed = useRef<{ id: number; x: number; y: number } | null>(null);
  const down = (event: ThreeEvent<PointerEvent>) => {
    if (pickingDisabled || event.button !== 0) return;
    pressed.current = { id: event.pointerId, x: event.clientX, y: event.clientY };
  };
  const up = (event: ThreeEvent<PointerEvent>) => {
    const initial = pressed.current; pressed.current = null;
    if (!initial || initial.id !== event.pointerId || pickingDisabled || Math.hypot(event.clientX - initial.x, event.clientY - initial.y) > 6) return;
    event.stopPropagation();
    onSelectPoint?.({ xMm: -event.point.z * 1000, yMm: -event.point.x * 1000 });
  };
  return <mesh position={world(center.xMm, center.yMm)} rotation={[-Math.PI / 2, 0, 0]}
    onPointerDown={down} onPointerUp={up} onPointerCancel={() => { pressed.current = null; }}>
    <planeGeometry args={[Math.max(16, 8 / zoom), Math.max(16, 8 / zoom)]} />
    <meshBasicMaterial transparent opacity={0} depthWrite={false} colorWrite={false} />
  </mesh>;
}

function Destinations({ waypoints = [], accent, muted }: Pick<MapSceneProps, 'waypoints'> & { accent: string; muted: string }) {
  const planned = useMemo(() => [[0, .006, 0], ...waypoints.map(point => {
    const [x, , z] = world(point.xMm, point.yMm); return [x, .006, z];
  })] as [number, number, number][], [waypoints]);
  return <>
    {waypoints.length > 0 && <Polyline points={planned} color={muted} />}
    {waypoints.map(point => {
      const [x, , z] = world(point.xMm, point.yMm), active = point.status === 'active';
      return <group key={point.id} position={[x, .015, z]}>
        <mesh rotation={[-Math.PI / 2, 0, 0]}><ringGeometry args={[active ? .065 : .045, active ? .085 : .06, 24]} /><meshBasicMaterial color={active ? accent : muted} /></mesh>
        <mesh rotation={[-Math.PI / 2, 0, 0]}><circleGeometry args={[.014, 12]} /><meshBasicMaterial color={active ? accent : muted} /></mesh>
      </group>;
    })}
  </>;
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
  return <Canvas orthographic={props.view === 'top'} frameloop="demand" dpr={[1, 1.5]} camera={{ position: [1, 1, 1], fov: 45, near: .01, far: 400 }}
    fallback={fallback ?? <div className="afr2-map-fallback">WebGL no está disponible.</div>}>
    <color attach="background" args={[color.floor]} /><ambientLight intensity={1.6} /><directionalLight position={[3, 5, 2]} intensity={1.8} />
    <CameraRig {...props} />
    <gridHelper position={floorCenter} args={[floorSize, floorSize * 2, color.grid, color.grid]} />
    {props.onSelectPoint && <DestinationPlane {...props} />}
    <Destinations waypoints={props.waypoints} accent={color.accent} muted={color.muted} />
    <Trajectory trail={props.trail} color={color.accent} />
    <group position={position} rotation={[0, props.pose ? props.pose.thetaMrad / 1000 : 0, 0]}><RobotModel wheelAngles={props.wheelAngles} /></group>
  </Canvas>;
}
