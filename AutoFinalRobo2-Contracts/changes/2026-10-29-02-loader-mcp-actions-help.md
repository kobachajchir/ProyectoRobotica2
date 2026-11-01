# AFR2-20261029-02 — Acceso a acciones MCP desde la GUI

Pedido: botón Ver acciones MCP en Servidor MCP, enlazado con la ayuda de todas las acciones, manteniendo versión 1.1.

Autoría Koba Chajchir / AFR2 conservada; adaptación mediante Codex.

- `AutoFinalRobo2-Loader/src/MainForm.Integration.cs`: botón Ver acciones MCP en la fila autoajustable disponible debajo de los controles del servidor. Abre la ayuda local mediante OpenManualSection("mcp-acciones"); no inicia solicitudes ni requiere servidor conectado.
- `AutoFinalRobo2-Loader/MANUAL.html`: sección mcp-acciones con las nueve herramientas públicas y las doce operaciones declaradas en McpServer.cs. Incluye consulta, configuración, captura, scripts FileSystem, diagnóstico, flasher, backup, cargas, seguimiento y cancelación; enlace desde la ayuda de conexión MCP.

Versión conservada: 1.1.0. Fuente y documentación editadas; cotejo manual con el código de herramientas/acciones. Sin compilaciones, tests, validadores, GUI, ejecución MCP, COM ni flasheos. ATmega/ESP/Web/CAD sin cambios por esta ficha.
