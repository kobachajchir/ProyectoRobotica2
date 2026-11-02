import type { SVGProps } from 'react';
import { ICON_PATHS, type UiIconName } from './icons/registry';

export type { UiIconName };
export type UiIconSize = 16 | 20 | 24;

/**
 * AFR2-20261101-07 / VIS-050. Una sola familia de iconos (Lucide + propios del dominio),
 * grilla 24 px, trazo 1,75 px y color `currentColor`. Tamaños: 16 en texto e insignias,
 * 20 en botones y barra lateral (por defecto), 24 en tarjetas.
 * Decorativo por defecto (`aria-hidden`). Si el icono es el único contenido de un
 * botón, el nombre accesible va en el botón (`aria-label`), o se pasa `label` aquí.
 */
export function UiIcon({ name, size = 20, label, ...props }: Omit<SVGProps<SVGSVGElement>, 'name'> & {
  name: UiIconName; size?: UiIconSize; label?: string;
}) {
  const a11y = label ? { role: 'img', 'aria-label': label } : { 'aria-hidden': true as const };
  return <svg width={size} height={size} viewBox="0 0 24 24" fill="none" stroke="currentColor" strokeWidth="1.75"
    strokeLinecap="round" strokeLinejoin="round" focusable="false" {...a11y} {...props}>
    {ICON_PATHS[name].map((d, index) => <path key={index} d={d} />)}
  </svg>;
}
