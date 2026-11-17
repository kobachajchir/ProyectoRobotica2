/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { useEffect, useState } from 'react';
import { useConnection } from './ConnectionContext';
import { useArmedMode, useArmedModeGuard } from './armedMode';
import { stateAge, type RobotMode } from '../../protocol/robotState';

/* AFR2-20261117-11: GOTO se muestra como «Ir». */
const LABELS: Record<RobotMode, string> = { idle: 'Reposo', manual: 'Manual', 'manual-pwm':'Manual PWM', 'line-follow': 'Seguimiento de línea', 'wheel-test': 'Prueba de rueda', goto: 'Ir', sequence: 'Secuencia', fault: 'Fallo', unknown: 'Sin datos' };

/** Shares the same authoritative snapshot as Control; never issues a second poll. */
export function useRobotMode(): string { return useRobotModeState().label; }
/** AFR2-20261110-16: raw mode too, for the header pill tone. 'none' = no fresh state.
 * AFR2-20261117-11: con el auto en Reposo o Manual, el modo «Ir» o «Línea» elegido en la Web se muestra como modo actual. */
export function useRobotModeState(): { mode: RobotMode | 'none'; label: string } {
  const { robotState, wsState, megaLinkState } = useConnection();
  const armed = useArmedMode();
  useArmedModeGuard();
  const [now, setNow] = useState(Date.now());
  useEffect(() => {
    setNow(Date.now());
    const age = stateAge(robotState, Date.now());
    if (age === null || age >= 6000) return;
    const timer = window.setTimeout(() => setNow(Date.now()), 6000 - age);
    return () => clearTimeout(timer);
  }, [robotState]);
  const age = stateAge(robotState, Math.max(now, Date.now()));
  if (!(wsState === 'ready' && megaLinkState === 'ready' && robotState && age !== null && age < 6000)) return { mode: 'none', label: 'Sin datos' };
  const mode = robotState.mode;
  if (armed && (mode === 'idle' || mode === 'manual' || mode === 'manual-pwm')) return armed === 'goto' ? { mode: 'goto', label: 'Ir' } : { mode: 'line-follow', label: 'Línea' };
  return { mode, label: LABELS[mode] };
}
