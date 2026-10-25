> AFR2-20261025-01: las reglas anteriores de acceso quedan sustituidas por [control directo EX01v7](DIRECT_CONTROL_7.md). Sin adquisición, renovación, liberación ni habilitación OLED. Los formatos anteriores solo se conservan donde DIRECT_CONTROL_7 mantiene compatibilidad.

> Histórico EX01v3: campos incorporados se conservan donde son compatibles. Estado vigente EX01v6 en [matriz15](../docs/ALINEACION_MAESTRO.md) y [formato6](EX01_SEQUENCE_DISTANCE_6.md); no adoptar los límites de versión/máscara de este documento como perfil actual.

# EX01 v3: integración vigente

AFR2-20261024-02, 24/10/2026. ROBO2/perfil 2 mantiene framing, rutas 0x31/0x13,
sesión/transacción, XOR y payload máximo de 64 bytes. `EX_HELLO` negocia EX01
versión 3; el ESP admite versiones 1..3 y solo anuncia GOTO completo desde v3.
Este documento sustituye los pendientes de integración de las fichas 19/20/21/01;
no acredita ejecución física. Los cuerpos siguientes excluyen los prefijos de
8 bytes de petición y 9 bytes de respuesta.

| Servicio Web | Procesamiento ESP / comando AVR | Resultado |
|---|---|---|
| system.hello / alive | ESP local; heartbeat y negociación ROBO2/EX01 independientes | Estado de enlaces y capacidades reales; una sesión WS |
| control.acquire / renew / release | LEASE 0x12, token de dueño y TTL 1500 ms | Control confirmado por AVR; release apaga salidas |
| robot.stop | STOP 0x06, slot prioritario | completed solo con apagado confirmado; conserva fault |
| robot.state | STATE 0x11 y POSE 0x18 cuando hay pose válida | Modo, lease, fallo, GOTO, EEPROM, pose/marco/edad |
| robot.drive / mode / test.wheel | 0x13 / 0x15 / 0x14 | Límites reales y dueño comprobados por AVR; prueba retorna accepted |
| robot.sensors / encoders | 0x04 / 0x05 | Cinco canales, HC-SR04 y cuatro contadores con validez/edad |
| robot.goto.start / cancel | 0x16 / 0x17 | Objetivo relativo completo; accepted no significa misión terminada |
| robot.config.get / calibrate | CONFIG 0x1A | Consulta o escritura RAM de un ID numérico del catálogo compartido |
| robot.preferences | PREFS 0x1B | save es accepted; estado EEPROM posterior confirma escritura. discard restaura guardado |
| robot.line.calibrate | LINE_CAL 0x1C | Confirmar/invalidar línea; no inventa calibración física |
| robot.screen.get | META 0x20, PAGE 0x21, RELEASE 0x23 | Framebuffer coherente de 1024 bytes; se descarta captura incompleta |
| robot.ui.action | ACTION 0x22 | Revisión, generación, caducidad, permisos y entrada local comprobados |
| robot.sequence.* | 0x40..0x45 | Adaptadores y ejecutor en fuente; ejecución bloqueada por macro de memoria y requisitos físicos |
| wifi.status / scan / configure | Servicio local ESP | Disponible sin Mega; scan/configure son aceptaciones asíncronas |
| wifi.draft.submit / cancel | Borrador local ESP, jobs 0x30/0x31 | Nunca aplica radio hasta Conectar físico desde OLED |

## GOTO y límites

GOTO 0x16 acepta cuerpo de 16 bytes:
`owner:u32, x:i32, y:i32, theta:i16, speed:u16`, todo LE. X/Y ±5000 mm,
theta ±3140 mrad y velocidad 10..límite configurado, mm/s. Se conserva lectura
del cuerpo histórico de 12 bytes para X/Y con velocidad configurada y theta 0.
El cliente v3 siempre envía los 16 bytes. X/Y y theta son relativos a pose de
inicio; orientación normalizada ±pi. Tiempo máximo 60 s, tolerancias 10 mm y
25 mrad, lease/STOP/odometría y condición frontal HC-SR04 supervisan la misión.
Las tolerancias son parámetros de fuente, no precisión medida.

LIMITS 0x1D recibe cuerpo vacío y devuelve 8 bytes:
`speed:u16, pwm:u8, step10:u8, wheelDiameter:u16, lxPlusLy:u16`.
El ESP publica estos límites en `capabilities.*.limits`; la Web restringe sus
controles y rechaza un programa cuya geometría/velocidades no coincidan.
Flags nuevos: GOTO completo 0x200, CONFIG 0x400, prueba PWM 0x800;
máscara total permitida 0xFFF. No se cambia la máscara legacy del handshake base.

CONFIG usa el catálogo `ATMEGA_CONFIG_ACTIONS.json` directamente importado por
la Web. Body: `owner:u32, op:u8, action:u8, value:i32`; op0 lee sin lease,
op1 escribe con control Web y salidas apagadas. Canal IR y rueda seleccionados
son contexto compartido. Guardar/descartar es explícito; no se asimila RAM a EEPROM.
Hall conserva medición manual de una vuelta por rueda desde la UI física.

## Wi-Fi solicitado por OLED

El ESP consulta `WIFI_POLL` 0x30 y devuelve status/redes/resultados mediante
`WIFI_UPDATE` 0x31. Ambos extremos conservan los offsets de EX01 original.
Atiende estado, scan, borrador Web, borrador por teclado local, conectar,
cancelar y reintentar. Publica versiones ESP/Web hacia Sistema del AVR.

`system.status.payload.wifiDraft` incluye `pending,jobId,mode,ssid,expiresInMs`;
nunca incluye contraseña. La Web abre un modal compartido desde cualquier ruta.
Submit: `jobId:u32,mode,ssid,password,persist`; cancel: `jobId:u32`.
El borrador vive en RAM, vence y se borra al cancelar/perder enlace. La Web no
guarda credenciales ni las registra. Solo el clic físico Conectar aplica el
borrador; en ese momento se respeta `persist`. Sin navegador conectado, el pedido
Web termina con error y sigue disponible el teclado local. Configuración directa
desde la página Wi-Fi mantiene el flujo existente y no necesita Mega.

## Navegación, seguridad y despliegue

Largo SW abre principal y descarta borrador también en teclado, conforme a las
instrucciones vigentes. Corto confirma; atrás solo mediante Volver. Renderers no
acceden al hardware. Las acciones remotas no eluden permisos locales ni escriben
credenciales/calibración mediante un clic genérico de encoder.

STOP y cancelación invalidan solicitudes anteriores. Identidades de programa y
ejecución se reservan antes de enviar el inicio para poder cancelarlo mientras
su ACK está pendiente. Al perder dueño/enlace no se reanuda una operación.
Una captura temporalmente ocupada genera BUSY reintentable, sin deshabilitar
permanentemente el visor. Sensores caducan en Web y se limpian al perder enlace.

Compilaciones y promoción de los seis assets Web a ESP/data/dist completadas;
sin tests, renderizado de navegador, imagen FS, flasheo ni ensayo físico. El perfil
ESP sigue siendo candidato: su layout por defecto no admite el bundle completo
de ~2,9 MB. Identificar flash/SPI y acordar layout común antes de crear imágenes.
`AFR2_HALL_CONNECTED=0` y `AFR2_SEQUENCE_MEMORY_APPROVED=0` permanecen intactos.
