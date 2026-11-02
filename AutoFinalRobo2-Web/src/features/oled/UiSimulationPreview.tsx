/** AFR2-20261102-05. Interactive local UI services, shared AVR drawing library. */
import { useCallback,useEffect,useMemo,useRef,useState } from 'react';
import { createUiSimulation,reduceUiSimulation,simulationItems,simulationScreenCode,simulationSnapshot,type SimulationEvent,type SimulationScenario } from '../developer/uiSimulation';
import { simulationGestures } from '../developer/uiSimulationFlow';
import { firmwareNode,renderFirmwareScreen } from './firmwareRenderer';
import { clearDashboard } from './dashboardRenderer';
import { Afr2OledBuffer } from './oledRasterizer';
import { EncoderControl } from './EncoderControl';
import { EncoderGlyph } from './EncoderGlyph';
import { MenuSideControls } from './MenuSideControls';
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
  return <div className="afr2-page-stack afr2-library-preview">
    <p className="afr2-help">Simulación local · datos ficticios. Las acciones modifican este ejemplo y permiten recorrer el flujo completo.</p>
    <label className="afr2-help">Escenario <select value={u.scenario} onChange={event=>changeScenario(event.target.value as SimulationScenario)} disabled={!active}>
      <option value="ready">Datos disponibles</option><option value="missing">Sin datos de sensores y red</option>
    </select></label>
    <ul className="afr2-gesture-legend" aria-label="Gestos del encoder">
      <li><EncoderGlyph kind="short" scale={2}/><span>Confirmar</span></li>
      <li><EncoderGlyph kind="long" scale={2}/><span>Volver al soltar</span></li>
      <li><EncoderGlyph kind="turn" scale={2}/><span>Recorrer</span></li>
    </ul>
    <div className="afr2-oled-with-side">
      <OledCanvas128x64 compact oledBuffer={buffer} screenCode={code} sourceMode="local-ui-simulation" showGrid={false}/>
      <MenuSideControls selection={u.selection} firstVisible={u.firstVisible} count={menu?items.length:0} visibleRows={visible} disabled={!active}
        onHighlight={index=>move(index-u.selection)} onScroll={direction=>move(direction*Math.min(3,direction<0?u.selection:items.length-u.selection-1))}/>
    </div>
    <EncoderControl label="Encoder · simulación UI" available={active} longPressMs={u.values[81]} onPressingChange={held} compact
      onRotate={delta=>dispatch({type:'rotate',delta})} onPress={kind=>dispatch({type:'press',kind})} statusMessage={u.feedback}/>
    <p className="afr2-help">Giro: {gestures.rotate} Corto: {gestures.short} Largo: {gestures.long}</p>
    {(u.node===L.symbols.AFR2_N_QR_AP || u.node===L.symbols.AFR2_N_QR_WEB) && <p className="afr2-help">QR de ejemplo: AP AFR2-DEMO / demo1234; Web 192.0.2.1. En la OLED del auto se usan los datos vigentes del ESP.</p>}
  </div>;
}
