import type { ReactNode } from 'react';
import { UiIcon } from './UiIcon';

export type NoticeTone = 'neutral' | 'info' | 'success' | 'warning' | 'error';

export interface StatusNoticeProps {
  tone?: NoticeTone;
  title: string;
  description?: ReactNode;
  children?: ReactNode;
  badge?: string;
  action?: ReactNode;
  onDismiss?: () => void;
  dismissLabel?: string;
  className?: string;
}

export function StatusNotice({
  tone = 'neutral',
  title,
  description,
  children,
  badge,
  action,
  onDismiss,
  dismissLabel = 'Cerrar aviso',
  className = '',
}: StatusNoticeProps) {
  return (
    <div
      role="status"
      aria-live="polite"
      data-tone={tone}
      className={`afr2-status-notice rounded-[var(--radius-sm)] px-4 py-3 text-sm ${className}`.trim()}
    >
      <div className="flex flex-wrap items-start justify-between gap-3">
        <div className="flex items-start gap-2.5 min-w-0 flex-1">
          <span className="afr2-status-dot mt-1.5" aria-hidden="true" />
          <div className="min-w-0 flex-1">
            <div className="flex flex-wrap items-center gap-2">
              <strong className="font-semibold text-[var(--text)]">{title}</strong>
              {badge && (
                <span className="inline-flex items-center rounded-md px-2 py-0.5 text-xs font-mono border border-[var(--border)] bg-[var(--surface-solid)] text-[var(--text-muted)]">
                  {badge}
                </span>
              )}
            </div>
            {description && (
              <div className="mt-1 text-xs leading-relaxed text-[var(--text-muted)] break-words">
                {description}
              </div>
            )}
            {children && (
              <div className="mt-1 text-xs leading-relaxed text-[var(--text-muted)] break-words">
                {children}
              </div>
            )}
          </div>
        </div>
        {(action || onDismiss) && (
          <div className="shrink-0 flex items-center gap-2">
            {action}
            {onDismiss && (
              <button
                type="button"
                onClick={onDismiss}
                aria-label={dismissLabel}
                title={dismissLabel}
                className="afr2-dismissible-notice__close"
              >
                <UiIcon name="close" className="w-4 h-4" />
              </button>
            )}
          </div>
        )}
      </div>
    </div>
  );
}
