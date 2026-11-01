/** AFR2-20261029-26. Measured ADC voltage; never infer charge percentage. */
export interface BatteryReading {
  bootId: number;
  sampleId: number;
  state: 'off' | 'measuring' | 'valid' | 'out-of-range' | 'stale';
  valid: boolean;
  ageMs: number | null;
  raw: number | null;
  millivolts: number | null;
  referenceMv: number;
  dividerNumerator: number;
  dividerDenominator: number;
  calibrated: false;
  analogPin: 'A5';
  receivedAtMs: number;
}
const uint = (n: unknown, max = 0xffffffff): n is number => typeof n === 'number' && Number.isInteger(n) && n >= 0 && n <= max;
export function parseBattery(value: unknown): BatteryReading | null {
  if (!value || typeof value !== 'object') return null;
  const p = value as Record<string, unknown>;
  if (!uint(p.bootId) || !p.bootId || !uint(p.sampleId) ||
      !['off', 'measuring', 'valid', 'out-of-range', 'stale'].includes(String(p.state)) ||
      typeof p.valid !== 'boolean' || p.valid !== (p.state === 'valid') ||
      p.referenceMv !== 5000 || p.dividerNumerator !== 2 || p.dividerDenominator !== 1 ||
      p.calibrated !== false || p.analogPin !== 'A5') return null;
  if (p.state === 'off' || p.state === 'measuring') {
    if (p.sampleId !== 0 || p.ageMs !== null || p.raw !== null || p.millivolts !== null) return null;
  } else {
    if (!p.sampleId || !uint(p.ageMs) || !uint(p.raw, 1023)) return null;
    if (p.state === 'valid') {
      if (p.ageMs > 2000 || p.raw >= 1023 || !uint(p.millivolts, 10000) || p.millivolts !== Math.floor((p.raw * 10000 + 512) / 1024)) return null;
    } else if (p.millivolts !== null || (p.state === 'stale' ? p.ageMs <= 2000 : p.ageMs > 2000)) return null;
  }
  return { ...p, receivedAtMs: Date.now() } as unknown as BatteryReading;
}
export function batteryAge(reading: BatteryReading | null, now: number): number | null {
  return reading?.ageMs === null || !reading ? null : reading.ageMs + Math.max(0, now - reading.receivedAtMs);
}
export function batteryIsFresh(reading: BatteryReading | null, now: number): boolean {
  const age = batteryAge(reading, now);
  return !!reading?.valid && age !== null && age <= 2000;
}
export function batteryVoltage(reading: BatteryReading | null): string {
  return reading?.millivolts === null || !reading ? '—' : `${(reading.millivolts / 1000).toLocaleString('es-AR', { minimumFractionDigits: 2, maximumFractionDigits: 2 })} V`;
}
