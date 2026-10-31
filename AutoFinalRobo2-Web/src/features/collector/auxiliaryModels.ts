/** Scene props are independent assets, never selectable robot/arm rigs. */
import type { Point3 } from './robotModel';

export interface AuxiliaryModelDefinition {
  id: string; label: string; url: string; size: Point3; units: 'm';
}
export const CUBE_MODEL: AuxiliaryModelDefinition = {
  id: 'cube-50mm', label: 'Cubo de referencia', url: '/models/AFR2_Cubo_50mm.glb',
  size: [.05, .05, .05], units: 'm',
};
export const AUXILIARY_MODELS: AuxiliaryModelDefinition[] = [CUBE_MODEL];
