import { useEffect, useMemo, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { FloatingModal, type ModalOrigin } from '../../components/ui/FloatingModal';
import { SegmentedControl } from '../../components/ui/SegmentedControl';
import { qrcodegen } from '../oled/vendor/qrcodegen';
import { useConnection } from './ConnectionContext';

function escapeWifiField(value: string): string {
  return value.replace(/([\\;,"':])/g, '\\$1');
}

function encodeQrSvg(text: string): { size: number; path: string } {
  const safeText = text.trim() || 'http://192.168.4.1/';
  const qr = qrcodegen.QrCode.encodeText(safeText, qrcodegen.QrCode.Ecc.MEDIUM);
  const quietZone = 4;
  const modules: string[] = [];
  for (let y = 0; y < qr.size; y += 1) {
    for (let x = 0; x < qr.size; x += 1) {
      if (qr.getModule(x, y)) {
        modules.push(`M${x + quietZone},${y + quietZone}h1v1h-1z`);
      }
    }
  }
  return { size: qr.size + quietZone * 2, path: modules.join('') };
}

function isEmbeddedOrPrivateHost(): boolean {
  if (import.meta.env.MODE === 'esp') return true;
  if (typeof window === 'undefined') return false;
  const host = window.location.hostname;
  return (
    /^(10\.|192\.168\.|172\.(1[6-9]|2\d|3[01])\.)/.test(host) ||
    host.endsWith('.local')
  );
}

interface WifiCredentialReply {
  available?: boolean;
  kind?: 'ap' | 'last-sta';
  ssid?: string;
  password?: string;
}

export function QrAccessModal({
  open,
  origin,
  onClose,
}: {
  open: boolean;
  origin: ModalOrigin | null;
  onClose: () => void;
}) {
  const { wsState, wifiStatus, requestCommand } = useConnection();
  const defaultEmbedded = useMemo(isEmbeddedOrPrivateHost, []);
  const [variant, setVariant] = useState<'auto' | 'online' | 'ip'>('auto');
  const [wifiKind, setWifiKind] = useState<'ap' | 'last-sta'>(() =>
    wifiStatus.staConnected ? 'last-sta' : 'ap'
  );
  const [wifiSsid, setWifiSsid] = useState<string>('');
  const [wifiPassword, setWifiPassword] = useState<string>('');
  const [loadingWifi, setLoadingWifi] = useState<boolean>(false);

  useEffect(() => {
    if (!open) return;
    const preferredKind = wifiStatus.staConnected ? 'last-sta' : 'ap';
    setWifiKind(preferredKind);
  }, [open, wifiStatus.staConnected]);

  useEffect(() => {
    if (!open) return;
    const fallbackSsid =
      wifiKind === 'last-sta'
        ? wifiStatus.staSsid || 'AutoFinalRobo2'
        : wifiStatus.apSsid || 'AutoFinalRobo2-AP';
    setWifiSsid(fallbackSsid);
    if (wsState !== 'ready') return;
    let cancelled = false;
    setLoadingWifi(true);
    void requestCommand<WifiCredentialReply>('wifi.credentials.get', { kind: wifiKind }, 2500)
      .then((res) => {
        if (cancelled) return;
        if (res.status === 'completed' && res.payload?.available && res.payload.ssid) {
          setWifiSsid(res.payload.ssid);
          setWifiPassword(res.payload.password ?? '');
        }
      })
      .catch(() => {
        /* Fallback SSID kept */
      })
      .finally(() => {
        if (!cancelled) setLoadingWifi(false);
      });
    return () => {
      cancelled = true;
    };
  }, [open, requestCommand, wifiKind, wifiStatus.apSsid, wifiStatus.staSsid, wsState]);

  const activeCarIp =
    (wifiStatus.staConnected && wifiStatus.staIp) ||
    wifiStatus.apIp ||
    (typeof window !== 'undefined' && /^\d+\.\d+\.\d+\.\d+$/.test(window.location.hostname)
      ? window.location.hostname
      : '192.168.4.1');

  const onlineUrl =
    typeof window !== 'undefined'
      ? `${window.location.origin}${window.location.pathname}`
      : 'https://kobachajchir.github.io/ProyectoRobotica2/';

  const showEmbeddedIp =
    variant === 'ip' || (variant === 'auto' && (defaultEmbedded || wsState === 'ready'));
  const appTargetUrl = showEmbeddedIp ? `http://${activeCarIp}/` : onlineUrl;

  const wifiPayload = useMemo(() => {
    const ssid = wifiSsid.trim() || 'AutoFinalRobo2';
    const pass = wifiPassword;
    const auth = pass ? 'WPA' : 'nopass';
    return `WIFI:T:${auth};S:${escapeWifiField(ssid)};${pass ? `P:${escapeWifiField(pass)};` : ''};`;
  }, [wifiSsid, wifiPassword]);

  const appQr = useMemo(() => encodeQrSvg(appTargetUrl), [appTargetUrl]);
  const wifiQr = useMemo(() => encodeQrSvg(wifiPayload), [wifiPayload]);

  return (
    <FloatingModal
      open={open}
      origin={origin}
      onClose={onClose}
      size="lg"
      title="Códigos QR de la aplicación y red Wi-Fi"
      description="Escaneá el QR de la red Wi-Fi (máximo 4 dispositivos) y el QR de acceso a la aplicación (1 cliente Web activo por vez)."
    >
      <div className="space-y-4">
        <div className="flex flex-wrap items-center justify-between gap-2">
          <SegmentedControl
            size="sm"
            ariaLabel="Variante de dirección de la app"
            value={variant}
            onChange={setVariant}
            items={[
              { value: 'auto', label: defaultEmbedded ? 'Automático (IP del auto)' : 'Automático (URL online)' },
              { value: 'ip', label: 'IP actual del auto' },
              { value: 'online', label: 'URL variante Online' },
            ]}
          />
          <SegmentedControl
            size="sm"
            ariaLabel="Red Wi-Fi a mostrar en QR"
            value={wifiKind}
            onChange={setWifiKind}
            items={[
              { value: 'ap', label: 'Punto de acceso (AP)' },
              { value: 'last-sta', label: 'Red Wi-Fi (STA)' },
            ]}
          />
        </div>

        <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
          <figure className="p-4 rounded-[var(--radius-md)] border border-[var(--border-default)] bg-[var(--surface-subtle)] flex flex-col items-center text-center space-y-2">
            <span className="text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
              {showEmbeddedIp ? 'QR de la App · IP actual del auto' : 'QR de la App · Variante Online'}
            </span>
            <div className="p-2 bg-white rounded-lg shadow-xs">
              <svg
                viewBox={`0 0 ${appQr.size} ${appQr.size}`}
                width={184}
                height={184}
                shapeRendering="crispEdges"
                role="img"
                aria-label={`Código QR de la aplicación: ${appTargetUrl}`}
              >
                <rect width={appQr.size} height={appQr.size} fill="#fff" />
                <path d={appQr.path} fill="#000" />
              </svg>
            </div>
            <code className="text-xs font-mono break-all px-2 py-1 rounded bg-[var(--surface-solid)] border border-[var(--border-default)]">
              {appTargetUrl}
            </code>
            <figcaption className="text-xs text-[var(--text-muted)]">
              {showEmbeddedIp
                ? 'Abre la interfaz embebida del auto en la red actual.'
                : 'Abre la copia fiel online de la aplicación Web.'}
            </figcaption>
          </figure>

          <figure className="p-4 rounded-[var(--radius-md)] border border-[var(--border-default)] bg-[var(--surface-subtle)] flex flex-col items-center text-center space-y-2">
            <span className="text-xs font-semibold uppercase tracking-wider text-[var(--text-muted)]">
              QR de la Red Wi-Fi ({wifiKind === 'ap' ? 'AP del auto' : 'Estación STA'})
            </span>
            <div className="p-2 bg-white rounded-lg shadow-xs">
              <svg
                viewBox={`0 0 ${wifiQr.size} ${wifiQr.size}`}
                width={184}
                height={184}
                shapeRendering="crispEdges"
                role="img"
                aria-label={`Código QR de la red Wi-Fi ${wifiSsid}`}
              >
                <rect width={wifiQr.size} height={wifiQr.size} fill="#fff" />
                <path d={wifiQr.path} fill="#000" />
              </svg>
            </div>
            <div className="w-full space-y-1.5 text-left">
              <label className="block text-xs text-[var(--text-muted)]">
                SSID
                <input
                  type="text"
                  value={wifiSsid}
                  onChange={(e) => setWifiSsid(e.target.value)}
                  className="afr2-glass-field w-full min-h-9 px-2.5 mt-0.5 text-xs font-mono text-[var(--text)]"
                />
              </label>
              <label className="block text-xs text-[var(--text-muted)]">
                Contraseña {loadingWifi ? '(consultando ESP…)' : '(vacío = abierta)'}
                <input
                  type="text"
                  value={wifiPassword}
                  onChange={(e) => setWifiPassword(e.target.value)}
                  placeholder="Sin clave / WPA2"
                  className="afr2-glass-field w-full min-h-9 px-2.5 mt-0.5 text-xs font-mono text-[var(--text)]"
                />
              </label>
            </div>
            <figcaption className="text-xs text-[var(--text-muted)]">
              Admite hasta 4 equipos en el Wi-Fi; 1 cliente Web activo a la vez.
            </figcaption>
          </figure>
        </div>

        <div className="flex justify-end">
          <Button variant="secondary" onClick={onClose}>
            Cerrar
          </Button>
        </div>
      </div>
    </FloatingModal>
  );
}
