import { Field } from '../../components/ui/Field';

const encoder = new TextEncoder();
const byteLength = (value: string) => encoder.encode(value).length;
const invalidSsid = (value: string) => byteLength(value) > 32 || value.includes('\0');
const invalidPassword = (value: string) => value.includes('\0') || (!!value && (byteLength(value) < 8 || byteLength(value) > 63));
export const validWifiCredentials = (ssid: string, password: string) => !!ssid && !invalidSsid(ssid) && !invalidPassword(password);

/** One UTF-8 policy and controlled field set for direct setup and OLED drafts.
 * Secrets remain in the owning form's ephemeral state; no storage here. */
export function WifiCredentialsFields({ id, mode, ssid, password, onSsid, onPassword, deferred = false }: {
  id: string; mode: 'STA' | 'AP' | 'AP_STA'; ssid: string; password: string;
  onSsid: (value: string) => void; onPassword: (value: string) => void; deferred?: boolean;
}) {
  const ssidError = invalidSsid(ssid) ? 'El SSID no admite NUL y debe ocupar hasta 32 bytes UTF-8.' : undefined;
  const passError = invalidPassword(password) ? 'Usá de 8 a 63 bytes UTF-8 o dejá el campo vacío para una red abierta.' : undefined;
  return <>
    <Field id={`${id}-ssid`} label={mode === 'AP' ? 'Nombre del punto de acceso que creará la ESP' : 'Nombre de la red a la que se conectará la ESP'}
      hint={mode === 'AP_STA' ? 'El punto de acceso conserva su configuración propia. SSID de hasta 32 bytes.' : 'SSID exacto, de 1 a 32 bytes. Se conservan los espacios.'} error={ssidError}>
      <input id={`${id}-ssid`} type="text" required autoComplete="off" value={ssid}
        aria-invalid={!!ssidError} aria-describedby={`${id}-ssid-${ssidError ? 'error' : 'hint'}`}
        onChange={e => onSsid(e.target.value)} placeholder="Ej. Laboratorio_Robotica_2G" className="afr2-glass-field w-full min-h-[42px] px-3 py-1.5 text-sm" />
    </Field>
    <Field id={`${id}-pass`} label="Contraseña de la red" error={passError}
      hint="Vacía para una red abierta; de 8 a 63 bytes para una frase WPA. No se guarda en el navegador.">
      <input id={`${id}-pass`} type="password" autoComplete="new-password" value={password}
        aria-invalid={!!passError} aria-describedby={`${id}-pass-${passError ? 'error' : 'hint'}`}
        onChange={e => onPassword(e.target.value)} placeholder="••••••••" className="afr2-glass-field w-full min-h-[42px] px-3 py-1.5 text-sm" />
    </Field>
    <p className="afr2-help">{deferred?'Al confirmar físicamente se guardará en LittleFS y se reiniciará el ESP.':'Se guardará en LittleFS y se reiniciará el ESP. La aceptación no confirma la nueva conexión.'}</p>
  </>;
}
