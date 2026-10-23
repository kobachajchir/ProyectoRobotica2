# Router de eventos de usuario

ID: `f4:module/Core/Inc/ui_event_router.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Decide qué flujo UI consume un evento de usuario antes del handler de la pantalla.

Prioridades de PIN, bloqueo, notificaciones, exploración Wi-Fi y navegación global.

<a id="modularity"></a>
## Modularidad observada

Enrutador de aplicación con callbacks, pero dependiente de modelos concretos y contexto global.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

UiRouterHandle presta services, SystemHandle, MenuSystem y WifiBridge_Handle. UiRouter_Context selecciona contexto activo; no posee el estado de cada pantalla.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- La composición conecta contexto y tabla; HandleEvent consulta guardas y entrega el evento al consumidor correspondiente.

## Trabajo en tándem

- Recibe eventos físicos desde app_tasks.c y eventos remotos desde UNER_App; comparten semántica lógica de UserEvent_t.
- Cambiar prioridad de consumo puede alterar ambas interfaces sin cambiar números de comando.

## Precauciones y límites

- La rotación física del encoder y un comando lógico CW/CCW no son el mismo punto de entrada.
- Un evento aceptado no implica que una acción restringida haya sido autorizada.
- USER largo global retorna al dashboard antes del handler OSK y sin OnBack/cleanup explícito; ver AMDOC-F4-011 antes de asumir cancelación equivalente.

## Hallazgos vinculados

- [AMDOC-F4-007: Los contextos activos de UI limitan independencia y reentrada](../../../review/FINDINGS.md#amdoc-f4-007)
- [AMDOC-F4-011: USER largo sale del OSK por el router sin ejecutar su cancelación](../../../review/FINDINGS.md#amdoc-f4-011)

## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/ui_event_router.h` | `types/userEvent_type.h` | `2310d621f15b6fff76ce3e499a0f9ff18f41827c91cc30d930a17f24623bf033` |
| `Core/Inc/uirouter_context.h` | `types/menu_types.h`, `permissions_types.h`, `screen_lock.h`, `wifi_bridge_types.h`, `system.h`, `system_flags.h`, `ui_event_router.h` | `fdc8ff75cb85dfc3153cfa100e6912971f28c656dffa0ba9d6ac7e07b4a93822` |
| `Core/Src/ui_event_router.c` | `uirouter_context.h`, `ui_event_router.h`, `permissions.h`, `screen_lock.h` | `a7462a56c24382af9099432352b4382186e887201cf569ae635532b412e88e44` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

### `UiRouterServices`

`f4:Core/Inc/uirouter_context.h:10`

| Atributo / firma | Función y precaución |
| --- | --- |
| `uint32_t (*now_ms)(void)` | Entrega reloj monotónico modular en milisegundos para deadlines y períodos. |
| `ScreenCode_t (*MenuSys_GetCurrentScreenCode)(const MenuSystem *ms)` | Consulta identidad de pantalla del MenuSystem recibido. |
| `void (*MenuSys_NotifyScreenContentChanged)(MenuSystem *ms, ScreenReportSource_t source)` | Solicita reportar cambio de contenido sin inventar un framebuffer remoto. |
| `void (*MenuSys_ResetMenu)(MenuSystem *ms)` | Restablece contexto de navegación del menú. |
| `void (*OledUtils_DismissNotification)(void)` | Descarta el aviso actual y avanza/restaura la vista correspondiente. |
| `uint8_t (*OledUtils_GetNotificationCount)(void)` | Consulta la cantidad de avisos conservados por la cola. |
| `PermissionId_t (*Permission_GetRequestedPermission)(void)` | Consulta qué permiso solicita el flujo local actual. |
| `AuthState_t (*Permission_GetState)(void)` | Consulta estado AUTH_* actual del gestor local. |
| `void (*Permission_HandleUserEvent)(UserEvent_t ev)` | Procesa entradas de PIN del flujo local activo. |
| `ScreenLock_EventResult_t (*ScreenLock_ProcessEvent)(UserEvent_t ev)` | Filtra un evento y devuelve si el bloqueo lo consumió. |
| `void (*WiFiScan_ClearResults)(void)` | Limpia resultados y estado de la sesión de exploración Wi-Fi. |
| `WifiBridge_Result (*WifiBridge_StopScan)(WifiBridge_Handle *h)` | Operación Wifi Bridge Stop Scan de Router de eventos de usuario; semántica fina pendiente de ampliación. |

### `UiRouterHandle`

`f4:Core/Inc/uirouter_context.h:24`

| Atributo / firma | Función y precaución |
| --- | --- |
| `const UiRouterServices *services` | Contrato de operaciones externo; conservar la tabla y su contexto durante el uso. |
| `SystemHandle *system` | Referencia al estado compartido AutoMicro; introduce dependencia del modelo de aplicación. |
| `MenuSystem *menuSystem` | Referencia al menú compuesto por la aplicación. |
| `WifiBridge_Handle *wifiBridge` | Referencia al bridge concreto AutoMicro; su acceso está mediado por servicios. |

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-uieventrouter_handleevent"></a>`UiEventRouterResult_t UiEventRouter_HandleEvent(UserEvent_t ev)` | Aplica prioridad de flujos UI y despacha el evento al consumidor activo. | `f4:Core/Inc/ui_event_router.h:22`; editorial |
