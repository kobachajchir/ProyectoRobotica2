import { useCallback, useEffect, useLayoutEffect, useRef, useState, type PropsWithChildren } from 'react';
import { createPortal } from 'react-dom';
import { AFR2_BRAND_LOGO } from './BrandMark';
import { useConnection } from '../../features/connection/ConnectionContext';

/** Adapted from ST2026-Radal StartupWelcome (335d3ed): logo readiness,
 * logo in/hold/out, acrylic veil/halo and 4s fallback. VIS-017 (no card). */
export function StartupWelcome({ children }: PropsWithChildren) {
  const [visible, setVisible] = useState(true), [logoReady, setLogoReady] = useState(false);
  const dialog = useRef<HTMLDialogElement>(null), logo = useRef<HTMLImageElement>(null), focusFrame = useRef(0);
  const { triggerEmergencyStop, stopAvailable } = useConnection();
  const finish = useCallback(() => {
    const restore = dialog.current?.contains(document.activeElement);
    dialog.current?.close(); setVisible(false);
    if (restore) focusFrame.current = requestAnimationFrame(() => document.getElementById('afr2-main')?.focus({ preventScroll: true }));
  }, []);

  useLayoutEffect(() => {
    const element = dialog.current;
    if (visible && element && !element.open) element.showModal();
    return () => { element?.close(); };
  }, [visible]);
  const prepareLogo = useCallback((image: HTMLImageElement) => {
    // Start the choreography only after the actual image has been decoded.
    void image.decode().then(() => setLogoReady(true), finish);
  }, [finish]);
  useEffect(() => {
    if (logo.current?.complete) {
      if (logo.current.naturalWidth > 0) prepareLogo(logo.current);
      else finish();
    }
  }, [finish, prepareLogo]);
  useEffect(() => {
    if (!visible) return;
    const timeout = window.setTimeout(finish, 4000);
    return () => clearTimeout(timeout);
  }, [visible, finish]);
  useEffect(() => () => cancelAnimationFrame(focusFrame.current), []);

  return <>
    <div className="afr2-startup-content" inert={visible} aria-hidden={visible || undefined}>{children}</div>
    {visible && createPortal(<dialog ref={dialog} className="afr2-startup" aria-label="Bienvenido a AutoFinalRobo2" data-ready={logoReady}
      onCancel={event => { event.preventDefault(); finish(); }}
      onAnimationEnd={event => { if (event.target === event.currentTarget && event.animationName === 'afr2-startup-veil-out') finish(); }}>
      {stopAvailable && <div className="afr2-startup__actions"><button type="button" className="afr2-startup__stop" onClick={() => { void triggerEmergencyStop('startup-stop'); finish(); }}>■ STOP</button></div>}
      <div className="afr2-startup__logo" aria-hidden="true"><img ref={logo} src={AFR2_BRAND_LOGO} alt="" width="1024" height="768" loading="eager" fetchPriority="high" decoding="async"
        onLoad={event => prepareLogo(event.currentTarget)} onError={finish} /></div>
      <button type="button" autoFocus className="afr2-startup__skip" onClick={finish}>Omitir introduccion <span aria-hidden="true">· Esc</span></button>
    </dialog>, document.body)}
  </>;
}
