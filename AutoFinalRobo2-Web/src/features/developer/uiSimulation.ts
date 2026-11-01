/** AFR2-20261031-01. Local port of Afr2Ui_Apply/Open/back and presentation
 * snapshots from afr2_app.c. Service results are explicit demo fixtures, not
 * AVR execution or hardware emulation. No protocol client or persistence. */
import { FIRMWARE_LIBRARY as L } from '../oled/generated/firmwareLibrary';
import { firmwareMenuItem, type FirmwareMenuItem, type FirmwareSnapshot, type FirmwareUiState } from '../oled/firmwareRenderer';

const S = L.symbols;
export type SimulationScenario = 'ready' | 'missing';
interface Position { node: number; selection: number; firstVisible: number }
export interface UiSimulation extends FirmwareUiState {
  scenario: SimulationScenario;
  history: Position[];
  editing: FirmwareMenuItem | null;
  values: Record<number, number>;
  saved: Record<number, number>;
  password: string;
  draftSsid: string;
  wifiDraftReady: boolean;
  connected: boolean;
  scanned: boolean;
  stopped: boolean;
  motionUntil: number;
  mode: number;
  hallMeasuring: boolean;
  hallCount: number;
  cpr: number;
  lineCalibrated: boolean;
  prefsLoaded: boolean;
  turns: number;
  clicks: number;
  holds: number;
  servoEnabled: boolean;
  radar: boolean;
  feedback: string;
}
export type SimulationEvent = { type: 'rotate'; delta: -1 | 1 } | { type: 'press'; kind: 'short' | 'long' } | { type: 'tick'; ms: number } | { type: 'inspect'; code: string };
const sampleNetworks = Array.from({ length: 8 }, (_, i) => `Red demo ${i + 1}`);
const wrap = (n: number, count: number) => ((n % count) + count) % count;
const limit = (n: number, min: number, max: number) => Math.max(min, Math.min(max, n));
export const simulationScreenCode = (state: UiSimulation) => `0x${(state.notice && state.notice.kind > 16 ? 0x040000 + state.notice.kind - 16 : Number(L.nodes[state.node][5])).toString(16).padStart(6, '0')}`;
export const simulationItems = (state: UiSimulation) => Array.from({ length: Number(L.nodes[state.node][3]) }, (_, i) => {
  const item = firmwareMenuItem(state.node, i);
  return state.node === S.AFR2_N_WIFI_NETWORKS && i < 8 && state.scanned ? { ...item, label: sampleNetworks[i] } : item;
});

export function createUiSimulation(scenario: SimulationScenario = 'ready'): UiSimulation {
  // Prepared examples, deliberately independent of connected telemetry/config.
  const values: Record<number, number> = {
    [S.AFR2_A_SPEED]: 150, [S.AFR2_A_LINE_SPEED]: 80, [S.AFR2_A_LINE_KP]: 100,
    [S.AFR2_A_GOTO_X]: 0, [S.AFR2_A_GOTO_Y]: 0, [S.AFR2_A_WHEEL]: 0,
    [S.AFR2_A_TEST_PWM]: 40, [S.AFR2_A_TEST_MS]: 500, [S.AFR2_A_SERVO_US]: 1500,
    [S.AFR2_A_LINE_KIND]: 2, [S.AFR2_A_LINE_CHANNEL]: 0, [S.AFR2_A_LINE_THRESHOLD]: 512,
    [S.AFR2_A_PWM_LIMIT]: 200, [S.AFR2_A_RAMP]: 4, [S.AFR2_A_KP]: 128, [S.AFR2_A_KI]: 8,
    [S.AFR2_A_MOTOR_READY]: scenario === 'ready' ? 1 : 0, [S.AFR2_A_WHEEL_MM]: 80,
    [S.AFR2_A_RADIUS_MM]: 150, [S.AFR2_A_HALL_WIRED]: scenario === 'ready' ? 1 : 0,
    [S.AFR2_A_GEOM_READY]: scenario === 'ready' ? 1 : 0, [S.AFR2_A_ENC_STEPS]: 4,
    [S.AFR2_A_CONTRAST]: 200, [S.AFR2_A_WIFI_MODE]: 3, [S.AFR2_A_SERVO_ANGLE]: 90,
    [S.AFR2_A_RADAR_MIN]: 0, [S.AFR2_A_RADAR_MAX]: 180, [S.AFR2_A_RADAR_SPEED]: 15,
    [S.AFR2_A_RADAR_SETTLE]: 100, [S.AFR2_A_US_INTERVAL]: 150,
  };
  return {
    node: S.AFR2_N_HOME, selection: 0, firstVisible: 0, clockMs: 0,
    noticeStartedMs: 0, scrollStartedMs: 0, redraw: true, scrollRedraw: false,
    keyboard: false, keyboardField: false, key: 0, length: 0, ssid: '',
    edit: false, editingLabel: '', draft: 0, notice: null, editing: null,
    scenario, history: [], values, saved: { ...values }, password: '', draftSsid: '',
    wifiDraftReady: false, connected: false, scanned: false, stopped: true,
    motionUntil: 0, mode: 0, hallMeasuring: false, hallCount: 0,
    cpr: scenario === 'ready' ? 360 : 0, lineCalibrated: scenario === 'ready', prefsLoaded: false,
    turns: 0, clicks: 0, holds: 0, servoEnabled: false, radar: false,
    feedback: 'Inicio simulado. Mantene SW durante 800 ms para abrir el menu, o confirma Menu con un toque corto.',
  };
}
function open(u: UiSimulation, node: number) {
  if (node === S.AFR2_N_HOME || node === S.AFR2_N_MENU) u.history = [];
  else if (u.history.length < 8) u.history.push({ node: u.node, selection: u.selection, firstVisible: u.firstVisible });
  u.node = node; u.selection = 0; u.firstVisible = 0; u.edit = false; u.scrollStartedMs = u.clockMs;
}
function back(u: UiSimulation, fallback: number) {
  const previous = u.history.pop();
  if (previous) Object.assign(u, previous, { scrollStartedMs: u.clockMs });
  else open(u, fallback);
}
function notify(u: UiSimulation, text = 'Aceptada', error = false) {
  u.notice = { title: error ? 'No disponible' : 'Operacion', text: text.slice(0, 17), kind: error ? 1 : 0, ms: 3000 };
  u.noticeStartedMs = u.clockMs;
}
const poseReady = (u: UiSimulation) => !!(u.values[S.AFR2_A_HALL_WIRED] && u.cpr && u.values[S.AFR2_A_GEOM_READY]);
export function simulationUnavailable(u: UiSimulation, item: FirmwareMenuItem): string | null {
  const action = item.action;
  if (([S.AFR2_A_MOVE, S.AFR2_A_LINE_START, S.AFR2_A_GOTO_START] as number[]).includes(action)) {
    if (!u.values[S.AFR2_A_MOTOR_READY]) return 'Habilitar motores';
    if (!poseReady(u)) return 'Config. geometria';
    if (action === S.AFR2_A_LINE_START && !u.lineCalibrated) return 'Calibrar linea';
  }
  if (action === S.AFR2_A_HALL_BEGIN && !u.values[S.AFR2_A_HALL_WIRED]) return 'Conectar Hall';
  if (action === S.AFR2_A_HALL_END && !u.hallMeasuring) return 'Iniciar conteo';
  if (action === S.AFR2_A_HALL_SAVE && (u.hallMeasuring || !u.hallCount)) return 'Finalizar vuelta';
  if (action === S.AFR2_A_WIFI_NETWORK && !u.scanned) return 'Buscar redes';
  if (action === S.AFR2_A_WIFI_CONNECT && !u.wifiDraftReady) return 'Falta borrador';
  return null;
}
function invoke(u: UiSimulation, item: FirmwareMenuItem, value: number) {
  const action = item.action;
  const setting = action === S.AFR2_A_SPEED || action === S.AFR2_A_LINE_SPEED || action === S.AFR2_A_LINE_KP || action === S.AFR2_A_TEST_MS ||
    (action >= S.AFR2_A_LINE_KIND && action <= S.AFR2_A_ENC_INVERT) || action === S.AFR2_A_CONTRAST || action === S.AFR2_A_ROTATION;
  if (setting && (!u.stopped || u.hallMeasuring)) { notify(u, 'Servicio ocupado', true); return; }
  if (action === S.AFR2_A_RADAR_MIN && value >= u.values[S.AFR2_A_RADAR_MAX] || action === S.AFR2_A_RADAR_MAX && value <= u.values[S.AFR2_A_RADAR_MIN]) {
    notify(u, 'Dato incorrecto', true); return;
  }
  if (item.step) u.values[action] = value;
  switch (action) {
    case S.AFR2_A_STOP: u.stopped = true; u.mode = 0; u.motionUntil = 0; break;
    case S.AFR2_A_MOVE: case S.AFR2_A_WHEEL_START: case S.AFR2_A_LINE_START: case S.AFR2_A_GOTO_START:
      if (u.radar) { notify(u, 'Servicio ocupado', true); return; }
      u.stopped = false; u.mode = action === S.AFR2_A_LINE_START ? 2 : action === S.AFR2_A_GOTO_START ? 3 : 1;
      u.motionUntil = u.clockMs + (action === S.AFR2_A_WHEEL_START ? u.values[S.AFR2_A_TEST_MS] : action === S.AFR2_A_MOVE ? 500 : 5000); break;
    case S.AFR2_A_HALL_BEGIN: u.hallMeasuring = true; u.hallCount = 0; break;
    case S.AFR2_A_HALL_END: u.hallMeasuring = false; u.hallCount = 360; break;
    case S.AFR2_A_HALL_SAVE: u.cpr = u.hallCount; u.prefsLoaded = true; break;
    case S.AFR2_A_HALL_CANCEL: u.hallMeasuring = false; u.hallCount = 0; break;
    case S.AFR2_A_SAVE: u.saved = { ...u.values }; u.prefsLoaded = true; break;
    case S.AFR2_A_DISCARD: u.values = { ...u.saved }; break;
    case S.AFR2_A_SPEED: u.values[S.AFR2_A_LINE_SPEED] = Math.min(value, u.values[S.AFR2_A_LINE_SPEED]); break;
    case S.AFR2_A_LINE_KIND: case S.AFR2_A_LINE_INVERT: case S.AFR2_A_LINE_THRESHOLD: u.lineCalibrated = false; break;
    case S.AFR2_A_LINE_CONFIRM:
      if (!u.values[S.AFR2_A_LINE_KIND]) { notify(u, 'Ver requisitos', true); return; }
      u.lineCalibrated = true; break;
    case S.AFR2_A_WHEEL_MM: case S.AFR2_A_RADIUS_MM: u.values[S.AFR2_A_GEOM_READY] = 0; break;
    case S.AFR2_A_WIFI_SCAN: u.scanned = true; break;
    case S.AFR2_A_WIFI_NETWORK: u.draftSsid = sampleNetworks[value]; u.wifiDraftReady = false; break;
    case S.AFR2_A_WIFI_WEB: u.draftSsid = 'Red demo Web'; u.wifiDraftReady = true; break;
    case S.AFR2_A_WIFI_CONNECT: u.connected = true; break;
    case S.AFR2_A_WIFI_CANCEL: u.draftSsid = ''; u.password = ''; u.wifiDraftReady = false; break;
    case S.AFR2_A_WIFI_SET_MODE: u.values[S.AFR2_A_WIFI_MODE] = value; u.wifiDraftReady = false; break;
    case S.AFR2_A_WIFI_MODE: u.wifiDraftReady = false; break;
    case S.AFR2_A_SERVO_CENTER: u.values[S.AFR2_A_SERVO_ANGLE] = 90; u.servoEnabled = true; u.radar = false; break;
    case S.AFR2_A_SERVO_APPLY: u.values[S.AFR2_A_SERVO_ANGLE] = Math.round((u.values[S.AFR2_A_SERVO_US] - 1000) * 180 / 1000); u.servoEnabled = true; break;
    case S.AFR2_A_SERVO_OFF: case S.AFR2_A_RADAR_STOP: u.servoEnabled = false; u.radar = false; break;
    case S.AFR2_A_RADAR_START: u.radar = true; u.servoEnabled = true; break;
    case S.AFR2_A_REBOOT_MEGA: {
      const restarted = createUiSimulation(u.scenario);
      Object.assign(u, restarted, { values: { ...u.saved }, saved: { ...u.saved }, clockMs: u.clockMs }); break;
    }
    case S.AFR2_A_REBOOT_ESP: u.connected = false; break;
  }
  u.feedback = `${item.label}: respuesta simulada. Los servicios usan ejemplos locales.`;
  notify(u);
}

export function reduceUiSimulation(previous: UiSimulation, event: SimulationEvent): UiSimulation {
  const u: UiSimulation = { ...previous, history: [...previous.history], values: { ...previous.values } };
  if (event.type === 'inspect') {
    const code = Number.parseInt(event.code.slice(2), 16);
    if (code > 0x040000 && code <= 0x040000 + S.AFR2_NET_EVENT_MAX) {
      u.notice = { title: 'Red simulada', text: '192.0.2.2', kind: 16 + code - 0x040000, ms: 3000 };
      u.noticeStartedMs = u.clockMs;
    } else {
      const node = L.nodes.findIndex(row => Number(row[5]) === code);
      if (node < 0) return previous;
      u.notice = null; u.keyboard = false; u.ssid = ''; u.password = ''; u.length = 0; u.editing = null;
      u.history = []; open(u, node);
      // Direct inspection starts with the static parent as return fallback.
      u.history = [];
    }
    u.feedback = 'Inspeccion directa con datos ficticios. Los controles conservan la navegacion del encoder.';
    return u;
  }
  if (event.type === 'tick') {
    u.clockMs += event.ms;
    if (u.notice && u.clockMs - u.noticeStartedMs >= u.notice.ms) u.notice = null;
    if (u.motionUntil && u.clockMs >= u.motionUntil) { u.stopped = true; u.mode = 0; u.motionUntil = 0; }
    return u;
  }
  const rotation = event.type === 'rotate' ? event.delta : 0;
  const long = event.type === 'press' && event.kind === 'long';
  const click = event.type === 'press' && event.kind === 'short';
  u.turns += rotation; u.clicks += Number(click); u.holds += Number(long);
  if (long) {
    u.notice = null; u.keyboard = false; u.ssid = ''; u.password = ''; u.length = 0; u.edit = false; u.editing = null;
    u.hallMeasuring = false; u.hallCount = 0; u.stopped = true; u.mode = 0; u.motionUntil = 0;
    open(u, S.AFR2_N_MENU); u.feedback = 'Menu principal. El borrador de edicion se descarta; soltar SW no confirma.'; return u;
  }
  if (u.notice) { if (click) u.notice = null; return u; }
  if (u.keyboard) {
    if (rotation) u.key = wrap(u.key + rotation, 97);
    if (click) {
      let text = u.keyboardField ? u.password : u.ssid;
      if (u.key < 95 && text.length < (u.keyboardField ? 63 : 32)) text += String.fromCharCode(32 + u.key);
      else if (u.key === 95) text = text.slice(0, -1);
      else if (u.key === 96) {
        if (!u.keyboardField) {
          if (!text.length) { notify(u, 'Dato incorrecto', true); return u; }
          u.keyboardField = true; u.length = 0; u.key = 0; return u;
        }
        if (text.length && text.length < 8) { notify(u, 'Dato incorrecto', true); return u; }
        u.draftSsid = u.ssid; u.wifiDraftReady = true; u.keyboard = false; u.ssid = ''; u.password = '';
        open(u, S.AFR2_N_WIFI_REVIEW); notify(u); return u;
      }
      if (u.keyboardField) u.password = text; else u.ssid = text;
      u.length = text.length;
    }
    return u;
  }
  if (u.edit && u.editing) {
    if (rotation) u.draft = limit(u.draft + rotation * u.editing.step, u.editing.min, u.editing.max);
    if (click) { invoke(u, u.editing, u.draft); u.edit = false; u.editing = null; }
    return u;
  }
  const sensorViews: number[] = [S.AFR2_N_RADAR_VIEW, S.AFR2_N_US_VIEW, S.AFR2_N_LINE_VIEW, S.AFR2_N_IR_VALUES, S.AFR2_N_SERVO_ANGLE];
  if (sensorViews.includes(u.node)) {
    if (click) back(u, Number(L.nodes[u.node][1]));
    else if (rotation) {
      if (u.node === S.AFR2_N_SERVO_ANGLE) { u.values[S.AFR2_A_SERVO_ANGLE] = limit(u.values[S.AFR2_A_SERVO_ANGLE] + rotation, 0, 180); u.servoEnabled = true; }
      else u.node = u.node === S.AFR2_N_RADAR_VIEW ? S.AFR2_N_US_VIEW : u.node === S.AFR2_N_US_VIEW ? S.AFR2_N_RADAR_VIEW : u.node === S.AFR2_N_LINE_VIEW ? S.AFR2_N_IR_VALUES : S.AFR2_N_LINE_VIEW;
      u.scrollStartedMs = u.clockMs;
    }
    return u;
  }
  if (rotation) {
    u.selection = wrap(u.selection + rotation, Number(L.nodes[u.node][3]));
    const visible = L.nodes[u.node][2] ? 1 : 3;
    if (u.selection < u.firstVisible) u.firstVisible = u.selection;
    if (u.selection >= u.firstVisible + visible) u.firstVisible = u.selection - visible + 1;
    u.scrollStartedMs = u.clockMs;
  }
  if (click) {
    const item = simulationItems(u)[u.selection], reason = simulationUnavailable(u, item);
    if (reason) { u.feedback = `Accion bloqueada en el escenario simulado: ${reason}.`; notify(u, reason, true); return u; }
    if (!item.action) {
      if (item.label.startsWith('Volver')) {
        if (([S.AFR2_N_WHEEL, S.AFR2_N_MOVE, S.AFR2_N_HALL_TEST] as number[]).includes(u.node)) { u.stopped = true; u.mode = 0; u.hallMeasuring = false; u.hallCount = 0; }
        back(u, item.target);
      } else open(u, item.target);
      u.feedback = 'Gira para elegir una opcion y toca SW para confirmar.';
    } else if (item.action === S.AFR2_A_KEYBOARD) {
      u.keyboard = true; u.keyboardField = false; u.ssid = u.draftSsid; u.password = ''; u.length = u.ssid.length; u.key = 0;
      u.feedback = 'Teclado OLED: gira para elegir caracter, < borra y OK pasa a clave o termina. Usa solo datos ficticios.';
    } else if (item.step) {
      u.edit = true; u.editing = item; u.editingLabel = item.label; u.draft = u.values[item.action] ?? item.min;
      u.feedback = 'Gira para cambiar el borrador; SW corto aplica. SW largo lo descarta y abre el menu.';
    } else invoke(u, item, item.arg);
  }
  return u;
}

export function simulationSnapshot(u: UiSimulation): FirmwareSnapshot {
  const v = u.values, ready = u.scenario === 'ready', hall = !!v[S.AFR2_A_HALL_WIRED], pose = poseReady(u);
  const angle = u.radar ? Math.round(v[S.AFR2_A_RADAR_MIN] + (v[S.AFR2_A_RADAR_MAX] - v[S.AFR2_A_RADAR_MIN]) * (1 - Math.cos(u.clockMs / 3000)) / 2) : v[S.AFR2_A_SERVO_ANGLE];
  const pulse = u.servoEnabled ? 1000 + Math.round(angle * 1000 / 180) : 0;
  const pwm = Array.from({ length: 4 }, (_, i) => !u.stopped && i === v[S.AFR2_A_WHEEL] ? v[S.AFR2_A_TEST_PWM] : 0);
  const lines: Record<number, string[]> = {
    [S.AFR2_N_HOME]: [`Modo:${u.mode} Ctrl:0`, `Salidas OFF:${Number(u.stopped)}`, 'SW largo: menu'],
    [S.AFR2_N_ROBOT_VIEW]: [`Modo:${u.mode} Ctrl:0`, `Salidas OFF:${Number(u.stopped)}`, 'SW largo: menu'],
    [S.AFR2_N_LINK_VIEW]: ['UART:1 Ext:1', 'Origen:0 Web:0', 'XOR mal:0'],
    [S.AFR2_N_ALERT_VIEW]: ['Fallo:0 Reset:0', 'OLED err:0', 'RX gaps:0'],
    [S.AFR2_N_DIAG]: ['Fallo:0 Reset:0', 'OLED err:0', 'RX gaps:0'],
    [S.AFR2_N_DISPLAY]: ['OLED listo:1', 'Errores:0', 'TWI IRQ 100kHz'],
    [S.AFR2_N_ENCODER_VIEW]: [`Giros:${u.turns}`, `Clics:${u.clicks}`, `Largos:${u.holds}`],
    [S.AFR2_N_HALL_VIEW]: hall ? [`Rueda:${v[S.AFR2_A_WHEEL]} C:${u.hallCount}`, `Vuelta:${u.hallCount} E:0`, `CPR:${u.cpr}`] : ['Hall sin conectar', 'A8..A15 propuesto', 'Captura bloqueada'],
    [S.AFR2_N_WHEEL_VIEW]: [`FL:${pwm[0]} FR:${pwm[1]}`, `RL:${pwm[2]} RR:${pwm[3]}`, `OFF:${Number(u.stopped)}`],
    [S.AFR2_N_LINE_REQ]: [`Linea valida:${Number(u.lineCalibrated)}`, `Hall:${Number(hall)} Geom:${v[S.AFR2_A_GEOM_READY]}`, `Motor habilit:${v[S.AFR2_A_MOTOR_READY]}`],
    [S.AFR2_N_GOTO_REVIEW]: [`X:${v[S.AFR2_A_GOTO_X]} mm`, `Y:${v[S.AFR2_A_GOTO_Y]} mm`, `V max:${v[S.AFR2_A_SPEED]} mm/s`],
    [S.AFR2_N_GOTO_RESULT]: [`Estado GOTO:${u.mode === 3 ? 1 : 0}`, `Pose valida:${Number(pose)}`, `Salidas OFF:${Number(u.stopped)}`],
    [S.AFR2_N_POSE]: pose ? ['X:0 Y:0', 'Yaw:0 mrad', 'Estimacion ruedas'] : ['Sin odometria', 'Conectar Hall', 'Calibrar geometria'],
    [S.AFR2_N_WIFI_VIEW]: [`Modo:${v[S.AFR2_A_WIFI_MODE]} STA:${Number(u.connected)}`, u.connected ? u.draftSsid : '', u.connected ? '192.0.2.2' : '192.0.2.1'],
    [S.AFR2_N_WIFI_REVIEW]: [u.draftSsid, `Modo:${v[S.AFR2_A_WIFI_MODE]} Listo:${Number(u.wifiDraftReady)}`, 'Giro: opciones'],
    [S.AFR2_N_VERSIONS]: ['Mega demo UI', 'ESP:demo', 'Web:demo'],
    [S.AFR2_N_CAPS]: ['UI/TWI/UART/STOP', `Linea:${Number(u.lineCalibrated)} Hall:${Number(hall)}`, `GOTO req:${Number(pose)}`],
    [S.AFR2_N_PREFS_VIEW]: [`EEPROM valida:${Number(u.prefsLoaded)}`, `Guardado estado:${u.prefsLoaded ? 2 : 0}`, 'CRC/version 1'],
  };
  const items = simulationItems(u).slice(u.firstVisible, u.firstVisible + 3);
  return {
    lines: (lines[u.node] ?? ['', '', '']).map(line => line.slice(0, 18)),
    labels: u.node === S.AFR2_N_WIFI_NETWORKS ? items.map(item => item.action === S.AFR2_A_WIFI_NETWORK && u.scanned ? item.label : '') : ['', '', ''],
    disabled: items.map(item => Number(!!simulationUnavailable(u, item))),
    sequenceState: u.clockMs - u.scrollStartedMs >= 9000 ? 4 : 3, sequenceCount: 3,
    sequenceIndex: Math.min(2, Math.floor((u.clockMs - u.scrollStartedMs) / 3000)),
    sequenceKind: Math.min(2, Math.floor((u.clockMs - u.scrollStartedMs) / 3000)),
    sequenceDurationMs: 3000, sequenceElapsedMs: (u.clockMs - u.scrollStartedMs) % 3000,
    networkValid: true, networkSta: u.connected && v[S.AFR2_A_WIFI_MODE] !== 2, networkAp: v[S.AFR2_A_WIFI_MODE] !== 1,
    networkSsid: u.draftSsid, networkApSsid: 'AFR2-DEMO', networkPassword: 'demo1234', networkIp: '192.0.2.2', networkApIp: '192.0.2.1',
    irRaw: ready ? [180, 320, 850, 300, 160].map(raw => v[S.AFR2_A_LINE_KIND] === 1 ? Number(raw > 512) : raw) : [],
    irKind: v[S.AFR2_A_LINE_KIND], irValid: ready && v[S.AFR2_A_LINE_KIND] !== 0, lineValid: ready && u.lineCalibrated,
    sensor: { nowMs: u.clockMs, angle, pulseUs: pulse, targetUs: pulse, enabled: u.servoEnabled, running: u.radar,
      latest: { id: ready ? 1 : 0, ms: u.clockMs, distanceMm: 720, error: 0 },
      bins: Array.from({ length: 37 }, (_, i) => ({ id: ready && u.radar ? 1 : 0, distanceMm: 700 + (i % 7) * 140, error: 0 })),
    },
  };
}
