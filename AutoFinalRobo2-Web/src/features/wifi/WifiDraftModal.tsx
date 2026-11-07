import { useEffect, useState } from 'react';
import { FloatingModal } from '../../components/ui/FloatingModal';
import { Button } from '../../components/ui/Button';
import { WifiCredentialsFields, validWifiCredentials } from './WifiCredentialsFields';
import { useConnection } from '../connection/ConnectionContext';

export function WifiDraftModal() {
  const {wifiDraft,requestCommand,webOperationActive,triggerEmergencyStop} = useConnection();
  const [ssid,setSsid]=useState(''),[password,setPassword]=useState('');
  const [busy,setBusy]=useState(false),[error,setError]=useState('');
  useEffect(() => {
    setSsid(wifiDraft?.ssid ?? '');setPassword('');setError('');setBusy(false);
    if(wifiDraft && webOperationActive)void triggerEmergencyStop('wifi-draft');
    // Job identity controls the lifetime of the secret; status refreshes preserve typing.
    // eslint-disable-next-line react-hooks/exhaustive-deps
  },[wifiDraft?.jobId]);
  const cancel=async () => {
    setPassword('');if(!wifiDraft)return;
    try {await requestCommand('wifi.draft.cancel',{jobId:wifiDraft.jobId});}catch(e){setError(e instanceof Error?e.message:'No se pudo cancelar.');}
  };
  const valid=validWifiCredentials(ssid,password);
  return <FloatingModal open={!!wifiDraft} title="Conexión solicitada desde OLED" description="Prepará los datos. El robot esperará que confirmes Conectar con el pulsador físico." onClose={()=>void cancel()}>
    <form className="space-y-4" onSubmit={async e=>{
      e.preventDefault();if(!wifiDraft || !valid || busy)return;setBusy(true);setError('');
      try {await requestCommand('wifi.draft.submit',{jobId:wifiDraft.jobId,mode:wifiDraft.mode,ssid,password,persist:true});setPassword('');}
      catch(e){setError(e instanceof Error?e.message:'No se pudo enviar el borrador.');}
      finally{setBusy(false);}
    }}>
      <p>Modo solicitado: {wifiDraft?.mode}. Las credenciales no se muestran en diagnósticos.</p>
      <WifiCredentialsFields id="wifi-draft" mode={wifiDraft?.mode ?? 'AP_STA'} ssid={ssid} password={password}
        onSsid={setSsid} onPassword={setPassword} deferred />
      {error && <p role="alert">{error}</p>}
      <div className="afr2-action-row"><Button type="submit" disabled={!valid || busy}>Enviar borrador a OLED</Button><Button type="button" disabled={busy} onClick={()=>void cancel()}>Cancelar</Button></div>
    </form>
  </FloatingModal>;
}
