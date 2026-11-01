import { useSyncExternalStore } from 'react';
import { getModelAssetState, ROBOT_MODEL_URL, subscribeModelAssets } from './modelAssetLoading';
import './modelAssetProgress.css';

function bytes(value: number) {
  if (value < 1024) return `${value} B`;
  return `${(value / (value < 1024 * 1024 ? 1024 : 1024 * 1024)).toLocaleString('es-AR', { maximumFractionDigits: 1 })} ${value < 1024 * 1024 ? 'KiB' : 'MiB'}`;
}
/** Reusable outside Canvas/Suspense. Multiple views observe one cached download. */
export function ModelAssetProgress({ url = ROBOT_MODEL_URL, label = 'Modelo 3D', hideWhenReady = false }: { url?: string; label?: string; hideWhenReady?: boolean }) {
  const state = useSyncExternalStore(subscribeModelAssets, () => getModelAssetState(url));
  if (hideWhenReady && state.phase === 'ready') return null;
  const total = state.totalBytes;
  const percent = total ? Math.min(100, Math.floor(state.loadedBytes * 100 / total)) : null;
  const active = state.phase === 'downloading' || state.phase === 'preparing';
  const title = state.phase === 'preparing' ? 'Preparando modelo' : state.phase === 'downloading' ? 'Descargando modelo' : state.phase === 'error' ? 'Error de carga' : label;
  return <div className="afr2-model-asset-progress" aria-busy={active}>
    <div><strong>{title}</strong><span title={total ? `${total.toLocaleString('es-AR')} bytes del archivo` : undefined}>{total ? bytes(total) : 'Tamaño por determinar'}</span></div>
    {active && <>
      <progress max={100} value={percent ?? undefined} aria-label={`Descarga de ${label}`} />
      <div><span>{bytes(state.loadedBytes)}{total ? ` de ${bytes(total)} · ${percent}%` : ' recibidos'}</span>
        <span>{state.phase === 'preparing' ? 'Descarga completa' : state.remainingSeconds !== null ? `~${Math.max(1, Math.ceil(state.remainingSeconds))} s restantes` : 'Calculando tiempo…'}</span></div>
    </>}
    {state.phase === 'error' && <p role="alert">{state.error}</p>}
  </div>;
}
