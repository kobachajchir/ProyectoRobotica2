import { Chevron } from '../../../components/ui/Chevron';
import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { Button } from '../../../components/ui/Button';
import { DismissibleNotice } from '../../../components/ui/DismissibleNotice';
import { StatusNotice } from '../../../components/ui/StatusNotice';
import { notify } from '../../../components/notifications/notify';
import {
  DEFAULT_SEQUENCE_LIMITS,
  type PhysicalSequenceState,
  type PhysicalSequenceStatusPayload,
} from '../../../protocol/wsEnvelope';
import { useConnection } from '../../connection/ConnectionContext';
import { ChassisSpinGraphic } from '../ChassisSpinGraphic';
import {
  createAdvancedWheelCopy,
  createDefaultMovementBlock,
  createDefaultWheelBlock,
  createSequenceUid,
  createStarterSequenceDocument,
  deriveWheelTracksFromMovements,
  evaluateSequenceSimulationAtTime,
  resolveMovementBlockEndpoints,
  validateSequenceDocument,
  type SimulationSnapshotAtCursor,
} from './mecanumKinematics';
import { type TimelineVisualBlock } from './MotorBlock';
import {
  exportSequenceDocumentAsJson,
  loadSequenceLibraryFromStorage,
  parseAndValidateSequenceJson,
  saveSequenceLibraryToStorage,
} from './sequenceStorage';
import {
  DURATION_STEP_MS,
  EDITOR_MAX_BLOCKS_PER_TRACK,
  WHEEL_TRACK_KEYS,
  WHEEL_TRACK_LABELS,
  type Afr2SequenceDocument,
  type MovementBlock,
  type MovementBlockKind,
  type WheelBlock,
  type WheelBlockKind,
  type WheelTrackKey,
} from './sequenceTypes';
import { TimelineRow } from './TimelineRow';
import { CompletionFields } from './CompletionFields';
import { wheelDurationMs } from './blockCompletion';
import { compilePhysicalProgram } from './physicalProgram';
import { movementDurationMs } from './mecanumKinematics';
import { FloatingModal } from '../../../components/ui/FloatingModal';

export interface SequenceEditorPanelProps {
  preview?: React.ReactNode;
  onSimulationChange: (snapshot: SimulationSnapshotAtCursor, isPlaying: boolean, profileSummary: string) => void;
}

type SelectionTarget =
  | { scope: 'movements'; blockId: string }
  | { scope: WheelTrackKey; blockId: string }
  | null;

const MOVEMENT_PALETTE: { kind: MovementBlockKind; label: string; hint: string }[] = [
  { kind: 'constant', label: 'Desplazamiento', hint: 'vx / vy constante' },
  { kind: 'ramp', label: 'Rampa', hint: 'Interpolacion lineal' },
  { kind: 'pivot', label: 'Giro sobre eje', hint: 'Antihorario / horario' },
  { kind: 'curve', label: 'Curva', hint: 'Avance + giro' },
  { kind: 'wait', label: 'Espera', hint: 'Consigna 0 mm/s' },
  { kind: 'finish', label: 'Finalizacion', hint: 'Terminal (0 ms)' },
];

const WHEEL_PALETTE: { kind: WheelBlockKind; label: string; hint: string }[] = [
  { kind: 'constant', label: 'Velocidad', hint: 'Consigna firmada mm/s' },
  { kind: 'ramp', label: 'Rampa', hint: 'Inicio → fin mm/s' },
  { kind: 'wait', label: 'Espera', hint: 'Consigna 0 mm/s' },
  { kind: 'finish', label: 'Finalizacion', hint: 'Terminal (0 ms)' },
];

const PHYSICAL_STATE_LABELS: Record<PhysicalSequenceState, string> = {
  idle: 'Inactiva',
  preparing: 'Preparando en ATmega',
  ready: 'Preparada',
  running: 'Ejecutando en robot',
  completed: 'Completada',
  cancelled: 'Cancelada',
  failed: 'Fallida',
};

function formatMovementSummary(block: MovementBlock): string {
  if (block.completion === 'distance') return `${block.distanceMm ?? 0} mm · rueda de referencia · Hall`;
  switch (block.kind) {
    case 'constant':
      return `${block.durationMs} ms · vx ${block.vxMmS} / vy ${block.vyMmS} mm/s`;
    case 'ramp':
      return `${block.durationMs} ms · (${block.fromVxMmS},${block.fromVyMmS}) → (${block.toVxMmS},${block.toVyMmS}) mm/s`;
    case 'pivot':
      return `${block.durationMs} ms · ${block.pivotDirection === 'ccw' ? 'Izq. +' : 'Der. -'}${Math.abs(block.wzMradS)} mrad/s`;
    case 'curve':
      return `${block.durationMs} ms · vx ${block.vxMmS} mm/s · ${block.arcSide === 'left' ? 'Izq.' : 'Der.'} ${Math.abs(block.wzMradS)} mrad/s`;
    case 'wait':
      return `${block.durationMs} ms · 0 mm/s`;
    case 'finish':
      return '0 ms · Terminal (0 mm/s)';
  }
}

function formatWheelSummary(block: WheelBlock): string {
  if (block.completion === 'distance') return `${block.distanceMm ?? 0} mm · ${wheelDurationMs(block)} ms simulados · Hall`;
  switch (block.kind) {
    case 'constant':
      return `${block.durationMs} ms · ${block.speedMmS >= 0 ? '+' : ''}${block.speedMmS} mm/s`;
    case 'ramp':
      return `${block.durationMs} ms · ${block.fromSpeedMmS} → ${block.toSpeedMmS} mm/s`;
    case 'wait':
      return `${block.durationMs} ms · 0 mm/s`;
    case 'finish':
      return '0 ms · Terminal (0 mm/s)';
  }
}

export function SequenceEditorPanel({ onSimulationChange, preview }: SequenceEditorPanelProps) {
  const {
    wsState,
    megaLinkState,
    capabilities,
    stopRecord,
    requestCommand,
    triggerEmergencyStop,
    setOperationActivity,
  } = useConnection();

  const initialStorage = useMemo(() => loadSequenceLibraryFromStorage(), []);
  const [programs, setPrograms] = useState<Afr2SequenceDocument[]>(initialStorage.programs);
  const [activeProgramId, setActiveProgramId] = useState<string>(initialStorage.activeId);

  // Pilas de deshacer/rehacer para el documento activo
  const [undoStack, setUndoStack] = useState<Afr2SequenceDocument[]>([]);
  const [redoStack, setRedoStack] = useState<Afr2SequenceDocument[]>([]);

  const activeDoc = useMemo(
    () => programs.find((p) => p.id === activeProgramId) ?? programs[0]!,
    [programs, activeProgramId]
  );

  // Seleccion y propiedades moviles
  const [selection, setSelection] = useState<SelectionTarget>(null);
  const [activeWheelTrackForAdd, setActiveWheelTrackForAdd] = useState<WheelTrackKey>('fl');
  const [selectedTrack, setSelectedTrack] = useState<WheelTrackKey | 'movements' | null>(null);
  const [pendingTrackDelete, setPendingTrackDelete] = useState<{ program: string; track: WheelTrackKey } | null>(null);
  const deletedTrack = useRef(false);
  const trackCountRef = useRef<HTMLSelectElement>(null);
  const enabledTracks = activeDoc.enabledWheelTracks ?? [...WHEEL_TRACK_KEYS];
  const visibleTracks = activeDoc.editorLevel === 'movements' ? [...WHEEL_TRACK_KEYS] : enabledTracks;
  useEffect(() => { if (!enabledTracks.includes(activeWheelTrackForAdd) && enabledTracks.length) setActiveWheelTrackForAdd(enabledTracks[0]); }, [enabledTracks, activeWheelTrackForAdd]);

  // Avisos locales cerrables (importacion/conversion/ejecucion)
  const [localBanner, setLocalBanner] = useState<{
    id: string;
    tone: 'error' | 'warning' | 'info';
    title: string;
    message: string;
  } | null>(null);

  // Estado del simulador determinista
  const [cursorMs, setCursorMs] = useState(0);
  const [simPlaying, setSimPlaying] = useState(false);
  const simRafRef = useRef<number>(0);
  const simStartWallMsRef = useRef<number>(0);
  const simStartCursorMsRef = useRef<number>(0);

  // Estado de ejecucion fisica (ROBO2 perfil 2 / EX01 v4: 0x40-0x45)
  const [physicalState, setPhysicalState] = useState<PhysicalSequenceState>('idle');
  const [physicalExecId, setPhysicalExecId] = useState<string | null>(null);
  const [physicalStatus, setPhysicalStatus] = useState<PhysicalSequenceStatusPayload | null>(null);
  const [physicalDetail, setPhysicalDetail] = useState<string | null>(null);
  const physicalActiveRef = useRef<{ sequenceId: string; executionId: string } | null>(null);
  const mounted = useRef(true);
  const preparationGeneration = useRef(0);
  const statusPollInFlightRef = useRef(false);
  const fileInputRef = useRef<HTMLInputElement | null>(null);

  const latestStop = useRef(stopRecord.lastIssuedAtMs); latestStop.current = stopRecord.lastIssuedAtMs;
  useEffect(() => { setOperationActivity('simulation', simPlaying); return () => setOperationActivity('simulation', false); }, [simPlaying, setOperationActivity]);
  useEffect(() => { const active = ['preparing', 'ready', 'running'].includes(physicalState); setOperationActivity('sequence', active); return () => setOperationActivity('sequence', false); }, [physicalState, setOperationActivity]);
  useEffect(() => { if (stopRecord.lastIssuedAtMs) setSimPlaying(false); }, [stopRecord.lastIssuedAtMs]);
  // Autoguardado local
  useEffect(() => {
    saveSequenceLibraryToStorage(programs, activeDoc.id);
  }, [programs, activeDoc.id]);

  // Limites anunciados por el controlador o valores iniciales por defecto (32 seg / 120 s)
  const controllerSequenceLimits = useMemo(() => {
    const lim = capabilities.sequence_runner?.limits;
    return {
      maxSegments: lim?.maxSegments ?? DEFAULT_SEQUENCE_LIMITS.maxSegments,
      maxDurationMs: lim?.maxDurationMs ?? DEFAULT_SEQUENCE_LIMITS.maxDurationMs,
    };
  }, [capabilities.sequence_runner]);

  const validation = useMemo(
    () => validateSequenceDocument(activeDoc, controllerSequenceLimits),
    [activeDoc, controllerSequenceLimits]
  );

  const totalDurationMs = validation.normalized.totalDurationMs;
  const physicalProgram = useMemo(() => compilePhysicalProgram(activeDoc), [activeDoc]);

  // Evaluacion determinista en el cursor actual
  const simSnapshot = useMemo(
    () => evaluateSequenceSimulationAtTime(activeDoc, cursorMs),
    [activeDoc, cursorMs]
  );

  const profileSummary = useMemo(
    () =>
      `L=${activeDoc.kinematicProfile.lxPlusLyMm} mm · Vmax=${activeDoc.kinematicProfile.maxWheelSpeedMmS} mm/s (${
        activeDoc.kinematicProfile.calibrated ? 'calibrado' : 'no calibrado'
      })`,
    [activeDoc.kinematicProfile]
  );

  useEffect(() => {
    onSimulationChange(simSnapshot, simPlaying, profileSummary);
  }, [simSnapshot, simPlaying, profileSummary, onSimulationChange]);

  const isLocked =
    simPlaying ||
    physicalState === 'preparing' ||
    physicalState === 'ready' ||
    physicalState === 'running';

  // Actualizar documento activo registrando historial undo/redo
  const commitDocUpdate = useCallback(
    (updater: (prev: Afr2SequenceDocument) => Afr2SequenceDocument, recordHistory = true) => {
      if (isLocked) return;
      setPrograms((prevList) =>
        prevList.map((item) => {
          if (item.id !== activeDoc.id) return item;
          const next = updater(item);
          const updated: Afr2SequenceDocument = {
            ...next,
            updatedAt: new Date().toISOString(),
          };
          if (recordHistory) {
            setUndoStack((u) => [...u.slice(-29), item]);
            setRedoStack([]);
          }
          return updated;
        })
      );
    },
    [activeDoc.id, isLocked]
  );

  const handleUndo = () => {
    if (isLocked || undoStack.length === 0) return;
    const previous = undoStack[undoStack.length - 1]!;
    setUndoStack((u) => u.slice(0, -1));
    setRedoStack((r) => [...r.slice(-29), activeDoc]);
    setPrograms((list) => list.map((p) => (p.id === activeDoc.id ? previous : p)));
  };

  const handleRedo = () => {
    if (isLocked || redoStack.length === 0) return;
    const next = redoStack[redoStack.length - 1]!;
    setRedoStack((r) => r.slice(0, -1));
    setUndoStack((u) => [...u.slice(-29), activeDoc]);
    setPrograms((list) => list.map((p) => (p.id === activeDoc.id ? next : p)));
  };

  // Reproduccion de simulacion con requestAnimationFrame
  useEffect(() => {
    if (!simPlaying) {
      if (simRafRef.current) {
        cancelAnimationFrame(simRafRef.current);
        simRafRef.current = 0;
      }
      return;
    }

    if (totalDurationMs <= 0) {
      setSimPlaying(false);
      return;
    }

    simStartWallMsRef.current = performance.now();
    simStartCursorMsRef.current = cursorMs >= totalDurationMs ? 0 : cursorMs;

    const stepFrame = (nowWall: number) => {
      const elapsed = nowWall - simStartWallMsRef.current;
      const rawNext = simStartCursorMsRef.current + elapsed;
      const snappedNext = Math.min(
        totalDurationMs,
        Math.floor(rawNext / DURATION_STEP_MS) * DURATION_STEP_MS
      );
      setCursorMs(snappedNext);

      if (rawNext >= totalDurationMs) {
        setCursorMs(totalDurationMs);
        setSimPlaying(false);
        simRafRef.current = 0;
        return;
      }
      simRafRef.current = requestAnimationFrame(stepFrame);
    };

    simRafRef.current = requestAnimationFrame(stepFrame);
    return () => {
      if (simRafRef.current) {
        cancelAnimationFrame(simRafRef.current);
        simRafRef.current = 0;
      }
    };
  }, [simPlaying, totalDurationMs]);

  // Cancelacion fisica inmediata ante ocultamiento de pagina, perdida de enlace, STOP o desmontaje
  const cancelPhysicalExecution = useCallback(
    async (
      reason: 'user-cancel' | 'page-hidden' | 'route-left' | 'disconnect' | 'stop'
    ) => {
      const activeExec = physicalActiveRef.current;
      ++preparationGeneration.current;
      physicalActiveRef.current = null;
      setPhysicalState('cancelled');
      setPhysicalDetail(`Ejecucion fisica cancelada (${reason}). No se reanuda automaticamente.`);

      try {
        await requestCommand(
          'robot.sequence.cancel',
          {
            sequenceId: activeExec?.sequenceId ?? activeDoc.id,
            executionId: activeExec?.executionId ?? physicalExecId,
            reason,
          },
          1500
        );
      } catch {
        // Si falla la cancelacion de secuencia, emitir STOP prioritario de respaldo
        await triggerEmergencyStop('safety');
      }
    },
    [activeDoc.id, physicalExecId, requestCommand, triggerEmergencyStop]
  );

  // Pausa de simulacion y cancelacion fisica al ocultar la pagina
  useEffect(() => {
    const onBlur = () => {
      ++preparationGeneration.current;
      setSimPlaying(false);
      if (physicalActiveRef.current) void cancelPhysicalExecution('page-hidden');
    };
    const onVisibilityChange = () => {
      if (document.visibilityState !== 'visible') {
        ++preparationGeneration.current;
        setSimPlaying(false);
        if (physicalActiveRef.current) {
          void cancelPhysicalExecution('page-hidden');
        }
      }
    };
    window.addEventListener('blur', onBlur);
    document.addEventListener('visibilitychange', onVisibilityChange);
    return () => {
      window.removeEventListener('blur', onBlur);
      document.removeEventListener('visibilitychange', onVisibilityChange);
    };
  }, [cancelPhysicalExecution]);

  // Cancelar ejecucion fisica al desmontar Secuencias / salir de Control
  useEffect(() => {
    mounted.current = true;
    return () => {
      mounted.current = false;
      ++preparationGeneration.current;
      const active = physicalActiveRef.current;
      physicalActiveRef.current = null;
      if (active) {
        void requestCommand(
          'robot.sequence.cancel',
          {
            sequenceId: active.sequenceId,
            executionId: active.executionId,
            reason: 'route-left',
          },
          1200
        ).catch(() => triggerEmergencyStop('safety'));
      }
    };
  }, [requestCommand, triggerEmergencyStop]);

  // Cancelar si se pierde conexion durante preparacion/ejecucion fisica
  useEffect(() => {
    if (!physicalActiveRef.current) return;
    if (
      wsState !== 'ready' ||
      megaLinkState !== 'ready'
    ) {
      void cancelPhysicalExecution(
        'disconnect'
      );
    }
  }, [wsState, megaLinkState, cancelPhysicalExecution]);

  // Si se emite una nueva orden STOP global, cancelar cualquier secuencia fisica activa
  useEffect(() => {
    if (stopRecord.status === 'sending' && physicalActiveRef.current) {
      physicalActiveRef.current = null;
      setPhysicalState('cancelled');
      setPhysicalDetail('Secuencia cancelada por orden prioritaria STOP.');
    }
  }, [stopRecord.noticeId, stopRecord.status]);

  // Consulta de progreso fisico cada 250 ms sin superponer solicitudes
  useEffect(() => {
    if (physicalState !== 'running' || !physicalActiveRef.current) return;
    let cancelled = false;
    let timerId: number | null = null;

    const pollStatus = async () => {
      if (cancelled || statusPollInFlightRef.current || !physicalActiveRef.current) return;
      statusPollInFlightRef.current = true;
      const { sequenceId, executionId } = physicalActiveRef.current;
      try {
        const res = await requestCommand<PhysicalSequenceStatusPayload>(
          'robot.sequence.status',
          { sequenceId, executionId },
          1500
        );
        if (cancelled) return;
        if (res.status === 'completed' && res.payload) {
          const st = res.payload;
          setPhysicalStatus(st);
          if (st.state === 'completed' || st.state === 'cancelled' || st.state === 'failed') {
            physicalActiveRef.current = null;
            setPhysicalState(st.state);
            setPhysicalDetail(
              st.state === 'completed'
                ? 'Secuencia fisica completada por el ATmega2560 (salidas en 0).'
                : `Secuencia fisica finalizada en estado ${st.state}${st.reason ? `: ${st.reason}` : ''}.`
            );
            return;
          }
        }
      } catch {
        // La perdida de enlace lo cancela desde el efecto supervisor
      } finally {
        statusPollInFlightRef.current = false;
        if (!cancelled && physicalActiveRef.current) {
          timerId = window.setTimeout(() => void pollStatus(), 250);
        }
      }
    };

    timerId = window.setTimeout(() => void pollStatus(), 250);
    return () => {
      cancelled = true;
      if (timerId !== null) window.clearTimeout(timerId);
    };
  }, [physicalState, requestCommand, totalDurationMs]);

  // Disponibilidad y motivo de bloqueo para ejecucion fisica
  const linksReady = wsState === 'ready' && megaLinkState === 'ready';
  const motorsReady =
    capabilities.mecanum_drive.present && capabilities.mecanum_drive.implemented;
  const sequencesReady =
    capabilities.sequence_runner?.present && capabilities.sequence_runner?.implemented;
  const profileCalibrated = activeDoc.kinematicProfile.calibrated;
  const actualLimits=capabilities.mecanum_drive.limits;
  const profileMatchesController=Boolean(actualLimits?.radiusSumMm && Math.abs(activeDoc.kinematicProfile.lxPlusLyMm-actualLimits.radiusSumMm)<1);
  const speedsMatchController=Boolean(actualLimits?.maxSpeedMmS && validation.normalized.segments.every(segment=>
    [...Object.values(segment.startWheelMmS),...Object.values(segment.endWheelMmS)].every(speed=>Math.abs(speed)<=actualLimits.maxSpeedMmS!)));

  const physicalBlockedReason = !linksReady
    ? 'Sin enlace activo Web↔ESP y ESP↔ATmega2560.'
      : !motorsReady
        ? capabilities.mecanum_drive.reason || 'Control PWM/direccion de motores y STOP aun no habilitados en firmware.'
        : !sequencesReady
          ? capabilities.sequence_runner?.reason || 'Capacidad de secuencias fisicas (ROBO2 perfil 2 / EX01 v4, 0x40–0x45) pendiente en controlador.'
          : !profileCalibrated
            ? 'El perfil cinematico figura como ilustrativo no calibrado. Confirma un perfil fisico calibrado antes de ejecutar en hardware.'
            : !profileMatchesController || !speedsMatchController
              ? 'La geometria o las velocidades del programa no coinciden con los limites reales informados por el ATmega.'
            : !validation.editorValid
              ? 'El programa tiene errores de validacion que deben corregirse.'
              : physicalProgram.error || (physicalProgram.mode !== 'timed' && !capabilities.sequence_runner.limits?.distanceBlocks)
                ? physicalProgram.error || 'La ejecucion por distancia requiere EX01 v6 y Hall/calibracion/memoria aprobados.'
              : physicalProgram.mode === 'timed' && !validation.physicalValid
                ? `El programa excede los limites fisicos del controlador (${controllerSequenceLimits.maxSegments} segmentos / ${(controllerSequenceLimits.maxDurationMs / 1000).toFixed(0)} s). No se trunca automaticamente.`
                : null;

  const canStartPhysical =
    physicalBlockedReason === null &&
    physicalState !== 'preparing' &&
    physicalState !== 'running';

  const handleStartPhysicalSequence = async () => {
    if (!canStartPhysical) return;
    setSimPlaying(false);
    const segments = physicalProgram.segments;
    const physicalDurationMs = physicalProgram.totalDurationMs;
    const sequenceId = activeDoc.id;
    const executionId = createSequenceUid('exec');
    setPhysicalExecId(executionId);
    setPhysicalState('preparing');
    setPhysicalStatus(null);
    setPhysicalDetail(
      `Preparando ${segments.length} instrucciones (${(physicalDurationMs / 1000).toFixed(2)} s${physicalProgram.mode === 'timed' ? '' : ' de limite de seguridad; finalizacion por Hall'})…`
    );
    const stopAtStart = latestStop.current;
    const generation = ++preparationGeneration.current;

    try {
      if (!mounted.current || generation !== preparationGeneration.current || latestStop.current !== stopAtStart || document.visibilityState !== 'visible') {
        if (mounted.current) setPhysicalState('cancelled');
        void triggerEmergencyStop('safety');
        return;
      }
      physicalActiveRef.current = { sequenceId, executionId };
      // 1. begin (EX01 v4 0x40)
      const beginRes = await requestCommand(
        'robot.sequence.begin',
        {
          sequenceId,
          name: activeDoc.name,
          segmentCount: segments.length,
          totalDurationMs: physicalDurationMs,
          executionMode: physicalProgram.mode,
        },
        2500
      );
      if (!beginRes.ok || beginRes.status !== 'completed') {
        throw new Error('El controlador rechazo robot.sequence.begin.');
      }

      // 2. append (EX01 v4 0x41) — exactamente un segmento por solicitud
      for (const seg of segments) {
        if (physicalActiveRef.current?.executionId !== executionId) {
          throw new Error('Carga interrumpida antes de completar append.');
        }
        setPhysicalDetail(
          `Cargando segmento ${seg.index + 1} de ${segments.length} en buffer estatico AVR…`
        );
        const appendRes = await requestCommand(
          'robot.sequence.append',
          {
            sequenceId,
            index: seg.index,
            durationMs: seg.durationMs,
            movementKind: seg.movementKind ?? 'wheels',
            startWheelMmS: seg.startWheelMmS,
            endWheelMmS: seg.endWheelMmS,
            ...(seg.wheel ? { wheel: seg.wheel, completion: seg.completion, limit: seg.limit, timeoutMs: seg.timeoutMs, rampMs: seg.rampMs, fromMmS: seg.fromMmS, toMmS: seg.toMmS } : {}),
          },
          2000
        );
        if (!appendRes.ok) {
          throw new Error(`Fallo al cargar el segmento #${seg.index + 1}.`);
        }
      }

      if (physicalActiveRef.current?.executionId !== executionId) throw new Error('Carga cancelada.');
      // 3. commit (EX01 v4 0x42) — verifica cantidad y coherencia antes de habilitar
      setPhysicalDetail('Verificando coherencia de segmentos (commit)…');
      const commitRes = await requestCommand(
        'robot.sequence.commit',
        {
          sequenceId,
          segmentCount: segments.length,
          totalDurationMs: physicalDurationMs,
        },
        2500
      );
      if (!commitRes.ok || commitRes.status !== 'completed') {
        throw new Error('El controlador no confirmo la integridad del programa (commit).');
      }
      if (physicalActiveRef.current?.executionId !== executionId) throw new Error('Ejecucion cancelada antes del inicio.');

      // 4. start (EX01 v4 0x43) — aceptacion de inicio no equivale a finalizacion
      setPhysicalDetail(physicalProgram.mode === 'timed' ? 'Iniciando ejecucion temporizada en el ATmega2560…' : 'Iniciando ejecucion por pulsos Hall con timeout de seguridad…');
      const startRes = await requestCommand<PhysicalSequenceStatusPayload>(
        'robot.sequence.start',
        { sequenceId, executionId },
        2500
      );
      if (!startRes.ok || startRes.status !== 'completed' || startRes.payload.state !== 'running') {
        throw new Error('El controlador rechazo robot.sequence.start.');
      }

      if (physicalActiveRef.current?.executionId !== executionId) return;
      setPhysicalState('running');
      setPhysicalDetail(
        'Ejecutando en ATmega2560 (temporizacion autoritativa en MCU; consultando progreso cada 250 ms).'
      );
      notify.info({
        id: 'afr2-seq-physical',
        title: 'Secuencia iniciada en el controlador',
        description: `${activeDoc.name} (${segments.length} segmentos). La aceptacion de inicio no equivale a finalizacion.`,
      });
    } catch (err) {
      // A late load/start failure cannot cancel or overwrite a newer run.
      if (!mounted.current || generation !== preparationGeneration.current || latestStop.current !== stopAtStart || (physicalActiveRef.current && physicalActiveRef.current.executionId !== executionId)) return;
      physicalActiveRef.current = null;
      void triggerEmergencyStop('safety');
      const msg =
        err instanceof Error ? err.message : 'Fallo durante la carga o inicio fisico de la secuencia.';
      setPhysicalState('failed');
      setPhysicalDetail(`Carga o ejecucion fallida: ${msg}. Una carga incompleta jamas queda ejecutable.`);
      setLocalBanner({
        id: `phys-err-${Date.now()}`,
        tone: 'error',
        title: 'No se pudo ejecutar la secuencia en el robot',
        message: msg,
      });
    }
  };

  // Bloques visuales para TimelineRow
  const movementVisualBlocks = useMemo<TimelineVisualBlock[]>(() => {
    const maxW = Math.max(100, activeDoc.kinematicProfile.maxWheelSpeedMmS);
    return activeDoc.movementBlocks.map((b) => {
      const { start, end } = resolveMovementBlockEndpoints(b, activeDoc.kinematicProfile);
      const startPeak = Math.max(Math.abs(start.fl), Math.abs(start.fr), Math.abs(start.rl), Math.abs(start.rr));
      const endPeak = Math.max(Math.abs(end.fl), Math.abs(end.fr), Math.abs(end.rl), Math.abs(end.rr));
      return {
        id: b.id,
        kind: b.kind,
        label: b.label,
        durationMs: movementDurationMs(b, activeDoc.kinematicProfile),
        summary: formatMovementSummary(b),
        pivotDirection: b.pivotDirection,
        arcSide: b.arcSide,
        rampFromRatio: startPeak / maxW,
        rampToRatio: endPeak / maxW,
      };
    });
  }, [activeDoc.movementBlocks, activeDoc.kinematicProfile]);

  const wheelVisualTracks = useMemo<Record<WheelTrackKey, TimelineVisualBlock[]>>(() => {
    const maxW = Math.max(100, activeDoc.kinematicProfile.maxWheelSpeedMmS);
    const effective = validation.normalized.effectiveTracks;
    const mapTrack = (list: WheelBlock[]): TimelineVisualBlock[] =>
      list.map((b) => ({
        id: b.id,
        kind: b.kind,
        label: b.label,
        durationMs: wheelDurationMs(b),
        summary: formatWheelSummary(b),
        rampFromRatio: Math.abs(b.fromSpeedMmS) / maxW,
        rampToRatio: Math.abs(b.toSpeedMmS) / maxW,
      }));
    return {
      fl: mapTrack(effective.fl),
      fr: mapTrack(effective.fr),
      rl: mapTrack(effective.rl),
      rr: mapTrack(effective.rr),
    };
  }, [validation.normalized.effectiveTracks, activeDoc.kinematicProfile.maxWheelSpeedMmS]);

  // Resolver bloque seleccionado
  const selectedMovementBlock = useMemo(() => {
    if (!selection || selection.scope !== 'movements') return null;
    return activeDoc.movementBlocks.find((b) => b.id === selection.blockId) ?? null;
  }, [activeDoc.movementBlocks, selection]);

  const selectedWheelBlock = useMemo(() => {
    if (!selection || selection.scope === 'movements') return null;
    const track = selection.scope;
    return validation.normalized.effectiveTracks[track].find((b) => b.id === selection.blockId) ?? null;
  }, [validation.normalized.effectiveTracks, selection]);

  // Acciones de edicion de bloques
  const handleAddMovementBlock = (kind: MovementBlockKind) => {
    if (isLocked || activeDoc.movementBlocks.some(block => block.kind === 'finish')) return;
    if (activeDoc.movementBlocks.length >= EDITOR_MAX_BLOCKS_PER_TRACK) {
      setLocalBanner({
        id: `limit-${Date.now()}`,
        tone: 'warning',
        title: 'Limite de pista alcanzado',
        message: `Cada pista admite hasta ${EDITOR_MAX_BLOCKS_PER_TRACK} bloques.`,
      });
      return;
    }
    const newBlock = createDefaultMovementBlock(kind);
    commitDocUpdate((doc) => {
      const finishIdx = doc.movementBlocks.findIndex((b) => b.kind === 'finish');
      const nextBlocks =
        kind !== 'finish' && finishIdx >= 0
          ? [
              ...doc.movementBlocks.slice(0, finishIdx),
              newBlock,
              ...doc.movementBlocks.slice(finishIdx),
            ]
          : [...doc.movementBlocks, newBlock];
      return { ...doc, movementBlocks: nextBlocks };
    });
    setSelection({ scope: 'movements', blockId: newBlock.id });
  };

  const handleAddWheelBlock = (kind: WheelBlockKind, track: WheelTrackKey = activeWheelTrackForAdd) => {
    if (isLocked || activeDoc.editorLevel !== 'wheels') return;
    if (!enabledTracks.includes(track) || activeDoc.wheelTracks[track].some(block => block.kind === 'finish')) return;
    if (activeDoc.wheelTracks[track].length >= EDITOR_MAX_BLOCKS_PER_TRACK) {
      setLocalBanner({
        id: `limit-${Date.now()}`,
        tone: 'warning',
        title: 'Limite de pista alcanzado',
        message: `La pista ${track.toUpperCase()} ya tiene ${EDITOR_MAX_BLOCKS_PER_TRACK} bloques.`,
      });
      return;
    }
    const newBlock = createDefaultWheelBlock(kind);
    commitDocUpdate((doc) => {
      const list = doc.wheelTracks[track];
      const finishIdx = list.findIndex((b) => b.kind === 'finish');
      const nextList =
        kind !== 'finish' && finishIdx >= 0
          ? [...list.slice(0, finishIdx), newBlock, ...list.slice(finishIdx)]
          : [...list, newBlock];
      return {
        ...doc,
        wheelTracks: {
          ...doc.wheelTracks,
          [track]: nextList,
        },
      };
    });
    setSelection({ scope: track, blockId: newBlock.id });
  };

  const handleDuplicateSelected = () => {
    if (isLocked || !selection) return;
    if (selectedMovementBlock?.kind === 'finish' || selectedWheelBlock?.kind === 'finish') return;
    if (selection.scope === 'movements' && selectedMovementBlock) {
      if (activeDoc.movementBlocks.length >= EDITOR_MAX_BLOCKS_PER_TRACK) return;
      const clone: MovementBlock = {
        ...selectedMovementBlock,
        id: createSequenceUid('mov'),
        label: `${selectedMovementBlock.label} copia`,
      };
      commitDocUpdate((doc) => {
        const idx = doc.movementBlocks.findIndex((b) => b.id === selectedMovementBlock.id);
        const next =
          idx >= 0
            ? [
                ...doc.movementBlocks.slice(0, idx + 1),
                clone,
                ...doc.movementBlocks.slice(idx + 1),
              ]
            : [...doc.movementBlocks, clone];
        return { ...doc, movementBlocks: next };
      });
      setSelection({ scope: 'movements', blockId: clone.id });
    } else if (selection.scope !== 'movements' && selectedWheelBlock && activeDoc.editorLevel === 'wheels') {
      const track = selection.scope;
      if (activeDoc.wheelTracks[track].length >= EDITOR_MAX_BLOCKS_PER_TRACK) return;
      const clone: WheelBlock = {
        ...selectedWheelBlock,
        id: createSequenceUid(`whl-${track}`),
        label: `${selectedWheelBlock.label} copia`,
      };
      commitDocUpdate((doc) => {
        const list = doc.wheelTracks[track];
        const idx = list.findIndex((b) => b.id === selectedWheelBlock.id);
        const next =
          idx >= 0
            ? [...list.slice(0, idx + 1), clone, ...list.slice(idx + 1)]
            : [...list, clone];
        return {
          ...doc,
          wheelTracks: { ...doc.wheelTracks, [track]: next },
        };
      });
      setSelection({ scope: track, blockId: clone.id });
    }
  };

  const handleDeleteSelected = () => {
    if (isLocked || !selection) return;
    if (selection.scope === 'movements') {
      commitDocUpdate((doc) => ({
        ...doc,
        movementBlocks: doc.movementBlocks.filter((b) => b.id !== selection.blockId),
      }));
    } else if (activeDoc.editorLevel === 'wheels') {
      const track = selection.scope;
      commitDocUpdate((doc) => ({
        ...doc,
        wheelTracks: {
          ...doc.wheelTracks,
          [track]: doc.wheelTracks[track].filter((b) => b.id !== selection.blockId),
        },
      }));
    }
    setSelection(null);
    
  };

  const handleCreateAdvancedWheelCopy = () => {
    if (isLocked) return;
    const copyDoc = createAdvancedWheelCopy(activeDoc);
    setPrograms((prev) => [...prev, copyDoc]);
    setActiveProgramId(copyDoc.id);
    setUndoStack([]);
    setRedoStack([]);
    setSelection(null);
    setLocalBanner({
      id: `copy-${Date.now()}`,
      tone: 'info',
      title: 'Copia avanzada de 4 ruedas creada',
      message: `Se creo «${copyDoc.name}» para editar cada rueda (FL/FR/RL/RR) de forma independiente. El programa original «${activeDoc.name}» se conserva intacto en la biblioteca.`,
    });
  };

  const handleImportFile = async (e: React.ChangeEvent<HTMLInputElement>) => {
    const file = e.target.files?.[0];
    e.target.value = '';
    if (!file || isLocked) return;

    try {
      const text = await file.text();
      const parsed = parseAndValidateSequenceJson(text);
      if (!parsed.ok) {
        setLocalBanner({
          id: `imp-err-${Date.now()}`,
          tone: 'error',
          title: 'Archivo de secuencia incompatible o dañado',
          message: `${parsed.error} Se conservo intacto el programa abierto.`,
        });
        return;
      }
      const imported: Afr2SequenceDocument = {
        ...parsed.document,
        id: createSequenceUid('prog'),
        updatedAt: new Date().toISOString(),
      };
      setPrograms((prev) => [...prev, imported]);
      setActiveProgramId(imported.id);
      setUndoStack([]);
      setRedoStack([]);
      setSelection(null);
      setCursorMs(0);
      setLocalBanner({
        id: `imp-ok-${Date.now()}`,
        tone: 'info',
        title: 'Programa importado correctamente',
        message: `Se valido y abrio «${imported.name}» (esquema ${imported.schema}).`,
      });
    } catch {
      setLocalBanner({
        id: `imp-read-err-${Date.now()}`,
        tone: 'error',
        title: 'No se pudo leer el archivo seleccionado',
        message: 'El documento actual permanece sin cambios.',
      });
    }
  };

  const renderPropertiesContent = () => {
    if (!selection) {
      return (
        <div className="text-xs text-[var(--text-muted)] space-y-2">
          <p>
            Selecciona cualquier bloque de la linea de tiempo para inspeccionar o editar su duracion, consignas, rampa o sentido de giro.
          </p>
          <p className="font-mono text-[11px]">
            Tip: doble clic sobre el titulo de un bloque para renombrarlo; arrastra o usa <code>Alt+←</code> / <code>Alt+→</code> para reordenarlo.
          </p>
        </div>
      );
    }

    if (selection.scope === 'movements' && selectedMovementBlock) {
      const b = selectedMovementBlock;
      const idx = activeDoc.movementBlocks.findIndex((item) => item.id === b.id);
      const updateField = (patch: Partial<MovementBlock>) => {
        commitDocUpdate((doc) => ({
          ...doc,
          movementBlocks: doc.movementBlocks.map((item) =>
            item.id === b.id ? { ...item, ...patch } : item
          ),
        }));
      };

      return (
        <div className="space-y-3.5 text-xs">
          <div className="flex items-center justify-between gap-2">
            <span className="font-mono uppercase text-[11px] px-2 py-0.5 rounded-xs border border-[var(--border-default)] bg-[var(--surface-subtle)] text-[var(--text)]">
              Movimiento · {b.kind}
            </span>
            <span className="font-mono text-[var(--text-muted)]">
              Posicion #{idx + 1} de {activeDoc.movementBlocks.length}
            </span>
          </div>

          <div>
            <label className="block font-medium text-[var(--text-muted)] mb-1">
              Nombre del bloque
            </label>
            <input
              type="text"
              disabled={isLocked}
              value={b.label}
              onChange={(e) => updateField({ label: e.target.value })}
              className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm text-[var(--text)]"
            />
          </div>

          {b.kind !== "finish" && <CompletionFields completion={b.completion} distanceMm={b.distanceMm} durationMs={b.durationMs} estimatedMs={movementDurationMs(b, activeDoc.kinematicProfile)} disabled={isLocked} allowDistance={b.kind !== "wait"} onChange={updateField} />}

          {b.kind === 'constant' && (
            <div className="grid grid-cols-2 gap-3">
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  Avance `vx` (mm/s)
                </label>
                <input
                  type="number"
                  step={10}
                  disabled={isLocked}
                  value={b.vxMmS}
                  onChange={(e) => updateField({ vxMmS: Math.round(Number(e.target.value) || 0) })}
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  Lateral `vy` (mm/s)
                </label>
                <input
                  type="number"
                  step={10}
                  disabled={isLocked}
                  value={b.vyMmS}
                  onChange={(e) => updateField({ vyMmS: Math.round(Number(e.target.value) || 0) })}
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
            </div>
          )}

          {b.kind === 'ramp' && (
            <div className="grid grid-cols-2 gap-3">
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  `vx` inicial (mm/s)
                </label>
                <input
                  type="number"
                  step={10}
                  disabled={isLocked}
                  value={b.fromVxMmS}
                  onChange={(e) => updateField({ fromVxMmS: Math.round(Number(e.target.value) || 0) })}
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  `vx` final (mm/s)
                </label>
                <input
                  type="number"
                  step={10}
                  disabled={isLocked}
                  value={b.toVxMmS}
                  onChange={(e) => updateField({ toVxMmS: Math.round(Number(e.target.value) || 0) })}
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  `vy` inicial (mm/s)
                </label>
                <input
                  type="number"
                  step={10}
                  disabled={isLocked}
                  value={b.fromVyMmS}
                  onChange={(e) => updateField({ fromVyMmS: Math.round(Number(e.target.value) || 0) })}
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  `vy` final (mm/s)
                </label>
                <input
                  type="number"
                  step={10}
                  disabled={isLocked}
                  value={b.toVyMmS}
                  onChange={(e) => updateField({ toVyMmS: Math.round(Number(e.target.value) || 0) })}
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
            </div>
          )}

          {b.kind === 'pivot' && (
            <div className="space-y-3">
              <div>
                <span className="block font-medium text-[var(--text-muted)] mb-1.5">
                  Sentido de giro sobre el eje
                </span>
                <div className="grid grid-cols-2 gap-2">
                  <button
                    type="button"
                    disabled={isLocked}
                    aria-pressed={b.pivotDirection === 'ccw'}
                    onClick={() =>
                      updateField({
                        pivotDirection: 'ccw',
                        label: b.label === 'Girar derecha' ? 'Girar izquierda' : b.label,
                      })
                    }
                    className="afr2-spin-axis-btn rounded-[var(--radius-sm)] p-2.5 flex items-center gap-2 text-left"
                  >
                    <ChassisSpinGraphic direction="ccw" active={b.pivotDirection === 'ccw'} size="sm" />
                    <span className="font-semibold text-xs">Girar izquierda</span>
                  </button>
                  <button
                    type="button"
                    disabled={isLocked}
                    aria-pressed={b.pivotDirection === 'cw'}
                    onClick={() =>
                      updateField({
                        pivotDirection: 'cw',
                        label: b.label === 'Girar izquierda' ? 'Girar derecha' : b.label,
                      })
                    }
                    className="afr2-spin-axis-btn rounded-[var(--radius-sm)] p-2.5 flex items-center gap-2 text-left"
                  >
                    <ChassisSpinGraphic direction="cw" active={b.pivotDirection === 'cw'} size="sm" />
                    <span className="font-semibold text-xs">Girar derecha</span>
                  </button>
                </div>
              </div>
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  Velocidad angular `|wz|` (mrad/s)
                </label>
                <input
                  type="number"
                  min={50}
                  max={3140}
                  step={50}
                  disabled={isLocked}
                  value={Math.abs(b.wzMradS)}
                  onChange={(e) =>
                    updateField({ wzMradS: Math.abs(Math.round(Number(e.target.value) || 0)) })
                  }
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
            </div>
          )}

          {b.kind === 'curve' && (
            <div className="space-y-2.5">
              <div className="grid grid-cols-2 gap-2">
                <div>
                  <label className="block font-medium text-[var(--text-muted)] mb-1">
                    Avance `vx` (mm/s)
                  </label>
                  <input
                    type="number"
                    step={10}
                    disabled={isLocked}
                    value={b.vxMmS}
                    onChange={(e) => updateField({ vxMmS: Math.round(Number(e.target.value) || 0) })}
                    className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                  />
                </div>
                <div>
                  <label className="block font-medium text-[var(--text-muted)] mb-1">
                    Giro `|wz|` (mrad/s)
                  </label>
                  <input
                    type="number"
                    min={50}
                    max={3140}
                    step={50}
                    disabled={isLocked}
                    value={Math.abs(b.wzMradS)}
                    onChange={(e) =>
                      updateField({ wzMradS: Math.abs(Math.round(Number(e.target.value) || 0)) })
                    }
                    className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                  />
                </div>
              </div>
              <div className="afr2-segmented" role="group" aria-label="Lado de curva">
                <button
                  type="button"
                  disabled={isLocked}
                  aria-pressed={b.arcSide === 'left'}
                  onClick={() => updateField({ arcSide: 'left' })}
                  className="afr2-segmented-btn"
                >
                  ↶ Curva izquierda (+wz)
                </button>
                <button
                  type="button"
                  disabled={isLocked}
                  aria-pressed={b.arcSide === 'right'}
                  onClick={() => updateField({ arcSide: 'right' })}
                  className="afr2-segmented-btn"
                >
                  ↷ Curva derecha (-wz)
                </button>
              </div>
            </div>
          )}

          <div className="flex flex-wrap gap-2 pt-2 border-t border-[var(--border-default)]">
            <Button
              variant="secondary"
              size="md"
              disabled={isLocked}
              onClick={handleDuplicateSelected}
            >
              Duplicar
            </Button>
            <Button
              variant="danger"
              size="md"
              disabled={isLocked}
              onClick={handleDeleteSelected}
            >
              Eliminar
            </Button>
          </div>
        </div>
      );
    }

    if (selection.scope !== 'movements' && selectedWheelBlock) {
      const track = selection.scope;
      const b = selectedWheelBlock;
      const readOnly = activeDoc.editorLevel !== 'wheels' || isLocked;

      const updateWheelField = (patch: Partial<WheelBlock>) => {
        if (readOnly) return;
        commitDocUpdate((doc) => ({
          ...doc,
          wheelTracks: {
            ...doc.wheelTracks,
            [track]: doc.wheelTracks[track].map((item) =>
              item.id === b.id ? { ...item, ...patch } : item
            ),
          },
        }));
      };

      return (
        <div className="space-y-3.5 text-xs">
          <div className="flex items-center justify-between gap-2">
            <span className="font-mono uppercase text-[11px] px-2 py-0.5 rounded-xs border border-[var(--border-default)] bg-[var(--surface-subtle)] text-[var(--text)]">
              {WHEEL_TRACK_LABELS[track]} · {b.kind}
            </span>
            {activeDoc.editorLevel === 'movements' && (
              <span className="text-[11px] text-[var(--text-muted)]">Pista derivada (consulta)</span>
            )}
          </div>

          {activeDoc.editorLevel === 'movements' && (
            <StatusNotice tone="info" title="Pista derivada de solo consulta">
              Usa <strong>«Crear copia avanzada»</strong> para editar las cuatro ruedas de forma independiente sin modificar este programa original.
            </StatusNotice>
          )}

          <div>
            <label className="block font-medium text-[var(--text-muted)] mb-1">
              Nombre del bloque
            </label>
            <input
              type="text"
              disabled={readOnly}
              value={b.label}
              onChange={(e) => updateWheelField({ label: e.target.value })}
              className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm text-[var(--text)]"
            />
          </div>

          {b.kind !== "finish" && <CompletionFields completion={b.completion} distanceMm={b.distanceMm} durationMs={b.durationMs} estimatedMs={wheelDurationMs(b)} disabled={readOnly} allowDistance={b.kind !== "wait"} onChange={updateWheelField} />}

          {b.kind === 'constant' && (
            <div>
              <label className="block font-medium text-[var(--text-muted)] mb-1">
                Velocidad firmada de rueda (`mm/s`, + adelante / - reversa)
              </label>
              <input
                type="number"
                step={10}
                disabled={readOnly}
                value={b.speedMmS}
                onChange={(e) => {
                  const val = Math.round(Number(e.target.value) || 0);
                  updateWheelField({ speedMmS: val, fromSpeedMmS: val, toSpeedMmS: val });
                }}
                className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
              />
            </div>
          )}

          {b.kind === 'ramp' && (
            <div className="grid grid-cols-2 gap-3">
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  Velocidad inicial (`mm/s`)
                </label>
                <input
                  type="number"
                  step={10}
                  disabled={readOnly}
                  value={b.fromSpeedMmS}
                  onChange={(e) =>
                    updateWheelField({ fromSpeedMmS: Math.round(Number(e.target.value) || 0) })
                  }
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
              <div>
                <label className="block font-medium text-[var(--text-muted)] mb-1">
                  Velocidad final (`mm/s`)
                </label>
                <input
                  type="number"
                  step={10}
                  disabled={readOnly}
                  value={b.toSpeedMmS}
                  onChange={(e) => {
                    const val = Math.round(Number(e.target.value) || 0);
                    updateWheelField({ toSpeedMmS: val, speedMmS: val });
                  }}
                  className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
                />
              </div>
            </div>
          )}

          {!readOnly && (
            <div className="flex flex-wrap gap-2 pt-2 border-t border-[var(--border-default)]">
              <Button variant="secondary" size="md" onClick={handleDuplicateSelected}>
                Duplicar
              </Button>
              <Button variant="danger" size="md" onClick={handleDeleteSelected}>
                Eliminar
              </Button>
            </div>
          )}
        </div>
      );
    }

    return null;
  };

  const propertiesPanel = selection ? <section className="afr2-block-properties" aria-label="Propiedades del bloque seleccionado">
          <div className="afr2-action-row"><h3>Propiedades del bloque</h3><Button size="sm" onClick={() => setSelection(null)}>Cerrar</Button></div>
          {renderPropertiesContent()}
        </section> : null;

  return (
    <div className="afr2-sequence-workspace">
      {localBanner && (
        <DismissibleNotice
          id={localBanner.id}
          tone={localBanner.tone}
          title={localBanner.title}
          message={localBanner.message}
          onDismiss={() => setLocalBanner(null)}
        />
      )}

      {preview}

      {/* Barra de transporte: Simulacion (principal, offline) y Ejecucion fisica */}
      <div className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
        <div className="flex flex-wrap items-start justify-between gap-4">
          <div>
            <span className="text-xs font-mono uppercase tracking-wider text-[var(--text-muted)]">
              Simulacion y ejecucion
            </span>
            <h3 className="text-base font-semibold text-[var(--text)] mt-0.5">
              Prepara, simula y envia la secuencia
            </h3>
          </div>

          <div className="flex flex-wrap items-center gap-2 text-xs font-mono">
            <span className="px-2.5 py-1 rounded-md border border-[var(--border-default)] bg-[var(--surface-subtle)] text-[var(--text)]">
              Cursor: {(cursorMs / 1000).toFixed(2)} s / {(totalDurationMs / 1000).toFixed(2)} s
            </span>
            <span className="px-2.5 py-1 rounded-md border border-[var(--border-default)] bg-[var(--surface-subtle)] text-[var(--text)]">
              Bloques de ejecucion: {validation.normalized.segments.length}/{controllerSequenceLimits.maxSegments}
            </span>
          </div>
        </div>

        {/* Controles del simulador */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">
          <div className="rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)] p-4 space-y-3">
            <div className="flex items-center justify-between gap-2">
              <strong className="text-xs uppercase tracking-wider text-[var(--text)]">
                Simulacion local
              </strong>
              <span className="text-xs font-mono text-[var(--text-muted)]">
                {simPlaying ? 'Reproduciendo' : 'Detenido'}
              </span>
            </div>

            <div className="flex flex-wrap gap-2">
              {!simPlaying ? (
                <Button
                  variant="primary"
                  size="md"
                  disabled={!validation.editorValid || physicalState === 'running'}
                  onClick={() => setSimPlaying(true)}
                >
                  ▶ Reproducir simulacion
                </Button>
              ) : (
                <Button
                  variant="secondary"
                  size="md"
                  onClick={() => setSimPlaying(false)}
                >
                  ⏸ Pausar
                </Button>
              )}
              <Button
                variant="secondary"
                size="md"
                disabled={!simPlaying && cursorMs === 0}
                onClick={() => {
                  setSimPlaying(false);
                  setCursorMs(0);
                }}
              >
                ⏹ Detener / Reiniciar
              </Button>
            </div>

            <div>
              <div className="flex justify-between text-xs font-mono text-[var(--text-muted)] mb-1">
                <span>Desplazar cursor temporal (en detenido)</span>
                <span>{cursorMs} ms</span>
              </div>
              <input
                type="range"
                min={0}
                max={Math.max(10, totalDurationMs)}
                step={DURATION_STEP_MS}
                disabled={simPlaying || physicalState === 'running' || totalDurationMs <= 0}
                value={Math.min(cursorMs, Math.max(10, totalDurationMs))}
                onChange={(e) => setCursorMs(Number(e.target.value))}
                aria-label="Cursor temporal de la secuencia en milisegundos"
                className="w-full accent-[var(--action)]"
              />
            </div>

            <div className="grid grid-cols-4 gap-1.5 text-[11px] font-mono text-center">
              {WHEEL_TRACK_KEYS.map((k) => (
                <div
                  key={k}
                  className="rounded-xs border border-[var(--border-default)] bg-[var(--surface-solid)] px-2 py-1"
                >
                  <span className="text-[var(--text-muted)] uppercase">{k}: </span>
                  <strong className="text-[var(--text)]">{simSnapshot.wheelSpeedsMmS[k]} mm/s</strong>
                </div>
              ))}
            </div>
          </div>

          {/* Controles de ejecucion fisica (solo Iniciar y Cancelar) */}
          <div className="rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)] p-4 space-y-3">
            <div className="flex items-center justify-between gap-2">
              <strong className="text-xs uppercase tracking-wider text-[var(--text)]">
                Enviar al robot
              </strong>
              <span className="text-xs font-mono text-[var(--text-muted)]">
                Estado: {PHYSICAL_STATE_LABELS[physicalState]}
              </span>
            </div>

            <div className="flex flex-wrap gap-2">
              <Button
                variant="primary"
                size="md"
                disabled={!canStartPhysical}
                onClick={() => void handleStartPhysicalSequence()}
              >
                Enviar al robot
              </Button>

            </div>

            {physicalBlockedReason && (
              <p className="text-xs text-[var(--text-muted)] leading-relaxed">
                <strong className="text-[var(--text)]">Motivo de bloqueo fisico: </strong>
                {physicalBlockedReason}
              </p>
            )}

            {physicalDetail && (
              <p className="text-xs font-mono text-[var(--text)]">
                {physicalDetail}
                {physicalStatus && physicalState === 'running' && (
                  <>
                    {' '}
                    · Seg. {(physicalStatus.activeSegmentIndex ?? 0) + 1}/{physicalStatus.segmentCount} ·{' '}
                    {(physicalStatus.elapsedMs / 1000).toFixed(2)} s
                  </>
                )}
              </p>
            )}
          </div>
        </div>
      </div>

      {/* Biblioteca de programas, importacion/exportacion JSON y perfil cinematico */}
      <div className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
        <div className="flex flex-wrap items-center justify-between gap-3">
          <div className="flex flex-wrap items-center gap-2 min-w-0 flex-1">
            <label htmlFor="afr2-seq-library-select" className="text-xs font-medium text-[var(--text-muted)]">
              Programa:
            </label>
            <span className="afr2-select-wrap"><select
              id="afr2-seq-library-select"
              disabled={isLocked}
              value={activeDoc.id}
              onChange={(e) => {
                setActiveProgramId(e.target.value);
                setUndoStack([]);
                setRedoStack([]);
                setSelection(null);
                setCursorMs(0);
              }}
              className="afr2-glass-field min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm text-[var(--text)]"
            >
              {programs.map((p) => (
                <option key={p.id} value={p.id}>
                  {p.name} ({p.editorLevel === 'movements' ? 'Movimientos' : '4 ruedas'})
                </option>
              ))}
            </select><Chevron /></span>

            <input
              type="text"
              disabled={isLocked}
              value={activeDoc.name}
              aria-label="Renombrar programa activo"
              onChange={(e) => commitDocUpdate((d) => ({ ...d, name: e.target.value }), false)}
              className="afr2-glass-field min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm text-[var(--text)] w-56"
            />
          </div>

          <div className="flex flex-wrap items-center gap-2">
            <Button
              variant="secondary"
              size="md"
              disabled={isLocked || undoStack.length === 0}
              onClick={handleUndo}
            >
              ↶ Deshacer ({undoStack.length})
            </Button>
            <Button
              variant="secondary"
              size="md"
              disabled={isLocked || redoStack.length === 0}
              onClick={handleRedo}
            >
              ↷ Rehacer ({redoStack.length})
            </Button>
            <Button
              variant="secondary"
              size="md"
              disabled={isLocked}
              onClick={() => {
                const fresh = createStarterSequenceDocument(`Programa ${programs.length + 1}`);
                fresh.id = createSequenceUid('prog');
                setPrograms((prev) => [...prev, fresh]);
                setActiveProgramId(fresh.id);
                setUndoStack([]);
                setRedoStack([]);
                setSelection(null);
                setCursorMs(0);
              }}
            >
              + Nuevo
            </Button>
            <Button
              variant="secondary"
              size="md"
              onClick={() => exportSequenceDocumentAsJson(activeDoc)}
            >
              Exportar JSON
            </Button>
            <Button
              variant="secondary"
              size="md"
              disabled={isLocked}
              onClick={() => fileInputRef.current?.click()}
            >
              Importar JSON
            </Button>
            <input
              ref={fileInputRef}
              type="file"
              accept=".json,application/json"
              onChange={handleImportFile}
              className="hidden"
            />
          </div>
        </div>

        {/* Nivel de edicion y procedencia */}
        <div className="flex flex-wrap items-center justify-between gap-3 pt-2 border-t border-[var(--border-default)]">
          <div className="flex flex-wrap items-center gap-2">
            <span className="text-xs font-medium text-[var(--text-muted)]">
              Modo de edicion:
            </span>
            <span className="afr2-select-wrap"><select className="afr2-glass-field min-h-11 px-3" aria-label="Modo de edicion" disabled={isLocked} value={activeDoc.editorLevel} onChange={event => {
              const editorLevel = event.target.value as 'movements' | 'wheels';
              commitDocUpdate(doc => ({ ...doc, editorLevel, wheelDraftReady: true, wheelTracks: editorLevel === 'wheels' && !doc.wheelDraftReady && doc.editorLevel === 'movements' ? deriveWheelTracksFromMovements(doc.movementBlocks, doc.kinematicProfile) : doc.wheelTracks }));
              setSelection(null); setSelectedTrack(null); setCursorMs(0);
            }}><option value="movements">Movimientos del chasis</option><option value="wheels">Pistas independientes</option></select><Chevron /></span>
            {activeDoc.editorLevel === 'wheels' && <label className="afr2-track-count">Pistas independientes<span className="afr2-select-wrap"><select ref={trackCountRef} className="afr2-glass-field min-h-11 px-3" disabled={isLocked} value={enabledTracks.length} onChange={event => {
              const count = Number(event.target.value);
              const next = [...enabledTracks, ...WHEEL_TRACK_KEYS.filter(key => !enabledTracks.includes(key))].slice(0, count);
              commitDocUpdate(doc => ({ ...doc, enabledWheelTracks: next })); setSelection(null); setSelectedTrack(null); setCursorMs(0);
            }}>{[0, 1, 2, 3, 4].map(count => <option key={count} value={count}>{count}</option>)}</select><Chevron /></span></label>}
            <span className="px-2.5 py-1 rounded-md border border-[var(--border-default)] bg-[var(--surface-subtle)] text-xs font-semibold text-[var(--text)]">
              {activeDoc.editorLevel === 'movements'
                ? 'Movimientos del chasis · Cuatro ruedas derivadas'
                : `${enabledTracks.length} pistas independientes (${enabledTracks.map(key => key.toUpperCase()).join(' / ') || 'sin pistas activas'})`}
            </span>
            {activeDoc.provenance && (
              <span className="text-xs font-mono text-[var(--text-muted)]">
                Derivado de «{activeDoc.provenance.convertedFromName}»
              </span>
            )}
          </div>

          {activeDoc.editorLevel === 'movements' && (
            <Button
              variant="secondary"
              size="md"
              disabled={isLocked}
              onClick={handleCreateAdvancedWheelCopy}
            >
              Crear copia avanzada (4 ruedas editables)
            </Button>
          )}
        </div>

        {/* Perfil cinematico configurable (etiquetado explicitamente como no calibrado por defecto) */}
        <div className="grid grid-cols-1 md:grid-cols-3 gap-3 pt-2 border-t border-[var(--border-default)] text-xs">
          <div>
            <label className="block font-medium text-[var(--text-muted)] mb-1">
              Geometria `lx + ly` (mm · inicial 300 mm no calibrado)
            </label>
            <input
              type="number"
              min={80}
              max={1200}
              step={5}
              disabled={isLocked}
              value={activeDoc.kinematicProfile.lxPlusLyMm}
              onChange={(e) =>
                commitDocUpdate((d) => ({
                  ...d,
                  kinematicProfile: {
                    ...d.kinematicProfile,
                    lxPlusLyMm: Math.max(80, Math.round(Number(e.target.value) || 300)),
                  },
                }))
              }
              className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
            />
          </div>
          <div>
            <label className="block font-medium text-[var(--text-muted)] mb-1">
              Velocidad max. rueda (mm/s · inicial 800 mm/s no calibrado)
            </label>
            <input
              type="number"
              min={100}
              max={2500}
              step={20}
              disabled={isLocked}
              value={activeDoc.kinematicProfile.maxWheelSpeedMmS}
              onChange={(e) =>
                commitDocUpdate((d) => ({
                  ...d,
                  kinematicProfile: {
                    ...d.kinematicProfile,
                    maxWheelSpeedMmS: Math.max(100, Math.round(Number(e.target.value) || 800)),
                  },
                }))
              }
              className="afr2-glass-field w-full min-h-11 rounded-[var(--radius-sm)] px-3 py-2 text-sm font-mono text-[var(--text)]"
            />
          </div>
          <div className="flex items-end">
            <label className="flex items-center gap-3 min-h-11 px-3 py-2 rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)] cursor-pointer w-full">
              <input
                type="checkbox"
                disabled={isLocked}
                checked={activeDoc.kinematicProfile.calibrated}
                onChange={(e) =>
                  commitDocUpdate((d) => ({
                    ...d,
                    kinematicProfile: {
                      ...d.kinematicProfile,
                      calibrated: e.target.checked,
                    },
                  }))
                }
              />
              <span className="text-xs text-[var(--text)]">
                Perfil fisico verificado/calibrado (requerido para habilitar ejecucion fisica)
              </span>
            </label>
          </div>
        </div>
      </div>

      {/* Avisos de validacion sin recorte silencioso */}
      {validation.issues.length > 0 && (
        <div className="space-y-2">
          {validation.issues.map((issue) => (
            <StatusNotice
              key={issue.id}
              tone={issue.severity === 'error' ? 'error' : 'warning'}
              title={
                issue.severity === 'error'
                  ? 'Error de validacion en secuencia'
                  : 'Advertencia de limite fisico'
              }
            >
              {issue.message}
            </StatusNotice>
          ))}
        </div>
      )}

      {/* Cuerpo principal: Paleta + Lineas de tiempo + Panel lateral de propiedades (escritorio) */}
      <div className="afr2-sequence-editor-body">
        <div className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
          {/* Paleta para agregar bloques */}
          {activeDoc.editorLevel === 'movements' ? (
            <div className="space-y-2">
              <span className="block text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
                Añadir bloque de movimiento al auto
              </span>
              <div className="flex flex-wrap gap-2">
                {!activeDoc.movementBlocks.some(block => block.kind === 'finish') && MOVEMENT_PALETTE.map((item) => (
                  <Button
                    key={item.kind}
                    variant="secondary"
                    size="md"
                    disabled={isLocked}
                    onClick={() => handleAddMovementBlock(item.kind)}
                  >
                    + {item.label}
                  </Button>
                ))}
              </div>
            </div>
          ) : (
            <div className="space-y-2.5">
              <div className="flex flex-wrap items-center justify-between gap-2">
                <span className="text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
                  Añadir bloque a pista de rueda:
                </span>
                <div className="afr2-segmented" role="group" aria-label="Pista de rueda destino">
                  {enabledTracks.map((tk) => (
                    <button
                      key={tk}
                      type="button"
                      aria-pressed={activeWheelTrackForAdd === tk}
                      onClick={() => { setActiveWheelTrackForAdd(tk); setSelectedTrack(tk); }}
                      className="afr2-segmented-btn uppercase font-mono"
                    >
                      {tk.toUpperCase()}
                    </button>
                  ))}
                </div>
              </div>
              <div className="flex flex-wrap gap-2">
                {enabledTracks.includes(activeWheelTrackForAdd) && !activeDoc.wheelTracks[activeWheelTrackForAdd].some(block => block.kind === 'finish') && WHEEL_PALETTE.map((item) => (
                  <Button
                    key={item.kind}
                    variant="secondary"
                    size="md"
                    disabled={isLocked}
                    onClick={() => handleAddWheelBlock(item.kind, activeWheelTrackForAdd)}
                  >
                    + {item.label} ({activeWheelTrackForAdd.toUpperCase()})
                  </Button>
                ))}
              </div>
            </div>
          )}

          {/* Pista de movimientos (cuando el documento esta en Nivel 1) */}
          {activeDoc.editorLevel === 'movements' && (
            <TimelineRow
              trackId="movements"
              title="Secuencia de movimientos del chasis"
              subtitle="Nivel 1 editable"
              blocks={movementVisualBlocks}
              scaleTotalMs={totalDurationMs}
              cursorMs={cursorMs}
              selectedId={selection?.scope === 'movements' ? selection.blockId : null}
              trackSelected={selectedTrack === 'movements'}
              onTrackSelect={() => setSelectedTrack('movements')}
              details={selection?.scope === 'movements' ? propertiesPanel : null}
              readOnly={isLocked}
              onSelect={(id) => {
                setSelection(id ? { scope: 'movements', blockId: id } : null);
              }}
              onReorder={(nextIds) => {
                commitDocUpdate((doc) => {
                  const map = new Map(doc.movementBlocks.map((b) => [b.id, b]));
                  return {
                    ...doc,
                    movementBlocks: nextIds
                      .map((id) => map.get(id))
                      .filter((b): b is MovementBlock => Boolean(b)),
                  };
                });
              }}
              onLabelChange={(blockId, newLabel) => {
                commitDocUpdate((doc) => ({
                  ...doc,
                  movementBlocks: doc.movementBlocks.map((b) =>
                    b.id === blockId ? { ...b, label: newLabel } : b
                  ),
                }));
              }}
            />
          )}

          {/* Cuatro pistas sincronizadas FL / FR / RL / RR */}
          <div className="space-y-3 pt-2 border-t border-[var(--border-default)]">
            <div className="flex flex-wrap items-center justify-between gap-2">
              <span className="text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
                {activeDoc.editorLevel === 'movements'
                  ? 'Cuatro pistas sincronizadas FL / FR / RL / RR (derivadas en consulta)'
                  : `${enabledTracks.length} pistas independientes · Las ruedas sin pista permanecen detenidas`}
              </span>
            </div>

            {visibleTracks.map((tk) => (
              <TimelineRow
                key={tk}
                trackId={tk}
                title={WHEEL_TRACK_LABELS[tk]}
                subtitle={
                  activeDoc.editorLevel === 'movements' ? 'Derivada de movimientos' : 'Editable'
                }
                trackSelected={selectedTrack === tk}
                onTrackSelect={() => { setSelectedTrack(tk); setActiveWheelTrackForAdd(tk); }}
                headerAction={activeDoc.editorLevel === 'wheels' && <button type="button" className="afr2-track-delete" disabled={isLocked} aria-label={`Eliminar pista ${tk.toUpperCase()}`} onClick={event => { event.stopPropagation(); deletedTrack.current = false; setPendingTrackDelete({ program: activeDoc.id, track: tk }); }}><svg viewBox="0 0 24 24" width="18" height="18" fill="none" stroke="currentColor" strokeWidth="1.6" aria-hidden="true"><path d="M4 6h16M9 6V3h6v3M6 6l1 15h10l1-15M10 10v7m4-7v7" /></svg></button>}
                blocks={wheelVisualTracks[tk]}
                scaleTotalMs={totalDurationMs}
                cursorMs={cursorMs}
                selectedId={selection?.scope === tk ? selection.blockId : null}
                details={selection?.scope === tk ? propertiesPanel : null}
                readOnly={activeDoc.editorLevel !== 'wheels' || isLocked}
                onSelect={(id) => {
                  setActiveWheelTrackForAdd(tk);
                  setSelection(id ? { scope: tk, blockId: id } : null);
                }}
                onReorder={(nextIds) => {
                  if (activeDoc.editorLevel !== 'wheels') return;
                  commitDocUpdate((doc) => {
                    const map = new Map(doc.wheelTracks[tk].map((b) => [b.id, b]));
                    return {
                      ...doc,
                      wheelTracks: {
                        ...doc.wheelTracks,
                        [tk]: nextIds
                          .map((id) => map.get(id))
                          .filter((b): b is WheelBlock => Boolean(b)),
                      },
                    };
                  });
                }}
                onLabelChange={(blockId, newLabel) => {
                  if (activeDoc.editorLevel !== 'wheels') return;
                  commitDocUpdate((doc) => ({
                    ...doc,
                    wheelTracks: {
                      ...doc.wheelTracks,
                      [tk]: doc.wheelTracks[tk].map((b) =>
                        b.id === blockId ? { ...b, label: newLabel } : b
                      ),
                    },
                  }));
                }}
              />
            ))}
          </div>
        </div>


      </div>

      <FloatingModal open={pendingTrackDelete !== null} title="Eliminar pista" description={`Se eliminaran los bloques de ${pendingTrackDelete?.track.toUpperCase() ?? 'la pista'}. Podes deshacer esta accion.`} onClose={() => { setPendingTrackDelete(null); if (deletedTrack.current) { deletedTrack.current = false; trackCountRef.current?.focus(); } }}>
        <div className="afr2-action-row"><Button onClick={() => setPendingTrackDelete(null)}>Conservar pista</Button><Button variant="danger" disabled={isLocked || pendingTrackDelete?.program !== activeDoc.id} onClick={() => {
          if (!pendingTrackDelete || isLocked || pendingTrackDelete.program !== activeDoc.id || activeDoc.editorLevel !== 'wheels' || !enabledTracks.includes(pendingTrackDelete.track)) return;
          const track = pendingTrackDelete.track;
          commitDocUpdate(doc => ({ ...doc, enabledWheelTracks: (doc.enabledWheelTracks ?? [...WHEEL_TRACK_KEYS]).filter(key => key !== track), wheelTracks: { ...doc.wheelTracks, [track]: [] } }));
          setSelection(null); setSelectedTrack(null); deletedTrack.current = true; setPendingTrackDelete(null);
        }}>Eliminar pista</Button></div>
      </FloatingModal>
    </div>
  );
}
