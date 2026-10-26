import { Link } from 'react-router-dom';
import { BrandMark } from '../../components/ui/BrandMark';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { UiIcon } from '../../components/ui/UiIcon';

/** Content structure adapted from ST2026-Radal DeveloperInformation.
 * Product colors, route and shell belong to AutoFinalRobo2. */
export function DeveloperPage() {
  return <div className="afr2-route-view afr2-page-stack">
    <SectionHeader title="Desarrollador" description="AutoFinalRobo2 · Ingenieria, firmware e interfaz." />
    <section className="afr2-developer-profile"><div className="afr2-developer-heading"><BrandMark size="hero" /><div className="afr2-developer-copy"><p className="afr2-kicker">Desarrollo del proyecto</p><h2>Koba Chajchir</h2><p>Una plataforma integrada para el robot Mecanum.</p></div></div>
      <div className="afr2-action-row"><a href="https://github.com/kobachajchir" target="_blank" rel="noopener noreferrer">GitHub ↗</a><a href="https://www.linkedin.com/in/kchajchir" target="_blank" rel="noopener noreferrer">LinkedIn ↗</a><a href="tel:+543884889973">388 488 9973</a></div>
    </section>
    <Link className="afr2-developer-back" to="/"><UiIcon name="back" />Volver al inicio</Link>
  </div>;
}
