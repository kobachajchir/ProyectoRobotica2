import { useEffect, useMemo, useState } from 'react';
import { Canvas, useLoader, useThree } from '@react-three/fiber';
import { Box3, PropertyBinding, Vector3 } from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { DRACOLoader } from 'three/addons/loaders/DRACOLoader.js';
import { Link } from 'react-router-dom';
import { OrbitControls } from '../../components/three/OrbitControls';
import { MODEL_ASSETS, modelAssetUrl } from '../../components/three/modelAssets';
import { Button } from '../../components/ui/Button';

const draco = new DRACOLoader().setDecoderPath('/draco/').setDecoderConfig({ type: 'wasm' });
const ARM_REFERENCES = [
  { node: 'base-fixed', label: 'Montura y control', detail: 'Montura baja, fijaciones y electronica del brazo.' },
  { node: 'j1', label: 'J1 · Base', detail: 'Plataforma giratoria y torre del hombro.' },
  { node: 'j2', label: 'J2 · Brazo', detail: 'Primer eslabon y motor remoto del codo junto al hombro.' },
  { node: 'j3', label: 'J3 · Antebrazo', detail: 'Segundo eslabon, polea conducida y soporte distal.' },
  { node: 'j4', label: 'J4 · Giro de muneca', detail: 'Giro axial de la muneca.' },
  { node: 'j5', label: 'J5 · Inclinacion', detail: 'Horquilla, actuador y apoyos de inclinacion.' },
  { node: 'j6', label: 'J6 · Herramienta', detail: 'Palma y giro de la herramienta.' },
  { node: 'jaw-left', label: 'Mordaza izquierda', detail: 'Corredera y superficie de agarre de la pinza.' },
  { node: 'jaw-right', label: 'Mordaza derecha', detail: 'Corredera opuesta de la pinza.' },
  { node: 'drive-j2', label: 'Transmision del hombro', detail: 'Pinon de entrada de la reduccion 18/72.' },
  { node: 'drive-j3', label: 'Entrada de correa del codo', detail: 'Polea motriz y eje de la transmision 20/50.' },
  { node: 'drive-j4', label: 'Transmision de muneca', detail: 'Pinon del giro axial con transmision 24/24.' },
  { node: 'drive-grip', label: 'Transmision de pinza', detail: 'Pinon que acciona las dos cremalleras.' },
];
const CUBE_REFERENCES = [{ node: 'AFR2_Auxiliar_Cubo50', label: 'Cubo de referencia', detail: 'Pieza auxiliar de 50 × 50 × 50 mm usada en el simulador.' }];

export function clearInspectionAssetCache() {
  useLoader.clear(GLTFLoader, modelAssetUrl('brazo'));
  useLoader.clear(GLTFLoader, modelAssetUrl('cubo'));
}

function Camera({ extent, targetY }: { extent: number; targetY: number }) {
  const { camera, size, invalidate } = useThree();
  useEffect(() => {
    const distance = extent * 1.7 / Math.min(1, size.width / size.height);
    camera.position.set(distance, targetY + distance * .65, distance);
    camera.lookAt(0, targetY, 0); camera.updateProjectionMatrix(); invalidate();
  }, [camera, size.width, size.height, extent, targetY, invalidate]);
  return <OrbitControls targetY={targetY} />;
}

/** Static inspection uses the same GLB as the simulator, retaining PBR materials. */
export function AssetInspection({ model, referencesVisible, onToggleReferences }: {
  model: 'brazo' | 'cubo'; referencesVisible: boolean; onToggleReferences: () => void;
}) {
  const asset = useLoader(GLTFLoader, modelAssetUrl(model), loader => loader.setDRACOLoader(draco));
  const prepared = useMemo(() => {
    const scene = asset.scene.clone(true); scene.updateMatrixWorld(true);
    const bounds = new Box3().setFromObject(scene), size = bounds.getSize(new Vector3()), center = bounds.getCenter(new Vector3());
    const offset = new Vector3(-center.x, -bounds.min.y, -center.z);
    const definitions = model === 'brazo' ? ARM_REFERENCES : CUBE_REFERENCES;
    const references = definitions.flatMap(ref => {
      const target = scene.getObjectByName(ref.node) ?? scene.getObjectByName(PropertyBinding.sanitizeNodeName(ref.node));
      if (!target) return [];
      const position = new Box3().setFromObject(target).getCenter(new Vector3()).add(offset);
      return [{ ...ref, position }];
    });
    return { scene, size, offset, references, extent: Math.max(size.x, size.y, size.z, .01) };
  }, [asset.scene, model]);
  const [selected, setSelected] = useState<string | null>(null);
  const selectedRef = prepared.references.find(ref => ref.node === selected);
  useEffect(() => {
    const close = (event: KeyboardEvent) => { if (event.key === 'Escape') setSelected(null); };
    window.addEventListener('keydown', close); return () => window.removeEventListener('keydown', close);
  }, []);
  return <div className="afr2-model-inspection">
    <div className="afr2-model-inspection__toolbar">
      <div className="afr2-model-inspection__picker">
        <label htmlFor={`afr2-inspect-${model}`}>Explorar componentes</label>
        <span className="afr2-select-wrap"><select id={`afr2-inspect-${model}`} value={selected ?? ''} disabled={!referencesVisible} onChange={event => setSelected(event.target.value || null)}>
          <option value="">Seleccionar componente</option>
          {prepared.references.map(ref => <option key={ref.node} value={ref.node}>{ref.label}</option>)}
        </select></span>
      </div>
      <Button size="sm" aria-pressed={referencesVisible} onClick={() => { setSelected(null); onToggleReferences(); }}>
        {referencesVisible ? 'Ocultar referencias' : 'Mostrar referencias'}
      </Button>
    </div>
    <div className="afr2-model-inspection__viewport">
      <Canvas frameloop="demand" dpr={[1, 1.5]} camera={{ fov: 45, near: .001, far: 20 }} onPointerMissed={() => setSelected(null)} fallback={<p>WebGL no esta disponible.</p>}>
        <ambientLight intensity={.9} /><directionalLight position={[4, 6, 4]} intensity={1.4} /><directionalLight position={[-4, -2, -4]} intensity={.5} />
        <gridHelper args={[prepared.extent * 3, 12, '#b81d33', '#1e293b']} />
        <group position={prepared.offset} onClick={event => {
          let target = event.object;
          while (target) {
            const ref = prepared.references.find(item => target.name === item.node || target.name === PropertyBinding.sanitizeNodeName(item.node));
            if (ref && referencesVisible) { event.stopPropagation(); setSelected(ref.node); return; }
            if (!target.parent) break; target = target.parent;
          }
        }}><primitive object={prepared.scene} dispose={null} /></group>
        {referencesVisible && prepared.references.map(ref => <mesh key={ref.node} position={ref.position} onClick={event => { event.stopPropagation(); setSelected(ref.node); }}>
          <sphereGeometry args={[prepared.extent * .018, 12, 8]} /><meshBasicMaterial color={ref.node === selected ? '#fbbf24' : '#e11d48'} depthTest={false} />
        </mesh>)}
        <Camera extent={prepared.extent} targetY={prepared.size.y / 2} />
      </Canvas>
      {referencesVisible && selectedRef && <aside className="afr2-model-info" role="region" aria-label={selectedRef.label}>
        <header className="afr2-model-info__header"><h3>{selectedRef.label}</h3><Button size="sm" onClick={() => setSelected(null)}>Cerrar</Button></header>
        <div className="afr2-model-info__body"><p>{selectedRef.detail}</p><p>Escala: 1 unidad = 1 m.</p></div>
        <footer><Link to="/simulaciones?tipo=brazo">Abrir simulador del brazo</Link></footer>
      </aside>}
    </div>
    <p className="afr2-help afr2-model-inspection__help">{MODEL_ASSETS[model].label}: {prepared.references.length} referencias. Selecciona un punto o un componente de la lista.</p>
  </div>;
}
