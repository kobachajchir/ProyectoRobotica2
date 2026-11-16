/**
 * AFR2-20261111-03. Precarga de los modelos 3D apenas la conexión con el auto queda estable.
 *
 * Antes cada .glb se pedía recién al abrir una página 3D y, con el ESP sirviendo ~1,7 MB, la página tardaba o no cargaba.
 * Ahora, con el WebSocket listo y sin reinicios durante STABLE_MS, se descargan los modelos uno por uno (para no saturar
 * al ESP) con un toast de avance. Si la descarga se corta (enlace caído, error HTTP, ESP reiniciado) se descarta lo parcial
 * y vuelve a empezar: el ESP sirve los .glb comprimidos (gzip), así que no se puede retomar por rango de bytes.
 * Lo descargado queda en memoria y `ProgressGLTFLoader` lo usa sin volver a pedirlo.
 */
import { MODEL_ASSETS, modelAssetUrl } from './modelAssets';
import { reportModelAsset, getModelAssetState } from './modelAssetLoading';
/** Igual a ROBOT_MODEL_URL (modelAssetLoading); literal para no evaluar ese módulo al cargar este (importación cíclica). */
const ROBOT_MODEL_URL = '/models/AutoFinalRobo2.glb';
import sizes from './modelAssetSizes.json';

export interface PrefetchTarget { url: string; label: string }
export const PREFETCH_TARGETS: readonly PrefetchTarget[] = [
  { url: ROBOT_MODEL_URL, label: 'Auto' },
  { url: modelAssetUrl('brazo'), label: MODEL_ASSETS.brazo.label },
  { url: modelAssetUrl('cubo'), label: MODEL_ASSETS.cubo.label },
];
/** Conexión sin cortes durante este tiempo antes de empezar o reanudar. */
export const STABLE_MS = 4000;
export const MAX_ATTEMPTS = 5;

const buffers = new Map<string, ArrayBuffer>();
const waiters = new Map<string, { promise: Promise<ArrayBuffer>; resolve: (data: ArrayBuffer) => void }>();
const decodedBytes = (url: string) => (sizes as Record<string, number>)[url.split('?')[0]] ?? null;

export function prefetchedModel(url: string): ArrayBuffer | undefined { return buffers.get(url); }
/** Promise of a model that the prefetcher is downloading right now (the loader waits instead of fetching twice). */
export function pendingModel(url: string): Promise<ArrayBuffer> | undefined { return waiters.get(url)?.promise; }

function waiter(url: string) {
  let entry = waiters.get(url);
  if (!entry) {
    let resolve!: (data: ArrayBuffer) => void;
    const promise = new Promise<ArrayBuffer>(done => { resolve = done; });
    entry = { promise, resolve };
    waiters.set(url, entry);
  }
  return entry;
}

export type PrefetchProgress = (target: PrefetchTarget, loaded: number, total: number | null) => void;
/** Downloads one model, streaming so the toast can show progress. Throws on abort or HTTP/network error. */
export async function downloadModel(target: PrefetchTarget, signal: AbortSignal, onProgress: PrefetchProgress): Promise<ArrayBuffer> {
  const cached = buffers.get(target.url);
  if (cached) return cached;
  const total = decodedBytes(target.url);
  const response = await fetch(target.url, { signal });
  if (!response.ok) throw new Error(`HTTP ${response.status} al pedir ${target.label}`);
  const chunks: Uint8Array[] = [];
  let loaded = 0;
  if (response.body) {
    const reader = response.body.getReader();
    for (;;) {
      const { done, value } = await reader.read();
      if (done) break;
      chunks.push(value); loaded += value.byteLength;
      onProgress(target, loaded, total);
      reportModelAsset(target.url, { phase: 'downloading', loadedBytes: loaded, totalBytes: total, error: null });
    }
  } else {
    const data = new Uint8Array(await response.arrayBuffer());
    chunks.push(data); loaded = data.byteLength;
  }
  if (total !== null && loaded < total) throw new Error(`Descarga incompleta de ${target.label} (${loaded} de ${total} bytes)`);
  const data = new Uint8Array(loaded);
  let offset = 0;
  for (const chunk of chunks) { data.set(chunk, offset); offset += chunk.byteLength; }
  buffers.set(target.url, data.buffer);
  waiter(target.url).resolve(data.buffer);
  return data.buffer;
}

/** Marks every pending model so a loader started meanwhile waits for the prefetch instead of fetching again. */
export function announcePrefetch(targets: readonly PrefetchTarget[]) {
  for (const target of targets) if (!buffers.has(target.url) && getModelAssetState(target.url).phase !== 'ready') waiter(target.url);
}
/** When the prefetch gives up, a waiting loader falls back to its own normal download. */
export function releasePrefetch(url: string) {
  const entry = waiters.get(url);
  if (entry && !buffers.has(url)) { waiters.delete(url); entry.resolve(new ArrayBuffer(0)); }
}
export const prefetchComplete = () => PREFETCH_TARGETS.every(target => buffers.has(target.url) || getModelAssetState(target.url).phase === 'ready');
