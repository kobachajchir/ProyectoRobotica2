/* AFR2-20261117-12. Visor de seguimiento: misma interfaz inmersiva que el control táctil (FloatingModal a pantalla
 * completa, estado arriba, escenario, pie con el estado y los mandos, riel lateral con hojas flotantes), con la vista
 * cenital del seguidor de línea (LineFollowerLiveView) ocupando todo el escenario.
 *
 * Mandos (firmware AFR2-20261117-11):
 *  - Iniciar / Reanudar: robot.mode { mode: 'line-follow', start: true }.
 *  - Pausar: STOP y vuelta a robot.mode 'line-follow' sin start (queda armado y quieto, con los parámetros).
 *  - Detener: STOP y modo Reposo.
 * Sin auto conectado la vista es una simulación local y los mandos no envían nada. */
import { useCallback, useEffect, useState } from 'react';
import { FloatingModal, type ModalOrigin } from '../../components/ui/FloatingModal';
import { FloatingSheet } from '../../components/robot/TurnPresets';
import { RangeNumberField } from '../../components/ui/RangeNumberField';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import { useConnection } from '../connection/ConnectionContext';
import { LineFollowerLiveView } from '../control/LineFollowerLiveView';
import { LineTapeField } from '../control/lineTape';
import { analyzeLine, LINE_CENTER_TEXT, LINE_ORIENTATION_TEXT } from '../sensors/lineAnalysis';

type Sheet = 'info' | 'params' | 'calibration' | null;
type Busy = 'start' | 'pause' | 'stop' | 'params' | null;

export function LineFollowerOverlay({ open, origin, onClose }: { open: boolean; origin: ModalOrigin | null; onClose: () => void }) {
  const {
    wsState, megaLinkState, capabilities, robotState, sensors, stopRecord,
    requestCommand, executeCommandWithToast, refreshSensors, triggerEmergencyStop,
  } = useConnection();
  const connected = wsState === 'ready' && megaLinkState === 'ready';
  const lineMode = robotState?.mode === 'line-follow';
  const running = lineMode && robotState?.stopped === false;
  const [paused, setPaused] = useState(false);
  const [busy, setBusy] = useState<Busy>(null);
  const [sheet, setSheet] = useState<Sheet>(null);
  const [anchor, setAnchor] = useState<HTMLElement | null>(null);
  const [speedMmS, setSpeedMmS] = useState(80);
  const [kp, setKp] = useState(100);
  const [message, setMessage] = useState<string | null>(null);

  // Mismos requisitos que el panel del seguidor (LineFollowerPanel).
  const canStart = connected && capabilities.line_follow.implemented && capabilities.mecanum_drive.implemented &&
    capabilities.wheel_odometry.calibrated && robotState?.motorsEnabled === true && stopRecord.status !== 'sending';
  const blockedReason = !connected ? 'Auto desconectado: vista previa local, sin movimientos.'
    : !robotState?.motorsEnabled ? 'Habilitá el control de motores (Controles rápidos o acción 36).'
    : !capabilities.wheel_odometry.calibrated ? 'Se requiere odometría Hall calibrada y pose válida.'
    : !capabilities.line_follow.implemented ? capabilities.line_follow.reason || 'Calibrá los 5 sensores IR sobre el fondo y la línea.'
    : null;

  // Al abrir, todo cerrado; si el seguidor ya estaba armado y quieto, se muestra como pausa.
  useEffect(() => { if (open) { setSheet(null); setMessage(null); } }, [open]);
  useEffect(() => { if (!lineMode) setPaused(false); else if (running) setPaused(false); }, [lineMode, running]);

  // Lecturas IR frecuentes mientras el visor está abierto (como el panel).
  useEffect(() => {
    if (!open || !connected) return;
    void refreshSensors();
    const id = window.setInterval(() => { if (document.visibilityState === 'visible') void refreshSensors(); }, 250);
    return () => window.clearInterval(id);
  }, [open, connected, refreshSensors]);

  const run = useCallback(async (kind: Exclude<Busy, null>, action: () => Promise<void>) => {
    if (busy) return;
    setBusy(kind); setMessage(null);
    try { await action(); }
    catch (error) { setMessage(error instanceof Error ? error.message : 'No se pudo completar la orden.'); }
    finally { setBusy(null); }
  }, [busy]);

  const start = () => run('start', async () => {
    await executeCommandWithToast('robot.mode', { mode: 'line-follow', start: true },
      { loadingTitle: paused ? 'Reanudando seguimiento…' : 'Iniciando seguimiento…', successTitle: 'Siguiendo la línea' });
    setPaused(false);
  });
  const pause = () => run('pause', async () => {
    const stopped = await triggerEmergencyStop('line-pause');
    if (!stopped) throw new Error('STOP sin confirmar. Revisá el auto antes de continuar.');
    await requestCommand('robot.mode', { mode: 'line-follow' }, 2500);
    setPaused(true);
  });
  const stop = () => run('stop', async () => {
    const stopped = await triggerEmergencyStop('line-stop');
    if (!stopped) throw new Error('STOP sin confirmar. Revisá el auto antes de continuar.');
    await requestCommand('robot.mode', { mode: 'idle' }, 2500).catch(() => undefined);
    setPaused(false);
  });
  const applyParams = () => run('params', async () => {
    await executeCommandWithToast('robot.calibrate', { action: 6, value: speedMmS }, { loadingTitle: 'Aplicando velocidad…', successTitle: 'Velocidad de línea actualizada' });
    await executeCommandWithToast('robot.calibrate', { action: 7, value: kp }, { loadingTitle: 'Aplicando Kp…', successTitle: 'Ganancia de línea actualizada' });
    setMessage(`Aplicado en RAM: ${speedMmS} mm/s · Kp ${kp}.`);
  });
  const loadParams = () => run('params', async () => {
    const [s, k] = await Promise.all([
      requestCommand<{ value: number }>('robot.config.get', { action: 6 }),
      requestCommand<{ value: number }>('robot.config.get', { action: 7 }),
    ]);
    if (s.status === 'completed' && Number.isInteger(s.payload.value)) setSpeedMmS(s.payload.value);
    if (k.status === 'completed' && Number.isInteger(k.payload.value)) setKp(k.payload.value);
  });
  const calibrate = (payload: Record<string, unknown>, loadingTitle: string, successTitle: string) =>
    run('params', async () => { await executeCommandWithToast('robot.line.calibrate', payload, { loadingTitle, successTitle }); });

  const live = connected && sensors.line.valid && sensors.line.bits !== null;
  const analysis = live ? analyzeLine(sensors.line.bits!, []) : null;
  const status = !connected ? 'Vista previa · simulación local'
    : running ? (analysis ? `Siguiendo · ${LINE_CENTER_TEXT[analysis.center]}` : 'Siguiendo la línea')
    : lineMode && paused ? 'En pausa · listo para reanudar'
    : lineMode ? 'Seguidor armado · listo para iniciar'
    : blockedReason && !canStart ? 'Seguidor no disponible'
    : 'Detenido · listo para iniciar';
  const stopping = busy === 'pause' || busy === 'stop';
  const openSheet = (next: Exclude<Sheet, null>, element: HTMLElement) => { setAnchor(element); setSheet(previous => previous === next ? null : next); };
  const paramsDisabled = !connected || running || busy !== null;

  return <FloatingModal open={open} origin={origin} size="fullscreen" title="Seguimiento de línea" kicker="Visor de seguimiento"
    headerStatus={<span className="afr2-manual-connection" data-connected={connected} role="status" aria-live="polite">
      <span className="afr2-manual-connection-dot" aria-hidden="true" />{connected ? 'Conectado' : 'Desconectado'}
    </span>}
    onBeforeClose={() => { setSheet(null); }} onClose={onClose}>
    <div className="afr2-manual-drive afr2-line-visor" data-moving={running} data-state={running ? 'run' : paused ? 'pause' : 'idle'}>
      <div className="afr2-manual-stage afr2-line-visor__stage">
        <LineFollowerLiveView fill speedMmS={speedMmS} />
      </div>
      <footer className="afr2-manual-footer afr2-line-visor__footer">
        <p className="afr2-manual-direction" role="status" aria-live="polite" data-moving={running}>
          <span key={status} className="afr2-manual-direction__text">{status}</span>
        </p>
        {analysis && <p className="afr2-help">Orientación: {LINE_ORIENTATION_TEXT[analysis.orientation]}</p>}
        <div className="afr2-line-visor__controls" role="group" aria-label="Mandos del seguidor">
          <Button variant="primary" loading={busy === 'start'} disabled={!canStart || running || busy !== null}
            icon={<UiIcon name="play" size={18} />} onClick={() => void start()}>
            {paused ? 'Reanudar' : 'Iniciar movimiento'}
          </Button>
          <Button variant="secondary" loading={busy === 'pause'} disabled={!connected || !running || stopping}
            icon={<UiIcon name="pause" size={18} />} onClick={() => void pause()}>
            Pausar
          </Button>
          <Button variant="emergency" loading={busy === 'stop'} disabled={!connected || (!lineMode && !running) || busy === 'stop'}
            icon={<UiIcon name="stop" size={18} />} onClick={() => void stop()}>
            Detener
          </Button>
        </div>
        {(message || (!canStart && !lineMode && blockedReason)) && <p className="afr2-help" role="status">{message ?? blockedReason}</p>}
      </footer>
      <nav className="afr2-manual-rail" aria-label="Opciones del visor de seguimiento">
        <button type="button" className="afr2-manual-rail__item" aria-haspopup="dialog" aria-pressed={sheet === 'info'} onClick={event => openSheet('info', event.currentTarget)}>
          <span className="afr2-manual-rail__icon"><UiIcon name="info" /></span><span className="afr2-manual-rail__label">Información</span></button>
        <button type="button" className="afr2-manual-rail__item" aria-haspopup="dialog" aria-pressed={sheet === 'params'} onClick={event => openSheet('params', event.currentTarget)}>
          <span className="afr2-manual-rail__icon"><UiIcon name="settings" /></span><span className="afr2-manual-rail__label">Parámetros</span></button>
        <button type="button" className="afr2-manual-rail__item" aria-haspopup="dialog" aria-pressed={sheet === 'calibration'} onClick={event => openSheet('calibration', event.currentTarget)}>
          <span className="afr2-manual-rail__icon"><UiIcon name="sensors" /></span><span className="afr2-manual-rail__label">Calibrar</span></button>
      </nav>
      <FloatingSheet mode="contained" placement="side" anchor={anchor} open={sheet === 'info'} onClose={() => setSheet(null)} title="Información">
        <p className="afr2-help">
          La cinta queda fija al centro con su ancho real y el auto se mueve sobre ella según IR1…IR5. El piso corre mientras sigue la línea.
          Iniciar arranca el seguimiento; Pausar detiene el auto y lo deja armado para reanudar; Detener lo pasa a Reposo.
          {connected ? ' Control proporcional con recuperación de pérdida breve (250 ms) y parada en cruce transversal (150 ms).' : ' Sin auto: la vista es una simulación local y no se envían órdenes.'}
        </p>
      </FloatingSheet>
      <FloatingSheet mode="contained" placement="side" anchor={anchor} open={sheet === 'params'} onClose={() => setSheet(null)} title="Parámetros">
        <RangeNumberField label="Velocidad de línea" value={speedMmS} min={10} max={500} step={10} unit="mm/s" disabled={paramsDisabled} onChange={setSpeedMmS} />
        <RangeNumberField label="Ganancia proporcional Kp" value={kp} min={0} max={1000} step={10} disabled={paramsDisabled} onChange={setKp} />
        <LineTapeField />
        <div className="afr2-action-row">
          <Button size="sm" variant="secondary" disabled={!connected || busy !== null} onClick={() => void loadParams()}>Consultar</Button>
          <Button size="sm" variant="primary" disabled={paramsDisabled} onClick={() => void applyParams()}>Aplicar en RAM</Button>
        </div>
        <p className="afr2-help">{running ? 'Pausá o detené el auto para cambiar los parámetros.' : 'Velocidad y Kp se aplican en RAM; el ancho de la cinta solo cambia el dibujo.'}</p>
      </FloatingSheet>
      <FloatingSheet mode="contained" placement="side" anchor={anchor} open={sheet === 'calibration'} onClose={() => setSheet(null)} title="Calibrar suelo y cinta">
        <p className="afr2-help">Con el auto detenido: apoyá los sensores sobre el fondo, después sobre la cinta, y confirmá.</p>
        <div className="afr2-action-row">
          <Button size="sm" variant="secondary" disabled={paramsDisabled} onClick={() => void calibrate({ confirm: false, sample: 'background' }, 'Tomando muestra de fondo…', 'Fondo registrado')}>1. Fondo</Button>
          <Button size="sm" variant="secondary" disabled={paramsDisabled} onClick={() => void calibrate({ confirm: false, sample: 'line' }, 'Tomando muestra de línea…', 'Línea registrada')}>2. Línea</Button>
          <Button size="sm" variant="primary" disabled={paramsDisabled} onClick={() => void calibrate({ confirm: true }, 'Confirmando calibración…', 'Línea calibrada')}>3. Confirmar</Button>
        </div>
      </FloatingSheet>
    </div>
  </FloatingModal>;
}
