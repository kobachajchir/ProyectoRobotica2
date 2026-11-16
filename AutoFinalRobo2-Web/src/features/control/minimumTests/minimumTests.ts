/**
 * AFR2-20261111-01. Pruebas mínimas de la cátedra: definiciones, registro y estadística (sin React ni red).
 *
 * Marco de referencia: todas las poses van en el sistema del auto al arrancar la prueba (X adelante, Y izquierda,
 * ψ antihorario positivo, mm y grados), igual que las consignas GOTO relativas del ATmega.
 *  - consigna: pose final pedida al auto;
 *  - odometría: pose final informada por el auto, restada a la pose inicial y expresada en ese marco;
 *  - medida: lo que el usuario mide afuera (cinta/cuadrícula);
 *  - error final = medida − consigna;
 *  - deriva = odometría − medida (lo que la odometría se equivoca respecto de la realidad).
 * Ningún valor ausente se rellena: queda `null` y se excluye de medias y desvíos.
 */
import type { RobotPose } from '../../../protocol/robotState';
import type { GotoRunCause } from '../goto/useGotoRouteRunner';

export type MinimumTestId = 'advance' | 'lateral' | 'turn' | 'task' | 'task-loaded';
export type MinimumBatch = 'adjust' | 'verify';
export type MinimumSide = 'left' | 'right' | 'cw' | 'ccw' | null;
export interface Pose2D { xMm: number; yMm: number; psiDeg: number }

export const MINIMUM_REPETITIONS = [1, 2, 3, 4, 5] as const;
export const LOAD_GRAMS = 200;
export const TURN_DEGREES = 90;
export const LINEAR_MM = 1000;
export const DEFAULT_MISSION_SPEED_MMS = 100;
export const MIN_MISSION_SPEED_MMS = 10;
export const DEFAULT_BOX_DISTANCE_MM = 1000;

export interface MinimumTestDef { id: MinimumTestId; label: string; short: string; loaded: boolean; sides?: readonly [MinimumSide, string][] }
export const MINIMUM_TESTS: readonly MinimumTestDef[] = [
  { id: 'advance', label: 'Avance 1,0 m', short: 'Avance 1,0 m', loaded: false },
  { id: 'lateral', label: 'Lateral 1,0 m (izquierda/derecha)', short: 'Lateral 1,0 m', loaded: false, sides: [['left', 'Izquierda'], ['right', 'Derecha']] },
  { id: 'turn', label: 'Giro 90° sobre el centro (CW/CCW)', short: 'Giro 90°', loaded: false, sides: [['ccw', 'Antihorario (CCW)'], ['cw', 'Horario (CW)']] },
  { id: 'task', label: 'Trayectoria de la tarea: ir hasta la caja y volver al drop', short: 'Trayectoria', loaded: false },
  { id: 'task-loaded', label: 'Trayectoria de la tarea con carga (caja de 200 g)', short: 'Trayectoria con carga', loaded: true },
];
export const BATCH_LABEL: Record<MinimumBatch, string> = {
  adjust: 'Ajuste (odometría sin calibrar)',
  verify: 'Verificación (calibrada)',
};
export const CAUSE_LABEL: Record<MinimumCause, string> = {
  completed: 'Completado', stop: 'STOP', cancelled: 'Cancelado (auto o app)', fault: 'Falla del auto', failed: 'No llegó',
  timeout: 'Tiempo agotado', rejected: 'Rechazado por el auto', 'not-sent': 'No enviado',
};
export type MinimumCause = GotoRunCause | 'not-sent';

export interface MinimumTestRecord {
  id: string;
  testId: MinimumTestId;
  testLabel: string;
  side: MinimumSide;
  batch: MinimumBatch;
  repetition: number;
  createdAt: string;
  productVersion: string;
  missionSpeedMmS: number;
  batteryMv: number | null;
  loaded: boolean;
  loadGrams: number;
  setpoint: Pose2D;
  odometryStart: Pose2D | null;
  odometryFinal: Pose2D | null;
  measured: Pose2D | null;
  finalError: Pose2D | null;
  drift: Pose2D | null;
  durationMs: number;
  /** El auto no informa el PWM por ninguna orden existente: queda vacío, no se inventa. */
  pwmMax: number | null;
  cause: MinimumCause;
  fault: number | null;
  legs: number;
  message: string | null;
  notes: string;
}

const round = (value: number, digits = 1) => { const k = 10 ** digits; return Math.round(value * k) / k; };
export const mradToDeg = (mrad: number) => mrad * 180 / Math.PI / 1000;
export function wrapDegrees(value: number) { let v = (value + 180) % 360; if (v < 0) v += 360; return v - 180; }
export const subtractPoses = (a: Pose2D, b: Pose2D): Pose2D => ({
  xMm: round(a.xMm - b.xMm), yMm: round(a.yMm - b.yMm), psiDeg: round(wrapDegrees(a.psiDeg - b.psiDeg)),
});
/** Pose of `end` expressed in the frame of `start` (X forward, Y left). */
export function relativePose(start: RobotPose, end: RobotPose): Pose2D {
  const dx = end.xMm - start.xMm, dy = end.yMm - start.yMm, a = start.thetaMrad / 1000;
  return {
    xMm: round(dx * Math.cos(a) + dy * Math.sin(a)),
    yMm: round(-dx * Math.sin(a) + dy * Math.cos(a)),
    psiDeg: round(wrapDegrees(mradToDeg(end.thetaMrad - start.thetaMrad))),
  };
}
/** AFR2-20261111-08: pose final informada por el recorrido del auto (marco de partida, mm y mrad). */
export const routePose = (pose: { xMm: number; yMm: number; thetaMrad: number }): Pose2D => ({ xMm: round(pose.xMm), yMm: round(pose.yMm), psiDeg: round(wrapDegrees(mradToDeg(pose.thetaMrad))) });
export const rawPose = (pose: RobotPose): Pose2D => ({ xMm: pose.xMm, yMm: pose.yMm, psiDeg: round(mradToDeg(pose.thetaMrad)) });

/** Setpoint (final pose requested) of the three single-leg tests. */
export function singleLegSetpoint(id: MinimumTestId, side: MinimumSide): Pose2D {
  if (id === 'advance') return { xMm: LINEAR_MM, yMm: 0, psiDeg: 0 };
  if (id === 'lateral') return { xMm: 0, yMm: side === 'right' ? -LINEAR_MM : LINEAR_MM, psiDeg: 0 };
  return { xMm: 0, yMm: 0, psiDeg: side === 'cw' ? -TURN_DEGREES : TURN_DEGREES };
}
/** One GOTO leg with the firmware convention (x forward, y left, theta CCW in mrad). */
export function singleLeg(id: MinimumTestId, side: MinimumSide) {
  const target = singleLegSetpoint(id, side);
  return { xMm: target.xMm, yMm: target.yMm, thetaMrad: Math.round(target.psiDeg * Math.PI / 180 * 1000) };
}

export function withErrors(record: MinimumTestRecord, measured: Pose2D | null): MinimumTestRecord {
  return {
    ...record,
    measured,
    finalError: measured ? subtractPoses(measured, record.setpoint) : null,
    drift: measured && record.odometryFinal ? subtractPoses(record.odometryFinal, measured) : null,
  };
}

export const recordKey = (r: Pick<MinimumTestRecord, 'testId' | 'side' | 'batch' | 'repetition'>) => `${r.testId}|${r.side ?? ''}|${r.batch}|${r.repetition}`;
export const groupKey = (r: Pick<MinimumTestRecord, 'testId' | 'side' | 'batch'>) => `${r.testId}|${r.side ?? ''}|${r.batch}`;

export interface Stat { n: number; mean: number | null; sd: number | null }
export function stat(values: readonly (number | null | undefined)[]): Stat {
  const v = values.filter((x): x is number => typeof x === 'number' && Number.isFinite(x));
  if (!v.length) return { n: 0, mean: null, sd: null };
  const mean = v.reduce((a, b) => a + b, 0) / v.length;
  const sd = v.length > 1 ? Math.sqrt(v.reduce((a, b) => a + (b - mean) ** 2, 0) / (v.length - 1)) : null; // desvío muestral
  return { n: v.length, mean: round(mean, 2), sd: sd === null ? null : round(sd, 2) };
}
export const formatStat = (s: Stat, digits = 1) =>
  s.mean === null ? '—' : `${s.mean.toFixed(digits)}${s.sd === null ? '' : ` ± ${s.sd.toFixed(digits)}`}`;

export interface SummaryRow {
  key: string; testId: MinimumTestId; label: string; side: MinimumSide; batch: MinimumBatch; loaded: boolean;
  runs: number; measured: number; speedMmS: Stat;
  errorX: Stat; errorY: Stat; errorPsi: Stat; driftX: Stat; driftY: Stat; driftPsi: Stat; durationS: Stat;
}
const sideText = (id: MinimumTestId, side: MinimumSide) => MINIMUM_TESTS.find(t => t.id === id)?.sides?.find(([s]) => s === side)?.[1];
export const recordLabel = (r: Pick<MinimumTestRecord, 'testId' | 'testLabel' | 'side'>) =>
  `${r.testLabel}${r.side ? ` · ${sideText(r.testId, r.side) ?? r.side}` : ''}`;
export function summarize(records: readonly MinimumTestRecord[]): SummaryRow[] {
  const groups = new Map<string, MinimumTestRecord[]>();
  for (const r of records) groups.set(groupKey(r), [...(groups.get(groupKey(r)) ?? []), r]);
  const order = (r: MinimumTestRecord) => MINIMUM_TESTS.findIndex(t => t.id === r.testId) * 10 + (r.batch === 'adjust' ? 0 : 1);
  return [...groups.entries()].map(([key, list]) => {
    const done = list.filter(r => r.cause === 'completed' && r.measured);
    return {
      key, testId: list[0].testId, label: recordLabel(list[0]), side: list[0].side, batch: list[0].batch, loaded: list[0].loaded,
      runs: list.length, measured: done.length, speedMmS: stat(done.map(r => r.missionSpeedMmS)),
      errorX: stat(done.map(r => r.finalError?.xMm)), errorY: stat(done.map(r => r.finalError?.yMm)), errorPsi: stat(done.map(r => r.finalError?.psiDeg)),
      driftX: stat(done.map(r => r.drift?.xMm)), driftY: stat(done.map(r => r.drift?.yMm)), driftPsi: stat(done.map(r => r.drift?.psiDeg)),
      durationS: stat(done.map(r => r.durationMs / 1000)),
    };
  }).sort((a, b) => order({ testId: a.testId, batch: a.batch } as MinimumTestRecord) - order({ testId: b.testId, batch: b.batch } as MinimumTestRecord));
}

// ---- Exportación -------------------------------------------------------------------------------
const csvCell = (value: unknown) => {
  const text = value === null || value === undefined ? '' : String(value);
  return /[;"\n\r]/.test(text) ? `"${text.replace(/"/g, '""')}"` : text;
};
const csv = (rows: readonly (readonly unknown[])[]) => '﻿' + rows.map(row => row.map(csvCell).join(';')).join('\r\n') + '\r\n';
const pose = (p: Pose2D | null) => p ? [p.xMm, p.yMm, p.psiDeg] : ['', '', ''];

export const RECORD_CSV_HEADER = [
  'Ensayo', 'Lado/sentido', 'Tanda', 'Repetición', 'Fecha', 'Versión de producto', 'Velocidad de misión (mm/s)', 'Batería (mV)', 'Con carga', 'Carga (g)',
  'Consigna x (mm)', 'Consigna y (mm)', 'Consigna ψ (°)', 'Odometría final x (mm)', 'Odometría final y (mm)', 'Odometría final ψ (°)',
  'Medida x (mm)', 'Medida y (mm)', 'Medida ψ (°)', 'Error final x (mm)', 'Error final y (mm)', 'Error final ψ (°)',
  'Deriva x (mm)', 'Deriva y (mm)', 'Deriva ψ (°)', 'Duración (s)', 'PWM máximo', 'Causa de fin', 'Falla', 'Tramos', 'Mensaje', 'Notas',
] as const;
export function recordsToCsv(records: readonly MinimumTestRecord[]) {
  return csv([RECORD_CSV_HEADER, ...records.map(r => [
    r.testLabel, r.side ? sideText(r.testId, r.side) ?? r.side : '', BATCH_LABEL[r.batch], r.repetition, r.createdAt, r.productVersion, r.missionSpeedMmS,
    r.batteryMv ?? '', r.loaded ? 'sí' : 'no', r.loadGrams, ...pose(r.setpoint), ...pose(r.odometryFinal), ...pose(r.measured), ...pose(r.finalError), ...pose(r.drift),
    round(r.durationMs / 1000, 2), r.pwmMax ?? '', CAUSE_LABEL[r.cause], r.fault ?? '', r.legs, r.message ?? '', r.notes,
  ])]);
}
export function recordsToJson(records: readonly MinimumTestRecord[], productVersion: string) {
  return JSON.stringify({
    schema: 'afr2-minimum-tests-1', exportedAt: new Date().toISOString(), productVersion,
    frame: 'X adelante, Y izquierda, ψ antihorario (mm, grados), referida a la pose del auto al iniciar cada prueba',
    definitions: { finalError: 'medida − consigna', drift: 'odometría final − medida' }, records,
  }, null, 2);
}

/**
 * Formato de la Tabla 24 del informe D2. El texto de esa tabla no está en el repositorio (Revision/R02_20261110 no existe
 * todavía): se exporta una tabla por ensayo y tanda con media ± desvío muestral de las repeticiones medidas. Si la tabla
 * del informe pide otras columnas, ajustar TABLE24_HEADER y este mapeo.
 */
export const TABLE24_HEADER = [
  'Ensayo', 'Tanda', 'Carga', 'Repeticiones medidas / enviadas', 'Velocidad (mm/s)',
  'Error final x (mm)', 'Error final y (mm)', 'Error final ψ (°)', 'Deriva x (mm)', 'Deriva y (mm)', 'Deriva ψ (°)', 'Duración (s)',
] as const;
export function summaryToTable24Csv(rows: readonly SummaryRow[]) {
  return csv([TABLE24_HEADER, ...rows.map(r => [
    r.label, BATCH_LABEL[r.batch], r.loaded ? `${LOAD_GRAMS} g` : 'sin carga', `${r.measured} / ${r.runs}`, formatStat(r.speedMmS, 0),
    formatStat(r.errorX), formatStat(r.errorY), formatStat(r.errorPsi), formatStat(r.driftX), formatStat(r.driftY), formatStat(r.driftPsi), formatStat(r.durationS, 1),
  ])]);
}

// ---- Almacenamiento local (por navegador) -------------------------------------------------------
const STORAGE_KEY = 'afr2.minimumTests.v1';
export function loadRecords(): MinimumTestRecord[] {
  try {
    const raw = window.localStorage.getItem(STORAGE_KEY);
    const parsed = raw ? JSON.parse(raw) : [];
    return Array.isArray(parsed) ? parsed.filter(r => r && typeof r.id === 'string' && typeof r.testId === 'string') : [];
  } catch { return []; }
}
export function saveRecords(records: readonly MinimumTestRecord[]) {
  try { window.localStorage.setItem(STORAGE_KEY, JSON.stringify(records)); return true; } catch { return false; }
}
export function downloadText(filename: string, text: string, type: string) {
  const url = URL.createObjectURL(new Blob([text], { type }));
  const link = document.createElement('a');
  link.href = url; link.download = filename; document.body.appendChild(link); link.click(); link.remove();
  setTimeout(() => URL.revokeObjectURL(url), 1000);
}
