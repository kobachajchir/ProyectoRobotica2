import { createContext, useContext, useEffect, useRef, useState, type Dispatch, type SetStateAction } from 'react';
import { capturePoint, downloadPoints, frameSignature, loadPoints, parsePointLibrary, pointId, serializePoints, ARM_POINTS_KEY, type SavedArmPoint } from './armPointStorage';
import { modelProblems, robotModel } from './robotModel';
import type { ArmMotionPlan, MotionResult, MotionRequest } from './armMotion';
import { halt, type CollectorState } from './collectorSimulation';
import { motionEnvironment, pointBusy, pointContext } from './pointMotionContext';
import ArmMotionWorker from './armMotion.worker?worker&inline';

export function useArmPointsController(state: CollectorState, update: Dispatch<SetStateAction<CollectorState>>, kind: string) {
  const [loaded] = useState(loadPoints);
  const [points, setPoints] = useState(loaded.points), [storageIssue, setStorageIssue] = useState(loaded.issue);
  const storageBlocked = useRef(loaded.blocked);
  const [selectedId, setSelectedId] = useState<string | null>(null), [showPoints, setShowPoints] = useState(true);
  const [restoreOpening, setRestoreOpening] = useState(false), [calculating, setCalculating] = useState(false);
  const [preview, setPreview] = useState<ArmMotionPlan | null>(null), [message, setMessage] = useState('Guarda una postura para comenzar.');
  const worker = useRef<Worker | null>(null), generation = useRef(0), alive = useRef(true);
  const latest = useRef({ state, kind, selectedId, points, restoreOpening }); latest.current = { state, kind, selectedId, points, restoreOpening };
  const selected = points.find(p => p.id === selectedId);
  const context = selected ? pointContext(state, selected.id, selected.updatedAt, restoreOpening) : '';
  const contextRef = useRef(context); contextRef.current = context;
  const requestContext = useRef<string | null>(null);
  const lastRun = useRef<string | null>(null);
  const validPlan = preview?.context === context && !pointBusy(state) ? preview : null;
  const cancelCalculation = () => { ++generation.current; worker.current?.terminate(); worker.current = null; requestContext.current = null; setCalculating(false); };
  useEffect(() => {
    if (requestContext.current !== null && requestContext.current !== context) cancelCalculation();
    if (preview && preview.context !== context) setPreview(null);
  }, [context, preview]);
  useEffect(() => {
    const interrupt = () => { cancelCalculation(); setPreview(null); };
    window.addEventListener('blur', interrupt);
    const visibility = () => { if (document.visibilityState !== 'visible') interrupt(); };
    document.addEventListener('visibilitychange', visibility);
    return () => { window.removeEventListener('blur', interrupt); document.removeEventListener('visibilitychange', visibility); };
  }, []);
  useEffect(() => { alive.current = true; return () => { alive.current = false; ++generation.current; worker.current?.terminate(); }; }, []);
  useEffect(() => { cancelCalculation(); setPreview(null); }, [kind]);
  useEffect(() => {
    if (state.pointRun) lastRun.current = state.pointRun.id;
    else if (lastRun.current) { lastRun.current = null; setMessage(state.detail); }
  }, [state.pointRun, state.detail]);

  const commit = (next: SavedArmPoint[]) => {
    setPoints(next); setPreview(null);
    if (storageBlocked.current) return;
    try { localStorage.setItem(ARM_POINTS_KEY, serializePoints(next)); setStorageIssue(''); }
    catch { storageBlocked.current = true; setStorageIssue('No se pudo guardar en este navegador. Los puntos siguen en memoria: exporta un respaldo.'); }
  };
  const canTeach = !state.rigIssue && !pointBusy(state) && modelProblems(robotModel()).length === 0;
  const save = (name: string, replaceId?: string) => {
    if (!canTeach || !name.trim()) return;
    if (!replaceId && points.length >= 1000) { setMessage('La biblioteca admite hasta 1000 puntos. Exporta un respaldo.'); return; }
    const captured = capturePoint(name, state.joints, state.opening, kind);
    const old = points.find(p => p.id === replaceId);
    const point = old ? { ...captured, id: old.id, createdAt: old.createdAt } : captured;
    commit(old ? points.map(p => p.id === old.id ? point : p) : [...points, point]);
    setSelectedId(point.id); setMessage('Postura guardada. El destino conserva posicion y orientacion respecto del montaje.');
  };
  const rename = (name: string) => {
    if (!selected || !name.trim()) return;
    commit(points.map(p => p.id === selected.id ? { ...p, name: name.trim().slice(0, 80), updatedAt: new Date().toISOString() } : p));
  };
  const remove = () => { if (selected) { commit(points.filter(p => p.id !== selected.id)); setSelectedId(null); setMessage('Punto eliminado.'); } };
  const reviewFrames = () => {
    if (!selected || pointBusy(state)) return;
    commit(points.map(p => p.id === selected.id ? { ...p, updatedAt: new Date().toISOString(), source: { ...p.source, frameSignature: frameSignature() } } : p));
    setMessage('Marco revisado explicitamente. El destino numerico no cambio; calcula su trayectoria.');
  };
  const execute = (plan: ArmMotionPlan) => {
    const snapshot = latest.current, point = snapshot.points.find(p => p.id === plan.pointId);
    if (!point) return;
    update(s => {
      if (s.rigIssue || pointBusy(s) || s.cubes.some(c => c.location === 'gripper') || pointContext(s, point.id, point.updatedAt, snapshot.restoreOpening) !== plan.context) return s;
      const next = halt(s, 'Recorriendo el plan mostrado. Puedes cancelarlo.');
      return { ...next, jointTarget: [...plan.stages[0]], jointQueue: plan.stages.slice(1).map(q => [...q]),
        pointRun: { id: point.id, revision: robotModel().revision, modelId: robotModel().id, environment: motionEnvironment(s),
          opening: s.opening, finishOpening: snapshot.restoreOpening ? point.opening : null, plan } };
    });
    setPreview(null);
  };
  const calculate = (go = false) => {
    if (state.rigIssue || !selected || pointBusy(state) || state.cubes.some(c => c.location === 'gripper') || calculating || document.visibilityState !== 'visible') return;
    if (selected.source.frameSignature !== frameSignature()) { setMessage('Cambio el marco del montaje o del TCP. Revisa el destino antes de usarlo.'); return; }
    const problems = modelProblems(robotModel());
    if (problems.length) { setMessage(problems.join(' ')); return; }
    if (restoreOpening && (selected.opening < robotModel().gripper.min || selected.opening > robotModel().gripper.max)) { setMessage('La apertura guardada no esta dentro del rango del modelo activo. Desactiva restaurarla o actualiza el punto.'); return; }
    if (go && validPlan) { execute(validPlan); return; }
    cancelCalculation(); const id = generation.current, fingerprint = context;
    const model = robotModel();
    const taught = selected.source.jointIds.join('|') === model.joints.map(j => j.id).join('|') ? selected.source.angles : undefined;
    const payload: MotionRequest = { requestId: id, context: fingerprint, pointId: selected.id, model, from: [...state.joints],
      target: { position: selected.position, quaternion: selected.quaternion }, taught, opening: state.opening,
      finishOpening: restoreOpening ? selected.opening : undefined, environment: motionEnvironment(state) };
    setMessage('Calculando posicion, orientacion y recorrido…'); setCalculating(true); setPreview(null); requestContext.current = fingerprint;
    try {
      const job = new ArmMotionWorker(); worker.current = job;
      job.onmessage = (event: MessageEvent<{ requestId: number; context: string; result: MotionResult }>) => {
        job.terminate();
        if (!alive.current || event.data.requestId !== generation.current || event.data.context !== contextRef.current) return;
        worker.current = null; requestContext.current = null; setCalculating(false);
        if (!event.data.result.ok) { setMessage(event.data.result.message); return; }
        const plan = event.data.result.plan;
        setPreview(plan); setMessage(`Recorrido disponible · ${plan.duration.toFixed(1)} s · error ${(plan.positionError * 1000).toFixed(2)} mm / ${(plan.orientationError * 180 / Math.PI).toFixed(2)}°. Modelo aproximado.`);
        if (go) execute(plan);
      };
      job.onerror = event => { event.preventDefault(); job.terminate(); if (id !== generation.current || !alive.current) return;
        worker.current = null; requestContext.current = null; setCalculating(false); setMessage('No se pudo iniciar el calculo. El destino se conserva; puedes reintentar.'); };
      job.postMessage(payload);
    } catch { cancelCalculation(); setMessage('El navegador no pudo abrir el Worker de calculo. Los puntos se conservan.'); }
  };
  const importFile = async (file: File) => {
    try {
      if (file.size > 2 * 1024 * 1024) throw new Error('El archivo supera 2 MiB.');
      const imported = parsePointLibrary(await file.text());
      // Use latest library: file reading must not overwrite intervening edits.
      const existing = latest.current.points;
      if (existing.length + imported.length > 1000) throw new Error('La biblioteca resultante supera 1000 puntos.');
      const now = new Date().toISOString();
      commit([...existing, ...imported.map(p => ({ ...p, id: pointId(), createdAt: now, updatedAt: now }))]);
      setMessage(`${imported.length} puntos importados como registros nuevos.`);
    } catch (error) { setMessage(error instanceof Error ? error.message : 'No se pudo importar. La biblioteca se conserva.'); }
  };
  const retryStorage = () => {
    try { localStorage.setItem(ARM_POINTS_KEY, serializePoints(points)); storageBlocked.current = false; setStorageIssue(''); }
    catch { setStorageIssue('Sigue sin ser posible guardar. Exporta los puntos en memoria.'); }
  };
  return { points, selected, selectedId, setSelectedId, showPoints, setShowPoints, restoreOpening, setRestoreOpening,
    calculating, plan: validPlan, displayPlan: state.pointRun?.plan ?? validPlan, message, storageIssue, canTeach, save, rename, remove, reviewFrames, calculate,
    cancel: () => { cancelCalculation(); setPreview(null); update(s => halt(s, 'Recorrido cancelado; postura conservada.')); setMessage('Recorrido cancelado.'); },
    importFile, exportFile: () => downloadPoints(serializePoints(points)),
    exportOriginal: loaded.raw ? () => downloadPoints(loaded.raw!, 'respaldo-original-puntos.json') : null, retryStorage };
}
export type ArmPointsController = ReturnType<typeof useArmPointsController>;
export const ArmPointsContext = createContext<ArmPointsController | null>(null);
export function useArmPoints() { const value = useContext(ArmPointsContext); if (!value) throw new Error('Biblioteca de puntos no disponible.'); return value; }
