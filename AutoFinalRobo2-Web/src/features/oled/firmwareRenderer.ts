/** AFR2-20261024-09. Browser copy of current AVR pure layout library.
 * Ported from afr2_{screens,network_screen,sequence_screen,icons}.c.
 * C layout source hashes are pinned in firmwareRenderer.sources.json; build
 * refuses a changed layout until this port is reviewed. Resources are copied.
 * Requires the same PREPARED snapshot, UI state, clock and previous framebuffer
 * as AVR. Does not fabricate telemetry or predict remote navigation/actions.
 */
import { Afr2OledBuffer } from './oledRasterizer';
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
const S = L.symbols;
const trunc = Math.trunc;
const elapsed = (now: number, then: number) => (now - then) >>> 0;
export interface FirmwareMenuItem { label: string; target: number; action: number; arg: number; min: number; max: number; step: number }
export interface FirmwareUiState {
  node: number; selection: number; firstVisible: number;
  clockMs: number; noticeStartedMs: number; scrollStartedMs: number;
  redraw: boolean; scrollRedraw: boolean;
  keyboard: boolean; keyboardField: boolean; key: number; length: number; ssid: string;
  edit: boolean; editingLabel: string; draft: number;
  notice: { title: string; text: string; ms: number; kind: number } | null;
}
export interface FirmwareSnapshot {
  lines: readonly string[]; labels: readonly string[]; disabled: readonly number[];
  sequenceState: number; sequenceCount: number; sequenceIndex: number; sequenceKind: number;
  sequenceDurationMs: number; sequenceElapsedMs: number;
  networkValid: boolean; networkSta: boolean; networkAp: boolean;
  networkSsid: string; networkApSsid: string; networkPassword: string; networkIp: string; networkApIp: string;
}
export function firmwareMenuItem(node: number, index: number): FirmwareMenuItem {
  const n = L.nodes[node] ?? L.nodes[0];
  index = Math.min(index, Number(n[3]) - 1);
  let row: readonly (string | number)[];
  const first = Number(n[4]);
  if (first & 0x8000) row = L.extra[(first & 0x7fff) + index];
  else if (node === S.AFR2_N_SYSTEM && index === 5) row = L.extra[0];
  else if (node === S.AFR2_N_WIFI && (index === 1 || index === 2)) row = L.extra[index];
  else {
    if (node === S.AFR2_N_SYSTEM && index > 5) index--;
    if (node === S.AFR2_N_WIFI && index > 2) index -= 2;
    row = L.items[first + index];
  }
  return { label: String(row[0]), target: Number(row[1]), action: Number(row[2]), arg: Number(row[3]), min: Number(row[4]), max: Number(row[5]), step: Number(row[6]) };
}
export function firmwareTextOffset(text: string, width: number, time: number): number {
  const pixels = text.length * 7;
  if (pixels <= width) return 0;
  const end = pixels - width, phase = (time >>> 0) % (1600 + end * 40);
  return phase <= 800 ? 0 : Math.min(end, trunc((phase - 800) / 40));
}
function icon(o: Afr2OledBuffer, id: number, x: number, y: number, size: number) {
  const image = L.icons[id];
  if (!image || !size) return;
  const largest = Math.max(image.width, image.height), stride = trunc((image.width + 7) / 8);
  const width = trunc(image.width * size / largest), height = trunc(image.height * size / largest);
  x += trunc((size - width) / 2); y += trunc((size - height) / 2);
  for (let py = 0; py < height; py++) for (let px = 0; px < width; px++) {
    const sx = trunc(px * image.width / width), sy = trunc(py * image.height / height);
    if (image.bytes[sy * stride + trunc(sx / 8)] & (1 << (sx & 7))) o.setPixel(x + px, y + py);
  }
}
function menuIcon(node: number, target: number, action: number, label: string): number {
  if (label.startsWith('Volver')) return S.AFR2_ICON_BACK;
  if (action === S.AFR2_A_REBOOT_MEGA || action === S.AFR2_A_REBOOT_ESP || target === S.AFR2_N_REBOOT) return S.AFR2_ICON_REFRESH;
  if (target === S.AFR2_N_AP_INFO || node === S.AFR2_N_WIFI_AP) return S.AFR2_ICON_AP;
  if (target === S.AFR2_N_WIFI || target === S.AFR2_N_NETWORK || node === S.AFR2_N_WIFI || (action >= S.AFR2_A_WIFI_STATUS && action <= S.AFR2_A_WIFI_SET_MODE)) return S.AFR2_ICON_WIFI;
  if (target === S.AFR2_N_ENCODER_VIEW || target === S.AFR2_N_ENCODER_CONFIG) return S.AFR2_ICON_ENCODER;
  if (([S.AFR2_N_SENSORS, S.AFR2_N_LINE_VIEW, S.AFR2_N_US_VIEW, S.AFR2_N_HALL_VIEW] as number[]).includes(target)) return S.AFR2_ICON_SENSORS;
  if (target === S.AFR2_N_PREFS || target === S.AFR2_N_PREFS_VIEW || action === S.AFR2_A_SAVE || action === S.AFR2_A_DISCARD) return S.AFR2_ICON_PREFS;
  if (target === S.AFR2_N_LINK_VIEW) return S.AFR2_ICON_LINK;
  if (([S.AFR2_N_CONTROL, S.AFR2_N_MOVE, S.AFR2_N_WHEEL] as number[]).includes(target) || node === S.AFR2_N_MOVE || node === S.AFR2_N_WHEEL) return S.AFR2_ICON_MOTOR;
  if (target === S.AFR2_N_TESTS || target === S.AFR2_N_CALIB || node === S.AFR2_N_TESTS || node === S.AFR2_N_CALIB) return S.AFR2_ICON_TOOLS;
  if (target === S.AFR2_N_SYSTEM || target === S.AFR2_N_DISPLAY_MENU || node === S.AFR2_N_SYSTEM || node === S.AFR2_N_DISPLAY_MENU) return S.AFR2_ICON_CONFIG;
  if (action === S.AFR2_A_STOP || target === S.AFR2_N_ALERT_VIEW) return S.AFR2_ICON_WARNING;
  return S.AFR2_ICON_INFO;
}
function line(o: Afr2OledBuffer, y: number, text: string, time: number) {
  o.fillRect(0, y, 128, 10, 'black'); o.textWindow(0, 128, y, text, firmwareTextOffset(text, 128, time));
}
function labeled(o: Afr2OledBuffer, y: number, label: string, left: number, text: string, time: number) {
  o.fillRect(0, y, 128, 10, 'black'); o.drawText(0, y, label); o.textWindow(left, 128, y, text, firmwareTextOffset(text, 128 - left, time));
}
function network(o: Afr2OledBuffer, u: FirmwareUiState, s: FirmwareSnapshot) {
  const ap = u.node === S.AFR2_N_AP_INFO, time = elapsed(u.clockMs, u.scrollStartedMs);
  icon(o, ap ? S.AFR2_ICON_AP : S.AFR2_ICON_WIFI, 0, 0, 14); o.drawText(21, 1, ap ? 'Datos de red AP' : 'IP de conexion');
  if (!s.networkValid) { line(o, 17, 'Sin datos actuales', time); line(o, 32, 'Actualizar Wi-Fi', time); }
  else if (ap) {
    if (!s.networkAp) { line(o, 17, 'AP no esta activo', time); line(o, 32, 'IP: --', time); }
    else { labeled(o, 16, 'SSID:', 35, s.networkApSsid, time); labeled(o, 28, 'Clave:', 42, s.networkPassword || 'Sin clave', time); labeled(o, 40, 'IP:', 21, s.networkApIp, time); }
  } else if (s.networkSta || s.networkAp) {
    labeled(o, 16, 'SSID:', 35, s.networkSta ? s.networkSsid : s.networkApSsid, time);
    labeled(o, 29, 'IP:', 21, s.networkSta ? s.networkIp : s.networkApIp, time);
    line(o, 41, s.networkSta ? 'STA / abrir en Web' : 'AP / abrir en Web', time);
  } else { line(o, 17, 'Sin IP adquirida', time); line(o, 32, 'Esperando conexion', time); }
  icon(o, S.AFR2_ICON_BACK, 0, 54, 8); o.drawText(14, 53, 'Volver');
}
function networkNotice(o: Afr2OledBuffer, u: FirmwareUiState, s: FirmwareSnapshot) {
  const n = u.notice!, kind = n.kind - 16, time = elapsed(u.clockMs, u.noticeStartedMs);
  let image: number = S.AFR2_ICON_INFO;
  if (kind === S.AFR2_NET_GOT_IP || kind === S.AFR2_NET_STA_LOST) image = S.AFR2_ICON_WIFI;
  else if (kind === S.AFR2_NET_AP_READY || kind === S.AFR2_NET_AP_STOPPED) image = S.AFR2_ICON_AP;
  else if (([S.AFR2_NET_CLIENT_JOIN, S.AFR2_NET_CLIENT_LEAVE, S.AFR2_NET_WS_OPEN, S.AFR2_NET_WS_CLOSE] as number[]).includes(kind)) image = S.AFR2_ICON_LINK;
  icon(o, image, 0, 6, 16);
  switch (kind) {
    case S.AFR2_NET_BOOT: o.drawText(24, 9, 'ESP iniciada'); line(o, 31, 'Firmware propio', time); break;
    case S.AFR2_NET_WEB_READY: o.drawText(24, 9, 'Servidor listo'); line(o, 31, 'HTTP disponible', time); break;
    case S.AFR2_NET_GOT_IP: o.drawText(24, 9, 'IP adquirida'); line(o, 28, s.networkValid ? s.networkSsid : 'Wi-Fi', time); line(o, 41, n.text, time); break;
    case S.AFR2_NET_AP_READY:
      o.drawText(24, 8, 'Red AP creada');
      if (s.networkValid && s.networkAp) { labeled(o, 23, 'SSID:', 35, s.networkApSsid, time); labeled(o, 34, 'Clave:', 42, s.networkPassword || 'Sin clave', time); labeled(o, 45, 'IP:', 21, s.networkApIp, time); }
      else line(o, 31, 'Consultar datos AP', time);
      break;
    case S.AFR2_NET_CLIENT_JOIN: o.drawText(24, 9, 'Cliente entro'); line(o, 32, n.text, time); break;
    case S.AFR2_NET_CLIENT_LEAVE: o.drawText(24, 9, 'Cliente salio'); line(o, 32, n.text, time); break;
    case S.AFR2_NET_WS_OPEN: o.drawText(24, 9, 'WebSocket OK'); line(o, 32, 'Cliente conectado', time); break;
    case S.AFR2_NET_WS_CLOSE: o.drawText(24, 9, 'WebSocket cerro'); line(o, 32, 'Cliente salio', time); break;
    case S.AFR2_NET_WEB_SERVED: o.drawText(24, 9, 'Web servida'); line(o, 32, 'HTTP 200 iniciado', time); break;
    case S.AFR2_NET_STA_LOST: o.drawText(24, 9, 'Wi-Fi sin red'); line(o, 32, 'STA desconectada', time); break;
    case S.AFR2_NET_AP_STOPPED: o.drawText(24, 9, 'Red AP apagada'); line(o, 32, 'Sin acceso por AP', time); break;
  }
  if (kind !== S.AFR2_NET_AP_READY) o.drawText(0, 54, 'Click: cerrar');
}
function sequence(o: Afr2OledBuffer, s: FirmwareSnapshot) {
  const count = s.sequenceCount, index = s.sequenceIndex;
  const completed = s.sequenceState === 4 ? count : index < count ? index : 0;
  for (let i = 0; i < count; i++) {
    const x = 1 + trunc(i * 126 / count), w = trunc((i + 1) * 126 / count) - trunc(i * 126 / count) - 1;
    if (i < completed) o.fillRect(x, 0, w, 7);
    else { o.drawRect(x, 0, w, 7); if (s.sequenceState === 3 && i === index && s.sequenceDurationMs) {
      const fill = trunc((w > 2 ? w - 2 : 0) * s.sequenceElapsedMs / s.sequenceDurationMs) & 255;
      if (fill) o.fillRect(x + 1, 2, fill, 3);
    } }
  }
  let kind = s.sequenceKind < L.sequenceNames.length ? s.sequenceKind : S.AFR2_SEQ_WHEELS;
  if (s.sequenceState === 4) kind = S.AFR2_SEQ_FINISH;
  for (let y = 0; y < 8; y++) for (let x = 0; x < 8; x++) if (L.sequenceIcons[kind][y] & (0x80 >> x)) o.drawRect(56 + 2 * x, 10 + 2 * y, 2, 2);
  const name = s.sequenceState === 5 ? 'Cancelada' : s.sequenceState === 6 ? 'Error de ejecucion' : L.sequenceNames[kind];
  o.drawText(trunc((128 - 7 * name.length) / 2), 28, name);
  o.drawText(0, 42, `Tiempo ${trunc(s.sequenceDurationMs / 1000)}.${String(trunc((s.sequenceDurationMs % 1000) / 10)).padStart(2, '0')} s`.slice(0, 17));
  o.drawText(0, 54, `${index < count ? index + 1 : count}/${count}  > Volver`.slice(0, 17));
}
function keyboard(o: Afr2OledBuffer, u: FirmwareUiState) {
  o.drawText(0, 0, u.keyboardField ? 'Clave Wi-Fi' : 'SSID Wi-Fi');
  const length = Math.min(u.length, 17);
  o.drawText(0, 13, u.keyboardField ? '*'.repeat(length) : u.ssid.slice(u.length - length, u.length));
  const first = trunc(trunc(u.key / 10) / 2) * 20;
  for (let j = 0; j < 20 && first + j < 97; j++) {
    const k = first + j, x = (j % 10) * 12, y = 29 + trunc(j / 10) * 12;
    o.drawText(x, y, k < 95 ? String.fromCharCode(32 + k) : k === 95 ? '<' : 'OK');
    if (k === u.key) o.drawRect(x - 1, y - 1, 12, 12, 'invert');
  }
  o.drawText(0, 53, 'Largo: cancelar');
}
/** Corresponds to Afr2Ui_Render, including partial scroll updates. Caller owns
 * revision/redraw flags and must provide a retained buffer for partial draws. */
export function renderFirmwareScreen(u: FirmwareUiState, o: Afr2OledBuffer, s: FirmwareSnapshot): void {
  const n = L.nodes[u.node] ?? L.nodes[0], partial = !u.redraw && u.scrollRedraw;
  if (!partial) o.clear();
  if (u.notice) {
    const a = u.notice, width = Math.min(128, trunc((((elapsed(u.clockMs, u.noticeStartedMs) * 128) >>> 0) / a.ms)) & 0xffff);
    o.fillRect(0, 0, 128, 2, 'black'); o.fillRect(0, 0, width, 2);
    if (a.kind > 16) networkNotice(o, u, s);
    else if (!partial) { icon(o, a.kind ? S.AFR2_ICON_WARNING : S.AFR2_ICON_INFO, 56, 6, 16); o.drawText(0, 25, a.title); o.drawText(0, 39, a.text); o.drawText(0, 53, 'Click: cerrar'); }
  } else if (u.keyboard) keyboard(o, u);
  else if (u.node === S.AFR2_N_SEQUENCE) sequence(o, s);
  else if (u.node === S.AFR2_N_NETWORK || u.node === S.AFR2_N_AP_INFO) network(o, u, s);
  else if (u.edit) { o.drawText(0, 0, u.editingLabel); o.drawText(0, 23, String(u.draft).slice(0, 17)); o.drawText(0, 40, 'Click: aplicar'); o.drawText(0, 53, 'Largo: descartar'); }
  else {
    const view = Number(n[2]) !== 0, count = view ? 1 : 3;
    if (!partial) { o.drawText(0, 0, String(n[0])); if (view) for (let j = 0; j < 3; j++) o.drawText(0, 13 + j * 12, s.lines[j]); }
    for (let row = 0; row < count && u.firstVisible + row < Number(n[3]); row++) {
      const item = firmwareMenuItem(u.node, u.firstVisible + row), selected = u.firstVisible + row === u.selection;
      if (partial && !selected) continue;
      const y = view ? 52 : 16 + 16 * row, left = view ? 14 : 25, top = view ? y : y + 3;
      if (!partial) { icon(o, menuIcon(u.node, item.target, item.action, item.label), view ? 0 : 3, view ? 54 : y, view ? 8 : 16); if (selected && !view) o.drawRect(0, y, 128, 16); o.drawText(120, top, selected ? s.disabled[row] ? '!' : '>' : s.disabled[row] ? '!' : ' '); }
      const label = s.labels[row] || item.label;
      o.fillRect(left, top, 118 - left, 10, 'black'); o.textWindow(left, 118, top, label, selected ? firmwareTextOffset(label, 118 - left, elapsed(u.clockMs, u.scrollStartedMs)) : 0);
    }
  }
}
