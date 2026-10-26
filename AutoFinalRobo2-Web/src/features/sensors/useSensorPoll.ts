import { useEffect, useRef } from 'react';
import { useConnection } from '../connection/ConnectionContext';
import type { Afr2CommandName } from '../../protocol/wsEnvelope';
/** Visible-only reader with one outstanding request and no periodic toasts. */
export function useSensorPoll<T>(command:Afr2CommandName,interval:number,receive:(value:T)=>void,payload:()=>Record<string,unknown>=()=>({}),enabled=true){
  const {wsState,megaLinkState,requestCommand}=useConnection();const callbacks=useRef({receive,payload});callbacks.current={receive,payload};
  useEffect(()=>{
    if(!enabled || wsState!=='ready' || megaLinkState!=='ready')return;
    let cancelled=false,pending=false,timer=0;
    const read=async()=>{if(cancelled || pending || document.visibilityState!=='visible')return;pending=true;
      try{const r=await requestCommand<T>(command,callbacks.current.payload(),2000);if(!cancelled && r.status==='completed')callbacks.current.receive(r.payload);}
      catch{/* A failed request never becomes a sample. */}finally{pending=false;if(!cancelled)timer=window.setTimeout(read,interval);}};
    const visibility=()=>{clearTimeout(timer);if(document.visibilityState==='visible')void read();};
    void read();document.addEventListener('visibilitychange',visibility);
    return()=>{cancelled=true;clearTimeout(timer);document.removeEventListener('visibilitychange',visibility);};
  },[enabled,wsState,megaLinkState,command,interval,requestCommand]);
}
