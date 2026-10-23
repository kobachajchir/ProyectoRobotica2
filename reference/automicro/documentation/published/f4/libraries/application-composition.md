# Composición, tareas y acciones de aplicación

ID: `f4:module/Core/Inc/app_tasks.h` · Capa: **composición** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Construye instancias, conecta puertos y coordina tareas, acciones de menú y eventos de hardware.

Es el punto permitido de conocimiento de todas las librerías: sensores, control, UI, preferencias y comunicación.

<a id="modularity"></a>
## Modularidad observada

Composición deliberadamente dependiente de HAL y APIs concretas. Los núcleos no deben incluir app_tasks, globals o adaptadores para acceder indirectamente a otros dispositivos.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

SystemHandle agrupa runtime/connectivity/configuration/security/metadata y referencias opacas. Cada librería conserva su estado propio; app_tasks/eventManagers mantienen además estado privado de coordinación. System no es un sustituto de todos los handles.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- APP_Init conecta preferencias, motores, AutoMotion, I2C, MPU, balance, estimación, línea, display y menú en el orden del código.
- La aplicación ejecuta tareas periódicas y trabajo diferido; los callbacks HAL I2C reenvían eventos al gestor.
- El camino de control reúne entradas, atiende seguidor/GOTO, estimación y MotionControl; los modos determinan qué productor puede operar.
- eventManagers modifica borradores/acciones y wrappers seleccionan render; una edición no se considera aplicada hasta guardar/confirmar.

## Trabajo en tándem

- Aquí se conectan OLED y MPU al mismo árbitro y las tablas AM_* a instancias concretas.
- Los handlers remotos llaman acciones APP y pasan por guardas de modo, permiso y seguridad.

## Precauciones y límites

- Las funciones HAL_*Callback son entradas de interrupción y pueden tocar estado compartido; no inferir serialización de tareas por el nombre.
- Esta ficha inventaría también funciones internas; su descripción estructural no certifica todos los caminos dinámicos.
- Los cambios de orden de inicialización/control requieren revisar consumidores y no solo includes.

## Hallazgos vinculados

- [AMDOC-F4-003: La marca RESP permite longitudes fuera del rango de solicitud](../../../review/FINDINGS.md#amdoc-f4-003)
- [AMDOC-F4-009: GetScaledSteps devuelve 1 con desplazamiento cero](../../../review/FINDINGS.md#amdoc-f4-009)
- [AMDOC-F4-010: sampleTenMs se almacena pero no gobierna el muestreo](../../../review/FINDINGS.md#amdoc-f4-010)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/app_tasks.h` | `system.h`, `stdint.h`, `app_preferences_binding.h`, `user_button.h`, `encoder.h`, `i2c_manager.h`, `types/carmode_type.h`, `tcrt5000_data.h`, `line_calibration.h` | `559931f16be0aff75a266bc5b26b9aa688a60db2bdd2491437e9d04a7177b32b` |
| `Core/Inc/eventManagers.h` | `screen_codes.h`, `types/userEvent_type.h` | `ad76b259fba382c4eca927f57ee964035fff784521e73c2381120a471f30cd21` |
| `Core/Inc/menu_definitions.h` | `app_menu_binding.h` | `c7e9b9ac094807c7331e0b2ec758fba914dd80820c24401207e0d06e6ff80103` |
| `Core/Inc/system.h` | `stdbool.h`, `stdint.h`, `types/bitmap_type.h`, `types/IPStruct_t.h`, `types/carmode_type.h` | `f882776bcb51dfc1d328ed3facb8f1f162bb4ed6399e2768715a61be7d2230d6` |
| `Core/Inc/system_flags.h` | `utils/macros_utils.h` | `53735947552bceb90deeeb92662ef0e0c8e20c0e0210b69efd9b244610b482f7` |
| `Core/Inc/globals.h` | `system.h`, `tcrt5000.h`, `balance_control.h`, `encoder.h`, `i2c_manager.h`, `line_follow_control.h`, `app_menu_binding.h`, `motor_control.h`, `motion_control.h`, `motion_estimator.h`, `mpu9250.h`, `oled_handle.h`, `types/IPStruct_t.h`, `types/carmode_type.h`, `user_button.h`, `utils/macros_utils.h`, `stdbool.h`, `stdint.h`, `system_flags.h` | `1f0a3a413497d25d43256611f853cee6f7126160426bf1684e47190b223e5794` |
| `Core/Src/app_tasks.c` | `app_tasks.h`, `app_stm32_ports.h`, `app_i2c_ports.h`, `app_control_ports.h`, `app_menu_services.h`, `app_wifi_ports.h`, `app_screens_ports.h`, `app_auxiliary_ports.h`, `math.h`, `string.h`, `app_preferences_binding.h`, `eventManagers.h`, `globals.h`, `line_follow_control.h`, `main.h`, `menu_definitions.h`, `app_menu_binding.h`, `motion_estimator.h`, `mpu9250.h`, `menu_favorites.h`, `oled_canvas.h`, `oled_utils.h`, `permissions.h`, `screen_codes.h`, `screen_lock.h`, `screenWrappers.h`, `app_display.h`, `tcrt5000.h`, `ui_event_router.h`, `uner_app.h`, `usbd_cdc_if.h`, `usb_mode_manager.h`, `utils/macros_utils.h`, `app_wifi_binding.h` | `3780c5c2408fc62a030384cfca230973f9db944e2b6f2b348c3cc0cf03ffdf06` |
| `Core/Src/eventManagers.c` | `app_wifi_binding.h`, `main.h`, `menu_favorites.h`, `automicro_boot_contract.h`, `app_preferences_binding.h`, `app_tasks.h`, `eventManagers.h`, `oled_canvas.h`, `screenWrappers.h`, `oled_utils.h`, `oled_handle.h`, `globals.h`, `permissions.h`, `menu_definitions.h`, `screen_lock.h`, `stm32f4xx_hal.h`, `string.h` | `08a5b3df9cc68722d6b8c56715de569d5676b514ac3f3b744e481fd2b3f8ac8f` |
| `Core/Src/menu_definition.c` | `main.h`, `menu_definitions.h`, `screenWrappers.h`, `notificationWrappers.h`, `app_preferences_binding.h`, `app_ap_config.h`, `eventManagers.h`, `app_tasks.h`, `oled_utils.h`, `oled_canvas.h`, `fonts.h`, `globals.h`, `app_wifi_binding.h`, `wifi_bridge_at.h`, `usb_mode_manager.h`, `osk_integration.h`, `stdio.h`, `math.h`, `string.h` | `7451f93230301ce366a2596ea77f05f350e5e0ec34e7477c8dc4d6ac08afe910` |
| `Core/Src/app_globals.c` | `globals.h`, `uirouter_context.h`, `ap_context.h`, `favorites_context.h`, `permission_context.h`, `screen_context.h`, `app_wifi_binding.h`, `app_preferences_binding.h`, `app_display.h`, `app_version.h` | `5e16175426eb58cebed8fa2dd7037505798e45c017efa97fa5945c89eadaf643` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `EventCallbacks_t`

`f4:Core/Inc/eventManagers.h:14`

| Atributo / firma | Función y precaución |
| --- | --- |
| `RenderFn onRotateCW` | Dato de estado/configuración `onRotateCW`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderFn onRotateCCW` | Dato de estado/configuración `onRotateCCW`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderFn onShortPress` | Dato de estado/configuración `onShortPress`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderFn onLongPress` | Dato de estado/configuración `onLongPress`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderFn onUserButton` | Dato de estado/configuración `onUserButton`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderFn onEncLongPress` | Dato de estado/configuración `onEncLongPress`; declaración estructural, interpretación específica pendiente de ampliar. |

### `RenderFn`

`f4:Core/Inc/eventManagers.h:6`

Firma: `void (*RenderFn)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `RenderWrapperFn`

`f4:Core/Inc/eventManagers.h:9`

Firma: `void (*RenderWrapperFn)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `GenericConfirmAction_t`

`f4:Core/Inc/eventManagers.h:63`

Firma: `void (*GenericConfirmAction_t)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `GenericConfirmCancel_t`

`f4:Core/Inc/eventManagers.h:64`

Firma: `void (*GenericConfirmCancel_t)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `ESPWiFiConnectionInfo_t`

`f4:Core/Inc/system.h:12`

| Atributo / firma | Función y precaución |
| --- | --- |
| `IPStruct_t staIp` | Dato de estado/configuración `staIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `IPStruct_t apIp` | Dato de estado/configuración `apIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char staSsid[WIFI_SSID_MAX_LEN + 1u]` | Dato de estado/configuración `staSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char apSsid[WIFI_SSID_MAX_LEN + 1u]` | Dato de estado/configuración `apSsid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char apPassword[65u]` | Dato de estado/configuración `apPassword`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t staIpValid` | Dato de estado/configuración `staIpValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t apIpValid` | Dato de estado/configuración `apIpValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t apClients` | Dato de estado/configuración `apClients`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t apInfoValid` | Dato de estado/configuración `apInfoValid`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SystemRuntime`

`f4:Core/Inc/system.h:23`

| Atributo / firma | Función y precaución |
| --- | --- |
| `volatile Byte_Flag_Struct systemFlags` | Dato de estado/configuración `systemFlags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile Byte_Flag_Struct systemFlags2` | Dato de estado/configuración `systemFlags2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile Byte_Flag_Struct systemFlags3` | Dato de estado/configuración `systemFlags3`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile Byte_Flag_Struct carModeFlags` | Dato de estado/configuración `carModeFlags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t sensor_raw_data[8]` | Dato de estado/configuración `sensor_raw_data`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t inside_menu_flag` | Dato de estado/configuración `inside_menu_flag`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t encoder_fast_scroll_enabled` | Dato de estado/configuración `encoder_fast_scroll_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile bool oled_first_draw` | Dato de estado/configuración `oled_first_draw`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile bool mpu_trigger` | Dato de estado/configuración `mpu_trigger`; declaración estructural, interpretación específica pendiente de ampliar. |
| `CarMode_t auxCarMode` | Dato de estado/configuración `auxCarMode`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SystemConnectivity`

`f4:Core/Inc/system.h:35`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint16_t wifiSearchingTimeout` | Dato de estado/configuración `wifiSearchingTimeout`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t networksFound` | Dato de estado/configuración `networksFound`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiNetworkSsids[WIFI_SCAN_MAX_NETWORKS][WIFI_SSID_MAX_LEN + 1u]` | Dato de estado/configuración `wifiNetworkSsids`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char wifiNetworkEncryptions[WIFI_SCAN_MAX_NETWORKS][WIFI_ENCRYPTION_MAX_LEN + 1u]` | Dato de estado/configuración `wifiNetworkEncryptions`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t wifiNetworkSignalStrengths[WIFI_SCAN_MAX_NETWORKS]` | Dato de estado/configuración `wifiNetworkSignalStrengths`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiNetworkChannels[WIFI_SCAN_MAX_NETWORKS]` | Dato de estado/configuración `wifiNetworkChannels`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiNetworkEncryptionTypes[WIFI_SCAN_MAX_NETWORKS]` | Dato de estado/configuración `wifiNetworkEncryptionTypes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiNetworkDetailValid[WIFI_SCAN_MAX_NETWORKS]` | Dato de estado/configuración `wifiNetworkDetailValid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiSelectedNetworkIndex` | Dato de estado/configuración `wifiSelectedNetworkIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t wifiScanSessionActive` | Dato de estado/configuración `wifiScanSessionActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t wifiScanResultsPending` | Dato de estado/configuración `wifiScanResultsPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile IPStruct_t espStaIp` | Dato de estado/configuración `espStaIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile IPStruct_t espApIp` | Dato de estado/configuración `espApIp`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ESPWiFiConnectionInfo_t espWifiConnection` | Dato de estado/configuración `espWifiConnection`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char espFirmwareVersion[33]` | Dato de estado/configuración `espFirmwareVersion`; declaración estructural, interpretación específica pendiente de ampliar. |

### `SystemHandle`

`f4:Core/Inc/system.h:52`

| Atributo / firma | Función y precaución |
| --- | --- |
| `SystemRuntime runtime` | Dato de estado/configuración `runtime`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SystemConnectivity connectivity` | Dato de estado/configuración `connectivity`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } configuration` | Dato de estado/configuración `configuration`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } security` | Dato de estado/configuración `security`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } metadata` | Dato de estado/configuración `metadata`; declaración estructural, interpretación específica pendiente de ampliar. |
| `struct { estructura anidada } handles` | Dato de estado/configuración `handles`; declaración estructural, interpretación específica pendiente de ampliar. |

### `configuration`

`f4:Core/Inc/system.h:55`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const void *preferences` | Dato de estado/configuración `preferences`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t capabilities` | Dato de estado/configuración `capabilities`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool ir_objects_enabled, ir_line_enabled, ir_lights_enabled` | Dato de estado/configuración `ir_lights_enabled`; declaración estructural, interpretación específica pendiente de ampliar. |

### `security`

`f4:Core/Inc/system.h:56`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *permissions` | Dato de estado/configuración `permissions`; declaración estructural, interpretación específica pendiente de ampliar. |

### `metadata`

`f4:Core/Inc/system.h:57`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const char *name` | Dato de estado/configuración `name`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *version` | Dato de estado/configuración `version`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *target` | Dato de estado/configuración `target`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *build_date, *build_time, *build_profile, *build_compiler` | Dato de estado/configuración `build_compiler`; declaración estructural, interpretación específica pendiente de ampliar. |

### `handles`

`f4:Core/Inc/system.h:58`

| Atributo / firma | Función y precaución |
| --- | --- |
| `void *encoder, *button, *motors, *mpu, *ir, *i2c, *menu, *display` | Dato de estado/configuración `display`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *balance, *motion, *estimator, *line, *wifi, *protocol` | Dato de estado/configuración `protocol`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *screens, *tasks, *events, *preferences` | Dato de estado/configuración `preferences`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *oled_ui, *favorites, *ap_config, *ui_router` | Dato de estado/configuración `ui_router`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppTasksState`

`f4:Core/Src/app_tasks.c:115`

| Atributo / firma | Función y precaución |
| --- | --- |
| `PidController_Handle balancePid` | Dato de estado/configuración `balancePid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `PidController_Handle linePid` | Dato de estado/configuración `linePid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppI2cBinding oledBusBinding` | Dato de estado/configuración `oledBusBinding`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppI2cBinding mpuBusBinding` | Dato de estado/configuración `mpuBusBinding`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t appInputReady` | Dato de estado/configuración `appInputReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t appTim3Tick10ms` | Dato de estado/configuración `appTim3Tick10ms`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t appMpuTick250us` | Dato de estado/configuración `appMpuTick250us`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t appUiTick10msPending` | Dato de estado/configuración `appUiTick10msPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint8_t appMotionTick5msPending` | Dato de estado/configuración `appMotionTick5msPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appRemoteMotorTestActive` | Dato de estado/configuración `appRemoteMotorTestActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t appRemoteMotorTestUntilMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `volatile uint8_t appLineTick10msPending` | Dato de estado/configuración `appLineTick10msPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appDisplayReady` | Dato de estado/configuración `appDisplayReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appMpuReady` | Dato de estado/configuración `appMpuReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appMpuCalibrationSavePending` | Dato de estado/configuración `appMpuCalibrationSavePending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appOled10msCounter` | Dato de estado/configuración `appOled10msCounter`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appOledAutoRotation10msCounter` | Dato de estado/configuración `appOledAutoRotation10msCounter`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appOledAutoRotationInTiltZone` | Dato de estado/configuración `appOledAutoRotationInTiltZone`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appOledAutoRotationLastEnabled` | Dato de estado/configuración `appOledAutoRotationLastEnabled`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appEspPostInitRebootDisconnectObserved` | Dato de estado/configuración `appEspPostInitRebootDisconnectObserved`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t appLastEspAliveMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `WifiBridge_Backend appLastEspAliveBackend` | Dato de estado/configuración `appLastEspAliveBackend`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appEspAliveBackendInitialized` | Dato de estado/configuración `appEspAliveBackendInitialized`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appEspNotifWatchdogCounter` | Dato de estado/configuración `appEspNotifWatchdogCounter`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t appEspPostInitRebootLastAttemptMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t appEspPostInitRebootGraceUntilMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t appUsbAliveLastMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t appUsbAliveCount` | Dato de estado/configuración `appUsbAliveCount`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t appMpuCalibrationPromptStartedMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t appMpuCalibrationDoneStartedMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t appLineFollowWasActive` | Dato de estado/configuración `appLineFollowWasActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appUsbDtrLast` | Dato de estado/configuración `appUsbDtrLast`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appStmStartupComplete` | Dato de estado/configuración `appStmStartupComplete`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appEspPostInitRebootPending` | Dato de estado/configuración `appEspPostInitRebootPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appAtModeChangePending` | Dato de estado/configuración `appAtModeChangePending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appAtConnectCommandPending` | Dato de estado/configuración `appAtConnectCommandPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appWifiApFallbackSyncConfirmed` | Dato de estado/configuración `appWifiApFallbackSyncConfirmed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appWifiApFallbackSyncedValue` | Dato de estado/configuración `appWifiApFallbackSyncedValue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t appWifiApFallbackNextSyncMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t appAtConnectPending` | Dato de estado/configuración `appAtConnectPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appWifiStatusRefreshPending` | Dato de estado/configuración `appWifiStatusRefreshPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appAtFirmwareRequestPending` | Dato de estado/configuración `appAtFirmwareRequestPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t appEspStartupGraceUntilMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `AppMpuCalibrationUiState appMpuCalibrationUiState` | Dato de estado/configuración `appMpuCalibrationUiState`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppEventsState`

`f4:Core/Src/eventManagers.c:105`

| Atributo / firma | Función y precaución |
| --- | --- |
| `char wifiAtPasswordDraft[WIFI_BRIDGE_PASS_MAX_LEN + 1u]` | Dato de estado/configuración `wifiAtPasswordDraft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiAtPasswordLength` | Dato de estado/configuración `wifiAtPasswordLength`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t wifiAtPasswordCharacterIndex` | Dato de estado/configuración `wifiAtPasswordCharacterIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool dashboardModeCanConfirm` | Dato de estado/configuración `dashboardModeCanConfirm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t dashboardModeLastInputMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t dashboardActionTimeoutLastRenderMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t dashboardQuickActionIndex` | Dato de estado/configuración `dashboardQuickActionIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool dashboardQuickActionActive` | Dato de estado/configuración `dashboardQuickActionActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t dashboardQuickActionLastInputMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `bool motorMinPwmEditLeft` | Dato de estado/configuración `motorMinPwmEditLeft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t motorMinPwmDraft` | Dato de estado/configuración `motorMinPwmDraft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t motorMinPwmOriginal` | Dato de estado/configuración `motorMinPwmOriginal`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t systemActionTimeoutDraft` | Dato de estado/configuración `systemActionTimeoutDraft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t systemActionTimeoutOriginal` | Dato de estado/configuración `systemActionTimeoutOriginal`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t usbAlivePeriodDraftMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t usbAlivePeriodOriginalMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint16_t lineBlackWidthDraftMm` | Dato de estado/configuración `lineBlackWidthDraftMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t lineBlackWidthOriginalMm` | Dato de estado/configuración `lineBlackWidthOriginalMm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t oledRotationDraftDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t oledRotationOriginalDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t oledRotationMpuTestDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint16_t oledRotationMpuTestOriginalDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `int16_t oledRotationMpuTestYawDeg` | Ángulo en grados; respetar eje y referencia de esta librería. |
| `uint8_t oledRotationMpuTestReady` | Dato de estado/configuración `oledRotationMpuTestReady`; declaración estructural, interpretación específica pendiente de ampliar. |
| `Motor_Data_t motorMinPwmSavedData` | Dato de estado/configuración `motorMinPwmSavedData`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool motorMinPwmSaved` | Dato de estado/configuración `motorMinPwmSaved`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool motorSingleTestLeft` | Dato de estado/configuración `motorSingleTestLeft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float motorSingleTestSpeed` | Dato de estado/configuración `motorSingleTestSpeed`; declaración estructural, interpretación específica pendiente de ampliar. |
| `Motor_Direction_t motorSingleTestDirection` | Dato de estado/configuración `motorSingleTestDirection`; declaración estructural, interpretación específica pendiente de ampliar. |
| `Motor_Data_t motorSingleTestSavedData` | Dato de estado/configuración `motorSingleTestSavedData`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool motorSingleTestSaved` | Dato de estado/configuración `motorSingleTestSaved`; declaración estructural, interpretación específica pendiente de ampliar. |
| `BalancePidGainTarget_t balancePidGainTarget` | Dato de estado/configuración `balancePidGainTarget`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float balancePidGainDraft` | Dato de estado/configuración `balancePidGainDraft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `float balancePidGainOriginal` | Dato de estado/configuración `balancePidGainOriginal`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balancePidLogDurationDraftSec` | Dato de estado/configuración `balancePidLogDurationDraftSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t balancePidLogDurationOriginalSec` | Dato de estado/configuración `balancePidLogDurationOriginalSec`; declaración estructural, interpretación específica pendiente de ampliar. |

### `GenericConfirmContext_t`

`f4:Core/Src/eventManagers.c:651`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t startedMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t lastRenderMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t timeoutMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `const char *line1` | Dato de estado/configuración `line1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *line2` | Dato de estado/configuración `line2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `GenericConfirmAction_t onConfirm` | Dato de estado/configuración `onConfirm`; declaración estructural, interpretación específica pendiente de ampliar. |
| `GenericConfirmCancel_t onCancel` | Dato de estado/configuración `onCancel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t screenCode` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void (*prevRenderFn)(void)` | Operación prev Render Fn de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. |
| `void (*prevUserEventManagerFn)(UserEvent_t)` | Operación prev User Event Manager Fn de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. |
| `uint8_t prevInsideMenuFlag` | Memoria de la observación anterior usada por la máquina de estados. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-app_applyirstate"></a>`void APP_ApplyIrState(void)` | Traslada las preferencias/habilitaciones IR a las salidas y estado de aplicación. | `f4:Core/Inc/app_tasks.h:61`; editorial |
| <a id="fn-app_applypreferenceruntimechanges"></a>`void APP_ApplyPreferenceRuntimeChanges(AppPreferencesFieldMask_t changedFields)` | Propaga los campos de preferencias modificados a sus consumidores runtime. | `f4:Core/Inc/app_tasks.h:60`; editorial |
| <a id="fn-app_automotionbusy"></a>`bool APP_AutoMotionBusy(void)` | Consulta si AutoMotion tiene una operación activa. | `f4:Core/Inc/app_tasks.h:26`; editorial |
| <a id="fn-app_automotioncancel"></a>`void APP_AutoMotionCancel(void)` | Cancela misión/calibración GOTO activa. | `f4:Core/Inc/app_tasks.h:27`; editorial |
| <a id="fn-app_automotionrequest"></a>`uint8_t APP_AutoMotionRequest(uint8_t cmd,const uint8_t *request,uint8_t length,uint8_t *response)` | Decodifica solicitudes GOTO/calibración y construye respuesta; requiere corregir la guarda de longitud descrita en AMDOC-F4-003. | `f4:Core/Inc/app_tasks.h:28`; editorial |
| <a id="fn-app_balancesetenabled"></a>`uint8_t APP_BalanceSetEnabled(bool enabled)` | Coordina `APP_AutoMotionCancel`, `APP_LineCalibrationBusy`, `APP_LineFollowStop` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:31`; structural |
| <a id="fn-app_copyadcsnapshot"></a>`void APP_CopyAdcSnapshot(uint16_t raw[8])` | Adapta la llamada a `APP_CopyAdc` con el contexto de esta integración. | `f4:Core/Inc/app_tasks.h:39`; structural |
| <a id="fn-app_espfirmwarerequestrequested"></a>`void APP_ESPFirmwareRequestRequested(void)` | Registra que se solicitó firmware ESP para atenderlo en el flujo de UI/aplicación. | `f4:Core/Inc/app_tasks.h:48`; editorial |
| <a id="fn-app_espmodechangecommandaccepted"></a>`void APP_ESPModeChangeCommandAccepted(void)` | Registra aceptación del comando de cambio de modo sin afirmar finalización de red. | `f4:Core/Inc/app_tasks.h:43`; editorial |
| <a id="fn-app_espstatusrefreshrequested"></a>`void APP_ESPStatusRefreshRequested(void)` | Programa una actualización de estado ESP. | `f4:Core/Inc/app_tasks.h:47`; editorial |
| <a id="fn-app_espwifiapfallbackresponse"></a>`void APP_ESPWifiApFallbackResponse(uint8_t status, uint8_t enabled)` | Coordina `AppPreferences_IsWifiApFallbackEnabled`, `HAL_GetTick` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:45`; structural |
| <a id="fn-app_espwificonnectrequested"></a>`void APP_ESPWifiConnectRequested(void)` | Registra inicio de solicitud de conexión Wi-Fi ESP. | `f4:Core/Inc/app_tasks.h:44`; editorial |
| <a id="fn-app_evaluateroute"></a>`bool APP_EvaluateRoute(TCRT5000_RouteSample_t *out)` | Adapta la llamada a `APP_ReadIrRouteSnapshot` con el contexto de esta integración. | `f4:Core/Inc/app_tasks.h:37`; structural |
| <a id="fn-app_eventsbindsystem"></a>`void APP_EventsBindSystem(void)` | Conecta el sistema usado por los gestores de eventos. | `f4:Core/Inc/eventManagers.h:23`; editorial |
| <a id="fn-app_getlinestate"></a>`void APP_GetLineState(uint8_t payload[APP_LINE_STATE_BYTES], uint8_t request_reason)` | Serializa el estado conjunto de calibración y seguimiento para reportes de aplicación. | `f4:Core/Inc/app_tasks.h:36`; editorial |
| <a id="fn-app_getwifiapfallbacksyncstate"></a>`uint8_t APP_GetWifiApFallbackSyncState(void)` | Consulta el estado de sincronización de la política AP fallback. | `f4:Core/Inc/app_tasks.h:46`; editorial |
| <a id="fn-app_init"></a>`void APP_Init(void)` | Construye el sistema y conecta instancias, servicios, periféricos y estado inicial. | `f4:Core/Inc/app_tasks.h:41`; editorial |
| <a id="fn-app_isirlightsenabled"></a>`bool APP_IsIrLightsEnabled(void)` | Consulta habilitación lógica de luces IR en el estado de aplicación. | `f4:Core/Inc/app_tasks.h:65`; editorial |
| <a id="fn-app_isirlineenabled"></a>`bool APP_IsIrLineEnabled(void)` | Consulta habilitación de los sensores IR de línea. | `f4:Core/Inc/app_tasks.h:64`; editorial |
| <a id="fn-app_isirobjectsenabled"></a>`bool APP_IsIrObjectsEnabled(void)` | Consulta habilitación de los sensores IR de objetos. | `f4:Core/Inc/app_tasks.h:63`; editorial |
| <a id="fn-app_linecalibrationbusy"></a>`bool APP_LineCalibrationBusy(void)` | Adapta la llamada a `LineCalibration_Busy` con el contexto de esta integración. | `f4:Core/Inc/app_tasks.h:30`; structural |
| <a id="fn-app_linecalibrationcontrol"></a>`uint8_t APP_LineCalibrationControl(uint8_t operation, uint32_t session)` | Coordina `APP_AutoMotionCancel`, `APP_LineCalibrationBusy`, `APP_LineFollowStop` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:32`; structural |
| <a id="fn-app_linefollowenabled"></a>`bool APP_LineFollowEnabled(void)` | Consulta si seguimiento de línea está habilitado en la aplicación. | `f4:Core/Inc/app_tasks.h:35`; editorial |
| <a id="fn-app_linefollowsetenabled"></a>`uint8_t APP_LineFollowSetEnabled(bool enabled)` | Coordina `APP_LineCalibrationBusy`, `APP_LineFollowEnabled`, `APP_LineFollowStop` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:33`; structural |
| <a id="fn-app_linefollowstop"></a>`void APP_LineFollowStop(void)` | Detiene seguimiento y su producción de órdenes. | `f4:Core/Inc/app_tasks.h:34`; editorial |
| <a id="fn-app_loop"></a>`void APP_Loop(void)` | Coordina `APP_ESPAlive_Task`, `APP_ESPRebootAfterStmStartup_Task`, `APP_LineServiceTask` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:42`; structural |
| <a id="fn-app_mpu_canrecalibrate"></a>`uint8_t APP_MPU_CanRecalibrate(void)` | Adapta la llamada a `APP_MPU_IsCalibrationRunning` con el contexto de esta integración. | `f4:Core/Inc/app_tasks.h:75`; structural |
| <a id="fn-app_mpu_getcalibrationprogresspercent"></a>`uint8_t APP_MPU_GetCalibrationProgressPercent(void)` | Adapta la llamada a `MPU9250_GetCalibrationProgressPercent` con el contexto de esta integración. | `f4:Core/Inc/app_tasks.h:74`; structural |
| <a id="fn-app_mpu_iscalibrationdonenoticeactive"></a>`uint8_t APP_MPU_IsCalibrationDoneNoticeActive(void)` | Adapta la llamada a `HAL_GetTick` con el contexto de esta integración. | `f4:Core/Inc/app_tasks.h:76`; structural |
| <a id="fn-app_mpu_iscalibrationpromptactive"></a>`uint8_t APP_MPU_IsCalibrationPromptActive(void)` | Operación MPU Is Calibration Prompt Active de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/app_tasks.h:72`; inferred-name |
| <a id="fn-app_mpu_iscalibrationrunning"></a>`uint8_t APP_MPU_IsCalibrationRunning(void)` | Adapta la llamada a `MPU9250_IsCalibrating` con el contexto de esta integración. | `f4:Core/Inc/app_tasks.h:73`; structural |
| <a id="fn-app_mpu_requestcalibration"></a>`void APP_MPU_RequestCalibration(void)` | Coordina `APP_AutoMotionCancel`, `APP_LineCalibrationBusy`, `APP_LineFollowStop` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:59`; structural |
| <a id="fn-app_motortestisactive"></a>`uint8_t APP_MotorTestIsActive(void)` | Operación Motor Test Is Active de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/app_tasks.h:55`; inferred-name |
| <a id="fn-app_motortestset"></a>`AM_Result APP_MotorTestSet(float left, float right, uint16_t duration_ms)` | Coordina `APP_LineCalibrationBusy`, `GET_CAR_MODE`, `HAL_GetTick` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:56`; structural |
| <a id="fn-app_motorteststop"></a>`void APP_MotorTestStop(void)` | Detiene prueba directa de motores. | `f4:Core/Inc/app_tasks.h:57`; editorial |
| <a id="fn-app_readirroutesnapshot"></a>`bool APP_ReadIrRouteSnapshot(uint16_t raw[8],TCRT5000_RouteSample_t *out)` | Lee Ir Route Snapshot dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:38`; inferred-name |
| <a id="fn-app_setirlightsenabled"></a>`void APP_SetIrLightsEnabled(bool enabled)` | Configura Ir Lights Enabled dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:68`; inferred-name |
| <a id="fn-app_setirlineenabled"></a>`void APP_SetIrLineEnabled(bool enabled)` | Configura Ir Line Enabled dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:67`; inferred-name |
| <a id="fn-app_setirobjectsenabled"></a>`void APP_SetIrObjectsEnabled(bool enabled)` | Configura Ir Objects Enabled dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:66`; inferred-name |
| <a id="fn-app_transitioncarmode"></a>`bool APP_TransitionCarMode(CarMode_t mode)` | Coordina `APP_ApplyIrState`, `APP_AutoMotionCancel`, `APP_LineCalibrationBusy` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:70`; structural |
| <a id="fn-app_usb_getalivecount"></a>`uint32_t APP_USB_GetAliveCount(void)` | Operación USB Get Alive Count de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/app_tasks.h:71`; inferred-name |
| <a id="fn-about_usereventmanager"></a>`void About_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:33`; structural |
| <a id="fn-balancepid_beginkdedit"></a>`void BalancePid_BeginKdEdit(void)` | Adapta la llamada a `BalancePid_BeginGainEdit` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:124`; structural |
| <a id="fn-balancepid_beginkiedit"></a>`void BalancePid_BeginKiEdit(void)` | Adapta la llamada a `BalancePid_BeginGainEdit` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:123`; structural |
| <a id="fn-balancepid_beginkpedit"></a>`void BalancePid_BeginKpEdit(void)` | Adapta la llamada a `BalancePid_BeginGainEdit` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:122`; structural |
| <a id="fn-balancepid_beginlogconfig"></a>`void BalancePid_BeginLogConfig(void)` | Operación Balance Pid Begin Log Config de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:125`; inferred-name |
| <a id="fn-balancepid_getcurrentgaindraft"></a>`float BalancePid_GetCurrentGainDraft(void)` | Operación Balance Pid Get Current Gain Draft de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:130`; inferred-name |
| <a id="fn-balancepid_getcurrentgainlabel"></a>`const char *BalancePid_GetCurrentGainLabel(void)` | Operación Balance Pid Get Current Gain Label de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:129`; inferred-name |
| <a id="fn-balancepid_getcurrentgainscreencode"></a>`ScreenCode_t BalancePid_GetCurrentGainScreenCode(void)` | Operación Balance Pid Get Current Gain Screen Code de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:128`; inferred-name |
| <a id="fn-balancepid_getlogavailablebytes"></a>`uint32_t BalancePid_GetLogAvailableBytes(void)` | Adapta la llamada a `volatile` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:132`; structural |
| <a id="fn-balancepid_getlogdurationsec"></a>`uint16_t BalancePid_GetLogDurationSec(void)` | Operación Balance Pid Get Log Duration Sec de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:131`; inferred-name |
| <a id="fn-balancepid_getlogestimatedbytes"></a>`uint32_t BalancePid_GetLogEstimatedBytes(void)` | Operación Balance Pid Get Log Estimated Bytes de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:133`; inferred-name |
| <a id="fn-bootstrestartconfirm_actionbegin"></a>`void BootSTRestartConfirm_ActionBegin(void)` | Adapta la llamada a `GenericConfirm_Begin` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:79`; structural |
| <a id="fn-bootv2restartconfirm_actionbegin"></a>`void BootV2RestartConfirm_ActionBegin(void)` | Adapta la llamada a `GenericConfirm_Begin` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:78`; structural |
| <a id="fn-clickcancelar_usereventmanager"></a>`void ClickCancelar_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:59`; structural |
| <a id="fn-dashboard_getactiontimeoutcontext"></a>`uint8_t Dashboard_GetActionTimeoutContext(uint32_t *startedMs, uint32_t *timeoutMs)` | Coordina `Dashboard_GetActionTimeoutMs`, `IS_FLAG_SET` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:62`; structural |
| <a id="fn-dashboard_modeselectiontick"></a>`void Dashboard_ModeSelectionTick(void)` | Coordina `Dashboard_BackToDashboard`, `Dashboard_CancelPendingMode`, `Dashboard_GetActionTimeoutMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:61`; structural |
| <a id="fn-dashboard_resetmodeconfirmstate"></a>`void Dashboard_ResetModeConfirmState(void)` | Adapta la llamada a `CLEAR_FLAG` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:60`; structural |
| <a id="fn-encoder_maintask"></a>`void Encoder_MainTask(ENC_Handle_t *enc)` | Coordina `CLEAR_FLAG`, `ENC_GetScaledSteps`, `IS_FLAG_SET` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:52`; structural |
| <a id="fn-genericconfirmeventmanager"></a>`void GenericConfirmEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:71`; structural |
| <a id="fn-genericconfirm_begin"></a>`void GenericConfirm_Begin(const char* line1, const char* line2, GenericConfirmAction_t onConfirm, GenericConfirmCancel_t onCancel, uint32_t timeoutMs, uint32_t screenCode)` | Coordina `HAL_GetTick` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:66`; structural |
| <a id="fn-genericconfirm_getscreencode"></a>`uint32_t GenericConfirm_GetScreenCode(void)` | Devuelve el campo `genericConfirmCtx.screenCode` del estado actual de la instancia. | `f4:Core/Inc/eventManagers.h:76`; structural |
| <a id="fn-genericconfirm_gettimeoutcontext"></a>`uint8_t GenericConfirm_GetTimeoutContext(uint32_t *startedMs, uint32_t *timeoutMs)` | Operación Generic Confirm Get Timeout Context de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:73`; inferred-name |
| <a id="fn-genericconfirm_tick"></a>`void GenericConfirm_Tick(void)` | Coordina `GenericConfirm_Cancel`, `HAL_GetTick` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:72`; structural |
| <a id="fn-genericeventmanager"></a>`void GenericEventManager(UserEvent_t ev, const EventCallbacks_t *callbacks)` | Despacha evento de usuario al flujo global activo. | `f4:Core/Inc/eventManagers.h:26`; editorial |
| <a id="fn-hal_adc_convcpltcallback"></a>`void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *adc)` | Adapta la llamada a `APP_AdcPublish` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:278`; structural |
| <a id="fn-hal_adc_convhalfcpltcallback"></a>`void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *adc)` | Adapta la llamada a `APP_AdcPublish` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:277`; structural |
| <a id="fn-hal_i2c_errorcallback"></a>`void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)` | Adapta la llamada a `I2C_Manager_OnError` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1529`; structural |
| <a id="fn-hal_i2c_masterrxcpltcallback"></a>`void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)` | Adapta la llamada a `I2C_Manager_OnRxCplt` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1522`; structural |
| <a id="fn-hal_i2c_mastertxcpltcallback"></a>`void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)` | Adapta la llamada a `I2C_Manager_OnTxCplt` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1508`; structural |
| <a id="fn-hal_i2c_memrxcpltcallback"></a>`void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)` | Adapta la llamada a `I2C_Manager_OnRxCplt` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1515`; structural |
| <a id="fn-hal_i2c_memtxcpltcallback"></a>`void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)` | Adapta la llamada a `I2C_Manager_OnTxCplt` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1501`; structural |
| <a id="fn-hal_tim_periodelapsedcallback"></a>`void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)` | Coordina `ENC_Task_N10ms`, `IS_FLAG_SET`, `UserButton_Task10ms` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1467`; structural |
| <a id="fn-hal_uart_errorcallback"></a>`void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)` | Coordina `WifiBridge_OnError`, `WifiBridge_Start` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1557`; structural |
| <a id="fn-hal_uart_rxcpltcallback"></a>`void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)` | Adapta la llamada a `WifiBridge_OnRxComplete` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1550`; structural |
| <a id="fn-hal_uart_rxhalfcpltcallback"></a>`void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart)` | Adapta la llamada a `WifiBridge_OnRxHalfComplete` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1543`; structural |
| <a id="fn-hal_uart_txcpltcallback"></a>`void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)` | Adapta la llamada a `WifiBridge_OnTxComplete` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1536`; structural |
| <a id="fn-irvalues_usereventmanager"></a>`void IRValues_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:42`; structural |
| <a id="fn-itemeventmanager"></a>`void ItemEventManager(UserEvent_t ev, RenderWrapperFn wrapper)` | Despacha los eventos de usuario de Item Event Manager. | `f4:Core/Inc/eventManagers.h:136`; inferred-name |
| <a id="fn-mpucalibration_requestfrommenu"></a>`void MPUCalibration_RequestFromMenu(void)` | Adapta la llamada a `APP_MPU_RequestCalibration` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:110`; structural |
| <a id="fn-mpuvalues_usereventmanager"></a>`void MPUValues_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:44`; structural |
| <a id="fn-menuitem_isscreenlocktimeoutvisible"></a>`bool MenuItem_IsScreenLockTimeoutVisible(void)` | Adapta la llamada a `AppPreferences_IsScreenLockEnabled` con el contexto de esta integración. | `f4:Core/Inc/menu_definitions.h:74`; structural |
| <a id="fn-menuitem_istestmodevisible"></a>`bool MenuItem_IsTestModeVisible(void)` | Adapta la llamada a `GET_CAR_MODE` con el contexto de esta integración. | `f4:Core/Inc/menu_definitions.h:73`; structural |
| <a id="fn-motorsingletest_beginleft"></a>`void MotorSingleTest_BeginLeft(void)` | Coordina `MotorSingleTest_Apply`, `MotorSingleTest_SaveState` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:112`; structural |
| <a id="fn-motorsingletest_beginright"></a>`void MotorSingleTest_BeginRight(void)` | Coordina `MotorSingleTest_Apply`, `MotorSingleTest_SaveState` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:113`; structural |
| <a id="fn-motorsingletest_getdirectionlabel"></a>`const char *MotorSingleTest_GetDirectionLabel(void)` | Operación Motor Single Test Get Direction Label de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:117`; inferred-name |
| <a id="fn-motorsingletest_getlabel"></a>`const char *MotorSingleTest_GetLabel(void)` | Operación Motor Single Test Get Label de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:116`; inferred-name |
| <a id="fn-motorsingletest_getmaxpwm"></a>`uint16_t MotorSingleTest_GetMaxPwm(void)` | Devuelve el campo `motor->max_pwm` del estado actual de la instancia. | `f4:Core/Inc/eventManagers.h:121`; structural |
| <a id="fn-motorsingletest_getminpwm"></a>`uint16_t MotorSingleTest_GetMinPwm(void)` | Operación Motor Single Test Get Min Pwm de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:120`; inferred-name |
| <a id="fn-motorsingletest_getpwm"></a>`uint16_t MotorSingleTest_GetPwm(void)` | Adapta la llamada a `Motor_SpeedPercentToPwm` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:119`; structural |
| <a id="fn-motorsingletest_getscreencode"></a>`ScreenCode_t MotorSingleTest_GetScreenCode(void)` | Operación Motor Single Test Get Screen Code de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:115`; inferred-name |
| <a id="fn-motorsingletest_getspeedpercentdisplay"></a>`uint8_t MotorSingleTest_GetSpeedPercentDisplay(void)` | Adapta la llamada a `Motor_SpeedPercentToDisplay` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:118`; structural |
| <a id="fn-motor_maintask"></a>`void Motor_MainTask(void)` | Coordina `APP_LineFollowEnabled`, `APP_MotorTestStop`, `AutoMotion_Task` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:54`; structural |
| <a id="fn-notificationdismiss_onlongpress"></a>`void NotificationDismiss_OnLongPress(void)` | Adapta la llamada a `OledUtils_DismissNotification` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:58`; structural |
| <a id="fn-notificationdismiss_onshortpress"></a>`void NotificationDismiss_OnShortPress(void)` | Adapta la llamada a `OledUtils_DismissNotification` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:57`; structural |
| <a id="fn-oled_maintask"></a>`void OLED_MainTask(void)` | Coordina `CLEAR_FLAG`, `HAL_GetTick`, `IS_FLAG_SET` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:58`; structural |
| <a id="fn-preferences_beginespnotifwatchdoglimitedit"></a>`void Preferences_BeginEspNotifWatchdogLimitEdit(void *param)` | Operación Preferences Begin Esp Notif Watchdog Limit Edit de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Inc/eventManagers.h:90`; inferred-name |
| <a id="fn-preferences_beginlineblackwidthedit"></a>`void Preferences_BeginLineBlackWidthEdit(void)` | Adapta la llamada a `AppPreferences_GetLineBlackWidthMm` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:96`; structural |
| <a id="fn-preferences_beginmotorleftminpwmedit"></a>`void Preferences_BeginMotorLeftMinPwmEdit(void)` | Coordina `AppPreferences_GetMotorLeftMinPwm`, `MotorDrive_SetLeftMinPwm`, `MotorMinPwm_ApplyLiveTest` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:105`; structural |
| <a id="fn-preferences_beginmotorrightminpwmedit"></a>`void Preferences_BeginMotorRightMinPwmEdit(void)` | Coordina `AppPreferences_GetMotorRightMinPwm`, `MotorDrive_SetRightMinPwm`, `MotorMinPwm_ApplyLiveTest` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:106`; structural |
| <a id="fn-preferences_beginoledrotationmputest"></a>`void Preferences_BeginOledRotationMpuTest(void)` | Coordina `AppPreferences_GetOledRotationDeg`, `MPU9250_AttitudeResetYaw`, `OledUtils_SetRotationAndRefresh` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:100`; structural |
| <a id="fn-preferences_beginoledrotationtest"></a>`void Preferences_BeginOledRotationTest(void)` | Coordina `AppPreferences_GetOledRotationDeg`, `Preferences_ApplyOledRotationDraft` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:98`; structural |
| <a id="fn-preferences_beginscreenlocktimeoutedit"></a>`void Preferences_BeginScreenLockTimeoutEdit(void)` | Adapta la llamada a `ScreenLock_BeginTimeoutEdit` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:91`; structural |
| <a id="fn-preferences_beginsystemactiontimeoutedit"></a>`void Preferences_BeginSystemActionTimeoutEdit(void)` | Adapta la llamada a `AppPreferences_GetSystemActionTimeoutSec` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:92`; structural |
| <a id="fn-preferences_beginusbaliveperiodedit"></a>`void Preferences_BeginUsbAlivePeriodEdit(void)` | Adapta la llamada a `AppPreferences_GetUsbAlivePeriodMs` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:94`; structural |
| <a id="fn-preferences_getlineblackwidthdraftmm"></a>`uint16_t Preferences_GetLineBlackWidthDraftMm(void)` | Operación Preferences Get Line Black Width Draft Mm de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:97`; inferred-name |
| <a id="fn-preferences_getmotorminpwmdraft"></a>`uint16_t Preferences_GetMotorMinPwmDraft(void)` | Operación Preferences Get Motor Min Pwm Draft de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:107`; inferred-name |
| <a id="fn-preferences_getmotorminpwmeditlabel"></a>`const char *Preferences_GetMotorMinPwmEditLabel(void)` | Operación Preferences Get Motor Min Pwm Edit Label de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:108`; inferred-name |
| <a id="fn-preferences_getmotorminpwmeditscreencode"></a>`ScreenCode_t Preferences_GetMotorMinPwmEditScreenCode(void)` | Operación Preferences Get Motor Min Pwm Edit Screen Code de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:109`; inferred-name |
| <a id="fn-preferences_getoledrotationdraftdeg"></a>`uint16_t Preferences_GetOledRotationDraftDeg(void)` | Operación Preferences Get Oled Rotation Draft Deg de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:99`; inferred-name |
| <a id="fn-preferences_getoledrotationmputestdeg"></a>`uint16_t Preferences_GetOledRotationMpuTestDeg(void)` | Operación Preferences Get Oled Rotation Mpu Test Deg de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:102`; inferred-name |
| <a id="fn-preferences_getoledrotationmputestyawdeg"></a>`int16_t Preferences_GetOledRotationMpuTestYawDeg(void)` | Operación Preferences Get Oled Rotation Mpu Test Yaw Deg de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:103`; inferred-name |
| <a id="fn-preferences_getsystemactiontimeoutdraft"></a>`uint16_t Preferences_GetSystemActionTimeoutDraft(void)` | Operación Preferences Get System Action Timeout Draft de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:93`; inferred-name |
| <a id="fn-preferences_getusbaliveperioddraftms"></a>`uint16_t Preferences_GetUsbAlivePeriodDraftMs(void)` | Operación Preferences Get Usb Alive Period Draft Ms de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:95`; inferred-name |
| <a id="fn-preferences_isoledrotationmputestready"></a>`uint8_t Preferences_IsOledRotationMpuTestReady(void)` | Operación Preferences Is Oled Rotation Mpu Test Ready de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:104`; inferred-name |
| <a id="fn-preferences_toggleencoderinvert"></a>`void Preferences_ToggleEncoderInvert(void)` | Coordina `AppPreferences_IsEncoderInverted`, `AppPreferences_SetEncoderInverted`, `ENC_SetDirectionInverted` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:80`; structural |
| <a id="fn-preferences_toggleespnotifwatchdog"></a>`void Preferences_ToggleEspNotifWatchdog(void *param)` | Coordina `AppPreferences_ApplyPatch`, `AppPreferences_IsEspNotifWatchdogEnabled` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:81`; structural |
| <a id="fn-preferences_toggleirlights"></a>`void Preferences_ToggleIrLights(void)` | Coordina `APP_IsIrLightsEnabled`, `APP_SetIrLightsEnabled`, `OledUtils_ShowSimpleNotificationMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:89`; structural |
| <a id="fn-preferences_toggleirline"></a>`void Preferences_ToggleIrLine(void)` | Coordina `APP_IsIrLineEnabled`, `APP_SetIrLineEnabled`, `OledUtils_ShowSimpleNotificationMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:88`; structural |
| <a id="fn-preferences_toggleirobjects"></a>`void Preferences_ToggleIrObjects(void)` | Coordina `APP_IsIrObjectsEnabled`, `APP_SetIrObjectsEnabled`, `OledUtils_ShowSimpleNotificationMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:87`; structural |
| <a id="fn-preferences_toggleoledautorotation"></a>`void Preferences_ToggleOledAutoRotation(void)` | Coordina `APP_ApplyPreferenceRuntimeChanges`, `AppPreferences_IsOledAutoRotationEnabled`, `AppPreferences_SetOledAutoRotationEnabled` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:83`; structural |
| <a id="fn-preferences_togglepinlock"></a>`void Preferences_TogglePinLock(void)` | Coordina `APP_ApplyPreferenceRuntimeChanges`, `AppPreferences_IsPinLockEnabled`, `AppPreferences_SetPinLockEnabled` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:85`; structural |
| <a id="fn-preferences_togglescreenlock"></a>`void Preferences_ToggleScreenLock(void)` | Coordina `AppPreferences_IsScreenLockEnabled`, `AppPreferences_SetScreenLockEnabled`, `ScreenLock_OnPreferencesChanged` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:82`; structural |
| <a id="fn-preferences_toggleunerrouterforwarding"></a>`void Preferences_ToggleUnerRouterForwarding(void)` | Coordina `APP_ApplyPreferenceRuntimeChanges`, `AppPreferences_IsUnerRouterForwardingEnabled`, `AppPreferences_SetUnerRouterForwardingEnabled` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:84`; structural |
| <a id="fn-preferences_togglewifiapfallback"></a>`void Preferences_ToggleWifiApFallback(void)` | Coordina `APP_ApplyPreferenceRuntimeChanges`, `AppPreferences_IsWifiApFallbackEnabled`, `AppPreferences_SetWifiApFallbackEnabled` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:86`; structural |
| <a id="fn-preferences_updateoledrotationfrommputest"></a>`void Preferences_UpdateOledRotationFromMpuTest(void)` | Coordina `MPU9250_GetAttitude`, `MPU9250_GetNormalizedData`, `OledUtils_SetRotationAndRefresh` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/eventManagers.h:101`; structural |
| <a id="fn-readonly_onlongpress"></a>`void ReadOnly_OnLongPress(void)` | Adapta la llamada a `clearScreen` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1503`; structural |
| <a id="fn-readonly_usereventmanager"></a>`void ReadOnly_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:41`; structural |
| <a id="fn-routealignment_usereventmanager"></a>`void RouteAlignment_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:43`; structural |
| <a id="fn-unerroutertest_usereventmanager"></a>`void UnerRouterTest_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:45`; structural |
| <a id="fn-userbtn_maintask"></a>`void UserBtn_MainTask(UserButton_Handle_t *btn)` | Coordina `CLEAR_FLAG`, `IS_FLAG_SET`, `UiEventRouter_HandleEvent` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:51`; structural |
| <a id="fn-wifiatpassword_getlength"></a>`uint8_t WiFiAtPassword_GetLength(void)` | Operación Wi Fi At Password Get Length de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:39`; inferred-name |
| <a id="fn-wifiatpassword_getselectedcharacter"></a>`char WiFiAtPassword_GetSelectedCharacter(void)` | Operación Wi Fi At Password Get Selected Character de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Inc/eventManagers.h:40`; inferred-name |
| <a id="fn-wifiatpassword_usereventmanager"></a>`void WiFiAtPassword_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:38`; structural |
| <a id="fn-wifidetails_usereventmanager"></a>`void WiFiDetails_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:37`; structural |
| <a id="fn-wifisearch_usereventmanager"></a>`void WiFiSearch_UserEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:35`; structural |
| <a id="fn-balancepidgaineventmanager"></a>`void balancePidGainEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:126`; structural |
| <a id="fn-balancepidlogeventmanager"></a>`void balancePidLogEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:127`; structural |
| <a id="fn-balancepidstatuseventmanager"></a>`void balancePidStatusEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:48`; structural |
| <a id="fn-dashboardeventmanager"></a>`void dashboardEventManager(UserEvent_t ev)` | Despacha los eventos de usuario de dashboard Event Manager. | `f4:Core/Inc/eventManagers.h:29`; inferred-name |
| <a id="fn-dashboardquickactioneventmanager"></a>`void dashboardQuickActionEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:30`; structural |
| <a id="fn-espnotifwatchdoglimiteventmanager"></a>`void espNotifWatchdogLimitEventManager(UserEvent_t event)` | Despacha los eventos de usuario de esp Notif Watchdog Limit Event Manager. | `f4:Core/Inc/eventManagers.h:47`; inferred-name |
| <a id="fn-i2cmanager_maintask"></a>`void i2cManager_MainTask(void)` | Adapta la llamada a `I2C_Manager_Tick` con el contexto de esta integración. | `f4:Core/Inc/app_tasks.h:53`; structural |
| <a id="fn-initcarmode"></a>`void initCarMode(void)` | Coordina `AppPreferences_IsEncoderInverted`, `ENC_Init`, `ENC_SetDirectionInverted` para Composición, tareas y acciones de aplicación. | `f4:Core/Inc/app_tasks.h:50`; structural |
| <a id="fn-lineblackwidtheventmanager"></a>`void lineBlackWidthEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:52`; structural |
| <a id="fn-menueventmanager"></a>`void menuEventManager(UserEvent_t ev)` | Despacha los eventos de usuario de menu Event Manager. | `f4:Core/Inc/eventManagers.h:32`; inferred-name |
| <a id="fn-motorminpwmeventmanager"></a>`void motorMinPwmEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:53`; structural |
| <a id="fn-motorsingletesteventmanager"></a>`void motorSingleTestEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:114`; structural |
| <a id="fn-motortesteventmanager"></a>`void motorTestEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:34`; structural |
| <a id="fn-mpucalibrationeventmanager"></a>`void mpuCalibrationEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:111`; structural |
| <a id="fn-oledcanvaseventmanager"></a>`void oledCanvasEventManager(UserEvent_t ev)` | Adapta la llamada a `OledCanvas_HandleLocalEvent` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:56`; structural |
| <a id="fn-oledrotationmputesteventmanager"></a>`void oledRotationMpuTestEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:55`; structural |
| <a id="fn-oledrotationtesteventmanager"></a>`void oledRotationTestEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:54`; structural |
| <a id="fn-screenlocktimeouteventmanager"></a>`void screenLockTimeoutEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:49`; structural |
| <a id="fn-screenlockedeventmanager"></a>`void screenLockedEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:46`; structural |
| <a id="fn-stabilizationprompteventmanager"></a>`void stabilizationPromptEventManager(UserEvent_t ev)` | Adapta la llamada a `dashboardQuickActionEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:31`; structural |
| <a id="fn-systemactiontimeouteventmanager"></a>`void systemActionTimeoutEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:50`; structural |
| <a id="fn-usbaliveperiodeventmanager"></a>`void usbAlivePeriodEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:51`; structural |
| <a id="fn-volatile"></a>`__asm__ volatile ("mrs %0, msp" : "=r" (sp))` | Operación volatile de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Src/eventManagers.c:2567`; inferred-name |
| <a id="fn-wifieventmanager"></a>`void wifiEventManager(UserEvent_t ev)` | Adapta la llamada a `GenericEventManager` con el contexto de esta integración. | `f4:Core/Inc/eventManagers.h:36`; structural |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-app_oledrotationfromdeg"></a>`static OledRotation_t APP_OledRotationFromDeg(uint16_t rotationDeg)` | Adapta la llamada a `AppPreferences_ClampOledRotationDeg` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:245`; structural |
| <a id="fn-app_getpreferenceoledrotation"></a>`static OledRotation_t APP_GetPreferenceOledRotation(void)` | Consulta Preference Oled Rotation dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:253`; inferred-name |
| <a id="fn-app_adcpublish"></a>`static void APP_AdcPublish(unsigned offset)` | Coordina `HAL_GetTick`, `LineCalibration_PushScan` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:270`; structural |
| <a id="fn-app_copyadc"></a>`static void APP_CopyAdc(uint16_t raw[8],uint32_t *seq,uint32_t *ms)` | Coordina `__disable_irq`, `__enable_irq`, `__get_PRIMASK` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:279`; structural |
| <a id="fn-app_linepose"></a>`static LineCalibration_Pose APP_LinePose(void)` | Coordina `App_MpuReady`, `HAL_GetTick`, `MPU9250_GetFrontTiltDeg` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:307`; structural |
| <a id="fn-app_linereadsnapshot"></a>`static bool APP_LineReadSnapshot(void *ctx,LineFollow_Input *in,uint16_t snapshot[8])` | Coordina `APP_CopyAdc`, `APP_LineCalibrationBusy`, `APP_LinePose` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:318`; structural |
| <a id="fn-app_lineread"></a>`static bool APP_LineRead(void *ctx,LineFollow_Input *in)` | Adapta la llamada a `APP_LineReadSnapshot` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:346`; structural |
| <a id="fn-app_linecommand"></a>`static bool APP_LineCommand(void *ctx,float forward,float turn,uint16_t lease,uint32_t now)` | Coordina `MotionControl_GetDefaultCommand`, `MotionControl_SetCommand` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:347`; structural |
| <a id="fn-app_linehold"></a>`static void APP_LineHold(void *ctx)` | Adapta la llamada a `MotionControl_Stop` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:354`; structural |
| <a id="fn-app_callights"></a>`static void APP_CalLights(void *ctx,bool ir,bool white)` | Coordina `HAL_GPIO_WritePin`, `TCRT5000_EmittersGroup3Set` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:355`; structural |
| <a id="fn-app_calrestore"></a>`static void APP_CalRestore(void *ctx)` | Coordina `HAL_GPIO_WritePin`, `HAL_GetTick`, `TCRT5000_EmittersGroup3Set` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:360`; structural |
| <a id="fn-app_calsave"></a>`static bool APP_CalSave(void *ctx,const LineCalibration_Store *store)` | Coordina `APP_MotorTestIsActive`, `AppPreferences_SetLineCalibration`, `LineCalibration_Ready` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:367`; structural |
| <a id="fn-app_lineservicetask"></a>`static void APP_LineServiceTask(uint32_t now)` | Coordina `APP_LineCalibrationBusy`, `APP_LineFollowStop`, `APP_LinePose` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:426`; structural |
| <a id="fn-app_line_u32"></a>`static void app_line_u32(uint8_t *p,uint32_t n)` | Operación app line u32 de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/app_tasks.c:452`; inferred-name |
| <a id="fn-app_line_f32"></a>`static void app_line_f32(uint8_t *p,float f)` | Adapta la llamada a `app_line_u32` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:453`; structural |
| <a id="fn-app_autoread"></a>`static bool APP_AutoRead(void *ctx,AutoMotion_Input *out)` | Coordina `APP_CopyAdc`, `APP_IsIrLightsEnabled`, `APP_LinePose` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:486`; structural |
| <a id="fn-app_autocommand"></a>`static bool APP_AutoCommand(void *ctx,float forward,float turn,uint32_t now)` | Coordina `MotionControl_GetDefaultCommand`, `MotionControl_SetCommand` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:508`; structural |
| <a id="fn-app_autosave"></a>`static bool APP_AutoSave(void *ctx,const AutoMotion_Profile *profile)` | Coordina `APP_LineCalibrationBusy`, `APP_MotorTestIsActive`, `AppPreferences_SetAutoMotionProfile` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:513`; structural |
| <a id="fn-app_clearstaip"></a>`static void APP_ClearStaIp(void)` | Limpia Sta Ip dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:707`; inferred-name |
| <a id="fn-app_clearapip"></a>`static void APP_ClearApIp(void)` | Limpia Ap Ip dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:720`; inferred-name |
| <a id="fn-app_modefromconnectivityflags"></a>`static uint8_t APP_ModeFromConnectivityFlags(void)` | Coordina `IS_FLAG_SET` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:733`; structural |
| <a id="fn-app_isbootnotificationshowing"></a>`static bool APP_IsBootNotificationShowing(void)` | Adapta la llamada a `OledUtils_IsSimpleNotificationShowing` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:748`; structural |
| <a id="fn-app_showconnectivitysimplenotification"></a>`static void APP_ShowConnectivitySimpleNotification(OledSimpleNotificationId notificationId, uint16_t timeoutMs)` | Coordina `APP_IsBootNotificationShowing`, `Dashboard_InvalidateConnectivity`, `HAL_GetTick` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:755`; structural |
| <a id="fn-app_updateatconnectivityflags"></a>`static void APP_UpdateAtConnectivityFlags(void)` | Actualiza At Connectivity Flags dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:780`; inferred-name |
| <a id="fn-app_finishatwificonnectionifready"></a>`static void APP_FinishAtWifiConnectionIfReady(void)` | Coordina `OledUtils_IsNotificationShowing`, `OledUtils_SetWiFiConnectedSsid`, `OledUtils_ShowNotificationMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:800`; structural |
| <a id="fn-app_wifibridgeeventcallback"></a>`static void APP_WifiBridgeEventCallback(const WifiBridge_Event *evt)` | Coordina `APP_ClearApIp`, `APP_ClearStaIp`, `APP_FinishAtWifiConnectionIfReady` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:818`; structural |
| <a id="fn-app_wifiapfallbacksync_task"></a>`static void APP_WifiApFallbackSync_Task(void)` | Coordina `AppPreferences_IsWifiApFallbackEnabled`, `HAL_GetTick`, `UNER_App_IsMcuResetPending` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1030`; structural |
| <a id="fn-app_wificonnectrequest_task"></a>`static void APP_WifiConnectRequest_Task(void)` | Coordina `AppPreferences_IsWifiApFallbackEnabled`, `OledUtils_SetWiFiCredentialsWebResultStatus`, `OledUtils_ShowNotificationMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1054`; structural |
| <a id="fn-app_wifistatusrefresh_task"></a>`static void APP_WifiStatusRefresh_Task(void)` | Coordina `WifiBridge_GetActiveBackend`, `WifiBridge_GetStatus`, `WifiBridge_IsWaitingValidation` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1087`; structural |
| <a id="fn-app_wififirmwarerequest_task"></a>`static void APP_WifiFirmwareRequest_Task(void)` | Coordina `WifiBridge_GetActiveBackend`, `WifiBridge_IsWaitingValidation`, `WifiBridge_SendCommand` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1108`; structural |
| <a id="fn-app_initmenusystem"></a>`static void APP_InitMenuSystem(void)` | Inicializa Menu System dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1565`; inferred-name |
| <a id="fn-app_initmotordrive"></a>`static void APP_InitMotorDrive(void)` | Inicializa Motor Drive dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1586`; inferred-name |
| <a id="fn-app_initbalancemotion"></a>`static void APP_InitBalanceMotion(void)` | Inicializa Balance Motion dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1616`; inferred-name |
| <a id="fn-app_initmpu"></a>`static void APP_InitMPU(void)` | Inicializa MPU dentro de Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1668`; inferred-name |
| <a id="fn-app_mpu_dataready"></a>`static void APP_MPU_DataReady(void)` | Adapta la llamada a `SET_FLAG` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:1703`; structural |
| <a id="fn-app_mpu_loadsavedcalibration"></a>`static uint8_t APP_MPU_LoadSavedCalibration(void)` | Coordina `AppPreferences_GetMpuCalibration`, `MPU9250_ApplyCalibrationData` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1708`; structural |
| <a id="fn-app_mpu_savecalibrationifpending"></a>`static void APP_MPU_SaveCalibrationIfPending(void)` | Coordina `AppPreferences_SetMpuCalibration`, `MPU9250_GetCalibrationData`, `MPU9250_IsCalibrating` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1729`; structural |
| <a id="fn-app_mpucalibration_task"></a>`static void APP_MPUCalibration_Task(void)` | Coordina `APP_MPU_SaveCalibrationIfPending`, `HAL_GetTick`, `MPU9250_IsCalibrating` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1818`; structural |
| <a id="fn-app_espalive_task"></a>`static void APP_ESPAlive_Task(void)` | Coordina `APP_ShowConnectivitySimpleNotification`, `AppPreferences_GetEspNotifWatchdogLimit`, `AppPreferences_GetWebAlivePeriodMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1859`; structural |
| <a id="fn-app_esprebootafterstmstartup_task"></a>`static void APP_ESPRebootAfterStmStartup_Task(void)` | Coordina `HAL_GetTick`, `UNER_App_IsMcuResetPending`, `UNER_App_SendEspReboot` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1940`; structural |
| <a id="fn-app_usb_cdc_task"></a>`static void APP_USB_CDC_Task(void)` | Coordina `AppPreferences_GetUsbAlivePeriodMs`, `CDC_IsDtrActive_FS`, `CLEAR_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:1973`; structural |
| <a id="fn-app_currentscreenrequiresremoteui"></a>`static uint8_t APP_CurrentScreenRequiresRemoteUi(void)` | Adapta la llamada a `MenuSys_GetCurrentScreenCode` con el contexto de esta integración. | `f4:Core/Src/app_tasks.c:2017`; structural |
| <a id="fn-app_returntowifimenuaftercapabilityloss"></a>`static void APP_ReturnToWifiMenuAfterCapabilityLoss(void)` | Coordina `MenuSys_SetCurrentMenuScreenCode`, `OledUtils_DisableContinuousRender`, `OledUtils_NotificationRestore` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:2024`; structural |
| <a id="fn-app_uicapabilityguardtask"></a>`static void APP_UICapabilityGuardTask(void)` | Coordina `APP_CurrentScreenRequiresRemoteUi`, `APP_ReturnToWifiMenuAfterCapabilityLoss`, `WifiBridge_HasCapability` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:2045`; structural |
| <a id="fn-app_ui10ms_task"></a>`static void APP_UI10ms_Task(void)` | Coordina `APP_OLED_Task10ms`, `ScreenLock_Task10ms`, `__disable_irq` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:2056`; structural |
| <a id="fn-app_absfloat"></a>`static float APP_AbsFloat(float value)` | Operación Abs Float de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/app_tasks.c:2072`; inferred-name |
| <a id="fn-app_oled_shouldskipautorotation"></a>`static uint8_t APP_OLED_ShouldSkipAutoRotation(void)` | Operación OLED Should Skip Auto Rotation de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/app_tasks.c:2077`; inferred-name |
| <a id="fn-app_oled_applyrotationdegifneeded"></a>`static void APP_OLED_ApplyRotationDegIfNeeded(uint16_t rotationDeg)` | Coordina `APP_OledRotationFromDeg`, `OledUtils_SetRotationAndRefresh`, `ssd1306_GetRotation` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:2088`; structural |
| <a id="fn-app_oled_autorotationtask10ms"></a>`static void APP_OLED_AutoRotationTask10ms(void)` | Coordina `APP_AbsFloat`, `APP_OLED_ApplyRotationDegIfNeeded`, `APP_OLED_ShouldSkipAutoRotation` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:2097`; structural |
| <a id="fn-app_oled_task10ms"></a>`static void APP_OLED_Task10ms(void)` | Coordina `APP_OLED_AutoRotationTask10ms`, `OledUtils_NotificationTick10ms`, `SET_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/app_tasks.c:2174`; structural |
| <a id="fn-dashboard_wrapquickaction"></a>`static uint8_t Dashboard_WrapQuickAction(int16_t delta)` | Operación Dashboard Wrap Quick Action de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:214`; inferred-name |
| <a id="fn-dashboard_wrapmode"></a>`static CarMode_t Dashboard_WrapMode(CarMode_t mode, int16_t delta)` | Operación Dashboard Wrap Mode de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:228`; inferred-name |
| <a id="fn-dashboard_getquickactionrender"></a>`static RenderFunction Dashboard_GetQuickActionRender(uint8_t action)` | Operación Dashboard Get Quick Action Render de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:242`; inferred-name |
| <a id="fn-dashboard_touchquickaction"></a>`static void Dashboard_TouchQuickAction(void)` | Adapta la llamada a `HAL_GetTick` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:256`; structural |
| <a id="fn-dashboard_openquickaction"></a>`static void Dashboard_OpenQuickAction(uint8_t action)` | Coordina `CLEAR_FLAG`, `Dashboard_GetQuickActionRender`, `Dashboard_TouchQuickAction` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:263`; structural |
| <a id="fn-dashboard_startmodeselection"></a>`static void Dashboard_StartModeSelection(void)` | Coordina `GET_CAR_MODE`, `HAL_GetTick`, `SET_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:285`; structural |
| <a id="fn-dashboard_applypendingmode"></a>`static void Dashboard_ApplyPendingMode(void *ctx)` | Coordina `APP_TransitionCarMode`, `CLEAR_FLAG`, `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:307`; structural |
| <a id="fn-dashboard_backtodashboard"></a>`static void Dashboard_BackToDashboard(void)` | Coordina `CLEAR_FLAG`, `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:322`; structural |
| <a id="fn-dashboard_openstabilizationprompt"></a>`static void Dashboard_OpenStabilizationPrompt(void)` | Adapta la llamada a `Dashboard_OpenQuickAction` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:343`; structural |
| <a id="fn-dashboard_touchmodeselection"></a>`static void Dashboard_TouchModeSelection(void)` | Adapta la llamada a `HAL_GetTick` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:348`; structural |
| <a id="fn-dashboard_getactiontimeoutms"></a>`static uint32_t Dashboard_GetActionTimeoutMs(void)` | Adapta la llamada a `AppPreferences_GetSystemActionTimeoutSec` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:354`; structural |
| <a id="fn-dashboard_cancelpendingmode"></a>`static void Dashboard_CancelPendingMode(void)` | Coordina `CLEAR_FLAG`, `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:359`; structural |
| <a id="fn-dashboard_onrotatecw"></a>`static void Dashboard_OnRotateCW(void)` | Coordina `Dashboard_StartModeSelection`, `Dashboard_TouchModeSelection`, `Dashboard_WrapMode` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:375`; structural |
| <a id="fn-dashboard_onrotateccw"></a>`static void Dashboard_OnRotateCCW(void)` | Coordina `Dashboard_StartModeSelection`, `Dashboard_TouchModeSelection`, `Dashboard_WrapMode` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:391`; structural |
| <a id="fn-dashboard_onshortpress"></a>`static void Dashboard_OnShortPress(void)` | Coordina `Dashboard_ApplyPendingMode`, `Dashboard_OpenStabilizationPrompt`, `IS_FLAG_SET` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:407`; structural |
| <a id="fn-dashboard_onuserbutton"></a>`static void Dashboard_OnUserButton(void)` | Coordina `CLEAR_FLAG`, `Dashboard_CancelPendingMode`, `IS_FLAG_SET` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:417`; structural |
| <a id="fn-dashboard_onlongpress"></a>`static void Dashboard_OnLongPress(void)` | Coordina `CLEAR_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:437`; structural |
| <a id="fn-dashboard_onenclongpress"></a>`static void Dashboard_OnEncLongPress(void)` | Coordina `CLEAR_FLAG`, `MenuSys_GetFavoritesSubMenu`, `MenuSys_OpenSubMenu` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:453`; structural |
| <a id="fn-dashboardquickaction_onshortpress"></a>`static void DashboardQuickAction_OnShortPress(void)` | Coordina `APP_BalanceSetEnabled`, `Dashboard_BackToDashboard`, `Dashboard_StartModeSelection` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:492`; structural |
| <a id="fn-dashboardquickaction_onrotatecw"></a>`static void DashboardQuickAction_OnRotateCW(void)` | Coordina `Dashboard_OpenQuickAction`, `Dashboard_WrapQuickAction` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:511`; structural |
| <a id="fn-dashboardquickaction_onrotateccw"></a>`static void DashboardQuickAction_OnRotateCCW(void)` | Coordina `Dashboard_OpenQuickAction`, `Dashboard_WrapQuickAction` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:516`; structural |
| <a id="fn-menu_onrotatecw"></a>`static void Menu_OnRotateCW(void)` | Adapta la llamada a `MenuSys_MoveCursorUp` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:530`; structural |
| <a id="fn-menu_onrotateccw"></a>`static void Menu_OnRotateCCW(void)` | Adapta la llamada a `MenuSys_MoveCursorDown` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:536`; structural |
| <a id="fn-menu_onshortpress"></a>`static void Menu_OnShortPress(void)` | Adapta la llamada a `MenuSys_HandleClick` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:542`; structural |
| <a id="fn-menu_onenclongpress"></a>`static void Menu_OnEncLongPress(void)` | Coordina `MenuSys_CanBeFavorite`, `MenuSys_ToggleFavorite`, `OledUtils_ShowSimpleNotificationMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:547`; structural |
| <a id="fn-menu_onuserbutton"></a>`static void Menu_OnUserButton(void)` | Coordina `Dashboard_BackToDashboard`, `MenuSys_NavigateBack` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:573`; structural |
| <a id="fn-menu_onlongpress"></a>`static void Menu_OnLongPress(void)` | Operación Menu On Long Press de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:582`; inferred-name |
| <a id="fn-about_onrotate"></a>`static void About_OnRotate(void)` | Coordina `CLEAR_FLAG`, `IS_FLAG_SET`, `SET_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:600`; structural |
| <a id="fn-about_onshortpress"></a>`static void About_OnShortPress(void)` | Coordina `CLEAR_FLAG`, `MenuSys_NavigateToMain`, `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:615`; structural |
| <a id="fn-about_onlongpress"></a>`static void About_OnLongPress(void)` | Coordina `CLEAR_FLAG`, `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:628`; structural |
| <a id="fn-genericconfirm_returntoprevious"></a>`static void GenericConfirm_ReturnToPrevious(void)` | Coordina `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:668`; structural |
| <a id="fn-genericconfirm_cancel"></a>`static void GenericConfirm_Cancel(void)` | Coordina `GenericConfirm_ReturnToPrevious`, `onCancel` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:682`; structural |
| <a id="fn-genericconfirm_onshortpress"></a>`static void GenericConfirm_OnShortPress(void)` | Coordina `GenericConfirm_ReturnToPrevious`, `onConfirm` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:691`; structural |
| <a id="fn-bootv2restart_execute"></a>`static void BootV2Restart_Execute(void)` | Coordina `APP_BalanceSetEnabled`, `AutoMicroBootMailbox_WriteRequest`, `CLEAR_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:790`; structural |
| <a id="fn-bootstrestart_execute"></a>`static void BootSTRestart_Execute(void)` | Coordina `APP_BalanceSetEnabled`, `AutoMicroBootMailbox_WriteRequest`, `CLEAR_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:821`; structural |
| <a id="fn-motortest_adjustspeed"></a>`static void MotorTest_AdjustSpeed(Motor_Config_t *cfg, float delta)` | Adapta la llamada a `Motor_ClampSpeedPercent` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:856`; structural |
| <a id="fn-motortest_toggledirection"></a>`static void MotorTest_ToggleDirection(Motor_Config_t *cfg)` | Operación Motor Test Toggle Direction de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:865`; inferred-name |
| <a id="fn-motortest_onrotatecw"></a>`static void MotorTest_OnRotateCW(void)` | Coordina `MotorTest_AdjustSpeed`, `NIBBLEH_GET_STATE` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:876`; structural |
| <a id="fn-motortest_onrotateccw"></a>`static void MotorTest_OnRotateCCW(void)` | Coordina `MotorTest_AdjustSpeed`, `NIBBLEH_GET_STATE` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:891`; structural |
| <a id="fn-motortest_onshortpress"></a>`static void MotorTest_OnShortPress(void)` | Coordina `MotorTest_ToggleDirection`, `NIBBLEH_GET_STATE` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:906`; structural |
| <a id="fn-motortest_onenclongpress"></a>`static void MotorTest_OnEncLongPress(void)` | Coordina `NIBBLEH_GET_STATE`, `NIBBLEH_SET_STATE` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:921`; structural |
| <a id="fn-motortest_onuserbutton"></a>`static void MotorTest_OnUserButton(void)` | Coordina `CLEAR_FLAG`, `IS_FLAG_SET`, `SET_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:929`; structural |
| <a id="fn-motortest_onlongpress"></a>`static void MotorTest_OnLongPress(void)` | Coordina `CLEAR_FLAG`, `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:939`; structural |
| <a id="fn-motorsingletest_savestate"></a>`static void MotorSingleTest_SaveState(void)` | Operación Motor Single Test Save State de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:962`; inferred-name |
| <a id="fn-motorsingletest_apply"></a>`static void MotorSingleTest_Apply(void)` | Coordina `MotorDrive_ApplyTestState`, `NIBBLEH_SET_STATE`, `SET_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:968`; structural |
| <a id="fn-eventmanagers_restoremenu"></a>`static void EventManagers_RestoreMenu(void)` | Coordina `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:987`; structural |
| <a id="fn-motorsingletest_returntomenu"></a>`static void MotorSingleTest_ReturnToMenu(void)` | Coordina `EventManagers_RestoreMenu`, `MotorDrive_ApplyTestState`, `MotorDrive_StopAll` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1002`; structural |
| <a id="fn-motorsingletest_onrotatecw"></a>`static void MotorSingleTest_OnRotateCW(void)` | Coordina `MotorSingleTest_Apply`, `Motor_ClampSpeedPercent` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1015`; structural |
| <a id="fn-motorsingletest_onrotateccw"></a>`static void MotorSingleTest_OnRotateCCW(void)` | Coordina `MotorSingleTest_Apply`, `Motor_ClampSpeedPercent` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1022`; structural |
| <a id="fn-motorsingletest_onuserbutton"></a>`static void MotorSingleTest_OnUserButton(void)` | Adapta la llamada a `MotorSingleTest_Apply` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1029`; structural |
| <a id="fn-mpucalibration_returntomenu"></a>`static void MPUCalibration_ReturnToMenu(void)` | Adapta la llamada a `EventManagers_RestoreMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1118`; structural |
| <a id="fn-mpucalibration_onuserbutton"></a>`static void MPUCalibration_OnUserButton(void)` | Coordina `APP_MPU_CanRecalibrate`, `APP_MPU_RequestCalibration` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1123`; structural |
| <a id="fn-wifiresults_backtowifimenu"></a>`static void WiFiResults_BackToWifiMenu(void)` | Coordina `MenuSys_NavigateBack`, `WiFiScan_ClearResults`, `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1144`; structural |
| <a id="fn-wifiresults_restartscan"></a>`static void WiFiResults_RestartScan(void)` | Coordina `CLEAR_FLAG`, `WiFiScan_ClearResults`, `clearScreen` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1158`; structural |
| <a id="fn-wifisearch_cancel"></a>`static void WiFiSearch_Cancel(void)` | Coordina `CLEAR_FLAG`, `WiFiScan_ClearResults`, `WifiBridge_StopScan` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1179`; structural |
| <a id="fn-wifisearch_onshortpress"></a>`static void WiFiSearch_OnShortPress(void)` | Adapta la llamada a `WiFiSearch_Cancel` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1200`; structural |
| <a id="fn-wifisearch_onlongpress"></a>`static void WiFiSearch_OnLongPress(void)` | Coordina `CLEAR_FLAG`, `WiFiScan_ClearResults`, `WifiBridge_StopScan` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1205`; structural |
| <a id="fn-wifiresults_onrotatecw"></a>`static void WiFiResults_OnRotateCW(void)` | Adapta la llamada a `MenuSys_MoveCursorUp` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1236`; structural |
| <a id="fn-wifiresults_onrotateccw"></a>`static void WiFiResults_OnRotateCCW(void)` | Adapta la llamada a `MenuSys_MoveCursorDown` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1242`; structural |
| <a id="fn-wifiresults_onshortpress"></a>`static void WiFiResults_OnShortPress(void)` | Coordina `WiFiResults_BackToWifiMenu`, `WiFiResults_RestartScan`, `WifiBridge_GetActiveBackend` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1248`; structural |
| <a id="fn-wifiresults_onenclongpress"></a>`static void WiFiResults_OnEncLongPress(void)` | Adapta la llamada a `WiFiResults_BackToWifiMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1285`; structural |
| <a id="fn-wifiresults_onuserbutton"></a>`static void WiFiResults_OnUserButton(void)` | Adapta la llamada a `WiFiScan_ClearResults` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1290`; structural |
| <a id="fn-wifidetails_backtoresults"></a>`static void WiFiDetails_BackToResults(void)` | Operación Wi Fi Details Back To Results de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:1316`; inferred-name |
| <a id="fn-wifiatpassword_showfailed"></a>`static void WiFiAtPassword_ShowFailed(void)` | Coordina `OledUtils_DismissNotification`, `OledUtils_SetWiFiCredentialsWebResultStatus`, `OledUtils_ShowNotificationMsEx` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1328`; structural |
| <a id="fn-wifiatpassword_connectselected"></a>`static void WiFiAtPassword_ConnectSelected(const char *password)` | Coordina `APP_ESPWifiConnectRequested`, `OledUtils_DismissNotification`, `OledUtils_SetWiFiConnectedSsid` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1339`; structural |
| <a id="fn-wifiatpassword_begin"></a>`static void WiFiAtPassword_Begin(void)` | Coordina `memset` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1372`; structural |
| <a id="fn-wifidetails_onshortpress"></a>`static void WiFiDetails_OnShortPress(void)` | Coordina `WiFiAtPassword_Begin`, `WiFiAtPassword_ConnectSelected`, `WifiBridge_GetActiveBackend` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1383`; structural |
| <a id="fn-wifiatpassword_onrotatecw"></a>`static void WiFiAtPassword_OnRotateCW(void)` | Operación Wi Fi At Password On Rotate CW de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:1413`; inferred-name |
| <a id="fn-wifiatpassword_onrotateccw"></a>`static void WiFiAtPassword_OnRotateCCW(void)` | Operación Wi Fi At Password On Rotate CCW de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:1422`; inferred-name |
| <a id="fn-wifiatpassword_onshortpress"></a>`static void WiFiAtPassword_OnShortPress(void)` | Operación Wi Fi At Password On Short Press de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:1433`; inferred-name |
| <a id="fn-wifiatpassword_onuserbutton"></a>`static void WiFiAtPassword_OnUserButton(void)` | Operación Wi Fi At Password On User Button de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:1445`; inferred-name |
| <a id="fn-wifiatpassword_onenclongpress"></a>`static void WiFiAtPassword_OnEncLongPress(void)` | Adapta la llamada a `WiFiAtPassword_ConnectSelected` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1461`; structural |
| <a id="fn-readonly_onshortpress"></a>`static void ReadOnly_OnShortPress(void)` | Adapta la llamada a `clearScreen` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1491`; structural |
| <a id="fn-irvalues_toggleview"></a>`static void IRValues_ToggleView(void)` | Adapta la llamada a `OledUtils_ToggleValoresIRView` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1525`; structural |
| <a id="fn-irvalues_onshortpress"></a>`static void IRValues_OnShortPress(void)` | Coordina `OledUtils_EndIRValuesSession`, `ReadOnly_OnShortPress` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1530`; structural |
| <a id="fn-irvalues_onlongpress"></a>`static void IRValues_OnLongPress(void)` | Coordina `OledUtils_EndIRValuesSession`, `ReadOnly_OnLongPress` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1536`; structural |
| <a id="fn-irvalues_onenclongpress"></a>`static void IRValues_OnEncLongPress(void)` | Adapta la llamada a `HAL_GPIO_TogglePin` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1542`; structural |
| <a id="fn-routealignment_toggleview"></a>`static void RouteAlignment_ToggleView(void)` | Adapta la llamada a `OledUtils_ToggleTestCaminoView` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1556`; structural |
| <a id="fn-mpuvalues_toggleview"></a>`static void MPUValues_ToggleView(void)` | Adapta la llamada a `OledUtils_ToggleValoresMPUView` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1570`; structural |
| <a id="fn-unerroutertest_toggleview"></a>`static void UnerRouterTest_ToggleView(void)` | Adapta la llamada a `OledUtils_NextUnerRouterPacketView` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1584`; structural |
| <a id="fn-unerroutertest_previousview"></a>`static void UnerRouterTest_PreviousView(void)` | Adapta la llamada a `OledUtils_PreviousUnerRouterPacketView` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1589`; structural |
| <a id="fn-preferences_oledrotationfromdeg"></a>`static OledRotation_t Preferences_OledRotationFromDeg(uint16_t rotationDeg)` | Adapta la llamada a `AppPreferences_ClampOledRotationDeg` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1778`; structural |
| <a id="fn-preferences_applyoledrotationdraft"></a>`static void Preferences_ApplyOledRotationDraft(void)` | Coordina `OledUtils_SetRotationAndRefresh`, `Preferences_OledRotationFromDeg` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1786`; structural |
| <a id="fn-preferences_wrapoledrotationdeg"></a>`static uint16_t Preferences_WrapOledRotationDeg(uint16_t currentDeg, int16_t delta)` | Coordina `AppPreferences_ClampOledRotationDeg` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1792`; structural |
| <a id="fn-motorminpwm_savelivestateonce"></a>`static void MotorMinPwm_SaveLiveStateOnce(void)` | Operación Motor Min Pwm Save Live State Once de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:1890`; inferred-name |
| <a id="fn-motorminpwm_applylivetest"></a>`static void MotorMinPwm_ApplyLiveTest(void)` | Coordina `MotorDrive_ApplyTestState`, `NIBBLEH_SET_STATE`, `SET_FLAG` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1900`; structural |
| <a id="fn-motorminpwm_restorelivestate"></a>`static void MotorMinPwm_RestoreLiveState(void)` | Coordina `MotorDrive_ApplyTestState`, `MotorDrive_StopAll` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1918`; structural |
| <a id="fn-screenlocktimeout_backtomenu"></a>`static void ScreenLockTimeout_BackToMenu(bool save)` | Coordina `EventManagers_RestoreMenu`, `ScreenLock_CancelTimeoutDraft`, `ScreenLock_SaveTimeoutDraft` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:1969`; structural |
| <a id="fn-screenlocktimeout_onrotatecw"></a>`static void ScreenLockTimeout_OnRotateCW(void)` | Adapta la llamada a `ScreenLock_AdjustTimeoutDraft` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1980`; structural |
| <a id="fn-screenlocktimeout_onrotateccw"></a>`static void ScreenLockTimeout_OnRotateCCW(void)` | Adapta la llamada a `ScreenLock_AdjustTimeoutDraft` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1986`; structural |
| <a id="fn-screenlocktimeout_onshortpress"></a>`static void ScreenLockTimeout_OnShortPress(void)` | Adapta la llamada a `ScreenLockTimeout_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1992`; structural |
| <a id="fn-screenlocktimeout_onlongpress"></a>`static void ScreenLockTimeout_OnLongPress(void)` | Adapta la llamada a `ScreenLockTimeout_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:1997`; structural |
| <a id="fn-systemactiontimeout_backtomenu"></a>`static void SystemActionTimeout_BackToMenu(bool save)` | Coordina `AppPreferences_SetSystemActionTimeoutSec`, `EventManagers_RestoreMenu` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2011`; structural |
| <a id="fn-systemactiontimeout_onrotatecw"></a>`static void SystemActionTimeout_OnRotateCW(void)` | Adapta la llamada a `AppPreferences_ClampSystemActionTimeoutSec` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2022`; structural |
| <a id="fn-systemactiontimeout_onrotateccw"></a>`static void SystemActionTimeout_OnRotateCCW(void)` | Coordina `AppPreferences_ClampSystemActionTimeoutSec` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2029`; structural |
| <a id="fn-systemactiontimeout_onshortpress"></a>`static void SystemActionTimeout_OnShortPress(void)` | Adapta la llamada a `SystemActionTimeout_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2042`; structural |
| <a id="fn-systemactiontimeout_onlongpress"></a>`static void SystemActionTimeout_OnLongPress(void)` | Adapta la llamada a `SystemActionTimeout_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2047`; structural |
| <a id="fn-usbaliveperiod_backtomenu"></a>`static void UsbAlivePeriod_BackToMenu(bool save)` | Coordina `AppPreferences_SetUsbAlivePeriodMs`, `EventManagers_RestoreMenu` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2061`; structural |
| <a id="fn-usbaliveperiod_adjustdraft"></a>`static void UsbAlivePeriod_AdjustDraft(int16_t delta)` | Coordina `AppPreferences_ClampUsbAlivePeriodMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2072`; structural |
| <a id="fn-usbaliveperiod_onrotatecw"></a>`static void UsbAlivePeriod_OnRotateCW(void)` | Adapta la llamada a `UsbAlivePeriod_AdjustDraft` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2085`; structural |
| <a id="fn-usbaliveperiod_onrotateccw"></a>`static void UsbAlivePeriod_OnRotateCCW(void)` | Adapta la llamada a `UsbAlivePeriod_AdjustDraft` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2091`; structural |
| <a id="fn-usbaliveperiod_onshortpress"></a>`static void UsbAlivePeriod_OnShortPress(void)` | Adapta la llamada a `UsbAlivePeriod_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2097`; structural |
| <a id="fn-usbaliveperiod_onlongpress"></a>`static void UsbAlivePeriod_OnLongPress(void)` | Adapta la llamada a `UsbAlivePeriod_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2102`; structural |
| <a id="fn-lineblackwidth_backtomenu"></a>`static void LineBlackWidth_BackToMenu(bool save)` | Coordina `APP_ApplyPreferenceRuntimeChanges`, `AppPreferences_SetLineBlackWidthMm`, `EventManagers_RestoreMenu` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2116`; structural |
| <a id="fn-lineblackwidth_adjustdraft"></a>`static void LineBlackWidth_AdjustDraft(int16_t delta)` | Coordina `AppPreferences_ClampLineBlackWidthMm` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2129`; structural |
| <a id="fn-lineblackwidth_onrotatecw"></a>`static void LineBlackWidth_OnRotateCW(void)` | Adapta la llamada a `LineBlackWidth_AdjustDraft` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2142`; structural |
| <a id="fn-lineblackwidth_onrotateccw"></a>`static void LineBlackWidth_OnRotateCCW(void)` | Adapta la llamada a `LineBlackWidth_AdjustDraft` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2148`; structural |
| <a id="fn-lineblackwidth_onshortpress"></a>`static void LineBlackWidth_OnShortPress(void)` | Adapta la llamada a `LineBlackWidth_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2154`; structural |
| <a id="fn-lineblackwidth_onlongpress"></a>`static void LineBlackWidth_OnLongPress(void)` | Adapta la llamada a `LineBlackWidth_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2159`; structural |
| <a id="fn-oledrotationtest_backtomenu"></a>`static void OledRotationTest_BackToMenu(bool save)` | Coordina `AppPreferences_SetOledRotationDeg`, `EventManagers_RestoreMenu`, `OledUtils_SetRotationAndRefresh` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2173`; structural |
| <a id="fn-oledrotationtest_adjust"></a>`static void OledRotationTest_Adjust(int16_t delta)` | Coordina `Preferences_ApplyOledRotationDraft`, `Preferences_WrapOledRotationDeg` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2186`; structural |
| <a id="fn-oledrotationtest_onrotatecw"></a>`static void OledRotationTest_OnRotateCW(void)` | Adapta la llamada a `OledRotationTest_Adjust` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2192`; structural |
| <a id="fn-oledrotationtest_onrotateccw"></a>`static void OledRotationTest_OnRotateCCW(void)` | Adapta la llamada a `OledRotationTest_Adjust` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2197`; structural |
| <a id="fn-oledrotationtest_onshortpress"></a>`static void OledRotationTest_OnShortPress(void)` | Adapta la llamada a `OledRotationTest_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2202`; structural |
| <a id="fn-oledrotationtest_onlongpress"></a>`static void OledRotationTest_OnLongPress(void)` | Adapta la llamada a `OledRotationTest_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2207`; structural |
| <a id="fn-oledrotationmputest_backtomenu"></a>`static void OledRotationMpuTest_BackToMenu(void)` | Coordina `EventManagers_RestoreMenu`, `OledUtils_SetRotationAndRefresh`, `Preferences_OledRotationFromDeg` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2221`; structural |
| <a id="fn-motorminpwm_backtomenu"></a>`static void MotorMinPwm_BackToMenu(bool save)` | Coordina `AppPreferences_GetMotorLeftMinPwm`, `AppPreferences_GetMotorRightMinPwm`, `AppPreferences_SetMotorLeftMinPwm` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2238`; structural |
| <a id="fn-motorminpwm_adjustdraft"></a>`static void MotorMinPwm_AdjustDraft(int16_t delta)` | Coordina `AppPreferences_ClampMotorMinPwm`, `MotorDrive_SetLeftMinPwm`, `MotorDrive_SetRightMinPwm` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2261`; structural |
| <a id="fn-motorminpwm_onrotatecw"></a>`static void MotorMinPwm_OnRotateCW(void)` | Adapta la llamada a `MotorMinPwm_AdjustDraft` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2280`; structural |
| <a id="fn-motorminpwm_onrotateccw"></a>`static void MotorMinPwm_OnRotateCCW(void)` | Adapta la llamada a `MotorMinPwm_AdjustDraft` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2286`; structural |
| <a id="fn-motorminpwm_onshortpress"></a>`static void MotorMinPwm_OnShortPress(void)` | Adapta la llamada a `MotorMinPwm_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2292`; structural |
| <a id="fn-motorminpwm_onlongpress"></a>`static void MotorMinPwm_OnLongPress(void)` | Adapta la llamada a `MotorMinPwm_BackToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2297`; structural |
| <a id="fn-balancepid_clampgain"></a>`static float BalancePid_ClampGain(float value)` | Operación Balance Pid Clamp Gain de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:2311`; inferred-name |
| <a id="fn-balancepid_clamplogduration"></a>`static uint16_t BalancePid_ClampLogDuration(uint16_t value)` | Coordina `BalancePid_GetLogAvailableBytes` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2322`; structural |
| <a id="fn-balancepid_readselectedgain"></a>`static float BalancePid_ReadSelectedGain(void)` | Devuelve el campo `balanceControl.config.kp` del estado actual de la instancia. | `f4:Core/Src/eventManagers.c:2342`; structural |
| <a id="fn-balancepid_applyselectedgain"></a>`static void BalancePid_ApplySelectedGain(float value)` | Coordina `BalanceControl_SetPid`, `BalancePid_ClampGain` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2355`; structural |
| <a id="fn-balancepid_begingainedit"></a>`static void BalancePid_BeginGainEdit(BalancePidGainTarget_t target)` | Adapta la llamada a `BalancePid_ReadSelectedGain` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2379`; structural |
| <a id="fn-balancepid_returntomenu"></a>`static void BalancePid_ReturnToMenu(void)` | Adapta la llamada a `EventManagers_RestoreMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2410`; structural |
| <a id="fn-balancepidgain_adjust"></a>`static void BalancePidGain_Adjust(float delta)` | Coordina `BalancePid_ApplySelectedGain`, `BalancePid_ClampGain` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2417`; structural |
| <a id="fn-balancepidgain_onrotatecw"></a>`static void BalancePidGain_OnRotateCW(void)` | Adapta la llamada a `BalancePidGain_Adjust` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2424`; structural |
| <a id="fn-balancepidgain_onrotateccw"></a>`static void BalancePidGain_OnRotateCCW(void)` | Adapta la llamada a `BalancePidGain_Adjust` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2429`; structural |
| <a id="fn-balancepidgain_onshortpress"></a>`static void BalancePidGain_OnShortPress(void)` | Coordina `AppPreferences_GetBalancePid`, `AppPreferences_SetBalancePid`, `BalanceControl_Reset` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2434`; structural |
| <a id="fn-balancepidgain_onlongpress"></a>`static void BalancePidGain_OnLongPress(void)` | Coordina `BalanceControl_Reset`, `BalancePid_ApplySelectedGain`, `BalancePid_ReturnToMenu` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2457`; structural |
| <a id="fn-balancepidlog_adjust"></a>`static void BalancePidLog_Adjust(int16_t delta)` | Coordina `BalancePid_ClampLogDuration`, `BalancePid_GetLogAvailableBytes` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2474`; structural |
| <a id="fn-balancepidlog_onrotatecw"></a>`static void BalancePidLog_OnRotateCW(void)` | Adapta la llamada a `BalancePidLog_Adjust` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2496`; structural |
| <a id="fn-balancepidlog_onrotateccw"></a>`static void BalancePidLog_OnRotateCCW(void)` | Adapta la llamada a `BalancePidLog_Adjust` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2501`; structural |
| <a id="fn-balancepidlog_onshortpress"></a>`static void BalancePidLog_OnShortPress(void)` | Adapta la llamada a `BalancePid_ReturnToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2506`; structural |
| <a id="fn-balancepidlog_onlongpress"></a>`static void BalancePidLog_OnLongPress(void)` | Adapta la llamada a `BalancePid_ReturnToMenu` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2512`; structural |
| <a id="fn-screenlocked_onunlock"></a>`static void ScreenLocked_OnUnlock(void)` | Adapta la llamada a `ScreenLock_Unlock` con el contexto de esta integración. | `f4:Core/Src/eventManagers.c:2587`; structural |
| <a id="fn-dashboard_requestactiontimeoutbarrefresh"></a>`static void Dashboard_RequestActionTimeoutBarRefresh(uint32_t now, uint32_t startedMs, uint32_t timeoutMs)` | Operación Dashboard Request Action Timeout Bar Refresh de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/eventManagers.c:2695`; inferred-name |
| <a id="fn-balancepidstatus_onshortpress"></a>`static void BalancePidStatus_OnShortPress(void)` | Coordina `APP_BalanceSetEnabled`, `MotionControl_IsArmed` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/eventManagers.c:2870`; structural |
| <a id="fn-menuicon_espnotifwatchdog"></a>`static const uint8_t *MenuIcon_EspNotifWatchdog(void)` | Adapta la llamada a `AppPreferences_IsEspNotifWatchdogEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:39`; structural |
| <a id="fn-menuicon_irobjects"></a>`static const uint8_t *MenuIcon_IrObjects(void)` | Adapta la llamada a `APP_IsIrObjectsEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:44`; structural |
| <a id="fn-menuicon_irline"></a>`static const uint8_t *MenuIcon_IrLine(void)` | Adapta la llamada a `APP_IsIrLineEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:51`; structural |
| <a id="fn-menuicon_irlights"></a>`static const uint8_t *MenuIcon_IrLights(void)` | Adapta la llamada a `APP_IsIrLightsEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:58`; structural |
| <a id="fn-menuname_irobjects"></a>`static const char *MenuName_IrObjects(void)` | Adapta la llamada a `APP_IsIrObjectsEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:65`; structural |
| <a id="fn-menuname_irline"></a>`static const char *MenuName_IrLine(void)` | Adapta la llamada a `APP_IsIrLineEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:70`; structural |
| <a id="fn-menuname_irlights"></a>`static const char *MenuName_IrLights(void)` | Adapta la llamada a `APP_IsIrLightsEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:75`; structural |
| <a id="fn-menuitem_isespnotifwatchdoglimitvisible"></a>`static bool MenuItem_IsEspNotifWatchdogLimitVisible(void)` | Adapta la llamada a `AppPreferences_IsEspNotifWatchdogEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:80`; structural |
| <a id="fn-menuaction_enterwifiap"></a>`static void MenuAction_EnterWifiAp(void)` | Coordina `MenuSys_OpenSubMenu`, `OledUtils_ShowSimpleNotificationMs`, `WifiBridge_IsConnected` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:173`; structural |
| <a id="fn-menuitem_isfollowmodevisible"></a>`static bool MenuItem_IsFollowModeVisible(void)` | Adapta la llamada a `GET_CAR_MODE` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:186`; structural |
| <a id="fn-linenoticetext"></a>`static const char *LineNoticeText(uint8_t reason)` | Operación Line Notice Text de Composición, tareas y acciones de aplicación; semántica fina pendiente de ampliación. | `f4:Core/Src/menu_definition.c:187`; inferred-name |
| <a id="fn-menulineback"></a>`static void MenuLineBack(void)` | Coordina `MenuSys_OpenSubMenu`, `OledUtils_DisableContinuousRender` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:201`; structural |
| <a id="fn-menulinenoticeevents"></a>`static void MenuLineNoticeEvents(UserEvent_t ev)` | Adapta la llamada a `MenuSys_OpenSubMenu` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:207`; structural |
| <a id="fn-menurender_linenotice"></a>`static void MenuRender_LineNotice(void)` | Coordina `LineNoticeText`, `MenuSys_SetCurrentScreenCode`, `OledUtils_Clear` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:213`; structural |
| <a id="fn-menuaction_linefollow"></a>`static void MenuAction_LineFollow(void)` | Coordina `APP_LineFollowEnabled`, `APP_LineFollowSetEnabled` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:218`; structural |
| <a id="fn-menuname_linefollow"></a>`static const char *MenuName_LineFollow(void)` | Adapta la llamada a `APP_LineFollowEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:226`; structural |
| <a id="fn-menulinecalevents"></a>`static void MenuLineCalEvents(UserEvent_t ev)` | Coordina `APP_GetLineState`, `APP_LineCalibrationBusy`, `APP_LineCalibrationControl` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:227`; structural |
| <a id="fn-menurender_linecalibration"></a>`static void MenuRender_LineCalibration(void)` | Coordina `APP_GetLineState`, `LineNoticeText`, `MenuSys_SetCurrentScreenCode` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:239`; structural |
| <a id="fn-menuaction_linecal"></a>`static void MenuAction_LineCal(uint8_t target)` | Coordina `APP_GetLineState`, `APP_LineCalibrationControl`, `OledUtils_EnableContinuousRender` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:244`; structural |
| <a id="fn-menuaction_linewhite"></a>`static void MenuAction_LineWhite(void)` | Adapta la llamada a `MenuAction_LineCal` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:256`; structural |
| <a id="fn-menuaction_linetrack"></a>`static void MenuAction_LineTrack(void)` | Adapta la llamada a `MenuAction_LineCal` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:257`; structural |
| <a id="fn-menuaction_enterir"></a>`static void MenuAction_EnterIr(void)` | Adapta la llamada a `MenuSys_OpenSubMenu` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:258`; structural |
| <a id="fn-menuaction_irback"></a>`static void MenuAction_IrBack(void)` | Coordina `GET_CAR_MODE`, `MenuSys_OpenSubMenu` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:259`; structural |
| <a id="fn-osktest_oncomplete"></a>`static void OskTest_OnComplete(OSK_State *st, void *ctx)` | Adapta la llamada a `MenuSys_SetCurrentScreenCode` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:414`; structural |
| <a id="fn-osktest_oncancel"></a>`static void OskTest_OnCancel(void *ctx)` | Adapta la llamada a `OskTest_OnComplete` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:425`; structural |
| <a id="fn-menusys_osktestalpha_wrapper"></a>`static void MenuSys_OskTestAlpha_Wrapper(void)` | Prepara el contexto de UI y selecciona la vista Menu Sys Osk Test Alpha. | `f4:Core/Src/menu_definition.c:430`; inferred-name |
| <a id="fn-menusys_osktestnumeric_wrapper"></a>`static void MenuSys_OskTestNumeric_Wrapper(void)` | Prepara el contexto de UI y selecciona la vista Menu Sys Osk Test Numeric. | `f4:Core/Src/menu_definition.c:436`; inferred-name |
| <a id="fn-menuitem_motorcalibrationvisible"></a>`static bool MenuItem_MotorCalibrationVisible(void)` | Adapta la llamada a `GET_CAR_MODE` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:500`; structural |
| <a id="fn-menumotorback"></a>`static void MenuMotorBack(void)` | Coordina `MenuSys_OpenSubMenu`, `OledUtils_DisableContinuousRender` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:501`; structural |
| <a id="fn-menumotorcalrender"></a>`static void MenuMotorCalRender(void)` | Coordina `APP_AutoMotionRequest`, `MenuSys_SetCurrentScreenCode`, `OledUtils_RenderMotorCalibration` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:505`; structural |
| <a id="fn-menumotorcalevents"></a>`static void MenuMotorCalEvents(UserEvent_t ev)` | Coordina `APP_AutoMotionBusy`, `APP_AutoMotionCancel`, `APP_AutoMotionRequest` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:510`; structural |
| <a id="fn-menumotorcalbegin"></a>`static void MenuMotorCalBegin(void)` | Adapta la llamada a `OledUtils_EnableContinuousRender` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:527`; structural |
| <a id="fn-menuaction_motors"></a>`static void MenuAction_Motors(void)` | Coordina `GET_CAR_MODE`, `MenuSys_OpenSubMenu` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:531`; structural |
| <a id="fn-menuaction_motorsback"></a>`static void MenuAction_MotorsBack(void)` | Coordina `GET_CAR_MODE`, `MenuSys_OpenSubMenu` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:532`; structural |
| <a id="fn-menuaction_rebootextnrf24"></a>`static void MenuAction_RebootExtNRF24(void)` | Coordina `AppPreferences_SetExtProfile`, `NVIC_SystemReset` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:826`; structural |
| <a id="fn-menuaction_rebootextbeeper"></a>`static void MenuAction_RebootExtBeeper(void)` | Coordina `AppPreferences_SetExtProfile`, `NVIC_SystemReset` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:832`; structural |
| <a id="fn-menuitem_isusbinstalleravailable"></a>`static bool MenuItem_IsUsbInstallerAvailable(void)` | Adapta la llamada a `MotionControl_GetStatus` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:874`; structural |
| <a id="fn-menuaction_selectusbcdc"></a>`static void MenuAction_SelectUsbCdc(void)` | Adapta la llamada a `USB_ModeManager_RequestCdc` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:888`; structural |
| <a id="fn-menuaction_selectusbinstaller"></a>`static void MenuAction_SelectUsbInstaller(void)` | Coordina `MenuItem_IsUsbInstallerAvailable`, `USB_ModeManager_RequestInstaller` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:893`; structural |
| <a id="fn-menuicon_usbcdc"></a>`static const uint8_t *MenuIcon_UsbCdc(void)` | Adapta la llamada a `USB_ModeManager_GetActiveMode` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:900`; structural |
| <a id="fn-menuicon_usbinstaller"></a>`static const uint8_t *MenuIcon_UsbInstaller(void)` | Adapta la llamada a `USB_ModeManager_GetActiveMode` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:907`; structural |
| <a id="fn-menuicon_wifiapfallback"></a>`static const uint8_t *MenuIcon_WifiApFallback(void)` | Adapta la llamada a `AppPreferences_IsWifiApFallbackEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:922`; structural |
| <a id="fn-menuaction_retrywifi"></a>`static void MenuAction_RetryWifi(void)` | Adapta la llamada a `APP_ESPWifiConnectRequested` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:928`; structural |
| <a id="fn-menuitem_iswifiretryvisible"></a>`static bool MenuItem_IsWifiRetryVisible(void)` | Coordina `IS_FLAG_SET`, `MenuItem_IsWifiWebStaVisible` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:933`; structural |
| <a id="fn-menuicon_encoderinvert"></a>`static const uint8_t *MenuIcon_EncoderInvert(void)` | Adapta la llamada a `AppPreferences_IsEncoderInverted` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:939`; structural |
| <a id="fn-menuicon_screenlock"></a>`static const uint8_t *MenuIcon_ScreenLock(void)` | Adapta la llamada a `AppPreferences_IsScreenLockEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:946`; structural |
| <a id="fn-menuicon_oledautorotation"></a>`static const uint8_t *MenuIcon_OledAutoRotation(void)` | Adapta la llamada a `AppPreferences_IsOledAutoRotationEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:955`; structural |
| <a id="fn-menuicon_unerrouterforwarding"></a>`static const uint8_t *MenuIcon_UnerRouterForwarding(void)` | Adapta la llamada a `AppPreferences_IsUnerRouterForwardingEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:962`; structural |
| <a id="fn-menuicon_pinlock"></a>`static const uint8_t *MenuIcon_PinLock(void)` | Adapta la llamada a `AppPreferences_IsPinLockEnabled` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:969`; structural |
| <a id="fn-menuicon_wifimodetoggle"></a>`static const uint8_t *MenuIcon_WifiModeToggle(void)` | Adapta la llamada a `IS_FLAG_SET` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:976`; structural |
| <a id="fn-menuicon_webmodetoggle"></a>`static const uint8_t *MenuIcon_WebModeToggle(void)` | Adapta la llamada a `IS_FLAG_SET` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:983`; structural |
| <a id="fn-menuname_wifiinfo"></a>`static const char *MenuName_WifiInfo(void)` | Adapta la llamada a `IS_FLAG_SET` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:990`; structural |
| <a id="fn-menuname_wifimodetoggle"></a>`static const char *MenuName_WifiModeToggle(void)` | Adapta la llamada a `IS_FLAG_SET` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:998`; structural |
| <a id="fn-menuname_webmodetoggle"></a>`static const char *MenuName_WebModeToggle(void)` | Adapta la llamada a `IS_FLAG_SET` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1005`; structural |
| <a id="fn-menuitem_iswebbackendvisible"></a>`static bool MenuItem_IsWebBackendVisible(void)` | Adapta la llamada a `WifiBridge_GetActiveBackend` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1012`; structural |
| <a id="fn-menuitem_isatmodetogglevisible"></a>`static bool MenuItem_IsAtModeToggleVisible(void)` | Adapta la llamada a `WifiBridge_GetActiveBackend` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1017`; structural |
| <a id="fn-menuitem_isatwifisearchvisible"></a>`static bool MenuItem_IsAtWifiSearchVisible(void)` | Coordina `IS_FLAG_SET`, `MenuItem_IsAtModeToggleVisible` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:1022`; structural |
| <a id="fn-menuname_attransport"></a>`static const char *MenuName_AtTransport(void)` | Adapta la llamada a `AppPreferences_GetWifiAtProtocol` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1027`; structural |
| <a id="fn-menuaction_toggleattransport"></a>`static void MenuAction_ToggleAtTransport(void)` | Coordina `AppPreferences_GetWifiAtProtocol`, `AppPreferences_SetWifiAtProtocol`, `WifiBridge_AT_SetNetworkProtocol` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:1034`; structural |
| <a id="fn-menuaction_togglestabilization"></a>`static void MenuAction_ToggleStabilization(void)` | Coordina `APP_BalanceSetEnabled`, `MotionControl_IsArmed` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:1050`; structural |
| <a id="fn-menuicon_stabilization"></a>`static const uint8_t *MenuIcon_Stabilization(void)` | Adapta la llamada a `MotionControl_IsArmed` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1062`; structural |
| <a id="fn-menuname_stabilization"></a>`static const char *MenuName_Stabilization(void)` | Adapta la llamada a `MotionControl_IsArmed` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1069`; structural |
| <a id="fn-menuaction_wificonnectiontoggle"></a>`static void MenuAction_WifiConnectionToggle(void)` | Coordina `IS_FLAG_SET`, `WifiBridge_Connect`, `WifiBridge_Disconnect` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:1076`; structural |
| <a id="fn-menuicon_wificonnectiontoggle"></a>`static const uint8_t *MenuIcon_WifiConnectionToggle(void)` | Adapta la llamada a `WifiBridge_IsConnected` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1093`; structural |
| <a id="fn-menuname_wificonnectiontoggle"></a>`static const char *MenuName_WifiConnectionToggle(void)` | Coordina `IS_FLAG_SET`, `WifiBridge_IsConnected`, `snprintf` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:1100`; structural |
| <a id="fn-menuitem_iswifiwebstavisible"></a>`static bool MenuItem_IsWifiWebStaVisible(void)` | Coordina `IS_FLAG_SET`, `MenuItem_IsWebBackendVisible` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:1119`; structural |
| <a id="fn-menuitem_iswifiwebapvisible"></a>`static bool MenuItem_IsWifiWebApVisible(void)` | Adapta la llamada a `WifiBridge_GetActiveBackend` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1124`; structural |
| <a id="fn-menuaction_togglewifiappasswordprotected"></a>`static void MenuAction_ToggleWifiApPasswordProtected(void)` | Coordina `AppApConfig_ApplyPatch`, `AppApConfig_Get`, `OledUtils_ShowSimpleNotificationMs` para Composición, tareas y acciones de aplicación. | `f4:Core/Src/menu_definition.c:1130`; structural |
| <a id="fn-menuicon_wifiappasswordprotected"></a>`static const uint8_t *MenuIcon_WifiApPasswordProtected(void)` | Adapta la llamada a `AppPreferences_IsWifiApPasswordProtected` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1151`; structural |
| <a id="fn-menuitem_iswifiappasswordprotectedvisible"></a>`static bool MenuItem_IsWifiApPasswordProtectedVisible(void)` | Adapta la llamada a `AppPreferences_IsWifiApPasswordProtected` con el contexto de esta integración. | `f4:Core/Src/menu_definition.c:1158`; structural |
