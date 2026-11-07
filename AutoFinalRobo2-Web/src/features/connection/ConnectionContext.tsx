/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import type { HeadReading, ScreenStatus } from '../../protocol/sensorHead';
import { parseBattery, type BatteryReading } from '../../protocol/battery';
import { parseRobotState, type RobotStateSnapshot } from '../../protocol/robotState';
import React, { createContext, useCallback, useContext, useEffect, useMemo, useRef, useState } from 'react';
import { Afr2Client, Afr2CommandError, type Afr2ClientStateSnapshot } from '../../protocol/afr2Client';
import {
  type Afr2CommandName,
  type Afr2WsResponse,
  EMPTY_SENSORS_SNAPSHOT,
  EMPTY_WIFI_SNAPSHOT,
  type SensorsSnapshot,
  type WifiNetworkCandidate,
  type WifiStatusSnapshot,
} from '../../protocol/wsEnvelope';
import { notify } from '../../components/notifications/notify';

export interface StopStatusRecord {
  noticeId: number;
  lastIssuedAtMs: number | null;
  lastConfirmedAtMs: number | null;
  status: 'idle' | 'sending' | 'confirmed' | 'unconfirmed-offline';
  detail: string | null;
}

interface ConnectionContextValue extends Afr2ClientStateSnapshot {
  battery: BatteryReading | null;
  refreshBattery: (measure?: boolean) => Promise<void>;
  systemInfo: Record<string, unknown> | null;
  systemInfoUpdatedAtMs: number | null;
  bootReports: { device: string; bootId: number; receivedAtMs: number; observation: string }[];
  refreshSystemInfo: () => Promise<void>;
  stopAvailable: boolean;
  webOperationActive: boolean;
  hasWebOperation: () => boolean;
  setOperationActivity: (name: string, active: boolean) => void;
  wifiDraft: {jobId:number;mode:'STA'|'AP'|'AP_STA';ssid:string} | null;
  robotState: RobotStateSnapshot | null;
  sensors: SensorsSnapshot;
  headReading: HeadReading | null;
  screenStatus: ScreenStatus | null;
  registerScreenViewer: () => () => void;
  registerHeadViewer: () => () => void;
  screenSubscribed: boolean;
  wifiStatus: WifiStatusSnapshot;
  wifiNetworks: WifiNetworkCandidate[];
  wifiScanState: 'idle' | 'scanning' | 'completed' | 'failed';
  wifiScanError: string | null;
  wifiScanTruncated: boolean;
  stopRecord: StopStatusRecord;
  stopNoticeVisible: boolean;
  dismissStopNotice: () => void;
  connectToEndpoint: (url?: string) => void;
  disconnectEndpoint: () => void;
  setEndpointUrl: (url: string) => void;
  triggerEmergencyStop: (reason?: string) => Promise<boolean>;
  sendDriveCommand: (vxMmS: number, vyMmS: number, wzMradS: number) => Promise<void>;
  sendPwmCommand: (forwardPct:number,strafePct:number,turnPct:number)=>Promise<void>;
  requestCommand: <TPayload = Record<string, unknown>>(
    cmd: Afr2CommandName, payload: Record<string, unknown>, timeoutMs?: number
  ) => Promise<Afr2WsResponse<TPayload>>;
  executeCommandWithToast: <TPayload = Record<string, unknown>>(
    cmd: Afr2CommandName,
    payload: Record<string, unknown>,
    labels: {
      loadingTitle: string;
      successTitle: string;
      acceptedTitle?: string;
    },
    timeoutMs?: number
  ) => Promise<Afr2WsResponse<TPayload> | null>;
  refreshSensors: () => Promise<void>;
  refreshWifiStatus: () => Promise<void>;
  scanWifiNetworks: () => Promise<void>;
}

const ConnectionContext = createContext<ConnectionContextValue | null>(null);

export function ConnectionProvider({ children }: { children: React.ReactNode }) {
  const clientRef = useRef<Afr2Client | null>(null);
  if (!clientRef.current) {
    clientRef.current = new Afr2Client();
  }
  const client = clientRef.current;

  const [snapshot, setSnapshot] = useState<Afr2ClientStateSnapshot>(() => client.getState());
  const [systemInfo, setSystemInfo] = useState<Record<string, unknown> | null>(null);
  const [systemInfoUpdatedAtMs, setSystemInfoUpdatedAtMs] = useState<number | null>(null);
  const [bootReports, setBootReports] = useState<ConnectionContextValue['bootReports']>([]);
  const observedBoots = useRef<Record<string, number>>({});
  const [bootRevision, setBootRevision] = useState(0);
  const [activities, setActivities] = useState<Record<string, boolean>>({});
  const activityRef = useRef<Record<string, boolean>>({});
  const motionEpoch = useRef(0);
  const setOperationActivity = useCallback((name: string, active: boolean) => {
    activityRef.current = { ...activityRef.current, [name]: active };
    setActivities(prev => prev[name] === active ? prev : { ...prev, [name]: active });
  }, []);
  const hasWebOperation = useCallback(() => ['manual', 'command', 'commandPending', 'configuration', 'sequence'].some(name => activityRef.current[name]), []);
  useEffect(() => { client.connect(`${window.location.protocol === 'https:' ? 'wss:' : 'ws:'}//${window.location.host}/ws`); return () => client.disconnect(); }, [client]);
  const [headReading,setHeadReading]=useState<HeadReading|null>(null);
  const [battery, setBattery] = useState<BatteryReading | null>(null);
  const batteryGeneration = useRef(0);
  const [screenStatus,setScreenStatus]=useState<ScreenStatus|null>(null);
  const [screenViewers, setScreenViewers] = useState(0);
  const [headViewers, setHeadViewers] = useState(0);
  const registerHeadViewer = useCallback(() => {
    setHeadViewers(n => n + 1); let released = false;
    return () => { if(!released){released=true;setHeadViewers(n=>Math.max(0,n-1));} };
  }, []);
  const [screenSubscribed, setScreenSubscribed] = useState(false);
  const registerScreenViewer = useCallback(() => {
    setScreenViewers(n => n + 1); let released = false;
    return () => { if (!released) { released = true; setScreenViewers(n => Math.max(0, n - 1)); } };
  }, []);
  const requestTail=useRef<Promise<unknown>>(Promise.resolve());
  const requestEpoch=useRef(0);
  const [robotState, setRobotState] = useState<RobotStateSnapshot | null>(null);
  const [sensors, setSensors] = useState<SensorsSnapshot>(EMPTY_SENSORS_SNAPSHOT);
  const [wifiStatus, setWifiStatus] = useState<WifiStatusSnapshot>(EMPTY_WIFI_SNAPSHOT);
  const [wifiNetworks, setWifiNetworks] = useState<WifiNetworkCandidate[]>([]);
  const [wifiDraft,setWifiDraft] = useState<ConnectionContextValue['wifiDraft']>(null);
  useEffect(() => {if(snapshot.megaLinkState !== 'ready')setSensors(EMPTY_SENSORS_SNAPSHOT);},[snapshot.megaLinkState]);
  const connectionRef = useRef(snapshot);connectionRef.current=snapshot;
  const stopNoticeCounterRef = useRef(0);
  const [stopRecord, setStopRecord] = useState<StopStatusRecord>({
    noticeId: 0,
    lastIssuedAtMs: null,
    lastConfirmedAtMs: null,
    status: 'idle',
    detail: null,
  });
  const [dismissedStopNoticeId, setDismissedStopNoticeId] = useState<number | null>(null);

  const [wifiScanState, setWifiScanState] = useState<'idle' | 'scanning' | 'completed' | 'failed'>('idle');
  const [wifiScanError, setWifiScanError] = useState<string | null>(null);
  const [wifiScanTruncated, setWifiScanTruncated] = useState(false);
  const wifiScanPending = useRef(false);
  const expectedScanId = useRef<number | null>(null);
  const wifiScanTimer = useRef<ReturnType<typeof setTimeout> | null>(null);
  const wifiScanGeneration = useRef(0);
  const prevWsStateRef = useRef(snapshot.wsState);

  useEffect(() => {
    const unsubState = client.subscribeState((next) => {
      const prev = prevWsStateRef.current;
      prevWsStateRef.current = next.wsState;
      setSnapshot(next);
      if (prev === 'ready' && next.wsState !== 'ready') {
        ++requestEpoch.current; expectedScanId.current=null;
        setWifiDraft(null);setSensors(EMPTY_SENSORS_SNAPSHOT);
        wifiScanGeneration.current++;
        wifiScanPending.current = false;
        if (wifiScanTimer.current) clearTimeout(wifiScanTimer.current);
        setWifiScanState('idle'); setWifiScanError(null); setWifiNetworks([]);
        setWifiScanTruncated(false); setWifiStatus(EMPTY_WIFI_SNAPSHOT);
      }

      if (prev === 'ready' && next.wsState === 'disconnected') {
        notify.connectionLost(
          next.lastError ?? 'Se perdió la sesión con el broker ESP8266. Operaciones en vuelo canceladas.'
        );
      } else if (prev !== 'ready' && next.wsState === 'ready') {
        notify.connectionRecovered(
          `Sesión activa con ${next.endpointUrl} (enlace Mega: ${next.megaLinkState}).`
        );
      }
    });

    const unsubEvents = client.subscribeEvents((evt) => {
      if (evt.event === 'system.info' && evt.payload) { setSystemInfo(evt.payload); setSystemInfoUpdatedAtMs(Date.now()); }
      if (evt.event === 'system.boot' && (evt.payload?.device === 'esp' || evt.payload?.device === 'atmega') && typeof evt.payload.bootId === 'number') {
        const device = evt.payload.device, id = evt.payload.bootId, observation = String(evt.payload.observation ?? 'identity-snapshot');
        const previous = observedBoots.current[device];observedBoots.current[device] = id;
        setBootReports(prev => [{ device, bootId:id, receivedAtMs:Date.now(), observation }, ...prev.filter(report => report.device !== device)].slice(0,2));
        if (observation === 'uart-boot-event' || (previous !== undefined && previous !== id)) notify.info({ id:`afr2-boot-${device}`, title:`${device === 'esp' ? 'ESP' : 'ATmega'} inició`, description:`Arranque ${id}. El movimiento permanece detenido después del arranque.` });
        if (previous !== id) { setBootRevision(v => v + 1);setSystemInfo(null);
          if(device === 'esp'){ setWifiDraft(null);setWifiNetworks([]);setWifiScanState('idle'); }
        }
      }
      if(evt.event === 'system.status') {
        const h=evt.payload?.sensorHead as {known?:boolean;running?:boolean;enabled?:boolean}|undefined;
        if(h?.known){setOperationActivity('radar',h.running===true);setOperationActivity('servo',h.enabled===true);}
        const draft = evt.payload?.wifiDraft as Record<string, unknown> | undefined;
        if(draft?.pending === true && Number.isInteger(draft.jobId) && (draft.mode === 'STA' || draft.mode === 'AP' || draft.mode === 'AP_STA')) {
          setWifiDraft(prev => prev?.jobId === draft.jobId ? prev : {jobId:draft.jobId as number,mode:draft.mode as 'STA'|'AP'|'AP_STA',ssid:typeof draft.ssid === 'string'?draft.ssid:''});
        } else setWifiDraft(null);
      }
      if (evt.event === 'robot.sensors.snapshot' && evt.payload) {
        const p = evt.payload as Partial<SensorsSnapshot>;
        setSensors((prev) => ({
          line: p.line ?? prev.line,
          ultrasonic: p.ultrasonic ?? prev.ultrasonic,
          encoders: p.encoders ?? prev.encoders,
          receivedAtMs: Date.now(),
        }));
      } else if (evt.event === 'wifi.scan.result' &&
        (evt.payload?.status === 'completed' || evt.payload?.status === 'failed') &&
        Array.isArray(evt.payload.networks)) {
        if(!wifiScanPending.current || (expectedScanId.current!==null && evt.payload.scanId!==expectedScanId.current))return;
        if (wifiScanTimer.current) clearTimeout(wifiScanTimer.current);
        wifiScanPending.current = false;
        const failed = evt.payload.status === 'failed';
        setWifiNetworks(failed ? [] : (evt.payload.networks as WifiNetworkCandidate[]).filter(
          (net) => typeof net?.ssid === 'string' && typeof net.rssiDbm === 'number' &&
            typeof net.channel === 'number' && typeof net.secure === 'boolean'
        ));
        setWifiScanState(failed ? 'failed' : 'completed');
        setWifiScanError(failed ? 'La ESP no pudo completar el escaneo. Volvé a intentarlo.' : null);
        setWifiScanTruncated(!failed && evt.payload.truncated === true);
      }
    });

    return () => {
      unsubState();
      unsubEvents();
      if (wifiScanTimer.current) clearTimeout(wifiScanTimer.current);
      wifiScanGeneration.current++; wifiScanPending.current = false;
    };
  }, [client,setOperationActivity]);

  // Radio status is an ESP-only service: it remains available without Mega.
  useEffect(() => {
    if (snapshot.wsState !== 'ready') return;
    let cancelled = false, pending = false;
    const read = async () => {
      if (cancelled || pending || document.visibilityState !== 'visible') return;
      pending = true;
      try {
        const response = await client.sendCommand<WifiStatusSnapshot>('wifi.info', {}, 2500);
        if (!cancelled && response.status === 'completed') setWifiStatus({...response.payload,receivedAtMs:Date.now()});
      } catch { if (!cancelled) setWifiStatus(EMPTY_WIFI_SNAPSHOT); }
      finally { pending = false; }
    };
    void read();
    const timer = setInterval(() => void read(), 3000);
    document.addEventListener('visibilitychange', read);
    return () => { cancelled = true; clearInterval(timer); document.removeEventListener('visibilitychange', read); };
  }, [client, snapshot.wsState, snapshot.endpointUrl]);

  // Shared robot.state reader for mode and map. No periodic toasts, no
  // overlapping queries, and no second request stream from page components.
  useEffect(() => {
    let cancelled = false, pending = false, timer = 0, visibilityRevision = 0;
    setRobotState(null);
    if (snapshot.wsState !== 'ready' || snapshot.megaLinkState !== 'ready') return;
    const read = async () => {
      if (cancelled || pending || document.visibilityState !== 'visible') return;
      pending = true;
      const revision = visibilityRevision;
      const commandEpoch = motionEpoch.current;
      try {
        const response = await client.sendCommand<Record<string, unknown>>('robot.state', {}, 2500);
        if (!cancelled && revision === visibilityRevision && commandEpoch === motionEpoch.current && document.visibilityState === 'visible') setRobotState(response.status === 'completed' ? parseRobotState(response.payload) : null);
      } catch { if (!cancelled) setRobotState(null); }
      finally {
        pending = false;
        if (!cancelled && document.visibilityState === 'visible') { clearTimeout(timer); timer = window.setTimeout(read, 1000); }
      }
    };
    const visibility = () => {
      visibilityRevision++; clearTimeout(timer); setRobotState(null);
      if (document.visibilityState === 'visible') void read();
    };
    void read(); document.addEventListener('visibilitychange', visibility);
    return () => { cancelled = true; clearTimeout(timer); document.removeEventListener('visibilitychange', visibility); };
  }, [client, snapshot.wsState, snapshot.megaLinkState, snapshot.endpointUrl]);

  const connectToEndpoint = useCallback(
    (url?: string) => {
      client.connect(url);
    },
    [client]
  );

  const disconnectEndpoint = useCallback(() => {
    client.disconnect();
  }, [client]);

  const setEndpointUrl = useCallback(
    (url: string) => {
      client.setEndpointUrl(url);
    },
    [client]
  );

  const dismissStopNotice = useCallback(() => {
    setDismissedStopNoticeId(stopRecord.noticeId);
  }, [stopRecord.noticeId]);

  const stopNoticeVisible =
    stopRecord.status !== 'idle' &&
    Boolean(stopRecord.detail) &&
    stopRecord.noticeId !== dismissedStopNoticeId;

  const triggerEmergencyStop = useCallback(
    async (reason = 'user-stop'): Promise<boolean> => {
      ++requestEpoch.current;
      const issuedAt = Date.now();
      const sendingNoticeId = ++stopNoticeCounterRef.current;
      setStopRecord((prev) => ({
        ...prev,
        noticeId: sendingNoticeId,
        lastIssuedAtMs: issuedAt,
        status: 'sending',
        detail: 'Enviando orden prioritaria STOP...',
      }));

      if (activityRef.current.simulation && !Object.entries(activityRef.current).some(([key, active]) => key !== 'simulation' && active) && connectionRef.current.wsState !== 'ready') {
        activityRef.current={};setActivities({});setStopRecord({ noticeId: sendingNoticeId, lastIssuedAtMs: issuedAt, lastConfirmedAtMs: null, status: 'idle', detail: null });return true;
      }
      try {
        const res = await client.sendPriorityStop(reason);
        // AFR2-20261023-11: acceptance alone never confirms an actuator shutdown.
        if (res.status !== 'completed' || res.payload?.stopped !== true) {
          throw new Error('STOP aceptado sin confirmación autoritativa de parada.');
        }
        activityRef.current={};setActivities({});
        const confirmedAt = Date.now();
        const confirmedNoticeId = ++stopNoticeCounterRef.current;
        setStopRecord({
          noticeId: confirmedNoticeId,
          lastIssuedAtMs: issuedAt,
          lastConfirmedAtMs: confirmedAt,
          status: 'confirmed',
          detail: 'Parada confirmada por el controlador.',
        });
        notify.warning({
          id: 'afr2-stop-status',
          title: 'STOP confirmado por el productor',
          description: `Parada autoritativa registrada (${res.status}).`,
        });
        return true;
      } catch (err) {
        const message =
          err instanceof Error
            ? err.message
            : 'Sin enlace activo para confirmar parada en el ATmega2560.';
        const failedNoticeId = ++stopNoticeCounterRef.current;
        setStopRecord((prev) => ({
          ...prev,
          noticeId: failedNoticeId,
          lastIssuedAtMs: issuedAt,
          status: 'unconfirmed-offline',
          detail: `Consignas locales anuladas. Sin confirmación física: ${message}`,
        }));
        notify.error({
          id: 'afr2-stop-status',
          title: 'STOP emitido localmente (sin confirmación remota)',
          description: message,
        });
        return false;
      }
    },
    [client]
  );

  // Polling/remote UI reports inline; it must not create one toast per snapshot.
  const requestCommand = useCallback(
    <TPayload = Record<string, unknown>,>(cmd: Afr2CommandName, payload: Record<string, unknown>, timeoutMs = 2500): Promise<Afr2WsResponse<TPayload>> =>
      {
        if(cmd==='robot.stop'){++requestEpoch.current;return client.sendCommand<TPayload>(cmd,payload,timeoutMs);}
        const epoch=requestEpoch.current;
        const operation=requestTail.current.then(()=>{if(epoch!==requestEpoch.current)throw new Afr2CommandError('SUPERSEDED','Petición anterior a STOP o cambio de sesión.',false,'web');return client.sendCommand<TPayload>(cmd,payload,timeoutMs);});
        requestTail.current=operation.catch(()=>undefined);return operation;
      },
    [client]
  );

  // AFR2-20261029-26: cached battery reads never start an unwired ADC input.
  const refreshBattery = useCallback(async (measure = false): Promise<void> => {
    const generation = batteryGeneration.current;
    const current = connectionRef.current;
    if (current.wsState !== 'ready' || current.megaLinkState !== 'ready' || !current.capabilities.battery_voltage?.implemented) {
      throw new Error('La medición de batería requiere el controlador con EX01 v9 conectado.');
    }
    const requestedAt = Date.now();
    const response = await requestCommand(measure ? 'system.battery.measure' : 'system.battery.get', {}, 2500);
    if (generation !== batteryGeneration.current) return;
    const reading = response.status === 'completed' ? parseBattery(response.payload) : null;
    if (!reading) throw new Error('La respuesta de batería no contiene una medición reconocida.');
    // Conservative age includes the request round trip and local queue delay.
    setBattery({ ...reading, ageMs: reading.ageMs === null ? null : reading.ageMs + Math.max(0, Date.now() - requestedAt) });
  }, [requestCommand]);
  useEffect(() => {
    ++batteryGeneration.current; setBattery(null);
    let cancelled = false, pending = false;
    if (snapshot.wsState !== 'ready' || snapshot.megaLinkState !== 'ready' || !snapshot.capabilities.battery_voltage?.implemented) return;
    const read = async () => {
      if (cancelled || pending || document.visibilityState !== 'visible') return;
      pending = true;
      try { await refreshBattery(); } catch { /* Age expires; polling produces no toasts. */ }
      finally { pending = false; }
    };
    void read();
    const timer = window.setInterval(() => void read(), 1000);
    document.addEventListener('visibilitychange', read);
    return () => { cancelled = true; ++batteryGeneration.current; clearInterval(timer); document.removeEventListener('visibilitychange', read); };
  }, [snapshot.wsState, snapshot.megaLinkState, snapshot.endpointUrl, snapshot.capabilities.battery_voltage?.implemented, bootRevision, refreshBattery]);

  // AFR2-20261029-19: no screen traffic without a visible viewer.
  useEffect(() => {
    let cancelled = false, busy = false;
    setScreenSubscribed(false); setScreenStatus(null);
    if (snapshot.wsState !== 'ready') return;
    if (!screenViewers) return;
    const update = async () => {
      if (busy || cancelled) return;
      busy = true;
      const enabled = screenViewers > 0 && document.visibilityState === 'visible' && snapshot.megaLinkState === 'ready';
      try {
        const r = await requestCommand('robot.screen.subscribe', { enabled }, 2000);
        if (cancelled) return;
        setScreenSubscribed(enabled && r.status === 'completed');
        if (enabled && r.status === 'completed') {
          const status = await requestCommand<ScreenStatus>('robot.screen.status', {}, 2000);
          if (!cancelled && status.status === 'completed') setScreenStatus(status.payload);
        } else setScreenStatus(null);
      } catch { if (!cancelled) { setScreenSubscribed(false); setScreenStatus(null); } }
      finally { busy = false; }
    };
    void update(); const timer = window.setInterval(() => void update(), 1000);
    const visible = () => { setScreenSubscribed(false); void update(); };
    document.addEventListener('visibilitychange', visible);
    return () => {
      cancelled = true; clearInterval(timer); document.removeEventListener('visibilitychange', visible);
      void requestCommand('robot.screen.subscribe', { enabled: false }, 2000).catch(() => undefined);
    };
  }, [screenViewers, snapshot.wsState, snapshot.megaLinkState, bootRevision, requestCommand]);

  // One shared head reader feeds every animated robot instance. Each response owns its age.
  useEffect(()=>{
    if(!headViewers || snapshot.wsState!=='ready' || snapshot.megaLinkState!=='ready' || !snapshot.capabilities.sensor_head?.implemented){setHeadReading(null);return;}
    let cancelled=false,headPending=false;
    const head=async()=>{
      if(cancelled || headPending || document.visibilityState!=='visible')return;headPending=true;
      try{const r=await requestCommand<Omit<HeadReading,'receivedAtMs'>>('robot.ultrasonic.get',{},2000);
        if(!cancelled && r.status==='completed' && Number.isInteger(r.payload.angleDeg) && r.payload.sample){
          setHeadReading({...r.payload,receivedAtMs:Date.now()});setOperationActivity('radar',r.payload.running);setOperationActivity('servo',r.payload.enabled);
        }
      }catch{if(!cancelled)setHeadReading(null);}finally{headPending=false;}
    };
    void head();const a=window.setInterval(()=>void head(),100);
    const visible=()=>{void head();};document.addEventListener('visibilitychange',visible);
    return()=>{cancelled=true;clearInterval(a);document.removeEventListener('visibilitychange',visible);};
  },[headViewers,snapshot.wsState,snapshot.megaLinkState,snapshot.endpointUrl,snapshot.capabilities.sensor_head?.implemented,bootRevision,requestCommand,setOperationActivity]);

  useEffect(()=>{const fence=()=>{++requestEpoch.current;};const visibility=()=>{if(document.visibilityState!=='visible')fence();};window.addEventListener('blur',fence);document.addEventListener('visibilitychange',visibility);return()=>{window.removeEventListener('blur',fence);document.removeEventListener('visibilitychange',visibility);};},[]);

  const refreshSystemInfo = useCallback(async () => {
    try { const response = await client.sendCommand<Record<string, unknown>>('system.info', {}, 4000); if (response.status === 'completed' && response.payload) { setSystemInfo(response.payload); setSystemInfoUpdatedAtMs(Date.now()); } }
    catch { setSystemInfo(null); }
  }, [client]);
  useEffect(() => {
    if (snapshot.wsState !== 'ready') { setSystemInfo(null); return; }
    void refreshSystemInfo();
  }, [snapshot.wsState, snapshot.megaLinkState, bootRevision, refreshSystemInfo]);
  const webOperationActive = ['manual', 'command', 'commandPending', 'configuration', 'sequence'].some(name => activities[name]) || (robotState?.origin === 'web' && robotState.stopped === false);
  const stopAvailable = Object.values(activities).some(Boolean) || robotState?.stopped === false || stopRecord.status === 'sending' || stopRecord.status === 'unconfirmed-offline';

  const executeCommandWithToast = useCallback(
    async <TPayload = Record<string, unknown>,>(
      cmd: Afr2CommandName,
      payload: Record<string, unknown>,
      labels: {
        loadingTitle: string;
        successTitle: string;
        acceptedTitle?: string;
      },
      timeoutMs = 3000
    ): Promise<Afr2WsResponse<TPayload> | null> => {
      const toastId = `cmd-${cmd}`;
      notify.loading({
        id: toastId,
        title: labels.loadingTitle,
        description: `Comando ${cmd} enviado al broker...`,
      });

      const motionCommand = ['robot.mode', 'robot.test.wheel', 'robot.goto.start'].includes(cmd);
      if (motionCommand) {
        ++motionEpoch.current;
        setOperationActivity('command', true);
        setOperationActivity('commandPending', true);
      }
      try {
        const res = await client.sendCommand<TPayload>(cmd, payload, timeoutMs);
        if(cmd === 'robot.encoders' && res.status === 'completed') {
          setSensors(prev=>{
            const now=Date.now(),delta=prev.receivedAtMs===null?0:now-prev.receivedAtMs;
            return {...prev,line:{...prev.line,ageMs:prev.line.ageMs===null?null:prev.line.ageMs+delta},ultrasonic:{...prev.ultrasonic,ageMs:prev.ultrasonic.ageMs===null?null:prev.ultrasonic.ageMs+delta},encoders:res.payload as SensorsSnapshot['encoders'],receivedAtMs:now};
          });
        }
        if (res.status === 'accepted') {
          notify.info({
            id: toastId,
            title: labels.acceptedTitle ?? 'Petición aceptada; esperando resultado',
            description: 'Petición aceptada por el broker; pendiente de resultado final.',
          });
        } else {
          notify.success({
            id: toastId,
            title: labels.successTitle,
            description: `Confirmado (${cmd}).`,
          });
        }
        return res;
      } catch (err) {
        const msg =
          err instanceof Afr2CommandError
            ? `[${err.code}] ${err.message}`
            : err instanceof Error
              ? err.message
              : 'Fallo al ejecutar el comando.';
        notify.error({
          id: toastId,
          title: `No se pudo completar ${cmd}`,
          description: msg,
        });
        return null;
      } finally {
        if (motionCommand) {
          // Only a state read started after this outcome may hide STOP.
          ++motionEpoch.current;
          setOperationActivity('command', true);
          setOperationActivity('commandPending', false);
        }
      }
    },
    [client]
  );

  useEffect(() => { if (robotState?.stopped === true) setOperationActivity('command', false); }, [robotState?.receivedAtMs, robotState?.stopped, setOperationActivity]);

  const sendDriveCommand = useCallback(
    async (vxMmS: number, vyMmS: number, wzMradS: number): Promise<void> => {
      // A released/queued target is not authoritative proof of stopped outputs.
      ++motionEpoch.current;
      setOperationActivity('command', true);
      try { await client.sendDrive(vxMmS, vyMmS, wzMradS, 400); }
      finally { ++motionEpoch.current; }
    },
    [client, setOperationActivity]
  );

  const sendPwmCommand = useCallback(async(forwardPct:number,strafePct:number,turnPct:number):Promise<void>=>{
    ++motionEpoch.current;setOperationActivity('command',true);
    try{await client.sendPwm(forwardPct,strafePct,turnPct,400);}finally{++motionEpoch.current;}
  },[client,setOperationActivity]);

  const refreshSensors = useCallback(async (): Promise<void> => {
    const res = await executeCommandWithToast< Partial<SensorsSnapshot> >(
      'robot.sensors',
      {},
      {
        loadingTitle: 'Solicitando telemetría de sensores...',
        successTitle: 'Lectura de sensores actualizada',
      },
      2000
    );
    if (res?.payload) {
      setSensors((prev) => ({
        line: res.payload.line ?? prev.line,
        ultrasonic: res.payload.ultrasonic ?? prev.ultrasonic,
        encoders: res.payload.encoders ?? {...prev.encoders,ageMs:prev.encoders.ageMs===null?null:prev.encoders.ageMs+(prev.receivedAtMs===null?0:Date.now()-prev.receivedAtMs)},
        receivedAtMs: Date.now(),
      }));
    }
  }, [executeCommandWithToast]);

  const refreshWifiStatus = useCallback(async (): Promise<void> => {
    const res = await executeCommandWithToast<Partial<WifiStatusSnapshot>>(
      'wifi.info',
      {},
      {
        loadingTitle: 'Consultando estado Wi-Fi del ESP8266...',
        successTitle: 'Estado Wi-Fi actualizado',
      },
      2500
    );
    if (res?.payload) {
      setWifiStatus((prev) => ({
        ...prev,
        ...res.payload,
        ageMs: 0, receivedAtMs:Date.now(),
      }));
    }
  }, [executeCommandWithToast]);

  const scanWifiNetworks = useCallback(async (): Promise<void> => {
    if (wifiScanPending.current) return;
    wifiScanPending.current = true;
    const generation = ++wifiScanGeneration.current;
    expectedScanId.current=null;setWifiScanState('scanning'); setWifiScanError(null);
    setWifiNetworks([]); setWifiScanTruncated(false);
    // accepted only starts the scan. The event is its terminal outcome.
    wifiScanTimer.current = setTimeout(() => {
      if (generation !== wifiScanGeneration.current || !wifiScanPending.current) return;
      wifiScanPending.current = false; setWifiScanState('failed');
      setWifiScanError('No llegó el resultado del escaneo en 30 segundos.');
    }, 30000);
    try {
      const result=await client.sendCommand<{scanId?:number}>('wifi.scan', {}, 10000);
      if(generation===wifiScanGeneration.current)expectedScanId.current=result.payload.scanId??null;
    } catch (error) {
      if (generation !== wifiScanGeneration.current || !wifiScanPending.current) return;
      if (wifiScanTimer.current) clearTimeout(wifiScanTimer.current);
      wifiScanPending.current = false; setWifiScanState('failed');
      setWifiScanError(error instanceof Error ? error.message : 'No se pudo iniciar el escaneo.');
    }
  }, [client]);

  const value = useMemo<ConnectionContextValue>(
    () => ({
      ...snapshot,
      battery, refreshBattery,
      systemInfo, systemInfoUpdatedAtMs, bootReports, refreshSystemInfo, stopAvailable, webOperationActive, hasWebOperation, setOperationActivity,
      wifiDraft,
      robotState,
      sensors, headReading, screenStatus, registerScreenViewer, registerHeadViewer, screenSubscribed,
      wifiStatus,
      wifiNetworks,
      wifiScanState, wifiScanError, wifiScanTruncated,
      stopRecord,
      stopNoticeVisible,
      dismissStopNotice,
      connectToEndpoint,
      disconnectEndpoint,
      setEndpointUrl,
      triggerEmergencyStop,
      sendDriveCommand,
      sendPwmCommand,
      requestCommand,
      executeCommandWithToast,
      refreshSensors,
      refreshWifiStatus,
      scanWifiNetworks,
    }),
    [
      snapshot, battery, refreshBattery, systemInfo, systemInfoUpdatedAtMs, bootReports, refreshSystemInfo, stopAvailable, webOperationActive, hasWebOperation, setOperationActivity,
      wifiDraft,
      robotState,
      sensors, headReading, screenStatus, registerScreenViewer, registerHeadViewer, screenSubscribed,
      wifiStatus,
      wifiNetworks,
      wifiScanState, wifiScanError, wifiScanTruncated,
      stopRecord,
      stopNoticeVisible,
      dismissStopNotice,
      connectToEndpoint,
      disconnectEndpoint,
      setEndpointUrl,
      triggerEmergencyStop,
      sendDriveCommand,
      sendPwmCommand,
      requestCommand,
      executeCommandWithToast,
      refreshSensors,
      refreshWifiStatus,
      scanWifiNetworks,
    ]
  );

  return <ConnectionContext.Provider value={value}>{children}</ConnectionContext.Provider>;
}

export function useConnection(): ConnectionContextValue {
  const ctx = useContext(ConnectionContext);
  if (!ctx) {
    throw new Error('useConnection debe utilizarse dentro de ConnectionProvider');
  }
  return ctx;
}
