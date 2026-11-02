/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { version as WEB_VERSION } from '../../package.json';
import {
  AFR2_PRODUCT_ID,
  AFR2_WS_SCHEMA,
  type Afr2CapabilitiesMap,
  type Afr2CommandName,
  type Afr2ErrorCode,
  type Afr2WsErrorFrame,
  type Afr2WsEventFrame,
  type Afr2WsIncomingFrame,
  type Afr2WsRequest,
  type Afr2WsResponse,
  DEFAULT_DESIGN_CAPABILITIES,
  type MegaLinkState,
  type WsConnectionState,
} from './wsEnvelope';


export class Afr2CommandError extends Error {
  public readonly code: Afr2ErrorCode;
  public readonly retryable: boolean;
  public readonly source: 'web' | 'esp' | 'atmega';

  constructor(
    code: Afr2ErrorCode,
    message: string,
    retryable = false,
    source: 'web' | 'esp' | 'atmega' = 'web'
  ) {
    super(message);
    this.name = 'Afr2CommandError';
    this.code = code;
    this.retryable = retryable;
    this.source = source;
  }
}

interface PendingTransaction {
  requestId: string;
  cmd: Afr2CommandName;
  timer: ReturnType<typeof setTimeout>;
  resolve: (res: Afr2WsResponse) => void;
  reject: (err: Afr2CommandError) => void;
}

export interface Afr2ClientStateSnapshot {
  endpointUrl: string;
  autoReconnect: boolean;
  wsState: WsConnectionState;
  megaLinkState: MegaLinkState;
  brokerVersion: string | null;
  lastTrafficMs: number | null;
  lastError: string | null;
  reconnectAttempt: number;
  capabilities: Afr2CapabilitiesMap;
  connectionDiagnostics?: Record<string, unknown>;
  lastSocketClose?: {code:number;reason:string;clean:boolean;atMs:number};
}

type StateListener = (state: Afr2ClientStateSnapshot) => void;
type EventListener = (evt: Afr2WsEventFrame) => void;

const HELLO_TIMEOUT_MS = 5000;
const ALIVE_INTERVAL_MS = 500;
const SESSION_EXPIRY_MS = 30000;
const BACKOFF_BASE_MS = 500;
const BACKOFF_MAX_MS = 15000;

export function getDefaultWsEndpoint(): string {
  if (typeof window === 'undefined') return 'ws://192.168.4.1/ws';
  const proto = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
  const host = window.location.host || '192.168.4.1';
  return `${proto}//${host}/ws`;
}

export class Afr2Client {
  private socket: WebSocket | null = null;
  private generationId = 0;
  private closedGeneration = -1;
  private heartbeatPending = false;
  private queuedDriveCommand: 'robot.drive' | 'robot.drive.pwm' = 'robot.drive';
  private seqCounter = 0;
  private pending = new Map<string, PendingTransaction>();
  private aliveTimer: ReturnType<typeof setInterval> | null = null;
  private reconnectTimer: ReturnType<typeof setTimeout> | null = null;
  private driveInFlightId: string | null = null;
  private queuedDrivePayload: Record<string, unknown> | null = null;

  private state: Afr2ClientStateSnapshot = {
    endpointUrl: getDefaultWsEndpoint(),
    autoReconnect: false,
    wsState: 'disconnected',
    megaLinkState: 'unknown',
    brokerVersion: null,
    lastTrafficMs: null,
    lastError: null,
    reconnectAttempt: 0,
    capabilities: DEFAULT_DESIGN_CAPABILITIES,
  };

  private stateListeners = new Set<StateListener>();
  private eventListeners = new Set<EventListener>();

  public getState(): Afr2ClientStateSnapshot {
    return this.state;
  }

  public subscribeState(listener: StateListener): () => void {
    this.stateListeners.add(listener);
    listener(this.state);
    return () => {
      this.stateListeners.delete(listener);
    };
  }

  public subscribeEvents(listener: EventListener): () => void {
    this.eventListeners.add(listener);
    return () => {
      this.eventListeners.delete(listener);
    };
  }

  private updateState(patch: Partial<Afr2ClientStateSnapshot>): void {
    this.state = { ...this.state, ...patch };
    this.stateListeners.forEach((fn) => fn(this.state));
  }

  public setEndpointUrl(url: string): void {
    const trimmed = url.trim();
    if (!trimmed) return;
    this.updateState({ endpointUrl: trimmed });
  }

  public connect(customUrl?: string): void {
    if (customUrl) {
      this.setEndpointUrl(customUrl);
    }
    this.clearReconnectTimer();
    this.teardownSocket('Reconexión manual solicitada');

    const currentGen = ++this.generationId;
    this.updateState({
      autoReconnect: true,
      wsState: 'connecting',
      megaLinkState: 'unknown',
      capabilities: DEFAULT_DESIGN_CAPABILITIES,
      lastError: null,
    });

    let ws: WebSocket;
    try {
      ws = new WebSocket(this.state.endpointUrl);
    } catch (err) {
      const msg = err instanceof Error ? err.message : 'URL WebSocket inválida';
      this.updateState({
        wsState: 'disconnected',
        lastError: msg,
      });
      this.scheduleReconnect(currentGen);
      return;
    }

    this.socket = ws;

    ws.onopen = () => {
      if (currentGen !== this.generationId) return;
      this.updateState({
        wsState: 'handshaking',
        lastTrafficMs: Date.now(),
      });
      void this.performHandshake(currentGen);
    };

    ws.onmessage = (event) => {
      if (currentGen !== this.generationId) return;
      this.handleRawMessage(event.data);
    };

    ws.onerror = () => {
      if (currentGen !== this.generationId) return;
      this.updateState({
        lastError: 'Sin respuesta del broker ESP8266 en el endpoint seleccionado.',
      });
    };

    ws.onclose = (event) => {
      if (currentGen !== this.generationId) return;
      const reason = event.reason || 'Sin motivo remoto';
      this.updateState({lastSocketClose:{code:event.code,reason,clean:event.wasClean,atMs:Date.now()}});
      this.handleSocketClosed(currentGen, `WebSocket cerrado (${event.code}): ${reason}`);
    };
  }

  public disconnect(): void {
    this.generationId++;
    this.clearReconnectTimer();
    this.updateState({
      autoReconnect: false,
      reconnectAttempt: 0,
    });
    this.teardownSocket('Desconectado por el usuario.');
    this.updateState({
      wsState: 'disconnected',
      megaLinkState: 'unknown',
    });
  }

  private async performHandshake(gen: number): Promise<void> {
    try {
      const res = await this.sendRawRequest(
        'system.hello',
        {
          client: 'afr2-web',
          clientVersion: WEB_VERSION,
        },
        HELLO_TIMEOUT_MS,
        true
      );
      if (gen !== this.generationId || this.closedGeneration === gen) return;

      const payload = res.payload ?? {};
      if (payload.product !== AFR2_PRODUCT_ID || payload.controlMode !== 'direct') {
        this.updateState({
          autoReconnect: false,
          lastError: `Producto incompatible recibido en handshake: ${String(payload.product ?? 'desconocido')}`,
        });
        this.teardownSocket('Producto incompatible');
        return;
      }

      const megaLink = (payload.megaLink as MegaLinkState) ?? 'unknown';
      const brokerVersion = typeof payload.brokerVersion === 'string' ? payload.brokerVersion : '0.1.0';
      const remoteCaps = (payload.capabilities as Partial<Afr2CapabilitiesMap> | undefined) ?? {};

      this.updateState({
        wsState: 'ready',
        megaLinkState: megaLink,
        brokerVersion,
        reconnectAttempt: 0,
        lastError: null,
        capabilities: {
          ...DEFAULT_DESIGN_CAPABILITIES,
          ...remoteCaps,
        },
      });

      this.startHeartbeat(gen);
    } catch (err) {
      if (gen !== this.generationId || this.closedGeneration === gen) return;
      const msg = err instanceof Error ? err.message : 'Fallo en handshake system.hello';
      this.handleSocketClosed(gen, msg);
    }
  }

  private startHeartbeat(gen: number): void {
    this.stopHeartbeat();
    this.aliveTimer = setInterval(() => {
      if (gen !== this.generationId || this.state.wsState !== 'ready') return;
      const now = Date.now();
      if (
        this.state.lastTrafficMs !== null &&
        now - this.state.lastTrafficMs > SESSION_EXPIRY_MS
      ) {
        this.handleSocketClosed(gen, 'Sesión expirada (> 30000 ms sin respuesta del broker).');
        return;
      }
      if(this.heartbeatPending)return;
      this.heartbeatPending=true;
      void this.sendRawRequest('system.alive', { clientTs: now, interactive: document.visibilityState === 'visible' && document.hasFocus() }, 4000, true)
        .then((res) => {
          if (gen !== this.generationId || this.closedGeneration === gen) return;
          const p = res.payload ?? {};
          this.applyBrokerStatus(p);
        })
        .catch(() => {
          // La expiracion de transporte de 30000 ms gestiona el corte definitivo
        }).finally(() => {if(gen===this.generationId)this.heartbeatPending=false;});
    }, ALIVE_INTERVAL_MS);
  }

  private stopHeartbeat(): void {
    this.heartbeatPending=false;
    if (this.aliveTimer) {
      clearInterval(this.aliveTimer);
      this.aliveTimer = null;
    }
  }

  private clearReconnectTimer(): void {
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer);
      this.reconnectTimer = null;
    }
  }

  private scheduleReconnect(gen: number): void {
    if (!this.state.autoReconnect || gen !== this.generationId) return;
    this.clearReconnectTimer();
    const attempt = this.state.reconnectAttempt + 1;
    const expDelay = Math.min(BACKOFF_MAX_MS, BACKOFF_BASE_MS * Math.pow(2, attempt - 1));
    const jitter = expDelay * 0.2 * (Math.random() * 2 - 1);
    const delayMs = Math.round(Math.max(400, expDelay + jitter));

    this.updateState({ reconnectAttempt: attempt });
    this.reconnectTimer = setTimeout(() => {
      if (gen !== this.generationId || !this.state.autoReconnect) return;
      this.connect();
    }, delayMs);
  }

  private handleSocketClosed(gen: number, reason: string): void {
    if(gen!==this.generationId || this.closedGeneration===gen)return;
    this.closedGeneration=gen;
    this.teardownSocket(reason);
    this.updateState({
      wsState: 'disconnected',
      megaLinkState: 'disconnected',
      capabilities: DEFAULT_DESIGN_CAPABILITIES,
      lastError: reason,
    });
    this.scheduleReconnect(gen);
  }

  private teardownSocket(reason: string): void {
    this.stopHeartbeat();
    this.queuedDrivePayload = null;
    this.driveInFlightId = null;

    if (this.socket) {
      try {
        this.socket.onopen = null;
        this.socket.onmessage = null;
        this.socket.onerror = null;
        this.socket.onclose = null;
        if (
          this.socket.readyState === WebSocket.OPEN ||
          this.socket.readyState === WebSocket.CONNECTING
        ) {
          this.socket.close();
        }
      } catch {
        // Ignorar errores al cerrar socket
      }
      this.socket = null;
    }

    for (const [reqId, tx] of this.pending.entries()) {
      clearTimeout(tx.timer);
      tx.reject(new Afr2CommandError('CONNECTION_CLOSED', reason, false, 'web'));
      this.pending.delete(reqId);
    }
  }

  /** AFR2-20261023-11: runtime capabilities may change after the Web handshake. */
  private applyBrokerStatus(payload: Record<string, unknown>): void {
    const remoteCaps = payload.capabilities;
    this.updateState({
      megaLinkState: (payload.megaLink as MegaLinkState) ?? this.state.megaLinkState,
      ...(payload.connectionDiagnostics && typeof payload.connectionDiagnostics==='object' ? {connectionDiagnostics:payload.connectionDiagnostics as Record<string,unknown>} : {}),
      ...(remoteCaps && typeof remoteCaps === 'object'
        ? { capabilities: { ...this.state.capabilities, ...(remoteCaps as Partial<Afr2CapabilitiesMap>) } }
        : {}),
    });
  }

  private handleRawMessage(raw: unknown): void {
    if (typeof raw !== 'string') return;
    let parsed: Afr2WsIncomingFrame;
    try {
      parsed = JSON.parse(raw) as Afr2WsIncomingFrame;
    } catch {
      return;
    }

    if (!parsed || parsed.schema !== AFR2_WS_SCHEMA) return;
    this.updateState({ lastTrafficMs: Date.now() });

    if (parsed.type === 'evt') {
      if (parsed.event === 'system.status' && parsed.payload) {
        this.applyBrokerStatus(parsed.payload);
      }
      this.eventListeners.forEach((fn) => fn(parsed));
      return;
    }

    if (parsed.type === 'res' || parsed.type === 'err') {
      const tx = this.pending.get(parsed.requestId);
      if (!tx) return; // Descartar ACK tardío / cuarentena post-timeout
      if(parsed.cmd !== tx.cmd || (parsed.type === 'res' && (parsed.ok !== true || (parsed.status !== 'accepted' && parsed.status !== 'completed')))) return;
      clearTimeout(tx.timer);
      this.pending.delete(parsed.requestId);

      if (this.driveInFlightId === parsed.requestId) {
        this.driveInFlightId = null;
        this.flushQueuedDrive();
      }

      if (parsed.type === 'res') {
        if (parsed.status === 'completed' && parsed.payload?.megaLink) this.applyBrokerStatus(parsed.payload);
        tx.resolve(parsed);
      } else {
        const errFrame = parsed as Afr2WsErrorFrame;
        tx.reject(
          new Afr2CommandError(
            errFrame.error?.code ?? 'INTERNAL_ERROR',
            errFrame.error?.message ?? 'Error reportado por el broker',
            Boolean(errFrame.error?.retryable),
            errFrame.error?.source ?? 'esp'
          )
        );
      }
    }
  }

  private flushQueuedDrive(): void {
    if (!this.queuedDrivePayload || this.driveInFlightId) return;
    const nextPayload = this.queuedDrivePayload;
    this.queuedDrivePayload = null;
    if(this.queuedDriveCommand==='robot.drive.pwm'){
      void this.sendPwm(Number(nextPayload.forwardPct??0),Number(nextPayload.strafePct??0),Number(nextPayload.turnPct??0),Number(nextPayload.ttlMs??400)).catch(()=>undefined);return;
    }
    void this.sendDrive(
      Number(nextPayload.vxMmS ?? 0),
      Number(nextPayload.vyMmS ?? 0),
      Number(nextPayload.wzMradS ?? 0),
      Number(nextPayload.ttlMs ?? 400)
    ).catch(() => {
      // Silenciar rechazos de ranura reemplazada
    });
  }

  public async sendCommand<TPayload = Record<string, unknown>>(
    cmd: Afr2CommandName,
    payload: Record<string, unknown> = {},
    timeoutMs = 2500
  ): Promise<Afr2WsResponse<TPayload>> {
    if (cmd === 'robot.stop') {
      return this.sendPriorityStop(String(payload.reason ?? 'user-stop')) as Promise<
        Afr2WsResponse<TPayload>
      >;
    }
    return this.sendRawRequest<TPayload>(cmd, payload, timeoutMs, false);
  }

  /**
   * `robot.stop` es prioritario e idempotente: invalida inmediatamente cualquier consigna de
   * movimiento en vuelo (`SUPERSEDED`) y se transmite sin operación previa de habilitación.
   */
  public async sendPriorityStop(
    reason: 'user-stop' | 'focus-lost' | 'control-reset' | 'safety' | string = 'user-stop'
  ): Promise<Afr2WsResponse> {
    this.queuedDrivePayload = null;
    for (const [reqId, tx] of this.pending.entries()) {
      if (
        tx.cmd === 'robot.drive' || tx.cmd === 'robot.drive.pwm' ||
        tx.cmd === 'robot.test.wheel' ||
        tx.cmd === 'robot.goto.start' ||
        tx.cmd === 'robot.sequence.begin' ||
        tx.cmd === 'robot.sequence.append' ||
        tx.cmd === 'robot.sequence.commit' ||
        tx.cmd === 'robot.sequence.start'
      ) {
        clearTimeout(tx.timer);
        this.pending.delete(reqId);
        tx.reject(
          new Afr2CommandError(
            'SUPERSEDED',
            'Consigna cancelada por orden prioritaria STOP.',
            false,
            'web'
          )
        );
      }
    }
    this.driveInFlightId = null;
    return this.sendRawRequest('robot.stop', { reason }, 1500, false);
  }

  /**
   * Política `single-latest-slot`: máximo una petición `robot.drive` en vuelo.
   */
  public async sendDrive(
    vxMmS: number,
    vyMmS: number,
    wzMradS: number,
    ttlMs = 400
  ): Promise<Afr2WsResponse | null> {
    const payload = {
      vxMmS: Math.round(vxMmS),
      vyMmS: Math.round(vyMmS),
      wzMradS: Math.round(wzMradS),
      ttlMs: Math.round(ttlMs),
    };

    if (this.driveInFlightId) {
      this.queuedDriveCommand='robot.drive';this.queuedDrivePayload = payload;
      return null;
    }

    const reqPromise = this.sendRawRequest('robot.drive', payload, 600, false);
    return reqPromise;
  }

  /** AFR2-20261029-27: percentages are explicit; never reinterpret mm/s. */
  public async sendPwm(forwardPct:number,strafePct:number,turnPct:number,ttlMs=400):Promise<Afr2WsResponse|null>{
    const payload={forwardPct:Math.round(forwardPct),strafePct:Math.round(strafePct),turnPct:Math.round(turnPct),ttlMs:Math.round(ttlMs)};
    if(this.driveInFlightId){this.queuedDriveCommand='robot.drive.pwm';this.queuedDrivePayload=payload;return null;}
    return this.sendRawRequest('robot.drive.pwm',payload,600,false);
  }

  private sendRawRequest<TPayload = Record<string, unknown>>(
    cmd: Afr2CommandName,
    payload: Record<string, unknown>,
    timeoutMs: number,
    allowDuringHandshake: boolean
  ): Promise<Afr2WsResponse<TPayload>> {
    return new Promise((resolve, reject) => {
      if (!this.socket || this.socket.readyState !== WebSocket.OPEN) {
        reject(
          new Afr2CommandError(
            'CONNECTION_CLOSED',
            'No hay sesión WebSocket activa con el broker ESP8266.',
            true,
            'web'
          )
        );
        return;
      }

      if (!allowDuringHandshake && this.state.wsState !== 'ready') {
        reject(
          new Afr2CommandError(
            'CONNECTION_CLOSED',
            'El enlace WebSocket aún no completó el handshake system.hello.',
            true,
            'web'
          )
        );
        return;
      }

      const requestId = `req-${Date.now()}-${++this.seqCounter}`;
      const frame: Afr2WsRequest = {
        schema: AFR2_WS_SCHEMA,
        type: 'req',
        requestId,
        cmd,
        payload,
        ts: Date.now(),
      };

      if (cmd === 'robot.drive' || cmd === 'robot.drive.pwm') {
        this.driveInFlightId = requestId;
      }

      const timer = setTimeout(() => {
        this.pending.delete(requestId);
        if (this.driveInFlightId === requestId) {
          this.driveInFlightId = null;
        }
        reject(
          new Afr2CommandError(
            'TIMEOUT',
            `Tiempo de espera agotado (${timeoutMs} ms) para ${cmd}.`,
            true,
            'web'
          )
        );
      }, timeoutMs);

      this.pending.set(requestId, {
        requestId,
        cmd,
        timer,
        resolve: resolve as (res: Afr2WsResponse) => void,
        reject,
      });

      try {
        this.socket.send(JSON.stringify(frame));
      } catch (err) {
        clearTimeout(timer);
        this.pending.delete(requestId);
        if (this.driveInFlightId === requestId) {
          this.driveInFlightId = null;
        }
        reject(
          new Afr2CommandError(
            'CONNECTION_CLOSED',
            err instanceof Error ? err.message : 'Fallo al transmitir trama WebSocket',
            true,
            'web'
          )
        );
      }
    });
  }
}
