/** AFR2-20261101-06. Dedicated workspace, using Protocol Studio's shell. */
import { useState } from 'react';
import { Link } from 'react-router-dom';
import { Button } from '../../components/ui/Button';
import { SectionHeader } from '../../components/ui/SectionHeader';
import { FloatingModal } from '../../components/ui/FloatingModal';
import { UiIcon } from '../../components/ui/UiIcon';
import { UiSimulator } from './UiSimulator';
import '../../styles/protocol-studio.css';

export function UiSimulatorPage() {
  const [helpOpen,setHelpOpen]=useState(false);
  return <div className="afr2-route-view afr2-page-stack afr2-protocol-studio afr2-ui-studio">
    <SectionHeader title="Simulador UI" description="Explora las pantallas y la navegacion del encoder del robot." badge="OLED · 128 × 64"
      actions={<div className="afr2-action-row"><Button variant="subtle" onClick={()=>setHelpOpen(true)}>Como usar el simulador</Button><Link className="afr2-developer-back" to="/desarrollador"><UiIcon name="back" />Desarrollador</Link></div>} />
    <UiSimulator />
    <FloatingModal open={helpOpen} onClose={()=>setHelpOpen(false)} title="Como usar el simulador" description="Pantallas y servicios ficticios en una sesion local.">
      <div className="afr2-page-stack">
        <p>En Simulador, elige un equipo de ejemplo y usa el encoder para recorrer las opciones. El clic corto confirma; el largo se ejecuta al soltar y depende de la pantalla actual.</p>
        <p>La guia junto al OLED describe cada gesto. Los cambios pendientes requieren elegir Si o No antes de salir.</p>
        <p>En Arbol y catalogo puedes consultar los destinos o abrir una pantalla directamente. Volver a Simulador conserva la sesion; Reiniciar simulacion restaura el equipo de ejemplo.</p>
        <p>Los sensores, motores, Wi-Fi y EEPROM son ejemplos en memoria. Usa credenciales ficticias. Salir de esta seccion descarta la sesion.</p>
      </div>
    </FloatingModal>
  </div>;
}