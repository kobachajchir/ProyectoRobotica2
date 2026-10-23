# Permisos y sesiones PIN

ID: `f4:module/Core/Inc/permissions.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Gestiona permiso local con continuación y autenticación remota independiente por nodo.

Máquina AUTH_*, intentos, bloqueo, expiración, autorización y revocación; el archivo conserva el nombre histórico permissions_stub.c aunque contiene lógica efectiva.

<a id="modularity"></a>
## Modularidad observada

Reloj/verificación PIN/dibujo son inyectados, pero el handle conoce tipos de menú/sistema y la API usa contexto global. No es política totalmente independiente del modelo UI.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

PermissionHandle presta services, SystemHandle y MenuSystem y posee PermissionManager_t. El manager guarda solicitud local, continuation+ctx, dígitos, deadlines y remote[16]. Permission_Context es contexto global activo; el evento usa un callback distinto a la continuación.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init prepara estados; Request acepta o rechaza una solicitud local y conserva continuación.
- Task aplica expiraciones y transiciones; HandleUserEvent edita/confirma PIN local.
- RemoteAuthenticate opera sobre nodo autorizado por remote_node_valid; Touch renueva sesión y Revoke la invalida.

## Trabajo en tándem

- UNER_App comprueba transporte y nodo además de consultar Permission; la sesión del navegador no hereda autorización Qt.
- UiEventRouter entrega primero los eventos que corresponden al PIN; UI bloquea notificaciones durante flujos prioritarios.

## Precauciones y límites

- No publicar PIN, credenciales ni contenido real de sesiones en el manual.
- TTL y bloqueo usan reloj modular en ms; autorización histórica no equivale a autorización vigente.
- El nombre stub no implica ausencia de implementación.

## Hallazgos vinculados

- [AMDOC-F4-007: Los contextos activos de UI limitan independencia y reentrada](../../../review/FINDINGS.md#amdoc-f4-007)
- [AMDOC-WEB-002: El visor rellena estados que el reporte de pantalla no transporta](../../../review/FINDINGS.md#amdoc-web-002)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/permissions.h` | `permissions_types.h` | `404d84ba97e90d72e62302f3abd6496301c9bd6a830183468f803c987f5a2fa1` |
| `Core/Inc/permissions_types.h` | `stdbool.h`, `stdint.h`, `types/userEvent_type.h` | `a3bfe167df95644e66688d370d4012a81a455bd5643936b0aa7582b361084fcc` |
| `Core/Inc/permission_context.h` | `permissions_types.h`, `ssd1306_types.h`, `app_preferences_types.h`, `types/menu_types.h`, `system.h`, `fonts.h` | `693be07293427064056725f623ef1afe42bcc0f84f923049771533e5c57a1b33` |
| `Core/Src/permissions_stub.c` | `permission_context.h`, `permissions.h`, `stddef.h`, `string.h` | `62deb6c161fdd26ffab14a5f1ff717c48a7886a7ce48e53a2dc2e1e6776a854b` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `PermissionPolicy_t`

`f4:Core/Inc/permissions_types.h:71`

| Atributo / firma | Función y precaución |
| --- | --- |
| `PermissionId_t id` | Dato de estado/configuración `id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t ttl_ms` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint8_t max_attempts` | Dato de estado/configuración `max_attempts`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PermissionEvent_t`

`f4:Core/Inc/permissions_types.h:77`

| Atributo / firma | Función y precaución |
| --- | --- |
| `PermissionEventId_t event` | Dato de estado/configuración `event`; declaración estructural, interpretación específica pendiente de ampliar. |
| `PermissionId_t permission` | Dato de estado/configuración `permission`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t request_id` | Dato de estado/configuración `request_id`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t attempts_left` | Dato de estado/configuración `attempts_left`; declaración estructural, interpretación específica pendiente de ampliar. |

### `PermissionContinuationFn`

`f4:Core/Inc/permissions_types.h:83`

Firma: `void (*PermissionContinuationFn)(void *ctx)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `PermissionEventCallback`

`f4:Core/Inc/permissions_types.h:85`

Firma: `void (*PermissionEventCallback)(const PermissionEvent_t *event)`.
Tipo de callback: consultar sus registros e invocaciones antes de asumir contexto de ejecución.

### `PendingPermissionAction_t`

`f4:Core/Inc/permission_context.h:10`

| Atributo / firma | Función y precaución |
| --- | --- |
| `PermissionContinuationFn onGranted` | Dato de estado/configuración `onGranted`; declaración estructural, interpretación específica pendiente de ampliar. |
| `void *ctx` | Contexto opaco prestado al callback; el integrador conserva su almacenamiento. |

### `PermissionRemoteSession_t`

`f4:Core/Inc/permission_context.h:15`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t authenticated` | Marca de autenticación del nodo; debe combinarse con el deadline vigente. |
| `uint8_t attemptsLeft` | Intentos restantes antes del bloqueo. |
| `uint32_t authorizedUntilMs` | Deadline modular de autorización remota. |
| `uint32_t blockedUntilMs` | Deadline modular de bloqueo por intentos fallidos. |

### `PermissionManager_t`

`f4:Core/Inc/permission_context.h:22`

| Atributo / firma | Función y precaución |
| --- | --- |
| `AuthState_t state` | Dato de estado/configuración `state`; declaración estructural, interpretación específica pendiente de ampliar. |
| `PermissionId_t requestedPermission` | Dato de estado/configuración `requestedPermission`; declaración estructural, interpretación específica pendiente de ampliar. |
| `PendingPermissionAction_t pendingAction` | Continuación local y contexto guardados hasta concesión o cancelación. |
| `uint8_t requestId` | Dato de estado/configuración `requestId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t pinDigits[PERMISSION_PIN_DIGITS]` | Dato de estado/configuración `pinDigits`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t pinIndex` | Dato de estado/configuración `pinIndex`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t attemptsLeft` | Dato de estado/configuración `attemptsLeft`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t localDeadlineMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t submitAtMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t localBlockedUntilMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `PermissionAuthNoticeId_t remoteAuthNoticeId` | Dato de estado/configuración `remoteAuthNoticeId`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t remoteAuthNoticeUntilMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t authorizedUntil[PERMISSION_COUNT]` | Dato de estado/configuración `authorizedUntil`; declaración estructural, interpretación específica pendiente de ampliar. |
| `RenderFunction previousRenderFn` | Memoria de la observación anterior usada por la máquina de estados. |
| `UserEventManagerFn previousUserEventManagerFn` | Memoria de la observación anterior usada por la máquina de estados. |
| `bool previousAllowPeriodicRefresh` | Memoria de la observación anterior usada por la máquina de estados. |
| `uint8_t previousInsideMenu` | Memoria de la observación anterior usada por la máquina de estados. |
| `PermissionEventCallback eventCallback` | Receptor de eventos de permiso; distinto de la continuación onGranted. |
| `PermissionRemoteSession_t remote[PERMISSION_UNER_NODE_COUNT]` | Sesiones por nodo UNER, sin herencia entre navegador/Qt. |

### `PermissionServices`

`f4:Core/Inc/permission_context.h:44`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t (*now_ms)(void)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |
| `bool (*remote_node_valid)(uint8_t node)` | Operación remote node valid de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `bool (*AppPreferences_IsPinLockEnabled)(void)` | Operación App Preferences Is Pin Lock Enabled de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `bool (*AppPreferences_VerifyAuthPin)(const uint8_t pinDigits[APP_PREFS_AUTH_PIN_DIGITS])` | Operación App Preferences Verify Auth Pin de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `void (*MenuSys_SetCurrentScreenCode)(MenuSystem *ms, ScreenCode_t screen_code, ScreenReportSource_t source)` | Actualiza identidad/origen de pantalla y marca reporte pendiente. |
| `void (*OledUtils_DismissNotification)(void)` | Descarta el aviso actual y avanza/restaura la vista correspondiente. |
| `void (*OledUtils_SetNotificationsBlocked)(bool blocked)` | Bloquea o habilita la presentación de avisos durante flujos prioritarios. |
| `void (*ssd1306_Clear)(void)` | Operación ssd1306 Clear de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawBitmap)(uint8_t X, uint8_t Y, uint8_t W, uint8_t H, const uint8_t* pBMP)` | Operación ssd1306 Draw Bitmap de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `void (*ssd1306_DrawRect)(int16_t x, int16_t y, int16_t width, int16_t height)` | Operación ssd1306 Draw Rect de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `void (*ssd1306_FillRect)(int16_t xMove, int16_t yMove, int16_t width, int16_t height)` | Operación ssd1306 Fill Rect de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `void (*ssd1306_SetColor)(SSD1306_COLOR color)` | Operación ssd1306 Set Color de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `void (*ssd1306_SetCursor)(uint8_t x, uint8_t y)` | Operación ssd1306 Set Cursor de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |
| `char (*ssd1306_WriteString)(char* str, FontDef Font)` | Operación ssd1306 Write String de Permisos y sesiones PIN; semántica fina pendiente de ampliación. |

### `PermissionHandle`

`f4:Core/Inc/permission_context.h:60`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const PermissionServices *services` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `SystemHandle *system` | Referencia al estado compartido AutoMicro; introduce dependencia del modelo de aplicación. |
| `MenuSystem *menuSystem` | Referencia al menú compuesto por la aplicación. |
| `PermissionManager_t permissionManager` | Dato de estado/configuración `permissionManager`; declaración estructural, interpretación específica pendiente de ampliar. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-permission_cancellocalrequest"></a>`void Permission_CancelLocalRequest(void)` | Cancela la solicitud local y restaura contexto UI. | `f4:Core/Inc/permissions.h:19`; editorial |
| <a id="fn-permission_getremotettlremaining"></a>`uint32_t Permission_GetRemoteTtlRemaining(uint8_t source_node, uint32_t now_ms)` | Consulta tiempo restante de sesión del nodo. | `f4:Core/Inc/permissions.h:43`; editorial |
| <a id="fn-permission_getrequestedpermission"></a>`PermissionId_t Permission_GetRequestedPermission(void)` | Consulta qué permiso solicita el flujo local actual. | `f4:Core/Inc/permissions.h:25`; editorial |
| <a id="fn-permission_getstate"></a>`AuthState_t Permission_GetState(void)` | Consulta estado AUTH_* actual del gestor local. | `f4:Core/Inc/permissions.h:26`; editorial |
| <a id="fn-permission_grantcurrentrequest"></a>`bool Permission_GrantCurrentRequest(uint32_t ttl_ms)` | Concede solicitud actual y ejecuta la continuación registrada. | `f4:Core/Inc/permissions.h:23`; editorial |
| <a id="fn-permission_handleuserevent"></a>`void Permission_HandleUserEvent(UserEvent_t ev)` | Procesa entradas de PIN del flujo local activo. | `f4:Core/Inc/permissions.h:21`; editorial |
| <a id="fn-permission_hasactivelocalrequest"></a>`bool Permission_HasActiveLocalRequest(void)` | Indica si una solicitud de permiso local sigue en curso. | `f4:Core/Inc/permissions.h:24`; editorial |
| <a id="fn-permission_init"></a>`void Permission_Init(void)` | Inicializa permisos locales y sesiones por nodo. | `f4:Core/Inc/permissions.h:7`; editorial |
| <a id="fn-permission_isauthorized"></a>`bool Permission_IsAuthorized(PermissionId_t permission, uint32_t now_ms)` | Consulta autorización con el tiempo recibido. | `f4:Core/Inc/permissions.h:10`; editorial |
| <a id="fn-permission_isauthorizednow"></a>`bool Permission_IsAuthorizedNow(PermissionId_t permission)` | Consulta autorización usando el reloj del servicio. | `f4:Core/Inc/permissions.h:11`; editorial |
| <a id="fn-permission_isremotenodeauthorized"></a>`bool Permission_IsRemoteNodeAuthorized(uint8_t source_node, uint32_t now_ms)` | Comprueba sesión del nodo frente al tiempo recibido. | `f4:Core/Inc/permissions.h:38`; editorial |
| <a id="fn-permission_isremotenodeauthorizednow"></a>`bool Permission_IsRemoteNodeAuthorizedNow(uint8_t source_node)` | Comprueba sesión remota con el reloj del servicio. | `f4:Core/Inc/permissions.h:39`; editorial |
| <a id="fn-permission_remoteauthenticate"></a>`bool Permission_RemoteAuthenticate( uint8_t source_node, const uint8_t pin_digits[PERMISSION_PIN_DIGITS], uint32_t now_ms, uint32_t *ttl_ms, uint8_t *attempts_left, bool *new_session)` | Comprueba PIN/intentos y abre o rechaza sesión del nodo. | `f4:Core/Inc/permissions.h:31`; editorial |
| <a id="fn-permission_renderpinentry"></a>`void Permission_RenderPinEntry(void)` | Dibuja entrada PIN y estado de intentos. | `f4:Core/Inc/permissions.h:22`; editorial |
| <a id="fn-permission_request"></a>`bool Permission_Request(PermissionId_t permission, PermissionContinuationFn on_granted, void *ctx)` | Solicita permiso local y registra continuación con su contexto. | `f4:Core/Inc/permissions.h:13`; editorial |
| <a id="fn-permission_revoke"></a>`void Permission_Revoke(PermissionId_t permission)` | Revoca el permiso local indicado. | `f4:Core/Inc/permissions.h:17`; editorial |
| <a id="fn-permission_revokeall"></a>`void Permission_RevokeAll(void)` | Revoca todos los permisos locales. | `f4:Core/Inc/permissions.h:18`; editorial |
| <a id="fn-permission_revokeallremote"></a>`void Permission_RevokeAllRemote(void)` | Revoca todas las sesiones remotas. | `f4:Core/Inc/permissions.h:42`; editorial |
| <a id="fn-permission_revokeremotenode"></a>`void Permission_RevokeRemoteNode(uint8_t source_node)` | Revoca autorización del nodo indicado. | `f4:Core/Inc/permissions.h:41`; editorial |
| <a id="fn-permission_seteventcallback"></a>`void Permission_SetEventCallback(PermissionEventCallback callback)` | Registra receptor de eventos del ciclo de permisos. | `f4:Core/Inc/permissions.h:27`; editorial |
| <a id="fn-permission_showremoteauthnotice"></a>`bool Permission_ShowRemoteAuthNotice(PermissionAuthNoticeId_t notice_id, uint16_t timeout_ms)` | Coordina `Permission_ExtendDeadline`, `Permission_IsExpired`, `Permission_RequestRender` para Permisos y sesiones PIN. | `f4:Core/Inc/permissions.h:28`; structural |
| <a id="fn-permission_task"></a>`void Permission_Task(uint32_t now_ms)` | Procesa vencimientos, bloqueo y transiciones temporales. | `f4:Core/Inc/permissions.h:8`; editorial |
| <a id="fn-permission_touchremotenode"></a>`void Permission_TouchRemoteNode(uint8_t source_node, uint32_t now_ms)` | Renueva actividad de la sesión remota autorizada. | `f4:Core/Inc/permissions.h:40`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-permission_isexpired"></a>`static bool Permission_IsExpired(uint32_t deadline, uint32_t now_ms)` | Indica si se cumple la condición Expired dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:50`; inferred-name |
| <a id="fn-permission_remotenodeisvalid"></a>`static bool Permission_RemoteNodeIsValid(uint8_t source_node)` | Adapta la llamada a `remote_node_valid` con el contexto de esta integración. | `f4:Core/Src/permissions_stub.c:55`; structural |
| <a id="fn-permission_getpolicy"></a>`static const PermissionPolicy_t *Permission_GetPolicy(PermissionId_t permission)` | Consulta Policy dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:60`; inferred-name |
| <a id="fn-permission_emitfor"></a>`static void Permission_EmitFor(PermissionId_t permission, PermissionEventId_t event)` | Coordina `eventCallback` para Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:69`; structural |
| <a id="fn-permission_emit"></a>`static void Permission_Emit(PermissionEventId_t event)` | Adapta la llamada a `Permission_EmitFor` con el contexto de esta integración. | `f4:Core/Src/permissions_stub.c:84`; structural |
| <a id="fn-permission_clearpin"></a>`static void Permission_ClearPin(void)` | Limpia Pin dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:89`; inferred-name |
| <a id="fn-permission_extenddeadline"></a>`static void Permission_ExtendDeadline(uint32_t *deadline_ms, uint32_t extension_ms)` | Operación Extend Deadline de Permisos y sesiones PIN; semántica fina pendiente de ampliación. | `f4:Core/Src/permissions_stub.c:95`; inferred-name |
| <a id="fn-permission_requestrender"></a>`static void Permission_RequestRender(void)` | Solicita Render dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:108`; inferred-name |
| <a id="fn-permission_saveuicontext"></a>`static void Permission_SaveUiContext(void)` | Operación Save Ui Context de Permisos y sesiones PIN; semántica fina pendiente de ampliación. | `f4:Core/Src/permissions_stub.c:117`; inferred-name |
| <a id="fn-permission_restoreuicontext"></a>`static void Permission_RestoreUiContext(void)` | Coordina `OledUtils_SetNotificationsBlocked`, `clearScreen` para Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:126`; structural |
| <a id="fn-permission_clearpendingrequest"></a>`static void Permission_ClearPendingRequest(void)` | Limpia Pending Request dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:144`; inferred-name |
| <a id="fn-permission_resolvettl"></a>`static uint32_t Permission_ResolveTtl(PermissionId_t permission, uint32_t ttl_ms)` | Coordina `Permission_GetPolicy` para Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:157`; structural |
| <a id="fn-permission_grant"></a>`static void Permission_Grant(uint32_t ttl_ms)` | Coordina `Permission_ClearPendingRequest`, `Permission_Emit`, `Permission_ResolveTtl` para Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:175`; structural |
| <a id="fn-permission_cancelrequest"></a>`static void Permission_CancelRequest(void)` | Coordina `Permission_ClearPendingRequest`, `Permission_Emit`, `Permission_RestoreUiContext` para Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:201`; structural |
| <a id="fn-permission_applydeniedstate"></a>`static void Permission_ApplyDeniedState(void)` | Aplica Denied State dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:209`; inferred-name |
| <a id="fn-permission_startinput"></a>`static void Permission_StartInput(void)` | Inicia Input dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:228`; inferred-name |
| <a id="fn-permission_submitpin"></a>`static void Permission_SubmitPin(void)` | Coordina `AppPreferences_VerifyAuthPin`, `Permission_ApplyDeniedState`, `Permission_Emit` para Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:248`; structural |
| <a id="fn-permission_beginpinsubmit"></a>`static void Permission_BeginPinSubmit(void)` | Coordina `Permission_RequestRender`, `now_ms` para Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:267`; structural |
| <a id="fn-permission_resetforretry"></a>`static void Permission_ResetForRetry(void)` | Restablece For Retry dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:277`; inferred-name |
| <a id="fn-permission_drawtext"></a>`static void Permission_DrawText(uint8_t x, uint8_t y, FontDef font, const char *text)` | Dibuja Text dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:288`; inferred-name |
| <a id="fn-permission_drawbaselinetext"></a>`static void Permission_DrawBaselineText(uint8_t x, uint8_t baseline_y, FontDef font, const char *text)` | Adapta la llamada a `Permission_DrawText` con el contexto de esta integración. | `f4:Core/Src/permissions_stub.c:295`; structural |
| <a id="fn-permission_drawcenteredbaselinetext"></a>`static void Permission_DrawCenteredBaselineText(uint8_t baseline_y, FontDef font, const char *text)` | Dibuja Centered Baseline Text dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:309`; inferred-name |
| <a id="fn-permission_renderremoteauthnotice"></a>`static void Permission_RenderRemoteAuthNotice(void)` | Renderiza Remote Auth Notice dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:330`; inferred-name |
| <a id="fn-permission_renderpindigits"></a>`static void Permission_RenderPinDigits(void)` | Renderiza Pin Digits dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:377`; inferred-name |
| <a id="fn-permission_renderencoderhint"></a>`static void Permission_RenderEncoderHint(const char *label, uint8_t text_x)` | Renderiza Encoder Hint dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:409`; inferred-name |
| <a id="fn-permission_renderpinform"></a>`static void Permission_RenderPinForm(void)` | Renderiza Pin Form dentro de Permisos y sesiones PIN. | `f4:Core/Src/permissions_stub.c:417`; inferred-name |
| <a id="fn-permission_formatattempts"></a>`static void Permission_FormatAttempts(char *buf, size_t len, uint8_t attempts)` | Operación Format Attempts de Permisos y sesiones PIN; semántica fina pendiente de ampliación. | `f4:Core/Src/permissions_stub.c:436`; inferred-name |
