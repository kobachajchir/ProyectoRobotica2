import { useState } from 'react';
import { useConnection } from './ConnectionContext';
import { Disclosure } from '../../components/ui/Disclosure';
import { Button } from '../../components/ui/Button';

const INFO_LABELS: Record<string, string> = { version:'Versión', compiledAt:'Compilación', compiler:'Compilador', revision:'Revisión', core:'Arduino Core', sdk:'SDK', bootId:'Identidad de arranque', resetReason:'Causa del reinicio', resetCauseBits:'Causa de reset (bits)', uptimeMs:'Tiempo activo (ms)', clockMHz:'Reloj (MHz)', sketchBytes:'Programa (bytes)', freeHeapBytes:'Heap libre (bytes)', flashBytes:'Flash detectada (bytes)', architecture:'Arquitectura', extensionVersion:'Protocolo EX01' };
export function SystemPanel() {
  const { wsState, megaLinkState, systemInfo, systemInfoUpdatedAtMs, bootReports, refreshSystemInfo, requestCommand } = useConnection();
  const [refreshing, setRefreshing] = useState(false);
  const [busy, setBusy] = useState<string | null>(null);
  const [message, setMessage] = useState('');
  const filesystem = systemInfo?.filesystem && typeof systemInfo.filesystem === 'object'
    ? systemInfo.filesystem as Record<string, unknown> : null;
  const filesystemRows: [string, string][] = [
    ['LittleFS', typeof filesystem?.mounted === 'boolean' ? filesystem.mounted ? 'Montado' : 'No montado' : 'Sin información'],
    ['Paquete Web', typeof filesystem?.webReady === 'boolean' ? filesystem.webReady ? 'Disponible' : 'No disponible' : 'Sin información'],
    ['Diagnóstico', typeof filesystem?.fault === 'string' ? filesystem.fault : '—'],
    ['Ocupado / capacidad', typeof filesystem?.totalBytes === 'number' && typeof filesystem?.usedBytes === 'number' ? `${filesystem.usedBytes.toLocaleString('es-AR')} / ${filesystem.totalBytes.toLocaleString('es-AR')} bytes` : 'Sin información'],
    ['Versión Web', typeof filesystem?.webVersion === 'string' ? filesystem.webVersion : 'No informada por el paquete'],
    ['Empaquetado', typeof filesystem?.packagedAt === 'string' && Number.isFinite(Date.parse(filesystem.packagedAt)) ? new Date(filesystem.packagedAt).toLocaleString('es-AR') : 'Sin información'],
    ['Perfil', typeof filesystem?.buildProfile === 'string' ? filesystem.buildProfile : 'Sin información'],
    ['Activos / comprimidos', typeof filesystem?.assetCount === 'number' && typeof filesystem?.gzipBytes === 'number' ? `${filesystem.assetCount} / ${filesystem.gzipBytes.toLocaleString('es-AR')} bytes` : 'Sin información'],
    ['SHA-256 del índice declarado', typeof filesystem?.indexSha256 === 'string' ? filesystem.indexSha256 : 'Sin información'],
  ];
  const restart = async (target: 'esp' | 'atmega') => {
    setBusy(target); setMessage('');
    try {
      const response = await requestCommand('system.reboot', { target, confirm: true }, 4000);
      setMessage(response.status === 'accepted' ? 'Reinicio aceptado. Esperando el aviso de arranque y la reconexión.' : 'Solicitud confirmada.');
    } catch (error) { setMessage(error instanceof Error ? error.message : 'No se pudo solicitar el reinicio.'); }
    finally { setBusy(null); }
  };
  return <section className="afr2-settings-system">
    <Disclosure title="Información de hardware">
    <div className="afr2-hardware-refresh"><p className="afr2-help">Última información: <time dateTime={systemInfoUpdatedAtMs ? new Date(systemInfoUpdatedAtMs).toISOString() : undefined}>{systemInfoUpdatedAtMs ? new Date(systemInfoUpdatedAtMs).toLocaleString('es-AR') : 'Sin actualización'}</time></p>
    <Button size="sm" loading={refreshing} disabled={wsState !== 'ready'} onClick={async () => { setRefreshing(true); try { await refreshSystemInfo(); } finally { setRefreshing(false); } }} icon={<svg viewBox="0 0 20 20" width="18" height="18" fill="none" aria-hidden="true"><path d="M16 8a6 6 0 1 0 0 5M16 3v5h-5" stroke="currentColor" strokeWidth="1.6" strokeLinecap="round" strokeLinejoin="round" /></svg>}>Actualizar</Button></div>
    <div className="afr2-hardware-sections">
    <div><Disclosure title="Detalles de la conexión"><dl className="afr2-status-list"><div><dt>Web y ESP</dt><dd>{wsState}</dd></div><div><dt>ESP y controlador</dt><dd>{megaLinkState}</dd></div></dl></Disclosure></div>
    {(['esp', 'atmega'] as const).map(device => <div key={device}><Disclosure title={device === 'esp' ? 'ESP · Red y servidor' : 'ATmega · Controlador'}>
      <dl className="afr2-status-list">{systemInfo?.[device] && typeof systemInfo[device] === 'object'
        ? Object.entries(systemInfo[device] as Record<string, unknown>).map(([key, value]) => <div key={key}><dt>{INFO_LABELS[key] ?? key}</dt><dd>{typeof value === 'object' ? '—' : String(value ?? '—')}</dd></div>)
        : <div><dt>Información</dt><dd>Sin datos de la placa</dd></div>}</dl>
    </Disclosure></div>)}
    <div><Disclosure title="LittleFS y paquete Web"><dl className="afr2-status-list">{filesystemRows.map(([label, value]) => <div key={label}><dt>{label}</dt><dd>{value}</dd></div>)}</dl><p className="afr2-help">El ESP informa montaje, manifiesto y tamaños de los archivos. El hash identifica el índice declarado; los bytes no se recalculan en la placa.</p></Disclosure></div>
    <div><Disclosure title="Arranques observados">{bootReports.length > 0 ? <ul className="space-y-3">{bootReports.map(report => <li key={report.device}><strong>{report.device === 'esp' ? 'ESP' : 'ATmega'}</strong> · {report.bootId} · {report.observation === 'uart-boot-event' ? 'Aviso de arranque recibido' : 'Identidad consultada al conectar'}</li>)}</ul> : <p className="afr2-help">Sin arranques observados.</p>}</Disclosure></div>
    </div></Disclosure>
    <Disclosure title="Comandos de reinicio">
      <p className="afr2-help">Detené el movimiento antes de reiniciar. La conexión se recupera automáticamente; un ACK no equivale a un nuevo arranque.</p>
      <div className="afr2-action-row"><Button disabled={!!busy || wsState !== 'ready' || megaLinkState !== 'ready'} onClick={() => void restart('atmega')}>Reiniciar ATmega</Button>
        <Button disabled={!!busy || wsState !== 'ready'} onClick={() => void restart('esp')}>Reiniciar ESP</Button></div>
      {message && <p role="status" className="afr2-help">{message}</p>}
    </Disclosure>
  </section>;
}
