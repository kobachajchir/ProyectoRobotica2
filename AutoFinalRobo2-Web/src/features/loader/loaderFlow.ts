/* AFR2-20261102-24. Recorridos del AFR2 Loader 1.1.4, tomados de AutoFinalRobo2-Loader
 * (README.md, MCP.md, FILESYSTEM_SCRIPTS.md, src/Operations*.cs, src/McpServer.cs).
 * Describen la fuente del Loader; no acreditan una carga ni el cableado de una placa. */
import type { UiIconName } from '../../components/ui/UiIcon';

export type LoaderJourney = 'mega' | 'esp';
/** Zonas visuales compartidas con Infraestructura: pc celeste, link rubí (Loader), esp ámbar, client verde. */
export type LoaderZone = 'pc' | 'link' | 'esp' | 'client';
export interface LoaderStep {
  title: string; short: string; zone: LoaderZone; place: string; icon: UiIconName;
  input: string; output: string; description: string; details: string[]; source: string;
}

export const LOADER_VERSION = '1.1.4';

export const LOADER_JOURNEYS: Record<LoaderJourney, LoaderStep[]> = {
  mega: [
    {
      title: 'Perfil y puerto', short: 'Destino ATmega', zone: 'pc', place: 'En la computadora', icon: 'settings',
      input: 'Perfil JSON + COM', output: 'Operación configurada',
      description: 'El perfil guarda rutas de avr-gcc, make y avrdude, el HEX de cada objetivo y los baudios. Se elige el puerto y el destino ATmega: firmware completo del auto o solo comunicación.',
      details: [
        'Configuración y registros viven fuera del repositorio: otro equipo usa su propio perfil sin tocar fuentes.',
        'Actualizar puertos y Detectar son acciones separadas. El COM se comprueba justo antes de operar, después de cerrar el monitor; es una comprobación puntual, no una reserva.',
      ],
      source: 'Loader/src/Profile.cs · MainForm.Profiles.cs · PortAvailability.cs',
    },
    {
      title: 'Compilar', short: 'make → .hex', zone: 'pc', place: 'En la computadora', icon: 'programs',
      input: 'Fuentes C del ATmega', output: 'HEX del objetivo',
      description: 'Ejecuta los objetivos de make del firmware (producto, comunicación y biblioteca de UI) y captura su salida en el registro de la operación.',
      details: [
        '«Flashear» solo se habilita tras compilar esa sección en la sesión actual. Cambiar perfil, fuentes o imágenes obliga a preparar de nuevo.',
        'Un error de compilación detiene el flujo y queda con su detalle; no se carga un HEX anterior por omisión.',
      ],
      source: 'Loader/src/BuildPreparation.cs · Atmega/Makefile',
    },
    {
      title: 'Backup de la placa', short: 'flash + hfuse', zone: 'link', place: 'AFR2 Loader', icon: 'lock',
      input: 'ATmega2560 conectado', output: 'Copia .bin verificada',
      description: 'Antes de escribir, lee la flash y el fusible alto dos veces y compara las lecturas. Comprueba la firma del chip (0x1E9801, ATmega2560) para no operar sobre otra placa.',
      details: [
        'Si las dos lecturas no coinciden, o falla la firma, la carga no sigue.',
        'El backup es opcional con aviso configurable. «Flashear último backup» restaura la copia completa y la contrasta con una lectura posterior.',
      ],
      source: 'Loader/src/Operations.MegaBackup.cs',
    },
    {
      title: 'Escribir', short: 'avrdude flash:w', zone: 'link', place: 'Del Loader a la placa', icon: 'arrow-down',
      input: 'HEX + bootloader por USB', output: 'Flash escrita',
      description: 'avrdude escribe la aplicación a través del bootloader del Mega por el puerto USB. La barra muestra el progreso que informa la herramienta y el tiempo sin salida.',
      details: [
        'El progreso indeterminado se muestra como actividad: no se infiere un bloqueo ni se cambian los plazos.',
        'Cancelar o un error dejan la operación como no completada, con la salida de avrdude disponible.',
      ],
      source: 'Loader/src/Operations.cs · Runner.cs',
    },
    {
      title: 'Verificar', short: 'Lectura posterior', zone: 'esp', place: 'En el ATmega2560', icon: 'check',
      input: 'Flash escrita', output: 'Carga confirmada',
      description: 'Vuelve a leer la memoria y la contrasta con lo que se quiso escribir. La verificación (avrdude -n -D -U flash:v) no escribe ni borra flash, EEPROM ni fusibles.',
      details: [
        'Sin verificación correcta la carga no se marca como hecha.',
        'Verificar confirma los bytes en la placa; el funcionamiento del auto se comprueba aparte.',
      ],
      source: 'Loader/src/Operations.VerifyMegaFlasher.cs',
    },
  ],
  esp: [
    {
      title: 'Transporte y capacidad', short: 'USB-TTL o Mega', zone: 'pc', place: 'En la computadora', icon: 'settings',
      input: 'Perfil + cableado', output: 'Transporte y flash confirmados',
      description: 'Se elige cómo llegar al ESP: adaptador USB-TTL de 3,3 V (usb-ttl), puente físico del Mega con RESET a GND (mega_reset) o flasher de software en D18/D19 con RESET libre (mega_bridge). El asistente dibuja el cableado de cada caso.',
      details: [
        'La capacidad de la flash se consulta con esptool (flash_id) y se confirma antes de ver las particiones.',
        'Un error no cambia de transporte por su cuenta: hay que elegirlo de nuevo.',
      ],
      source: 'Loader/MCP.md · src/Operations.MegaBridge.cs · EspFlashRules.cs',
    },
    {
      title: 'Preparar imágenes', short: 'Firmware + LittleFS', zone: 'pc', place: 'En la computadora', icon: 'programs',
      input: 'Fuentes ESP + Web', output: 'firmware.bin + littlefs.bin',
      description: 'Firmware ESP compila con PlatformIO; FileSystem ejecuta el script del perfil (para esta Web, scripts/Prepare-Web.ps1, que corre npm run build:esp) y arma la imagen de LittleFS. Se pueden preparar ambas.',
      details: [
        'El layout sale del linker del ESP8266 (4 MiB con 3 MiB de LittleFS): no se supone que toda la flash libre sea ejecutable.',
        'Otros proyectos pueden usar su propio script sin npm. El modo AT acepta un BIN completo reconocido por SHA-256.',
      ],
      source: 'Loader/FILESYSTEM_SCRIPTS.md · scripts/Prepare-Web.ps1 · Operations.Filesystem.cs',
    },
    {
      title: 'Modo de carga', short: 'KEY + RST', zone: 'link', place: 'AFR2 Loader', icon: 'touch',
      input: 'Botones del shield', output: 'Cargador ROM listo',
      description: 'El ESP entra al cargador ROM manteniendo KEY y pulsando RST. El Loader guía cada paso, puede escuchar el arranque a 74 880 baudios y consulta la ROM (read_mac) sin escribir antes de seguir.',
      details: [
        'La intervención física es siempre local: ni la autoaceptación MCP ni la omisión de confirmaciones la saltean, y la ventana vuelve al frente aunque esté minimizada.',
        'Si la ROM no responde, la verificación se detiene y se ofrece repetir KEY/RST, sin reescribir.',
      ],
      source: 'Loader/src/MainForm.EspIntervention.cs · BootObservation.cs',
    },
    {
      title: 'Escribir', short: 'esptool write_flash', zone: 'link', place: 'Del Loader al ESP', icon: 'arrow-down',
      input: 'Imágenes + offsets', output: 'Flash escrita',
      description: 'esptool escribe cada imagen en su offset (firmware en 0x0 y LittleFS donde indica el layout), conservando el modo y la frecuencia SPI de la placa. Python corre sin buffer para que el progreso llegue en vivo.',
      details: [
        'Cada porcentaje corresponde a una imagen: 100 % no reemplaza la verificación.',
        'Tras 15 s sin salida cambia el color del estado, sin suponer un bloqueo.',
      ],
      source: 'Loader/src/Operations.cs · Runner.cs',
    },
    {
      title: 'Verificar', short: 'Hash por imagen', zone: 'esp', place: 'En el ESP8266', icon: 'check',
      input: 'Flash escrita', output: 'Hashes confirmados',
      description: 'esptool verifica el hash de cada imagen en la misma sesión. El Loader exige esa confirmación por imagen y una salida correcta; si falta, la carga queda sin verificar.',
      details: [
        'No se agregan reconexiones ni verificaciones extra que vuelvan a depender de KEY/RST.',
        'Una placa verificada todavía debe arrancar: la respuesta del firmware se ve después en la Web o la OLED.',
      ],
      source: 'Loader/src/Operations.cs · README.md',
    },
  ],
};

export const LOADER_REASONS: readonly { icon: UiIconName; title: string; text: string }[] = [
  { icon: 'settings', title: 'Cargas repetibles', text: 'El perfil reúne rutas de herramientas, puertos, HEX, imágenes y scripts. La misma operación da el mismo resultado en otra computadora con su propio perfil, sin editar el código.' },
  { icon: 'check', title: 'Nada a ciegas', text: 'Firma del chip, doble lectura del backup, hash por imagen y lectura posterior. Una escritura sin verificar no se da por buena, y no hay reintentos automáticos que oculten un fallo.' },
  { icon: 'touch', title: 'Guía física', text: 'Asistentes con el cableado del transporte elegido y pasos de KEY/RST en primer plano. Las confirmaciones manuales se pueden omitir en la sesión; las comprobaciones técnicas, nunca.' },
  { icon: 'programs', title: 'Automatizable', text: 'Servidor MCP por stdio (JSON-RPC por líneas, sin puerto TCP). Una IA puede consultar el estado y pedir operaciones; la GUI muestra cada solicitud y las acciones físicas siguen siendo locales.' },
];

export const LOADER_TOOLS: readonly { icon: UiIconName; title: string; text: string }[] = [
  { icon: 'screen', title: 'Monitor serie', text: 'Recepción en ASCII, HEX o texto decodificado. Envío manual en ASCII, HEX o decimal con CR/LF opcionales, hasta 64 KiB por envío, 1–1000 repeticiones y pausa de 0 a 60 s. Registros .rx y .tx en binario y texto.' },
  { icon: 'redo', title: 'Conversor HEX / ASCII', text: 'Independiente del puerto: convierte HEX a ASCII legible o escapado y ASCII a HEX normalizado. Una selección del monitor se abre directo en el conversor.' },
  { icon: 'board', title: 'Flasher del Mega', text: 'Instala en el ATmega un puente USB↔USART1↔ESP para cargar el ESP sin adaptador externo, previo backup, y restaura después el firmware del auto desde ese backup.' },
  { icon: 'book', title: 'Manual integrado', text: 'Cada campo de Configuración tiene ayuda y acceso a su sección del manual HTML, con cableado, transporte ESP, MCP y recursos del curso.' },
];
