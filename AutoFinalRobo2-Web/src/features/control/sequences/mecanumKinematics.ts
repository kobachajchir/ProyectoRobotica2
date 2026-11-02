import { MOTION_REFERENCE } from '../motionReference';
import type { PoseSample } from '../controlMapTypes';
import { ROBOT_WHEEL_KEYS, wheelAnglesFromTravel, type RobotWheelValues } from '../../../components/three/robotWheelMotion';
import { distanceDurationMs, meanAbsoluteSpeed, wheelDurationMs } from './blockCompletion';
import {
  AFR2_SEQUENCE_SCHEMA,
  DEFAULT_KINEMATIC_PROFILE,
  DURATION_STEP_MS,
  EDITOR_MAX_BLOCKS_PER_TRACK,
  EDITOR_MAX_DURATION_MS,
  PHYSICAL_MAX_DURATION_MS,
  PHYSICAL_MAX_SEGMENTS,
  WHEEL_TRACK_KEYS,
  WHEEL_TRACK_LABELS,
  type Afr2SequenceDocument,
  type MecanumKinematicProfile,
  type MovementBlock,
  type MovementBlockKind,
  type NormalizedSequenceSegment,
  type SequenceValidationIssue,
  type WheelBlock,
  type WheelBlockKind,
  type WheelSpeedVectorMmS,
  type WheelTrackKey,
} from './sequenceTypes';

export function createSequenceUid(prefix = 'seq'): string {
  return `${prefix}-${Date.now().toString(36)}-${Math.random().toString(36).slice(2, 7)}`;
}

export function snapDurationMs(value: number, isFinish = false): number {
  if (isFinish) return 0;
  if (!Number.isFinite(value)) return 500;
  const clamped = Math.max(DURATION_STEP_MS, Math.min(EDITOR_MAX_DURATION_MS, Math.round(value)));
  return Math.round(clamped / DURATION_STEP_MS) * DURATION_STEP_MS;
}

/**
 * Cinemática inversa Mecanum sin recortes silenciosos de velocidad:
 * Convierte consignas del chasis (vxMmS, vyMmS, wzMradS) a las 4 ruedas (mm/s).
 * f = forwardWheelSign * vx; r = yawWheelSign * wz * (lx + ly)
 * v_FL = f - vy - r; v_FR = f + vy + r
 * v_RL = f + vy - r; v_RR = f - vy + r
 * Fixed reference matches AVR; wheel tracks keep their own signed basis.
 * Donde wz(rad/s) = wzMradS / 1000.
 */
export function inverseMecanumKinematics(
  vxMmS: number,
  vyMmS: number,
  wzMradS: number,
  lxPlusLyMm: number
): WheelSpeedVectorMmS {
  const forwardWheelMmS = vxMmS * MOTION_REFERENCE.forwardWheelSign;
  const rotTangentialMmS = (wzMradS * lxPlusLyMm) / 1000 * MOTION_REFERENCE.yawWheelSign;
  return {
    fl: Math.round(forwardWheelMmS - vyMmS - rotTangentialMmS),
    fr: Math.round(forwardWheelMmS + vyMmS + rotTangentialMmS),
    rl: Math.round(forwardWheelMmS + vyMmS - rotTangentialMmS),
    rr: Math.round(forwardWheelMmS - vyMmS + rotTangentialMmS),
  };
}

/**
 * Cinemática directa Mecanum determinista desde las 4 ruedas (mm/s) hacia el chasis:
 * vx = forwardWheelSign * (v_FL + v_FR + v_RL + v_RR) / 4
 * vy = (-v_FL + v_FR + v_RL - v_RR) / 4
 * wz(mrad/s) = yawWheelSign * ((-v_FL + v_FR - v_RL + v_RR) / (4 * (lx + ly))) * 1000
 */
export function forwardMecanumKinematics(
  wheels: WheelSpeedVectorMmS,
  lxPlusLyMm: number
): { vxMmS: number; vyMmS: number; wzMradS: number } {
  const safeL = Math.max(50, lxPlusLyMm);
  const vxMmS = MOTION_REFERENCE.forwardWheelSign * (wheels.fl + wheels.fr + wheels.rl + wheels.rr) / 4;
  const vyMmS = (-wheels.fl + wheels.fr + wheels.rl - wheels.rr) / 4;
  const wzMradS = MOTION_REFERENCE.yawWheelSign * ((-wheels.fl + wheels.fr - wheels.rl + wheels.rr) * 1000) / (4 * safeL);
  return { vxMmS, vyMmS, wzMradS };
}

export function createDefaultMovementBlock(kind: MovementBlockKind): MovementBlock {
  const id = createSequenceUid('mov');
  switch (kind) {
    case 'constant':
      return {
        id,
        kind: 'constant',
        label: 'Desplazamiento',
        durationMs: 1000,
        vxMmS: 240,
        vyMmS: 0,
        wzMradS: 0,
        fromVxMmS: 0,
        fromVyMmS: 0,
        toVxMmS: 240,
        toVyMmS: 0,
        pivotDirection: 'ccw',
        arcSide: 'left',
      };
    case 'ramp':
      return {
        id,
        kind: 'ramp',
        label: 'Rampa lineal',
        durationMs: 1000,
        vxMmS: 0,
        vyMmS: 0,
        wzMradS: 0,
        fromVxMmS: 0,
        fromVyMmS: 0,
        toVxMmS: 300,
        toVyMmS: 0,
        pivotDirection: 'ccw',
        arcSide: 'left',
      };
    case 'pivot':
      return {
        id,
        kind: 'pivot',
        label: 'Girar CCW',
        durationMs: 800,
        vxMmS: 0,
        vyMmS: 0,
        wzMradS: 650,
        fromVxMmS: 0,
        fromVyMmS: 0,
        toVxMmS: 0,
        toVyMmS: 0,
        pivotDirection: 'ccw',
        arcSide: 'left',
      };
    case 'curve':
      return {
        id,
        kind: 'curve',
        label: 'Curva suave',
        durationMs: 1200,
        vxMmS: 220,
        vyMmS: 0,
        wzMradS: 400,
        fromVxMmS: 0,
        fromVyMmS: 0,
        toVxMmS: 0,
        toVyMmS: 0,
        pivotDirection: 'ccw',
        arcSide: 'left',
      };
    case 'wait':
      return {
        id,
        kind: 'wait',
        label: 'Espera',
        durationMs: 500,
        vxMmS: 0,
        vyMmS: 0,
        wzMradS: 0,
        fromVxMmS: 0,
        fromVyMmS: 0,
        toVxMmS: 0,
        toVyMmS: 0,
        pivotDirection: 'ccw',
        arcSide: 'left',
      };
    case 'finish':
      return {
        id,
        kind: 'finish',
        label: 'Finalización',
        durationMs: 0,
        vxMmS: 0,
        vyMmS: 0,
        wzMradS: 0,
        fromVxMmS: 0,
        fromVyMmS: 0,
        toVxMmS: 0,
        toVyMmS: 0,
        pivotDirection: 'ccw',
        arcSide: 'left',
      };
  }
}

export function createDefaultWheelBlock(kind: WheelBlockKind): WheelBlock {
  const id = createSequenceUid('whl');
  switch (kind) {
    case 'constant':
      return {
        id,
        kind: 'constant',
        label: 'Velocidad',
        durationMs: 800,
        speedMmS: 240,
        fromSpeedMmS: 0,
        toSpeedMmS: 240,
      };
    case 'ramp':
      return {
        id,
        kind: 'ramp',
        label: 'Rampa rueda',
        durationMs: 800,
        speedMmS: 0,
        fromSpeedMmS: 0,
        toSpeedMmS: 300,
      };
    case 'wait':
      return {
        id,
        kind: 'wait',
        label: 'Espera',
        durationMs: 500,
        speedMmS: 0,
        fromSpeedMmS: 0,
        toSpeedMmS: 0,
      };
    case 'finish':
      return {
        id,
        kind: 'finish',
        label: 'Finalización',
        durationMs: 0,
        speedMmS: 0,
        fromSpeedMmS: 0,
        toSpeedMmS: 0,
      };
  }
}

export function resolveMovementBlockEndpoints(
  block: MovementBlock,
  profile: MecanumKinematicProfile
): { start: WheelSpeedVectorMmS; end: WheelSpeedVectorMmS } {
  const L = profile.lxPlusLyMm;
  switch (block.kind) {
    case 'constant': {
      const w = inverseMecanumKinematics(block.vxMmS, block.vyMmS, 0, L);
      return { start: w, end: w };
    }
    case 'ramp': {
      const start = inverseMecanumKinematics(block.fromVxMmS, block.fromVyMmS, 0, L);
      const end = inverseMecanumKinematics(block.toVxMmS, block.toVyMmS, 0, L);
      return { start, end };
    }
    case 'pivot': {
      const signedWz =
        block.pivotDirection === 'ccw'
          ? Math.abs(block.wzMradS)
          : -Math.abs(block.wzMradS);
      const w = inverseMecanumKinematics(0, 0, signedWz, L);
      return { start: w, end: w };
    }
    case 'curve': {
      const signedWz =
        block.arcSide === 'left'
          ? Math.abs(block.wzMradS)
          : -Math.abs(block.wzMradS);
      const w = inverseMecanumKinematics(block.vxMmS, 0, signedWz, L);
      return { start: w, end: w };
    }
    case 'wait':
    case 'finish':
    default: {
      const zero = { fl: 0, fr: 0, rl: 0, rr: 0 };
      return { start: zero, end: zero };
    }
  }
}

/**
 * Deriva las cuatro pistas de rueda (`FL`, `FR`, `RL`, `RR`) a partir de la secuencia de movimientos del auto.
 * Respeta la semántica terminal de `finish` y no aplica recortes silenciosos de velocidad.
 */
export function deriveWheelTracksFromMovements(
  movementBlocks: MovementBlock[],
  profile: MecanumKinematicProfile
): Record<WheelTrackKey, WheelBlock[]> {
  const result: Record<WheelTrackKey, WheelBlock[]> = {
    fl: [],
    fr: [],
    rl: [],
    rr: [],
  };

  for (const block of movementBlocks) {
    if (block.kind === 'finish') {
      for (const key of WHEEL_TRACK_KEYS) {
        result[key].push({
          id: `${block.id}-${key}`,
          kind: 'finish',
          label: block.label || 'Finalización',
          durationMs: 0,
          speedMmS: 0,
          fromSpeedMmS: 0,
          toSpeedMmS: 0,
        });
      }
      break;
    }

    if (block.kind === 'wait') {
      for (const key of WHEEL_TRACK_KEYS) {
        result[key].push({
          id: `${block.id}-${key}`,
          kind: 'wait',
          label: block.label || 'Espera',
          durationMs: block.durationMs,
          speedMmS: 0,
          fromSpeedMmS: 0,
          toSpeedMmS: 0,
        });
      }
      continue;
    }

    const { start, end } = resolveMovementBlockEndpoints(block, profile);
    const resolvedDuration = movementDurationMs(block, profile);
    const referenceSpeed = Math.max(...WHEEL_TRACK_KEYS.map(key => meanAbsoluteSpeed(start[key], end[key])));
    for (const key of WHEEL_TRACK_KEYS) {
      const s0 = start[key];
      const s1 = end[key];
      const isRamp = block.kind === 'ramp' || s0 !== s1;
      result[key].push({
        id: `${block.id}-${key}`,
        kind: isRamp ? 'ramp' : s0 === 0 ? 'wait' : 'constant',
        label: block.label,
        durationMs: resolvedDuration,
        completion: block.completion === 'distance' && meanAbsoluteSpeed(s0, s1) > 0 ? 'distance' : 'time',
        distanceMm: Math.max(1, Math.round(block.completion === 'distance' && referenceSpeed > 0 ? (block.distanceMm ?? 0) * meanAbsoluteSpeed(s0, s1) / referenceSpeed : meanAbsoluteSpeed(s0, s1) * resolvedDuration / 1000)),
        speedMmS: isRamp ? s1 : s0,
        fromSpeedMmS: s0,
        toSpeedMmS: s1,
      });
    }
  }

  return result;
}

export function movementDurationMs(block: MovementBlock, profile: MecanumKinematicProfile): number {
  if (block.kind === 'finish') return 0;
  if (block.completion !== 'distance') return block.durationMs;
  const { start, end } = resolveMovementBlockEndpoints(block, profile);
  const speed = Math.max(...WHEEL_TRACK_KEYS.map(key => meanAbsoluteSpeed(start[key], end[key])));
  return distanceDurationMs(block.distanceMm ?? 0, speed);
}

/**
 * Crea una copia editable de 4 pistas («Crear copia avanzada») a partir de un programa de movimientos,
 * conservando intacto el documento original y registrando `provenance`.
 */
export function createAdvancedWheelCopy(source: Afr2SequenceDocument): Afr2SequenceDocument {
  const derived = deriveWheelTracksFromMovements(source.movementBlocks, source.kinematicProfile);
  const nowIso = new Date().toISOString();
  const clonedTracks: Record<WheelTrackKey, WheelBlock[]> = {
    fl: derived.fl.map((b) => ({ ...b, id: createSequenceUid('whl-fl') })),
    fr: derived.fr.map((b) => ({ ...b, id: createSequenceUid('whl-fr') })),
    rl: derived.rl.map((b) => ({ ...b, id: createSequenceUid('whl-rl') })),
    rr: derived.rr.map((b) => ({ ...b, id: createSequenceUid('whl-rr') })),
  };

  return {
    schema: AFR2_SEQUENCE_SCHEMA,
    id: createSequenceUid('prog'),
    name: `${source.name} (Copia 4 ruedas)`,
    updatedAt: nowIso,
    editorLevel: 'wheels',
    units: { distance: 'mm', time: 'ms', angle: 'mrad' },
    kinematicProfile: { ...source.kinematicProfile },
    provenance: {
      convertedFromId: source.id,
      convertedFromName: source.name,
      convertedAt: nowIso,
    },
    movementBlocks: source.movementBlocks.map((b) => ({ ...b })),
    wheelTracks: clonedTracks,
  };
}

interface TrackInterval {
  startMs: number;
  endMs: number;
  fromMmS: number;
  toMmS: number;
  label: string;
}

function buildWheelTrackIntervals(blocks: WheelBlock[]): {
  intervals: TrackInterval[];
  totalDurationMs: number;
} {
  const intervals: TrackInterval[] = [];
  let cursorMs = 0;

  for (const b of blocks) {
    if (b.kind === 'finish') {
      break;
    }
    const dur = Math.max(0, wheelDurationMs(b));
    if (dur <= 0) continue;
    const fromMmS =
      b.kind === 'ramp' ? b.fromSpeedMmS : b.kind === 'wait' ? 0 : b.speedMmS;
    const toMmS =
      b.kind === 'ramp' ? b.toSpeedMmS : b.kind === 'wait' ? 0 : b.speedMmS;

    intervals.push({
      startMs: cursorMs,
      endMs: cursorMs + dur,
      fromMmS,
      toMmS,
      label: b.label,
    });
    cursorMs += dur;
  }

  return { intervals, totalDurationMs: cursorMs };
}

function sampleTrackAt(intervals: TrackInterval[], tMs: number, boundary: 'start' | 'end'): number {
  for (const iv of intervals) {
    if (boundary === 'start') {
      if (tMs >= iv.startMs && tMs < iv.endMs) {
        const span = iv.endMs - iv.startMs;
        if (span <= 0) return iv.toMmS;
        const alpha = (tMs - iv.startMs) / span;
        return Math.round(iv.fromMmS + (iv.toMmS - iv.fromMmS) * alpha);
      }
    } else {
      if (tMs > iv.startMs && tMs <= iv.endMs) {
        const span = iv.endMs - iv.startMs;
        if (span <= 0) return iv.toMmS;
        const alpha = (tMs - iv.startMs) / span;
        return Math.round(iv.fromMmS + (iv.toMmS - iv.fromMmS) * alpha);
      }
    }
  }
  // Una rueda cuya pista termine antes permanece con consigna cero.
  return 0;
}

/**
 * Normaliza un documento (`movements` o `wheels`) en una lista de segmentos temporales sincronizados
 * para las cuatro ruedas (`FL`, `FR`, `RL`, `RR`).
 */
export function normalizeSequenceDocument(doc: Afr2SequenceDocument): {
  segments: NormalizedSequenceSegment[];
  totalDurationMs: number;
  effectiveTracks: Record<WheelTrackKey, WheelBlock[]>;
} {
  const effectiveTracks =
    doc.editorLevel === 'movements'
      ? deriveWheelTracksFromMovements(doc.movementBlocks, doc.kinematicProfile)
      : Object.fromEntries(WHEEL_TRACK_KEYS.map(key => [key, (doc.enabledWheelTracks ?? WHEEL_TRACK_KEYS).includes(key) ? doc.wheelTracks[key] : []])) as Record<WheelTrackKey, WheelBlock[]>;

  if (doc.editorLevel === 'movements') {
    const segments: NormalizedSequenceSegment[] = [];
    let cursorMs = 0;
    for (const block of doc.movementBlocks) {
      if (block.kind === 'finish') break;
      const dur = Math.max(0, movementDurationMs(block, doc.kinematicProfile));
      if (dur <= 0) continue;
      const { start, end } = resolveMovementBlockEndpoints(block, doc.kinematicProfile);
      segments.push({
        index: segments.length,
        startTimeMs: cursorMs,
        endTimeMs: cursorMs + dur,
        durationMs: dur,
        startWheelMmS: start,
        endWheelMmS: end,
        sourceLabel: block.label,
        movementKind: block.kind,
      });
      cursorMs += dur;
    }
    return { segments, totalDurationMs: cursorMs, effectiveTracks };
  }

  // Nivel 'wheels': partición temporal sincronizada entre las 4 pistas FL/FR/RL/RR
  const perTrack = {
    fl: buildWheelTrackIntervals(effectiveTracks.fl),
    fr: buildWheelTrackIntervals(effectiveTracks.fr),
    rl: buildWheelTrackIntervals(effectiveTracks.rl),
    rr: buildWheelTrackIntervals(effectiveTracks.rr),
  };

  const breakpointsSet = new Set<number>([0]);
  for (const key of WHEEL_TRACK_KEYS) {
    for (const iv of perTrack[key].intervals) {
      breakpointsSet.add(iv.startMs);
      breakpointsSet.add(iv.endMs);
    }
  }

  const sortedPoints = Array.from(breakpointsSet).sort((a, b) => a - b);
  const segments: NormalizedSequenceSegment[] = [];

  for (let i = 0; i < sortedPoints.length - 1; i += 1) {
    const t0 = sortedPoints[i]!;
    const t1 = sortedPoints[i + 1]!;
    const dur = t1 - t0;
    if (dur <= 0) continue;

    segments.push({
      index: segments.length,
      startTimeMs: t0,
      endTimeMs: t1,
      durationMs: dur,
      startWheelMmS: {
        fl: sampleTrackAt(perTrack.fl.intervals, t0, 'start'),
        fr: sampleTrackAt(perTrack.fr.intervals, t0, 'start'),
        rl: sampleTrackAt(perTrack.rl.intervals, t0, 'start'),
        rr: sampleTrackAt(perTrack.rr.intervals, t0, 'start'),
      },
      endWheelMmS: {
        fl: sampleTrackAt(perTrack.fl.intervals, t1, 'end'),
        fr: sampleTrackAt(perTrack.fr.intervals, t1, 'end'),
        rl: sampleTrackAt(perTrack.rl.intervals, t1, 'end'),
        rr: sampleTrackAt(perTrack.rr.intervals, t1, 'end'),
      },
      sourceLabel: `Segmento ${segments.length + 1} (${t0}–${t1} ms)`,
    });
  }

  const totalDurationMs =
    segments.length > 0 ? segments[segments.length - 1]!.endTimeMs : 0;

  return { segments, totalDurationMs, effectiveTracks };
}

/**
 * Valida la estructura, múltiplos de 10 ms, bloques terminales, límites de velocidades sin recorte
 * silencioso y límites tanto del editor (64 bloques / 10 min) como físicos (32 segmentos / 120 s).
 */
export function validateSequenceDocument(
  doc: Afr2SequenceDocument,
  physicalLimits: { maxSegments: number; maxDurationMs: number } = {
    maxSegments: PHYSICAL_MAX_SEGMENTS,
    maxDurationMs: PHYSICAL_MAX_DURATION_MS,
  }
): {
  issues: SequenceValidationIssue[];
  editorValid: boolean;
  physicalValid: boolean;
  normalized: ReturnType<typeof normalizeSequenceDocument>;
} {
  const issues: SequenceValidationIssue[] = [];
  const maxWheelSpeed = Math.max(50, doc.kinematicProfile.maxWheelSpeedMmS);

  if (doc.editorLevel === 'movements') {
    if (doc.movementBlocks.length === 0) {
      issues.push({
        id: 'mov-empty',
        severity: 'error',
        track: 'movements',
        message: 'La secuencia de movimientos no tiene bloques.',
      });
    }
    if (doc.movementBlocks.length > EDITOR_MAX_BLOCKS_PER_TRACK) {
      issues.push({
        id: 'mov-max-blocks',
        severity: 'error',
        track: 'movements',
        message: `La pista supera el máximo del editor (${EDITOR_MAX_BLOCKS_PER_TRACK} bloques).`,
      });
    }

    let seenFinish = false;
    for (const block of doc.movementBlocks) {
      if (seenFinish) {
        issues.push({
          id: `mov-after-finish-${block.id}`,
          severity: 'error',
          track: 'movements',
          blockId: block.id,
          message: `El bloque «${block.label}» está después de Finalización (que es terminal).`,
        });
      }
      if (block.kind === 'finish') {
        seenFinish = true;
        if (block.durationMs !== 0) {
          issues.push({
            id: `mov-finish-dur-${block.id}`,
            severity: 'error',
            track: 'movements',
            blockId: block.id,
            message: 'El bloque de Finalización no debe consumir duración (0 ms).',
          });
        }
        continue;
      }

      if (block.durationMs <= 0 || block.durationMs % DURATION_STEP_MS !== 0) {
        issues.push({
          id: `mov-step-${block.id}`,
          severity: 'error',
          track: 'movements',
          blockId: block.id,
          message: `La duración de «${block.label}» (${block.durationMs} ms) debe ser múltiplo positivo de ${DURATION_STEP_MS} ms.`,
        });
      }

      const { start, end } = resolveMovementBlockEndpoints(block, doc.kinematicProfile);
      const peak = Math.max(
        Math.abs(start.fl),
        Math.abs(start.fr),
        Math.abs(start.rl),
        Math.abs(start.rr),
        Math.abs(end.fl),
        Math.abs(end.fr),
        Math.abs(end.rl),
        Math.abs(end.rr)
      );
      if (peak > maxWheelSpeed) {
        issues.push({
          id: `mov-overspeed-${block.id}`,
          severity: 'error',
          track: 'movements',
          blockId: block.id,
          message: `«${block.label}» demanda ${peak} mm/s en una rueda, superando el máximo configurado (${maxWheelSpeed} mm/s). No se recorta silenciosamente; reducí la consigna o ajustá el perfil.`,
        });
      }
    }
  } else {
    const totalBlocksAcrossTracks = [...(doc.enabledWheelTracks ?? WHEEL_TRACK_KEYS)].reduce<number>(
      (acc, k) => acc + doc.wheelTracks[k].length,
      0
    );
    if (totalBlocksAcrossTracks === 0) {
      issues.push({
        id: 'wheels-empty',
        severity: 'error',
        track: 'global',
        message: 'No hay bloques en las pistas activas.',
      });
    }

    for (const key of doc.enabledWheelTracks ?? WHEEL_TRACK_KEYS) {
      const list = doc.wheelTracks[key];
      if (list.length > EDITOR_MAX_BLOCKS_PER_TRACK) {
        issues.push({
          id: `whl-max-blocks-${key}`,
          severity: 'error',
          track: key,
          message: `${WHEEL_TRACK_LABELS[key]} supera el máximo del editor (${EDITOR_MAX_BLOCKS_PER_TRACK} bloques).`,
        });
      }
      let seenFinish = false;
      for (const block of list) {
        if (seenFinish) {
          issues.push({
            id: `whl-after-finish-${block.id}`,
            severity: 'error',
            track: key,
            blockId: block.id,
            message: `En ${key.toUpperCase()}, «${block.label}» está ubicado tras un bloque terminal de Finalización.`,
          });
        }
        if (block.kind === 'finish') {
          seenFinish = true;
          if (block.durationMs !== 0) {
            issues.push({
              id: `whl-finish-dur-${block.id}`,
              severity: 'error',
              track: key,
              blockId: block.id,
              message: `En ${key.toUpperCase()}, la Finalización debe tener duración 0 ms.`,
            });
          }
          continue;
        }
        if (block.durationMs <= 0 || block.durationMs % DURATION_STEP_MS !== 0) {
          issues.push({
            id: `whl-step-${block.id}`,
            severity: 'error',
            track: key,
            blockId: block.id,
            message: `En ${key.toUpperCase()}, «${block.label}» (${block.durationMs} ms) debe ser múltiplo positivo de ${DURATION_STEP_MS} ms.`,
          });
        }
        const peak =
          block.kind === 'ramp'
            ? Math.max(Math.abs(block.fromSpeedMmS), Math.abs(block.toSpeedMmS))
            : block.kind === 'constant'
              ? Math.abs(block.speedMmS)
              : 0;
        if (peak > maxWheelSpeed) {
          issues.push({
            id: `whl-overspeed-${block.id}`,
            severity: 'error',
            track: key,
            blockId: block.id,
            message: `En ${key.toUpperCase()}, «${block.label}» solicita ${peak} mm/s (máximo nominal ${maxWheelSpeed} mm/s).`,
          });
        }
      }
    }
  }

  const normalized = normalizeSequenceDocument(doc);
  const checkedBlocks = doc.editorLevel === 'movements' ? doc.movementBlocks : (doc.enabledWheelTracks ?? WHEEL_TRACK_KEYS).flatMap(key => doc.wheelTracks[key]);
  for (const block of checkedBlocks) if (block.completion === 'distance') {
    const duration = doc.editorLevel === 'movements' ? movementDurationMs(block as MovementBlock, doc.kinematicProfile) : wheelDurationMs(block as WheelBlock);
    if (!Number.isInteger(block.distanceMm) || (block.distanceMm ?? 0) < 1 || (block.distanceMm ?? 0) > 10000 || duration <= 0 || duration > EDITOR_MAX_DURATION_MS || block.kind === 'wait' || block.kind === 'finish')
      issues.push({ id: `distance-${block.id}`, severity: 'error', blockId: block.id, message: 'Distancia inválida, sin velocidad o duración simulada fuera de límites.' });
  }

  if (normalized.totalDurationMs <= 0) {
    issues.push({
      id: 'total-zero',
      severity: 'error',
      track: 'global',
      message: 'La duración efectiva del programa es 0 ms. Agregá al menos un bloque con duración positiva.',
    });
  }

  if (normalized.totalDurationMs > EDITOR_MAX_DURATION_MS) {
    issues.push({
      id: 'editor-max-duration',
      severity: 'error',
      track: 'global',
      message: `La duración total (${(normalized.totalDurationMs / 1000).toFixed(1)} s) supera el límite del editor (600 s / 10 min).`,
    });
  }

  const editorErrors = issues.filter((i) => i.severity === 'error');

  let physicalValid = editorErrors.length === 0;
  if (normalized.segments.length > physicalLimits.maxSegments) {
    physicalValid = false;
    issues.push({
      id: 'phys-max-segments',
      severity: 'warning',
      track: 'physical',
      message: `El programa genera ${normalized.segments.length} segmentos normalizados y excede el límite físico del controlador (${physicalLimits.maxSegments} segmentos). Podés simularlo localmente, pero el controlador rechazará su carga física sin truncarlo.`,
    });
  }

  if (normalized.totalDurationMs > physicalLimits.maxDurationMs) {
    physicalValid = false;
    issues.push({
      id: 'phys-max-duration',
      severity: 'warning',
      track: 'physical',
      message: `La duración total (${(normalized.totalDurationMs / 1000).toFixed(1)} s) excede el límite físico del controlador (${(physicalLimits.maxDurationMs / 1000).toFixed(0)} s).`,
    });
  }

  return {
    issues,
    editorValid: editorErrors.length === 0,
    physicalValid,
    normalized,
  };
}

export interface SimulationSnapshotAtCursor {
  cursorMs: number;
  totalDurationMs: number;
  xMm: number;
  yMm: number;
  thetaMrad: number;
  wheelSpeedsMmS: WheelSpeedVectorMmS;
  wheelAngles: RobotWheelValues;
  chassisVelocity: {
    vxMmS: number;
    vyMmS: number;
    wzMradS: number;
  };
  activeSegmentIndex: number | null;
  trail: PoseSample[];
}

function wrapAngleRad(rad: number): number {
  let a = rad;
  while (a > Math.PI) a -= 2 * Math.PI;
  while (a < -Math.PI) a += 2 * Math.PI;
  return a;
}

/**
 * Motor determinista de simulación cinemática Mecanum en pasos fijos de 10 ms.
 * - Al evaluar cualquier `targetTimeMs`, produce exactamente el mismo estado e historial espacial.
 * - Al finalizar la secuencia (`targetTimeMs >= totalDurationMs`), las consignas de salida quedan en 0 mm/s.
 */
export function evaluateSequenceSimulationAtTime(
  doc: Afr2SequenceDocument,
  targetTimeMs: number
): SimulationSnapshotAtCursor {
  const { segments, totalDurationMs } = normalizeSequenceDocument(doc);
  const clampedCursorMs = Math.max(0, Math.min(totalDurationMs, Math.round(targetTimeMs / DURATION_STEP_MS) * DURATION_STEP_MS));

  let xMm = 0;
  let yMm = 0;
  let thetaRad = 0;
  const wheelTravelMm: RobotWheelValues = { fl: 0, fr: 0, rl: 0, rr: 0 };

  const trail: PoseSample[] = [
    {
      schema: 'afr2-pose-1',
      frameId: 'sim-mecanum',
      sequence: 0,
      xMm: 0,
      yMm: 0,
      thetaMrad: 0,
      ageMs: 0,
      segmentStart: true,
    },
  ];

  let segIdx = 0;
  const totalSteps = Math.floor(clampedCursorMs / DURATION_STEP_MS);
  const dtSec = DURATION_STEP_MS / 1000;

  let currentWheels: WheelSpeedVectorMmS = { fl: 0, fr: 0, rl: 0, rr: 0 };
  let activeSegmentIndex: number | null = null;

  for (let step = 0; step < totalSteps; step += 1) {
    const tMs = step * DURATION_STEP_MS;
    while (segIdx < segments.length && tMs >= segments[segIdx]!.endTimeMs) {
      segIdx += 1;
    }
    const seg = segments[segIdx];
    if (!seg) {
      currentWheels = { fl: 0, fr: 0, rl: 0, rr: 0 };
      break;
    }
    activeSegmentIndex = seg.index;
    const midAlpha = Math.max(
      0,
      Math.min(1, (tMs + DURATION_STEP_MS * 0.5 - seg.startTimeMs) / Math.max(DURATION_STEP_MS, seg.durationMs))
    );
    currentWheels = {
      fl: Math.round(seg.startWheelMmS.fl + (seg.endWheelMmS.fl - seg.startWheelMmS.fl) * midAlpha),
      fr: Math.round(seg.startWheelMmS.fr + (seg.endWheelMmS.fr - seg.startWheelMmS.fr) * midAlpha),
      rl: Math.round(seg.startWheelMmS.rl + (seg.endWheelMmS.rl - seg.startWheelMmS.rl) * midAlpha),
      rr: Math.round(seg.startWheelMmS.rr + (seg.endWheelMmS.rr - seg.startWheelMmS.rr) * midAlpha),
    };
    for (const key of ROBOT_WHEEL_KEYS) wheelTravelMm[key] += currentWheels[key] * dtSec;

    const { vxMmS, vyMmS, wzMradS } = forwardMecanumKinematics(
      currentWheels,
      doc.kinematicProfile.lxPlusLyMm
    );
    const wzRadS = wzMradS / 1000;
    const midTheta = thetaRad + wzRadS * dtSec * 0.5;
    const cosT = Math.cos(midTheta);
    const sinT = Math.sin(midTheta);

    xMm += (vxMmS * cosT - vyMmS * sinT) * dtSec;
    yMm += (vxMmS * sinT + vyMmS * cosT) * dtSec;
    thetaRad = wrapAngleRad(thetaRad + wzRadS * dtSec);

    // Muestrear hasta 256 puntos para el trazo del mapa
    const sampleStride = Math.max(1, Math.ceil(totalSteps / 240));
    if ((step + 1) % sampleStride === 0 || step === totalSteps - 1) {
      trail.push({
        schema: 'afr2-pose-1',
        frameId: 'sim-mecanum',
        sequence: trail.length,
        xMm: Math.round(xMm),
        yMm: Math.round(yMm),
        thetaMrad: Math.round(thetaRad * 1000),
        ageMs: 0,
        segmentStart: false,
      });
    }
  }

  // Si el cursor está al final del programa o detenido en 0, las salidas solicitadas son 0
  if (clampedCursorMs >= totalDurationMs || clampedCursorMs === 0) {
    currentWheels = { fl: 0, fr: 0, rl: 0, rr: 0 };
    if (clampedCursorMs >= totalDurationMs) {
      activeSegmentIndex = null;
    }
  }

  const chassisVelocity = forwardMecanumKinematics(
    currentWheels,
    doc.kinematicProfile.lxPlusLyMm
  );

  return {
    cursorMs: clampedCursorMs,
    totalDurationMs,
    xMm: Math.round(xMm),
    yMm: Math.round(yMm),
    thetaMrad: Math.round(thetaRad * 1000),
    wheelSpeedsMmS: currentWheels,
    wheelAngles: wheelAnglesFromTravel(wheelTravelMm),
    chassisVelocity: {
      vxMmS: Math.round(chassisVelocity.vxMmS),
      vyMmS: Math.round(chassisVelocity.vyMmS),
      wzMradS: Math.round(chassisVelocity.wzMradS),
    },
    activeSegmentIndex,
    trail,
  };
}

export function createStarterSequenceDocument(
  name = 'Recorrido Mecanum de demostración'
): Afr2SequenceDocument {
  const movBlocks: MovementBlock[] = [
    {
      id: 'demo-mov-1',
      kind: 'ramp',
      label: 'Aceleración frontal',
      durationMs: 800,
      vxMmS: 0,
      vyMmS: 0,
      wzMradS: 0,
      fromVxMmS: 0,
      fromVyMmS: 0,
      toVxMmS: 280,
      toVyMmS: 0,
      pivotDirection: 'ccw',
      arcSide: 'left',
    },
    {
      id: 'demo-mov-2',
      kind: 'constant',
      label: 'Avance constante',
      durationMs: 1200,
      vxMmS: 280,
      vyMmS: 0,
      wzMradS: 0,
      fromVxMmS: 0,
      fromVyMmS: 0,
      toVxMmS: 280,
      toVyMmS: 0,
      pivotDirection: 'ccw',
      arcSide: 'left',
    },
    {
      id: 'demo-mov-3',
      kind: 'constant',
      label: 'Desplazamiento lateral izq.',
      durationMs: 1000,
      vxMmS: 0,
      vyMmS: 220,
      wzMradS: 0,
      fromVxMmS: 0,
      fromVyMmS: 0,
      toVxMmS: 0,
      toVyMmS: 220,
      pivotDirection: 'ccw',
      arcSide: 'left',
    },
    {
      id: 'demo-mov-4',
      kind: 'pivot',
      label: 'Girar CCW 90°',
      durationMs: 1000,
      vxMmS: 0,
      vyMmS: 0,
      wzMradS: 750,
      fromVxMmS: 0,
      fromVyMmS: 0,
      toVxMmS: 0,
      toVyMmS: 0,
      pivotDirection: 'ccw',
      arcSide: 'left',
    },
    {
      id: 'demo-mov-5',
      kind: 'curve',
      label: 'Curva derecha',
      durationMs: 1200,
      vxMmS: 240,
      vyMmS: 0,
      wzMradS: 450,
      fromVxMmS: 0,
      fromVyMmS: 0,
      toVxMmS: 0,
      toVyMmS: 0,
      pivotDirection: 'cw',
      arcSide: 'right',
    },
    {
      id: 'demo-mov-6',
      kind: 'wait',
      label: 'Espera en reposo',
      durationMs: 400,
      vxMmS: 0,
      vyMmS: 0,
      wzMradS: 0,
      fromVxMmS: 0,
      fromVyMmS: 0,
      toVxMmS: 0,
      toVyMmS: 0,
      pivotDirection: 'ccw',
      arcSide: 'left',
    },
    {
      id: 'demo-mov-7',
      kind: 'finish',
      label: 'Finalización',
      durationMs: 0,
      vxMmS: 0,
      vyMmS: 0,
      wzMradS: 0,
      fromVxMmS: 0,
      fromVyMmS: 0,
      toVxMmS: 0,
      toVyMmS: 0,
      pivotDirection: 'ccw',
      arcSide: 'left',
    },
  ];

  const profile = { ...DEFAULT_KINEMATIC_PROFILE };
  const derived = deriveWheelTracksFromMovements(movBlocks, profile);

  return {
    schema: AFR2_SEQUENCE_SCHEMA,
    id: 'demo-mecanum-sequence',
    name,
    updatedAt: new Date().toISOString(),
    editorLevel: 'movements',
    units: { distance: 'mm', time: 'ms', angle: 'mrad' },
    kinematicProfile: profile,
    provenance: null,
    movementBlocks: movBlocks,
    wheelTracks: derived,
  };
}
