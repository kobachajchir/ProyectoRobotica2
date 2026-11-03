import { useEffect, useRef, useState, type CSSProperties, type KeyboardEvent } from 'react';
import { Link } from 'react-router-dom';
import { UiIcon, type UiIconName } from '../../components/ui/UiIcon';
import { Button } from '../../components/ui/Button';
import { SegmentedControl, type SegmentedItem } from '../../components/ui/SegmentedControl';
import { MODEL_ASSETS } from '../../components/three/modelAssets';
import modelSizes from '../../components/three/modelAssetSizes.json';
import { JOURNEYS, type Journey } from './infrastructureFlow';
import { useInfrastructurePackage } from './useInfrastructurePackage';
import { useConnection } from '../connection/ConnectionContext';
import { FlowNode, Reveal, Stat, Wire } from './infraPrimitives';

/* AFR2-20261102-07: reforma visual de AFR2-20261102-04. Mismo contenido y orden; portada con mapa
   de flujo animado, recorrido en «línea de metro», barras de peso y anillo de ocupación. */

const JOURNEY_CHOICES: readonly SegmentedItem<Journey>[] = [
  { value: 'web', label: 'Viaje de la Web', icon: 'programs' },
  { value: 'model', label: 'Viaje del modelo 3D', icon: 'cube' },
];
type Zone = 'pc' | 'esp' | 'link' | 'client';
const ZONES: Record<Zone, { label: string; icon: UiIconName }> = {
  pc: { label: 'En la computadora', icon: 'settings' },
  esp: { label: 'En el ESP8266', icon: 'board' },
  link: { label: 'Por Wi-Fi', icon: 'wifi' },
  client: { label: 'En tu dispositivo', icon: 'screen' },
};
const zoneOf = (place: string): Zone => place.startsWith('Del ESP') ? 'link' : place.includes('ESP') ? 'esp' : place.includes('celular') ? 'client' : 'pc';
const formatBytes = (value: number | undefined) => value === undefined ? 'Sin dato' : value < 1024
  ? `${value.toLocaleString('es-AR')} B`
  : `${(value / (value < 1048576 ? 1024 : 1048576)).toLocaleString('es-AR', { maximumFractionDigits: 2 })} ${value < 1048576 ? 'KiB' : 'MiB'}`;
const exactBytes = (value: number | undefined) => value === undefined ? 'Sin dato' : `${value.toLocaleString('es-AR')} bytes`;
const date = (value: string) => Number.isFinite(Date.parse(value)) ? new Date(value).toLocaleString('es-AR') : 'Fecha no informada';
/** Ocupación que informa el ESP en system.info / system.hello (Storage::health). */
interface BoardFilesystem { usedBytes?: number; totalBytes?: number; mounted?: boolean; webReady?: boolean; fault?: string }
const safeBytes = (value: unknown): value is number => typeof value === 'number' && Number.isSafeInteger(value) && value >= 0;
function readBoardFilesystem(value: unknown): BoardFilesystem | null {
  if (!value || typeof value !== 'object' || Array.isArray(value)) return null;
  const fs = value as Record<string, unknown>;
  return {
    ...(safeBytes(fs.usedBytes) ? { usedBytes: fs.usedBytes } : {}),
    ...(safeBytes(fs.totalBytes) ? { totalBytes: fs.totalBytes } : {}),
    ...(typeof fs.mounted === 'boolean' ? { mounted: fs.mounted } : {}),
    ...(typeof fs.webReady === 'boolean' ? { webReady: fs.webReady } : {}),
    ...(typeof fs.fault === 'string' ? { fault: fs.fault } : {}),
  };
}
const sizeFor = (id: keyof typeof MODEL_ASSETS) => (modelSizes as Record<string, number>)[MODEL_ASSETS[id].url];
function changeLabel(raw: number, gzip: number) {
  if (!raw) return 'Sin comparación';
  const percent = Math.abs((1 - gzip / raw) * 100).toLocaleString('es-AR', { maximumFractionDigits: 1 });
  return `${percent}% ${gzip <= raw ? 'menos' : 'más'}`;
}

const CONCEPTS: readonly { icon: UiIconName; title: string; text: string }[] = [
  { icon: 'cube', title: 'Minimización, gzip y Draco', text: 'La minimización reduce el código JavaScript y CSS. gzip comprime los bytes de cada recurso para guardarlo y transmitirlo. Draco comprime geometría dentro de un GLB. El navegador retira gzip antes de que el cargador 3D decodifique Draco.' },
  { icon: 'board', title: 'Flash, LittleFS y RAM', text: 'Flash es la memoria persistente del ESP. LittleFS organiza parte de esa memoria en carpetas y archivos. La RAM del ESP permite ejecutar el servidor. El navegador usa su propia RAM y memoria gráfica para React y las mallas reconstruidas.' },
  { icon: 'programs', title: 'Paquete, imagen y firmware', text: 'El paquete es el conjunto de archivos .gz y su manifiesto. La imagen littlefs.bin contiene un filesystem preparado para la flash. El firmware es otro binario: el programa que arranca el ESP, monta LittleFS y atiende HTTP y WebSocket. Cambiar las fuentes no actualiza automáticamente ninguno de esos artefactos.' },
  { icon: 'screen', title: '¿Qué se hace en cada equipo?', text: 'Fusion, Blender, Vite y el empaquetador trabajan en la computadora de desarrollo. El ESP guarda y sirve archivos, y enlaza la Web con el ATmega. El ATmega controla el robot. Tu navegador ejecuta React, la simulación y el renderizado 3D.' },
];

/** AFR2-20261102-04. Recorrido educativo basado en las fuentes del proyecto, sin cargar escenas 3D. */
export function InfrastructurePage() {
  const [journey, setJourney] = useState<Journey>('web');
  const [selected, setSelected] = useState(0);
  const journeyRef = useRef<HTMLElement>(null);
  const stationRefs = useRef<(HTMLButtonElement | null)[]>([]);
  const { report, loading, error, refresh } = useInfrastructurePackage();
  const { wsState, systemInfo, systemInfoUpdatedAtMs, refreshSystemInfo } = useConnection();
  const [boardQuery, setBoardQuery] = useState<'idle' | 'busy' | 'failed'>('idle');
  const boardReady = wsState === 'ready';
  const boardFs = readBoardFilesystem(systemInfo?.filesystem);
  const askBoard = async () => {
    setBoardQuery('busy');
    try { await refreshSystemInfo(); setBoardQuery('idle'); } catch { setBoardQuery('failed'); }
  };
  const steps = JOURNEYS[journey];
  const step = steps[selected];
  const zone = zoneOf(step.place);
  const auto = sizeFor('auto'), arm = sizeFor('brazo'), cube = sizeFor('cubo');
  // La simulación conjunta usa auto, brazo y el cubo auxiliar (un solo GLB reutilizado por cada cubo).
  const catalogTotal = [auto, arm, cube].every(v => typeof v === 'number') ? (auto ?? 0) + (arm ?? 0) + (cube ?? 0) : undefined;
  const weightMax = Math.max(1, catalogTotal ?? 0, auto ?? 0, arm ?? 0, cube ?? 0);
  const manifest = report?.manifest;
  // Primero lo que devuelve /api/health cuando la Web se sirve desde el ESP; si no, la respuesta de la placa por WebSocket.
  const httpFs = report?.filesystem && (report.filesystem.usedBytes !== undefined || report.filesystem.mounted !== undefined) ? report.filesystem : null;
  const fs = httpFs ?? boardFs;
  const fsOrigin = httpFs ? 'http' : boardFs ? 'board' : null;
  const localPackage = report?.source === 'local-prepared';
  const used = fs?.usedBytes, capacity = fs?.totalBytes;
  const occupancy = used !== undefined && capacity !== undefined && capacity > 0 ? Math.min(100, used / capacity * 100) : null;
  const gzipRatio = manifest && manifest.totalRawBytes > 0 ? Math.min(1, manifest.totalGzipBytes / manifest.totalRawBytes) : null;
  const chooseJourney = (next: Journey) => { setJourney(next); setSelected(0); };
  const pick = (next: Journey, index: number) => {
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

  return <div className="afr2-route-view afr2-page-stack afr2-infra">
    <section className="afr2-infra-hero" aria-labelledby="infra-title">
      <div className="afr2-infra-hero__glow" aria-hidden="true" />
      <div className="afr2-infra-hero__copy">
        <p className="afr2-infra-hero__eyebrow"><span>Explorar</span> Infraestructura</p>
        <h1 id="infra-title">Del diseño al robot,<br /><span>del robot a tu pantalla.</span></h1>
        <p className="afr2-infra-hero__lede">Descubrí cómo viajan el código y los modelos 3D que hacen posible esta aplicación. La computadora prepara los archivos, la flash del ESP los conserva y tu navegador ejecuta la aplicación y dibuja el 3D.</p>
        <ul className="afr2-infra-hero__chips" aria-label="En pocas palabras">
          <li><UiIcon name="cube" size={16} />3 modelos GLB · {formatBytes(catalogTotal)}</li>
          <li><UiIcon name="board" size={16} />Una aplicación dentro del robot</li>
          <li><UiIcon name="check" size={16} />Sin depender de un CDN</li>
        </ul>
      </div>

      <div className="afr2-infra-flow" role="group" aria-label="Mapa del recorrido: React y el diseño Fusion/Blender producen recursos que se comprimen con gzip, se guardan en una imagen LittleFS, se cargan en la flash del ESP y viajan por HTTP al navegador. Cada nodo abre su etapa.">
        <div className="afr2-infra-lane" data-zone="pc">
          <span className="afr2-infra-lane__label"><UiIcon name={ZONES.pc.icon} size={14} />{ZONES.pc.label}</span>
          <div className="afr2-infra-lane__body">
            <div className="afr2-infra-flow__origins">
              <FlowNode zone="pc" icon="programs" title="React + Vite" detail="Interfaz y lógica" onPick={() => pick('web', 0)} />
              <FlowNode zone="pc" icon="cube" title="Fusion → Blender" detail="GLB con Draco" onPick={() => pick('model', 0)} />
            </div>
            <Wire delay={0} />
            <FlowNode zone="pc" icon="programs" title="Paquete gzip" detail="Archivos + manifiesto" onPick={() => pick('web', 2)} />
          </div>
        </div>
        <Wire delay={.5} label=".bin" />
        <div className="afr2-infra-lane" data-zone="esp">
          <span className="afr2-infra-lane__label"><UiIcon name={ZONES.esp.icon} size={14} />{ZONES.esp.label}</span>
          <div className="afr2-infra-lane__body">
            <FlowNode zone="esp" icon="board" title="LittleFS + flash" detail="Imagen .bin en el ESP" onPick={() => pick('web', 4)} />
            <Wire delay={1} />
            <FlowNode zone="esp" icon="wifi" title="Servidor HTTP" detail="Transmite por Wi-Fi" onPick={() => pick('web', 5)} />
          </div>
        </div>
        <Wire delay={1.5} label="gzip" />
        <div className="afr2-infra-lane" data-zone="client">
          <span className="afr2-infra-lane__label"><UiIcon name={ZONES.client.icon} size={14} />{ZONES.client.label}</span>
          <div className="afr2-infra-lane__body">
            <FlowNode zone="client" icon="screen" title="Tu navegador" detail="gzip → Draco → escena" onPick={() => pick('web', 6)} />
          </div>
        </div>
      </div>

      <p className="afr2-infra-hero__takeaway"><UiIcon name="screen" size={18} /><span><strong>El navegador hace el trabajo gráfico.</strong> El ESP transmite archivos comprimidos; el celular o la computadora los abre, ejecuta React y reconstruye las mallas 3D.</span></p>
    </section>

    <Reveal className="afr2-infra-journey" aria-labelledby="infra-flow-title">
      <span ref={journeyRef as never} className="afr2-infra-anchor" aria-hidden="true" />
      <div className="afr2-infra-heading"><div><p className="afr2-kicker">Paso a paso</p><h2 id="infra-flow-title">Seguí el viaje de los datos</h2></div>
        <SegmentedControl items={JOURNEY_CHOICES} value={journey} onChange={chooseJourney} ariaLabel="Elegir recorrido de infraestructura" fill="never" />
      </div>
      <nav className="afr2-infra-metro" aria-label={journey === 'web' ? 'Etapas de la Web' : 'Etapas del modelo 3D'}
        style={{ '--stations': steps.length, '--progress': steps.length > 1 ? selected / (steps.length - 1) : 0 } as CSSProperties}>
        <ol onKeyDown={onStationKey}>{steps.map((item, index) => <li key={`${journey}-${item.title}`} data-zone={zoneOf(item.place)} data-state={index < selected ? 'done' : index === selected ? 'current' : 'next'}>
          <button type="button" ref={node => { stationRefs.current[index] = node; }} tabIndex={index === selected ? 0 : -1}
            aria-current={index === selected ? 'step' : undefined} aria-controls="infra-step-detail" onClick={() => setSelected(index)}>
            <span className="afr2-infra-metro__dot">{index < selected ? <UiIcon name="check" size={14} /> : String(index + 1).padStart(2, '0')}</span>
            <span className="afr2-infra-metro__label"><strong>{item.title}</strong><small>{item.short}</small></span>
          </button>
        </li>)}</ol>
      </nav>

      <article key={`${journey}-${selected}`} id="infra-step-detail" className="afr2-infra-stage" data-zone={zone} aria-labelledby="infra-step-title" aria-live="polite">
        <div className="afr2-infra-stage__main">
          <div className="afr2-infra-stage__top"><span className="afr2-infra-zone-tag"><UiIcon name={ZONES[zone].icon} size={14} />{step.place}</span><span className="afr2-infra-counter">{String(selected + 1).padStart(2, '0')} / {String(steps.length).padStart(2, '0')}</span></div>
          <div className="afr2-infra-stage__title"><span className="afr2-infra-stage__icon"><UiIcon name={step.icon} size={28} /></span><h3 id="infra-step-title">{step.title}</h3></div>
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
          <div className="afr2-infra-stage__nav">
            <Button size="sm" disabled={selected === 0} icon={<UiIcon name="back" />} onClick={() => setSelected(value => value - 1)}>Anterior</Button>
            <Button size="sm" variant={selected === steps.length - 1 ? 'secondary' : 'primary'} disabled={selected === steps.length - 1} onClick={() => setSelected(value => value + 1)}>Siguiente<UiIcon name="arrow" /></Button>
          </div>
        </div>
      </article>
    </Reveal>

    <Reveal className="afr2-infra-card afr2-infra-weights" aria-labelledby="infra-models-title">
      <div className="afr2-infra-heading"><div><p className="afr2-kicker">Catálogo de esta Web</p><h2 id="infra-models-title">Cuánto pesa el 3D</h2></div><Link className="afr2-infra-link" to="/modelos">Explorar modelos<UiIcon name="arrow" size={16} /></Link></div>
      <ul className="afr2-infra-bars">
        {(['auto', 'brazo', 'cubo'] as const).map((id, index) => <li key={id} style={{ '--i': index } as CSSProperties}>
          <span className="afr2-infra-bars__name">{MODEL_ASSETS[id].label}</span>
          <span className="afr2-infra-bars__track" aria-hidden="true"><span className="afr2-infra-bars__fill" data-model={id} style={{ '--w': (sizeFor(id) ?? 0) / weightMax } as CSSProperties} /></span>
          <strong title={exactBytes(sizeFor(id))}>{formatBytes(sizeFor(id))}</strong>
        </li>)}
        <li data-kind="sum" style={{ '--i': 3 } as CSSProperties}>
          <span className="afr2-infra-bars__name">Simulación conjunta: auto + brazo + cubo<small>Tres GLB compartidos; el cubo se reutiliza para cada pieza</small></span>
          <span className="afr2-infra-bars__track" aria-hidden="true">
            <span className="afr2-infra-bars__fill" data-model="auto" style={{ '--w': (auto ?? 0) / weightMax } as CSSProperties} />
            <span className="afr2-infra-bars__fill" data-model="brazo" style={{ '--w': (arm ?? 0) / weightMax, '--x': (auto ?? 0) / weightMax } as CSSProperties} />
            <span className="afr2-infra-bars__fill" data-model="cubo" style={{ '--w': (cube ?? 0) / weightMax, '--x': ((auto ?? 0) + (arm ?? 0)) / weightMax } as CSSProperties} />
          </span>
          <strong title={exactBytes(catalogTotal)}>{formatBytes(catalogTotal)}</strong>
        </li>
      </ul>
      <p className="afr2-help">Tamaños GLB del catálogo de esta versión, con la geometría ya comprimida con Draco y antes de gzip. No representan RAM ni memoria gráfica. La simulación conjunta suma los tres archivos: auto, brazo y cubo auxiliar.</p>
    </Reveal>

    <Reveal className="afr2-infra-card afr2-infra-package" aria-labelledby="infra-package-title" aria-busy={loading}>
      <div className="afr2-infra-heading"><div><p className="afr2-kicker">Archivos, compresión y almacenamiento</p><h2 id="infra-package-title">El paquete que se sirve</h2></div>
        <Button size="sm" loading={loading} icon={<UiIcon name="refresh" />} onClick={refresh}>Actualizar datos</Button></div>
      {loading && <p role="status" className="afr2-help">Consultando el manifiesto del servidor…</p>}
      {error && <div role="status" className="afr2-infra-empty"><span className="afr2-infra-empty__icon"><UiIcon name="book" size={24} /></span><div><strong>El recorrido está disponible sin un reporte de archivos.</strong><p>{error} Los tamaños del paquete aparecerán al abrir esta Web desde un servidor compatible.</p><p>En una placa con firmware anterior, esta consulta requiere la versión del ESP que incluye el reporte de infraestructura.</p></div></div>}
      {manifest && report && <>
        <div className="afr2-infra-package-origin"><span className="afr2-infra-zone-tag" data-zone={localPackage ? 'pc' : 'esp'}><UiIcon name={localPackage ? 'settings' : 'board'} size={14} />{localPackage ? 'Paquete local preparado · no acredita una carga' : 'Manifiesto leído del ESP'}</span>
          <p className="afr2-help">Web {manifest.webVersion} · perfil {manifest.buildProfile} · empaquetado {date(manifest.packagedAt)}</p>
          {manifest.modelAssetsUpdatedAt && <p className="afr2-help">Modelos actualizados por separado: {date(manifest.modelAssetsUpdatedAt)}. {manifest.modelAssetsOnlyUpdate}</p>}
          {localPackage && <p className="afr2-help">Referencia de ESP/data/dist. Puede corresponder a una versión anterior a las fuentes que estás viendo; no mide los archivos instalados en una placa.</p>}
        </div>
      </>}
      <div className="afr2-infra-package-grid">
        {manifest && <>
            <div className="afr2-infra-compress">
              <span className="afr2-infra-stat__label">Compresión gzip del paquete</span>
              <div className="afr2-infra-compress__bar" data-kind="raw"><span style={{ '--w': 1 } as CSSProperties} /><em>Antes · {formatBytes(manifest.totalRawBytes)}</em></div>
              <div className="afr2-infra-compress__bar" data-kind="gzip"><span style={{ '--w': gzipRatio ?? 0 } as CSSProperties} /><em>Guardado · {formatBytes(manifest.totalGzipBytes)}</em></div>
              <strong className="afr2-infra-compress__delta">{changeLabel(manifest.totalRawBytes, manifest.totalGzipBytes)}</strong>
              <small>{manifest.files.length} archivos · suma declarada de recursos Web</small>
            </div>
        </>}
        <div className="afr2-infra-storage" data-state={occupancy !== null ? 'ready' : 'empty'}>
          {occupancy !== null
            ? <div className="afr2-infra-ring" style={{ '--p': occupancy } as CSSProperties} aria-hidden="true"><span>{occupancy.toLocaleString('es-AR', { maximumFractionDigits: 1 })}%</span></div>
            : <div className="afr2-infra-ring" data-empty="true" style={{ '--p': 0 } as CSSProperties} aria-hidden="true"><UiIcon name="board" size={28} /></div>}
          <div>
            <strong>Flash dedicada a LittleFS</strong>
            {occupancy !== null ? <>
              <span>{formatBytes(used)} de {formatBytes(capacity)}</span>
              <progress className="sr-only" value={used} max={capacity} aria-label="Ocupación de LittleFS reportada por el ESP" />
              <p className="afr2-help">{fsOrigin === 'http' ? 'Leída del ESP al servir esta Web.' : `Respuesta de la placa a system.info${systemInfoUpdatedAtMs ? ` · ${new Date(systemInfoUpdatedAtMs).toLocaleTimeString('es-AR')}` : ''}.`} Incluye la estructura del filesystem y otros archivos guardados, además de los recursos Web.</p>
            </> : <p className="afr2-help">{boardReady
              ? 'La placa está conectada: pedile la ocupación con el comando system.info.'
              : <>Conectá la Web al robot y la ocupación se le pide a la placa con el comando system.info. <Link to="/conexion">Ir a Conexión</Link></>}</p>}
            {boardQuery === 'failed' && <p className="afr2-help" role="status">La placa no respondió. Probá de nuevo.</p>}
            {boardReady && fsOrigin !== 'http' && <Button size="sm" loading={boardQuery === 'busy'} icon={<UiIcon name="refresh" />} onClick={() => { void askBoard(); }}>{occupancy !== null ? 'Volver a consultar' : 'Consultar a la placa'}</Button>}
          </div>
        </div>
      </div>
      {manifest && report && <>
        <div className="afr2-infra-stats">
          <Stat icon="programs" label="Antes de gzip" value={formatBytes(manifest.totalRawBytes)} bytes={manifest.totalRawBytes} detail="Suma declarada de recursos Web" />
          <Stat icon="check" label="Archivos gzip" value={formatBytes(manifest.totalGzipBytes)} bytes={manifest.totalGzipBytes} detail={`${changeLabel(manifest.totalRawBytes, manifest.totalGzipBytes)} · ${manifest.files.length} archivos`} />
          <Stat icon="board" label="Ocupación LittleFS" value={formatBytes(used)} bytes={used} detail={capacity === undefined ? boardReady ? 'Pedila a la placa con system.info' : 'Conectá la placa para consultarla' : `Capacidad reportada: ${formatBytes(capacity)}`} />
          <Stat icon="cube" label="Estimación del paquete en FS" value={formatBytes(manifest.estimatedFilesystemBytes)} bytes={manifest.estimatedFilesystemBytes} detail="Bloques y reserva; no tamaño del .bin" />
        </div>
        {fs && <p className="afr2-help">LittleFS: {fs.mounted === undefined ? 'sin estado' : fs.mounted ? 'montado' : 'no montado'} · paquete: {fs.webReady === undefined ? 'sin estado' : fs.webReady ? 'disponible' : 'no disponible'}{fs.fault && fs.fault !== 'none' ? ` · diagnóstico: ${fs.fault}` : ''}.</p>}
        <div className="afr2-infra-file-table" tabIndex={0} role="region" aria-label="Tamaños declarados por archivo, tabla desplazable">
          <table><caption>Archivos del manifiesto {localPackage ? 'local preparado' : 'del ESP'} · tamaños antes y después de gzip</caption>
            <thead><tr><th scope="col">Archivo solicitado</th><th scope="col">Antes de gzip</th><th scope="col">Guardado .gz</th><th scope="col">Cambio de tamaño</th></tr></thead>
            <tbody>{manifest.files.map(file => <tr key={file.route}>
              <th scope="row"><code>{file.route}</code></th><td title={exactBytes(file.rawBytes)}>{formatBytes(file.rawBytes)}</td><td title={exactBytes(file.gzipBytes)}>{formatBytes(file.gzipBytes)}</td>
              <td><div className="afr2-infra-file-bars" aria-hidden="true"><span style={{ width: `${file.rawBytes / Math.max(1, file.rawBytes, file.gzipBytes) * 100}%` }} /><span style={{ width: `${file.gzipBytes / Math.max(1, file.rawBytes, file.gzipBytes) * 100}%` }} /></div><small>{changeLabel(file.rawBytes, file.gzipBytes)}</small></td>
            </tr>)}</tbody>
          </table>
        </div>
        <p className="afr2-help">Barras por fila: arriba, antes de gzip; abajo, guardado .gz. 1 KiB = 1.024 bytes; 1 MiB = 1.048.576 bytes. gzip puede aumentar algunos archivos pequeños o ya comprimidos.</p>
        <p className="afr2-help">Consulta: {date(report.observedAt)}. Son tamaños declarados por el manifiesto; no se recalculan hashes ni se mide el tráfico HTTP. La imagen littlefs.bin incluye la estructura de la partición y tiene un tamaño diferente.</p>
      </>}
    </Reveal>

    <Reveal className="afr2-infra-concepts" aria-labelledby="infra-concepts-title">
      <div className="afr2-infra-heading"><div><p className="afr2-kicker">Glosario</p><h2 id="infra-concepts-title">Cuatro conceptos para entender el recorrido</h2></div></div>
      <div className="afr2-infra-glossary">{CONCEPTS.map((concept, index) => <article key={concept.title} className="afr2-infra-concept" style={{ '--i': index } as CSSProperties}>
        <span className="afr2-infra-concept__icon"><UiIcon name={concept.icon} size={22} /></span>
        <h3>{concept.title}</h3><p>{concept.text}</p>
      </article>)}</div>
    </Reveal>
  </div>;
}
