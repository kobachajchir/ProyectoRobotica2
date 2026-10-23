# Catálogo documental de registros de comandos

Extraído de `f4:Core/Src/uner_app.c`, `esp:src/f4_command_registry.c`, `esp:src/ws_bridge/dispatch.inc` y `web:src/protocol/wsApi.ts`. Metadatos de fuente, sin comprobación automática de paridad ni ejecución.

La tabla conserva por separado F4 y ESP. `impl=true` es una declaración del registro ESP; no certifica handler activo. Las filas no implican compatibilidad. La respuesta efectiva se fija leyendo el handler.

| ID | Nombre ESP / símbolo F4 | F4 request min..max; flags; timeout | ESP request / response bytes; política; timeout | Dueño / impl / privilegio ESP |
| --- | --- | --- | --- | --- |
| <a id="command-0x10"></a>`0x10` | SET_MODE_AP / UNER_CMD_ID_SET_MODE_AP | 0u..0u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 300u | 0u..0u / 1u..255u; F4_RESPONSE_SAME_COMMAND; 300u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x11"></a>`0x11` | SET_MODE_STA / UNER_CMD_ID_SET_MODE_STA | 0u..0u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 300u | 0u..0u / 1u..255u; F4_RESPONSE_SAME_COMMAND; 300u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x12"></a>`0x12` | SET_CREDENTIALS / UNER_CMD_ID_SET_CREDENTIALS | 0u..255u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 300u | 0u..255u / 1u..255u; F4_RESPONSE_SAME_COMMAND; 300u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x13"></a>`0x13` | CLEAR_CREDENTIALS / UNER_CMD_ID_CLEAR_CREDENTIALS | 0u..0u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 0u..0u / 1u..1u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 100u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x14"></a>`0x14` | START_SCAN / UNER_CMD_ID_START_SCAN | 0u..0u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 200u | 0u..0u / 2u..255u; F4_RESPONSE_SAME_COMMAND; 200u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x15"></a>`0x15` | GET_SCAN_RESULTS / UNER_CMD_ID_GET_SCAN_RESULTS | 0u..255u; UNER_SPEC_F_RESP; 100u | 0u..255u / 2u..255u; F4_RESPONSE_SAME_COMMAND; 100u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x16"></a>`0x16` | REBOOT_ESP / UNER_CMD_ID_REBOOT_ESP | 1u..1u; UNER_SPEC_F_ACK; 50u | 1u..1u / 0u..0u; F4_RESPONSE_ACK; 50u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x17"></a>`0x17` | FACTORY_RESET / UNER_CMD_ID_FACTORY_RESET | 0u..0u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 200u | 0u..0u / 1u..1u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 200u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x18"></a>`0x18` | STOP_SCAN / UNER_CMD_ID_STOP_SCAN | 0u..0u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 0u..0u / 1u..2u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 100u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x19"></a>`0x19` | RESET_MCU / UNER_CMD_ID_RESET_MCU | 0u..0u; UNER_SPEC_F_ACK; 100u | 0u..0u / 0u..0u; F4_RESPONSE_ACK; 250u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x1a"></a>`0x1A` | WIFI_GET_DETAIL / UNER_CMD_ID_WIFI_GET_DETAIL | 1u..(WIFI_SSID_MAX_LEN + 4u); UNER_SPEC_F_RESP; 150u | 1u..36u / 4u..36u; F4_RESPONSE_SAME_COMMAND; 150u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x1b"></a>`0x1B` | SET_AP_FALLBACK / UNER_CMD_ID_SET_AP_FALLBACK | 1u..2u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 1000u | 1u..1u / 2u..2u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 1000u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x1c"></a>`0x1C` | GET_AP_FALLBACK / UNER_CMD_ID_GET_AP_FALLBACK | 0u..0u; UNER_SPEC_F_RESP; 100u | 0u..0u / 3u..3u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x1d"></a>`0x1D` | LINE_CALIBRATION_CONTROL / UNER_CMD_ID_LINE_CALIBRATION_CONTROL | 5u..5u; UNER_SPEC_F_RESP; 100u | 5u..5u / 168u..168u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x1e"></a>`0x1E` | GET_LINE_STATE / UNER_CMD_ID_GET_LINE_STATE | 0u..0u; UNER_SPEC_F_RESP; 100u | 0u..0u / 168u..168u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x1f"></a>`0x1F` | SET_LINE_FOLLOW / UNER_CMD_ID_SET_LINE_FOLLOW | 1u..1u; UNER_SPEC_F_RESP; 100u | 1u..1u / 168u..168u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x23"></a>`0x23` | stm.goto.get / 0x23u | 0u..0u; UNER_SPEC_F_RESP; 500u | 0u..0u / 128u..128u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x24"></a>`0x24` | stm.goto.start / 0x24u | 8u..8u; UNER_SPEC_F_RESP; 500u | 8u..8u / 128u..128u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x25"></a>`0x25` | stm.goto.cancel / 0x25u | 0u..0u; UNER_SPEC_F_RESP; 500u | 0u..0u / 128u..128u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x26"></a>`0x26` | stm.goto.irCalibration.set / 0x26u | 186u..186u; UNER_SPEC_F_RESP; 500u | 186u..186u / 128u..128u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x27"></a>`0x27` | stm.motorCalibration.get / 0x27u | 0u..0u; UNER_SPEC_F_RESP; 500u | 0u..0u / 128u..128u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x28"></a>`0x28` | stm.motorCalibration.control / 0x28u | 13u..13u; UNER_SPEC_F_RESP; 500u | 13u..13u / 128u..128u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x29"></a>`0x29` | stm.motorCalibration.profile.get / 0x29u | 0u..0u; UNER_SPEC_F_RESP; 500u | 0u..0u / 88u..88u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x2a"></a>`0x2A` | stm.motorCalibration.profile.set / 0x2au | 88u..88u; UNER_SPEC_F_RESP; 500u | 88u..88u / 88u..88u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x2b"></a>`0x2B` | MPU_CALIBRATION_REQUEST / 0x2Bu | 0u..0u; UNER_SPEC_F_RESP; 500u | 0u..0u / 128u..128u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x30"></a>`0x30` | GET_STATUS / UNER_CMD_ID_GET_STATUS | 0u..255u; UNER_SPEC_F_RESP; 100u | 0u..255u / 18u..255u; F4_RESPONSE_SAME_COMMAND; 100u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x31"></a>`0x31` | PING_ALIVE / UNER_CMD_ID_PING | 0u..16u; UNER_SPEC_F_RESP; 0u | 0u..16u / 0u..16u; F4_RESPONSE_SAME_COMMAND; 150u | F4_COMMAND_OWNER_DUAL; true; F4_PRIVILEGE_PUBLIC |
| <a id="command-0x40"></a>`0x40` | GET_PREFERENCES / UNER_CMD_ID_GET_PREFERENCES | 0u..1u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 0u..1u / 1u..255u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 100u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x41"></a>`0x41` | REQUEST_FIRMWARE / UNER_CMD_ID_REQUEST_FIRMWARE | 0u..64u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 200u | 0u..64u / 1u..33u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 200u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x42"></a>`0x42` | ECHO / UNER_CMD_ID_ECHO | 0u..255u; 0u; 0u | 0u..255u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x43"></a>`0x43` | SET_ENCODER_FAST | sin entrada extraída | 1u..1u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_F4; false; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x44"></a>`0x44` | GET_CONNECTED_USERS | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_ESP; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x45"></a>`0x45` | GET_USER_INFO | sin entrada extraída | 1u..1u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_ESP; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x46"></a>`0x46` | GET_INTERFACES_CONNECTED | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_ESP; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x47"></a>`0x47` | GET_CREDENTIALS | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_ESP; false; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x48"></a>`0x48` | CONNECT_WIFI / UNER_CMD_ID_CONNECT_WIFI | 0u..255u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 300u | 0u..255u / 5u..5u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 300u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x49"></a>`0x49` | DISCONNECT_WIFI / UNER_CMD_ID_DISCONNECT_WIFI | 0u..255u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 0u..255u / 1u..1u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 100u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x4a"></a>`0x4A` | SET_AP_CONFIG / UNER_CMD_ID_SET_AP_CONFIG | 1u..255u; UNER_SPEC_F_RESP; 300u | 8u..102u / 1u..1u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 200u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x4b"></a>`0x4B` | START_AP / UNER_CMD_ID_START_AP | 0u..255u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 300u | 0u..255u / 5u..5u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 300u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x4c"></a>`0x4C` | STOP_AP / UNER_CMD_ID_STOP_AP | 0u..255u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 0u..255u / 1u..1u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 100u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x4d"></a>`0x4D` | GET_CONNECTED_USERS_MODE | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_ESP; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x4e"></a>`0x4E` | SET_AUTO_RECONNECT / UNER_CMD_ID_SET_AUTO_RECONNECT | 1u..1u; UNER_SPEC_F_RESP; 100u | 1u..1u / 2u..2u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 100u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x51"></a>`0x51` | AUTH_VALIDATE_PIN / UNER_CMD_ID_AUTH_VALIDATE_PIN | 6u..10u; UNER_SPEC_F_RESP; 500u | 6u..10u / 9u..9u; F4_RESPONSE_SAME_COMMAND; 1500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_PUBLIC |
| <a id="command-0x52"></a>`0x52` | GET_CURRENT_SCREEN / UNER_CMD_ID_GET_CURRENT_SCREEN | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 5u..45u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x53"></a>`0x53` | MENU_ITEM_CLICK / UNER_CMD_ID_MENU_ITEM_CLICK | 4u..5u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 4u..5u / 0u..0u; F4_RESPONSE_ACK; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x54"></a>`0x54` | TRIGGER_ENCODER_BUTTON / UNER_CMD_ID_TRIGGER_ENCODER_BUTTON | 4u..5u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 4u..5u / 0u..0u; F4_RESPONSE_ACK; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x55"></a>`0x55` | TRIGGER_USER_BUTTON / UNER_CMD_ID_TRIGGER_USER_BUTTON | 4u..5u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 4u..5u / 0u..0u; F4_RESPONSE_ACK; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x56"></a>`0x56` | REQUEST_SCREEN_PAGE / UNER_CMD_ID_REQUEST_SCREEN_PAGE | 4u..5u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 4u..5u / 0u..0u; F4_RESPONSE_ACK; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x57"></a>`0x57` | TRIGGER_ENCODER_ROTATE_LEFT / UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_LEFT | 4u..4u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 4u..4u / 0u..0u; F4_RESPONSE_ACK; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x58"></a>`0x58` | TRIGGER_ENCODER_ROTATE_RIGHT / UNER_CMD_ID_TRIGGER_ENCODER_ROTATE_RIGHT | 4u..4u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 100u | 4u..4u / 0u..0u; F4_RESPONSE_ACK; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x59"></a>`0x59` | AUTH_PIN_GRANTED_DEPRECATED / UNER_CMD_ID_AUTH_PIN_GRANTED | 4u..4u; UNER_SPEC_F_RESP; 50u | 4u..4u / 0u..0u; F4_RESPONSE_NACK_ONLY; 50u | F4_COMMAND_OWNER_F4; false; F4_PRIVILEGE_PUBLIC |
| <a id="command-0x5a"></a>`0x5A` | BINARY_ASSET_STREAM / UNER_CMD_ID_BINARY_ASSET_STREAM | 1u..255u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 1000u | 1u..255u / 9u..255u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 1000u | F4_COMMAND_OWNER_ESP; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x5b"></a>`0x5B` | GET_CAR_MODE / UNER_CMD_ID_GET_CAR_MODE | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 1u..1u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x5c"></a>`0x5C` | WIFI_CREDENTIALS_WEB_RESULT / UNER_CMD_ID_WIFI_CREDENTIALS_WEB_RESULT | 2u..96u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 200u | 2u..96u / 0u..0u; F4_RESPONSE_ACK; 200u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x5d"></a>`0x5D` | AUTH_REMOTE_RESULT / UNER_CMD_ID_AUTH_REMOTE_RESULT | 1u..1u; UNER_SPEC_F_RESP; 50u | 1u..1u / 6u..6u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_PUBLIC |
| <a id="command-0x5e"></a>`0x5E` | SET_I2C_POLICY / UNER_CMD_ID_SET_I2C_POLICY | 1u..1u; UNER_SPEC_F_RESP; 50u | 1u..1u / 2u..2u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x5f"></a>`0x5F` | GET_I2C_POLICY / UNER_CMD_ID_GET_I2C_POLICY | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 2u..2u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x60"></a>`0x60` | GET_MPU_SNAPSHOT / UNER_CMD_ID_GET_MPU_SNAPSHOT | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 42u..42u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x61"></a>`0x61` | SET_MPU_STREAM / UNER_CMD_ID_SET_MPU_STREAM | 1u..3u; UNER_SPEC_F_RESP; 50u | 1u..3u / 4u..4u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x62"></a>`0x62` | STOP_MPU_STREAM / UNER_CMD_ID_STOP_MPU_STREAM | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 1u..1u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x63"></a>`0x63` | SET_PREFERENCES / UNER_CMD_ID_SET_PREFERENCES | 4u..64u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 250u | 4u..64u / 5u..5u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 250u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x64"></a>`0x64` | GET_TELEMETRY_SNAPSHOT / UNER_CMD_ID_GET_TELEMETRY_SNAPSHOT | 0u..0u; UNER_SPEC_F_RESP; 80u | 0u..0u / 92u..92u; F4_RESPONSE_SAME_COMMAND; 80u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x65"></a>`0x65` | GET_BALANCE_PID / UNER_CMD_ID_GET_BALANCE_PID | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 13u..13u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x66"></a>`0x66` | SET_BALANCE_PID / UNER_CMD_ID_SET_BALANCE_PID | 5u..13u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 80u | 5u..13u / 14u..14u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 80u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x67"></a>`0x67` | GET_CONTROL_SNAPSHOT / UNER_CMD_ID_GET_CONTROL_SNAPSHOT | 0u..0u; UNER_SPEC_F_RESP; 80u | 0u..0u / 64u..64u; F4_RESPONSE_SAME_COMMAND; 80u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x68"></a>`0x68` | GET_FIRMWARE_INFO / UNER_CMD_ID_GET_FIRMWARE_INFO | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 1u..196u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_PUBLIC |
| <a id="command-0x69"></a>`0x69` | GET_BUILD_INFO / UNER_CMD_ID_GET_BUILD_INFO | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 1u..149u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_PUBLIC |
| <a id="command-0x6a"></a>`0x6A` | GET_IR_SNAPSHOT / UNER_CMD_ID_GET_IR_SNAPSHOT | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 56u..56u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x6b"></a>`0x6B` | SET_IR_STREAM / UNER_CMD_ID_SET_IR_STREAM | 1u..3u; UNER_SPEC_F_RESP; 50u | 1u..3u / 4u..4u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x6c"></a>`0x6C` | STOP_IR_STREAM / UNER_CMD_ID_STOP_IR_STREAM | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 1u..1u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x6d"></a>`0x6D` | OLED_CANVAS_TRANSFER / UNER_CMD_ID_OLED_CANVAS_TRANSFER | 3u..118u; UNER_SPEC_F_RESP; 5000u | 3u..118u / 10u..10u; F4_RESPONSE_SAME_COMMAND; 1000u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x6e"></a>`0x6E` | SET_CAR_MODE / WIFI_BRIDGE_CMD_ID_SET_CAR_MODE | 1u..1u; UNER_SPEC_F_RESP; 100u | 1u..1u / 2u..2u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x6f"></a>`0x6F` | GET_BOOT_REPORT / UNER_CMD_ID_GET_BOOT_REPORT | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 22u..22u; F4_RESPONSE_SAME_COMMAND; 50u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_PUBLIC |
| <a id="command-0x70"></a>`0x70` | WIFI_CREDENTIALS_FLOW / UNER_CMD_ID_WIFI_CREDENTIALS_FLOW | 1u..255u; UNER_SPEC_F_RESP; 500u | 1u..255u / 3u..255u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x71"></a>`0x71` | AP_CLIENTS_PAGE / UNER_CMD_ID_AP_CLIENTS_PAGE | 2u..2u; UNER_SPEC_F_RESP; 250u | 2u..2u / 3u..255u; F4_RESPONSE_SAME_COMMAND; 250u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x72"></a>`0x72` | AP_CLIENT_DISCONNECT / UNER_CMD_ID_AP_CLIENT_DISCONNECT | 6u..6u; UNER_SPEC_F_RESP; 250u | 6u..6u / 2u..2u; F4_RESPONSE_SAME_COMMAND; 250u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_ESP_ADMIN |
| <a id="command-0x73"></a>`0x73` | GET_EXTENSION_PROFILE / UNER_CMD_ID_GET_EXTENSION_PROFILE | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 2u..2u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x74"></a>`0x74` | SET_EXTENSION_PROFILE / UNER_CMD_ID_SET_EXTENSION_PROFILE | 1u..1u; UNER_SPEC_F_ACK; 50u | 1u..1u / 1u..1u; F4_RESPONSE_SAME_COMMAND; 3500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x75"></a>`0x75` | GET_EXTENSION_PIN_MAP | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_F4; false; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x76"></a>`0x76` | REBOOT_INTO_PROFILE / UNER_CMD_ID_REBOOT_INTO_PROFILE | 1u..1u; UNER_SPEC_F_ACK; 50u | 1u..1u / 0u..0u; F4_RESPONSE_ACK; 3500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x77"></a>`0x77` | GET_AP_CONFIG / UNER_CMD_ID_GET_AP_CONFIG | 0u..0u; UNER_SPEC_F_RESP; 100u | 0u..0u / 10u..41u; F4_RESPONSE_SAME_COMMAND; 200u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x78"></a>`0x78` | SET_AP_CONFIG_F4 / UNER_CMD_ID_SET_AP_CONFIG_DESIRED | 1u..255u; UNER_SPEC_F_ACK / UNER_SPEC_F_RESP; 250u | 1u..103u / 3u..3u; F4_RESPONSE_ACK_THEN_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x79"></a>`0x79` | GET_IR_EMITTERS / UNER_CMD_ID_GET_IR_EMITTERS | 0u..0u; UNER_SPEC_F_RESP; 50u | 0u..0u / 3u..3u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x7a"></a>`0x7A` | SET_IR_EMITTERS / UNER_CMD_ID_SET_IR_EMITTERS | 3u..3u; UNER_SPEC_F_RESP; 100u | 3u..3u / 4u..4u; F4_RESPONSE_SAME_COMMAND; 500u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x7b"></a>`0x7B` | GET_MOTION_STATE / UNER_CMD_ID_GET_MOTION_STATE | 0u..0u; UNER_SPEC_F_RESP; 100u | 0u..0u / 8u..8u; F4_RESPONSE_SAME_COMMAND; 100u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x7c"></a>`0x7C` | SET_BALANCE_ENABLED / UNER_CMD_ID_SET_BALANCE_ENABLED | 1u..1u; UNER_SPEC_F_RESP; 100u | 1u..1u / 8u..8u; F4_RESPONSE_SAME_COMMAND; 100u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x7d"></a>`0x7D` | SET_MOTION / UNER_CMD_ID_SET_MOTION | 10u..10u; UNER_SPEC_F_RESP; 100u | 10u..10u / 8u..8u; F4_RESPONSE_SAME_COMMAND; 100u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x7e"></a>`0x7E` | STOP_MOTION / UNER_CMD_ID_STOP_MOTION | 1u..1u; UNER_SPEC_F_RESP; 100u | 1u..1u / 8u..8u; F4_RESPONSE_SAME_COMMAND; 100u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x7f"></a>`0x7F` | SET_MOTOR_TEST / UNER_CMD_ID_SET_MOTOR_TEST | 10u..10u; UNER_SPEC_F_RESP; 100u | 10u..10u / 8u..8u; F4_RESPONSE_SAME_COMMAND; 100u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x80"></a>`0x80` | EVT_BOOT | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x81"></a>`0x81` | EVT_MODE_CHANGED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x82"></a>`0x82` | EVT_STA_CONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x83"></a>`0x83` | EVT_STA_DISCONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x84"></a>`0x84` | EVT_AP_CLIENT_JOIN | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x85"></a>`0x85` | EVT_AP_CLIENT_LEAVE | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x86"></a>`0x86` | EVT_APP_USER_CONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x87"></a>`0x87` | EVT_APP_USER_DISCONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x88"></a>`0x88` | EVT_WEBSERVER_UP | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x89"></a>`0x89` | EVT_WEBSERVER_CLIENT_CONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x8a"></a>`0x8A` | EVT_WEBSERVER_CLIENT_DISCONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x8b"></a>`0x8B` | EVT_LAST_WIFI_NOT_FOUND | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x8c"></a>`0x8C` | EVT_CONTROLLER_CONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x8d"></a>`0x8D` | EVT_CONTROLLER_DISCONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x8e"></a>`0x8E` | EVT_USB_CONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x8f"></a>`0x8F` | EVT_USB_DISCONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x90"></a>`0x90` | EVT_MPU_STREAM / UNER_EVT_ID_MPU_STREAM | MPU_SNAPSHOT_PAYLOAD_SIZE..MPU_SNAPSHOT_PAYLOAD_SIZE; UNER_SPEC_F_EVT; 0u | 0u..0u / 42u..42u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x91"></a>`0x91` | EVT_IR_STREAM / UNER_EVT_ID_IR_STREAM | IR_SNAPSHOT_PAYLOAD_SIZE..IR_SNAPSHOT_PAYLOAD_SIZE; UNER_SPEC_F_EVT; 0u | 0u..0u / 56u..56u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x92"></a>`0x92` | EVT_WIFI_CONNECTED | sin entrada extraída | 0u..255u / 0u..255u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x93"></a>`0x93` | EVT_NETWORK_IP | sin entrada extraída | 5u..5u / 5u..5u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x94"></a>`0x94` | EVT_BOOT_COMPLETE | sin entrada extraída | 5u..5u / 5u..5u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_ESP; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0x95"></a>`0x95` | EVT_SCREEN_CHANGED | sin entrada extraída | 0u..0u / 5u..9u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x96"></a>`0x96` | EVT_MENU_SELECTION_CHANGED | sin entrada extraída | 0u..0u / 7u..7u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x97"></a>`0x97` | EVT_CAR_MODE_CHANGED | sin entrada extraída | 0u..0u / 1u..1u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x98"></a>`0x98` | EVT_IR_EMITTERS_CHANGED / UNER_EVT_ID_IR_EMITTERS_CHANGED | 3u..3u; UNER_SPEC_F_EVT; 0u | 0u..0u / 3u..3u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x9e"></a>`0x9E` | EVT_QT_USB_STATE | sin entrada extraída | 0u..0u / 1u..1u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0x9f"></a>`0x9F` | EVT_MCU_BOOT_REPORT | sin entrada extraída | 0u..0u / 22u..22u; F4_RESPONSE_EVENT; 0u | F4_COMMAND_OWNER_F4; true; F4_PRIVILEGE_STM_SESSION |
| <a id="command-0xa4"></a>`0xA4` | LEGACY_WIFI_SCAN_STARTED | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_RESERVED; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0xa5"></a>`0xA5` | LEGACY_WIFI_SCAN_COMPLETED | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_RESERVED; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0xa6"></a>`0xA6` | LEGACY_WIFI_STA_CONNECT_REQUESTED | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_RESERVED; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0xa7"></a>`0xA7` | LEGACY_WIFI_STA_DISCONNECT_REQUESTED | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_RESERVED; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0xa8"></a>`0xA8` | LEGACY_WIFI_AP_STATE_REQUESTED | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_RESERVED; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0xa9"></a>`0xA9` | LEGACY_WEB_COMMAND_REJECTED | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_RESERVED; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0xaa"></a>`0xAA` | LEGACY_CREDENTIALS_UPDATED | sin entrada extraída | 0u..0u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_RESERVED; false; F4_PRIVILEGE_INTERNAL |
| <a id="command-0xe0"></a>`0xE0` | ACK | sin entrada extraída | 1u..2u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_DUAL; true; F4_PRIVILEGE_INTERNAL |
| <a id="command-0xe1"></a>`0xE1` | NACK | sin entrada extraída | 1u..2u / 0u..0u; F4_RESPONSE_NONE; 0u | F4_COMMAND_OWNER_DUAL; true; F4_PRIVILEGE_INTERNAL |

La fila 0x74 tiene una contradicción observada entre metadata F4 y handler: [AMDOC-F4-006](../../review/FINDINGS.md#amdoc-f4-006). Los timeouts aquí son de registro; no son el timeout global de la Promise Web.

## Rutas JSON ESP y declaraciones Web

Una ruta en la tabla tiene validador y handler registrados. `Web declarada` indica una constante encontrada, no que todas las pantallas la invoquen. El ID UNER se construye dentro del handler indicado.

| Comando JSON | Destino / ruta | Validador → handler | Esquema declarado | Web declarada |
| --- | --- | --- | --- | --- |
| `esp.web.page.ready` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_web_page_ready` | `local.web.page.ready` | sí |
| `stm.mpu.calibration.request` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.goto.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.goto.start` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_auto_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.goto.cancel` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.goto.irCalibration.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_auto_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.motorCalibration.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.motorCalibration.control` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_auto_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.motorCalibration.profile.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.motorCalibration.profile.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_auto_params` → `cmd_stm_auto` | `uner.auto.v1` | sí |
| `stm.ir.lineCalibration.control` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_line_calibration_params` → `cmd_stm_line` | `uner.line.state.v1` | sí |
| `stm.ir.lineCalibration.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_line` | `uner.line.state.v1` | sí |
| `stm.lineFollow.setEnabled` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_line_follow_params` → `cmd_stm_line` | `uner.line.state.v1` | sí |
| `stm.motion.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_motion` | `uner.motion.state.v1` | sí |
| `stm.balance.setEnabled` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_motion_switch_params` → `cmd_stm_motion` | `uner.motion.state.v1` | sí |
| `stm.motion.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_motion_percent_params` → `cmd_stm_motion` | `uner.motion.state.v1` | sí |
| `stm.motion.stop` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_motion_switch_params` → `cmd_stm_motion` | `uner.motion.state.v1` | sí |
| `stm.motor.test` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_motion_percent_params` → `cmd_stm_motion` | `uner.motion.state.v1` | sí |
| `esp.wifi.getMode` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_optional_params` → `cmd_get_mode` | `local.wifi.mode` | no encontrada |
| `esp.device.getStatus` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_optional_params` → `cmd_get_status` | `local.device.status` | sí |
| `rebootEsp` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_reboot_esp_params` → `cmd_reboot_esp` | `local.device.reboot` | no encontrada |
| `esp.reboot` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_reboot_esp_params` → `cmd_reboot_esp` | `local.device.reboot` | sí |
| `esp.device.reboot` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_reboot_esp_params` → `cmd_reboot_esp` | `local.device.reboot` | no encontrada |
| `esp.wifi.debug.ap.set` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_stm_ap_config_set_params` → `cmd_debug_ap_set` | `local.debug.ap` | sí |
| `esp.wifi.debug.ap.get` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_debug_ap_get` | `local.debug.ap` | sí |
| `esp.wifi.debug.mode.set` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_reboot_esp_params` → `cmd_debug_mode_set` | `local.debug.mode` | sí |
| `esp.wifi.debug.fallback.get` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_debug_fallback` | `local.debug.fallback` | sí |
| `esp.wifi.debug.fallback.set` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_ap_fallback_params` → `cmd_debug_fallback` | `local.debug.fallback` | sí |
| `esp.auth.debug.login` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_pin_params` → `cmd_debug_wifi_login` | `local.auth.debug.wifi` | sí |
| `esp.auth.debug.logout` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_debug_wifi_logout` | `local.auth.debug.wifi` | sí |
| `esp.auth.pin.login` | esp / BRIDGE_ROUTE_FORWARD_STM | `validate_pin_params` → `cmd_auth_pin_login` | `uner.auth.remote` | sí |
| `esp.auth.pin.validateScreen` | esp / BRIDGE_ROUTE_FORWARD_STM | `validate_auth_screen_params` → `cmd_auth_pin_validate_screen` | `uner.auth.remote` | sí |
| `esp.auth.pin.change` | esp / BRIDGE_ROUTE_FORWARD_STM | `validate_auth_change_params` → `cmd_auth_pin_change` | `uner.auth.remote` | sí |
| `esp.auth.session.logout` | esp / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_auth_session_logout` | `uner.auth.session` | sí |
| `esp.auth.session.get` | esp / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_auth_session_get` | `uner.auth.session` | sí |
| `oledCanvas.begin` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_oled_canvas_begin_params` → `cmd_oled_canvas_begin` | `local.oled.canvas.begin` | sí |
| `oledCanvas.chunk` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_oled_canvas_chunk_params` → `cmd_oled_canvas_chunk` | `local.oled.canvas.chunk` | sí |
| `oledCanvas.commit` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_oled_canvas_transfer_params` → `cmd_oled_canvas_commit` | `uner.oled.canvas.rendered` | sí |
| `oledCanvas.cancel` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_oled_canvas_transfer_params` → `cmd_oled_canvas_cancel` | `uner.oled.canvas.cancel` | sí |
| `wifi.scan.start` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_optional_params` → `cmd_get_scan` | `local.wifi.scan` | no encontrada |
| `wifi.scan.stop` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_stop_scan` | `local.wifi.scan.stop` | no encontrada |
| `esp.wifi.getScan` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_optional_params` → `cmd_get_scan` | `local.wifi.scan` | no encontrada |
| `wifi.detail.get` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_wifi_credentials_cancel_params` → `cmd_get_wifi_detail` | `local.wifi.detail` | no encontrada |
| `esp.wifi.getDetail` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_wifi_credentials_cancel_params` → `cmd_get_wifi_detail` | `local.wifi.detail` | no encontrada |
| `wifi.ap.credentials.set` | esp / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_ap_config_set_params` → `cmd_stm_ap_config_set_alias` | `uner.ap.config` | no encontrada |
| `esp.wifi.setAp` | esp / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_ap_config_set_params` → `cmd_stm_ap_config_set_alias` | `uner.ap.config` | no encontrada |
| `wifi.ap.credentials.result.get` | esp / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_ap_config_get_alias` | `uner.ap.config` | no encontrada |
| `wifi.ap.clients.list` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_ap_clients_list` | `local.wifi.ap.clients` | sí |
| `wifi.ap.clients.disconnect` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_ap_client_disconnect_params` → `cmd_ap_client_disconnect` | `local.wifi.ap.client.disconnect` | sí |
| `esp.wifi.connect` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_optional_params` → `cmd_connect_wifi` | `local.wifi.connect` | no encontrada |
| `wifi.credentials.getCurrent` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_wifi_credentials_get_current` | `local.wifi.credentials.current` | no encontrada |
| `wifi.credentials.current` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_wifi_credentials_get_current` | `local.wifi.credentials.current` | no encontrada |
| `esp.wifi.credentials.getCurrent` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_wifi_credentials_get_current` | `local.wifi.credentials.current` | no encontrada |
| `esp.wifi.credentials.current` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_empty_params` → `cmd_wifi_credentials_get_current` | `local.wifi.credentials.current` | no encontrada |
| `wifi.credentials.submit` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_wifi_credentials_submit_params` → `cmd_wifi_credentials_submit` | `local.wifi.credentials.submit` | no encontrada |
| `esp.wifi.credentials.submit` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_wifi_credentials_submit_params` → `cmd_wifi_credentials_submit` | `local.wifi.credentials.submit` | no encontrada |
| `wifi.credentials.cancel` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_wifi_credentials_cancel_params` → `cmd_wifi_credentials_cancel` | `local.wifi.credentials.cancel` | no encontrada |
| `esp.wifi.credentials.cancel` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_wifi_credentials_cancel_params` → `cmd_wifi_credentials_cancel` | `local.wifi.credentials.cancel` | no encontrada |
| `esp.wifi.disconnect` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_optional_params` → `cmd_disconnect_wifi` | `local.wifi.disconnect` | no encontrada |
| `esp.ap.start` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_optional_params` → `cmd_start_ap` | `local.ap.start` | no encontrada |
| `esp.ap.stop` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_optional_params` → `cmd_stop_ap` | `local.ap.stop` | no encontrada |
| `esp.wifi.setAutoReconnect` | esp / BRIDGE_ROUTE_LOCAL_ESP | `validate_required_params` → `cmd_auto_reconnect` | `local.wifi.autoreconnect` | no encontrada |
| `stm.telemetry.setRate` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_required_params` → `cmd_telemetry_set_rate` | `uner.telemetry.ack` | no encontrada |
| `stm.sensors.telemetry.setRate` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_required_params` → `cmd_telemetry_set_rate` | `uner.telemetry.ack` | no encontrada |
| `stm.screen.getCurrent` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_get_current_screen` | `uner.screen.report` | no encontrada |
| `stm.getCurrentScreen` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_get_current_screen` | `uner.screen.report` | no encontrada |
| `rebootStm` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_reboot_stm` | `uner.ack.reset_mcu` | sí |
| `resetMcu` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_reboot_stm` | `uner.ack.reset_mcu` | no encontrada |
| `stm.menu.click` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_menu_click_params` → `cmd_stm_menu_click` | `uner.command.status` | no encontrada |
| `stm.screen.page` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_screen_page_params` → `cmd_stm_screen_page` | `uner.command.status` | no encontrada |
| `stm.input.encoderButton` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_press_params` → `cmd_stm_encoder_button` | `uner.command.status` | no encontrada |
| `stm.input.userButton` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_press_params` → `cmd_stm_user_button` | `uner.command.status` | no encontrada |
| `stm.input.rotateLeft` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_screen_code_params` → `cmd_stm_rotate_left` | `uner.command.status` | no encontrada |
| `stm.input.rotateRight` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_screen_code_params` → `cmd_stm_rotate_right` | `uner.command.status` | no encontrada |
| `stm.wifi.apFallback.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_get_ap_fallback` | `uner.ap.fallback` | sí |
| `stm.wifi.apFallback.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_ap_fallback_params` → `cmd_stm_set_ap_fallback` | `uner.ap.fallback` | sí |
| `stm.irEmitters.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_get_ir_emitters` | `uner.ir.emitters` | sí |
| `stm.irEmitters.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_ir_emitters_params` → `cmd_stm_set_ir_emitters` | `uner.ir.emitters` | sí |
| `stm.extensionProfile.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_get_extension_profile` | `uner.extension.profile` | sí |
| `stm.extensionProfile.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_extension_profile_params` → `cmd_stm_set_extension_profile` | `uner.extension.profile` | sí |
| `stm.extensionProfile.reboot` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_extension_profile_params` → `cmd_stm_reboot_into_profile` | `uner.extension.profile` | sí |
| `stm.apConfig.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_ap_config_get` | `uner.ap.config` | no encontrada |
| `stm.apConfig.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_ap_config_set_params` → `cmd_stm_ap_config_set` | `uner.ap.config` | no encontrada |
| `stm.carMode.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_car_mode` | `uner.command.payload` | no encontrada |
| `stm.carMode.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_car_mode_params` → `cmd_stm_set_car_mode` | `uner.command.payload` | no encontrada |
| `stm.i2c.policy.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_i2c_policy_params` → `cmd_stm_set_i2c_policy` | `uner.command.payload` | no encontrada |
| `stm.i2c.policy.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_i2c_policy` | `uner.command.payload` | no encontrada |
| `stm.mpu.snapshot.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_mpu_snapshot` | `uner.mpu.snapshot` | no encontrada |
| `stm.mpu.stream.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_mpu_stream_params` → `cmd_stm_set_mpu_stream` | `uner.mpu.stream` | no encontrada |
| `stm.mpu.stream.stop` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_stop_mpu_stream` | `uner.mpu.stream` | no encontrada |
| `stm.preferences.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_preferences` | `uner.preferences` | no encontrada |
| `stm.preferences.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_preferences_params` → `cmd_stm_set_preferences` | `uner.preferences` | no encontrada |
| `stm.telemetry.snapshot.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_telemetry_snapshot` | `uner.telemetry.snapshot` | no encontrada |
| `stm.balance.pid.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_balance_pid` | `uner.balance.pid` | no encontrada |
| `stm.balance.pid.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_pid_params` → `cmd_stm_set_balance_pid` | `uner.balance.pid` | no encontrada |
| `stm.control.snapshot.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_control_snapshot` | `uner.control.snapshot` | no encontrada |
| `stm.firmware.info.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_firmware_info` | `uner.firmware.info` | no encontrada |
| `stm.build.info.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_build_info` | `uner.build.info` | no encontrada |
| `stm.boot.report.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_boot_report` | `uner.boot.report` | no encontrada |
| `stm.ir.snapshot.get` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_ir_snapshot` | `uner.ir.snapshot` | no encontrada |
| `stm.ir.stream.set` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_mpu_stream_params` → `cmd_stm_set_ir_stream` | `uner.ir.stream` | no encontrada |
| `stm.ir.stream.stop` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_stop_ir_stream` | `uner.ir.stream` | no encontrada |
| `getPreferences` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_preferences` | `uner.preferences` | sí |
| `setPreferences` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_preferences_params` → `cmd_stm_set_preferences` | `uner.preferences` | sí |
| `getCurrentScreen` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_get_current_screen` | `uner.screen.report` | sí |
| `menuItemClick` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_menu_click_params` → `cmd_stm_menu_click` | `uner.command.status` | no encontrada |
| `triggerEncoderButton` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_press_params` → `cmd_stm_encoder_button` | `uner.command.status` | no encontrada |
| `triggerUserButton` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_press_params` → `cmd_stm_user_button` | `uner.command.status` | no encontrada |
| `requestScreenPage` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_screen_page_params` → `cmd_stm_screen_page` | `uner.command.status` | no encontrada |
| `rotateEncoderLeft` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_screen_code_params` → `cmd_stm_rotate_left` | `uner.command.status` | no encontrada |
| `rotateEncoderRight` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_screen_code_params` → `cmd_stm_rotate_right` | `uner.command.status` | no encontrada |
| `getExtensionProfile` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_empty_params` → `cmd_stm_get_extension_profile` | `uner.extension.profile` | no encontrada |
| `setExtensionProfile` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_extension_profile_params` → `cmd_stm_set_extension_profile` | `uner.extension.profile` | no encontrada |
| `rebootIntoProfile` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_extension_profile_params` → `cmd_stm_reboot_into_profile` | `uner.extension.profile` | no encontrada |
| `getCarMode` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_car_mode` | `uner.command.payload` | sí |
| `setCarMode` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_car_mode_params` → `cmd_stm_set_car_mode` | `uner.command.payload` | sí |
| `setI2cPolicy` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_i2c_policy_params` → `cmd_stm_set_i2c_policy` | `uner.command.payload` | no encontrada |
| `getI2cPolicy` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_i2c_policy` | `uner.command.payload` | no encontrada |
| `getMpuSnapshot` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_mpu_snapshot` | `uner.mpu.snapshot` | sí |
| `subscribeMpu` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_mpu_stream_params` → `cmd_stm_set_mpu_stream` | `uner.mpu.stream` | sí |
| `unsubscribeMpu` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_stop_mpu_stream` | `uner.mpu.stream` | sí |
| `getTelemetrySnapshot` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_telemetry_snapshot` | `uner.telemetry.snapshot` | sí |
| `getBalancePid` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_balance_pid` | `uner.balance.pid` | sí |
| `setBalancePid` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_pid_params` → `cmd_stm_set_balance_pid` | `uner.balance.pid` | sí |
| `getControlSnapshot` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_control_snapshot` | `uner.control.snapshot` | sí |
| `getFirmwareInfo` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_firmware_info` | `uner.firmware.info` | sí |
| `getBuildInfo` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_build_info` | `uner.build.info` | sí |
| `getBootReport` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_boot_report` | `uner.boot.report` | sí |
| `getIrSnapshot` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_get_ir_snapshot` | `uner.ir.snapshot` | sí |
| `subscribeIr` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_stm_mpu_stream_params` → `cmd_stm_set_ir_stream` | `uner.ir.stream` | sí |
| `unsubscribeIr` | stm / BRIDGE_ROUTE_FORWARD_STM | `validate_optional_params` → `cmd_stm_stop_ir_stream` | `uner.ir.stream` | sí |
