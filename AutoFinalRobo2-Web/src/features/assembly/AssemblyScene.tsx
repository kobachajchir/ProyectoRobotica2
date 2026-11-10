import { Suspense, useEffect, useLayoutEffect, useMemo, useState, type ReactNode } from 'react';
import { Canvas, useLoader, useThree } from '@react-three/fiber';
import { Box3, Matrix4, OrthographicCamera, PMREMGenerator, Vector3 } from 'three';
import { RoomEnvironment } from 'three/addons/environments/RoomEnvironment.js';
import { DRACOLoader } from 'three/addons/loaders/DRACOLoader.js';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { useTheme } from '../../app/providers/ThemeProvider';
import { useRobotModel } from '../../components/three/RobotModel';
import { ProgressGLTFLoader } from '../../components/three/modelAssetLoading';
import { modelAssetUrl } from '../../components/three/modelAssets';
import { frameModel } from '../../components/three/frameModel';
import { CUBE_MODEL } from '../collector/auxiliaryModels';
import { prepareAssemblyCar, poseAssemblyCar } from './assemblyCar';
import { ARM_TO_SCENE, prepareAssemblyArm, poseAssemblyArm, toolInScene } from './assemblyArm';
import { ADDITIONS_FROM, ADDITIONS_TO, ASSEMBLY_CHAPTERS, assemblyPose, chapterIndex, CONTACT_PROGRESS, FINAL_CLOSEUP_FROM, HOLD_PROGRESS, ease, mechanicalProgress, segment } from './assemblyTimeline';
import { disposeRevealMaterials, revealMaterials, revealOpacity } from './assemblyMaterials';
import { ADDITIONS_ASSET_URL, ADDITIONS_VIEW_DIRECTION, disposeAdditions, poseAdditions, prepareAdditions } from './assemblyAdditions';
import { AssemblyCollectionEnvironment } from './AssemblyCollectionEnvironment';
import { interfaceReveal } from './AssemblyInterfaceShowcase';

export const ASSEMBLY_ARM_URL = modelAssetUrl('brazo');
const draco = new DRACOLoader().setDecoderPath('/draco/').setDecoderConfig({ type: 'wasm' });
const CONTACT_TOOL = toolInScene(assemblyPose(CONTACT_PROGRESS).q);
// Keep cube faces aligned with the jaws, with its up axis opposite the tool Z.
const BOX_REST = CONTACT_TOOL.clone().multiply(new Matrix4().makeRotationX(-Math.PI / 2));
const BOX_IN_TOOL = CONTACT_TOOL.clone().invert().multiply(BOX_REST);

function Models({ progress, reduced, onReady }: { progress: number; reduced: boolean; onReady: () => void }) {
  const { scene, offset } = useRobotModel();
  const armAsset = useLoader(ProgressGLTFLoader, ASSEMBLY_ARM_URL, loader => loader.setDRACOLoader(draco));
  const cubeAsset = useLoader(GLTFLoader, CUBE_MODEL.url);
  const additionsAsset = useLoader(ProgressGLTFLoader, ADDITIONS_ASSET_URL, loader => loader.setDRACOLoader(draco));
  const car = useMemo(() => prepareAssemblyCar(scene), [scene]);
  const arm = useMemo(() => prepareAssemblyArm(armAsset.scene), [armAsset.scene]);
  const additions = useMemo(() => prepareAdditions(scene, additionsAsset.scene), [scene, additionsAsset.scene]);
  const cube = useMemo(() => {
    const root = cubeAsset.scene.clone(true);
    return { root, materials: revealMaterials(root) };
  }, [cubeAsset.scene]);
  const { camera, size, invalidate } = useThree();
  const bounds = useMemo(() => {
    poseAssemblyCar(car, 1, true);
    const assembled = assemblyPose(.87);
    poseAssemblyArm(arm, assembled.q, assembled.opening, 1, true);
    poseAdditions(additions, 1, false);
    const base = new Box3().setFromObject(car.model, true).translate(offset);
    base.union(new Box3().setFromObject(additions.root, true).translate(offset));
    const whole = base.clone().union(new Box3().setFromObject(arm.root, true).applyMatrix4(ARM_TO_SCENE));
    whole.expandByPoint(new Vector3().setFromMatrixPosition(BOX_REST).add(new Vector3(.04, .08, .04)));
    return { base, whole };
  }, [car, arm, additions, offset]);
  useEffect(() => () => {
    car.geometries.forEach(geometry => geometry.dispose());
    car.parts.forEach(part => disposeRevealMaterials(part.materials));
  }, [car]);
  useEffect(() => () => arm.bindings.forEach(binding => disposeRevealMaterials(binding.materials)), [arm]);
  useEffect(() => () => disposeRevealMaterials(cube.materials), [cube]);
  useEffect(() => () => disposeAdditions(additions), [additions]);
  useEffect(onReady, [onReady]);
  useLayoutEffect(() => {
    const selected = chapterIndex(progress), nextChapter = ASSEMBLY_CHAPTERS[selected + 1];
    const at = reduced ? (ASSEMBLY_CHAPTERS[selected].code === 'C600' ? .46 : ASSEMBLY_CHAPTERS[selected].code === 'SENSOR A5' ? (ADDITIONS_FROM + ADDITIONS_TO) / 2 : nextChapter ? nextChapter.at - .001 : 1) : progress;
    const clock = mechanicalProgress(at), pose = assemblyPose(clock);
    const feature = poseAdditions(additions, at, reduced);
    poseAssemblyCar(car, clock, reduced, 1 - feature.isolation);
    poseAssemblyArm(arm, pose.q, pose.opening, clock, reduced);
    const reveal = reduced ? 1 : ease(segment(clock, .89, .925));
    cube.root.visible = clock >= .89;
    revealOpacity(cube.materials, reveal);
    cube.root.matrixAutoUpdate = false;
    const lift = clock >= HOLD_PROGRESS;
    cube.root.matrix.copy(lift ? toolInScene(pose.q).multiply(BOX_IN_TOOL) : BOX_REST);
    if (!reduced) cube.root.matrix.premultiply(new Matrix4().makeTranslation(0, .08 * (1 - reveal), 0));
    cube.root.matrixWorldNeedsUpdate = true;

    // Frame the growing assembly; show incoming pieces entering from above.
    // All camera state derives from progress, including reverse scroll and jumps.
    const growing = reduced ? 1 : ease(segment(clock, .58, .87));
    const framing = new Box3(bounds.base.min.clone().lerp(bounds.whole.min, growing), bounds.base.max.clone().lerp(bounds.whole.max, growing));
    framing.max.y += .035;
    framing.expandByScalar(.018);
    const closeup = reduced ? 0 : ease(segment(at, FINAL_CLOSEUP_FROM, 1));
    const interfaces = interfaceReveal(progress);
    if (closeup > 0) {
      const tight = new Box3().setFromObject(car.model, true).union(new Box3().setFromObject(arm.root, true)).union(new Box3().setFromObject(additions.root, true));
      if (cube.root.visible) tight.union(new Box3().setFromObject(cube.root, true));
      tight.expandByScalar(.006);
      framing.min.lerp(tight.min, closeup); framing.max.lerp(tight.max, closeup);
    }
    framing.min.lerp(feature.macroBounds.min, feature.isolation);
    framing.max.lerp(feature.macroBounds.max, feature.isolation);
    const orbit = reduced ? 1 : ease(segment(clock, .87, .945));
    const drift = reduced ? .15 : .15 * ease(segment(clock, 0, .87));
    const angle = -.82 + drift + orbit * (Math.PI * 2 + .1);
    const direction = new Vector3(Math.sin(angle), .62 - growing * .14, -Math.cos(angle)).normalize().lerp(ADDITIONS_VIEW_DIRECTION, feature.isolation).normalize();
    const center = frameModel(camera, framing, size, 'perspective', direction.toArray());
    // Leave negative space for the editorial copy, while the product moves to
    // the opposite side. Blend at chapter boundaries; reversing is continuous.
    if (center && camera instanceof OrthographicCamera && size.width >= 760) {
      let side = -1;
      for (let i = 1; i < ASSEMBLY_CHAPTERS.length; i++) {
        const previous = ASSEMBLY_CHAPTERS[i - 1].side === 'left' ? -1 : 1;
        const next = ASSEMBLY_CHAPTERS[i].side === 'left' ? -1 : 1;
        side += (next - previous) * ease(segment(progress, ASSEMBLY_CHAPTERS[i].at - .012, ASSEMBLY_CHAPTERS[i].at + .012));
      }
      if (reduced) side = -1;
      side += (.4 - side) * interfaces;
      camera.zoom *= .86 + .14 * closeup; camera.updateProjectionMatrix();
      const shift = new Vector3().setFromMatrixColumn(camera.matrixWorld, 0).multiplyScalar(side * size.width / camera.zoom * .13 * (1 - closeup));
      camera.position.add(shift); camera.lookAt(center.add(shift)); camera.updateMatrixWorld(true);
    }
    if (center && camera instanceof OrthographicCamera && interfaces > 0) {
      camera.zoom *= 1 - .18 * interfaces; camera.updateProjectionMatrix();
      if (size.width < 760) {
        const shift = new Vector3().setFromMatrixColumn(camera.matrixWorld, 0).multiplyScalar(size.width / camera.zoom * .15 * interfaces);
        camera.position.add(shift); camera.lookAt(center.add(shift)); camera.updateMatrixWorld(true);
      }
    }
    if (center && camera instanceof OrthographicCamera && lift) {
      // During the final lift, bring the held box to the vertical midpoint of
      // the image. Keep the complete assembly within the new framing.
      cube.root.updateWorldMatrix(true, false);
      const up = new Vector3().setFromMatrixColumn(camera.matrixWorld, 1);
      const boxCenter = new Vector3().setFromMatrixPosition(cube.root.matrixWorld);
      const amount = reduced ? 1 : ease(segment(clock, HOLD_PROGRESS, 1));
      const shift = up.clone().multiplyScalar(boxCenter.sub(center).dot(up) * amount);
      camera.position.add(shift); center.add(shift); camera.lookAt(center);
      let halfHeight = 0;
      for (const x of [framing.min.x, framing.max.x]) for (const y of [framing.min.y, framing.max.y]) for (const z of [framing.min.z, framing.max.z]) {
        halfHeight = Math.max(halfHeight, Math.abs(new Vector3(x, y, z).sub(center).dot(up)));
      }
      camera.zoom = Math.min(camera.zoom, size.height / (2 * Math.max(.001, halfHeight) * 1.08));
      camera.updateProjectionMatrix(); camera.updateMatrixWorld(true);
    }
    invalidate();
  }, [progress, reduced, car, arm, cube, additions, camera, size.width, size.height, bounds, invalidate]);
  return <><group dispose={null}>
    <group position={offset}><primitive object={car.model} /></group>
    <group position={offset}><primitive object={additions.root} /></group>
    <group matrix={ARM_TO_SCENE} matrixAutoUpdate={false}><primitive object={arm.root} /></group>
    <primitive object={cube.root} />
  </group>
    <AssemblyCollectionEnvironment source={cubeAsset.scene} clock={mechanicalProgress(progress)} reduced={reduced}
      floorY={bounds.base.min.y} target={new Vector3().setFromMatrixPosition(BOX_REST)} />
  </>;
}

export function AssemblyScene({ progress, reduced, onReady, fallback }: {
  progress: number; reduced: boolean; onReady: () => void; fallback: ReactNode;
}) {
  return <Canvas orthographic frameloop="demand" dpr={[1, 1.75]} gl={{ alpha: true, antialias: true }}
    className="afr2-assembly__canvas" camera={{ near: .001, far: 40, zoom: 300 }}
    onCreated={({ gl }) => gl.setClearColor(0x000000, 0)}
    fallback={<Unavailable onReady={onReady}>{fallback}</Unavailable>}>
    <Studio />
    <Suspense fallback={null}><Models progress={progress} reduced={reduced} onReady={onReady} /></Suspense>
  </Canvas>;
}

/** Procedural reflection room: no panorama downloads, visible walls or floor. */
function Studio() {
  const { gl, scene, invalidate } = useThree();
  const { resolvedTheme } = useTheme();
  const readPalette = () => {
    const css = getComputedStyle(document.documentElement);
    return { cool: css.getPropertyValue('--infra-hero-zone-pc').trim(), warm: css.getPropertyValue('--infra-hero-zone-link').trim() };
  };
  const [palette, setPalette] = useState(readPalette);
  useEffect(() => {
    const frame = requestAnimationFrame(() => { setPalette(readPalette()); invalidate(); });
    return () => cancelAnimationFrame(frame);
  }, [resolvedTheme, invalidate]);
  useEffect(() => {
    const room = new RoomEnvironment(), pmrem = new PMREMGenerator(gl);
    const reflection = pmrem.fromScene(room, .04);
    const previous = scene.environment, intensity = scene.environmentIntensity;
    scene.environment = reflection.texture; scene.environmentIntensity = .75;
    room.dispose(); pmrem.dispose(); invalidate();
    return () => { scene.environment = previous; scene.environmentIntensity = intensity; reflection.dispose(); };
  }, [gl, scene, invalidate]);
  return <>
    <ambientLight intensity={.55} />
    <directionalLight position={[-2, 4, 3]} intensity={3.2} />
    <directionalLight position={[2, 1, -3]} intensity={2} color={palette.cool} />
    <directionalLight position={[-3, .5, -1]} intensity={1.2} color={palette.warm} />
  </>;
}

function Unavailable({ children, onReady }: { children: ReactNode; onReady: () => void }) {
  useEffect(onReady, [onReady]);
  return children;
}
