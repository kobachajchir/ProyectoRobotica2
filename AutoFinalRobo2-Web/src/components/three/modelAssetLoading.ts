// AFR2-20261030-16. Shared download state; no second fetch, HEAD request or global manager override.
import { GLTFLoader, type GLTF } from 'three/addons/loaders/GLTFLoader.js';
import sizes from './modelAssetSizes.json';

export const ROBOT_MODEL_URL = '/models/AutoFinalRobo2.glb';
export interface ModelAssetState {
  url: string;
  phase: 'idle' | 'downloading' | 'preparing' | 'ready' | 'error';
  loadedBytes: number;
  totalBytes: number | null;
  remainingSeconds: number | null;
  error: string | null;
}
const states = new Map<string, ModelAssetState>();
const listeners = new Set<() => void>();
let notificationPending = false;
export function getModelAssetState(url: string): ModelAssetState {
  let state = states.get(url);
  if (!state) {
    state = { url, phase: 'idle', loadedBytes: 0, totalBytes: (sizes as Record<string, number>)[url] ?? null, remainingSeconds: null, error: null };
    states.set(url, state);
  }
  return state;
}
export function subscribeModelAssets(listener: () => void) {
  listeners.add(listener);
  return () => { listeners.delete(listener); };
}
function update(url: string, patch: Partial<ModelAssetState>) {
  states.set(url, { ...getModelAssetState(url), ...patch });
  // Loaders may start during a suspended render. Notify only after that render unwinds.
  if (!notificationPending) {
    notificationPending = true;
    queueMicrotask(() => { notificationPending = false; listeners.forEach(listener => listener()); });
  }
}
export function resetModelAsset(url: string) {
  update(url, { phase: 'idle', loadedBytes: 0, remainingSeconds: null, error: null });
}
/** Same loader/cache shared by every camera and inspection view. */
export class ProgressGLTFLoader extends GLTFLoader {
  override load(url: string, onLoad: (data: GLTF) => void, onProgress?: (event: ProgressEvent) => void, onError?: (error: unknown) => void): void {
    const started = performance.now();
    update(url, { phase: 'downloading', loadedBytes: 0, remainingSeconds: null, error: null });
    super.load(url, data => {
      const state = getModelAssetState(url);
      update(url, { phase: 'ready', loadedBytes: state.loadedBytes || state.totalBytes || 0, totalBytes: state.loadedBytes || state.totalBytes, remainingSeconds: null });
      onLoad(data);
    }, event => {
      // Three counts decoded response bytes. Bundled asset sizes use the same unit,
      // unlike Content-Length when the ESP serves gzip. This is asset progress, not wire traffic.
      const total = (sizes as Record<string, number>)[url] ?? null;
      const elapsed = (performance.now() - started) / 1000;
      const preparing = total !== null && event.loaded >= total;
      update(url, { phase: preparing ? 'preparing' : 'downloading', loadedBytes: event.loaded,
        totalBytes: total, remainingSeconds: !preparing && total && event.loaded > 0 && elapsed >= .75 ? Math.max(0, (total - event.loaded) * elapsed / event.loaded) : null });
      onProgress?.(event);
    }, error => {
      update(url, { phase: 'error', error: error instanceof Error ? error.message : 'No se pudo cargar el modelo.', remainingSeconds: null });
      onError?.(error);
    });
  }
}
