/* AFR2-20261102-24 / 25. Recorridos del AFR2 Loader 1.1.4, tomados de AutoFinalRobo2-Loader
 * (README.md, MCP.md, FILESYSTEM_SCRIPTS.md, src/Operations*.cs, src/McpServer.cs).
 * Describen la fuente del Loader; no acreditan una carga ni el cableado de una placa. */
import type { UiIconName } from '../../components/ui/UiIcon';

export type LoaderJourney = 'mega' | 'esp';
/** Zonas visuales compartidas con Infraestructura: pc celeste, link rubí (Loader), esp ámbar, client verde. */
export type LoaderZone = 'pc' | 'link' | 'esp' | 'client';
export interface LoaderStep {
  title: string; short: string; zone: LoaderZone; place: string; icon: UiIconName;
  input: string; output: string; points: string[]; details: string[];
}

export const LOADER_VERSION = '2.0.0';

export const LOADER_JOURNEYS: Record<LoaderJourney, LoaderStep[]> = {
  mega: [
    {
      title: 'Perfil y puerto', short: 'Destino ATmega', zone: 'pc', place: 'En la computadora', icon: 'settings',
      input: 'Perfil JSON + COM', output: 'Operación configurada',
      points: [
        'Perfiles personalizados',
        'Se indican el HEX y los baudios',
        'Se elige el puerto y el destino',
        'Firmware completo o solo comunicación (flasher)',
      ],
      details: [
        'Configuración y registros viven fuera del repositorio: otro equipo usa su propio perfil.',
        'El COM se comprueba justo antes de operar, con el monitor cerrado.',
      ],
    },
    {
      title: 'Compilar', short: 'make → .hex', zone: 'pc', place: 'En la computadora', icon: 'programs',
      input: 'Fuentes C del ATmega', output: 'HEX del objetivo',
      points: [
        'make: producto, comunicación y UI',
        'Salida completa en el registro',
        'Flashear se habilita tras compilar',
      ],
      details: [
        'Cambiar perfil, fuentes o imágenes obliga a preparar de nuevo.',
        'Un error de compilación detiene el flujo; no se carga un HEX anterior.',
      ],
    },
    {
      title: 'Backup de la placa', short: 'flash + hfuse', zone: 'link', place: 'AFR2 Loader', icon: 'lock',
      input: 'ATmega2560 conectado', output: 'Copia .bin verificada',
      points: [
        'Firma del chip: 0x1E9801',
        'Lee flash y hfuse dos veces',
        'Compara las lecturas',
      ],
      details: [
        'Si las lecturas no coinciden o falla la firma, la carga no sigue.',
        '«Flashear último backup» restaura la copia y la verifica.',
      ],
    },
    {
      title: 'Escribir', short: 'avrdude flash:w', zone: 'link', place: 'Del Loader a la placa', icon: 'arrow-down',
      input: 'HEX + bootloader por USB', output: 'Flash escrita',
      points: [
        'avrdude por el bootloader del Mega',
        'Progreso y tiempo sin salida a la vista',
        'Cancelar deja la operación incompleta',
      ],
      details: [
        'El progreso indeterminado se muestra como actividad, sin suponer un bloqueo.',
        'La salida de avrdude queda disponible ante un error.',
      ],
    },
    {
      title: 'Verificar', short: 'Lectura posterior', zone: 'esp', place: 'En el ATmega2560', icon: 'check',
      input: 'Flash escrita', output: 'Carga confirmada',
      points: [
        'Relee la flash y la contrasta',
        'avrdude -n -D -U flash:v',
        'No escribe ni borra nada',
      ],
      details: [
        'Sin verificación correcta la carga no se marca como hecha.',
        'El funcionamiento del auto se comprueba aparte.',
      ],
    },
  ],
  esp: [
    {
      title: 'Transporte y capacidad', short: 'USB-TTL o Mega', zone: 'pc', place: 'En la computadora', icon: 'settings',
      input: 'Perfil + cableado', output: 'Transporte y flash confirmados',
      points: [
        'usb-ttl: adaptador de 3,3 V',
        'mega_reset: puente con RESET a GND',
        'mega_bridge: flasher en D18/D19',
        'Asistente de cableado y verificación previa',
      ],
      details: [
        'La capacidad de flash se consulta con esptool (flash_id).',
        'Un error no cambia de transporte por su cuenta.',
      ],
    },
    {
      title: 'Preparar imágenes', short: 'Firmware + LittleFS', zone: 'pc', place: 'En la computadora', icon: 'programs',
      input: 'Fuentes ESP + Web', output: 'firmware.bin + littlefs.bin',
      points: [
        'Firmware ESP con PlatformIO',
        'LittleFS con el script del perfil',
        'Web: Prepare-Web.ps1 → npm run build:esp',
      ],
      details: [
        'Layout del linker: 4 MiB con 3 MiB de LittleFS.',
        'Otros proyectos pueden usar su propio script.',
      ],
    },
    {
      title: 'Modo de carga', short: 'KEY + RST', zone: 'link', place: 'AFR2 Loader', icon: 'touch',
      input: 'Botones del shield', output: 'Cargador ROM listo',
      points: [
        'Mantener KEY y pulsar RST',
        'Escucha el arranque a 74 880 baudios',
        'Consulta la ROM (read_mac) sin escribir',
      ],
      details: [
        'La intervención física es siempre local; la ventana vuelve al frente.',
        'Si la ROM no responde, se ofrece repetir KEY/RST sin reescribir.',
      ],
    },
    {
      title: 'Escribir', short: 'esptool write_flash', zone: 'link', place: 'Del Loader al ESP', icon: 'arrow-down',
      input: 'Imágenes + offsets', output: 'Flash escrita',
      points: [
        'Firmware en 0x0, LittleFS según layout',
        'Conserva modo y frecuencia SPI',
        'Progreso en vivo, por imagen',
      ],
      details: [
        'Un 100 % no reemplaza la verificación.',
        'Tras 15 s sin salida cambia el color del estado, sin suponer un bloqueo.',
      ],
    },
    {
      title: 'Verificar', short: 'Hash por imagen', zone: 'esp', place: 'En el ESP8266', icon: 'check',
      input: 'Flash escrita', output: 'Hashes confirmados',
      points: [
        'Hash de cada imagen en la misma sesión',
        'Se exige la confirmación por imagen',
        'Sin ella, la carga queda sin verificar',
      ],
      details: [
        'No se agregan reconexiones que vuelvan a depender de KEY/RST.',
        'Que arranque se ve después en la Web o la OLED.',
      ],
    },
  ],
};

export const LOADER_REASONS: readonly { icon: UiIconName; title: string; text: string }[] = [
  { icon: 'settings', title: 'Cargas repetibles', text: 'Un perfil por equipo: rutas, puertos, HEX e imágenes. Misma operación, mismo resultado.' },
  { icon: 'check', title: 'Nada a ciegas', text: 'Firma del chip, doble lectura, hash por imagen y lectura posterior. Sin reintentos que oculten un fallo.' },
  { icon: 'touch', title: 'Guía física', text: 'Cableado según el transporte y pasos KEY/RST en primer plano. Se verifica antes de escribir.' },
  { icon: 'programs', title: 'Automatizable', text: 'Servidor MCP por stdio: una IA consulta el estado y pide operaciones; la GUI las muestra.' },
];

export const LOADER_TOOLS: readonly { icon: UiIconName; title: string; text: string }[] = [
  { icon: 'screen', title: 'Monitor serie', text: 'ASCII, HEX o texto. Envío de hasta 64 KiB, 1–1000 repeticiones, registros .rx y .tx.' },
  { icon: 'redo', title: 'Conversor HEX / ASCII', text: 'Independiente del puerto; recibe la selección del monitor.' },
  { icon: 'board', title: 'Flasher del Mega', text: 'Puente USB↔USART1↔ESP para cargar el ESP sin adaptador, con backup y restauración.' },
  { icon: 'book', title: 'Manual integrado', text: 'Ayuda por campo y manual HTML con cableado, transporte ESP y MCP.' },
];

/** Operaciones del Loader (loader_request_operation) con su efecto en una línea. */
export const LOADER_OPERATIONS: readonly { name: string; text: string }[] = [
  { name: 'build_mega', text: 'Compila el firmware del ATmega a HEX.' },
  { name: 'build_esp', text: 'Compila el firmware del ESP con PlatformIO.' },
  { name: 'build_web_fs', text: 'Arma la imagen LittleFS con la Web.' },
  { name: 'build_all', text: 'Compila Mega, ESP y Web en una pasada.' },
  { name: 'flash_mega', text: 'Escribe el HEX en el ATmega con avrdude.' },
  { name: 'flash_esp', text: 'Escribe el firmware en el ESP con esptool.' },
  { name: 'flash_fs', text: 'Escribe solo la imagen LittleFS.' },
  { name: 'flash_esp_fs', text: 'Escribe firmware y LittleFS juntos.' },
  { name: 'flash_id', text: 'Lee el ID y la capacidad de la flash del ESP.' },
  { name: 'verify_flash', text: 'Contrasta la flash con lo escrito, sin modificarla.' },
  { name: 'install_mega_flasher', text: 'Instala en el Mega el puente hacia el ESP.' },
  { name: 'verify_mega_flasher', text: 'Comprueba el puente instalado, sin escribir.' },
  { name: 'flash_mega_backup', text: 'Restaura el último backup del Mega.' },
  { name: 'restore_mega_product', text: 'Devuelve el firmware del auto tras usar el flasher.' },
];
