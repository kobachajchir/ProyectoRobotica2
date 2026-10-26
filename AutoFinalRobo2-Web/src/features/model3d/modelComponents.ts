import { Box3, PropertyBinding, Vector3, type Object3D } from 'three';
import type { UiIconName } from '../../components/ui/UiIcon';

export interface ModelComponent {
  id: string;
  label: string;
  model: string;
  icon: UiIconName;
  description: string;
  facts: [string, string][];
  to: string;
  destination: string;
}

export const MODEL_COMPONENTS: ModelComponent[] = [
  {
    id: 'servo', label: 'Servomotor', model: 'SG90', icon: 'servo',
    description: 'Orienta el soporte del ultrasonido alrededor de su eje. El cuerpo del servo permanece fijo al chasis.',
    facts: [['Recorrido del programa', '0° a 180°'], ['Orientacion', '0° derecha · 90° frente · 180° izquierda'], ['Modos', 'Angulo manual o barrido automatico']],
    to: '/sensores?seccion=distancia', destination: 'Distancia y servo',
  },
  {
    id: 'ultrasound', label: 'Sensor ultrasonico', model: 'HC-SR04', icon: 'radar',
    description: 'Emite un pulso ultrasonico y mide el tiempo de regreso del eco para calcular la distancia frontal. El servo cambia la direccion de cada medicion.',
    facts: [['Medicion', 'Distancia por tiempo de eco'], ['Mapa', 'Muestras asociadas al angulo del servo'], ['Sin eco', 'No equivale a camino libre']],
    to: '/sensores?seccion=distancia', destination: 'Distancia y servo',
  },
  {
    id: 'fl', label: 'Ruedas Mecanum', model: 'Mecanum · 4WD', icon: 'wheel',
    description: 'Las cuatro ruedas comparten este diseño. Sus rodillos inclinados permiten combinar avance, desplazamiento lateral y giro al coordinarlas.',
    facts: [['Referencia mostrada', 'FL · Delantera izquierda'], ['Rueda del modelo', 'Mecanum de 80 mm'], ['Motor', 'GM25-370']],
    to: '/control?tarea=wheel&rueda=FL', destination: 'Prueba de rueda FL',
  },
  {
    id: 'atmega', label: 'Placa controladora', model: 'Mega 2560 · ATmega2560', icon: 'panel',
    description: 'Ejecuta el control del robot: motores, sensores y navegacion de la OLED. Recibe las ordenes de la Web a traves del ESP y aplica las salidas fisicas.',
    facts: [['Funcion', 'Control de motores y sensores'], ['Enlace con ESP', 'UART'], ['Interfaz local', 'OLED y encoder']],
    to: '/control', destination: 'Control del robot',
  },
  {
    id: 'esp', label: 'Placa de comunicacion', model: 'ESP-12S · ESP8266 / shield OSOYOO', icon: 'wifi',
    description: 'Conecta el robot por Wi-Fi, sirve esta aplicacion y comunica las ordenes con el ATmega. Puede trabajar en una red Wi-Fi o como punto de acceso.',
    facts: [['Red', 'Wi-Fi STA / AP'], ['Aplicacion', 'Servidor Web y WebSocket'], ['Archivos', 'LittleFS']],
    to: '/wifi', destination: 'Configuracion Wi-Fi',
  },
  {
    id: 'oled', label: 'Pantalla OLED', model: 'OLED 0,96″ · 128 × 64', icon: 'screen',
    description: 'Muestra los menus, mediciones y avisos del robot. El visor de la aplicacion permite consultar la pantalla y acceder a sus controles.',
    facts: [['Resolucion', '128 × 64 pixeles'], ['Conexion', 'I²C'], ['Navegacion', 'Encoder y un unico SW']],
    to: '/oled', destination: 'Pantalla y controles',
  },
  {
    id: 'encoder', label: 'Encoder de navegacion', model: 'KY-040 · sin breakout', icon: 'settings',
    description: 'El mando giratorio recorre las opciones de la pantalla. Su pulsador SW confirma una opcion o abre el menu segun la duracion de la pulsacion.',
    facts: [['Girar', 'Recorrer opciones'], ['SW corto', 'Confirmar'], ['SW largo · 800 ms', 'Abrir menu principal'], ['Volver', 'Opcion del submenu']],
    to: '/oled', destination: 'Pantalla y controles',
  },
  {
    id: 'line', label: 'Sensor de linea inferior', model: 'Arreglo infrarrojo · 5 IR', icon: 'sensors',
    description: 'Observa la reflectancia del suelo con cinco canales infrarrojos. Sus señales permiten distinguir la linea del fondo bajo la parte delantera del chasis.',
    facts: [['Ubicacion', 'Parte inferior delantera'], ['Canales', 'IR1 a IR5'], ['Lectura', 'Señal de reflectancia']],
    to: '/sensores?seccion=linea', destination: 'Linea IR',
  },
];

/** References in the current CAD asset, in meters. No edits to shared geometry.
 * OLED/encoder are merged into one exported node: their local references are
 * inferred from its display face / knob bounds, not physical metrology. */
export function getComponentAnchors(scene: Object3D, offset: Vector3) {
  const anchors = new Map<string, Vector3>();
  scene.updateMatrixWorld(true);
  const find = (prefix: string) => {
    // GLTFLoader sanitizes CAD punctuation such as ':' and '.' in node names.
    const loadedPrefix = PropertyBinding.sanitizeNodeName(prefix);
    let part: Object3D | undefined;
    scene.traverse(node => { if (!part && node.name.startsWith(loadedPrefix)) part = node; });
    return part;
  };
  const bounds = (prefix: string) => {
    const part = find(prefix);
    return part ? new Box3().setFromObject(part, true) : null;
  };
  const put = (id: string, point: Vector3) => anchors.set(id, point.add(offset));
  const servo = bounds('AFR2_SG90:');
  const sensor = bounds('AFR2_HC_SR04:');
  if (servo && sensor) {
    const point = sensor.getCenter(new Vector3());
    const origin = find('AFR2_SG90:')!.getWorldPosition(new Vector3());
    put('servo', new Vector3(point.x, servo.max.y, origin.z));
  }
  if (sensor) { const point = sensor.getCenter(new Vector3()); point.z = sensor.min.z; put('ultrasound', point); }
  const mega = bounds('AFR2_Mega2560_CAD:');
  if (mega) { const point = mega.getCenter(new Vector3()); point.x = mega.max.x; put('atmega', point); }
  const wifi = find('AFR2_OSOYOOWifiShield:');
  if (wifi) put('esp', wifi.localToWorld(new Vector3(-.024, .006, -.008)));
  const panel = find('AFR2_AFR2_UI_B2_Fondo_OLED15_Auto5:');
  if (panel) {
    put('oled', panel.localToWorld(new Vector3(.058, .0538, .078)));
    put('encoder', panel.localToWorld(new Vector3(.058, .0663, .109)));
  }
  const line = bounds('AFR2_SensorLinea_5IR:');
  if (line) { const point = line.getCenter(new Vector3()); point.z = line.min.z; put('line', point); }
  scene.traverse(node => {
    if (!node.name.startsWith('AFR2_Rueda80_Mano')) return;
    const box = new Box3().setFromObject(node, true);
    const point = box.getCenter(new Vector3());
    const id = `${point.z < 0 ? 'f' : 'r'}${point.x < 0 ? 'l' : 'r'}`;
    // One representative wheel reference keeps the inspection view uncluttered.
    if (id !== 'fl') return;
    point.x = point.x < 0 ? box.min.x : box.max.x;
    put(id, point);
  });
  return anchors;
}
