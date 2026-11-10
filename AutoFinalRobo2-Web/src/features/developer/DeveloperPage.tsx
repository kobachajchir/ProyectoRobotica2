import { useMemo } from 'react';
import { Link } from 'react-router-dom';
import { BrandMark } from '../../components/ui/BrandMark';
import { PageHeader } from '../../components/ui/PageHeader';
import { UiIcon } from '../../components/ui/UiIcon';
import { qrcodegen } from '../oled/vendor/qrcodegen';

import '../../styles/protocol-studio.css';
import '../../styles/developer.css';

const repositoryUrl = 'https://github.com/kobachajchir/ProyectoRobotica2';

function createRepositoryQr() {
  const qr = qrcodegen.QrCode.encodeText(repositoryUrl, qrcodegen.QrCode.Ecc.MEDIUM);
  const quietZone = 4;
  const modules: string[] = [];
  for (let y = 0; y < qr.size; y += 1) {
    for (let x = 0; x < qr.size; x += 1) {
      if (qr.getModule(x, y)) modules.push(`M${x + quietZone},${y + quietZone}h1v1h-1z`);
    }
  }
  return { size: qr.size + quietZone * 2, path: modules.join('') };
}

/** Content structure adapted from ST2026-Radal DeveloperInformation.
 * Product colors, route and shell belong to AutoFinalRobo2. */
export function DeveloperPage() {
  const repositoryQr = useMemo(createRepositoryQr, []);
  return <div className="afr2-route-view afr2-page-stack">
    <PageHeader title="Desarrollador" description="AutoFinalRobo2 · Ingeniería, firmware e interfaz." />
    <section className="afr2-developer-profile"><div className="afr2-developer-heading"><BrandMark size="hero" /><div className="afr2-developer-copy"><p className="afr2-kicker">Desarrollo del proyecto</p><h2>Koba Chajchir</h2><p>Una plataforma integrada para el robot Mecanum.</p></div></div>
      <div className="afr2-action-row"><a href="https://github.com/kobachajchir" target="_blank" rel="noopener noreferrer">GitHub <UiIcon name="external" size={16} /></a><a href="https://www.linkedin.com/in/kchajchir" target="_blank" rel="noopener noreferrer">LinkedIn <UiIcon name="external" size={16} /></a><a href="tel:+543884889973">388 488 9973</a></div>
    </section>
    <div className="afr2-protocol-card afr2-developer-repository" role="region" aria-labelledby="developer-repository-title">
      <div className="afr2-developer-repository__copy">
        <p className="afr2-kicker">Código del proyecto</p>
        <h2 id="developer-repository-title">AutoFinalRobo2 en GitHub</h2>
        <p>Encontrá el código, los planos y la documentación del proyecto en un mismo lugar.</p>
        <span className="afr2-developer-repository__url">github.com/kobachajchir/ProyectoRobotica2</span>
        <a className="afr2-developer-back afr2-developer-repository__link" href={repositoryUrl} target="_blank" rel="noopener noreferrer" aria-label="Abrir el repositorio AutoFinalRobo2 en GitHub (nueva pestaña)">
          Abrir en GitHub <UiIcon name="external" size={16} />
        </a>
      </div>
      <figure className="afr2-developer-repository__qr">
        <a className="afr2-developer-repository__code" href={repositoryUrl} target="_blank" rel="noopener noreferrer" aria-label="Abrir el repositorio desde el código QR (nueva pestaña)">
          <svg viewBox={`0 0 ${repositoryQr.size} ${repositoryQr.size}`} width={208} height={208} shapeRendering="crispEdges" aria-hidden="true" focusable="false">
            <rect width={repositoryQr.size} height={repositoryQr.size} fill="#fff" />
            <path d={repositoryQr.path} fill="#000" />
          </svg>
        </a>
        <figcaption>Escaneá para abrir el repositorio</figcaption>
      </figure>
    </div>
    <Link className="afr2-protocol-card afr2-developer-studio" to="/desarrollador/protocol-studio">
      <div className="afr2-protocol-output-heading"><h2>Protocol Studio</h2><UiIcon name="arrow" /></div>
      <p>Traduce capturas HEX a texto, inspecciona tramas ROBO2 y copia comandos en distintos formatos.</p>
    </Link>
    <Link className="afr2-developer-back" to="/"><UiIcon name="back" />Volver al inicio</Link>
  </div>;
}
