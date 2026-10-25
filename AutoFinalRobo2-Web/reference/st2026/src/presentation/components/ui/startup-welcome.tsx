import { useEffect, useLayoutEffect, useRef, useState, type PropsWithChildren } from "react";
import { createPortal } from "react-dom";

import RadalHorizontalLight from "@/presentation/images/radal_horizontal_light.png";
import "@/presentation/styles/startup-welcome.css";

/** The public welcome material stays visible until the real Desktop task ends. */
export function StartupActivity({active,title,message,detail}:{active:boolean;title:string;message:string;detail:string}){
  const [rendered,setRendered]=useState(active),[logoReady,setLogoReady]=useState(false);
  const logoRef=useRef<HTMLImageElement>(null);
  useEffect(()=>{
    if(active){setRendered(true);return;}
    // Fallback for interrupted CSS animation events, never a simulated load time.
    const timeout=window.setTimeout(()=>setRendered(false),400);
    return()=>window.clearTimeout(timeout);
  },[active]);
  useEffect(()=>{if(logoRef.current?.complete&&logoRef.current.naturalWidth>0)setLogoReady(true);},[rendered]);
  if(!rendered)return null;
  return createPortal(<div className="startup-welcome brand-theme--radal" data-activity="true" data-desktop-lifecycle="true" data-ready={logoReady} data-active={active}
    onAnimationEnd={event=>{if(!active&&event.target===event.currentTarget&&event.animationName==="startup-welcome-veil-out")setRendered(false);}}>
    <div className="startup-welcome__activity">
      <div aria-hidden="true" className="startup-welcome__logo"><img alt="" decoding="async" loading="eager" src={RadalHorizontalLight} ref={logoRef} onLoad={()=>setLogoReady(true)} onError={()=>setLogoReady(false)}/></div>
      <div className="startup-welcome__status" role="status" aria-live="polite" aria-atomic="true">
        <h1>{title}</h1><p className="startup-welcome__step"><span aria-hidden="true" className="startup-welcome__spinner"/>{message}</p><p className="startup-welcome__detail">{detail}</p>
      </div>
    </div>
  </div>,document.body);
}

export function StartupWelcome({ children }: PropsWithChildren) {
  const [visible, setVisible] = useState(true);
  const [logoReady, setLogoReady] = useState(false);
  const contentRef = useRef<HTMLDivElement>(null);
  const logoRef = useRef<HTMLImageElement>(null);

  useLayoutEffect(() => {
    const content = contentRef.current;
    if (!content) return;
    content.inert = visible;
    return () => { content.inert = false; };
  }, [visible]);

  useEffect(() => {
    if (logoRef.current?.complete && logoRef.current.naturalWidth > 0) {
      setLogoReady(true);
    }
  }, []);

  useEffect(() => {
    if (!visible) return;
    // Release the page even if an image request or animation event fails.
    const timeout = window.setTimeout(() => setVisible(false), 4000);
    return () => window.clearTimeout(timeout);
  }, [visible]);

  return (
    <>
      <div
        aria-hidden={visible || undefined}
        className="startup-welcome-content"
        ref={contentRef}
      >
        {children}
      </div>
      {visible && createPortal(
        <div
          aria-label="Bienvenido a Radal"
          className="startup-welcome brand-theme--radal"
          data-ready={logoReady}
          onAnimationEnd={event => {
            if (event.target === event.currentTarget && event.animationName === "startup-welcome-veil-out") {
              setVisible(false);
            }
          }}
          role="status"
        >
          <div aria-hidden="true" className="startup-welcome__logo">
            <img
              alt=""
              decoding="async"
              {...{ fetchpriority: "high" }}
              loading="eager"
              onError={() => setVisible(false)}
              onLoad={() => setLogoReady(true)}
              ref={logoRef}
              src={RadalHorizontalLight}
            />
          </div>
        </div>,
        document.body,
      )}
    </>
  );
}
