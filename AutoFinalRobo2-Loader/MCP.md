# AFR2 Loader 0.2 — conexión con Codex

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
| `loader_status` | Versión, sesión, activo/pendiente, estado y evidencias UART/ROM. No abre COM. |
| `loader_ports` | Enumera COM presentes. No abre puertos. |
| `loader_profile` | Valores visibles, rutas resueltas y existencia de archivos/carpetas; no comprueba versiones de herramientas. |
| `loader_request_profile` | Propone opciones por clave; muestra actual → propuesto. Solo se aplica/guarda al aprobar en GUI. |
| `loader_request_operation` | Reserva un trabajo y devuelve `jobId`, `awaiting_approval`, `executed:false`. |
| `loader_job` | Estado y registro incremental; pasar `nextCursor` como cursor siguiente. |
| `loader_request_cancel` | Rechaza uno pendiente, o pide confirmación de interrupción para uno activo. No equivale a cancelación consumada. |

Recurso `loader://manual`: el manual HTML distribuido con esta versión.
No hay herramienta para ejecutar comandos libres, modificar fuses, leer
archivos arbitrarios o pulsar botones de hardware.

## Operaciones

| action | Qué hace después de aprobar |
| --- | --- |
| `build_mega` | Producto AVR, comunicación y biblioteca. |
| `build_esp` | Firmware del entorno PlatformIO configurado. |
| `build_web_fs` | `npm run build:esp`, promoción Web y `pio ... -t buildfs`. |
| `build_all` | ATmega, ESP y Web/FS, en secuencia; se detiene ante un error. |
| `probe_esp` | Asistente de conexión, escucha opcional y consulta ROM/flash. No escribe flash. |
| `flash_mega` | Asistente de cableado, avrdude y verificación. |
| `flash_esp` | Firmware ESP, con consulta previa y verificación posterior. |
| `flash_fs` | LittleFS completo, con advertencia de reemplazo de datos y verificación. |
| `flash_esp_fs` | Firmware y LittleFS del mismo layout; verifica ambos. |

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

El job pasa por `awaiting_approval` → `running` → `completed`, `failed` o
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

Una sola GUI por usuario de Windows; servidor MCP y GUI deben correr con el
mismo usuario y nivel de privilegios normal. El canal local usa una named pipe
con ACL de ese usuario. Solo hay un trabajo activo o pendiente: una segunda
solicitud se rechaza. La solicitud pendiente bloquea los controles locales de
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
