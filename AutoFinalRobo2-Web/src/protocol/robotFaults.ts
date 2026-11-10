/** Single fault code reported by the controller, not a bit mask or counter. */
export const ROBOT_FAULT_TEXT: Record<number, string> = {
  1: 'lazo de control demorado',
  2: 'Hall/odometría no disponible o salto de cuentas',
  3: 'rueda sin pulsos Hall',
  4: 'sentido PWM/Hall cruzado en una rueda',
};

export const ROBOT_FAULT_HELP: Record<number, { detail: string; steps: string[] }> = {
  1: {
    detail: 'El controlador pasó más de 100 ms sin actualizar el control con salidas activas y detuvo los motores.',
    steps: ['Con las salidas detenidas, usá Limpiar fallo en este panel o en el encoder del auto. La limpieza no vuelve a mover los motores.',
      'Si vuelve a aparecer, mantené el auto detenido y revisá el log USB del ATmega en el Loader: los mensajes LOOP slow y los tiempos de render ayudan a localizar la demora.'],
  },
  2: {
    detail: 'No se cumplen los requisitos de Hall/geometría para odometría, o se recibió un salto de cuentas fuera de rango.',
    steps: ['Revisá en Puesta a punto que Hall esté habilitado y que cada rueda tenga sus pulsos por vuelta guardados.',
      'En el auto, revisá los conectores Hall y las alimentaciones. Detené las salidas antes de intervenir.'],
  },
  3: {
    detail: 'Se aplicó potencia a una rueda y no llegaron cambios de cuentas Hall durante más de 500 ms.',
    steps: ['Detené las salidas y comprobá que la rueda no esté trabada y que su conector Hall esté firme.',
      'Usá la prueba de Hall de Puesta a punto para observar las cuentas antes de volver a iniciar una marcha.'],
  },
  4: {
    detail: 'El sentido medido por Hall no coincide con el esperado por el control de la rueda.',
    steps: ['Revisá los signos de motor y Hall por rueda en Puesta a punto, con las salidas detenidas.',
      'No inviertas el sentido en la Web para ocultar el problema. Corregí la calibración o el cableado de la rueda afectada.'],
  },
};

export interface RobotFaultDiagnostics {
  fault: number; stopped: boolean; cleared: boolean; lateCount: number; controlGapMs: number;
  timing: { totalMs: number; appMs: number; uartMs: number; inputMs: number; renderMs: number; ioMs: number };
}
export function parseFaultDiagnostics(value: unknown): RobotFaultDiagnostics | null {
  if (!value || typeof value !== 'object') return null;
  const d=value as RobotFaultDiagnostics;
  const uint=(v: unknown,max: number): v is number => typeof v==='number' && Number.isInteger(v) && v>=0 && v<=max;
  if(!uint(d.fault,255) || typeof d.stopped!=='boolean' || typeof d.cleared!=='boolean' || !uint(d.lateCount,65535) || !uint(d.controlGapMs,65535) || !d.timing) return null;
  if(!['totalMs','appMs','uartMs','inputMs','renderMs','ioMs'].every(key=>uint(d.timing[key as keyof RobotFaultDiagnostics['timing']],65535))) return null;
  return d;
}
