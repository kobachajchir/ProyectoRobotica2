/** AFR2-20261102-05. Interactive local UI services, shared AVR drawing library. */
import { useCallback,useEffect,useMemo,useRef,useState } from 'react';
import { createUiSimulation,reduceUiSimulation,simulationItems,simulationScreenCode,simulationSnapshot,type SimulationEvent,type SimulationScenario } from '../developer/uiSimulation';
import { simulationGestures } from '../developer/uiSimulationFlow';
import { firmwareNode,renderFirmwareScreen } from './firmwareRenderer';
import { clearDashboard } from './dashboardRenderer';
import { Afr2OledBuffer } from './oledRasterizer';
import { OledWorkbench } from './OledWorkbench';
import { SegmentedControl } from '../../components/ui/SegmentedControl';
import { OledCanvas128x64 } from './OledCanvas128x64';
import { AFR2_SCREEN_META,type Afr2ScreenCode } from './screenCatalog';
import { FIRMWARE_LIBRARY as L } from './generated/firmwareLibrary';

export function UiSimulationPreview({initialCode,onNavigate,active=true}:{initialCode:Afr2ScreenCode;onNavigate:(code:Afr2ScreenCode)=>void;active?:boolean}){
  const [u,setUi]=useState(()=>reduceUiSimulation(createUiSimulation(),{type:'inspect',code:initialCode}));
  const dispatch=useCallback((event:SimulationEvent)=>setUi(previous=>reduceUiSimulation(previous,event)),[]);
  const held=useCallback((pressed:boolean)=>dispatch({type:'input-held',held:pressed}),[dispatch]);
  const raster=useRef(new Afr2OledBuffer());
  const lastFrame=useRef(new Afr2OledBuffer());
  const code=simulationScreenCode(u) as Afr2ScreenCode;
  const lastCode=useRef(initialCode);
  useEffect(()=>{if(code!==lastCode.current && AFR2_SCREEN_META[code]){lastCode.current=code;onNavigate(code);}},[code,onNavigate]);
  useEffect(()=>{
    if(!active)return;
    const timer=window.setInterval(()=>{if(document.visibilityState==='visible')dispatch({type:'tick',ms:40});},40);
    return ()=>window.clearInterval(timer);
  },[active,dispatch]);
  const snapshot=useMemo(()=>simulationSnapshot(u),[u]);
  const buffer=useMemo(()=>{const changed=renderFirmwareScreen({...u,redraw:true,scrollRedraw:false},raster.current,snapshot);if(changed===false)return lastFrame.current;const frame=new Afr2OledBuffer();frame.buffer.set(raster.current.buffer);lastFrame.current=frame;return frame;},[u,snapshot]);
  const items=simulationItems(u),gestures=simulationGestures(u);
  const node=firmwareNode(u.node),menu=!Number(node[2]) && !u.edit && !u.keyboard && !u.notice;
  const visible=u.node===L.symbols.AFR2_N_SAVE_CONFIRM?2:menu?3:1;
  const move=(delta:number)=>{for(let i=0;i<Math.min(3,Math.abs(delta));i++)dispatch({type:'rotate',delta:delta<0?-1:1});};
  const changeScenario=(scenario:SimulationScenario)=>{clearDashboard(raster.current);setUi(reduceUiSimulation(createUiSimulation(scenario),{type:'inspect',code}));};
  const [lastInput,setLastInput]=useState('ninguna');
  const rotate=(delta:-1|1)=>{setLastInput(delta<0?'Giro izquierdo (-1)':'Giro derecho (+1)');dispatch({type:'rotate',delta});};
  return <OledWorkbench
    lastInput={lastInput}
    statusBar={<>
      <span className="afr2-status-pill" data-state="ready"><span className="afr2-status-dot" aria-hidden="true"/>Simulación local · datos ficticios</span>
      <SegmentedControl ariaLabel="Escenario de la simulación" size="sm" fill="never" value={u.scenario}
        items={[{value:'ready',label:'Con datos'},{value:'missing',label:'Sin datos'}]} onChange={scenario=>{if(active)changeScenario(scenario as SimulationScenario);}}/>
    </>}
    screen={<OledCanvas128x64 compact oledBuffer={buffer} screenCode={code} sourceMode="local-ui-simulation" showGrid={false}/>}
    side={{selection:u.selection,firstVisible:u.firstVisible,count:menu?items.length:0,visibleRows:visible,disabled:!active,
      onHighlight:index=>{setLastInput(`Resaltar opción ${index+1}`);move(index-u.selection);},
      onScroll:direction=>move(direction*Math.min(3,direction<0?u.selection:items.length-u.selection-1))}}
    caption={<>
      <p className="afr2-help">Giro: {gestures.rotate} Corto: {gestures.short} Largo: {gestures.long}</p>
      {(u.node===L.symbols.AFR2_N_QR_AP || u.node===L.symbols.AFR2_N_QR_WEB) && <p className="afr2-help">QR de ejemplo: AP AFR2-DEMO / demo1234; Web 192.0.2.1. En la OLED del auto se usan los datos vigentes del ESP.</p>}
    </>}
    encoder={{available:active,longPressMs:u.values[81],onPressingChange:held,statusMessage:u.feedback,
      onRotate:rotate,onPress:kind=>{setLastInput(kind==='long'?'SW largo (Volver al soltar)':'SW corto (Confirmar)');dispatch({type:'press',kind});}}}
  />;
}
