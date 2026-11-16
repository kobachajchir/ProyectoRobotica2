/**
 * AFR2-20261111-03. Descarga los modelos 3D en segundo plano en cuanto la conexión con el auto queda estable, con un toast
 * «Descargando modelo 3D» que muestra el avance. Si el enlace se cae a mitad de camino la descarga se cancela y, cuando vuelve
 * a estar estable, empieza de nuevo el modelo que faltaba (los ya completos no se vuelven a pedir). Errores HTTP o de red
 * se reintentan hasta MAX_ATTEMPTS veces con espera creciente; después se avisa y cada página 3D lo pide por su cuenta.
 */
import { useEffect, useRef } from 'react';
import { useConnection } from '../../features/connection/ConnectionContext';
import { notify } from '../notifications/notify';
import { getModelAssetState } from './modelAssetLoading';
import { announcePrefetch, downloadModel, MAX_ATTEMPTS, PREFETCH_TARGETS, prefetchComplete, prefetchedModel, releasePrefetch, STABLE_MS, type PrefetchTarget } from './modelPrefetch';

const TOAST_ID = 'afr2-model-prefetch';
const kb = (bytes: number) => `${Math.round(bytes / 1024).toLocaleString('es-AR')} KB`;
const sleep = (ms: number, signal: AbortSignal) => new Promise<void>((resolve, reject) => {
  const timer = setTimeout(resolve, ms);
  signal.addEventListener('abort', () => { clearTimeout(timer); reject(new DOMException('aborted', 'AbortError')); }, { once: true });
});

export function ModelPrefetcher() {
  const { wsState } = useConnection();
  const running = useRef<AbortController | null>(null);
  const finished = useRef(false);

  useEffect(() => {
    if (wsState !== 'ready' || finished.current) return;
    const controller = new AbortController();
    const stable = setTimeout(() => { running.current = controller; void run(controller.signal); }, STABLE_MS);
    return () => {
      // Link lost (or page leaving): cancel what is in flight; a loader that was waiting downloads by itself.
      clearTimeout(stable); controller.abort();
      if (running.current === controller) {
        running.current = null;
        if (!finished.current) notify.info({ id: TOAST_ID, title: 'Descarga del modelo 3D en pausa', description: 'Se retoma, desde el modelo que faltaba, cuando la conexión vuelva a estar estable.', duration: 4000 });
      }
    };
  }, [wsState]);

  const run = async (signal: AbortSignal) => {
    const pending = PREFETCH_TARGETS.filter(target => !prefetchedModel(target.url) && getModelAssetState(target.url).phase !== 'ready');
    if (!pending.length) { finished.current = true; return; }
    announcePrefetch(pending);
    const totalBytes = pending.reduce((sum, target) => sum + (getModelAssetState(target.url).totalBytes ?? 0), 0);
    let doneBytes = 0, lastToast = 0;
    const show = (target: PrefetchTarget, loaded: number, total: number | null, force = false) => {
      const now = performance.now();
      if (!force && now - lastToast < 250) return;
      lastToast = now;
      const overall = totalBytes ? Math.min(100, Math.round((doneBytes + loaded) * 100 / totalBytes)) : null;
      notify.loading({ id: TOAST_ID, title: `Descargando modelo 3D${overall === null ? '' : ` · ${overall} %`}`,
        description: `${target.label}: ${kb(loaded)}${total ? ` de ${kb(total)}` : ''}. Las páginas 3D van a abrir sin esperar.` });
    };
    try {
      for (const target of pending) {
        for (let attempt = 1; ; attempt++) {
          if (signal.aborted) { PREFETCH_TARGETS.forEach(t => { if (!prefetchedModel(t.url)) releasePrefetch(t.url); }); return; }
          try {
            show(target, 0, getModelAssetState(target.url).totalBytes, true);
            const data = await downloadModel(target, signal, show);
            doneBytes += data.byteLength;
            break;
          } catch (error) {
            if (signal.aborted) { PREFETCH_TARGETS.forEach(t => { if (!prefetchedModel(t.url)) releasePrefetch(t.url); }); return; } // reconnect → start this model again
            if (attempt >= MAX_ATTEMPTS) {
              PREFETCH_TARGETS.forEach(t => releasePrefetch(t.url));
              notify.warning({ id: TOAST_ID, title: 'No se pudo descargar el modelo 3D',
                description: `${error instanceof Error ? error.message : 'Error de red'}. Se vuelve a intentar al reconectar; cada página 3D también lo pide sola.` });
              return;
            }
            notify.loading({ id: TOAST_ID, title: 'Reintentando la descarga del modelo 3D', description: `${target.label}: intento ${attempt + 1} de ${MAX_ATTEMPTS}. Empieza de nuevo.` });
            await sleep(1500 * attempt, signal);
          }
        }
      }
      if (prefetchComplete()) {
        finished.current = true;
        notify.success({ id: TOAST_ID, title: 'Modelo 3D descargado', description: 'Auto, brazo y cubo listos para las páginas 3D.', duration: 3500 });
      }
    } catch {
      /* aborted while waiting between attempts: resumes on the next stable connection */
      PREFETCH_TARGETS.forEach(t => { if (!prefetchedModel(t.url)) releasePrefetch(t.url); });
    }
  };
  return null;
}
