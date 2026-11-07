> **1.1.2 / AFR2-20261029-13:** KEY/RST del ESP siempre requiere intervención local, restaura la ventana minimizada y presenta el aviso al frente. Ni skipManualChecks ni Autoaceptar ni listenBoot:false lo omiten. Con escucha se pide Continuar antes de abrirla y se espera arranque real; sin escucha o con flasher Mega se exige casilla local y Confirmar antes de consultar ROM. El flasheo ATmega conserva su flujo sin este requisito. Esto sustituye la omisión del botón Continuar documentada en 1.1.1. [Ayuda](MANUAL.html#esp-intervencion).

> **1.1.1 / AFR2-20261029-12:** nueva herramienta `loader_request_session_options` con `skipManualChecks` (booleano) y `requestId`. Activar muestra SIEMPRE una advertencia local en la GUI: el servidor no puede aceptarla y Autoaceptar no la omite. Se retorna jobId; consultar `loader_job` y `loader_status.skipManualChecks`. No iniciar operaciones antes de la aceptación local. Desactivar (`false`, requestId nuevo) restablece las confirmaciones sin advertencia. La autorización termina al desmarcar/cerrar la app; no se persiste. Ejemplo: `{"skipManualChecks":true,"requestId":"checks-01"}`. Durante el diálogo el job pasa a `awaiting_local_confirmation`; al finalizar queda `completed` o `rejected`.
>
> Aceptar da por confirmados los diálogos manuales y sus casillas para GUI/MCP, incluido continuar sin backup si esa es la elección y el botón Continuar antes de escuchar KEY/RST. No cambia la elección Backup ni elimina firma, capacidad, hashes, recibos, errores, verificación o acciones físicas. El software no mide cableado, aislación o alimentación. La advertencia no utiliza el asistente genérico ni las preferencias de casillas; requiere clic humano y queda registrada.

> **1.1.0 / AFR2-20261029-01:** build_web_fs conserva su ID y usa el lanzador del perfil (FsScript, FsSourceDir, FsScriptHost, FsScriptArguments). [Contrato y ejemplos](FILESYSTEM_SCRIPTS.md).

> **AFR2-20261028-23 / 0.9.5 — Servidor MCP:** el alta ofrece un prompt genérico con comando/args stdio de Mcp.ps1. Ejecutar prueba genera un prompt para Codex con sessionId/testId de cinco minutos y lo abre para copiar; no ejecuta una carga ni confirma conexión por prepararlo. Las operaciones de flash exigen preparación compilada/revisada de su sección en la misma sesión también desde MCP. El flasher se compila dentro de su instalación; backups conservan sus propias comprobaciones. La verificación ESP es el hash integrado de write_flash; sin read_mac/verify_flash posterior. No cambia la aprobación MCP ni su esquema de herramientas.

> **0.9.5 / AFR2-20261028-16:** la GUI organiza solicitudes en tabla y separa conector/conexión comprobada. Preferencias de casillas en Opciones; los jobs serie comprueban disponibilidad del COM tras cerrar el monitor. El envío de monitor es exclusivamente manual desde la GUI: no se agrega herramienta TX MCP. Snapshot de captura incorpora bytesSent/transmitLog/transmitBinary; RX sigue separado. Aprobación, deduplicación y operaciones disponibles se conservan. Fuente sin compilar ni ejecutar.

# AFR2 Loader 0.9.5 — conexión con Codex

El servidor pertenece al mismo módulo y comparte la sesión del Loader gráfico.

Desde0.8, `EspTransport` selecciona `mega_bridge` (flasher temporal, D18/D19,
RESET libre) o `usb-ttl` (adaptador externo3,3V). El método D0/D1 con RESET-GND
(`mega_reset`/`mega-usb`) fue retirado; los perfiles antiguos pasan a `mega_bridge`. Se consulta en
loader_status/perfil. Las operaciones ESP existentes usan esa selección y sus
asistentes: no saltan al otro modo ante un error. `listenBoot` solo gobierna la
escucha74880 en Mega físico o USB–TTL; en el software se guía KEY/RST manualmente y
se consulta ROM al baud fijo. Importar/proponer un perfil no acredita cableado.

`install_mega_flasher` usa la casilla Backup de ATmega antes de cargar de la
ventana. Si está marcada lee/contrasta flash y hfuse antes de escribir el
servicio; un fallo detiene la carga. Desmarcada aplica el aviso y la preferencia
No mostrar de nuevo. No necesita HEX de producto para crear un backup real.
`flash_mega` carga el HEX configurado. `flash_mega_backup` carga el último backup
leído y completado, mostrando origen/destino; comprueba hashes/hfuse y contrasta
la aplicación completa mediante lectura posterior. `restore_mega_product` se
conserva como alias de esta última acción; ya no usa copias HEX de PC de0.8.
`loader_status` expone `backupBeforeFlasher` y `showNoBackupWarning`; MCP no
modifica las preferencias ni desactiva asistentes. No hay restauración al
cerrar. El registro local no detecta cargas externas ni cambios de placa.
Los asistentes físicos permanecen activos incluso con autoaceptación MCP.

Transporte MCP **stdio**, JSON-RPC UTF-8 por líneas. No requiere puerto TCP,
servicio de Windows, administrador ni dependencias npm/Python para el servidor.
Las herramientas de firmware siguen siendo externas y configurables.

## Iniciar y registrar

Primero abrir la ventana. La distribución 2.0.0 usa ejecutables precompilados; la copia de desarrollo puede preparar la herramienta si hace falta:

```powershell
# Abrí PowerShell en la carpeta extraída del Loader
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\Iniciar.ps1"
```

Registrar una vez por computadora, con Codex CLI disponible en PATH:

```powershell
# Abrí PowerShell en la carpeta extraída del Loader
powershell.exe -NoProfile -ExecutionPolicy Bypass -File ".\RegistrarMcp.ps1"
```

Ejecutá los ejemplos desde la carpeta extraída en esa computadora. Si el CLI no está en PATH,
usar `-Codex "C:\ruta\codex.exe"` con el registrador. También se puede copiar
el comando ya resuelto de la pestaña **Servidor MCP**. El registro usa:

```powershell
codex mcp add afr2-loader -- powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File (Join-Path (Get-Location) 'Mcp.ps1')
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

El selector local Vista (ASCII / HEX con espacios / Texto decodificado)
reconvierte la ventana retenida sin reabrir el puerto ni cambiar `captureId`.
No transforma `received.lines`: sigue siendo texto decodificado según `codec`
de la captura, igual que `.rx.txt`. `.rx.bin` conserva bytes originales.
`skippedDisplayBytes` informa bytes omitidos por atraso de la cola visual;
se conserva `skippedDisplayCharacters` para el texto decodificado omitido.
Limpiar la vista tampoco borra el journal MCP ni mueve sus cursores.

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
| `install_mega_flasher` | Compila el servicio, realiza backup leído si la casilla está marcada, carga/verifica y selecciona mega_bridge con EspBaud alineado. Conserva aviso y asistentes. |
| `flash_mega_backup` | Carga la aplicación del último backup completado; muestra origen/destino, comprueba hashes/hfuse y contrasta lectura posterior completa. |
| `restore_mega_product` | Alias de flash_mega_backup desde0.9. No usa antiguas copias de HEX de PC. |
| `build_esp` | Familia Web: firmware PlatformIO con el layout elegido. Familia AT: revisar BIN del paquete precompilado, sin compilación Arduino. |
| `build_web_fs` | Requiere FS activado. Firmware propio: script configurable del perfil y buildfs; no exige Web ni npm. AT: revisión del paquete del proveedor. |
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

Antes del comando de verificación ESP se exige una consulta nueva al cargador
(`read_mac`/`read-mac`, sin escritura). Si falla, `checking_bootloader` pasa a
`verification_paused` y la operación espera el aviso/asistente KEY/RST de la
ventana. La recuperación pide botones aunque `listenBoot:false`; la opción
solo omite la guía inicial. Tras recuperarlo se vuelve a comprobar y se
continúa con la verificación, sin reenviar la escritura. Cancelar conserva
`writingStarted:true` y `contentVerified:false`. La autoaceptación MCP no
omite este asistente físico; no se agrega una herramienta MCP nueva.

El perfil incluye `EspTransport`: `mega_bridge` (flasher) o `usb-ttl` para un adaptador externo con lógica3,3V. Cambia las
instrucciones de conexión; no fija el COM ni configura sus niveles eléctricos.
La GUI y las acciones MCP usan el mismo generador de argumentos: Python `-u`,
`no_reset` antes/después, baudios y SPI del perfil y stub normal al escribir.
Mantener K (KEY) desde la secuencia con RST hasta terminar la operación,
incluidas escritura y verificación; `listenBoot:false` no elimina ese paso
manual si ya se entró en programación. El recordatorio de soltarlo se emite
al finalizar la operación, también al fallar/cancelar, sin convertir el fallo
en una verificación correcta.

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

## Integración0.9.1 — AFR2-20261028-11

Las funciones0.9.0 de backups y flasher se conservan junto con monitor
ASCII/HEX/texto y verificación de0.7.4. La consulta nueva antes de verificar
usa dos intentos y15s (30s con wrapper). Si falla, la recuperación respeta el
transporte seleccionado: mega_bridge no abre escucha74880 ni exige RESET-GND.
KEY se mantiene hasta finalizar la operación. No cambia la revisión manual/
autoaceptación de la sesión ni crea permisos nuevos. Fuente sin ejecutar ni
compilar esta integración; no se actualiza automáticamente una ventana ya abierta.

## Conversor universal0.9.2

La pestaña Conversor HEX / ASCII es una utilidad GUI local sin asociación a comandos.
No agrega herramienta MCP, jobs, tráfico serie ni logs del contenido pegado.
Puede usarse durante una operación sin tocar sus recursos. Fuente sin ejecutar.


## Feedback de carga0.9.3
La GUI muestra actividad de procesos, mensajes parciales y porcentaje informado de escritura. El formato MCP no cambia: loader_job conserva salida, etapa y resultado; el porcentaje visual no declara verificación ni crea nuevas operaciones serie. Fuente sin compilar/ejecutar en esta entrega.


## BIN AT reconocido0.9.4
AtPackage también acepta el BIN completo Ai-Thinker AT v1.5.4.1, identificado por hash y tamaño. Se conserva el resto del esquema MCP, perfil y comprobaciones. Otros BIN requieren manifiesto JSON. La selección no inicia escritura. Fuente sin compilar/ejecutar.





## Loader1.1.3 — Verificar flasher (AFR2-20261029-21)

En **Flasher ATmega → Verificar flasher**, el Loader compara la flash instalada con la referencia compilada para los baudios seleccionados, por bootloader y avrdude `-n -D -U flash:v:...:i`. Requiere firma y confirmación de bytes coincidentes. Reinicia el Mega por auto-reset; no escribe firmware, EEPROM ni fuses y no consulta al ESP. Un firmware distinto/otra variante no se acredita. El flasher transparente existente no necesita actualización. GUI/MCP compilados; sin prueba COM de esta revisión.

Desde Codex: `loader_request_operation` con `action: verify_mega_flasher`, `port` y `requestId` nuevo; consultar `loader_job` hasta completar. Se aplican aprobación/exclusión del COM y confirmaciones vigentes. No confundir el último registro local con una comprobación nueva del dispositivo.


## Loader1.1.4 — Configuración y carga explícitas (AFR2-20261029-22)

Elegir paquete/sección solo cambia configuración. La selección de paquete invalida la revisión anterior; pulsá Consultar tamaño de flash/Detectar ESP y luego Revisar paquete AT o Compilar según corresponda. La carga se inicia desde Flashear; selección/preparación no abren automáticamente detección ni escritura. Compilar/Revisar con capacidad pendiente informa el requisito sin iniciar una consulta.
