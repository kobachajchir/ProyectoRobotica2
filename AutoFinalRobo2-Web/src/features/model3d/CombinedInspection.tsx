/**
 * AFR2-20261102-01. Inspección del conjunto: el auto con el brazo V17 montado en la misma posición
 * que usa la simulación conjunta (`robotModel().mounts.mounted`). Reutiliza RobotModel y
 * ArticulatedArmAsset del simulador; el brazo queda en su pose de reposo (restAngles), la misma
 * del GLB, así las referencias calculadas sobre el GLB coinciden con lo que se dibuja.
 */
import { useEffect, useMemo, useState, type ReactNode } from 'react';
import { Canvas, useLoader, useThree } from '@react-three/fiber';
import { Box3, Group, PropertyBinding, Vector3 } from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { DRACOLoader } from 'three/addons/loaders/DRACOLoader.js';
import { frameModel } from '../../components/three/frameModel';
import { InspectionEnvironment, useInspectionPalette } from '../../components/three/InspectionEnvironment';
import { OrbitControls } from '../../components/three/OrbitControls';
import { RobotModel, useRobotModel } from '../../components/three/RobotModel';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import { ArticulatedArmAsset } from '../collector/ArmVisualAdapter';
import { rigid } from '../collector/modelKinematics';
import { CAD_TO_THREE } from '../collector/armKinematics';
import { identity, robotModel } from '../collector/robotModel';
import { ARM_REFERENCES } from './AssetInspection';
import { getComponentAnchors, MODEL_COMPONENTS } from './modelComponents';

const draco = new DRACOLoader().setDecoderPath('/draco/').setDecoderConfig({ type: 'wasm' });

interface Reference { id: string; group: 'Auto' | 'Brazo'; label: string; detail: string; position: Vector3 }

function Camera({ bounds }: { bounds: Box3 }) {
  const { camera, size, invalidate } = useThree();
  useEffect(() => { frameModel(camera, bounds, size); invalidate(); }, [camera, size.width, size.height, bounds, invalidate]);
  return <OrbitControls targetY={bounds.getCenter(new Vector3()).y} />;
}

export function CombinedInspection({ referencesVisible, onToggleReferences, action }: {
  referencesVisible: boolean; onToggleReferences: () => void; action?: ReactNode;
}) {
  const palette = useInspectionPalette();
  const { scene: carScene, offset } = useRobotModel();
  const arm = robotModel();
  const mount = arm.mounts.mounted;
  const rest = useMemo(() => arm.visual.restAngles ?? arm.joints.map(() => 0), [arm]);
  const armUrl = `${arm.visual.url}${arm.visual.url!.includes('?') ? '&' : '?'}v=${encodeURIComponent(arm.revision)}`;
  const armAsset = useLoader(GLTFLoader, armUrl, loader => loader.setDRACOLoader(draco));
  const prepared = useMemo(() => {
    const anchors = getComponentAnchors(carScene, offset);
    const carRefs: Reference[] = MODEL_COMPONENTS.filter(c => anchors.has(c.id)).map(c => ({
      id: `auto:${c.id}`, group: 'Auto', label: c.label, detail: c.description, position: anchors.get(c.id)!.clone(),
    }));
    // Misma cadena que el simulador: CAD_TO_THREE · montaje · (restToMount · GLB en reposo).
    const root = new Group(), armScene = armAsset.scene.clone(true);
    root.add(armScene);
    armScene.applyMatrix4(rigid(arm.visual.restToMount ?? identity()));
    root.applyMatrix4(rigid(mount));
    root.applyMatrix4(CAD_TO_THREE);
    root.updateMatrixWorld(true);
    const armRefs: Reference[] = ARM_REFERENCES.flatMap(ref => {
      const target = armScene.getObjectByName(ref.node) ?? armScene.getObjectByName(PropertyBinding.sanitizeNodeName(ref.node));
      if (!target) return [];
      return [{ id: `brazo:${ref.node}`, group: 'Brazo' as const, label: ref.label, detail: ref.detail, position: new Box3().setFromObject(target).getCenter(new Vector3()) }];
    });
    const carBounds = new Box3().setFromObject(carScene, true);
    carBounds.translate(offset);
    const bounds = carBounds.union(new Box3().setFromObject(root, true));
    const size = bounds.getSize(new Vector3());
    return { references: [...carRefs, ...armRefs], bounds, extent: Math.max(size.x, size.y, size.z, .01) };
  }, [carScene, offset, armAsset.scene, arm, mount]);
  const [selected, setSelected] = useState<string | null>(null);
  const selectedRef = prepared.references.find(ref => ref.id === selected);
  useEffect(() => {
    const close = (event: KeyboardEvent) => { if (event.key === 'Escape') setSelected(null); };
    window.addEventListener('keydown', close); return () => window.removeEventListener('keydown', close);
  }, []);
  return <div className="afr2-model-inspection">
    <div className="afr2-model-inspection__toolbar">
      <div className="afr2-model-inspection__picker">
        <label htmlFor="afr2-inspect-conjunto">Explorar componentes</label>
        <span className="afr2-select-wrap"><select id="afr2-inspect-conjunto" value={selected ?? ''} disabled={!referencesVisible} onChange={event => setSelected(event.target.value || null)}>
          <option value="">Seleccionar componente</option>
          {(['Auto', 'Brazo'] as const).map(group => <optgroup key={group} label={group}>
            {prepared.references.filter(ref => ref.group === group).map(ref => <option key={ref.id} value={ref.id}>{ref.label}</option>)}
          </optgroup>)}
        </select></span>
      </div>
      <Button size="sm" aria-pressed={referencesVisible} onClick={() => { setSelected(null); onToggleReferences(); }}>
        {referencesVisible ? 'Ocultar referencias' : 'Mostrar referencias'}
      </Button>
    </div>
    <div className="afr2-model-inspection__viewport">
      <Canvas frameloop="demand" dpr={[1, 1.5]} camera={{ fov: 45, near: .002, far: 40 }} onPointerMissed={() => setSelected(null)} fallback={<p className="afr2-model-inspection__fallback">WebGL no está disponible.</p>}>
        <InspectionEnvironment palette={palette} extent={prepared.extent} />
        <RobotModel animateServo={false} />
        <group matrix={CAD_TO_THREE} matrixAutoUpdate={false}>
          <ArticulatedArmAsset model={arm} q={rest} opening={arm.visual.restOpening ?? 0} environment="mounted" onIssue={() => undefined} />
        </group>
        {referencesVisible && prepared.references.map(ref => <mesh key={ref.id} position={ref.position} onClick={event => { event.stopPropagation(); setSelected(ref.id); }}>
          <sphereGeometry args={[prepared.extent * (ref.id === selected ? .016 : .012), 12, 8]} /><meshBasicMaterial color={palette.reference} depthTest={false} toneMapped={false} />
        </mesh>)}
        <Camera bounds={prepared.bounds} />
      </Canvas>
      {referencesVisible && selectedRef && <aside className="afr2-model-info" role="region" aria-label={selectedRef.label}>
        <header className="afr2-model-info__heading">
          <span className="afr2-model-info__icon"><UiIcon name={selectedRef.group === 'Auto' ? 'control' : 'arm'} /></span>
          <div><p className="afr2-kicker">{selectedRef.group === 'Auto' ? 'Componente del auto' : 'Componente del brazo'}</p><h2>{selectedRef.label}</h2></div>
          <button type="button" className="afr2-model-info__close afr2-pressable" aria-label="Cerrar ficha del componente" onClick={() => setSelected(null)}><UiIcon name="close" /></button>
        </header>
        <div className="afr2-model-info__body"><p>{selectedRef.detail}</p><p>Escala: 1 unidad = 1 m. Brazo en pose de reposo.</p></div>
      </aside>}
      {action && <div className="afr2-model-inspection__cta">{action}</div>}
    </div>
    <p className="afr2-help afr2-model-inspection__help">Auto con el brazo V17 montado como en la simulación conjunta: {prepared.references.length} referencias. Elegí un punto o un componente de la lista.</p>
  </div>;
}
