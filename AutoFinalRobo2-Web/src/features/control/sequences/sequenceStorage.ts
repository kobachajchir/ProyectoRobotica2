import {
  createStarterSequenceDocument,
  deriveWheelTracksFromMovements,
  validateSequenceDocument,
} from './mecanumKinematics';
import {
  AFR2_SEQUENCE_SCHEMA,
  DURATION_STEP_MS,
  EDITOR_MAX_BLOCKS_PER_TRACK,
  EDITOR_MAX_DURATION_MS,
  WHEEL_TRACK_KEYS,
  type Afr2SequenceDocument,
  type MovementBlock,
  type MovementBlockKind,
  type WheelBlock,
  type WheelBlockKind,
  type WheelTrackKey,
} from './sequenceTypes';

const STORAGE_LIBRARY_KEY = 'afr2-sequence-library-v2';
const STORAGE_ACTIVE_ID_KEY = 'afr2-sequence-active-id-v2';

const VALID_MOVEMENT_KINDS: MovementBlockKind[] = [
  'constant',
  'ramp',
  'pivot',
  'curve',
  'wait',
  'finish',
];

const VALID_WHEEL_KINDS: WheelBlockKind[] = [
  'constant',
  'ramp',
  'wait',
  'finish',
];

function isFiniteInt(val: unknown): val is number {
  return typeof val === 'number' && Number.isFinite(val);
}

function parseMovementBlock(raw: unknown, idx: number): { ok: true; block: MovementBlock } | { ok: false; error: string } {
  if (!raw || typeof raw !== 'object' || Array.isArray(raw)) {
    return { ok: false, error: `Bloque de movimiento #${idx + 1} no es un objeto válido.` };
  }
  const obj = raw as Record<string, unknown>;
  if (obj.completion !== undefined && obj.completion !== 'time' && obj.completion !== 'distance') return { ok: false, error: 'Modo de finalización desconocido.' };
  const kind = obj.kind as MovementBlockKind;
  if (!VALID_MOVEMENT_KINDS.includes(kind)) {
    return { ok: false, error: `Bloque de movimiento #${idx + 1} tiene tipo desconocido (${String(obj.kind)}).` };
  }
  const durationMs = Number(obj.durationMs ?? 0);
  if (!Number.isInteger(durationMs) || durationMs < 0 || durationMs > EDITOR_MAX_DURATION_MS) {
    return { ok: false, error: `Bloque de movimiento #${idx + 1} tiene duración fuera de rango.` };
  }
  if (kind === 'finish' && durationMs !== 0) {
    return { ok: false, error: `Bloque de finalización #${idx + 1} debe tener duración 0 ms.` };
  }
  if (kind !== 'finish' && (durationMs <= 0 || durationMs % DURATION_STEP_MS !== 0)) {
    return {
      ok: false,
      error: `Bloque de movimiento #${idx + 1} tiene duración ${durationMs} ms (debe ser múltiplo positivo de ${DURATION_STEP_MS} ms).`,
    };
  }

  return {
    ok: true,
    block: {
      id: typeof obj.id === 'string' && obj.id.trim() ? obj.id.trim() : `mov-${idx + 1}`,
      kind,
      label: kind === 'pivot' && (obj.label === 'Girar izquierda' || obj.label === 'Girar derecha')
        ? obj.pivotDirection === 'cw' ? 'Girar CW' : 'Girar CCW'
        : typeof obj.label === 'string' && obj.label.trim() ? obj.label.trim().slice(0, 48) : `Bloque ${idx + 1}`,
      durationMs,
      completion: obj.completion === 'distance' ? 'distance' : 'time',
      distanceMm: typeof obj.distanceMm === 'number' ? obj.distanceMm : undefined,
      vxMmS: isFiniteInt(obj.vxMmS) ? Math.round(obj.vxMmS) : 0,
      vyMmS: isFiniteInt(obj.vyMmS) ? Math.round(obj.vyMmS) : 0,
      wzMradS: isFiniteInt(obj.wzMradS) ? Math.round(obj.wzMradS) : 0,
      fromVxMmS: isFiniteInt(obj.fromVxMmS) ? Math.round(obj.fromVxMmS) : 0,
      fromVyMmS: isFiniteInt(obj.fromVyMmS) ? Math.round(obj.fromVyMmS) : 0,
      toVxMmS: isFiniteInt(obj.toVxMmS) ? Math.round(obj.toVxMmS) : 0,
      toVyMmS: isFiniteInt(obj.toVyMmS) ? Math.round(obj.toVyMmS) : 0,
      pivotDirection: obj.pivotDirection === 'cw' ? 'cw' : 'ccw',
      arcSide: obj.arcSide === 'right' ? 'right' : 'left',
    },
  };
}

function parseWheelBlock(
  raw: unknown,
  track: WheelTrackKey,
  idx: number
): { ok: true; block: WheelBlock } | { ok: false; error: string } {
  if (!raw || typeof raw !== 'object' || Array.isArray(raw)) {
    return { ok: false, error: `Bloque #${idx + 1} de pista ${track.toUpperCase()} es inválido.` };
  }
  const obj = raw as Record<string, unknown>;
  if (obj.completion !== undefined && obj.completion !== 'time' && obj.completion !== 'distance') return { ok: false, error: 'Modo de finalización desconocido.' };
  const kind = obj.kind as WheelBlockKind;
  if (!VALID_WHEEL_KINDS.includes(kind)) {
    return {
      ok: false,
      error: `Bloque #${idx + 1} de pista ${track.toUpperCase()} tiene tipo desconocido (${String(obj.kind)}).`,
    };
  }
  const durationMs = Number(obj.durationMs ?? 0);
  if (!Number.isInteger(durationMs) || durationMs < 0 || durationMs > EDITOR_MAX_DURATION_MS) {
    return { ok: false, error: `Bloque #${idx + 1} de pista ${track.toUpperCase()} tiene duración inválida.` };
  }
  if (kind === 'finish' && durationMs !== 0) {
    return { ok: false, error: `Finalización en pista ${track.toUpperCase()} debe tener duración 0 ms.` };
  }
  if (kind !== 'finish' && (durationMs <= 0 || durationMs % DURATION_STEP_MS !== 0)) {
    return {
      ok: false,
      error: `Bloque #${idx + 1} en ${track.toUpperCase()} (${durationMs} ms) debe ser múltiplo positivo de ${DURATION_STEP_MS} ms.`,
    };
  }

  return {
    ok: true,
    block: {
      id: typeof obj.id === 'string' && obj.id.trim() ? obj.id.trim() : `whl-${track}-${idx + 1}`,
      kind,
      label: typeof obj.label === 'string' && obj.label.trim() ? obj.label.trim().slice(0, 48) : `Rueda ${idx + 1}`,
      durationMs,
      completion: obj.completion === 'distance' ? 'distance' : 'time',
      distanceMm: typeof obj.distanceMm === 'number' ? obj.distanceMm : undefined,
      speedMmS: isFiniteInt(obj.speedMmS) ? Math.round(obj.speedMmS) : 0,
      fromSpeedMmS: isFiniteInt(obj.fromSpeedMmS) ? Math.round(obj.fromSpeedMmS) : 0,
      toSpeedMmS: isFiniteInt(obj.toSpeedMmS) ? Math.round(obj.toSpeedMmS) : 0,
    },
  };
}

/**
 * Comprueba versión (`afr2-sequence-1`), unidades, estructura y límites antes de reemplazar
 * el documento abierto. Los archivos incompatibles o dañados devuelven `ok: false` con motivo explícito.
 */
export function parseAndValidateSequenceJson(
  rawJson: string, allowDraft = false
): { ok: true; document: Afr2SequenceDocument } | { ok: false; error: string } {
  let parsed: unknown;
  try {
    parsed = JSON.parse(rawJson);
  } catch {
    return { ok: false, error: 'El archivo seleccionado no contiene JSON válido.' };
  }

  if (!parsed || typeof parsed !== 'object' || Array.isArray(parsed)) {
    return { ok: false, error: 'La raíz del documento JSON debe ser un objeto.' };
  }

  const root = parsed as Record<string, unknown>;
  if (root.schema !== AFR2_SEQUENCE_SCHEMA && root.schema !== 'afr2-sequence-1') {
    return {
      ok: false,
      error: `Versión de esquema incompatible (${String(root.schema ?? 'ausente')}). Se requiere "${AFR2_SEQUENCE_SCHEMA}".`,
    };
  }

  const units = root.units as Record<string, unknown> | undefined;
  if (
    !units ||
    units.distance !== 'mm' ||
    units.time !== 'ms' ||
    units.angle !== 'mrad'
  ) {
    return {
      ok: false,
      error: 'Las unidades del documento deben ser explícitamente { distance: "mm", time: "ms", angle: "mrad" }.',
    };
  }

  const editorLevel = root.editorLevel;
  if (editorLevel !== 'movements' && editorLevel !== 'wheels') {
    return {
      ok: false,
      error: 'El campo editorLevel debe ser "movements" o "wheels".',
    };
  }

  const rawProfile = (root.kinematicProfile ?? {}) as Record<string, unknown>;
  const lxPlusLyMm = isFiniteInt(rawProfile.lxPlusLyMm)
    ? Math.max(80, Math.min(1200, Math.round(rawProfile.lxPlusLyMm)))
    : 300;
  const maxWheelSpeedMmS = isFiniteInt(rawProfile.maxWheelSpeedMmS)
    ? Math.max(100, Math.min(2500, Math.round(rawProfile.maxWheelSpeedMmS)))
    : 800;
  const calibrated = Boolean(rawProfile.calibrated);

  const rawMovements = Array.isArray(root.movementBlocks) ? root.movementBlocks : [];
  if (rawMovements.length > EDITOR_MAX_BLOCKS_PER_TRACK) {
    return {
      ok: false,
      error: `El documento contiene ${rawMovements.length} bloques de movimiento (máximo ${EDITOR_MAX_BLOCKS_PER_TRACK}).`,
    };
  }

  const movementBlocks: MovementBlock[] = [];
  for (let i = 0; i < rawMovements.length; i += 1) {
    const res = parseMovementBlock(rawMovements[i], i);
    if (!res.ok) return res;
    movementBlocks.push(res.block);
  }

  const rawTracks = (root.wheelTracks ?? {}) as Record<string, unknown>;
  const wheelTracks: Record<WheelTrackKey, WheelBlock[]> = {
    fl: [],
    fr: [],
    rl: [],
    rr: [],
  };

  for (const key of WHEEL_TRACK_KEYS) {
    const arr = Array.isArray(rawTracks[key]) ? (rawTracks[key] as unknown[]) : [];
    if (arr.length > EDITOR_MAX_BLOCKS_PER_TRACK) {
      return {
        ok: false,
        error: `La pista ${key.toUpperCase()} contiene ${arr.length} bloques (máximo ${EDITOR_MAX_BLOCKS_PER_TRACK}).`,
      };
    }
    for (let i = 0; i < arr.length; i += 1) {
      const res = parseWheelBlock(arr[i], key, i);
      if (!res.ok) return res;
      wheelTracks[key].push(res.block);
    }
  }

  const kinematicProfile = { lxPlusLyMm, maxWheelSpeedMmS, calibrated };
  const finalWheelTracks =
    editorLevel === 'movements'
      ? deriveWheelTracksFromMovements(movementBlocks, kinematicProfile)
      : wheelTracks;

  let provenance: Afr2SequenceDocument['provenance'] = null;
  if (root.provenance && typeof root.provenance === 'object' && !Array.isArray(root.provenance)) {
    const prov = root.provenance as Record<string, unknown>;
    if (typeof prov.convertedFromId === 'string' && typeof prov.convertedFromName === 'string') {
      provenance = {
        convertedFromId: prov.convertedFromId,
        convertedFromName: prov.convertedFromName,
        convertedAt: typeof prov.convertedAt === 'string' ? prov.convertedAt : new Date().toISOString(),
      };
    }
  }

  const doc: Afr2SequenceDocument = {
    schema: AFR2_SEQUENCE_SCHEMA,
    id: typeof root.id === 'string' && root.id.trim() ? root.id.trim() : `import-${Date.now()}`,
    name: typeof root.name === 'string' && root.name.trim() ? root.name.trim().slice(0, 64) : 'Programa importado',
    updatedAt: typeof root.updatedAt === 'string' ? root.updatedAt : new Date().toISOString(),
    editorLevel,
    units: { distance: 'mm', time: 'ms', angle: 'mrad' },
    kinematicProfile,
    provenance,
    movementBlocks,
    wheelTracks: root.wheelDraftReady === true ? wheelTracks : finalWheelTracks,
    wheelDraftReady: root.wheelDraftReady === true || editorLevel === 'wheels',
    enabledWheelTracks: Array.isArray(root.enabledWheelTracks) ? WHEEL_TRACK_KEYS.filter(key => (root.enabledWheelTracks as unknown[]).includes(key)) : [...WHEEL_TRACK_KEYS],
  };

  const validation = validateSequenceDocument(doc);
  if (!validation.editorValid && !allowDraft) {
    const firstErr = validation.issues.find((i) => i.severity === 'error');
    return {
      ok: false,
      error: firstErr?.message ?? 'El documento incumple las reglas de validación del editor.',
    };
  }

  return { ok: true, document: doc };
}

export function loadSequenceLibraryFromStorage(): {
  programs: Afr2SequenceDocument[];
  activeId: string;
} {
  const fallback = createStarterSequenceDocument();
  if (typeof window === 'undefined') {
    return { programs: [fallback], activeId: fallback.id };
  }
  try {
    const raw = window.localStorage.getItem(STORAGE_LIBRARY_KEY) ?? window.localStorage.getItem('afr2-sequence-library-v1');
    const activeRaw = window.localStorage.getItem(STORAGE_ACTIVE_ID_KEY) ?? window.localStorage.getItem('afr2-sequence-active-id-v1');
    if (!raw) {
      return { programs: [fallback], activeId: fallback.id };
    }
    const list = JSON.parse(raw);
    if (!Array.isArray(list) || list.length === 0) {
      return { programs: [fallback], activeId: fallback.id };
    }
    const validPrograms: Afr2SequenceDocument[] = [];
    for (const item of list) {
      const res = parseAndValidateSequenceJson(JSON.stringify(item), true);
      if (res.ok) {
        validPrograms.push(res.document);
      }
    }
    if (validPrograms.length === 0) {
      return { programs: [fallback], activeId: fallback.id };
    }
    const activeId =
      activeRaw && validPrograms.some((p) => p.id === activeRaw)
        ? activeRaw
        : validPrograms[0]!.id;
    return { programs: validPrograms, activeId };
  } catch {
    return { programs: [fallback], activeId: fallback.id };
  }
}

export function saveSequenceLibraryToStorage(
  programs: Afr2SequenceDocument[],
  activeId: string
): void {
  if (typeof window === 'undefined') return;
  try {
    window.localStorage.setItem(STORAGE_LIBRARY_KEY, JSON.stringify(programs));
    window.localStorage.setItem(STORAGE_ACTIVE_ID_KEY, activeId);
  } catch {
    // Ignorar errores de cuota de almacenamiento local
  }
}

export function exportSequenceDocumentAsJson(doc: Afr2SequenceDocument): void {
  if (typeof window === 'undefined') return;
  const serialized = JSON.stringify(doc, null, 2);
  const blob = new Blob([serialized], { type: 'application/json;charset=utf-8' });
  const url = URL.createObjectURL(blob);
  const safeName = doc.name
    .toLowerCase()
    .replace(/[^a-z0-9-_]+/g, '-')
    .replace(/^-+|-+$/g, '') || 'secuencia-afr2';
  const anchor = document.createElement('a');
  anchor.href = url;
  anchor.download = `${safeName}.afr2-sequence.json`;
  document.body.appendChild(anchor);
  anchor.click();
  document.body.removeChild(anchor);
  URL.revokeObjectURL(url);
}
