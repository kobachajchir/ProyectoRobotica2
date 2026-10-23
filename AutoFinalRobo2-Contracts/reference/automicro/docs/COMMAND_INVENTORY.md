# Inventario de comandos compartidos

Documentación del registro ESP modificado; no es validación de paridad. Los rangos no sustituyen un esquema. Campos exactos A–F: [contrato](PROTOCOL_A_F.md). Otros campos y choques quedan pendientes fuera de A–F.

| ID | Nombre | Fuente ESP | Campos |
|---|---|---|---|
| 0x10 | SET_MODE_AP | f4_command_registry.c:23 | pending-outside-A-F |
| 0x11 | SET_MODE_STA | f4_command_registry.c:24 | pending-outside-A-F |
| 0x12 | SET_CREDENTIALS | f4_command_registry.c:25 | pending-outside-A-F |
| 0x13 | CLEAR_CREDENTIALS | f4_command_registry.c:26 | pending-outside-A-F |
| 0x14 | START_SCAN | f4_command_registry.c:27 | pending-outside-A-F |
| 0x15 | GET_SCAN_RESULTS | f4_command_registry.c:28 | pending-outside-A-F |
| 0x16 | REBOOT_ESP | f4_command_registry.c:29 | pending-outside-A-F |
| 0x17 | FACTORY_RESET | f4_command_registry.c:30 | pending-outside-A-F |
| 0x18 | STOP_SCAN | f4_command_registry.c:31 | pending-outside-A-F |
| 0x19 | RESET_MCU | f4_command_registry.c:32 | documented-A-F |
| 0x1A | WIFI_GET_DETAIL | f4_command_registry.c:33 | pending-outside-A-F |
| 0x30 | GET_STATUS | f4_command_registry.c:37 | pending-outside-A-F |
| 0x31 | PING_ALIVE | f4_command_registry.c:38 | pending-outside-A-F |
| 0x40 | GET_PREFERENCES | f4_command_registry.c:41 | pending-outside-A-F |
| 0x41 | REQUEST_FIRMWARE | f4_command_registry.c:42 | pending-outside-A-F |
| 0x42 | ECHO | f4_command_registry.c:43 | pending-outside-A-F |
| 0x43 | SET_ENCODER_FAST | f4_command_registry.c:44 | pending-outside-A-F |
| 0x44 | GET_CONNECTED_USERS | f4_command_registry.c:45 | pending-outside-A-F |
| 0x45 | GET_USER_INFO | f4_command_registry.c:46 | pending-outside-A-F |
| 0x46 | GET_INTERFACES_CONNECTED | f4_command_registry.c:47 | pending-outside-A-F |
| 0x47 | GET_CREDENTIALS | f4_command_registry.c:48 | pending-outside-A-F |
| 0x48 | CONNECT_WIFI | f4_command_registry.c:49 | pending-outside-A-F |
| 0x49 | DISCONNECT_WIFI | f4_command_registry.c:50 | pending-outside-A-F |
| 0x4A | SET_AP_CONFIG | f4_command_registry.c:51 | pending-outside-A-F |
| 0x4B | START_AP | f4_command_registry.c:52 | pending-outside-A-F |
| 0x4C | STOP_AP | f4_command_registry.c:53 | pending-outside-A-F |
| 0x4D | GET_CONNECTED_USERS_MODE | f4_command_registry.c:54 | pending-outside-A-F |
| 0x4E | SET_AUTO_RECONNECT | f4_command_registry.c:55 | pending-outside-A-F |
| 0x51 | AUTH_VALIDATE_PIN | f4_command_registry.c:57 | pending-outside-A-F |
| 0x52 | GET_CURRENT_SCREEN | f4_command_registry.c:58 | documented-A-F |
| 0x53 | MENU_ITEM_CLICK | f4_command_registry.c:59 | documented-A-F |
| 0x54 | TRIGGER_ENCODER_BUTTON | f4_command_registry.c:60 | documented-A-F |
| 0x55 | TRIGGER_USER_BUTTON | f4_command_registry.c:61 | documented-A-F |
| 0x56 | REQUEST_SCREEN_PAGE | f4_command_registry.c:62 | documented-A-F |
| 0x57 | TRIGGER_ENCODER_ROTATE_LEFT | f4_command_registry.c:63 | documented-A-F |
| 0x58 | TRIGGER_ENCODER_ROTATE_RIGHT | f4_command_registry.c:64 | documented-A-F |
| 0x59 | AUTH_PIN_GRANTED_DEPRECATED | f4_command_registry.c:65 | pending-outside-A-F |
| 0x5A | BINARY_ASSET_STREAM | f4_command_registry.c:66 | pending-outside-A-F |
| 0x5B | GET_CAR_MODE | f4_command_registry.c:67 | pending-outside-A-F |
| 0x5C | WIFI_CREDENTIALS_WEB_RESULT | f4_command_registry.c:68 | pending-outside-A-F |
| 0x5D | AUTH_REMOTE_RESULT | f4_command_registry.c:69 | pending-outside-A-F |
| 0x5E | SET_I2C_POLICY | f4_command_registry.c:70 | pending-outside-A-F |
| 0x5F | GET_I2C_POLICY | f4_command_registry.c:71 | pending-outside-A-F |
| 0x60 | GET_MPU_SNAPSHOT | f4_command_registry.c:72 | pending-outside-A-F |
| 0x61 | SET_MPU_STREAM | f4_command_registry.c:73 | pending-outside-A-F |
| 0x62 | STOP_MPU_STREAM | f4_command_registry.c:74 | pending-outside-A-F |
| 0x63 | SET_PREFERENCES | f4_command_registry.c:75 | pending-outside-A-F |
| 0x64 | GET_TELEMETRY_SNAPSHOT | f4_command_registry.c:76 | pending-outside-A-F |
| 0x65 | GET_BALANCE_PID | f4_command_registry.c:77 | pending-outside-A-F |
| 0x66 | SET_BALANCE_PID | f4_command_registry.c:78 | pending-outside-A-F |
| 0x67 | GET_CONTROL_SNAPSHOT | f4_command_registry.c:79 | pending-outside-A-F |
| 0x68 | GET_FIRMWARE_INFO | f4_command_registry.c:80 | pending-outside-A-F |
| 0x69 | GET_BUILD_INFO | f4_command_registry.c:81 | pending-outside-A-F |
| 0x6A | GET_IR_SNAPSHOT | f4_command_registry.c:82 | pending-outside-A-F |
| 0x6B | SET_IR_STREAM | f4_command_registry.c:83 | pending-outside-A-F |
| 0x6C | STOP_IR_STREAM | f4_command_registry.c:84 | pending-outside-A-F |
| 0x6D | OLED_CANVAS_TRANSFER | f4_command_registry.c:85 | pending-outside-A-F |
| 0x6E | SET_CAR_MODE | f4_command_registry.c:86 | pending-outside-A-F |
| 0x6F | GET_BOOT_REPORT | f4_command_registry.c:87 | pending-outside-A-F |
| 0x70 | WIFI_CREDENTIALS_FLOW | f4_command_registry.c:88 | pending-outside-A-F |
| 0x71 | AP_CLIENTS_PAGE | f4_command_registry.c:89 | pending-outside-A-F |
| 0x72 | AP_CLIENT_DISCONNECT | f4_command_registry.c:90 | pending-outside-A-F |
| 0x73 | GET_EXTENSION_PROFILE | f4_command_registry.c:91 | documented-A-F |
| 0x74 | SET_EXTENSION_PROFILE | f4_command_registry.c:92 | documented-A-F |
| 0x76 | REBOOT_INTO_PROFILE | f4_command_registry.c:93 | documented-A-F |
| 0x75 | GET_EXTENSION_PIN_MAP | f4_command_registry.c:94 | documented-A-F |
| 0x77 | GET_AP_CONFIG | f4_command_registry.c:95 | pending-outside-A-F |
| 0x78 | SET_AP_CONFIG_F4 | f4_command_registry.c:96 | pending-outside-A-F |
| 0x79 | GET_IR_EMITTERS | f4_command_registry.c:97 | documented-A-F |
| 0x7A | SET_IR_EMITTERS | f4_command_registry.c:98 | documented-A-F |
| 0x80 | EVT_BOOT | f4_command_registry.c:101 | pending-outside-A-F |
| 0x81 | EVT_MODE_CHANGED | f4_command_registry.c:102 | pending-outside-A-F |
| 0x82 | EVT_STA_CONNECTED | f4_command_registry.c:103 | pending-outside-A-F |
| 0x83 | EVT_STA_DISCONNECTED | f4_command_registry.c:104 | pending-outside-A-F |
| 0x84 | EVT_AP_CLIENT_JOIN | f4_command_registry.c:105 | pending-outside-A-F |
| 0x85 | EVT_AP_CLIENT_LEAVE | f4_command_registry.c:106 | pending-outside-A-F |
| 0x86 | EVT_APP_USER_CONNECTED | f4_command_registry.c:107 | pending-outside-A-F |
| 0x87 | EVT_APP_USER_DISCONNECTED | f4_command_registry.c:108 | pending-outside-A-F |
| 0x88 | EVT_WEBSERVER_UP | f4_command_registry.c:109 | pending-outside-A-F |
| 0x89 | EVT_WEBSERVER_CLIENT_CONNECTED | f4_command_registry.c:110 | pending-outside-A-F |
| 0x8A | EVT_WEBSERVER_CLIENT_DISCONNECTED | f4_command_registry.c:111 | pending-outside-A-F |
| 0x8B | EVT_LAST_WIFI_NOT_FOUND | f4_command_registry.c:112 | pending-outside-A-F |
| 0x8C | EVT_CONTROLLER_CONNECTED | f4_command_registry.c:113 | pending-outside-A-F |
| 0x8D | EVT_CONTROLLER_DISCONNECTED | f4_command_registry.c:114 | pending-outside-A-F |
| 0x8E | EVT_USB_CONNECTED | f4_command_registry.c:115 | pending-outside-A-F |
| 0x8F | EVT_USB_DISCONNECTED | f4_command_registry.c:116 | pending-outside-A-F |
| 0x90 | EVT_MPU_STREAM | f4_command_registry.c:117 | pending-outside-A-F |
| 0x91 | EVT_IR_STREAM | f4_command_registry.c:118 | pending-outside-A-F |
| 0x92 | EVT_WIFI_CONNECTED | f4_command_registry.c:119 | pending-outside-A-F |
| 0x93 | EVT_NETWORK_IP | f4_command_registry.c:120 | pending-outside-A-F |
| 0x94 | EVT_BOOT_COMPLETE | f4_command_registry.c:121 | pending-outside-A-F |
| 0x95 | EVT_SCREEN_CHANGED | f4_command_registry.c:122 | documented-A-F |
| 0x96 | EVT_MENU_SELECTION_CHANGED | f4_command_registry.c:123 | documented-A-F |
| 0x97 | EVT_CAR_MODE_CHANGED | f4_command_registry.c:124 | pending-outside-A-F |
| 0x98 | EVT_IR_EMITTERS_CHANGED | f4_command_registry.c:125 | documented-A-F |
| 0x9E | EVT_QT_USB_STATE | f4_command_registry.c:126 | pending-outside-A-F |
| 0x9F | EVT_MCU_BOOT_REPORT | f4_command_registry.c:127 | pending-outside-A-F |
| 0xA4 | LEGACY_WIFI_SCAN_STARTED | f4_command_registry.c:129 | pending-outside-A-F |
| 0xA5 | LEGACY_WIFI_SCAN_COMPLETED | f4_command_registry.c:130 | pending-outside-A-F |
| 0xA6 | LEGACY_WIFI_STA_CONNECT_REQUESTED | f4_command_registry.c:131 | pending-outside-A-F |
| 0xA7 | LEGACY_WIFI_STA_DISCONNECT_REQUESTED | f4_command_registry.c:132 | pending-outside-A-F |
| 0xA8 | LEGACY_WIFI_AP_STATE_REQUESTED | f4_command_registry.c:133 | pending-outside-A-F |
| 0xA9 | LEGACY_WEB_COMMAND_REJECTED | f4_command_registry.c:134 | pending-outside-A-F |
| 0xAA | LEGACY_CREDENTIALS_UPDATED | f4_command_registry.c:135 | pending-outside-A-F |
| 0xE0 | ACK | f4_command_registry.c:137 | pending-outside-A-F |
| 0xE1 | NACK | f4_command_registry.c:138 | pending-outside-A-F |


## Asignación en Protocol Studio

La fotografía de comandos que guarda Studio está en [catalog/protocol-studio.json](../catalog/protocol-studio.json), con formularios, formatos, disponibilidad, permisos y finalización. [Inventario y pendientes de Studio](PROTOCOL_STUDIO_COMMANDS.md). Incluye 106 IDs nombrados; 95 operativos observados y 11 para lectura. La herramienta Web `npm run protocol:inventory` informa futuras altas sin asignación y renombrados; ejecutar solamente al solicitar revisión. AMI-20261002-07 registra implementación y compilación.


2026-10-05 — AMI-20261005-01: 0x1C GET_AP_FALLBACK asignado a F4 (0 -> 3 bytes, sesión STM). 0x1B conserva el espejo ESP de la política; 0x63 conserva la escritura persistente F4. GET_STATUS local F4 agrega marca final y pasa de 92 a 93 bytes; no confundir con el bloque variable ESP. Fuentes actualizadas, sin validación runtime.
# Comandos agregados el 2026-10-07

AMI-20261007-01 asigna 0x23..0x2B a consultas/inicio/cancelación GOTO, transferencia IR, ensayo de motores, perfil y apertura de calibración MPU. Requests de longitud fija 0/8/13/88/186; respuestas 128 o 88 bytes, misma ID y sesión STM. [Esquemas y errores exactos](../changes/2026-10-07-01-goto-motor-calibration.md). Actualizados productor F4 y consumidores ESP/Web/Qt en fuente, sin builds ni validaciones.

