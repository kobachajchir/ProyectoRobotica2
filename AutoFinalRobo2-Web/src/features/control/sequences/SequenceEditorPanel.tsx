import { Chevron } from '../../../components/ui/Chevron';
import React, { useCallback, useEffect, useMemo, useRef, useState } from 'react';
import { Button } from '../../../components/ui/Button';
import { UiIcon } from '../../../components/ui/UiIcon';
import { SegmentedControl } from '../../../components/ui/SegmentedControl';
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
import type { SimulationMapFeed } from '../ControlMap';

export interface SequenceEditorPanelProps {
  preview?: (feed: SimulationMapFeed) => React.ReactNode;
}

type SelectionTarget =
  | { scope: 'movements'; blockId: string }
  | { scope: WheelTrackKey; blockId: string }
  | null;

const MOVEMENT_PALETTE: { kind: MovementBlockKind; label: string; hint: string }[] = [
  { kind: 'constant', label: 'Desplazamiento', hint: 'vx / vy constante' },
  { kind: 'ramp', label: 'Rampa', hint: 'Interpolación lineal' },
  { kind: 'pivot', label: 'Giro sobre eje', hint: 'CCW / CW' },
  { kind: 'curve', label: 'Curva', hint: 'Avance + giro' },
  { kind: 'wait', label: 'Espera', hint: 'Consigna 0 mm/s' },
  { kind: 'finish', label: 'Finalización', hint: 'Terminal (0 ms)' },
];

const WHEEL_PALETTE: { kind: WheelBlockKind; label: string; hint: string }[] = [
  { kind: 'constant', label: 'Velocidad', hint: 'Consigna firmada mm/s' },
  { kind: 'ramp', label: 'Rampa', hint: 'Inicio → fin mm/s' },
  { kind: 'wait', label: 'Espera', hint: 'Consigna 0 mm/s' },
  { kind: 'finish', label: 'Finalización', hint: 'Terminal (0 ms)' },
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
      return `${block.durationMs} ms · ${block.pivotDirection === 'ccw' ? 'CCW +' : 'CW -'}${Math.abs(block.wzMradS)} mrad/s`;
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

export function SequenceEditorPanel({ preview }: SequenceEditorPanelProps) {
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

  // Selección y propiedades móviles
  const [selection, setSelection] = useState<SelectionTarget>(null);
  const [activeWheelTrackForAdd, setActiveWheelTrackForAdd] = useState<WheelTrackKey>('fl');
  const [selectedTrack, setSelectedTrack] = useState<WheelTrackKey | 'movements' | null>(null);
  const [timelineScroll, setTimelineScroll] = useState(0);
  const [linkDurations, setLinkDurations] = useState(true);
  const [restoreChassis, setRestoreChassis] = useState(false);
  const [pendingTrackDelete, setPendingTrackDelete] = useState<{ program: string; track: WheelTrackKey } | null>(null);
  const deletedTrack = useRef(false);
  const trackCountRef = useRef<HTMLSelectElement>(null);
  const enabledTracks = activeDoc.enabledWheelTracks ?? [...WHEEL_TRACK_KEYS];
  const visibleTracks = activeDoc.editorLevel === 'movements' ? [...WHEEL_TRACK_KEYS] : enabledTracks;
  useEffect(() => { if (!enabledTracks.includes(activeWheelTrackForAdd) && enabledTracks.length) setActiveWheelTrackForAdd(enabledTracks[0]); }, [enabledTracks, activeWheelTrackForAdd]);

  // Avisos locales cerrables (importación/conversión/ejecución)
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

  // Estado de ejecución física (ROBO2 perfil 2 / EX01 v4: 0x40-0x45)
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

  // Límites anunciados por el controlador o valores iniciales por defecto (32 seg / 120 s)
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

  // Evaluación determinista en el cursor actual
  const simSnapshot = useMemo(
    () => evaluateSequenceSimulationAtTime(activeDoc, Math.min(cursorMs, totalDurationMs)),
    [activeDoc, cursorMs, totalDurationMs]
  );
  const plannedSnapshot = useMemo(() => evaluateSequenceSimulationAtTime(activeDoc, totalDurationMs), [activeDoc, totalDurationMs]);
  useEffect(() => { setCursorMs(value => Math.min(value, totalDurationMs)); }, [totalDurationMs]);
  const previewFeed: SimulationMapFeed = {
    pose: { schema: 'afr2-pose-1', frameId: 'sim-mecanum-local', sequence: Math.round(simSnapshot.cursorMs/10), xMm: simSnapshot.xMm, yMm: simSnapshot.yMm, thetaMrad: simSnapshot.thetaMrad, ageMs: 0 },
    trail: simSnapshot.trail, plannedRoute: plannedSnapshot.trail, wheelAngles: simSnapshot.wheelAngles,
    cursorMs: simSnapshot.cursorMs, totalDurationMs, isPlaying: simPlaying,
    profileSummary: 'Vista previa de las pistas activas · Reloj compartido para las cuatro ruedas.',
    onResetOrigin: () => { setSimPlaying(false); setCursorMs(0); },
  };

  const isLocked =
    simPlaying ||
    physicalState === 'preparing' ||
    physicalState === 'ready' ||
    physicalState === 'running';

  // Actualizar documento activo registrando historial undo/redo
  const commitDocUpdate = useCallback(
    (updater: (prev: Afr2SequenceDocument) => Afr2SequenceDocument, recordHistory = true) => {
      if (isLocked) return;
      const next = updater(activeDoc);
      const updated: Afr2SequenceDocument = {
        ...next,
        wheelDraftReady: next.editorLevel === 'movements' && (next.movementBlocks !== activeDoc.movementBlocks || next.kinematicProfile !== activeDoc.kinematicProfile) ? false : next.wheelDraftReady,
        updatedAt: new Date().toISOString(),
      };
      if (recordHistory) { setUndoStack(u => [...u.slice(-29), activeDoc]); setRedoStack([]); }
      setPrograms(prevList => prevList.map(item => item.id === activeDoc.id ? updated : item));
    },
    [activeDoc, isLocked]
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

  // Reproducción de simulación con requestAnimationFrame
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

  // Cancelación física inmediata ante ocultamiento de página, pérdida de enlace, STOP o desmontaje
  const cancelPhysicalExecution = useCallback(
    async (
      reason: 'user-cancel' | 'page-hidden' | 'route-left' | 'disconnect' | 'stop'
    ) => {
      const activeExec = physicalActiveRef.current;
      ++preparationGeneration.current;
      physicalActiveRef.current = null;
      setPhysicalState('cancelled');
      setPhysicalDetail(`Ejecución física cancelada (${reason}). No se reanuda automáticamente.`);

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
        // Si falla la cancelación de secuencia, emitir STOP prioritario de respaldo
        await triggerEmergencyStop('safety');
      }
    },
    [activeDoc.id, physicalExecId, requestCommand, triggerEmergencyStop]
  );

  // Pausa de simulación y cancelación física al ocultar la página
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

  // Cancelar ejecución física al desmontar Secuencias / salir de Control
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

  // Cancelar si se pierde conexión durante preparación/ejecución física
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

  // Si se emite una nueva orden STOP global, cancelar cualquier secuencia física activa
  useEffect(() => {
    if (stopRecord.status === 'sending' && physicalActiveRef.current) {
      physicalActiveRef.current = null;
      setPhysicalState('cancelled');
      setPhysicalDetail('Secuencia cancelada por orden prioritaria STOP.');
    }
  }, [stopRecord.noticeId, stopRecord.status]);

  // Consulta de progreso físico cada 250 ms sin superponer solicitudes
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
                ? 'Secuencia física completada por el ATmega2560 (salidas en 0).'
                : `Secuencia física finalizada en estado ${st.state}${st.reason ? `: ${st.reason}` : ''}.`
            );
            return;
          }
        }
      } catch {
        // La pérdida de enlace lo cancela desde el efecto supervisor
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

  // Disponibilidad y motivo de bloqueo para ejecución física
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
        ? capabilities.mecanum_drive.reason || 'Control PWM/dirección de motores y STOP aún no habilitados en firmware.'
        : !sequencesReady
          ? capabilities.sequence_runner?.reason || 'Capacidad de secuencias físicas (ROBO2 perfil 2 / EX01 v4, 0x40–0x45) pendiente en controlador.'
          : !profileCalibrated
            ? 'El perfil cinemático figura como ilustrativo no calibrado. Confirmá un perfil físico calibrado antes de ejecutar en hardware.'
            : !profileMatchesController || !speedsMatchController
              ? 'La geometría o las velocidades del programa no coinciden con los límites reales informados por el ATmega.'
            : !validation.editorValid
              ? 'El programa tiene errores de validación que deben corregirse.'
              : physicalProgram.error || (physicalProgram.mode !== 'timed' && !capabilities.sequence_runner.limits?.distanceBlocks)
                ? physicalProgram.error || 'La ejecución por distancia requiere EX01 v6 y Hall/calibración/memoria aprobados.'
              : physicalProgram.mode === 'timed' && !validation.physicalValid
                ? `El programa excede los límites físicos del controlador (${controllerSequenceLimits.maxSegments} segmentos / ${(controllerSequenceLimits.maxDurationMs / 1000).toFixed(0)} s). No se trunca automáticamente.`
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
      `Preparando ${segments.length} instrucciones (${(physicalDurationMs / 1000).toFixed(2)} s${physicalProgram.mode === 'timed' ? '' : ' de límite de seguridad; finalización por Hall'})…`
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
        throw new Error('El controlador rechazó robot.sequence.begin.');
      }

      // 2. append (EX01 v4 0x41) — exactamente un segmento por solicitud
      for (const seg of segments) {
        if (physicalActiveRef.current?.executionId !== executionId) {
          throw new Error('Carga interrumpida antes de completar append.');
        }
        setPhysicalDetail(
          `Cargando segmento ${seg.index + 1} de ${segments.length} en buffer estático AVR…`
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
        throw new Error('El controlador no confirmó la integridad del programa (commit).');
      }
      if (physicalActiveRef.current?.executionId !== executionId) throw new Error('Ejecución cancelada antes del inicio.');

      // 4. start (EX01 v4 0x43) — aceptación de inicio no equivale a finalización
      setPhysicalDetail(physicalProgram.mode === 'timed' ? 'Iniciando ejecución temporizada en el ATmega2560…' : 'Iniciando ejecución por pulsos Hall con timeout de seguridad…');
      const startRes = await requestCommand<PhysicalSequenceStatusPayload>(
        'robot.sequence.start',
        { sequenceId, executionId },
        2500
      );
      if (!startRes.ok || startRes.status !== 'completed' || startRes.payload.state !== 'running') {
        throw new Error('El controlador rechazó robot.sequence.start.');
      }

      if (physicalActiveRef.current?.executionId !== executionId) return;
      setPhysicalState('running');
      setPhysicalDetail(
        'Ejecutando en ATmega2560 (temporización autoritativa en MCU; consultando progreso cada 250 ms).'
      );
      notify.info({
        id: 'afr2-seq-physical',
        title: 'Secuencia iniciada en el controlador',
        description: `${activeDoc.name} (${segments.length} segmentos). La aceptación de inicio no equivale a finalización.`,
      });
    } catch (err) {
      // A late load/start failure cannot cancel or overwrite a newer run.
      if (!mounted.current || generation !== preparationGeneration.current || latestStop.current !== stopAtStart || (physicalActiveRef.current && physicalActiveRef.current.executionId !== executionId)) return;
      physicalActiveRef.current = null;
      void triggerEmergencyStop('safety');
      const msg =
        err instanceof Error ? err.message : 'Fallo durante la carga o inicio físico de la secuencia.';
      setPhysicalState('failed');
      setPhysicalDetail(`Carga o ejecución fallida: ${msg}. Una carga incompleta jamás queda ejecutable.`);
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
    if (activeDoc.editorLevel !== 'movements' || !selection || selection.scope !== 'movements') return null;
    return activeDoc.movementBlocks.find((b) => b.id === selection.blockId) ?? null;
  }, [activeDoc.editorLevel, activeDoc.movementBlocks, selection]);

  const selectedWheelBlock = useMemo(() => {
    if (!selection || selection.scope === 'movements') return null;
    const track = selection.scope;
    return validation.normalized.effectiveTracks[track].find((b) => b.id === selection.blockId) ?? null;
  }, [validation.normalized.effectiveTracks, selection]);

  // Acciones de edición de bloques
  const handleAddMovementBlock = (kind: MovementBlockKind) => {
    if (isLocked) return;
    if (activeDoc.editorLevel === 'wheels') {
      const newMovement = createDefaultMovementBlock(kind);
      const derived = deriveWheelTracksFromMovements([newMovement], activeDoc.kinematicProfile);
      const nextTracks = { ...activeDoc.wheelTracks };
      const addedIds: Partial<Record<WheelTrackKey, string>> = {};
      for (const key of WHEEL_TRACK_KEYS) {
        const source = enabledTracks.includes(key) ? activeDoc.wheelTracks[key] : [];
        const list = source.filter(block => block.kind !== 'finish');
        const elapsed = list.reduce((sum, block) => sum + wheelDurationMs(block), 0);
        const wait = totalDurationMs > elapsed ? [{ ...createDefaultWheelBlock('wait'), label: 'Sincronizar inicio', durationMs: totalDurationMs-elapsed }] : [];
        const terminal = source.find(block => block.kind === 'finish');
        const addition = derived[key].map(block => ({ ...block, id: createSequenceUid(`whl-${key}`) }));
        addedIds[key] = addition[0]?.id;
        nextTracks[key] = [...list, ...wait, ...addition, ...(terminal && kind !== 'finish' ? [terminal] : [])];
        if (nextTracks[key].length > EDITOR_MAX_BLOCKS_PER_TRACK) {
          setLocalBanner({ id: `limit-${Date.now()}`, tone: 'warning', title: 'No se agregó el movimiento', message: 'Una pista no tiene espacio para el bloque y su espera de sincronización.' }); return;
        }
      }
      commitDocUpdate(doc => ({ ...doc, wheelTracks: nextTracks, enabledWheelTracks: [...WHEEL_TRACK_KEYS], wheelDraftReady: true }));
      if (addedIds[activeWheelTrackForAdd]) setSelection({ scope: activeWheelTrackForAdd, blockId: addedIds[activeWheelTrackForAdd]! });
      setCursorMs(totalDurationMs); return;
    }
    if (activeDoc.movementBlocks.length >= EDITOR_MAX_BLOCKS_PER_TRACK) {
      setLocalBanner({
        id: `limit-${Date.now()}`,
        tone: 'warning',
        title: 'Límite de pista alcanzado',
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
    setCursorMs(totalDurationMs);
  };

  const handleAddWheelBlock = (kind: WheelBlockKind, track: WheelTrackKey = activeWheelTrackForAdd) => {
    if (isLocked || activeDoc.editorLevel !== 'wheels') return;
    if (!enabledTracks.includes(track) || kind === 'finish' && activeDoc.wheelTracks[track].some(block => block.kind === 'finish')) return;
    if (activeDoc.wheelTracks[track].length >= EDITOR_MAX_BLOCKS_PER_TRACK) {
      setLocalBanner({
        id: `limit-${Date.now()}`,
        tone: 'warning',
        title: 'Límite de pista alcanzado',
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
    setCursorMs(activeDoc.wheelTracks[track].reduce((sum, block) => sum + wheelDurationMs(block), 0));
  };

  const alignTrackEnds = () => {
    if (isLocked || activeDoc.editorLevel !== 'wheels') return;
    const next = { ...activeDoc.wheelTracks };
    for (const key of enabledTracks) {
      const list = next[key].filter(block => block.kind !== 'finish');
      const duration = list.reduce((sum, block) => sum + wheelDurationMs(block), 0);
      if (duration >= totalDurationMs) continue;
      if (next[key].length >= EDITOR_MAX_BLOCKS_PER_TRACK) {
        setLocalBanner({ id: `sync-${Date.now()}`, tone: 'warning', title: 'No se igualaron las pistas', message: `La pista ${key.toUpperCase()} no tiene espacio para una espera.` }); return;
      }
      const terminal = next[key].find(block => block.kind === 'finish');
      next[key] = [...list, { ...createDefaultWheelBlock('wait'), label: 'Esperar al resto', durationMs: totalDurationMs-duration }, ...(terminal ? [terminal] : [])];
    }
    commitDocUpdate(doc => ({ ...doc, wheelTracks: next }));
  };

  const selectBlock = (scope: WheelTrackKey | 'movements', blockId: string | null) => {
    setSelection(blockId ? { scope, blockId } : null); setSelectedTrack(scope);
    if (scope !== 'movements') setActiveWheelTrackForAdd(scope);
    if (!blockId || isLocked) return;
    const blocks = scope === 'movements' ? movementVisualBlocks : wheelVisualTracks[scope];
    let start = 0;
    for (const block of blocks) {
      if (block.id === blockId) { setCursorMs(Math.min(totalDurationMs, start + Math.min(10, block.durationMs))); return; }
      start += block.durationMs;
    }
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
      message: `Se creó «${copyDoc.name}» para editar cada rueda (FL/FR/RL/RR) de forma independiente. El programa original «${activeDoc.name}» se conserva intacto en la biblioteca.`,
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
          message: `${parsed.error} Se conservó intacto el programa abierto.`,
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
        message: `Se validó y abrió «${imported.name}» (esquema ${imported.schema}).`,
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
            Seleccioná cualquier bloque de la línea de tiempo para inspeccionar o editar su duración, consignas, rampa o sentido de giro.
          </p>
          <p className="font-mono text-[11px]">
            Tip: doble clic sobre el título de un bloque para renombrarlo; arrastrá o usá <code>Alt+←</code> / <code>Alt+→</code> para reordenarlo.
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
              Posición #{idx + 1} de {activeDoc.movementBlocks.length}
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
                        label: b.label === 'Girar CW' ? 'Girar CCW' : b.label,
                      })
                    }
                    className="afr2-spin-axis-btn rounded-[var(--radius-sm)] p-2.5 flex items-center gap-2 text-left"
                  >
                    <ChassisSpinGraphic direction="ccw" active={b.pivotDirection === 'ccw'} size="sm" />
                    <span className="font-semibold text-xs">Girar CCW</span>
                  </button>
                  <button
                    type="button"
                    disabled={isLocked}
                    aria-pressed={b.pivotDirection === 'cw'}
                    onClick={() =>
                      updateField({
                        pivotDirection: 'cw',
                        label: b.label === 'Girar CCW' ? 'Girar CW' : b.label,
                      })
                    }
                    className="afr2-spin-axis-btn rounded-[var(--radius-sm)] p-2.5 flex items-center gap-2 text-left"
                  >
                    <ChassisSpinGraphic direction="cw" active={b.pivotDirection === 'cw'} size="sm" />
                    <span className="font-semibold text-xs">Girar CW</span>
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
              <SegmentedControl
                ariaLabel="Lado de curva"
                fill="always"
                value={b.arcSide}
                onChange={(arcSide) => updateField({ arcSide })}
                items={[
                  { value: 'left', label: 'Curva izquierda (+wz)', icon: 'rotate-ccw', disabled: isLocked },
                  { value: 'right', label: 'Curva derecha (-wz)', icon: 'rotate-cw', disabled: isLocked },
                ]}
              />
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
        commitDocUpdate(doc => {
          const tracks = { ...doc.wheelTracks, [track]: doc.wheelTracks[track].map(item => item.id === b.id ? { ...item, ...patch } : item) };
          // Match start times, not indexes: independent tracks may have different block counts.
          if (linkDurations && patch.durationMs !== undefined && b.completion !== 'distance') {
            const index = doc.wheelTracks[track].findIndex(item => item.id === b.id);
            const start = doc.wheelTracks[track].slice(0,index).reduce((sum,item) => sum + wheelDurationMs(item),0);
            for (const key of doc.enabledWheelTracks ?? WHEEL_TRACK_KEYS) {
              if (key === track) continue;
              let elapsed = 0;
              tracks[key] = doc.wheelTracks[key].map(item => {
                const matches = elapsed === start && item.kind !== 'finish' && item.completion !== 'distance';
                elapsed += wheelDurationMs(item);
                return matches ? { ...item, durationMs: patch.durationMs! } : item;
              });
            }
          }
          return { ...doc, wheelTracks: tracks };
        });
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
              Usá <strong>«Editar rueda»</strong> en la cabecera de la pista para ajustar este programa, o <strong>«Crear copia avanzada»</strong> para trabajar sobre una copia.
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

  const blockPalette = <section className="afr2-sequence-palette" aria-label="Añadir movimientos y bloques">
          {/* Paleta para agregar bloques */}
          {(
            <div className="space-y-2">
              <span className="block text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
                Añadir bloque de movimiento al auto
              </span>
              <p className="afr2-help">{activeDoc.editorLevel === 'movements' ? 'Cada movimiento actualiza las cuatro ruedas y la ruta de la vista previa.' : 'Se agrega al final común de las cuatro ruedas, con esperas para que empiecen juntas.'}</p>
              <div className="flex flex-wrap gap-2">
                {MOVEMENT_PALETTE.filter(item => item.kind !== 'finish' || !(activeDoc.editorLevel === 'movements' ? activeDoc.movementBlocks : activeDoc.wheelTracks.fl).some(block => block.kind === 'finish')).map((item) => (
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
          )}
          {activeDoc.editorLevel === 'wheels' && (
            <div className="space-y-2.5">
              <div className="flex flex-wrap items-center justify-between gap-2">
                <span className="text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
                  Añadir bloque a pista de rueda:
                </span>
                <SegmentedControl
                  ariaLabel="Pista de rueda destino"
                  size="sm"
                  fill="never"
                  reselect
                  value={activeWheelTrackForAdd}
                  onChange={(tk) => { setActiveWheelTrackForAdd(tk); setSelectedTrack(tk); }}
                  items={enabledTracks.map((tk) => ({ value: tk, label: tk.toUpperCase() }))}
                />
              </div>
              <div className="flex flex-wrap gap-2">
                {enabledTracks.includes(activeWheelTrackForAdd) && WHEEL_PALETTE.filter(item => item.kind !== 'finish' || !activeDoc.wheelTracks[activeWheelTrackForAdd].some(block => block.kind === 'finish')).map((item) => (
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

  </section>;

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

      <div className="afr2-sequence-preview-layout">
      <div className="afr2-sequence-preview-layout__scene">{preview?.(previewFeed)}{blockPalette}{propertiesPanel ?? <p className="afr2-sequence-edit-hint">Elegí un bloque de la línea de tiempo para ajustar su movimiento.</p>}</div>

      {/* Barra de transporte: Simulación (principal, offline) y Ejecución física */}
      <div className="afr2-sequence-transport p-5 space-y-4">
        <div className="flex flex-wrap items-start justify-between gap-4">
          <div>
            <span className="text-xs font-mono uppercase tracking-wider text-[var(--text-muted)]">
              Simulación y ejecución
            </span>
            <h3 className="text-base font-semibold text-[var(--text)] mt-0.5">
              Probá los cambios de tus pistas
            </h3>
          </div>

          <div className="flex flex-wrap items-center gap-2 text-xs font-mono">
            <span className="px-2.5 py-1 rounded-md border border-[var(--border-default)] bg-[var(--surface-subtle)] text-[var(--text)]">
              Cursor: {(cursorMs / 1000).toFixed(2)} s / {(totalDurationMs / 1000).toFixed(2)} s
            </span>
            <span className="px-2.5 py-1 rounded-md border border-[var(--border-default)] bg-[var(--surface-subtle)] text-[var(--text)]">
              Bloques de ejecución: {validation.normalized.segments.length}/{controllerSequenceLimits.maxSegments}
            </span>
          </div>
        </div>

        {/* Controles del simulador */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">
          <div className="rounded-[var(--radius-sm)] border border-[var(--border-default)] bg-[var(--surface-subtle)] p-4 space-y-3">
            <div className="flex items-center justify-between gap-2">
              <strong className="text-xs uppercase tracking-wider text-[var(--text)]">
                Simulación local
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
                  icon={<UiIcon name="play" />}
                >
                  Reproducir simulación
                </Button>
              ) : (
                <Button
                  variant="secondary"
                  size="md"
                  onClick={() => setSimPlaying(false)}
                  icon={<UiIcon name="pause" />}
                >
                  Pausar
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
                icon={<UiIcon name="square" />}
              >
                Detener / Reiniciar
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

          {/* Controles de ejecución física (solo Iniciar y Cancelar) */}
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
                <strong className="text-[var(--text)]">Motivo de bloqueo físico: </strong>
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

      {/* Biblioteca de programas, importación/exportación JSON y perfil cinemático */}
      </div>
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
              icon={<UiIcon name="undo" />}
            >
              Deshacer ({undoStack.length})
            </Button>
            <Button
              variant="secondary"
              size="md"
              disabled={isLocked || redoStack.length === 0}
              onClick={handleRedo}
              icon={<UiIcon name="redo" />}
            >
              Rehacer ({redoStack.length})
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

        {/* Nivel de edición y procedencia */}
        <div className="flex flex-wrap items-center justify-between gap-3 pt-2 border-t border-[var(--border-default)]">
          <div className="flex flex-wrap items-center gap-2">
            <span className="text-xs font-medium text-[var(--text-muted)]">
              Modo de edición:
            </span>
            <span className="afr2-select-wrap"><select className="afr2-glass-field min-h-11 px-3" aria-label="Modo de edición" disabled={isLocked} value={activeDoc.editorLevel} onChange={event => {
              const editorLevel = event.target.value as 'movements' | 'wheels';
              if (editorLevel === 'movements' && activeDoc.editorLevel === 'wheels') { setRestoreChassis(true); return; }
              commitDocUpdate(doc => ({ ...doc, editorLevel, enabledWheelTracks: [...WHEEL_TRACK_KEYS], wheelDraftReady: true, wheelTracks: deriveWheelTracksFromMovements(doc.movementBlocks, doc.kinematicProfile) }));
              setSelection(null); setSelectedTrack(null); setCursorMs(0); setTimelineScroll(0);
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

        {/* Perfil cinemático configurable (etiquetado explícitamente como no calibrado por defecto) */}
        <div className="grid grid-cols-1 md:grid-cols-3 gap-3 pt-2 border-t border-[var(--border-default)] text-xs">
          <div>
            <label className="block font-medium text-[var(--text-muted)] mb-1">
              Geometría `lx + ly` (mm · inicial 300 mm no calibrado)
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
              Velocidad máx. rueda (mm/s · inicial 800 mm/s no calibrado)
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
                Perfil físico verificado/calibrado (requerido para habilitar ejecución física)
              </span>
            </label>
          </div>
        </div>
      </div>

      {/* Avisos de validación sin recorte silencioso */}
      {validation.issues.length > 0 && (
        <div className="space-y-2">
          {validation.issues.map((issue) => (
            <StatusNotice
              key={issue.id}
              tone={issue.severity === 'error' ? 'error' : 'warning'}
              title={
                issue.severity === 'error'
                  ? 'Error de validación en secuencia'
                  : 'Advertencia de límite físico'
              }
            >
              {issue.message}
            </StatusNotice>
          ))}
        </div>
      )}

      {/* Cuerpo principal: Paleta + Líneas de tiempo + Panel lateral de propiedades (escritorio) */}
      <div className="afr2-sequence-editor-body">
        <div className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4">
          {/* Pista de movimientos (cuando el documento está en Nivel 1) */}
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
              readOnly={isLocked}
              onSelect={id => selectBlock('movements', id)}
              scrollLeft={timelineScroll} onTimelineScroll={setTimelineScroll}
              onSeekCursor={isLocked ? undefined : setCursorMs}
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
            <div className="afr2-sequence-sync-tools">
              <p className="afr2-help">Tocá un bloque para editarlo y ver su instante en el mapa. El desplazamiento horizontal se comparte entre pistas.</p>
              {activeDoc.editorLevel === 'wheels' && <>
                <label><input type="checkbox" disabled={isLocked} checked={linkDurations} onChange={event => setLinkDurations(event.target.checked)} /> Vincular duración de bloques que empiezan juntos</label>
                <Button size="sm" disabled={isLocked || enabledTracks.length < 2} onClick={alignTrackEnds}>Igualar finales con esperas</Button>
                <p className="afr2-help">Las distancias Hall conservan su criterio de finalización. El tiempo del simulador es una estimación.</p>
              </>}
            </div>
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
                headerAction={activeDoc.editorLevel === 'wheels' ? <button type="button" className="afr2-track-delete" disabled={isLocked} aria-label={`Eliminar pista ${tk.toUpperCase()}`} onClick={event => { event.stopPropagation(); deletedTrack.current = false; setPendingTrackDelete({ program: activeDoc.id, track: tk }); }}><UiIcon name="trash" /></button> : <Button size="sm" disabled={isLocked} onClick={() => {
                  commitDocUpdate(doc => ({ ...doc, editorLevel: 'wheels', enabledWheelTracks: [...WHEEL_TRACK_KEYS], wheelDraftReady: true, wheelTracks: deriveWheelTracksFromMovements(doc.movementBlocks, doc.kinematicProfile) }));
                  setSelection(null); setSelectedTrack(tk); setActiveWheelTrackForAdd(tk);
                }}>Editar rueda</Button>}
                blocks={wheelVisualTracks[tk]}
                scaleTotalMs={totalDurationMs}
                cursorMs={cursorMs}
                selectedId={selection?.scope === tk ? selection.blockId : null}
                readOnly={activeDoc.editorLevel !== 'wheels' || isLocked}
                onSelect={id => selectBlock(tk, id)}
                scrollLeft={timelineScroll} onTimelineScroll={setTimelineScroll}
                onSeekCursor={isLocked ? undefined : setCursorMs}
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

      <FloatingModal open={restoreChassis} title="Volver a movimientos del chasis" description="Se recuperará el borrador del chasis y dejarán de usarse los ajustes de ruedas independientes. Podés deshacer este cambio. Para seguir trabajando sobre las ruedas, elegí Conservar pistas." onClose={() => setRestoreChassis(false)}>
        <div className="afr2-action-row"><Button onClick={() => setRestoreChassis(false)}>Conservar pistas</Button><Button variant="primary" disabled={isLocked} onClick={() => {
          commitDocUpdate(doc => ({ ...doc, editorLevel: 'movements', wheelDraftReady: false }));
          setSelection(null); setSelectedTrack(null); setCursorMs(0); setTimelineScroll(0); setRestoreChassis(false);
        }}>Recuperar borrador del chasis</Button></div>
      </FloatingModal>

      <FloatingModal open={pendingTrackDelete !== null} title="Eliminar pista" description={`Se eliminarán los bloques de ${pendingTrackDelete?.track.toUpperCase() ?? 'la pista'}. Podés deshacer esta acción.`} onClose={() => { setPendingTrackDelete(null); if (deletedTrack.current) { deletedTrack.current = false; trackCountRef.current?.focus(); } }}>
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
