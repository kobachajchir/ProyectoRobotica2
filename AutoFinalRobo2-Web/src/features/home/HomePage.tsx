import { Link, useNavigate } from 'react-router-dom';
import { useRef } from 'react';
import { BrandMark } from '../../components/ui/BrandMark';
import { UiIcon, type UiIconName } from '../../components/ui/UiIcon';
import { useOverlays } from '../../app/providers/OverlayContext';
import { SectionArtwork } from './SectionArtwork';

const SECTIONS: { to: string; title: string; description: string; icon: UiIconName }[] = [
  { to: '/control', title: 'Control', description: 'Movimiento, pruebas y calibración.', icon: 'control' },
  { to: '/sensores', title: 'Sensores', description: 'Línea, distancia y ruedas.', icon: 'sensors' },
  { to: '/wifi', title: 'Wi-Fi', description: 'Tu conexión con el robot.', icon: 'wifi' },
  { to: '/oled', title: 'Pantalla OLED', description: 'Abrí la pantalla en vivo.', icon: 'screen' },
  { to: '/modelo-3d', title: 'Modelo 3D', description: 'Explorá el modelo del robot.', icon: 'cube' },
  { to: '/documentacion', title: 'Documentación', description: 'Guías y referencias del proyecto.', icon: 'book' },
];
export function HomePage() {
  const navigate = useNavigate();
  const taps = useRef({ count: 0, at: 0 });
  const developer = () => { const now = performance.now(); taps.current.count = now - taps.current.at < 900 ? taps.current.count + 1 : 1; taps.current.at = now; if (taps.current.count === 3) { taps.current.count = 0; navigate('/desarrollador'); } };
  const { openOled } = useOverlays();
  return <div className="afr2-route-view afr2-page-stack">
    <section className="afr2-welcome"><button type="button" className="afr2-brand-trigger" aria-label="AutoFinalRobo2" onClick={developer}><BrandMark size="hero" /></button><div><p className="afr2-kicker">Tu espacio de trabajo</p><h1>¿Qué querés hacer?</h1><p>Elegí una sección. El resto puede esperar.</p></div></section>
    <nav aria-label="Elegir sección" className="afr2-home-grid">{SECTIONS.map(item => {
      const content = <><SectionArtwork name={item.icon} /><div className="afr2-section-copy"><span className="afr2-section-symbol"><UiIcon name={item.icon} /></span><h2>{item.title}</h2><p>{item.description}</p><span className="afr2-section-entry" aria-hidden="true">{item.icon === 'screen' ? 'Ver pantalla' : 'Abrir sección'}<UiIcon name="arrow" /></span></div></>;
      return item.icon === 'screen' ? <button type="button" key={item.to} className="afr2-section-card" aria-haspopup="dialog" onClick={event => openOled({ element: event.currentTarget, keyboard: event.detail === 0 })}>{content}</button>
        : <Link to={item.to} key={item.to} className="afr2-section-card">{content}</Link>;
    })}</nav>

  </div>;
}
