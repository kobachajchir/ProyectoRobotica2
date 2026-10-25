import { normalizeSequenceDocument } from './mecanumKinematics';
import { wheelDurationMs } from './blockCompletion';
import { WHEEL_TRACK_KEYS, type Afr2SequenceDocument, type NormalizedSequenceSegment, type WheelTrackKey } from './sequenceTypes';
export interface PhysicalRecord extends NormalizedSequenceSegment {
  wheel?: WheelTrackKey; completion?: 'time' | 'distance'; limit?: number;
  timeoutMs?: number; rampMs?: number; fromMmS?: number; toMmS?: number;
}
export function compilePhysicalProgram(doc: Afr2SequenceDocument): {
  mode: 'timed' | 'independent-wheels' | 'barrier-wheels'; segments: PhysicalRecord[]; totalDurationMs: number; error: string | null;
} {
  const normalized = normalizeSequenceDocument(doc), tracks = normalized.effectiveTracks;
  const distance = WHEEL_TRACK_KEYS.some(key => tracks[key].some(block => block.completion === 'distance'));
  if (!distance) return { mode: 'timed', segments: normalized.segments, totalDurationMs: normalized.totalDurationMs, error: null };
  const segments: PhysicalRecord[] = [], sums = [0, 0, 0, 0], phases: number[] = [];
  for (const [wheel, key] of WHEEL_TRACK_KEYS.entries()) {
    let phase = 0;
    for (const block of tracks[key]) {
      if (block.kind === 'finish') break;
      const rampMs = wheelDurationMs(block), isDistance = block.completion === 'distance';
      const timeoutMs = isDistance ? rampMs * 3 + 1000 : rampMs;
      const from = block.kind === 'ramp' ? block.fromSpeedMmS : block.kind === 'wait' ? 0 : block.speedMmS;
      const to = block.kind === 'ramp' ? block.toSpeedMmS : from;
      const startWheelMmS = { fl: 0, fr: 0, rl: 0, rr: 0, [key]: from };
      const endWheelMmS = { ...startWheelMmS, [key]: to };
      segments.push({ index: segments.length, startTimeMs: sums[wheel], endTimeMs: sums[wheel] + timeoutMs, durationMs: timeoutMs, startWheelMmS, endWheelMmS, sourceLabel: block.label,
        wheel: key, completion: isDistance ? 'distance' : 'time', limit: isDistance ? block.distanceMm : rampMs, timeoutMs, rampMs, fromMmS: from, toMmS: to, movementKind: 'wheels' });
      sums[wheel] += timeoutMs; phases[phase] = Math.max(phases[phase] ?? 0, timeoutMs); phase++;
    }
  }
  const totalDurationMs = (doc.editorLevel === 'movements' ? phases.reduce((sum, value) => sum + value, 0) : Math.max(...sums)) + 1000;
  return { mode: doc.editorLevel === 'movements' ? 'barrier-wheels' : 'independent-wheels', segments, totalDurationMs,
    error: !segments.length || segments.length > 32 || totalDurationMs > 120000 ? 'El modo por pulsos admite 32 instrucciones de rueda y 120 s de tiempo máximo de seguridad. Dividí el programa.' : null };
}
