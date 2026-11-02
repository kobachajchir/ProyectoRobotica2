import { createContext, useContext, useEffect, useRef, useState, type Dispatch, type ReactNode, type SetStateAction } from 'react';
import { useLocation, useSearchParams } from 'react-router-dom';
import { useOverlays } from '../../app/providers/OverlayContext';
import { useConnection } from '../connection/ConnectionContext';
import { ROBOT_WHEEL_KEYS } from '../../components/three/robotWheelMotion';
import { halt, initialCollector, tickCollector, type CollectorState } from './collectorSimulation';
import { setArmModel } from './armKinematics';
import { robotModel } from './robotModel';
import { ArmPointsContext, useArmPointsController } from './ArmPointsContext';

export type SimulationKind = 'auto' | 'brazo' | 'conjunta';
const CollectorContext = createContext<{ state: CollectorState; update: Dispatch<SetStateAction<CollectorState>>; kind: SimulationKind; changeModel: (id: string) => void } | null>(null);

/** Separate in-memory workspaces for the new Arm and Combined routes. No actuator commands and no
 * persistence of a running mission across reloads. Idle simulation has no timer. */
/** AFR2-20261102-01: el tipo puede venir de la ruta (/simulacion/:tipo); `?tipo=` queda como respaldo. */
export function CollectorProvider({ children, kind: routeKind }: { children: ReactNode; kind?: SimulationKind }) {
  const { pathname } = useLocation();
  const [params] = useSearchParams();
  const requested = routeKind ?? params.get('tipo');
  const kind: SimulationKind = requested === 'auto' ? 'auto' : requested === 'conjunta' ? 'conjunta' : 'brazo';
  const [simulation, updateSimulation] = useState<CollectorState>(() => initialCollector());
  const [arm, updateArm] = useState<CollectorState>(() => ({ ...initialCollector(), cubes: [], environment: 'standalone', selected: 'j0' }));
  const [car, updateCar] = useState<CollectorState>(() => ({ ...initialCollector(), cubes: [], workspace: false, references: false }));
  const state = kind === 'brazo' ? arm : kind === 'auto' ? car : simulation;
  const update = kind === 'brazo' ? updateArm : kind === 'auto' ? updateCar : updateSimulation;
  const points = useArmPointsController(state, update, kind);
  const changeModel = (id: string) => {
    if (id === robotModel().id) return;
    points.cancel(); setArmModel(id);
    updateSimulation(initialCollector());
    updateArm({ ...initialCollector(), cubes: [], environment: 'standalone', selected: 'j0' });
    updateCar({ ...initialCollector(), cubes: [], workspace: false, references: false });
  };
  const { modalOpen } = useOverlays();
  const { stopRecord } = useConnection();
  const lastStop = useRef(stopRecord.lastIssuedAtMs);
  const active = state.running || state.scanning || !!state.jointTarget || ROBOT_WHEEL_KEYS.some(k => state.wheels[k] !== 0);
  useEffect(() => {
    if (!active) return;
    const timer = window.setInterval(() => update(s => tickCollector(s, .04)), 40);
    return () => window.clearInterval(timer);
  }, [active, update]);
  useEffect(() => { update(s => halt(s)); return () => update(s => halt(s)); }, [pathname, update]);
  useEffect(() => { if (modalOpen) update(s => halt(s, 'Simulación detenida al abrir un panel.')); }, [modalOpen, update]);
  useEffect(() => {
    if (lastStop.current !== stopRecord.lastIssuedAtMs) {
      lastStop.current = stopRecord.lastIssuedAtMs;
      update(s => halt(s, 'Simulación detenida por STOP.'));
    }
  }, [stopRecord.lastIssuedAtMs, update]);
  useEffect(() => {
    const stop = () => update(s => halt(s, 'Simulación detenida al perder el foco.'));
    const visibility = () => { if (document.visibilityState !== 'visible') stop(); };
    window.addEventListener('blur', stop);
    document.addEventListener('visibilitychange', visibility);
    return () => { window.removeEventListener('blur', stop); document.removeEventListener('visibilitychange', visibility); };
  }, [update]);
  return <CollectorContext.Provider value={{ state, update, kind, changeModel }}><ArmPointsContext.Provider value={points}>{children}</ArmPointsContext.Provider></CollectorContext.Provider>;
}
export function useCollector() {
  const value = useContext(CollectorContext);
  if (!value) throw new Error('CollectorProvider requerido');
  return value;
}
