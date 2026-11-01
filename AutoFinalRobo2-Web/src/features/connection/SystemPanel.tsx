/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { useEffect, useRef, useState } from 'react';
import { useConnection } from './ConnectionContext';
import { Disclosure } from '../../components/ui/Disclosure';
import { Button } from '../../components/ui/Button';
import { FloatingModal, type ModalOrigin } from '../../components/ui/FloatingModal';
import { CalibrationPanel } from '../control/CalibrationPanel';
import { BatteryDetails } from './BatteryStatus';

const INFO_LABELS: Record<string, string> = { version:'Version', compiledAt:'Compilacion', compiler:'Compilador', revision:'Revision', core:'Arduino Core', sdk:'SDK', bootId:'Identidad de arranque', resetReason:'Causa del reinicio', resetCauseBits:'Causa de reset (bits)', uptimeMs:'Tiempo activo (ms)', clockMHz:'Reloj (MHz)', sketchBytes:'Programa (bytes)', freeHeapBytes:'Heap libre (bytes)', flashBytes:'Flash detectada (bytes)', architecture:'Arquitectura', extensionVersion:'Protocolo EX01' };
export function SystemPanel() {
  const { wsState, megaLinkState, systemInfo, systemInfoUpdatedAtMs, bootReports, refreshSystemInfo, requestCommand, connectionDiagnostics, lastSocketClose } = useConnection();
  const [refreshing, setRefreshing] = useState(false);
  const [busy, setBusy] = useState<string | null>(null);
  const [message, setMessage] = useState('');
  const [restartTarget, setRestartTarget] = useState<'esp' | 'atmega' | null>(null);
  const [restartOrigin, setRestartOrigin] = useState<ModalOrigin | null>(null);
  const cancelRestart = useRef<HTMLButtonElement>(null);
  const restartPending = useRef(false);
  useEffect(() => {
    if (wsState !== 'ready' || (restartTarget === 'atmega' && megaLinkState !== 'ready')) setRestartTarget(null);
  }, [wsState, megaLinkState, restartTarget]);
  const filesystem = systemInfo?.filesystem && typeof systemInfo.filesystem === 'object'
    ? systemInfo.filesystem as Record<string, unknown> : null;
  const filesystemRows: [string, string][] = [
    ['LittleFS', typeof filesystem?.mounted === 'boolean' ? filesystem.mounted ? 'Montado' : 'No montado' : 'Sin informacion'],
    ['Paquete Web', typeof filesystem?.webReady === 'boolean' ? filesystem.webReady ? 'Disponible' : 'No disponible' : 'Sin informacion'],
    ['Diagnostico', typeof filesystem?.fault === 'string' ? filesystem.fault : '—'],
    ['Ocupado / capacidad', typeof filesystem?.totalBytes === 'number' && typeof filesystem?.usedBytes === 'number' ? `${filesystem.usedBytes.toLocaleString('es-AR')} / ${filesystem.totalBytes.toLocaleString('es-AR')} bytes` : 'Sin informacion'],
    ['Version Web', typeof filesystem?.webVersion === 'string' ? filesystem.webVersion : 'No informada por el paquete'],
    ['Empaquetado', typeof filesystem?.packagedAt === 'string' && Number.isFinite(Date.parse(filesystem.packagedAt)) ? new Date(filesystem.packagedAt).toLocaleString('es-AR') : 'Sin informacion'],
    ['Perfil', typeof filesystem?.buildProfile === 'string' ? filesystem.buildProfile : 'Sin informacion'],
    ['Activos / comprimidos', typeof filesystem?.assetCount === 'number' && typeof filesystem?.gzipBytes === 'number' ? `${filesystem.assetCount} / ${filesystem.gzipBytes.toLocaleString('es-AR')} bytes` : 'Sin informacion'],
    ['SHA-256 del indice declarado', typeof filesystem?.indexSha256 === 'string' ? filesystem.indexSha256 : 'Sin informacion'],
  ];
  const restart = async (target: 'esp' | 'atmega') => {
    if (restartPending.current || wsState !== 'ready' || (target === 'atmega' && megaLinkState !== 'ready')) return;
    restartPending.current = true;
    setRestartTarget(null);
    setBusy(target); setMessage('');
    try {
      const response = await requestCommand('system.reboot', { target, confirm: true }, 4000);
      setMessage(response.status === 'accepted' ? 'Reinicio aceptado. Esperando el aviso de arranque y la reconexion.' : 'Solicitud confirmada.');
    } catch (error) { setMessage(error instanceof Error ? error.message : 'No se pudo solicitar el reinicio.'); }
    finally { restartPending.current = false; setBusy(null); }
  };
  return <section className="afr2-settings-system">
    <Disclosure title="Preferencias"><CalibrationPanel preferences /></Disclosure>
    <Disclosure title="Informacion de hardware">
    <div className="afr2-hardware-refresh"><p className="afr2-help">Ultima informacion: <time dateTime={systemInfoUpdatedAtMs ? new Date(systemInfoUpdatedAtMs).toISOString() : undefined}>{systemInfoUpdatedAtMs ? new Date(systemInfoUpdatedAtMs).toLocaleString('es-AR') : 'Sin actualizacion'}</time></p>
    <Button size="sm" loading={refreshing} disabled={wsState !== 'ready'} onClick={async () => { setRefreshing(true); try { await refreshSystemInfo(); } finally { setRefreshing(false); } }} icon={<svg viewBox="0 0 20 20" width="18" height="18" fill="none" aria-hidden="true"><path d="M16 8a6 6 0 1 0 0 5M16 3v5h-5" stroke="currentColor" strokeWidth="1.6" strokeLinecap="round" strokeLinejoin="round" /></svg>}>Actualizar</Button></div>
    <div className="afr2-hardware-sections">
    <div><Disclosure title="Detalles de la conexion"><dl className="afr2-status-list"><div><dt>Web y ESP</dt><dd>{wsState}</dd></div><div><dt>ESP y controlador</dt><dd>{megaLinkState}</dd></div></dl></Disclosure></div>
    {(['esp', 'atmega'] as const).map(device => <div key={device}><Disclosure title={device === 'esp' ? 'ESP · Red y servidor' : 'ATmega · Controlador'}>
      <dl className="afr2-status-list">{systemInfo?.[device] && typeof systemInfo[device] === 'object'
        ? Object.entries(systemInfo[device] as Record<string, unknown>).map(([key, value]) => <div key={key}><dt>{INFO_LABELS[key] ?? key}</dt><dd>{typeof value === 'object' ? '—' : String(value ?? '—')}</dd></div>)
        : <div><dt>Informacion</dt><dd>Sin datos de la placa</dd></div>}</dl>
    </Disclosure></div>)}
    <div><Disclosure title="LittleFS y paquete Web"><dl className="afr2-status-list">{filesystemRows.map(([label, value]) => <div key={label}><dt>{label}</dt><dd>{value}</dd></div>)}</dl><p className="afr2-help">El ESP informa montaje, manifiesto y tamaños de los archivos. El hash identifica el indice declarado; los bytes no se recalculan en la placa.</p></Disclosure></div>
    <div><Disclosure title="Diagnostico WebSocket"><dl className="afr2-status-list">
      <div><dt>Ultimo cierre del cliente</dt><dd>{lastSocketClose ? `${lastSocketClose.code}: ${lastSocketClose.reason}` : 'Sin cierres observados'}</dd></div>
      <div><dt>Wi-Fi STA en ultimo reporte</dt><dd>{typeof connectionDiagnostics?.staConnected==='boolean' ? connectionDiagnostics.staConnected ? 'Conectado' : 'Desconectado' : 'Sin informacion'}</dd></div>
      <div><dt>Sesiones abiertas / cerradas</dt><dd>{typeof connectionDiagnostics?.wsOpened==='number' && typeof connectionDiagnostics?.wsClosed==='number' ? `${connectionDiagnostics.wsOpened} / ${connectionDiagnostics.wsClosed}` : 'Sin informacion'}</dd></div>
      <div><dt>Ultimo cierre ordenado por ESP</dt><dd>{typeof connectionDiagnostics?.lastServerCloseCode==='number' && connectionDiagnostics.lastServerCloseCode!==0 ? `${connectionDiagnostics.lastServerCloseCode}: ${connectionDiagnostics.lastServerCloseReason}` : 'Sin cierre informado'}</dd></div>
    </dl></Disclosure></div>
    <div><Disclosure title="Arranques observados">{bootReports.length > 0 ? <ul className="space-y-3">{bootReports.map(report => <li key={report.device}><strong>{report.device === 'esp' ? 'ESP' : 'ATmega'}</strong> · {report.bootId} · {report.observation === 'uart-boot-event' ? 'Aviso de arranque recibido' : 'Identidad consultada al conectar'}</li>)}</ul> : <p className="afr2-help">Sin arranques observados.</p>}</Disclosure></div>
    </div></Disclosure>
    <Disclosure title="Bateria"><BatteryDetails /></Disclosure>
    <Disclosure title="Comandos de reinicio">
      <p className="afr2-help">Detene el movimiento antes de reiniciar. La conexion se recupera automaticamente; un ACK no equivale a un nuevo arranque.</p>
      <div className="afr2-action-row"><Button disabled={!!busy || wsState !== 'ready' || megaLinkState !== 'ready'} aria-haspopup="dialog" onClick={event => { setRestartOrigin({ element: event.currentTarget, keyboard: event.detail === 0 }); setRestartTarget('atmega'); }}>Reiniciar ATmega</Button>
        <Button disabled={!!busy || wsState !== 'ready'} aria-haspopup="dialog" onClick={event => { setRestartOrigin({ element: event.currentTarget, keyboard: event.detail === 0 }); setRestartTarget('esp'); }}>Reiniciar ESP</Button></div>
      {message && <p role="status" className="afr2-help">{message}</p>}
    </Disclosure>
    <FloatingModal open={restartTarget !== null} title={`Confirmar reinicio de ${restartTarget === 'esp' ? 'ESP' : 'ATmega'}`}
      description="El dispositivo se reiniciara y la conexion se interrumpira temporalmente."
      origin={restartOrigin} initialFocusRef={cancelRestart} onClose={() => setRestartTarget(null)}>
      <p className="afr2-help">Confirma que queres reiniciar {restartTarget === 'esp' ? 'el ESP' : 'el ATmega'}. El controlador debe tener las salidas detenidas.</p>
      <div className="afr2-action-row">
        <button ref={cancelRestart} type="button" className="afr2-session-button" onClick={() => setRestartTarget(null)}>Cancelar</button>
        <Button variant="danger" disabled={!!busy || restartTarget === null || wsState !== 'ready' || (restartTarget === 'atmega' && megaLinkState !== 'ready')}
          onClick={() => { if (restartTarget) void restart(restartTarget); }}>Confirmar y reiniciar</Button>
      </div>
    </FloatingModal>
  </section>;
}
