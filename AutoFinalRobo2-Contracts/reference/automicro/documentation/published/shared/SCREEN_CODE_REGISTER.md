# Registro documental de códigos de pantalla

Declaraciones leídas, sin compilar ni ejecutar comparación de píxeles. Un mismo literal no acredita equivalencia de render ni contrato completo de interacción.

| Código | Símbolo F4 | Literal Web | Metadata | Camino Web candidato |
| --- | --- | --- | --- | --- |
| [0x000000](screens/000000.md) | `SCREEN_CODE_NONE` | same-literal | no extraída | no extraído |
| [0x010001](screens/010001.md) | `SCREEN_CODE_CORE_STARTUP` | same-literal | sí | switch-case |
| [0x010101](screens/010101.md) | `SCREEN_CODE_CORE_DASHBOARD` | same-literal | sí | switch-case |
| [0x010102](screens/010102.md) | `SCREEN_CODE_CORE_MODE_CHANGE` | same-literal | sí | switch-case |
| [0x010103](screens/010103.md) | `SCREEN_CODE_CORE_STABILIZE_PROMPT` | same-literal | no extraída | identified-table |
| [0x010104](screens/010104.md) | `SCREEN_CODE_CORE_MODE_PROMPT` | same-literal | no extraída | identified-table |
| [0x010105](screens/010105.md) | `SCREEN_CODE_CORE_REMOTE_MODE_CHANGED` | same-literal | sí | switch-case |
| [0x010201](screens/010201.md) | `SCREEN_CODE_CORE_MAIN_MENU` | same-literal | sí | switch-case |
| [0x010202](screens/010202.md) | `SCREEN_CODE_CORE_FAVORITES` | same-literal | sí | remediated-priority |
| [0x020101](screens/020101.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_MENU` | same-literal | sí | switch-case |
| [0x020201](screens/020201.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_STATUS` | same-literal | sí | switch-case |
| [0x020202](screens/020202.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_SEARCHING` | same-literal | sí | switch-case |
| [0x020203](screens/020203.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_RESULTS` | same-literal | sí | switch-case |
| [0x020204](screens/020204.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_NOT_CONNECTED` | same-literal | sí | switch-case |
| [0x020205](screens/020205.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_CONNECTING` | same-literal | sí | switch-case |
| [0x020206](screens/020206.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_CONNECTED` | same-literal | sí | switch-case |
| [0x020207](screens/020207.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_SEARCH_COMPLETE` | same-literal | sí | switch-case |
| [0x020208](screens/020208.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_SEARCH_CANCELED` | same-literal | sí | switch-case |
| [0x020209](screens/020209.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_DETAILS` | same-literal | sí | identified-table |
| [0x02020A](screens/02020a.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_WEB` | same-literal | sí | switch-case |
| [0x02020B](screens/02020b.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_SUCCEEDED` | same-literal | sí | switch-case |
| [0x02020C](screens/02020c.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_FAILED` | same-literal | sí | switch-case |
| [0x02020D](screens/02020d.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_CREDENTIALS_AT` | same-literal | sí | switch-case |
| [0x02020E](screens/02020e.md) | `SCREEN_CODE_CONNECTIVITY_OSK_WIFI_SSID` | same-literal | sí | remediated-priority |
| [0x02020F](screens/02020f.md) | `SCREEN_CODE_CONNECTIVITY_OSK_WIFI_PASSWORD` | same-literal | sí | remediated-priority |
| [0x020210](screens/020210.md) | `SCREEN_CODE_CONNECTIVITY_OSK_WIFI_IPV4` | same-literal | sí | remediated-priority |
| [0x020211](screens/020211.md) | `SCREEN_CODE_CONNECTIVITY_WIFI_RETRY` | web-symbol-not-extracted | no extraída | no extraído |
| [0x020301](screens/020301.md) | `SCREEN_CODE_CONNECTIVITY_ESP_MENU` | same-literal | sí | switch-case |
| [0x020302](screens/020302.md) | `SCREEN_CODE_CONNECTIVITY_ESP_CHECKING` | same-literal | sí | switch-case |
| [0x020303](screens/020303.md) | `SCREEN_CODE_CONNECTIVITY_ESP_FIRMWARE_REQUEST` | same-literal | sí | switch-case |
| [0x020304](screens/020304.md) | `SCREEN_CODE_CONNECTIVITY_ESP_RESET_SENT` | same-literal | sí | switch-case |
| [0x020305](screens/020305.md) | `SCREEN_CODE_CONNECTIVITY_ESP_CHECK_REQUIRED` | same-literal | sí | switch-case |
| [0x020306](screens/020306.md) | `SCREEN_CODE_CONNECTIVITY_ESP_BOOT_RECEIVED` | same-literal | sí | switch-case |
| [0x020307](screens/020307.md) | `SCREEN_CODE_CONNECTIVITY_ESP_FIRMWARE_RECEIVED` | same-literal | sí | switch-case |
| [0x020308](screens/020308.md) | `SCREEN_CODE_CONNECTIVITY_ESP_MODE_CHANGED` | same-literal | sí | switch-case |
| [0x020309](screens/020309.md) | `SCREEN_CODE_CONNECTIVITY_ESP_AP_STARTED` | same-literal | sí | switch-case |
| [0x02030A](screens/02030a.md) | `SCREEN_CODE_CONNECTIVITY_ESP_REBOOT_MODE` | same-literal | sí | identified-table |
| [0x02030B](screens/02030b.md) | `SCREEN_CODE_CONNECTIVITY_ESP_UNRESPONSIVE` | same-literal | no extraída | switch-case |
| [0x02030C](screens/02030c.md) | `SCREEN_CODE_CONNECTIVITY_ESP_WATCHDOG_RESET` | same-literal | no extraída | switch-case |
| [0x02030D](screens/02030d.md) | `SCREEN_CODE_CONNECTIVITY_ESP_EXCEPTION_RESET` | same-literal | no extraída | switch-case |
| [0x020401](screens/020401.md) | `SCREEN_CODE_CONNECTIVITY_USB_CONNECTED` | same-literal | sí | switch-case |
| [0x020402](screens/020402.md) | `SCREEN_CODE_CONNECTIVITY_USB_DISCONNECTED` | same-literal | sí | switch-case |
| [0x020501](screens/020501.md) | `SCREEN_CODE_CONNECTIVITY_WEB_SERVER_UP` | same-literal | sí | switch-case |
| [0x020502](screens/020502.md) | `SCREEN_CODE_CONNECTIVITY_WEB_CLIENT_CONNECTED` | same-literal | sí | switch-case |
| [0x020503](screens/020503.md) | `SCREEN_CODE_CONNECTIVITY_WEB_CLIENT_DISCONNECTED` | same-literal | sí | switch-case |
| [0x020601](screens/020601.md) | `SCREEN_CODE_CONNECTIVITY_AP_DEVICE_CONNECTED` | same-literal | sí | switch-case |
| [0x020602](screens/020602.md) | `SCREEN_CODE_CONNECTIVITY_AP_DEVICE_DISCONNECTED` | same-literal | sí | switch-case |
| [0x030101](screens/030101.md) | `SCREEN_CODE_SENSORS_MENU` | same-literal | sí | switch-case |
| [0x030200](screens/030200.md) | `SCREEN_CODE_SENSORS_IR_MENU` | same-literal | no extraída | identified-table |
| [0x030201](screens/030201.md) | `SCREEN_CODE_SENSORS_IR_VALUES` | same-literal | sí | switch-case |
| [0x030202](screens/030202.md) | `SCREEN_CODE_SENSORS_ROUTE_ALIGNMENT` | same-literal | no extraída | identified-table |
| [0x030203](screens/030203.md) | `SCREEN_CODE_SENSORS_IR_AMBIENT` | same-literal | sí | remediated-priority |
| [0x030204](screens/030204.md) | `SCREEN_CODE_LINE_CAL_MENU` | same-literal | no extraída | no extraído |
| [0x030205](screens/030205.md) | `SCREEN_CODE_LINE_CAL_WHITE` | same-literal | no extraída | no extraído |
| [0x030206](screens/030206.md) | `SCREEN_CODE_LINE_CAL_TRACK` | same-literal | no extraída | no extraído |
| [0x030207](screens/030207.md) | `SCREEN_CODE_LINE_FOLLOW` | same-literal | no extraída | no extraído |
| [0x030208](screens/030208.md) | `SCREEN_CODE_LINE_NOTICE` | same-literal | no extraída | no extraído |
| [0x030300](screens/030300.md) | `SCREEN_CODE_SENSORS_MPU_MENU` | same-literal | no extraída | identified-table |
| [0x030301](screens/030301.md) | `SCREEN_CODE_SENSORS_MPU_VALUES` | same-literal | sí | switch-case |
| [0x030302](screens/030302.md) | `SCREEN_CODE_SENSORS_MPU_STREAM_STARTED` | same-literal | sí | identified-table |
| [0x030303](screens/030303.md) | `SCREEN_CODE_SENSORS_MPU_STREAM_STOPPED` | same-literal | sí | identified-table |
| [0x030304](screens/030304.md) | `SCREEN_CODE_SENSORS_MPU_CALIBRATION` | same-literal | no extraída | identified-table |
| [0x030305](screens/030305.md) | `SCREEN_CODE_SENSORS_MPU_CALIBRATION_NOTICE` | same-literal | no extraída | identified-table |
| [0x030306](screens/030306.md) | `SCREEN_CODE_SENSORS_MPU_CALIBRATION_DONE` | same-literal | no extraída | identified-table |
| [0x030401](screens/030401.md) | `SCREEN_CODE_SENSORS_RADAR` | same-literal | sí | switch-case |
| [0x030500](screens/030500.md) | `SCREEN_CODE_SENSORS_DISPLAY_MENU` | same-literal | sí | switch-case |
| [0x030501](screens/030501.md) | `SCREEN_CODE_SENSORS_DISPLAY_ROTATION_TEST` | same-literal | sí | identified-table |
| [0x030502](screens/030502.md) | `SCREEN_CODE_SENSORS_DISPLAY_ROTATION_MPU_TEST` | same-literal | sí | identified-table |
| [0x030503](screens/030503.md) | `SCREEN_CODE_SENSORS_DISPLAY_OLED_CANVAS` | same-literal | sí | switch-case |
| [0x030600](screens/030600.md) | `SCREEN_CODE_SENSORS_KEYBOARD_MENU` | same-literal | sí | remediated-priority |
| [0x030601](screens/030601.md) | `SCREEN_CODE_TEST_KEYBOARD_ASCII` | same-literal | sí | remediated-priority |
| [0x030602](screens/030602.md) | `SCREEN_CODE_TEST_KEYBOARD_NUMERIC` | same-literal | sí | remediated-priority |
| [0x040100](screens/040100.md) | `SCREEN_CODE_MOTOR_MENU` | same-literal | no extraída | identified-table |
| [0x040101](screens/040101.md) | `SCREEN_CODE_MOTOR_TEST` | same-literal | sí | switch-case |
| [0x040102](screens/040102.md) | `SCREEN_CODE_MOTOR_LEFT_TEST` | same-literal | no extraída | identified-table |
| [0x040103](screens/040103.md) | `SCREEN_CODE_MOTOR_RIGHT_TEST` | same-literal | no extraída | identified-table |
| [0x040104](screens/040104.md) | `SCREEN_CODE_MOTOR_CALIBRATION` | same-literal | no extraída | remediated-priority |
| [0x040200](screens/040200.md) | `SCREEN_CODE_BALANCE_MENU` | same-literal | no extraída | identified-table |
| [0x040201](screens/040201.md) | `SCREEN_CODE_BALANCE_PID_STATUS` | same-literal | no extraída | identified-table |
| [0x040202](screens/040202.md) | `SCREEN_CODE_BALANCE_PID_LOG` | same-literal | no extraída | identified-table |
| [0x040203](screens/040203.md) | `SCREEN_CODE_BALANCE_PID_KP` | same-literal | no extraída | identified-table |
| [0x040204](screens/040204.md) | `SCREEN_CODE_BALANCE_PID_KI` | same-literal | no extraída | identified-table |
| [0x040205](screens/040205.md) | `SCREEN_CODE_BALANCE_PID_KD` | same-literal | no extraída | identified-table |
| [0x050101](screens/050101.md) | `SCREEN_CODE_SETTINGS_MENU` | same-literal | sí | switch-case |
| [0x050200](screens/050200.md) | `SCREEN_CODE_SETTINGS_ABOUT_MENU` | same-literal | no extraída | identified-table |
| [0x050201](screens/050201.md) | `SCREEN_CODE_SETTINGS_ABOUT_PROJECT` | same-literal | sí | switch-case |
| [0x050202](screens/050202.md) | `SCREEN_CODE_SETTINGS_ABOUT_REPO` | same-literal | sí | switch-case |
| [0x050203](screens/050203.md) | `SCREEN_CODE_SETTINGS_ABOUT_BUILD` | same-literal | no extraída | identified-table |
| [0x050204](screens/050204.md) | `SCREEN_CODE_SETTINGS_ABOUT_BOOTLOADER` | same-literal | sí | remediated-priority |
| [0x050301](screens/050301.md) | `SCREEN_CODE_SETTINGS_WARNING_TIME` | same-literal | sí | switch-case |
| [0x050400](screens/050400.md) | `SCREEN_CODE_SETTINGS_PREFS_MENU` | same-literal | no extraída | identified-table |
| [0x050401](screens/050401.md) | `SCREEN_CODE_SETTINGS_PREFS_ENCODER_MENU` | same-literal | no extraída | identified-table |
| [0x050402](screens/050402.md) | `SCREEN_CODE_SETTINGS_PREFS_ENCODER_INVERT` | same-literal | no extraída | identified-table |
| [0x050403](screens/050403.md) | `SCREEN_CODE_SETTINGS_PREFS_DISPLAY_MENU` | same-literal | no extraída | identified-table |
| [0x050404](screens/050404.md) | `SCREEN_CODE_SETTINGS_PREFS_DISPLAY_LOCK` | same-literal | no extraída | identified-table |
| [0x050405](screens/050405.md) | `SCREEN_CODE_SETTINGS_PREFS_DISPLAY_TIMEOUT` | same-literal | no extraída | identified-table |
| [0x050406](screens/050406.md) | `SCREEN_CODE_SETTINGS_PREFS_MOTORS_MENU` | same-literal | no extraída | identified-table |
| [0x050407](screens/050407.md) | `SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_MENU` | same-literal | no extraída | identified-table |
| [0x050408](screens/050408.md) | `SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_LEFT` | same-literal | no extraída | identified-table |
| [0x050409](screens/050409.md) | `SCREEN_CODE_SETTINGS_PREFS_MOTOR_PWM_RIGHT` | same-literal | no extraída | identified-table |
| [0x05040A](screens/05040a.md) | `SCREEN_CODE_SETTINGS_PREFS_SYSTEM_MENU` | same-literal | no extraída | identified-table |
| [0x05040B](screens/05040b.md) | `SCREEN_CODE_SETTINGS_PREFS_SYSTEM_ACTION_TIMEOUT` | same-literal | no extraída | identified-table |
| [0x05040C](screens/05040c.md) | `SCREEN_CODE_SETTINGS_PREFS_ROTATION_MENU` | same-literal | no extraída | identified-table |
| [0x05040D](screens/05040d.md) | `SCREEN_CODE_SETTINGS_PREFS_ROTATION_TEST` | same-literal | no extraída | identified-table |
| [0x05040E](screens/05040e.md) | `SCREEN_CODE_SETTINGS_PREFS_ROTATION_MPU_TEST` | same-literal | no extraída | identified-table |
| [0x05040F](screens/05040f.md) | `SCREEN_CODE_SETTINGS_PREFS_ROTATION_AUTO` | same-literal | no extraída | identified-table |
| [0x050410](screens/050410.md) | `SCREEN_CODE_SETTINGS_PREFS_UNER_ROUTER_MENU` | same-literal | no extraída | identified-table |
| [0x050411](screens/050411.md) | `SCREEN_CODE_SETTINGS_PREFS_UNER_ROUTER_FORWARDING` | same-literal | no extraída | identified-table |
| [0x050412](screens/050412.md) | `SCREEN_CODE_SETTINGS_PREFS_LINE_MENU` | same-literal | no extraída | identified-table |
| [0x050413](screens/050413.md) | `SCREEN_CODE_SETTINGS_PREFS_LINE_WIDTH` | same-literal | no extraída | identified-table |
| [0x050414](screens/050414.md) | `SCREEN_CODE_SETTINGS_PREFS_SYSTEM_USB_ALIVE` | same-literal | no extraída | identified-table |
| [0x050415](screens/050415.md) | `SCREEN_CODE_SETTINGS_PREFS_SECURITY_MENU` | same-literal | no extraída | identified-table |
| [0x050416](screens/050416.md) | `SCREEN_CODE_SETTINGS_PREFS_SECURITY_PIN_LOCK` | same-literal | no extraída | identified-table |
| [0x050417](screens/050417.md) | `SCREEN_CODE_SETTINGS_PREFS_UNER_FORWARDING_ENABLED` | same-literal | no extraída | switch-case |
| [0x050418](screens/050418.md) | `SCREEN_CODE_SETTINGS_PREFS_UNER_FORWARDING_DISABLED` | same-literal | no extraída | switch-case |
| [0x050419](screens/050419.md) | `SCREEN_CODE_SETTINGS_PREFS_IR_MENU` | same-literal | sí | remediated-priority |
| [0x05041A](screens/05041a.md) | `SCREEN_CODE_SETTINGS_PREFS_IR_OBJECTS` | same-literal | sí | remediated-priority |
| [0x05041B](screens/05041b.md) | `SCREEN_CODE_SETTINGS_PREFS_IR_LINE` | same-literal | sí | remediated-priority |
| [0x05041C](screens/05041c.md) | `SCREEN_CODE_SETTINGS_PREFS_IR_LIGHTS` | same-literal | sí | remediated-priority |
| [0x05041D](screens/05041d.md) | `SCREEN_CODE_SETTINGS_PREFS_WIFI_AP_FALLBACK` | web-symbol-not-extracted | no extraída | no extraído |
| [0x050500](screens/050500.md) | `SCREEN_CODE_SETTINGS_SYSTEM_MENU` | same-literal | sí | remediated-priority |
| [0x050501](screens/050501.md) | `SCREEN_CODE_SETTINGS_SYSTEM_BOOTLOADER_CONFIRM` | same-literal | sí | remediated-priority |
| [0x060101](screens/060101.md) | `SCREEN_CODE_DIAG_CONTROLLER_CONNECTED` | same-literal | sí | switch-case |
| [0x060102](screens/060102.md) | `SCREEN_CODE_DIAG_CONTROLLER_DISCONNECTED` | same-literal | sí | switch-case |
| [0x060201](screens/060201.md) | `SCREEN_CODE_DIAG_COMMAND_RECEIVED` | same-literal | sí | switch-case |
| [0x060202](screens/060202.md) | `SCREEN_CODE_DIAG_PING_RECEIVED` | same-literal | sí | switch-case |
| [0x060301](screens/060301.md) | `SCREEN_CODE_DIAG_ESP_CONN_SUCCEEDED` | same-literal | sí | switch-case |
| [0x060302](screens/060302.md) | `SCREEN_CODE_DIAG_ESP_CONN_FAILED` | same-literal | sí | switch-case |
| [0x070101](screens/070101.md) | `SCREEN_CODE_SERVICE_TEST_SCREEN` | same-literal | sí | switch-case |
| [0x070200](screens/070200.md) | `SCREEN_CODE_SERVICE_UNER_ROUTER_MENU` | same-literal | no extraída | identified-table |
| [0x070201](screens/070201.md) | `SCREEN_CODE_SERVICE_UNER_ROUTER_USB` | same-literal | no extraída | identified-table |
| [0x070202](screens/070202.md) | `SCREEN_CODE_SERVICE_UNER_ROUTER_UART` | same-literal | no extraída | identified-table |
| [0x070203](screens/070203.md) | `SCREEN_CODE_SERVICE_UNER_ROUTER_NRF` | same-literal | no extraída | identified-table |
| [0x080101](screens/080101.md) | `SCREEN_CODE_WARNING_LOCKED` | same-literal | sí | switch-case |
| [0x080102](screens/080102.md) | `SCREEN_CODE_WARNING_PIN_INCORRECT` | same-literal | sí | switch-case |
| [0x080103](screens/080103.md) | `SCREEN_CODE_WARNING_PIN_MODIFIED` | same-literal | sí | switch-case |
| [0x080104](screens/080104.md) | `SCREEN_CODE_WARNING_PIN_ENTRY` | same-literal | sí | switch-case |
| [0x080105](screens/080105.md) | `SCREEN_CODE_WARNING_PIN_WAITING` | same-literal | sí | switch-case |
| [0x080106](screens/080106.md) | `SCREEN_CODE_WARNING_PIN_DENIED` | same-literal | sí | switch-case |
| [0x080107](screens/080107.md) | `SCREEN_CODE_WARNING_PIN_TIMEOUT` | same-literal | sí | switch-case |
| [0x080108](screens/080108.md) | `SCREEN_CODE_WARNING_PIN_BLOCKED` | same-literal | sí | switch-case |
| [0x080109](screens/080109.md) | `SCREEN_CODE_WARNING_PERMISSION_DENIED` | same-literal | sí | switch-case |
| [0x08010A](screens/08010a.md) | `SCREEN_CODE_WARNING_REMOTE_AUTHENTICATED` | same-literal | no extraída | identified-table |
| [0x08010B](screens/08010b.md) | `SCREEN_CODE_WARNING_PIN_CORRECT` | same-literal | no extraída | identified-table |
| [0x08010C](screens/08010c.md) | `SCREEN_CODE_WARNING_PIN_BYPASSED` | same-literal | no extraída | identified-table |
| [0x090101](screens/090101.md) | `SCREEN_CODE_INPUT_KEYBOARD_ALPHA` | same-literal | sí | remediated-priority |
| [0x090102](screens/090102.md) | `SCREEN_CODE_INPUT_KEYBOARD_NUMERIC` | same-literal | sí | remediated-priority |
