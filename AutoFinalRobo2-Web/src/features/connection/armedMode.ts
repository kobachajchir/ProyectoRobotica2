/* AFR2-20261117-11. Modos «Ir» y «Línea» preparados desde la Web.
 * El firmware solo entra en GOTO o en seguimiento de línea al iniciar el movimiento, así que elegir uno de esos modos
 * deja al auto habilitado (modo Manual, sin consigna) y la Web recuerda cuál se eligió. Nunca inicia movimiento. */
import { useCallback, useEffect, useSyncExternalStore } from 'react';
import { useConnection } from './ConnectionContext';

export type ArmedMode = 'goto' | 'line' | null;
export type UiMode = 'idle' | 'manual' | 'line-follow' | 'goto';
export const UI_MODE_NAMES: Record<UiMode, string> = { idle: 'Reposo', manual: 'Manual', 'line-follow': 'Línea', goto: 'Ir' };

let armed: ArmedMode = null;
const listeners = new Set<() => void>();
export function setArmedMode(next: ArmedMode) {
  if (armed === next) return;
  armed = next;
  listeners.forEach(listener => listener());
}
function subscribe(listener: () => void) { listeners.add(listener); return () => { listeners.delete(listener); }; }
export function useArmedMode(): ArmedMode { return useSyncExternalStore(subscribe, () => armed, () => null); }

/** Suelta el modo preparado cuando el auto ya no está disponible o hace otra cosa. */
export function useArmedModeGuard() {
  const { wsState, megaLinkState, robotState } = useConnection();
  const mode = robotState?.mode;
  const linked = wsState === 'ready' && megaLinkState === 'ready';
  useEffect(() => {
    if (!linked) { setArmedMode(null); return; }
    if (mode === 'wheel-test' || mode === 'sequence' || mode === 'fault') setArmedMode(null);
    else if (mode === 'line-follow' && armed === 'goto') setArmedMode(null);
    else if (mode === 'goto' && armed === 'line') setArmedMode(null);
  }, [linked, mode]);
}

/** Cambia el modo del auto. «Ir» y «Línea» envían Manual (habilita motores, sin movimiento) y quedan preparados. */
export function useSelectRobotMode() {
  const { executeCommandWithToast } = useConnection();
  return useCallback(async (next: UiMode): Promise<boolean> => {
    const name = UI_MODE_NAMES[next];
    const wire = next === 'goto' || next === 'line-follow' ? 'manual' : next;
    const res = await executeCommandWithToast('robot.mode', { mode: wire }, { loadingTitle: `Cambiando modo a ${name}...`, successTitle: `Modo ${name} aplicado` });
    if (!res) return false;
    setArmedMode(next === 'goto' ? 'goto' : next === 'line-follow' ? 'line' : null);
    return true;
  }, [executeCommandWithToast]);
}
