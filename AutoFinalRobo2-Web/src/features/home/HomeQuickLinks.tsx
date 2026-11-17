import { Link } from 'react-router-dom';
import { UiIcon, type UiIconName } from '../../components/ui/UiIcon';
// AFR2-20261117-05: ilustraciones de menos de 2 KB incrustadas en el bundle (data URI). Como archivos sueltos
// superaban el límite de 24 activos del Storage del ESP y sumaban 3 bloques LittleFS.
import shotGoto from './shots/quick-goto.svg';
import shotControl from './shots/quick-control.svg';
import shotLine from './shots/quick-line.svg';

/** Accesos directos bajo el banner. AFR2-20261117-04: GOTO usa una imagen interior
 * con velo y marco independientes del contenido. shots/ conserva ilustraciones;
 * la fotografía original sigue pendiente de ubicación. */
const QUICK_LINKS: readonly { to: string; title: string; description: string; icon: UiIconName; shot: string }[] = [
  { to: '/programas?tarea=goto', title: 'Ir', description: 'Recorrido GOTO a un punto.', icon: 'crosshair', shot: shotGoto },
  { to: '/conducir', title: 'Controlar', description: 'Conducción táctil en cualquier dirección.', icon: 'touch', shot: shotControl },
  { to: '/seguidor-linea', title: 'Seguidor de línea', description: 'El auto sobre la cinta, en vivo.', icon: 'sensors', shot: shotLine },
];

export function HomeQuickLinks() {
  return <nav aria-label="Accesos rápidos" className="afr2-quick-links">
    {QUICK_LINKS.map(item => <Link key={item.to} to={item.to}
      className={`afr2-quick-link${item.icon === 'crosshair' ? ' afr2-quick-link--goto' : ''}`}>
      {item.icon === 'crosshair'
        ? <span className="afr2-quick-link__preview" aria-hidden="true"><img className="afr2-quick-link__image" src={item.shot} alt="" /></span>
        : <span className="afr2-quick-link__shot" aria-hidden="true" style={{ backgroundImage: `url("${item.shot}")` }} />}
      <span className="afr2-quick-link__symbol"><UiIcon name={item.icon} /></span>
      <span className="afr2-quick-link__copy"><strong>{item.title}</strong><span>{item.description}</span></span>
      <span className="afr2-quick-link__arrow" aria-hidden="true"><UiIcon name="arrow" /></span>
    </Link>)}
  </nav>;
}
