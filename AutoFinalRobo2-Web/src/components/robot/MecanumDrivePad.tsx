import { HoldActionButton } from '../ui/HoldActionButton';
import { Button } from '../ui/Button';
import { UiIcon, type UiIconName } from '../ui/UiIcon';

export interface DriveIntent {
  id: string; label: string; icon: UiIconName; vx: -1 | 0 | 1; vy: -1 | 0 | 1; wz: -1 | 0 | 1;
}
const DIRECTIONS: DriveIntent[] = [
  { id: 'nw', label: 'Diagonal izquierda adelante', icon: 'arrow-up-left', vx: 1, vy: 1, wz: 0 },
  { id: 'n', label: 'Adelante', icon: 'arrow-up', vx: 1, vy: 0, wz: 0 },
  { id: 'ne', label: 'Diagonal derecha adelante', icon: 'arrow-up-right', vx: 1, vy: -1, wz: 0 },
  { id: 'w', label: 'Izquierda', icon: 'arrow-left', vx: 0, vy: 1, wz: 0 },
  { id: 'e', label: 'Derecha', icon: 'arrow-right', vx: 0, vy: -1, wz: 0 },
  { id: 'sw', label: 'Diagonal izquierda atrás', icon: 'arrow-down-left', vx: -1, vy: 1, wz: 0 },
  { id: 's', label: 'Atrás', icon: 'arrow-down', vx: -1, vy: 0, wz: 0 },
  { id: 'se', label: 'Diagonal derecha atrás', icon: 'arrow-down-right', vx: -1, vy: -1, wz: 0 },
];
const LEFT: DriveIntent = { id: 'rot-ccw', label: 'Girar CCW', icon: 'rotate-ccw', vx: 0, vy: 0, wz: 1 };
const RIGHT: DriveIntent = { id: 'rot-cw', label: 'Girar CW', icon: 'rotate-cw', vx: 0, vy: 0, wz: -1 };

/** Identical presentation for physical and simulated movement. Factors leave
 * physical limits/units and simulation integration entirely in their adapters. */
export function MecanumDrivePad({ disabled = false, activeId, onStart, onStop }: {
  disabled?: boolean; activeId?: string | null; onStart: (intent: DriveIntent) => void; onStop: () => void;
}) {
  const action = (intent: DriveIntent, className = '') => <HoldActionButton key={intent.id}
    label={intent.label} disabled={disabled} active={activeId === intent.id} onStart={() => onStart(intent)} onStop={onStop}
    className={`afr2-drive-pad__button ${className}`}>
    <UiIcon name={intent.icon} size={24} />{intent.wz !== 0 && <small>{intent.wz > 0 ? 'Girar CCW' : 'Girar CW'}</small>}
  </HoldActionButton>;
  return <div className="afr2-drive-pad" role="group" aria-label="Movimiento Mecanum">
    {DIRECTIONS.slice(0, 4).map(intent => action(intent))}
    <Button disabled={disabled} className="afr2-drive-pad__button afr2-drive-pad__stop" aria-label="Detener consigna de movimiento" title="Detener consigna" onClick={onStop}><UiIcon name="square" size={24} /></Button>
    {DIRECTIONS.slice(4).map(intent => action(intent))}
    {action(LEFT, 'afr2-drive-pad__turn-left')}
    {action(RIGHT, 'afr2-drive-pad__turn-right')}
  </div>;
}
