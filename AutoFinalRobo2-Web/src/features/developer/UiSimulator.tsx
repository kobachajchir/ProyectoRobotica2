import { useEffect, useId, useMemo, useRef, useState } from 'react';
import { EncoderControl } from '../oled/EncoderControl';
import { OledCanvas128x64 } from '../oled/OledCanvas128x64';
import { Afr2OledBuffer } from '../oled/oledRasterizer';
import { renderFirmwareScreen } from '../oled/firmwareRenderer';
import { FIRMWARE_LIBRARY as L } from '../oled/generated/firmwareLibrary';
import { AFR2_SCREEN_META, getScreenMetadata } from '../oled/screenCatalog';
import { Button } from '../../components/ui/Button';
import { Disclosure } from '../../components/ui/Disclosure';
import { createUiSimulation, reduceUiSimulation, simulationItems, simulationScreenCode, simulationSnapshot, simulationUnavailable, type SimulationScenario } from './uiSimulation';

/** Shared live encoder gestures; callbacks only reach the in-memory reducer. */
export function UiSimulator() {
  const [state, setState] = useState(() => createUiSimulation());
  const [visible, setVisible] = useState(() => document.visibilityState === 'visible');
  const [session, setSession] = useState(0);
  const [lastInput, setLastInput] = useState('Sin entradas');
  const [query, setQuery] = useState('');
  const uid = useId(), optionsRef = useRef<HTMLOListElement>(null);
  const catalogue = useMemo(() => Object.values(AFR2_SCREEN_META).filter(screen =>
    `${screen.code} ${screen.name}`.toLocaleLowerCase('es').includes(query.trim().toLocaleLowerCase('es'))), [query]);
  useEffect(() => {
    const visibility = () => setVisible(document.visibilityState === 'visible');
    document.addEventListener('visibilitychange', visibility);
    return () => document.removeEventListener('visibilitychange', visibility);
  }, []);
  useEffect(() => {
    if (!visible) return;
    let last = performance.now();
    const timer = window.setInterval(() => {
      const now = performance.now(), ms = Math.floor(now - last); last += ms;
      setState(previous => reduceUiSimulation(previous, { type: 'tick', ms }));
    }, 40);
    return () => window.clearInterval(timer);
  }, [visible]);
  const buffer = useMemo(() => {
    const output = new Afr2OledBuffer();
    renderFirmwareScreen(state, output, simulationSnapshot(state));
    return output;
  }, [state]);
  const code = simulationScreenCode(state), metadata = getScreenMetadata(code);
  const items = simulationItems(state), selected = items[state.selection];
  const reason = selected ? simulationUnavailable(state, selected) : null;
  const sensorNodes: readonly number[] = [L.symbols.AFR2_N_RADAR_VIEW, L.symbols.AFR2_N_US_VIEW, L.symbols.AFR2_N_LINE_VIEW, L.symbols.AFR2_N_IR_VALUES, L.symbols.AFR2_N_SERVO_ANGLE];
  const special = sensorNodes.includes(state.node);
  const path = [...state.history.map(position => String(L.nodes[position.node][0])), String(L.nodes[state.node][0])];
  const guidance = state.notice ? 'SW corto cierra este aviso. Tambien se cierra a los 3 segundos; SW largo abre el menu principal.'
    : state.keyboard ? 'Gira para elegir un caracter. SW corto escribe; < borra; OK termina el campo. SW largo descarta y abre el menu.'
    : state.edit ? `Borrador: ${state.draft}. Rango ${state.editing?.min} a ${state.editing?.max}, paso ${state.editing?.step}. SW corto aplica; largo descarta y abre el menu.`
    : special ? state.node === L.symbols.AFR2_N_SERVO_ANGLE ? 'Gira para cambiar el angulo simulado. SW corto vuelve al listado.' : 'Gira para alternar las dos vistas. SW corto vuelve al listado conservando la seleccion.'
    : reason ? `La opcion esta bloqueada en este escenario: ${reason}. SW corto muestra el aviso del simulador.`
    : selected?.step ? 'SW corto abre el editor. Despues gira para cambiar el valor y confirma con SW corto.'
    : selected?.action ? 'SW corto ejecuta la accion sobre el equipo simulado y muestra su respuesta local.'
    : selected?.label.startsWith('Volver') ? 'SW corto vuelve al contexto anterior y recupera su seleccion y desplazamiento.'
    : selected ? 'SW corto abre la opcion seleccionada. Gira para cambiar de opcion.' : 'Pantalla informativa. Mantene SW para abrir el menu principal.';
  const title = state.notice ? state.notice.title : state.keyboard ? state.keyboardField ? 'Clave Wi-Fi' : 'Nombre de red Wi-Fi'
    : state.edit ? `Editar ${state.editingLabel}` : metadata?.name ?? String(L.nodes[state.node][0]);
  const mode = state.notice ? 'Aviso' : state.keyboard ? 'Teclado' : state.edit ? 'Edición' : 'Navegación';
  const selectionText = state.notice ? state.notice.text : state.keyboard ? state.key < 95 ? `Caracter: ${JSON.stringify(String.fromCharCode(32 + state.key))}`
    : state.key === 95 ? 'Borrar <' : 'Terminar OK' : state.edit ? `Valor: ${state.draft}` : special ? 'Vista de sensores' : selected?.label ?? 'Vista informativa';
  const showOptions = !state.notice && !state.keyboard && !state.edit && !special && items.length > 0;
  useEffect(() => {
    const list = optionsRef.current, item = list?.querySelector<HTMLElement>('[data-selected="true"]');
    if (!list || !item) return;
    if (item.offsetTop < list.scrollTop) list.scrollTop = item.offsetTop;
    else if (item.offsetTop + item.offsetHeight > list.scrollTop + list.clientHeight)
      list.scrollTop = item.offsetTop + item.offsetHeight - list.clientHeight;
  }, [state.node, state.selection, state.notice, state.keyboard, state.edit]);
  const reset = (scenario: SimulationScenario) => {
    setState(createUiSimulation(scenario)); setSession(previous => previous + 1); setLastInput('Sin entradas');
  };
  return <section className="afr2-ui-simulator" aria-labelledby={`${uid}-title`}>
    <header className="afr2-ui-simulator-heading">
      <div><p className="afr2-kicker">Laboratorio de interfaz</p><h2 id={`${uid}-title`}>Simulador UI</h2><p>Explorá las pantallas del robot con el encoder virtual.</p></div>
      <span className="afr2-ui-simulator-badge">Equipo ficticio · sesión local</span>
    </header>
    <div className="afr2-ui-simulator-toolbar">
      <label htmlFor={`${uid}-scenario`}>Equipo de ejemplo
        <select id={`${uid}-scenario`} value={state.scenario} onChange={event => reset(event.target.value as SimulationScenario)}>
          <option value="ready">Preparado · sensores y calibración</option>
          <option value="missing">Pendiente · sin lecturas ni calibración</option>
        </select>
      </label>
      <p>Cambiar de equipo reinicia sus valores. Las ediciones duran durante esta sesión.</p>
      <Button onClick={() => reset(state.scenario)}>Reiniciar simulación</Button>
    </div>
    <div className="afr2-ui-simulator-workspace">
      <div className="afr2-ui-simulator-device">
        <div className="afr2-ui-simulator-device-heading"><span>Pantalla del robot</span><span>OLED · 128 × 64</span></div>
        <div className="afr2-ui-simulator-display"><OledCanvas128x64 compact oledBuffer={buffer} screenCode={code} sourceMode="local-ui-simulation" showGrid={false} /></div>
        <div className="afr2-ui-simulator-encoder">
        <EncoderControl key={session} label="Encoder · simulador UI" available={visible}
          disabledReason={!visible ? 'Simulación pausada mientras la pestaña está oculta.' : undefined}
          onRotate={delta => { setLastInput(delta < 0 ? 'Giro izquierdo' : 'Giro derecho'); setState(previous => reduceUiSimulation(previous, { type: 'rotate', delta })); }}
          onPress={kind => { setLastInput(kind === 'long' ? 'SW largo · Menu' : 'SW corto · Confirmar'); setState(previous => reduceUiSimulation(previous, { type: 'press', kind })); }}
          lastEventLabel={lastInput} statusMessage={state.feedback} compact />
        </div>
        <dl className="afr2-ui-simulator-gestures">
          <div><dt>Girar</dt><dd>Cambiar opción o valor</dd></div>
          <div><dt>SW corto</dt><dd>Entrar o confirmar</dd></div>
          <div><dt>Mantener 0,8 s</dt><dd>Abrir el menú principal</dd></div>
        </dl>
      </div>
      <aside className="afr2-ui-simulator-guide" aria-label="Guia de navegacion simulada">
        <header className="afr2-ui-simulator-guide-heading"><p className="afr2-kicker">Pantalla actual</p><span>{mode}</span></header>
        <h3>{title}</h3>
        <p className="afr2-ui-simulator-path" aria-label="Ruta de navegación">{path.join(' / ')}</p>
        <div className="afr2-ui-simulator-guidance" aria-live="polite" aria-atomic="true">
          <span className="afr2-ui-simulator-caption">{state.notice ? 'Mensaje' : state.edit ? 'Valor en edición' : 'Selección actual'}</span>
          <strong>{selectionText}</strong>
          <p>{guidance}</p>
        </div>
        {showOptions && <div className="afr2-ui-simulator-menu-heading"><span>Opciones de la pantalla</span><span>{state.selection + 1} / {items.length}</span></div>}
        {showOptions && <ol ref={optionsRef} className="afr2-ui-simulator-options" aria-label="Opciones de esta pantalla">
          {items.map((item, index) => {
            const blocked = simulationUnavailable(state, item);
            return <li key={index} data-selected={index === state.selection} aria-current={index === state.selection ? 'step' : undefined}>
              <span className="afr2-ui-simulator-option-number" aria-hidden="true">{index + 1}</span>
              <div><span>{item.label}</span>{blocked && <small>Requiere: {blocked}</small>}</div>
              {index === state.selection && <span className="afr2-ui-simulator-option-marker" aria-hidden="true">›</span>}
            </li>;
          })}
        </ol>}
        <p className="afr2-ui-simulator-return">Para regresar, elegí <strong>Volver</strong> con el encoder. En las vistas de sensores, SW corto vuelve al listado.</p>
        <div className="afr2-ui-simulator-screen-code"><span>Identidad de pantalla</span><code>{code}</code></div>
      </aside>
    </div>
    <div className="afr2-ui-simulator-settings"><Disclosure title="Catálogo de pantallas y alcance" meta="Inspección">
      <div className="afr2-ui-simulator-settings-grid">
        <div className="afr2-ui-simulator-catalogue">
        <label htmlFor={`${uid}-search`}>Buscar pantalla
          <input id={`${uid}-search`} type="search" value={query} onChange={event => setQuery(event.target.value)} placeholder="Nombre o código" autoComplete="off" />
        </label>
        <label htmlFor={`${uid}-inspect`}>Abrir una pantalla o aviso
          <select id={`${uid}-inspect`} value="" onChange={event => { const code = event.target.value; if (code) { setSession(previous => previous + 1); setState(previous => reduceUiSimulation(previous, { type: 'inspect', code })); } }}>
            <option value="">Elegir del catalogo del ATmega…</option>
            {catalogue.map(screen => <option key={screen.code} value={screen.code}>{screen.code} · {screen.name}</option>)}
          </select>
        </label>
        <p className="afr2-help" role="status">{catalogue.length ? `${catalogue.length} pantallas disponibles. Abrir una pantalla salta la navegación; reiniciá para volver a Inicio.` : 'No hay pantallas con ese nombre o código.'}</p>
        </div>
        <div className="afr2-ui-simulator-scope"><h3>Un entorno para explorar</h3>
          <p>Comparte catálogo, fuentes, iconos y renderers OLED con el proyecto. Sensores, red, motores y EEPROM usan respuestas de ejemplo en memoria.</p>
          <p>Las ediciones se descartan al salir. Usá nombres y claves Wi-Fi de ejemplo.</p>
          <p>Adapta la navegación del ATmega; no ejecuta su binario ni reproduce los tiempos, protocolos o física del hardware.</p>
        </div>
      </div>
    </Disclosure></div>
  </section>;
}
