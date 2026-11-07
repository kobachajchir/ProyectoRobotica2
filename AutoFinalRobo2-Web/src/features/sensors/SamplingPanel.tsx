/* AFR2-20261106-35. Muestreo del HC-SR04 desde la Web.
 * El ATmega es dueño del disparo: la Web solo sobrescribe su intervalo con robot.ultrasonic.configure
 * (70..1000 ms, rechazado con BUSY durante el barrido automático) y registra las muestras que observa
 * en la telemetría del cabezal. Tres modos:
 *  - Período fijo: aplica un intervalo y registra mientras esté activo.
 *  - Por cantidad: aplica un intervalo y registra N muestras nuevas; luego se detiene solo.
 *  - Variable: alterna entre intervalo mínimo y máximo según cambie la distancia medida.
 * Una muestra con id salteado se cuenta como no observada; nunca se inventa un valor. */
import { useEffect, useRef, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { Disclosure } from '../../components/ui/Disclosure';
import { SegmentedControl, type SegmentedItem } from '../../components/ui/SegmentedControl';
import { useConnection } from '../connection/ConnectionContext';
import { SENSOR_ERRORS, type HeadReading, type RangeSample } from '../../protocol/sensorHead';
import { ScanRangeField } from './ScanControls';

type Mode = 'fixed' | 'count' | 'variable';
const MODES: readonly SegmentedItem<Mode>[] = [
  { value: 'fixed', label: 'Período fijo' }, { value: 'count', label: 'Por cantidad' }, { value: 'variable', label: 'Variable' },
];
interface Logged { sample: RangeSample; atMs: number; intervalMs: number }
const MAX_LOG = 200;
const RECONFIGURE_GAP_MS = 500;

export function SamplingPanel({ online, head, running }: { online: boolean; head: HeadReading | null; running: boolean }) {
  const { requestCommand } = useConnection();
  const [mode, setMode] = useState<Mode>('fixed');
  const [periodMs, setPeriodMs] = useState(100), [count, setCount] = useState(20);
  const [minMs, setMinMs] = useState(70), [maxMs, setMaxMs] = useState(500), [thresholdMm, setThresholdMm] = useState(30);
  const [active, setActive] = useState(false), [log, setLog] = useState<Logged[]>([]), [missed, setMissed] = useState(0);
  const [status, setStatus] = useState<string | null>(null), [applied, setApplied] = useState<number | null>(null);
  const last = useRef({ id: 0, distance: null as number | null, configuredAt: 0, interval: 0, sending: false });
  const target = useRef(count); target.current = count;

  const configure = async (intervalMs: number) => {
    const ref = last.current;
    if (ref.sending || ref.interval === intervalMs) return true;
    ref.sending = true;
    try {
      await requestCommand('robot.ultrasonic.configure', { intervalMs });
      ref.interval = intervalMs; ref.configuredAt = Date.now(); setApplied(intervalMs); return true;
    } catch (e) {
      setStatus(`El ATmega rechazó el intervalo: ${e instanceof Error ? e.message : 'sin detalle'}`); setActive(false); return false;
    } finally { ref.sending = false; }
  };
  const start = async () => {
    setStatus(null); setLog([]); setMissed(0);
    last.current = { id: head?.sample.sampleId ?? 0, distance: null, configuredAt: 0, interval: 0, sending: false };
    const ok = await configure(mode === 'variable' ? maxMs : periodMs);
    if (ok) { setActive(true); setStatus(mode === 'count' ? `Registrando ${count} muestras…` : 'Registrando muestras…'); }
  };
  const stop = (message = 'Registro detenido. El ATmega conserva el último intervalo aplicado.') => { setActive(false); setStatus(message); };

  useEffect(() => { if (active && (!online || running)) stop(running ? 'Detenido: el barrido automático toma el sensor.' : 'Detenido: sin conexión con el robot.'); }, [active, online, running]);
  useEffect(() => {
    if (!active || !head) return;
    const sample = head.sample, ref = last.current;
    if (!sample.sampleId || sample.sampleId === ref.id) return;
    const gap = ref.id ? ((sample.sampleId - ref.id) | 0) - 1 : 0;
    if (gap < 0) return; // muestra anterior o reinicio de secuencia
    ref.id = sample.sampleId;
    if (gap > 0) setMissed(n => n + gap);
    setLog(previous => {
      const next = [{ sample, atMs: Date.now(), intervalMs: ref.interval }, ...previous].slice(0, MAX_LOG);
      if (mode === 'count' && next.length >= target.current) queueMicrotask(() => stop(`Listo: ${target.current} muestras registradas.`));
      return next;
    });
    if (mode === 'variable' && sample.valid && sample.distanceMm !== null) {
      const changed = ref.distance !== null && Math.abs(sample.distanceMm - ref.distance) >= thresholdMm;
      ref.distance = sample.distanceMm;
      const wanted = changed ? minMs : Math.min(maxMs, Math.max(minMs, Math.round((ref.interval || maxMs) * 1.5)));
      if (wanted !== ref.interval && Date.now() - ref.configuredAt >= RECONFIGURE_GAP_MS) void configure(wanted);
    }
  }, [head, active, mode, minMs, maxMs, thresholdMm]);

  const locked = !online || running;
  const valid = log.filter(item => item.sample.valid).length;
  return <div className="afr2-radar-settings afr2-sampling"><Disclosure title="Muestreo HC-SR04"
    meta={active ? `Registrando · ${log.length}${mode === 'count' ? ` / ${count}` : ''}` : applied ? `Intervalo aplicado ${applied} ms` : 'Sin aplicar'}>
    <div className="afr2-sampling__body">
      <SegmentedControl size="sm" ariaLabel="Modo de muestreo" items={MODES} value={mode} onChange={value => { if (!active) setMode(value); }} />
      <div className="afr2-scan-parameters">
        {mode !== 'variable' && <ScanRangeField label="Período" value={periodMs} min={70} max={1000} step={10} unit="ms" disabled={locked || active} onChange={setPeriodMs} />}
        {mode === 'count' && <ScanRangeField label="Cantidad" value={count} min={1} max={MAX_LOG} step={1} unit="muestras" disabled={locked || active} onChange={setCount} />}
        {mode === 'variable' && <>
          <ScanRangeField label="Intervalo mínimo" value={minMs} min={70} max={maxMs} step={10} unit="ms" disabled={locked || active} onChange={setMinMs} />
          <ScanRangeField label="Intervalo máximo" value={maxMs} min={minMs} max={1000} step={10} unit="ms" disabled={locked || active} onChange={setMaxMs} />
          <ScanRangeField label="Cambio que acelera" value={thresholdMm} min={5} max={500} step={5} unit="mm" disabled={locked || active} onChange={setThresholdMm} />
        </>}
      </div>
      <div className="afr2-scan-apply">
        <p className="afr2-help">{!online ? 'Conectá el robot para muestrear.' : running ? 'Detené el barrido automático: el radar usa el sensor.'
          : mode === 'variable' ? 'Con distancia estable el intervalo crece hasta el máximo; un cambio vuelve al mínimo.' : 'Aplicar sobrescribe el intervalo del ATmega (en RAM).'}</p>
        {active ? <Button size="sm" variant="secondary" onClick={() => stop()}>Detener registro</Button>
          : <Button size="sm" disabled={locked} onClick={() => void start()}>Aplicar y registrar</Button>}
      </div>
      {status && <p className="afr2-sensor-feedback" role="status">{status}</p>}
      <p className="afr2-help">{log.length} observadas · {valid} válidas{missed ? ` · ${missed} no observadas por la Web (el sondeo es más lento que el período)` : ''}</p>
      {log.length > 0 && <div className="afr2-sampling__table" role="region" aria-label="Muestras registradas" tabIndex={0}><table>
        <thead><tr><th>#</th><th>Distancia</th><th>Ángulo</th><th>Intervalo</th><th>Estado</th></tr></thead>
        <tbody>{log.slice(0, 40).map(item => <tr key={item.sample.sampleId} data-valid={item.sample.valid || undefined}>
          <td>{item.sample.sampleId}</td><td>{item.sample.valid && item.sample.distanceMm !== null ? `${item.sample.distanceMm} mm` : '—'}</td>
          <td>{item.sample.angleDeg}°</td><td>{item.intervalMs} ms</td><td>{item.sample.valid ? 'Válida' : SENSOR_ERRORS[item.sample.error]}</td>
        </tr>)}</tbody>
      </table></div>}
    </div>
  </Disclosure></div>;
}
