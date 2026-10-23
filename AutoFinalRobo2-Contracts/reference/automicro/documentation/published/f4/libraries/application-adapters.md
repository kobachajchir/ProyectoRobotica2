# Adaptadores de hardware y servicios

ID: `f4:module/Core/Inc/app_stm32_ports.h` · Capa: **plataforma** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Empareja contratos neutrales con funciones y recursos concretos de AutoMicro/STM32.

Puertos GPIO/timer/PWM/ADC/I2C/UART/Flash y tablas de servicios UI, control, preferencias y Wi-Fi.

<a id="modularity"></a>
## Modularidad observada

Es la frontera de plataforma y composición; aquí las dependencias concretas son esperadas. Que una función sea inline no vuelve neutral su acceso HAL.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

AppI2cBinding conserva manager/id/port; los otros adaptadores usan recursos prestados o contexto activo. Las tablas static const declaradas en headers tienen copias por unidad de compilación que las incluya; su duración es estática.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Construir bindings antes de Init/Bind de los núcleos.
- App_I2cConnect empareja manager y device ID; los callbacks vuelven al mismo cliente.
- BindStm32 fija geometría Flash y operaciones; las macros históricas eligen el handle suministrado por aplicación.

## Trabajo en tándem

- app_control_ports conecta MPU → orientación y balance/PID/motores; no mueve esos objetos dentro del consumidor.
- App_I2cUs usa HAL_GetTick()*1000: expresa microsegundos con resolución de ms.
- Flash unlock/erase/program/lock permanece síncrono.

## Precauciones y límites

- No compartir contextos con tablas incompatibles: void* no tiene comprobación de tipo.
- El puerto I2C de dispositivo expone también hardware/hardware_ctx; no impone por sí solo que todo acceso pase por arbitraje.
- Campos públicos y macros permiten acoplamiento por estado aunque no haya llamada directa.

## Hallazgos vinculados

- [AMDOC-F4-008: El reloj now_us tiene resolución efectiva de milisegundos](../../../review/FINDINGS.md#amdoc-f4-008)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/app_stm32_ports.h` | `main.h`, `user_button.h`, `encoder.h`, `motor_control.h`, `tcrt5000.h` | `a1f608b90d7d62a7633ce382ccbbd25b5ef3b2bdc014123b709dc4142fd7c40a` |
| `Core/Inc/app_i2c_ports.h` | `main.h`, `i2c_manager.h` | `e5da177f556cb2b59d16af68089800fb82b425901b322d7ad2d5b3200b694585` |
| `Core/Inc/app_control_ports.h` | `pid_controller.h`, `mpu9250.h`, `motor_control.h`, `balance_control.h`, `motion_control.h`, `motion_estimator.h`, `tcrt5000.h` | `f81ecc18a357bf9ca15ba4d65263bd9632fde4b5806cb530dc54cc758feb5a04` |
| `Core/Inc/app_auxiliary_ports.h` | `globals.h`, `main.h`, `permissions.h`, `menu_favorites.h`, `menu_definitions.h`, `screenWrappers.h`, `oled_utils.h`, `app_display.h`, `app_preferences_binding.h`, `app_ap_config.h`, `app_wifi_binding.h`, `eventManagers.h`, `ui_event_router.h`, `string.h`, `permission_context.h`, `favorites_context.h`, `ap_context.h`, `uirouter_context.h` | `6b596812bed81c90f777686bf35a523e397ed00f07b1b984155b833fed7cee88` |
| `Core/Inc/app_display.h` | `ssd1306.h` | `975fc1cd040f98256bbdc0bafd53ff443ff72b8b1342e0f2b835474fbed91003` |
| `Core/Inc/app_screens_ports.h` | `globals.h`, `main.h`, `screen_context.h`, `osk.h`, `menu_definitions.h`, `string.h`, `eventManagers.h`, `app_tasks.h`, `app_wifi_binding.h`, `app_display.h`, `app_preferences_binding.h`, `app_ap_config.h`, `permissions.h`, `menu_favorites.h`, `uner_app.h` | `3e09a7282056c78cbe1dcb9eed6fa67df454a3681b0cb7e92e65816ab080e1b2` |
| `Core/Inc/app_menu_services.h` | `app_menu_binding.h`, `app_preferences_binding.h`, `app_wifi_binding.h` | `72fd6893d18ef52a4727efffa36b46b82c070083e39723d201b97751cf02003b` |
| `Core/Inc/app_menu_binding.h` | `menusystem.h` | `df0ea66d4aea5beeae4c845728b1b90c761e24aefe7a32dc11ada93b77d21d25` |
| `Core/Inc/app_preferences_binding.h` | `app_preferences.h` | `7c94fc3c89c98d939a1d86a94735e31572944510813a68867a90e373f1db493d` |
| `Core/Inc/app_preferences_stm32.h` | `main.h`, `app_preferences_binding.h`, `app_ap_config.h`, `automicro_boot_contract.h` | `831d835ff2cd470176b4e383255db3e2c9a07e598aae27e50e8d016005b098d9` |
| `Core/Inc/app_wifi_binding.h` | `wifi_bridge.h` | `47c2a40f194236a3aa7bf6b93ead47d396bf50e28c59f389ef6ec1d9fcb2a08a` |
| `Core/Inc/app_wifi_ports.h` | `main.h`, `wifi_bridge.h`, `uner_app.h`, `permissions.h`, `app_preferences_binding.h`, `app_ap_config.h`, `screenWrappers.h` | `8b3bf59e019775bf724ab29c13e18de5471db871c4fde71eff950130ee17ac93` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `AppGpioContext`

`f4:Core/Inc/app_stm32_ports.h:8`

| Atributo / firma | Función y precaución |
| --- | --- |
| `GPIO_TypeDef *gpio` | Dato de estado/configuración `gpio`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint16_t pin` | Dato de estado/configuración `pin`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppEncoderContext`

`f4:Core/Inc/app_stm32_ports.h:9`

| Atributo / firma | Función y precaución |
| --- | --- |
| `TIM_HandleTypeDef *timer` | Dato de estado/configuración `timer`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppGpioContext button` | Dato de estado/configuración `button`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppIrContext`

`f4:Core/Inc/app_stm32_ports.h:32`

| Atributo / firma | Función y precaución |
| --- | --- |
| `AppGpioContext objects, line` | Dato de estado/configuración `line`; declaración estructural, interpretación específica pendiente de ampliar. |
| `ADC_HandleTypeDef *adc` | Dato de estado/configuración `adc`; declaración estructural, interpretación específica pendiente de ampliar. |
| `volatile uint16_t *samples` | Dato de estado/configuración `samples`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t count` | Dato de estado/configuración `count`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppI2cBinding`

`f4:Core/Inc/app_i2c_ports.h:16`

| Atributo / firma | Función y precaución |
| --- | --- |
| `I2C_ManagerHandle *manager` | Dato de estado/configuración `manager`; declaración estructural, interpretación específica pendiente de ampliar. |
| `I2C_DeviceID id` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AM_I2C_DevicePort port` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-appap_apppreferences_get"></a>`static inline const AppPreferences_t * AppAp_AppPreferences_Get(void)` | Adapta la llamada a `AppPreferences_Get` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:60`; structural |
| <a id="fn-appap_apppreferences_getwifiapconfig"></a>`static inline void AppAp_AppPreferences_GetWifiApConfig(AppApConfig_t *out)` | Adapta la llamada a `AppPreferences_GetWifiApConfig` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:63`; structural |
| <a id="fn-appap_apppreferences_setwifiapapplypending"></a>`static inline AM_Result AppAp_AppPreferences_SetWifiApApplyPending(bool pending)` | Adapta la llamada a `AppPreferences_SetWifiApApplyPending` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:61`; structural |
| <a id="fn-appap_apppreferences_setwifiapconfig"></a>`static inline AM_Result AppAp_AppPreferences_SetWifiApConfig(const AppApConfig_t *config, bool applyPending)` | Adapta la llamada a `AppPreferences_SetWifiApConfig` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:62`; structural |
| <a id="fn-appauxiliarybind"></a>`static inline void AppAuxiliaryBind(void)` | Conecta contextos/servicios de favoritos, permisos, AP y router. | `f4:Core/Inc/app_auxiliary_ports.h:91`; editorial |
| <a id="fn-appfavorites_apppreferences_applypatch"></a>`static inline AM_Result AppFavorites_AppPreferences_ApplyPatch(const AppPreferencesPatch_t *patch, AppPreferencesFieldMask_t *changedFields)` | Adapta la llamada a `AppPreferences_ApplyPatch` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:47`; structural |
| <a id="fn-appfavorites_apppreferences_get"></a>`static inline const AppPreferences_t * AppFavorites_AppPreferences_Get(void)` | Adapta la llamada a `AppPreferences_Get` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:46`; structural |
| <a id="fn-appmenuavailable"></a>`static inline bool AppMenuAvailable(void *ctx, const MenuItem *item)` | Coordina `AppPreferences_GetExtProfile`, `MENU_EXT_MASK` para Adaptadores de hardware y servicios. | `f4:Core/Inc/app_menu_services.h:6`; structural |
| <a id="fn-appmenureport"></a>`static inline bool AppMenuReport(void *ctx, uint32_t code, uint8_t source)` | Adapta la llamada a `WifiBridge_ReportScreenChanged` con el contexto de esta integración. | `f4:Core/Inc/app_menu_services.h:11`; structural |
| <a id="fn-appmenuselection"></a>`static inline bool AppMenuSelection(void *ctx, uint32_t code, uint8_t index, uint8_t count, uint8_t source)` | Adapta la llamada a `WifiBridge_ReportMenuSelectionChanged` con el contexto de esta integración. | `f4:Core/Inc/app_menu_services.h:14`; structural |
| <a id="fn-apppermissionnodevalid"></a>`static inline bool AppPermissionNodeValid(uint8_t node)` | Aplica la política de aplicación sobre nodos remotos admitidos. | `f4:Core/Inc/app_auxiliary_ports.h:27`; editorial |
| <a id="fn-apppermission_apppreferences_ispinlockenabled"></a>`static inline bool AppPermission_AppPreferences_IsPinLockEnabled(void)` | Adapta la llamada a `AppPreferences_IsPinLockEnabled` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:23`; structural |
| <a id="fn-apppermission_apppreferences_verifyauthpin"></a>`static inline bool AppPermission_AppPreferences_VerifyAuthPin(const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS])` | Adapta la llamada a `AppPreferences_VerifyAuthPin` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:25`; structural |
| <a id="fn-apppermission_ssd1306_clear"></a>`static inline void AppPermission_ssd1306_Clear(void)` | Adapta la llamada a `ssd1306_Clear` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:26`; structural |
| <a id="fn-apppermission_ssd1306_drawbitmap"></a>`static inline void AppPermission_ssd1306_DrawBitmap(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)` | Adapta la llamada a `ssd1306_DrawBitmap` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:18`; structural |
| <a id="fn-apppermission_ssd1306_drawrect"></a>`static inline void AppPermission_ssd1306_DrawRect(int16_t x, int16_t y, int16_t width, int16_t height)` | Adapta la llamada a `ssd1306_DrawRect` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:19`; structural |
| <a id="fn-apppermission_ssd1306_fillrect"></a>`static inline void AppPermission_ssd1306_FillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height)` | Adapta la llamada a `ssd1306_FillRect` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:24`; structural |
| <a id="fn-apppermission_ssd1306_setcolor"></a>`static inline void AppPermission_ssd1306_SetColor(SSD1306_COLOR color)` | Adapta la llamada a `ssd1306_SetColor` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:22`; structural |
| <a id="fn-apppermission_ssd1306_setcursor"></a>`static inline void AppPermission_ssd1306_SetCursor(uint8_t x, uint8_t y)` | Adapta la llamada a `ssd1306_SetCursor` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:20`; structural |
| <a id="fn-apppermission_ssd1306_writestring"></a>`static inline char AppPermission_ssd1306_WriteString(char* str, FontDef Font)` | Adapta la llamada a `ssd1306_WriteString` con el contexto de esta integración. | `f4:Core/Inc/app_auxiliary_ports.h:21`; structural |
| <a id="fn-apppreferences_bindstm32"></a>`static inline void AppPreferences_BindStm32(void)` | Conecta geometría Flash activa/legado y operaciones de almacenamiento. | `f4:Core/Inc/app_preferences_stm32.h:31`; editorial |
| <a id="fn-apppreferences_legacysectorisoutsideimage"></a>`static inline bool AppPreferences_LegacySectorIsOutsideImage(void)` | Comprueba si el sector legado puede leerse sin invadir la imagen de aplicación. | `f4:Core/Inc/app_preferences_stm32.h:20`; editorial |
| <a id="fn-appscreensbind"></a>`static inline void AppScreensBind(Screens_Handle *h)` | Instala la tabla de servicios y contexto de pantallas. | `f4:Core/Inc/app_screens_ports.h:219`; editorial |
| <a id="fn-appscreensroutelights"></a>`static inline bool AppScreensRouteLights(void)` | Adapta la llamada a `HAL_GPIO_ReadPin` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:18`; structural |
| <a id="fn-appscreens_apppreferences_clampscreenlocktimeoutsec"></a>`static inline uint16_t AppScreens_AppPreferences_ClampScreenLockTimeoutSec(uint16_t timeoutSec)` | Adapta la llamada a `AppPreferences_ClampScreenLockTimeoutSec` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:19`; structural |
| <a id="fn-appscreens_apppreferences_getespnotifwatchdoglimit"></a>`static inline uint8_t AppScreens_AppPreferences_GetEspNotifWatchdogLimit(void)` | Adapta la llamada a `AppPreferences_GetEspNotifWatchdogLimit` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:20`; structural |
| <a id="fn-appscreens_apppreferences_getscreenlocktimeoutsec"></a>`static inline uint16_t AppScreens_AppPreferences_GetScreenLockTimeoutSec(void)` | Adapta la llamada a `AppPreferences_GetScreenLockTimeoutSec` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:21`; structural |
| <a id="fn-appscreens_apppreferences_isscreenlockenabled"></a>`static inline bool AppScreens_AppPreferences_IsScreenLockEnabled(void)` | Adapta la llamada a `AppPreferences_IsScreenLockEnabled` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:24`; structural |
| <a id="fn-appscreens_apppreferences_iswifiapfallbackenabled"></a>`static inline bool AppScreens_AppPreferences_IsWifiApFallbackEnabled(void)` | Adapta la llamada a `AppPreferences_IsWifiApFallbackEnabled` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:25`; structural |
| <a id="fn-appscreens_apppreferences_setscreenlocktimeoutsec"></a>`static inline AM_Result AppScreens_AppPreferences_SetScreenLockTimeoutSec(uint16_t timeoutSec)` | Adapta la llamada a `AppPreferences_SetScreenLockTimeoutSec` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:26`; structural |
| <a id="fn-appscreens_wifibridge_getruntimecredentials"></a>`static inline const WifiBridge_Credentials * AppScreens_WifiBridge_GetRuntimeCredentials(void)` | Adapta la llamada a `WifiBridge_GetRuntimeCredentials` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:27`; structural |
| <a id="fn-appscreens_ssd1306_clear"></a>`static inline void AppScreens_ssd1306_Clear(void)` | Adapta la llamada a `ssd1306_Clear` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:28`; structural |
| <a id="fn-appscreens_ssd1306_drawbitmap"></a>`static inline void AppScreens_ssd1306_DrawBitmap(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)` | Adapta la llamada a `ssd1306_DrawBitmap` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:29`; structural |
| <a id="fn-appscreens_ssd1306_drawbitmap_msb"></a>`static inline void AppScreens_ssd1306_DrawBitmap_MSB(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)` | Adapta la llamada a `ssd1306_DrawBitmap_MSB` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:30`; structural |
| <a id="fn-appscreens_ssd1306_drawhorizontalline"></a>`static inline void AppScreens_ssd1306_DrawHorizontalLine(int16_t x, int16_t y, int16_t length)` | Adapta la llamada a `ssd1306_DrawHorizontalLine` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:31`; structural |
| <a id="fn-appscreens_ssd1306_drawline"></a>`static inline void AppScreens_ssd1306_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1)` | Adapta la llamada a `ssd1306_DrawLine` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:32`; structural |
| <a id="fn-appscreens_ssd1306_drawlinearprogress"></a>`static inline void AppScreens_ssd1306_DrawLinearProgress(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint32_t elapsed, uint32_t total)` | Adapta la llamada a `ssd1306_DrawLinearProgress` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:33`; structural |
| <a id="fn-appscreens_ssd1306_drawpixel"></a>`static inline void AppScreens_ssd1306_DrawPixel(uint8_t x, uint8_t y)` | Adapta la llamada a `ssd1306_DrawPixel` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:39`; structural |
| <a id="fn-appscreens_ssd1306_drawrect"></a>`static inline void AppScreens_ssd1306_DrawRect(int16_t x, int16_t y, int16_t width, int16_t height)` | Adapta la llamada a `ssd1306_DrawRect` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:40`; structural |
| <a id="fn-appscreens_ssd1306_drawverticalline"></a>`static inline void AppScreens_ssd1306_DrawVerticalLine(int16_t x, int16_t y, int16_t length)` | Adapta la llamada a `ssd1306_DrawVerticalLine` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:41`; structural |
| <a id="fn-appscreens_ssd1306_fillrect"></a>`static inline void AppScreens_ssd1306_FillRect(int16_t xMove, int16_t yMove, int16_t width, int16_t height)` | Adapta la llamada a `ssd1306_FillRect` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:42`; structural |
| <a id="fn-appscreens_ssd1306_getdefaultcontrast"></a>`static inline uint8_t AppScreens_ssd1306_GetDefaultContrast(void)` | Adapta la llamada a `ssd1306_GetDefaultContrast` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:43`; structural |
| <a id="fn-appscreens_ssd1306_geterrorcount"></a>`static inline uint32_t AppScreens_ssd1306_GetErrorCount(void)` | Adapta la llamada a `ssd1306_GetErrorCount` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:44`; structural |
| <a id="fn-appscreens_ssd1306_presentframebuffer"></a>`static inline AM_Result AppScreens_ssd1306_PresentFramebuffer(const uint8_t *framebuffer, uint16_t len)` | Adapta la llamada a `ssd1306_PresentFramebuffer` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:45`; structural |
| <a id="fn-appscreens_ssd1306_requestpageupdate"></a>`static inline AM_Result AppScreens_ssd1306_RequestPageUpdate(uint8_t page, I2C_RequestPrio prio, uint8_t keep_bus)` | Adapta la llamada a `ssd1306_RequestPageUpdate` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:47`; structural |
| <a id="fn-appscreens_ssd1306_setcolor"></a>`static inline void AppScreens_ssd1306_SetColor(SSD1306_COLOR color)` | Adapta la llamada a `ssd1306_SetColor` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:50`; structural |
| <a id="fn-appscreens_ssd1306_setcontrast"></a>`static inline AM_Result AppScreens_ssd1306_SetContrast(uint8_t contrast)` | Adapta la llamada a `ssd1306_SetContrast` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:51`; structural |
| <a id="fn-appscreens_ssd1306_setcursor"></a>`static inline void AppScreens_ssd1306_SetCursor(uint8_t x, uint8_t y)` | Adapta la llamada a `ssd1306_SetCursor` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:52`; structural |
| <a id="fn-appscreens_ssd1306_setrotation"></a>`static inline AM_Result AppScreens_ssd1306_SetRotation(OledRotation_t rotation)` | Adapta la llamada a `ssd1306_SetRotation` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:53`; structural |
| <a id="fn-appscreens_ssd1306_updatescreen"></a>`static inline void AppScreens_ssd1306_UpdateScreen(void)` | Adapta la llamada a `ssd1306_UpdateScreen` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:54`; structural |
| <a id="fn-appscreens_ssd1306_updatescreencompleted"></a>`static inline char AppScreens_ssd1306_UpdateScreenCompleted(void)` | Adapta la llamada a `ssd1306_UpdateScreenCompleted` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:55`; structural |
| <a id="fn-appscreens_ssd1306_writechar"></a>`static inline char AppScreens_ssd1306_WriteChar(char ch, FontDef Font)` | Adapta la llamada a `ssd1306_WriteChar` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:56`; structural |
| <a id="fn-appscreens_ssd1306_writestring"></a>`static inline char AppScreens_ssd1306_WriteString(char* str, FontDef Font)` | Adapta la llamada a `ssd1306_WriteString` con el contexto de esta integración. | `f4:Core/Inc/app_screens_ports.h:57`; structural |
| <a id="fn-appwifiidle"></a>`static inline void AppWifiIdle(void *ctx)` | Adapta la llamada a `__HAL_UART_ENABLE_IT` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:13`; structural |
| <a id="fn-appwifiready"></a>`static inline bool AppWifiReady(void *ctx)` | Consulta disponibilidad de recepción del recurso UART del bridge. | `f4:Core/Inc/app_wifi_ports.h:14`; editorial |
| <a id="fn-appwifiremaining"></a>`static inline uint16_t AppWifiRemaining(void *ctx)` | Adapta la llamada a `__HAL_DMA_GET_COUNTER` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:15`; structural |
| <a id="fn-appwifirx"></a>`static inline AM_Result AppWifiRx(void *ctx,uint8_t *data,uint16_t len)` | Adapta la llamada a `HAL_UART_Receive_DMA` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:11`; structural |
| <a id="fn-appwifitick"></a>`static inline uint32_t AppWifiTick(void *ctx)` | Adapta la llamada a `HAL_GetTick` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:10`; structural |
| <a id="fn-appwifitx"></a>`static inline AM_Result AppWifiTx(void *ctx,const uint8_t *data,uint16_t len)` | Adapta la llamada a `HAL_UART_Transmit_DMA` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:12`; structural |
| <a id="fn-appwifi_appapconfig_get"></a>`static inline void AppWifi_AppApConfig_Get(void *ctx, AppApConfig_t *out)` | Adapta la llamada a `AppApConfig_Get` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:17`; structural |
| <a id="fn-appwifi_appapconfig_onespapplyresponse"></a>`static inline void AppWifi_AppApConfig_OnEspApplyResponse(void *ctx, uint8_t status)` | Adapta la llamada a `AppApConfig_OnEspApplyResponse` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:18`; structural |
| <a id="fn-appwifi_apppreferences_getwifiatprotocol"></a>`static inline uint8_t AppWifi_AppPreferences_GetWifiAtProtocol(void *ctx)` | Adapta la llamada a `AppPreferences_GetWifiAtProtocol` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:19`; structural |
| <a id="fn-appwifi_permission_revokeremotenode"></a>`static inline void AppWifi_Permission_RevokeRemoteNode(void *ctx, uint8_t source_node)` | Adapta la llamada a `Permission_RevokeRemoteNode` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:20`; structural |
| <a id="fn-appwifi_uner_app_feeduart1bytes"></a>`static inline void AppWifi_UNER_App_FeedUart1Bytes(void *ctx, const uint8_t *data, uint16_t len)` | Adapta la llamada a `UNER_App_FeedUart1Bytes` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:21`; structural |
| <a id="fn-appwifi_uner_app_getbinaryasset"></a>`static inline uint8_t AppWifi_UNER_App_GetBinaryAsset(void *ctx, uint8_t asset_id, BinaryAssetView_t *asset)` | Adapta la llamada a `UNER_App_GetBinaryAsset` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:22`; structural |
| <a id="fn-appwifi_uner_app_getwaitingcommandid"></a>`static inline uint8_t AppWifi_UNER_App_GetWaitingCommandId(void *ctx)` | Adapta la llamada a `UNER_App_GetWaitingCommandId` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:23`; structural |
| <a id="fn-appwifi_uner_app_init"></a>`static inline UNER_Status AppWifi_UNER_App_Init(void *ctx, WifiBridge_Handle *bridge)` | Adapta la llamada a `UNER_App_Init` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:24`; structural |
| <a id="fn-appwifi_uner_app_iswaitingvalidation"></a>`static inline uint8_t AppWifi_UNER_App_IsWaitingValidation(void *ctx)` | Adapta la llamada a `UNER_App_IsWaitingValidation` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:25`; structural |
| <a id="fn-appwifi_uner_app_notifyuart1rx"></a>`static inline void AppWifi_UNER_App_NotifyUart1Rx(void *ctx)` | Adapta la llamada a `UNER_App_NotifyUart1Rx` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:26`; structural |
| <a id="fn-appwifi_uner_app_onatnetworkdisconnected"></a>`static inline void AppWifi_UNER_App_OnAtNetworkDisconnected(void *ctx)` | Adapta la llamada a `UNER_App_OnAtNetworkDisconnected` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:27`; structural |
| <a id="fn-appwifi_uner_app_onuart1txcomplete"></a>`static inline void AppWifi_UNER_App_OnUart1TxComplete(void *ctx)` | Adapta la llamada a `UNER_App_OnUart1TxComplete` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:28`; structural |
| <a id="fn-appwifi_uner_app_poll"></a>`static inline void AppWifi_UNER_App_Poll(void *ctx)` | Adapta la llamada a `UNER_App_Poll` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:29`; structural |
| <a id="fn-appwifi_uner_app_reportcarmodechanged"></a>`static inline UNER_Status AppWifi_UNER_App_ReportCarModeChanged(void *ctx, uint8_t mode)` | Adapta la llamada a `UNER_App_ReportCarModeChanged` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:30`; structural |
| <a id="fn-appwifi_uner_app_reportmenuselectionchanged"></a>`static inline UNER_Status AppWifi_UNER_App_ReportMenuSelectionChanged(void *ctx, uint32_t screen_code, uint8_t selected_index, uint8_t item_count, uint8_t source)` | Adapta la llamada a `UNER_App_ReportMenuSelectionChanged` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:31`; structural |
| <a id="fn-appwifi_uner_app_reportscreenchanged"></a>`static inline UNER_Status AppWifi_UNER_App_ReportScreenChanged(void *ctx, uint32_t screen_code, uint8_t source)` | Adapta la llamada a `UNER_App_ReportScreenChanged` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:35`; structural |
| <a id="fn-appwifi_uner_app_requestbinaryasset"></a>`static inline UNER_Status AppWifi_UNER_App_RequestBinaryAsset(void *ctx, uint8_t asset_id)` | Adapta la llamada a `UNER_App_RequestBinaryAsset` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:36`; structural |
| <a id="fn-appwifi_uner_app_requestwifinetworkdetail"></a>`static inline UNER_Status AppWifi_UNER_App_RequestWifiNetworkDetail(void *ctx, const char *ssid)` | Adapta la llamada a `UNER_App_RequestWifiNetworkDetail` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:37`; structural |
| <a id="fn-appwifi_uner_app_resetuart1rxpos"></a>`static inline void AppWifi_UNER_App_ResetUart1RxPos(void *ctx)` | Adapta la llamada a `UNER_App_ResetUart1RxPos` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:38`; structural |
| <a id="fn-appwifi_uner_app_sendcommand"></a>`static inline UNER_Status AppWifi_UNER_App_SendCommand(void *ctx, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Adapta la llamada a `UNER_App_SendCommand` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:39`; structural |
| <a id="fn-appwifi_uner_app_sendespreboot"></a>`static inline UNER_Status AppWifi_UNER_App_SendEspReboot(void *ctx, uint8_t boot_mode)` | Adapta la llamada a `UNER_App_SendEspReboot` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:40`; structural |
| <a id="fn-appwifi_uner_app_sendwificredentialrequest"></a>`static inline UNER_Status AppWifi_UNER_App_SendWifiCredentialRequest(void *ctx, const char *ssid)` | Adapta la llamada a `UNER_App_SendWifiCredentialRequest` con el contexto de esta integración. | `f4:Core/Inc/app_wifi_ports.h:41`; structural |
| <a id="fn-app_balanceenable"></a>`static inline void App_BalanceEnable(void *p,uint8_t v)` | Adapta la llamada a `BalanceControl_SetEnabled` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:28`; structural |
| <a id="fn-app_balanceoutput"></a>`static inline float App_BalanceOutput(const void *p)` | Adapta la llamada a `BalanceControl_GetOutputPercent` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:31`; structural |
| <a id="fn-app_balancereset"></a>`static inline void App_BalanceReset(void *p)` | Adapta la llamada a `BalanceControl_Reset` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:29`; structural |
| <a id="fn-app_balancetarget"></a>`static inline void App_BalanceTarget(void *p,float v)` | Adapta la llamada a `BalanceControl_SetTargetPitchDeg` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:27`; structural |
| <a id="fn-app_balanceupdate"></a>`static inline AM_Result App_BalanceUpdate(void *p,float v,uint32_t ms)` | Adapta la llamada a `BalanceControl_Update` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:30`; structural |
| <a id="fn-app_driveapply"></a>`static inline void App_DriveApply(void *p,float l,float r)` | Adapta la llamada a `MotorDrive_SetSignedPercent` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:35`; structural |
| <a id="fn-app_drivebrake"></a>`static inline void App_DriveBrake(void *p)` | Adapta la llamada a `Motor_Brake` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:34`; structural |
| <a id="fn-app_drivestop"></a>`static inline void App_DriveStop(void *p)` | Adapta la llamada a `MotorDrive_StopAll` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:33`; structural |
| <a id="fn-app_encoderbutton"></a>`static inline bool App_EncoderButton(void *ctx)` | Adapta la llamada a `App_ReadLevel` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:22`; structural |
| <a id="fn-app_encodercount"></a>`static inline uint16_t App_EncoderCount(void *ctx)` | Adapta la llamada a `__HAL_TIM_GET_COUNTER` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:13`; structural |
| <a id="fn-app_encoderreset"></a>`static inline void App_EncoderReset(void *ctx)` | Adapta la llamada a `__HAL_TIM_SET_COUNTER` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:16`; structural |
| <a id="fn-app_encoderstart"></a>`static inline void App_EncoderStart(void *ctx)` | Adapta la llamada a `HAL_TIM_Encoder_Start` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:19`; structural |
| <a id="fn-app_flasherase"></a>`static inline AM_Result App_FlashErase(void *ctx)` | Adapta la llamada a `HAL_FLASHEx_Erase` con el contexto de esta integración. | `f4:Core/Inc/app_preferences_stm32.h:10`; structural |
| <a id="fn-app_flashlock"></a>`static inline AM_Result App_FlashLock(void *ctx)` | Adapta la llamada a `HAL_FLASH_Lock` con el contexto de esta integración. | `f4:Core/Inc/app_preferences_stm32.h:9`; structural |
| <a id="fn-app_flashprogram"></a>`static inline AM_Result App_FlashProgram(void *ctx,uint32_t address,uint32_t word)` | Adapta la llamada a `HAL_FLASH_Program` con el contexto de esta integración. | `f4:Core/Inc/app_preferences_stm32.h:16`; structural |
| <a id="fn-app_flashunlock"></a>`static inline AM_Result App_FlashUnlock(void *ctx)` | Adapta la llamada a `HAL_FLASH_Unlock` con el contexto de esta integración. | `f4:Core/Inc/app_preferences_stm32.h:8`; structural |
| <a id="fn-app_flashview"></a>`static inline const void *App_FlashView(void *ctx,uint32_t address,size_t len)` | Devuelve vista de memoria Flash para el rango solicitado al adaptador. | `f4:Core/Inc/app_preferences_stm32.h:7`; editorial |
| <a id="fn-app_i2cbind"></a>`static inline AM_Result App_I2cBind(void *ctx,uint16_t a,uint8_t p,void *c,I2C_OnGrantedCb g,I2C_OnTxDoneCb tx,I2C_OnRxDoneCb rx,I2C_OnErrorCb e)` | Adapta la llamada a `I2C_Manager_RegisterDevice` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:17`; structural |
| <a id="fn-app_i2cconnect"></a>`static inline void App_I2cConnect(AppI2cBinding *b,I2C_ManagerHandle *m,I2C_DeviceID id)` | Construye el puerto de un dispositivo enlazado al manager y a su ID lógico. | `f4:Core/Inc/app_i2c_ports.h:24`; editorial |
| <a id="fn-app_i2cdelay"></a>`static inline void App_I2cDelay(void *ctx,uint32_t ms)` | Adapta la llamada a `HAL_Delay` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:7`; structural |
| <a id="fn-app_i2cms"></a>`static inline uint32_t App_I2cMs(void *ctx)` | Adapta la llamada a `HAL_GetTick` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:5`; structural |
| <a id="fn-app_i2cowner"></a>`static inline bool App_I2cOwner(void *ctx)` | Adapta la llamada a `I2C_Manager_GetOwner` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:22`; structural |
| <a id="fn-app_i2cpolicy"></a>`static inline I2C_PolicyMode App_I2cPolicy(void *ctx)` | Adapta la llamada a `I2C_Manager_GetPolicy` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:23`; structural |
| <a id="fn-app_i2cprobe"></a>`static inline AM_Result App_I2cProbe(void *ctx,uint16_t a,uint32_t n,uint32_t t)` | Adapta la llamada a `HAL_I2C_IsDeviceReady` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:9`; structural |
| <a id="fn-app_i2cread"></a>`static inline AM_Result App_I2cRead(void *ctx,uint16_t a,uint16_t r,uint16_t rs,uint8_t *p,uint16_t n,uint32_t t)` | Adapta la llamada a `HAL_I2C_Mem_Read` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:11`; structural |
| <a id="fn-app_i2cready"></a>`static inline bool App_I2cReady(void *ctx)` | Adapta la llamada a `HAL_I2C_GetState` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:8`; structural |
| <a id="fn-app_i2crelease"></a>`static inline AM_Result App_I2cRelease(void *ctx)` | Adapta la llamada a `I2C_Manager_ReleaseBus` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:21`; structural |
| <a id="fn-app_i2crequest"></a>`static inline AM_Result App_I2cRequest(void *ctx,I2C_RequestPrio p)` | Adapta la llamada a `I2C_Manager_RequestBusPrio` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:20`; structural |
| <a id="fn-app_i2crx"></a>`static inline AM_Result App_I2cRx(void *ctx,uint16_t a,uint8_t *p,uint16_t n)` | Adapta la llamada a `HAL_I2C_Master_Receive_DMA` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:14`; structural |
| <a id="fn-app_i2ctx"></a>`static inline AM_Result App_I2cTx(void *ctx,uint16_t a,uint8_t *p,uint16_t n)` | Adapta la llamada a `HAL_I2C_Master_Transmit_DMA` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:13`; structural |
| <a id="fn-app_i2cus"></a>`static inline uint32_t App_I2cUs(void *ctx)` | Entrega HAL_GetTick multiplicado por 1000, con resolución real de milisegundos. | `f4:Core/Inc/app_i2c_ports.h:6`; editorial |
| <a id="fn-app_i2cwrite"></a>`static inline AM_Result App_I2cWrite(void *ctx,uint16_t a,uint16_t r,uint16_t rs,uint8_t *p,uint16_t n,uint32_t t)` | Adapta la llamada a `HAL_I2C_Mem_Write` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:10`; structural |
| <a id="fn-app_i2cwritedma"></a>`static inline AM_Result App_I2cWriteDma(void *ctx,uint16_t a,uint16_t r,uint16_t rs,uint8_t *p,uint16_t n)` | Adapta la llamada a `HAL_I2C_Mem_Write_DMA` con el contexto de esta integración. | `f4:Core/Inc/app_i2c_ports.h:12`; structural |
| <a id="fn-app_iremitters"></a>`static inline void App_IrEmitters(void *ctx,uint8_t group,bool enabled)` | Adapta la llamada a `HAL_GPIO_WritePin` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:33`; structural |
| <a id="fn-app_irinitialize"></a>`static inline AM_Result App_IrInitialize(void *ctx)` | Adapta la llamada a `App_IrEmitters` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:38`; structural |
| <a id="fn-app_irstartsampling"></a>`static inline AM_Result App_IrStartSampling(AppIrContext *p)` | Adapta la llamada a `HAL_ADC_Start_DMA` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:42`; structural |
| <a id="fn-app_linearmed"></a>`static inline uint8_t App_LineArmed(const void *p)` | Adapta la llamada a `MotionControl_IsArmed` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:38`; structural |
| <a id="fn-app_linecommand"></a>`static inline AM_Result App_LineCommand(void *p,const MotionControl_Command_t *c,uint32_t ms)` | Adapta la llamada a `MotionControl_SetCommand` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:37`; structural |
| <a id="fn-app_lineemitters"></a>`static inline bool App_LineEmitters(void *p)` | Adapta la llamada a `TCRT5000_EmittersGroup3IsOn` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:40`; structural |
| <a id="fn-app_lineestimate"></a>`static inline const MotionEstimator_Status_t *App_LineEstimate(const void *p)` | Adapta la llamada a `MotionEstimator_GetStatus` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:39`; structural |
| <a id="fn-app_mpuattitudems"></a>`static inline uint32_t App_MpuAttitudeMs(const void *p)` | Consulta la marca de última actitud calculada en la instancia MPU. | `f4:Core/Inc/app_control_ports.h:23`; editorial |
| <a id="fn-app_mpucalibrating"></a>`static inline uint8_t App_MpuCalibrating(const void *p)` | Adapta la llamada a `MPU9250_IsCalibrating` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:19`; structural |
| <a id="fn-app_mpufresh"></a>`static inline uint8_t App_MpuFresh(const void *p,uint32_t ms)` | Adapta la llamada a `MPU9250_IsSampleFresh` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:20`; structural |
| <a id="fn-app_mpufront"></a>`static inline float App_MpuFront(const void *p)` | Adapta la llamada a `MPU9250_GetFrontTiltDeg` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:21`; structural |
| <a id="fn-app_mpunow"></a>`static inline uint32_t App_MpuNow(const void *p)` | Adapta la llamada a `now_ms` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:25`; structural |
| <a id="fn-app_mpuready"></a>`static inline bool App_MpuReady(const void *p)` | Coordina `MPU9250_IsCalibrated`, `MPU9250_IsCalibrating` para Adaptadores de hardware y servicios. | `f4:Core/Inc/app_control_ports.h:18`; structural |
| <a id="fn-app_mpusequence"></a>`static inline uint16_t App_MpuSequence(const void *p)` | Consulta la secuencia de muestra actual de la instancia MPU. | `f4:Core/Inc/app_control_ports.h:22`; editorial |
| <a id="fn-app_mpusnapshot"></a>`static inline AM_Result App_MpuSnapshot(void *p,MPU9250_Snapshot_t *s)` | Adapta la llamada a `MPU9250_GetLatest` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:24`; structural |
| <a id="fn-app_pidgains"></a>`static inline void App_PidGains(void *p,float a,float b,float c)` | Adapta la llamada a `PidController_SetGains` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:13`; structural |
| <a id="fn-app_pidinit"></a>`static inline AM_Result App_PidInit(void *p,const PidController_Config_t *c)` | Adapta la llamada a `PidController_Init` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:10`; structural |
| <a id="fn-app_pidoutput"></a>`static inline float App_PidOutput(const void *p)` | Adapta la llamada a `PidController_GetOutput` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:15`; structural |
| <a id="fn-app_pidreset"></a>`static inline void App_PidReset(void *p)` | Adapta la llamada a `PidController_Reset` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:11`; structural |
| <a id="fn-app_pidsetpoint"></a>`static inline void App_PidSetpoint(void *p,float v)` | Adapta la llamada a `PidController_SetSetpoint` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:12`; structural |
| <a id="fn-app_pidstatus"></a>`static inline const PidController_Status_t *App_PidStatus(const void *p)` | Adapta la llamada a `PidController_GetStatus` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:16`; structural |
| <a id="fn-app_pidupdate"></a>`static inline AM_Result App_PidUpdate(void *p,float a,float b,float c)` | Adapta la llamada a `PidController_Update` con el contexto de esta integración. | `f4:Core/Inc/app_control_ports.h:14`; structural |
| <a id="fn-app_pwmcompare"></a>`static inline void App_PwmCompare(void *ctx,uint32_t channel,uint16_t value)` | Adapta la llamada a `__HAL_TIM_SET_COMPARE` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:28`; structural |
| <a id="fn-app_pwmstart"></a>`static inline AM_Result App_PwmStart(void *ctx,uint32_t channel)` | Adapta la llamada a `HAL_TIM_PWM_Start` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:25`; structural |
| <a id="fn-app_readlevel"></a>`static inline bool App_ReadLevel(void *ctx)` | Adapta la llamada a `HAL_GPIO_ReadPin` con el contexto de esta integración. | `f4:Core/Inc/app_stm32_ports.h:10`; structural |
