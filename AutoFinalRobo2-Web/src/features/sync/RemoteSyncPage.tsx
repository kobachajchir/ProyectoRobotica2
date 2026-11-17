import { useCallback, useEffect, useRef, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { PageHeader } from '../../components/ui/PageHeader';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { UiIcon } from '../../components/ui/UiIcon';
import {
  FirebaseSyncError,
  normalizePairCode,
  PAIR_CODE_PATTERN,
  readRemoteState,
  redeemPairCode,
  unlinkThisDevice,
  type RemoteSyncDoc,
} from './firebaseSync';

/**
 * AFR2-20261117-05 — Vista remota (solo lectura) de la copia en Firebase.
 * Sin vincular: pide el código de 6 caracteres que muestra el OLED. Vinculada: lee `car/state` cada 5 s
 * mientras la pestaña está visible. No envía comandos al auto.
 */
const POLL_MS = 5000;
/** Si el auto no escribió en este tiempo, la copia se muestra como «sin datos recientes» (el latido es de 60 s). */
const LIVE_WINDOW_MS = 90_000;

type Row = [string, string];
type Layer = Record<string, unknown> | undefined;
type ViewState = 'checking' | 'unlinked' | 'empty' | 'ok' | 'error';

const yesNo = (v: unknown) => (v === true ? 'Sí' : v === false ? 'No' : '—');
const text = (v: unknown) => (v === null || v === undefined || v === '' ? '—' : String(v));

function ago(ms: number) {
  const s = Math.max(0, Math.round(ms / 1000));
  if (s < 60) return `hace ${s} s`;
  const m = Math.round(s / 60);
  if (m < 60) return `hace ${m} min`;
  const h = Math.round(m / 60);
  return h < 48 ? `hace ${h} h` : `hace ${Math.round(h / 24)} días`;
}

const CONFIG_LABELS: Record<string, string> = {
  motion: 'Movimiento', motors: 'Motores', odometry: 'Odometría', line: 'Línea IR', radar: 'Radar', ui: 'Pantalla y encoder',
};

function flatten(obj: unknown, prefix = ''): Row[] {
  if (obj === null || typeof obj !== 'object') return [[prefix || 'valor', text(obj)]];
  return Object.entries(obj as Record<string, unknown>).flatMap(([k, v]) => {
    const key = prefix ? `${prefix}.${k}` : k;
    if (v !== null && typeof v === 'object' && !Array.isArray(v)) return flatten(v, key);
    return [[key, Array.isArray(v) ? v.join(', ') : typeof v === 'boolean' ? yesNo(v) : text(v)] as Row];
  });
}

function Card({ title, rows }: { title: string; rows: Row[] }) {
  return (
    <section className="afr2-glass-panel afr2-remote-card">
      <h2>{title}</h2>
      <dl className="afr2-network-details">
        {rows.map(([k, v]) => <div key={k}><dt>{k}</dt><dd>{v}</dd></div>)}
      </dl>
    </section>
  );
}

function PairForm({ onLinked }: { onLinked: () => void }) {
  const [code, setCode] = useState('');
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const valid = PAIR_CODE_PATTERN.test(code);
  const submit = async () => {
    if (!valid || busy) return;
    setBusy(true);
    setError(null);
    try {
      await redeemPairCode(code);
      onLinked();
    } catch (e) {
      setError(e instanceof Error ? e.message : 'No se pudo vincular.');
    } finally {
      setBusy(false);
    }
  };
  return (
    <section className="afr2-glass-panel afr2-remote-card afr2-remote-pair">
      <h2>Vincular esta compu</h2>
      <ol className="afr2-help">
        <li>En la Web del auto abrí <strong>Sync web → Copia en Firebase → Vincular una compu</strong>.</li>
        <li>El código aparece en la pantalla del auto. Escribilo acá (vale 5 minutos y sirve una sola vez).</li>
      </ol>
      <label className="afr2-remote-code">
        <span>Código</span>
        <input className="afr2-glass-field" type="text" inputMode="text" autoComplete="one-time-code" autoCapitalize="characters"
          spellCheck={false} maxLength={7} placeholder="K7M-4QX" value={code} aria-invalid={error !== null}
          onChange={e => { setCode(normalizePairCode(e.target.value)); setError(null); }}
          onKeyDown={e => { if (e.key === 'Enter') void submit(); }} />
      </label>
      {error && <p className="afr2-remote-error" role="alert">{error}</p>}
      <div className="afr2-action-row">
        <Button variant="primary" loading={busy} disabled={!valid} onClick={() => void submit()}>Vincular</Button>
      </div>
      <p className="afr2-help">La compu vinculada solo ve el estado del auto; no puede enviarle comandos.</p>
    </section>
  );
}

export function RemoteSyncPage() {
  const [doc, setDoc] = useState<RemoteSyncDoc | null>(null);
  const [state, setState] = useState<ViewState>('checking');
  const [error, setError] = useState<string | null>(null);
  const [now, setNow] = useState(() => Date.now());
  const inFlight = useRef(false);

  const load = useCallback(async () => {
    if (inFlight.current) return;
    inFlight.current = true;
    try {
      const result = await readRemoteState();
      setDoc(result);
      setState(result ? 'ok' : 'empty');
      setError(null);
    } catch (e) {
      if (e instanceof FirebaseSyncError && e.status === 403) {
        setDoc(null);
        setState('unlinked');
      } else {
        setState(prev => (prev === 'ok' ? prev : 'error'));
        setError(e instanceof Error ? e.message : 'No se pudo leer Firebase.');
      }
    } finally {
      inFlight.current = false;
    }
  }, []);

  useEffect(() => { void load(); }, [load]);
  useEffect(() => {
    if (state === 'unlinked' || state === 'checking') return;
    const id = window.setInterval(() => {
      setNow(Date.now());
      if (document.visibilityState === 'visible') void load();
    }, POLL_MS);
    return () => window.clearInterval(id);
  }, [state, load]);

  const unlink = async () => {
    if (!window.confirm('¿Desvincular esta compu? Para volver a ver el estado vas a necesitar un código nuevo del auto.')) return;
    await unlinkThisDevice();
    setDoc(null);
    setState('unlinked');
  };

  const layers = doc?.layers ?? {};
  const system = layers.system as Layer;
  const wifi = layers.wifi as Layer;
  const robot = layers.robot as Layer;
  const config = (layers.config as Record<string, unknown> | undefined) ?? {};
  const pose = robot?.pose as Layer;
  const live = doc ? now - doc.ts < LIVE_WINDOW_MS : false;

  return (
    <div className="afr2-route-view afr2-page-stack">
      <PageHeader eyebrow="Sincronización" title="Vista remota"
        description="Último estado que el auto publicó en Firebase. Solo lectura: desde acá no se envían comandos." />

      {state === 'checking' && <p role="status" className="afr2-help">Consultando la copia online…</p>}
      {state === 'unlinked' && <PairForm onLinked={() => { setState('checking'); void load(); }} />}

      {state !== 'unlinked' && state !== 'checking' && (
        <>
          <div className="afr2-remote-status">
            <span className="afr2-remote-live" data-state={state === 'error' ? 'error' : live ? 'live' : 'stale'}>
              <UiIcon name={state === 'error' ? 'warning' : live ? 'wifi' : 'wifi-off'} size={16} />
              {state === 'error' ? 'Error' : !doc ? 'Sin datos' : live ? 'En vivo' : 'Sin datos recientes'}
            </span>
            {doc && <span>Actualizado {ago(now - doc.ts)} · rev {doc.rev} · Web {text(doc.webVersion)}</span>}
            <Button variant="secondary" size="sm" icon={<UiIcon name="refresh" size={16} />} onClick={() => void load()}>Actualizar</Button>
            <Button variant="secondary" size="sm" onClick={() => void unlink()}>Desvincular</Button>
          </div>

          {state === 'error' && <StatusNotice tone="warning" title="No se pudo leer la copia online">{error}</StatusNotice>}
          {state === 'empty' && (
            <StatusNotice tone="info" title="Vinculada, todavía sin datos">
              El auto publica cuando su Web está abierta, conectada al auto y con Internet.
            </StatusNotice>
          )}

          {doc && (
            <div className="afr2-remote-grid">
              <Card title="Robot" rows={[
                ['Modo', text(robot?.mode)],
                ['Detenido', yesNo(robot?.stopped)],
                ['Motores habilitados', yesNo(robot?.motorsEnabled)],
                ['Falla', text(robot?.fault)],
                ['Cambios sin guardar', yesNo(robot?.configurationDirty)],
                ['Pose', pose ? `x ${text(pose.xMm)} mm · y ${text(pose.yMm)} mm · θ ${(Number(pose.thetaMrad) / 1000).toFixed(2)} rad` : 'Sin pose válida'],
              ]} />
              <Card title="Sistema" rows={[
                ['Versión', text(system?.version)],
                ['Enlace con el ATmega', text(system?.megaLink)],
                ['WebSocket', text(system?.wsState)],
                ['Batería', typeof system?.batteryMv === 'number' ? `${(system.batteryMv / 1000).toFixed(2)} V` : '—'],
                ['Arranque del ESP', text(system?.bootId)],
                ['Dispositivo', text(doc.deviceId)],
              ]} />
              <Card title="Wi-Fi" rows={[
                ['Modo', text(wifi?.mode)],
                ['Red conectada', yesNo(wifi?.staConnected)],
                ['IP en la red', text(wifi?.staIp)],
                ['Punto de acceso', wifi?.apReady ? `${text(wifi?.apIp)} · ${text(wifi?.apClients)}/${text(wifi?.apMaxClients)} clientes` : 'Apagado'],
                ['Señal', typeof wifi?.rssiDbm === 'number' ? `${wifi.rssiDbm} dBm` : '—'],
              ]} />
              {Object.entries(config).map(([name, value]) => (
                <Card key={name} title={CONFIG_LABELS[name] ?? name} rows={flatten(value)} />
              ))}
              {doc.simulators && (
                <Card title="Simuladores (envío explícito)" rows={[
                  ['Origen', text(doc.simulators.source)],
                  ['Enviado', doc.simulators.syncedAtMs ? new Date(doc.simulators.syncedAtMs).toLocaleString() : '—'],
                ]} />
              )}
            </div>
          )}
        </>
      )}
    </div>
  );
}
