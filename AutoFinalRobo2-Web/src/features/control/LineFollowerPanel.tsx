import { useEffect, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { UiIcon } from '../../components/ui/UiIcon';
import { useConnection } from '../connection/ConnectionContext';
import { Link } from 'react-router-dom';
import { LineFollowerLiveView } from './LineFollowerLiveView';
import { analyzeLine, LINE_CENTER_TEXT, LINE_ORIENTATION_TEXT } from '../sensors/lineAnalysis';

/**
 * AFR2-20261116-01 — Panel operativo y de ajuste del Seguidor de Línea (`AFR2_LINE`).
 *
 * Permite supervisar los 5 canales IR1..IR5, iniciar/detener el modo `line-follow`,
 * ajustar en RAM la velocidad de avance (`AFR2_A_LINE_SPEED`, acción 6) y la ganancia
 * proporcional (`AFR2_A_LINE_KP`, acción 7), y ejecutar el flujo de calibración de suelo/línea.
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

  const [lineSpeedMmS, setLineSpeedMmS] = useState<number>(80);
  const [lineKp, setLineKp] = useState<number>(100);
  const [busy, setBusy] = useState<boolean>(false);
  const [feedback, setFeedback] = useState<string | null>(null);

  const online = wsState === 'ready' && megaLinkState === 'ready';
  const isLineMode = robotState?.mode === 'line-follow';
  const stopped = robotState?.stopped ?? true;
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

  const toggleLineFollower = async () => {
    if (!online || busy) return;
    if (isLineMode) {
      await triggerEmergencyStop('line-stop');
      return;
    }
    setBusy(true);
    try {
      await executeCommandWithToast(
        'robot.mode',
        { mode: 'line-follow' },
        {
          loadingTitle: 'Iniciando seguidor de línea…',
          successTitle: 'Seguidor de línea en marcha',
        }
      );
    } finally {
      setBusy(false);
    }
  };

  const bits = sensors.line.valid && sensors.line.bits !== null ? sensors.line.bits : 0;
  const channels = sensors.line.channels ?? [false, false, false, false, false];
  const analysis = analyzeLine(bits, []);

  return (
    <div className="afr2-page-stack">
      <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">
        <section className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
          <div className="flex items-center justify-between gap-2">
            <div>
              <h2 className="text-base font-semibold text-[var(--text)]">
                Seguidor de línea (`AFR2_LINE`)
              </h2>
              <p className="text-xs text-[var(--text-muted)] mt-0.5">
                Control proporcional sobre los 5 sensores IR1…IR5 con recuperación de pérdida breve (250 ms) y parada en cruce transversal (150 ms).
              </p>
            </div>
            <span className="afr2-status-pill" data-state={isLineMode ? 'ready' : 'idle'}>
              {isLineMode ? 'Siguiendo línea' : 'En espera'}
            </span>
          </div>

          {!canStartLine && !isLineMode && (
            <StatusNotice tone="info" title="Requisitos para iniciar el seguidor de línea">
              {!online
                ? 'Conectá el WebSocket al ESP y verificá el enlace con el ATmega.'
                : !robotState?.motorsEnabled
                  ? 'Habilitá el control de motores (acción 36 o desde Controles rápidos).'
                  : !capabilities.wheel_odometry.calibrated
                    ? 'Se requiere odometría Hall calibrada y pose válida.'
                    : capabilities.line_follow.reason || 'Calibrá los 5 sensores IR sobre el fondo y la línea.'}
            </StatusNotice>
          )}

          <div className="grid grid-cols-5 gap-2" aria-label="Estado de los 5 sensores IR">
            {channels.map((active, idx) => (
              <div
                key={idx}
                className={`p-2.5 rounded-[var(--radius-sm)] border text-center font-mono text-xs ${
                  active
                    ? 'border-[var(--action)] bg-[var(--action)]/15 text-[var(--text)] font-bold'
                    : 'border-[var(--border-default)] bg-[var(--surface-subtle)] text-[var(--text-muted)]'
                }`}
              >
                <div>IR{idx + 1}</div>
                <div className="mt-1">{active ? 'LÍNEA' : 'FONDO'}</div>
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
            <Button
              variant={isLineMode ? 'emergency' : 'primary'}
              disabled={!online || busy || (!isLineMode && !canStartLine)}
              icon={<UiIcon name={isLineMode ? 'stop' : 'play'} size={16} />}
              onClick={() => void toggleLineFollower()}
            >
              {isLineMode ? 'Detener seguidor de línea' : 'Iniciar seguidor de línea'}
            </Button>
          </div>
        </section>

        <section className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
          <h3 className="text-base font-semibold text-[var(--text)]">
            Ajuste y calibración de línea
          </h3>
          <p className="text-xs text-[var(--text-muted)]">
            Con el auto detenido podés ajustar la velocidad de avance y la ganancia de giro, o tomar muestras de fondo y línea.
          </p>

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
        </section>
      </div>

      {showView && <section className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-3">
        <LineFollowerLiveView speedMmS={lineSpeedMmS} />
        <Link className="afr2-developer-back" to="/seguidor-linea">Abrir la vista completa del seguidor</Link>
      </section>}
    </div>
  );
}
