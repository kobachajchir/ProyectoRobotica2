import { ModelAssetProgress } from '../../components/three/ModelAssetProgress';
import { Suspense, useEffect, useMemo, useRef, useState } from 'react';
import { Canvas, useFrame, useThree } from '@react-three/fiber';
import {
  AdditiveBlending, BufferGeometry, CanvasTexture, CatmullRomCurve3, Color, DoubleSide, Group,
  LineCurve3, OrthographicCamera, Shape, TubeGeometry, Vector3,
} from 'three';
import { RobotModel } from '../../components/three/RobotModel';
import { advanceServo } from '../../components/three/servoMotion';
import type { RangeSample } from '../../protocol/sensorHead';

const SENSOR_Z = -.1028;
type Point = [number, number, number];
const point = (angle: number, radius: number, height = .018): Point => [
  Math.cos(angle * Math.PI / 180) * radius, height, SENSOR_Z - Math.sin(angle * Math.PI / 180) * radius,
];
function arc(radius: number, height: number, from = 0, to = 180): Point[] {
  const count = Math.max(3, Math.ceil((to - from) / 2.5) + 1);
  return Array.from({ length: count }, (_, i) => point(from + (to - from) * i / (count - 1), radius, height));
}
function sector(radius: number, from: number, to: number) {
  const shape = new Shape();
  shape.moveTo(0, 0);
  shape.lineTo(Math.cos(from * Math.PI / 180) * radius, Math.sin(from * Math.PI / 180) * radius);
  shape.absarc(0, 0, radius, from * Math.PI / 180, to * Math.PI / 180, false);
  shape.lineTo(0, 0);
  return shape;
}

/** Volumetric strokes with a restrained halo; no postprocessing or decorative render loop. */
function NeonLine({ points, color, radius, opacity = 1, glow = false }: {
  points: Point[]; color: string; radius: number; opacity?: number; glow?: boolean;
}) {
  const geometry = useMemo(() => {
    const vectors = points.map(p => new Vector3(...p));
    const curve = vectors.length === 2 ? new LineCurve3(vectors[0], vectors[1]) : new CatmullRomCurve3(vectors);
    return new TubeGeometry(curve, Math.max(1, points.length * 2), radius, 6, false);
  }, [points, radius]);
  useEffect(() => () => geometry.dispose(), [geometry]);
  return <group>
    <mesh>
      <primitive object={geometry} attach="geometry" dispose={null} />
      <meshBasicMaterial color={color} transparent opacity={opacity} toneMapped={false} depthWrite={false} />
    </mesh>
    {glow && <mesh>
      <tubeGeometry args={[geometry.parameters.path, geometry.parameters.tubularSegments, radius * 3.5, 6, false]} />
      <meshBasicMaterial color={color} transparent opacity={opacity * .12} blending={AdditiveBlending}
        toneMapped={false} depthWrite={false} />
    </mesh>}
  </group>;
}

export type RadarView = 'perspective' | 'top';
function Camera({ range, view }: { range: number; view: RadarView }) {
  const { camera, size, invalidate } = useThree();
  useEffect(() => {
    const center = SENSOR_Z - range * .38;
    if (view === 'top') {
      camera.position.set(0, 8, center); camera.up.set(0, 0, -1);
    } else {
      camera.position.set(range * .15, 6, center + 5.2); camera.up.set(0, 1, 0);
    }
    camera.lookAt(0, 0, center); camera.updateMatrixWorld(true);
    // Fit the projected field and the real car, rather than a largely empty rectangle.
    const bounds: Point[] = [...arc(range * 1.2, .03), [-.23, 0, .3], [.23, .23, .3], [-.23, .23, SENSOR_Z]];
    const projected = bounds.map(p => new Vector3(...p).applyMatrix4(camera.matrixWorldInverse));
    const xs = projected.map(p => p.x), ys = projected.map(p => p.y);
    const left = Math.min(...xs), right = Math.max(...xs), bottom = Math.min(...ys), top = Math.max(...ys);
    camera.translateX((left + right) / 2); camera.translateY((bottom + top) / 2);
    const margin = range * .08 + .06;
    const aspect = Math.max(size.width, 1) / Math.max(size.height, 1);
    const height = Math.max(top - bottom + margin * 2, (right - left + margin * 2) / aspect);
    if (camera instanceof OrthographicCamera) {
      camera.left = -height * aspect / 2; camera.right = -camera.left;
      camera.top = height / 2; camera.bottom = -camera.top;
    }
    camera.updateProjectionMatrix(); camera.updateMatrixWorld(true); invalidate();
  }, [camera, size.width, size.height, range, view, invalidate]);
  return null;
}

interface Props {
  range: number; angle: number | null; enabled: boolean; points: readonly RangeSample[];
  currentId?: number; dark: boolean; historical: boolean;
  scanMin: number; scanMax: number; preview: boolean; view: RadarView;
}
function readPalette() {
  const css = getComputedStyle(document.documentElement);
  return {
    floor: css.getPropertyValue('--map-floor').trim(),
    grid: css.getPropertyValue('--map-grid-strong').trim(),
    text: css.getPropertyValue('--text-muted').trim(),
    accent: css.getPropertyValue('--action').trim(),
  };
}
type Palette = ReturnType<typeof readPalette>;

function PlotLabel({ text, position, color, scale }: { text: string; position: Point; color: string; scale: number }) {
  const texture = useMemo(() => {
    const canvas = document.createElement('canvas'); canvas.width = 256; canvas.height = 80;
    const ctx = canvas.getContext('2d')!;
    ctx.font = '500 36px ui-monospace, monospace'; ctx.textAlign = 'center'; ctx.textBaseline = 'middle';
    ctx.fillStyle = color; ctx.fillText(text, 128, 40);
    return new CanvasTexture(canvas);
  }, [text, color]);
  useEffect(() => () => texture.dispose(), [texture]);
  return <sprite position={position} scale={[scale * 3.2, scale, 1]}>
    <spriteMaterial map={texture} transparent depthTest={false} depthWrite={false} toneMapped={false} />
  </sprite>;
}

function Needle({ angle, range, color, muted }: { angle: number; range: number; color: string; muted: boolean }) {
  const group = useRef<Group>(null), velocity = useRef(0), { invalidate } = useThree();
  const [reduce, setReduce] = useState(() => matchMedia('(prefers-reduced-motion: reduce)').matches);
  const target = (angle - 90) * Math.PI / 180;
  const points = useMemo<Point[]>(() => [[0, .095, 0], [0, .095, -range]], [range]);
  const fan = useMemo(() => sector(range, 86, 94), [range]);
  useEffect(() => {
    const q = matchMedia('(prefers-reduced-motion: reduce)'), change = () => setReduce(q.matches);
    q.addEventListener('change', change); return () => q.removeEventListener('change', change);
  }, []);
  useEffect(() => {
    if (reduce && group.current) { group.current.rotation.y = target; velocity.current = 0; }
    invalidate();
  }, [target, reduce, invalidate]);
  useFrame((_, delta) => {
    if (!group.current) return;
    if (Math.abs(group.current.rotation.y - target) < .0001 && Math.abs(velocity.current) < .001) {
      group.current.rotation.y = target; velocity.current = 0; return;
    }
    const [value, v] = advanceServo(group.current.rotation.y, velocity.current, target, delta);
    group.current.rotation.y = reduce ? target : value; velocity.current = reduce ? 0 : v; invalidate();
  });
  return <group ref={group} position={[0, 0, SENSOR_Z]}>
    <mesh position={[0, .014, 0]} rotation={[-Math.PI / 2, 0, 0]}>
      <shapeGeometry args={[fan, 48]} />
      <meshBasicMaterial color={color} transparent opacity={muted ? .02 : .065} depthWrite={false} side={DoubleSide} />
    </mesh>
    <NeonLine points={points} color={color} radius={Math.max(.0015, range * .0018)} opacity={muted ? .4 : 1} glow />
  </group>;
}

// A single transparent grid plane replaces the disconnected solid pedestals.
// The radial fade keeps the scene part of the workspace, including when there are no echoes.
function FloorGrid({ range, palette }: { range: number; palette: Palette }) {
  const uniforms = useMemo(() => ({
    uGrid: { value: new Color(palette.grid) }, uAccent: { value: new Color(palette.accent) },
    uRange: { value: range }, uStep: { value: range / 4 },
  }), [range, palette.grid, palette.accent]);
  return <mesh position={[0, -.008, SENSOR_Z]} rotation={[-Math.PI / 2, 0, 0]}>
    <planeGeometry args={[range * 2.7 + .6, range * 2.7 + .6]} />
    <shaderMaterial uniforms={uniforms} transparent depthWrite={false}
      vertexShader={`varying vec2 vPoint;
        void main(){vPoint=position.xy;gl_Position=projectionMatrix*modelViewMatrix*vec4(position,1.0);}`}
      fragmentShader={`varying vec2 vPoint;
        uniform vec3 uGrid;uniform vec3 uAccent;uniform float uRange;uniform float uStep;
        void main(){
          vec2 distanceToLine=abs(fract(vPoint/uStep-.5)-.5)*uStep;
          vec2 pixels=distanceToLine/max(fwidth(vPoint),vec2(.00001));
          float line=1.0-min(min(pixels.x,pixels.y),1.0);
          float radius=length(vPoint)/uRange;
          float fade=1.0-smoothstep(.72,1.28,radius);
          float halo=exp(-radius*radius*5.0)*.045;
          float alpha=(line*.18+halo)*fade;
          gl_FragColor=vec4(mix(uGrid,uAccent,halo*5.0),alpha);
          #include <tonemapping_fragment>
          #include <colorspace_fragment>
        }`} />
  </mesh>;
}

function Marks({ range, color }: { range: number; color: string }) {
  const geometry = useMemo(() => new BufferGeometry().setFromPoints(
    Array.from({ length: 37 }, (_, i) => [point(i * 5, range * 1.015), point(i * 5, range * (i % 3 ? 1.028 : 1.05))])
      .flat().map(p => new Vector3(...p)),
  ), [range]);
  useEffect(() => () => geometry.dispose(), [geometry]);
  return <lineSegments>
    <primitive object={geometry} attach="geometry" dispose={null} />
    <lineBasicMaterial color={color} transparent opacity={.55} depthWrite={false} />
  </lineSegments>;
}

function CarShadow({ color }: { color: string }) {
  const texture = useMemo(() => {
    const canvas = document.createElement('canvas'); canvas.width = canvas.height = 64;
    const ctx = canvas.getContext('2d')!, gradient = ctx.createRadialGradient(32, 32, 3, 32, 32, 32);
    gradient.addColorStop(0, 'rgba(0,0,0,.26)'); gradient.addColorStop(1, 'rgba(0,0,0,0)');
    ctx.fillStyle = gradient; ctx.fillRect(0, 0, 64, 64);
    return new CanvasTexture(canvas);
  }, []);
  useEffect(() => () => texture.dispose(), [texture]);
  return <mesh position={[0, -.003, .035]} rotation={[-Math.PI / 2, 0, 0]}>
    <planeGeometry args={[.46, .56]} />
    <meshBasicMaterial map={texture} color={color} transparent opacity={.65} depthWrite={false} />
  </mesh>;
}

function Plot({ range, angle, enabled, points, currentId, historical, scanMin, scanMax, preview, view, dark, palette }: Props & { palette: Palette }) {
  const { grid, accent, text } = palette;
  const radius = Math.max(.0009, range * .0009);
  const arcs = useMemo(() => [1, 2, 3, 4].map(n => arc(range * n / 4, .014)), [range]);
  const selected = useMemo(() => [1, 2, 3, 4].map(n => arc(range * n / 4, .017, scanMin, scanMax)), [range, scanMin, scanMax]);
  const rays = useMemo(() => [30, 60, 90, 120, 150].map(a => [point(a, 0, .01), point(a, range, .01)]), [range]);
  const bounds = useMemo(() => [scanMin, scanMax].map(a => [point(a, 0, .019), point(a, range, .019)]), [range, scanMin, scanMax]);
  const zone = useMemo(() => sector(range, scanMin, scanMax), [range, scanMin, scanMax]);
  const bright = useMemo(() => `#${new Color(accent).lerp(new Color('#ffffff'), dark ? .28 : .03).getHexString()}`, [accent, dark]);
  return <>
    <Camera range={range} view={view} />
    <FloorGrid range={range} palette={palette} />
    {arcs.map((p, i) => <NeonLine key={i} points={p} color={grid} radius={radius * .7} opacity={.4} />)}
    {rays.map((p, i) => <NeonLine key={i} points={p} color={grid} radius={radius * .45} opacity={.19} />)}
    <Marks range={range} color={grid} />
    <mesh position={[0, .006, SENSOR_Z]} rotation={[-Math.PI / 2, 0, 0]}>
      <shapeGeometry args={[zone, 72]} />
      <meshBasicMaterial color={accent} transparent opacity={dark ? .065 : .045} side={DoubleSide} depthWrite={false} />
    </mesh>
    {selected.map((p, i) => <NeonLine key={i} points={p} color={i === 3 ? bright : accent}
      radius={radius * (i === 3 ? 1.6 : .9)} opacity={i === 3 ? preview ? .6 : .9 : .28} glow={i === 3 && dark} />)}
    {bounds.map((p, i) => <NeonLine key={i} points={p} color={accent} radius={radius} opacity={.45} />)}
    {[1, 2, 3, 4].map(n => <PlotLabel key={n} text={`${range * n / 4} m`} position={point(158, range * n / 4, .035)} color={text} scale={range * .1 + .012} />)}
    {[0, 30, 60, 90, 120, 150, 180].map(a => <PlotLabel key={a} text={`${a}°`} position={point(a, range * 1.105, .025)} color={text} scale={range * .08 + .012} />)}
    <CarShadow color={grid} />
    {angle !== null && enabled && <Needle angle={angle} range={range} color={bright} muted={historical} />}
    {points.filter(p => p.valid && p.distanceMm !== null && p.distanceMm <= range * 1000).map(p => {
      const current = p.sampleId === currentId;
      const size = Math.max(.004, range * .006);
      return <group key={p.angleDeg} position={point(p.angleDeg, p.distanceMm! / 1000, .025)}>
        <mesh rotation={[-Math.PI / 2, 0, 0]}>
          <circleGeometry args={[size * 2.8, 24]} />
          <meshBasicMaterial color={bright} transparent opacity={current ? .18 : .07} side={DoubleSide} depthWrite={false} toneMapped={false} />
        </mesh>
        <mesh position={[0, size, 0]}>
          <sphereGeometry args={[size * (current ? 1.35 : 1), 12, 8]} />
          <meshBasicMaterial color={current ? bright : accent} transparent opacity={current ? 1 : historical ? .45 : .85} toneMapped={false} />
        </mesh>
      </group>;
    })}
    <ambientLight intensity={.85} />
    <hemisphereLight args={['#ffffff', palette.floor, .8]} />
    <directionalLight position={[1, 4, 2]} intensity={2} />
    <Suspense fallback={null}><RobotModel /></Suspense>
  </>;
}

export function RadarScene(props: Props) {
  const [palette, setPalette] = useState(readPalette);
  useEffect(() => { const frame = requestAnimationFrame(() => setPalette(readPalette())); return () => cancelAnimationFrame(frame); }, [props.dark]);
  return <div style={{ position: 'relative', width: '100%', height: '100%' }}><div className="afr2-model-asset-overlay"><ModelAssetProgress label="Modelo del auto" hideWhenReady /></div><Canvas orthographic frameloop="demand" dpr={[1, 1.5]} camera={{ near: .01, far: 30 }}
    fallback={<p>WebGL no disponible. Las mediciones siguen disponibles a la derecha.</p>}>
    <color attach="background" args={[palette.floor]} />
    <Plot {...props} palette={palette} />
  </Canvas></div>;
}
