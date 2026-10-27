import { HoldActionButton } from '../ui/HoldActionButton';
import { Button } from '../ui/Button';

export interface DriveIntent {
  id: string; label: string; symbol: string; vx: -1 | 0 | 1; vy: -1 | 0 | 1; wz: -1 | 0 | 1;
}
const DIRECTIONS: DriveIntent[] = [
  { id: 'nw', label: 'Diagonal izquierda adelante', symbol: '↖', vx: 1, vy: 1, wz: 0 },
  { id: 'n', label: 'Adelante', symbol: '↑', vx: 1, vy: 0, wz: 0 },
  { id: 'ne', label: 'Diagonal derecha adelante', symbol: '↗', vx: 1, vy: -1, wz: 0 },
  { id: 'w', label: 'Izquierda', symbol: '←', vx: 0, vy: 1, wz: 0 },
  { id: 'e', label: 'Derecha', symbol: '→', vx: 0, vy: -1, wz: 0 },
  { id: 'sw', label: 'Diagonal izquierda atras', symbol: '↙', vx: -1, vy: 1, wz: 0 },
  { id: 's', label: 'Atras', symbol: '↓', vx: -1, vy: 0, wz: 0 },
  { id: 'se', label: 'Diagonal derecha atras', symbol: '↘', vx: -1, vy: -1, wz: 0 },
];
const LEFT: DriveIntent = { id: 'rot-ccw', label: 'Girar izquierda', symbol: '↶', vx: 0, vy: 0, wz: 1 };
const RIGHT: DriveIntent = { id: 'rot-cw', label: 'Girar derecha', symbol: '↷', vx: 0, vy: 0, wz: -1 };

/** Identical presentation for physical and simulated movement. Factors leave
 * physical limits/units and simulation integration entirely in their adapters. */
export function MecanumDrivePad({ disabled = false, activeId, onStart, onStop }: {
  disabled?: boolean; activeId?: string | null; onStart: (intent: DriveIntent) => void; onStop: () => void;
}) {
  const action = (intent: DriveIntent, className = '') => <HoldActionButton key={intent.id}
    label={intent.label} disabled={disabled} active={activeId === intent.id} onStart={() => onStart(intent)} onStop={onStop}
    className={`afr2-drive-pad__button ${className}`}>
    <span aria-hidden="true">{intent.symbol}</span>{intent.wz !== 0 && <small>{intent.wz > 0 ? 'Giro izq.' : 'Giro der.'}</small>}
  </HoldActionButton>;
  return <div className="afr2-drive-pad" role="group" aria-label="Movimiento Mecanum">
    {DIRECTIONS.slice(0, 4).map(intent => action(intent))}
    <Button disabled={disabled} className="afr2-drive-pad__button afr2-drive-pad__stop" aria-label="Detener consigna de movimiento" title="Detener consigna" onClick={onStop}><span aria-hidden="true">■</span></Button>
    {DIRECTIONS.slice(4).map(intent => action(intent))}
    {action(LEFT, 'afr2-drive-pad__turn-left')}
    {action(RIGHT, 'afr2-drive-pad__turn-right')}
  </div>;
}
