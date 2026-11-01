import { useEffect, useId, useRef, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { CollapsiblePanel } from '../../components/ui/CollapsiblePanel';
import { useCollector } from './CollectorContext';
import { useArmPoints } from './ArmPointsContext';
import { frameSignature } from './armPointStorage';
import { pointBusy } from './pointMotionContext';
import { modelProblems, robotModel, ROBOT_MODELS } from './robotModel';

export function SavedArmPointsPanel() {
  const { state, changeModel } = useCollector(), library = useArmPoints();
  const [expanded, setExpanded] = useState(false), [name, setName] = useState('');
  const [rename, setRename] = useState(''), [confirm, setConfirm] = useState<'update' | 'delete' | 'storage' | 'frame' | null>(null);
  const file = useRef<HTMLInputElement>(null), uid = useId();
  const model = robotModel(), point = library.selected, busy = pointBusy(state);
  const issue = [...modelProblems(model), ...(state.rigIssue ? [state.rigIssue] : [])].join(' ');
  const frameChanged = !!point && point.source.frameSignature !== frameSignature();
  const disabled = busy || library.calculating || !!issue || state.cubes.some(c => c.location === 'gripper');
  useEffect(() => { setRename(point?.name ?? ''); setConfirm(null); }, [point?.id]);
  const confirmAction = () => {
    if (confirm === 'delete') library.remove();
    if (confirm === 'update' && point) library.save(point.name, point.id);
    if (confirm === 'storage') library.retryStorage();
    if (confirm === 'frame') library.reviewFrames();
    setConfirm(null);
  };
  return <CollapsiblePanel className="afr2-arm-points" title="Puntos guardados" subtitle="Enseña una postura y recupera su destino" expanded={expanded} onToggle={() => setExpanded(v => !v)}>
    <div className="afr2-arm-points__model"><div><span>Modelo activo</span><strong>{model.label}</strong><small>{model.revision} · TCP respecto del montaje</small></div>
      {ROBOT_MODELS.length > 1 && <label htmlFor={`${uid}-model`}>Cambiar modelo<select id={`${uid}-model`} value={model.id} onChange={e => changeModel(e.target.value)}>{ROBOT_MODELS.map(m => <option value={m.id} key={m.id}>{m.label}</option>)}</select></label>}
    </div>
    {issue && <p className="afr2-arm-points__notice" role="status">Movimiento deshabilitado: {issue}</p>}
    <form className="afr2-arm-points__capture" onSubmit={event => { event.preventDefault(); library.save(name || `Punto ${library.points.length + 1}`); setName(''); }}>
      <label htmlFor={`${uid}-new`}>Nombre del nuevo punto<input id={`${uid}-new`} className="afr2-glass-field" maxLength={80} value={name} placeholder={`Punto ${library.points.length + 1}`} onChange={e => setName(e.target.value)} /></label>
      <Button type="submit" variant="primary" disabled={!library.canTeach || !!issue}>Guardar postura actual</Button>
    </form>
    <div className="afr2-arm-points__toolbar">
      <label><input type="checkbox" checked={library.showPoints} onChange={e => library.setShowPoints(e.target.checked)} /> Mostrar puntos en 3D</label>
      <Button size="sm" onClick={library.exportFile}>Exportar</Button><Button size="sm" onClick={() => file.current?.click()}>Importar</Button>
      <input hidden type="file" accept=".json,application/json" ref={file} onChange={e => { const selected = e.target.files?.[0]; if (selected) void library.importFile(selected); e.target.value = ''; }} />
    </div>
    {library.storageIssue && <div className="afr2-arm-points__notice"><p role="status">{library.storageIssue}</p>
      {library.exportOriginal && <Button size="sm" onClick={library.exportOriginal}>Resguardar contenido original</Button>}
      <Button size="sm" onClick={() => setConfirm('storage')}>Guardar biblioteca en memoria como nueva copia local</Button></div>}
    <div className="afr2-arm-points__layout"><div className="afr2-arm-points__list" role="group" aria-label="Elegir punto guardado">
      {!library.points.length && <p className="afr2-collector-hint">Move las articulaciones y guarda tu primera postura. Se conservara en este navegador.</p>}
      {library.points.map(p => <button type="button" key={p.id} aria-pressed={p.id === library.selectedId} onClick={() => library.setSelectedId(p.id)}>
        <strong>{p.name}</strong><span>{p.position.map(v => (v * 1000).toFixed(1)).join(' / ')} mm</span>
        <small>{p.source.frameSignature === frameSignature() ? 'Destino conservado · calcular alcance' : 'Marco pendiente de revision'}</small>
      </button>)}
    </div>
    {point && <div className="afr2-arm-points__detail">
      <label htmlFor={`${uid}-rename`}>Nombre del punto<input id={`${uid}-rename`} className="afr2-glass-field" maxLength={80} value={rename} onChange={e => setRename(e.target.value)} /></label>
      <div className="afr2-action-row"><Button size="sm" disabled={!rename.trim()} onClick={() => library.rename(rename)}>Renombrar</Button>
        <Button size="sm" disabled={!library.canTeach || !!issue} onClick={() => setConfirm('update')}>Actualizar con postura actual</Button>
        <Button size="sm" variant="danger" onClick={() => setConfirm('delete')}>Eliminar</Button></div>
      <dl className="afr2-arm-points__facts"><div><dt>Destino XYZ</dt><dd>{point.position.map(v => (v * 1000).toFixed(1)).join(' / ')} mm</dd></div>
        <div><dt>Orientacion XYZW</dt><dd>{point.quaternion.map(v => v.toFixed(3)).join(' / ')}</dd></div>
        <div><dt>Apertura guardada</dt><dd>{point.opening.toFixed(1)} mm</dd></div>
        <div><dt>Modelo de origen</dt><dd>{point.source.modelId} · {point.source.revision}</dd></div></dl>
      <details><summary>Angulos enseñados (referencia)</summary><p>{point.source.angles.map((v, i) => `${point.source.jointIds[i]}: ${v.toFixed(1)}°`).join(' · ')}</p></details>
      {frameChanged && <div className="afr2-arm-points__notice"><p>Cambio la definicion del montaje o de la herramienta. Revisa que estas coordenadas sigan representando tu destino.</p><Button disabled={busy} onClick={() => setConfirm('frame')}>Revisar y aceptar el marco actual</Button></div>}
      <label className="afr2-arm-points__restore"><input type="checkbox" disabled={busy} checked={library.restoreOpening} onChange={e => library.setRestoreOpening(e.target.checked)} /> Restaurar apertura al llegar</label>
      <div className="afr2-arm-points__actions"><Button disabled={disabled || frameChanged} onClick={() => library.calculate()}>Calcular trayectoria</Button>
        <Button variant="primary" disabled={disabled || frameChanged} onClick={() => library.calculate(true)}>Ir al punto</Button></div>
      {state.cubes.some(c => c.location === 'gripper') && <p className="afr2-collector-hint">Deposita el cubo sujeto antes de recuperar puntos.</p>}
    </div>}
    </div>
    {confirm && <div className="afr2-arm-points__confirm" role="group" aria-label="Confirmar cambio"><p>{confirm === 'delete' ? '¿Eliminar este punto?' : confirm === 'update' ? '¿Reemplazar este destino por la postura actual?' : confirm === 'frame' ? '¿Interpretar el destino guardado en el marco actual sin cambiar sus coordenadas?' : '¿Reemplazar el contenido local por los puntos que estan en memoria? Resguarda primero el original si lo necesitas.'}</p>
      <Button onClick={confirmAction}>Confirmar</Button><Button onClick={() => setConfirm(null)}>Volver</Button></div>}
    <div className="afr2-arm-points__status" role="status" aria-live="polite"><p>{state.pointRun ? state.detail : library.message}</p>
      {(library.calculating || state.jointTarget) && <Button variant="danger" onClick={library.cancel}>Cancelar recorrido</Button>}
      {!state.pointRun && <small>{state.detail}</small>}
    </div>
    <p className="afr2-collector-hint">Los destinos siguen al montaje; sus angulos se recalculan para cada modelo. Trayectorias articulares con restricciones aproximadas. Al recargar no se inicia ningun movimiento.</p>
  </CollapsiblePanel>;
}
