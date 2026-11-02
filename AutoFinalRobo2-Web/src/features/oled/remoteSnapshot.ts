import { Afr2OledBuffer } from './oledRasterizer';
import { normalizeScreenCode } from './screenCatalog';

export interface RemoteOledSnapshot {
  screenCode: string;
  revision: number | null;
  selection: number;
  firstVisible: number;
  itemCount: number|null;
  buffer: Afr2OledBuffer;
  receivedAtMs: number;
  representation: 'framebuffer';
}
function uint(value: unknown, max: number): value is number { return typeof value === 'number' && Number.isInteger(value) && value >= 0 && value <= max; }

/** Current EX01 requires the authoritative framebuffer. Local previews never
 * substitute a missing remote capture or infer state from a ScreenCode. */
export function parseRemoteOled(payload: Record<string, unknown>): RemoteOledSnapshot {
  const code = payload.screenCode;
  if (typeof code !== 'string' || !/^0x(?:[0-9a-fA-F]{6}|[0-9a-fA-F]{8})$/.test(code) || !uint(payload.selection, 255) || !uint(payload.firstVisible, 255)) throw new Error('El snapshot OLED no tiene identidad y selección válidas.');
  const screenCode = normalizeScreenCode(code)!;
  const buffer = new Afr2OledBuffer();
  if (typeof payload.framebufferBase64 === 'string') {
    if (payload.framebufferBase64.length !== 1368) throw new Error('La captura debe usar Base64 canónico para 1024 bytes.');
    const data = atob(payload.framebufferBase64);
    if (btoa(data) !== payload.framebufferBase64 || data.length !== 1024) throw new Error('El framebuffer OLED debe contener exactamente 1024 bytes.');
    for (let index = 0; index < 1024; index++) buffer.buffer[index] = data.charCodeAt(index);
  } else {
    throw new Error('Falta el framebuffer del ATmega. La biblioteca local no reemplaza una captura remota.');
  }
  return { screenCode, revision: uint(payload.revision, 0xffffffff) ? payload.revision : null, selection: payload.selection, firstVisible: payload.firstVisible, itemCount:uint(payload.itemCount,255)?payload.itemCount:null, buffer, receivedAtMs: Date.now(), representation: 'framebuffer' };
}
