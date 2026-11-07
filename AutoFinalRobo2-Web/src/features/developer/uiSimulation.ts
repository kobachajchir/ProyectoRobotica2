/** AFR2-20261101-02. Local port of Afr2Ui_Apply/Open/go_back and app/wire
 * transitions. Prior AFR2 authors retained. Services use explicit local demo
 * fixtures, never protocol commands, hardware or persistent credentials. */
import { FIRMWARE_LIBRARY as L } from '../oled/generated/firmwareLibrary';
import { firmwareNode, firmwareNodeCount, firmwareMenuItem, type FirmwareMenuItem, type FirmwareSnapshot, type FirmwareUiState } from '../oled/firmwareRenderer';

const S: Record<string, number> = L.symbols;
const MODE = { idle:0, manual:1, line:2, wheel:3, goto:4, fault:5, sequence:6, pwm:7 } as const;
export type SimulationScenario = 'ready' | 'missing';
interface Position { node: number; selection: number; firstVisible: number }
interface Pending { kind: 'save' | 'wifi' | 'scan' | 'web' | 'mode' | 'credentials' | 'esp'; at: number; value: number }
export interface UiSimulation extends FirmwareUiState {
  scenario: SimulationScenario; history: Position[]; editing: FirmwareMenuItem | null;
  values: Record<number, number>; saved: Record<number, number>;
  password: string; draftSsid: string; wifiDraftReady: boolean; connected: boolean;
  wifiPage:number;wifiRequestedMode:number;lineSamples:number;scanFixture:'networks'|'empty'|'failed';scanFailed:boolean;
  scanned: boolean; stopped: boolean; motionUntil: number; mode: number;
  hallMeasuring: boolean; hallCount: number; cpr: number; hallCpr: number[];
  hallDraft: number[]; hallPending: number; lineCalibrated: boolean; savedLineCalibrated: boolean;
  prefsLoaded: boolean; turns: number; clicks: number; holds: number;
  servoEnabled: boolean; radar: boolean; resumeRadar: boolean; feedback: string;
  quickSelection: number; radarPosition: Position; confirmSave: number; confirmDiscard: number; confirmTarget: number;
  pending: Pending | null; noticeTarget: number | null; quickUntil: number;
  credentialsUntil: number; batteryMeasured: boolean; diagState: number; webClient: boolean;
}
export type SimulationEvent = {type:'scan-fixture';outcome:'networks'|'empty'|'failed'} | {type:'input-held';held:boolean} | { type: 'web-client'; connected: boolean } | { type: 'rotate'; delta: -1 | 1 } | { type: 'press'; kind: 'short' | 'long' } | { type: 'tick'; ms: number } | { type: 'inspect'; code: string };
const sampleNetworks = Array.from({ length: 16 }, (_, i) => `Red demo ${i + 1}`);
const wrap = (n: number, count: number) => ((n % count) + count) % count;
const limit = (n: number, min: number, max: number) => Math.max(min, Math.min(max, n));
const persistent = [4, 6, 7, 14, 25, 26, 28, 29, 30, 31, 32, 33, 34, 35, 36, 41, 42, 53, 54, 68, 70, 71, 81];
const dirty = (u: UiSimulation) => persistent.some(key => u.values[key] !== u.saved[key]) || u.lineCalibrated !== u.savedLineCalibrated;
const radarDirty = (u: UiSimulation) => [70, 71].some(key => u.values[key] !== u.saved[key]);
const poseReady = (u: UiSimulation) => !!(u.values[39] && u.hallCpr.every(Boolean) && u.values[40]);
const actionState = (u: UiSimulation, action: number) => action === 74 ? u.radar : action === 75 ? u.servoEnabled : action === 76 ? !!u.values[36] : action === 77 ? !u.stopped && u.mode === MODE.line : action === 71 ? !!u.values[71] : action === 86 ? !u.stopped && u.mode === MODE.wheel : action === 87 ? !u.stopped && u.mode === MODE.goto : false;
/** Snapshot subset shared by option resolution and OLED drawing; no recursion. */
function menuSnapshot(u: UiSimulation): FirmwareSnapshot {
  return { networkScanState:u.pending?.kind==='scan'?1:u.scanFailed?3:u.scanned?2:0,wifiDraftReady:u.wifiDraftReady,hallPending:u.hallPending,configDirty: dirty(u),networkMode:u.values[44],networkBusy:!!u.pending && ['scan','mode','wifi'].includes(u.pending.kind), wifiConnected: u.connected, networkCount: u.scanned && u.scanFixture!=='empty' && u.pending?.kind!=='scan' ? 8 : 0,networkPage:u.wifiPage,networkTotal:u.scanned && u.scanFixture!=='empty'?16:0,
    actionStates: Object.fromEntries([74, 75, 76, 77, 71, 86, 87].map(key => [key, actionState(u, key)])),
  } as FirmwareSnapshot;
}
export const simulationScreenCode = (u: UiSimulation) => {
  const code = (u.edit && u.editingAction === 12) || u.node === S.AFR2_N_HALL_PICK ? 0x030044
    : u.edit && u.editingAction === 83 ? 0x030040 : u.touch ? 0x030035
    : u.notice && u.notice.kind > 16 ? 0x040000 + u.notice.kind - 16 : Number(firmwareNode(u.node)[5]);
  return `0x${code.toString(16).padStart(6, '0')}`;
};
export function simulationItems(u: UiSimulation): FirmwareMenuItem[] {
  const snapshot = menuSnapshot(u);
  const item = (label: string): FirmwareMenuItem => ({ label, target: 255, action: 0, arg: 0, min: 0, max: 0, step: 0 });
  if (u.node === S.AFR2_N_HOME || u.node === S.AFR2_N_HALL_COUNT || u.node === S.AFR2_N_ENCODER_TEST) return [];
  if (u.node === S.AFR2_N_SAVE_CONFIRM) return ['Si', 'No'].map(item);
  if (u.node === S.AFR2_N_HALL_PICK) return ['FL', 'FR', 'RL', 'RR'].map(item);
  if (u.node === S.AFR2_N_WIFI_MODE_SELECT) return ['STA', 'AP', 'AP-STA'].map(item);
  if (u.node === S.AFR2_N_CAPS) return ['UI y OLED', 'UART y ESP', 'Sensores', 'PWM y STOP', 'GOTO y pose', 'Secuencias y EEPROM'].map(item);
  if (u.node === S.AFR2_N_NETWORK) return ['Modo e IP', 'Mascara y puerta de enlace'].map(item);
  return Array.from({ length: firmwareNodeCount(u.node, snapshot.configDirty, snapshot) }, (_, i) => {
    const row = firmwareMenuItem(u.node, i, snapshot);
    return u.node === S.AFR2_N_WIFI_NETWORKS && i < (snapshot.networkCount ?? 0) ? { ...row, label: sampleNetworks[u.wifiPage*8+i] } : row;
  });
}
export function createUiSimulation(scenario: SimulationScenario = 'ready'): UiSimulation {
  const ready = scenario === 'ready';
  const values: Record<number, number> = { 4:150, 6:80, 7:100, 9:0, 10:0, 12:0, 13:40, 14:500, 20:1500,
    25:1, 26:31, 27:0, 28:0, 29:Number(ready), 30:0, 31:255, 32:4, 33:128, 34:8, 35:0,
    36:Number(ready), 37:80, 38:150, 39:Number(ready), 40:1, 41:4, 42:0, 44:3, 53:200, 54:0,
    59:90, 63:0, 64:180, 65:15, 66:100, 67:150, 68:0, 70:2, 71:1, 81:800, 83:100, 85:2000 };
  return {
    dashboardInputMs:0,dashboardHeld:false,
    node:S.AFR2_N_HOME, selection:0, firstVisible:0, clockMs:0, noticeStartedMs:0, scrollStartedMs:0,
    redraw:true, scrollRedraw:false, keyboard:false, keyboardField:false, key:0, length:0, ssid:'',
    edit:false, editingLabel:'', draft:0, notice:null, editing:null, scenario, history:[], values, saved:{...values},
    password:'', draftSsid:'', wifiDraftReady:false, connected:false, scanned:false,wifiPage:0,wifiRequestedMode:3,lineSamples:0,scanFixture:'networks',scanFailed:false, stopped:true, motionUntil:0, mode:0,
    hallMeasuring:false, hallCount:0, cpr:ready?360:0, hallCpr:Array(4).fill(ready?360:0), hallDraft:Array(4).fill(0), hallPending:0,
    lineCalibrated:ready, savedLineCalibrated:ready, prefsLoaded:false, turns:0, clicks:0, holds:0,
    servoEnabled:false, radar:false, resumeRadar:false, quickSelection:0,
    radarPosition:{node:S.AFR2_N_RADAR_CONFIG, selection:0, firstVisible:0}, confirmSave:23, confirmDiscard:24, confirmTarget:0,
    pending:null, noticeTarget:null, quickUntil:0, credentialsUntil:0, batteryMeasured:false, diagState:0, webClient:ready,
    feedback:'Inicio: corto abre Menu principal; largo abre Controles rapidos al soltar.',
  };
}
function sync(u: UiSimulation) { u.scrollStartedMs=u.clockMs; u.redraw=true; u.scrollRedraw=false; }
function open(u: UiSimulation, node: number) {
  if (node < 0 || node >= L.nodes.length) return;
  if (u.node===S.AFR2_N_RADAR_CONFIG) u.radarPosition={node:u.node,selection:u.selection,firstVisible:u.firstVisible};
  if (u.node===S.AFR2_N_QUICK) u.quickSelection=u.selection;
  if (node===S.AFR2_N_HOME || node===S.AFR2_N_QUICK) u.history=[];
  else if (u.history.length<8) u.history.push({node:u.node,selection:u.selection,firstVisible:u.firstVisible});
  u.node=node; u.selection=node===S.AFR2_N_QUICK?u.quickSelection:0; u.firstVisible=0; u.edit=false; u.editing=null;
  if (node===S.AFR2_N_RADAR_CONFIG) Object.assign(u,u.radarPosition);
  if (node===S.AFR2_N_WIFI_MODE_SELECT) u.selection=u.values[44]-1;
  if (node===S.AFR2_N_ENCODER_TEST) u.testShort=u.testLong=u.testBack=false;
  sync(u);
  if(node===S.AFR2_N_WIFI_SEARCH && !u.pending){u.wifiPage=0;schedule(u,'scan');}
}
function back(u: UiSimulation, fallback: number) {
  if(u.node===S.AFR2_N_RADAR_CONFIG) u.radarPosition={node:u.node,selection:u.selection,firstVisible:u.firstVisible};
  const previous=u.history.pop();
  if(previous){Object.assign(u,previous);sync(u);}else open(u,fallback);
}
function notify(u: UiSimulation, text='Aceptada', error=false, title=error?'No disponible':'Operacion', ms=3000) {
  u.notice={title,text:text.slice(0,17),kind:error?1:0,ms};u.noticeStartedMs=u.clockMs;
}
function dismiss(u: UiSimulation) {
  u.notice=null;
  if(u.noticeTarget!==null){
    const target=u.noticeTarget;u.noticeTarget=null;
    const index=u.history.map(row=>row.node).lastIndexOf(target);
    if(index>=0){Object.assign(u,u.history[index]);u.history=u.history.slice(0,index);sync(u);}else open(u,target);
  }
}
function stop(u:UiSimulation) { u.stopped=true;u.mode=0;u.motionUntil=0; }
function confirm(u:UiSimulation,save:number,discard:number,target:number){
  if(u.node===S.AFR2_N_RADAR_CONFIG)u.radarPosition={node:u.node,selection:u.selection,firstVisible:u.firstVisible};
  u.confirmSave=save;u.confirmDiscard=discard;u.confirmTarget=target;u.confirmWait=false;
  u.node=S.AFR2_N_SAVE_CONFIRM;u.selection=0;u.firstVisible=0;sync(u);
}
function leaveConfirm(u:UiSimulation){
  u.confirmWait=false;
  if(u.confirmTarget===S.AFR2_N_HOME || u.confirmTarget===S.AFR2_N_QUICK)open(u,u.confirmTarget);else back(u,u.confirmTarget);
}
function goBack(u:UiSimulation){
  if(u.notice){dismiss(u);return;}
  if(u.edit){u.edit=false;u.editing=null;return;}
  if(u.node===S.AFR2_N_HOME){open(u,S.AFR2_N_QUICK);return;}
  const target=u.history.at(-1)?.node??Number(firmwareNode(u.node)[1]);
  if((u.node===S.AFR2_N_HALL_TEST || (u.node===S.AFR2_N_HALL_VIEW && target!==S.AFR2_N_HALL_TEST)) && u.hallPending){confirm(u,18,19,target);return;}
  if([S.AFR2_N_QUICK,S.AFR2_N_RADAR_VIEW,S.AFR2_N_US_VIEW].includes(u.node) && radarDirty(u)){
    if(u.node===S.AFR2_N_QUICK)u.quickSelection=u.selection;
    confirm(u,23,73,target);return;
  }
  if([S.AFR2_N_PREFS,S.AFR2_N_CALIB,S.AFR2_N_ENCODER_CONFIG,S.AFR2_N_DISPLAY_MENU,S.AFR2_N_MOTOR_CALIB,S.AFR2_N_LINE_CALIB,S.AFR2_N_RADAR_CONFIG].includes(u.node) && dirty(u)){confirm(u,23,24,target);return;}
  if(u.node===S.AFR2_N_SERVO_ANGLE){u.radar=u.resumeRadar;u.resumeRadar=false;}
  if(u.node===S.AFR2_N_WHEEL || u.node===S.AFR2_N_MOVE){stop(u);u.hallMeasuring=false;u.hallCount=0;}
  back(u,target);
}
export function simulationUnavailable(u:UiSimulation,item:FirmwareMenuItem):string|null{
  const a=item.action;
  if(!a)return null;
  if(u.pending?.kind==='save')return 'Guardando EEPROM';
  if([74,75,77,86,87].includes(a) && actionState(u,a))return null;
  if([5,8,11,15,77,86,87].includes(a)){
    if(!u.values[36])return 'Habilitar motores';
    if(u.radar || u.hallMeasuring)return 'Servicio ocupado';
    if([8,11,77,87].includes(a)){
      if(!poseReady(u))return 'Calibrar CPR Hall';
      if([8,77].includes(a) && !u.lineCalibrated)return 'Calibrar linea';
      if([11,87].includes(a) && !u.values[9] && !u.values[10])return 'Objetivo no nulo';
    }
    if([15,86].includes(a) && Math.abs(u.values[13])>u.values[31])return 'Limite PWM';
  }
  if(a===16 && (!u.values[39] || u.radar))return u.radar?'Servicio ocupado':'Hall deshabilitado';
  if(a===17 && (!u.hallMeasuring || !u.hallCount))return 'Medicion invalida';
  if(a===18 && (u.hallMeasuring || !u.hallPending))return 'Finalizar vuelta';
  if(u.pending && ['wifi','scan','web','mode','credentials'].includes(u.pending.kind) && [43,44,45,46,47,48,49,50,51,52,91,92,93,94].includes(a))return 'Solicitud en curso';
  if([27,28].includes(a) && (!u.values[25] || u.scenario==='missing'))return 'Esperando IR';
  if(a===49 && !u.scanned)return 'Buscar redes';
  if(a===47 && !u.wifiDraftReady)return 'Falta borrador';
  if([37,38,39,40].includes(a))return 'CAD nominal fijo';
  if(a===84 && u.scenario==='missing')return 'Enlace ESP';
  if(a===84 && u.pending?.kind==='esp')return 'Prueba en curso';
  if([23,24,83,91,92,52,46].includes(a) && !u.stopped)return 'Servicio ocupado';
  if(a===18 && !u.stopped)return 'Servicio ocupado';
  if(a===24 && u.hallMeasuring)return 'Servicio ocupado';
  if(a===46 && u.connected)return 'Ya conectado';
  if(a===46 && u.values[44]===2)return 'Cambiar modo STA';
  if([12,13,9,10].includes(a) && (!u.stopped || u.hallMeasuring))return 'Servicio ocupado';
  if((persistent.includes(a) || [63,64,65,66,67].includes(a)) && (!u.stopped || u.hallMeasuring) && !(a===36 && u.values[36]))return 'Servicio ocupado';
  if([59,60,61,74,75,78].includes(a) && (!u.stopped || u.hallMeasuring))return 'Servicio ocupado';
  if(a===68 && (u.servoEnabled || u.radar))return 'Desactivar servo';
  return null;
}
function schedule(u:UiSimulation,kind:Pending['kind'],value=0){u.pending={kind,at:u.clockMs+600,value};}
/** Returns service acceptance; presentation/transition belongs to Apply. */
function invoke(u:UiSimulation,item:FirmwareMenuItem,value:number):boolean{
  const a=item.action,reason=simulationUnavailable(u,item);
  if(reason){notify(u,reason,true);return false;}
  if((a===63 && value>=u.values[64]) || (a===64 && value<=u.values[63])){notify(u,'Dato incorrecto',true);return false;}
  if(item.step)u.values[a]=value;
  switch(a){
    case 1:stop(u);break;
    case 5:case 8:case 11:case 15:case 77:case 86:case 87:
      if([77,86,87].includes(a) && actionState(u,a)){stop(u);break;}
      u.stopped=false;u.mode=[8,77].includes(a)?MODE.line:[11,87].includes(a)?MODE.goto:[15,86].includes(a)?MODE.wheel:MODE.pwm;
      u.motionUntil=u.clockMs+(u.mode===MODE.wheel?u.values[14]:u.mode===MODE.pwm?500:u.mode===MODE.line?30000:60000);break;
    case 12:u.values[12]=value;u.hallCount=0;break;
    case 16:stop(u);u.hallMeasuring=true;u.hallCount=0;u.hallCpr[u.values[12]]=0;u.hallPending&=~(1<<u.values[12]);break;
    case 17:u.hallMeasuring=false;u.hallDraft[u.values[12]]=u.hallCount;u.hallPending|=1<<u.values[12];break;
    case 18:schedule(u,'save',18);break;
    case 19:stop(u);u.hallMeasuring=false;u.hallCount=0;if(value===2){u.hallPending&=~(1<<u.values[12]);u.hallDraft[u.values[12]]=0;}else if(value!==1){u.hallPending=0;u.hallDraft.fill(0);}break;
    case 23:schedule(u,'save',23);break;
    case 24:for(const key of persistent)u.values[key]=u.saved[key];u.lineCalibrated=u.savedLineCalibrated;break;
    case 4:u.values[6]=Math.min(value,u.values[6]);break;
    case 25:case 26:u.lineCalibrated=false;u.lineSamples=0;break;
    case 27:u.lineSamples=1;u.lineCalibrated=false;break;
    case 28:if(u.lineSamples!==1){notify(u,'Tomar fondo',true);return false;}u.lineSamples=3;break;
    case 29:if(!u.values[25] || u.lineSamples!==3){notify(u,'Ver requisitos',true);return false;}u.lineCalibrated=true;break;
    case 45:u.wifiPage=0;u.scanFailed=false;schedule(u,'scan');break;
    case 94:u.wifiPage=value;schedule(u,'scan');break;
    case 49:u.wifiRequestedMode=u.values[44]===3?3:1;u.draftSsid=sampleNetworks[u.wifiPage*8+value];u.wifiDraftReady=false;break;
    case 51:if(!u.webClient){notify(u,'Conectados',true,'No hay clientes');u.noticeTarget=S.AFR2_N_WIFI_NETWORKS;}else schedule(u,'web');break;
    case 47:schedule(u,'mode',u.wifiRequestedMode);break;
    case 46:schedule(u,'wifi',1);break;
    case 91:schedule(u,'wifi',0);break;
    case 48:u.draftSsid='';u.password='';u.wifiDraftReady=false;break;
    case 52:case 92:schedule(u,'mode',value);break;
    case 93:schedule(u,'credentials');break;
    case 60:u.values[59]=90;u.servoEnabled=true;u.radar=false;break;
    case 21:u.values[59]=Math.round((u.values[20]-1000)*180/1000);u.servoEnabled=true;break;
    case 22:case 62:u.servoEnabled=false;u.radar=false;break;
    case 59:u.values[59]=value;u.servoEnabled=true;break;
    case 61:u.radar=true;u.servoEnabled=true;break;
    case 69:u.batteryMeasured=true;break;
    case 70:case 71:u.values[a]=value;break;
    case 73:for(const key of [70,71])u.values[key]=u.saved[key];break;
    case 74:u.radar=!u.radar;u.servoEnabled=u.radar;break;
    case 75:u.servoEnabled=!u.servoEnabled;if(!u.servoEnabled)u.radar=false;break;
    case 76:u.values[36]=Number(!u.values[36]);if(!u.values[36])stop(u);break;
    case 78:u.resumeRadar=u.radar;u.radar=false;u.servoEnabled=true;break;
    case 80:if(value===0)stop(u);else if(!u.stopped){notify(u,'Servicio ocupado',true);return false;}else u.mode=1;break;
    case 83:u.values[83]=value;break;
    case 84:u.diagState=1;schedule(u,'esp');break;
    case 57:{const saved={...u.saved},clock=u.clockMs,cpr=[...u.hallCpr],calibrated=u.savedLineCalibrated;Object.assign(u,createUiSimulation(u.scenario),{values:saved,saved:{...saved},clockMs:clock,hallCpr:cpr,lineCalibrated:calibrated,savedLineCalibrated:calibrated});break;}
    case 58:u.connected=false;break;
  }
  u.feedback=`${item.label}: respuesta del equipo ficticio; sin comunicacion con el robot.`;
  return true;
}
const actionItem=(action:number,label='Operacion'):FirmwareMenuItem=>({action,label,target:255,arg:0,min:0,max:0,step:0});
function tick(u:UiSimulation,ms:number){
  u.clockMs+=ms;if(u.dashboardHeld)u.holdProgress=Math.min(16,Math.floor((u.clockMs-(u.holdStartedMs??u.clockMs))*16/(u.values[81]||800)));if(u.dashboardHeld)u.dashboardInputMs=u.clockMs;
  if(u.hallMeasuring)u.hallCount=Math.min(1000000,u.hallCount+Math.floor(u.clockMs/40)-Math.floor((u.clockMs-ms)/40));
  if(u.motionUntil && u.clockMs>=u.motionUntil)stop(u);
  if(u.quickPending && u.clockMs>=u.quickUntil)u.quickPending=false;
  if(u.pending && u.clockMs>=u.pending.at){
    const pending=u.pending;u.pending=null;
    switch(pending.kind){
      case 'save':
        if(pending.value===18){for(let i=0;i<4;i++)if(u.hallPending&(1<<i))u.hallCpr[i]=u.hallDraft[i];u.cpr=u.hallCpr[u.values[12]];u.hallPending=0;u.hallCount=0;}
        u.saved={...u.values};u.savedLineCalibrated=u.lineCalibrated;u.prefsLoaded=true;
        if(u.confirmWait)leaveConfirm(u);notify(u,'Guardados',false,'Cambios',2500);break;
      case 'scan':u.scanFailed=u.scanFixture==='failed';u.scanned=!u.scanFailed;notify(u,u.scanFailed?'Operacion fallida':u.scanFixture==='empty'?'Sin redes':'Redes listas',u.scanFailed,'Wi-Fi',2500);if(!u.scanFailed)u.noticeTarget=S.AFR2_N_WIFI_NETWORKS;break;
      case 'web':u.draftSsid='Red demo Web';u.wifiDraftReady=true;open(u,S.AFR2_N_WIFI_REVIEW);notify(u,'Datos listos',false,'Wi-Fi',2500);break;
      case 'wifi':u.connected=!!pending.value;u.wifiDraftReady=false;notify(u,u.connected?'Conectado':'Red desconectada',false,'Operacion exitosa',2500);break;
      case 'mode':u.values[44]=pending.value;u.wifiRequestedMode=pending.value;u.connected=false;u.scanned=false;u.wifiDraftReady=false;u.password='';if(dirty(u))confirm(u,23,24,S.AFR2_N_MENU);else open(u,S.AFR2_N_MENU);notify(u,'ESP reiniciado',false,'Wi-Fi',2500);break;
      case 'credentials':u.draftSsid='Red demo guardada';u.password='demo1234';u.credentialsUntil=u.clockMs+30000;open(u,S.AFR2_N_WIFI_DATA);break;
      case 'esp':u.diagState=2;notify(u,'Respuesta recibida',false,'Prueba ESP',2500);break;
    }
  }
  if(u.notice && u.clockMs-u.noticeStartedMs>=u.notice.ms)dismiss(u);
  const count=simulationItems(u).length;
  if(count && u.node!==S.AFR2_N_CAPS && u.node!==S.AFR2_N_NETWORK && u.selection>=count){u.selection=count-1;u.firstVisible=Math.max(0,u.selection-2);sync(u);}
  if(u.credentialsUntil && (u.clockMs>=u.credentialsUntil || u.node!==S.AFR2_N_WIFI_DATA)){u.password='';u.credentialsUntil=0;}
}
export function reduceUiSimulation(previous:UiSimulation,event:SimulationEvent):UiSimulation{
  const u:UiSimulation={...previous,history:[...previous.history],values:{...previous.values},hallCpr:[...previous.hallCpr],hallDraft:[...previous.hallDraft]};
  if(event.type==='scan-fixture'){u.scanFixture=event.outcome;u.scanFailed=false;u.scanned=false;u.wifiPage=0;sync(u);return u;}
  if(event.type==='input-held'){if(event.held && !u.dashboardHeld)u.holdStartedMs=u.clockMs;if(!event.held)u.holdProgress=0;u.dashboardHeld=event.held;u.dashboardInputMs=u.clockMs;return u;}
  if(event.type==='rotate' || event.type==='press')u.dashboardInputMs=u.clockMs;
  if(event.type==='web-client'){u.webClient=event.connected;return u;}
  if(event.type==='inspect'){
    const code=Number(event.code);
    u.notice=null;u.noticeTarget=null;u.keyboard=false;u.edit=false;u.editing=null;u.touch=undefined;u.password='';u.credentialsUntil=0;
    if(code>0x040000 && code<=0x040000+S.AFR2_NET_EVENT_MAX){u.notice={title:'Red simulada',text:'192.0.2.2',kind:16+code-0x040000,ms:3000};u.noticeStartedMs=u.clockMs;}
    else if(code===0x030035)u.touch={direction:8,heading:0,startedMs:u.clockMs,full:true};
    else if(code===0x030044){open(u,S.AFR2_N_HALL_PICK);u.selection=u.values[12];u.history=[];}
    else {const node=L.nodes.findIndex(row=>Number(row[5])===code);if(node<0)return previous;open(u,node);u.history=[];
      if(node===S.AFR2_N_SAVE_CONFIRM){u.confirmTarget=S.AFR2_N_HALL_TEST;u.confirmSave=18;u.confirmDiscard=19;u.confirmWait=false;}
      if(node===S.AFR2_N_HALL_COUNT){u.hallMeasuring=!!u.values[39];u.hallCount=0;}}
    u.feedback='Inspeccion directa: salto de catalogo, sin historial; datos ficticios.';return u;
  }
  if(event.type==='tick'){tick(u,event.ms);return u;}
  const rotation=event.type==='rotate'?event.delta*(u.values[42]?-1:1):0,long=event.type==='press' && event.kind==='long',click=event.type==='press' && event.kind==='short';
  if(u.touch){if(long){stop(u);u.touch=undefined;}return u;}
  // Apply gives the physical encoder test precedence, even over notices.
  if(u.node===S.AFR2_N_ENCODER_TEST){if(rotation)u.testBack=true;if(click){if(u.testBack)goBack(u);else u.testShort=true;}if(long){if(u.testLong)goBack(u);else u.testLong=true;}return u;}
  u.turns+=rotation;u.clicks=Math.min(65535,u.clicks+Number(click));u.holds=Math.min(65535,u.holds+Number(long));
  if(u.node===S.AFR2_N_SAVE_CONFIRM){
    if(u.notice){if(click)dismiss(u);return u;}if(u.confirmWait)return u;
    if(rotation)u.selection^=1;
    if(click && invoke(u,actionItem(u.selection?u.confirmDiscard:u.confirmSave),u.selection && u.confirmSave===18?2:0)){
      if(!u.selection)u.confirmWait=true;else{leaveConfirm(u);notify(u,'Descartados',false,'Cambios',2500);}}
    return u;
  }
  if(u.node===S.AFR2_N_HALL_PICK || u.node===S.AFR2_N_HALL_COUNT){
    if(long){invoke(u,actionItem(19),1);u.notice=null;back(u,S.AFR2_N_HALL_TEST);notify(u,'Cancelada',false,'Operacion',2500);return u;}
    if(u.notice){if(click)dismiss(u);return u;}
    if(u.node===S.AFR2_N_HALL_PICK){if(rotation)u.selection=wrap(u.selection+rotation,4);
      if(click && invoke(u,actionItem(12),u.selection) && invoke(u,actionItem(16),0)){u.node=S.AFR2_N_HALL_COUNT;u.selection=0;sync(u);}}
    else if(click){if(invoke(u,actionItem(17),0))confirm(u,18,19,S.AFR2_N_HALL_TEST);else notify(u,'Medicion invalida',true,'Operacion',2500);}
    return u;
  }
  if(u.keyboard){
    if(long){u.keyboard=false;u.ssid='';u.password='';u.length=0;u.notice=null;return u;}
    if(u.notice){if(click)dismiss(u);return u;}
    if(rotation)u.key=wrap(u.key+rotation,97);
    if(click){let text=u.keyboardField?u.password:u.ssid;
      if(u.key<95 && text.length<(u.keyboardField?63:32))text+=String.fromCharCode(32+u.key);
      else if(u.key===95)text=text.slice(0,-1);
      else if(u.key===96){
        if(!u.keyboardField){if(!text.length){notify(u,'Dato incorrecto',true);return u;}u.keyboardField=true;u.length=0;u.key=0;return u;}
        if(text.length && text.length<8){notify(u,'Dato incorrecto',true);return u;}
        u.draftSsid=u.ssid;u.wifiDraftReady=true;u.keyboard=false;u.ssid='';u.password='';open(u,S.AFR2_N_WIFI_REVIEW);notify(u);return u;}
      if(u.keyboardField)u.password=text;else u.ssid=text;u.length=text.length;}
    return u;
  }
  // AFR2-20261106-09: same as AVR — on QR, short or long always leaves and drops queued notices.
  if((u.node===S.AFR2_N_QR_AP || u.node===S.AFR2_N_QR_WEB) && (click || long)){u.notice=null;u.noticeTarget=null;goBack(u);return u;}
  if(long){goBack(u);return u;}
  if(u.notice){if(click)dismiss(u);return u;}
  if(u.node===S.AFR2_N_HOME){if(click)open(u,S.AFR2_N_MENU);return u;}
  if(u.node===S.AFR2_N_WIFI_DATA || u.node===S.AFR2_N_QR_AP || u.node===S.AFR2_N_QR_WEB){if(click)goBack(u);return u;}
  if([S.AFR2_N_NETWORK,S.AFR2_N_CAPS,S.AFR2_N_WIFI_MODE_SELECT].includes(u.node)){
    if(rotation)u.selection=limit(u.selection+rotation,0,u.node===S.AFR2_N_NETWORK?1:u.node===S.AFR2_N_CAPS?5:2);
    if(click){if(u.node===S.AFR2_N_WIFI_MODE_SELECT){if(invoke(u,actionItem(92),u.selection+1))notify(u,'Esperando ESP',false,'Cambiar modo',1500);}else goBack(u);}return u;
  }
  if(u.node===S.AFR2_N_QUICK){
    if(u.quickPending)return u;
    if(rotation){u.selection=limit(u.selection+rotation,0,6);u.quickSelection=u.selection;sync(u);}
    if(click){if(u.selection===5)open(u,S.AFR2_N_MENU);else if(u.selection===6)open(u,S.AFR2_N_QUICK_MODES);
      else{const item=simulationItems(u)[u.selection],expected=!actionState(u,item.action);
        if(invoke(u,item,Number(expected))){u.quickPending=true;u.quickExpected=expected;u.quickUntil=u.clockMs+40;}}}return u;
  }
  if(u.edit && u.editing){
    if(rotation)u.draft=u.editing.action===12?wrap(u.draft+rotation,4):limit(u.draft+rotation*u.editing.step,u.editing.min,u.editing.max);
    if(click){const accepted=invoke(u,u.editing,u.draft);u.edit=false;u.editing=null;if(accepted)notify(u);}return u;
  }
  const sensors=[S.AFR2_N_RADAR_VIEW,S.AFR2_N_US_VIEW,S.AFR2_N_LINE_VIEW,S.AFR2_N_IR_VALUES,S.AFR2_N_SERVO_ANGLE];
  if(sensors.includes(u.node)){
    if(click){if([S.AFR2_N_RADAR_VIEW,S.AFR2_N_US_VIEW].includes(u.node))open(u,S.AFR2_N_RADAR_OPTIONS);return u;}
    if(rotation){if(u.node===S.AFR2_N_SERVO_ANGLE)invoke(u,actionItem(59),limit(u.values[59]-rotation,0,180)); /* AFR2-20261106-35: mismo sentido invertido que el ATmega. */
      else if(rotation>0 && u.node===S.AFR2_N_RADAR_VIEW)u.node=S.AFR2_N_US_VIEW;
      else if(rotation<0 && u.node===S.AFR2_N_US_VIEW)u.node=S.AFR2_N_RADAR_VIEW;
      else if(rotation>0 && u.node===S.AFR2_N_LINE_VIEW)u.node=S.AFR2_N_IR_VALUES;
      else if(rotation<0 && u.node===S.AFR2_N_IR_VALUES)u.node=S.AFR2_N_LINE_VIEW;sync(u);}return u;
  }
  const count=simulationItems(u).length;
  if(rotation && count){u.selection=wrap(u.selection+rotation,count);const visible=Number(firmwareNode(u.node)[2])?1:3;
    if(u.selection<u.firstVisible)u.firstVisible=u.selection;if(u.selection>=u.firstVisible+visible)u.firstVisible=u.selection-visible+1;sync(u);}
  if(click){
    const item=simulationItems(u)[u.selection];if(!item)return u;
    const reason=simulationUnavailable(u,item);if(reason){notify(u,reason,true);return u;}
    if(!item.action){if(item.label.startsWith('Volver'))goBack(u);else{
      if(item.target===S.AFR2_N_SERVO_ANGLE && !invoke(u,actionItem(78),0))return u;
      open(u,item.target);if(item.target===S.AFR2_N_RADAR_RANGE){u.selection=u.values[70];u.firstVisible=Math.max(0,u.selection-2);}}}
    else if(item.action===16){open(u,S.AFR2_N_HALL_PICK);u.selection=u.values[12];}
    else if(item.action===50){u.keyboard=true;u.keyboardField=false;u.ssid=u.draftSsid;u.password='';u.length=u.ssid.length;u.key=0;}
    else if(item.step){u.edit=true;u.editing=item;u.editingAction=item.action;u.editingLabel=item.label;u.draft=u.values[item.action]??item.min;}
    else if(invoke(u,item,item.arg)){
      if(item.action===49)open(u,S.AFR2_N_WIFI_SETUP);
      else if(item.action===80)notify(u,item.arg?'Manual':'Reposo',false,'Modo seleccionado',2500);
      else if(item.action===70)back(u,S.AFR2_N_RADAR_OPTIONS);
      else if(![18,51,84].includes(item.action))notify(u);
    }
  }
  // Match Render's clamp after dynamic menu counts shrink.
  const nextCount=simulationItems(u).length;
  if(nextCount && u.node!==S.AFR2_N_CAPS && u.node!==S.AFR2_N_NETWORK && u.selection>=nextCount){u.selection=nextCount-1;u.firstVisible=Math.max(0,u.selection-2);}
  return u;
}

export function simulationSnapshot(u: UiSimulation): FirmwareSnapshot {
  const v = u.values, ready = u.scenario === 'ready', hall = !!v[S.AFR2_A_HALL_WIRED], pose = poseReady(u);
  const angle = u.radar ? Math.round(v[S.AFR2_A_RADAR_MIN] + (v[S.AFR2_A_RADAR_MAX] - v[S.AFR2_A_RADAR_MIN]) * (1 - Math.cos(u.clockMs / 3000)) / 2) : v[S.AFR2_A_SERVO_ANGLE];
  const pulse = u.servoEnabled ? 1000 + Math.round(angle * 1000 / 180) : 0;
  const pwm = Array.from({ length: 4 }, (_, i) => !u.stopped && i === v[S.AFR2_A_WHEEL] ? v[S.AFR2_A_TEST_PWM] : 0);
  const lines: Record<number, string[]> = {
    [S.AFR2_N_HOME]: [["Reposo","MANUAL","SEGUIR LINEA","PRUEBA DE RUEDA","GOTO","FALLO","SECUENCIA","PWM MANUAL"][u.mode] ?? "Modo: --", "", ""],
    [S.AFR2_N_ROBOT_VIEW]: [`Modo:${u.mode} Ctrl:0`, `Salidas OFF:${Number(u.stopped)}`, 'Estado de salidas'],
    [S.AFR2_N_LINK_VIEW]: ['UART:1 Ext:1', 'Origen:0 Web:0', 'XOR mal:0'],
    [S.AFR2_N_ALERT_VIEW]: ['Fallo:0 Reset:0', 'OLED err:0', 'RX gaps:0'],
    [S.AFR2_N_DIAG]: ['Fallo:0 Reset:0', 'OLED err:0', 'RX gaps:0'],
    [S.AFR2_N_DISPLAY]: ['OLED listo:1', 'Errores:0', 'TWI IRQ 100kHz'],
    [S.AFR2_N_ENCODER_VIEW]: [`Giros:${u.turns}`, `Clics:${u.clicks}`, `Largos:${u.holds}`],
    [S.AFR2_N_HALL_VIEW]: hall ? [`Rueda:${v[S.AFR2_A_WHEEL]} C:${u.hallCount}`, `Vuelta:${u.hallCount} E:0`, `CPR:${u.cpr}`] : ['Hall sin conectar', 'A8..A15 propuesto', 'Captura bloqueada'],
    // AFR2-20261106-07: no simulated wheel turning; zero after entry, same format as AVR.
    [S.AFR2_N_HALL_LIVE]: ['FL     0 FR     0', 'RL     0 RR     0', hall ? 'Gire a mano' : 'Hall deshabilitado'],
    [S.AFR2_N_WHEEL_VIEW]: [`FL:${pwm[0]} FR:${pwm[1]}`, `RL:${pwm[2]} RR:${pwm[3]}`, `OFF:${Number(u.stopped)}`],
    [S.AFR2_N_LINE_REQ]: [`Linea valida:${Number(u.lineCalibrated)}`, `Hall:${Number(hall)} Geom:${v[S.AFR2_A_GEOM_READY]}`, `Motor habilit:${v[S.AFR2_A_MOTOR_READY]}`],
    [S.AFR2_N_GOTO_REVIEW]: [`X:${v[S.AFR2_A_GOTO_X]} mm`, `Y:${v[S.AFR2_A_GOTO_Y]} mm`, `V max:${v[S.AFR2_A_SPEED]} mm/s`],
    [S.AFR2_N_GOTO_RESULT]: [`Estado GOTO:${u.mode === MODE.goto ? 1 : 0}`, `Pose valida:${Number(pose)}`, `Salidas OFF:${Number(u.stopped)}`],
    [S.AFR2_N_POSE]: pose ? ['X:0 Y:0', 'Yaw:0 mrad', 'Estimacion ruedas'] : ['Sin odometria', 'Conectar Hall', 'Calibrar geometria'],
    [S.AFR2_N_WIFI_VIEW]: [`Modo:${v[S.AFR2_A_WIFI_MODE]} STA:${Number(u.connected)}`, u.connected ? u.draftSsid : '', u.connected ? '192.0.2.2' : '192.0.2.1'],
    [S.AFR2_N_WIFI_REVIEW]: [u.draftSsid, `Modo:${v[S.AFR2_A_WIFI_MODE]} Listo:${Number(u.wifiDraftReady)}`, 'Giro: opciones'],
    [S.AFR2_N_VERSIONS]: ['Mega demo UI', 'ESP:demo', 'Web:demo'],
    [S.AFR2_N_CAPS]: ['UI/TWI/UART/STOP', `Linea:${Number(u.lineCalibrated)} Hall:${Number(hall)}`, `GOTO req:${Number(pose)}`],
    [S.AFR2_N_PREFS_VIEW]: [`EEPROM valida:${Number(u.prefsLoaded)}`, `Guardado estado:${u.prefsLoaded ? 2 : 0}`, 'CRC/version 10'],
  };
  const items = simulationItems(u).slice(u.firstVisible, u.firstVisible + 3);
  if(u.node===S.AFR2_N_SAVE_CONFIRM && u.confirmSave===18)lines[u.node]=[`Sensor Hall ${['FL','FR','RL','RR'][v[12]]}`,`Pulsos:${u.hallDraft[v[12]]}`,''];
  lines[S.AFR2_N_HALL_COUNT] = [`Rueda ${v[12]+1}`, `Pulsos:${u.hallCount}`, ''];
  lines[S.AFR2_N_WIFI_VIEW] = [u.connected?'Conectado':'Desconectado','',''];
  lines[S.AFR2_N_NETWORK] = u.selection ? ['Mascara:', '255.255.255.0', 'GW:192.0.2.1'] : [`Modo: ${['','STA','AP','AP-STA'][v[44]]}`, u.connected?'IP STA:':'IP AP:', u.connected?'192.0.2.2':'192.0.2.1'];
  lines[S.AFR2_N_WIFI_DATA] = ['',u.connected?'Senal: -52 dBm':'Senal: --','WPA2'];
  lines[S.AFR2_N_BATTERY_VIEW] = u.batteryMeasured && ready ? ['Bateria:7.40 V','Edad:0 ms','Sin calibrar'] : ['Bateria: -- V',u.batteryMeasured?'Midiendo...':'Realizar medicion','Desde Bateria'];
  lines[S.AFR2_N_GEOMETRY] = [`Rueda:${v[37]} mm`,`Lx+Ly:${v[38]} mm`,'CAD nominal'];
  lines[S.AFR2_N_CAPS] = [
    ['UI: encoder/OLED','Dibujo por TWI','OLED listo:1'],
    ['UART: puente ESP','Wi-Fi/HTTP/WS',`Enlace EX01:${Number(ready)}`],
    ['Sensores: IR/HC',`Linea:${Number(u.lineCalibrated)} Eco:${Number(ready)}`,`Hall disponible:${Number(hall)}`],
    ['PWM: sin odometria','STOP corta salidas',`Motor:${v[36]} Fallo:0`],
    ['GOTO requiere pose','CPR y CAD nominal',`Pose valida:${Number(pose)}`],
    ['Secuencias: pasos',`Sec:0 EEPROM:${Number(u.prefsLoaded)}`,'Guardar explicito'],
  ][u.selection] ?? [];
  return {
    ...menuSnapshot(u),
    lines: (lines[u.node] ?? ['', '', '']).map(line => line.slice(0, 18)),
    labels: u.node === S.AFR2_N_WIFI_DATA ? [u.draftSsid || 'AFR2-DEMO',u.credentialsUntil?u.password:'',''] : u.node === S.AFR2_N_WIFI_NETWORKS ? items.map(item => item.action === S.AFR2_A_WIFI_NETWORK && u.scanned ? item.label : '') : ['', '', ''],
    rowSecurity: u.node===S.AFR2_N_WIFI_NETWORKS ? items.map(item=>item.action===49?4:0) : [4,0,0],
    hallCpr: u.hallCpr.map((value,i)=>u.hallPending&(1<<i)?u.hallDraft[i]:value), hallPending:u.hallPending,
    quickFlags:[74,75,76,77,71].reduce((bits,action,i)=>bits|(Number(actionState(u,action))<<i),0),
    radarRange:v[70],radarTrace:!!v[71],powerKnown:true,powerPercent:v[83],
    dashboardValid:true,dashboardMode:u.mode,dashboardMoving:!u.stopped,dashboardFault:u.mode===MODE.fault,
    batteryValid:u.batteryMeasured && ready,batteryMv:u.batteryMeasured && ready?7400:undefined,
    disabled: items.map(item => Number(!!simulationUnavailable(u, item))),
    sequenceState: u.clockMs - u.scrollStartedMs >= 9000 ? 4 : 3, sequenceCount: 3,
    sequenceIndex: Math.min(2, Math.floor((u.clockMs - u.scrollStartedMs) / 3000)),
    sequenceKind: Math.min(2, Math.floor((u.clockMs - u.scrollStartedMs) / 3000)),
    sequenceDurationMs: 3000, sequenceElapsedMs: (u.clockMs - u.scrollStartedMs) % 3000,
    networkServer: ready,qrAllowed:u.stopped && !u.radar,networkValid: ready, networkSta: u.connected && v[S.AFR2_A_WIFI_MODE] !== 2, networkAp: v[S.AFR2_A_WIFI_MODE] !== 1,
    networkSsid: u.draftSsid, networkApSsid: 'AFR2-DEMO', networkPassword: 'demo1234', networkIp: '192.0.2.2', networkApIp: '192.0.2.1',
    irRaw: ready ? (u.lineSamples===1?[1,1,1,1,1]:u.lineSamples===3?[0,0,0,0,0]:[1,1,0,1,1]) : [],
    lineBits:u.lineSamples===1?0:u.lineSamples===3?31:4,irKind: v[S.AFR2_A_LINE_KIND], irValid: ready && v[S.AFR2_A_LINE_KIND] !== 0, lineValid: ready && u.lineCalibrated,
    sensor: { nowMs: u.clockMs, angle, pulseUs: pulse, targetUs: pulse, enabled: u.servoEnabled, running: u.radar,
      latest: { id: ready ? 1 : 0, ms: u.clockMs, distanceMm: 720, error: 0 },
      bins: Array.from({ length: 37 }, (_, i) => ({ id: ready && u.radar ? 1 : 0, distanceMm: 700 + (i % 7) * 140, error: 0 })),
    },
  };
}
