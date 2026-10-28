/** AFR2-20261028-10. Field-card packet visualization adapted from AutoMicro.
 * Actual ROBO2 offsets and BT01 exception replace all historical UNER offsets. */
import { commandById } from './catalog';
import { hexByte, hexBytes } from './codec';
import { StudioPanel } from './StudioPrimitives';

export function PacketStructure({ bytes }: { bytes: number[] }) {
  if (bytes.length < 12) return null;
  const payloadEnd = bytes.length - 1, boot = bytes[10] === 0x52;
  const response = bytes[9] === 0x13;
  const parts = [
    { id: 'header', label: 'Cabecera', start: 0, end: 5, note: 'ROBO2', detail: 'Cinco bytes ASCII: R O B O 2. Identifican la cabecera.' },
    { id: 'meta', label: 'Longitud N', start: 5, end: 6, note: `${bytes[5]} bytes de payload`, detail: 'Incluye correlación, resultado cuando corresponda y cuerpo; no incluye framing ni XOR.' },
    { id: 'meta', label: 'Tokens y versión', start: 6, end: 9, note: '3A A5 · framing 01', detail: 'Dos tokens fijos y la versión de framing, independiente del perfil de aplicación.' },
    { id: 'route', label: 'Ruta', start: 9, end: 10, note: `${bytes[9] >> 4} → ${bytes[9] & 15}`, detail: 'Nibble alto: emisor. Nibble bajo: receptor. Nodo 1 ATmega, nodo 3 ESP. Rutas válidas: 31 y 13.' },
    { id: 'command', label: 'Comando', start: 10, end: 11, note: commandById(bytes[10])?.name ?? 'Desconocido', detail: commandById(bytes[10])?.description ?? 'Comando sin definición en el catálogo actual.' },
    ...(!boot && payloadEnd >= 19 ? [
      { id: 'correlation', label: 'Sesión', start: 11, end: 15, note: 'u32 little-endian', detail: 'Identidad de transporte no nula. Una trama local no negocia una sesión con el robot.' },
      { id: 'correlation', label: 'Transacción', start: 15, end: 19, note: 'u32 little-endian', detail: 'Identifica la petición y su respuesta dentro de la sesión.' },
      ...(response && payloadEnd >= 20 ? [{ id: 'meta', label: 'Resultado', start: 19, end: 20, note: '0 OK · 1..4 error', detail: 'Un error lleva N=9, sin cuerpo de aplicación.' }] : []),
    ] : []),
    { id: 'payload', label: boot ? 'Evento BT01' : 'Cuerpo de aplicación', start: boot || payloadEnd < 19 ? 11 : response ? Math.min(20, payloadEnd) : 19, end: payloadEnd, note: boot ? 'Sin correlación' : 'Según comando y dirección', detail: boot ? 'Aviso de arranque de 14 bytes. No contiene sesión, transacción ni resultado.' : 'Bytes propios del comando. Los enteros se serializan explícitamente en little-endian.' },
    { id: 'checksum', label: 'Checksum', start: payloadEnd, end: bytes.length, note: 'XOR', detail: 'XOR de todos los bytes anteriores. Detecta algunas alteraciones; no confirma ejecución.' },
  ];
  return <StudioPanel title="Estructura del paquete" description="Lee de izquierda a derecha. Despliega un campo para ver su significado." badge={`${bytes.length} bytes`}>
    <div className="afr2-protocol-packet-grid">{parts.map(part => <details key={part.label} className="afr2-protocol-packet-field" data-role={part.id}><summary><span className="afr2-protocol-packet-label"><strong>{part.label}</strong><span>{part.start === part.end ? 'Vacío' : `${part.start}${part.end > part.start + 1 ? `–${part.end - 1}` : ''}`}</span></span><code>{hexBytes(bytes.slice(part.start, part.end)) || 'Vacío'}</code><span className="afr2-protocol-packet-note">{part.note}</span><span className="afr2-protocol-packet-more">Detalles <span aria-hidden="true">⌄</span></span></summary><p>{part.detail}</p></details>)}</div>
    <div className="afr2-protocol-byte-stream" tabIndex={0} role="group" aria-label="Bytes de la trama por campo">{parts.map(part => <span key={part.label} className="afr2-protocol-byte-group" data-role={part.id} aria-label={part.label}>{bytes.slice(part.start, part.end).map((byte, i) => <span key={i} title={`${part.label} · offset ${part.start + i}`}>{hexByte(byte)}</span>)}</span>)}</div>
    <p className="afr2-help">Offsets en bytes desde 0. Las etiquetas y separaciones identifican cada campo.</p>
  </StudioPanel>;
}
