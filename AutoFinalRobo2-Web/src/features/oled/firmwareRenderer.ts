import { componentHeader,componentPanel,componentProgress,componentMetric } from './oledComponents';
import { renderQrScreen,clearQrScreenCache } from './qrScreen';
import { renderMotorSelector } from './motorSelector';
import { ENCODER_LONG } from './encoderLongIcon';
/* AFR2-20261030-01: AutoMicro dashboard adaptation and linked icons; prior authors retained. */
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
/** AFR2-20261024-09. Browser copy of current AVR pure layout library.
 * Ported from afr2_{screens,network_screen,sequence_screen,icons}.c.
 * C layout source hashes are pinned in firmwareRenderer.sources.json; build
 * refuses a changed layout until this port is reviewed. Resources are copied.
 * Requires the same PREPARED snapshot, UI state, clock and previous framebuffer
 * as AVR. Does not fabricate telemetry or predict remote navigation/actions.
 */
import { renderSensorScreen,type FirmwareSensorSnapshot } from './firmwareSensorRenderer';
import { REFORM_MENUS, REFORM_ROWS } from './reformCatalogue';
import { WIFI_MENUS, WIFI_ROWS } from './wifiCatalogue';
import { gestureHint } from './firmwareHints';
import { ENCODER_TURN, WIFI_LOCK } from './reformResources';
import { TOUCH_CAR, TOUCH_SIN, TOUCH_CAR_WIDTH, TOUCH_CAR_HEIGHT } from './touchBitmap';
import { Afr2OledBuffer } from './oledRasterizer';
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';
// New symbols are emitted by sync-oled-library with the appended AVR nodes.
const S = L.symbols as typeof L.symbols & { AFR2_N_HALL_PICK:number; AFR2_N_HALL_COUNT:number; AFR2_N_SAVE_CONFIRM:number; AFR2_N_BATTERY:number; AFR2_N_BATTERY_VIEW:number; AFR2_A_BATTERY_MEASURE:number };
const trunc = Math.trunc;
const elapsed = (now: number, then: number) => (now - then) >>> 0;
export interface FirmwareMenuItem { label: string; target: number; action: number; arg: number; min: number; max: number; step: number }
export interface FirmwareUiState {
  confirmWait?: boolean;
  editingAction?:number; testShort?:boolean; testLong?:boolean; testBack?:boolean;
  quickPending?:boolean;quickExpected?:boolean;
  touch?: { direction: number; heading: number; startedMs: number; full: boolean };
  node: number; selection: number; firstVisible: number;
  clockMs: number; noticeStartedMs: number; scrollStartedMs: number;
  redraw: boolean; scrollRedraw: boolean;
  keyboard: boolean; keyboardField: boolean; key: number; length: number; ssid: string;
  edit: boolean; editingLabel: string; draft: number;
  notice: { title: string; text: string; ms: number; kind: number } | null;
}
export interface FirmwareSnapshot {
  /** AFR2-20261030-01: prepared fresh volts, no inferred charge. */
  batteryValid?:boolean; batteryMv?:number;
  networkServer?:boolean;qrAllowed?:boolean;
  networkCount?:number; wifiConnected?:boolean|null; rowSecurity?:readonly number[];
  networkRssi?:number; networkRssiValid?:boolean;
  configDirty?:boolean; actionStates?:Record<number,boolean>; powerPercent?:number; powerChangedMs?:number; powerKnown?:boolean;
  hallCpr?: readonly number[]; hallPending?: number;
  radarRange?:number;radarTrace?:boolean;quickFlags?:number;
  sensor?: FirmwareSensorSnapshot; irRaw?: readonly number[]; irKind?:number; irValid?:boolean; lineValid?:boolean;
  lines: readonly string[]; labels: readonly string[]; disabled: readonly number[];
  sequenceState: number; sequenceCount: number; sequenceIndex: number; sequenceKind: number;
  sequenceDurationMs: number; sequenceElapsedMs: number;
  networkValid: boolean; networkSta: boolean; networkAp: boolean;
  networkSsid: string; networkApSsid: string; networkPassword: string; networkIp: string; networkApIp: string;
}
export const QUICK_MODES_NODE = 61;
export function firmwareNode(node:number):readonly (string|number)[] {
  const virtual:Record<number,readonly (string|number)[]>={61:['Elegir modo',S.AFR2_N_QUICK,0,3,0x802c,0x3003d],62:['Prueba encoder',S.AFR2_N_TESTS,1,1,89,0x3003e],63:['Prueba ESP',S.AFR2_N_TESTS,0,3,0,0x3003f],64:['Potencia manual',S.AFR2_N_CONTROL,1,1,26,0x30040],65:['Datos de red',32,1,1,138,0x30041],66:['Buscar redes',31,0,3,0,0x30042],67:['Cambiar modo',31,1,3,138,0x30043],70:['Elegir un motor',S.AFR2_N_TESTS,1,4,0,0x30044]};
  let n:readonly (string|number)[]=L.nodes[node]??virtual[node]??L.nodes[0];
  if(WIFI_MENUS[node]){const [first,count]=WIFI_MENUS[node];const title=node===32?'Estado de red':node===34?'Credenciales':node===35?'Configurar':n[0];return [title,node===34?33:node===35?32:n[1],0,node===31?count+2:count,0x2000+first,n[5]];}
  if(node===33)n=['Redes encontradas',66,n[2],n[3],n[4],n[5]];
  if(node===45)n=['Estado conexion',32,n[2],n[3],n[4],n[5]];
  if(REFORM_MENUS[node]){const [first,count]=REFORM_MENUS[node];n=[n[0],n[1],0,count,0x4000+first,n[5]];}
  if(node===S.AFR2_N_SERVO)n=['Servo',n[1],n[2],n[3],n[4],n[5]];
  if(node===S.AFR2_N_GEOMETRY)n=['Geometria nominal',S.AFR2_N_STATE,1,1,122,n[5]];
  return n;
}
export function firmwareNodeCount(node:number,dirty=false,snapshot?:FirmwareSnapshot):number {
  if(node===70 || node===S.AFR2_N_HALL_PICK)return 4;
  if(node===34)return 4;
  if(node===33)return (snapshot?.networkCount??0)+1;
  if(node===32)return snapshot?.wifiConnected===false?6:snapshot?.wifiConnected===true?5:4;
  if(node===35 && typeof snapshot?.wifiConnected!=="boolean")return 2;
  const count=node===S.AFR2_N_MENU?7:node===S.AFR2_N_DIAG?3:node===S.AFR2_N_QUICK?7:Number(firmwareNode(node)[3]);
  return count-(!dirty && (node===S.AFR2_N_PREFS || node===S.AFR2_N_CALIB)?1:0);
}
export function firmwareMenuItem(node: number, index: number,snapshot?:FirmwareSnapshot): FirmwareMenuItem {
  const n = firmwareNode(node);
  index = Math.min(index, firmwareNodeCount(node,snapshot?.configDirty,snapshot) - 1);
  if(node===33 && index>=(snapshot?.networkCount??0))return {label:'Volver',target:66,action:0,arg:0,min:0,max:0,step:0};
  if(node===34){if(index===0)return {label:"Traer credenciales ESP",target:255,action:93,arg:0,min:0,max:0,step:0};index--;}
  if(node===31 && index>=4){if(index<6)return {label:index===4?'QR conectar AP':'QR abrir Web',target:index===4?68:69,action:0,arg:0,min:0,max:0,step:0};index-=2;}
  if(WIFI_MENUS[node]){
    if(node===32){if(typeof snapshot?.wifiConnected!=="boolean" && index>=3)index+=2;else if(snapshot?.wifiConnected===true && index>=4)index++;}
    if(node===35 && typeof snapshot?.wifiConnected!=="boolean" && index>=1)index++;
    const row=WIFI_ROWS[WIFI_MENUS[node][0]+index];let action=Number(row[2]),label=String(row[0]);
    if(action===47 && snapshot?.wifiConnected===true){action=91;label='Desconectar';}
    return {label,target:Number(row[1]),action,arg:Number(row[3]),min:Number(row[4]),max:Number(row[5]),step:Number(row[6])};
  }
  if(REFORM_MENUS[node]){
    if(!snapshot?.configDirty && ((node===S.AFR2_N_PREFS && index>=3) || (node===S.AFR2_N_CALIB && index>=4)))index++;
    const row=REFORM_ROWS[REFORM_MENUS[node][0]+index];
    const action=Number(row[2]);const toggle=[74,77,86,87].includes(action);
    return {label:toggle?(snapshot?.actionStates?.[action]?'Detener':'Iniciar'):String(row[0]),target:Number(row[1]),action,arg:Number(row[3]),min:Number(row[4]),max:Number(row[5]),step:Number(row[6])};
  }
  if(node===S.AFR2_N_QUICK && index===6)return {label:'Seleccion de modo',target:QUICK_MODES_NODE,action:0,arg:0,min:0,max:0,step:0};
  if(node===QUICK_MODES_NODE)return index===2 ? {label:'Volver',target:S.AFR2_N_QUICK,action:0,arg:0,min:0,max:0,step:0} : {label:index?'Manual':'Reposo',target:255,action:80,arg:index,min:0,max:0,step:0};
  const mapped = node===S.AFR2_N_MENU ? L.items[[1,7,5,4,6,9,10][index]]
    : node===S.AFR2_N_STATE && index>=3 ? L.items[[2,3,19][index-3]]
    : node===S.AFR2_N_DISPLAY_MENU && index===4 ? L.extra[7]
    : node===S.AFR2_N_SYSTEM ? (index===2 ? L.extra[26] : index===3 ? L.extra[0] : L.items[[166,168,0,0,169,167,8,171][index]])
    : node===S.AFR2_N_DIAG ? L.items[[18,170,179][index]] : undefined;
  if(mapped)return {label:String(mapped[0]),target:Number(mapped[1]),action:Number(mapped[2]),arg:Number(mapped[3]),min:Number(mapped[4]),max:Number(mapped[5]),step:Number(mapped[6])};
  // AFR2-20261029-20: hide legacy END menu item, retain AVR resource offsets.
  if(node===S.AFR2_N_HALL_TEST && index>=3)index++;
  let row: readonly (string | number)[];
  const first = Number(n[4]);
  const extra: readonly (readonly (string | number)[])[] = L.extra;
  if (first & 0x4000) row=REFORM_ROWS[(first & 0x3fff)+index];
  else if (first & 0x8000) row = extra[(first & 0x7fff) + index];
  else if (node === S.AFR2_N_SYSTEM && index === 5) row = extra[0];
  else if (node === S.AFR2_N_SYSTEM && index === 6) row = extra[26];
  else if (node === S.AFR2_N_WIFI && (index === 1 || index === 2)) row = L.extra[index];
  else {
    if (node === S.AFR2_N_SYSTEM && index > 6) index -= 2;
    if (node === S.AFR2_N_WIFI && index > 2) index -= 2;
    row = L.items[first + index];
  }
  return { label: Number(row[2])===S.AFR2_A_MOVE && Number(row[3])===8?'Girar CCW':Number(row[2])===S.AFR2_A_MOVE && Number(row[3])===9?'Girar CW':String(row[0])==='Estado STA/AP/IP'?'Estado de red':String(row[0])==='Configuracion y pruebas'?'Menu principal':String(row[0])==='Control del servo' || String(row[0])==='Servo ultrasonido'?'Servo':String(row[0]), target: Number(row[1]), action: Number(row[2]), arg: Number(row[3]), min: Number(row[4]), max: Number(row[5]), step: Number(row[6]) };
}
export function firmwareTextOffset(text: string, width: number, time: number): number {
  const pixels = text.length * 7;
  if (pixels <= width) return 0;
  const end = pixels - width, phase = (time >>> 0) % (1600 + end * 40);
  return phase <= 800 ? 0 : Math.min(end, trunc((phase - 800) / 40));
}
function icon(o: Afr2OledBuffer, id: number, x: number, y: number, size: number) {
  const image = id===17?ENCODER_LONG:id===25?WIFI_LOCK:id===24?ENCODER_TURN:L.icons[id];
  if (!image || !size) return;
  const largest = Math.max(image.width, image.height), stride = trunc((image.width + 7) / 8);
  const width = trunc(image.width * size / largest), height = trunc(image.height * size / largest);
  x += trunc((size - width) / 2); y += trunc((size - height) / 2);
  for (let py = 0; py < height; py++) for (let px = 0; px < width; px++) {
    const sx = trunc(px * image.width / width), sy = trunc(py * image.height / height);
    if (image.bytes[sy * stride + trunc(sx / 8)] & (1 << (sx & 7))) o.setPixel(x + px, y + py);
  }
}
/** AFR2-20261030-01. Literal port of render_dashboard; same XBM and clipping. */
// AFR2-20261030-02: identical independent ping-pong lanes and partial bands.
function dashboard(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot,partial:boolean) {
  void partial;o.clear();componentHeader(o,S.AFR2_ICON_CONFIG,'AutoFinal');
  componentPanel(o,0,17,76,32);componentPanel(o,80,17,48,32);
  const mv=s.batteryMv??0,battery=s.batteryValid?S.AFR2_ICON_BATTERY_EMPTY+(mv===0?0:mv<6000?1:mv<7000?2:mv<8000?3:4):S.AFR2_ICON_BATTERY_UNKNOWN;
  icon(o,battery,5,19,12);o.drawText(21,20,'Bateria');
  const voltage=s.batteryValid?`${trunc(mv/1000)}.${trunc(mv%1000/100)}`:'--';
  componentMetric(o,7,32,voltage);o.drawText(61,35,'V');
  const mode=(s.lines[0]||'--').replace(/^Modo: /,'');o.textWindow(84,124,21,mode,firmwareTextOffset(mode,40,elapsed(u.clockMs,u.scrollStartedMs)));
  const connected=s.networkValid && (s.networkSta || s.networkAp);
  icon(o,!connected?S.AFR2_ICON_WIFI_OFF:s.networkSta?S.AFR2_ICON_WIFI:S.AFR2_ICON_AP,84,34,12);o.drawText(100,36,connected?s.networkSta?'STA':'AP':'--');
  gestureHint(o,0,52,false,'Menu');gestureHint(o,62,52,true,'Rapidas');
}
function menuIcon(node: number, target: number, action: number, label: string): number {
  if(action===45)return S.AFR2_ICON_REFRESH;
  if (label.startsWith('Volver')) return S.AFR2_ICON_BACK;
  if (action === S.AFR2_A_REBOOT_MEGA || action === S.AFR2_A_REBOOT_ESP || target === S.AFR2_N_REBOOT) return S.AFR2_ICON_REFRESH;
  if (([S.AFR2_N_DISTANCE_SERVO,S.AFR2_N_RADAR_VIEW,S.AFR2_N_RADAR_CONFIG] as number[]).includes(target) || (action>=S.AFR2_A_RADAR_START && action<=S.AFR2_A_US_INTERVAL))return S.AFR2_ICON_RADAR;
  if(target===S.AFR2_N_SERVO || target===S.AFR2_N_SERVO_ANGLE || action===S.AFR2_A_SERVO_CENTER || action===S.AFR2_A_SERVO_OFF)return S.AFR2_ICON_SERVO;
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
  if(u.node===S.AFR2_N_NETWORK){o.clear();o.drawText(0,0,u.selection?'<2/2 Conexion':'1/2> Conexion');for(let i=0;i<3;i++)o.drawText(0,14+i*12,s.lines[i]??'');gestureHint(o,0,53,false,'Volver');gestureHint(o,66,53,true,'Volver');return;}
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
  icon(o, S.AFR2_ICON_BACK, 0, 51, 12); o.drawText(14, 53, 'Volver');
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
  if (kind !== S.AFR2_NET_AP_READY){gestureHint(o,0,53,false,'Cerrar');gestureHint(o,66,53,true,'Cerrar');}
}
function sequence(o:Afr2OledBuffer,s:FirmwareSnapshot,clock:number){
  const count=s.sequenceCount,index=s.sequenceIndex;componentHeader(o,S.AFR2_ICON_MOTOR,'Secuencia',index<count?index+1:count,count);
  const kind=s.sequenceState===4?S.AFR2_SEQ_FINISH:s.sequenceKind<L.sequenceNames.length?s.sequenceKind:S.AFR2_SEQ_WHEELS;
  for(let y=0;y<8;y++)for(let x=0;x<8;x++)if(L.sequenceIcons[kind][y]&(0x80>>x))o.setPixel(2+x,18+y);
  const name=s.sequenceState===5?'Cancelada':s.sequenceState===6?'Error de ejecucion':L.sequenceNames[kind];o.textWindow(14,128,17,name,0);
  const percent=s.sequenceState===4?100:s.sequenceDurationMs?Math.min(100,Math.trunc(s.sequenceElapsedMs*100/s.sequenceDurationMs)):0;
  componentProgress(o,30,s.sequenceDurationMs>0 || s.sequenceState===4,percent,clock);
  o.drawText(0,40,`Tiempo ${trunc(s.sequenceDurationMs/1000)}.${String(trunc((s.sequenceDurationMs%1000)/10)).padStart(2,'0')} s`.slice(0,17));gestureHint(o,94,52,true,'Volver');
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
  gestureHint(o,0,53,true,'Cancelar');
}
/** AFR2-20261029-20: same pure wizard/status/confirmation layout as AVR.
 * Values are prepared measurements; missing CPR never becomes a fake zero. */
function hall(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot){
  if(u.node===S.AFR2_N_HALL_PICK){
    renderMotorSelector(o,u.selection);
  }else if(u.node===S.AFR2_N_HALL_COUNT){
    o.drawText(0,0,'Gire una vuelta');o.drawText(0,13,s.lines[0]??'');o.drawText(0,26,s.lines[1]??'');
    gestureHint(o,0,40,false,'Finalizar');gestureHint(o,0,53,true,'Cancelar');
  }else{
    o.drawText(0,0,'Pulsos / vuelta');
    for(let i=0;i<4;i++){
      const cpr=s.hallCpr?.[i];
      o.drawText(0,12+i*10,`${i+1} ${['FL','FR','RL','RR'][i]}:${cpr?`${cpr}${((s.hallPending??0)&(1<<i))?'*':''}`:'--'}`);
    }
    o.drawText(0,54,'*Nuevo');icon(o,S.AFR2_ICON_BACK,58,51,12);o.drawText(72,53,'Volver');
  }
}
function saveConfirm(o:Afr2OledBuffer,u:FirmwareUiState){
  o.drawText(0,0,'Cambios pendientes');
  if(u.confirmWait){o.drawText(0,26,'Guardando...');return;}
  o.drawText(0,15,'Desea guardarlos?');o.drawText(14,34,'Si');o.drawText(14,49,'No');o.drawRect(0,u.selection?47:32,128,14);
}
/** Corresponds to Afr2Ui_Render, including partial scroll updates. Caller owns
 * revision/redraw flags and must provide a retained buffer for partial draws. */
export function renderTouchScreen(o: Afr2OledBuffer, direction: number, heading: number, intro: boolean, full: boolean): void {
  if (full) {
    o.clear(); o.drawText(18, 0, 'Control tactil');
    if (intro) { o.drawText(36, 26, 'Activado'); o.drawText(8, 53, 'Desde la Web'); }
  }
  if (intro) return;
  // AFR2-20261030-12: literal numeric heading footer, same 15-degree sectors.
  o.fillRect(0,53,128,10,'black');o.drawText(0,53,`Angulo: ${heading*15} grados`);
  o.fillRect(32, 12, 64, 40, 'black');
  const sn = TOUCH_SIN[heading], cs = TOUCH_SIN[(heading + 6) % 24];
  const stride = trunc((TOUCH_CAR_WIDTH + 7) / 8);
  for (let y = -18; y < 18; y++) for (let x = -18; x < 18; x++) {
    const sx = trunc((x * cs - y * sn) / 256) + trunc(TOUCH_CAR_WIDTH / 2), sy = trunc((x * sn + y * cs) / 256) + trunc(TOUCH_CAR_HEIGHT / 2);
    if (sx >= 0 && sx < TOUCH_CAR_WIDTH && sy >= 0 && sy < TOUCH_CAR_HEIGHT && (TOUCH_CAR[sy * stride + trunc(sx / 8)] & (1 << (sx & 7)))) o.setPixel(64 + x, 32 + y);
  }
  if (direction < 8) {
    const d = direction * 3, dx = TOUCH_SIN[d], dy = -TOUCH_SIN[(d + 6) % 24];
    for (let j = 0; j < 3; j++) for (let k = -3; k <= 3; k++) {
      const along = 14 + j * 3 - Math.abs(k), x = 64 + trunc((along * dx - k * dy) / 256), y = 32 + trunc((along * dy + k * dx) / 256);
      if (x >= 32 && x < 96 && y >= 12 && y < 52) o.setPixel(x, y);
    }
  }
}
function quick(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot){
  o.clear();const item=firmwareMenuItem(S.AFR2_N_QUICK,u.selection);
  componentHeader(o,menuIcon(S.AFR2_N_QUICK,item.target,item.action,item.label),['Radar','Servo','Motores','Linea','Rastro','Ajustes','Modo'][u.selection],u.selection+1,7);componentPanel(o,0,16,128,34);
  const center=(y:number,text:string)=>o.drawText(text.length*7<128?trunc((128-text.length*7)/2):0,y,text);
  const on=!!((s.quickFlags??0)&(1<<u.selection));
  if(u.selection===5 || u.selection===6){center(20,u.selection===5?'AJUSTES Y PRUEBAS':'MODO DEL ROBOT');gestureHint(o,28,36,false,'Entrar');}
  else if(s.quickFlags===undefined){center(20,'SIN ESTADO');center(36,'Requiere equipo');}
  else {
    center(20,u.selection===0?(on?'ACTIVO':'DETENIDO'):u.selection===2?(on?'HABILITADOS':'BLOQUEADOS'):u.selection===3?(on?'EN MARCHA':'DETENIDO'):u.selection===4?(on?'VISIBLE':'OCULTO'):(on?'ACTIVADO':'DESACTIVADO'));
    if(u.quickPending)center(36,u.quickExpected?'Activando...':'Desactivando...');else gestureHint(o,28,36,false,u.selection===2?(on?'Bloquear':'Habilitar'):u.selection===4?(on?'Ocultar':'Mostrar'):(on?'Detener':'Activar'));
  }
  o.fillRect(3,33,122,15,'invert');icon(o,24,2,50,13);gestureHint(o,94,52,true,'Volver');
}
function capabilities(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot){
  o.clear();o.drawText(0,0,'Capacidades');o.drawText(87,54,`${u.selection?'<' : ''}${u.selection+1}/6${u.selection<5?'>' : ''}`);icon(o,24,0,48,16);
  for(let i=0;i<3;i++)o.drawText(0,14+i*12,s.lines[i]??'');
}
export function renderPower(o:Afr2OledBuffer,percent:number){
  percent=Math.max(0,Math.min(100,percent));o.clear();o.drawText(0,0,'Velocidad modific.');o.drawText(0,15,`Valor: ${Math.floor((percent*255+50)/100)} /255`);
  componentProgress(o,30,true,percent,0);o.drawText(52,43,`${percent}%`);icon(o,24,0,47,16);
}
export function renderFirmwareScreen(u: FirmwareUiState, o: Afr2OledBuffer, s: FirmwareSnapshot): void {
  if(u.notice || (u.node!==68 && u.node!==69))clearQrScreenCache(o);
  if(u.touch && s.powerKnown && s.powerPercent!==undefined && s.powerChangedMs!==undefined && elapsed(u.clockMs,s.powerChangedMs)<1200){renderPower(o,s.powerPercent);return;}
  if (u.touch) { renderTouchScreen(o, u.touch.direction, u.touch.heading, elapsed(u.clockMs, u.touch.startedMs) < 1000, u.touch.full); return; }
  const n = firmwareNode(u.node), partial = !u.redraw && u.scrollRedraw;
  if(!u.notice && (u.node===68 || u.node===69)){renderQrScreen(o,s,u.node===68);return;}
  if (!partial) o.clear();
  if (u.notice) {
    const a = u.notice, width = Math.min(128, trunc((((elapsed(u.clockMs, u.noticeStartedMs) * 128) >>> 0) / a.ms)) & 0xffff);
    o.fillRect(0, 0, 128, 2, 'black'); o.fillRect(0, 0, width, 2);
    if (a.kind > 16) networkNotice(o, u, s);
    else if (!partial) { icon(o, a.kind ? S.AFR2_ICON_WARNING : S.AFR2_ICON_INFO, 56, 6, 16); o.drawText(0, 25, a.title); o.drawText(0, 39, a.text); gestureHint(o,0,53,false,'Cerrar');gestureHint(o,66,53,true,'Cerrar'); }
  } else if(u.node===S.AFR2_N_HOME)dashboard(o,u,s,!!partial);
  else if(u.node===S.AFR2_N_QUICK)quick(o,u,s);
  else if(u.node===62){
    o.drawText(0,0,'Prueba encoder');gestureHint(o,0,14,'turn','Elegir Volver');gestureHint(o,0,27,false,u.testShort?'Detectado':'Pendiente');gestureHint(o,0,40,true,u.testLong?'Salir':'Pendiente');gestureHint(o,0,53,false,'Volver');if(u.testBack)o.drawRect(0,50,128,14);
  }
  else if(u.node===70)renderMotorSelector(o,u.selection);
  else if(u.node===64){if(s.powerKnown && s.powerPercent!==undefined)renderPower(o,s.powerPercent);else {o.drawText(0,0,'Potencia manual');o.drawText(0,25,'Sin valor vigente');}}
  else if(u.node===67){
    o.drawText(0,0,'Cambiar modo');o.drawText(0,14,'Opciones de modos');o.drawText(3,28,'STA');o.drawText(45,28,'AP');o.drawText(80,28,'AP-STA');o.fillRect(u.selection===0?0:u.selection===1?40:78,40,u.selection===2?49:31,2);gestureHint(o,0,53,false,'Elegir');gestureHint(o,66,53,true,'Volver');
  }
  else if(u.node===65){
    o.drawText(0,0,'Datos de red');if(s.labels[0])o.textWindow(0,128,14,s.labels[0],firmwareTextOffset(s.labels[0],128,elapsed(u.clockMs,u.scrollStartedMs)));else o.drawText(0,14,'Sin datos actuales');if(s.labels[1]){o.textWindow(0,128,28,s.labels[1],firmwareTextOffset(s.labels[1],128,elapsed(u.clockMs,u.scrollStartedMs)));o.textWindow(0,128,40,s.labels[2]??'',firmwareTextOffset(s.labels[2]??'',128,elapsed(u.clockMs,u.scrollStartedMs)));}else o.drawText(0,28,s.lines[1]??'');const secure=(s.rowSecurity?.[0]??0)>=2;if(secure)icon(o,25,0,39,12);o.drawText(secure?14:0,40,s.lines[2]??'');gestureHint(o,0,53,false,'Volver');gestureHint(o,66,53,true,'Volver');
  }
  else if(u.node===S.AFR2_N_ENCODER_VIEW){o.drawText(0,0,'Estado encoder');for(let i=0;i<3;i++){icon(o,i===0?24:i===1?7:17,0,11+13*i,13);o.drawText(16,14+13*i,s.lines[i]??'');}gestureHint(o,0,53,true,'Volver');}
  else if(u.node===S.AFR2_N_CAPS)capabilities(o,u,s);
  else if(u.node===S.AFR2_N_SAVE_CONFIRM)saveConfirm(o,u);
  else if(u.node===S.AFR2_N_HALL_PICK || u.node===S.AFR2_N_HALL_COUNT || u.node===S.AFR2_N_HALL_VIEW)hall(o,u,s);
  else if (u.keyboard) keyboard(o, u);
  else if (u.node === S.AFR2_N_SEQUENCE) sequence(o, s,u.clockMs);
  else if (u.node === S.AFR2_N_NETWORK || u.node === S.AFR2_N_AP_INFO) network(o, u, s);
  else if (([S.AFR2_N_RADAR_VIEW,S.AFR2_N_US_VIEW,S.AFR2_N_LINE_VIEW,S.AFR2_N_IR_VALUES,S.AFR2_N_SERVO_ANGLE] as number[]).includes(u.node))renderSensorScreen(o,u.node,s.sensor,{raw:s.irRaw??[],kind:s.irKind??0,valid:s.irValid??false,lineValid:s.lineValid??false},{rangeIndex:s.radarRange??0,trace:s.radarTrace??true});
  else if(u.edit && u.editingAction===12)renderMotorSelector(o,u.draft);
  else if(u.edit && u.editingAction===83)renderPower(o,u.draft);
  else if (u.edit) { o.drawText(0, 0, u.editingLabel); o.drawText(0, 13, String(u.draft).slice(0, 17));gestureHint(o,0,26,'turn','Ajustar'); gestureHint(o,0,40,false,'Aplicar');gestureHint(o,0,53,true,'Descartar'); }
  else {
    const view = u.node!==S.AFR2_N_DIAG && Number(n[2]) !== 0, count = view ? 1 : 3;
    if (!partial) { if(!view)componentHeader(o,S.AFR2_ICON_CONFIG,u.node===32?(s.lines[0]||'Estado de red'):String(n[0]));else o.drawText(0,0,String(n[0])); if (view) for (let j = 0; j < 3; j++) o.drawText(0, 13 + j * 12, s.lines[j]); }
    for (let row = 0; row < count && u.firstVisible + row < firmwareNodeCount(u.node,s.configDirty,s); row++) {
      const item = firmwareMenuItem(u.node, u.firstVisible + row,s), selected = u.firstVisible + row === u.selection;
      if (partial && !selected) continue;
      const y = view ? 52 : 16 + 16 * row, left = view ? 16 : 23, top = view ? y : y + 3;
      if (!partial) { icon(o, item.label.startsWith('Volver')?S.AFR2_ICON_ENCODER_LONG:menuIcon(u.node, item.target, item.action, item.label), view ? 0 : 3, view ? 51 : y, view ? 12 : 16);  if(!(u.node===33 && (s.rowSecurity?.[row]??0)>=2))o.drawText(120, top, selected ? s.disabled[row] ? '!' : item.label.startsWith('Volver') ? ' ' : '>' : s.disabled[row] ? '!' : ' '); }
      const right=u.node===33 && (s.rowSecurity?.[row]??0)>=2?108:118;if(right===108)icon(o,25,111,top,10);
      const label = s.labels[row] || item.label;
      if(item.label.startsWith('Volver')){if(!partial)icon(o,S.AFR2_ICON_BACK,left,top-1,13);}
      else {o.fillRect(left,top,right-left,10,partial && selected && !view?'white':'black');o.textWindow(left,right,top,label,selected?firmwareTextOffset(label,right-left,elapsed(u.clockMs,u.scrollStartedMs)):0,partial && selected && !view?'black':'white');}
      if(!partial && selected && !view)o.fillRect(0,y,128,16,'invert');
    }
  }
}
