import { createPortal } from 'react-dom';
import { Toaster, toast } from 'sonner';
import { useTheme } from '../../app/providers/ThemeProvider';
import { UiIcon } from '../ui/UiIcon';

export type ToastTone = 'info' | 'success' | 'warning' | 'error' | 'loading';

export interface NotifyOptions {
  id?: string | number;
  title: string;
  description?: string;
  duration?: number;
  actionLabel?: string;
  onAction?: () => void;
}

const CONNECTION_TOAST_ID = 'afr2-connection';
let hadConnectionLossInSession = false;

function ToneIcon({ tone }: { tone: ToastTone }) {
  if (tone === 'loading') {
    return (
      <span
        className="inline-block h-4 w-4 shrink-0 animate-spin rounded-full border-2 border-current border-t-transparent"
        style={{ animationDuration: 'var(--duration-spinner)' }}
        aria-hidden="true"
      />
    );
  }
  // AFR2-20261101-07 / VIS-050: iconos del registro comun (Lucide).
  const name = tone === 'success' ? 'success' : tone === 'warning' ? 'warning' : tone === 'error' ? 'error' : 'info';
  return <UiIcon name={name} size={16} className="shrink-0" />;
}

const TONE_STYLES: Record<ToastTone, { color: string; bg: string; border: string; label: string }> = {
  info: {
    color: 'var(--info)',
    bg: 'var(--info-subtle)',
    border: 'var(--info-border)',
    label: 'INFO',
  },
  success: {
    color: 'var(--success)',
    bg: 'var(--success-subtle)',
    border: 'var(--success-border)',
    label: 'CONFIRMADO',
  },
  warning: {
    color: 'var(--warning)',
    bg: 'var(--warning-subtle)',
    border: 'var(--warning-border)',
    label: 'ATENCIÓN',
  },
  error: {
    color: 'var(--error)',
    bg: 'var(--error-subtle)',
    border: 'var(--error-border)',
    label: 'FALLO',
  },
  loading: {
    color: 'var(--action)',
    bg: 'var(--action-subtle)',
    border: 'var(--action-border)',
    label: 'EN VUELO',
  },
};

const TONE_LABELS: Record<ToastTone, string> = { info: 'Información', success: 'Listo', warning: 'Atención', error: 'Error', loading: 'En curso' };
function renderCustomToast(tone: ToastTone, options: NotifyOptions) {
  const meta = TONE_STYLES[tone];
  return toast.custom(id => (
    <div role={tone === 'error' || tone === 'warning' ? 'alert' : 'status'} className="afr2-glass-popover afr2-toast" style={{ borderColor: meta.border }}>
      <div className="afr2-toast__icon" style={{ color: meta.color }}><ToneIcon tone={tone} /></div>
      <div className="afr2-toast__copy">
        <span className="afr2-toast__tone" style={{ color: meta.color }}>{TONE_LABELS[tone]}</span>
        <p className="afr2-toast__title">{options.title}</p>
        {options.description && <p className="afr2-toast__description">{options.description}</p>}
        {options.actionLabel && options.onAction && <button type="button" className="afr2-toast__action" onClick={() => { options.onAction?.(); toast.dismiss(id); }}>{options.actionLabel}</button>}
      </div>
      <button type="button" aria-label="Cerrar notificación" className="afr2-toast__close" onClick={() => toast.dismiss(id)}><UiIcon name="close" size={16} /></button>
    </div>
  ), { id: options.id, duration: options.duration ?? (tone === 'loading' ? Infinity : tone === 'error' ? 6000 : 4000) });
}

export const notify = {
  info: (options: NotifyOptions) => renderCustomToast('info', options),
  success: (options: NotifyOptions) => renderCustomToast('success', options),
  warning: (options: NotifyOptions) => renderCustomToast('warning', options),
  error: (options: NotifyOptions) => renderCustomToast('error', options),
  loading: (options: NotifyOptions) => renderCustomToast('loading', options),
  dismiss: (id?: string | number) => toast.dismiss(id),

  /**
   * Notifica pérdida de enlace usando el ID estable `afr2-connection` para evitar apilar toasts en reintentos.
   */
  connectionLost: (reason: string) => {
    hadConnectionLossInSession = true;
    return renderCustomToast('warning', {
      id: CONNECTION_TOAST_ID,
      title: 'Enlace WebSocket interrumpido',
      description: reason,
      duration: 5000,
    });
  },

  /**
   * Solo notifica recuperación si previamente se observó una caída en la sesión actual (VIS-010).
   */
  connectionRecovered: (detail: string) => {
    if (!hadConnectionLossInSession) return;
    hadConnectionLossInSession = false;
    return renderCustomToast('success', {
      id: CONNECTION_TOAST_ID,
      title: 'Enlace restablecido',
      description: detail,
      duration: 3500,
    });
  },
};

export function Afr2Toaster() {
  const { resolvedTheme } = useTheme();
  return createPortal(<Toaster
    className="afr2-toaster"
    theme={resolvedTheme}
    position="bottom-right"
    visibleToasts={1}
    expand
    closeButton={false}
    style={{
      width: 'min(400px, calc(100vw - var(--afr2-toast-left) - var(--afr2-toast-right)))',
      maxWidth: 'calc(100vw - 24px)', left: 'auto', right: 'var(--afr2-toast-right)', bottom: 'var(--afr2-toast-bottom)',
    }}
    offset={24}
    mobileOffset={16}
    toastOptions={{ unstyled: true, style: { width: '100%', maxWidth: '100%', minWidth: 0 } }}
  />, document.body);
}
