import { useEffect, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { FloatingModal } from '../../components/ui/FloatingModal';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { useConnection } from '../connection/ConnectionContext';

export type WifiCredentialsKind = 'last-sta' | 'ap';
export interface WifiSavedCredentials {
  available: boolean; kind: WifiCredentialsKind; ssid?: string; password?: string;
  source?: 'connected' | 'last-connected' | 'legacy-confirmed-config' | 'legacy-confirmed-previous' | 'saved-ap' | 'active-ap';
  confirmedLast?: boolean;
}

/** Explicit private response. No context state, localStorage, events or telemetry. */
export function WifiCredentialsDialog({ kind, onClose, onUse }: {
  kind: WifiCredentialsKind | null; onClose: () => void;
  onUse: (credentials: WifiSavedCredentials) => void;
}) {
  const { requestCommand, wsState } = useConnection();
  const [data, setData] = useState<WifiSavedCredentials | null>(null);
  const [error, setError] = useState('');
  const [visible, setVisible] = useState(false);
  useEffect(() => {
    let cancelled = false;setData(null);setVisible(false);setError('');
    if (!kind || wsState !== 'ready') return;
    void requestCommand<WifiSavedCredentials>('wifi.credentials.get', { kind }, 4000).then(response => {
      if (cancelled) return;
      const result = response.payload;
      if (response.status !== 'completed' || result.kind !== kind || typeof result.available !== 'boolean' || result.available && (typeof result.ssid !== 'string' || typeof result.password !== 'string')) {
        setError('El ESP no devolvió una lectura válida.');return;
      }
      setData(result);
    }).catch(cause => { if (!cancelled) setError(cause instanceof Error ? cause.message : 'No se pudieron leer las credenciales.'); });
    return () => { cancelled = true; };
  }, [kind, wsState, requestCommand]);
  useEffect(() => { if (kind && wsState !== 'ready') onClose(); }, [kind, wsState, onClose]);
  useEffect(() => {
    if (!data) return;
    const timer = window.setTimeout(() => { setData(null);setVisible(false);onClose(); }, 30000);
    return () => window.clearTimeout(timer);
  }, [data, onClose]);
  return <FloatingModal open={kind !== null} title={kind === 'ap' ? 'Configuración guardada del AP' : 'Última red Wi-Fi conectada'} description="Lectura solicitada al ESP. Se oculta al cerrar, desconectar o después de 30 segundos." onClose={onClose}>
    <div className="afr2-page-stack">
      {error ? <StatusNotice tone="warning" title={error}/> : !data ? <p role="status">Leyendo desde el ESP…</p> : !data.available ? <p>{kind==='ap'?'No hay una configuración AP disponible para leer.':'No hay una conexión anterior confirmada disponible. El registro se crea cuando el auto logra conectarse a una red.'}</p> : <>
        {data.confirmedLast === false && <StatusNotice tone="info" title="Dato histórico">Esta red guardada conectó anteriormente, pero el firmware anterior no registraba cuál fue la última. No se confirma el orden.</StatusNotice>}
        <label className="afr2-page-stack">Nombre de red (SSID)<input className="afr2-glass-field min-h-11 px-3" readOnly value={data.ssid ?? ''}/></label>
        <label className="afr2-page-stack">Contraseña<input className="afr2-glass-field min-h-11 px-3" readOnly autoComplete="off" type={visible ? 'text' : 'password'} value={data.password ?? ''}/></label>
        {!data.password && <p className="afr2-help">Red abierta, sin contraseña.</p>}
        <div className="afr2-action-row"><Button aria-pressed={visible} onClick={() => setVisible(value => !value)}>{visible ? 'Ocultar contraseña' : 'Mostrar contraseña'}</Button><Button onClick={() => { onUse(data);onClose(); }}>Usar en configuración</Button></div>
      </>}
      <Button onClick={onClose}>Cerrar</Button>
    </div>
  </FloatingModal>;
}
