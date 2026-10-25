import type { WheelBlock } from './sequenceTypes';
export function meanAbsoluteSpeed(from: number, to: number): number {
  if (from * to >= 0) return (Math.abs(from) + Math.abs(to)) / 2;
  return (from * from + to * to) / (2 * (Math.abs(from) + Math.abs(to)));
}
export function distanceDurationMs(distanceMm: number, meanSpeed: number): number {
  if (!Number.isFinite(distanceMm) || distanceMm <= 0 || meanSpeed <= 0) return 0;
  return Math.ceil(distanceMm / meanSpeed * 100) * 10;
}
export function wheelDurationMs(block: WheelBlock): number {
  if (block.kind === 'finish') return 0;
  if (block.completion !== 'distance') return block.durationMs;
  return distanceDurationMs(block.distanceMm ?? 0, block.kind === 'ramp' ? meanAbsoluteSpeed(block.fromSpeedMmS, block.toSpeedMmS) : block.kind === 'constant' ? Math.abs(block.speedMmS) : 0);
}
