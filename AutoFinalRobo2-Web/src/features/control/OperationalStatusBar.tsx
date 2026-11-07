import { useConnection } from '../connection/ConnectionContext';
/* Códigos de fallo del ATmega (afr2_product.c). AFR2-20261107-39 agrega el 4. */
const FAULT_TEXT: Record<number, string> = {
  1: 'lazo de control demorado', 2: 'Hall/odometría no disponible o salto de cuentas',
  3: 'rueda sin pulsos Hall', 4: 'sentido PWM/Hall cruzado en una rueda',
};

/**
 * AFR2-20261101-12 / VIS-053. Barra de estado operativo bajo la barra superior, solo en Conducir y
 * Programas: Modo, GOTO, Salidas y Fallo con el mismo formato. Datos del ATmega; un dato
 * desconocido se muestra como «—». Sin estado del robot no se dibuja.
 */
export function OperationalStatusBar() {
  const { robotState } = useConnection();
  if (!robotState) return null;
  return <div className="afr2-operational-bar">
    <ul className="afr2-status-chips" aria-label="Estado operativo del robot">
      <li className="afr2-status-chip"><span>Modo</span><strong>{robotState.mode}</strong></li>
      <li className="afr2-status-chip"><span>GOTO</span><strong>{robotState.gotoState}</strong></li>
      <li className="afr2-status-chip"><span>Salidas</span><strong>{robotState.stopped === null ? '—' : robotState.stopped ? 'Apagadas' : 'Activas'}</strong></li>
      <li className="afr2-status-chip" data-tone={robotState.fault ? 'error' : undefined} title={robotState.fault ? FAULT_TEXT[robotState.fault] : undefined}><span>Fallo</span><strong>{robotState.fault ? `${robotState.fault} · ${FAULT_TEXT[robotState.fault] ?? 'código desconocido'}` : '—'}</strong></li>
    </ul>
  </div>;
}
