import { UiIcon } from './UiIcon';

export type DismissibleNoticeTone = 'neutral' | 'info' | 'warning' | 'error' | 'success';

export interface DismissibleNoticeProps {
  id?: string;
  tone?: DismissibleNoticeTone;
  title?: string;
  message: string;
  onDismiss: () => void;
  dismissLabel?: string;
  className?: string;
}

/**
 * Aviso compartido cerrable con X para cabecera, modales y bloques de estado (VIS-018).
 * - Area tactil minima de 44×44 px, accesible por teclado y con foco visible.
 * - Cerrar oculta unicamente la representacion del mensaje; conserva el estado real de STOP, enlace y control.
 * - Texto multilinea sin desbordes.
 */
export function DismissibleNotice({
  id,
  tone = 'warning',
  title,
  message,
  onDismiss,
  dismissLabel = 'Cerrar aviso',
  className = '',
}: DismissibleNoticeProps) {
  return (
    <div
      id={id}
      role={tone === 'error' || tone === 'warning' ? 'alert' : 'status'}
      aria-live="polite"
      data-tone={tone}
      className={`afr2-dismissible-notice ${className}`.trim()}
    >
      <div className="afr2-dismissible-notice__content">
        <span className="afr2-status-dot mt-1.5 shrink-0" aria-hidden="true" />
        <div className="afr2-dismissible-notice__copy">
          {title && <strong className="afr2-dismissible-notice__title">{title}</strong>}
          <p className="afr2-dismissible-notice__message">{message}</p>
        </div>
      </div>
      <button
        type="button"
        onClick={onDismiss}
        aria-label={dismissLabel}
        title={dismissLabel}
        className="afr2-dismissible-notice__close"
      >
        <UiIcon name="close" className="w-4 h-4" />
      </button>
    </div>
  );
}
