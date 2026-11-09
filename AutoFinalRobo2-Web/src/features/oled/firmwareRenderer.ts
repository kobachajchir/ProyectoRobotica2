import { renderDashboard,clearDashboard } from './dashboardRenderer';
import { componentHeader,componentPanel,componentProgress } from './oledComponents';
import { renderQrScreen,clearQrScreenCache } from './qrScreen';
import { renderMotorSelector } from './motorSelector';
import { ENCODER_LONG } from './encoderLongIcon';
/* AFR2-20261030-01: AutoMicro dashboard adaptation and linked icons; prior authors retained. */
/* AFR2-20261029-27: quick controls/radar/PWM/WS integration; prior authors retained. */
/** AFR2-20261024-09. Browser copy of current AVR pure layout library.
 * Ported from afr2_{screens,network_screen,sequence_screen,icons}.c.
 * C layout source hashes are pinned in firmwareRenderer.sources.json; build
 * refuses a changed layout until this port is reviewed. Resources are copied.
 * AFR2-20261106-04: AVR flash text/notifications preserve these strings and timings.
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
import { afr2Font7x10Row } from './font7x10';
import { FIRMWARE_VERSION } from './firmwareVersion';
// New symbols are emitted by sync-oled-library with the appended AVR nodes.
const S = L.symbols as typeof L.symbols & { AFR2_N_ODOM_MENU:number; AFR2_N_ODOM_RUN:number; AFR2_N_ODOM_ASK:number; AFR2_N_HALL_PICK:number; AFR2_N_HALL_COUNT:number; AFR2_N_SAVE_CONFIRM:number; AFR2_N_BATTERY:number; AFR2_N_BATTERY_VIEW:number; AFR2_A_BATTERY_MEASURE:number; AFR2_N_WIFI_ASK:number; AFR2_NET_STA_NOT_FOUND:number };
/** AFR2-20261108-06: same 5 s as AFR2_WIFI_ASK_MS (afr2_screens.h). */
export const WIFI_ASK_MS = 5000;
/** AFR2-20261108-07: AFR2_SPLASH_MS and AFR2_SCREEN_SPLASH. */
export const SPLASH_MS = 3000;
export const SPLASH_SCREEN_CODE = 0x010004;
const trunc = Math.trunc;
const elapsed = (now: number, then: number) => (now - then) >>> 0;
export interface FirmwareMenuItem { label: string; target: number; action: number; arg: number; min: number; max: number; step: number }
export interface FirmwareUiState {
  dashboardInputMs?:number;dashboardHeld?:boolean;holdStartedMs?:number;holdProgress?:number;
  confirmWait?: boolean; confirmSave?:number;
  editingAction?:number; testLong?:boolean; testClicks?:number; testTurns?:number;
  quickPending?:boolean;quickExpected?:boolean;
  /** AFR2-20261108-06: wifi_ask_timer / wifi_ask_ms of the "Sin red Wi-Fi" selector. */
  wifiAskTimer?:boolean;wifiAskMs?:number;
  /** AFR2-20261108-07: splash visible since this clock (Afr2Ui splash==2 / splash_ms). */
  splashMs?:number;
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
  dashboardValid?:boolean;dashboardMode?:number;dashboardMoving?:boolean;dashboardFault?:boolean;
  networkServer?:boolean;qrAllowed?:boolean;
  networkAskFallback?:boolean;networkPolicyValid?:boolean;networkPage?:number;networkTotal?:number;networkBusy?:boolean;networkMode?:number;networkTruncated?:boolean;networkScanState?:number;wifiDraftReady?:boolean;
  networkCount?:number; wifiConnected?:boolean|null; rowSecurity?:readonly number[];
  networkRssi?:number; networkRssiValid?:boolean;
  configDirty?:boolean; actionStates?:Record<number,boolean>; powerPercent?:number; powerChangedMs?:number; powerKnown?:boolean;
  hallCpr?: readonly number[]; hallPending?: number;
  radarRange?:number;radarTrace?:boolean;quickFlags?:number;
  sensor?: FirmwareSensorSnapshot; irRaw?: readonly number[]; irKind?:number; irValid?:boolean; lineValid?:boolean;lineBits?:number;
  lines: readonly string[]; labels: readonly string[]; disabled: readonly number[];
  sequenceState: number; sequenceCount: number; sequenceIndex: number; sequenceKind: number;
  sequenceDurationMs: number; sequenceElapsedMs: number;
  networkValid: boolean; networkSta: boolean; networkAp: boolean;
  networkSsid: string; networkApSsid: string; networkPassword: string; networkIp: string; networkApIp: string;
}
export const QUICK_MODES_NODE = 61;
/** Web-only preview node; moved from 70 because AVR AFR2_N_HALL_LIVE now uses 70 (AFR2-20261106-07). */
export const MOTOR_SELECTOR_NODE = 75; // AFR2-20261108-06: 72 = ODOM_RUN, 73 = ODOM_ASK, 74 = WIFI_ASK
export function firmwareNode(node:number):readonly (string|number)[] {
  const virtual:Record<number,readonly (string|number)[]>={61:['Elegir modo',S.AFR2_N_QUICK,0,3,0x802c,0x3003d],62:['Prueba encoder',S.AFR2_N_TESTS,1,1,89,0x3003e],63:['Prueba ESP',S.AFR2_N_TESTS,0,3,0,0x3003f],64:['Potencia manual',S.AFR2_N_CONTROL,1,1,26,0x30040],65:['Datos de red',32,1,1,138,0x30041],66:['Buscar redes',31,0,3,0,0x30042],67:['Cambiar modo',31,1,3,138,0x30043],75:['Elegir un motor',S.AFR2_N_TESTS,1,4,0,0x30044]};
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
  if(node===MOTOR_SELECTOR_NODE || node===S.AFR2_N_HALL_PICK)return 4;
  if(node===S.AFR2_N_TESTS)return 8; // AFR2-20261108-02: sin Hall una vuelta; Prueba Hall segunda.
  if(node===S.AFR2_N_HALL_TEST)return 3;
  if(node===S.AFR2_N_SAVE_CONFIRM)return 2; // Si/No
  if(node===S.AFR2_N_ODOM_MENU)return 5; // AFR2-20261108-02
  if(node===S.AFR2_N_PREFS)return 7-(dirty?0:1); // AFR2-20261108-06: +Reintento Wi-Fi s
  if(node===31)return 6+Number(!!((snapshot?.networkMode??0)&1));
  if(node===36)return 3;
  if(node===66)return snapshot?.networkScanState===1?1:snapshot?.networkTotal?3:2;
  if(node===34)return 4;
  if(node===33)return (snapshot?.networkCount??0)+2+Number((snapshot?.networkPage??0)>0)+Number((snapshot?.networkTotal??0)>((snapshot?.networkPage??0)+1)*8);
  if(node===32)return snapshot?.wifiConnected===false?6:snapshot?.wifiConnected===true?5:4;
  if(node===35 && typeof snapshot?.wifiConnected!=="boolean")return 2;
  const count=node===S.AFR2_N_MENU?7:node===S.AFR2_N_DIAG?3:node===S.AFR2_N_QUICK?8:Number(firmwareNode(node)[3]);
  return count-(!dirty && (node===S.AFR2_N_PREFS || node===S.AFR2_N_CALIB)?1:0);
}
export function firmwareMenuItem(node: number, index: number,snapshot?:FirmwareSnapshot): FirmwareMenuItem {
  const n = firmwareNode(node);
  index = Math.min(index, firmwareNodeCount(node,snapshot?.configDirty,snapshot) - 1);
  if(node===31 && !((snapshot?.networkMode??0)&1) && index>=2)index++;
  if(node===36)return {label:index===0?(snapshot?.networkAp?'Detener AP':'Iniciar AP'):index===1?`Preguntar AP: ${!snapshot?.networkPolicyValid?'--':snapshot.networkAskFallback?'SI':'NO'}`:'Volver',target:index===2?31:255,action:index===0?110:index===1?109:0,arg:0,min:0,max:0,step:0};
  if(node===66){
    const row=(label:string,action=0,target=255):FirmwareMenuItem=>({label,action,target,arg:0,min:0,max:0,step:0});
    if(snapshot?.networkScanState===1)return row('Buscando redes');
    if(snapshot?.networkTotal){if(!index)return row('Ver redes',0,33);index--;}
    return !index?row('Refrescar',45):row('Volver',0,31);
  }
  if(node===33 && index>=(snapshot?.networkCount??0)){
    let extra=index-(snapshot?.networkCount??0);const page=snapshot?.networkPage??0;
    const row=(label:string,action:number,arg=0,target=255):FirmwareMenuItem=>({label,action,arg,target,min:0,max:0,step:0});
    if(page){if(!extra)return row('Pagina anterior',94,page-1);extra--;}
    if((snapshot?.networkTotal??0)>(page+1)*8){if(!extra)return row('Pagina siguiente',94,page+1);extra--;}
    return !extra?row('Refrescar',45):row('Volver',0,0,66);
  }
  if(node===S.AFR2_N_LINE_CALIB){
    if(index===0)return {label:'IR digital ON/OFF',action:25,arg:0,target:255,min:0,max:1,step:1};
    if(index===2 || index===3)return {label:index===2?'Tomar fondo':'Tomar linea',action:index===2?27:28,arg:0,target:255,min:0,max:0,step:0};
  }
  if(node===S.AFR2_N_HALL_TEST)return {
    label:['Estado','Calibrar motor','Volver'][index],target:index===0?S.AFR2_N_HALL_VIEW:index===2?S.AFR2_N_CALIB:255,
    action:index===1?S.AFR2_A_HALL_BEGIN:0,arg:0,min:0,max:0,step:0};
  if(node===34){if(index===0)return {label:"Traer credenciales ESP",target:255,action:93,arg:0,min:0,max:0,step:0};index--;}
  if(node===31 && index>=4){if(index<6)return {label:index===4?'QR conectar AP':'QR abrir Web',target:index===4?68:69,action:0,arg:0,min:0,max:0,step:0};index-=2;}
  if(WIFI_MENUS[node]){
    if(node===32){if(typeof snapshot?.wifiConnected!=="boolean" && index>=3)index+=2;else if(snapshot?.wifiConnected===true && index>=4)index++;}
    if(node===35 && typeof snapshot?.wifiConnected!=="boolean" && index>=1)index++;
    const row=WIFI_ROWS[WIFI_MENUS[node][0]+index];let action=Number(row[2]),label=String(row[0]);
    if(action===47 && snapshot?.wifiConnected===true){action=91;label='Desconectar';}
    else if(action===47 && !snapshot?.wifiDraftReady)return {label:'Configurar red',target:34,action:0,arg:0,min:0,max:0,step:0};
    return {label,target:Number(row[1]),action,arg:Number(row[3]),min:Number(row[4]),max:Number(row[5]),step:Number(row[6])};
  }
  if(node===S.AFR2_N_ODOM_MENU){ // AFR2-20261108-02: lista de la prueba de distancia (mismas filas que odom[] del AVR)
    const row=(label:string,target:number,action:number,min=0,max=0,step=0):FirmwareMenuItem=>({label,target,action,arg:0,min,max,step});
    return [row('Estado',S.AFR2_N_POSE,0),row('Direccion',255,99,0,7,1),row('Distancia cm',255,100,5,100,5),
      row(snapshot?.actionStates?.[101]?'Detener':'Iniciar',255,101),row('Volver',S.AFR2_N_CALIB,0)][index];
  }
  if(node===S.AFR2_N_PREFS){ // AFR2-20261108-06: port of Afr2Ui_MenuItem + Afr2Menu_Item (row 2 = extra[47])
    if(!snapshot?.configDirty && index>=4)index++;
    if(index===2)return {label:'Reintento Wi-Fi s',target:255,action:106,arg:0,min:10,max:300,step:10};
    if(index>2)index--;
    const row=REFORM_ROWS[REFORM_MENUS[node][0]+index];
    return {label:String(row[0]),target:Number(row[1]),action:Number(row[2]),arg:Number(row[3]),min:Number(row[4]),max:Number(row[5]),step:Number(row[6])};
  }
  if(node===S.AFR2_N_TESTS && index===1)return {label:'Prueba Hall',target:S.AFR2_N_HALL_LIVE,action:0,arg:0,min:0,max:0,step:0};
  if(REFORM_MENUS[node]){
    if(!snapshot?.configDirty && ((node===S.AFR2_N_PREFS && index>=3) || (node===S.AFR2_N_CALIB && index>=4)))index++;
    const row=REFORM_ROWS[REFORM_MENUS[node][0]+index];
    const action=Number(row[2]);const toggle=[74,77,86,87].includes(action);
    return {label:toggle?(snapshot?.actionStates?.[action]?'Detener':'Iniciar'):String(row[0]),target:Number(row[1]),action,arg:Number(row[3]),min:Number(row[4]),max:Number(row[5]),step:Number(row[6])};
  }
  // AFR2-20261108-10: 5 Ultrasonido (extra[48]), 6 Ajustes (extra[36]), 7 Modo.
  // AFR2-20261108-12: Modo, Motores, Ultrasonido, Radar, Servo, Linea, Rastro, Ajustes (extra[] map of afr2_menu_catalog.c).
  if(node===S.AFR2_N_QUICK){
    if(index===0)return {label:'Seleccion de modo',target:QUICK_MODES_NODE,action:0,arg:0,min:0,max:0,step:0};
    if(index===7)return {label:'Menu principal',target:S.AFR2_N_MENU,action:0,arg:0,min:0,max:0,step:0};
    const row=L.extra[[43,33,48,31,32,49,35,36][index]]; // AFR2-20261108-13: Linea = IR sensors (extra[49])
    return {label:String(row[0]),target:Number(row[1]),action:Number(row[2]),arg:Number(row[3]),min:Number(row[4]),max:Number(row[5]),step:Number(row[6])};
  }
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
function menuIcon(node: number, target: number, action: number, label: string): number {
  if(action===45)return S.AFR2_ICON_REFRESH;
  if (label.startsWith('Volver')) return S.AFR2_ICON_BACK;
  if(target===S.AFR2_N_QR_AP || target===S.AFR2_N_WIFI_AP)return S.AFR2_ICON_AP;
  if(target===S.AFR2_N_QR_WEB)return S.AFR2_ICON_LINK;
  if(target===S.AFR2_N_BATTERY || target===S.AFR2_N_BATTERY_VIEW || action===S.AFR2_A_BATTERY_MEASURE)return S.AFR2_ICON_BATTERY_UNKNOWN;
  if(target===S.AFR2_N_MENU)return S.AFR2_ICON_CONFIG;
  if(target===S.AFR2_N_ENCODER_TEST)return S.AFR2_ICON_ENCODER;
  if(target===S.AFR2_N_IR_VALUES || action===S.AFR2_A_LINE_TOGGLE || action===S.AFR2_A_US_TOGGLE || action===S.AFR2_A_IR_TOGGLE)return S.AFR2_ICON_SENSORS;
  if(([S.AFR2_N_WIFI_VIEW,S.AFR2_N_WIFI_NETWORKS,S.AFR2_N_WIFI_SETUP,S.AFR2_N_WIFI_REVIEW,S.AFR2_N_WIFI_SEARCH,S.AFR2_N_WIFI_MODE_SELECT,S.AFR2_N_WIFI_DATA] as number[]).includes(target))return S.AFR2_ICON_WIFI;
  if(target===S.AFR2_N_RADAR_OPTIONS || target===S.AFR2_N_RADAR_RANGE || action===S.AFR2_A_RADAR_TOGGLE || action===S.AFR2_A_RADAR_TRACE || action===S.AFR2_A_RADAR_RANGE)return S.AFR2_ICON_RADAR;
  if(([S.AFR2_A_SERVO_TOGGLE,S.AFR2_A_SERVO_ANGLE,S.AFR2_A_SERVO_US,S.AFR2_A_SERVO_APPLY,S.AFR2_A_SERVO_EDIT_BEGIN,S.AFR2_A_SERVO_EDIT_END] as number[]).includes(action))return S.AFR2_ICON_SERVO;
  if(([S.AFR2_A_MOTOR_TOGGLE,S.AFR2_A_WHEEL_TOGGLE,S.AFR2_A_GOTO_TOGGLE,S.AFR2_A_POWER] as number[]).includes(action))return S.AFR2_ICON_MOTOR;
  if (action === S.AFR2_A_REBOOT_MEGA || action === S.AFR2_A_REBOOT_ESP || target === S.AFR2_N_REBOOT) return S.AFR2_ICON_REFRESH;
  if (([S.AFR2_N_DISTANCE_SERVO,S.AFR2_N_RADAR_VIEW,S.AFR2_N_RADAR_CONFIG] as number[]).includes(target) || (action>=S.AFR2_A_RADAR_START && action<=S.AFR2_A_US_INTERVAL))return S.AFR2_ICON_RADAR;
  if(target===S.AFR2_N_SERVO || target===S.AFR2_N_SERVO_ANGLE || action===S.AFR2_A_SERVO_CENTER || action===S.AFR2_A_SERVO_OFF)return S.AFR2_ICON_SERVO;
  if (target === S.AFR2_N_AP_INFO || node === S.AFR2_N_WIFI_AP) return S.AFR2_ICON_AP;
  if (target === S.AFR2_N_WIFI || target === S.AFR2_N_NETWORK || node === S.AFR2_N_WIFI || (action >= S.AFR2_A_WIFI_STATUS && action <= S.AFR2_A_WIFI_SET_MODE)) return S.AFR2_ICON_WIFI;
  if (target === S.AFR2_N_ENCODER_VIEW || target === S.AFR2_N_ENCODER_CONFIG) return S.AFR2_ICON_ENCODER;
  if (([S.AFR2_N_SENSORS, S.AFR2_N_LINE_VIEW, S.AFR2_N_US_VIEW, S.AFR2_N_HALL_VIEW, S.AFR2_N_HALL_LIVE] as number[]).includes(target)) return S.AFR2_ICON_SENSORS;
  if (target === S.AFR2_N_PREFS || target === S.AFR2_N_PREFS_VIEW || action === S.AFR2_A_SAVE || action === S.AFR2_A_DISCARD) return S.AFR2_ICON_PREFS;
  if (target === S.AFR2_N_LINK_VIEW) return S.AFR2_ICON_LINK;
  if (([S.AFR2_N_CONTROL, S.AFR2_N_MOVE, S.AFR2_N_WHEEL] as number[]).includes(target) || node === S.AFR2_N_MOVE || node === S.AFR2_N_WHEEL) return S.AFR2_ICON_MOTOR;
  if (target === S.AFR2_N_TESTS || target === S.AFR2_N_CALIB || node === S.AFR2_N_TESTS || node === S.AFR2_N_CALIB) return S.AFR2_ICON_TOOLS;
  if (target === S.AFR2_N_SYSTEM || target === S.AFR2_N_DISPLAY_MENU || node === S.AFR2_N_SYSTEM || node === S.AFR2_N_DISPLAY_MENU) return S.AFR2_ICON_CONFIG;
  if (action === S.AFR2_A_STOP || target === S.AFR2_N_ALERT_VIEW) return S.AFR2_ICON_WARNING;
  return S.AFR2_ICON_INFO;
}
function line(o: Afr2OledBuffer, y: number, text: string, time: number) {
  const right=y>=40?104:128;o.fillRect(0,y,128,10,'black');o.textWindow(0,right,y,text,firmwareTextOffset(text,right,time));
}
function labeled(o: Afr2OledBuffer, y: number, label: string, left: number, text: string, time: number) {
  const right=y>=40?104:128;o.fillRect(0,y,128,10,'black');o.drawText(0,y,label);o.textWindow(left,right,y,text,firmwareTextOffset(text,right-left,time));
}
function network(o: Afr2OledBuffer, u: FirmwareUiState, s: FirmwareSnapshot) {
  if(u.node===S.AFR2_N_NETWORK){o.clear();componentHeader(o,S.AFR2_ICON_WIFI,'Conexion',u.selection+1,2);for(let i=0;i<3;i++)line(o,16+i*12,s.lines[i]??'',elapsed(u.clockMs,u.scrollStartedMs));gestureHint(o,0,53,false,'Volver');gestureHint(o,66,53,true,'Volver');return;}
  const ap = u.node === S.AFR2_N_AP_INFO, time = elapsed(u.clockMs, u.scrollStartedMs);
  componentHeader(o,ap?S.AFR2_ICON_AP:S.AFR2_ICON_WIFI,ap?'Datos de red AP':'IP de conexion');
  if (!s.networkValid) { line(o, 17, 'Sin datos actuales', time); line(o, 32, 'Actualizar Wi-Fi', time); }
  else if (ap) {
    if (!s.networkAp) { line(o, 17, 'AP no esta activo', time); line(o, 32, 'IP: --', time); }
    else { labeled(o, 16, 'SSID:', 35, s.networkApSsid, time); labeled(o, 28, 'Clave:', 42, s.networkPassword || 'Sin clave', time); labeled(o, 40, 'IP:', 21, s.networkApIp, time); }
  } else if (s.networkSta || s.networkAp) {
    labeled(o, 16, 'SSID:', 35, s.networkSta ? s.networkSsid : s.networkApSsid, time);
    labeled(o, 29, 'IP:', 21, s.networkSta ? s.networkIp : s.networkApIp, time);
    line(o, 41, s.networkSta ? 'STA / abrir en Web' : 'AP / abrir en Web', time);
  } else { line(o, 17, 'Sin IP adquirida', time); line(o, 32, 'Esperando conexion', time); }
  gestureHint(o,94,52,true,'Volver');
}
function networkNotice(o: Afr2OledBuffer, u: FirmwareUiState, s: FirmwareSnapshot) {
  const n = u.notice!, kind = n.kind - 16, time = elapsed(u.clockMs, u.noticeStartedMs);
  let image: number = S.AFR2_ICON_INFO;
  if (kind === S.AFR2_NET_GOT_IP || kind === S.AFR2_NET_STA_LOST) image = S.AFR2_ICON_WIFI;
  else if (kind === S.AFR2_NET_STA_NOT_FOUND) image = S.AFR2_ICON_WIFI_OFF;
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
    case S.AFR2_NET_STA_NOT_FOUND: o.drawText(24, 4, 'Wi-Fi no'); o.drawText(24, 15, 'encontrado'); line(o, 32, n.text, time); break; // AFR2-20261108-06
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
  componentHeader(o,S.AFR2_ICON_WIFI,u.keyboardField?'Clave Wi-Fi':'SSID Wi-Fi');
  const length = Math.min(u.length, 17);
  o.drawText(0, 16, u.keyboardField ? '*'.repeat(length) : u.ssid.slice(u.length - length, u.length));
  const first = trunc(trunc(u.key / 10) / 2) * 20;
  for (let j = 0; j < 20 && first + j < 97; j++) {
    const k = first + j, x = (j % 10) * 10, y = 28 + trunc(j / 10) * 12;
    o.drawText(x, y, k < 95 ? String.fromCharCode(32 + k) : k === 95 ? '<' : 'OK');
    if (k === u.key) o.drawRect(x - 1, y - 1, 10, 12, 'invert');
  }
  gestureHint(o,0,53,true,'Cancelar');
}
/** AFR2-20261029-20: same pure wizard/status/confirmation layout as AVR.
 * Values are prepared measurements; missing CPR never becomes a fake zero. */
function hall(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot){
  if(u.node===S.AFR2_N_HALL_PICK){
    renderMotorSelector(o,u.selection);
  }else if(u.node===S.AFR2_N_HALL_COUNT){
    componentHeader(o,S.AFR2_ICON_MOTOR,'Gire CW 1 vuelta');o.drawText(0,16,s.lines[0]??'');o.drawText(0,28,s.lines[1]??'');
    gestureHint(o,0,40,false,'Finalizar');gestureHint(o,0,53,true,'Cancelar');
  }else{
    o.drawText(0,0,'Estado Hall');
    for(let i=0;i<4;i++){
      const cpr=s.hallCpr?.[i];
      o.drawText(0,12+i*10,`${['FL','FR','RL','RR'][i]}:${cpr?`${cpr} ${((s.hallPending??0)&(1<<i))?'*':'OK'}`:'Sin calibrar'}`);
    }
    o.drawText(0,54,'*Nuevo');gestureHint(o,94,52,true,'Volver');
  }
}
function saveConfirm(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot){
  if(u.confirmSave===S.AFR2_A_HALL_SAVE){
    componentHeader(o,S.AFR2_ICON_MOTOR,s.lines[0]??'Sensor Hall');o.drawText(0,16,s.lines[1]??'');
    if(u.confirmWait){o.drawText(0,34,'Guardando...');return;}
    icon(o,S.AFR2_ICON_PREFS,0,33,12);o.drawText(16,34,'Confirmar');
    icon(o,S.AFR2_ICON_BACK,0,48,12);o.drawText(16,49,'Descartar');
    o.fillRect(0,u.selection?47:32,128,14,'invert');return;
  }
  componentHeader(o,S.AFR2_ICON_PREFS,'Guardar cambios');
  if(u.confirmWait){o.drawText(0,26,'Guardando...');return;}
  o.drawText(0,16,'Desea guardarlos?');o.drawText(14,34,'Si');o.drawText(14,49,'No');o.fillRect(0,u.selection?47:32,128,14,'invert');
}
/** Corresponds to Afr2Ui_Render, including partial scroll updates. Caller owns
 * revision/redraw flags and must provide a retained buffer for partial draws. */
export function renderTouchScreen(o: Afr2OledBuffer, direction: number, heading: number, intro: boolean, full: boolean): void {
  if (full) {
    o.clear(); componentHeader(o,S.AFR2_ICON_MOTOR,'Control tactil');
    if (intro) { o.drawText(36, 26, 'Activado'); o.drawText(8, 53, 'Desde la Web'); }
  }
  if (intro) return;
  // AFR2-20261030-12: literal numeric heading footer, same 15-degree sectors.
  o.fillRect(0,53,128,10,'black');o.drawText(0,53,`Angulo: ${heading*15} grados`);
  o.fillRect(32,14,64,38,'black');
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
      if (x >= 32 && x < 96 && y >= 14 && y < 52) o.setPixel(x, y);
    }
  }
}
function quick(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot){
  o.clear();const item=firmwareMenuItem(S.AFR2_N_QUICK,u.selection);
  componentHeader(o,menuIcon(S.AFR2_N_QUICK,item.target,item.action,item.label),['Modo','Motores','Ultrasonido','Radar','Servo','Linea','Rastro','Ajustes'][u.selection],u.selection+1,8);componentPanel(o,0,16,128,34);
  const center=(y:number,text:string)=>o.drawText(text.length*7<128?trunc((128-text.length*7)/2):0,y,text);
  const on=!!((s.quickFlags??0)&(1<<u.selection));
  if(u.selection===0 || u.selection===7){center(20,u.selection===7?'AJUSTES Y PRUEBAS':'MODO DEL ROBOT');gestureHint(o,28,36,false,'Entrar');}
  else if(s.quickFlags===undefined){center(20,'SIN ESTADO');center(36,'Requiere equipo');}
  else {
    center(20,u.selection===1?(on?'HABILITADOS':'BLOQUEADOS'):u.selection===2?(on?'MIDIENDO':'APAGADO'):u.selection===3?(on?'ACTIVO':'DETENIDO'):u.selection===5?(on?'LEYENDO':'APAGADO'):u.selection===6?(on?'VISIBLE':'OCULTO'):(on?'ACTIVADO':'DESACTIVADO'));
    if(u.quickPending)center(36,u.quickExpected?'Activando...':'Desactivando...');else gestureHint(o,28,36,false,u.selection===1?(on?'Bloquear':'Habilitar'):u.selection===6?(on?'Ocultar':'Mostrar'):(on?'Detener':'Activar'));
  }
  o.fillRect(3,33,122,15,'invert');icon(o,24,2,50,13);gestureHint(o,94,52,true,'Volver');
}
function capabilities(o:Afr2OledBuffer,u:FirmwareUiState,s:FirmwareSnapshot){
  o.clear();componentHeader(o,S.AFR2_ICON_INFO,'Capacidades',u.selection+1,6);
  for(let i=0;i<3;i++)line(o,16+i*12,s.lines[i]??'',elapsed(u.clockMs,u.scrollStartedMs));gestureHint(o,0,52,false,'OK');gestureHint(o,94,52,true,'Volver');
}
export function renderPower(o:Afr2OledBuffer,percent:number){
  percent=Math.max(0,Math.min(100,percent));o.clear();componentHeader(o,S.AFR2_ICON_MOTOR,'Potencia');o.drawText(0,15,`Valor: ${Math.floor((percent*255+50)/100)} /255`);
  componentProgress(o,30,true,percent,0);o.drawText(52,43,`${percent}%`);icon(o,24,0,47,16);
}
/** AFR2-20261108-07: port of big_text_flash/render_splash (afr2_screens.c). */
function renderSplash(o:Afr2OledBuffer,u:FirmwareUiState){
  const spent=Math.min(SPLASH_MS,elapsed(u.clockMs,u.splashMs??u.clockMs));
  o.clear();o.fillRect(0,0,trunc(spent*128/SPLASH_MS),2);
  const letters=Math.min(4,1+trunc(spent/200));
  for(let i=0;i<letters;i++)for(let row=0;row<10;row++){
    const bits=afr2Font7x10Row('AFR2'.charCodeAt(i),row);
    for(let col=0;col<7;col++)if(bits&(0x80>>col))o.fillRect(36+i*14+col*2,8+row*2,2,2);
  }
  const center=(y:number,text:string)=>o.drawText(text.length*7<128?trunc((128-text.length*7)/2):0,y,text);
  center(32,'ATmega');center(47,`Firmware ${FIRMWARE_VERSION}`);
}
function renderFirmwareContent(u: FirmwareUiState, o: Afr2OledBuffer, s: FirmwareSnapshot): boolean | void {
  if(u.splashMs!==undefined){clearDashboard(o);clearQrScreenCache(o);renderSplash(o,u);return;}
  if(u.node!==S.AFR2_N_HOME || u.notice || u.touch)clearDashboard(o);
  if(u.notice || (u.node!==68 && u.node!==69))clearQrScreenCache(o);
  if(u.touch && s.powerKnown && s.powerPercent!==undefined && s.powerChangedMs!==undefined && elapsed(u.clockMs,s.powerChangedMs)<1200){renderPower(o,s.powerPercent);return;}
  if (u.touch) { renderTouchScreen(o, u.touch.direction, u.touch.heading, elapsed(u.clockMs, u.touch.startedMs) < 1000, u.touch.full); return; }
  if(u.node===S.AFR2_N_HOME && !u.notice)return renderDashboard(o,u,s);
  const n = firmwareNode(u.node), partial = !u.redraw && u.scrollRedraw;
  if(!u.notice && (u.node===68 || u.node===69)){renderQrScreen(o,s,u.node===68);return;}
  if (!partial) o.clear();
  if (u.notice) {
    const a = u.notice, width = Math.min(128, trunc((((elapsed(u.clockMs, u.noticeStartedMs) * 128) >>> 0) / a.ms)) & 0xffff);
    o.fillRect(0, 0, 128, 2, 'black'); o.fillRect(0, 0, width, 2);
    if (a.kind > 16) networkNotice(o, u, s);
    else if (!partial) { icon(o, a.kind ? S.AFR2_ICON_WARNING : S.AFR2_ICON_INFO, 56, 6, 16); o.drawText(0, 25, a.title); o.drawText(0, 39, a.text); gestureHint(o,0,53,false,'Cerrar');gestureHint(o,66,53,true,'Cerrar'); }
  } else if(u.node===S.AFR2_N_QUICK)quick(o,u,s);
  else if(u.node===62){
    o.drawText(0,0,'Prueba encoder');o.drawText(0,13,`Clicks: ${u.testClicks??0}`);o.drawText(0,26,`Giro: ${u.testTurns??0}`);gestureHint(o,0,39,true,u.testLong?'Correcto':'Pulse');if(u.testLong)gestureHint(o,0,52,true,'Salir');
  }
  else if(u.node===MOTOR_SELECTOR_NODE)renderMotorSelector(o,u.selection);
  else if(u.node===S.AFR2_N_ODOM_ASK){ // AFR2-20261108-04: port of the ODOM_ASK branch of render_screen
    componentHeader(o,S.AFR2_ICON_TOOLS,'Prueba odometria');o.drawText(0,16,s.lines[0]??'');o.drawText(0,28,s.lines[1]??'');
    gestureHint(o,0,40,false,'Iniciar');gestureHint(o,0,53,true,'Descartar');
  }
  else if(u.node===S.AFR2_N_WIFI_ASK){ // AFR2-20261108-06: port of render_wifi_ask
    componentHeader(o,S.AFR2_ICON_WIFI_OFF,'Sin red Wi-Fi');
    if(u.wifiAskTimer){
      const spent=Math.min(WIFI_ASK_MS,elapsed(u.clockMs,u.wifiAskMs??u.clockMs)),left=WIFI_ASK_MS-spent;
      o.drawText(0,16,`Inicia AP en ${trunc((left+999)/1000)} s`);o.fillRect(0,28,trunc(left*128/WIFI_ASK_MS),2);
    } else o.drawText(0,16,'Elija una opcion');
    icon(o,S.AFR2_ICON_AP,0,33,12);o.drawText(16,34,'Iniciar AP');
    icon(o,S.AFR2_ICON_REFRESH,0,48,12);o.drawText(16,49,'Seguir esperando');
    o.fillRect(0,u.selection?47:32,128,14,'invert');
  }
  else if(u.node===64){if(s.powerKnown && s.powerPercent!==undefined)renderPower(o,s.powerPercent);else {o.drawText(0,0,'Potencia manual');o.drawText(0,25,'Sin valor vigente');}}
  else if(u.node===67){
    componentHeader(o,S.AFR2_ICON_WIFI,'Cambiar modo');o.drawText(0,16,'Opciones de modos');o.drawText(3,28,'STA');o.drawText(45,28,'AP');o.drawText(80,28,'AP-STA');o.fillRect(u.selection===0?0:u.selection===1?40:78,40,u.selection===2?49:31,2);gestureHint(o,0,53,false,'Elegir');gestureHint(o,66,53,true,'Volver');
  }
  else if(u.node===65){
    componentHeader(o,S.AFR2_ICON_WIFI,'Datos de red');if(s.labels[0])o.textWindow(0,128,16,s.labels[0],firmwareTextOffset(s.labels[0],128,elapsed(u.clockMs,u.scrollStartedMs)));else o.drawText(0,16,'Sin datos actuales');if(s.labels[1]){o.textWindow(0,128,28,s.labels[1],firmwareTextOffset(s.labels[1],128,elapsed(u.clockMs,u.scrollStartedMs)));o.textWindow(0,128,40,s.labels[2]??'',firmwareTextOffset(s.labels[2]??'',128,elapsed(u.clockMs,u.scrollStartedMs)));}else o.drawText(0,28,s.lines[1]??'');const secure=(s.rowSecurity?.[0]??0)>=2;if(secure)icon(o,25,0,39,12);o.drawText(secure?14:0,40,s.lines[2]??'');gestureHint(o,0,53,false,'Volver');gestureHint(o,66,53,true,'Volver');
  }
  else if(u.node===S.AFR2_N_ENCODER_VIEW){componentHeader(o,S.AFR2_ICON_ENCODER,'Estado encoder');for(let i=0;i<3;i++){icon(o,i===0?24:i===1?7:17,0,16+12*i,10);o.drawText(14,16+12*i,s.lines[i]??'');}gestureHint(o,0,53,true,'Volver');}
  else if(u.node===S.AFR2_N_CAPS)capabilities(o,u,s);
  else if(u.node===S.AFR2_N_SAVE_CONFIRM)saveConfirm(o,u,s);
  else if(u.node===S.AFR2_N_HALL_PICK || u.node===S.AFR2_N_HALL_COUNT || u.node===S.AFR2_N_HALL_VIEW)hall(o,u,s);
  else if (u.keyboard) keyboard(o, u);
  else if (u.node === S.AFR2_N_SEQUENCE) sequence(o, s,u.clockMs);
  else if (u.node === S.AFR2_N_NETWORK || u.node === S.AFR2_N_AP_INFO) network(o, u, s);
  else if (([S.AFR2_N_RADAR_VIEW,S.AFR2_N_US_VIEW,S.AFR2_N_LINE_VIEW,S.AFR2_N_IR_VALUES,S.AFR2_N_SERVO_ANGLE] as number[]).includes(u.node))renderSensorScreen(o,u.node,s.sensor,{raw:s.irRaw??[],kind:s.irKind??0,valid:s.irValid??false,lineValid:s.lineValid??false,bits:s.lineBits??0},{rangeIndex:s.radarRange??0,trace:s.radarTrace??true});
  else if(u.edit && u.editingAction===12)renderMotorSelector(o,u.draft);
  else if(u.edit && u.editingAction===83)renderPower(o,u.draft);
  else if (u.edit) { componentHeader(o,S.AFR2_ICON_CONFIG,u.editingLabel); o.drawText(0,16,(u.editingAction===99?['Adelante','Atras','Izquierda','Derecha','Diagonal AD-I','Diagonal AD-D','Diagonal AT-I','Diagonal AT-D'][u.draft&7]:u.editingAction===100?`${u.draft} cm`:String(u.draft)).slice(0,17));gestureHint(o,0,28,'turn','Ajustar'); gestureHint(o,0,40,false,u.editingAction===99||u.editingAction===100?'Guardar':'Aplicar');gestureHint(o,0,53,true,'Descartar'); }
  else {
    const view = u.node!==S.AFR2_N_DIAG && Number(n[2]) !== 0, count = view ? 1 : 3;
    if (!partial) { const title=u.node===31?`Wi-Fi ${s.networkMode===1?'STA':s.networkMode===2?'AP':s.networkMode===3?'AP-STA':'--'}`:u.node===66?(s.networkScanState===1?'Buscando redes':s.networkScanState===3?'Busqueda fallo':!s.networkScanState?'Buscar redes':s.networkTotal?'Redes listas':'Sin redes'):u.node===33?`Redes ${(s.networkPage??0)+1}/${(s.networkTotal??0)>8?2:1}${s.networkTruncated?'+':''}`:u.node===32?(s.lines[0]||'Estado de red'):String(n[0]);componentHeader(o,menuIcon(u.node,u.node,0,title),title); if (view) for (let j = 0; j < 3; j++) o.textWindow(0,j===2?104:128,16+j*12,s.lines[j],j===2?firmwareTextOffset(s.lines[j],104,elapsed(u.clockMs,u.scrollStartedMs)):0); }
    for (let row = 0; row < count && u.firstVisible + row < firmwareNodeCount(u.node,s.configDirty,s); row++) {
      const item = firmwareMenuItem(u.node, u.firstVisible + row,s), selected = u.firstVisible + row === u.selection;
      if (partial && !selected) continue;
      if(view && item.label.startsWith('Volver')){if(!partial)gestureHint(o,94,52,true,'Volver');continue;}
      const y = view ? 52 : 16 + 16 * row, left = view ? 16 : 23, top = view ? y : y + 3;
      if (!partial) { icon(o, menuIcon(u.node, item.target, item.action, item.label), view ? 0 : 3, view ? 51 : y, view ? 12 : 16);  if(row!==2 && !(u.node===33 && (s.rowSecurity?.[row]??0)>=2))o.drawText(120, top, selected ? s.disabled[row] ? '!' : item.label.startsWith('Volver') ? ' ' : '>' : s.disabled[row] ? '!' : ' '); }
      let right=u.node===33 && (s.rowSecurity?.[row]??0)>=2?108:118;if(row===2)right=Math.min(right,104);
      if(u.node===33 && (s.rowSecurity?.[row]??0)>=2){const lockx=row===2?92:111;if(row===2)right=89;o.fillRect(lockx,top,10,10,'black');icon(o,25,lockx,top,10);if(partial && selected && !view)o.fillRect(lockx,top,10,10,'invert');}
      const label = s.labels[row] || item.label;
      o.fillRect(left,top,right-left,10,partial && selected && !view?'white':'black');o.textWindow(left,right,top,label,selected?firmwareTextOffset(label,right-left,elapsed(u.clockMs,u.scrollStartedMs)):0,partial && selected && !view?'black':'white');
      if(!partial && selected && !view)o.fillRect(0,y,128,16,'invert');
    }
  }
}

/** Same prepared hold progress and coordinates as the AVR wrapper. */
export function renderFirmwareScreen(u:FirmwareUiState,o:Afr2OledBuffer,s:FirmwareSnapshot):boolean|void{
  const result=renderFirmwareContent(u,o,s);
  if(u.node===68 || u.node===69)return result;
  // AFR2-20261106-07: only while pressed, no frame; disc of ring radius + 2 px cleared first.
  if(!u.dashboardHeld)return result;
  for(let dy=-11;dy<=11;dy++){let dx=0;while((dx+1)*(dx+1)+dy*dy<=121)dx++;o.fillRect(117-dx,54+dy,2*dx+1,1,'black');}
  icon(o,S.AFR2_ICON_ENCODER_LONG,112,49,11);
  const ring=[[0,-9],[3,-8],[6,-6],[8,-3],[9,0],[8,3],[6,6],[3,8],[0,9],[-3,8],[-6,6],[-8,3],[-9,0],[-8,-3],[-6,-6],[-3,-8]];
  ring.forEach(([x,y],i)=>{o.setPixel(117+x,54+y);if(i<(u.holdProgress??0))o.fillRect(116+x,53+y,2,2);});
  return result;
}
