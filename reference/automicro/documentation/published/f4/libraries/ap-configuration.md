# Configuración deseada y aplicación de AP

ID: `f4:module/Core/Inc/app_ap_config.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Gestiona configuración AP persistida y su aplicación diferida al ESP con deadlines y reintento.

SSID, protección y reglas IPv4, patch deseado y estado de aplicación; no implementa el servidor Wi-Fi físico.

<a id="modularity"></a>
## Modularidad observada

Servicios externos para persistencia/envío, con tipos concretos de sistema/bridge y contexto global.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

ApHandle presta servicios, SystemHandle y bridge; posee applyState, notBefore y deadline. Ap_Context selecciona el contexto activo. Configuración deseada y conectividad observada son datos separados.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init establece estado; ApplyPatch revisa valores y actualiza preferencia deseada.
- Task envía cuando el backend/estado lo permite; OnEspApplyResponse completa la operación.
- RequestRetry reabre un intento sin inventar que la configuración ya está activa.

## Trabajo en tándem

- AppPreferences conserva applyPending; WifiBridge envía al ESP y entrega resultado.
- Web/Qt deben presentar deseado/pendiente/aplicado según respuesta real.

## Precauciones y límites

- No publicar el valor real de contraseñas en ejemplos.
- La API histórica usa contexto global y requiere ciclo periódico para completar reintentos.

## Hallazgos vinculados

- [AMDOC-F4-007: Los contextos activos de UI limitan independencia y reentrada](../../../review/FINDINGS.md#amdoc-f4-007)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/app_ap_config.h` | `stdbool.h`, `stdint.h`, `app_ap_config_data.h` | `ccb0571e22daee98f3952dc06f7806b81fe7e383bb04170daa45f9eea7332d17` |
| `Core/Inc/app_ap_config_data.h` | `am_port.h` | `36780103f18e0ecdcc86572744bf8dc4ebe14037991d6c74814b0a9d3e7011ca` |
| `Core/Inc/ap_context.h` | `app_ap_config_data.h`, `app_preferences_types.h`, `wifi_bridge_types.h`, `system.h`, `system_flags.h` | `a942da15f7860ea6606019192cbd0f48d6dede488d975dcff4a496cd021d0f9e` |
| `Core/Src/app_ap_config.c` | `ap_context.h`, `app_ap_config.h`, `string.h`, `utils/macros_utils.h` | `a1dd8db15e8ef144a46ce8cfa9d75361c6e53546d6482cc7c7355f3f2de73d83` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `AppApConfig_t`

`f4:Core/Inc/app_ap_config_data.h:31`

| Atributo / firma | Función y precaución |
| --- | --- |
| `char ssid[APP_AP_SSID_MAX_BYTES + 1u]` | Dato de estado/configuración `ssid`; declaración estructural, interpretación específica pendiente de ampliar. |
| `char password[APP_AP_PASSWORD_MAX_BYTES + 1u]` | Dato de estado/configuración `password`; declaración estructural, interpretación específica pendiente de ampliar. |
| `bool protected` | Dato de estado/configuración `protected`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint8_t ipv4[4]` | Dato de estado/configuración `ipv4`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppApApplyState_t applyState` | Dato de estado/configuración `applyState`; declaración estructural, interpretación específica pendiente de ampliar. |

### `AppApConfigPatch_t`

`f4:Core/Inc/app_ap_config_data.h:39`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint8_t fieldMask` | Dato de estado/configuración `fieldMask`; declaración estructural, interpretación específica pendiente de ampliar. |
| `AppApConfig_t values` | Dato de estado/configuración `values`; declaración estructural, interpretación específica pendiente de ampliar. |

### `ApServices`

`f4:Core/Inc/ap_context.h:8`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t (*now_ms)(void)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |
| `const AppPreferences_t * (*AppPreferences_Get)(void)` | Operación App Preferences Get de Configuración deseada y aplicación de AP; semántica fina pendiente de ampliación. |
| `void (*AppPreferences_GetWifiApConfig)(AppApConfig_t *out)` | Operación App Preferences Get Wifi Ap Config de Configuración deseada y aplicación de AP; semántica fina pendiente de ampliación. |
| `AM_Result (*AppPreferences_SetWifiApApplyPending)(bool pending)` | Operación App Preferences Set Wifi Ap Apply Pending de Configuración deseada y aplicación de AP; semántica fina pendiente de ampliación. |
| `AM_Result (*AppPreferences_SetWifiApConfig)(const AppApConfig_t *config, bool applyPending)` | Operación App Preferences Set Wifi Ap Config de Configuración deseada y aplicación de AP; semántica fina pendiente de ampliación. |
| `WifiBridge_Backend (*WifiBridge_GetActiveBackend)(const WifiBridge_Handle *h)` | Consulta backend seleccionado por la detección. |
| `uint8_t (*WifiBridge_IsWaitingValidation)(WifiBridge_Handle *h)` | Indica si el backend espera finalizar una solicitud. |
| `WifiBridge_Result (*WifiBridge_SendCommand)(WifiBridge_Handle *h, uint8_t cmd, const uint8_t *payload, uint8_t len)` | Operación Wifi Bridge Send Command de Configuración deseada y aplicación de AP; semántica fina pendiente de ampliación. |

### `ApHandle`

`f4:Core/Inc/ap_context.h:21`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const ApServices *services` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `SystemHandle *system` | Referencia al estado compartido AutoMicro; introduce dependencia del modelo de aplicación. |
| `WifiBridge_Handle *wifiBridge` | Referencia al bridge concreto AutoMicro; su acceso está mediado por servicios. |
| `AppApApplyState_t appApApplyState` | Dato de estado/configuración `appApApplyState`; declaración estructural, interpretación específica pendiente de ampliar. |
| `uint32_t appApNotBeforeMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |
| `uint32_t appApApplyDeadlineMs` | Tiempo, período o marca temporal en milisegundos; ver el tipo y la operación que lo escribe. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-appapconfig_applypatch"></a>`AppApStatus_t AppApConfig_ApplyPatch(const AppApConfigPatch_t *patch, uint8_t *changedMask)` | Revisa un patch, guarda cambios y marca aplicación pendiente. | `f4:Core/Inc/app_ap_config.h:14`; editorial |
| <a id="fn-appapconfig_get"></a>`void AppApConfig_Get(AppApConfig_t *out)` | Copia configuración AP deseada. | `f4:Core/Inc/app_ap_config.h:13`; editorial |
| <a id="fn-appapconfig_init"></a>`void AppApConfig_Init(void)` | Inicializa estado de aplicación AP desde preferencias. | `f4:Core/Inc/app_ap_config.h:12`; editorial |
| <a id="fn-appapconfig_ipv4isvalid"></a>`bool AppApConfig_Ipv4IsValid(const uint8_t ipv4[4])` | Consulta si los octetos cumplen las reglas AP. | `f4:Core/Inc/app_ap_config.h:17`; editorial |
| <a id="fn-appapconfig_onespapplyresponse"></a>`void AppApConfig_OnEspApplyResponse(uint8_t status)` | Procesa el status recibido del ESP para la aplicación AP. | `f4:Core/Inc/app_ap_config.h:19`; editorial |
| <a id="fn-appapconfig_parseipv4"></a>`AppApStatus_t AppApConfig_ParseIpv4(const char *text, uint8_t out[4])` | Convierte IPv4 textual a cuatro octetos con reglas del módulo. | `f4:Core/Inc/app_ap_config.h:16`; editorial |
| <a id="fn-appapconfig_requestretry"></a>`void AppApConfig_RequestRetry(void)` | Programa un nuevo intento de aplicar la configuración deseada. | `f4:Core/Inc/app_ap_config.h:20`; editorial |
| <a id="fn-appapconfig_task"></a>`void AppApConfig_Task(void)` | Avanza envío, timeout y aplicación pendiente de configuración. | `f4:Core/Inc/app_ap_config.h:18`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-appapconfig_strlen"></a>`static size_t AppApConfig_StrLen(const char *text, size_t capacity)` | Operación Str Len de Configuración deseada y aplicación de AP; semántica fina pendiente de ampliación. | `f4:Core/Src/app_ap_config.c:30`; inferred-name |
| <a id="fn-appapconfig_ssidisvalid"></a>`static bool AppApConfig_SsidIsValid(const char *ssid)` | Coordina `AppApConfig_StrLen` para Configuración deseada y aplicación de AP. | `f4:Core/Src/app_ap_config.c:43`; structural |
| <a id="fn-appapconfig_passwordisvalid"></a>`static bool AppApConfig_PasswordIsValid(const char *password, bool protected)` | Adapta la llamada a `AppApConfig_StrLen` con el contexto de esta integración. | `f4:Core/Src/app_ap_config.c:59`; structural |
