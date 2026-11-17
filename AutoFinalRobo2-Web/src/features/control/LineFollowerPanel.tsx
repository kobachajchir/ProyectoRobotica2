import { useEffect, useState, type ReactNode } from 'react';
import { Button } from '../../components/ui/Button';
import { Chevron } from '../../components/ui/Chevron';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { UiIcon } from '../../components/ui/UiIcon';
import { useConnection } from '../connection/ConnectionContext';
import { useArmedMode, useSelectRobotMode } from '../connection/armedMode';
import type { ModalOrigin } from '../../components/ui/FloatingModal';
import { LineFollowerOverlay } from '../lineFollower/LineFollowerOverlay';
import { LineFollowerLiveView } from './LineFollowerLiveView';
import { LineTapeField } from './lineTape';
import { analyzeLine, LINE_CENTER_TEXT, LINE_ORIENTATION_TEXT } from '../sensors/lineAnalysis';

/** AFR2-20261117-11: tarjeta con encabezado plegable (chevron). `aside` queda fuera del botón para poder llevar acciones. */
function CollapsibleCard({ id, title, description, aside, defaultOpen = true, children }: {
  id: string; title: ReactNode; description?: ReactNode; aside?: ReactNode; defaultOpen?: boolean; children: ReactNode;
}) {
  const [open, setOpen] = useState(defaultOpen);
  return (
    <section className="afr2-glass-panel afr2-line-card rounded-[var(--radius-md)] p-5" data-open={open}>
      <div className="afr2-line-card__header">
        <button type="button" className="afr2-line-card__toggle" aria-expanded={open} aria-controls={`${id}-body`} onClick={() => setOpen(value => !value)}>
          <span className="afr2-line-card__title">
            <span className="text-base font-semibold text-[var(--text)]">{title}</span>
            {description && <span className="text-xs text-[var(--text-muted)]">{description}</span>}
          </span>
          <Chevron open={open} />
        </button>
        {aside && <div className="afr2-line-card__aside">{aside}</div>}
      </div>
      {open && <div id={`${id}-body`} className="afr2-line-card__body space-y-4">{children}</div>}
    </section>
  );
}

/**
 * AFR2-20261116-01 — Panel operativo y de ajuste del Seguidor de Línea (`AFR2_LINE`).
 *
 * Permite supervisar los 5 canales IR1..IR5, iniciar/detener el modo `line-follow`,
 * ajustar en RAM la velocidad de avance (`AFR2_A_LINE_SPEED`, acción 6) y la ganancia
 * proporcional (`AFR2_A_LINE_KP`, acción 7), y ejecutar el flujo de calibración de suelo/línea.
 *
 * AFR2-20261117-11: las dos tarjetas van una debajo de la otra y se pliegan. Activar el seguidor prepara el modo
 * Línea sin mover el auto; «Iniciar movimiento» (que solo aparece con el seguidor activo) lo pone en marcha y,
 * en marcha, aparece Detener arriba y ese mismo botón de abajo pasa a ser Detener. El ancho de la cinta vive en
 * «Ajuste y calibración de línea».
 */
/** AFR2-20261117-01: `showView=false` en la sección propia, que ya muestra el escenario grande. */
export function LineFollowerPanel({ showView = true }: { showView?: boolean } = {}) {
  const {
    wsState,
    megaLinkState,
    capabilities,
    robotState,
    sensors,
    stopRecord,
    requestCommand,
    executeCommandWithToast,
    refreshSensors,
    triggerEmergencyStop,
  } = useConnection();
  const selectMode = useSelectRobotMode();
  const armed = useArmedMode();

  const [lineSpeedMmS, setLineSpeedMmS] = useState<number>(80);
  const [lineKp, setLineKp] = useState<number>(100);
  const [busy, setBusy] = useState<boolean>(false);
  const [feedback, setFeedback] = useState<string | null>(null);
  // AFR2-20261117-15: «Vista completa» abre el visor inmersivo, como «Control táctil» en Conducir.
  const [visorOpen, setVisorOpen] = useState(false);
  const [visorOrigin, setVisorOrigin] = useState<ModalOrigin | null>(null);

  const online = wsState === 'ready' && megaLinkState === 'ready';
  const isLineMode = robotState?.mode === 'line-follow';
  const stopped = robotState?.stopped ?? true;
  /** Seguidor activo: Línea preparado desde la Web o el auto ya siguiendo la línea. */
  const active = armed === 'line' || isLineMode;
  const canStartLine =
    online &&
    capabilities.line_follow.implemented &&
    capabilities.mecanum_drive.implemented &&
    capabilities.wheel_odometry.calibrated &&
    robotState?.motorsEnabled === true &&
    stopRecord.status !== 'sending';

  useEffect(() => {
    if (!online) return;
    void refreshSensors();
    const id = window.setInterval(() => {
      if (document.visibilityState === 'visible') {
        void refreshSensors();
      }
    }, 250);
    return () => window.clearInterval(id);
  }, [online, refreshSensors]);

  const loadLineParameters = async () => {
    if (!online || busy) return;
    setBusy(true);
    try {
      const [speedRes, kpRes] = await Promise.all([
        requestCommand<{ value: number }>('robot.config.get', { action: 6 }),
        requestCommand<{ value: number }>('robot.config.get', { action: 7 }),
      ]);
      if (speedRes.status === 'completed' && Number.isInteger(speedRes.payload.value)) {
        setLineSpeedMmS(speedRes.payload.value);
      }
      if (kpRes.status === 'completed' && Number.isInteger(kpRes.payload.value)) {
        setLineKp(kpRes.payload.value);
      }
      setFeedback('Parámetros actuales de línea leídos desde el ATmega.');
    } catch (e) {
      setFeedback(e instanceof Error ? e.message : 'No se pudieron consultar los parámetros de línea.');
    } finally {
      setBusy(false);
    }
  };

  const applyLineParameters = async () => {
    if (!online || !stopped || busy) return;
    setBusy(true);
    try {
      await executeCommandWithToast(
        'robot.calibrate',
        { action: 6, value: lineSpeedMmS },
        { loadingTitle: 'Aplicando velocidad de línea…', successTitle: 'Velocidad de línea actualizada' }
      );
      await executeCommandWithToast(
        'robot.calibrate',
        { action: 7, value: lineKp },
        { loadingTitle: 'Aplicando ganancia Kp de línea…', successTitle: 'Ganancia de línea actualizada' }
      );
      setFeedback(`Aplicado en RAM: ${lineSpeedMmS} mm/s · Kp ${lineKp}.`);
    } catch (e) {
      setFeedback(e instanceof Error ? e.message : 'No se pudieron aplicar los parámetros.');
    } finally {
      setBusy(false);
    }
  };

  /** Activa el seguidor: modo Línea preparado, sin movimiento. */
  const activateLineFollower = async () => {
    if (!online || busy) return;
    setBusy(true);
    try { await selectMode('line-follow'); } finally { setBusy(false); }
  };
  /** Desactiva el seguidor y deja el auto en Reposo. */
  const deactivateLineFollower = async () => {
    if (!online || busy) return;
    setBusy(true);
    try { await selectMode('idle'); } finally { setBusy(false); }
  };
  const startMovement = async () => {
    if (!online || busy || !canStartLine) return;
    setBusy(true);
    try {
      await executeCommandWithToast(
        'robot.mode',
        { mode: 'line-follow' },
        {
          loadingTitle: 'Iniciando movimiento…',
          successTitle: 'Seguidor de línea en marcha',
        }
      );
    } finally {
      setBusy(false);
    }
  };
  const stopMovement = async () => {
    if (!online) return;
    await triggerEmergencyStop('line-stop');
  };

  const bits = sensors.line.valid && sensors.line.bits !== null ? sensors.line.bits : 0;
  const channels = sensors.line.channels ?? [false, false, false, false, false];
  const analysis = analyzeLine(bits, []);

  return (
    <div className="afr2-page-stack">
      {/* AFR2-20261117-15: encabezado y botón como «Conducción manual / Control táctil»; la vista va primero y las
          dos tarjetas abajo, plegadas al entrar. */}
      {showView && <>
        <div className="afr2-manual-heading">
          <h2>Seguidor de línea</h2>
          <Button variant="primary" aria-haspopup="dialog" aria-expanded={visorOpen} icon={<UiIcon name="sensors" />}
            onClick={event => { setVisorOrigin({ element: event.currentTarget, keyboard: event.detail === 0 }); setVisorOpen(true); }}>
            Vista completa
          </Button>
        </div>
        <section className="afr2-glass-panel rounded-[var(--radius-md)] p-5">
          <LineFollowerLiveView speedMmS={lineSpeedMmS} />
        </section>
        <LineFollowerOverlay open={visorOpen} origin={visorOrigin} onClose={() => setVisorOpen(false)} />
      </>}
      <CollapsibleCard
        id="afr2-line-follower"
        defaultOpen={false}
        title="Seguidor de línea (`AFR2_LINE`)"
        description="Control proporcional sobre los 5 sensores IR1…IR5 con recuperación de pérdida breve (250 ms) y parada en cruce transversal (150 ms)."
        aside={
          <>
            <span className="afr2-status-pill" data-state={isLineMode ? 'ready' : 'idle'}>
              {isLineMode ? 'Siguiendo línea' : active ? 'Activo · detenido' : 'En espera'}
            </span>
            {isLineMode && (
              <Button variant="emergency" size="sm" disabled={!online} icon={<UiIcon name="stop" size={16} />} onClick={() => void stopMovement()}>
                Detener
              </Button>
            )}
          </>
        }
      >
        {!canStartLine && !isLineMode && (
          <StatusNotice tone="info" title="Requisitos para iniciar el seguidor de línea">
            {!online
              ? 'Conectá el WebSocket al ESP y verificá el enlace con el ATmega.'
              : !robotState?.motorsEnabled
                ? 'Activá el seguidor (habilita los motores) o usá Controles rápidos.'
                : !capabilities.wheel_odometry.calibrated
                  ? 'Se requiere odometría Hall calibrada y pose válida.'
                  : capabilities.line_follow.reason || 'Calibrá los 5 sensores IR sobre el fondo y la línea.'}
          </StatusNotice>
        )}

        <div className="grid grid-cols-5 gap-2" aria-label="Estado de los 5 sensores IR">
          {channels.map((on, idx) => (
            <div
              key={idx}
              className={`p-2.5 rounded-[var(--radius-sm)] border text-center font-mono text-xs ${
                on
                  ? 'border-[var(--action)] bg-[var(--action)]/15 text-[var(--text)] font-bold'
                  : 'border-[var(--border-default)] bg-[var(--surface-subtle)] text-[var(--text-muted)]'
              }`}
            >
              <div>IR{idx + 1}</div>
              <div className="mt-1">{on ? 'LÍNEA' : 'FONDO'}</div>
            </div>
          ))}
        </div>

        <dl className="grid grid-cols-2 gap-2 text-xs p-3 rounded-[var(--radius-sm)] border border-[var(--border-default)]">
          <div>
            <dt className="text-[var(--text-muted)]">Posición respecto al centro</dt>
            <dd className="font-semibold mt-0.5">
              {sensors.line.valid ? LINE_CENTER_TEXT[analysis.center] : 'Sin lectura IR válida'}
            </dd>
          </div>
          <div>
            <dt className="text-[var(--text-muted)]">Orientación detectada</dt>
            <dd className="font-semibold mt-0.5">
              {sensors.line.valid ? LINE_ORIENTATION_TEXT[analysis.orientation] : '—'}
            </dd>
          </div>
        </dl>

        <div className="afr2-action-row">
          {!active && (
            <Button variant="primary" disabled={!online || busy} icon={<UiIcon name="play" size={16} />} onClick={() => void activateLineFollower()}>
              Iniciar seguidor de línea
            </Button>
          )}
          {active && !isLineMode && (
            <>
              <Button variant="primary" disabled={!online || busy || !canStartLine} icon={<UiIcon name="play" size={16} />} onClick={() => void startMovement()}>
                Iniciar movimiento
              </Button>
              <Button variant="secondary" disabled={!online || busy} onClick={() => void deactivateLineFollower()}>
                Desactivar seguidor
              </Button>
            </>
          )}
          {isLineMode && (
            <Button variant="emergency" disabled={!online} icon={<UiIcon name="stop" size={16} />} onClick={() => void stopMovement()}>
              Detener movimiento
            </Button>
          )}
        </div>
      </CollapsibleCard>

      <CollapsibleCard
        id="afr2-line-calibration"
        defaultOpen={false}
        title="Ajuste y calibración de línea"
        description="Con el auto detenido podés ajustar la velocidad de avance y la ganancia de giro, o tomar muestras de fondo y línea."
      >
        <LineTapeField />

        <div className="grid grid-cols-1 sm:grid-cols-2 gap-3">
          <label className="block text-xs font-medium text-[var(--text-muted)]">
            Velocidad de línea (10–500 mm/s)
            <input
              type="number"
              min={10}
              max={500}
              step={10}
              disabled={!stopped || busy}
              value={lineSpeedMmS}
              onChange={(e) => setLineSpeedMmS(Math.min(500, Math.max(10, Number(e.target.value) || 10)))}
              className="afr2-glass-field w-full min-h-10 px-3 mt-1 text-sm font-mono text-[var(--text)]"
            />
          </label>
          <label className="block text-xs font-medium text-[var(--text-muted)]">
            Ganancia proporcional Kp (0–1000)
            <input
              type="number"
              min={0}
              max={1000}
              step={10}
              disabled={!stopped || busy}
              value={lineKp}
              onChange={(e) => setLineKp(Math.min(1000, Math.max(0, Number(e.target.value) || 0)))}
              className="afr2-glass-field w-full min-h-10 px-3 mt-1 text-sm font-mono text-[var(--text)]"
            />
          </label>
        </div>

        <div className="afr2-action-row">
          <Button variant="secondary" size="sm" disabled={!online || busy} onClick={() => void loadLineParameters()}>
            Consultar valores
          </Button>
          <Button variant="primary" size="sm" disabled={!online || !stopped || busy} onClick={() => void applyLineParameters()}>
            Aplicar en RAM
          </Button>
        </div>

        <div className="pt-2 border-t border-[var(--border-default)] space-y-2">
          <span className="block text-xs font-medium text-[var(--text-muted)]">
            Calibración rápida de suelo y cinta
          </span>
          <div className="afr2-action-row">
            <Button
              size="sm"
              variant="secondary"
              disabled={!online || !stopped || busy}
              onClick={() =>
                void executeCommandWithToast(
                  'robot.line.calibrate',
                  { confirm: false, sample: 'background' },
                  { loadingTitle: 'Tomando muestra de fondo…', successTitle: 'Fondo registrado' }
                )
              }
            >
              1. Tomar fondo
            </Button>
            <Button
              size="sm"
              variant="secondary"
              disabled={!online || !stopped || busy}
              onClick={() =>
                void executeCommandWithToast(
                  'robot.line.calibrate',
                  { confirm: false, sample: 'line' },
                  { loadingTitle: 'Tomando muestra de línea…', successTitle: 'Línea registrada' }
                )
              }
            >
              2. Tomar línea
            </Button>
            <Button
              size="sm"
              variant="primary"
              disabled={!online || !stopped || busy}
              onClick={() =>
                void executeCommandWithToast(
                  'robot.line.calibrate',
                  { confirm: true },
                  { loadingTitle: 'Confirmando calibración…', successTitle: 'Línea calibrada' }
                )
              }
            >
              3. Confirmar
            </Button>
          </div>
        </div>

        {feedback && (
          <p role="status" className="text-xs text-[var(--text-muted)]">
            {feedback}
          </p>
        )}
      </CollapsibleCard>

    </div>
  );
}
