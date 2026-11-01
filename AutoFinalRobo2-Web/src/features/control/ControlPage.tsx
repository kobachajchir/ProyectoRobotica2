import { MotorSelector } from '../../components/ui/MotorSelector';
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { Chevron } from '../../components/ui/Chevron';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { CollapsiblePanel } from '../../components/ui/CollapsiblePanel';
import React, { useCallback, useEffect, useRef, useState } from 'react';
import { useSearchParams } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { Disclosure } from '../../components/ui/Disclosure';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { TaskSections } from '../../components/ui/TaskSections';
import type { RobotPose } from '../../protocol/robotState';
import { useConnection } from '../connection/ConnectionContext';
import { MecanumDrivePad } from '../../components/robot/MecanumDrivePad';
import { RangeNumberField } from '../../components/ui/RangeNumberField';
import {
  ControlMap,
  type RequestedMotion,
  type SimulationMapFeed,
} from './ControlMap';
import type { TrailPoint } from './controlMapTypes';
import type { RobotWheelValues } from '../../components/three/robotWheelMotion';
import type { SimulationSnapshotAtCursor } from './sequences/mecanumKinematics';
import { SequenceEditorPanel } from './sequences/SequenceEditorPanel';
import { CalibrationPanel } from './CalibrationPanel';
import { GotoSimulatorPanel } from './goto/GotoSimulatorPanel';
import type { GotoCommand } from './goto/gotoSimulation';
import { ManualDriveOverlay, type ManualDirection } from './manual/ManualDriveOverlay';
import type { ModalOrigin } from '../../components/ui/FloatingModal';

type WheelId = 'FL' | 'FR' | 'RL' | 'RR';
type RobotModeOption = 'idle' | 'manual' | 'line-follow';
type ControlTaskId = 'drive' | 'sequences' | 'wheel' | 'goto' | 'calibrate';

export function ControlPage() {
  const [entryParams] = useSearchParams();
  const {
    wsState,
    megaLinkState,
    hasWebOperation,
    capabilities,
    robotState,
    triggerEmergencyStop,
    sendPwmCommand,
    executeCommandWithToast,
    stopRecord,
    setOperationActivity,
  } = useConnection();

  const [activeTask, setActiveTask] = useState<ControlTaskId>(() => entryParams.get('tarea') === 'wheel' ? 'wheel' : 'drive');
  const [cardExpanded, setCardExpanded] = useState(true);
  const [linearSpeedMmS, setLinearSpeedMmS] = useState(100);
  const [manualPowerPct,setManualPowerPct]=useState(50);
  const [turnPowerPct,setTurnPowerPct]=useState(50);
  const [activePadId, setActivePadId] = useState<string | null>(null);
  const [requestedMotion, setRequestedMotion] = useState<RequestedMotion | null>(null);
  const [selectedMode, setSelectedMode] = useState<RobotModeOption>('idle');
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

  const [gotoXMm, setGotoXMm] = useState(500);
  const [gotoYMm, setGotoYMm] = useState(0);
  const [gotoThetaMrad, setGotoThetaMrad] = useState(0);
  const [gotoCommands, setGotoCommands] = useState<GotoCommand[]>([]);
  const [gotoSimSpeedMmS, setGotoSimSpeedMmS] = useState(400);

  const maxSpeed = capabilities.mecanum_drive.limits?.maxSpeedMmS ?? 100;
  const maxPwm = Math.min(60, capabilities.wheel_test.limits?.maxPwmPct ?? 25);
  useEffect(() => {setLinearSpeedMmS(v => Math.min(v,maxSpeed));setTestPwmPct(v => Math.min(v,maxPwm));}, [maxSpeed,maxPwm]);

  // Estado para conectar la simulacion de Secuencias con el ControlMap existente
  const [simMapPose, setSimMapPose] = useState<RobotPose | null>(null);
  const [simMapTrail, setSimMapTrail] = useState<TrailPoint[]>([]);
  const [simWheelAngles, setSimWheelAngles] = useState<RobotWheelValues>();
  const [simCursorMs, setSimCursorMs] = useState(0);
  const [simTotalDurationMs, setSimTotalDurationMs] = useState(0);
  const [simIsPlaying, setSimIsPlaying] = useState(false);
  const [simProfileSummary, setSimProfileSummary] = useState('L=300 mm · Vmax=800 mm/s (no calibrado)');

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
  const driveBlockedReason = wsState !== 'ready' ? 'Falta conexion WebSocket con el ESP.'
    : megaLinkState !== 'ready' ? 'Falta enlace del ESP con el ATmega.'
    : stopRecord.status === 'sending' ? 'Esperando la confirmacion de STOP.'
    : stopRecord.status === 'unconfirmed-offline' ? 'El ultimo STOP sigue sin confirmar. Volve a enviar STOP con el enlace activo.'
    : !capabilities.manual_pwm?.implemented ? capabilities.manual_pwm?.reason || 'El ESP todavia no informa PWM manual disponible.'
    : null;
  const canDrive = driveBlockedReason === null;

  const gotoAvailable =
    canPrepareControl && capabilities.mecanum_drive.implemented &&
    capabilities.wheel_odometry.present &&
    capabilities.wheel_odometry.implemented &&
    capabilities.wheel_odometry.calibrated &&
    capabilities.goto_relative.implemented;

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
        void sendPwmCommand(0, 0, 0).catch(() => { void triggerEmergencyStop('safety'); });
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
    setCardExpanded(true);
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
    const send = () => {
      if (generation !== holdGeneration.current || !holdVector.current) return;
      const target = holdVector.current;
      void sendPwmCommand(target.forwardPct, target.strafePct, target.turnPct).catch(e => {
        if (generation !== holdGeneration.current) return;
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

  const handleModeSubmit = async (mode: RobotModeOption) => {
    if (!prepareAction()) return;
    setSelectedMode(mode);
    await executeCommandWithToast(
      'robot.mode',
      { mode },
      {
        loadingTitle: `Cambiando modo a ${mode}...`,
        successTitle: `Modo ${mode} aplicado`,
      }
    );
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

  const handleGotoSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!prepareAction()) return;
    await executeCommandWithToast(
      'robot.goto.start',
      {
        xMm: gotoXMm,
        yMm: gotoYMm,
        thetaMrad: gotoThetaMrad,
        maxSpeedMmS: Math.min(linearSpeedMmS,maxSpeed),
      },
      {
        loadingTitle: 'Enviando objetivo relativo GOTO...',
        successTitle: 'Mision GOTO completada',
        acceptedTitle: 'Mision GOTO aceptada (en progreso)',
      }
    );
  };

  const handleSimulationUpdate = useCallback(
    (snapshot: SimulationSnapshotAtCursor, isPlaying: boolean, profileSummary: string) => {
      setSimCursorMs(snapshot.cursorMs);
      setSimTotalDurationMs(snapshot.totalDurationMs);
      setSimIsPlaying(isPlaying);
      setSimProfileSummary(profileSummary);
      setSimWheelAngles(snapshot.wheelAngles);
      setSimMapPose({
        schema: 'afr2-pose-1',
        frameId: 'sim-mecanum-local',
        sequence: Math.round(snapshot.cursorMs / 10),
        xMm: snapshot.xMm,
        yMm: snapshot.yMm,
        thetaMrad: snapshot.thetaMrad,
        ageMs: 0,
      });
      setSimMapTrail(
        snapshot.trail.map((pt, idx) => ({
          ...pt,
          segmentStart: idx === 0,
        }))
      );
    },
    []
  );

  const simulationFeed: SimulationMapFeed | null =
    activeTask === 'sequences'
      ? {
          pose: simMapPose,
          trail: simMapTrail,
          cursorMs: simCursorMs,
          totalDurationMs: simTotalDurationMs,
          isPlaying: simIsPlaying,
          profileSummary: simProfileSummary,
          wheelAngles: simWheelAngles,
        }
      : null;

  const controlTasks = [
          {
            id: 'drive',
            label: 'Movimiento',
            content: (
              <div className="space-y-5">
                {!canDrive && (
                  <StatusNotice
                    tone="info"
                    title={wsState !== 'ready' || megaLinkState !== 'ready' ? 'Simulacion local disponible' : 'Movimiento manual no disponible'}
                    badge="Movimiento manual"
                  >
                    {wsState !== 'ready' || megaLinkState !== 'ready'
                      ? 'Abri Control tactil para simular sin conexion al robot. Las ordenes fisicas requieren enlace Web↔ESP↔ATmega2560.'
                      : driveBlockedReason}
                  </StatusNotice>
                )}

                <div className="afr2-manual-heading">
                  <h2>Conduccion manual</h2>
                  <Button variant="primary" aria-haspopup="dialog" aria-expanded={manualOverlayOpen} icon={<svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.7" aria-hidden="true"><path d="M8 3H3v5m13-5h5v5M3 16v5h5m13-5v5h-5" /></svg>}
                    onClick={event => { stopActiveHold(true); setDriveError(null); setManualOrigin({ element: event.currentTarget, keyboard: event.detail === 0 }); setManualOverlayOpen(true); }}>
                    Control tactil
                  </Button>
                </div>

                <div className="grid grid-cols-1 lg:grid-cols-12 gap-6 items-start">
                  <div className="lg:col-span-7 afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
                    <h2 className="text-base font-semibold text-[var(--text)]">Mover el robot</h2>
                    <MecanumDrivePad disabled={!canDrive} activeId={activePadId}
                      onStart={intent => { void startHoldVector(intent.id, intent.label, intent.vx * manualPowerPct, intent.vy * manualPowerPct, intent.wz * turnPowerPct); }}
                      onStop={() => stopActiveHold(true)} />
                  </div>
                  {/* Parametros de velocidad y modo operativo */}
                  <div className="lg:col-span-5 afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-5">
                    <h2 className="text-base font-semibold text-[var(--text)]">
                      Parametros y modo operativo
                    </h2>

                    <RangeNumberField label="Potencia de traslacion" value={manualPowerPct} min={0} max={100} step={5} unit="% del limite PWM" onChange={setManualPowerPct} />
                    <RangeNumberField label="Potencia de giro" value={turnPowerPct} min={10} max={100} step={5} unit="% del limite PWM" onChange={setTurnPowerPct} />
                    <div className="space-y-2 pt-2 border-t border-[var(--border-default)]">
                      <span className="block text-xs font-medium text-[var(--text-muted)]">
                        Modo operativo
                      </span>
                      <div className="afr2-segmented" role="group" aria-label="Modo operativo del robot">
                        {(['idle', 'manual', 'line-follow'] as RobotModeOption[]).map((mode) => (
                          <button
                            key={mode}
                            type="button"
                            disabled={!canPrepareControl || (mode === 'manual' && !capabilities.manual_pwm?.implemented) || (mode === 'line-follow' && !capabilities.line_follow.implemented)}
                            aria-pressed={selectedMode === mode}
                            onClick={() => void handleModeSubmit(mode)}
                            className="afr2-segmented-btn disabled:opacity-55 disabled:cursor-not-allowed"
                          >
                            {mode === 'idle' ? 'Reposo' : mode === 'manual' ? 'Manual' : 'Linea'}
                          </button>
                        ))}
                      </div>
                    </div>
                  </div>
                </div>
              </div>
            ),
          },
          {
            id: 'sequences',
            label: 'Secuencias',
            content: <SequenceEditorPanel onSimulationChange={handleSimulationUpdate} preview={<ControlMap
        requestedMotion={requestedMotion}
        onViewChange={() => stopActiveHold(true)}
        simulationFeed={simulationFeed}
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
                    Acciona una sola rueda con limite de PWM (≤ 60 %) y duracion acotada (100–2000 ms).
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
                        PWM (%) — max. 60 %
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
                        Duracion (ms)
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
              <GotoSimulatorPanel commands={gotoCommands} onCommandsChange={setGotoCommands} speedMmS={gotoSimSpeedMmS} onSpeedChange={setGotoSimSpeedMmS} />
              <Disclosure title="Enviar un objetivo al robot" meta="Requiere odometria y calibracion">
              <div className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4 max-w-2xl">
                <div>
                  <h2 className="text-base font-semibold text-[var(--text)]">
                    Posicionamiento relativo (`robot.goto.start`)
                  </h2>
                  <p className="text-xs text-[var(--text-muted)] mt-1">
                    Desplazamiento relativo (`xMm`, `yMm`, `thetaMrad`) sujeto a odometria de ruedas.
                  </p>
                </div>

                {!gotoAvailable && (
                  <StatusNotice
                    tone="warning"
                    title="GOTO deshabilitado"
                    badge="Sin odometria"
                  >
                    {capabilities.goto_relative.reason} ({capabilities.wheel_odometry.reason})
                  </StatusNotice>
                )}

                <form onSubmit={handleGotoSubmit} className="space-y-4">
                  <div className="grid grid-cols-1 sm:grid-cols-3 gap-3">
                    <div>
                      <label htmlFor="goto-x" className="block text-xs font-medium text-[var(--text-muted)] mb-1">
                        Î”X (mm)
                      </label>
                      <input
                        id="goto-x"
                        type="number"
                        min={-5000}
                        max={5000}
                        disabled={!gotoAvailable}
                        value={gotoXMm}
                        onChange={(e) => setGotoXMm(Number(e.target.value))}
                        className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-base font-mono text-[var(--text)] disabled:opacity-55"
                      />
                    </div>
                    <div>
                      <label htmlFor="goto-y" className="block text-xs font-medium text-[var(--text-muted)] mb-1">
                        Î”Y (mm)
                      </label>
                      <input
                        id="goto-y"
                        type="number"
                        min={-5000}
                        max={5000}
                        disabled={!gotoAvailable}
                        value={gotoYMm}
                        onChange={(e) => setGotoYMm(Number(e.target.value))}
                        className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-base font-mono text-[var(--text)] disabled:opacity-55"
                      />
                    </div>
                    <div>
                      <label htmlFor="goto-theta" className="block text-xs font-medium text-[var(--text-muted)] mb-1">
                        Î”Î¸ (mrad)
                      </label>
                      <input
                        id="goto-theta"
                        type="number"
                        min={-3140}
                        max={3140}
                        disabled={!gotoAvailable}
                        value={gotoThetaMrad}
                        onChange={(e) => setGotoThetaMrad(Number(e.target.value))}
                        className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-base font-mono text-[var(--text)] disabled:opacity-55"
                      />
                    </div>
                  </div>

                  <div className="flex flex-wrap gap-2">
                    <Button
                      variant="primary"
                      size="md"
                      type="submit"
                      disabled={!gotoAvailable}
                    >
                      Iniciar GOTO
                    </Button>
                    <Button
                      variant="secondary"
                      size="md"
                      type="button"
                      disabled={wsState !== 'ready'}
                      onClick={() =>
                        void executeCommandWithToast(
                          'robot.goto.cancel',
                          {},
                          {
                            loadingTitle: 'Cancelando mision GOTO...',
                            successTitle: 'Mision GOTO cancelada',
                          }
                        )
                      }
                    >
                      Cancelar GOTO
                    </Button>
                  </div>
                </form>
              </div>
              </Disclosure>
              </div>
            ),
          },
          {
            id: 'calibrate',
            label: 'Calibracion',
            content: (
              <CalibrationPanel />
            ),
          },

  ];
  const controlGroup = activeTask === 'drive' || activeTask === 'sequences' ? 'movement' : activeTask === 'wheel' || activeTask === 'calibrate' ? 'checks' : 'goto';
  const renderControlCard = (id: ControlTaskId, title: string) => <CollapsiblePanel
    className={`afr2-control-card afr2-control-card--${id}`} title={title} expanded={cardExpanded && activeTask === id}
    onToggle={() => {
      if (cardExpanded && activeTask === id) { stopActiveHold(true); setManualOverlayOpen(false); setCardExpanded(false); }
      else handleTaskChange(id);
    }}>
    {controlTasks.find(task => task.id === id)?.content}
  </CollapsiblePanel>;
  return (
    <div className="afr2-route-view afr2-control-workspace">
      <SectionHeader title="Control" description="Movimientos, secuencias y simulacion desde la Web." />
      {driveError && <StatusNotice tone="error" title="Consigna sin confirmar">{driveError}</StatusNotice>}
      {robotState && <p className="afr2-help" role="status">Modo AVR: {robotState.mode} · GOTO: {robotState.gotoState} · Salidas apagadas: {robotState.stopped === null ? 'desconocido' : robotState.stopped ? 'si' : 'no'} · Fallo: {robotState.fault ?? 'desconocido'}</p>}



      <TaskSections ariaLabel="Tareas de Control" activeId={controlGroup}
        onChange={group => { if (group !== controlGroup) handleTaskChange(group === 'movement' ? 'drive' : group === 'checks' ? 'wheel' : 'goto'); }}
        items={[
          { id: 'movement', label: 'Movimiento', content: <div className="afr2-control-cards">
            {renderControlCard('drive', 'Conduccion manual')}
            {renderControlCard('sequences', 'Secuencias')}
          </div> },
          { id: 'checks', label: 'Pruebas y calibracion', content: <div className="afr2-control-cards">
            {renderControlCard('wheel', 'Prueba de rueda')}
            {renderControlCard('calibrate', 'Calibracion')}
          </div> },
          { id: 'goto', label: 'GOTO', content: controlTasks.find(task => task.id === 'goto')?.content },
        ]}
      />

      <ManualDriveOverlay open={manualOverlayOpen} origin={manualOrigin} onClose={() => setManualOverlayOpen(false)}
        onRelease={() => stopActiveHold(true)} canDrive={canDrive} blockedReason={driveBlockedReason} speed={manualPowerPct} maxSpeed={100}
        onSpeedChange={setManualPowerPct} stopSignal={stopRecord.lastIssuedAtMs} error={driveError}
        onDirectionChange={(direction: ManualDirection) => { const speed = Math.floor(manualPowerPct / (Math.abs(direction.vx) + Math.abs(direction.vy))); void startHoldVector(`touch-${direction.id}`, direction.label, direction.vx * speed, direction.vy * speed, 0); }} />
      <Disclosure
        title="Supervision del control Web"
        meta="ATmega2560 autoritativo"
      >
        <ul className="list-disc pl-5 space-y-1 text-xs">
          <li>
            La supervision se prepara automaticamente al operar desde la Web. Una orden pierde vigencia si se interrumpe la conexion o deja de renovarse.
          </li>
          <li>
            La perdida de foco, ocultamiento de pestaña, cambio de tarea o desconexion detiene inmediatamente el envio de consignas, cancela cualquier secuencia fisica activa y emite <code>robot.stop</code>.
          </li>
          <li>
            La confirmacion de parada fisica proviene unicamente del productor autoritativo en el ATmega2560.
          </li>
        </ul>
      </Disclosure>
    </div>
  );
}
