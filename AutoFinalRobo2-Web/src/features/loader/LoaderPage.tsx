import { useEffect, useRef, useState, type CSSProperties, type KeyboardEvent } from 'react';
import { Link } from 'react-router-dom';
import { UiIcon, type UiIconName } from '../../components/ui/UiIcon';
import { Button } from '../../components/ui/Button';
import { SegmentedControl, type SegmentedItem } from '../../components/ui/SegmentedControl';
import { FlowNode, Reveal, Stat, Wire } from '../infrastructure/infraPrimitives';
import { LOADER_JOURNEYS, LOADER_REASONS, LOADER_TOOLS, LOADER_VERSION, type LoaderJourney, type LoaderZone } from './loaderFlow';

/* AFR2-20261102-24: página del AFR2 Loader en Explorar, con el mismo lenguaje visual que
   Infraestructura (portada con carriles, línea de estaciones, ficha de etapa, cifras y tarjetas). */

const JOURNEY_CHOICES: readonly SegmentedItem<LoaderJourney>[] = [
  { value: 'mega', label: 'Cargar el ATmega', icon: 'board' },
  { value: 'esp', label: 'Cargar el ESP', icon: 'wifi' },
];
const ZONES: Record<LoaderZone, { label: string; icon: UiIconName }> = {
  pc: { label: 'En la computadora', icon: 'settings' },
  link: { label: 'AFR2 Loader', icon: 'arrow-down' },
  esp: { label: 'En las placas', icon: 'board' },
  client: { label: 'En las placas', icon: 'board' },
};

export function LoaderPage() {
  const [journey, setJourney] = useState<LoaderJourney>('mega');
  const [selected, setSelected] = useState(0);
  const journeyRef = useRef<HTMLSpanElement>(null);
  const stationRefs = useRef<(HTMLButtonElement | null)[]>([]);
  const steps = LOADER_JOURNEYS[journey];
  const step = steps[selected];
  const chooseJourney = (next: LoaderJourney) => { setJourney(next); setSelected(0); };
  const pick = (next: LoaderJourney, index: number) => {
    setJourney(next); setSelected(index);
    const reduce = window.matchMedia('(prefers-reduced-motion: reduce)').matches;
    journeyRef.current?.scrollIntoView({ behavior: reduce ? 'auto' : 'smooth', block: 'start' });
  };
  const onStationKey = (event: KeyboardEvent<HTMLOListElement>) => {
    const delta = event.key === 'ArrowRight' || event.key === 'ArrowDown' ? 1 : event.key === 'ArrowLeft' || event.key === 'ArrowUp' ? -1 : 0;
    const target = event.key === 'Home' ? 0 : event.key === 'End' ? steps.length - 1 : delta ? Math.min(steps.length - 1, Math.max(0, selected + delta)) : null;
    if (target === null) return;
    event.preventDefault(); setSelected(target); stationRefs.current[target]?.focus();
  };
  useEffect(() => {
    // Solo desplaza la línea en horizontal (celular); nunca la página.
    const station = stationRefs.current[selected], rail = station?.closest('.afr2-infra-metro');
    if (!station || !(rail instanceof HTMLElement) || rail.scrollWidth <= rail.clientWidth) return;
    const offset = station.getBoundingClientRect().left - rail.getBoundingClientRect().left + rail.scrollLeft;
    const reduce = window.matchMedia('(prefers-reduced-motion: reduce)').matches;
    rail.scrollTo({ left: offset - rail.clientWidth / 2 + station.offsetWidth / 2, behavior: reduce ? 'auto' : 'smooth' });
  }, [selected, journey]);

  return <div className="afr2-route-view afr2-page-stack afr2-infra afr2-loader">
    <section className="afr2-infra-hero" aria-labelledby="loader-title">
      <div className="afr2-infra-hero__glow" aria-hidden="true" />
      <div className="afr2-infra-hero__copy">
        <p className="afr2-infra-hero__eyebrow"><span>Explorar</span> AFR2 Loader</p>
        <h1 id="loader-title">De la fuente a la placa,<br /><span>con cada byte verificado.</span></h1>
        <p className="afr2-infra-hero__lede">AFR2 Loader es la aplicación de escritorio para Windows que compila el firmware del ATmega2560, el del ESP8266 y la Web, y los carga por el puerto serie. Encadena avrdude, esptool y los scripts del proyecto con comprobaciones antes y después de escribir, para que ninguna carga deje una placa en un estado desconocido.</p>
        <ul className="afr2-infra-hero__chips" aria-label="En pocas palabras">
          <li><UiIcon name="programs" size={16} />C# · WinForms · .NET Framework 4</li>
          <li><UiIcon name="board" size={16} />avrdude + esptool</li>
          <li><UiIcon name="check" size={16} />Versión {LOADER_VERSION}</li>
        </ul>
      </div>

      <div className="afr2-infra-flow" role="group" aria-label="Mapa del Loader: el perfil y la compilación en la computadora; el Loader hace backup, escribe y verifica; el resultado queda en el ATmega2560 o en el ESP8266. Cada nodo abre su etapa.">
        <div className="afr2-infra-lane" data-zone="pc">
          <span className="afr2-infra-lane__label"><UiIcon name={ZONES.pc.icon} size={14} />{ZONES.pc.label}</span>
          <div className="afr2-infra-lane__body">
            <FlowNode zone="pc" icon="settings" title="Perfil JSON" detail="Rutas, puertos, imágenes" onPick={() => pick('mega', 0)} />
            <Wire delay={0} />
            <FlowNode zone="pc" icon="programs" title="Compilar" detail="make · PlatformIO · npm" onPick={() => pick('esp', 1)} />
          </div>
        </div>
        <Wire delay={.5} label="COM" />
        <div className="afr2-infra-lane" data-zone="link">
          <span className="afr2-infra-lane__label"><UiIcon name={ZONES.link.icon} size={14} />{ZONES.link.label}</span>
          <div className="afr2-infra-lane__body">
            <FlowNode zone="link" icon="lock" title="Backup" detail="Firma + doble lectura" onPick={() => pick('mega', 2)} />
            <Wire delay={1} />
            <FlowNode zone="link" icon="arrow-down" title="Escribir" detail="avrdude · esptool" onPick={() => pick('esp', 3)} />
          </div>
        </div>
        <Wire delay={1.5} label="verify" />
        <div className="afr2-infra-lane" data-zone="esp">
          <span className="afr2-infra-lane__label"><UiIcon name={ZONES.esp.icon} size={14} />{ZONES.esp.label}</span>
          <div className="afr2-infra-lane__body">
            <FlowNode zone="esp" icon="board" title="ATmega2560" detail="Lectura posterior" onPick={() => pick('mega', 4)} />
            <FlowNode zone="esp" icon="wifi" title="ESP8266" detail="Hash por imagen" onPick={() => pick('esp', 4)} />
          </div>
        </div>
      </div>

      <p className="afr2-infra-hero__takeaway"><UiIcon name="check" size={18} /><span><strong>Una carga solo cuenta cuando se verifica.</strong> Sin la comprobación de hash o la lectura posterior, el Loader no la da por hecha; si algo falla lo informa con la salida completa y no reintenta a ciegas.</span></p>
    </section>

    <Reveal className="afr2-infra-journey" aria-labelledby="loader-flow-title">
      <span ref={journeyRef} className="afr2-infra-anchor" aria-hidden="true" />
      <div className="afr2-infra-heading"><div><p className="afr2-kicker">Paso a paso</p><h2 id="loader-flow-title">Qué hace en cada carga</h2></div>
        <SegmentedControl items={JOURNEY_CHOICES} value={journey} onChange={chooseJourney} ariaLabel="Elegir placa a cargar" fill="never" />
      </div>
      <nav className="afr2-infra-metro" aria-label={journey === 'mega' ? 'Etapas de la carga del ATmega' : 'Etapas de la carga del ESP'}
        style={{ '--stations': steps.length, '--progress': steps.length > 1 ? selected / (steps.length - 1) : 0 } as CSSProperties}>
        <ol onKeyDown={onStationKey}>{steps.map((item, index) => <li key={`${journey}-${item.title}`} data-zone={item.zone} data-state={index < selected ? 'done' : index === selected ? 'current' : 'next'}>
          <button type="button" ref={node => { stationRefs.current[index] = node; }} tabIndex={index === selected ? 0 : -1}
            aria-current={index === selected ? 'step' : undefined} aria-controls="loader-step-detail" onClick={() => setSelected(index)}>
            <span className="afr2-infra-metro__dot">{index < selected ? <UiIcon name="check" size={14} /> : String(index + 1).padStart(2, '0')}</span>
            <span className="afr2-infra-metro__label"><strong>{item.title}</strong><small>{item.short}</small></span>
          </button>
        </li>)}</ol>
      </nav>

      <article key={`${journey}-${selected}`} id="loader-step-detail" className="afr2-infra-stage" data-zone={step.zone} aria-labelledby="loader-step-title" aria-live="polite">
        <div className="afr2-infra-stage__main">
          <div className="afr2-infra-stage__top"><span className="afr2-infra-zone-tag"><UiIcon name={ZONES[step.zone].icon} size={14} />{step.place}</span><span className="afr2-infra-counter">{String(selected + 1).padStart(2, '0')} / {String(steps.length).padStart(2, '0')}</span></div>
          <div className="afr2-infra-stage__title"><span className="afr2-infra-stage__icon"><UiIcon name={step.icon} size={28} /></span><h3 id="loader-step-title">{step.title}</h3></div>
          <p className="afr2-infra-stage__description">{step.description}</p>
          <div className="afr2-infra-transform">
            <div><span>Entra</span><strong>{step.input}</strong></div>
            <Wire delay={0} />
            <div><span>Sale</span><strong>{step.output}</strong></div>
          </div>
        </div>
        <div className="afr2-infra-stage__side">
          <h4>Lo que conviene saber</h4>
          <ol className="afr2-infra-facts">{step.details.map((detail, index) => <li key={detail} style={{ '--i': index } as CSSProperties}>{detail}</li>)}</ol>
          <p className="afr2-help afr2-loader-source"><UiIcon name="book" size={14} />Fuente: <code>{step.source}</code></p>
          <div className="afr2-infra-stage__nav">
            <Button size="sm" disabled={selected === 0} icon={<UiIcon name="back" />} onClick={() => setSelected(value => value - 1)}>Anterior</Button>
            <Button size="sm" variant={selected === steps.length - 1 ? 'secondary' : 'primary'} disabled={selected === steps.length - 1} onClick={() => setSelected(value => value + 1)}>Siguiente<UiIcon name="arrow" /></Button>
          </div>
        </div>
      </article>
    </Reveal>

    <Reveal className="afr2-infra-card" aria-labelledby="loader-numbers-title">
      <div className="afr2-infra-heading"><div><p className="afr2-kicker">En cifras</p><h2 id="loader-numbers-title">El Loader por dentro</h2></div>
        <Link className="afr2-infra-link" to="/ayuda?seccion=atmega">Ver la ayuda técnica<UiIcon name="arrow" size={16} /></Link></div>
      <div className="afr2-infra-stats">
        <Stat icon="check" label="Versión" value={LOADER_VERSION} detail="GUI y servidor MCP comparten versión y sesión" />
        <Stat icon="wifi" label="Transportes ESP" value="3" detail="usb-ttl · mega_reset · mega_bridge" />
        <Stat icon="programs" label="Herramientas MCP" value="10" detail="Estado, perfil, puertos, serie y operaciones" />
        <Stat icon="board" label="Operaciones" value="14" detail="Compilar, preparar, flashear, backup y verificar" />
      </div>
      <p className="afr2-help">Operaciones del Loader: build_mega, build_esp, build_web_fs, build_all, flash_mega, flash_esp, flash_fs, flash_esp_fs, flash_id, verify_flash, install_mega_flasher, verify_mega_flasher, flash_mega_backup y restore_mega_product. Por MCP se piden con loader_request_operation y se siguen con loader_job.</p>
    </Reveal>

    <Reveal className="afr2-infra-concepts" aria-labelledby="loader-why-title">
      <div className="afr2-infra-heading"><div><p className="afr2-kicker">Por qué existe</p><h2 id="loader-why-title">Qué resuelve frente a cargar a mano</h2></div></div>
      <div className="afr2-infra-glossary">{LOADER_REASONS.map((item, index) => <article key={item.title} className="afr2-infra-concept" style={{ '--i': index } as CSSProperties}>
        <span className="afr2-infra-concept__icon"><UiIcon name={item.icon} size={22} /></span>
        <h3>{item.title}</h3><p>{item.text}</p>
      </article>)}</div>
    </Reveal>

    <Reveal className="afr2-infra-concepts" aria-labelledby="loader-tools-title">
      <div className="afr2-infra-heading"><div><p className="afr2-kicker">Además de cargar</p><h2 id="loader-tools-title">Herramientas incluidas</h2></div></div>
      <div className="afr2-infra-glossary">{LOADER_TOOLS.map((item, index) => <article key={item.title} className="afr2-infra-concept" style={{ '--i': index } as CSSProperties}>
        <span className="afr2-infra-concept__icon"><UiIcon name={item.icon} size={22} /></span>
        <h3>{item.title}</h3><p>{item.text}</p>
      </article>)}</div>
      <p className="afr2-help">El Loader vive en AutoFinalRobo2-Loader, en el mismo repositorio que el resto del proyecto. Su configuración y sus registros se guardan fuera del repositorio. Esta página describe su fuente; no abre puertos ni realiza cargas.</p>
    </Reveal>
  </div>;
}
