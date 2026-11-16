/* AFR2-20261113-01. Interpretación de los cinco IR de línea (solo Web, a partir de la máscara digital).
 * IR1 (bit 0) está a la izquierda e IR5 (bit 4) a la derecha, igual que el seguidor del ATmega: posición = Σ(2 − i) / n,
 * positiva con la línea a la izquierda. Si la línea queda a la izquierda, el auto está derivando hacia la derecha. */
export type LineCenter = 'none' | 'centered' | 'left' | 'right' | 'cross' | 'split';
export type LineOrientation = 'unknown' | 'aligned' | 'tilt-left' | 'tilt-right' | 'horizontal';
export interface LineAnalysis {
  bits: number; detected: boolean; count: number; contiguous: boolean;
  /** Línea horizontal: atraviesa los cinco sensores (o cuatro seguidos), p. ej. un cruce o una meta. */
  horizontal: boolean;
  /** −2..2 en pasos de sensor; + = línea a la izquierda. null sin línea. */
  position: number | null;
  center: LineCenter; orientation: LineOrientation;
  /** Velocidad lateral de la línea bajo los sensores (sensores/s, + hacia la izquierda), con el historial reciente. */
  slope: number | null;
}
export interface LineSample { atMs: number; position: number | null; horizontal: boolean }
export const LINE_HISTORY_MS = 1200;
const CENTER_BAND = .5;
const TILT_SLOPE = .8;

export function analyzeLine(bits: number, history: readonly LineSample[]): LineAnalysis {
  const active = Array.from({ length: 5 }, (_, i) => !!(bits & (1 << i)));
  const count = active.filter(Boolean).length;
  const first = active.indexOf(true), last = active.lastIndexOf(true);
  const contiguous = count > 0 && last - first + 1 === count;
  const horizontal = count === 5 || (count >= 4 && contiguous);
  const position = count ? active.reduce((sum, on, i) => sum + (on ? 2 - i : 0), 0) / count : null;
  const center: LineCenter = !count ? 'none' : horizontal ? 'cross' : !contiguous ? 'split'
    : Math.abs(position!) <= CENTER_BAND ? 'centered' : position! > 0 ? 'left' : 'right';
  // Orientación estimada por cómo se desplaza la línea bajo los sensores mientras el auto avanza.
  const valid = history.filter(sample => sample.position !== null && !sample.horizontal);
  let slope: number | null = null;
  if (valid.length >= 3 && position !== null && !horizontal) {
    const oldest = valid[0], newest = valid[valid.length - 1], dt = (newest.atMs - oldest.atMs) / 1000;
    if (dt >= .3) slope = (newest.position! - oldest.position!) / dt;
  }
  const orientation: LineOrientation = horizontal ? 'horizontal' : position === null || slope === null ? 'unknown'
    : Math.abs(slope) < TILT_SLOPE ? 'aligned' : slope > 0 ? 'tilt-left' : 'tilt-right';
  return { bits, detected: count > 0, count, contiguous, horizontal, position, center, orientation, slope };
}

export const LINE_CENTER_TEXT: Record<LineCenter, string> = {
  none: 'Sin línea bajo los sensores',
  centered: 'Sobre la línea (centrado)',
  left: 'Derivando a la derecha · la línea quedó a la izquierda',
  right: 'Derivando a la izquierda · la línea quedó a la derecha',
  cross: 'Sobre una línea horizontal',
  split: 'Dos marcas separadas (bifurcación o ruido)',
};
export const LINE_ORIENTATION_TEXT: Record<LineOrientation, string> = {
  unknown: 'Sin estimar (hace falta línea y movimiento)',
  aligned: 'Alineada con el auto',
  'tilt-left': 'Inclinada hacia la izquierda',
  'tilt-right': 'Inclinada hacia la derecha',
  horizontal: 'Horizontal (cruza al auto)',
};
