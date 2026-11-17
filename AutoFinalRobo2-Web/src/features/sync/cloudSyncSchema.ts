/**
 * AFR2-20261116-01 — Esquema de sincronización en capas JSON (key-value) para la App Online.
 *
 * Modela las configuraciones y el estado del auto en capas anidadas (`system`, `wifi`, `robot`,
 * `config.motion`, `config.motors`, `config.odometry`, `config.line`, `config.radar`, `config.ui`,
 * `simulators`) y calcula actualizaciones parciales (deltas) cada 5–10 s para minimizar el tráfico
 * sobre el ESP8266 y el enlace UART.
 *
 * Regla de simuladores: la capa `simulators` NO se envía en la sincronización periódica; únicamente
 * se incluye cuando el usuario pulsa explícitamente «Sincronizar a web».
 */

export const AFR2_CLOUD_SYNC_SCHEMA = 'afr2-cloud-sync-1' as const;

export interface CloudSyncSystemLayer {
  version: string;
  bootId: number | null;
  megaLink: string;
  wsState: string;
  batteryMv: number | null;
}

export interface CloudSyncWifiLayer {
  mode: string;
  staConnected: boolean;
  staSsid: string | null;
  staIp: string | null;
  apReady: boolean;
  apSsid: string | null;
  apIp: string | null;
  apClients: number;
  apMaxClients: number;
  rssiDbm: number | null;
}

export interface CloudSyncRobotLayer {
  mode: string;
  stopped: boolean | null;
  motorsEnabled: boolean;
  fault: number;
  configurationDirty: boolean;
  pose: { xMm: number; yMm: number; thetaMrad: number; valid: boolean } | null;
}

export interface CloudSyncConfigLayers {
  motion?: {
    speedLimitMmS: number;
    pwmLimit: number;
    rampStep: number;
    kp: number;
    ki: number;
    kd: number;
  };
  motors?: {
    enabled: boolean;
    motorSigns: number;
    trimPermille: { fl: number; fr: number; rl: number; rr: number };
  };
  odometry?: {
    wheelDiameterMm: number;
    radiusSumMm: number;
    hallWired: boolean;
    geometryReady: boolean;
    cpr: { fl: number; fr: number; rl: number; rr: number };
    directionalScalePermille: {
      forward: number;
      reverse: number;
      left: number;
      right: number;
      cw: number;
      ccw: number;
    };
  };
  line?: {
    kind: number;
    invertMask: number;
    calibrated: boolean;
    speedMmS: number;
    kp: number;
  };
  radar?: {
    rangeIndex: number;
    trace: boolean;
    servoInverted: boolean;
  };
  ui?: {
    encoderSteps: number;
    encoderInvert: boolean;
    contrast: number;
    rotation: number;
    longPressMs: number;
    wifiRetryS: number;
    themePreference: string;
  };
}

export interface CloudSyncSimulatorsLayer {
  explicitPush: boolean;
  syncedAtMs: number;
  source: 'goto' | 'sequences' | '3d' | 'all';
  goto?: unknown;
  sequences?: unknown;
  model3d?: unknown;
}

export interface CloudSyncLayeredState {
  system: CloudSyncSystemLayer;
  wifi: CloudSyncWifiLayer;
  robot: CloudSyncRobotLayer;
  config: Required<CloudSyncConfigLayers>;
  simulators?: CloudSyncSimulatorsLayer;
}

export interface CloudSyncEnvelope {
  schema: typeof AFR2_CLOUD_SYNC_SCHEMA;
  deviceId: string;
  mode: 'snapshot' | 'delta';
  rev: number;
  baseRev: number;
  ts: number;
  includeSimulators: boolean;
  layers: {
    system?: CloudSyncSystemLayer;
    wifi?: CloudSyncWifiLayer;
    robot?: CloudSyncRobotLayer;
    config?: CloudSyncConfigLayers;
    simulators?: CloudSyncSimulatorsLayer;
  };
}

/** Claves de capa que participan del ciclo periódico (AFR2-20261117-01). `simulators` queda afuera a propósito. */
export const CLOUD_SYNC_AUTO_LAYERS = [
  'system', 'wifi', 'robot',
  'config.motion', 'config.motors', 'config.odometry', 'config.line', 'config.radar', 'config.ui',
] as const;
export type CloudSyncLayerKey = typeof CLOUD_SYNC_AUTO_LAYERS[number];
export type CloudSyncLayerHashes = Record<CloudSyncLayerKey, string>;

/** JSON canónico: claves ordenadas para que el mismo contenido produzca siempre el mismo hash. */
function canonicalJson(value: unknown): string {
  if (value === null || typeof value !== 'object') return JSON.stringify(value) ?? 'null';
  if (Array.isArray(value)) return `[${value.map(canonicalJson).join(',')}]`;
  const entries = Object.entries(value as Record<string, unknown>)
    .filter(([, v]) => v !== undefined)
    .sort(([a], [b]) => (a < b ? -1 : a > b ? 1 : 0));
  return `{${entries.map(([k, v]) => `${JSON.stringify(k)}:${canonicalJson(v)}`).join(',')}}`;
}

/** FNV-1a de 32 bits sobre las unidades UTF-16 del texto, en hexadecimal de 8 dígitos. */
export function fnv1a32(text: string): string {
  let hash = 0x811c9dc5;
  for (let i = 0; i < text.length; i++) {
    hash ^= text.charCodeAt(i);
    hash = Math.imul(hash, 0x01000193) >>> 0;
  }
  return hash.toString(16).padStart(8, '0');
}

function layerValue(state: CloudSyncLayeredState, key: CloudSyncLayerKey): unknown {
  if (key.startsWith('config.')) return state.config[key.slice(7) as keyof CloudSyncLayeredState['config']];
  return state[key as 'system' | 'wifi' | 'robot'];
}

/** Hash FNV-1a por capa: solo se guarda esto de la última versión confirmada, no la copia completa del estado. */
export function computeLayerHashes(state: CloudSyncLayeredState): CloudSyncLayerHashes {
  const out = {} as CloudSyncLayerHashes;
  for (const key of CLOUD_SYNC_AUTO_LAYERS) out[key] = fnv1a32(canonicalJson(layerValue(state, key)));
  return out;
}

/**
 * Construye un sobre de sincronización (snapshot completo o delta de capas modificadas).
 * Una capa es "sucia" cuando su hash difiere del confirmado. Nunca incluye `simulators`
 * salvo que `explicitSimulators` esté definido.
 */
export function buildCloudSyncEnvelope(params: {
  deviceId: string;
  rev: number;
  ackRev: number;
  current: CloudSyncLayeredState;
  ackedHashes: CloudSyncLayerHashes | null;
  forceSnapshot?: boolean;
  explicitSimulators?: CloudSyncSimulatorsLayer;
}): { envelope: CloudSyncEnvelope; changedKeys: string[]; dirtyLayers: CloudSyncLayerKey[]; hashes: CloudSyncLayerHashes; byteSize: number } {
  const { deviceId, rev, ackRev, current, ackedHashes, forceSnapshot = false, explicitSimulators } = params;
  const isSnapshot = forceSnapshot || !ackedHashes || ackRev === 0;
  const hashes = computeLayerHashes(current);
  const dirtyLayers = CLOUD_SYNC_AUTO_LAYERS.filter(key => isSnapshot || hashes[key] !== ackedHashes![key]);
  const changedKeys: string[] = [...dirtyLayers];
  const layers: CloudSyncEnvelope['layers'] = {};
  const cfg: CloudSyncConfigLayers = {};

  for (const key of dirtyLayers) {
    if (key.startsWith('config.')) {
      const sub = key.slice(7) as keyof CloudSyncConfigLayers;
      (cfg as Record<string, unknown>)[sub] = current.config[sub];
    } else {
      (layers as Record<string, unknown>)[key] = current[key as 'system' | 'wifi' | 'robot'];
    }
  }
  if (Object.keys(cfg).length > 0) {
    layers.config = cfg;
  }

  if (explicitSimulators) {
    layers.simulators = explicitSimulators;
    changedKeys.push(`simulators.${explicitSimulators.source}`);
  }

  const envelope: CloudSyncEnvelope = {
    schema: AFR2_CLOUD_SYNC_SCHEMA,
    deviceId,
    mode: isSnapshot ? 'snapshot' : 'delta',
    rev,
    baseRev: isSnapshot ? 0 : ackRev,
    ts: Date.now(),
    includeSimulators: Boolean(explicitSimulators),
    layers,
  };

  const serialized = JSON.stringify(envelope);
  return {
    envelope,
    changedKeys,
    dirtyLayers,
    hashes,
    byteSize: serialized.length,
  };
}
