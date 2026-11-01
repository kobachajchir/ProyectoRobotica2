# AFR2-20261029-21 — Verificar flasher instalado

Pedido humano: comprobar desde Codex/Loader si el servicio flasher está instalado en ATmega, agregar botón y recargar el Loader.

Loader1.1.3: botón Verificar flasher en Flasher ATmega, junto a Detectar ESP. Operación MCP verify_mega_flasher por loader_request_operation; comparte cola, exclusión del COM, cierre del monitor, cancelación, registro y revisión existentes. No ejecuta al abrir ni cambia el método ESP seleccionado.

El servicio esp_uart_bridge.c es binario transparente y no tiene handshake propio. Se conserva sin modificar. La nueva operación prepara el HEX de referencia de la variante BridgeBaud, solicita auto-reset y usa avrdude -n -D -U flash:v:archivo.hex:i por el bootloader Mega. Exige firma0x1e9801, salida0, confirmación positiva de bytes verificados y referencias fuente/HEX sin cambios. No escribe flash/EEPROM/fuses ni envía una sonda a través del ESP. Identifica coincidencia exacta con esta variante, no cualquier programa llamado flasher; un firmware distinto o baud de variante diferente no se acredita.

El auto-reset interrumpe temporalmente el programa instalado. No comprueba ejecución del puente, UART1, cableado ni respuesta ROM ESP. El registro local anterior por sí solo no confirma el dispositivo; se agrega fecha/resultado de comprobación. Un fallo o cancelación deja estado unconfirmed y evita reutilizar evidencia positiva. El resultado GUI/etapas distingue comprobación de carga; sin etiqueta falsa de escritura. El backup anterior se conserva.

Archivos: Operations.VerifyMegaFlasher.cs nuevo; Operations.MegaBridge.cs campos de evidencia; MainForm.MegaBridge.cs botón/resumen; MainForm.Integration.cs y McpServer.cs allowlist/dispatch; Operations.cs y MainForm.Workflow.cs etapa de comparación sin escritura; JobState.cs versión1.1.3. Manual/README/MCP actualizados. ATmega/ESP/Web sin cambios de fuente.

Validación autorizada: GUI y MCP compilados con .NET Framework csc (PowerShell Windows), sin errores. El primer intento con pwsh no localizó csc del Framework; corregido usando powershell.exe. Sin tests ni consultas COM ni flasheo en esta revisión. Loader recargado; comprobar versión mediante loader_status. La verificación física queda para pulsar el botón o pedir verify_mega_flasher.

[Evidencia de compilación y recarga](../documentation/builds/2026-10-29-21/README.md).
