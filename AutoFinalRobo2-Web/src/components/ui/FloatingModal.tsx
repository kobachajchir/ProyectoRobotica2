import { useCallback, useEffect, useId, useRef, useState, type ReactNode, type RefObject } from 'react';
import { createPortal } from 'react-dom';
import { Button } from './Button';
import { DismissibleNotice } from './DismissibleNotice';
import { UiIcon } from './UiIcon';
import { useConnection } from '../../features/connection/ConnectionContext';

type Phase = 'closed' | 'opening' | 'open' | 'closing';
export interface ModalOrigin {
  element: HTMLElement;
  keyboard: boolean;
}
interface Props {
  open: boolean;
  title: string;
  description?: string;
  kicker?: string;
  headerStatus?: ReactNode;
  size?: 'md' | 'lg' | 'fullscreen';
  origin?: ModalOrigin | null;
  initialFocusRef?: RefObject<HTMLElement | null>;
  onClose: () => void;
  onBeforeClose?: () => void;
  onBeforeStop?: () => void;
  children: ReactNode;
}

/** Adapted from ST2026-Radal PublicCatalogSearch (335d3ed): native dialog,
 * material reveal, 240/200 ms, glass backdrop and deferred exit. See provenance. */
export function FloatingModal({ open, title, description, kicker = 'AutoFinalRobo2', headerStatus, size = 'md', origin = null, initialFocusRef, onClose, onBeforeClose, onBeforeStop, children }: Props) {
  const id = useId();
  const dialog = useRef<HTMLDialogElement>(null);
  const panel = useRef<HTMLDivElement>(null);
  const material = useRef<HTMLDivElement>(null);
  const trigger = useRef<HTMLElement | null>(null);
  const phaseRef = useRef<Phase>('closed');
  const frame = useRef(0);
  const exitTimer = useRef(0);
  const backdropPress = useRef(false);
  const [phase, setPhase] = useState<Phase>('closed');
  const [expanded, setExpanded] = useState(false);
  const [instant, setInstant] = useState(false);
  const { triggerEmergencyStop, stopAvailable, stopRecord, stopNoticeVisible, dismissStopNotice } = useConnection();
  const closeCallback = useRef(onClose);
  closeCallback.current = onClose;
  const beforeClose = useRef(onBeforeClose); beforeClose.current = onBeforeClose;
  const active = phase !== 'closed';

  const finishClose = useCallback(() => {
    beforeClose.current?.();
    cancelAnimationFrame(frame.current);
    clearTimeout(exitTimer.current);
    phaseRef.current = 'closed';
    setPhase('closed');
    setExpanded(false);
    dialog.current?.close();
    if (trigger.current?.isConnected) trigger.current.focus({ preventScroll: true });
    closeCallback.current();
  }, []);

  const collapseMaterial = useCallback(() => {
    const source = trigger.current?.getBoundingClientRect();
    const target = panel.current?.getBoundingClientRect();
    if (!source || !target || !material.current) return;
    material.current.style.transform = `translate(${source.left - target.left}px, ${source.top - target.top}px)`;
    material.current.style.clipPath = `inset(0 ${Math.max(0, target.width - source.width)}px ${Math.max(0, target.height - source.height)}px 0 round 24px)`;
  }, []);

  const close = useCallback(
    (keyboard = false) => {
      if (phaseRef.current === 'closed') return;
      beforeClose.current?.();
      cancelAnimationFrame(frame.current);
      clearTimeout(exitTimer.current);
      setInstant(keyboard);
      setExpanded(false);
      phaseRef.current = 'closing';
      setPhase('closing');
      if (keyboard) {
        finishClose();
        return;
      }
      const reduced = matchMedia('(prefers-reduced-motion: reduce)').matches;
      if (!reduced) collapseMaterial();
      exitTimer.current = window.setTimeout(finishClose, reduced ? 120 : 200);
    },
    [collapseMaterial, finishClose]
  );

  useEffect(() => {
    if (!open) {
      if (phaseRef.current !== 'closed' && phaseRef.current !== 'closing') close();
      return;
    }
    if (phaseRef.current === 'open' || phaseRef.current === 'opening') return;
    clearTimeout(exitTimer.current);
    cancelAnimationFrame(frame.current);
    trigger.current = origin?.element ?? null;
    const keyboard = origin?.keyboard ?? true;
    const reduced = matchMedia('(prefers-reduced-motion: reduce)').matches;
    setInstant(keyboard);
    phaseRef.current = 'opening';
    setPhase('opening');
    if (!dialog.current?.open) dialog.current?.showModal();
    // Mount content before measuring. Changing layout is never animated.
    frame.current = requestAnimationFrame(() => {
      if (phaseRef.current !== 'opening') return;
      if (material.current) {
        material.current.style.transition = 'none';
        if (!keyboard && !reduced) collapseMaterial();
        else {
          material.current.style.transform = 'none';
          material.current.style.clipPath = size === 'fullscreen' ? 'inset(0 round 0px)' : 'inset(0 round 28px)';
        }
        material.current.getBoundingClientRect();
      }
      frame.current = requestAnimationFrame(() => {
        if (phaseRef.current !== 'opening') return;
        if (material.current) {
          material.current.style.transition = '';
          material.current.style.transform = 'none';
          material.current.style.clipPath = size === 'fullscreen' ? 'inset(0 round 0px)' : 'inset(0 round 28px)';
        }
        setExpanded(true);
        phaseRef.current = 'open';
        setPhase('open');
        (initialFocusRef?.current ?? dialog.current?.querySelector<HTMLButtonElement>('[data-modal-close]'))?.focus({ preventScroll: true });
      });
    });
  }, [open, origin, close, collapseMaterial, size, initialFocusRef]);

  useEffect(() => {
    if (!active) return;
    const overflow = document.body.style.overflow;
    document.body.style.overflow = 'hidden';
    return () => {
      document.body.style.overflow = overflow;
    };
  }, [active]);

  useEffect(() => {
    const element = dialog.current;
    return () => {
      cancelAnimationFrame(frame.current);
      clearTimeout(exitTimer.current);
      element?.close();
    };
  }, []);

  return createPortal(
    <dialog
      ref={dialog}
      className="afr2-floating-modal"
      aria-labelledby={`${id}-title`}
      aria-describedby={description ? `${id}-description` : undefined}
      data-expanded={expanded}
      data-phase={phase}
      data-motion={instant ? 'instant' : 'animated'}
      data-size={size}
      onCancel={(event) => {
        event.preventDefault();
        close(true);
      }}
      onClose={() => {
        if (phaseRef.current !== 'closed') finishClose();
      }}
      onPointerDown={(event) => {
        backdropPress.current = event.target === event.currentTarget;
        if (phaseRef.current === 'closing' && !backdropPress.current) {
          clearTimeout(exitTimer.current);
          phaseRef.current = 'open';
          setPhase('open');
          setExpanded(true);
          if (material.current) {
            material.current.style.transform = 'none';
            material.current.style.clipPath = size === 'fullscreen' ? 'inset(0 round 0px)' : 'inset(0 round 28px)';
          }
        }
      }}
      onPointerUp={(event) => {
        if (backdropPress.current && event.target === event.currentTarget) close();
        backdropPress.current = false;
      }}
    >
      <div ref={panel} className="afr2-floating-modal__surface">
        <div ref={material} className="afr2-floating-modal__material" aria-hidden="true" />
        <div className="afr2-floating-modal__content">
          <header className="afr2-floating-modal__header" data-has-status={!!headerStatus}>
            <div>
              <p className="afr2-kicker">{kicker}</p>
              <h2 id={`${id}-title`}>{title}</h2>
              {description && <p id={`${id}-description`}>{description}</p>}
            </div>
            <div className="afr2-action-row">
              {stopAvailable && <Button variant="emergency" onClick={() => { onBeforeStop?.(); void triggerEmergencyStop('modal-stop'); }}>
                ■ STOP
              </Button>}
              {headerStatus}
              <button
                data-modal-close
                className={`afr2-icon-button${size === 'fullscreen' ? ' afr2-fullscreen-close' : ''}`}
                type="button"
                aria-label={`Cerrar ${title}`}
                onClick={(event) => close(event.detail === 0)}
              >
                <UiIcon name="close" />
              </button>
            </div>
          </header>
          {stopNoticeVisible && stopRecord.status !== 'idle' && stopRecord.detail && (
            <div className="px-7 pb-4">
              <DismissibleNotice
                id={`modal-stop-notice-${stopRecord.noticeId}`}
                tone={stopRecord.status === 'confirmed' ? 'warning' : 'error'}
                title={stopRecord.status === 'confirmed' ? 'STOP confirmado' : 'Aviso de parada STOP'}
                message={stopRecord.detail}
                onDismiss={dismissStopNotice}
                dismissLabel="Cerrar aviso de parada en modal"
              />
            </div>
          )}
          <div className="afr2-floating-modal__body" inert={phase === 'closing'}>
            {active ? children : null}
          </div>
        </div>
      </div>
    </dialog>,
    document.body
  );
}
