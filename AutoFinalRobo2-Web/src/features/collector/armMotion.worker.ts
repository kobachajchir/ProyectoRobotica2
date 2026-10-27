import { planMotion, type MotionRequest } from './armMotion';

// Vite bundles this module as an independent worker. No DOM/React/physical IO.
const workerScope = self as unknown as { onmessage: ((event: MessageEvent<MotionRequest>) => void) | null; postMessage: (value: unknown) => void };
workerScope.onmessage = event => {
  const { requestId, context } = event.data;
  try { workerScope.postMessage({ requestId, context, result: planMotion(event.data) }); }
  catch { workerScope.postMessage({ requestId, context, result: { ok: false, code: 'model', message: 'No se pudo calcular con este descriptor de modelo.' } }); }
};
