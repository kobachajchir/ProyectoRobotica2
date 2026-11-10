import type { UiIconName } from '../../components/ui/UiIcon';
import { PVC25_V17, type ArmAngles } from '../collector/robotModel';

/** AFR2-20261109-04. Presentation timeline, independent of live robot state. */
export const ASSEMBLY_CHAPTERS: readonly {
  at: number; code: string; title: string; short: string; text: string;
  icon: UiIconName; side: 'left' | 'right';
}[] = [
  { at: 0, code: 'C100', title: 'Todo empieza por la estructura.', short: 'Chasis', text: 'La placa inferior es el punto de partida. Cada pieza encuentra su lugar alrededor de ella.', icon: 'cube', side: 'left' },
  { at: .065, code: 'C100', title: 'Separar. Alinear. Fijar.', short: 'Fijaciones', text: 'Los separadores dan altura al chasis. Sus fijaciones mantienen conectadas las dos placas.', icon: 'settings', side: 'right' },
  { at: .16, code: 'C200', title: 'La fuerza empieza adentro.', short: 'Motores', text: 'Los soportes y los cuatro motores se montan sobre la estructura.', icon: 'wheel', side: 'left' },
  { at: .21, code: 'C200', title: 'Cuatro ruedas. Más direcciones.', short: 'Ruedas', text: 'Las ruedas Mecanum llegan desde los laterales, se alinean con los ejes y reciben sus fijaciones.', icon: 'wheel', side: 'left' },
  { at: .255, code: 'C100', title: 'Una segunda capa.', short: 'Placa superior', text: 'La placa superior baja sobre los separadores y recibe sus fijaciones.', icon: 'cube', side: 'right' },
  { at: .29, code: 'C300', title: 'Un cerebro, varias funciones.', short: 'Control', text: 'Mega, driver Model Y y shield Wi-Fi: control, potencia y comunicación.', icon: 'board', side: 'left' },
  { at: .37, code: 'C500', title: 'El robot percibe su entorno.', short: 'Sensores', text: 'Los sensores de línea, el servo, el ultrasonido y su soporte frontal encuentran su lugar en el auto.', icon: 'sensors', side: 'right' },
  { at: .43, code: 'C600', title: 'OLED y encoder rotativo.', short: 'OLED y encoder', text: 'OLED de 0,96″ · 128×64 · I²C. Encoder rotativo con botón: girar para navegar, pulsar para confirmar y mantener para acceder a controles rápidos.', icon: 'screen', side: 'left' },
  { at: .482, code: 'SENSOR A5', title: 'Sensor analógico de batería.', short: 'Sensor de batería', text: 'Dos resistencias de 10 kΩ adaptan 8 V de batería (referencia de 100%) a 4 V en A5. El capacitor 104 filtra la señal; la lectura analógica se reconvierte a voltaje y se mapea al nivel de batería.', icon: 'board', side: 'left' },
  { at: .665, code: 'A100', title: 'Ahora, el brazo.', short: 'Montura', text: 'La montura y el control fijo se incorporan al chasis. Es la unión entre la plataforma móvil y el manipulador.', icon: 'arm', side: 'right' },
  { at: .705, code: 'A200', title: 'La base orienta el movimiento.', short: 'J1 · J2', text: 'Se integran el giro J1, la torre y la entrada J2. El hombro inicia la cadena articulada.', icon: 'arm', side: 'left' },
  { at: .765, code: 'A300 · A400', title: 'El alcance toma forma.', short: 'J3 · J4', text: 'El eslabón y el accionamiento remoto J3 se conectan con el antebrazo y el giro de muñeca J4.', icon: 'arm', side: 'right' },
  { at: .825, code: 'A500 · A600', title: 'La última unión es el agarre.', short: 'Muñeca y pinza', text: 'La inclinación J5, el soporte J6 y la pinza V7 completan el brazo. Las mordazas se incorporan al final.', icon: 'arm', side: 'left' },
  { at: .89, code: 'ROBOT COMPLETO', title: 'Cada conjunto, un movimiento.', short: 'Conjunto', text: 'Nos acercamos al robot mientras la cámara lo recorre y el brazo se orienta hacia la caja.', icon: 'cube', side: 'right' },
  { at: .95, code: 'DEMOSTRACIÓN', title: 'Del ensamblaje a la acción.', short: 'Agarre', text: 'La pinza se aproxima, cierra sobre la caja y la eleva, ahora en primer plano.', icon: 'arm', side: 'left' },
];

export const clamp01 = (value: number) => Math.min(1, Math.max(0, value));
export const segment = (progress: number, from: number, to: number) => clamp01((progress - from) / (to - from));

export const ADDITIONS_FROM = .43, ADDITIONS_TO = .60;
export const FINAL_CLOSEUP_FROM = (.825 + .89) / 2;
const CLOCK_ANCHORS = [[0, 0], [.025, 0], [.065, .07], [.16, .19], [.21, .275], [.255, .28], [.29, .35], [.37, .44], [.43, .53], [.60, .53], [.665, .61], [.705, .66], [.765, .73], [.825, .80], [.89, .87], [.95, .94], [1, 1]];
/** Hold the existing mechanical timeline while chapter7 presents the additions. */
export function mechanicalProgress(progress: number) {
  for (let i = 1; i < CLOCK_ANCHORS.length; i++) {
    const [to, end] = CLOCK_ANCHORS[i], [from, start] = CLOCK_ANCHORS[i - 1];
    if (progress <= to) return start + (end - start) * segment(progress, from, to);
  }
  return 1;
}

/** Solve the existing --ease-in-out curve (0.77, 0, 0.175, 1).
 * Scroll is the clock; no elapsed-time integration or delayed spring state. */
export function ease(value: number) {
  const x = clamp01(value);
  if (x === 0 || x === 1) return x;
  let low = 0, high = 1, t = x;
  for (let i = 0; i < 14; i++) {
    t = (low + high) / 2;
    const sample = 3 * (1 - t) ** 2 * t * .77 + 3 * (1 - t) * t ** 2 * .175 + t ** 3;
    if (sample < x) low = t; else high = t;
  }
  return 3 * (1 - t) * t ** 2 + t ** 3;
}

export function chapterIndex(progress: number) {
  for (let i = ASSEMBLY_CHAPTERS.length - 1; i >= 0; i--) if (progress >= ASSEMBLY_CHAPTERS[i].at) return i;
  return 0;
}

export const ARM_ASSEMBLY_STAGES: Record<string, [number, number]> = {
  'base-fixed': [.61, .655], j1: [.66, .69], 'drive-j2': [.675, .71], j2: [.695, .73],
  'drive-j3': [.73, .755], j3: [.742, .775], 'drive-j4': [.76, .79], j4: [.77, .80],
  j5: [.80, .825], j6: [.815, .84], 'drive-grip': [.827, .852],
  'jaw-left': [.84, .865], 'jaw-right': [.845, .87],
};

// Authored illustration poses in the V17 descriptor's joint frames, in degrees.
// J2 + J3 + J5 = 180° at contact: the TCP points down toward the floor.
const arm = PVC25_V17;
const shoulderHeight = arm.joints[0].origin.position[2] + arm.joints[1].origin.position[2];
const upper = arm.joints[2].origin.position[2];
const forearm = arm.joints[3].origin.position[2] + arm.joints[4].origin.position[2];
const tool = arm.joints[5].origin.position[2] + arm.tcp.position[2];
const elbow = 30 * Math.PI / 180;
const a = upper + forearm * Math.cos(elbow), b = forearm * Math.sin(elbow);
// Place the contact TCP at the center of the existing 50mm cube, on the floor.
const shoulder = (Math.acos((.025 - arm.mounts.mounted.position[2] - shoulderHeight + tool) / Math.hypot(a, b)) - Math.atan2(b, a)) * 180 / Math.PI;
const REACH: ArmAngles = [55, shoulder, 30, 0, 150 - shoulder, 0];
// Raise the load to the middle of the product shot, keeping the tool downward
// (J2+J3+J5=180deg) and the same closed jaw spacing throughout the lift.
const LIFT: ArmAngles = [55, 62, 30, 0, 88, 0];
export const CONTACT_PROGRESS = .969;
export const HOLD_PROGRESS = .982;
export function assemblyPose(progress: number) {
  const approach = ease(segment(progress, .89, CONTACT_PROGRESS));
  const lift = ease(segment(progress, HOLD_PROGRESS, 1));
  const q = PVC25_V17.initial.map((angle, i) => angle + (REACH[i] - angle) * approach + (LIFT[i] - REACH[i]) * lift);
  const closing = ease(segment(progress, CONTACT_PROGRESS, HOLD_PROGRESS));
  return { q, opening: PVC25_V17.gripper.open + (50 - PVC25_V17.gripper.open) * closing };
}
