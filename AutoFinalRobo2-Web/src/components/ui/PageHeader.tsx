import type { ReactNode } from 'react';
import { HeaderBadge, type HeaderBadgeTone } from './SectionHeader';
import { UiIcon, type UiIconName } from './UiIcon';

const HEADER_ICONS: Record<string, UiIconName> = {
  'Conducir': 'control', 'Control': 'control', 'Programas': 'programs',
  'Puesta a punto': 'settings', 'Sensores': 'sensors', 'Conexión': 'wifi',
  'Conexion': 'wifi', 'Ayuda': 'book', 'Desarrollador': 'settings',
  'Protocol Studio': 'programs', 'Simulador': 'cube', 'Modelos y simulación': 'cube',
  'Armado del robot': 'arm',
};

export interface PageHeaderProps {
  /** Repite el nombre de la página en la barra lateral. Es el único `<h1>` de la página. */
  title: string;
  /** Una línea: qué se hace en la página. */
  description?: ReactNode;
  eyebrow?: string;
  badge?: string;
  badgeTone?: HeaderBadgeTone;
  /** Acciones de la página, a la derecha del título (p. ej. ayuda o volver). */
  actions?: ReactNode;
  /** Fila opcional pegada a la cabecera (p. ej. un selector segmentado). */
  children?: ReactNode;
}

/**
 * AFR2-20261101-10 / VIS-051. Cabecera de página: fila 1 con `<h1>` (`--type-title`) y acciones,
 * fila 2 con la descripción (`--type-body`, `--text-muted`, hasta 60 caracteres por línea) y
 * fila 3 opcional. Espacios: 8 px título-descripción, 20 px hasta la fila 3 y 32 px hasta el contenido.
 */
export function PageHeader({ title, description, eyebrow, badge, badgeTone, actions, children }: PageHeaderProps) {
  return (
    <header className="afr2-page-header">
      <span className="afr2-page-header__glow" aria-hidden="true" />
      <div className="afr2-page-header__row">
        <div className="afr2-page-header__titles">
          {eyebrow && <p className="afr2-kicker">{eyebrow}</p>}
          <div className="afr2-page-header__title-line">
            <span className="afr2-page-header__icon" aria-hidden="true"><UiIcon name={HEADER_ICONS[title] ?? 'cube'} size={24} /></span>
            <h1 className="afr2-page-header__title">{title}</h1>
            {badge && <HeaderBadge text={badge} tone={badgeTone} />}
          </div>
        </div>
        {actions && <div className="afr2-page-header__actions">{actions}</div>}
      </div>
      {description && <p className="afr2-page-header__description">{description}</p>}
      {children && <div className="afr2-page-header__extra">{children}</div>}
    </header>
  );
}
