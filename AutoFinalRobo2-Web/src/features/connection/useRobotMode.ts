/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { useEffect, useState } from 'react';
import { useConnection } from './ConnectionContext';
import { stateAge, type RobotMode } from '../../protocol/robotState';

const LABELS: Record<RobotMode, string> = { idle: 'Reposo', manual: 'Manual', 'manual-pwm':'Manual PWM', 'line-follow': 'Seguimiento de línea', 'wheel-test': 'Prueba de rueda', goto: 'GOTO', sequence: 'Secuencia', fault: 'Fallo', unknown: 'Sin datos' };

/** Shares the same authoritative snapshot as Control; never issues a second poll. */
export function useRobotMode(): string {
  const { robotState, wsState, megaLinkState } = useConnection();
  const [now, setNow] = useState(Date.now());
  useEffect(() => {
    setNow(Date.now());
    const age = stateAge(robotState, Date.now());
    if (age === null || age >= 6000) return;
    const timer = window.setTimeout(() => setNow(Date.now()), 6000 - age);
    return () => clearTimeout(timer);
  }, [robotState]);
  const age = stateAge(robotState, Math.max(now, Date.now()));
  return wsState === 'ready' && megaLinkState === 'ready' && robotState && age !== null && age < 6000 ? LABELS[robotState.mode] : 'Sin datos';
}
