/** AFR2-20261028-09. AutoMicro Protocol Studio concepts adapted to ROBO2.
 * Original AutoMicro authorship remains in reference/web; this catalogue uses
 * Contracts/afr2_{wire,product_wire,network_wire,sensor_wire}.h and EX01v8 docs.
 * Lengths count application body bytes, excluding correlation and result. */
export interface StudioCommand {
  id: number;
  name: string;
  description: string;
  requestBytes?: number | number[];
  replyBytes?: number;
  example?: string;
  retired?: boolean;
  readOnly?: boolean;
}

export const COMMANDS: StudioCommand[] = [
  { id: 0x01, name: 'HELLO', description: 'Negocia AFR2 / perfil 2.', requestBytes: 5, replyBytes: 14, example: '41 46 52 32 02' },
  { id: 0x02, name: 'ALIVE', description: 'Consulta el tiempo de actividad.', requestBytes: 0, replyBytes: 4 },
  { id: 0x03, name: 'STATE', description: 'Estado base del robot.', requestBytes: 0, replyBytes: 7 },
  { id: 0x04, name: 'SENSORS', description: 'Línea y distancia del perfil base.', requestBytes: 0, replyBytes: 13 },
  { id: 0x05, name: 'ENCODERS', description: 'Cuentas Hall FL / FR / RL / RR.', requestBytes: 0, replyBytes: 22 },
  { id: 0x06, name: 'STOP', description: 'Solicita la parada autoritativa.', requestBytes: 0, replyBytes: 2 },
  { id: 0x10, name: 'EXT_HELLO', description: 'Negocia EX01; versión vigente 8.', requestBytes: 4, replyBytes: 10, example: '45 58 30 31' },
  { id: 0x11, name: 'EXT_STATE', description: 'Estado de producto y origen de actividad.', requestBytes: 0, replyBytes: 16 },
  { id: 0x12, name: 'WEB_PULSE', description: 'Edad de presencia Web en ms, u16 LE; no concede permisos.', requestBytes: 2, replyBytes: 2, example: '00 00' },
  { id: 0x13, name: 'DRIVE', description: 'Consigna de movimiento; reserved:u32 debe ser cero.', requestBytes: 12, replyBytes: 0 },
  { id: 0x14, name: 'WHEEL', description: 'Prueba de rueda; reserved:u32 debe ser cero.', requestBytes: 9, replyBytes: 0 },
  { id: 0x15, name: 'MODE', description: 'Modo de operación; reserved:u32 debe ser cero.', requestBytes: 5, replyBytes: 0 },
  { id: 0x16, name: 'GOTO', description: 'Destino relativo; reserved:u32 debe ser cero.', requestBytes: [12, 16], replyBytes: 0 },
  { id: 0x17, name: 'CANCEL', description: 'Cancela una operación de producto.', requestBytes: 0, replyBytes: 1 },
  { id: 0x18, name: 'POSE', description: 'Pose y odometría de producto.', requestBytes: 0, replyBytes: 25 },
  { id: 0x19, name: 'HALL_CAL', description: 'Retirado de UART en EX01v7; calibración por vuelta solo local.', retired: true },
  { id: 0x1a, name: 'CONFIG', description: 'Configuración de producto; cuerpo según operación.', requestBytes: 10, replyBytes: 4 },
  { id: 0x1b, name: 'PREFS', description: 'Preferencias de producto; cuerpo según operación.', requestBytes: 5, replyBytes: 0 },
  { id: 0x1c, name: 'LINE_CAL', description: 'Calibración de línea; cuerpo según operación.', requestBytes: 5, replyBytes: 1 },
  { id: 0x1d, name: 'LIMITS', description: 'Límites de movimiento.', requestBytes: 0, replyBytes: 8 },
  { id: 0x20, name: 'UI_META', description: 'Inicia una captura OLED coherente.', requestBytes: 0, replyBytes: 13 },
  { id: 0x21, name: 'UI_PAGE', description: 'Lee un bloque del framebuffer OLED.', requestBytes: 7 },
  { id: 0x22, name: 'UI_ACTION', description: 'Giro o SW con revisión de captura; largo abre menú.', requestBytes: 6, replyBytes: 5 },
  { id: 0x23, name: 'UI_RELEASE', description: 'Libera una captura OLED.', requestBytes: 4, replyBytes: 0 },
  { id: 0x30, name: 'WIFI_POLL', description: 'ESP consulta trabajo Wi-Fi iniciado localmente.', requestBytes: 1 },
  { id: 0x31, name: 'WIFI_UPDATE', description: 'ESP actualiza el trabajo Wi-Fi local.' },
  { id: 0x40, name: 'SEQ_BEGIN', description: 'Prepara secuencia temporal (13 B) o por rueda EX01v6+ (14 B); reserved:u32=0.', requestBytes: [13, 14], replyBytes: 10 },
  { id: 0x41, name: 'SEQ_APPEND', description: 'Segmento temporal (30 B) o registro por rueda EX01v6+ (28 B).', requestBytes: [28, 30], replyBytes: 6 },
  { id: 0x42, name: 'SEQ_COMMIT', description: 'Cierra la carga de una secuencia.', requestBytes: 13, replyBytes: 10 },
  { id: 0x43, name: 'SEQ_START', description: 'Inicia una secuencia preparada.', requestBytes: 12, replyBytes: 14 },
  { id: 0x44, name: 'SEQ_CANCEL', description: 'Cancela la secuencia identificada.', requestBytes: 13, replyBytes: 10 },
  { id: 0x45, name: 'SEQ_STATUS', description: 'Estado de programa y ejecución.', requestBytes: 8, replyBytes: 19 },
  { id: 0x50, name: 'INFO', description: 'Página de identidad de compilación (0..4).', requestBytes: 1, example: '00' },
  { id: 0x51, name: 'REBOOT', description: 'Solicitud explícita de reinicio ATmega.', requestBytes: 1, replyBytes: 5, example: 'A5' },
  { id: 0x52, name: 'BOOT', description: 'Evento BT01 de arranque, sin sesión/transacción; solo lectura.', readOnly: true },
  { id: 0x53, name: 'NETWORK_STATE', description: 'Estado de red ESP → ATmega para la interfaz local.' },
  { id: 0x54, name: 'NETWORK_EVENT', description: 'Aviso de red ESP → ATmega para la interfaz local.' },
  { id: 0x60, name: 'US_GET', description: 'Muestra ultrasónica y estado frontal.', requestBytes: 0, replyBytes: 35 },
  { id: 0x61, name: 'US_CONFIG', description: 'Intervalo ultrasónico 70..1000 ms, u16 LE.', requestBytes: 2, replyBytes: 2, example: '46 00' },
  { id: 0x62, name: 'IR_GET', description: 'Cinco sensores IR y validez de control.', requestBytes: 0, replyBytes: 27 },
  { id: 0x63, name: 'SERVO_GET', description: 'PWM aplicado y objetivo del servo.', requestBytes: 0, replyBytes: 18 },
  { id: 0x64, name: 'SERVO_SET', description: 'operation:u8 (0 off / 1 ángulo / 2 centro), angle:u8.', requestBytes: 2, replyBytes: 18, example: '02 00' },
  { id: 0x65, name: 'RADAR_CONFIG', description: 'write, min, max, speed, settle:u16, interval:u16, invert.', requestBytes: 9, replyBytes: 8, example: '00 00 B4 0A 64 00 46 00 00' },
  { id: 0x66, name: 'RADAR_START', description: 'Inicia barrido autónomo.', requestBytes: 0, replyBytes: 1 },
  { id: 0x67, name: 'RADAR_STOP', description: 'Detiene barrido y PWM.', requestBytes: 0, replyBytes: 1 },
  { id: 0x68, name: 'RADAR_GET', description: 'mode:u8 (0 snapshot / 1 delta), cursor:u32 LE.', requestBytes: 5, example: '00 00 00 00 00' },
  { id: 0x69, name: 'UI_STATUS', description: 'Código y revisión OLED; no captura imagen.', requestBytes: 0, replyBytes: 11 },
  { id: 0x6a, name: 'MOTION_HALT', description: 'Parada de tracción interna del ESP.', requestBytes: 0, replyBytes: 1 },
];

export const RESULT_NAMES = ['OK', 'BAD_REQUEST', 'CAPABILITY_UNAVAILABLE', 'SESSION_REQUIRED', 'INTERNAL_ERROR'];
export const commandById = (id: number) => COMMANDS.find(command => command.id === id);
export const bodySizeText = (size: number | number[] | undefined) => Array.isArray(size) ? size.join(' o ') : String(size);
