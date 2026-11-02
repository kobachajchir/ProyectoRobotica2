import { Link, useNavigate } from 'react-router-dom';
import { useRef } from 'react';
import { UiIcon } from '../../components/ui/UiIcon';
import { DESTINATIONS } from '../../app/navigation';
import { SectionArtwork } from './SectionArtwork';
import { HomeHero } from './HomeHero';

// AFR2-20261101-12 / VIS-053: los accesos repiten los destinos de la navegación; el visor OLED es un modal global.
const SECTIONS = DESTINATIONS.map(item => ({ to: item.to, title: item.label, description: item.description, icon: item.icon }));
export function HomePage() {
  const navigate = useNavigate();
  const taps = useRef({ count: 0, at: 0 });
  const developer = () => { const now = performance.now(); taps.current.count = now - taps.current.at < 900 ? taps.current.count + 1 : 1; taps.current.at = now; if (taps.current.count === 3) { taps.current.count = 0; navigate('/desarrollador'); } };
  return <div className="afr2-route-view afr2-page-stack">
    {/* AFR2-20261102-01: banner con el render del robot (plan visual, sección «Banner de Inicio»). */}
    <HomeHero onSecretTap={developer} />
    <nav aria-label="Elegir sección" className="afr2-home-grid">{SECTIONS.map(item => {
      const content = <><SectionArtwork name={item.icon} /><div className="afr2-section-copy"><span className="afr2-section-symbol"><UiIcon name={item.icon} /></span><h2>{item.title}</h2><p>{item.description}</p><span className="afr2-section-entry" aria-hidden="true">Abrir sección<UiIcon name="arrow" /></span></div></>;
      return <Link to={item.to} key={item.to} className="afr2-section-card">{content}</Link>;
    })}</nav>

  </div>;
}
