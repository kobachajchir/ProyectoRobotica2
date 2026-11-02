import { Link } from 'react-router-dom';
import { useConnection } from '../connection/ConnectionContext';
import { useRobotMode } from '../connection/useRobotMode';
import { batteryVoltage } from '../../protocol/battery';

/**
 * AFR2-20261102-01. Banner de Inicio con el render CAD del robot (plan visual 01-11, «Banner de Inicio»).
 * Es el <h1> de Inicio. Los chips muestran datos reales del enlace; lo desconocido es «—» y la batería
 * es voltaje medido, nunca porcentaje. Sin animación propia: entra con la transición de página.
 * El triple toque sobre la imagen abre Desarrollador (también en Configuración → Acerca de).
 */
export function HomeHero({ onSecretTap }: { onSecretTap: () => void }) {
  const { wsState, megaLinkState, wifiStatus, battery } = useConnection();
  const mode = useRobotMode();
  const wifi = wsState !== 'ready' ? 'Sin conexión' : wifiStatus.connectionInterface === 'AP' ? 'Punto de acceso' : wifiStatus.staConnected ? (wifiStatus.staSsid || 'Conectado') : '—';
  const link = wsState !== 'ready' ? '—' : megaLinkState === 'ready' ? 'Conectado' : 'Sin enlace';
  const chips: [string, string][] = [['Wi-Fi', wifi], ['ATmega', link], ['Modo', mode || '—'], ['Batería', batteryVoltage(battery)]];
  return <section className="afr2-hero" aria-labelledby="afr2-home-title">
    <picture className="afr2-hero__media" onClick={onSecretTap}>
      <source media="(min-width: 840px)" srcSet="/brand/home-hero-1600.webp" />
      <img src="/brand/home-hero-mobile-720.webp" alt="Render CAD del robot: base Mecanum con el brazo V17 sosteniendo un cubo." decoding="async" />
    </picture>
    <div className="afr2-hero__copy">
      <p className="afr2-hero__kicker">AutoFinalRobo2 · Mecanum + brazo V17</p>
      <h1 id="afr2-home-title">Movimiento en todas las direcciones</h1>
      <p>Conducí la base Mecanum, revisá sus sensores y probá el brazo en simulación antes de llevarlo al robot.</p>
      <ul className="afr2-hero__chips" aria-label="Estado del robot">
        {chips.map(([label, value]) => <li key={label}><Link to="/conexion" className="afr2-hero__chip"><span>{label}</span><strong>{value}</strong></Link></li>)}
      </ul>
    </div>
    <span className="afr2-hero__tag">Render CAD</span>
  </section>;
}
