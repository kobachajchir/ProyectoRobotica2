import { useMemo, type ReactNode } from 'react';
import { SIMULATION_FLOW } from './uiSimulationFlow';
/** AFR2-20261101-02. Expandable catalogue graph; repeated destinations are
 * references so context-dependent returns cannot create recursive cycles. */
export function UiSimulationTree(){
  const tree=useMemo(()=>{
    const seen=new Set<number>();
    function branch(node:number,label?:string):ReactNode {
      const entry=SIMULATION_FLOW[node];if(!entry)return null;
      if(seen.has(node))return <li key={`${node}-${label}`}>{label??entry.title} → {entry.title} <code>{entry.code}</code> (compartida)</li>;
      seen.add(node);
      const transitions=node===0?[{label:'SW corto',target:1},{label:'SW largo al soltar',target:58}]
        :node===49 || node===23?[{label:'SW corto: opciones',target:59}]
        :node===53?[{label:'SW corto: iniciar conteo',target:54}]:[];
      return <li key={node}><details><summary>{label??entry.title} <code>{entry.code}</code></summary>
        <p>Corto: {entry.gestures.short} Largo: {entry.gestures.long}</p>
        <ul>{entry.options.map((item,i)=>item.action || item.target===255 || item.label.startsWith('Volver')
          ?<li key={`${node}-${i}`}>{item.label}{item.step?` → editor (${item.min} a ${item.max}, paso ${item.step})`:item.action?' → respuesta local':''}</li>
          :branch(item.target,item.label))}{transitions.map(item=>branch(item.target,item.label))}</ul>
      </details></li>;
    }
    const main=branch(0);
    const remaining=SIMULATION_FLOW.filter(entry=>!seen.has(entry.node)).map(entry=>branch(entry.node));
    return <><ul>{main}</ul><h4>Vistas por acciones, eventos o inspeccion</h4><ul>{remaining}</ul></>;
  },[]);
  return <div className="afr2-ui-simulator-flow">
    <p>Arbol del catalogo AVR. Las pantallas compartidas aparecen como referencia; el retorno real conserva el contexto de entrada. Se incluyen las filas condicionales de cambios pendientes y ocho redes de ejemplo.</p>
    <p>Hall: Iniciar conteo → elegir motor → conteo → Hall. Wi-Fi: elegir red → Credenciales; teclado o entrega Web → Configurar; consulta local de credenciales → Datos de red. Al salir con borradores se abre Si/No. Conectado sustituye Conectar por Desconectar y oculta Reintentar.</p>
    {tree}
  </div>;
}