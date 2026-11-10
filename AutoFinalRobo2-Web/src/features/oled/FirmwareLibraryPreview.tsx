import { MOTOR_IDS, stepMotor } from '../../domain/motorSelection';
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
import { useEffect, useMemo, useRef, useState } from 'react';
import { FIRMWARE_LIBRARY as library } from './generated/firmwareLibrary';
import { firmwareMenuItem, firmwareNodeCount, firmwareNode, renderFirmwareScreen, MOTOR_SELECTOR_NODE, SPLASH_MS, type FirmwareSnapshot } from './firmwareRenderer';
import { Afr2OledBuffer } from './oledRasterizer';
import { OledCanvas128x64 } from './OledCanvas128x64';
import { OledWorkbench } from './OledWorkbench';
import { normalizeScreenCode, type Afr2ScreenCode, type Afr2ScreenMetadata } from './screenCatalog';

// AFR2-20261024-14 extends09: same pure AVR layout, static navigation only.
// Never seed sensor/network/sequence values, invoke actions or display credentials.
const layout: FirmwareSnapshot = {
  lines: ['', '', ''], labels: ['', '', ''], disabled: [0, 0, 0],
  sequenceState: 0, sequenceCount: 0, sequenceIndex: 0, sequenceKind: 0, sequenceDurationMs: 0, sequenceElapsedMs: 0,
  networkValid: false, networkSta: false, networkAp: false, networkSsid: '', networkApSsid: '', networkPassword: '', networkIp: '', networkApIp: '',
};
interface Position { node: number; selection: number; firstVisible: number }
const clock = () => Math.floor(performance.now()) >>> 0;
export function FirmwareLibraryPreview({ screen, onNavigate, active = true }: { screen: Afr2ScreenMetadata; onNavigate: (code: Afr2ScreenCode) => void; active?: boolean }) {
  const foundNode = library.nodes.findIndex(item => Number(item[5]) === Number.parseInt(screen.code.slice(2), 16));
  const node = foundNode<0 && ['0x03003d','0x03003e','0x03003f','0x030040','0x030041','0x030042','0x030043','0x030044'].includes(screen.code) ? screen.code==='0x030044'?MOTOR_SELECTOR_NODE:61+['0x03003d','0x03003e','0x03003f','0x030040','0x030041','0x030042','0x030043','0x030044'].indexOf(screen.code) : foundNode;
  const defaultSel = (n: number) => (n === 34 || n === 35 ? 1 : 0);
  const [position, setPosition] = useState<Position>({ node, selection: defaultSel(node), firstVisible: 0 });
  const [clockMs, setClockMs] = useState(clock);
  const [feedback, setFeedback] = useState<string | null>(null);
  const quickSelection=useRef(0);
  const radarPosition=useRef({selection:0,firstVisible:0});
  const history = useRef<Position[]>([]), scrollStarted = useRef(clock());
  const current = node>=0 ? firmwareNode(node) : undefined;
  const state = position.node === node ? position : { node, selection: defaultSel(node), firstVisible: 0 };
  const quick=node===library.symbols.AFR2_N_QUICK,home=node===library.symbols.AFR2_N_HOME;
  const dynamic = node === library.symbols.AFR2_N_WIFI_NETWORKS;
  const motorPick=screen.code==='0x030044';
  const hallPick=screen.code==='0x030036',hallCount=screen.code==='0x030037',saveChoice=screen.code==='0x030038';
  // AFR2-20261108-06/07: "Sin red Wi-Fi" (two choices, no countdown in the library) and the boot splash (no node).
  const wifiAsk=screen.code==='0x03004b',splash=screen.code==='0x010004';
  const hallStatus=node===library.symbols.AFR2_N_HALL_VIEW;
  const caps=node===library.symbols.AFR2_N_CAPS;
  const batteryView=screen.code==='0x03003a';
  const count = home?0:node===45?2:node===67?3:caps?6:motorPick || hallPick?4:saveChoice || wifiAsk?2:current && !dynamic ? firmwareNodeCount(node,false,layout) : 0;
  const visible = saveChoice || wifiAsk?2:current?.[2] && node!==library.symbols.AFR2_N_DIAG ? 1 : 3;
  const siblingViews=[library.symbols.AFR2_N_RADAR_VIEW,library.symbols.AFR2_N_US_VIEW,library.symbols.AFR2_N_LINE_VIEW,library.symbols.AFR2_N_IR_VALUES] as number[];
  const sensorView=siblingViews.includes(node) || node===library.symbols.AFR2_N_SERVO_ANGLE;
  const drawable = splash || (!!current && (quick || home || caps || node===62 || node===64 || node===65 || node===67 || node===68 || node===69 || node===45 || !current[2] || node===library.symbols.AFR2_N_DIAG || sensorView || motorPick || hallPick || hallCount || saveChoice || hallStatus || batteryView || wifiAsk) && !dynamic);
  useEffect(() => {
    if (position.node !== node) { history.current = []; setPosition({ node, selection: defaultSel(node), firstVisible: 0 }); }
    scrollStarted.current = clock(); setClockMs(clock()); setFeedback(null);
  }, [node, position.node]);
  const longText = drawable && count>0 && !motorPick && !hallPick && !hallCount && !saveChoice && firmwareMenuItem(node, state.selection,layout).label.length * 7 > 93;
  useEffect(() => {
    if ((!longText && !splash) || !active) return;
    const timer = window.setInterval(() => { if (document.visibilityState === 'visible') setClockMs(clock()); }, 40);
    return () => window.clearInterval(timer);
  }, [longText, splash, active]);
  const buffer = useMemo(() => {
    const result = new Afr2OledBuffer();
    if (drawable) renderFirmwareScreen({ ...state, clockMs, scrollStartedMs: scrollStarted.current,
      noticeStartedMs: 0, redraw: true, scrollRedraw: false, keyboard: false, keyboardField: false,
      key: 0, length: 0, ssid: '', edit: false, editingLabel: '', draft: 0, notice: null,
      splashMs: splash ? (clockMs - ((clockMs - scrollStarted.current) >>> 0) % SPLASH_MS) >>> 0 : undefined,
    }, result, caps ? {...layout,lines:[['UI: encoder/OLED','Dibujo por TWI','Estado: sin datos'],['UART: puente ESP','Wi-Fi/HTTP/WS','Estado: sin datos'],['Sensores: IR/HC','Linea / Hall','Estado: sin datos'],['PWM: sin odometria','STOP corta salidas','Estado: sin datos'],['GOTO requiere pose','CPR y CAD nominal','Estado: sin datos'],['Secuencias: pasos','Guardar explicito','Estado: sin datos']][state.selection]} : batteryView ? { ...layout, lines: ['Bateria: -- V', 'Realizar medicion', 'Desde Bateria'] } : layout);
    return result;
  }, [drawable, state.node, state.selection, state.firstVisible, clockMs, batteryView]);
  const move = (delta: number) => {
    if(!active)return;
    if(hallCount)return;
    if(motorPick){setPosition({...state,selection:stepMotor(state.selection,delta)});setFeedback(null);return;}
    if(node===45 || node===67){setPosition({...state,selection:Math.max(0,Math.min(node===45?1:2,state.selection+delta))});return;}
    if(caps || quick){const selection=Math.max(0,Math.min(caps?5:6,state.selection+delta));if(quick)quickSelection.current=selection;setPosition({...state,selection});scrollStarted.current=clock();setClockMs(clock());return;}
    if(hallPick || saveChoice || wifiAsk){setPosition({...state,selection:(state.selection+delta%count+count)%count});setFeedback(null);return;}
    if(sensorView){
      if(node===library.symbols.AFR2_N_SERVO_ANGLE){setFeedback('El control físico del servo requiere el equipo. Usá Distancia y servo para enviar una orden.');return;}
      if((delta>0 && (node===library.symbols.AFR2_N_RADAR_VIEW || node===library.symbols.AFR2_N_LINE_VIEW)) || (delta<0 && (node===library.symbols.AFR2_N_US_VIEW || node===library.symbols.AFR2_N_IR_VALUES))){const target=node===library.symbols.AFR2_N_RADAR_VIEW?library.symbols.AFR2_N_US_VIEW:node===library.symbols.AFR2_N_US_VIEW?library.symbols.AFR2_N_RADAR_VIEW:node===library.symbols.AFR2_N_LINE_VIEW?library.symbols.AFR2_N_IR_VALUES:library.symbols.AFR2_N_LINE_VIEW;navigate({node:target,selection:0,firstVisible:0});}return;
    }
    if (!count || !active) return;
    const next = { ...state };
    for (let i = 0; i < Math.min(3, Math.abs(delta)); i++) {
      next.selection = (next.selection + Math.sign(delta) + count) % count;
      if (next.selection < next.firstVisible) next.firstVisible = next.selection;
      if (next.selection >= next.firstVisible + visible) next.firstVisible = next.selection - visible + 1;
    }
    if(quick)quickSelection.current=state.selection;
    scrollStarted.current = clock(); setClockMs(clock()); setPosition(next); setFeedback(null);
  };
  const navigate = (next: Position) => {
    if(node===library.symbols.AFR2_N_RADAR_CONFIG)radarPosition.current={selection:state.selection,firstVisible:state.firstVisible};
    if(next.node===library.symbols.AFR2_N_RADAR_CONFIG)next={...next,...radarPosition.current};
    if(quick)quickSelection.current=state.selection;
    scrollStarted.current = clock(); setClockMs(clock()); setPosition(next); setFeedback(null);
    onNavigate(normalizeScreenCode('0x' + Number(firmwareNode(next.node)[5]).toString(16))!);
  };
  const navigateBack = (targetNode: number) => {
    const idx = history.current.map(h => h.node).lastIndexOf(targetNode);
    if (idx >= 0) {
      const targetPos = history.current[idx];
      history.current = history.current.slice(0, idx);
      navigate(targetPos);
    } else {
      navigate({ node: targetNode, selection: defaultSel(targetNode), firstVisible: 0 });
    }
  };
  const press = (kind: 'short' | 'long') => {
    if(active && splash){setFeedback('La pantalla de inicio dura 3 s al encender; corto o largo la saltean.');return;}
    if (!active || !current) return;
    if(motorPick){if(kind==='long')navigate(history.current.pop()??{node:library.symbols.AFR2_N_TESTS,selection:0,firstVisible:0});else {const chosen=MOTOR_IDS[state.selection&3];navigate(history.current.pop()??{node:library.symbols.AFR2_N_TESTS,selection:0,firstVisible:0});setFeedback(`Motor ${chosen} confirmado en la vista local; sin modificar el equipo.`);}return;}
    if(node===45 || node===65 || node===68 || node===69){if(kind==='long' || kind==='short')navigateBack(node===68 || node===69?31:32);return;}
    if(node===67 && kind==='short'){setFeedback('Cambiar el modo requiere confirmación del ESP; la biblioteca no modifica la radio.');return;}
    if(node===62){if(kind==='long')navigate(history.current.pop()??{node:library.symbols.AFR2_N_TESTS,selection:0,firstVisible:0});else setFeedback('La detección de pulsaciones requiere el encoder físico.');return;}
    if(wifiAsk){setFeedback('Iniciar AP o Seguir esperando lo decide el auto con el ESP; la biblioteca no cambia la red.');return;}
    if(saveChoice){setFeedback('La elección Sí/No requiere cambios reales del controlador; la biblioteca no guarda ni descarta mediciones.');return;}
    if(hallPick || hallCount){
      if(kind==='long'){navigate(history.current.pop()??{node:library.symbols.AFR2_N_HALL_TEST,selection:0,firstVisible:0});setFeedback('Vista previa cerrada; no había un conteo físico activo.');}
      else setFeedback('Confirmar la rueda y contar pulsos requiere el controlador conectado. No se simulan mediciones.');
      return;
    }
    if(caps && kind==='short'){navigateBack(library.symbols.AFR2_N_SYSTEM);return;}
    if(hallStatus && kind==='short'){navigateBack(Number(current[1]));return;}
    if(kind==='long'){
      if(home){history.current=[];navigate({node:library.symbols.AFR2_N_QUICK,selection:quickSelection.current,firstVisible:0});}
      else navigateBack(Number(current[1]));
      return;
    }
    if(home){history.current=[{...state}];navigate({node:library.symbols.AFR2_N_MENU,selection:0,firstVisible:0});return;}
    if(sensorView){
      if(node===library.symbols.AFR2_N_RADAR_VIEW || node===library.symbols.AFR2_N_US_VIEW){history.current.push({...state});navigate({node:library.symbols.AFR2_N_RADAR_OPTIONS,selection:0,firstVisible:0});}
      return;
    }
    if (!count) { setFeedback('Esta pantalla necesita datos preparados por el controlador.'); return; }
    const item = firmwareMenuItem(node, state.selection,layout);
    if(item.action===library.symbols.AFR2_A_WHEEL){history.current.push({...state});navigate({node:MOTOR_SELECTOR_NODE,selection:0,firstVisible:0});return;}
    if(item.action===library.symbols.AFR2_A_HALL_BEGIN){
      const target=library.nodes.findIndex(n=>Number(n[5])===0x030036);
      if(target>=0){history.current.push({...state});navigate({node:target,selection:0,firstVisible:0});}return;
    }
    if (item.action) { setFeedback('Esta acción se ejecuta con el equipo conectado. La biblioteca local solo permite navegar.'); return; }
    if (item.label.startsWith('Volver')) navigateBack(Number(current[1]));
    else {
      if (item.target === library.symbols.AFR2_N_HOME || item.target === library.symbols.AFR2_N_QUICK || item.target === library.symbols.AFR2_N_MENU) history.current = [];
      else if (history.current.length < 8) history.current.push({ ...state });
      navigate({ node: item.target, selection: defaultSel(item.target), firstVisible: 0 });
    }
  };
  const [lastInput, setLastInput] = useState('ninguna');
  const backProp = screen.role === 'menu' && count > 0 ? () => {
    if (!active || !current) return;
    for (let i = 0; i < count; i++) { const item = firmwareMenuItem(node, i); if (!item.action && item.label.startsWith('Volver')) { navigateBack(Number(current[1])); break; } }
  } : undefined;
  return <OledWorkbench
    lastInput={lastInput}
    statusBar={<span className="afr2-status-pill" data-state="ready"><span className="afr2-status-dot" aria-hidden="true" />Vista local del diseño · sin datos del equipo</span>}
    screen={drawable
      ? <OledCanvas128x64 compact oledBuffer={buffer} screenCode={screen.code} sourceMode="local-firmware-preview" showGrid={false} />
      : <div className="afr2-oled-data-required"><strong>{screen.name}</strong><p>Esta vista necesita un snapshot del equipo. Consultala en el visor conectado.</p></div>}
    side={{ selection: state.selection, firstVisible: state.firstVisible, count: motorPick || hallPick || sensorView || quick || home || caps || node === 62 || node === 45 || node === 67 ? 0 : count, visibleRows: visible, disabled: !active,
      onBack: backProp,
      onHighlight: index => { setLastInput(`Resaltar opción ${index + 1}`); move(index - state.selection); },
      onScroll: direction => move(direction * Math.min(3, direction < 0 ? state.selection : count - state.selection - 1)) }}
    encoder={{ available: node >= 0 && active, statusMessage: feedback,
      onRotate: delta => { setLastInput(delta < 0 ? 'Giro izquierdo (-1)' : 'Giro derecho (+1)'); move(delta); },
      onPress: kind => { setLastInput(kind === 'long' ? 'SW largo (Volver al soltar)' : 'SW corto (Confirmar)'); press(kind); } }}
  />;
}
