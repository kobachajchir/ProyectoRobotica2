# AFR2 Loader 0.7 — conexión con Codex

El servidor pertenece al mismo módulo y comparte la sesión del Loader gráfico.
Transporte MCP **stdio**, JSON-RPC UTF-8 por líneas. No requiere puerto TCP,
servicio de Windows, administrador ni dependencias npm/Python para el servidor.
Las herramientas de firmware siguen siendo externas y configurables.

## Iniciar y registrar

Primero abrir la ventana; este comando compila la herramienta si hace falta:

```powershell
cd "D:\ProyectoRobotica2\AutoFinalRobo2-Loader"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\Iniciar.ps1"
```

Registrar una vez por computadora, con Codex CLI disponible en PATH:

```powershell
cd "D:\ProyectoRobotica2\AutoFinalRobo2-Loader"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\RegistrarMcp.ps1"
```

En otra computadora, cambiar el `cd` por su copia. Si el CLI no está en PATH,
usar `-Codex "C:\ruta\codex.exe"` con el registrador. También se puede copiar
el comando ya resuelto de la pestaña **Codex / MCP**. El registro usa:

```powershell
codex mcp add afr2-loader -- powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File "D:\ProyectoRobotica2\AutoFinalRobo2-Loader\Mcp.ps1"
```

Cerrar/reabrir o reconectar el cliente MCP de Codex para que descubra el servidor.
**Registrar no acredita conexión.** La primera consulta `loader_status` tiene
que devolver versión, sessionId y estado de esta ventana. Listar herramientas
solo demuestra que el proceso MCP respondió; no demuestra conexión con la app
ni funcionamiento del ESP. Esta entrega no ejecutó esos pasos.

Si no se dispone de CLI, usar las opciones de MCP de Codex o adaptar
`codex-mcp.example.toml` al archivo de configuración de Codex de ese usuario.
Conservar las demás secciones existentes. La plantilla usa una ruta de ejemplo,
no variables de PowerShell dentro del TOML. No hay que registrar la GUI como
servidor ni lanzar `Mcp.ps1` desde el monitor serie.

Para retirar el registro: `codex mcp remove afr2-loader`. Esto no borra perfiles,
fuentes ni registros. Estos comandos modifican la configuración de Codex
**solo cuando el usuario los ejecuta**; el Loader no los lanza al abrir.

## Herramientas expuestas

| Herramienta | Resultado y efecto |
| --- | --- |
| `loader_status` | Versión, sesión, activo/pendiente, `mcpConnection`, `autoAcceptRequests`, `espFirmwareMode`, `includeFilesystem`, estado y evidencias UART/ROM. No abre COM. |
| `loader_connection_test` | Recibe `sessionId` y `testId` de Ejecutar prueba. Comprueba la sesión y la prueba vigente y muestra Servidor MCP conectado. Solo conexión; no abre COM, inicia jobs ni aplica perfiles. |
| `loader_ports` | Enumera COM presentes. No abre puertos. |
| `loader_profile` | Valores visibles, `espMemory` con resumen de memoria y rutas resueltas/existencia; no comprueba versiones de herramientas. |
| `loader_serial` | Estado y texto de la última captura serie, contador RX, observación de arranque, errores y rutas de archivos. No abre/cierra puertos. |
| `loader_request_profile` | Propone opciones por clave; muestra actual → propuesto. Se aplica/guarda tras revisión o por la casilla Autoaceptar activada en GUI. |
| `loader_request_operation` | Reserva un trabajo y devuelve `jobId`, `autoAccepted` y `executed:false`: `awaiting_approval` en modo manual o `accepted` en automático. |
| `loader_job` | Estado y registro incremental; pasar `nextCursor` como cursor siguiente. |
| `loader_request_cancel` | Rechaza uno pendiente; para uno activo pide confirmación manual o solicita interrupción directamente si Autoaceptar está marcado. No equivale a cancelación consumada. |

Recurso `loader://manual`: el manual HTML distribuido con esta versión.
No hay herramienta para ejecutar comandos libres, modificar fuses, leer
archivos arbitrarios o pulsar botones de hardware.

### Prueba de conexión con esta ventana

Arriba de **Codex / MCP** aparece **Estado del servidor MCP**. El comando de
PowerShell está identificado como **Comando para añadir servidor MCP** y se
copia con su propio botón. Solo registra el servidor; no ejecuta la prueba.

1. Pulsar **Ejecutar prueba**. Despliega una instrucción para pegar en el chat
   de Codex con `loader_connection_test`, el ID de esta sesión y un ID nuevo
   para la prueba.
2. Copiar la instrucción con su botón y enviarla en Codex. No es un comando
   para el monitor serie ni para pegar directamente en PowerShell.
3. La herramienta devuelve `ok:true`, `status:connected`, `sessionId`, `testId`,
   `version`, `verifiedAt` UTC, `transport:stdio`, `localEndpoint`,
   `networkEndpoint:null`, `opensPort:false`, `executesOperation:false` y
   `duplicate`. La ventana muestra **Servidor MCP conectado** y la hora/cliente.

Los IDs se copian tal como los genera la ventana. Otra sesión, un ID anterior
o una prueba de más de cinco minutos se rechazan. Volver a pulsar el botón
genera otra prueba y reemplaza la pendiente. Una repetición de la misma prueba
ya completada por el mismo proceso devuelve el resultado con `duplicate:true`,
sin cambiar la hora original. Registrar, listar herramientas, `ping` o detectar
el proceso no bastan para poner el indicador en conectado.

`mcpConnection` en `loader_status` expone `connected`, `transport`,
`localEndpoint`, `networkEndpoint`, `testPending`, `testExpired`, `verifiedAt`
y `display`. `connected` requiere una prueba correcta y un proceso stdio que
continúe comunicándose con esta ventana. La señal local de vida se envía cada
cinco segundos; el indicador pasa a desconectado al cerrar el cliente o tras
30 segundos sin señales. No es una medición de red, COM ni del robot. La prueba
y la señal de vida siguen disponibles con Aceptar solicitudes desmarcado,
sin iniciar un trabajo ni necesitar la casilla de autoaceptación.

Esta implementación no usa IP ni puerto TCP: muestra stdio y el nombre del
canal local de Windows. No confundir el COM del robot con el transporte MCP.
Si la herramienta de prueba no aparece, desconectar el servidor anterior,
reabrir el Loader actualizado y reconectar MCP; los lanzadores pueden compilar
los EXE cuando cambian las fuentes.

### Lectura del monitor

Abrir la captura desde **Monitor serie** en la GUI. `loader_serial` no inicia
una captura ni envía datos al dispositivo. Consulta inicial:

```json
{"cursor":0,"limit":60}
```

Continuar con `captureId` igual a `id` de la respuesta y `cursor` igual a
`received.nextCursor`. La captura cambia al reabrir el puerto o aplicar nuevos
baudios; si el ID no coincide se rechaza el cursor, para no mezclar sesiones.
`received.lines` contiene fragmentos de texto, no necesariamente líneas UART
completas. `received.hasMore`, `firstAvailableCursor` y `cursorWasTruncated`
informan el recorte de memoria. Los registros completos están en `.rx.txt` y
`.rx.bin`; si hubo un fallo de disco, el estado lo indica y la captura termina.

`state`: opening/open/closing/closed/failed. `portReleased` es independiente
del éxito de captura: un fallo de lectura puede terminar con puerto liberado.
`observed` conserva el último mensaje de arranque con hora; no garantiza que
el chip siga en ese modo después de otro reset no recibido. `loader_status`
incluye un resumen de captura y `lastFailureReport` para el último fallo de la
sesión. No se interpreta texto genérico del dispositivo como carga verificada.

Los trabajos de diagnóstico/carga cierran y esperan el monitor antes de usar
la herramienta externa, tanto por GUI como por MCP. Mientras el monitor está
abriendo/cerrando se rechazan propuestas nuevas; esperar y consultar estado.
Un build puede conservar una captura ya abierta. El servidor no reabre ni
reconecta el COM al finalizar una carga; se hace manualmente desde la ventana.

## Operaciones

| action | Qué hace después de aprobar |
| --- | --- |
| `build_mega` | Producto AVR, comunicación y biblioteca. |
| `build_esp` | Familia Web: firmware PlatformIO con el layout elegido. Familia AT: revisar BIN del paquete precompilado, sin compilación Arduino. |
| `build_web_fs` | Requiere FS activado. Web: build:esp, promoción y buildfs. AT: revisión del paquete del proveedor. |
| `build_all` | ATmega y preparación ESP según familia; Web/FS solo para familia Web con FS activado. Se detiene ante un error. |
| `probe_esp` | Asistente de conexión, escucha opcional y consulta ROM/flash. No escribe flash. |
| `flash_mega` | Asistente de cableado, avrdude y verificación. |
| `flash_esp` | Web: BIN Arduino. AT: regiones firmware y system del paquete. Consulta previa y verificación posterior por USB/ROM. |
| `flash_fs` | Solo filesystem del layout: LittleFS Web o regiones FS del paquete AT; requiere casilla FS activa. |
| `flash_esp_fs` | Firmware y filesystem de la misma familia/layout; verifica todos los BIN elegidos. |

Las acciones ESP usan `EspFirmwareMode` (`web`/`at`), `WebUseFilesystem`,
`WebLayoutPreset`, `AtPackage`, `AtLayoutId` y `AtUseFilesystem` del perfil
visible al aprobar. Antes de solicitar una carga, consultar `loader_profile`
para conocer familia y memoria. Cambiar esos valores sigue el flujo habitual
de `loader_request_profile` con revisión/autoaceptación. No cambia el firmware
del Mega ni lo hace compatible con AT. [Reglas completas](../AutoFinalRobo2-Contracts/contracts/LOADER_FLASH_MODES.md).

Las variantes Web exigen BIN construidos con el layout seleccionado; las AT
usan el mapa del proveedor para ESP8266 y sus BIN precompilados. Cambiar de
familia no ejecuta comandos AT ni actualizaciones OTA: ambas cargas son USB.
`package_ready` indica revisión local de archivos; `verified` exige escritura
y verificación de los BIN. Ninguno certifica la ejecución física del robot.

Ejemplo de argumentos de `loader_request_operation`:

```json
{"action":"probe_esp","port":"COM16","listenBoot":true,"requestId":"diagnostico-esp-001"}
```

COM16 es un ejemplo; elegir el COM de esa computadora. Para builds se omite
`port`. Las cargas usan el perfil visible al aprobar, sin modificarlo a partir
de texto del chat. `listenBoot` predetermina true para ESP; false pasa directo
a la consulta ROM y no omite la identificación ni la verificación.

Ejemplo de consulta posterior:

```json
{"jobId":"ID_DEVUELTO","cursor":0,"limit":60}
```

El job pasa por `awaiting_approval` (manual) o `accepted` (automático) → `running` → `completed`, `failed` o
`cancelled`; puede terminar `rejected` antes de ejecutar. Un cambio de perfil
aprobado termina `completed` sin abrir puertos. `completed` significa que
terminó **esa acción**, no que se flasheó si la acción era un build/diagnóstico.
`writingStarted` marca que se intentó iniciar un comando de escritura, incluso
si después falla. El resultado conserva la distinción entre escrito y verificado.

Usar siempre un `requestId` por intención. Reenviar el mismo ID y argumentos en
la misma sesión devuelve el trabajo anterior: no vuelve a ejecutarlo. Un mismo
ID con otros argumentos se rechaza. Si hubo timeout, consultar primero estado/
job y repetir solo la misma solicitud con el mismo ID; nunca inventar otro ID
para reintentar una escritura automáticamente. La deduplicación se pierde al
cerrar la ventana: registrar/revisar el resultado antes de reiniciar.

## Ventana y concurrencia

**Autoaceptar todas las solicitudes de Codex (esta sesión)** se activa
manualmente en Codex / MCP y empieza desmarcado al abrir. La casilla permanece
visible desde el inicio en una fila independiente que ajusta su altura al
texto. Deshabilitar solicitudes la deja visible pero inactiva y desmarcada;
no se necesita marcar Aceptar para que aparezca.

Acepta la solicitud pendiente y las nuevas operaciones/cambios de perfil sin el diálogo de revisión
MCP. Los cambios de perfil se guardan en el perfil activo o en una copia local
nueva si el perfil importado no tenía archivo. La aprobación figura como
`approvalMode: "automatic"` o `"manual"` en el trabajo y el resumen JSON de
operación. `accepted` todavía no significa que se haya ejecutado o escrito:
seguir consultando el mismo jobId. No reenviar una escritura para iniciarla.
Desmarcar la casilla no cancela un trabajo ya iniciado. Los asistentes de
cableado, K/RST y confirmación final de escritura conservan su interacción.

Una sola GUI por usuario de Windows; servidor MCP y GUI deben correr con el
mismo usuario y nivel de privilegios normal. El canal local usa una named pipe
con ACL de ese usuario. Solo hay un trabajo activo o pendiente: una segunda
solicitud se rechaza. También se rechazan propuestas mientras se usa un diálogo
de perfiles. La solicitud pendiente bloquea los controles locales de
ejecución/configuración hasta aprobar/rechazar. Consultas de estado siguen
disponibles mientras trabaja. Los diálogos de cableado y la confirmación final
de escritura siguen exigiendo intervención en la ventana.

Desmarcar **Aceptar solicitudes de Codex** bloquea propuestas nuevas; no
interrumpe una acción ya aprobada ni elimina una pendiente. Para eso usar
Rechazar o Cancelar. El servidor no abre automáticamente la GUI. Sin ventana,
la consulta devuelve un error explícito y no inicia un proceso de flasheo.

Los últimos 100 trabajos y lotes de registro se conservan en memoria durante
la sesión. `cursorWasTruncated` indica que se recortaron líneas antiguas;
`firstAvailableCursor` y `hasMore` permiten recorrer lo disponible. El archivo
de texto y el JSON de cada operación ejecutada quedan en LOCALAPPDATA. Un job
pendiente/rechazado no genera un informe de ejecución en disco. La memoria
deduplica hasta 1000 solicitudes por sesión; al alcanzar el límite pide reinicio.

## Diagnóstico de integración

- MCP no inicia: conservar stderr; revisar .NET Framework 4.8, fuentes completas,
  ruta del lanzador y permisos de LOCALAPPDATA. El lanzador compila un EXE consola
  separado del EXE WinForms; stdout queda reservado a JSON-RPC.
- Actualización bloqueada: cerrar ventana y desconectar MCP antes de actualizar
  su ejecutable; luego reconectar. Un fallo de compilación no inicia firmware.
- Herramientas aparecen pero `loader_status` falla: abrir Iniciar.cmd con el
  mismo usuario. Tener el proceso MCP no significa tener la GUI disponible.
- Timeout al consultar: revisar diálogos/ventana bloqueada. No repetir cargas con
  IDs nuevos. La ejecución y el log permanecen en la GUI si Codex se desconecta.
- `boot mode:(1,...)` y falla ROM: el arranque solo prueba ESP→PC. Revisar
  PC→ESP y la conexión de servicio del manual; MCP no corrige el cableado.

Fuentes del protocolo y registro:
[MCP stdio](https://modelcontextprotocol.io/specification/2025-06-18/basic/transports),
[herramientas MCP](https://modelcontextprotocol.io/specification/2025-06-18/server/tools),
[MCP en Codex](https://learn.chatgpt.com/docs/extend/mcp).

**Estado de entrega:** implementación en fuente. No compilada, ejecutada ni
conectada a Codex; no hay tests, validación visual, apertura COM ni flasheo de
esta revisión. El registro global no se ejecutó desde esta entrega.
