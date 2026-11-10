import { useRef, useState } from 'react';
import { HoldActionButton } from '../ui/HoldActionButton';
import { Button } from '../ui/Button';
import { UiIcon, type UiIconName } from '../ui/UiIcon';
import { FloatingSheet, TurnPresetList, type TurnSide } from './TurnPresets';

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

/** AFR2-20261110-16: optional 90° presets beside CW/CCW (arrow or long tap). */
export interface TurnMenu { available: boolean; reason: string | null; onTurnTo: (thetaMrad: number) => void }

/** Identical presentation for physical and simulated movement. Factors leave
 * physical limits/units and simulation integration entirely in their adapters. */
export function MecanumDrivePad({ disabled = false, activeId, onStart, onStop, turnMenu }: {
  disabled?: boolean; activeId?: string | null; onStart: (intent: DriveIntent) => void; onStop: () => void; turnMenu?: TurnMenu;
}) {
  const [menu, setMenu] = useState<TurnSide | null>(null);
  const anchors = useRef<Record<TurnSide, HTMLButtonElement | null>>({ cw: null, ccw: null });
  const action = (intent: DriveIntent, className = '') => <HoldActionButton key={intent.id}
    label={intent.label} disabled={disabled} active={activeId === intent.id} onStart={() => onStart(intent)} onStop={onStop}
    className={`afr2-drive-pad__button ${className}`}>
    <UiIcon name={intent.icon} size={24} />{intent.wz !== 0 && <small>{intent.wz > 0 ? 'Girar CCW' : 'Girar CW'}</small>}
  </HoldActionButton>;
  const turn = (intent: DriveIntent, side: TurnSide) => {
    if (!turnMenu) return action(intent, side === 'ccw' ? 'afr2-drive-pad__turn-left' : 'afr2-drive-pad__turn-right');
    return <div key={intent.id}
      className={`afr2-drive-pad__turn ${side === 'ccw' ? 'afr2-drive-pad__turn-left' : 'afr2-drive-pad__turn-right'}`}
      // Long tap (touch) or right click opens the presets; it ends any open-loop hold first.
      onContextMenu={event => { event.preventDefault(); if (!turnMenu.available) return; onStop(); setMenu(side); }}>
      {action(intent)}
      <button type="button" ref={element => { anchors.current[side] = element; }} className="afr2-drive-pad__turn-more" aria-haspopup="dialog" aria-expanded={menu === side}
        aria-label={`Giros de 90° ${side === 'ccw' ? 'a la izquierda' : 'a la derecha'}`} title="Giros de 90°"
        onClick={() => setMenu(current => current === side ? null : side)}><UiIcon name="chevron-down" size={14} /></button>
    </div>;
  };
  return <div className="afr2-drive-pad" role="group" aria-label="Movimiento Mecanum">
    {DIRECTIONS.slice(0, 4).map(intent => action(intent))}
    <Button disabled={disabled} className="afr2-drive-pad__button afr2-drive-pad__stop" aria-label="Detener consigna de movimiento" title="Detener consigna" onClick={onStop}><UiIcon name="square" size={24} /></Button>
    {DIRECTIONS.slice(4).map(intent => action(intent))}
    {turn(LEFT, 'ccw')}
    {turn(RIGHT, 'cw')}
    {turnMenu && <FloatingSheet variant="popover" open={menu !== null} onClose={() => setMenu(null)} anchor={menu ? anchors.current[menu] : null}
      title={menu === 'ccw' ? 'Girar a la izquierda' : 'Girar a la derecha'}>
      <TurnPresetList side={menu ?? undefined} available={turnMenu.available && !disabled} reason={disabled ? 'control no disponible.' : turnMenu.reason}
        onTurnTo={turnMenu.onTurnTo} onDone={() => setMenu(null)} />
    </FloatingSheet>}
  </div>;
}
