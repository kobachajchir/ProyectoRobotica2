import { TaskSections } from '../../components/ui/TaskSections';
import { GlassSurface } from '../../components/ui/GlassSurface';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { Button } from '../../components/ui/Button';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { useConnection } from '../connection/ConnectionContext';
import { computeFreshness, formatAgeLabel } from '../../protocol/wsEnvelope';
import { useEffect,useState } from 'react';

export function SensorsPage() {
  const { wsState, sensors:sample, refreshSensors, executeCommandWithToast } = useConnection();
  const [now,setNow]=useState(Date.now());
  useEffect(()=>{const id=window.setInterval(()=>setNow(Date.now()),500);return()=>window.clearInterval(id);},[]);
  const delta=sample.receivedAtMs===null?0:Math.max(0,now-sample.receivedAtMs);
  const sensors={...sample,line:{...sample.line,ageMs:sample.line.ageMs===null?null:sample.line.ageMs+delta},ultrasonic:{...sample.ultrasonic,ageMs:sample.ultrasonic.ageMs===null?null:sample.ultrasonic.ageMs+delta}};

  const lineFreshness = computeFreshness(sensors.line.valid, sensors.line.ageMs);
  const usFreshness = computeFreshness(sensors.ultrasonic.valid, sensors.ultrasonic.ageMs);

  const lineBits = sensors.line.valid && sensors.line.bits !== null ? sensors.line.bits : null;

  return (<div className="afr2-route-view afr2-page-stack">
    <SectionHeader title="Sensores" description="Consultá las lecturas y su antigüedad por sección." />
    <TaskSections label="Grupos de sensores" items={[
{ id: 'entorno', label: 'Línea y distancia', content: (<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader
          eyebrow="robot.sensors · robot.encoders"
          title="Línea y distancia"
          description="Muestra únicamente lecturas validadas por el ATmega2560 con su antigüedad explícita. Los canales sin lectura válida o sin cablear se indican como '—', nunca como cero falso."
          badge={wsState === 'ready' ? 'Enlace WS Listo' : 'Sin Enlace WS'}
          badgeTone={wsState === 'ready' ? 'success' : 'neutral'}
          actions={
            <>
              <Button
                variant="primary"
                size="sm"
                onClick={() => void refreshSensors()}
              >
                Solicitar `robot.sensors`
              </Button>
              <Button
                variant="secondary"
                size="sm"
                onClick={() =>
                  void executeCommandWithToast(
                    'robot.encoders',
                    {},
                    {
                      loadingTitle: 'Consultando contadores Hall de ruedas...',
                      successTitle: 'Estado de encoders recibido',
                    }
                  )
                }
              >
                Consultar `robot.encoders`
              </Button>
            </>
          }
        />

        {sensors.receivedAtMs === null && (
          <StatusNotice
            tone="info"
            title="Sin muestras de telemetría en la sesión actual"
            description="Conecte con el broker ESP8266 y solicite robot.sensors para recibir lecturas reales de IR1..IR5 y HC-SR04."
            className="mt-4"
          />
        )}

        <div className="mt-6 grid grid-cols-1 gap-6 lg:grid-cols-12">
          {/* 5 canales de línea IR1..IR5 */}
          <div className="afr2-surface-cell flex flex-col justify-between gap-4 p-5 lg:col-span-7">
            <div className="flex flex-wrap items-center justify-between gap-2">
              <div>
                <h3 className="text-base font-bold" style={{ color: 'var(--text-strong)' }}>
                  Arreglo Infrarrojo de 5 Canales (`IR1..IR5`)
                </h3>
                <p className="text-xs" style={{ color: 'var(--text-muted)' }}>
                  Entradas digitales independientes; no infieren distancia ultrasónica.
                </p>
              </div>
              <div className="text-right">
                <span
                  className="afr2-tabular rounded px-2 py-0.5 text-xs font-bold uppercase"
                  style={{
                    backgroundColor:
                      lineFreshness === 'fresh' ? 'var(--success-subtle)' : 'var(--surface-solid)',
                    color: lineFreshness === 'fresh' ? 'var(--success)' : 'var(--text-muted)',
                  }}
                >
                  {lineFreshness}
                </span>
                <p className="afr2-tabular mt-1 text-xs" style={{ color: 'var(--text-muted)' }}>
                  {formatAgeLabel(sensors.line.ageMs)}
                </p>
              </div>
            </div>

            <div className="grid grid-cols-5 gap-2.5">
              {[0, 1, 2, 3, 4].map((idx) => {
                const isKnown = lineBits !== null;
                const active = isKnown ? (lineBits & (1 << idx)) !== 0 : false;
                return (
                  <div
                    key={idx}
                    className="flex flex-col items-center justify-center rounded-xl border p-3 text-center"
                    style={{
                      backgroundColor: !isKnown
                        ? 'var(--surface-solid)'
                        : active
                          ? 'var(--action-subtle)'
                          : 'var(--surface-solid)',
                      borderColor: !isKnown
                        ? 'var(--border-subtle)'
                        : active
                          ? 'var(--action)'
                          : 'var(--border-default)',
                    }}
                  >
                    <span className="afr2-tabular text-xs font-semibold" style={{ color: 'var(--text-muted)' }}>
                      IR{idx + 1}
                    </span>
                    <span
                      className="afr2-tabular mt-1 text-xl font-bold"
                      style={{
                        color: !isKnown
                          ? 'var(--text-muted)'
                          : active
                            ? 'var(--action)'
                            : 'var(--text-strong)',
                      }}
                    >
                      {!isKnown ? '—' : active ? '1' : '0'}
                    </span>
                    <span className="mt-1 text-[11px]" style={{ color: 'var(--text-muted)' }}>
                      {!isKnown ? 'sin dato' : active ? 'detectado' : 'libre'}
                    </span>
                  </div>
                );
              })}
            </div>

            <div className="flex flex-wrap items-center justify-between gap-2 border-t pt-3 text-xs" style={{ borderColor: 'var(--border-subtle)' }}>
              <span style={{ color: 'var(--text-muted)' }}>Máscara binaria (`line_bits`):</span>
              <span className="afr2-tabular font-bold" style={{ color: 'var(--text-strong)' }}>
                {lineBits !== null ? `0b${lineBits.toString(2).padStart(5, '0')} (0x${lineBits.toString(16).toUpperCase()})` : '—'}
              </span>
            </div>
          </div>

          {/* Ultrasonido HC-SR04 */}
          <div className="afr2-surface-cell flex flex-col justify-between gap-4 p-5 lg:col-span-5">
            <div className="flex flex-wrap items-center justify-between gap-2">
              <div>
                <h3 className="text-base font-bold" style={{ color: 'var(--text-strong)' }}>
                  Ultrasonido Frontal `HC-SR04`
                </h3>
                <p className="text-xs" style={{ color: 'var(--text-muted)' }}>
                  Medición por tiempo de vuelo con bandera explícita `ultrasonic_valid`.
                </p>
              </div>
              <span
                className="afr2-tabular rounded px-2 py-0.5 text-xs font-bold uppercase"
                style={{
                  backgroundColor:
                    usFreshness === 'fresh' ? 'var(--success-subtle)' : 'var(--surface-solid)',
                  color: usFreshness === 'fresh' ? 'var(--success)' : 'var(--text-muted)',
                }}
              >
                {usFreshness}
              </span>
            </div>

            <div
              className="flex flex-col items-center justify-center rounded-xl border p-6 text-center"
              style={{
                backgroundColor: 'var(--surface-solid)',
                borderColor: 'var(--border-default)',
              }}
            >
              <span className="text-xs font-semibold uppercase" style={{ color: 'var(--text-muted)' }}>
                Distancia medida
              </span>
              <p
                className="afr2-tabular mt-2 font-bold"
                style={{
                  fontSize: 'var(--text-telemetry-lg)',
                  color: 'var(--text-strong)',
                }}
              >
                {sensors.ultrasonic.valid && sensors.ultrasonic.distanceMm !== null
                  ? `${sensors.ultrasonic.distanceMm} mm`
                  : '—'}
              </p>
              <p className="afr2-tabular mt-1 text-xs" style={{ color: 'var(--text-muted)' }}>
                Validez: {sensors.ultrasonic.valid ? 'válida' : 'sin eco válido'} ·{' '}
                {formatAgeLabel(sensors.ultrasonic.ageMs)}
              </p>
            </div>
          </div>
        </div>
      </GlassSurface>) },
{ id: 'ruedas', label: 'Ruedas', content: (<GlassSurface variant="panel" className="p-5 sm:p-6">
        <SectionHeader
          eyebrow="Odometría de Ruedas"
          title="Encoders de las ruedas"
          description="Separados físicamente del encoder UI KY-040. Mientras no estén cableados al ATmega2560, el contrato reporta wired=false."
          badge={sensors.encoders.wired ? 'Cableados' : 'unwired-in-supplied-docs'}
          badgeTone={sensors.encoders.wired ? 'success' : 'warning'}
        />

        <div className="mt-4 grid grid-cols-2 gap-3 sm:grid-cols-4">
          {(['fl', 'fr', 'rl', 'rr'] as const).map((wheelKey) => {
            const val =
              sensors.encoders.wired && sensors.encoders.valid
                ? sensors.encoders.counts[wheelKey]
                : null;
            return (
              <div
                key={wheelKey}
                className="afr2-surface-cell flex flex-col items-center justify-center p-4 text-center"
              >
                <span className="afr2-tabular text-xs font-bold uppercase" style={{ color: 'var(--text-muted)' }}>
                  Rueda {wheelKey.toUpperCase()}
                </span>
                <span className="afr2-tabular mt-1.5 text-2xl font-bold" style={{ color: 'var(--text-strong)' }}>
                  {val !== null ? `${val}` : '—'}
                </span>
                <span className="mt-1 text-[11px]" style={{ color: 'var(--text-muted)' }}>
                  {val !== null ? 'cuentas int32' : 'sin cablear / sin muestra'}
                </span>
              </div>
            );
          })}
        </div>
      </GlassSurface>) }]} />
</div>);
}
