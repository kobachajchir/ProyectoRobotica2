/** AFR2-20261025-14. EX01v8. Position derives from applied PWM, not shaft feedback. */
export interface RangeSample {
  sampleId: number; angleDeg: number; pulseUs: number; distanceMm: number | null;
  echoUs: number; valid: boolean; error: 'none'|'no-rise'|'no-fall'|'echo-high'|'out-of-range'|'moving'|'stale'|'unknown';
  ageMs: number | null; measuredAtMs: number | null;
}
export interface HeadReading {
  bootId: number; angleDeg: number; pulseUs: number; targetUs: number; enabled: boolean;
  scanRevision: number; running: boolean; intervalMs: number; frontalEligible: boolean; sample: RangeSample; receivedAtMs: number;
}
export interface IrReading {
  bootId: number; sampleId: number; kind: 'off'|'digital'|'adc'; rawValid: boolean;
  calibrated: boolean; valid: boolean; bits: number; raw: (number|null)[]; ageMs: number|null;
}
export interface RadarConfig {
  minAngleDeg: number; maxAngleDeg: number; speedDegS: number; settleMs: number; intervalMs: number; inverted: boolean;
}
export interface RadarReply {
  bootId: number; scanRevision: number; nextCursor: number; dropped: number; mode: 'snapshot'|'delta'; running: boolean;
  angleDeg: number; enabled: boolean; pulseUs: number; targetUs: number; samples: RangeSample[];
}
export interface ScreenStatus { screenCode: number; navigationRevision: number; selection: number; firstVisible: number; private: boolean }
export const SENSOR_ERRORS: Record<RangeSample['error'],string> = {
  none:'Lectura valida', 'no-rise':'Sin subida de ECHO', 'no-fall':'ECHO no termino', 'echo-high':'ECHO ocupado',
  'out-of-range':'Fuera de 2 cm–4 m', moving:'Servo en movimiento', stale:'Lectura vencida', unknown:'Sin medicion',
};
