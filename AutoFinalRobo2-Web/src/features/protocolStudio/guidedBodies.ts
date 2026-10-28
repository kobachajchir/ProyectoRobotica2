/** AFR2-20261028-10. AutoMicro's parameter-driven builder adapted to AFR2.
 * Layouts: current avr services afr2_app_wire, afr2_sequence, sensor_commands.
 * These forms prepare local bytes; firmware still owns operational limits. */
import { commandById } from './catalog';
import { hexBytes } from './codec';

type IntegerType = 'u8' | 'i8' | 'u16' | 'i16' | 'u32' | 'i32';
export interface ParameterField {
  id: string; label: string; type: IntegerType; initial: string;
  min?: number; max?: number; step?: number; unit?: string;
  options?: { value: string; label: string }[];
}
export interface RequestSchema { id: string; label: string; prefix?: number[]; fields: ParameterField[] }
const choices = (...labels: string[]) => labels.map((label, value) => ({ value: String(value), label }));
const field = (id: string, label: string, type: IntegerType, initial = '0', extra: Partial<ParameterField> = {}): ParameterField => ({ id, label, type, initial, ...extra });
const schema = (id: string, label: string, fields: ParameterField[], prefix?: number[]): RequestSchema => ({ id, label, fields, prefix });
const reserved = [0, 0, 0, 0];
const seq = () => field('sequence', 'ID de secuencia', 'u32', '1', { min: 1 });
const execution = () => field('execution', 'ID de ejecución', 'u32', '1', { min: 1 });
const count = () => field('count', 'Cantidad de segmentos', 'u8', '1', { min: 1, max: 32 });
const total = () => field('total', 'Duración máxima', 'u32', '1000', { min: 10, max: 120000, step: 10, unit: 'ms' });
const wheel = () => field('wheel', 'Rueda', 'u8', '0', { options: choices('FL · delantera izquierda', 'FR · delantera derecha', 'RL · trasera izquierda', 'RR · trasera derecha') });
const kind = () => field('kind', 'Tipo de movimiento', 'u8', '6', { options: choices('Desplazamiento', 'Rampa', 'Giro', 'Curva', 'Espera', 'Finalización', 'Ruedas') });
const speed = (id: string, label: string) => field(id, label, 'i16', '0', { unit: 'mm/s' });
const xy = () => [field('x', 'Destino X', 'i32', '0', { unit: 'mm' }), field('y', 'Destino Y', 'i32', '0', { unit: 'mm' })];
const revision = () => field('revision', 'Revisión OLED', 'u32', '1', { min: 1 });

const SCHEMAS: Record<number, RequestSchema[]> = {
  0x01: [schema('hello', 'Identidad AFR2 / perfil 2', [], [0x41, 0x46, 0x52, 0x32, 2])],
  0x10: [schema('ext-hello', 'Identidad EX01', [], [0x45, 0x58, 0x30, 0x31])],
  0x12: [schema('pulse', 'Presencia Web', [field('age', 'Edad de presencia', 'u16', '0', { max: 1500, unit: 'ms' })])],
  0x13: [schema('drive', 'Consigna de movimiento', [speed('vx', 'Velocidad X'), speed('vy', 'Velocidad Y'), field('omega', 'Velocidad angular', 'i16', '0', { unit: 'mrad/s' }), field('ttl', 'Vigencia de consigna', 'u16', '400', { min: 200, max: 1000, unit: 'ms' })], reserved)],
  0x14: [schema('wheel', 'Prueba de rueda', [wheel(), speed('speed', 'Velocidad'), field('duration', 'Duración', 'u16', '500', { min: 100, max: 2000, unit: 'ms' })], reserved)],
  0x15: [schema('mode', 'Modo de operación', [field('mode', 'Modo', 'u8', '0', { options: choices('Reposo / detener', 'Manual', 'Seguimiento de línea') })], reserved)],
  0x16: [schema('goto-xy', 'Destino X / Y · 12 B', xy(), reserved), schema('goto-pose', 'Destino, orientación y velocidad · 16 B', [...xy(), field('yaw', 'Orientación final', 'i16', '0', { unit: 'mrad' }), field('speed', 'Límite de velocidad', 'u16', '100', { min: 1, unit: 'mm/s' })], reserved)],
  0x1a: [schema('config', 'Consultar / escribir un parámetro', [field('write', 'Operación', 'u8', '0', { options: choices('Consultar', 'Escribir') }), field('action', 'ID del parámetro', 'u8'), field('value', 'Valor del parámetro', 'i32')], reserved)],
  0x1b: [schema('prefs', 'Preferencias', [field('operation', 'Operación', 'u8', '0', { options: choices('Guardar', 'Descartar') })], reserved)],
  0x1c: [schema('line-cal', 'Calibración de línea', [field('operation', 'Operación', 'u8', '0', { options: choices('Confirmar calibración', 'Aplicar tipo de lectura actual') })], reserved)],
  0x21: [schema('ui-page', 'Página de framebuffer', [revision(), field('offset', 'Offset del bloque', 'u16', '0', { max: 1023, unit: 'bytes' }), field('count', 'Cantidad de bytes', 'u8', '40', { min: 1, max: 40 })])],
  0x22: [schema('ui-action', 'Encoder: giro o SW', [revision(), field('action', 'Acción', 'u8', '1', { options: choices('Girar', 'SW corto · confirmar', 'SW largo · menú') }), field('delta', 'Paso (giro ±1; SW 0)', 'i8', '0', { min: -1, max: 1 })])],
  0x23: [schema('ui-release', 'Liberar captura', [revision()])],
  0x30: [schema('wifi-poll', 'Página del trabajo Wi-Fi', [field('page', 'Página', 'u8')])],
  0x40: [schema('seq-timed', 'Secuencia temporal · 13 B', [seq(), count(), total()], reserved), schema('seq-wheels', 'Secuencia por rueda · 14 B', [seq(), count(), total(), field('mode', 'Sincronización', 'u8', '1', { options: [{ value: '1', label: 'Ruedas independientes' }, { value: '2', label: 'Barrera entre ruedas' }] })], reserved)],
  0x41: [schema('append-timed', 'Segmento temporal · 30 B', [seq(), field('index', 'Índice del segmento', 'u8', '0', { max: 31 }), field('duration', 'Duración', 'u32', '1000', { min: 10, max: 120000, step: 10, unit: 'ms' }), ...['FL', 'FR', 'RL', 'RR'].map(name => speed(`from${name}`, `${name} inicial`)), ...['FL', 'FR', 'RL', 'RR'].map(name => speed(`to${name}`, `${name} final`)), kind()], reserved), schema('append-wheel', 'Registro de una rueda · 28 B', [seq(), field('index', 'Índice del registro', 'u8', '0', { max: 31 }), wheel(), field('completion', 'Finalización', 'u8', '0', { options: choices('Tiempo', 'Distancia de rueda') }), kind(), field('limit', 'Límite (ms o mm)', 'u32', '1000', { min: 1 }), field('timeout', 'Tiempo máximo', 'u32', '1000', { min: 10, max: 120000, step: 10, unit: 'ms' }), field('ramp', 'Rampa', 'u32', '100', { min: 10, step: 10, unit: 'ms' }), speed('from', 'Velocidad inicial'), speed('to', 'Velocidad final')], reserved)],
  0x42: [schema('commit', 'Cerrar carga', [seq(), count(), total()], reserved)],
  0x43: [schema('start', 'Iniciar secuencia', [seq(), execution()], reserved)],
  0x44: [schema('cancel', 'Cancelar secuencia', [seq(), execution(), field('reason', 'Motivo', 'u8')], reserved)],
  0x45: [schema('status', 'Consultar secuencia', [seq(), field('execution', 'ID de ejecución (0 antes de iniciar)', 'u32')])],
  0x50: [schema('info', 'Identidad de compilación', [field('page', 'Página', 'u8', '0', { options: choices('Versión', 'Fecha y hora', 'Compilador', 'Arquitectura y reloj', 'Revisión') })])],
  0x51: [schema('reboot', 'Confirmación de reinicio A5', [], [0xa5])],
  0x61: [schema('us-config', 'Intervalo ultrasónico', [field('interval', 'Intervalo', 'u16', '70', { min: 70, max: 1000, unit: 'ms' })])],
  0x64: [schema('servo', 'Control del servo', [field('operation', 'Operación', 'u8', '2', { options: choices('Desactivar', 'Ángulo', 'Centrar') }), field('angle', 'Ángulo (operación Ángulo)', 'u8', '90', { max: 180, unit: '°' })])],
  0x65: [schema('radar-config', 'Configuración del radar', [field('write', 'Operación', 'u8', '0', { options: choices('Consultar', 'Aplicar') }), field('min', 'Ángulo mínimo', 'u8', '0', { max: 175, step: 5, unit: '°' }), field('max', 'Ángulo máximo', 'u8', '180', { min: 5, max: 180, step: 5, unit: '°' }), field('speed', 'Velocidad nominal', 'u8', '10', { min: 1, max: 30, unit: '°/s' }), field('settle', 'Estabilización', 'u16', '100', { min: 50, max: 500, unit: 'ms' }), field('interval', 'Intervalo ultrasónico', 'u16', '70', { min: 70, max: 1000, unit: 'ms' }), field('inverted', 'Sentido invertido', 'u8', '0', { options: choices('No', 'Sí') })])],
  0x68: [schema('radar-get', 'Lectura de radar', [field('mode', 'Modo de lectura', 'u8', '0', { options: choices('Snapshot', 'Delta') }), field('cursor', 'Cursor', 'u32')])],
};

export function requestSchemas(command: number): RequestSchema[] {
  if (SCHEMAS[command]) return SCHEMAS[command];
  return commandById(command)?.requestBytes === 0 ? [schema('empty', 'Consulta sin cuerpo', [])] : [];
}
export function initialParameters(form: RequestSchema | undefined): Record<string, string> {
  return Object.fromEntries(form?.fields.map(item => [item.id, item.initial]) ?? []);
}

const LIMITS: Record<IntegerType, [number, number, number]> = { u8: [0, 255, 1], i8: [-128, 127, 1], u16: [0, 65535, 2], i16: [-32768, 32767, 2], u32: [0, 4294967295, 4], i32: [-2147483648, 2147483647, 4] };
export function encodeParameters(form: RequestSchema, values: Record<string, string>): string {
  const bytes = [...(form.prefix ?? [])];
  const numbers: Record<string, number> = {};
  for (const item of form.fields) {
    const raw = values[item.id] ?? '';
    const value = Number(raw), [min, max, width] = LIMITS[item.type];
    if (!/^-?\d+$/.test(raw) || !Number.isSafeInteger(value) || value < (item.min ?? min) || value > (item.max ?? max) || (item.step && value % item.step !== 0) || (item.options && !item.options.some(option => option.value === raw))) throw new Error(`${item.label}: completa un entero válido dentro del rango indicado.`);
    numbers[item.id] = value;
    for (let i = 0; i < width; i++) bytes.push((value >>> (i * 8)) & 255);
  }
  if (form.id === 'ui-page' && numbers.offset + numbers.count > 1024) throw new Error('El bloque OLED supera los 1024 bytes del framebuffer.');
  if (form.id === 'ui-action' && (numbers.action === 0 ? Math.abs(numbers.delta) !== 1 : numbers.delta !== 0)) throw new Error('Giro requiere paso −1 o +1; SW corto/largo requiere paso 0.');
  if (form.id === 'radar-config' && numbers.write && numbers.min >= numbers.max) throw new Error('El ángulo mínimo debe ser menor que el máximo.');
  if (form.id === 'radar-get' && numbers.mode === 0 && numbers.cursor > 37) throw new Error('El cursor de snapshot debe estar entre 0 y 37.');
  if (form.id === 'append-wheel') {
    if (numbers.ramp > numbers.timeout) throw new Error('La rampa no puede superar el tiempo máximo.');
    if (numbers.completion === 0 && (numbers.limit !== numbers.timeout || numbers.limit % 10)) throw new Error('Por tiempo, límite y tiempo máximo deben coincidir y ser múltiplos de 10 ms.');
    if (numbers.completion === 1 && (numbers.limit > 10000 || (!numbers.from && !numbers.to))) throw new Error('Por distancia, el límite es 1..10000 mm y al menos una velocidad debe ser distinta de cero.');
  }
  return hexBytes(bytes);
}
