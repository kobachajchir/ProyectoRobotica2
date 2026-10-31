import { Suspense, useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { Canvas, useThree, type ThreeEvent } from '@react-three/fiber';
import { BufferGeometry, CanvasTexture, Matrix4, Plane, Quaternion, Vector3 } from 'three';
import { useRobotModel } from '../../components/three/RobotModel';
import { SimulationRobot, SimulationOrbit } from './SimulationModel';
import { RenderBoundary } from '../../components/ui/RenderBoundary';
import { Button } from '../../components/ui/Button';
import { useTheme } from '../../app/providers/ThemeProvider';
import { getComponentAnchors } from '../model3d/modelComponents';
import { ARM_MOUNT, STANDALONE_MOUNT, CAD_TO_THREE, forwardArm, groundWorkspacePoints, localToWorld, rad, toThree, workspacePoints, type Point3 } from './armKinematics';
import { BIN, cubeReach, halt, moveBase, positionCube, setJoint } from './collectorSimulation';
import { useCollector } from './CollectorContext';
import { useArmPoints } from './ArmPointsContext';
import { robotModel, modelProblems } from './robotModel';
import { rigid, forwardModel } from './modelKinematics';
import { ArmLink, ArticulatedArmAsset, ProceduralGripper } from './ArmVisualAdapter';
import { frameSignature } from './armPointStorage';
import { AuxiliaryModelAsset } from './AuxiliaryModelAsset';
import { CUBE_MODEL } from './auxiliaryModels';

export type SceneView = 'perspective' | 'top' | 'side';
export type SceneTool = 'orbit' | 'move' | 'place';
interface SceneProps { view: SceneView; tool: SceneTool; focus: Point3; revision: number; armPanel: boolean; carOnly?: boolean }

function readPalette() {
  const css = getComputedStyle(document.documentElement);
  const get = (key: string) => css.getPropertyValue(key).trim();
  return { accent: get('--action'), floor: get('--map-floor'), grid: get('--map-grid-strong'),
    text: get('--text-muted'), surface: get('--surface-solid'), error: get('--error'), metal: get('--text-subtle'), success: get('--success') };
}

function Label({ text, position, color }: { text: string; position: Point3; color: string }) {
  const texture = useMemo(() => {
    const canvas = document.createElement('canvas'); canvas.width = 384; canvas.height = 80;
    const ctx = canvas.getContext('2d')!;
    ctx.fillStyle = color; ctx.font = '600 28px system-ui'; ctx.textAlign = 'center'; ctx.textBaseline = 'middle';
    ctx.fillText(text, 192, 40);
    return new CanvasTexture(canvas);
  }, [text, color]);
  useEffect(() => () => texture.dispose(), [texture]);
  return <sprite position={position} scale={[.17, .035, 1]}>
    <spriteMaterial map={texture} transparent depthTest={false} depthWrite={false} />
  </sprite>;
}
function Stroke({ points, color }: { points: Point3[]; color: string }) {
  const geometry = useMemo(() => new BufferGeometry().setFromPoints(points.map(p => new Vector3(...p))), [points]);
  useEffect(() => () => geometry.dispose(), [geometry]);
  return <lineSegments geometry={geometry}><lineBasicMaterial color={color} transparent opacity={.7} /></lineSegments>;
}
function Camera({ view, focus, armPanel, carOnly }: Pick<SceneProps, 'view' | 'focus' | 'armPanel' | 'carOnly'>) {
  const { camera, size, invalidate } = useThree();
  const model = robotModel(), modelKey = `${model.id}:${model.revision}`;
  const reach = model.joints.reduce((sum, j) => sum + Math.hypot(...j.origin.position), Math.hypot(...model.tcp.position));
  useEffect(() => {
    const target = new Vector3(...focus);
    camera.up.set(0, 1, 0);
    if (view === 'top') { camera.position.copy(target).add(new Vector3(0, 3, 0)); camera.up.set(0, 0, -1); }
    else if (view === 'side') camera.position.copy(target).add(new Vector3(3, 0, 0));
    else camera.position.copy(target).add(new Vector3(1.4, 1.2, 1.5));
    camera.zoom = Math.min(size.width / (armPanel ? Math.max(.3, reach * 2.1) : carOnly ? 1.1 : Math.max(2.8, reach * 2.1)), size.height / (armPanel ? Math.max(.3, reach * 1.4) : carOnly ? .9 : Math.max(2, reach * 1.4)));
    camera.lookAt(target); camera.updateProjectionMatrix(); invalidate();
  }, [camera, size.width, size.height, view, focus, armPanel, carOnly, invalidate, modelKey, reach]);
  return null;
}
function Workspace({ accent, error, standalone }: { accent: string; error: string; standalone: boolean }) {
  const modelKey = `${robotModel().id}:${robotModel().revision}`;
  const points = useMemo(() => workspacePoints(standalone ? 'standalone' : 'mounted'), [standalone, modelKey]);
  const ground = useMemo(() => standalone ? new Float32Array() : groundWorkspacePoints(), [standalone, modelKey]);
  return <>
    {!standalone && <points><bufferGeometry><bufferAttribute attach="attributes-position" args={[ground, 3]} /></bufferGeometry>
      <pointsMaterial size={.014} color={accent} transparent opacity={.65} depthWrite={false} />
    </points>}
    <points><bufferGeometry><bufferAttribute attach="attributes-position" args={[points.reachable, 3]} /></bufferGeometry>
      <pointsMaterial size={.008} color={accent} transparent opacity={standalone ? .25 : .12} depthWrite={false} />
    </points>
    <points><bufferGeometry><bufferAttribute attach="attributes-position" args={[points.excluded, 3]} /></bufferGeometry>
      <pointsMaterial size={.006} color={error} transparent opacity={.35} depthWrite={false} />
    </points>
  </>;
}
function ArmRig({ accent, metal, text, standalone, onInteracting }: {
  accent: string; metal: string; text: string; standalone: boolean; onInteracting: (active: boolean) => void;
}) {
  const { state, update } = useCollector(), model = robotModel();
  const reportIssue = useCallback((issue: string | null) => { update(s => s.rigIssue === issue ? s : { ...halt(s), rigIssue: issue }); }, [update]);
  useEffect(() => { if (model.visual.kind === 'procedural') reportIssue(null); }, [model, reportIssue]);
  const drag = useRef<{ pointer: number; index: number; x: number; y: number; angle: number } | null>(null);
  const end = () => { drag.current = null; onInteracting(false); };
  useEffect(() => {
    window.addEventListener('blur', end);
    return () => { window.removeEventListener('blur', end); onInteracting(false); };
  }, [onInteracting]);
  const start = (index: number, event: ThreeEvent<PointerEvent>) => {
    if (event.button !== 0 && event.button !== 2) return;
    if (state.rigIssue || modelProblems(model).length || state.running || state.scanning || state.jointTarget || Object.values(state.wheels).some(v => v !== 0)) return;
    event.stopPropagation(); event.nativeEvent.preventDefault();
    (event.target as Element).setPointerCapture(event.pointerId);
    drag.current = { pointer: event.pointerId, index, x: event.clientX, y: event.clientY, angle: state.joints[index] };
    onInteracting(true); update(s => ({ ...s, selected: `j${index}`, selectedJoint: index }));
  };
  const move = (event: ThreeEvent<PointerEvent>) => {
    const gesture = drag.current;
    if (!gesture || event.pointerId !== gesture.pointer) return;
    event.stopPropagation();
    const angle = gesture.angle + ((event.clientX - gesture.x) - (event.clientY - gesture.y) * .65) * .45;
    update(s => setJoint(s, gesture.index, angle));
  };
  const release = (event: ThreeEvent<PointerEvent>) => {
    if (!drag.current || event.pointerId !== drag.current.pointer) return;
    event.stopPropagation();
    const target = event.target as Element;
    if (target.hasPointerCapture(event.pointerId)) target.releasePointerCapture(event.pointerId);
    end();
  };
  const environment = standalone ? 'standalone' : 'mounted';
  const { frames, tcp } = forwardArm(state.joints, standalone ? STANDALONE_MOUNT : ARM_MOUNT);
  return <group matrix={CAD_TO_THREE} matrixAutoUpdate={false} onClick={event => { event.stopPropagation(); update(s => ({ ...s, selected: 'arm' })); }}>
    {model.visual.kind === 'gltf' && model.visual.url
      ? <ArticulatedArmAsset model={model} q={state.joints} opening={state.opening} environment={environment} onIssue={reportIssue} />
      : <group matrix={rigid(model.mounts[environment])} matrixAutoUpdate={false}>
        <mesh position={[0, 0, model.visual.baseZ]}><boxGeometry args={model.visual.baseSize} /><meshStandardMaterial color={metal} roughness={.6} /></mesh>
        <ArmLink offset={[0, 0, model.joints.slice(0, 2).reduce((sum, j) => sum + j.origin.position[2], 0)]} radius={model.collision.towerRadius} color={metal} />
      </group>}
    {frames.map((matrix, index) => <group key={model.joints[index].id} matrix={matrix} matrixAutoUpdate={false}
      onClick={event => { event.stopPropagation(); update(s => ({ ...s, selected: `j${index}`, selectedJoint: index })); }}
      onPointerDown={event => start(index, event)} onPointerMove={move} onPointerUp={release} onPointerCancel={end} onLostPointerCapture={end}>
      <mesh><sphereGeometry args={[model.joints[index].radius * 1.4, 16, 12]} /><meshStandardMaterial color={accent} transparent={model.visual.kind === 'gltf'} opacity={model.visual.kind === 'gltf' ? .35 : 1} roughness={.35} /></mesh>
      {state.selectedJoint === index && <mesh quaternion={new Quaternion().setFromUnitVectors(new Vector3(0, 0, 1), new Vector3(...model.joints[index].axis))}>
        <torusGeometry args={[.036, .003, 8, 48]} /><meshBasicMaterial color={accent} depthTest={false} /></mesh>}
      {model.visual.kind === 'procedural' && <ArmLink offset={model.joints[index + 1]?.origin.position ?? model.tcp.position} radius={model.joints[index].radius} color={metal} />}
    </group>)}
    <group matrix={frames[frames.length - 1]} matrixAutoUpdate={false} onClick={event => { event.stopPropagation(); update(s => ({ ...s, selected: 'gripper' })); }}>
      {model.visual.kind === 'procedural' && <ProceduralGripper model={model} opening={state.opening} color={accent} />}
    </group>
    {state.references && frames.map((frame, index) => <group key={index} position={new Vector3().setFromMatrixPosition(frame)}>
      <group matrix={CAD_TO_THREE.clone().invert()} matrixAutoUpdate={false}><Label text={model.joints[index].id.toUpperCase()} position={[.035, .025, 0]} color={text} /></group>
    </group>)}
    <mesh position={tcp}><sphereGeometry args={[.007, 12, 8]} /><meshBasicMaterial color={accent} /></mesh>
  </group>;
}
function SavedPointsOverlay({ accent, text, standalone }: { accent: string; text: string; standalone: boolean }) {
  const library = useArmPoints(), model = robotModel();
  const { update } = useCollector();
  const mount = model.mounts[standalone ? 'standalone' : 'mounted'];
  const plan = library.displayPlan;
  const ghost = plan ? forwardModel(plan.solution, model, mount) : null;
  return <group matrix={CAD_TO_THREE} matrixAutoUpdate={false}>
    <group matrix={rigid(mount)} matrixAutoUpdate={false}>
      {library.showPoints && library.points.map(point => <group key={point.id} position={point.position}
        onClick={event => { event.stopPropagation(); library.setSelectedId(point.id); update(s => ({ ...s, selected: `point:${point.id}` })); }}>
        <mesh><sphereGeometry args={[point.id === library.selectedId ? .012 : .008, 12, 8]} /><meshBasicMaterial color={point.id === library.selectedId ? accent : text} /></mesh>
        <group matrix={CAD_TO_THREE.clone().invert()} matrixAutoUpdate={false}><Label text={`${point.name}${point.source.frameSignature !== frameSignature() ? ' · revisar marco' : ''}`} position={[0, .035, 0]} color={text} /></group>
      </group>)}
      {plan && <Stroke points={plan.samples.flatMap((p, i) => i ? [plan.samples[i - 1], p] : [])} color={accent} />}
    </group>
    {ghost && ghost.frames.map((frame, i) => <group key={model.joints[i].id} matrix={frame} matrixAutoUpdate={false}>
      <ArmLink offset={model.joints[i + 1]?.origin.position ?? model.tcp.position} radius={model.joints[i].radius * 1.2} color={accent} ghost />
      {i === ghost.frames.length - 1 && <ProceduralGripper model={model} opening={plan?.finishOpening ?? plan?.opening ?? model.gripper.open} color={accent} ghost />}
    </group>)}
  </group>;
}
function RobotReferences({ accent, text }: { accent: string; text: string }) {
  const { update } = useCollector();
  const { scene, offset } = useRobotModel();
  const anchors = useMemo(() => getComponentAnchors(scene, offset), [scene, offset]);
  return <>{Array.from(anchors).map(([id, point], i) => <group key={id} position={point}
    onClick={event => { event.stopPropagation(); update(s => ({ ...s, selected: id })); }}>
    <mesh><sphereGeometry args={[.009, 12, 8]} /><meshBasicMaterial color={accent} depthTest={false} /></mesh>
    <Label text={String(i + 1).padStart(2, '0')} position={[.02, .025, 0]} color={text} />
  </group>)}</>;
}
function Basket({ accent, surface }: { accent: string; surface: string }) {
  return <><mesh position={toThree([.085, -.10, .105])}>
    <boxGeometry args={[.055, .012, .23]} /><meshStandardMaterial color={surface} roughness={.8} />
  </mesh><group position={toThree(BIN.center)}>
    <mesh><boxGeometry args={[BIN.width, .006, BIN.length]} /><meshStandardMaterial color={surface} roughness={.8} /></mesh>
    {[-1, 1].map(side => <group key={side}>
      <mesh position={[side * BIN.width / 2, .03, 0]}><boxGeometry args={[.004, .06, BIN.length]} /><meshStandardMaterial color={accent} transparent opacity={.3} /></mesh>
      <mesh position={[0, .03, side * BIN.length / 2]}><boxGeometry args={[BIN.width, .06, .004]} /><meshStandardMaterial color={accent} transparent opacity={.3} /></mesh>
    </group>)}
    <mesh position={[0, .015, 0]}><boxGeometry args={[.002, .03, BIN.length]} /><meshStandardMaterial color={accent} /></mesh>
    <mesh position={[0, .015, 0]}><boxGeometry args={[BIN.width, .03, .002]} /><meshStandardMaterial color={accent} /></mesh>
  </group></>;
}

function SceneContents({ view, tool, focus, armPanel, carOnly = false }: SceneProps) {
  const { state, update } = useCollector();
  const { resolvedTheme } = useTheme();
  const [palette, setPalette] = useState(readPalette);
  useEffect(() => {
    const frame = requestAnimationFrame(() => setPalette(readPalette()));
    return () => cancelAnimationFrame(frame);
  }, [resolvedTheme]);
  const [dragging, setDragging] = useState(false);
  const [jointDragging, setJointDragging] = useState(false);
  const drag = useRef<{ dx: number; dy: number } | null>(null);
  const plane = useMemo(() => new Plane(new Vector3(0, 1, 0), 0), []);
  const groundPoint = (event: ThreeEvent<PointerEvent>) => event.ray.intersectPlane(plane, new Vector3());
  const endDrag = () => { drag.current = null; setDragging(false); };
  useEffect(() => { if (tool !== 'move') endDrag(); }, [tool]);
  useEffect(() => {
    window.addEventListener('blur', endDrag);
    return () => window.removeEventListener('blur', endDrag);
  }, []);
  const sensorOrigin = toThree(localToWorld([.103, 0, state.sensorHeight], state.pose));
  const sensorEnd = toThree(localToWorld([.103 + Math.cos(rad(state.sensorAngle)) * (state.reading?.distance ?? .65),
    Math.sin(rad(state.sensorAngle)) * (state.reading?.distance ?? .65), state.sensorHeight], state.pose));
  const heldOrientation = new Quaternion().setFromRotationMatrix(new Matrix4().makeRotationY(state.pose.yaw)
    .multiply(CAD_TO_THREE).multiply(forwardArm(state.joints).tool));
  return <>
    <color attach="background" args={[palette.floor || palette.surface]} />
    <ambientLight intensity={1.4} /><directionalLight position={[3, 5, 2]} intensity={2} />
    <directionalLight position={[-3, 2, -2]} intensity={.7} />
    <gridHelper args={[4, 40, palette.grid, palette.grid]} />
    {!armPanel && !carOnly && <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -.002, 0]} onClick={event => {
      if (tool !== 'place' || state.running || state.scanning || state.jointTarget || event.delta > 5) return;
      const p = event.ray.intersectPlane(plane, new Vector3()); if (!p) return;
      event.stopPropagation();
      update(s => positionCube(s, Math.max(0, ...s.cubes.map(c => c.id)) + 1, [-p.z, -p.x, .025]));
    }}><planeGeometry args={[4, 4]} /><meshBasicMaterial transparent opacity={0} depthWrite={false} /></mesh>}
    <Camera view={view} focus={focus} armPanel={armPanel} carOnly={carOnly} />
    <SimulationOrbit enabled={!dragging && !jointDragging && tool === 'orbit'} focus={focus} />
    <group position={armPanel ? [0, 0, 0] : toThree([state.pose.x, state.pose.y, 0])} rotation={[0, armPanel ? 0 : state.pose.yaw, 0]}>
      {!armPanel && <group
        onClick={event => { event.stopPropagation(); update(s => ({ ...s, selected: 'robot' })); }}
        onPointerDown={event => {
          if (tool !== 'move' || event.button !== 0 || state.running || state.cubes.some(c => c.location === 'gripper')) return;
          const p = groundPoint(event); if (!p) return;
          event.stopPropagation(); (event.target as Element).setPointerCapture(event.pointerId);
          drag.current = { dx: -p.z - state.pose.x, dy: -p.x - state.pose.y };
          setDragging(true); update(s => ({ ...halt(s), selected: 'robot' }));
        }}
        onPointerMove={event => {
          if (!drag.current) return;
          const p = groundPoint(event); if (!p) return;
          event.stopPropagation(); const { dx, dy } = drag.current;
          update(s => moveBase(s, -p.z - dx, -p.x - dy));
        }}
        onPointerUp={event => { if (drag.current) { event.stopPropagation(); (event.target as Element).releasePointerCapture(event.pointerId); endDrag(); } }}
        onPointerCancel={endDrag} onLostPointerCapture={endDrag}
      >
        <SimulationRobot wheelAngles={state.wheelAngles} angle={state.sensorAngle} />
      </group>}
      {!carOnly && <ArmRig accent={palette.accent} metal={palette.metal} text={palette.text} standalone={armPanel} onInteracting={setJointDragging} />}
      {!armPanel && !carOnly && <group onClick={event => { event.stopPropagation(); update(s => ({ ...s, selected: 'bin' })); }}><Basket accent={palette.accent} surface={palette.surface} /></group>}
      {!armPanel && state.selected === 'robot' && <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, .003, 0]}>
        <ringGeometry args={[.18, .184, 64]} /><meshBasicMaterial color={palette.accent} />
      </mesh>}
      {!carOnly && state.workspace && <Workspace accent={palette.accent} error={palette.error} standalone={armPanel} />}
      {!carOnly && <SavedPointsOverlay accent={palette.accent} text={palette.text} standalone={armPanel} />}
      {!armPanel && state.references && <>
        <RobotReferences accent={palette.accent} text={palette.text} />
        <Label text="Deposito · 4 celdas" position={toThree([BIN.center[0], BIN.center[1], .225])} color={palette.text} />
      </>}
    </group>
    {!armPanel && !carOnly && state.cubes.map(cube => {
      const reachable = cubeReach(state, cube).reachable, selected = state.selected === `cube:${cube.id}`;
      return <group key={cube.id} position={toThree(cube.point)} quaternion={cube.location === 'gripper' ? heldOrientation
        : new Quaternion().setFromAxisAngle(new Vector3(0, 1, 0), cube.location === 'bin' ? state.pose.yaw : 0)}
        onClick={event => { event.stopPropagation(); update(s => ({ ...s, selected: `cube:${cube.id}` })); }}>
        <AuxiliaryModelAsset model={CUBE_MODEL} />
        {selected && <mesh rotation={[-Math.PI / 2, 0, 0]} position={[0, -.021, 0]}>
          <ringGeometry args={[.04, .044, 40]} /><meshBasicMaterial color={palette.accent} />
        </mesh>}
        {state.references && <Label text={`C${cube.id}${reachable ? ' · Tomable' : ''}`} position={[0, .055, 0]} color={palette.text} />}
      </group>;
    })}
    {!armPanel && !carOnly && (state.scanning || state.reading) && <Stroke points={[sensorOrigin, sensorEnd]} color={palette.accent} />}
    {!armPanel && !carOnly && state.echoes.filter(e => e.point).map((echo, i) => <mesh key={i} position={toThree(echo.point!)}>
      <sphereGeometry args={[.007, 8, 6]} /><meshBasicMaterial color={palette.accent} transparent opacity={.7} />
    </mesh>)}
  </>;
}
function SceneFailure({ retry }: { retry: () => void }) {
  const { update } = useCollector();
  useEffect(() => { update(s => ({ ...halt(s, 'Visor 3D no disponible.'), rigIssue: 'Visor o rig 3D no disponible.' })); }, [update]);
  return <div className="afr2-collector-loading"><p>No se pudo abrir el modelo 3D.</p><Button onClick={retry}>Reintentar carga</Button></div>;
}
export function CollectorScene(props: SceneProps) {
  const [attempt, setAttempt] = useState(0);
  return <RenderBoundary key={attempt} fallback={<SceneFailure retry={() => {
    // Cache reset is shared with the existing model viewer.
    import('../../components/three/RobotModel').then(m => { m.clearRobotModelCache(); setAttempt(v => v + 1); });
  }} />}>
    <div className="afr2-collector-viewport" data-tool={props.tool}>
      <Canvas orthographic frameloop="demand" dpr={[1, 1.5]} camera={{ near: .001, far: 30, zoom: 300 }} onContextMenu={event => event.preventDefault()}
        fallback={<p>WebGL no esta disponible en este dispositivo.</p>}>
        <Suspense fallback={null}><SceneContents key={props.revision} {...props} /></Suspense>
      </Canvas>
    </div>
  </RenderBoundary>;
}
