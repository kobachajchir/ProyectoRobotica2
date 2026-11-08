import { useEffect, useRef, useState } from 'react';
import { Button } from '../../components/ui/Button';
import { StatusNotice } from '../../components/ui/StatusNotice';
import { Chevron } from '../../components/ui/Chevron';
import { useConnection } from '../connection/ConnectionContext';

/**
 * AFR2-20261108-04. Selector de prueba de odometría: dirección y distancia se proponen al auto con
 * `robot.calibrate` (acción 102, valor = token<<16 | dirección<<8 | cm). El auto abre una pantalla de
 * confirmación; con el encoder el usuario inicia (clic) o descarta (pulsación larga). La Web consulta
 * el estado con `robot.config.get` (acción 103, valor = token<<8 | estado) hasta recibir la respuesta.
 * No hay evento push: la respuesta llega por consulta periódica.
 */
const ACTION_PROPOSE = 102;
const ACTION_STATE = 103;
// Códigos 0..7 iguales a AFR2_A_MOVE del ATmega.
export const ODOMETRY_DIRECTIONS = [
  'Adelante', 'Atrás', 'Izquierda', 'Derecha',
  'Diagonal adelante-izquierda', 'Diagonal adelante-derecha', 'Diagonal atrás-izquierda', 'Diagonal atrás-derecha',
] as const;
export const ODOMETRY_MIN_CM = 5;
export const ODOMETRY_MAX_CM = 100;
export const ODOMETRY_STEP_CM = 5;
// Estados que informa el auto (AFR2_A_ODOM_STATE & 0xff).
const STATE_TEXT: Record<number, { tone: 'info' | 'success' | 'warning'; title: string; text: string }> = {
  1: { tone: 'info', title: 'Esperando al auto', text: 'En el auto: clic del encoder inicia la prueba y pulsación larga la descarta.' },
  2: { tone: 'success', title: 'Prueba lanzada', text: 'El auto inició el desplazamiento y muestra la odometría actual y el objetivo.' },
  3: { tone: 'warning', title: 'Prueba descartada', text: 'Se descartó en el auto (o se abandonó la pantalla): no se movió.' },
  4: { tone: 'warning', title: 'No se pudo lanzar', text: 'El auto aceptó iniciar pero la prueba fue rechazada (motores, Hall, pose o radar). Revisá el aviso en su pantalla.' },
  5: { tone: 'warning', title: 'Propuesta vencida', text: 'Pasaron 60 s sin respuesta en el auto: no se movió.' },
};
export const packOdometryProposal = (token: number, direction: number, cm: number) => (token << 16) | (direction << 8) | cm;
export const odometryToken = () => 1 + Math.floor(Math.random() * 32766);
const POLL_MS = 700;
const GIVE_UP_MS = 75_000;

export function OdometryTestPanel() {
  const { wsState, megaLinkState, capabilities, requestCommand, robotState } = useConnection();
  const [direction, setDirection] = useState(0);
  const [cm, setCm] = useState(50);
  const [state, setState] = useState<number | null>(null);
  const [message, setMessage] = useState('');
  const [busy, setBusy] = useState(false);
  const run = useRef(0), mounted = useRef(true);
  const ready = wsState === 'ready' && megaLinkState === 'ready' && capabilities.configuration.implemented;
  const stopped = robotState?.stopped === true;
  const waiting = state === 1;
  const validCm = Number.isInteger(cm) && cm >= ODOMETRY_MIN_CM && cm <= ODOMETRY_MAX_CM && cm % ODOMETRY_STEP_CM === 0;

  useEffect(() => { mounted.current = true; return () => { mounted.current = false; ++run.current; }; }, []);
  useEffect(() => { if (!ready) { ++run.current; setBusy(false); } }, [ready]);

  const send = async () => {
    if (!ready || busy || !validCm) return;
    const ticket = ++run.current, token = odometryToken();
    setBusy(true); setState(null); setMessage('Enviando la prueba al auto…');
    const live = () => mounted.current && run.current === ticket;
    try {
      const res = await requestCommand<{ value: number }>('robot.calibrate', { action: ACTION_PROPOSE, value: packOdometryProposal(token, direction, cm) }, 4000);
      if (!live()) return;
      if (res.status !== 'completed') throw new Error('El auto no aceptó la prueba.');
      setState(1); setMessage('');
      const started = Date.now();
      while (live() && Date.now() - started < GIVE_UP_MS) {
        await new Promise(resolve => setTimeout(resolve, POLL_MS));
        if (!live()) return;
        const poll = await requestCommand<{ value: number }>('robot.config.get', { action: ACTION_STATE }, 2500);
        if (!live()) return;
        const value = poll.status === 'completed' ? poll.payload.value : NaN;
        if (!Number.isInteger(value) || (value >> 8) !== token) continue;
        const next = value & 0xff;
        setState(next);
        if (next !== 1) return;
      }
      if (live()) { setState(null); setMessage('No hubo respuesta del auto a tiempo. Mirá su pantalla y volvé a proponer la prueba.'); }
    } catch (error) {
      if (live()) { setState(null); setMessage(error instanceof Error ? error.message : 'No se pudo enviar la prueba.'); }
    } finally {
      if (mounted.current && run.current === ticket) setBusy(false);
    }
  };
  const stopWaiting = () => { ++run.current; setBusy(false); setState(null); setMessage('Dejaste de esperar. Si el auto sigue mostrando la pantalla, descartala con pulsación larga.'); };

  const outcome = state !== null ? STATE_TEXT[state] : undefined;
  return <section className="afr2-calibration-editor space-y-4" aria-label="Prueba de odometría">
    <h2 className="text-base font-semibold">Prueba de odometría</h2>
    <p className="afr2-help">Elegí una dirección y una distancia (hasta 1 m). Se envía al auto, que muestra la propuesta en su pantalla: el clic del encoder inicia la prueba y la pulsación larga la descarta. Acá vas a ver cuál de las dos pasó.</p>
    {!ready && <StatusNotice tone="info" title="Sin conexión">Conectá el Mega para proponer la prueba.</StatusNotice>}
    {ready && !stopped && <StatusNotice tone="info" title="Salidas activas">El auto solo acepta la propuesta con las salidas apagadas.</StatusNotice>}
    <form className="space-y-4" onSubmit={event => { event.preventDefault(); void send(); }}>
      <label className="block">Dirección
        <span className="afr2-select-wrap">
          <select disabled={busy} className="afr2-glass-field w-full min-h-11 px-3" value={direction} onChange={event => setDirection(Number(event.target.value))}>
            {ODOMETRY_DIRECTIONS.map((name, index) => <option key={name} value={index}>{name}</option>)}
          </select><Chevron />
        </span>
      </label>
      <label className="block">Distancia en cm ({ODOMETRY_MIN_CM}…{ODOMETRY_MAX_CM}, de a {ODOMETRY_STEP_CM})
        <input disabled={busy} className="afr2-glass-field w-full min-h-11 px-3" type="number" required min={ODOMETRY_MIN_CM} max={ODOMETRY_MAX_CM}
          step={ODOMETRY_STEP_CM} value={cm} onChange={event => setCm(Number(event.target.value))} />
      </label>
      <div className="afr2-action-row">
        <Button type="submit" variant="primary" disabled={!ready || !stopped || busy || !validCm}>Enviar al auto</Button>
        {waiting && <Button type="button" onClick={stopWaiting}>Dejar de esperar</Button>}
      </div>
    </form>
    {message && <p role="status" className="afr2-help">{message}</p>}
    {outcome && <StatusNotice tone={outcome.tone} title={outcome.title}>{outcome.text}</StatusNotice>}
  </section>;
}
