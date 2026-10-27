# AutoFinalRobo2 - Loader Windows

Módulo propio de D:/ProyectoRobotica2. Hereda las instrucciones de AGENTS.md raíz.
Leer README.md y ../AutoFinalRobo2-Contracts/README.md antes de modificarlo.
Comparte el único Git global: no inicializar un repositorio ni submódulo aquí.

- Fuentes C# WinForms en src/, lanzadores Iniciar.cmd e Iniciar.ps1, perfiles
  de ejemplo y manual local pertenecen a este módulo.
- Rutas, puerto, herramientas y layout deben ser configurables por usuario;
  no fijar D:/ProyectoRobotica2, COM16 ni nombres de usuario en el programa.
- Contracts conserva contratos y fichas AFR2 de cambios compartidos.
- No ejecutar tests, validadores, compilación de la herramienta o firmware,
  escucha de puertos ni flasheos salvo pedido expreso.
- Conservar la distinción entre log de arranque y comunicación ROM, confirmación
  de cableado, escritura y verificación. No declarar hardware probado por compilar.
- Perfiles, ejecutables y registros de usuario van a LOCALAPPDATA, no a Git.
- MCP stdio usa Mcp.ps1; stdout contiene únicamente JSON-RPC. Diagnósticos y
  compilación del lanzador van por stderr. No registrar globalmente al iniciar.
- MCP comparte la ventana mediante LocalBridge. Mantener revisión GUI,
  deduplicación, exclusión de trabajos simultáneos y asistentes físicos. No
  agregar ejecución libre de shell ni declarar conectado por solo registrarlo.
- Preservar trabajo ajeno y módulos Atmega/ESP/Web/CAD fuera del alcance solicitado.
