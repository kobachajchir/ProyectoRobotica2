import { useEffect, useMemo, useRef, useState } from 'react';
import { FIRMWARE_LIBRARY as library } from './generated/firmwareLibrary';
import { firmwareMenuItem, renderFirmwareScreen, type FirmwareSnapshot } from './firmwareRenderer';
import { Afr2OledBuffer } from './oledRasterizer';
import { OledCanvas128x64 } from './OledCanvas128x64';
import { EncoderControl } from './EncoderControl';
import { MenuSideControls } from './MenuSideControls';
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
  const node = library.nodes.findIndex(item => Number(item[5]) === Number.parseInt(screen.code.slice(2), 16));
  const [position, setPosition] = useState<Position>({ node, selection: 0, firstVisible: 0 });
  const [clockMs, setClockMs] = useState(clock);
  const [feedback, setFeedback] = useState<string | null>(null);
  const history = useRef<Position[]>([]), scrollStarted = useRef(clock());
  const current = library.nodes[node];
  const state = position.node === node ? position : { node, selection: 0, firstVisible: 0 };
  const dynamic = node === library.symbols.AFR2_N_WIFI_NETWORKS;
  const count = current && !dynamic ? Number(current[3]) : 0;
  const visible = current?.[2] ? 1 : 3;
  const drawable = !!current && !current[2] && !dynamic;
  useEffect(() => {
    if (position.node !== node) { history.current = []; setPosition({ node, selection: 0, firstVisible: 0 }); }
    scrollStarted.current = clock(); setClockMs(clock()); setFeedback(null);
  }, [node, position.node]);
  const longText = drawable && firmwareMenuItem(node, state.selection).label.length * 7 > 93;
  useEffect(() => {
    if (!longText || !active) return;
    const timer = window.setInterval(() => { if (document.visibilityState === 'visible') setClockMs(clock()); }, 40);
    return () => window.clearInterval(timer);
  }, [longText, active]);
  const buffer = useMemo(() => {
    const result = new Afr2OledBuffer();
    if (drawable) renderFirmwareScreen({ ...state, clockMs, scrollStartedMs: scrollStarted.current,
      noticeStartedMs: 0, redraw: true, scrollRedraw: false, keyboard: false, keyboardField: false,
      key: 0, length: 0, ssid: '', edit: false, editingLabel: '', draft: 0, notice: null,
    }, result, layout);
    return result;
  }, [drawable, state.node, state.selection, state.firstVisible, clockMs]);
  const move = (delta: number) => {
    if (!count || !active) return;
    const next = { ...state };
    for (let i = 0; i < Math.min(3, Math.abs(delta)); i++) {
      next.selection = (next.selection + Math.sign(delta) + count) % count;
      if (next.selection < next.firstVisible) next.firstVisible = next.selection;
      if (next.selection >= next.firstVisible + visible) next.firstVisible = next.selection - visible + 1;
    }
    scrollStarted.current = clock(); setClockMs(clock()); setPosition(next); setFeedback(null);
  };
  const navigate = (next: Position) => {
    scrollStarted.current = clock(); setClockMs(clock()); setPosition(next); setFeedback(null);
    onNavigate(normalizeScreenCode('0x' + Number(library.nodes[next.node][5]).toString(16))!);
  };
  const press = (kind: 'short' | 'long') => {
    if (!active) return;
    if (kind === 'long') { history.current = []; navigate({ node: library.symbols.AFR2_N_MENU, selection: 0, firstVisible: 0 }); return; }
    if (!count) { setFeedback('Esta pantalla necesita datos preparados por el controlador.'); return; }
    const item = firmwareMenuItem(node, state.selection);
    if (item.action) { setFeedback('Esta acción se ejecuta con el equipo conectado. La biblioteca local solo permite navegar.'); return; }
    if (item.label.startsWith('Volver')) navigate(history.current.pop() ?? { node: item.target, selection: 0, firstVisible: 0 });
    else {
      if (item.target === library.symbols.AFR2_N_HOME || item.target === library.symbols.AFR2_N_MENU) history.current = [];
      else if (history.current.length < 8) history.current.push({ ...state });
      navigate({ node: item.target, selection: 0, firstVisible: 0 });
    }
  };
  return <div className="afr2-page-stack afr2-library-preview">
    <p className="afr2-help">Vista local del diseño. Botones 1–3: resaltar fila; SW: confirmar. Flechas: desplazar selección. Acciones y datos del equipo requieren el visor conectado.</p>
    <div className="afr2-oled-with-side">
      {drawable ? <OledCanvas128x64 compact oledBuffer={buffer} screenCode={screen.code} sourceMode="local-firmware-preview" showGrid={false} />
        : <div className="afr2-oled-data-required"><strong>{screen.name}</strong><p>Esta vista necesita un snapshot del equipo. Consultala en el visor conectado.</p></div>}
      <MenuSideControls selection={state.selection} firstVisible={state.firstVisible} count={count} visibleRows={visible} disabled={!active}
        onHighlight={index => move(index - state.selection)} onScroll={direction => move(direction * Math.min(3, direction < 0 ? state.selection : count - state.selection - 1))} />
    </div>
    <EncoderControl label="Encoder · biblioteca local" available={node >= 0 && active} onRotate={move} onPress={press} statusMessage={feedback} compact />
  </div>;
}
