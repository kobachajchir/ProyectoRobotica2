# AFR2-20261029-13 — Loader 1.1.2: intervención física ESP obligatoria

Pedido: mantener el aviso físico KEY/RST del ESP aun con omisión de confirmaciones, restaurar la app minimizada y traer el aviso al frente. ATmega no requiere este paso. Usuario lo denomina SP/admin; se aplica a ESP/ATmega según contexto y botones reales KEY/RST del shield.

Autoría Koba Chajchir / AFR2 conservada; adaptación mediante Codex.

## Adaptaciones por archivo

- AutoFinalRobo2-Loader/src/MainForm.EspIntervention.cs: presentación local específica fuera de Confirm, restaura owner si está minimizado, selecciona Operación, solicita primer plano y sube el diálogo temporalmente. Casilla obligatoria no premarcada y Confirmar deshabilitado hasta intervención declarada. Cancelar no permite seguir; token se revisa antes y después del diálogo.
- src/MainForm.Workflow.cs: elimina la omisión del asistente de escucha por autorización de sesión. Aviso explícito KEY/RST del ESP y restauración/primer plano antes de mostrar. Conserva Continuar→puerto listo→evidencia de modo programación.
- src/MainForm.cs: conecta el callback obligatorio EspPhysicalConfirmation a cada instancia de Operations.
- src/Operations.cs: callback físico separado y error bloqueante si falta BootAssistant al escuchar. No cambia escritura/verificación.
- src/Operations.MegaBridge.cs: sin escucha usa el nuevo aviso obligatorio; puente Mega también pasa por él en lugar del Confirm omitible. Si falta asistente o no se confirma, no se consulta ROM/escribe. Conserva baud fijo y RESET Mega libre; no introduce escucha 74880 en el flasher.
- src/MainForm.EspModes.cs: renombra Guiar KEY/RST a Escuchar arranque ESP (74880), con tooltip que indica que desmarcar no elimina la intervención física.
- src/MainForm.SessionChecks.cs: advertencia actualizada con excepción obligatoria KEY/RST.
- src/McpServer.cs: descripciones de skipManualChecks/listenBoot actualizadas; no se agrega endpoint capaz de confirmar intervención por servidor.
- src/JobState.cs y launcher/Launcher.cs: versión 1.1.2 / metadatos 1.1.2.0.
- Preparar-Distribucion.ps1: nueva carpeta/ZIP 1.1.2, conserva entregas anteriores.
- MANUAL.html, MCP.md, README.md: excepción, secuencia y límites; Contracts/contracts/LOADER_SESSION_CHECKS.md actualizado e índices referencian esta ficha.

Alcance: firmware ESP propio/AT, FileSystem y consulta ROM pasan por preparación ESP y asistencia física. Las compilaciones de contenido no requieren botones. ATmega conserva el flujo anterior; no se exige KEY/RST al cargar el Mega. Confirmar intervención no acredita respuesta ROM: esta se comprueba después. Potencia, aislación, cableado y GPIO no se miden por el Loader.

## Estado real

GUI y MCP compilados con salida 0 mediante empaquetado aislado. Distribución AFR2-Loader-dist-1.1.2 y ZIP generados. Sin ejecución GUI ni MCP de esta revisión, tests, validadores, puertos, compilación/carga de firmware o pruebas físicas. Pendiente comprobar visualmente la restauración/foco en Windows, incluida su política de primer plano, y el asistente en cada transporte. No se detuvieron servidores/procesos existentes ni se activó la omisión por el agente.

ATmega/ESP/Web/CAD sin cambios de producto por esta ficha. La modificación corresponde al Loader, no a una versión de firmware de las placas.
