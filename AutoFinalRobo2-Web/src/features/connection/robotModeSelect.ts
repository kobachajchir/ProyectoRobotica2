/* AFR2-20261117-11. Selector de modo operativo real (Reposo, Manual, Seguir línea, Ir).
 * El modo vive en el ATmega y se reporta por robot.state; se puede fijar desde la Web (robot.mode) o desde la OLED.
 * Cambiar de modo nunca inicia movimiento. */
import { useCallback } from 'react';
import { useConnection } from './ConnectionContext';

export type UiMode = 'idle' | 'manual' | 'line-follow' | 'goto';
export const UI_MODE_NAMES: Record<UiMode, string> = { idle: 'Reposo', manual: 'Manual', 'line-follow': 'Seguir línea', goto: 'Ir' };

/** Modo de robot informado → modo de interfaz (manual-pwm cuenta como Manual). */
export function toUiMode(mode: string | undefined): UiMode | null {
  return mode === 'idle' ? 'idle' : mode === 'manual' || mode === 'manual-pwm' ? 'manual' : mode === 'line-follow' ? 'line-follow' : mode === 'goto' ? 'goto' : null;
}

export function useSelectRobotMode() {
  const { executeCommandWithToast } = useConnection();
  return useCallback(async (next: UiMode, start = false): Promise<boolean> => {
    const name = UI_MODE_NAMES[next];
    const payload: Record<string, unknown> = { mode: next };
    if (start && next === 'line-follow') payload.start = true;
    const res = await executeCommandWithToast('robot.mode', payload, start
      ? { loadingTitle: 'Iniciando movimiento...', successTitle: 'Movimiento iniciado' }
      : { loadingTitle: `Cambiando modo a ${name}...`, successTitle: `Modo ${name} aplicado` });
    return !!res;
  }, [executeCommandWithToast]);
}
