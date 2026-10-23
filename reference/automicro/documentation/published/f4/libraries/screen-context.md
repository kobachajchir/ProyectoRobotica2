# Contexto y servicios de pantallas

ID: `f4:module/Core/Inc/screen_context.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Reúne el estado de las pantallas y las operaciones que las conectan con los servicios de AutoMicro.

ScreensState, ScreensServices, Screens_Handle y macros que adaptan las entradas históricas sin argumento a un contexto activo.

<a id="modularity"></a>
## Modularidad observada

Los punteros a función eliminan muchas llamadas enlazadas directamente, pero Screens_Handle conserva tipos concretos y screens_internal.h accede al estado a través de macros. Cohesión de UI del proyecto, no independencia completa entre librerías.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

Screens_Handle presta SystemHandle, menú, encoder, WiFi, IR, motores, controles, MPU, submenús y OledHandle; posee ScreensState. Screens_Context selecciona una instancia global activa. OledHandle es estado de notificaciones y es distinto de SSD1306_Handle, que es el driver físico.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- La aplicación construye servicios y asigna Screens_Context antes de cualquier renderer.
- Screens_Init_* prepara cada submódulo de UI sobre ese contexto.
- Los callbacks de dibujo/reportes se resuelven mediante services; el contexto debe permanecer válido durante callbacks y tareas.

## Trabajo en tándem

- Wrappers seleccionan vista y eventos; oled_utils dibuja; lock/OSK/canvas conservan su estado dentro de ScreensState.
- ScreensServices comprende consulta de sensores, preferencias, acciones, dibujo y reportes UNER; la tabla expuesta no prueba ejecución de todos sus miembros.

## Precauciones y límites

- No hay garantía de reentrada ni de varios Screens_Context simultáneos.
- Cambiar un campo del modelo de vista puede obligar a adaptar productor, renderer y representación remota.

## Hallazgos vinculados

- [AMDOC-F4-007: Los contextos activos de UI limitan independencia y reentrada](../../../review/FINDINGS.md#amdoc-f4-007)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/screen_context.h` | `am_port.h`, `system.h`, `system_flags.h`, `types/menu_types.h`, `fonts.h`, `oled_handle.h`, `oled_utils.h`, `oled_canvas.h`, `osk_integration.h`, `screen_lock.h`, `encoder_types.h`, `motor_control_types.h`, `mpu9250_types.h`, `tcrt5000_types.h`, `balance_control_types.h`, `motion_control_types.h`, `line_follow_control_types.h`, `app_preferences_types.h`, `permissions_types.h`, `uner_handle_types.h`, `wifi_bridge_types.h`, `automicro_boot_contract.h` | `8b05c657019ef71db7c3dd58b6c9615be92db15041877cf23a58ef44a6bc61f2` |
| `Core/Inc/screens_internal.h` | `screen_context.h` | `ae507f96f2b5cd0461dc2e7aba44790fe1582c2194e05c9f6c531d572a740b35` |
| `Core/Inc/oled_handle.h` | `types/menu_types.h`, `stdbool.h`, `stdint.h` | `9a6aa8d962398978135bbaf3df2a4fa7a52d3fe4a67b5777880b505a9577d4e3` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `OledSimpleNotificationDef`

`f4:Core/Inc/screen_context.h:31`

| Atributo / firma | Función y precaución |
| --- | --- |
| `ScreenCode_t screenCode` | Dato de estado/configuración `screenCode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const uint8_t *icon` | Dato de estado/configuración `icon`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *line1` | Dato de estado/configuración `line1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char *line2` | Dato de estado/configuración `line2`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t iconW` | Dato de estado/configuración `iconW`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t iconH` | Dato de estado/configuración `iconH`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t layout` | Dato de estado/configuración `layout`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t iconFlags` | Dato de estado/configuración `iconFlags`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t strikeCount` | Dato de estado/configuración `strikeCount`; declaración estructural, interpretación específica pendiente de ampliar. |

### `DashboardScrollLane`

`f4:Core/Inc/screen_context.h:42`

| Atributo / firma | Función y precaución |
| --- | --- |
| `char text[WIFI_SSID_MAX_LEN + 1u]` | Dato de estado/configuración `text`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t startedMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `int16_t lastOffset` | Dato de estado/configuración `lastOffset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t x` | Dato de estado/configuración `x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t right` | Dato de estado/configuración `right`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OSK_SavedContext`

`f4:Core/Inc/screen_context.h:49`

| Atributo / firma | Función y precaución |
| --- | --- |
| `RenderFunction prevRenderFn` | Memoria de la observación anterior usada por la máquina de estados. |
| `UserEventManagerFn prevEventManagerFn` | Memoria de la observación anterior usada por la máquina de estados. |
| `bool prevAllowPeriodicRefresh` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t prevInsideMenu` | Memoria de la observación anterior usada por la máquina de estados. |
| `ScreenCode_t prevScreenCode` | Memoria de la observación anterior usada por la máquina de estados. |

### `OledCanvasPendingResponse_t`

`f4:Core/Inc/screen_context.h:69`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t valid` | Dato de estado/configuración `valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |
| `uint8_t destination_node` | Dato de estado/configuración `destination_node`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t payload[OLED_CANVAS_RESPONSE_BYTES]` | Vista del payload; copiar antes de permitir la reutilización del slot del parser. |

### `OledCanvasContext_t`

`f4:Core/Inc/screen_context.h:75`

| Atributo / firma | Función y precaución |
| --- | --- |
| `OledCanvasState_t state` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t transport_id` | Transporte de entrada/salida; forma parte de la identidad del endpoint. |
| `uint8_t source_node` | Identidad UNER del solicitante; no sustituir por un requestId JSON. |
| `uint16_t transfer_id` | Identificador de transacción canvas dentro del endpoint transporte+nodo. |
| `uint16_t received_len` | Cantidad de bytes válidos ensamblados en el framebuffer. |
| `uint32_t expected_crc32` | CRC anunciado por el productor para el framebuffer completo. |
| `uint32_t computed_crc32` | CRC calculado sobre los bytes recibidos. |
| `uint32_t last_activity_ms` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t display_error_count_before` | Dato de estado/configuración `display_error_count_before`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t framebuffer[OLED_CANVAS_FRAMEBUFFER_BYTES]` | Imagen binaria retenida durante el ensamblaje/visualización del canvas. |
| `uint8_t last_result_valid` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t last_result_source_node` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint16_t last_result_transfer_id` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t last_result_status` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint32_t last_result_crc32` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t cancel_display_status` | Dato de estado/configuración `cancel_display_status`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledCanvasUiAction_t deferred_ui_action` | Dato de estado/configuración `deferred_ui_action`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledCanvasPendingResponse_t pending_response` | Respuesta final conservada hasta que el envío pueda aceptarse. |
| `OledCanvasSendResponseFn send_response` | Callback que envía payload final a transporte y nodo con send_ctx. |
| `void *send_ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |

### `ScreensState`

`f4:Core/Inc/screen_context.h:99`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const FontDef * oled_font` | Dato de estado/configuración `oled_font`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char oled_wifi_connected_ssid[WIFI_SSID_MAX_LEN + 1u]` | Dato de estado/configuración `oled_wifi_connected_ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t oled_wifi_credentials_result_status` | Dato de estado/configuración `oled_wifi_credentials_result_status`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char oled_remote_mode_notification_line[12]` | Dato de estado/configuración `oled_remote_mode_notification_line`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool oledNotificationsBlocked` | Dato de estado/configuración `oledNotificationsBlocked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool menu_scroll_active` | Dato de estado/configuración `menu_scroll_active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t menu_scroll_start_tick` | Dato de estado/configuración `menu_scroll_start_tick`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const MenuItem * menu_scroll_current_item` | Dato de estado/configuración `menu_scroll_current_item`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t scroll_text_x` | Dato de estado/configuración `scroll_text_x`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t scroll_text_y` | Dato de estado/configuración `scroll_text_y`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t scroll_max_chars` | Dato de estado/configuración `scroll_max_chars`; declaración estructural, interpretación específica pendiente de ampliar. |
| `const char * scroll_text_ptr` | Dato de estado/configuración `scroll_text_ptr`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t scroll_text_len` | Dato de estado/configuración `scroll_text_len`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int16_t scroll_last_offset` | Dato de estado/configuración `scroll_last_offset`; declaración estructural, interpretación específica pendiente de ampliar. |
| `DashboardScrollLane dashboardScrollLanes[2]` | Dato de estado/configuración `dashboardScrollLanes`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool dashboardScrollActive` | Dato de estado/configuración `dashboardScrollActive`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t appEspNotifWatchdogLimitDraft` | Dato de estado/configuración `appEspNotifWatchdogLimitDraft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool dashboardModeReportPending` | Dato de estado/configuración `dashboardModeReportPending`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool dashboardConnectivityDirty` | Dato de estado/configuración `dashboardConnectivityDirty`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool irValuesRawView` | Dato de estado/configuración `irValuesRawView`; declaración estructural, interpretación específica pendiente de ampliar. |
| `int8_t irValuesSessionMode` | Dato de estado/configuración `irValuesSessionMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool irValuesSavedGroup5` | Dato de estado/configuración `irValuesSavedGroup5`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool irValuesSavedGroup3` | Dato de estado/configuración `irValuesSavedGroup3`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool irValuesAmbientMode` | Dato de estado/configuración `irValuesAmbientMode`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool routeAlignmentDetailView` | Dato de estado/configuración `routeAlignmentDetailView`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t mpuValuesViewIndex` | Dato de estado/configuración `mpuValuesViewIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t unerRouterTransportId` | Dato de estado/configuración `unerRouterTransportId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool unerRouterTxView` | Dato de estado/configuración `unerRouterTxView`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t dashboardWebStatusRequestedAt` | Dato de estado/configuración `dashboardWebStatusRequestedAt`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MenuItem espResetModeItems[3]` | Dato de estado/configuración `espResetModeItems`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SubMenu espResetModeMenu` | Dato de estado/configuración `espResetModeMenu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool wifiApEnableProtectionAfterPassword` | Dato de estado/configuración `wifiApEnableProtectionAfterPassword`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OSK_State s_oskState` | Estado del teclado integrado en esta instancia de pantallas. |
| `OSK_SavedContext s_savedCtx` | Dato de estado/configuración `s_savedCtx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OSK_DrawHandle s_drawHandle` | Dato de estado/configuración `s_drawHandle`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool s_initialized` | Dato de estado/configuración `s_initialized`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OSK_CompleteFn s_userOnComplete` | Dato de estado/configuración `s_userOnComplete`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OSK_CancelFn s_userOnCancel` | Dato de estado/configuración `s_userOnCancel`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void * s_userCtx` | Dato de estado/configuración `s_userCtx`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool screenLockLocked` | Dato de estado/configuración `screenLockLocked`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t screenLockIdleTicks10ms` | Dato de estado/configuración `screenLockIdleTicks10ms`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t screenLockTimeoutDraftSec` | Dato de estado/configuración `screenLockTimeoutDraftSec`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderFunction screenLockPreviousRenderFn` | Dato de estado/configuración `screenLockPreviousRenderFn`; declaración estructural, interpretación específica pendiente de ampliar. |
| `UserEventManagerFn screenLockPreviousEventManagerFn` | Dato de estado/configuración `screenLockPreviousEventManagerFn`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool screenLockPreviousAllowPeriodicRefresh` | Dato de estado/configuración `screenLockPreviousAllowPeriodicRefresh`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t screenLockPreviousInsideMenu` | Dato de estado/configuración `screenLockPreviousInsideMenu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledCanvasContext_t oled_canvas` | Estado de transferencia, framebuffer y respuesta pendientes del canvas. |

### `ScreensServices`

`f4:Core/Inc/screen_context.h:148`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t (*now_ms)(void)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |
| `bool (*route_emitters_on)(void)` | Operación route emitters on de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `bool (*APP_EvaluateRoute)(TCRT5000_RouteSample_t *out)` | Operación APP Evaluate Route de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `bool (*APP_LineCalibrationBusy)(void)` | Operación APP Line Calibration Busy de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*APP_ESPFirmwareRequestRequested)(void)` | Registra que se solicitó firmware ESP para atenderlo en el flujo de UI/aplicación. |
| `void (*APP_ESPModeChangeCommandAccepted)(void)` | Registra aceptación del comando de cambio de modo sin afirmar finalización de red. |
| `void (*APP_ESPStatusRefreshRequested)(void)` | Programa una actualización de estado ESP. |
| `bool (*APP_IsIrLineEnabled)(void)` | Consulta habilitación de los sensores IR de línea. |
| `bool (*APP_IsIrObjectsEnabled)(void)` | Consulta habilitación de los sensores IR de objetos. |
| `uint8_t (*APP_MPU_GetCalibrationProgressPercent)(void)` | Operación APP MPU Get Calibration Progress Percent de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint8_t (*APP_MPU_IsCalibrationDoneNoticeActive)(void)` | Operación APP MPU Is Calibration Done Notice Active de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint8_t (*APP_MPU_IsCalibrationPromptActive)(void)` | Operación APP MPU Is Calibration Prompt Active de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint8_t (*APP_MPU_IsCalibrationRunning)(void)` | Operación APP MPU Is Calibration Running de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*About_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de About User Event Manager. |
| `AppApStatus_t (*AppApConfig_ApplyPatch)(const AppApConfigPatch_t *patch, uint8_t *changedMask)` | Revisa un patch, guarda cambios y marca aplicación pendiente. |
| `void (*AppApConfig_Get)(AppApConfig_t *out)` | Copia configuración AP deseada. |
| `AppApStatus_t (*AppApConfig_ParseIpv4)(const char *text, uint8_t out[4])` | Convierte IPv4 textual a cuatro octetos con reglas del módulo. |
| `uint16_t (*AppPreferences_ClampScreenLockTimeoutSec)(uint16_t timeoutSec)` | Limita timeout de bloqueo al rango de preferencias, en segundos. |
| `uint8_t (*AppPreferences_GetEspNotifWatchdogLimit)(void)` | Operación App Preferences Get Esp Notif Watchdog Limit de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*AppPreferences_GetScreenLockTimeoutSec)(void)` | Operación App Preferences Get Screen Lock Timeout Sec de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const char* (*AppPreferences_GetWifiApPassword)(void)` | Operación App Preferences Get Wifi Ap Password de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const char* (*AppPreferences_GetWifiApSsid)(void)` | Operación App Preferences Get Wifi Ap Ssid de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `bool (*AppPreferences_IsScreenLockEnabled)(void)` | Operación App Preferences Is Screen Lock Enabled de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `bool (*AppPreferences_IsWifiApFallbackEnabled)(void)` | Operación App Preferences Is Wifi Ap Fallback Enabled de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `AM_Result (*AppPreferences_SetScreenLockTimeoutSec)(uint16_t timeoutSec)` | Operación App Preferences Set Screen Lock Timeout Sec de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const BalanceControl_Status_t * (*BalanceControl_GetStatus)(const BalanceControl_Handle *balance)` | Devuelve vista prestada del estado de balance. |
| `float (*BalancePid_GetCurrentGainDraft)(void)` | Operación Balance Pid Get Current Gain Draft de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const char * (*BalancePid_GetCurrentGainLabel)(void)` | Operación Balance Pid Get Current Gain Label de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `ScreenCode_t (*BalancePid_GetCurrentGainScreenCode)(void)` | Operación Balance Pid Get Current Gain Screen Code de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint32_t (*BalancePid_GetLogAvailableBytes)(void)` | Operación Balance Pid Get Log Available Bytes de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*BalancePid_GetLogDurationSec)(void)` | Operación Balance Pid Get Log Duration Sec de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint32_t (*BalancePid_GetLogEstimatedBytes)(void)` | Operación Balance Pid Get Log Estimated Bytes de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ClickCancelar_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de Click Cancelar User Event Manager. |
| `uint8_t (*Dashboard_GetActionTimeoutContext)(uint32_t *startedMs, uint32_t *timeoutMs)` | Operación Dashboard Get Action Timeout Context de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*Dashboard_ResetModeConfirmState)(void)` | Operación Dashboard Reset Mode Confirm State de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const char* (*GenericConfirm_GetLine1)(void)` | Operación Generic Confirm Get Line1 de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const char* (*GenericConfirm_GetLine2)(void)` | Operación Generic Confirm Get Line2 de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint32_t (*GenericConfirm_GetScreenCode)(void)` | Operación Generic Confirm Get Screen Code de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*IRValues_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de IRValues User Event Manager. |
| `AM_Result (*MPU9250_GetLatest)(MPU9250_Handle_t *hmpu, MPU9250_Snapshot_t *out)` | Copia el último snapshot disponible a la salida del llamador. |
| `void (*MPUValues_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de MPUValues User Event Manager. |
| `ScreenCode_t (*MenuSys_GetCurrentScreenCode)(const MenuSystem *ms)` | Consulta identidad de pantalla del MenuSystem recibido. |
| `uint8_t (*MenuSys_GetCurrentScreenSource)(const MenuSystem *ms)` | Consulta el origen registrado de la pantalla actual. |
| `bool (*MenuSys_IsFavorite)(const MenuItem *item)` | Indica si el ítem está en la lista actual. |
| `bool (*MenuSys_IsFavoritesMenu)(const SubMenu *menu)` | Identifica el submenú de favoritos activo. |
| `bool (*MenuSys_IsItemVisible)(const MenuItem *item)` | Evalúa visibilityFn o considera visible al ítem sin condición. |
| `void (*MenuSys_NavigateBack)(MenuSystem *ms)` | Vuelve al padre o al dashboard según la posición actual. |
| `void (*MenuSys_RenderMenu)(MenuSystem *ms)` | Dibuja los ítems visibles con los callbacks del menú. |
| `void (*MenuSys_SetCurrentMenuScreenCode)(MenuSystem *ms)` | Operación Menu Sys Set Current Menu Screen Code de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*MenuSys_SetCurrentScreenCode)(MenuSystem *ms, ScreenCode_t screen_code, ScreenReportSource_t source)` | Actualiza identidad/origen de pantalla y marca reporte pendiente. |
| `const MotionControl_Status_t * (*MotionControl_GetStatus)(const MotionControl_Handle *motion)` | Devuelve vista del estado aplicado y del motivo de rechazo. |
| `uint8_t (*MotionControl_IsArmed)(const MotionControl_Handle *motion)` | Consulta el estado de armado del árbitro. |
| `const char * (*MotorSingleTest_GetDirectionLabel)(void)` | Operación Motor Single Test Get Direction Label de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const char * (*MotorSingleTest_GetLabel)(void)` | Operación Motor Single Test Get Label de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*MotorSingleTest_GetMaxPwm)(void)` | Operación Motor Single Test Get Max Pwm de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*MotorSingleTest_GetMinPwm)(void)` | Operación Motor Single Test Get Min Pwm de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*MotorSingleTest_GetPwm)(void)` | Operación Motor Single Test Get Pwm de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `ScreenCode_t (*MotorSingleTest_GetScreenCode)(void)` | Operación Motor Single Test Get Screen Code de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint8_t (*MotorSingleTest_GetSpeedPercentDisplay)(void)` | Operación Motor Single Test Get Speed Percent Display de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint8_t (*Motor_SpeedPercentToDisplay)(float speed_percent)` | Convierte el porcentaje a un valor entero para presentación. |
| `const char * (*OSK_GetText)(const OSK_State *state)` | Devuelve una vista del buffer terminado en cero. |
| `bool (*OSK_IsActive)(const OSK_State *state)` | Consulta si la sesión de teclado permanece abierta. |
| `void (*OSK_OnBack)(OSK_State *state)` | Procesa retroceso/cancelación de la sesión. |
| `void (*OSK_OnLeft)(OSK_State *state)` | Mueve selección hacia la opción anterior del layout. |
| `void (*OSK_OnRight)(OSK_State *state)` | Mueve selección hacia la opción siguiente del layout. |
| `void (*OSK_OnSelect)(OSK_State *state)` | Ejecuta la tecla seleccionada, edición o confirmación. |
| `void (*OSK_Open)(OSK_State *state, const OSK_Config *cfg)` | Inicializa una sesión de edición alfanumérica o numérica. |
| `void (*OSK_Render)(OSK_State *state)` | Dibuja el estado del teclado usando OSK_DrawHandle. |
| `bool (*Permission_IsRemoteNodeAuthorizedNow)(uint8_t source_node)` | Comprueba sesión remota con el reloj del servicio. |
| `bool (*Permission_Request)(PermissionId_t permission, PermissionContinuationFn on_granted, void *ctx)` | Solicita permiso local y registra continuación con su contexto. |
| `uint16_t (*Preferences_GetLineBlackWidthDraftMm)(void)` | Operación Preferences Get Line Black Width Draft Mm de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*Preferences_GetMotorMinPwmDraft)(void)` | Operación Preferences Get Motor Min Pwm Draft de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const char * (*Preferences_GetMotorMinPwmEditLabel)(void)` | Operación Preferences Get Motor Min Pwm Edit Label de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `ScreenCode_t (*Preferences_GetMotorMinPwmEditScreenCode)(void)` | Operación Preferences Get Motor Min Pwm Edit Screen Code de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*Preferences_GetOledRotationDraftDeg)(void)` | Operación Preferences Get Oled Rotation Draft Deg de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*Preferences_GetOledRotationMpuTestDeg)(void)` | Operación Preferences Get Oled Rotation Mpu Test Deg de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `int16_t (*Preferences_GetOledRotationMpuTestYawDeg)(void)` | Operación Preferences Get Oled Rotation Mpu Test Yaw Deg de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*Preferences_GetSystemActionTimeoutDraft)(void)` | Operación Preferences Get System Action Timeout Draft de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint16_t (*Preferences_GetUsbAlivePeriodDraftMs)(void)` | Operación Preferences Get Usb Alive Period Draft Ms de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint8_t (*Preferences_IsOledRotationMpuTestReady)(void)` | Operación Preferences Is Oled Rotation Mpu Test Ready de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*Preferences_UpdateOledRotationFromMpuTest)(void)` | Operación Preferences Update Oled Rotation From Mpu Test de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ReadOnly_UserEventManager)(UserEvent_t ev)` | Lee Only User Event Manager dentro de Contexto y servicios de pantallas. |
| `void (*RouteAlignment_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de Route Alignment User Event Manager. |
| `bool (*TCRT5000_EmittersGroup3IsOn)(TCRT5000_Handle *h)` | Consulta el estado lógico ordenado al grupo de tres emisores de línea. |
| `void (*TCRT5000_EmittersGroup3Off)(TCRT5000_Handle *h)` | Operación TCRT5000 Emitters Group3 Off de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*TCRT5000_EmittersGroup3On)(TCRT5000_Handle *h)` | Operación TCRT5000 Emitters Group3 On de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*TCRT5000_EmittersGroup3Set)(TCRT5000_Handle *h, bool enabled)` | Operación TCRT5000 Emitters Group3 Set de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*TCRT5000_EmittersGroup5Off)(TCRT5000_Handle *h)` | Operación TCRT5000 Emitters Group5 Off de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*TCRT5000_EmittersGroup5On)(TCRT5000_Handle *h)` | Operación TCRT5000 Emitters Group5 On de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*TCRT5000_EmittersGroup5Set)(TCRT5000_Handle *h, bool enabled)` | Operación TCRT5000 Emitters Group5 Set de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const TCRT5000_SensorDef_t * (*TCRT5000_GetLineSensorDef)(TCRT5000_LineSensor_t sensor)` | Devuelve la definición e índice ADC de un sensor de línea. |
| `const TCRT5000_SensorDef_t * (*TCRT5000_GetObjectSensorDef)(TCRT5000_ObjectSensor_t sensor)` | Devuelve la definición e índice ADC de un sensor de objetos. |
| `uint16_t (*TCRT5000_GetRaw)(const volatile uint16_t *sensorData, const TCRT5000_SensorDef_t *sensor)` | Lee el canal ADC indicado por una definición de sensor. |
| `uint16_t (*TCRT5000_GetRawById)(const volatile uint16_t *sensorData, TCRT5000_SensorId_t sensorId)` | Resuelve un ID lógico y devuelve su lectura raw. |
| `const char * (*TCRT5000_GetRouteAlignmentText)(TCRT5000_RouteAlignment_t alignment)` | Devuelve el texto de la clasificación de alineación. |
| `uint16_t (*TCRT5000_NormalizeForDisplay)(uint16_t raw, TCRT5000_Polarity_t polarity)` | Normaliza el valor ADC según polaridad para su presentación. |
| `void (*TCRT5000_ReadRouteSensorsEx)(const volatile uint16_t *sensorData, bool irLightsOn, const TCRT5000_LineConfig_t *config, TCRT5000_RouteSample_t *sample)` | Evalúa patrón, error lateral y alineación con configuración explícita. |
| `uint8_t (*UNER_App_GetBootloaderVersion)(uint32_t *version)` | Obtiene versión de bootloader si el contrato de arranque la hace disponible. |
| `void (*UNER_App_GetRouterPacketSnapshot)(uint8_t tx_view, uint8_t transport_id, UNER_RouterSnapshot *out)` | Operación UNER App Get Router Packet Snapshot de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `UNER_Status (*UNER_App_ReportNotificationChanged)(uint32_t screen_code, uint8_t source, uint16_t remaining_ms, uint16_t total_ms)` | Publica identidad y tiempos de notificación. |
| `void (*UnerRouterTest_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de Uner Router Test User Event Manager. |
| `uint8_t (*WiFiAtPassword_GetLength)(void)` | Operación Wi Fi At Password Get Length de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `char (*WiFiAtPassword_GetSelectedCharacter)(void)` | Operación Wi Fi At Password Get Selected Character de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*WiFiAtPassword_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de Wi Fi At Password User Event Manager. |
| `void (*WiFiDetails_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de Wi Fi Details User Event Manager. |
| `void (*WiFiSearch_UserEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de Wi Fi Search User Event Manager. |
| `WifiBridge_Backend (*WifiBridge_GetActiveBackend)(const WifiBridge_Handle *h)` | Consulta backend seleccionado por la detección. |
| `const char * (*WifiBridge_GetBackendLabel)(const WifiBridge_Handle *h)` | Operación Wifi Bridge Get Backend Label de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `const WifiBridge_Credentials * (*WifiBridge_GetRuntimeCredentials)(void)` | Operación Wifi Bridge Get Runtime Credentials de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `WifiBridge_Result (*WifiBridge_GetStatus)(WifiBridge_Handle *h)` | Operación Wifi Bridge Get Status de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint8_t (*WifiBridge_IsConnected)(const WifiBridge_Handle *h)` | Consulta estado de conectividad del bridge, distinto de sesión PIN. |
| `uint8_t (*WifiBridge_IsWaitingValidation)(WifiBridge_Handle *h)` | Indica si el backend espera finalizar una solicitud. |
| `WifiBridge_Result (*WifiBridge_ReportCarModeChanged)(WifiBridge_Handle *h, uint8_t mode)` | Operación Wifi Bridge Report Car Mode Changed de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `WifiBridge_Result (*WifiBridge_SendCommand)(WifiBridge_Handle *h, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Operación Wifi Bridge Send Command de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `WifiBridge_Result (*WifiBridge_SendEspReboot)(WifiBridge_Handle *h, uint8_t boot_mode)` | Operación Wifi Bridge Send Esp Reboot de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `WifiBridge_Result (*WifiBridge_StartAP)(WifiBridge_Handle *h)` | Operación Wifi Bridge Start AP de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `WifiBridge_Result (*WifiBridge_StartScan)(WifiBridge_Handle *h)` | Operación Wifi Bridge Start Scan de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `WifiBridge_Result (*WifiBridge_StopAP)(WifiBridge_Handle *h)` | Operación Wifi Bridge Stop AP de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*balancePidGainEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de balance Pid Gain Event Manager. |
| `void (*balancePidLogEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de balance Pid Log Event Manager. |
| `void (*balancePidStatusEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de balance Pid Status Event Manager. |
| `void (*dashboardEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de dashboard Event Manager. |
| `void (*dashboardQuickActionEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de dashboard Quick Action Event Manager. |
| `void (*lineBlackWidthEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de line Black Width Event Manager. |
| `void (*menuEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de menu Event Manager. |
| `void (*motorMinPwmEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de motor Min Pwm Event Manager. |
| `void (*motorSingleTestEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de motor Single Test Event Manager. |
| `void (*motorTestEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de motor Test Event Manager. |
| `void (*mpuCalibrationEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de mpu Calibration Event Manager. |
| `void (*oledCanvasEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de oled Canvas Event Manager. |
| `void (*oledRotationMpuTestEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de oled Rotation Mpu Test Event Manager. |
| `void (*oledRotationTestEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de oled Rotation Test Event Manager. |
| `void (*screenLockTimeoutEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de screen Lock Timeout Event Manager. |
| `void (*screenLockedEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de screen Locked Event Manager. |
| `void (*ssd1306_Clear)(void)` | Operación ssd1306 Clear de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawBitmap)(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)` | Operación ssd1306 Draw Bitmap de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawBitmap_MSB)(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)` | Operación ssd1306 Draw Bitmap MSB de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawHorizontalLine)(int16_t x, int16_t y, int16_t length)` | Operación ssd1306 Draw Horizontal Line de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawLine)(int16_t x0, int16_t y0, int16_t x1, int16_t y1)` | Operación ssd1306 Draw Line de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawLinearProgress)(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint32_t elapsed, uint32_t total)` | Operación ssd1306 Draw Linear Progress de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawPixel)(uint8_t x, uint8_t y)` | Operación ssd1306 Draw Pixel de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawRect)(int16_t x, int16_t y, int16_t width, int16_t height)` | Operación ssd1306 Draw Rect de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawVerticalLine)(int16_t x, int16_t y, int16_t length)` | Operación ssd1306 Draw Vertical Line de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_FillRect)(int16_t xMove, int16_t yMove, int16_t width, int16_t height)` | Operación ssd1306 Fill Rect de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint8_t (*ssd1306_GetDefaultContrast)(void)` | Operación ssd1306 Get Default Contrast de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `uint32_t (*ssd1306_GetErrorCount)(void)` | Operación ssd1306 Get Error Count de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `AM_Result (*ssd1306_PresentFramebuffer)(const uint8_t *framebuffer, uint16_t len)` | Operación ssd1306 Present Framebuffer de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `AM_Result (*ssd1306_RequestPageUpdate)(uint8_t page, I2C_RequestPrio prio, uint8_t keep_bus)` | Operación ssd1306 Request Page Update de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_SetColor)(SSD1306_COLOR color)` | Operación ssd1306 Set Color de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `AM_Result (*ssd1306_SetContrast)(uint8_t contrast)` | Operación ssd1306 Set Contrast de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_SetCursor)(uint8_t x, uint8_t y)` | Operación ssd1306 Set Cursor de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `AM_Result (*ssd1306_SetRotation)(OledRotation_t rotation)` | Operación ssd1306 Set Rotation de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*ssd1306_UpdateScreen)(void)` | Operación ssd1306 Update Screen de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `char (*ssd1306_UpdateScreenCompleted)(void)` | Operación ssd1306 Update Screen Completed de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `char (*ssd1306_WriteChar)(char ch, FontDef Font)` | Operación ssd1306 Write Char de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `char (*ssd1306_WriteString)(char* str, FontDef Font)` | Operación ssd1306 Write String de Contexto y servicios de pantallas; semántica fina pendiente de ampliación. |
| `void (*systemActionTimeoutEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de system Action Timeout Event Manager. |
| `void (*usbAlivePeriodEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de usb Alive Period Event Manager. |
| `void (*wifiEventManager)(UserEvent_t ev)` | Despacha los eventos de usuario de wifi Event Manager. |

### `Screens_Handle`

`f4:Core/Inc/screen_context.h:333`

| Atributo / firma | Función y precaución |
| --- | --- |
| `SystemHandle *system` | Referencia al estado compartido AutoMicro; introduce dependencia del modelo de aplicación. |
| `const ScreensServices *services` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `ScreensState state` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MenuSystem *menuSystem` | Referencia al menú compuesto por la aplicación. |
| `ENC_Handle_t *encoder` | Instancia concreta del encoder prestada a la UI. |
| `WifiBridge_Handle *wifiBridge` | Referencia al bridge concreto AutoMicro; su acceso está mediado por servicios. |
| `TCRT5000_Handle *irSensors` | Instancia concreta de sensores IR usada por operaciones de pantalla. |
| `MotorControl_Handle *motorTask` | Instancia de tracción para vistas de pruebas; no contiene telemetría de ruedas. |
| `BalanceControl_Handle *balanceControl` | Dato de estado/configuración `balanceControl`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MotionControl_Handle *motionControl` | Dato de estado/configuración `motionControl`; declaración estructural, interpretación específica pendiente de ampliar. |
| `LineFollowControl_Handle *lineFollowControl` | Dato de estado/configuración `lineFollowControl`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MPU9250_Handle_t *mpuTask` | Instancia MPU concreta; la lectura se canaliza por ScreensServices. |
| `SubMenu *mainMenu` | Dato de estado/configuración `mainMenu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SubMenu *submenu1` | Dato de estado/configuración `submenu1`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SubMenu *submenuESP` | Dato de estado/configuración `submenuESP`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SubMenu *submenuSensorsDisplay` | Dato de estado/configuración `submenuSensorsDisplay`; declaración estructural, interpretación específica pendiente de ampliar. |
| `SubMenu *wifiResultsMenu` | Dato de estado/configuración `wifiResultsMenu`; declaración estructural, interpretación específica pendiente de ampliar. |
| `MenuItem *wifiResultsItems` | Dato de estado/configuración `wifiResultsItems`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledHandle *oledHandle` | Estado de cola de notificaciones, distinto del framebuffer SSD1306. |
| `const char *buildDate, *buildTime` | Dato de estado/configuración `buildTime`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledNotificationData`

`f4:Core/Inc/oled_handle.h:20`

| Atributo / firma | Función y precaución |
| --- | --- |
| `RenderFunction renderFn` | Renderer que se ejecuta cuando se muestra este aviso. |
| `uint8_t simpleId` | Identidad de definición simple, separada del screen code. |
| `uint8_t clientNumber` | Número de cliente conservado al encolar el aviso. |
| `uint16_t timeoutTicks` | Ticks de 10 ms restantes del aviso. |
| `uint16_t totalTicks` | Duración total en ticks de 10 ms para progreso. |
| `bool needsFullRender` | Dato de estado/configuración `needsFullRender`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledNotificationHook onShow` | Hook sin argumentos al entrar en presentación del aviso. |
| `OledNotificationHook onHide` | Hook sin argumentos al salir del aviso. |

### `OledNotificationState`

`f4:Core/Inc/oled_handle.h:31`

| Atributo / firma | Función y precaución |
| --- | --- |
| `bool active` | Dato de estado/configuración `active`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledNotificationData queue[OLED_MAX_NOTIFICATIONS]` | Dato de estado/configuración `queue`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t head` | Dato de estado/configuración `head`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t tail` | Dato de estado/configuración `tail`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t count` | Dato de estado/configuración `count`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderFunction previousRenderFn` | Memoria de la observación anterior usada por la máquina de estados. |
| `UserEventManagerFn previousEventManagerFn` | Memoria de la observación anterior usada por la máquina de estados. |
| `bool previousAllowPeriodicRefresh` | Memoria de la observación anterior usada por la máquina de estados. |
| `bool previousRenderFlag` | Memoria de la observación anterior usada por la máquina de estados. |
| `ScreenCode_t previousScreenCode` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t previousScreenSource` | Memoria de la observación anterior usada por la máquina de estados. |
| `struct { estructura anidada } suspended` | Dato de estado/configuración `suspended`; declaración estructural, interpretación específica pendiente de ampliar. |

### `suspended`

`f4:Core/Inc/oled_handle.h:46`

| Atributo / firma | Función y precaución |
| --- | --- |
| `bool valid` | Dato de estado/configuración `valid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `OledNotificationData data` | Dato de estado/configuración `data`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledHandle`

`f4:Core/Inc/oled_handle.h:52`

| Atributo / firma | Función y precaución |
| --- | --- |
| `OledNotificationState notification` | Dato de estado/configuración `notification`; declaración estructural, interpretación específica pendiente de ampliar. |

### `OledNotificationHook`

`f4:Core/Inc/oled_handle.h:14`

Firma: `void (*OledNotificationHook)(void)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-screens_init_notificationwrappers"></a>`void Screens_Init_notificationWrappers(void)` | Inicializa el estado propio de wrappers de notificaciones en Screens_Context. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Inc/screen_context.h:359`; editorial |
| <a id="fn-screens_init_oled_canvas"></a>`void Screens_Init_oled_canvas(void)` | Inicializa el estado canvas del contexto activo de pantallas. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Inc/screen_context.h:362`; editorial |
| <a id="fn-screens_init_oled_utils"></a>`void Screens_Init_oled_utils(void)` | Inicializa estado de render, scroll y notificaciones del contexto activo. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Inc/screen_context.h:357`; editorial |
| <a id="fn-screens_init_osk_integration"></a>`void Screens_Init_osk_integration(void)` | Inicializa estado de adaptación de teclado en el contexto activo. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Inc/screen_context.h:360`; editorial |
| <a id="fn-screens_init_screenwrappers"></a>`void Screens_Init_screenWrappers(void)` | Inicializa estado de navegación y vistas preparadas por wrappers. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Inc/screen_context.h:358`; editorial |
| <a id="fn-screens_init_screen_lock"></a>`void Screens_Init_screen_lock(void)` | Inicializa estado de bloqueo del contexto activo de pantallas. Implementación no localizada en los archivos de esta ficha. | `f4:Core/Inc/screen_context.h:361`; editorial |
