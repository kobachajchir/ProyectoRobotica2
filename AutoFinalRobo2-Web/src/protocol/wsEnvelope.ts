/**
 * Sobre JSON y tipos del contrato Web <-> ESP <-> ATmega2560 (AFR2-20261023-09 / AFR2-20261023-21)
 * Esquema: afr2-ws-1
 */

export const AFR2_WS_SCHEMA = 'afr2-ws-1' as const;
export const AFR2_PRODUCT_ID = 'AutoFinalRobo2' as const;

export type WsConnectionState =
  | 'disconnected'
  | 'connecting'
  | 'handshaking'
  | 'ready'
  | 'degraded';

export type MegaLinkState =
  | 'unknown'
  | 'disconnected'
  | 'handshaking'
  | 'ready'
  | 'stale'
  | 'fault';

export type Afr2CommandName =
  | 'system.hello'
  | 'system.alive'
  | 'system.info'
  | 'system.reboot'
  | 'robot.stop'
  | 'robot.state'
  | 'robot.drive'
  | 'robot.mode'
  | 'robot.test.wheel'
  | 'robot.sensors'
  | 'robot.encoders'
  | 'robot.calibrate'
  | 'robot.config.get'
  | 'robot.preferences'
  | 'robot.line.calibrate'
  | 'robot.goto.start'
  | 'robot.goto.cancel'
  | 'robot.screen.get'
  | 'robot.ui.action'
  | 'robot.sequence.begin'
  | 'robot.sequence.append'
  | 'robot.sequence.commit'
  | 'robot.sequence.start'
  | 'robot.sequence.cancel'
  | 'robot.sequence.status'
  | 'wifi.status'
  | 'wifi.scan'
  | 'wifi.configure'
  | 'wifi.draft.submit'
  | 'wifi.draft.cancel';

export type Afr2ErrorCode =
  | 'BAD_REQUEST'
  | 'UNSUPPORTED_PRODUCT'
  | 'MEGA_LINK_DOWN'
  | 'CAPABILITY_UNAVAILABLE'
  | 'CALIBRATION_REQUIRED'
  | 'BUSY_IN_FLIGHT'
  | 'TIMEOUT'
  | 'SUPERSEDED'
  | 'CONNECTION_CLOSED'
  | 'INTERNAL_ERROR';

export interface Afr2WsRequest<TPayload = Record<string, unknown>> {
  schema: typeof AFR2_WS_SCHEMA;
  type: 'req';
  requestId: string;
  cmd: Afr2CommandName;
  payload: TPayload;
  ts: number;
}

export interface Afr2WsResponse<TPayload = Record<string, unknown>> {
  schema: typeof AFR2_WS_SCHEMA;
  type: 'res';
  requestId: string;
  cmd: Afr2CommandName;
  ok: true;
  status: 'accepted' | 'completed';
  payload: TPayload;
  ts: number;
}

export interface Afr2WsErrorFrame {
  schema: typeof AFR2_WS_SCHEMA;
  type: 'err';
  requestId: string;
  cmd: Afr2CommandName;
  ok: false;
  error: {
    code: Afr2ErrorCode;
    message: string;
    retryable: boolean;
    source: 'web' | 'esp' | 'atmega';
  };
  ts: number;
}

export interface Afr2WsEventFrame<TPayload = Record<string, unknown>> {
  schema: typeof AFR2_WS_SCHEMA;
  type: 'evt';
  event: string;
  payload: TPayload;
  ts: number;
}

export type Afr2WsIncomingFrame =
  | Afr2WsResponse
  | Afr2WsErrorFrame
  | Afr2WsEventFrame;

export type FreshnessState = 'fresh' | 'stale' | 'unknown' | 'local-preview';

export interface SubsystemCapability {
  present: boolean;
  implemented: boolean;
  calibrated: boolean;
  valid: boolean;
  freshness: FreshnessState;
  reason: string;
  limits?: {
    maxSegments?: number;
    maxDurationMs?: number;
    stepMs?: number;
    maxSpeedMmS?: number;
    maxPwmPct?: number;
    wheelDiameterMm?: number;
    radiusSumMm?: number;
    distanceBlocks?: boolean;
  };
}

export interface Afr2CapabilitiesMap {
  uart_bridge: SubsystemCapability;
  mecanum_drive: SubsystemCapability;
  wheel_odometry: SubsystemCapability;
  goto_relative: SubsystemCapability;
  line_array_5ch: SubsystemCapability;
  ultrasonic_hcsr04: SubsystemCapability;
  oled_128x64: SubsystemCapability;
  ui_encoder_sw: SubsystemCapability;
  sequence_runner: SubsystemCapability;
  configuration: SubsystemCapability;
  wheel_test: SubsystemCapability;
  line_follow: SubsystemCapability;
}

export interface Afr2SequenceLimits {
  maxSegments: number;
  maxDurationMs: number;
}

export const DEFAULT_SEQUENCE_LIMITS: Afr2SequenceLimits = {
  maxSegments: 32,
  maxDurationMs: 120_000,
};

export type PhysicalSequenceState =
  | 'idle'
  | 'preparing'
  | 'ready'
  | 'running'
  | 'completed'
  | 'cancelled'
  | 'failed';

export interface PhysicalSequenceStatusPayload {
  sequenceId: string | null;
  executionId: string | null;
  state: PhysicalSequenceState;
  activeSegmentIndex: number | null;
  segmentCount: number;
  elapsedMs: number;
  totalDurationMs: number;
  reason: string | null;
}

export const DEFAULT_DESIGN_CAPABILITIES: Afr2CapabilitiesMap = {
  configuration: {present: true, implemented: false, calibrated: false, valid: false, freshness: 'unknown', reason: 'Esperando configuracion EX01 negociada.'},
  wheel_test: {present: true, implemented: false, calibrated: false, valid: false, freshness: 'unknown', reason: 'Esperando prueba PWM acotada del ATmega.'},
  line_follow: {present: true, implemented: false, calibrated: false, valid: false, freshness: 'unknown', reason: 'Requiere linea, motores y odometria disponibles.'},
  uart_bridge: {
    present: true,
    implemented: false,
    calibrated: false,
    valid: false,
    freshness: 'unknown',
    reason: 'Transporte físico D18/D19 documentado; broker ESP y parser Mega aún sin enlazar.',
  },
  mecanum_drive: {
    present: true,
    implemented: false,
    calibrated: false,
    valid: false,
    freshness: 'unknown',
    reason: 'Cuatro ruedas FL/FR/RL/RR; esperando capacidades y calibración del controlador.',
  },
  wheel_odometry: {
    present: false,
    implemented: false,
    calibrated: false,
    valid: false,
    freshness: 'unknown',
    reason: 'Encoders Hall de rueda sin cablear en la documentación suministrada.',
  },
  goto_relative: {
    present: false,
    implemented: false,
    calibrated: false,
    valid: false,
    freshness: 'unknown',
    reason: 'Deshabilitado hasta disponer de odometría de ruedas conectada y calibrada.',
  },
  line_array_5ch: {
    present: true,
    implemented: false,
    calibrated: false,
    valid: false,
    freshness: 'unknown',
    reason: 'Cinco canales IR1..IR5 definidos; telemetría en vivo pendiente de enlace.',
  },
  ultrasonic_hcsr04: {
    present: true,
    implemented: false,
    calibrated: true,
    valid: false,
    freshness: 'unknown',
    reason: 'HC-SR04 con ventana de eco y validez explícita; sin lectura en vivo sin enlace.',
  },
  oled_128x64: {
    present: true,
    implemented: true,
    calibrated: true,
    valid: true,
    freshness: 'local-preview',
    reason: 'Fuentes C (afr2_oled.c, font7x10.c, afr2_screens.c) y réplica Canvas 128×64 bit a bit.',
  },
  ui_encoder_sw: {
    present: true,
    implemented: true,
    calibrated: true,
    valid: true,
    freshness: 'local-preview',
    reason: 'Giro, SW corto (confirmar) y SW largo ≥ 800 ms (Menú principal); atrás solo por Volver.',
  },
  sequence_runner: {
    present: true,
    implemented: false,
    calibrated: false,
    valid: false,
    freshness: 'unknown',
    limits: {
      maxSegments: 32,
      maxDurationMs: 120_000,
      stepMs: 10,
    },
    reason: 'Simulador local disponible; ejecución física en ATmega/ESP pendiente de perfil EX01 v2 (0x40-0x45).',
  },
};

export interface SensorsSnapshot {
  line: {
    valid: boolean;
    bits: number | null;
    channels: [boolean, boolean, boolean, boolean, boolean] | null;
    ageMs: number | null;
  };
  ultrasonic: {
    valid: boolean;
    distanceMm: number | null;
    ageMs: number | null;
  };
  encoders: {
    wired: boolean;
    valid: boolean;
    counts: {
      fl: number | null;
      fr: number | null;
      rl: number | null;
      rr: number | null;
    };
    ageMs: number | null;
  };
  receivedAtMs: number | null;
}

export const EMPTY_SENSORS_SNAPSHOT: SensorsSnapshot = {
  line: {
    valid: false,
    bits: null,
    channels: null,
    ageMs: null,
  },
  ultrasonic: {
    valid: false,
    distanceMm: null,
    ageMs: null,
  },
  encoders: {
    wired: false,
    valid: false,
    counts: { fl: null, fr: null, rl: null, rr: null },
    ageMs: null,
  },
  receivedAtMs: null,
};

export interface WifiStatusSnapshot {
  applying?: boolean;
  fallbackActive?: boolean;
  scanRunning?: boolean;
  configPersisted?: boolean | null;
  configError?: string | null;
  staStatus?: 'disabled' | 'connected' | 'network-not-found' | 'connection-failed' | 'wrong-password' | 'connecting';
  mode: 'AP' | 'STA' | 'AP_STA' | 'OFF' | 'unknown';
  staConnected: boolean;
  staSsid: string | null;
  staIp: string | null;
  rssiDbm: number | null;
  apSsid: string | null;
  apIp: string | null;
  ageMs: number | null;
}

export const EMPTY_WIFI_SNAPSHOT: WifiStatusSnapshot = {
  mode: 'unknown',
  staConnected: false,
  staSsid: null,
  staIp: null,
  rssiDbm: null,
  apSsid: null,
  apIp: null,
  ageMs: null,
};

export interface WifiNetworkCandidate {
  ssid: string;
  rssiDbm: number;
  channel: number;
  secure: boolean;
}

export function computeFreshness(
  valid: boolean,
  ageMs: number | null,
  staleThresholdMs = 2000
): FreshnessState {
  if (!valid || ageMs === null || Number.isNaN(ageMs)) return 'unknown';
  return ageMs > staleThresholdMs ? 'stale' : 'fresh';
}

export function formatAgeLabel(ageMs: number | null): string {
  if (ageMs === null || Number.isNaN(ageMs)) return 'sin muestra';
  if (ageMs < 1000) return `hace ${Math.max(0, Math.round(ageMs))} ms`;
  return `hace ${(ageMs / 1000).toFixed(1)} s`;
}
