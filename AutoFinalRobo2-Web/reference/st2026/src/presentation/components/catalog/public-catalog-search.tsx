import { useCallback, useContext, useEffect, useId, useMemo, useRef, useState, type CSSProperties } from "react";
import { createPortal } from "react-dom";
import { useLocation, useNavigate } from "react-router-dom";
import { Command } from "cmdk";
import { ArrowRight, PackageOpen, RotateCw, Search, Wrench, X } from "lucide-react";
import { Spinner } from "@/presentation/components/ui/spinner";
import { useLocalCatalogRevision } from "@/presentation/hooks/use-local-catalog-revision";
import { DesktopNavigationContext } from "@/desktop/use-desktop-navigation";
import { findPublicSearchItems, loadPublicSearchItems, type CatalogSearchItem } from "./public-catalog-search-data";
import "@/presentation/styles/public-catalog-search.css";

type Phase = "closed" | "opening" | "open" | "closing";

export function PublicCatalogSearch({ brand = "radal", platform = false, desktop = false, appearance = "brand", style, className = "" }: { brand?: string; platform?: boolean; desktop?: boolean; appearance?: "brand" | "glass"; style?: CSSProperties; className?: string }) {
  const navigate = useNavigate(), location = useLocation();
  const desktopNavigation = useContext(DesktopNavigationContext);
  const id = useId();
  const trigger = useRef<HTMLButtonElement>(null);
  const dialog = useRef<HTMLDialogElement>(null);
  const bar = useRef<HTMLDivElement>(null);
  const material = useRef<HTMLDivElement>(null);
  const input = useRef<HTMLInputElement>(null);
  const phaseRef = useRef<Phase>("closed");
  const frame = useRef(0), exitTimer = useRef(0);
  const [phase, setPhase] = useState<Phase>("closed");
  const [expanded, setExpanded] = useState(false);
  const [instant, setInstant] = useState(false);
  const [query, setQuery] = useState("");
  const [generation, setGeneration] = useState(0);
  const localRevision = useLocalCatalogRevision();
  const [state, setState] = useState<{ items: CatalogSearchItem[]; loading: boolean; partial: boolean }>({ items: [], loading: false, partial: false });
  const active = phase !== "closed";

  const finishClose = useCallback(() => {
    window.cancelAnimationFrame(frame.current); window.clearTimeout(exitTimer.current);
    phaseRef.current = "closed"; setPhase("closed"); setExpanded(false);
    dialog.current?.close();
    trigger.current?.focus({ preventScroll: true });
  }, []);

  function collapseMaterial() {
    const origin = trigger.current?.getBoundingClientRect(), target = bar.current?.getBoundingClientRect();
    if (!origin || !target || !material.current) return;
    material.current.style.transform = `translateX(${origin.left - target.left}px)`;
    material.current.style.clipPath = `inset(0 ${Math.max(0, target.width - origin.width)}px 0 0 round 24px)`;
  }

  const close = useCallback((keyboard = false) => {
    if (phaseRef.current === "closed") return;
    window.cancelAnimationFrame(frame.current);
    window.clearTimeout(exitTimer.current);
    const reduced = window.matchMedia("(prefers-reduced-motion: reduce)").matches;
    setInstant(keyboard); setExpanded(false);
    phaseRef.current = "closing"; setPhase("closing");
    if (keyboard) { finishClose(); return; }
    if (!reduced) collapseMaterial();
    exitTimer.current = window.setTimeout(finishClose, reduced ? 120 : 200);
  }, [finishClose]);

  const open = useCallback((keyboard = false) => {
    if (phaseRef.current !== "closed") return;
    const origin = trigger.current?.getBoundingClientRect();
    if (!origin || !dialog.current) return;
    const reduced = window.matchMedia("(prefers-reduced-motion: reduce)").matches;
    window.clearTimeout(exitTimer.current);
    setInstant(keyboard); setQuery(""); setExpanded(false);
    phaseRef.current = "opening"; setPhase("opening");
    // Preserve the trigger's vertical position and height; only X changes on expansion.
    dialog.current.style.setProperty("--search-top", `${origin.top}px`);
    dialog.current.style.setProperty("--search-bar-height", `${origin.height}px`);
    dialog.current.showModal();
    if (material.current) {
      material.current.style.transition = "none";
      if (!keyboard && !reduced) collapseMaterial();
      else { material.current.style.transform = "translate(0, 0)"; material.current.style.clipPath = "inset(0 round 28px)"; }
      // One measurement establishes the starting geometry; no layout is animated.
      material.current.getBoundingClientRect();
    }
    frame.current = window.requestAnimationFrame(() => {
      if (phaseRef.current !== "opening") return;
      if (material.current) { material.current.style.transition = ""; material.current.style.transform = "translate(0, 0)"; material.current.style.clipPath = "inset(0 round 28px)"; }
      setExpanded(true); phaseRef.current = "open"; setPhase("open"); input.current?.focus({ preventScroll: true });
    });
  }, []);

  useEffect(() => {
    if (!desktop) return;
    return desktopNavigation?.registerCatalogSearch(() => open(true));
  }, [desktop, desktopNavigation, open]);

  useEffect(() => {
    if (!active) return;
    let current = true;
    setState(value => ({ ...value, loading: true }));
    void loadPublicSearchItems(platform, desktop).then(result => { if (current) setState({ ...result, loading: false }); }, () => { if (current) setState(value => ({ ...value, loading: false, partial: true })); });
    return () => { current = false; };
  }, [active, platform, desktop, generation, localRevision]);
  useEffect(() => {
    if (!active) return;
    const overflow = document.body.style.overflow;
    document.body.style.overflow = "hidden";
    return () => { document.body.style.overflow = overflow; };
  }, [active]);
  useEffect(() => { if (phaseRef.current !== "closed") finishClose(); }, [location.key, finishClose]);
  useEffect(() => {
    if (!active) return;
    const width = window.innerWidth;
    // Opening a mobile keyboard changes height; keep the input and results open.
    const viewport = () => {
      const height = window.visualViewport?.height ?? window.innerHeight;
      const bottom = (bar.current?.getBoundingClientRect().bottom ?? 64) - (window.visualViewport?.offsetTop ?? 0);
      dialog.current?.style.setProperty("--search-results-height", `${Math.max(180, height - bottom - 26)}px`);
    };
    const resize = () => { viewport(); if (window.innerWidth !== width) close(true); };
    viewport();
    window.addEventListener("resize", resize);
    window.visualViewport?.addEventListener("resize", viewport);
    return () => { window.removeEventListener("resize", resize); window.visualViewport?.removeEventListener("resize", viewport); };
  }, [active, close]);
  useEffect(() => () => { window.cancelAnimationFrame(frame.current); window.clearTimeout(exitTimer.current); dialog.current?.close(); }, []);

  const matches = useMemo(() => findPublicSearchItems(state.items, query, brand), [state.items, query, brand]);
  const hasQuery = query.trim().length > 0;
  const shown = hasQuery ? matches.slice(0, 30) : [];
  return <>
    <button aria-controls={id} aria-expanded={active} aria-haspopup="dialog" aria-keyshortcuts={desktop ? "Alt+B" : undefined} aria-label={desktop ? "Buscar productos y servicios · Alt+B" : "Buscar productos y servicios"} title={desktop ? "Buscar productos y servicios · Alt+B" : "Buscar productos y servicios"} className={"public-catalog-search__trigger" + (platform ? " public-catalog-search__trigger--platform" : appearance === "glass" ? " public-catalog-search__trigger--glass" : "") + " " + className} data-search-active={active} onClick={event => { event.stopPropagation(); open(event.detail === 0); }} ref={trigger} style={style} type="button"><Search aria-hidden="true" /></button>
    {createPortal(<dialog aria-label="Buscar productos y servicios" aria-modal={active ? true : undefined} className={"public-catalog-search" + (platform ? " public-catalog-search--platform" : " brand-theme brand-theme--" + brand)} data-expanded={expanded} data-motion={instant ? "instant" : "animated"} data-phase={phase} id={id} onCancel={event => { event.preventDefault(); event.stopPropagation(); close(true); }} onClose={() => { if (phaseRef.current !== "closed") finishClose(); }} onClick={event => event.stopPropagation()} onKeyDown={event => {
      if (event.key === "Escape") { event.preventDefault(); event.stopPropagation(); close(true); }
      if (phaseRef.current === "closing") event.preventDefault();
    }} onPointerDown={event => {
      event.stopPropagation();
      const target = event.target as Element;
      if (!target.closest("[data-search-surface]")) { close(); return; }
      if (phaseRef.current === "closing" && target.closest(".public-catalog-search__bar")) {
        window.clearTimeout(exitTimer.current);
        phaseRef.current = "open"; setPhase("open"); setExpanded(true);
        if (material.current) { material.current.style.transform = "translateX(0)"; material.current.style.clipPath = "inset(0 round 28px)"; }
        input.current?.focus({ preventScroll: true });
      }
    }} ref={dialog}>
      <Command label="Productos y servicios de nuestras marcas" loop shouldFilter={false}>
        <div className="public-catalog-search__bar" data-search-surface ref={bar}>
          <div aria-hidden="true" className="public-catalog-search__material" ref={material}><Search /></div>
          <div className="public-catalog-search__input-wrap">
            <Command.Input aria-label="Buscar por producto, servicio, marca o código" autoComplete="off" autoCorrect="off" placeholder="Buscar productos y servicios…" ref={input} spellCheck={false} value={query} onValueChange={setQuery} />
            {query ? <button aria-label="Limpiar búsqueda" className="public-catalog-search__clear" onClick={() => { setQuery(""); input.current?.focus(); }} type="button"><X aria-hidden="true" /></button> : null}
            <button aria-label="Cerrar buscador" className="public-catalog-search__close" onClick={event => close(event.detail === 0)} type="button"><span>Esc</span><X aria-hidden="true" /></button>
          </div>
        </div>
        <section aria-label="Resultados de búsqueda" className="public-catalog-search__results" data-search-surface>
          <header className="public-catalog-search__heading"><span>{hasQuery ? "Resultados" : "¿Qué estás buscando?"}</span>{hasQuery ? <span>{state.loading ? "Buscando…" : matches.length + (matches.length === 1 ? " coincidencia" : " coincidencias")}</span> : null}</header>
          {!hasQuery ? <div className="public-catalog-search__state"><Search aria-hidden="true" /><p>Buscá un producto o servicio.</p><span>Escribí su nombre, marca, modelo o código.</span></div> : null}
          {hasQuery && state.loading && !state.items.length ? <div className="public-catalog-search__state" role="status"><Spinner className="h-7 w-7" /><p>Cargando productos y servicios…</p></div> : null}
          {state.partial ? <div className="public-catalog-search__notice" role="status"><span>Parte del catálogo no pudo cargarse.</span><button disabled={state.loading} onClick={() => setGeneration(value => value + 1)} type="button"><RotateCw aria-hidden="true" />Reintentar</button></div> : null}
          <Command.List aria-busy={state.loading} className="public-catalog-search__list">
            {shown.map(item => <Command.Item className="public-catalog-search__item" data-kind={item.kind} key={item.id} onSelect={() => { finishClose(); if (desktop && desktopNavigation) desktopNavigation.requestNavigation(item.path); else navigate(item.path); }} value={item.id}>
              <div className="public-catalog-search__image">{item.image ? <img alt="" decoding="async" loading="lazy" src={item.image} /> : item.kind === "servicio" ? <Wrench aria-hidden="true" /> : <PackageOpen aria-hidden="true" />}</div>
              <div className="public-catalog-search__copy"><strong>{item.title}</strong><span className="public-catalog-search__section"><span>Sección {item.brandName}</span><i aria-hidden="true" /><span>{item.kind === "servicio" ? "Servicio" : "Producto"}</span></span>{!["servicio", "servicios", "producto", "productos"].includes(item.section.toLocaleLowerCase("es-AR")) ? <span className="public-catalog-search__category">{item.section}</span> : null}</div><span aria-hidden="true" className="public-catalog-search__go"><ArrowRight /></span>
            </Command.Item>)}
            {hasQuery && !state.loading && !shown.length ? <div className="public-catalog-search__state" role="status"><Search aria-hidden="true" /><p>{state.partial ? "No hay coincidencias entre los elementos que pudimos cargar." : "No encontramos productos ni servicios con esa búsqueda."}</p><span>Probá con otra marca, nombre o código.</span></div> : null}
          </Command.List>
          <footer className="public-catalog-search__footer"><span>{hasQuery && matches.length > shown.length ? "Mostrando " + shown.length + " de " + matches.length + ". Escribí más para afinar la búsqueda." : "Moura · Yuasa · Fulmar · Radal"}</span><span>↑ ↓ elegir · Enter abrir</span></footer>
        </section>
      </Command>
    </dialog>, document.body)}
  </>;
}
