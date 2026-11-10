import { useMemo, type CSSProperties } from 'react';
import { renderFirmwareScreen, type FirmwareSnapshot, type FirmwareUiState } from '../oled/firmwareRenderer';
import { FIRMWARE_LIBRARY as L } from '../oled/generated/firmwareLibrary';
import { Afr2OledBuffer } from '../oled/oledRasterizer';
import { OledCanvas128x64 } from '../oled/OledCanvas128x64';
import { assemblyTimeAtProgress } from './assemblyPlaybackTimeline';
import { ease, segment } from './assemblyTimeline';

export const interfaceReveal = (progress: number) =>
  ease(segment(progress, .43, .438)) * (1 - ease(segment(progress, .476, .482)));

const SCREENS = [
  { node: L.symbols.AFR2_N_HOME, title: 'Dashboard' },
  { node: L.symbols.AFR2_N_RADAR_VIEW, title: 'Radar gráfico' },
  { node: L.symbols.AFR2_N_WIFI_NETWORKS, title: 'Redes Wi-Fi' },
];
// Presentation fixtures only. Never read or send commands to a connected robot.
const NETWORKS = ['Koba', 'Taller_2G', 'Robotica_Lab'];

export function AssemblyInterfaceShowcase({ progress, reduced }: { progress: number; reduced: boolean }) {
  const clockMs = reduced ? 18000 : Math.round(assemblyTimeAtProgress(progress) * 1000 / 80) * 80;
  const frames = useMemo(() => {
    const angle = Math.round(25 + 130 * (1 - Math.cos((clockMs - 16620) / 1800)) / 2);
    const pulseUs = 1000 + Math.round(angle * 1000 / 180);
    const snapshot: FirmwareSnapshot = {
      lines: ['Reposo', '', ''], labels: ['', '', ''], disabled: [0, 0, 0],
      sequenceState: 0, sequenceCount: 0, sequenceIndex: 0, sequenceKind: 0, sequenceDurationMs: 0, sequenceElapsedMs: 0,
      networkValid: true, networkSta: true, networkAp: false, networkMode: 1,
      networkSsid: 'Koba', networkApSsid: '', networkPassword: '', networkIp: '192.0.2.2', networkApIp: '',
      networkServer: true, wifiConnected: true, networkRssi: -48, networkRssiValid: true,
      networkCount: 3, networkTotal: 3, networkPage: 0, networkScanState: 2,
      dashboardValid: true, dashboardMode: 0, dashboardMoving: false, dashboardFault: false,
      batteryValid: true, batteryMv: 7800, quickFlags: 0, radarRange: 0, radarTrace: true,
      sensor: { nowMs: clockMs, angle, pulseUs, targetUs: pulseUs, enabled: true, running: true,
        latest: { id: 1, ms: clockMs, distanceMm: 720, error: 0, angle, pulseUs },
        bins: Array.from({ length: 37 }, (_, i) => ({ id: i % 5 === 0 ? 1 : 0, distanceMm: 550 + (i % 7) * 170, error: 0 })),
      },
    };
    return SCREENS.map(({ node }) => {
      const state: FirmwareUiState = { node, selection: 0, firstVisible: 0, clockMs,
        noticeStartedMs: 0, scrollStartedMs: clockMs, redraw: true, scrollRedraw: false,
        keyboard: false, keyboardField: false, key: 0, length: 0, ssid: '',
        edit: false, editingLabel: '', draft: 0, notice: null,
      };
      const buffer = new Afr2OledBuffer();
      renderFirmwareScreen(state, buffer, node === L.symbols.AFR2_N_WIFI_NETWORKS
        ? { ...snapshot, labels: NETWORKS, rowSecurity: [3, 3, 0] } : snapshot);
      return buffer;
    });
  }, [clockMs]);
  const visibility = interfaceReveal(progress);
  if (progress < .43 || progress >= .482) return null;
  return <aside className="afr2-assembly__interfaces" aria-label="Interfaz OLED · demostración con datos de ejemplo"
    style={{ opacity: visibility }}>
    <p className="afr2-assembly__interface-label">Interfaz integrada <span>Datos de ejemplo</span></p>
    {SCREENS.map((screen, index) => {
      const enter = ease(segment(progress, .43 + index * .003, .438 + index * .003));
      const style: CSSProperties = { opacity: enter, transform: reduced ? 'none'
        : `translate3d(${(1 - enter) * 60}px, ${(1 - enter) * 12}px, 0) rotateY(${(1 - enter) * -10}deg) scale(${.96 + .04 * enter})` };
      return <figure key={screen.node} style={style}>
        <figcaption><span>{String(index + 1).padStart(2, '0')}</span>{screen.title}</figcaption>
        <OledCanvas128x64 oledBuffer={frames[index]} screenCode={`0x${Number(L.nodes[screen.node][5]).toString(16).padStart(6, '0')}`}
          sourceMode="local-ui-simulation" compact showGrid={false} scale={2} badge="Datos de ejemplo" />
      </figure>;
    })}
  </aside>;
}
