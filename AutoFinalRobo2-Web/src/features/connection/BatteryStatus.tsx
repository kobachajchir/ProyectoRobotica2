/** AFR2-20261029-26. Same pill material as network; measured volts only. */
import { useEffect, useState } from 'react';
import { useConnection } from './ConnectionContext';
import { batteryAge, batteryIsFresh, batteryVoltage } from '../../protocol/battery';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import type { ModalOrigin } from '../../components/ui/FloatingModal';

const BATTERY_ICONS = ['battery-0', 'battery-1', 'battery-2', 'battery-3', 'battery-4'] as const;
function BatteryIcon({ millivolts }: { millivolts: number | null }) {
  // Voltage bands only, not state of charge, chemistry or low-battery alarms.
  const bars = millivolts === null || millivolts <= 0 ? 0 : millivolts < 6000 ? 1 : millivolts < 7000 ? 2 : millivolts < 8000 ? 3 : 4;
  // AFR2-20261101-07 / VIS-050: icono propio del registro; sin lectura muestra la batería con raya (dato desconocido).
  return <UiIcon name={millivolts === null ? 'battery-unknown' : BATTERY_ICONS[bars]} />;
}
function useBatteryStatus() {
  const connection = useConnection();
  const [now, setNow] = useState(Date.now());
  useEffect(() => { const timer = window.setInterval(() => setNow(Date.now()), 500); return () => clearInterval(timer); }, []);
  const available = connection.wsState === 'ready' && connection.megaLinkState === 'ready' && connection.capabilities.battery_voltage?.implemented === true;
  const fresh = available && batteryIsFresh(connection.battery, now);
  const responseFresh = !!connection.battery && now - connection.battery.receivedAtMs <= 2000;
  const state = connection.battery?.state;
  const label = !available ? connection.wsState === 'ready' && connection.megaLinkState === 'ready' ? 'No disponible' : 'Sin conexión' : !connection.battery ? 'Sin datos' : !responseFresh ? 'Lectura caducada' : state === 'off' ? 'Sin medición' : state === 'measuring' ? 'Midiendo…' : state === 'out-of-range' ? 'Fuera de rango' : !fresh ? 'Lectura caducada' : batteryVoltage(connection.battery);
  return { ...connection, available, fresh, label, age: batteryAge(connection.battery, now) };
}
export function BatteryPill({ onOpen }: { onOpen: (origin: ModalOrigin) => void }) {
  const { battery, fresh, label } = useBatteryStatus();
  return <button type="button" className="afr2-network-pill afr2-battery-pill" data-mode="unknown" data-fresh={fresh}
    aria-haspopup="dialog" aria-label={`Batería: ${label}. Abrir medición de batería`} title={`Batería · ${label}`}
    onClick={event => onOpen({ element: event.currentTarget, keyboard: event.detail === 0 })}>
    <BatteryIcon millivolts={fresh ? battery?.millivolts ?? null : null}/><span>Batería</span><strong>{label}</strong>
  </button>;
}
export function BatteryDetails() {
  const { battery, available, fresh, label, age, refreshBattery } = useBatteryStatus();
  const [busy, setBusy] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const measure = async () => {
    if (busy) return;
    setBusy(true); setError(null);
    try { await refreshBattery(true); }
    catch (cause) { setError(cause instanceof Error ? cause.message : 'No se pudo iniciar la medición.'); }
    finally { setBusy(false); }
  };
  return <div className="afr2-page-stack">
    <p className="afr2-help">El controlador mide automáticamente desde el arranque con el divisor conectado a la entrada analógica A5. Podés solicitar una lectura nueva desde este panel.</p>
    <dl className="afr2-network-details">
      <div><dt>Voltaje de batería</dt><dd>{fresh ? batteryVoltage(battery) : '—'}</dd></div>
      <div><dt>Estado</dt><dd>{fresh ? 'Medición actual' : label}</dd></div>
      <div><dt>Antigüedad</dt><dd>{available && age !== null ? `${Math.round(age)} ms` : '—'}</dd></div>
      <div><dt>Lectura ADC</dt><dd>{available && age !== null && age <= 2000 ? battery?.raw ?? '—' : '—'}</dd></div>
      <div><dt>Calibración</dt><dd>Referencia nominal de 5 V · pendiente</dd></div>
    </dl>
    <div className="afr2-action-row"><Button variant="primary" loading={busy} disabled={!available} onClick={() => void measure()}>Actualizar medición de batería</Button></div>
    {error && <p className="afr2-help" role="alert">{error}</p>}
    <p className="afr2-help">Divisor 10 kΩ + 10 kΩ, sin diodo; capacitor 104 entre A5 y GND. El voltaje no indica un porcentaje de carga.</p>
  </div>;
}
