import React from 'react';

export interface SectionHeaderProps {
  eyebrow?: string;
  title: string;
  description?: string;
  badge?: string;
  badgeTone?: 'neutral' | 'info' | 'success' | 'warning' | 'error';
  actions?: React.ReactNode;
}

const BADGE_COLORS: Record<NonNullable<SectionHeaderProps['badgeTone']>, { color: string; bg: string; border: string }> = {
  neutral: {
    color: 'var(--text-muted)',
    bg: 'var(--surface-subtle)',
    border: 'var(--border-default)',
  },
  info: {
    color: 'var(--info)',
    bg: 'var(--info-subtle)',
    border: 'var(--info-border)',
  },
  success: {
    color: 'var(--success)',
    bg: 'var(--success-subtle)',
    border: 'var(--success-border)',
  },
  warning: {
    color: 'var(--warning)',
    bg: 'var(--warning-subtle)',
    border: 'var(--warning-border)',
  },
  error: {
    color: 'var(--error)',
    bg: 'var(--error-subtle)',
    border: 'var(--error-border)',
  },
};

export function SectionHeader({
  eyebrow,
  title,
  description,
  badge,
  badgeTone = 'neutral',
  actions,
}: SectionHeaderProps) {
  const tone = BADGE_COLORS[badgeTone];

  return (
    <div className="afr2-section-header">
      <div className="min-w-0">
        {eyebrow && (
          <p
            className="afr2-eyebrow"
            style={{ color: 'var(--action)' }}
          >
            {eyebrow}
          </p>
        )}
        <div className="mt-0.5 flex flex-wrap items-center gap-2.5">
          <h2
            className="text-xl font-semibold tracking-tight sm:text-2xl"
            style={{
              color: 'var(--text-strong)',
              fontFamily: 'var(--font-display)',
            }}
          >
            {title}
          </h2>
          {badge && (
            <span
              className="afr2-tabular rounded-full border px-2.5 py-0.5 text-xs font-semibold"
              style={{
                color: tone.color,
                backgroundColor: tone.bg,
                borderColor: tone.border,
              }}
            >
              {badge}
            </span>
          )}
        </div>
        {description && (
          <p className="mt-3 max-w-2xl text-sm leading-relaxed" style={{ color: 'var(--text-muted)' }}>
            {description}
          </p>
        )}
      </div>
      {actions && <div className="flex shrink-0 flex-wrap items-center gap-2">{actions}</div>}
    </div>
  );
}
