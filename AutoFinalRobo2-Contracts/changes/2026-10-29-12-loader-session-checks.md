# AFR2-20261029-12 — Loader 1.1.1: confirmaciones manuales por sesión

Pedido humano: checkbox debajo de Autoaceptar MCP para omitir casillas de verificación repetidas, activable por GUI o servidor; advertencia obligatoria aceptada por la persona antes de habilitar, válida solo hasta desactivar/cerrar. Versión del Loader (no firmware de placas): 1.1.1.

Autoría Koba Chajchir / AFR2 conservada; adaptación mediante Codex. [Contrato](../contracts/LOADER_SESSION_CHECKS.md).

## Adaptaciones por archivo

- AutoFinalRobo2-Loader/src/MainForm.SessionChecks.cs: estado no persistido, checkbox, advertencia local específica, dos botones y revisión de solicitud aún vigente antes de activar.
- src/MainForm.Integration.cs: posición debajo de Autoaceptar, endpoint de sesión con deduplicación/jobId y advertencia local aun sin Autoaceptar, estado real en loader_status. Excluye estas solicitudes de ScheduleAutomaticApproval.
- src/McpServer.cs: herramienta loader_request_session_options, esquema booleano/requestId y validación de claves.
- src/MainForm.cs: Confirm registra textos/casillas y continúa cuando la autorización está activa; camino previo intacto si no lo está.
- src/MainForm.Preferences.cs: aviso sin backup omitido durante autorización, conservando elección Backup y preferencias.
- src/MainForm.Workflow.cs: escucha KEY/RST sin botón Continuar bajo autorización; evidencia física y errores conservados.
- src/JobState.cs, launcher/Launcher.cs: versión 1.1.1 / metadatos 1.1.1.0.
- Preparar-Distribucion.ps1: salida AFR2-Loader-dist-1.1.1, conservando entrega 1.1 anterior.
- MANUAL.html y MCP.md: alcance, advertencia, nueva herramienta entre acciones, ejemplo y estados. README.md: acceso y versión.
- Contracts/contracts/LOADER_SESSION_CHECKS.md: contrato del estado, interfaz, activación local y límites; índices README/INFORME_ADAPTACION/CHANGELOG referencian la ficha.

## Estado real

Compilación GUI y MCP completada con salida 0 mediante empaquetado aislado; nueva carpeta y ZIP generados, sin reemplazar procesos o servidor activo. Se revisó el código y los esquemas por lectura. No se hicieron tests ni validadores ni se ejecutó la GUI/MCP nueva, por las reglas del proyecto. Comportamiento modal/visual pendiente de comprobar en uso. La conexión MCP existente no se actualiza hasta reiniciar/reconectar el servidor usando la nueva entrega.

No se activó esta opción ni se aceptó su advertencia por el agente. Sin puertos, flasheos ni compilación de firmware. ATmega/ESP/Web/CAD sin cambios de esta ficha. Checks automáticos y errores siguen bloqueando; ningún estado acredita cableado/aislación/alimentación medidos.
