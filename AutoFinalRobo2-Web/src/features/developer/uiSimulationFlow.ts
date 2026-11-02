/** AFR2-20261101-02. Gesture descriptions read alongside Afr2Ui_Apply.
 * Navigation choices come from the shared AVR catalogue, not another menu. */
import { FIRMWARE_LIBRARY as L } from '../oled/generated/firmwareLibrary';
import { firmwareNode } from '../oled/firmwareRenderer';
import { createUiSimulation, simulationItems, type UiSimulation } from './uiSimulation';
const S: Record<string,number> = L.symbols;
export interface SimulationGestures { rotate:string; short:string; long:string }
export function simulationGestures(u:UiSimulation):SimulationGestures {
  const general:SimulationGestures={rotate:'Recorrer opciones de forma circular.',short:'Abrir la opción o ejecutar su acción local.',long:'Volver al contexto anterior; conserva selección y desplazamiento. Si hay cambios, exige Sí/No.'};
  if(u.touch)return {rotate:'Sin acción.',short:'Sin acción.',long:'Detener el control táctil y recuperar la pantalla anterior.'};
  if(u.node===S.AFR2_N_ENCODER_TEST)return {rotate:'Seleccionar Volver.',short:u.testBack?'Volver al contexto anterior.':'Registrar la pulsación corta; permanece en la prueba.',long:u.testLong?'Volver al contexto anterior.':'Registrar el primer largo; permanece en la prueba.'};
  if(u.node===S.AFR2_N_SAVE_CONFIRM)return {rotate:u.confirmWait?'Espera la escritura ficticia.':'Alternar Sí / No.',short:u.confirmWait?'Espera la escritura ficticia.':'Sí guarda y espera completar; No descarta. Ambos vuelven al contexto pendiente.',long:'Sin acción: no permite evitar la decisión Sí/No.'};
  if(u.node===S.AFR2_N_HALL_PICK)return {rotate:'Elegir FL / FR / RL / RR con vuelta circular.',short:'Confirmar motor e iniciar el conteo Hall.',long:'Cancelar solo este intento y volver a Hall; conserva otras mediciones pendientes.'};
  if(u.node===S.AFR2_N_HALL_COUNT)return {rotate:'Sin acción sobre el conteo. Los pulsos de ejemplo avanzan con el reloj local.',short:'Finalizar la vuelta y regresar a Hall con la medición pendiente de guardar.',long:'Cancelar solo este intento y volver a Hall; conserva otras mediciones pendientes.'};
  if(u.keyboard)return {rotate:'Elegir caracter; < borra y OK termina el campo.',short:'Escribir; OK pasa de SSID a clave y después abre Configurar.',long:'Borrar la entrada del teclado y cerrarlo; permanece en Credenciales.'};
  if(u.notice)return {rotate:'Sin acción.',short:'Cerrar aviso; si tiene destino posterior, abrirlo.',long:'Cerrar aviso sin abandonar la pantalla; respeta el destino posterior del aviso.'};
  if(u.node===S.AFR2_N_HOME)return {rotate:'Sin acción.',short:'Abrir Menú principal (010002).',long:'Abrir Controles rápidos (010003).'};
  if(u.node===S.AFR2_N_QUICK)return {rotate:'Recorrer 7 tarjetas con límites, sin vuelta circular.',short:'Alternar la tarjeta; Menú abre el menú completo y Selección de modo abre Reposo/Manual.',long:'Volver a Inicio. Si cambió el rastro del radar, exige Sí/No.'};
  if(u.edit)return {rotate:u.editingAction===12?'Elegir motor de forma circular.':`Ajustar borrador entre ${u.editing?.min} y ${u.editing?.max}, paso ${u.editing?.step}.`,short:'Aplicar borrador; mostrar respuesta y permanecer en el menú.',long:'Descartar solo el borrador y cerrar el editor; permanece en el menú.'};
  if(u.node===S.AFR2_N_WIFI_MODE_SELECT)return {...general,rotate:'Elegir STA / AP / AP-STA con límites.',short:'Solicitar el modo y esperar la respuesta ficticia del ESP.'};
  if(u.node===S.AFR2_N_NETWORK)return {...general,rotate:'Recorrer 2 páginas con límites.',short:'Volver al contexto anterior.'};
  if(u.node===S.AFR2_N_CAPS)return {...general,rotate:'Recorrer 6 páginas con límites.',short:'Volver al contexto anterior.'};
  if(u.node===S.AFR2_N_QR_AP || u.node===S.AFR2_N_QR_WEB)return {...general,rotate:'Sin acción. QR de los datos ficticios del escenario.',short:'Volver al contexto anterior.'};
  if(u.node===S.AFR2_N_WIFI_DATA)return {...general,rotate:'Sin acción.',short:'Volver al contexto anterior; borrar credenciales visibles.'};
  if(u.node===S.AFR2_N_RADAR_VIEW || u.node===S.AFR2_N_US_VIEW)return {...general,rotate:'Derecha: Radar a Numérica. Izquierda: Numérica a Radar. Los extremos no envuelven.',short:'Abrir Opciones radar: alcance del gráfico o Volver.'};
  if(u.node===S.AFR2_N_LINE_VIEW || u.node===S.AFR2_N_IR_VALUES)return {...general,rotate:'Derecha: gráfica a valores. Izquierda: valores a gráfica. Los extremos no envuelven.',short:'Sin acción en el firmware actual. Para regresar, usar largo.'};
  if(u.node===S.AFR2_N_SERVO_ANGLE)return {...general,rotate:'Ajustar ángulo entre 0 y 180 grados.',short:'Sin acción en el firmware actual. Para regresar, usar largo; reanuda radar pausado.'};
  if(u.node===S.AFR2_N_POWER)return {...general,rotate:'Sin acción.',short:'Activa la opción de retorno del catálogo.'};
  return general;
}
export interface SimulationFlowNode {node:number; title:string; code:string; parent:number; gestures:SimulationGestures; options:ReturnType<typeof simulationItems>}
/** Includes every AVR node and all dynamic rows in the ready, scanned, dirty
 * fixture. Actual on-screen options always use the current simulation state. */
export const SIMULATION_FLOW:SimulationFlowNode[]=L.nodes.map((_,node)=>{
  const u=createUiSimulation();u.node=node;u.scanned=true;u.values[53]++;
  const row=firmwareNode(node);
  return {node,title:String(row[0]),code:`0x${Number(row[5]).toString(16).padStart(6,'0')}`,parent:Number(row[1]),gestures:simulationGestures(u),options:simulationItems(u)};
});