import { MotorSelector } from '../../components/ui/MotorSelector';
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { Chevron } from '../../components/ui/Chevron';
import { UiIcon } from '../../components/ui/UiIcon';
import { PageHeader } from '../../components/ui/PageHeader';
import React, { useCallback, useEffect, useRef, useState } from 'react';
import { useSearchParams } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { TaskSections } from '../../components/ui/TaskSections';
import { Afr2CommandError } from '../../protocol/afr2Client';
import { useConnection } from '../connection/ConnectionContext';
import { MecanumDrivePad } from '../../components/robot/MecanumDrivePad';
import { RangeNumberField } from '../../components/ui/RangeNumberField';
import {
  ControlMap,
  type RequestedMotion,
} from './ControlMap';
import { SequenceEditorPanel } from './sequences/SequenceEditorPanel';
import { CalibrationPanel } from './CalibrationPanel';
import { OdometryTestPanel } from './OdometryTestPanel';
import { GotoSimulatorPanel } from './goto/GotoSimulatorPanel';
import type { GotoCommand } from './goto/gotoSimulation';
import { ManualDriveOverlay, type ManualDirection } from './manual/ManualDriveOverlay';
import type { ModalOrigin } from '../../components/ui/FloatingModal';

type WheelId = 'FL' | 'FR' | 'RL' | 'RR';
type ControlTaskId = 'drive' | 'sequences' | 'wheel' | 'goto' | 'calibrate' | 'preferences';
/** AFR2-20261101-12 / VIS-053: Control se reparte en Conducir, Programas y Puesta a punto. */
export type ControlArea = 'drive' | 'programs' | 'setup';
const AREA_TASKS: Record<ControlArea, readonly ControlTaskId[]> = {
  drive: ['drive'], programs: ['sequences', 'goto'], setup: ['wheel', 'calibrate', 'preferences'],
};
const TASK_PARAM: Record<string, ControlTaskId> = {
  secuencias: 'sequences', goto: 'goto', ruedas: 'wheel', wheel: 'wheel', calibracion: 'calibrate', preferencias: 'preferences',
};
function initialTask(area: ControlArea, param: string | null): ControlTaskId {
  const requested = param ? TASK_PARAM[param] : undefined;
  return requested && AREA_TASKS[area].includes(requested) ? requested : AREA_TASKS[area][0];
}

export function ControlPage({ area = 'drive' }: { area?: ControlArea }) {
  const [entryParams, setEntryParams] = useSearchParams();
  const {
    wsState,
    megaLinkState,
    hasWebOperation,
    capabilities,
    triggerEmergencyStop,
    sendPwmCommand,
    executeCommandWithToast,
    stopRecord,
    setOperationActivity,
  } = useConnection();

  const [activeTask, setActiveTask] = useState<ControlTaskId>(() => initialTask(area, entryParams.get('tarea')));
  const [linearSpeedMmS, setLinearSpeedMmS] = useState(100);
  const [manualPowerPct,setManualPowerPct]=useState(50);
  const [turnPowerPct,setTurnPowerPct]=useState(50);
  const [activePadId, setActivePadId] = useState<string | null>(null);
  const [requestedMotion, setRequestedMotion] = useState<RequestedMotion | null>(null);
  const [driveError,setDriveError]=useState<string|null>(null);
  const [manualOverlayOpen, setManualOverlayOpen] = useState(false);
  const [manualOrigin, setManualOrigin] = useState<ModalOrigin | null>(null);

  const mounted = useRef(true);
  const latestStop = useRef(stopRecord.lastIssuedAtMs);latestStop.current=stopRecord.lastIssuedAtMs;

  const [testWheel, setTestWheel] = useState<WheelId>(() => {
    const wheel = entryParams.get('rueda');
    return wheel === 'FR' || wheel === 'RL' || wheel === 'RR' ? wheel : 'FL';
  });
  const [testDirection, setTestDirection] = useState<'forward' | 'reverse' | 'stop'>('forward');
  const [testPwmPct, setTestPwmPct] = useState(25);
  const [testDurationMs, setTestDurationMs] = useState(600);

  const [gotoCommands, setGotoCommands] = useState<GotoCommand[]>([]);
  const [gotoSimSpeedMmS, setGotoSimSpeedMmS] = useState(400);

  const maxSpeed = capabilities.mecanum_drive.limits?.maxSpeedMmS ?? 100;
  const maxPwm = Math.min(60, capabilities.wheel_test.limits?.maxPwmPct ?? 25);
  useEffect(() => {setLinearSpeedMmS(v => Math.min(v,maxSpeed));setTestPwmPct(v => Math.min(v,maxPwm));}, [maxSpeed,maxPwm]);

  // Estado para conectar la simulación de Secuencias con el ControlMap existente
  const holdTimerRef = useRef<ReturnType<typeof setInterval> | null>(null);
  const holdGeneration = useRef(0);
  const holdVector = useRef<{id:string;label:string;forwardPct:number;strafePct:number;turnPct:number}|null>(null);
  const consumedStop = useRef(stopRecord.lastIssuedAtMs);
  useEffect(() => {
    mounted.current = true;
    return () => {
      mounted.current = false;
      ++holdGeneration.current;
      setOperationActivity('manual', false);
      if (hasWebOperation()) void triggerEmergencyStop('route-leave');
    };
  }, [setOperationActivity, triggerEmergencyStop, hasWebOperation]);

  const canPrepareControl =
    wsState === 'ready' &&
    megaLinkState === 'ready' &&
    stopRecord.status !== 'sending' && stopRecord.status !== 'unconfirmed-offline';
  // AFR2-20261030-18: one permission and one reason for pad and touch.
  const driveBlockedReason = wsState !== 'ready' ? 'Falta conexión WebSocket con el ESP.'
    : megaLinkState !== 'ready' ? 'Falta enlace del ESP con el ATmega.'
    : stopRecord.status === 'sending' ? 'Esperando la confirmación de STOP.'
    : stopRecord.status === 'unconfirmed-offline' ? 'El último STOP sigue sin confirmar. Volvé a enviar STOP con el enlace activo.'
    : !capabilities.manual_pwm?.implemented ? capabilities.manual_pwm?.reason || 'El ESP todavía no informa PWM manual disponible.'
    : null;
  const canDrive = driveBlockedReason === null;

  const gotoAvailable =
    canPrepareControl && capabilities.mecanum_drive.implemented &&
    capabilities.wheel_odometry.present &&
    capabilities.wheel_odometry.implemented &&
    capabilities.wheel_odometry.calibrated &&
    capabilities.goto_relative.implemented;
  // AFR2-20261110-16: one reason for the route sender and the 90° turn presets.
  const gotoBlockedReason = gotoAvailable ? null
    : !canPrepareControl ? driveBlockedReason ?? 'Falta enlace con el auto.'
    : !capabilities.wheel_odometry.calibrated ? 'Calibrá la odometría Hall (pulsos por vuelta y geometría).'
    : capabilities.goto_relative.reason || capabilities.wheel_odometry.reason || 'GOTO no disponible en el auto.';

  const stopActiveHold = useCallback(
    (emitZero = true) => {
      ++holdGeneration.current;
      setOperationActivity('manual', false);
      const hadHold = holdVector.current !== null;
      holdVector.current = null;
      if (holdTimerRef.current) {
        clearInterval(holdTimerRef.current);
        holdTimerRef.current = null;
      }
      setActivePadId(null);
      setRequestedMotion(null);
      if (emitZero && hadHold) {
        void sendPwmCommand(0, 0, 0).catch(e => { if (!(e instanceof Afr2CommandError && (e.code === 'BUSY_IN_FLIGHT' || e.code === 'TIMEOUT'))) void triggerEmergencyStop('safety'); });
      }
    },
    [sendPwmCommand, setOperationActivity, triggerEmergencyStop]
  );

  const activeHoldStop = useRef(stopActiveHold);activeHoldStop.current=stopActiveHold;
  useEffect(() => {
    const onBlur = () => {
      activeHoldStop.current(false);
      if (hasWebOperation()) {
        void triggerEmergencyStop('focus-lost');
      }
    };
    const onVisibility = () => {
      if (document.visibilityState !== 'visible') {
        activeHoldStop.current(false);
        if (hasWebOperation()) void triggerEmergencyStop('focus-lost');
      }
    };

    window.addEventListener('blur', onBlur);
    document.addEventListener('visibilitychange', onVisibility);
    return () => {
      if (holdTimerRef.current) {
        clearInterval(holdTimerRef.current);
        holdTimerRef.current = null;
        void triggerEmergencyStop('route-leave');
      }
      window.removeEventListener('blur', onBlur);
      document.removeEventListener('visibilitychange', onVisibility);
    };
  }, [triggerEmergencyStop, hasWebOperation]);

  useEffect(() => {
    if (!canDrive && holdVector.current) { activeHoldStop.current(false); if (stopRecord.status !== 'sending') void triggerEmergencyStop('safety'); }
  }, [canDrive, stopRecord.status, triggerEmergencyStop]);

  useEffect(() => {
    if (consumedStop.current !== stopRecord.lastIssuedAtMs) {
      consumedStop.current = stopRecord.lastIssuedAtMs; activeHoldStop.current(false);
    }
  }, [stopRecord.lastIssuedAtMs, stopRecord.status]);

  const handleTaskChange = (nextTask: string) => {
    stopActiveHold(true);
    setManualOverlayOpen(false);
    const taskId = nextTask as ControlTaskId;
    setActiveTask(taskId);
  };

  const startHoldVector = async (id: string, label: string, vx: number, vy: number, wz: number) => {
    if (!canDrive || document.visibilityState !== 'visible') return;
    const next = { id, label, forwardPct:vx, strafePct:vy, turnPct:wz };
    // Direct command heartbeat; drag/speed changes replace the target.
    if (holdVector.current) {
      holdVector.current = next; setActivePadId(id); setRequestedMotion(null);
      return;
    }
    stopActiveHold(false);
    const generation = ++holdGeneration.current;
    const stopAtStart = latestStop.current;
    holdVector.current = next;
    setOperationActivity('manual', true);
    if (!mounted.current || generation !== holdGeneration.current || latestStop.current !== stopAtStart || !holdVector.current) return;
    setActivePadId(holdVector.current.id);
    setDriveError(null);
    setRequestedMotion(null);
    // AFR2-20261106-10: a busy UART slot or one late reply is transient; the 180 ms heartbeat
    // retries and the ATmega TTL (400 ms) still stops the car if commands really stop arriving.
    // AFR2-20261106-12: transient pressure is handled by latest-slot PWM and TTL, without emergency STOP.
    const send = () => {
      if (generation !== holdGeneration.current || !holdVector.current) return;
      const target = holdVector.current;
      void sendPwmCommand(target.forwardPct, target.strafePct, target.turnPct).then(() => undefined, e => {
        if (generation !== holdGeneration.current) return;
        if (e instanceof Afr2CommandError && (e.code === 'BUSY_IN_FLIGHT' || e.code === 'TIMEOUT')) return;
        stopActiveHold(false); setDriveError(e instanceof Error ? e.message : 'Consigna rechazada.');
        void triggerEmergencyStop('safety');
      });
    };
    send();
    holdTimerRef.current = setInterval(() => {
      send();
    }, 180);
  };

  const prepareAction = () => {
    stopActiveHold(true);
    return mounted.current && canPrepareControl && document.visibilityState === 'visible';
  };

  const handleWheelTestSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!prepareAction()) return;
    await executeCommandWithToast(
      'robot.test.wheel',
      {
        wheel: testWheel,
        direction: testDirection,
        pwmPct: testPwmPct,
        durationMs: testDurationMs,
      },
      {
        loadingTitle: `Probando rueda ${testWheel} (${testDirection}, ${testPwmPct} %)...`,
        successTitle: `Prueba de rueda ${testWheel} completada`,
        acceptedTitle: `Prueba de rueda ${testWheel} iniciada (${testDurationMs} ms)`,
      }
    );
  };

  // AFR2-20261106-08: touch twist with Hall -> in-place GOTO (x=y=0), closed by wheel odometry.
  const handleTurnTo = async (thetaMrad: number) => {
    if (!prepareAction()) return;
    const degrees = Math.round(thetaMrad * 180 / Math.PI / 1000);
    await executeCommandWithToast(
      'robot.goto.start',
      { xMm: 0, yMm: 0, thetaMrad, maxSpeedMmS: Math.min(linearSpeedMmS, maxSpeed) },
      {
        loadingTitle: `Girando ${degrees > 0 ? '+' : ''}${degrees}° con odometría Hall...`,
        successTitle: 'Giro completado',
        acceptedTitle: `Giro de ${degrees}° aceptado (en progreso)`,
      }
    );
  };

  const controlTasks = [
          {
            id: 'drive',
            label: 'Movimiento',
            content: (
              <div className="space-y-5">
                {!canDrive && (
                  <StatusNotice
                    tone="info"
                    title={wsState !== 'ready' || megaLinkState !== 'ready' ? 'Simulación local disponible' : 'Movimiento manual no disponible'}
                    badge="Movimiento manual"
                  >
                    {wsState !== 'ready' || megaLinkState !== 'ready'
                      ? 'Abrí Control táctil para simular sin conexión al robot. Las órdenes físicas requieren enlace Web↔ESP↔ATmega2560.'
                      : driveBlockedReason}
                  </StatusNotice>
                )}

                <div className="afr2-manual-heading">
                  <h2>Conducción manual</h2>
                  <Button variant="primary" aria-haspopup="dialog" aria-expanded={manualOverlayOpen} icon={<UiIcon name="touch" />}
                    onClick={event => { stopActiveHold(true); setDriveError(null); setManualOrigin({ element: event.currentTarget, keyboard: event.detail === 0 }); setManualOverlayOpen(true); }}>
                    Control táctil
                  </Button>
                </div>

                <div className="grid grid-cols-1 lg:grid-cols-12 gap-6 items-start">
                  <div className="lg:col-span-7 afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
                    <h2 className="text-base font-semibold text-[var(--text)]">Mover el robot</h2>
                    <MecanumDrivePad disabled={!canDrive} activeId={activePadId}
                      onStart={intent => { void startHoldVector(intent.id, intent.label, intent.vx * manualPowerPct, intent.vy * manualPowerPct, intent.wz * turnPowerPct); }}
                      onStop={() => stopActiveHold(true)}
                      turnMenu={{ available: gotoAvailable, reason: gotoBlockedReason, onTurnTo: thetaMrad => { void handleTurnTo(thetaMrad); } }} />
                  </div>
                  {/* AFR2-20261110-16: el modo operativo pasó a la pill de modo de la cabecera. */}
                  <div className="lg:col-span-5 afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-5">
                    <h2 className="text-base font-semibold text-[var(--text)]">
                      Parámetros
                    </h2>

                    <RangeNumberField label="Potencia de traslación" value={manualPowerPct} min={0} max={100} step={5} unit="% del límite PWM" onChange={setManualPowerPct} />
                    <RangeNumberField label="Potencia de giro" value={turnPowerPct} min={10} max={100} step={5} unit="% del límite PWM" onChange={setTurnPowerPct} />
                  </div>
                </div>
              </div>
            ),
          },
          {
            id: 'sequences',
            label: 'Secuencias',
            content: <SequenceEditorPanel preview={feed => <ControlMap
        requestedMotion={requestedMotion}
        onViewChange={() => stopActiveHold(true)}
        simulationFeed={feed}
      />} />,
          },
          {
            id: 'wheel',
            label: 'Prueba de rueda',
            content: (
              <div className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4 max-w-2xl">
                <div>
                  <h2 className="text-base font-semibold text-[var(--text)]">
                    Prueba individual de rueda (`robot.test.wheel`)
                  </h2>
                  <p className="text-xs text-[var(--text-muted)] mt-1">
                    Acciona una sola rueda con límite de PWM (≤ 60 %) y duración acotada (100–2000 ms).
                  </p>
                </div>

                <form onSubmit={handleWheelTestSubmit} className="space-y-4">
                  <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                    <div>
                      <MotorSelector value={testWheel} onChange={setTestWheel} />
                    </div>

                    <div>
                      <label htmlFor="wheel-dir-select" className="block text-xs font-medium text-[var(--text-muted)] mb-1">
                        Sentido
                      </label>
                      <span className="afr2-select-wrap"><select
                        id="wheel-dir-select"
                        value={testDirection}
                        onChange={(e) =>
                          setTestDirection(e.target.value as 'forward' | 'reverse' | 'stop')
                        }
                        className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-base text-[var(--text)]"
                      >
                        <option value="forward">Avance (Forward)</option>
                        <option value="reverse">Reversa (Reverse)</option>
                        <option value="stop">Detener rueda</option>
                      </select><Chevron /></span>
                    </div>
                  </div>

                  <div className="grid grid-cols-1 sm:grid-cols-2 gap-4">
                    <div>
                      <label htmlFor="wheel-pwm-input" className="block text-xs font-medium text-[var(--text-muted)] mb-1">
                        PWM (%) — máx. 60 %
                      </label>
                      <input
                        id="wheel-pwm-input"
                        type="number"
                        min={0}
                        max={maxPwm}
                        value={testPwmPct}
                        onChange={(e) =>
                          setTestPwmPct(Math.min(maxPwm, Math.max(0, Number(e.target.value))))
                        }
                        className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-base font-mono text-[var(--text)]"
                      />
                    </div>
                    <div>
                      <label htmlFor="wheel-dur-input" className="block text-xs font-medium text-[var(--text-muted)] mb-1">
                        Duración (ms)
                      </label>
                      <input
                        id="wheel-dur-input"
                        type="number"
                        min={100}
                        max={2000}
                        step={100}
                        value={testDurationMs}
                        onChange={(e) =>
                          setTestDurationMs(Math.min(2000, Math.max(100, Number(e.target.value))))
                        }
                        className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-base font-mono text-[var(--text)]"
                      />
                    </div>
                  </div>

                  <Button
                    variant="primary"
                    size="md"
                    type="submit"
                    disabled={!canPrepareControl || !capabilities.wheel_test.implemented}
                  >
                    Ejecutar prueba en {testWheel}
                  </Button>
                </form>
              </div>
            ),
          },
          {
            id: 'goto',
            label: 'GOTO',
            content: (
              <div className="afr2-page-stack">
              <GotoSimulatorPanel commands={gotoCommands} onCommandsChange={setGotoCommands} speedMmS={gotoSimSpeedMmS} onSpeedChange={setGotoSimSpeedMmS}
                robot={{ available: gotoAvailable, reason: gotoBlockedReason, maxSpeedMmS: maxSpeed }} />
              </div>
            ),
          },
          {
            id: 'calibrate',
            label: 'Calibración',
            content: (
              <>
                <CalibrationPanel />
                <OdometryTestPanel />
              </>
            ),
          },
          {
            // AFR2-20261101-12: parámetros del robot en EEPROM; antes en Configuración → Sistema.
            id: 'preferences',
            label: 'Preferencias',
            content: <CalibrationPanel preferences />,
          },

  ];
  // La misma instancia sirve a las tres rutas (React conserva el estado al cambiar de ruta),
  // así los borradores de GOTO y prueba de rueda sobreviven; al cambiar de área se detiene el movimiento.
  const previousArea = useRef(area);
  useEffect(() => {
    if (previousArea.current === area) return;
    previousArea.current = area;
    handleTaskChange(initialTask(area, entryParams.get('tarea')));
  // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [area]);
  const task = (id: ControlTaskId) => controlTasks.find(item => item.id === id)?.content;
  const changeTask = (id: string) => {
    if (id === activeTask) return;
    handleTaskChange(id);
    const param = Object.entries(TASK_PARAM).find(([, value]) => value === id)?.[0];
    if (param) setEntryParams(previous => { const next = new URLSearchParams(previous); next.set('tarea', param); return next; }, { replace: true });
  };
  const header = area === 'drive'
    ? <PageHeader title="Conducir" description="Mové la base en cualquier dirección y mirá los sensores en vivo." />
    : area === 'programs'
      ? <PageHeader title="Programas" description="Armá recorridos, simulalos y ejecutalos en el robot." />
      : <PageHeader title="Puesta a punto" description="Probá cada rueda y calibrá antes de operar." />;
  return (
    <div className="afr2-route-view afr2-control-workspace">
      {header}
      {driveError && <StatusNotice tone="error" title="Consigna sin confirmar">{driveError}</StatusNotice>}
      {area === 'drive' && task('drive')}
      {area === 'programs' && <TaskSections ariaLabel="Tipos de programa" value={activeTask} onValueChange={changeTask} items={[
        { id: 'sequences', label: 'Secuencias', content: task('sequences') },
        { id: 'goto', label: 'GOTO', content: task('goto') },
      ]} />}
      {area === 'setup' && <TaskSections ariaLabel="Puesta a punto" value={activeTask} onValueChange={changeTask} items={[
        { id: 'wheel', label: 'Ruedas', content: task('wheel') },
        { id: 'calibrate', label: 'Calibración', content: task('calibrate') },
        { id: 'preferences', label: 'Preferencias', content: task('preferences') },
      ]} />}

      <ManualDriveOverlay open={manualOverlayOpen} origin={manualOrigin} onClose={() => setManualOverlayOpen(false)}
        onRelease={() => stopActiveHold(true)} canDrive={canDrive} blockedReason={driveBlockedReason} speed={manualPowerPct} maxSpeed={100}
        onSpeedChange={setManualPowerPct} stopSignal={stopRecord.lastIssuedAtMs} error={driveError}
        onDirectionChange={(direction: ManualDirection) => { const speed = Math.floor(manualPowerPct / (Math.abs(direction.vx) + Math.abs(direction.vy))); void startHoldVector(`touch-${direction.id}`, direction.label, direction.vx * speed, direction.vy * speed, 0); }}
        hallTurnAvailable={gotoAvailable} hallTurnReason={gotoBlockedReason}
        turnPower={turnPowerPct} onTurnPowerChange={setTurnPowerPct}
        onTurnTo={(thetaMrad: number) => { void handleTurnTo(thetaMrad); }}
        onTurn={(side: number) => { void startHoldVector(side > 0 ? 'touch-ccw' : 'touch-cw', side > 0 ? 'girar a la izquierda' : 'girar a la derecha', 0, 0, Math.sign(side) * turnPowerPct); }} />
    </div>
  );
}
