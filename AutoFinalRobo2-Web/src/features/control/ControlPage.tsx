import { Chevron } from '../../components/ui/Chevron';
import { SectionHeader } from '../../components/ui/SectionHeader';
import React, { useCallback, useEffect, useRef, useState } from 'react';
import { useSearchParams } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { Disclosure } from '../../components/ui/Disclosure';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { TaskSections } from '../../components/ui/TaskSections';
import type { RobotPose } from '../../protocol/robotState';
import { useConnection } from '../connection/ConnectionContext';
import { ChassisSpinGraphic } from './ChassisSpinGraphic';
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

interface VectorPreset {
  id: string;
  label: string;
  symbol: string;
  vxFactor: -1 | 0 | 1;
  vyFactor: -1 | 0 | 1;
}

const MECANUM_VECTORS: VectorPreset[] = [
  { id: 'nw', label: 'Diag. Izq. Adelante', symbol: '↖', vxFactor: 1, vyFactor: 1 },
  { id: 'n', label: 'Adelante', symbol: '↑', vxFactor: 1, vyFactor: 0 },
  { id: 'ne', label: 'Diag. Der. Adelante', symbol: '↗', vxFactor: 1, vyFactor: -1 },
  { id: 'w', label: 'Lateral Izquierda', symbol: '←', vxFactor: 0, vyFactor: 1 },
  { id: 'stop', label: 'Detener consigna', symbol: '■', vxFactor: 0, vyFactor: 0 },
  { id: 'e', label: 'Lateral Derecha', symbol: '→', vxFactor: 0, vyFactor: -1 },
  { id: 'sw', label: 'Diag. Izq. Atras', symbol: '↙', vxFactor: -1, vyFactor: 1 },
  { id: 's', label: 'Atras', symbol: '↓', vxFactor: -1, vyFactor: 0 },
  { id: 'se', label: 'Diag. Der. Atras', symbol: '↘', vxFactor: -1, vyFactor: -1 },
];

export function ControlPage() {
  const [entryParams] = useSearchParams();
  const {
    wsState,
    megaLinkState,
    hasWebOperation,
    capabilities,
    robotState,
    triggerEmergencyStop,
    sendDriveCommand,
    executeCommandWithToast,
    stopRecord,
    setOperationActivity,
  } = useConnection();

  const [activeTask, setActiveTask] = useState<ControlTaskId>(() => entryParams.get('tarea') === 'wheel' ? 'wheel' : 'drive');
  const [linearSpeedMmS, setLinearSpeedMmS] = useState(100);
  const [angularSpeedMradS, setAngularSpeedMradS] = useState(800);
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
  const holdVector = useRef<RequestedMotion & { id: string } | null>(null);
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
  const canDrive = canPrepareControl && capabilities.mecanum_drive.implemented;

  const gotoAvailable =
    canDrive &&
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
        void sendDriveCommand(0, 0, 0).catch(() => { void triggerEmergencyStop('safety'); });
      }
    },
    [sendDriveCommand, setOperationActivity, triggerEmergencyStop]
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
    const next = { id, label, vxMmS: vx, vyMmS: vy, wzMradS: wz };
    // Direct command heartbeat; drag/speed changes replace the target.
    if (holdVector.current) {
      holdVector.current = next; setActivePadId(id); setRequestedMotion(next);
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
    setRequestedMotion(holdVector.current);
    const send = () => {
      if (generation !== holdGeneration.current || !holdVector.current) return;
      const target = holdVector.current;
      void sendDriveCommand(target.vxMmS, target.vyMmS, target.wzMradS).catch(e => {
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

  return (
    <div className="afr2-route-view afr2-control-workspace">
      <SectionHeader title="Control" description="Movimientos, secuencias y simulacion desde la Web." />
      {driveError && <StatusNotice tone="error" title="Consigna sin confirmar">{driveError}</StatusNotice>}
      {robotState && <p className="afr2-help" role="status">Modo AVR: {robotState.mode} · GOTO: {robotState.gotoState} · Salidas apagadas: {robotState.stopped === null ? 'desconocido' : robotState.stopped ? 'si' : 'no'} · Fallo: {robotState.fault ?? 'desconocido'}</p>}



      <TaskSections
        ariaLabel="Tareas de Control"
        activeId={activeTask}
        onChange={handleTaskChange}
        items={[
          {
            id: 'drive',
            label: 'Movimiento',
            content: (
              <div className="space-y-5">
                {!canDrive && (
                  <StatusNotice
                    tone="info"
                    title="Simulacion local disponible"
                    badge="Control tactil"
                  >
                    {wsState !== 'ready' || megaLinkState !== 'ready'
                      ? 'Abri Control tactil para simular sin conexion al robot. Las ordenes fisicas requieren enlace Web↔ESP↔ATmega2560.'
                      : `Control tactil permite simular localmente. Robot: ${capabilities.mecanum_drive.reason || 'Espera la confirmacion de parada antes de operar.'}`}
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
                  {/* Pad direccional 3x3 + controles graficos de giro sobre el eje */}
                  <div className="lg:col-span-7 afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
                    <h2 className="text-base font-semibold text-[var(--text)]">
                      Vectores planares (`vx`, `vy`) y giro sobre el eje (`wz`)
                    </h2>

                    <div
                      className="grid grid-cols-3 gap-3"
                      role="group"
                      aria-label="Vectores de traslacion Mecanum"
                    >
                      {MECANUM_VECTORS.map((vec) => {
                        const isStopCell = vec.id === 'stop';
                        if (isStopCell) return <span key={vec.id} aria-hidden="true" />;
                        const isActive = activePadId === vec.id;
                        return (
                          <button
                            key={vec.id}
                            type="button"
                            disabled={!canDrive && !isStopCell}
                            aria-label={vec.label}
                            onPointerDown={() => {
                              if (isStopCell) {
                                stopActiveHold(true);
                                return;
                              }
                              startHoldVector(
                                vec.id,
                                vec.label,
                                vec.vxFactor * linearSpeedMmS,
                                vec.vyFactor * linearSpeedMmS,
                                0
                              );
                            }}
                            onPointerUp={() => stopActiveHold(true)}
                            onPointerCancel={() => stopActiveHold(true)}
                            onPointerLeave={() => {
                              if (isActive) stopActiveHold(true);
                            }}
                            className={`min-h-16 rounded-[var(--radius-sm)] border p-2.5 flex flex-col items-center justify-center gap-1 select-none ${
                              isStopCell
                                ? 'bg-[var(--surface-solid)] border-[var(--error)] text-[var(--error)] font-semibold'
                                : isActive
                                  ? 'bg-[var(--action)] text-[var(--on-action)] border-[var(--action)]'
                                  : 'afr2-glass-field text-[var(--text)] disabled:opacity-55 disabled:cursor-not-allowed'
                            }`}
                          >
                            <span className="text-xl leading-none font-mono" aria-hidden="true">
                              {vec.symbol}
                            </span>
                            <span className="text-[11px] font-medium text-center leading-tight">
                              {vec.label}
                            </span>
                          </button>
                        );
                      })}
                    </div>

                    {/* Dos controles graficos de giro sobre el eje (vista superior del chasis + flecha circular) */}
                    <div className="space-y-2 pt-2 border-t border-[var(--border-default)]">
                      <div className="flex items-center justify-between gap-2">
                        <span className="text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
                          Giro sobre el eje (vista superior del chasis)
                        </span>
                        <span className="text-[11px] font-mono text-[var(--text-muted)]">
                          Accion solicitada · soltar o cancelar detiene
                        </span>
                      </div>

                      <div
                        className="grid grid-cols-1 sm:grid-cols-2 gap-3"
                        role="group"
                        aria-label="Controles de giro sobre el eje"
                      >
                        <button
                          type="button"
                          disabled={!canDrive}
                          aria-pressed={activePadId === 'rot-ccw'}
                          aria-label="Girar izquierda (sentido antihorario +wz)"
                          onPointerDown={(e) => {
                            if (e.button !== 0 || !canDrive) return;
                            try {
                              e.currentTarget.setPointerCapture(e.pointerId);
                            } catch {
                              // Ignorar
                            }
                            startHoldVector('rot-ccw', 'Girar izquierda', 0, 0, angularSpeedMradS);
                          }}
                          onPointerUp={(e) => {
                            try {
                              if (e.currentTarget.hasPointerCapture(e.pointerId)) {
                                e.currentTarget.releasePointerCapture(e.pointerId);
                              }
                            } catch {
                              // Ignorar
                            }
                            stopActiveHold(true);
                          }}
                          onPointerCancel={(e) => {
                            try {
                              if (e.currentTarget.hasPointerCapture(e.pointerId)) {
                                e.currentTarget.releasePointerCapture(e.pointerId);
                              }
                            } catch {
                              // Ignorar
                            }
                            stopActiveHold(true);
                          }}
                          onLostPointerCapture={() => {
                            if (activePadId === 'rot-ccw') stopActiveHold(true);
                          }}
                          onKeyDown={(e) => {
                            if (e.key === ' ' || e.key === 'Enter') {
                              e.preventDefault();
                              if (e.repeat || !canDrive) return;
                              startHoldVector('rot-ccw', 'Girar izquierda', 0, 0, angularSpeedMradS);
                            }
                          }}
                          onKeyUp={(e) => {
                            if (e.key === ' ' || e.key === 'Enter') {
                              e.preventDefault();
                              stopActiveHold(true);
                            }
                          }}
                          onBlur={() => {
                            if (activePadId === 'rot-ccw') stopActiveHold(true);
                          }}
                          className="afr2-spin-axis-btn min-h-16 rounded-[var(--radius-sm)] px-4 py-3 flex items-center gap-3.5 text-left select-none"
                        >
                          <ChassisSpinGraphic
                            direction="ccw"
                            active={activePadId === 'rot-ccw'}
                            size="md"
                          />
                          <div className="min-w-0">
                            <span className="block text-sm font-semibold">Girar izquierda</span>
                            <span className="block text-xs font-mono opacity-80">
                              Antihorario (+{angularSpeedMradS} mrad/s)
                            </span>
                          </div>
                        </button>

                        <button
                          type="button"
                          disabled={!canDrive}
                          aria-pressed={activePadId === 'rot-cw'}
                          aria-label="Girar derecha (sentido horario -wz)"
                          onPointerDown={(e) => {
                            if (e.button !== 0 || !canDrive) return;
                            try {
                              e.currentTarget.setPointerCapture(e.pointerId);
                            } catch {
                              // Ignorar
                            }
                            startHoldVector('rot-cw', 'Girar derecha', 0, 0, -angularSpeedMradS);
                          }}
                          onPointerUp={(e) => {
                            try {
                              if (e.currentTarget.hasPointerCapture(e.pointerId)) {
                                e.currentTarget.releasePointerCapture(e.pointerId);
                              }
                            } catch {
                              // Ignorar
                            }
                            stopActiveHold(true);
                          }}
                          onPointerCancel={(e) => {
                            try {
                              if (e.currentTarget.hasPointerCapture(e.pointerId)) {
                                e.currentTarget.releasePointerCapture(e.pointerId);
                              }
                            } catch {
                              // Ignorar
                            }
                            stopActiveHold(true);
                          }}
                          onLostPointerCapture={() => {
                            if (activePadId === 'rot-cw') stopActiveHold(true);
                          }}
                          onKeyDown={(e) => {
                            if (e.key === ' ' || e.key === 'Enter') {
                              e.preventDefault();
                              if (e.repeat || !canDrive) return;
                              startHoldVector('rot-cw', 'Girar derecha', 0, 0, -angularSpeedMradS);
                            }
                          }}
                          onKeyUp={(e) => {
                            if (e.key === ' ' || e.key === 'Enter') {
                              e.preventDefault();
                              stopActiveHold(true);
                            }
                          }}
                          onBlur={() => {
                            if (activePadId === 'rot-cw') stopActiveHold(true);
                          }}
                          className="afr2-spin-axis-btn min-h-16 rounded-[var(--radius-sm)] px-4 py-3 flex items-center gap-3.5 text-left select-none"
                        >
                          <ChassisSpinGraphic
                            direction="cw"
                            active={activePadId === 'rot-cw'}
                            size="md"
                          />
                          <div className="min-w-0">
                            <span className="block text-sm font-semibold">Girar derecha</span>
                            <span className="block text-xs font-mono opacity-80">
                              Horario (-{angularSpeedMradS} mrad/s)
                            </span>
                          </div>
                        </button>
                      </div>
                    </div>
                  </div>

                  {/* Parametros de velocidad y modo operativo */}
                  <div className="lg:col-span-5 afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-5">
                    <h2 className="text-base font-semibold text-[var(--text)]">
                      Parametros y modo operativo
                    </h2>

                    <div>
                      <div className="flex justify-between text-xs font-medium mb-1">
                        <label htmlFor="linear-speed-range" className="text-[var(--text-muted)]">
                          Velocidad lineal (`vx / vy`)
                        </label>
                        <span className="font-mono font-semibold text-[var(--text)]">
                          {linearSpeedMmS} mm/s
                        </span>
                      </div>
                      <input
                        id="linear-speed-range"
                        type="range"
                        min={10}
                        max={maxSpeed}
                        step={10}
                        value={linearSpeedMmS}
                        onChange={(e) => setLinearSpeedMmS(Number(e.target.value))}
                        className="w-full accent-[var(--action)]"
                      />
                    </div>

                    <div>
                      <div className="flex justify-between text-xs font-medium mb-1">
                        <label htmlFor="angular-speed-range" className="text-[var(--text-muted)]">
                          Velocidad angular (`wz`)
                        </label>
                        <span className="font-mono font-semibold text-[var(--text)]">
                          {angularSpeedMradS} mrad/s
                        </span>
                      </div>
                      <input
                        id="angular-speed-range"
                        type="range"
                        min={200}
                        max={2000}
                        step={100}
                        value={angularSpeedMradS}
                        onChange={(e) => setAngularSpeedMradS(Number(e.target.value))}
                        className="w-full accent-[var(--action)]"
                      />
                    </div>

                    <div className="space-y-2 pt-2 border-t border-[var(--border-default)]">
                      <span className="block text-xs font-medium text-[var(--text-muted)]">
                        Modo operativo (`robot.mode`)
                      </span>
                      <div className="afr2-segmented" role="group" aria-label="Modo operativo del robot">
                        {(['idle', 'manual', 'line-follow'] as RobotModeOption[]).map((mode) => (
                          <button
                            key={mode}
                            type="button"
                            disabled={!canPrepareControl || (mode !== 'idle' && !capabilities.mecanum_drive.implemented) || (mode === 'line-follow' && !capabilities.line_follow.implemented)}
                            aria-pressed={selectedMode === mode}
                            onClick={() => void handleModeSubmit(mode)}
                            className="afr2-segmented-btn disabled:opacity-55 disabled:cursor-not-allowed"
                          >
                            {mode === 'idle' ? 'IDLE' : mode === 'manual' ? 'MANUAL' : 'LINEA 5CH'}
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
                      <label htmlFor="wheel-select" className="block text-xs font-medium text-[var(--text-muted)] mb-1">
                        Rueda Mecanum
                      </label>
                      <span className="afr2-select-wrap"><select
                        id="wheel-select"
                        value={testWheel}
                        onChange={(e) => setTestWheel(e.target.value as WheelId)}
                        className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-base text-[var(--text)]"
                      >
                        <option value="FL">FL — Delantera Izquierda</option>
                        <option value="FR">FR — Delantera Derecha</option>
                        <option value="RL">RL — Trasera Izquierda</option>
                        <option value="RR">RR — Trasera Derecha</option>
                      </select><Chevron /></span>
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
                        ΔX (mm)
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
                        ΔY (mm)
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
                        Δθ (mrad)
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
        ]}
      />

      <ManualDriveOverlay open={manualOverlayOpen} origin={manualOrigin} onClose={() => setManualOverlayOpen(false)}
        onRelease={() => stopActiveHold(true)} canDrive={canDrive && maxSpeed >= 10} speed={linearSpeedMmS} maxSpeed={maxSpeed}
        onSpeedChange={setLinearSpeedMmS} stopSignal={stopRecord.lastIssuedAtMs} error={driveError}
        onDirectionChange={(direction: ManualDirection) => { const speed = Math.floor(Math.min(linearSpeedMmS, maxSpeed) / (Math.abs(direction.vx) + Math.abs(direction.vy))); void startHoldVector(`touch-${direction.id}`, direction.label, direction.vx * speed, direction.vy * speed, 0); }} />
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
