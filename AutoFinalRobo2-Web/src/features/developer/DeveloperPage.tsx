import { Link } from 'react-router-dom';
import { BrandMark } from '../../components/ui/BrandMark';
import { PageHeader } from '../../components/ui/PageHeader';
import { UiIcon } from '../../components/ui/UiIcon';


import '../../styles/protocol-studio.css';

/** Content structure adapted from ST2026-Radal DeveloperInformation.
 * Product colors, route and shell belong to AutoFinalRobo2. */
export function DeveloperPage() {
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader title="Desarrollador" description="AutoFinalRobo2 · Ingeniería, firmware e interfaz." />
    <section className="afr2-developer-profile"><div className="afr2-developer-heading"><BrandMark size="hero" /><div className="afr2-developer-copy"><p className="afr2-kicker">Desarrollo del proyecto</p><h2>Koba Chajchir</h2><p>Una plataforma integrada para el robot Mecanum.</p></div></div>
      <div className="afr2-action-row"><a href="https://github.com/kobachajchir" target="_blank" rel="noopener noreferrer">GitHub <UiIcon name="external" size={16} /></a><a href="https://www.linkedin.com/in/kchajchir" target="_blank" rel="noopener noreferrer">LinkedIn <UiIcon name="external" size={16} /></a><a href="tel:+543884889973">388 488 9973</a></div>
    </section>
    <Link className="afr2-protocol-card afr2-developer-studio" to="/desarrollador/protocol-studio">
      <div className="afr2-protocol-output-heading"><h2>Protocol Studio</h2><UiIcon name="arrow" /></div>
      <p>Traduce capturas HEX a texto, inspecciona tramas ROBO2 y copia comandos en distintos formatos.</p>
    </Link>
    <Link className="afr2-developer-back" to="/"><UiIcon name="back" />Volver al inicio</Link>
  </div>;
}
