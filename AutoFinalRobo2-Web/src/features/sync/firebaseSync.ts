/**
 * AFR2-20261117-05 — Copia online de la sincronización en Firebase (proyecto `autofinalrobo2`, un solo auto).
 *
 * Cliente REST mínimo (sin SDK, para no sumar peso al paquete del ESP) de Firebase Auth anónimo y Firestore.
 * Roles (ver `firebase/firestore.rules`):
 *  - Escritor: navegador conectado al auto; demuestra la clave de escritura que guarda el ESP.
 *  - Lector: compu vinculada con el código de 6 caracteres que muestra el OLED; solo lee `car/state`.
 */
import type { CloudSyncEnvelope, CloudSyncLayerHashes } from './cloudSyncSchema';
import { FIREBASE_WEB_CONFIG } from './firebaseConfig';

const { projectId: PROJECT_ID, apiKey: API_KEY } = FIREBASE_WEB_CONFIG;
export const FIREBASE_HOSTING_ORIGIN = `https://${PROJECT_ID}.web.app`;
export const REMOTE_VIEW_URL = `${FIREBASE_HOSTING_ORIGIN}/#/remoto`;
const DB = `projects/${PROJECT_ID}/databases/(default)/documents`;
const API = `https://firestore.googleapis.com/v1/${DB}`;
const SESSION_STORAGE_KEY = 'afr2.firebase.session.v1';
const KEY_PATTERN = /^[A-Za-z0-9_-]{32,48}$/;
/** Sin 0/O, 1/I/L ni U, para que se lea bien en el OLED. */
const CODE_ALPHABET = '23456789ABCDEFGHJKMNPQRSTVWXYZ';
export const PAIR_CODE_PATTERN = /^[0-9A-HJKMNP-TV-Z]{3}-[0-9A-HJKMNP-TV-Z]{3}$/;
export const PAIR_VALID_MS = 5 * 60_000;

/** true cuando la Web se abre desde Firebase Hosting (sin auto): nunca escribe. */
export function isHostedRemote(): boolean {
  if (typeof window === 'undefined') return false;
  const host = window.location.hostname;
  return host === `${PROJECT_ID}.web.app` || host === `${PROJECT_ID}.firebaseapp.com`;
}

export function isValidWriteKey(key: string): boolean {
  return KEY_PATTERN.test(key);
}

function randomString(alphabet: string, length: number): string {
  const bytes = new Uint8Array(length);
  crypto.getRandomValues(bytes);
  // 256 no es múltiplo de 30/64 en todos los casos: se descartan los bytes que sesgarían.
  let out = '';
  const limit = 256 - (256 % alphabet.length);
  for (let i = 0; out.length < length; i++) {
    if (i >= bytes.length) { crypto.getRandomValues(bytes); i = 0; }
    if (bytes[i] < limit) out += alphabet[bytes[i] % alphabet.length];
  }
  return out;
}

/** Clave de escritura de 32 caracteres (≈190 bits). La guarda el ESP. */
export function generateWriteKey(): string {
  return randomString('ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_', 32);
}

/** Código de vinculación «K7M-4QX» (30^6 ≈ 7,3·10^8 combinaciones, 5 min, un solo uso). */
export function generatePairCode(): string {
  const raw = randomString(CODE_ALPHABET, 6);
  return `${raw.slice(0, 3)}-${raw.slice(3)}`;
}

/** Normaliza lo que se tipea: mayúsculas, sin espacios, O→0, I/L→1 y guion en el medio. */
export function normalizePairCode(input: string): string {
  const raw = input.toUpperCase().replace(/[^0-9A-Z]/g, '').replace(/O/g, '0').replace(/[IL]/g, '1').slice(0, 6);
  return raw.length > 3 ? `${raw.slice(0, 3)}-${raw.slice(3)}` : raw;
}

/* ---------- Sesión anónima (Identity Toolkit REST) ---------- */

interface Session { uid: string; idToken: string; refreshToken: string; expiresAt: number }

function loadSession(): Session | null {
  try {
    const raw = localStorage.getItem(SESSION_STORAGE_KEY);
    if (!raw) return null;
    const s = JSON.parse(raw) as Session;
    return s.uid && s.refreshToken ? s : null;
  } catch {
    return null;
  }
}

function saveSession(s: Session | null) {
  try {
    if (s) localStorage.setItem(SESSION_STORAGE_KEY, JSON.stringify(s));
    else localStorage.removeItem(SESSION_STORAGE_KEY);
  } catch {
    /* Optional storage: la sesión vive solo en memoria. */
  }
}

let memorySession: Session | null = null;
let sessionPromise: Promise<Session> | null = null;

async function signUpAnonymous(): Promise<Session> {
  const res = await fetch(`https://identitytoolkit.googleapis.com/v1/accounts:signUp?key=${API_KEY}`, {
    method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify({ returnSecureToken: true }),
  });
  if (!res.ok) throw new FirebaseSyncError('No se pudo iniciar la sesión en Firebase.', res.status);
  const b = await res.json() as { localId: string; idToken: string; refreshToken: string; expiresIn: string };
  return { uid: b.localId, idToken: b.idToken, refreshToken: b.refreshToken, expiresAt: Date.now() + Number(b.expiresIn) * 1000 };
}

async function refresh(s: Session): Promise<Session> {
  const res = await fetch(`https://securetoken.googleapis.com/v1/token?key=${API_KEY}`, {
    method: 'POST', headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
    body: `grant_type=refresh_token&refresh_token=${encodeURIComponent(s.refreshToken)}`,
  });
  if (!res.ok) throw new FirebaseSyncError('La sesión de Firebase venció.', res.status);
  const b = await res.json() as { user_id: string; id_token: string; refresh_token: string; expires_in: string };
  return { uid: b.user_id, idToken: b.id_token, refreshToken: b.refresh_token, expiresAt: Date.now() + Number(b.expires_in) * 1000 };
}

/** Sesión anónima vigente (la crea la primera vez y la renueva un minuto antes de vencer). */
export async function getSession(): Promise<Session> {
  memorySession ??= loadSession();
  if (memorySession && memorySession.expiresAt - Date.now() > 60_000) return memorySession;
  sessionPromise ??= (async () => {
    try {
      let next: Session;
      try {
        next = memorySession ? await refresh(memorySession) : await signUpAnonymous();
      } catch (e) {
        // Refresh token revocado o usuario borrado: sesión nueva (pierde la vinculación anterior).
        if (!memorySession) throw e;
        next = await signUpAnonymous();
      }
      memorySession = next;
      saveSession(next);
      return next;
    } finally {
      sessionPromise = null;
    }
  })();
  return sessionPromise;
}

export function currentUid(): string | null {
  memorySession ??= loadSession();
  return memorySession?.uid ?? null;
}

/* ---------- Firestore REST ---------- */

export class FirebaseSyncError extends Error {
  constructor(message: string, public readonly status: number) { super(message); }
}

type FirestoreValue = Record<string, unknown>;

function toValue(value: unknown, insideArray = false): FirestoreValue {
  if (value === null || value === undefined) return { nullValue: null };
  if (typeof value === 'boolean') return { booleanValue: value };
  if (typeof value === 'number') {
    if (!Number.isFinite(value)) return { nullValue: null };
    return Number.isInteger(value) ? { integerValue: String(value) } : { doubleValue: value };
  }
  if (typeof value === 'string') return { stringValue: value };
  if (Array.isArray(value)) {
    // Firestore no admite arreglos dentro de arreglos: el interno se guarda como texto JSON.
    if (insideArray) return { stringValue: JSON.stringify(value) };
    return { arrayValue: value.length ? { values: value.map(v => toValue(v, true)) } : {} };
  }
  if (typeof value === 'object') return { mapValue: { fields: toFields(value as Record<string, unknown>) } };
  return { stringValue: String(value) };
}

function toFields(obj: Record<string, unknown>): Record<string, FirestoreValue> {
  const out: Record<string, FirestoreValue> = {};
  for (const [k, v] of Object.entries(obj)) if (v !== undefined) out[k] = toValue(v);
  return out;
}

function fromValue(v: FirestoreValue): unknown {
  if ('nullValue' in v) return null;
  if ('booleanValue' in v) return v.booleanValue;
  if ('integerValue' in v) return Number(v.integerValue);
  if ('doubleValue' in v) return Number(v.doubleValue);
  if ('stringValue' in v) return v.stringValue;
  if ('timestampValue' in v) return v.timestampValue;
  if ('arrayValue' in v) return (((v.arrayValue as { values?: FirestoreValue[] }).values) ?? []).map(fromValue);
  if ('mapValue' in v) return fromFields(((v.mapValue as { fields?: Record<string, FirestoreValue> }).fields) ?? {});
  return null;
}

function fromFields(fields: Record<string, FirestoreValue>): Record<string, unknown> {
  const out: Record<string, unknown> = {};
  for (const [k, v] of Object.entries(fields)) out[k] = fromValue(v);
  return out;
}

async function call(path: string, init: RequestInit & { signal?: AbortSignal } = {}): Promise<Response> {
  const s = await getSession();
  return fetch(path, { ...init, cache: 'no-store', headers: { ...(init.headers ?? {}), Authorization: `Bearer ${s.idToken}`, 'Content-Type': 'application/json' } });
}

async function getDoc(path: string, signal?: AbortSignal): Promise<Record<string, unknown> | null> {
  const res = await call(`${API}/${path}`, { signal });
  if (res.status === 404) return null;
  if (!res.ok) throw new FirebaseSyncError(res.status === 403 ? 'Sin permiso en la copia online.' : `Firebase respondió ${res.status}.`, res.status);
  const body = await res.json() as { fields?: Record<string, FirestoreValue> };
  return fromFields(body.fields ?? {});
}

type Write = { update: { name: string; fields: Record<string, FirestoreValue> }; currentDocument?: { exists: boolean } } | { delete: string };

async function commit(writes: Write[]): Promise<Response> {
  return call(`https://firestore.googleapis.com/v1/projects/${PROJECT_ID}/databases/(default)/documents:commit`, {
    method: 'POST', body: JSON.stringify({ writes }),
  });
}

const docName = (path: string) => `${DB}/${path}`;

/* ---------- Escritor (Web conectada al auto) ---------- */

/**
 * Registra este navegador como escritor probando la clave del ESP. La primera vez también reclama
 * `car/secret`. Devuelve 'ok', o 'claimed' si la copia ya tiene otra clave.
 */
export async function ensureWriter(writeKey: string): Promise<'ok' | 'claimed'> {
  if (!isValidWriteKey(writeKey)) throw new FirebaseSyncError('Clave de escritura inválida.', 400);
  const { uid } = await getSession();
  const mine = await getDoc(`writers/${uid}`).catch(() => null);
  if (mine && mine.w === writeKey) return 'ok';
  const at = Date.now();
  const writer: Write = { update: { name: docName(`writers/${uid}`), fields: toFields({ w: writeKey, at }) }, currentDocument: { exists: false } };
  if (mine) await call(`${API}/writers/${uid}`, { method: 'DELETE' });
  let res = await commit([writer]);
  if (res.ok) return 'ok';
  // Sin car/secret todavía: se reclama en el mismo commit.
  res = await commit([{ update: { name: docName('car/secret'), fields: toFields({ w: writeKey, at }) }, currentDocument: { exists: false } }, writer]);
  return res.ok ? 'ok' : 'claimed';
}

/** Los nombres de red no salen del auto: en la copia online la capa wifi va sin SSID. */
function redactWifi(wifi: NonNullable<CloudSyncEnvelope['layers']['wifi']>) {
  return { ...wifi, staSsid: null, apSsid: null };
}

function setPath(target: Record<string, unknown>, path: string[], value: unknown) {
  let node = target;
  for (const part of path.slice(0, -1)) {
    node[part] = (node[part] as Record<string, unknown> | undefined) ?? {};
    node = node[part] as Record<string, unknown>;
  }
  node[path[path.length - 1]] = value;
}

/**
 * Escribe el sobre en `car/state` con PATCH + updateMask: un snapshot reemplaza `layers` completo; un delta
 * actualiza solo las capas sucias. `simulators` se guarda aparte como JSON (no se pierde en el próximo snapshot
 * y evita arreglos anidados).
 */
export async function pushEnvelopeToFirebase(params: {
  envelope: CloudSyncEnvelope;
  hashes: CloudSyncLayerHashes;
  ackRev: number;
  webVersion: string;
  signal?: AbortSignal;
}): Promise<boolean> {
  const { envelope, hashes, ackRev, webVersion, signal } = params;
  const doc: Record<string, unknown> = {
    schema: envelope.schema, deviceId: envelope.deviceId.slice(0, 48), rev: envelope.rev, ackRev, ts: envelope.ts, webVersion, hashes,
  };
  const paths = ['schema', 'deviceId', 'rev', 'ackRev', 'ts', 'webVersion', 'hashes'];
  const { simulators, ...rawLayers } = envelope.layers;
  const layers = { ...rawLayers, ...(rawLayers.wifi ? { wifi: redactWifi(rawLayers.wifi) } : {}) };
  if (envelope.mode === 'snapshot') {
    doc.layers = layers;
    paths.push('layers');
  } else {
    for (const [name, value] of Object.entries(layers)) {
      if (name === 'config' && value) {
        for (const [sub, subValue] of Object.entries(value as Record<string, unknown>)) {
          setPath(doc, ['layers', 'config', sub], subValue);
          paths.push(`layers.config.${sub}`);
        }
      } else {
        setPath(doc, ['layers', name], value);
        paths.push(`layers.${name}`);
      }
    }
  }
  if (simulators) {
    doc.simulators = { source: simulators.source, syncedAtMs: simulators.syncedAtMs, json: JSON.stringify(simulators) };
    paths.push('simulators');
  }
  const mask = paths.map(p => `updateMask.fieldPaths=${encodeURIComponent(p)}`).join('&');
  const res = await call(`${API}/car/state?${mask}`, { method: 'PATCH', body: JSON.stringify({ fields: toFields(doc) }), signal });
  return res.ok;
}

/** Crea un código de vinculación de 5 min. Reintenta si por azar ya existiera. */
export async function createPairing(): Promise<{ code: string; expiresAt: number }> {
  const { uid } = await getSession();
  for (let attempt = 0; attempt < 3; attempt++) {
    const code = generatePairCode();
    const expiresAt = Date.now() + PAIR_VALID_MS;
    const res = await commit([{ update: { name: docName(`pairings/${code}`), fields: toFields({ expiresAt, by: uid }) }, currentDocument: { exists: false } }]);
    if (res.ok) return { code, expiresAt };
    if (res.status === 403) throw new FirebaseSyncError('Este navegador no está registrado como escritor del auto.', 403);
  }
  throw new FirebaseSyncError('No se pudo crear el código.', 409);
}

/** true mientras el código siga sin canjear. */
export async function pairingPending(code: string): Promise<boolean> {
  return (await getDoc(`pairings/${code}`)) !== null;
}

export async function cancelPairing(code: string): Promise<void> {
  await call(`${API}/pairings/${code}`, { method: 'DELETE' }).catch(() => undefined);
}

export interface LinkedViewer { uid: string; label: string; at: number }

export async function listViewers(): Promise<LinkedViewer[]> {
  const res = await call(`${API}/viewers?pageSize=50`);
  if (!res.ok) throw new FirebaseSyncError(`Firebase respondió ${res.status}.`, res.status);
  const body = await res.json() as { documents?: { name: string; fields?: Record<string, FirestoreValue> }[] };
  return (body.documents ?? []).map(d => {
    const f = fromFields(d.fields ?? {});
    return { uid: d.name.split('/').pop() ?? '', label: String(f.label ?? 'Navegador'), at: Number(f.at ?? 0) };
  }).sort((a, b) => b.at - a.at);
}

export async function revokeViewer(uid: string): Promise<boolean> {
  return (await call(`${API}/viewers/${uid}`, { method: 'DELETE' })).ok;
}

/* ---------- Lector (compu vinculada) ---------- */

function deviceLabel(): string {
  const ua = navigator.userAgent;
  const os = /Windows/.test(ua) ? 'Windows' : /Android/.test(ua) ? 'Android' : /iPhone|iPad/.test(ua) ? 'iOS' : /Mac/.test(ua) ? 'Mac' : /Linux/.test(ua) ? 'Linux' : 'Equipo';
  const br = /Edg\//.test(ua) ? 'Edge' : /Chrome\//.test(ua) ? 'Chrome' : /Firefox\//.test(ua) ? 'Firefox' : /Safari\//.test(ua) ? 'Safari' : 'Navegador';
  return `${os} · ${br}`;
}

/** Canjea el código del OLED: crea viewers/{uid} y borra el código en el mismo commit. */
export async function redeemPairCode(code: string): Promise<void> {
  if (!PAIR_CODE_PATTERN.test(code)) throw new FirebaseSyncError('El código tiene 6 caracteres, como K7M-4QX.', 400);
  const { uid } = await getSession();
  const res = await commit([
    { update: { name: docName(`viewers/${uid}`), fields: toFields({ code, at: Date.now(), label: deviceLabel() }) }, currentDocument: { exists: false } },
    { delete: docName(`pairings/${code}`) },
  ]);
  if (res.ok) return;
  if (res.status === 400 || res.status === 409) {
    // Ya vinculado antes con esta sesión: se considera hecho si el estado se puede leer.
    const linked = await getDoc(`viewers/${uid}`).catch(() => null);
    if (linked) return;
  }
  throw new FirebaseSyncError('Código incorrecto, vencido o ya usado. Pedí uno nuevo en el auto.', res.status);
}

export async function unlinkThisDevice(): Promise<void> {
  const uid = currentUid();
  if (uid) await call(`${API}/viewers/${uid}`, { method: 'DELETE' }).catch(() => undefined);
}

export interface RemoteSyncDoc {
  deviceId: string;
  rev: number;
  ackRev: number;
  ts: number;
  webVersion: string;
  layers: Record<string, unknown>;
  simulators: { source: string; syncedAtMs: number; data: unknown } | null;
}

/** Lee `car/state`. null = todavía no hay datos. Lanza 403 si este navegador no está vinculado. */
export async function readRemoteState(signal?: AbortSignal): Promise<RemoteSyncDoc | null> {
  if (!currentUid()) throw new FirebaseSyncError('Sin vincular.', 403);
  const data = await getDoc('car/state', signal);
  if (!data) return null;
  const sim = data.simulators as { source?: string; syncedAtMs?: number; json?: string } | undefined;
  let simData: unknown = null;
  if (sim?.json) { try { simData = JSON.parse(sim.json); } catch { simData = null; } }
  return {
    deviceId: String(data.deviceId ?? ''),
    rev: Number(data.rev ?? 0),
    ackRev: Number(data.ackRev ?? 0),
    ts: Number(data.ts ?? 0),
    webVersion: String(data.webVersion ?? ''),
    layers: (data.layers as Record<string, unknown>) ?? {},
    simulators: sim ? { source: String(sim.source ?? ''), syncedAtMs: Number(sim.syncedAtMs ?? 0), data: simData } : null,
  };
}
