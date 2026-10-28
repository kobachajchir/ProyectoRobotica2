# AFR2-20261027-24 — Loader0.7.0: alta y prueba del servidor MCP

27/10/2026. Pedido del usuario: distinguir el comando para añadir el servidor
de la instrucción para probarlo desde Codex, mostrar conexión/endpoint y
corregir la casilla de autoaceptación que solo aparecía al pulsar Aceptar.

## Fuente adaptada

- Codex / MCP comienza por **Estado del servidor MCP**, su indicador y el
  transporte/canal local. **Comando para añadir servidor MCP** identifica el
  registro de PowerShell y tiene un botón de copia propio con toast.
- **Ejecutar prueba** despliega la instrucción para pegar en el chat de Codex,
  con `loader_connection_test`, `sessionId` y un `testId` nuevo por intento.
  El botón prepara la prueba; no maneja Codex, ejecuta shell ni abre COM.
- La herramienta comprueba que se trate de esta ventana y de la prueba actual,
  con vencimiento de cinco minutos. Una respuesta correcta muestra **Servidor
  MCP conectado**, hora y nombre declarado por el cliente. Repetir la prueba
  completada por el mismo proceso devuelve el resultado sin alterar su hora.
  IDs reemplazados, de otra sesión o vencidos no acreditan una prueba nueva.
- El proceso stdio envía identidad y señal local cada cinco segundos mediante
  el puente existente, serializada con sus consultas. El cierre normal avisa
  a la ventana; tras 30 segundos sin señales se considera desconectado. Una
  señal de vida, registro, listado o ping no certifican una prueba correcta.
  Una pérdida de señales es evidencia de indisponibilidad del proceso/puente,
  no un diagnóstico de la red o del hardware. No se abre un endpoint nuevo.
- `loader_status.mcpConnection` ofrece conexión, transporte, canal, prueba
  pendiente/vencida, hora e indicador. La respuesta de prueba incluye los IDs,
  versión y `opensPort:false`/`executesOperation:false`. El MCP actual usa stdio
  y named pipe del usuario: `networkEndpoint:null`, sin IP/puerto TCP; el COM
  del perfil no es el endpoint MCP. Las consultas/prueba no dependen de Aceptar.
- Las dos casillas tienen filas independientes de altura automática y la vista
  completa admite desplazamiento. Autoaceptar está visible desde el inicio;
  deshabilitar solicitudes la deja visible pero inactiva y desmarcada. Se
  conservan aprobación por sesión, deduplicación y asistentes físicos.

Autoría de Koba Chajchir conservada. Cambios propios sobre la fuente local,
sin incorporar código externo ni modificar firmware del producto.
[Adaptaciones por archivo](../provenance/loader-mcp-connection-20261027.json).

| Módulo | Estado de esta ficha |
| --- | --- |
| Loader | Fuente nominal0.7.0 y documentación; sin compilar ni ejecutar esta revisión. |
| Contracts | Alcance, comportamiento y procedencia registrados. |
| atmega | Sin cambios de fuente, compilación ni carga por esta ficha. |
| esp | Sin cambios de firmware/INI, compilación ni carga por esta ficha. |
| web | Sin cambios de fuente, build ni promoción por esta ficha. |
| CAD | Sin cambios por esta ficha. |

Solo lectura y edición de fuente/documentación. No se ejecutaron tests,
compilaciones, validadores, GUI/MCP, registro global, COM, flasheos ni pruebas
físicas en esta revisión. La apariencia/DPI y la conexión ejecutada quedan
pendientes de pedido expreso. El proceso abierto no se actualiza con una
edición de fuente: debe reconstruirse el Loader y reconectarse el servidor
actualizado para descubrir la herramienta nueva.
