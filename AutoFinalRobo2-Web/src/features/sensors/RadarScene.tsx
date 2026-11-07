import { ModelAssetProgress } from '../../components/three/ModelAssetProgress';
import { Suspense, useEffect, useMemo, useRef, useState } from 'react';
import { Canvas, useFrame, useThree } from '@react-three/fiber';
import {
  AdditiveBlending, BufferGeometry, CanvasTexture, CatmullRomCurve3, Color, DoubleSide, Group,
  LineCurve3, Mesh, MeshBasicMaterial, OrthographicCamera, Shape, TubeGeometry, Vector3,
} from 'three';
import { RobotModel } from '../../components/three/RobotModel';
import { advanceServo } from '../../components/three/servoMotion';
import type { RangeSample } from '../../protocol/sensorHead';

/* AFR2-20261102-21 — Radar como instrumento: plato semicircular con degradado, anillos con
   jerarquía, marcas angulares, sector de escaneo, haz con estela suave, ecos con halo y contorno.
   Las etiquetas son HTML (tokens del manual, nítidas en cualquier escala) proyectadas desde la
   escena una vez por cambio de cámara; no hay bucle de dibujo continuo. */

const SENSOR_Z = -.1028;
type Point = [number, number, number];
const point = (angle: number, radius: number, height = .018): Point => [
  Math.cos(angle * Math.PI / 180) * radius, height, SENSOR_Z - Math.sin(angle * Math.PI / 180) * radius,
];
function arc(radius: number, height: number, from = 0, to = 180): Point[] {
  const count = Math.max(3, Math.ceil((to - from) / 2.5) + 1);
  return Array.from({ length: count }, (_, i) => point(from + (to - from) * i / (count - 1), radius, height));
}
function sector(radius: number, from: number, to: number, inner = 0) {
  const shape = new Shape();
  const a = from * Math.PI / 180, b = to * Math.PI / 180;
  shape.moveTo(Math.cos(a) * inner, Math.sin(a) * inner);
  shape.lineTo(Math.cos(a) * radius, Math.sin(a) * radius);
  shape.absarc(0, 0, radius, a, b, false);
  if (inner > 0) shape.absarc(0, 0, inner, b, a, true); else shape.lineTo(0, 0);
  return shape;
}

/** Distancia legible por escala: centímetros por debajo de 1 m, metros desde 1 m. */
export function formatRadarDistance(meters: number) {
  return meters < 1
    ? `${(meters * 100).toLocaleString('es-AR', { maximumFractionDigits: 2 })} cm`
    : `${meters.toLocaleString('es-AR', { maximumFractionDigits: 2 })} m`;
}

function Stroke({ points, color, radius, opacity = 1, glow = false }: {
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
      <tubeGeometry args={[geometry.parameters.path, geometry.parameters.tubularSegments, radius * 3.2, 6, false]} />
      <meshBasicMaterial color={color} transparent opacity={opacity * .14} blending={AdditiveBlending} toneMapped={false} depthWrite={false} />
    </mesh>}
  </group>;
}

export type RadarView = 'perspective' | 'top';
export interface RadarLabel { key: string; text: string; anchor: Point; kind: 'ring' | 'angle' | 'front'; minor?: boolean }
interface ScreenLabel extends Omit<RadarLabel, 'anchor'> { x: number; y: number }

function Camera({ range, view, labels, onLabels }: { range: number; view: RadarView; labels: RadarLabel[]; onLabels: (labels: ScreenLabel[]) => void }) {
  const { camera, size, invalidate } = useThree();
  useEffect(() => {
    const center = SENSOR_Z - range * .42;
    if (view === 'top') {
      camera.position.set(0, 8, center); camera.up.set(0, 0, -1);
    } else {
      camera.position.set(range * .12, 6, center + 5.6); camera.up.set(0, 1, 0);
    }
    camera.lookAt(0, 0, center); camera.updateMatrixWorld(true);
    // Encuadre: el plato con lugar para las etiquetas angulares y el auto real detrás del sensor.
    // El auto entra completo desde 1 m; en alcances cortos solo su frente, para que el plato mande.
    const car = Math.min(.3, range * .32);
    const bounds: Point[] = [...arc(range * 1.16, .03), point(0, range * 1.25), point(180, range * 1.25), [-Math.min(.2, range * .5), 0, SENSOR_Z + car], [Math.min(.2, range * .5), .05, SENSOR_Z + car]];
    const projected = bounds.map(p => new Vector3(...p).applyMatrix4(camera.matrixWorldInverse));
    const xs = projected.map(p => p.x), ys = projected.map(p => p.y);
    const left = Math.min(...xs), right = Math.max(...xs), bottom = Math.min(...ys), top = Math.max(...ys);
    camera.translateX((left + right) / 2); camera.translateY((bottom + top) / 2);
    const margin = range * .03 + .02;
    const aspect = Math.max(size.width, 1) / Math.max(size.height, 1);
    const height = Math.max(top - bottom + margin * 2, (right - left + margin * 2) / aspect);
    if (camera instanceof OrthographicCamera) {
      camera.left = -height * aspect / 2; camera.right = -camera.left;
      camera.top = height / 2; camera.bottom = -camera.top;
    }
    camera.updateProjectionMatrix(); camera.updateMatrixWorld(true); invalidate();
    onLabels(labels.map(({ anchor, ...label }) => {
      const ndc = new Vector3(...anchor).project(camera);
      return { ...label, x: (ndc.x + 1) / 2 * size.width, y: (1 - ndc.y) / 2 * size.height };
    }));
  }, [camera, size.width, size.height, range, view, invalidate, labels, onLabels]);
  return null;
}

interface Props {
  range: number; angle: number | null; enabled: boolean; points: readonly RangeSample[];
  currentId?: number; dark: boolean; historical: boolean;
  scanMin: number; scanMax: number; preview: boolean; view: RadarView;
  /** AFR2-20261106-35: barrido automático en curso con telemetría fresca; dibuja la estela tras el ángulo real. */
  sweeping?: boolean;
  /** Reloj de la página (ms) para desvanecer ecos según su antigüedad de recepción. */
  nowMs?: number;
}
type TimedSample = RangeSample & { receivedAtMs?: number };
const ECHO_FADE_MS = 8000;
function echoFade(p: TimedSample, nowMs: number | undefined) {
  if (nowMs === undefined || p.receivedAtMs === undefined) return 1;
  return Math.max(.3, 1 - Math.max(0, nowMs - p.receivedAtMs) / ECHO_FADE_MS);
}
/** Estela del barrido: sectores detrás del haz, en el sentido de avance observado del servo real.
 * Solo existe con barrido automático y ángulo reportado; sin datos no hay barrido. */
function SweepTrail({ angle, range, color, reduce }: { angle: number; range: number; color: string; reduce: boolean }) {
  const previous = useRef(angle), direction = useRef(1);
  if (angle !== previous.current) { direction.current = angle > previous.current ? 1 : -1; previous.current = angle; }
  const dir = direction.current;
  const slices = useMemo(() => [[0, 8, .16], [8, 16, .09], [16, 26, .04]].map(([a, b, alpha]) => {
    const from = Math.max(0, Math.min(180, angle - dir * b)), to = Math.max(0, Math.min(180, angle - dir * a));
    return { shape: sector(range, Math.min(from, to), Math.max(from, to)), alpha };
  }), [angle, dir, range]);
  if (reduce) return null;
  return <>{slices.map((slice, i) => <mesh key={i} position={[0, .008 + i * .0005, SENSOR_Z]} rotation={[-Math.PI / 2, 0, 0]}>
    <shapeGeometry args={[slice.shape, 24]} />
    <meshBasicMaterial color={color} transparent opacity={slice.alpha} side={DoubleSide} depthWrite={false} toneMapped={false} />
  </mesh>)}</>;
}
function readPalette() {
  const css = getComputedStyle(document.documentElement);
  const get = (name: string) => css.getPropertyValue(name).trim();
  return { floor: get('--radar-floor'), grid: get('--map-grid-strong'), text: get('--text-muted'), accent: get('--action'), surface: get('--surface-solid') };
}
type Palette = ReturnType<typeof readPalette>;

const reducedMotion = () => matchMedia('(prefers-reduced-motion: reduce)').matches;
function useReducedMotion() {
  const [reduce, setReduce] = useState(reducedMotion);
  useEffect(() => {
    const q = matchMedia('(prefers-reduced-motion: reduce)'), change = () => setReduce(q.matches);
    q.addEventListener('change', change); return () => q.removeEventListener('change', change);
  }, []);
  return reduce;
}

/** Plato del radar: degradado radial dentro del semicírculo y grilla de puntos que se apaga afuera. */
function Dish({ range, palette, dark }: { range: number; palette: Palette; dark: boolean }) {
  const uniforms = useMemo(() => ({
    uGrid: { value: new Color(palette.grid) }, uAccent: { value: new Color(palette.accent) },
    uRange: { value: range }, uStep: { value: range / 8 }, uDark: { value: dark ? 1 : 0 },
  }), [range, palette.grid, palette.accent, dark]);
  return <mesh position={[0, -.008, SENSOR_Z]} rotation={[-Math.PI / 2, 0, 0]}>
    <planeGeometry args={[range * 2.9 + .6, range * 2.9 + .6]} />
    <shaderMaterial uniforms={uniforms} transparent depthWrite={false}
      vertexShader={`varying vec2 vPoint;
        void main(){vPoint=position.xy;gl_Position=projectionMatrix*modelViewMatrix*vec4(position,1.0);}`}
      fragmentShader={`varying vec2 vPoint;
        uniform vec3 uGrid;uniform vec3 uAccent;uniform float uRange;uniform float uStep;uniform float uDark;
        void main(){
          float r=length(vPoint)/uRange;
          float inside=step(0.0,vPoint.y)*(1.0-smoothstep(.995,1.005,r));
          vec2 cell=abs(fract(vPoint/uStep)-.5)*uStep;
          float dot=1.0-smoothstep(0.0,1.6*length(fwidth(vPoint)),length(cell));
          float fade=1.0-smoothstep(.85,1.45,r);
          float core=exp(-r*r*7.0);
          float dish=inside*(mix(.13,.05,r)+core*.12)*mix(.8,1.0,uDark);
          float rim=inside*smoothstep(.9,1.0,r)*.06;
          float alpha=dot*.24*fade*(1.0-inside*.6)+dish+rim;
          vec3 color=mix(uGrid,uAccent,inside*core*.9);
          gl_FragColor=vec4(color,alpha);
          #include <tonemapping_fragment>
          #include <colorspace_fragment>
        }`} />
  </mesh>;
}

/** Marcas angulares: menores cada 5°, medias cada 10° y mayores cada 30°. */
function Ticks({ range, color }: { range: number; color: string }) {
  const geometry = useMemo(() => new BufferGeometry().setFromPoints(
    Array.from({ length: 37 }, (_, i) => {
      const a = i * 5, major = a % 30 === 0, mid = a % 10 === 0;
      return [point(a, range * 1.008, .016), point(a, range * (major ? 1.06 : mid ? 1.035 : 1.02), .016)];
    }).flat().map(p => new Vector3(...p)),
  ), [range]);
  useEffect(() => () => geometry.dispose(), [geometry]);
  return <lineSegments>
    <primitive object={geometry} attach="geometry" dispose={null} />
    <lineBasicMaterial color={color} transparent opacity={.7} depthWrite={false} />
  </lineSegments>;
}

/** Haz del sensor: cono de ±9° con caída angular y radial, línea central y punta luminosa. */
function Beam({ angle, range, color, muted }: { angle: number; range: number; color: string; muted: boolean }) {
  const group = useRef<Group>(null), velocity = useRef(0), initialized = useRef(false), { invalidate } = useThree();
  const reduce = useReducedMotion();
  const target = (angle - 90) * Math.PI / 180;
  const line = useMemo<Point[]>(() => [[0, .02, 0], [0, .02, -range]], [range]);
  const cone = useMemo(() => sector(range, 81, 99), [range]);
  const uniforms = useMemo(() => ({ uColor: { value: new Color(color) }, uRange: { value: range }, uStrength: { value: muted ? .25 : 1 } }), [color, range, muted]);
  useEffect(() => {
    if ((!initialized.current || reduce) && group.current) { group.current.rotation.y = target; velocity.current = 0; initialized.current = true; }
    invalidate();
  }, [target, reduce, invalidate]);
  useFrame((_, delta) => {
    if (!group.current) return;
    if (Math.abs(group.current.rotation.y - target) < .0001 && Math.abs(velocity.current) < .001) {
      group.current.rotation.y = target; velocity.current = 0; return;
    }
    const [value, v] = advanceServo(group.current.rotation.y, velocity.current, target, Math.min(delta, .05));
    group.current.rotation.y = reduce ? target : value; velocity.current = reduce ? 0 : v; invalidate();
  });
  const tip = range * .011;
  return <group ref={group} position={[0, 0, SENSOR_Z]}>
    <mesh position={[0, .015, 0]} rotation={[-Math.PI / 2, 0, 0]}>
      <shapeGeometry args={[cone, 48]} />
      <shaderMaterial uniforms={uniforms} transparent depthWrite={false} side={DoubleSide}
        vertexShader={`varying vec2 vPoint;void main(){vPoint=position.xy;gl_Position=projectionMatrix*modelViewMatrix*vec4(position,1.0);}`}
        fragmentShader={`varying vec2 vPoint;uniform vec3 uColor;uniform float uRange;uniform float uStrength;
          void main(){
            float r=length(vPoint)/uRange;
            float off=abs(atan(vPoint.x,vPoint.y))/radians(9.0);
            float alpha=pow(max(0.0,1.0-off),1.8)*mix(.34,.08,r)*uStrength;
            gl_FragColor=vec4(uColor,alpha);
            #include <colorspace_fragment>
          }`} />
    </mesh>
    <Stroke points={line} color={color} radius={range * .0021} opacity={muted ? .45 : 1} glow={!muted} />
    <mesh position={[0, .022, -range]}>
      <sphereGeometry args={[tip, 16, 10]} />
      <meshBasicMaterial color={color} transparent opacity={muted ? .45 : 1} toneMapped={false} />
    </mesh>
    <mesh position={[0, .021, -range]} rotation={[-Math.PI / 2, 0, 0]}>
      <circleGeometry args={[tip * 2.6, 24]} />
      <meshBasicMaterial color={color} transparent opacity={muted ? .06 : .18} toneMapped={false} depthWrite={false} />
    </mesh>
  </group>;
}

/** Señal breve de llegada para un eco real y reciente; nunca un barrido simulado. */
function EchoPulse({ position, radius, color }: { position: Point; radius: number; color: string }) {
  const mesh = useRef<Mesh>(null), material = useRef<MeshBasicMaterial>(null), elapsed = useRef(0);
  const { invalidate } = useThree();
  const reduce = useReducedMotion();
  useEffect(() => { invalidate(); }, [invalidate]);
  useFrame((_, delta) => {
    if (!mesh.current || !material.current) return;
    if (reduce || elapsed.current >= .45) { mesh.current.visible = false; return; }
    elapsed.current = Math.min(.45, elapsed.current + Math.min(delta, .05));
    const t = elapsed.current / .45, eased = 1 - Math.pow(1 - t, 3);
    mesh.current.scale.setScalar(1 + eased * 2.6);
    material.current.opacity = .7 * (1 - t);
    invalidate();
  });
  return <mesh ref={mesh} position={position} rotation={[-Math.PI / 2, 0, 0]} visible={!reduce}>
    <ringGeometry args={[radius * 1.5, radius * 1.85, 40]} />
    <meshBasicMaterial ref={material} color={color} transparent opacity={0} side={DoubleSide} depthWrite={false} toneMapped={false} />
  </mesh>;
}

function CarShadow({ dark }: { dark: boolean }) {
  const texture = useMemo(() => {
    const canvas = document.createElement('canvas'); canvas.width = canvas.height = 64;
    const ctx = canvas.getContext('2d')!, gradient = ctx.createRadialGradient(32, 32, 3, 32, 32, 32);
    gradient.addColorStop(0, 'rgba(0,0,0,.32)'); gradient.addColorStop(1, 'rgba(0,0,0,0)');
    ctx.fillStyle = gradient; ctx.fillRect(0, 0, 64, 64);
    return new CanvasTexture(canvas);
  }, []);
  useEffect(() => () => texture.dispose(), [texture]);
  return <mesh position={[0, -.003, .035]} rotation={[-Math.PI / 2, 0, 0]}>
    <planeGeometry args={[.5, .6]} />
    <meshBasicMaterial map={texture} transparent opacity={dark ? .8 : .4} depthWrite={false} />
  </mesh>;
}

function Plot({ range, angle, enabled, points, currentId, historical, scanMin, scanMax, preview, view, dark, palette, onLabels, sweeping = false, nowMs }: Props & { palette: Palette; onLabels: (labels: ScreenLabel[]) => void }) {
  const { grid, accent } = palette;
  const line = range * .0011;
  const rings = useMemo(() => [1, 2, 3, 4].map(n => arc(range * n / 4, .013)), [range]);
  const halfRings = useMemo(() => [1, 3, 5, 7].map(n => arc(range * n / 8, .012)), [range]);
  const rays = useMemo(() => [30, 60, 90, 120, 150].map(a => [point(a, range * .06, .011), point(a, range, .011)]), [range]);
  const baseline = useMemo<Point[]>(() => [point(180, range, .012), point(0, range, .012)], [range]);
  const bounds = useMemo(() => [scanMin, scanMax].map(a => [point(a, 0, .019), point(a, range, .019)]), [range, scanMin, scanMax]);
  const zoneEdge = useMemo(() => arc(range, .02, scanMin, scanMax), [range, scanMin, scanMax]);
  const zone = useMemo(() => sector(range, scanMin, scanMax), [range, scanMin, scanMax]);
  const bright = useMemo(() => `#${new Color(accent).lerp(new Color('#ffffff'), dark ? .22 : 0).getHexString()}`, [accent, dark]);
  const visible = useMemo(() => points.filter(p => p.valid && p.distanceMm !== null && p.distanceMm <= range * 1000)
    .slice().sort((a, b) => a.angleDeg - b.angleDeg), [points, range]);
  // Contorno: une ecos vecinos (≤ 10°) para leer la silueta de lo detectado.
  const contours = useMemo(() => {
    const runs: Point[][] = []; let run: Point[] = [];
    visible.forEach((p, i) => {
      const at = point(p.angleDeg, p.distanceMm! / 1000, .021);
      if (i && p.angleDeg - visible[i - 1].angleDeg > 10) { if (run.length > 1) runs.push(run); run = []; }
      run.push(at);
    });
    if (run.length > 1) runs.push(run);
    return runs;
  }, [visible]);
  const labels = useMemo<RadarLabel[]>(() => [
    ...[1, 2, 3, 4].map(n => ({ key: `r${n}`, kind: 'ring' as const, minor: n % 2 === 1, text: formatRadarDistance(range * n / 4), anchor: point(0, range * n / 4, .02) })),
    ...[0, 30, 60, 120, 150, 180].map(a => ({ key: `a${a}`, kind: 'angle' as const, text: `${a}°`, anchor: point(a, range * 1.12, .02) })),
    { key: 'front', kind: 'front' as const, text: '90° · frente', anchor: point(90, range * 1.12, .02) },
  ], [range]);
  const latest = !historical && currentId ? visible.find(p => p.sampleId === currentId) : undefined;
  const dot = range * .0105;
  const reduceMotion = useReducedMotion();
  return <>
    <Camera range={range} view={view} labels={labels} onLabels={onLabels} />
    <Dish range={range} palette={palette} dark={dark} />
    {halfRings.map((p, i) => <Stroke key={`h${i}`} points={p} color={grid} radius={line * .45} opacity={dark ? .4 : .3} />)}
    {rings.map((p, i) => <Stroke key={i} points={p} color={grid} radius={line * (i === 3 ? 1.3 : .85)} opacity={i === 3 ? 1 : dark ? .85 : .7} />)}
    {rays.map((p, i) => <Stroke key={i} points={p} color={grid} radius={line * .45} opacity={.28} />)}
    <Stroke points={baseline} color={grid} radius={line * .75} opacity={.6} />
    <Ticks range={range} color={grid} />
    <mesh position={[0, .006, SENSOR_Z]} rotation={[-Math.PI / 2, 0, 0]}>
      <shapeGeometry args={[zone, 72]} />
      <meshBasicMaterial color={accent} transparent opacity={preview ? .045 : dark ? .075 : .06} side={DoubleSide} depthWrite={false} />
    </mesh>
    <Stroke points={zoneEdge} color={accent} radius={line * 1.9} opacity={preview ? .5 : .95} glow={dark && !preview} />
    {bounds.map((p, i) => <Stroke key={i} points={p} color={accent} radius={line * .9} opacity={preview ? .35 : .6} />)}
    <CarShadow dark={dark} />
    {angle !== null && enabled && sweeping && !historical && <SweepTrail angle={angle} range={range} color={bright} reduce={reduceMotion} />}
    {angle !== null && enabled && <Beam angle={angle} range={range} color={bright} muted={historical} />}
    {contours.map((run, i) => <Stroke key={`c${i}`} points={run} color={bright} radius={line * 1.4} opacity={historical ? .35 : .7} />)}
    {latest && <EchoPulse key={latest.sampleId} position={point(latest.angleDeg, latest.distanceMm! / 1000, .03)} radius={dot} color={bright} />}
    {visible.map(p => {
      const current = p.sampleId === currentId, fade = current ? 1 : echoFade(p as TimedSample, nowMs);
      return <group key={p.angleDeg} position={point(p.angleDeg, p.distanceMm! / 1000, .026)}>
        <mesh rotation={[-Math.PI / 2, 0, 0]}>
          <circleGeometry args={[dot * (current ? 3.2 : 2.4), 28]} />
          <meshBasicMaterial color={bright} transparent opacity={(current ? .22 : historical ? .06 : .1) * fade} side={DoubleSide} depthWrite={false} toneMapped={false} />
        </mesh>
        <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, .001, 0]}>
          <ringGeometry args={[dot * 1.05, dot * 1.45, 28]} />
          <meshBasicMaterial color={palette.surface} transparent opacity={.9} side={DoubleSide} depthWrite={false} toneMapped={false} />
        </mesh>
        <mesh position={[0, dot * .5, 0]}>
          <sphereGeometry args={[dot * (current ? 1.3 : 1), 16, 10]} />
          <meshBasicMaterial color={current ? bright : accent} transparent opacity={(current ? 1 : historical ? .5 : .9) * fade} toneMapped={false} />
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
  const [labels, setLabels] = useState<ScreenLabel[]>([]);
  useEffect(() => { const frame = requestAnimationFrame(() => setPalette(readPalette())); return () => cancelAnimationFrame(frame); }, [props.dark]);
  return <div className="afr2-radar-stage">
    <div className="afr2-model-asset-overlay"><ModelAssetProgress label="Modelo del auto" hideWhenReady /></div>
    <Canvas orthographic frameloop="demand" dpr={[1, 2]} camera={{ near: .01, far: 30 }}
      fallback={<p>WebGL no disponible. Las mediciones siguen disponibles a la derecha.</p>}>
      <color attach="background" args={[palette.floor]} />
      <Plot {...props} palette={palette} onLabels={setLabels} />
    </Canvas>
    <div className="afr2-radar-labels" aria-hidden="true">
      {labels.map(label => <span key={label.key} className="afr2-radar-label" data-kind={label.kind} data-minor={label.minor ? 'true' : undefined}
        style={{ transform: `translate3d(${label.x.toFixed(1)}px, ${label.y.toFixed(1)}px, 0)` }}><b>{label.text}</b></span>)}
    </div>
  </div>;
}
