/**
 * AFR2-20261101-12 / VIS-053. Destinos de la Web ordenados por tarea (plan visual 01-11, fase 5).
 * Una sola definición para barra lateral, barra inferior, hoja «Más», título de la barra
 * superior e Inicio. Configuración es un modal y Desarrollador se abre desde Configuración.
 */
import type { UiIconName } from '../components/ui/UiIcon';

export type NavGroupId = 'operar' | 'preparar' | 'explorar' | 'sistema';

export interface NavDestination {
  to: string;
  label: string;
  icon: UiIconName;
  group: NavGroupId | null;
  description: string;
}

export const NAV_GROUPS: readonly { id: NavGroupId; label: string }[] = [
  { id: 'operar', label: 'Operar' },
  { id: 'preparar', label: 'Preparar' },
  { id: 'explorar', label: 'Explorar' },
  { id: 'sistema', label: 'Sistema' },
];

export const HOME: NavDestination = { to: '/', label: 'Inicio', icon: 'home', group: null, description: 'Accesos a cada tarea.' };

export const DESTINATIONS: readonly NavDestination[] = [
  { to: '/conducir', label: 'Conducir', icon: 'control', group: 'operar', description: 'Mové la base en cualquier dirección y mirá los sensores en vivo.' },
  { to: '/programas', label: 'Programas', icon: 'programs', group: 'operar', description: 'Armá recorridos, simulalos y ejecutalos en el robot.' },
  { to: '/puesta-a-punto', label: 'Puesta a punto', icon: 'setup', group: 'preparar', description: 'Probá cada rueda y calibrá antes de operar.' },
  { to: '/sensores', label: 'Sensores', icon: 'sensors', group: 'preparar', description: 'Distancia, línea y ruedas, con la antigüedad de cada lectura.' },
  { to: '/modelos', label: 'Modelos y simulación', icon: 'cube', group: 'explorar', description: 'Explorá el auto y el brazo V17, y simulá sus movimientos.' },
  { to: '/infraestructura', label: 'Infraestructura', icon: 'board', group: 'explorar', description: 'Seguí el viaje del código y los modelos 3D hasta la memoria del robot y tu navegador.' },
  { to: '/loader', label: 'Loader', icon: 'arrow-down', group: 'explorar', description: 'Cómo se compilan, cargan y verifican el ATmega, el ESP y la Web desde Windows.' },
  { to: '/conexion', label: 'Conexión', icon: 'wifi', group: 'sistema', description: 'Wi-Fi, punto de acceso y enlace entre el ESP y el ATmega.' },
  { to: '/ayuda', label: 'Ayuda', icon: 'help', group: 'sistema', description: 'Cómo funciona cada módulo y cada pantalla de la OLED.' },
];

/** Barra inferior en celular: cuatro destinos y «Más». */
export const BOTTOM_NAV: readonly string[] = ['/', '/conducir', '/programas', '/sensores'];

/** Destinos que van en la hoja «Más» del celular. */
export const MORE_DESTINATIONS = DESTINATIONS.filter(item => !BOTTOM_NAV.includes(item.to));

/** Rutas que pertenecen a un destino además de su prefijo propio (el simulador es parte de Modelos). */
export function belongsTo(to: string, pathname: string) {
  if (to === '/') return pathname === '/';
  return pathname === to || pathname.startsWith(`${to}/`) || (to === '/modelos' && pathname.startsWith('/simulacion'));
}

export function destination(to: string): NavDestination | undefined {
  return to === '/' ? HOME : DESTINATIONS.find(item => item.to === to);
}

/** Título de la barra superior según la ruta actual. */
export function titleForPath(pathname: string): string {
  if (pathname.startsWith('/desarrollador/protocol-studio')) return 'Protocol Studio';
  if (pathname.startsWith('/desarrollador')) return 'Desarrollador';
  if (pathname.startsWith('/simulacion')) return 'Modelos y simulación';
  return DESTINATIONS.find(item => pathname === item.to || pathname.startsWith(`${item.to}/`))?.label ?? HOME.label;
}
