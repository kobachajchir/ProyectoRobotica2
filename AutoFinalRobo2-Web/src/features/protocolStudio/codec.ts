/** AFR2-20261028-09/10. Offline adaptation of AutoMicro's builder / block scanner.
 * No UART/WS transport. Wire authority: Contracts/afr2_wire.h, ROBO2/profile 2.
 * Capture analysis cannot establish timing, negotiated capabilities or execution. */
import { COMMANDS, RESULT_NAMES, commandById } from './catalog';

export const MAX_INPUT_CHARS = 262144;
const MAX_BYTES = 65536;
const HEADER = [0x52, 0x4f, 0x42, 0x4f, 0x32];
export type InputFormat = 'hex' | 'text' | 'decimal';
export type TextFormat = 'escaped' | 'utf8' | 'ascii' | 'latin1';
export type OutputFormat = 'hex' | 'realterm' | 'c' | 'python' | 'decimal';
export interface CaptureBytes {
  bytes: number[];
  boundaries: number[];
  metadata: string[];
}
export interface FrameInfo {
  route: number;
  command: number;
  payload: number[];
  checksum: number;
  expectedChecksum: number;
  bodyOffset: number;
  fields: [string, string][];
  issues: string[];
}
export interface StreamSegment {
  kind: 'raw' | 'frame' | 'invalid' | 'incomplete';
  offset: number;
  length: number;
  note: string;
  frame?: FrameInfo;
}
export interface Translation extends CaptureBytes {
  segments: StreamSegment[];
  commandOnly?: number;
}

export const hexByte = (value: number) => value.toString(16).toUpperCase().padStart(2, '0');
export const hexBytes = (bytes: readonly number[]) => bytes.map(hexByte).join(' ');
const xor = (bytes: readonly number[]) => bytes.reduce((sum, byte) => sum ^ byte, 0);
const u16 = (b: number[], offset: number) => b[offset] | (b[offset + 1] << 8);
const u32 = (b: number[], offset: number) => (b[offset] | (b[offset + 1] << 8) | (b[offset + 2] << 16) | (b[offset + 3] << 24)) >>> 0;
const age = (value: number) => value === 0xffffffff ? 'desconocida' : `${value} ms`;

export function parseCapture(input: string, format: InputFormat): CaptureBytes {
  if (input.length > MAX_INPUT_CHARS) throw new Error('La entrada supera 256 Ki caracteres. Divide la captura en bloques.');
  if (!input.trim()) throw new Error('Pega una captura o escribe un comando.');
  if (format === 'text') {
    const bytes = Array.from(new TextEncoder().encode(input));
    if (bytes.length > MAX_BYTES) throw new Error('El texto supera 64 KiB en UTF-8. Divide la captura.');
    return { bytes, boundaries: [0, bytes.length], metadata: [] };
  }

  // Recognize complete C / Python byte containers, never mine byte-like words
  // from arbitrary logs. Array literals use decimal integers or explicit 0xHH.
  let source = input.trim();
  if ((source.startsWith('"') && source.endsWith('"')) || (source.startsWith("'") && source.endsWith("'"))) source = source.slice(1, -1).trim();
  source = source.replace(/^```(?:hex|text|c|cpp|python)?\s*\r?\n([\s\S]*?)\r?\n```$/i, '$1').trim();
  const container = source.match(/^(?:(?:[A-Za-z_]\w*\s*=\s*)?bytes\s*\(\s*\[([\s\S]*)\]\s*\)|(?:[\w\s*]+\[[\w\s]*\]\s*=\s*)?\{([\s\S]*)\}\s*;?|(?:[A-Za-z_]\w*\s*=\s*)?\[([\s\S]*)\])$/);
  if (container) {
    source = container[1] ?? container[2] ?? container[3];
  }
  const bytes: number[] = [];
  const boundaries = [0];
  const metadata: string[] = [];
  const lines = source.split(/\r?\n/);
  for (let index = 0; index < lines.length; index++) {
    const line = lines[index].trim();
    if (!line || /^```(?:hex|text|c|cpp|python)?$/i.test(line) || line === '"' || line === "'") continue;
    if (/^---\s+.+\s+---$/.test(line)) {
      metadata.push(line);
      if (boundaries[boundaries.length - 1] !== bytes.length) boundaries.push(bytes.length);
      continue;
    }
    for (const token of line.split(/[\s,;]+/).filter(Boolean)) {
      if (format === 'decimal' || (container && !/^0x/i.test(token))) {
        if (!/^\d{1,3}$/.test(token) || Number(token) > 255) throw new Error(`Línea ${index + 1}: se esperan bytes decimales de 0 a 255.`);
        bytes.push(Number(token));
      } else if (/^0x[\da-f]{1,2}$/i.test(token)) {
        bytes.push(Number.parseInt(token.slice(2), 16));
      } else if (/^(?:[\da-f]{2})+$/i.test(token)) {
        for (let position = 0; position < token.length; position += 2) bytes.push(Number.parseInt(token.slice(position, position + 2), 16));
      } else {
        throw new Error(`Línea ${index + 1}: HEX inválido. Usa pares HH o 0xHH; para texto elige Texto UTF-8.`);
      }
      if (bytes.length > MAX_BYTES) throw new Error('La captura supera 64 KiB. Divídela en bloques.');
    }
  }
  if (!bytes.length) throw new Error('La entrada no contiene bytes.');
  boundaries.push(bytes.length);
  return { bytes, boundaries, metadata };
}

export function formatBytes(bytes: readonly number[], format: OutputFormat): string {
  if (format === 'c') return `const uint8_t frame[] = { ${bytes.map(byte => `0x${hexByte(byte)}`).join(', ')} };`;
  if (format === 'python') return `bytes([${bytes.map(byte => `0x${hexByte(byte)}`).join(', ')}])`;
  if (format === 'decimal') return bytes.join(' ');
  const tokens = bytes.map(byte => `${format === 'realterm' ? '0x' : ''}${hexByte(byte)}`);
  const lines: string[] = [];
  for (let offset = 0; offset < tokens.length; offset += 16) lines.push(tokens.slice(offset, offset + 16).join(' '));
  return lines.join('\n');
}

export function decodeText(bytes: readonly number[], format: TextFormat): string {
  if (format === 'utf8') return new TextDecoder('utf-8').decode(Uint8Array.from(bytes));
  return bytes.map(byte => {
    if (format === 'latin1') return byte >= 32 && !(byte >= 127 && byte <= 159) ? String.fromCharCode(byte) : escapeByte(byte);
    if (byte === 10) return format === 'escaped' ? '\\n\n' : '\n';
    if (byte === 13) return format === 'escaped' ? '\\r' : '\r';
    if (byte === 9 && format === 'ascii') return '\t';
    if (byte >= 32 && byte <= 126) return byte === 92 && format === 'escaped' ? '\\\\' : String.fromCharCode(byte);
    return format === 'ascii' ? '·' : escapeByte(byte);
  }).join('');
}

function escapeByte(byte: number): string {
  if (byte === 10) return '\\n\n';
  if (byte === 13) return '\\r';
  if (byte === 9) return '\\t';
  return `\\x${hexByte(byte)}`;
}

function describePayload(command: number, route: number, payload: number[]): Pick<FrameInfo, 'fields' | 'issues'> {
  const fields: [string, string][] = [];
  const issues: string[] = [];
  const definition = commandById(command);
  const response = route === 0x13;
  const prefixLength = response ? 9 : 8;
  if (!definition) issues.push('Comando fuera del catálogo conocido; se conserva su payload.');
  if (definition?.retired) issues.push('Comando retirado del endpoint UART vigente.');
  if (command === 0x52) {
    if (payload.length !== 14 || hexBytes(payload.slice(0, 4)) !== '42 54 30 31') {
      issues.push('BOOT requiere payload BT01 de 14 bytes sin prefijo de correlación.');
    } else {
      fields.push(['Tipo', 'Aviso de arranque sin correlación'], ['Emisor', String(payload[4])], ['Boot ID', String(u32(payload, 5))], ['Causa de reset', `0x${hexByte(payload[9])}`], ['Firmware', payload.slice(10, 13).join('.')], ['EX01', String(payload[13])]);
      if (payload[4] !== (route >> 4)) issues.push('El nodo del evento no coincide con el emisor de la ruta.');
      if (!u32(payload, 5)) issues.push('Boot ID debe ser distinto de cero.');
    }
    return { fields, issues };
  }
  if (payload.length < prefixLength) {
    issues.push(`Prefijo incompleto: se requieren ${prefixLength} bytes de payload.`);
    return { fields, issues };
  }
  const session = u32(payload, 0), transaction = u32(payload, 4);
  fields.push(['Sesión u32 LE', `${session} (0x${session.toString(16).toUpperCase().padStart(8, '0')})`], ['Transacción u32 LE', String(transaction)]);
  if (!session || !transaction) issues.push('Sesión y transacción deben ser distintas de cero.');
  const body = payload.slice(prefixLength);
  if (response) {
    const result = payload[8];
    fields.push(['Resultado', RESULT_NAMES[result] ?? `Desconocido (${result})`]);
    if (result > 4) issues.push('Código de resultado fuera de 0..4.');
    if (result !== 0) {
      if (body.length) issues.push('Una respuesta de error debe tener N=9, sin cuerpo.');
      return { fields, issues };
    }
  }
  const expected = response ? definition?.replyBytes : definition?.requestBytes;
  const lengths = expected === undefined ? undefined : Array.isArray(expected) ? expected : [expected];
  const wrongLength = lengths !== undefined && !lengths.includes(body.length);
  if (wrongLength) issues.push(`Cuerpo ${response ? 'OK' : 'de petición'}: se esperan ${lengths!.join(' o ')} bytes; hay ${body.length}.`);
  if (expected === undefined) fields.push(['Cuerpo', 'Interpretación manual; longitud específica no comprobada.']);
  if (wrongLength) return { fields, issues };
  const add = (label: string, value: string | number) => fields.push([label, String(value)]);
  if (!response) {
    if (command === 0x01 && hexBytes(body) !== '41 46 52 32 02') issues.push('HELLO requiere AFR2 ASCII y perfil 2.');
    if (command === 0x10 && hexBytes(body) !== '45 58 30 31') issues.push('EXT_HELLO requiere EX01 ASCII.');
    const reserved = [0x13, 0x14, 0x15, 0x16, 0x1a, 0x1b, 0x1c, 0x40, 0x41, 0x42, 0x43, 0x44];
    if (reserved.includes(command) && (body.length < 4 || u32(body, 0) !== 0)) issues.push('El campo reserved:u32 inicial debe ser cero en EX01v7+.');
    if ((command === 0x12 || command === 0x61) && body.length === 2) {
      const value = u16(body, 0);
      add(command === 0x12 ? 'Presencia Web' : 'Intervalo ultrasónico', `${value} ms`);
      if (command === 0x12 ? value > 1500 : value < 70 || value > 1000) issues.push('Valor fuera del rango de producto.');
    }
    if (command === 0x64 && body.length === 2) {
      add('Operación servo', ['Desactivar', 'Ángulo', 'Centrar'][body[0]] ?? 'Desconocida');
      add('Ángulo solicitado', `${body[1]}°`);
      if (body[0] > 2 || body[1] > 180) issues.push('Operación o ángulo de servo fuera de rango.');
    }
    if (command === 0x68 && body.length === 5) {
      add('Lectura radar', body[0] === 0 ? 'Snapshot' : body[0] === 1 ? 'Delta' : 'Desconocida'); add('Cursor', u32(body, 1));
      if (body[0] > 1 || (body[0] === 0 && u32(body, 1) > 37)) issues.push('Modo o cursor de snapshot fuera de rango.');
    }
    if (command === 0x50 && body[0] > 4) issues.push('La página INFO debe estar entre 0 y 4.');
    if (command === 0x51 && body[0] !== 0xa5) issues.push('REBOOT requiere el byte de confirmación A5.');
  } else {
    if (command === 0x01 && body.length === 14) {
      add('Producto', String.fromCharCode(...body.slice(0, 4))); add('Perfil', body[4]);
      add('Firmware', body.slice(5, 8).join('.')); add('Capabilities', `0x${u16(body, 8).toString(16).toUpperCase()}`); add('Uptime', `${u32(body, 10)} ms`);
      if (hexBytes(body.slice(0, 5)) !== '41 46 52 32 02') issues.push('Identidad o perfil HELLO distintos de AFR2 / 2.');
    }
    if (command === 0x10 && body.length === 10) {
      add('Extensión', String.fromCharCode(...body.slice(0, 4))); add('Versión EX01', u16(body, 4)); add('Features', `0x${u32(body, 6).toString(16).toUpperCase()}`);
      if (hexBytes(body.slice(0, 4)) !== '45 58 30 31') issues.push('Identidad de extensión distinta de EX01.');
      if (u16(body, 4) !== 8) issues.push('Versión EX01 distinta de 8; interpretar con el contrato de esa versión.');
    }
    if (command === 0x02 && body.length === 4) add('Uptime', `${u32(body, 0)} ms`);
    if (command === 0x03 && body.length === 7) {
      add('Modo', body[0]); add('Parada declarada', body[1]); add('Edad', age(u32(body, 3)));
    }
    if (command === 0x04 && body.length === 13) {
      add('Línea válida declarada', body[0]); add('Bits IR', `0x${hexByte(body[1])}`); add('Edad de línea', age(u32(body, 2)));
      add('Ultrasonido válido declarado', body[6]); add('Distancia', body[6] ? `${u16(body, 7)} mm` : 'desconocida'); add('Edad de distancia', age(u32(body, 9)));
    }
    if (command === 0x05 && body.length === 22) {
      add('Hall conectado / válido', `${body[0]} / ${body[1]}`);
      for (let wheel = 0; wheel < 4; wheel++) add(`Cuentas ${['FL', 'FR', 'RL', 'RR'][wheel]}`, u32(body, 2 + wheel * 4) | 0);
      add('Edad', age(u32(body, 18)));
    }
    if (command === 0x06 && body.length === 2) { add('Parada declarada', body[0]); add('Modo', body[1]); }
    if (command === 0x11 && body.length === 16) {
      add('Modo', body[0]); add('Origen', ['Ninguno', 'Local', 'Web'][body[1]] ?? String(body[1])); add('Parada declarada', body[2]); add('Fallo', body[3]);
      add('Pose válida declarada', body[5]); add('Estado GOTO / EEPROM', `${body[6]} / ${body[7]}`); add('Features', `0x${u32(body, 8).toString(16).toUpperCase()}`); add('Uptime', `${u32(body, 12)} ms`);
      if (body[4] !== 0) issues.push('El byte reservado de EXT_STATE debe ser cero en EX01v7+.');
    }
    if ([0x12, 0x61].includes(command) && body.length === 2) add(command === 0x12 ? 'Presencia Web' : 'Intervalo ultrasónico', `${u16(body, 0)} ms`);
    if (command === 0x60 && body.length === 35) {
      const sampleId = u32(body, 8), error = body[23], distance = u16(body, 16);
      add('Boot ID', u32(body, 0)); add('Muestra', sampleId); add('Instante de muestra', `${u32(body, 12)} ms`);
      add('Distancia', sampleId && error === 0 && distance >= 20 && distance <= 4000 ? `${distance} mm` : 'desconocida / inválida');
      add('Error de eco', error); add('Duración de eco', `${u16(body, 18) * 25} µs`); add('Ángulo de muestra', `${body[22]}°`); add('Radar activo declarado', body[33]);
    }
    if (command === 0x62 && body.length === 27) {
      add('Boot ID', u32(body, 0)); add('Muestra IR', u32(body, 4)); add('Edad IR', age(u32(body, 8)));
      add('Tipo de lectura', ['Apagado', 'Digital', 'ADC'][body[12]] ?? String(body[12])); add('Datos originales válidos declarados', body[13]); add('Calibrado declarado', body[14]); add('Bits IR', `0x${hexByte(body[15])}`);
      add('IR1 / IR2 / IR3 / IR4 / IR5', Array.from({ length: 5 }, (_, i) => u16(body, 16 + i * 2)).join(' / ')); add('Válido para control declarado', body[26]);
    }
    if ((command === 0x63 || command === 0x64) && body.length === 18) {
      add('Boot ID', u32(body, 0)); add('Secuencia aplicada', u32(body, 4)); add('PWM aplicado', `${u16(body, 8)} µs`); add('PWM objetivo', `${u16(body, 10)} µs`); add('Ángulo nominal', `${body[12]}°`); add('Habilitado', body[13]); add('Edad', age(u32(body, 14)));
    }
    if (command === 0x69 && body.length === 11) {
      add('ScreenCode', `0x${u32(body, 0).toString(16).toUpperCase().padStart(6, '0')}`); add('Revisión de navegación', u32(body, 4)); add('Selección / primera fila', `${body[8]} / ${body[9]}`); add('Vista privada', body[10]);
    }
    if ([0x66, 0x67, 0x6a].includes(command) && body.length === 1) add(command === 0x6a ? 'Tracción detenida declarada' : 'Radar activo declarado', body[0]);
    if (command === 0x68) {
      if (body.length < 23 || body[14] > 2 || body.length !== 23 + body[14] * 16) issues.push('Longitud de respuesta RADAR_GET incoherente con el número de muestras.');
      else { add('Muestras', body[14]); add('Siguiente cursor', u32(body, 8)); add('Muestras perdidas', u16(body, 12)); }
    }
  }
  return { fields, issues };
}

export function scanCapture(capture: CaptureBytes): StreamSegment[] {
  const { bytes, boundaries } = capture;
  const segments: StreamSegment[] = [];
  for (let block = 0; block < boundaries.length - 1; block++) {
    const end = boundaries[block + 1];
    let offset = boundaries[block], rawStart = offset;
    const flushRaw = () => {
      if (offset > rawStart) segments.push({ kind: 'raw', offset: rawStart, length: offset - rawStart, note: 'Bytes fuera de una trama ROBO2.' });
    };
    while (offset < end) {
      const available = end - offset;
      const prefix = HEADER.slice(0, Math.min(available, HEADER.length)).every((byte, i) => bytes[offset + i] === byte);
      if (!prefix || (available < HEADER.length && available < 2)) { offset++; continue; }
      flushRaw();
      if (available < 11) {
        segments.push({ kind: 'incomplete', offset, length: available, note: 'Cabecera ROBO2 incompleta al final de la captura.' }); offset = end; rawStart = end; break;
      }
      const n = bytes[offset + 5], route = bytes[offset + 9], total = n + 12;
      const faults: string[] = [];
      if (bytes[offset + 6] !== 0x3a || bytes[offset + 7] !== 0xa5) faults.push('Tokens distintos de 3A A5.');
      if (bytes[offset + 8] !== 1) faults.push('Versión de framing distinta de 01.');
      if (faults.length) {
        const badField = bytes[offset + 6] !== 0x3a ? 6 : bytes[offset + 7] !== 0xa5 ? 7 : 8;
        const consumed = badField + (bytes[offset + badField] === HEADER[0] ? 0 : 1);
        segments.push({ kind: 'invalid', offset, length: consumed, note: faults.join(' ') }); offset += consumed; rawStart = offset; continue;
      }
      if (n > 64) faults.push(`N=${n} supera 64 bytes.`);
      if (route !== 0x31 && route !== 0x13) faults.push(`Ruta ${hexByte(route)} fuera de 31 / 13.`);
      if (available < total) {
        segments.push({ kind: 'incomplete', offset, length: available, note: `${faults.join(' ')} Trama truncada: hay ${available} de ${total} bytes.`.trim() }); offset = end; rawStart = end; break;
      }
      const frameBytes = bytes.slice(offset, offset + total);
      const expectedChecksum = xor(frameBytes.slice(0, -1)), checksum = frameBytes[total - 1];
      if (checksum !== expectedChecksum) faults.push(`XOR recibido ${hexByte(checksum)}; calculado ${hexByte(expectedChecksum)}.`);
      const payload = frameBytes.slice(11, -1), command = frameBytes[10];
      segments.push({ kind: faults.length ? 'invalid' : 'frame', offset, length: total, note: faults.join(' ') || 'Framing ROBO2 correcto.', frame: { route, command, payload, checksum, expectedChecksum, bodyOffset: command === 0x52 ? 0 : route === 0x13 ? 9 : 8, ...describePayload(command, route, payload) } });
      // Treat N bytes as one payload, even if they contain another header.
      offset += total; rawStart = offset;
    }
    flushRaw();
  }
  return segments;
}

export function translate(input: string, format: InputFormat): Translation {
  const namedCommand = format === 'hex' ? COMMANDS.find(command => command.name === input.trim().toUpperCase().replace(/\s+/g, '_')) : undefined;
  const bareCommand = format === 'hex' && /^(?:0x)?[\da-f]{1,2}$/i.test(input.trim()) ? Number.parseInt(input.trim().replace(/^0x/i, ''), 16) : undefined;
  const command = namedCommand?.id ?? bareCommand;
  const capture = command !== undefined ? { bytes: [command], boundaries: [0, 1], metadata: [] } : parseCapture(input, format);
  return { ...capture, segments: scanCapture(capture), commandOnly: capture.bytes.length === 1 ? capture.bytes[0] : undefined };
}

function uint32Input(raw: string, label: string): number {
  if (!/^(?:0x[\da-f]+|\d+)$/i.test(raw.trim())) throw new Error(`${label}: usa un entero decimal o 0xHEX.`);
  const value = Number(raw);
  if (!Number.isSafeInteger(value) || value < 1 || value > 0xffffffff) throw new Error(`${label}: rango 1..4294967295.`);
  return value;
}

export interface BuilderInput { command: number; response: boolean; session: string; transaction: string; result: number; body: string }
export function buildFrame(input: BuilderInput): number[] {
  const definition = commandById(input.command);
  if (!definition || definition.retired || definition.readOnly) throw new Error('Elige un comando vigente que admita construcción de tramas correlacionadas.');
  const session = uint32Input(input.session, 'Sesión'), transaction = uint32Input(input.transaction, 'Transacción');
  const body = input.body.trim() ? parseCapture(input.body, 'hex') : { bytes: [], metadata: [] };
  if (body.metadata.length) throw new Error('El cuerpo debe contener solo bytes HEX.');
  if (!Number.isInteger(input.result) || input.result < 0 || input.result > 4) throw new Error('Resultado fuera de 0..4.');
  const payload: number[] = [];
  for (const value of [session, transaction]) for (let i = 0; i < 4; i++) payload.push((value >>> (i * 8)) & 255);
  if (input.response) payload.push(input.result);
  payload.push(...body.bytes);
  if (payload.length > 64) throw new Error('El payload supera 64 bytes, incluidos sesión, transacción y resultado.');
  const route = input.response ? 0x13 : 0x31;
  const details = describePayload(input.command, route, payload);
  if (details.issues.length) throw new Error(details.issues.join(' '));
  const bytes = [...HEADER, payload.length, 0x3a, 0xa5, 1, route, input.command, ...payload];
  return [...bytes, xor(bytes)];
}
