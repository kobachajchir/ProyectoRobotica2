import { Chevron } from '../../components/ui/Chevron';
import { useEffect, useRef, useState } from 'react';
import config from '../../../../AutoFinalRobo2-Contracts/contracts/ATMEGA_CONFIG_ACTIONS.json';
import { Button } from '../../components/ui/Button';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { useConnection } from '../connection/ConnectionContext';
import type { Afr2CommandName } from '../../protocol/wsEnvelope';
import { withoutAccents } from '../../utils/displayText';

// IDs/ranges come from the same catalogue as the AVR numeric editors.
const actions = config.actions.filter(a => a.config_access).map(action => ({
  ...action, label: action.label ? withoutAccents(action.label) : action.label,
}));
export function CalibrationPanel() {
  const { wsState, megaLinkState, capabilities, requestCommand, executeCommandWithToast, robotState, stopRecord, setOperationActivity, triggerEmergencyStop } = useConnection();
  const [action, setAction] = useState(4);
  const [value, setValue] = useState(0);
  const [feedback, setFeedback] = useState('Selecciona un parametro y consulta su valor actual.');
  const [busy, setBusy] = useState(false);
  const [loadedAction, setLoadedAction] = useState<number | null>(null);
  const epoch = useRef(0), locked = useRef(false), mounted = useRef(true);
  const selected = actions.find(a => a.id === action)!;
  const ready = wsState === 'ready' && megaLinkState === 'ready' && capabilities.configuration.implemented;
  const writable = ready && robotState?.stopped === true
    && stopRecord.status !== 'sending' && stopRecord.status !== 'unconfirmed-offline';
  const latest = useRef({ writable, wsState, megaLinkState }); latest.current = { writable, wsState, megaLinkState };
  useEffect(() => { ++epoch.current; setLoadedAction(null); }, [wsState, megaLinkState, stopRecord.lastIssuedAtMs]);
  useEffect(() => {
    mounted.current = true;
    const cancel = () => { ++epoch.current; if (mounted.current) setFeedback('Interaccion interrumpida. Volve a consultar antes de editar.'); setLoadedAction(null); };
    const onVisibility = () => { if (document.visibilityState !== 'visible') cancel(); };
    window.addEventListener('blur', cancel); document.addEventListener('visibilitychange', onVisibility);
    return () => { mounted.current = false; ++epoch.current; setOperationActivity('configuration', false); window.removeEventListener('blur', cancel); document.removeEventListener('visibilitychange', onVisibility); };
  }, [setOperationActivity]);
  const current = (ticket: number) => mounted.current && epoch.current === ticket && document.visibilityState === 'visible' && latest.current.wsState === 'ready' && latest.current.megaLinkState === 'ready';
  const read = async () => {
    if (!ready || locked.current) return;
    locked.current = true; setBusy(true); const ticket = ++epoch.current;
    try {
      const res = await requestCommand<{ value: number }>('robot.config.get', { action });
      if (!current(ticket)) return;
      if (res.status !== 'completed' || !Number.isInteger(res.payload.value)) throw new Error('El controlador no entrego un valor valido.');
      setValue(res.payload.value);setLoadedAction(action);setFeedback(`Valor recibido: ${res.payload.value} ${selected.unit ?? ''}`);
    } catch(e) {if (current(ticket)) setFeedback(e instanceof Error ? e.message : 'No se pudo consultar.');}
    finally {locked.current = false; if (mounted.current) setBusy(false);}
  };
  const write = async (command: Afr2CommandName, payload: Record<string, unknown>, labels: { loadingTitle: string; successTitle: string; acceptedTitle?: string }) => {
    if (!latest.current.writable || locked.current || document.visibilityState !== 'visible') return;
    locked.current = true; setBusy(true); const ticket = ++epoch.current;
    setOperationActivity('configuration', true);
    try {
      if (!current(ticket)) { void triggerEmergencyStop('safety'); return; }
      const state = await requestCommand<{ stopped: boolean }>('robot.state', {}, 2500);
      if (!current(ticket) || !latest.current.writable) return;
      if (state.status !== 'completed' || state.payload.stopped !== true) throw new Error('Se necesita confirmar salidas apagadas antes de editar.');
      const result = await executeCommandWithToast(command, payload, labels);
      if (current(ticket) && result) { setLoadedAction(null); setFeedback('Solicitud confirmada. Consulta nuevamente el parametro antes de otra edicion.'); }
    } catch (error) { if (current(ticket)) setFeedback(error instanceof Error ? error.message : 'No se pudo aplicar la configuracion.'); }
    finally { locked.current = false; setOperationActivity('configuration', false); if (mounted.current) setBusy(false); }
  };
  return <div className="afr2-glass-panel rounded-[var(--radius-md)] p-5 space-y-4 max-w-2xl">
    <h2 className="text-base font-semibold">Configuracion y calibracion</h2>
    <p className="afr2-help">Los cambios se aplican en RAM con las salidas apagadas. Guardar en EEPROM y descartar son acciones explicitas. Los Hall se calibran por rueda desde la OLED; marcar geometria no mide el robot.</p>
    <p className="afr2-help">Aplicar, guardar o descartar envia la accion directamente. Antes de escribir se vuelve a consultar que las salidas esten apagadas.</p>
    {!writable && <StatusNotice tone="info" title="Edicion bloqueada">Conecta el Mega y detene las salidas. Podes consultar sin modificar la configuracion.</StatusNotice>}
    <form className="space-y-4" onSubmit={e => {e.preventDefault();if (loadedAction === action) void write('robot.calibrate', {action, value}, {loadingTitle:'Aplicando parametro…',successTitle:'Parametro aplicado en RAM'});}}>
      <label className="block">Parametro
        <span className="afr2-select-wrap"><select disabled={busy} className="afr2-glass-field w-full min-h-11 px-3" value={action} onChange={e => {setAction(Number(e.target.value));setLoadedAction(null);setFeedback('Consulta el valor actual antes de editar.');}}>
          {actions.map(a => <option key={a.id} value={a.id}>{a.label ?? a.symbol} · {a.unit ?? ''}</option>)}
        </select><Chevron /></span>
      </label>
      <label className="block">Valor ({selected.min}…{selected.max})
        <input disabled={busy} className="afr2-glass-field w-full min-h-11 px-3" type="number" required min={selected.min} max={selected.max} step={action === 41 ? 2 : 1} value={value} onChange={e => setValue(Number(e.target.value))} />
      </label>
      <div className="afr2-action-row">
        <Button type="button" disabled={!ready || busy} onClick={() => void read()}>Consultar</Button>
        <Button type="submit" disabled={!writable || busy || loadedAction !== action || !Number.isInteger(value)}>Aplicar en RAM</Button>
      </div>
    </form>
    <p role="status" className="afr2-help">{feedback}</p>
    <p className="afr2-help" role="status">EEPROM: {robotState?.preferencesState === 1 ? 'guardando' : robotState?.preferencesState === 2 ? 'guardado confirmado por AVR' : robotState?.preferencesState === 3 ? 'fallo de guardado' : 'sin guardado confirmado en esta sesion'}.</p>
    <div className="afr2-action-row">
      <Button disabled={!writable || busy} onClick={() => void write('robot.line.calibrate',{confirm:true},{loadingTitle:'Confirmando linea…',successTitle:'Calibracion de linea confirmada'})}>Confirmar linea</Button>
      <Button disabled={!writable || busy} onClick={() => void write('robot.line.calibrate',{confirm:false},{loadingTitle:'Invalidando linea…',successTitle:'Calibracion de linea invalidada'})}>Invalidar linea</Button>
      <Button disabled={!writable || busy} onClick={() => void write('robot.preferences',{action:'save'},{loadingTitle:'Solicitando guardado…',successTitle:'Preferencias guardadas',acceptedTitle:'Guardado EEPROM en curso'})}>Guardar EEPROM</Button>
      <Button disabled={!writable || busy} onClick={() => void write('robot.preferences',{action:'discard'},{loadingTitle:'Descartando…',successTitle:'Configuracion guardada restaurada'})}>Descartar cambios</Button>
    </div>
  </div>;
}
