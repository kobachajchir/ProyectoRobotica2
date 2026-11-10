/** Visual playback timeline. Historical chapter timings are preserved, but no
 * media element or audio event drives the scene. Scroll remains reversible. */
export const ASSEMBLY_DURATION = 43.885714;
const CUES: readonly (readonly [number, number])[] = [
  [0, 0], [1.4, .025], [3.1, .065],
  [5.32, .16], [6.18, .21], [8.6, .255],
  [9.34, .29], [15.9, .37],
  [16.62, .43], [21.96, .482], [26.74, .60],
  [26.995, .665], [29.1, .705], [32.56, .765],
  [36.949, .825], [38.99, .89], [41.06, .95],
  [41.84, .9759], [42.42, .9856], [43.54, 1],
  [ASSEMBLY_DURATION, 1],
];

function interpolate(value: number, source: 0 | 1, target: 0 | 1) {
  if (value <= CUES[0][source]) return CUES[0][target];
  for (let i = 1; i < CUES.length; i++) {
    const a = CUES[i - 1], b = CUES[i];
    if (value <= b[source]) {
      const t = (value - a[source]) / (b[source] - a[source]);
      return a[target] + t * (b[target] - a[target]);
    }
  }
  return CUES[CUES.length - 1][target];
}

export const assemblyProgressAtTime = (seconds: number) => interpolate(seconds, 0, 1);
export const assemblyTimeAtProgress = (progress: number) => interpolate(progress, 1, 0);
