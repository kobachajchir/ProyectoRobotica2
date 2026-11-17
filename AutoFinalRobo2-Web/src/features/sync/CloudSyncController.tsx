import React, { createContext, useCallback, useContext, useEffect, useMemo, useRef, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { FloatingModal, type ModalOrigin } from '../../components/ui/FloatingModal';
import { UiIcon } from '../../components/ui/UiIcon';
import { useTheme } from '../../app/providers/ThemeProvider';
import { notify } from '../../components/notifications/notify';
import { useConnection } from '../connection/ConnectionContext';
import { version as WEB_VERSION } from '../../../package.json';
import {
  buildCloudSyncEnvelope,
  type CloudSyncEnvelope,
  type CloudSyncLayerHashes,
  type CloudSyncLayeredState,
  type CloudSyncSimulatorsLayer,
} from './cloudSyncSchema';
import {
  cancelPairing,
  createPairing,
  ensureWriter,
  generateWriteKey,
  isHostedRemote,
  isValidWriteKey,
  listViewers,
  pairingPending,
  pushEnvelopeToFirebase,
  REMOTE_VIEW_URL,
  revokeViewer,
  type LinkedViewer,
} from './firebaseSync';

/** AFR2-20261117-05: la copia publicada en Firebase Hosting nunca escribe (solo muestra la vista remota). */
const HOSTED_REMOTE = isHostedRemote();
/** Latido a Firebase sin capas sucias: como mucho uno por minuto (cuida la cuota gratuita de escrituras). */
const FIREBASE_HEARTBEAT_MS = 60_000;

const SYNC_SETTINGS_STORAGE_KEY = 'afr2.cloudSync.settings.v1';
const SYNC_MIRROR_STORAGE_KEY = 'afr2.cloudSync.mirror.v1';
const MAX_CONSECUTIVE_FAILS = 5;

interface PersistedSyncSettings {
  enabled: boolean;
  intervalMs: number;
  endpointUrl: string;
}

function loadSyncSettings(): PersistedSyncSettings {
  try {
    const raw = localStorage.getItem(SYNC_SETTINGS_STORAGE_KEY);
    if (raw) {
      const parsed = JSON.parse(raw) as Partial<PersistedSyncSettings>;
      return {
        enabled: typeof parsed.enabled === 'boolean' ? parsed.enabled : false,
        intervalMs:
          typeof parsed.intervalMs === 'number' && parsed.intervalMs >= 5000 && parsed.intervalMs <= 10000
            ? parsed.intervalMs
            : 5000,
        endpointUrl: typeof parsed.endpointUrl === 'string' ? parsed.endpointUrl : '',
      };
    }
  } catch {
    /* Optional storage */
  }
  return { enabled: false, intervalMs: 5000, endpointUrl: '' };
}

export type FirebaseWriterState = 'no-car' | 'loading' | 'off' | 'registering' | 'ready' | 'claimed' | 'error';

export interface CloudSyncContextValue {
  enabled: boolean;
  online: boolean;
  internetOnline: boolean;
  checkingInternet: boolean;
  checkInternet: () => Promise<boolean>;
  /** false cuando no hay endpoint ni auto conectado: no hay a dónde enviar. */
  hasTarget: boolean;
  syncing: boolean;
  failCount: number;
  failModalOpen: boolean;
  intervalMs: number;
  endpointUrl: string;
  rev: number;
  ackRev: number;
  lastSyncAtMs: number | null;
  lastPayloadBytes: number;
  lastChangedKeys: string[];
  lastEnvelope: CloudSyncEnvelope | null;
  setEnabled: (enabled: boolean) => Promise<void>;
  setIntervalMs: (intervalMs: number) => void;
  setEndpointUrl: (url: string) => void;
  /** AFR2-20261117-05: la clave de escritura vive en el ESP; acá solo se sabe si existe. */
  firebaseKeySet: boolean;
  /** Estado del registro de este navegador como escritor de la copia online. */
  firebaseWriter: FirebaseWriterState;
  activateFirebase: () => Promise<void>;
  deactivateFirebase: () => Promise<void>;
  /** true cuando este navegador puede escribir en Firebase (clave + auto conectado + no es la copia en Hosting). */
  firebaseWritable: boolean;
  firebaseLastAtMs: number | null;
  hostedRemote: boolean;
  dismissFailureKeepSync: () => Promise<void>;
  disableSyncFromFailure: () => Promise<void>;
  triggerSyncNow: (options?: { forceSnapshot?: boolean }) => Promise<boolean>;
  syncSimulatorToWeb: (source: 'goto' | 'sequences' | '3d', data: unknown) => Promise<boolean>;
}

const CloudSyncContext = createContext<CloudSyncContextValue | null>(null);

export function useCloudSync(): CloudSyncContextValue {
  const ctx = useContext(CloudSyncContext);
  if (!ctx) {
    throw new Error('useCloudSync debe usarse dentro de CloudSyncProvider');
  }
  return ctx;
}

export function CloudSyncProvider({ children }: { children: React.ReactNode }) {
  const {
    wsState,
    megaLinkState,
    wifiStatus,
    espCloudSync,
    robotState,
    battery,
    capabilities,
    requestCommand,
  } = useConnection();
  const { preference: themePreference } = useTheme();

  const initial = useMemo(loadSyncSettings, []);
  const [enabled, setEnabledState] = useState<boolean>(initial.enabled);
  const [intervalMs, setIntervalMsState] = useState<number>(initial.intervalMs);
  const [endpointUrl, setEndpointUrlState] = useState<string>(initial.endpointUrl);
  const [internetOnline, setInternetOnline] = useState<boolean>(() => typeof navigator !== 'undefined' ? navigator.onLine !== false : false);
  const [checkingInternet, setCheckingInternet] = useState<boolean>(false);
  const internetOnlineRef = useRef<boolean>(internetOnline);
  internetOnlineRef.current = internetOnline;
  const internetProbeSeq = useRef<number>(0);

  const checkInternet = useCallback(async (): Promise<boolean> => {
    const seq = ++internetProbeSeq.current;
    if (typeof navigator !== 'undefined' && navigator.onLine === false) {
      internetOnlineRef.current = false;
      setInternetOnline(false);
      setCheckingInternet(false);
      return false;
    }
    setCheckingInternet(true);
    const controller = new AbortController();
    const timeout = window.setTimeout(() => controller.abort(), 3000);
    try {
      await fetch('https://firestore.googleapis.com/', { mode: 'no-cors', cache: 'no-store', signal: controller.signal });
      window.clearTimeout(timeout);
      if (seq !== internetProbeSeq.current) return internetOnlineRef.current;
      internetOnlineRef.current = true;
      setInternetOnline(true);
      setCheckingInternet(false);
      return true;
    } catch {
      window.clearTimeout(timeout);
      if (seq !== internetProbeSeq.current) return internetOnlineRef.current;
      internetOnlineRef.current = false;
      setInternetOnline(false);
      setCheckingInternet(false);
      return false;
    }
  }, []);

  useEffect(() => {
    const probe = () => { void checkInternet(); };
    const onOffline = () => {
      ++internetProbeSeq.current;
      internetOnlineRef.current = false;
      setInternetOnline(false);
      setCheckingInternet(false);
    };
    probe();
    window.addEventListener('online', probe);
    window.addEventListener('offline', onOffline);
    const onVis = () => { if (document.visibilityState === 'visible') probe(); };
    document.addEventListener('visibilitychange', onVis);
    const timer = window.setInterval(() => {
      if (document.visibilityState === 'visible') probe();
    }, 15000);
    return () => {
      window.removeEventListener('online', probe);
      window.removeEventListener('offline', onOffline);
      document.removeEventListener('visibilitychange', onVis);
      window.clearInterval(timer);
    };
  }, [checkInternet, wsState, wifiStatus.staConnected, wifiStatus.mode]);

  // AFR2-20261117-05: la clave de escritura la guarda el ESP (sync.key.get/set), así todos los navegadores
  // conectados al auto publican en el mismo lugar. Nunca se guarda en el navegador.
  const [firebaseKey, setFirebaseKey] = useState<string>('');
  const [firebaseWriter, setFirebaseWriter] = useState<FirebaseWriterState>('no-car');
  const [firebaseLastAtMs, setFirebaseLastAtMs] = useState<number | null>(null);
  const firebaseLastWriteRef = useRef<number>(0);

  const registerWriter = useCallback(async (key: string) => {
    setFirebaseWriter('registering');
    try {
      setFirebaseWriter(await ensureWriter(key) === 'ok' ? 'ready' : 'claimed');
    } catch {
      setFirebaseWriter('error');
    }
  }, []);

  useEffect(() => {
    if (HOSTED_REMOTE) return;
    if (wsState !== 'ready') { setFirebaseWriter('no-car'); setFirebaseKey(''); return; }
    let cancelled = false;
    setFirebaseWriter('loading');
    void (async () => {
      try {
        const res = await requestCommand<{ key?: string }>('sync.key.get', {}, 2000);
        const key = res.status === 'completed' && typeof res.payload.key === 'string' && isValidWriteKey(res.payload.key) ? res.payload.key : '';
        if (cancelled) return;
        setFirebaseKey(key);
        if (!key) { setFirebaseWriter('off'); return; }
        await registerWriter(key);
      } catch {
        // ESP anterior a 1.35.0 (sin sync.key.get) o enlace ocupado.
        if (!cancelled) setFirebaseWriter('error');
      }
    })();
    return () => { cancelled = true; };
  }, [wsState, requestCommand, registerWriter]);

  const activateFirebase = useCallback(async () => {
    if (wsState !== 'ready') return;
    if (!await checkInternet()) {
      notify.warning({
        id: 'afr2-sync-no-internet',
        title: 'Sin Internet',
        description: 'Conectate a Internet para activar la copia en Firebase.',
      });
      return;
    }
    const key = firebaseKey || generateWriteKey();
    if (!firebaseKey) {
      try {
        const res = await requestCommand('sync.key.set', { key }, 3000);
        if (res.status !== 'completed') { setFirebaseWriter('error'); return; }
      } catch {
        setFirebaseWriter('error');
        return;
      }
      setFirebaseKey(key);
    }
    firebaseLastWriteRef.current = 0;
    await registerWriter(key);
  }, [checkInternet, firebaseKey, registerWriter, requestCommand, wsState]);

  const deactivateFirebase = useCallback(async () => {
    if (wsState !== 'ready') return;
    try {
      const res = await requestCommand('sync.key.set', { key: '' }, 3000);
      if (res.status === 'completed') { setFirebaseKey(''); setFirebaseWriter('off'); setFirebaseLastAtMs(null); }
    } catch {
      /* El ESP no respondió: se mantiene el estado actual. */
    }
  }, [requestCommand, wsState]);

  // Solo escribe quien tiene el auto: sin él, el estado local son valores por defecto y pisaría la copia real.
  const firebaseWritable = !HOSTED_REMOTE && firebaseWriter === 'ready' && wsState === 'ready';
  const hasTarget = !HOSTED_REMOTE && (endpointUrl.trim() !== '' || wsState === 'ready');
  const [online, setOnline] = useState<boolean>(false);
  useEffect(() => { if (!hasTarget) setOnline(false); }, [hasTarget]);
  const [syncing, setSyncing] = useState<boolean>(false);
  const [failCount, setFailCount] = useState<number>(0);
  const [failModalOpen, setFailModalOpen] = useState<boolean>(false);
  const [rev, setRev] = useState<number>(1);
  const [ackRev, setAckRev] = useState<number>(0);
  const [lastSyncAtMs, setLastSyncAtMs] = useState<number | null>(null);
  const [lastPayloadBytes, setLastPayloadBytes] = useState<number>(0);
  const [lastChangedKeys, setLastChangedKeys] = useState<string[]>([]);
  const [lastEnvelope, setLastEnvelope] = useState<CloudSyncEnvelope | null>(null);

  // AFR2-20261117-01: se conserva solo el hash FNV-1a por capa de lo confirmado y de lo enviado.
  const ackedHashesRef = useRef<CloudSyncLayerHashes | null>(null);
  const pendingHashesRef = useRef<CloudSyncLayerHashes | null>(null);
  const revRef = useRef<number>(1);
  const ackRevRef = useRef<number>(0);
  const failCountRef = useRef<number>(0);

  useEffect(() => {
    try {
      localStorage.setItem(
        SYNC_SETTINGS_STORAGE_KEY,
        JSON.stringify({ enabled, intervalMs, endpointUrl })
      );
    } catch {
      /* Optional storage */
    }
  }, [enabled, intervalMs, endpointUrl]);

  const currentLayeredState = useMemo<CloudSyncLayeredState>(() => {
    return {
      system: {
        version: WEB_VERSION,
        bootId: wifiStatus.espBootId ?? null,
        megaLink: megaLinkState,
        wsState,
        batteryMv: battery?.valid ? battery.millivolts : null,
      },
      wifi: {
        mode: wifiStatus.mode,
        staConnected: wifiStatus.staConnected,
        staSsid: wifiStatus.staSsid,
        staIp: wifiStatus.staIp,
        apReady: wifiStatus.apReady ?? false,
        apSsid: wifiStatus.apSsid,
        apIp: wifiStatus.apIp,
        apClients: wifiStatus.apClients ?? 0,
        apMaxClients: 4,
        rssiDbm: wifiStatus.rssiDbm,
      },
      robot: {
        mode: robotState?.mode ?? 'unknown',
        stopped: robotState?.stopped ?? null,
        motorsEnabled: robotState?.motorsEnabled ?? false,
        fault: robotState?.fault ?? 0,
        configurationDirty: robotState?.configurationDirty ?? false,
        pose: robotState?.pose
          ? {
              xMm: robotState.pose.xMm,
              yMm: robotState.pose.yMm,
              thetaMrad: robotState.pose.thetaMrad,
              valid: true, // parseRobotState only exposes poses received with valid === true.
            }
          : null,
      },
      config: {
        motion: {
          speedLimitMmS: capabilities.mecanum_drive.limits?.maxSpeedMmS ?? 150,
          pwmLimit: Math.round(((capabilities.manual_pwm?.limits?.maxPwmPct ?? 100) * 255) / 100),
          rampStep: 4,
          kp: 128,
          ki: 8,
          kd: 0,
        },
        motors: {
          enabled: robotState?.motorsEnabled ?? false,
          motorSigns: 0,
          trimPermille: { fl: 1000, fr: 1000, rl: 1000, rr: 1000 },
        },
        odometry: {
          wheelDiameterMm: capabilities.mecanum_drive.limits?.wheelDiameterMm ?? 80,
          radiusSumMm: capabilities.mecanum_drive.limits?.radiusSumMm ?? 150,
          hallWired: capabilities.wheel_odometry.present,
          geometryReady: capabilities.wheel_odometry.calibrated,
          cpr: { fl: 370, fr: 370, rl: 370, rr: 370 },
          directionalScalePermille: {
            forward: 1000,
            reverse: 1000,
            left: 775,
            right: 775,
            cw: 1000,
            ccw: 1000,
          },
        },
        line: {
          kind: 1,
          invertMask: 0,
          calibrated: capabilities.line_follow.calibrated,
          speedMmS: 80,
          kp: 100,
        },
        radar: {
          rangeIndex: 0,
          trace: true,
          servoInverted: false,
        },
        ui: {
          encoderSteps: 4,
          encoderInvert: false,
          contrast: 200,
          rotation: 0,
          longPressMs: robotState?.longPressMs ?? 800,
          wifiRetryS: 30,
          themePreference,
        },
      },
    };
  }, [wsState, megaLinkState, wifiStatus, robotState, battery, capabilities, themePreference]);

  const recordSyncOutcome = useCallback(
    async (ok: boolean, nextAckRev: number) => {
      if (ok) {
        failCountRef.current = 0;
        setFailCount(0);
        setFailModalOpen(false);
        setOnline(true);
        setLastSyncAtMs(Date.now());
        ackRevRef.current = nextAckRev;
        setAckRev(nextAckRev);
        if (pendingHashesRef.current) ackedHashesRef.current = pendingHashesRef.current;
      } else {
        const nextFails = failCountRef.current + 1;
        failCountRef.current = nextFails;
        setFailCount(nextFails);
        setOnline(false);
        if (nextFails >= MAX_CONSECUTIVE_FAILS) {
          setFailModalOpen(true);
        }
      }
      if (wsState === 'ready') {
        try {
          await requestCommand(
            'sync.configure',
            {
              internetOk: internetOnlineRef.current,
              intervalMs,
              attemptOk: ok,
              ackRev: ok ? nextAckRev : ackRevRef.current,
              ...(endpointUrl ? { endpoint: endpointUrl.slice(0, 46) } : {}),
            },
            2000
          );
        } catch {
          /* Non-fatal if UART/WS busy */
        }
      }
    },
    [endpointUrl, intervalMs, requestCommand, wsState]
  );

  const performSync = useCallback(
    async (options?: { forceSnapshot?: boolean; explicitSimulators?: CloudSyncSimulatorsLayer }) => {
      if (!enabled) return false;
      // AFR2-20261117-01: sin endpoint y sin auto no hay a dónde enviar. No es «Online» ni cuenta como falla
      // (si no, el aviso de 5 fallas aparecería cada vez que se abre la Web sin el auto).
      if (!hasTarget) { setOnline(false); return false; }
      setSyncing(true);
      const nextRev = revRef.current + 1;
      revRef.current = nextRev;
      setRev(nextRev);

      const deviceId = wifiStatus.espBootId ? `afr2-${wifiStatus.espBootId}` : 'afr2-web';
      const { envelope, changedKeys, dirtyLayers, hashes, byteSize } = buildCloudSyncEnvelope({
        deviceId,
        rev: nextRev,
        ackRev: ackRevRef.current,
        current: currentLayeredState,
        ackedHashes: ackedHashesRef.current,
        forceSnapshot: options?.forceSnapshot,
        explicitSimulators: options?.explicitSimulators,
      });

      pendingHashesRef.current = hashes;
      setLastEnvelope(envelope);
      setLastPayloadBytes(byteSize);
      setLastChangedKeys(changedKeys);

      let attemptOk = internetOnlineRef.current;

      if (attemptOk && endpointUrl.trim()) {
        try {
          const controller = new AbortController();
          const timeout = setTimeout(() => controller.abort(), 3500);
          const response = await fetch(endpointUrl.trim(), {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(envelope),
            signal: controller.signal,
          });
          clearTimeout(timeout);
          attemptOk = response.ok;
        } catch {
          attemptOk = false;
        }
      } else if (wsState === 'ready' && !wifiStatus.staConnected && wifiStatus.mode === 'AP' && !firebaseWritable) {
        // En modo AP puro sin copia Firebase ni endpoint configurado, no hay destino online activo.
        attemptOk = false;
      }

      // AFR2-20261117-05: copia en Firebase (car/state). Escribe si hay capas sucias, simuladores o snapshot;
      // sin cambios, solo un latido por minuto. Con el navegador sin Internet el fetch falla y cuenta como falla.
      if (attemptOk && firebaseWritable) {
        const due = envelope.mode === 'snapshot' || dirtyLayers.length > 0 || Boolean(options?.explicitSimulators)
          || Date.now() - firebaseLastWriteRef.current >= FIREBASE_HEARTBEAT_MS;
        if (due) {
          const controller = new AbortController();
          const timeout = setTimeout(() => controller.abort(), 6000);
          try {
            attemptOk = await pushEnvelopeToFirebase({
              envelope, hashes, ackRev: ackRevRef.current, webVersion: WEB_VERSION, signal: controller.signal,
            });
          } catch {
            attemptOk = false;
          } finally {
            clearTimeout(timeout);
          }
          if (attemptOk) {
            firebaseLastWriteRef.current = Date.now();
            setFirebaseLastAtMs(firebaseLastWriteRef.current);
          }
        }
      }

      if (attemptOk) {
        try {
          localStorage.setItem(SYNC_MIRROR_STORAGE_KEY, JSON.stringify(envelope));
        } catch {
          /* Optional storage */
        }
      }

      await recordSyncOutcome(attemptOk, nextRev);
      setSyncing(false);
      return attemptOk;
    },
    [currentLayeredState, enabled, endpointUrl, firebaseWritable, hasTarget, recordSyncOutcome, wifiStatus.espBootId, wifiStatus.mode, wifiStatus.staConnected, wsState]
  );

  const performSyncRef = useRef(performSync);
  performSyncRef.current = performSync;
  const pushedNetRef = useRef<boolean | null>(null);
  const lastEspRevRef = useRef<number | null>(null);

  useEffect(() => {
    if (HOSTED_REMOTE) return;
    if (wsState !== 'ready') { pushedNetRef.current = null; lastEspRevRef.current = null; return; }
    if (pushedNetRef.current === internetOnline) return;
    pushedNetRef.current = internetOnline;
    void requestCommand('sync.configure', { internetOk: internetOnline, intervalMs }, 2000).catch(() => {
      if (pushedNetRef.current === internetOnline) pushedNetRef.current = null;
    });
  }, [internetOnline, intervalMs, requestCommand, wsState]);

  useEffect(() => {
    if (HOSTED_REMOTE || wsState !== 'ready' || !espCloudSync) return;
    const prevRev = lastEspRevRef.current;
    if (prevRev === null) {
      lastEspRevRef.current = espCloudSync.configRev;
      if (espCloudSync.configRev > 0 || !internetOnlineRef.current) {
        setEnabledState(espCloudSync.enabled);
        if (!espCloudSync.enabled) setOnline(false);
      } else if (enabled && internetOnlineRef.current) {
        void requestCommand('sync.configure', { enabled: true, internetOk: true, intervalMs }, 2000).catch(() => undefined);
      }
      return;
    }
    if (espCloudSync.configRev !== prevRev) {
      lastEspRevRef.current = espCloudSync.configRev;
      setEnabledState(espCloudSync.enabled);
      failCountRef.current = 0;
      setFailCount(0);
      setFailModalOpen(false);
      if (!espCloudSync.enabled) setOnline(false);
      else if (internetOnlineRef.current) {
        window.setTimeout(() => { void performSyncRef.current(); }, 0);
      }
    }
  }, [enabled, espCloudSync, intervalMs, requestCommand, wsState]);

  // Ciclo periódico: 5 s si hay cambios pendientes (delta), 10 s de latido si no cambiaron capas; se detiene si acumula 5 fallas hasta acción del usuario.
  useEffect(() => {
    if (!enabled || failModalOpen) {
      if (!enabled) setOnline(false);
      return;
    }
    const timer = window.setInterval(() => {
      if (document.visibilityState === 'visible') {
        void performSync();
      }
    }, intervalMs);
    return () => window.clearInterval(timer);
  }, [enabled, failModalOpen, intervalMs, performSync]);

  const setEnabled = useCallback(
    async (next: boolean) => {
      if (next && !await checkInternet()) {
        notify.warning({
          id: 'afr2-sync-no-internet',
          title: 'Sin Internet',
          description: 'Conectate a Internet para activar la sincronización web.',
        });
        return;
      }
      setEnabledState(next);
      failCountRef.current = 0;
      setFailCount(0);
      setFailModalOpen(false);
      if (!next) setOnline(false);
      if (wsState === 'ready') {
        try {
          const res = await requestCommand('sync.configure', { enabled: next, internetOk: internetOnlineRef.current, intervalMs }, 2000);
          if (next && res.status !== 'completed') {
            setEnabledState(false);
            setOnline(false);
          } else if (next && res.status === 'completed' && (firebaseWriter === 'off' || firebaseWriter === 'error')) {
            await activateFirebase();
          }
        } catch {
          /* Ignore */
        }
      }
    },
    [activateFirebase, checkInternet, firebaseWriter, intervalMs, requestCommand, wsState]
  );

  const dismissFailureKeepSync = useCallback(async () => {
    // Opción por defecto: «Cerrar» — cierra el aviso sin apagar la función de sincronización.
    failCountRef.current = 0;
    setFailCount(0);
    setFailModalOpen(false);
    if (wsState === 'ready') {
      try {
        await requestCommand('sync.dismiss', { disable: false }, 2000);
      } catch {
        /* Ignore */
      }
    }
  }, [requestCommand, wsState]);

  const disableSyncFromFailure = useCallback(async () => {
    // Segunda opción: «Desactivar sincronización» — apaga la función y deja de intentar actualizar.
    setEnabledState(false);
    setOnline(false);
    failCountRef.current = 0;
    setFailCount(0);
    setFailModalOpen(false);
    if (wsState === 'ready') {
      try {
        await requestCommand('sync.dismiss', { disable: true }, 2000);
      } catch {
        /* Ignore */
      }
    }
    notify.info({
      id: 'afr2-cloud-sync-disabled',
      title: 'Sincronización online desactivada',
      description: 'La función quedó apagada y no volverá a intentar sincronizar hasta que la reactives.',
    });
  }, [requestCommand, wsState]);

  const triggerSyncNow = useCallback(
    async (options?: { forceSnapshot?: boolean }) => {
      if (!await checkInternet()) {
        notify.warning({
          id: 'afr2-sync-no-internet',
          title: 'Sin Internet',
          description: 'Conectate a Internet para sincronizar con la nube.',
        });
        return false;
      }
      const ok = await performSync({ forceSnapshot: options?.forceSnapshot ?? true });
      if (ok) {
        notify.info({
          id: 'afr2-cloud-sync-ok',
          title: 'Sincronización completada',
          description: 'Las capas de configuración y estado se actualizaron con la copia online.',
        });
      }
      return ok;
    },
    [checkInternet, performSync]
  );

  const syncSimulatorToWeb = useCallback(
    async (source: 'goto' | 'sequences' | '3d', data: unknown) => {
      const simLayer: CloudSyncSimulatorsLayer = {
        explicitPush: true,
        syncedAtMs: Date.now(),
        source,
        ...(source === 'goto' ? { goto: data } : source === 'sequences' ? { sequences: data } : { model3d: data }),
      };
      const ok = await performSync({ explicitSimulators: simLayer });
      if (wsState === 'ready') {
        try {
          await requestCommand('sync.push', { full: false, includeSimulators: true, attemptOk: ok, ackRev: ackRevRef.current }, 2000);
        } catch {
          /* Ignore */
        }
      }
      if (ok) {
        notify.info({
          id: `afr2-sim-sync-${source}`,
          title: 'Simulador sincronizado a la Web',
          description: `El estado del simulador (${source.toUpperCase()}) fue enviado explícitamente en la capa «simulators».`,
        });
      } else {
        notify.warning({
          id: `afr2-sim-sync-err-${source}`,
          title: 'No se pudo sincronizar a la Web',
          description: enabled
            ? `Intento fallido (${failCountRef.current}/${MAX_CONSECUTIVE_FAILS}). Verificá que el Wi-Fi tenga conexión a Internet.`
            : 'La sincronización online está desactivada.',
        });
      }
      return ok;
    },
    [enabled, performSync, requestCommand, wsState]
  );

  const value = useMemo<CloudSyncContextValue>(
    () => ({
      enabled,
      online,
      internetOnline,
      checkingInternet,
      checkInternet,
      hasTarget,
      syncing,
      failCount,
      failModalOpen,
      intervalMs,
      endpointUrl,
      rev,
      ackRev,
      lastSyncAtMs,
      lastPayloadBytes,
      lastChangedKeys,
      lastEnvelope,
      setEnabled,
      setIntervalMs: (ms: number) => setIntervalMsState(Math.min(10000, Math.max(5000, ms))),
      setEndpointUrl: setEndpointUrlState,
      firebaseKeySet: firebaseKey !== '',
      firebaseWriter,
      activateFirebase,
      deactivateFirebase,
      firebaseWritable,
      firebaseLastAtMs,
      hostedRemote: HOSTED_REMOTE,
      dismissFailureKeepSync,
      disableSyncFromFailure,
      triggerSyncNow,
      syncSimulatorToWeb,
    }),
    [
      enabled,
      online,
      internetOnline,
      checkingInternet,
      checkInternet,
      hasTarget,
      syncing,
      failCount,
      failModalOpen,
      intervalMs,
      endpointUrl,
      rev,
      ackRev,
      lastSyncAtMs,
      lastPayloadBytes,
      lastChangedKeys,
      lastEnvelope,
      setEnabled,
      firebaseKey,
      firebaseWriter,
      activateFirebase,
      deactivateFirebase,
      firebaseWritable,
      firebaseLastAtMs,
      dismissFailureKeepSync,
      disableSyncFromFailure,
      triggerSyncNow,
      syncSimulatorToWeb,
    ]
  );

  return (
    <CloudSyncContext.Provider value={value}>
      {children}
      <CloudSyncFaultModal />
    </CloudSyncContext.Provider>
  );
}

/** AFR2-20261117-01: estado legible de la sincronización, compartido por la pill y su modal. */
function syncStatusText(enabled: boolean, online: boolean, syncing: boolean, failCount: number, hasTarget = true, internetOnline = true) {
  if (!internetOnline) return { short: 'Sin Internet', mode: 'off' as const };
  if (HOSTED_REMOTE) return { short: 'Solo lectura', mode: 'idle' as const };
  if (!enabled) return { short: 'Apagada', mode: 'off' as const };
  if (!hasTarget) return { short: 'Sin destino', mode: 'off' as const };
  if (syncing) return { short: 'Sincronizando…', mode: online ? 'on' as const : 'idle' as const };
  if (online) return { short: 'Online', mode: 'on' as const };
  if (failCount > 0) return { short: `Falla ${failCount}/${MAX_CONSECUTIVE_FAILS}`, mode: 'warn' as const };
  return { short: 'En espera', mode: 'idle' as const };
}

/**
 * Pill de la barra superior, con el mismo material que Red y Batería. Abre el modal de sincronización
 * (lo administra la barra, como los demás modales de estado).
 */
export function CloudSyncPill({ onOpen, open = false }: { onOpen: (origin: ModalOrigin) => void; open?: boolean }) {
  const { enabled, online, syncing, failCount, hasTarget, internetOnline } = useCloudSync();
  const status = syncStatusText(enabled, online, syncing, failCount, hasTarget, internetOnline);
  return (
    <button type="button" className="afr2-network-pill afr2-sync-pill" data-sync={status.mode} data-busy={syncing ? 'true' : 'false'}
      aria-haspopup="dialog" aria-expanded={open} aria-label={`Sincronización web: ${status.short}. Abrir ajustes de sincronización`}
      title={`Sync web · ${status.short}`}
      onClick={(e) => onOpen({ element: e.currentTarget, keyboard: e.detail === 0 })}>
      <UiIcon name={!internetOnline || !enabled ? 'wifi-off' : 'refresh'} />
      <span>Sync web</span>
      <strong>{status.short}</strong>
    </button>
  );
}

const SYNC_INTERVALS_S = [5, 6, 7, 8, 9, 10] as const;

/** AFR2-20261117-10: tarjeta principal de sincronización web; activable solo con conexión real a Internet. */
function CloudSyncCard({ onClose }: { onClose?: () => void }) {
  const { wsState } = useConnection();
  const {
    enabled, online, syncing, failCount, hasTarget, internetOnline, checkingInternet, checkInternet,
    endpointUrl, firebaseWriter, setEnabled,
  } = useCloudSync();
  const status = syncStatusText(enabled, online, syncing, failCount, hasTarget, internetOnline);
  const subtitle = !internetOnline
    ? 'No se detectó salida a Internet. La sincronización no puede activarse sin Internet.'
    : HOSTED_REMOTE
      ? 'Copia online disponible para consultar el último estado del auto.'
      : wsState === 'ready'
        ? (firebaseWriter === 'ready' ? 'Auto conectado · sincronización y Copia en Firebase activas.' : 'Auto conectado · listo para publicar en la nube.')
        : endpointUrl
          ? 'Usando endpoint personalizado configurado.'
          : 'Conectá el auto o cargá un endpoint para publicar.';

  return (
    <section className="afr2-sync-card" data-state={internetOnline ? status.mode : 'offline'} aria-label="Tarjeta de sincronización">
      <div className="afr2-sync-card__top">
        <div className="afr2-sync-card__title">
          <UiIcon name={!internetOnline || !enabled ? 'wifi-off' : 'refresh'} size={18} />
          <div>
            <strong>Sincronización web</strong>
            <span>{subtitle}</span>
          </div>
        </div>
        <span className="afr2-sync-card__badge">{!internetOnline ? 'Sin Internet' : HOSTED_REMOTE ? 'Online' : status.short}</span>
      </div>
      <div className="afr2-sync-card__actions">
        {!internetOnline ? (
          <>
            {!HOSTED_REMOTE && enabled ? (
              <Button variant="secondary" size="sm" onClick={() => void setEnabled(false)}>
                Desactivar
              </Button>
            ) : (
              <Button variant="primary" size="sm" disabled>
                Sin Internet
              </Button>
            )}
            <Button variant="secondary" size="sm" loading={checkingInternet} onClick={() => void checkInternet()}>
              Comprobar Internet
            </Button>
          </>
        ) : HOSTED_REMOTE ? (
          <a className="afr2-button afr2-button--primary afr2-button--sm" href="#/remoto" onClick={() => onClose?.()}>
            Abrir vista remota
          </a>
        ) : (
          <>
            <Button variant={enabled ? 'secondary' : 'primary'} size="sm" onClick={() => void setEnabled(!enabled)}>
              {enabled ? 'Desactivar' : 'Activar'}
            </Button>
            <a className="afr2-button afr2-button--ghost afr2-button--sm" href="#/remoto" onClick={() => onClose?.()}>
              Vista remota
            </a>
          </>
        )}
      </div>
    </section>
  );
}

/** Contenido del modal de sincronización: estado, parámetros editables y acciones. */
export function CloudSyncDetails({ onClose }: { onClose?: () => void } = {}) {
  return HOSTED_REMOTE ? <HostedSyncDetails onClose={onClose} /> : <LocalSyncDetails onClose={onClose} />;
}

function LocalSyncDetails({ onClose }: { onClose?: () => void }) {
  const {
    enabled, online, hasTarget, internetOnline, syncing, failCount, intervalMs, endpointUrl, rev, ackRev,
    lastSyncAtMs, lastPayloadBytes, lastChangedKeys,
    setIntervalMs, setEndpointUrl, triggerSyncNow,
  } = useCloudSync();
  const status = syncStatusText(enabled, online, syncing, failCount, hasTarget, internetOnline);
  const [endpointDraft, setEndpointDraft] = useState(endpointUrl);
  useEffect(() => setEndpointDraft(endpointUrl), [endpointUrl]);
  const endpointValid = endpointDraft.trim() === '' || /^https?:\/\/\S+$/i.test(endpointDraft.trim());
  const commitEndpoint = () => { if (endpointValid) setEndpointUrl(endpointDraft.trim()); };
  const rows: [string, string][] = [
    ['Estado', status.short],
    ['Internet', internetOnline ? 'Disponible' : 'Sin Internet'],
    ['Fallas seguidas', `${failCount} de ${MAX_CONSECUTIVE_FAILS}`],
    ['Última sincronización', lastSyncAtMs ? new Date(lastSyncAtMs).toLocaleTimeString() : '—'],
    ['Último envío', lastPayloadBytes ? `${lastPayloadBytes} B` : '—'],
    ['Capas enviadas', lastChangedKeys.length ? lastChangedKeys.join(', ') : 'Sin cambios'],
    ['Revisión', `${rev} (confirmada ${ackRev})`],
  ];
  return (
    <div className="afr2-page-stack">
      <CloudSyncCard onClose={onClose} />
      <p className="afr2-help">Envía solo las capas que cambiaron (hash por capa). Los simuladores se envían únicamente con su botón «Sincronizar a web».{!hasTarget && enabled ? ' Ahora no hay destino: conectá el auto o cargá un endpoint.' : ''}</p>
      <fieldset className="afr2-sync-interval" disabled={!enabled}>
        <legend>Intervalo</legend>
        <div role="radiogroup" aria-label="Intervalo de sincronización">
          {SYNC_INTERVALS_S.map(sec => (
            <button key={sec} type="button" role="radio" aria-checked={intervalMs === sec * 1000}
              data-active={intervalMs === sec * 1000 ? 'true' : 'false'} onClick={() => setIntervalMs(sec * 1000)}>
              {sec} s
            </button>
          ))}
        </div>
      </fieldset>
      <label className="afr2-sync-endpoint">
        <span>Endpoint online (opcional)</span>
        <input className="afr2-glass-field" type="url" inputMode="url" placeholder="https://… (vacío = solo el auto)"
          value={endpointDraft} aria-invalid={!endpointValid}
          onChange={e => setEndpointDraft(e.target.value)} onBlur={commitEndpoint}
          onKeyDown={e => { if (e.key === 'Enter') commitEndpoint(); }} />
        <small>{endpointValid ? 'Recibe el sobre JSON por POST. Se guarda en este navegador.' : 'Tiene que empezar con http:// o https://.'}</small>
      </label>
      <FirebaseSection />
      <dl className="afr2-network-details">
        {rows.map(([key, value]) => <div key={key}><dt>{key}</dt><dd>{value}</dd></div>)}
      </dl>
      <div className="afr2-action-row">
        <Button variant="primary" loading={syncing} disabled={!enabled || !internetOnline} icon={<UiIcon name="refresh" size={16} />}
          onClick={() => void triggerSyncNow({ forceSnapshot: false })}>
          Sincronizar cambios ahora
        </Button>
        <Button variant="secondary" disabled={!enabled || !internetOnline || syncing} onClick={() => void triggerSyncNow({ forceSnapshot: true })}>
          Enviar todo
        </Button>
      </div>
    </div>
  );
}

async function copyText(text: string, title: string) {
  try {
    await navigator.clipboard.writeText(text);
    notify.info({ id: 'afr2-firebase-copy', title, description: 'Quedó copiado en el portapapeles.' });
  } catch {
    window.prompt('Copiá el texto:', text);
  }
}

const WRITER_TEXT: Record<FirebaseWriterState, string> = {
  'no-car': 'Conectá el auto para publicar su estado en Internet.',
  loading: 'Consultando la clave guardada en el auto…',
  off: 'Apagada: el estado del auto no se publica en Internet.',
  registering: 'Registrando este navegador en Firebase…',
  ready: 'Publicando el estado del auto.',
  claimed: 'La copia online tiene otra clave (por ejemplo, se borró la memoria del ESP). Borrá car/secret en la consola de Firebase y volvé a activarla.',
  error: 'No se pudo hablar con Firebase o con el ESP. Revisá que haya Internet y que el ESP tenga la versión 1.35.0.',
};

/**
 * AFR2-20261117-05: copia en Firebase. La clave de escritura la guarda el ESP; una compu se vincula para ver
 * el estado con el código de 6 caracteres que aparece en el OLED.
 */
function FirebaseSection() {
  const { firebaseWriter, firebaseKeySet, firebaseLastAtMs, internetOnline, activateFirebase, deactivateFirebase } = useCloudSync();
  const [busy, setBusy] = useState(false);
  const run = async (fn: () => Promise<void>) => { setBusy(true); try { await fn(); } finally { setBusy(false); } };
  return (
    <section className="afr2-sync-firebase" aria-label="Copia en Firebase">
      <div className="afr2-sync-toggle">
        <div>
          <strong>Copia en Firebase</strong>
          <span>
            {!internetOnline && firebaseWriter !== 'ready'
              ? 'Sin Internet: conectate a Internet para activar la copia online.'
              : WRITER_TEXT[firebaseWriter]}
            {firebaseWriter === 'ready' && firebaseLastAtMs ? ` Última escritura ${new Date(firebaseLastAtMs).toLocaleTimeString()}.` : ''}
          </span>
        </div>
        {(firebaseWriter === 'off' || firebaseWriter === 'error' || firebaseWriter === 'claimed') && (
          <Button variant="primary" size="sm" loading={busy} disabled={!internetOnline} onClick={() => void run(activateFirebase)}>
            {!internetOnline ? 'Sin Internet' : firebaseKeySet ? 'Reintentar' : 'Activar'}
          </Button>
        )}
        {firebaseWriter === 'ready' && (
          <Button variant="secondary" size="sm" loading={busy}
            onClick={() => { if (window.confirm('¿Apagar la copia online? El auto borra su clave y deja de publicar; las compus vinculadas dejan de recibir datos nuevos.')) void run(deactivateFirebase); }}>
            Apagar
          </Button>
        )}
      </div>
      {firebaseWriter === 'ready' && <PairingPanel />}
    </section>
  );
}

/** Vincular una compu: el código aparece en el OLED y se escribe en la vista remota. */
function PairingPanel() {
  const { requestCommand } = useConnection();
  const [pairing, setPairing] = useState<{ code: string; expiresAt: number } | null>(null);
  const [now, setNow] = useState(() => Date.now());
  const [error, setError] = useState<string | null>(null);
  const [viewers, setViewers] = useState<LinkedViewer[] | null>(null);
  const [busy, setBusy] = useState(false);

  const refreshViewers = useCallback(async () => {
    try { setViewers(await listViewers()); } catch { setViewers(null); }
  }, []);
  useEffect(() => { void refreshViewers(); }, [refreshViewers]);

  const showOnCar = useCallback(async (code: string, expiresAt: number) => {
    const ttlS = Math.max(1, Math.min(60, Math.round((expiresAt - Date.now()) / 1000)));
    try {
      const res = await requestCommand('sync.pair.show', { code, ttlS }, 2000);
      if (res.status !== 'completed') setError('El auto no pudo mostrar el código; usalo desde esta pantalla.');
    } catch {
      setError('El auto no pudo mostrar el código; usalo desde esta pantalla.');
    }
  }, [requestCommand]);

  const start = async () => {
    setBusy(true);
    setError(null);
    try {
      const next = await createPairing();
      setPairing(next);
      await showOnCar(next.code, next.expiresAt);
    } catch (e) {
      setError(e instanceof Error ? e.message : 'No se pudo crear el código.');
    } finally {
      setBusy(false);
    }
  };

  // Cuenta regresiva y detección del canje (el código desaparece de Firebase).
  useEffect(() => {
    if (!pairing) return;
    const id = window.setInterval(() => {
      const t = Date.now();
      setNow(t);
      if (t >= pairing.expiresAt) { void cancelPairing(pairing.code); setPairing(null); return; }
      if (Math.floor(t / 1000) % 3 === 0) {
        void pairingPending(pairing.code).then(pending => {
          if (pending) return;
          setPairing(null);
          notify.info({ id: 'afr2-pair-ok', title: 'Compu vinculada', description: 'Ya puede ver el estado del auto desde la vista remota.' });
          void refreshViewers();
        }).catch(() => undefined);
      }
    }, 1000);
    return () => window.clearInterval(id);
  }, [pairing, refreshViewers]);

  const left = pairing ? Math.max(0, Math.round((pairing.expiresAt - now) / 1000)) : 0;
  return (
    <div className="afr2-sync-pair">
      {pairing ? (
        <div className="afr2-sync-pair__code" role="status" aria-live="polite">
          <span>Código en la pantalla del auto</span>
          <strong>{pairing.code}</strong>
          <small>Escribilo en {REMOTE_VIEW_URL.replace('https://', '')} · vence en {Math.floor(left / 60)}:{String(left % 60).padStart(2, '0')}</small>
          <div className="afr2-action-row">
            <Button variant="secondary" size="sm" onClick={() => void showOnCar(pairing.code, pairing.expiresAt)}>Mostrar de nuevo en el auto</Button>
            <Button variant="secondary" size="sm" onClick={() => { void cancelPairing(pairing.code); setPairing(null); }}>Cancelar</Button>
          </div>
        </div>
      ) : (
        <div className="afr2-action-row">
          <Button variant="primary" size="sm" loading={busy} icon={<UiIcon name="screen" size={16} />} onClick={() => void start()}>
            Vincular una compu
          </Button>
          <Button variant="secondary" size="sm" icon={<UiIcon name="external" size={16} />}
            onClick={() => window.open(REMOTE_VIEW_URL, '_blank', 'noopener')}>
            Abrir vista remota
          </Button>
          <Button variant="secondary" size="sm" onClick={() => void copyText(REMOTE_VIEW_URL, 'Enlace copiado')}>Copiar enlace</Button>
        </div>
      )}
      {error && <p className="afr2-help" role="alert">{error}</p>}
      {viewers && viewers.length > 0 && (
        <div className="afr2-sync-viewers">
          <span>Compus vinculadas (solo ven el estado)</span>
          <ul>
            {viewers.map(v => (
              <li key={v.uid}>
                <span>{v.label}<small>{v.at ? ` · desde ${new Date(v.at).toLocaleDateString()}` : ''}</small></span>
                <Button variant="secondary" size="sm" icon={<UiIcon name="trash" size={16} />}
                  onClick={() => void revokeViewer(v.uid).then(() => refreshViewers())}>Quitar</Button>
              </li>
            ))}
          </ul>
        </div>
      )}
    </div>
  );
}

/** Modal de sincronización en la copia de Firebase Hosting: no escribe; lleva a la vista remota. */
function HostedSyncDetails({ onClose }: { onClose?: () => void }) {
  return (
    <div className="afr2-page-stack">
      <CloudSyncCard onClose={onClose} />
      <p className="afr2-help">
        Esta es la copia online de la Web, sin auto conectado. No envía datos: muestra el último estado que publicó
        el auto. Para verlo, vinculá esta compu con el código que aparece en la pantalla del auto.
      </p>
    </div>
  );
}

/**
 * Notificación persistente tras 5 intentos fallidos consecutivos de sincronización.
 * La opción seleccionada por defecto es «Cerrar» para que el usuario no apague la función sin querer.
 */
function CloudSyncFaultModal() {
  const { failModalOpen, failCount, dismissFailureKeepSync, disableSyncFromFailure } = useCloudSync();
  const closeBtnRef = useRef<HTMLButtonElement | null>(null);

  useEffect(() => {
    if (failModalOpen) {
      const id = window.setTimeout(() => closeBtnRef.current?.focus(), 30);
      return () => window.clearTimeout(id);
    }
  }, [failModalOpen]);

  if (!failModalOpen) return null;

  return (
    <FloatingModal
      open={failModalOpen}
      origin={null}
      onClose={() => void dismissFailureKeepSync()}
      title="Fallo de sincronización"
      description={`No se pudo conectar tras ${failCount} intentos consecutivos de sincronización online.`}
    >
      <div className="space-y-4">
        <p className="text-sm text-[var(--text)]">
          El auto intentó sincronizar las capas de configuración 5 veces sin respuesta de red. Elegí{' '}
          <strong>Cerrar</strong> (opción por defecto) para mantener la función activa y volver a intentar, o{' '}
          <strong>Desactivar sincronización</strong> para apagar la función y dejar de intentar actualizar.
        </p>
        <div className="flex flex-wrap items-center justify-end gap-3 pt-2">
          <button
            ref={closeBtnRef}
            type="button"
            autoFocus
            onClick={() => void dismissFailureKeepSync()}
            className="afr2-button afr2-button--primary afr2-button--md"
          >
            Cerrar (mantener activa)
          </button>
          <Button variant="secondary" onClick={() => void disableSyncFromFailure()}>
            Desactivar sincronización
          </Button>
        </div>
      </div>
    </FloatingModal>
  );
}

/**
 * Botón explícito «Sincronizar a web» para los simuladores (GOTO, Secuencias y 3D).
 * Las interacciones dentro del simulador nunca se envían solas en el ciclo periódico.
 */
export function SyncSimulatorButton({
  simulator,
  data,
  disabled = false,
}: {
  simulator: 'goto' | 'sequences' | '3d';
  data: unknown;
  disabled?: boolean;
}) {
  const { syncSimulatorToWeb, syncing } = useCloudSync();
  return (
    <Button
      type="button"
      variant="secondary"
      size="sm"
      disabled={disabled || syncing}
      icon={<UiIcon name="refresh" size={16} />}
      onClick={() => void syncSimulatorToWeb(simulator, data)}
      title="Envía explícitamente el estado actual de este simulador a la copia Web (no se envía automáticamente)."
    >
      Sincronizar a web
    </Button>
  );
}
