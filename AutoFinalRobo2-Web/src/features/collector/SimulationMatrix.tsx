import { useId, useState } from 'react';
import { Matrix4 } from 'three';
import { forwardArm, JOINTS, rad, STANDALONE_MOUNT } from './armKinematics';
import { useCollector } from './CollectorContext';
import { CollapsiblePanel } from '../../components/ui/CollapsiblePanel';
import { HomogeneousMatrix } from '../../components/robot/HomogeneousMatrix';

type Frame = 'joint' | 'tool' | 'world';
const fmt = (value: number) => (Math.abs(value) < .0005 ? 0 : value).toFixed(3);

/** Demonstration of the nominal Web chain, linked to the same simulated pose.
 * Translations are displayed in mm; rotations are dimensionless. */
export function SimulationMatrix({ expanded, onToggle }: { expanded: boolean; onToggle: () => void }) {
  const { state, kind } = useCollector();
  const [frame, setFrame] = useState<Frame>('joint');
  const selectorId = useId();
  const arm = kind === 'brazo', car = kind === 'auto';
  const index = state.selectedJoint;
  if (!JOINTS[index]) return <CollapsiblePanel className="afr2-sim-matrix" title="Matematica en vivo" expanded={expanded} onToggle={onToggle}><p>El descriptor no contiene una cadena articular utilizable.</p></CollapsiblePanel>;
  const chain = forwardArm(state.joints, arm ? STANDALONE_MOUNT : undefined);
  const world = new Matrix4().makeTranslation(state.pose.x, state.pose.y, 0).multiply(new Matrix4().makeRotationZ(state.pose.yaw));
  const active = car ? 'world' : frame;
  let matrix: Matrix4, name: string;
  if (active === 'world') { matrix = world; name = 'T mundo → chasis'; }
  else if (active === 'tool') { matrix = arm ? chain.tool : world.clone().multiply(chain.tool); name = arm ? 'T base → pinza' : 'T mundo → pinza'; }
  else {
    matrix = index === 0 ? chain.frames[0] : chain.frames[index - 1].clone().invert().multiply(chain.frames[index]);
    name = `T ${index === 0 ? 'base' : `J${index}`} → J${index + 1}`;
  }
  const angle = car || active === 'world' ? state.pose.yaw : rad(state.joints[index]);
  const angleName = car || active === 'world' ? 'ψ' : `θ${index + 1}`;
  const joint = JOINTS[index];
  const axis = joint.axis[1] === 1 ? 'Y' : joint.axis[2] === 1 ? 'Z' : 'other';
  const symbols = active === 'joint' && axis !== 'other' && joint.sign === 1 && joint.zero === 0
    ? axis === 'Y' ? ['cos θ', '0', 'sin θ', 'tx', '0', '1', '0', 'ty', '−sin θ', '0', 'cos θ', 'tz', '0', '0', '0', '1']
      : ['cos θ', '−sin θ', '0', 'tx', 'sin θ', 'cos θ', '0', 'ty', '0', '0', '1', 'tz', '0', '0', '0', '1']
    : active === 'world' ? ['cos ψ', '−sin ψ', '0', 'x', 'sin ψ', 'cos ψ', '0', 'y', '0', '0', '1', 'z', '0', '0', '0', '1']
      : ['r11', 'r12', 'r13', 'px', 'r21', 'r22', 'r23', 'py', 'r31', 'r32', 'r33', 'pz', '0', '0', '0', '1'];
  return <CollapsiblePanel className="afr2-sim-matrix" title="Matematica en vivo" subtitle="Transformaciones del modelo" meta="Provisional" expanded={expanded} onToggle={onToggle}>
    <div className="afr2-sim-matrix__frame"><label htmlFor={selectorId}>Transformacion a observar</label><select id={selectorId} value={active} onChange={e => setFrame(e.target.value as Frame)}>
      {!car && <><option value="joint">Articulacion {JOINTS[index].label}</option><option value="tool">{arm ? 'Base' : 'Mundo'} → pinza</option></>}
      {!arm && <option value="world">Mundo → chasis</option>}
    </select></div>
    <HomogeneousMatrix matrix={matrix} name={name} symbols={symbols} />
    <div className="afr2-sim-trig"><div><span>{angleName}</span><strong>{(angle * 180 / Math.PI).toFixed(1)}°</strong></div>
      <div><span>cos {angleName}</span><strong>{fmt(Math.cos(angle))}</strong></div><div><span>sin {angleName}</span><strong>{fmt(Math.sin(angle))}</strong></div>
    </div>
    <p>{active === 'tool' ? 'Cadena compuesta · ' : ''}X adelante · Y izquierda · Z arriba · traslacion en mm</p>
    <details><summary>Ver expresion simbolica</summary><div className="afr2-sim-matrix__symbols">{symbols.map((s, i) => <span key={i}>{s}</span>)}</div></details>
  </CollapsiblePanel>;
}
