import { useEffect, useId, useRef, useState } from 'react';
import { MOTOR_IDS, stepMotor, type MotorId } from '../../domain/motorSelection';
export type { MotorId } from '../../domain/motorSelection';
import './motorSelector.css';
export function MotorSelector({ value, onChange, disabled=false, onCancel }: { value:MotorId; onChange:(motor:MotorId)=>void; disabled?:boolean; onCancel?:()=>void }) {
  const title=useId(),buttons=useRef<(HTMLButtonElement|null)[]>([]);
  const [draft,setDraft]=useState(value);
  useEffect(()=>setDraft(value),[value]);
  return <section className="afr2-motor-selector" aria-labelledby={title}>
    <h3 id={title}>Elegir un motor</h3>
    <div role="radiogroup" aria-labelledby={title} onKeyDown={event=>{
      if(disabled)return;
      const delta=event.key==='ArrowRight'||event.key==='ArrowDown'?1:event.key==='ArrowLeft'||event.key==='ArrowUp'?-1:0;
      if(delta){event.preventDefault();const next=stepMotor(MOTOR_IDS.indexOf(draft),delta);setDraft(MOTOR_IDS[next]);buttons.current[next]?.focus();}
      if(event.key==='Escape'){setDraft(value);onCancel?.();}
    }}>
      {MOTOR_IDS.map((motor,i)=><button type="button" key={motor} ref={node=>{buttons.current[i]=node;}} role="radio" aria-checked={draft===motor} disabled={disabled} tabIndex={draft===motor?0:-1}
        onClick={()=>{setDraft(motor);onChange(motor);}}>{motor}<span aria-hidden="true" /></button>)}
    </div>
  </section>;
}
