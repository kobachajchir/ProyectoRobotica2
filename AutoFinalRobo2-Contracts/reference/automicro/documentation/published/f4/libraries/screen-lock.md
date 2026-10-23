# Bloqueo de pantalla por inactividad

ID: `f4:module/Core/Inc/screen_lock.h` · Capa: **integración** · Fotografía: 2026-10-20.

Estado: documentación por lectura de fuente; sin tests, compilación ni validación automática.

## Responsabilidad y alcance

Aplica bloqueo visual por inactividad y controla edición del tiempo de bloqueo.

Temporización y filtrado de eventos locales; no sustituye las sesiones PIN remotas de Permission.

<a id="modularity"></a>
## Modularidad observada

Pertenece a UI AutoMicro y depende de menús, preferencias y servicios de pantalla.

<a id="state"></a>
## Handle, propiedad y exposición de atributos

El estado vive en ScreensState: locked, ticks de inactividad, borrador y renderer/eventos anteriores. El API usa Screens_Context implícito.

<a id="lifecycle"></a>
## Ciclo de vida y ejecución

- Init restablece estado; Task10ms cuenta inactividad.
- ProcessEvent decide consumo/desbloqueo de entrada antes del gestor de la vista.
- OnPreferencesChanged aplica nuevas reglas; SaveTimeoutDraft solicita persistencia.

## Trabajo en tándem

- UiEventRouter consulta bloqueo antes de propagar eventos ordinarios.
- Permission puede mantener un flujo de PIN distinto al bloqueo por inactividad.

## Precauciones y límites

- Bloqueo visual y autorización de comandos son políticas distintas.
- Requiere cadencia de 10 ms para interpretar correctamente el timeout.

## Hallazgos vinculados


## Archivos y dependencias de compilación

| Archivo | Includes directos | SHA-256 |
| --- | --- | --- |
| `Core/Inc/screen_lock.h` | `stdbool.h`, `stdint.h`, `types/userEvent_type.h` | `a442dd5f6bec7b372de70cea2374723519e4610b6ad6dee4d4a5b4a478fabf44` |
| `Core/Src/screen_lock.c` | `screens_internal.h`, `screen_lock.h`, `stddef.h`, `oled_utils.h`, `screenWrappers.h` | `3a7bdcc67c66f3e4928b70cd2996e5588802c24deead840bab6279ac3b66bb24` |

## Tipos, atributos y punteros a función

Las declaraciones se extraen sin preprocesar C. Un campo documentado no es permiso para modificarlo desde fuera. Los campos sin explicación editorial se señalan como pendientes; no se inventa encapsulación. Los argumentos y retornos exactos se conservan en la firma.

## Funciones públicas y símbolos externos

Cada función ocupa una línea. `editorial` identifica una explicación escrita para la API; `structural` describe una delegación observada; `inferred-name` requiere ampliar su semántica. Las llamadas listadas son tokens estructurales y pueden ser macros o callbacks, no un grafo resuelto. Una declaración sin implementación localizada queda visible.

| Función y firma | Descripción breve | Evidencia / descripción |
| --- | --- | --- |
| <a id="fn-screenlock_adjusttimeoutdraft"></a>`void ScreenLock_AdjustTimeoutDraft(int16_t deltaSec)` | Ajusta el borrador dentro de límites configurados. | `f4:Core/Inc/screen_lock.h:25`; editorial |
| <a id="fn-screenlock_begintimeoutedit"></a>`void ScreenLock_BeginTimeoutEdit(void)` | Inicializa edición del timeout desde preferencias. | `f4:Core/Inc/screen_lock.h:24`; editorial |
| <a id="fn-screenlock_canceltimeoutdraft"></a>`void ScreenLock_CancelTimeoutDraft(void)` | Descarta la edición pendiente del timeout. | `f4:Core/Inc/screen_lock.h:27`; editorial |
| <a id="fn-screenlock_gettimeoutdraftsec"></a>`uint16_t ScreenLock_GetTimeoutDraftSec(void)` | Consulta el borrador de timeout en segundos. | `f4:Core/Inc/screen_lock.h:23`; editorial |
| <a id="fn-screenlock_init"></a>`void ScreenLock_Init(void)` | Inicializa estado de bloqueo visual. | `f4:Core/Inc/screen_lock.h:17`; editorial |
| <a id="fn-screenlock_islocked"></a>`bool ScreenLock_IsLocked(void)` | Consulta si la pantalla está bloqueada. | `f4:Core/Inc/screen_lock.h:22`; editorial |
| <a id="fn-screenlock_onpreferenceschanged"></a>`void ScreenLock_OnPreferencesChanged(void)` | Aplica cambios de habilitación/tiempo de bloqueo. | `f4:Core/Inc/screen_lock.h:19`; editorial |
| <a id="fn-screenlock_processevent"></a>`ScreenLock_EventResult_t ScreenLock_ProcessEvent(UserEvent_t ev)` | Filtra un evento y devuelve si el bloqueo lo consumió. | `f4:Core/Inc/screen_lock.h:20`; editorial |
| <a id="fn-screenlock_savetimeoutdraft"></a>`void ScreenLock_SaveTimeoutDraft(void)` | Persiste el timeout editado. | `f4:Core/Inc/screen_lock.h:26`; editorial |
| <a id="fn-screenlock_task10ms"></a>`void ScreenLock_Task10ms(void)` | Actualiza inactividad y activa bloqueo cuando corresponde. | `f4:Core/Inc/screen_lock.h:18`; editorial |
| <a id="fn-screenlock_unlock"></a>`void ScreenLock_Unlock(void)` | Desbloquea y restaura la vista guardada. | `f4:Core/Inc/screen_lock.h:28`; editorial |
| <a id="fn-screens_init_screen_lock"></a>`void Screens_Init_screen_lock(void)` | Inicializa estado de bloqueo del contexto activo de pantallas. | `f4:Core/Src/screen_lock.c:203`; editorial |

## Funciones internas

| Función | Descripción breve | Evidencia |
| --- | --- | --- |
| <a id="fn-screenlock_iseligiblenow"></a>`static bool ScreenLock_IsEligibleNow(void)` | Indica si se cumple la condición Eligible Now dentro de Bloqueo de pantalla por inactividad. | `f4:Core/Src/screen_lock.c:21`; inferred-name |
| <a id="fn-screenlock_resetidle"></a>`static void ScreenLock_ResetIdle(void)` | Restablece Idle dentro de Bloqueo de pantalla por inactividad. | `f4:Core/Src/screen_lock.c:39`; inferred-name |
| <a id="fn-screenlock_lock"></a>`static void ScreenLock_Lock(void)` | Coordina `OledUtils_DismissNotification`, `clearScreen`, `ssd1306_SetContrast` para Bloqueo de pantalla por inactividad. | `f4:Core/Src/screen_lock.c:44`; structural |
