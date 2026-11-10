import { useEffect, useRef, useState } from 'react';
import { Link } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { FloatingModal, type ModalOrigin } from '../../components/ui/FloatingModal';
import { UiIcon } from '../../components/ui/UiIcon';
import { stateAge } from '../../protocol/robotState';
import { parseFaultDiagnostics, ROBOT_FAULT_HELP, ROBOT_FAULT_TEXT, type RobotFaultDiagnostics } from '../../protocol/robotFaults';
import { useConnection } from './ConnectionContext';
import '../../styles/robot-alerts.css';

interface AlertItem { id: string; title: string; detail: string; steps: string[]; tone: 'warning' | 'error' }
interface Props { open: boolean; origin: ModalOrigin | null; onOpen: (origin: ModalOrigin) => void; onClose: () => void; onOpenOled: () => void }

/** Observes the shared state reader; does not add robot.state polling. */
export function RobotAlerts({ open, origin, onOpen, onClose, onOpenOled }: Props) {
  const { wsState, megaLinkState, robotState, stopRecord, stopAvailable, triggerEmergencyStop, requestCommand, refreshSystemInfo } = useConnection();
  const [now, setNow] = useState(Date.now());
  const [busy, setBusy] = useState(false), [message, setMessage] = useState('');
  const [confirmRestart, setConfirmRestart] = useState(false);
  const [diagnostics,setDiagnostics]=useState<RobotFaultDiagnostics|null>(null);
  const [diagnosticError,setDiagnosticError]=useState('');
  const pending = useRef(false);
  const lifetime = useRef(0);
  useEffect(() => { const timer = window.setInterval(() => setNow(Date.now()), 1000); return () => clearInterval(timer); }, []);
  useEffect(() => { lifetime.current++; setMessage(''); setConfirmRestart(false); }, [open]);
  const age = stateAge(robotState, Math.max(now, Date.now()));
  const connected = wsState === 'ready' && megaLinkState === 'ready';
  const fresh = connected && age !== null && age < 6000;
  const fault = fresh ? robotState?.fault ?? null : null;
  useEffect(()=>{
    let cancelled=false;setDiagnostics(null);setDiagnosticError('');
    if(!open || !connected)return;
    void requestCommand('robot.fault.get',{},2500).then(response=>{
      if(cancelled)return;
      const data=parseFaultDiagnostics(response.payload);
      if(response.status!=='completed' || !data){setDiagnosticError('El controlador no devolvió un diagnóstico válido.');return;}
      setDiagnostics(data);
    }).catch(error=>{if(!cancelled)setDiagnosticError(error instanceof Error?error.message:'No se pudo consultar el diagnóstico.');});
    return()=>{cancelled=true;};
  },[open,connected,fault,requestCommand]);
  const alerts: AlertItem[] = [];
  if (wsState !== 'ready') alerts.push({ id: 'web-link', title: 'Sin comunicación Web con el auto', tone: 'warning',
    detail: wsState === 'connecting' || wsState === 'handshaking' ? 'La conexión se está negociando.' : 'La Web no tiene un enlace vigente con el ESP.',
    steps: ['Comprobá la red Wi-Fi o el punto de acceso del auto. Abrí la pill de Wi-Fi para ver el estado de red.'] });
  else if (megaLinkState !== 'ready') alerts.push({ id: 'mega-link', title: 'Enlace con ATmega no disponible', tone: 'warning',
    detail: 'El ESP responde, pero no hay una negociación vigente con el controlador de motores.',
    steps: ['En el auto, revisá la alimentación del Mega y la conexión UART del shield. Consultá Conexión para ver el diagnóstico.'] });
  else if (!fresh || fault === null) alerts.push({ id: 'state', title: 'Estado del controlador sin actualizar', tone: 'warning',
    detail: 'No hay una lectura reciente que permita confirmar los fallos o las salidas. No se interpreta como ausencia de fallos.',
    steps: ['Esperá la actualización del estado. Si no llega, revisá el enlace entre ESP y ATmega.'] });
  else if (fault > 0 || robotState?.mode === 'fault') {
    const help = ROBOT_FAULT_HELP[fault];
    alerts.push({ id: 'robot-fault', title: fault > 0 ? `Fallo ${fault} · ${ROBOT_FAULT_TEXT[fault] ?? 'código no reconocido'}` : 'Controlador en modo fallo', tone: 'error',
      detail: help?.detail ?? 'El controlador declaró un fallo. Consultá su diagnóstico antes de volver a operar.',
      steps: [...(help?.steps ?? []), 'También podés limpiar desde el auto: usá Menú principal → Estado → Fallos → Limpiar fallo con las salidas detenidas. Girá el encoder para elegir Detener o Limpiar fallo; corto confirma.'] });
  }
  if (stopRecord.status === 'unconfirmed-offline') alerts.push({ id: 'stop', title: 'Parada sin confirmar', tone: 'error',
    detail: stopRecord.detail ?? 'No llegó una confirmación de STOP del controlador.',
    steps: ['No asumas que los motores se detuvieron. Intervení desde el auto y comprobá las salidas antes de reanudar.'] });
  const count = alerts.length, hasFault = alerts.some(item => item.id === 'robot-fault');
  const restartReady = fresh && robotState?.stopped === true && robotState.configurationDirty === false && robotState.preferencesState !== 1;
  const clearReady=fresh && fault!==null && fault>0 && robotState?.stopped===true && diagnostics?.stopped===true && diagnostics.fault===fault;
  const act = async (operation: () => Promise<string>) => {
    if (!open || pending.current) return;
    pending.current = true; setBusy(true); setMessage(''); const run = lifetime.current;
    try { const text = await operation(); if (run === lifetime.current) setMessage(text); }
    catch (error) { if (run === lifetime.current) setMessage(error instanceof Error ? error.message : 'No se pudo completar la acción.'); }
    finally { pending.current = false; setBusy(false); }
  };
  return <>
    <button type="button" className="afr2-network-pill afr2-alert-pill" data-tone={alerts.some(item => item.tone === 'error') ? 'error' : count ? 'warning' : 'neutral'}
      aria-haspopup="dialog" aria-expanded={open} aria-label={`Alertas: ${count}. Abrir fallos y acciones`} title={`${count} ${count === 1 ? 'aviso activo' : 'avisos activos'}`}
      onClick={event => onOpen({ element: event.currentTarget, keyboard: event.detail === 0 })}>
      <UiIcon name="warning" /><span>Alertas</span><strong className="afr2-alert-count">{count}</strong>
    </button>
    <FloatingModal open={open} origin={origin} onClose={onClose} title="Fallos y avisos" description="Estado actual del auto y acciones para recuperarlo." headerStatus={<span>{count} {count === 1 ? 'aviso activo' : 'avisos activos'}</span>}>
      <div className="afr2-page-stack afr2-alert-details">
        <div className="afr2-action-row">
          {hasFault && <Button variant="primary" disabled={!clearReady || busy} onClick={()=>void act(async()=>{
            const ageNow=stateAge(robotState,Date.now());
            if(!clearReady || ageNow===null || ageNow>=6000 || fault===null)throw new Error('Se necesita un estado reciente con salidas detenidas.');
            const response=await requestCommand('robot.fault.clear',{confirm:true,expectedFault:fault},3000);
            const data=parseFaultDiagnostics(response.payload);
            if(response.status!=='completed' || !data || !data.cleared || data.fault!==0 || !data.stopped)throw new Error('El controlador no confirmó la limpieza del fallo.');
            setDiagnostics(data);setDiagnosticError('');
            return 'Fallo limpiado por el ATmega. Las salidas siguen detenidas; la lista se actualizará con el próximo estado.';
          })}>Limpiar fallo</Button>}
          <Button variant="emergency" icon={<UiIcon name="stop" />} disabled={!stopAvailable || busy} onClick={() => void act(async () => await triggerEmergencyStop('fault-panel') ? 'STOP confirmado por el controlador.' : 'STOP sin confirmar. Revisá el auto antes de continuar.')}>Detener salidas</Button>
          <Button icon={<UiIcon name="refresh" />} disabled={wsState !== 'ready' || busy} onClick={() => void act(async () => {
            const response=await requestCommand('robot.fault.get',{},2500),data=parseFaultDiagnostics(response.payload);
            if(response.status!=='completed' || !data)throw new Error('No llegó un diagnóstico válido del ATmega.');
            setDiagnostics(data);setDiagnosticError('');await refreshSystemInfo();
            return 'Diagnóstico consultado. El estado del controlador se actualiza automáticamente.';
          })}>Actualizar información</Button>
          {hasFault && <Button disabled={!restartReady || busy} onClick={() => setConfirmRestart(true)}>Reiniciar controlador</Button>}
        </div>
        {diagnosticError && <p className="afr2-help" role="status">Diagnóstico no disponible: {diagnosticError} La limpieza Web requiere el firmware actualizado del ATmega y del ESP.</p>}
        {hasFault && !clearReady && <p className="afr2-help">Para limpiar se necesitan salidas detenidas, estado reciente y diagnóstico disponible del ATmega. Primero usá Detener salidas y Actualizar información.</p>}
        {message && <p className="afr2-help" role="status">{message}</p>}
        {count === 0 ? <p className="afr2-alert-empty"><UiIcon name="check" />Sin fallos ni avisos activos en la última lectura.</p> : <ul className="afr2-alert-list">
          {alerts.map(item => <li key={item.id} className="afr2-alert-item" data-tone={item.tone}>
            <h3><UiIcon name="warning" size={20} />{item.title}</h3><p>{item.detail}</p>
            <ol>{item.steps.map(step => <li key={step}>{step}</li>)}</ol>
          </li>)}
        </ul>}
        <p className="afr2-help">El contador reúne avisos activos, no un historial. El ATmega informa un código de fallo a la vez. {fresh && age !== null ? `Estado recibido hace ${Math.round(age)} ms.` : 'Sin confirmación reciente del controlador.'}</p>
        {diagnostics && diagnostics.lateCount>0 && <section aria-label="Diagnóstico del último lazo demorado" className="afr2-alert-item">
          <h3>Último Fallo 1 registrado</h3>
          <p>Separación entre controles: <strong>{diagnostics.controlGapMs} ms</strong> · {diagnostics.lateCount} {diagnostics.lateCount===1?'registro':'registros'} desde el arranque.</p>
          <dl className="afr2-network-details">
            <div><dt>Ciclo asociado</dt><dd>{diagnostics.timing.totalMs} ms</dd></div>
            <div><dt>Servicios y control</dt><dd>{diagnostics.timing.appMs} ms</dd></div>
            <div><dt>UART y diagnóstico USB</dt><dd>{diagnostics.timing.uartMs} ms</dd></div>
            <div><dt>Entrada y encoder</dt><dd>{diagnostics.timing.inputMs} ms</dd></div>
            <div><dt>Dibujo de pantalla</dt><dd>{diagnostics.timing.renderMs} ms</dd></div>
            <div><dt>Servicio I²C/OLED</dt><dd>{diagnostics.timing.ioMs} ms</dd></div>
          </dl>
          <p className="afr2-help">Tiempos registrados por el ATmega al detectar la demora. Permiten orientar el diagnóstico; no identifican por sí solos una causa. Se conservan al limpiar.</p>
        </section>}
        {hasFault && !restartReady && <p className="afr2-help">Para reiniciar desde la Web se necesitan salidas apagadas y un estado reciente sin cambios de configuración pendientes. Resolvé Guardar/Descartar en el auto si corresponde.</p>}
        {confirmRestart && hasFault && <div className="afr2-alert-confirm" role="group" aria-label="Confirmar reinicio del controlador">
          <strong>¿Reiniciar el ATmega?</strong><p>Se interrumpirá el enlace mientras arranca. Las salidas quedarán apagadas; no se retomará la marcha automáticamente. Reiniciar no corrige por sí solo una causa de cableado o una demora persistente.</p>
          <div className="afr2-action-row"><Button disabled={busy} onClick={() => setConfirmRestart(false)}>Cancelar</Button>
            <Button variant="danger" disabled={!restartReady || busy} onClick={() => { setConfirmRestart(false); void act(async () => {
              const latestAge = stateAge(robotState, Date.now());
              if (!restartReady || latestAge === null || latestAge >= 6000) throw new Error('El estado cambió. Actualizá antes de reiniciar.');
              const response = await requestCommand('system.reboot', { target: 'atmega', confirm: true }, 4000);
              return response.status === 'accepted' ? 'Reinicio aceptado. Esperá el nuevo estado antes de operar.' : 'Solicitud confirmada. Esperá el nuevo estado del controlador.';
            }); }}>Confirmar reinicio</Button></div>
        </div>}
        <div className="afr2-action-row">
          <Link className="afr2-developer-back" to="/puesta-a-punto" onClick={onClose}>Ir a Puesta a punto<UiIcon name="arrow" size={16} /></Link>
          <Link className="afr2-developer-back" to="/conexion" onClick={onClose}>Ver conexión<UiIcon name="arrow" size={16} /></Link>
          <Button variant="subtle" onClick={onOpenOled}>Ver OLED del auto</Button>
        </div>
      </div>
    </FloatingModal>
  </>;
}
