/* AFR2-20261117-01. Vista en vivo del seguidor de línea, en el estilo del control táctil (vista cenital del modelo).
 * A diferencia de LineTopView (auto fijo, la cinta se mueve), acá la cinta queda fija en el centro con el ancho
 * configurado y en escala real, y el auto se desplaza de lado a lado sobre ella según lo que leen IR1..IR5.
 * Sin auto conectado muestra una vista previa local que zigzaguea sobre la cinta: no envía comandos. */
import { Suspense, useEffect, useMemo, useRef, useState } from 'react';
import { useFrame, useThree } from '@react-three/fiber';
import { CanvasTexture, RepeatWrapping, SRGBColorSpace, type Group } from 'three';
import { SimulationViewport, SimulationZoom } from '../../components/three/SimulationViewport';
import { ModelSpinner } from '../../components/three/ModelSpinner';
import { useRobotModel } from '../../components/three/RobotModel';
import { SimulationRobot } from '../collector/SimulationModel';
import { useSensorBar } from '../sensors/LineTopView';
import { clampTape, useLineTapeMm } from './lineTape';
import { analyzeLine, LINE_CENTER_TEXT, LINE_HISTORY_MS, type LineAnalysis, type LineSample } from '../sensors/lineAnalysis';
import { useConnection } from '../connection/ConnectionContext';

const ZERO_WHEELS = { fl: 0, fr: 0, rl: 0, rr: 0 };

function readColors() {
  const css = getComputedStyle(document.documentElement), get = (k: string) => css.getPropertyValue(k).trim();
  return { accent: get('--action') || '#ff5c7a', idle: get('--text-muted') || '#8a94a6', plate: get('--surface-solid') || '#111827', tape: '#0b0b0d', track: '#e9ecf1', tick: '#c9ced8' };
}

type Source = 'live' | 'preview';
interface SceneProps { source: Source; line: LineAnalysis | null; tapeMm: number; moving: boolean; speedMmS: number; onPreviewBits: (bits: number) => void }

/** Posición del auto respecto de la cinta (m, + = auto a la derecha) y su giro (rad, + = antihorario). */
function liveTarget(line: LineAnalysis | null, spacing: number, lastSide: number) {
  if (!line?.detected) return { x: lastSide * spacing * 2.8, yaw: -lastSide * .18 };
  if (line.horizontal) return { x: 0, yaw: 0 };
  // Línea a la izquierda del auto (position > 0) = el auto quedó a la derecha de la cinta.
  const x = (line.position ?? 0) * spacing;
  const yaw = line.orientation === 'tilt-left' ? -.22 : line.orientation === 'tilt-right' ? .22 : 0;
  return { x, yaw };
}

function Scene({ source, line, tapeMm, moving, speedMmS, onPreviewBits, distance = 1 }: SceneProps & { distance?: number }) {
  const bar = useSensorBar();
  const { size: model } = useRobotModel();
  const { camera, size, invalidate } = useThree();
  const [colors] = useState(readColors);
  const car = useRef<Group>(null);
  const rings = useRef<(Group | null)[]>([]);
  const state = useRef({ x: 0, yaw: 0, scroll: 0, t: 0, lastSide: 0, bits: -1 });
  const tapeM = clampTape(tapeMm) / 1000;
  // Trama del piso: un punto y una cruz tenue cada 4 cm, en una textura que se desplaza (sin mallas por marca).
  // AFR2-20261117-16: 4 m para que la vista completa alejada no muestre el borde en pantallas anchas.
  const FLOOR_SIZE = 4, FLOOR_TILE = .04;
  const floorTexture = useMemo(() => {
    const canvas = document.createElement('canvas'); canvas.width = canvas.height = 64;
    const g = canvas.getContext('2d')!;
    g.fillStyle = colors.tick; g.globalAlpha = .9;
    g.beginPath(); g.arc(32, 32, 4, 0, Math.PI * 2); g.fill();
    g.globalAlpha = .35; g.fillRect(31, 0, 2, 64); g.fillRect(0, 31, 64, 2);
    const texture = new CanvasTexture(canvas);
    texture.wrapS = texture.wrapT = RepeatWrapping; texture.colorSpace = SRGBColorSpace;
    texture.repeat.set(FLOOR_SIZE / FLOOR_TILE, FLOOR_SIZE / FLOOR_TILE); texture.anisotropy = 4;
    return texture;
  }, [colors.tick]);
  useEffect(() => () => floorTexture.dispose(), [floorTexture]);

  // Cámara cenital fija sobre la cinta: escala real, el frente del auto hacia arriba.
  useEffect(() => {
    // Entra el auto completo más su recorrido lateral (±2,8 sensores) a ambos lados.
    const viewW = Math.max(.34, model.x + bar.spacing * 7), viewH = Math.max(model.z, bar.length) * 1.35;
    camera.position.set(0, 5, bar.z * .4); camera.up.set(0, 0, -1); camera.lookAt(0, 0, bar.z * .4);
    // AFR2-20261117-16: `distance` < 1 aleja la cámara (vista completa: se ve más cinta alrededor del auto).
    camera.zoom = Math.min(size.width / viewW, size.height / viewH) * distance;
    camera.updateProjectionMatrix(); invalidate();
  }, [camera, size.width, size.height, bar, model, invalidate, distance]);
  useEffect(() => { invalidate(); }, [line, tapeMm, moving, source, invalidate]);
  useEffect(() => {
    if (line?.detected && !line.horizontal && line.position) state.current.lastSide = Math.sign(line.position);
  }, [line]);

  useFrame((_, delta) => {
    const s = state.current, dt = Math.min(delta, .1);
    let target: { x: number; yaw: number }, bits = 0;
    if (source === 'preview') {
      // Zigzag de demostración: período 4 s, amplitud 1,6 sensores; el giro sigue a la velocidad lateral.
      s.t += dt;
      const amp = bar.spacing * 1.6, w = Math.PI * 2 / 4, forward = Math.max(.06, speedMmS / 1000);
      const x = amp * Math.sin(w * s.t), vx = amp * w * Math.cos(w * s.t);
      target = { x, yaw: -Math.atan2(vx, forward) * .6 };
    } else {
      target = liveTarget(line, bar.spacing, s.lastSide);
      bits = line?.bits ?? 0;
    }
    const k = 1 - Math.exp(-dt * (source === 'preview' ? 14 : 7));
    s.x += (target.x - s.x) * k; s.yaw += (target.yaw - s.yaw) * k;
    // El auto gira alrededor de la barra IR: así la barra queda donde indican los sensores.
    if (car.current) { car.current.position.set(s.x, 0, bar.z); car.current.rotation.y = s.yaw; }
    if (source === 'preview') {
      // IR calculados con la pose dibujada (barra rotada), para que coincidan con la imagen.
      const c = Math.cos(s.yaw);
      for (let i = 0; i < 5; i++) if (Math.abs(s.x + (i - 2) * bar.spacing * c) <= tapeM / 2 + bar.spacing * .18) bits |= 1 << i;
      if (bits !== s.bits) { s.bits = bits; onPreviewBits(bits); }
    }
    // El auto avanza: el piso (marcas laterales) corre hacia abajo de la pantalla.
    const advancing = source === 'preview' || moving;
    // El piso corre en sentido contrario al avance: con la cámara fija sobre la cinta, el avance a lo largo de
    // la cinta es v·cos(giro); el desvío lateral (v·sen(giro)) ya lo muestra el propio auto al correrse.
    if (advancing) {
      const along = Math.max(.04, speedMmS / 1000) * Math.cos(s.yaw);
      s.scroll = (s.scroll + dt * along / FLOOR_TILE) % 1;
      floorTexture.offset.y = s.scroll;
    }
    rings.current.forEach((ring, i) => {
      if (!ring) return;
      const on = !!(bits & (1 << i));
      ring.children.forEach((child, idx) => { child.visible = idx === 0 || on; });
      const mat = (ring.children[0] as unknown as { material: { color: { set: (c: string) => void } } }).material;
      mat.color.set(on ? colors.accent : colors.idle);
    });
    const settling = Math.abs(target.x - s.x) > 1e-4 || Math.abs(target.yaw - s.yaw) > 1e-4;
    if (advancing || settling) invalidate();
  });

  const crossed = source === 'live' && !!line?.horizontal;
  return <>
    <mesh position={[0, .0004, bar.z * .4]} rotation={[-Math.PI / 2, 0, 0]}>
      <planeGeometry args={[FLOOR_SIZE, FLOOR_SIZE]} /><meshBasicMaterial color={colors.track} />
    </mesh>
    {/* Cinta fija en el centro, ancho configurado en escala real. */}
    <mesh position={[0, .001, bar.z * .4]} rotation={[-Math.PI / 2, 0, 0]}>
      <planeGeometry args={[tapeM, FLOOR_SIZE]} /><meshBasicMaterial color={colors.tape} />
    </mesh>
    {crossed && <mesh position={[0, .0012, bar.z]} rotation={[-Math.PI / 2, 0, 0]}>
      <planeGeometry args={[.6, tapeM]} /><meshBasicMaterial color={colors.tape} />
    </mesh>}
    <mesh position={[0, .0006, bar.z * .4]} rotation={[-Math.PI / 2, 0, 0]}>
      <planeGeometry args={[FLOOR_SIZE, FLOOR_SIZE]} /><meshBasicMaterial map={floorTexture} transparent depthWrite={false} />
    </mesh>
    <group ref={car}><group position={[-bar.x, 0, -bar.z]}>
      <SimulationRobot wheelAngles={ZERO_WHEELS} angle={0} />
      <mesh position={[bar.x, bar.top + .001, bar.z]} rotation={[-Math.PI / 2, 0, 0]} renderOrder={5}>
        <planeGeometry args={[bar.spacing * 5.4, bar.spacing * 1.3]} /><meshBasicMaterial color={colors.plate} transparent opacity={.8} depthTest={false} depthWrite={false} />
      </mesh>
      {[0, 1, 2, 3, 4].map(i => <group key={i} ref={el => { rings.current[i] = el; }} position={[bar.x + (i - 2) * bar.spacing, bar.top + .002, bar.z]} rotation={[-Math.PI / 2, 0, 0]}>
        <mesh renderOrder={6}><ringGeometry args={[bar.spacing * .3, bar.spacing * .44, 24]} /><meshBasicMaterial color={colors.idle} depthTest={false} transparent /></mesh>
        <mesh renderOrder={7} visible={false}><circleGeometry args={[bar.spacing * .28, 20]} /><meshBasicMaterial color={colors.accent} depthTest={false} transparent /></mesh>
      </group>)}
    </group></group>
  </>;
}

function ModelReady() { useRobotModel(); return null; }

/**
 * Escenario completo: usa la lectura IR del contexto (la refresca el panel del seguidor) y cae a la vista previa
 * cuando no hay auto o la lectura no es válida.
 */
/** AFR2-20261117-12: `fill` ocupa todo el contenedor (visor de seguimiento a pantalla completa). */
export function LineFollowerLiveView({ tall = false, fill = false, speedMmS = 80 }: { tall?: boolean; fill?: boolean; speedMmS?: number }) {
  const { wsState, megaLinkState, sensors, robotState } = useConnection();
  const online = wsState === 'ready' && megaLinkState === 'ready';
  const live = online && sensors.line.valid && sensors.line.bits !== null;
  const source: Source = live ? 'live' : 'preview';
  const tapeMm = useLineTapeMm();
  const [previewBits, setPreviewBits] = useState(0);
  const history = useRef<LineSample[]>([]);
  const liveBits = live ? sensors.line.bits! : null;

  const line = useMemo(() => {
    if (liveBits === null) { history.current = []; return null; }
    const at = Date.now(), first = analyzeLine(liveBits, []);
    history.current = [...history.current.filter(x => at - x.atMs <= LINE_HISTORY_MS), { atMs: at, position: first.position, horizontal: first.horizontal }];
    return analyzeLine(liveBits, history.current);
  // sensors.line.ageMs cambia en cada lectura nueva aunque la máscara se repita.
  // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [liveBits, sensors.line.ageMs]);

  const bits = live ? liveBits! : previewBits;
  const shown = live ? line : analyzeLine(previewBits, []);
  const moving = robotState?.mode === 'line-follow' && robotState.stopped === false;
  const label = live ? (shown ? LINE_CENTER_TEXT[shown.center] : 'Sin lectura IR válida') : 'Vista previa local: el auto zigzaguea sobre la cinta';
  // AFR2-20261117-17: en la vista completa, mismo selector de zoom que los simuladores. 100 % = encuadre ajustado;
  // arranca en 60 % (el zoom por defecto de la vista completa) y va de 10 % a 250 %.
  const [zoomPercent, setZoomPercent] = useState(60);

  return <div className="afr2-line-live" data-tall={tall ? 'true' : 'false'} data-fill={fill ? 'true' : undefined} data-source={source}>
    <div className="afr2-line-live__stage" role="img" aria-label={label}>
      <SimulationViewport floorSize={4} camera={{ position: [0, 5, 0], zoom: 600, near: .001, far: 50 }}
        overlay={fill ? <SimulationZoom percent={zoomPercent} info="Escala real · trama cada 4 cm"
          onChange={value => setZoomPercent(Math.max(10, Math.min(250, Math.round(value))))} /> : undefined}
        fallback={<p className="afr2-help">Sin 3D en este dispositivo.</p>}>
        <Suspense fallback={null}>
          <Scene source={source} line={line} tapeMm={tapeMm} moving={moving} speedMmS={speedMmS} onPreviewBits={setPreviewBits} distance={fill ? zoomPercent / 100 : 1} />
        </Suspense>
      </SimulationViewport>
      <Suspense fallback={<span className="afr2-line-live__loading"><ModelSpinner size="sm" label="" /></span>}><ModelReady /></Suspense>
      <div className="afr2-line-live__hud">
        <span className="afr2-line-live__pill" data-state={live ? (moving ? "run" : "idle") : "preview"}>
          {live ? (moving ? 'Siguiendo línea' : 'Auto detenido · lectura en vivo') : 'Vista previa · sin auto'}
        </span>
        <ol className="afr2-line-live__ir" aria-label="Sensores IR1 a IR5">
          {[0, 1, 2, 3, 4].map(i => <li key={i} data-on={bits & (1 << i) ? 'true' : 'false'}>IR{i + 1}</li>)}
        </ol>
      </div>
      <p className="afr2-line-live__caption">{label}</p>
    </div>
  </div>;
}
