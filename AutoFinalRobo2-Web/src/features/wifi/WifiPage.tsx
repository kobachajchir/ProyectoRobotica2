import { Chevron } from '../../components/ui/Chevron';
import { TaskSections } from '../../components/ui/TaskSections';
import React, { useState } from 'react';
import { GlassSurface } from '../../components/ui/GlassSurface';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { Button } from '../../components/ui/Button';
import { Field } from '../../components/ui/Field';
import { WifiCredentialsFields, validWifiCredentials } from './WifiCredentialsFields';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { useConnection } from '../connection/ConnectionContext';

export function WifiPage() {
  const {
    wsState,
    wifiStatus,
    wifiNetworks,
    wifiScanState, wifiScanError, wifiScanTruncated,
    refreshWifiStatus,
    scanWifiNetworks,
    executeCommandWithToast,
  } = useConnection();

  const [wifiView, setWifiView] = useState('estado');
  const scanning = wifiScanState === 'scanning';
  const [persist, setPersist] = useState(true);
  const [configAccepted, setConfigAccepted] = useState(false);
  const [targetMode, setTargetMode] = useState<'STA' | 'AP' | 'AP_STA'>('AP_STA');
  const [ssid, setSsid] = useState('');
  const [password, setPassword] = useState('');
  const [submittingConfig, setSubmittingConfig] = useState(false);

  const handleScan = () => scanWifiNetworks();

  const handleConfigureSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    if (!validWifiCredentials(ssid, password)) return;
    setSubmittingConfig(true);
    try {
      setConfigAccepted(false);
      const result = await executeCommandWithToast(
        'wifi.configure',
        {
          mode: targetMode,
          ssid,
          password,
          persist,
        },
        {
          loadingTitle: `Aplicando configuracion Wi-Fi (${targetMode})...`,
          acceptedTitle: 'Configuracion Wi-Fi aceptada por el ESP8266 (posible reconexion)',
          successTitle: 'Configuracion Wi-Fi aplicada',
        },
        6000
      );
      setConfigAccepted(result?.status === 'accepted');
      // Limpiar contraseña efimera tras el envio
      setPassword('');
    } finally {
      setSubmittingConfig(false);
    }
  };

  return (<div className="afr2-route-view afr2-page-stack"><SectionHeader title="Wi-Fi" description="Consulta la conexion o elegi una red para el robot." />
<TaskSections label="Secciones de Wi-Fi" value={wifiView} onValueChange={setWifiView} items={[
{ id: 'estado', label: 'Estado', content: (<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader
          eyebrow="wifi.status · wifi.scan · wifi.configure"
          title="Conexion del robot"
          description="El modulo ESP12S opera con firmware propio sin comandos AT. Las credenciales se envian de forma efimera y jamas se almacenan en el navegador ni en el repositorio."
          badge={`Modo: ${wifiStatus.mode}`}
          badgeTone={wifiStatus.staConnected ? 'success' : 'neutral'}
          actions={
            <>
              <Button
                variant="secondary"
                size="sm"
                onClick={() => void refreshWifiStatus()}
              >
                Actualizar estado
              </Button>
              <Button
                variant="primary"
                size="sm"
                loading={scanning}
                disabled={wsState !== 'ready' || wifiStatus.scanRunning === true}
                onClick={() => { setWifiView('redes'); void handleScan(); }}
              >
                Buscar redes
              </Button>
            </>
          }
        />

        {wifiStatus.configError && <StatusNotice tone="error" title="No se pudo aplicar toda la configuracion" description={wifiStatus.configError} />}
        {wifiStatus.fallbackActive && <StatusNotice tone="warning" title="Punto de acceso de recuperacion activo" description="Podes volver a configurar la red desde este punto de acceso." />}
        {wifiStatus.applying && <StatusNotice tone="info" title="Aplicando configuracion de red…" />}
        {wifiStatus.staStatus && wifiStatus.staStatus !== 'disabled' && <StatusNotice
          tone={wifiStatus.staConnected ? 'success' : ['wrong-password', 'network-not-found', 'connection-failed'].includes(wifiStatus.staStatus) ? 'warning' : 'info'}
          title={wifiStatus.staConnected ? 'Conectado a la red' : ({
            'connecting': 'Conectando a la red…', 'network-not-found': 'No se encontro la red',
            'wrong-password': 'La red rechazo la contraseña', 'connection-failed': 'Fallo la conexion',
          } as Record<string, string>)[wifiStatus.staStatus] ?? 'Sin conexion a la red'} />}
        <div className="mt-5 grid grid-cols-1 gap-4 sm:grid-cols-3">
          <div className="afr2-surface-cell p-4">
            <p className="text-xs font-semibold uppercase" style={{ color: 'var(--text-muted)' }}>
              Estacion (STA)
            </p>
            <p className="afr2-tabular mt-1 text-base font-bold" style={{ color: 'var(--text-strong)' }}>
              {wifiStatus.staSsid ?? '—'}
            </p>
            <p className="afr2-tabular mt-1 text-xs" style={{ color: 'var(--text-muted)' }}>
              IP: {wifiStatus.staIp ?? '—'} · RSSI:{' '}
              {wifiStatus.rssiDbm !== null ? `${wifiStatus.rssiDbm} dBm` : '—'}
            </p>
          </div>

          <div className="afr2-surface-cell p-4">
            <p className="text-xs font-semibold uppercase" style={{ color: 'var(--text-muted)' }}>
              Punto de acceso (AP)
            </p>
            <p className="afr2-tabular mt-1 text-base font-bold" style={{ color: 'var(--text-strong)' }}>
              {wifiStatus.apSsid ?? '—'}
            </p>
            <p className="afr2-tabular mt-1 text-xs" style={{ color: 'var(--text-muted)' }}>
              IP AP: {wifiStatus.apIp ?? '—'}
            </p>
          </div>

          <div className="afr2-surface-cell p-4">
            <p className="text-xs font-semibold uppercase" style={{ color: 'var(--text-muted)' }}>
              Puente Hardware Shield
            </p>
            <p className="afr2-tabular mt-1 text-base font-bold" style={{ color: 'var(--text-strong)' }}>
              D18/D19 (Operacion)
            </p>
            <p className="mt-1 text-xs" style={{ color: 'var(--text-muted)' }}>
              KEY/RST son fisicos del shield; ningun comando Web conmuta jumpers ni flashea.
            </p>
          </div>
        </div>
      </GlassSurface>) },
{ id: 'redes', label: 'Buscar red', content: (<GlassSurface variant="panel" className="relative p-5 sm:p-6 lg:col-span-6">
          <SectionHeader
            eyebrow="Escaneo Asincrono 2,4 GHz"
            title="Redes cercanas"
            description="Seleccione una red de la lista para copiar su SSID al formulario de configuracion."
            badge={`${wifiNetworks.length} redes`}
          />

          <div className="afr2-action-row"><Button loading={scanning} disabled={wsState !== 'ready' || wifiStatus.scanRunning === true} onClick={() => void handleScan()}>Buscar redes</Button></div>
          <div
            aria-busy={scanning}
            className="mt-4 flex flex-col gap-2"
          >
            {scanning && (
              <StatusNotice
                tone="info"
                title="Escaneo Wi-Fi en curso..."
                description="Esperando el resultado de la ESP. La aceptacion del pedido todavia no significa que la busqueda termino."
              />
            )}

            {wifiScanError && <StatusNotice tone="error" title="Busqueda sin completar" description={wifiScanError} />}
            {wifiScanTruncated && <StatusNotice tone="info" title="Se muestran las primeras 16 redes" description="La ESP encontro mas resultados; podes escribir el SSID si tu red no aparece." />}
            {!scanning && wifiScanState !== 'failed' && wifiNetworks.length === 0 ? (
              <div className="afr2-surface-cell p-6 text-center text-xs" style={{ color: 'var(--text-muted)' }}>
                {wifiScanState === 'completed' ? 'La busqueda termino sin encontrar redes.' : 'Todavia no hay resultados en esta sesion. Busca redes con la ESP conectada.'}
              </div>
            ) : (
              wifiNetworks.map((net) => (
                <button
                  key={`${net.ssid}-${net.channel}`}
                  type="button"
                  onClick={() => { setSsid(net.ssid); setWifiView('configurar'); }}
                  className="afr2-pressable afr2-surface-cell flex items-center justify-between p-3 text-left"
                >
                  <div>
                    <p className="text-sm font-bold" style={{ color: 'var(--text-strong)' }}>
                      {net.ssid || '(SSID oculto)'}
                    </p>
                    <p className="afr2-tabular text-xs" style={{ color: 'var(--text-muted)' }}>
                      Canal {net.channel} · {net.secure ? 'Protegida' : 'Abierta'}
                    </p>
                  </div>
                  <span className="afr2-tabular text-xs font-bold" style={{ color: 'var(--action)' }}>
                    {net.rssiDbm} dBm
                  </span>
                </button>
              ))
            )}
          </div>
        </GlassSurface>) },
{ id: 'configurar', label: 'Configurar', content: (<GlassSurface variant="panel" className="p-5 sm:p-6 lg:col-span-6">
          <SectionHeader
            eyebrow="Credenciales Efimeras"
            title="Configurar red"
            description="La conexion puede cortarse al cambiar de red. Si cambia la IP, conectate a la nueva red y abri la direccion correspondiente."
          />

          {configAccepted && <StatusNotice tone="info" title="Configuracion aceptada" description="La ESP va a aplicar los cambios. Consulta Estado para ver si logro conectarse; este aviso todavia no confirma la conexion." />}
          <form onSubmit={(e) => void handleConfigureSubmit(e)} className="mt-4 flex flex-col gap-4">
            <Field id="wifi-cfg-mode" label="Modo de operacion objetivo">
              <span className="afr2-select-wrap"><select
                id="wifi-cfg-mode"
                value={targetMode}
                onChange={(e) => setTargetMode(e.target.value as 'STA' | 'AP' | 'AP_STA')}
                className="afr2-glass-field min-h-[42px] px-3 py-1.5 text-sm"
              >
                <option value="AP_STA">Mixto AP + Estacion (AP_STA recomendado)</option>
                <option value="STA">Solo Estacion (STA)</option>
                <option value="AP">Solo Punto de Acceso (AP)</option>
              </select><Chevron /></span>
            </Field>

            <WifiCredentialsFields id="wifi-cfg" mode={targetMode} ssid={ssid} password={password} persist={persist}
              onSsid={setSsid} onPassword={setPassword} onPersist={setPersist} />
            <div className="flex flex-wrap items-center gap-2 pt-1">
              <Button
                type="submit"
                variant="primary"
                loading={submittingConfig}
                disabled={wsState !== 'ready' || scanning || wifiStatus.applying || wifiStatus.scanRunning || !validWifiCredentials(ssid, password)}
              >
                Aplicar configuracion
              </Button>
            </div>
          </form>
        </GlassSurface>) }]} />
</div>);
}
