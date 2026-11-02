import React from 'react';

export type HeaderBadgeTone = 'neutral' | 'info' | 'success' | 'warning' | 'error';

export interface SectionHeaderProps {
  eyebrow?: string;
  title: string;
  description?: string;
  badge?: string;
  badgeTone?: HeaderBadgeTone;
  actions?: React.ReactNode;
  /** Nivel del encabezado: 2 para bloques o pestañas (por defecto), 3 dentro de otro bloque. */
  level?: 2 | 3;
}

/** Insignia compartida por PageHeader y SectionHeader. */
export function HeaderBadge({ text, tone = 'neutral' }: { text: string; tone?: HeaderBadgeTone }) {
  return <span className="afr2-header-badge afr2-tabular" data-tone={tone}>{text}</span>;
}

/**
 * AFR2-20261101-10 / VIS-051. Encabezado interno de un bloque o pestaña (`<h2>`, `--type-heading`;
 * `<h3>`, `--type-subheading`, con level 3). La cabecera de página es PageHeader (`<h1>`).
 */
export function SectionHeader({ eyebrow, title, description, badge, badgeTone = 'neutral', actions, level = 2 }: SectionHeaderProps) {
  const Heading = level === 3 ? 'h3' : 'h2';
  return (
    <div className="afr2-section-header" data-level={level}>
      <div className="min-w-0">
        {eyebrow && <p className="afr2-eyebrow">{eyebrow}</p>}
        <div className="afr2-section-header__title-line">
          <Heading className="afr2-section-header__title">{title}</Heading>
          {badge && <HeaderBadge text={badge} tone={badgeTone} />}
        </div>
        {description && <p className="afr2-section-header__description">{description}</p>}
      </div>
      {actions && <div className="afr2-section-header__actions">{actions}</div>}
    </div>
  );
}
