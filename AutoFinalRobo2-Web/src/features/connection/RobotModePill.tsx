/* AFR2-20261110-16. Current robot mode, always visible in the header (also on phones).
 * Pressing it opens the operating-mode selector (moved here from Conducir). Choosing sends
 * robot.mode; the pill only shows what the robot reports, never the requested mode. */
import { useState } from 'react';
import { FloatingModal, type ModalOrigin } from '../../components/ui/FloatingModal';
import { SegmentedControl } from '../../components/ui/SegmentedControl';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { UiIcon, type UiIconName } from '../../components/ui/UiIcon';
import { useConnection } from './ConnectionContext';
import { useRobotModeState } from './useRobotMode';
import { useSelectRobotMode, toUiMode, type UiMode } from './robotModeSelect';

type ModeOption = UiMode;
const ICONS: Record<string, UiIconName> = { idle: 'pause', manual: 'move', 'manual-pwm': 'move', 'line-follow': 'sensors', 'wheel-test': 'wheel', goto: 'crosshair', sequence: 'programs', fault: 'warning', unknown: 'help', none: 'help' };
const OPTIONS: { value: ModeOption; label: string }[] = [{ value: 'idle', label: 'Reposo' }, { value: 'manual', label: 'Manual' }, { value: 'line-follow', label: 'Seguir línea' }, { value: 'goto', label: 'Ir' }];

interface Props { open: boolean; origin: ModalOrigin | null; onOpen: (origin: ModalOrigin) => void; onClose: () => void }
export function RobotModePill({ open, origin, onOpen, onClose }: Props) {
  const { mode, label } = useRobotModeState();
  const { wsState, megaLinkState, stopRecord } = useConnection();
  const selectMode = useSelectRobotMode();
  const [sending, setSending] = useState(false);
  const tone = mode === 'fault' ? 'error' : mode === 'idle' ? 'idle' : mode === 'none' || mode === 'unknown' ? 'unknown' : 'active';
  const current: ModeOption | null = mode === 'none' ? null : toUiMode(mode);
  const blocked = wsState !== 'ready' ? 'Falta conexión WebSocket con el ESP.'
    : megaLinkState !== 'ready' ? 'Falta enlace del ESP con el ATmega.'
    : stopRecord.status === 'sending' ? 'Esperando la confirmación de STOP.'
    : stopRecord.status === 'unconfirmed-offline' ? 'El último STOP sigue sin confirmar.' : null;
  const choose = async (next: ModeOption) => {
    if (blocked || sending) return;
    setSending(true);
    try {
      await selectMode(next);
    } finally { setSending(false); }
  };
  return <>
    <button type="button" className="afr2-mode-pill" data-tone={tone} aria-haspopup="dialog" aria-expanded={open}
      aria-label={`Modo actual: ${label}. Cambiar modo operativo`} title={`Modo actual: ${label}`}
      onClick={event => onOpen({ element: event.currentTarget, keyboard: event.detail === 0 })}>
      <UiIcon name={ICONS[mode] ?? 'help'} size={16} /><span className="afr2-mode-pill__label">{label}</span>
    </button>
    <FloatingModal open={open} origin={origin} onClose={onClose} title="Modo operativo" description={`Modo informado por el robot: ${label}.`}>
      <div className="afr2-page-stack">
        {blocked && <StatusNotice tone="info" title="Cambio de modo no disponible">{blocked}</StatusNotice>}
        {/* AFR2-20261101-09 / VIS-052: choosing sends the order, so arrows only move focus (manual activation). */}
        <SegmentedControl ariaLabel="Modo operativo del robot" activation="manual" reselect fill="always"
          value={current ?? 'idle'} onChange={value => void choose(value)}
          items={OPTIONS.map(item => ({ ...item,
            // AFR2-20261117-11: todos los modos se pueden elegir con el enlace listo; los requisitos (calibración, pose) se exigen al iniciar el movimiento.
            disabled: !!blocked || sending }))} />
        <p className="afr2-help">Reposo permite ver sensores y calibrar, sin mover el auto. Manual habilita botones, control táctil y secuencias. Seguir línea habilita el seguidor e Ir los recorridos; ninguno mueve el auto hasta que lo inicies desde su pantalla. El modo también se puede elegir en el auto (OLED).{current === null && mode !== 'none' ? ` El robot está en ${label}; elegir un modo lo reemplaza.` : ''}</p>
      </div>
    </FloatingModal>
  </>;
}
