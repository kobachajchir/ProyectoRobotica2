/**
 * AFR2-20261102-01. Modelos que se pueden inspeccionar y su simulador. Fuente única para la portada
 * de Modelos y simulación, la página de inspección y el selector del simulador.
 * Las imágenes son renders CAD (no fotos del robot real) en public/brand/models/.
 */
import type { UiIconName } from '../../components/ui/UiIcon';
import type { SimulationKind } from '../collector/CollectorContext';

export type ModelPageId = 'auto' | 'brazo' | 'conjunto' | 'cubo';

export interface ModelPage {
  id: ModelPageId;
  title: string;
  kicker: string;
  description: string;
  icon: UiIconName;
  simulation: SimulationKind;
  simulationLabel: string;
  /** Render horizontal 2,4:1 (escritorio) y 16:10 (celular). Sin imagen: solo texto. */
  banner?: { wide: string; narrow: string; alt: string };
}

export const MODEL_PAGES: Record<ModelPageId, ModelPage> = {
  auto: {
    id: 'auto', title: 'Auto robot', kicker: 'Base Mecanum 4WD',
    description: 'Chasis, ruedas Mecanum, placas y sensores con sus referencias.',
    icon: 'control', simulation: 'auto', simulationLabel: 'Simular el auto',
    banner: { wide: '/brand/models/model-auto-1200.webp', narrow: '/brand/models/model-auto-640.webp', alt: 'Render CAD del auto Mecanum con sus cuatro ruedas, placas y sensor ultrasónico.' },
  },
  brazo: {
    id: 'brazo', title: 'Brazo robótico', kicker: 'Brazo V17 de seis ejes',
    description: 'Articulaciones, transmisiones y pinza del brazo V17.',
    icon: 'arm', simulation: 'brazo', simulationLabel: 'Simular el brazo',
    banner: { wide: '/brand/models/model-brazo-1200.webp', narrow: '/brand/models/model-brazo-640.webp', alt: 'Render CAD del brazo robótico V17 blanco con su pinza.' },
  },
  conjunto: {
    id: 'conjunto', title: 'Simulación conjunta', kicker: 'Auto y brazo juntos',
    description: 'El brazo montado sobre el auto, listo para recolectar cubos.',
    icon: 'cube', simulation: 'conjunta', simulationLabel: 'Simular el conjunto',
    banner: { wide: '/brand/models/model-conjunto-1200.webp', narrow: '/brand/models/model-conjunto-640.webp', alt: 'Render CAD del brazo V17 montado sobre el auto Mecanum, sosteniendo un cubo.' },
  },
  cubo: {
    id: 'cubo', title: 'Cubo auxiliar', kicker: 'Pieza de 50 mm',
    description: 'Cubo de referencia que se manipula en la simulación conjunta.',
    icon: 'cube', simulation: 'conjunta', simulationLabel: 'Simular el conjunto',
  },
};

/** Portada: los tres banners principales. El cubo queda como enlace secundario. */
export const MODEL_BANNERS: readonly ModelPageId[] = ['auto', 'brazo', 'conjunto'];

export const isModelPageId = (value: string | undefined): value is ModelPageId =>
  value === 'auto' || value === 'brazo' || value === 'conjunto' || value === 'cubo';
