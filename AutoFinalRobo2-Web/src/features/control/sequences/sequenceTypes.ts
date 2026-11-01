import geometry from '../../../../../AutoFinalRobo2-Contracts/contracts/geometry.json';
/**
 * Tipos y constantes del documento versionado de secuencias `afr2-sequence-1` (AFR2-20261023-21 / VIS-018).
 * Unidades canonicas: milimetros (`mm`), milisegundos (`ms`) y milirradianes (`mrad`).
 */

export const AFR2_SEQUENCE_SCHEMA = 'afr2-sequence-2' as const;

export const WHEEL_TRACK_KEYS = ['fl', 'fr', 'rl', 'rr'] as const;
export type WheelTrackKey = (typeof WHEEL_TRACK_KEYS)[number];

export const WHEEL_TRACK_LABELS: Record<WheelTrackKey, string> = {
  fl: 'FL · Delantera izq.',
  fr: 'FR · Delantera der.',
  rl: 'RL · Trasera izq.',
  rr: 'RR · Trasera der.',
};

export const EDITOR_MAX_BLOCKS_PER_TRACK = 64;
export const EDITOR_MAX_DURATION_MS = 600_000; // 10 minutos
export const DURATION_STEP_MS = 10;

export const PHYSICAL_MAX_SEGMENTS = 32;
export const PHYSICAL_MAX_DURATION_MS = 120_000; // 120 segundos

export type SequenceEditorLevel = 'movements' | 'wheels';

export type MovementBlockKind =
  | 'constant'
  | 'ramp'
  | 'pivot'
  | 'curve'
  | 'wait'
  | 'finish';

export type WheelBlockKind =
  | 'constant'
  | 'ramp'
  | 'wait'
  | 'finish';

export interface MovementBlock {
  completion?: 'time' | 'distance';
  distanceMm?: number;
  id: string;
  kind: MovementBlockKind;
  label: string;
  /** Duracion en ms (multiplo de 10 ms; 0 ms para 'finish'). */
  durationMs: number;
  /** Velocidad longitudinal en mm/s (constant, curve). */
  vxMmS: number;
  /** Velocidad lateral en mm/s (+ izquierda, - derecha; constant). */
  vyMmS: number;
  /** Velocidad angular en mrad/s (+ antihorario/izquierda, - horario/derecha; pivot, curve). */
  wzMradS: number;
  /** Rampa inicial y final en mm/s (ramp). */
  fromVxMmS: number;
  fromVyMmS: number;
  toVxMmS: number;
  toVyMmS: number;
  /** Sentido explicito para giro sobre el eje ('pivot'). */
  pivotDirection: 'ccw' | 'cw';
  /** Lado de curvatura para 'curve'. */
  arcSide: 'left' | 'right';
}

export interface WheelBlock {
  completion?: 'time' | 'distance';
  distanceMm?: number;
  id: string;
  kind: WheelBlockKind;
  label: string;
  /** Duracion en ms (multiplo de 10 ms; 0 ms para 'finish'). */
  durationMs: number;
  /** Velocidad firmada de rueda en mm/s (constant). */
  speedMmS: number;
  /** Velocidades inicial y final firmadas en mm/s (ramp). */
  fromSpeedMmS: number;
  toSpeedMmS: number;
}

export interface MecanumKinematicProfile {
  /** Suma de semidistancias longitudinal y lateral (lx + ly) en mm. Valor ilustrativo inicial: 300 mm. */
  lxPlusLyMm: number;
  /** Velocidad maxima nominal por rueda en mm/s. Valor ilustrativo inicial: 800 mm/s. */
  maxWheelSpeedMmS: number;
  /** Indica si el perfil fisico fue calibrado contra el robot real. Por defecto false (no calibrado). */
  calibrated: boolean;
}

export const DEFAULT_KINEMATIC_PROFILE: MecanumKinematicProfile = {
  lxPlusLyMm: geometry.lxPlusLyMm,
  maxWheelSpeedMmS: 800,
  calibrated: false,
};

export interface SequenceConversionProvenance {
  convertedFromId: string;
  convertedFromName: string;
  convertedAt: string;
}

export interface Afr2SequenceDocument {
  enabledWheelTracks?: WheelTrackKey[];
  wheelDraftReady?: boolean;
  schema: typeof AFR2_SEQUENCE_SCHEMA;
  id: string;
  name: string;
  updatedAt: string;
  editorLevel: SequenceEditorLevel;
  units: {
    distance: 'mm';
    time: 'ms';
    angle: 'mrad';
  };
  kinematicProfile: MecanumKinematicProfile;
  provenance: SequenceConversionProvenance | null;
  movementBlocks: MovementBlock[];
  wheelTracks: Record<WheelTrackKey, WheelBlock[]>;
}

export interface WheelSpeedVectorMmS {
  fl: number;
  fr: number;
  rl: number;
  rr: number;
}

export interface NormalizedSequenceSegment {
  index: number;
  startTimeMs: number;
  endTimeMs: number;
  durationMs: number;
  startWheelMmS: WheelSpeedVectorMmS;
  endWheelMmS: WheelSpeedVectorMmS;
  sourceLabel: string;
  movementKind?: MovementBlockKind | 'wheels';
}

export interface SequenceValidationIssue {
  id: string;
  severity: 'error' | 'warning';
  track?: 'movements' | WheelTrackKey | 'global' | 'physical';
  blockId?: string;
  message: string;
}
