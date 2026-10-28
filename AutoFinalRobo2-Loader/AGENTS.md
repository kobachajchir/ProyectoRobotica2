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
- SerialCapture es dueño exclusivo del COM y sus archivos; esperar el cierre
  antes de entregar el puerto a esptool/avrdude, también en operaciones MCP.
  El monitor es de lectura: no enviar comandos ni activar DTR/RTS por defecto.
- Un mensaje UART no acredita carga verificada. Mantener eventos tipados de
  escritura/verificación y el error original/código/salida en los informes.
- Loader0.9: separar mega_bridge (D18/D19, RESET libre, baud fijo) de mega_reset
  (D0/D1, RESET-GND). No pedir74880 en el puente fijo ni cambiar de montaje
  silenciosamente. El wrapper espera después de abrir el mismo COM para esptool.
- Backup opcional antes de instalar el flasher: dos lecturas flash/hfuse,
  comparación y hashes antes de publicar el último respaldo. Un fallo bloquea
  la carga; no sustituirlo por una copia de HEX de PC. Conservar los históricos.
  Flashear último backup usa la región de aplicación y lectura posterior; no
  programar bootloader/fuses/EEPROM. Aviso al omitir configurable en Preferencias.
  Flashear Firmware ATmega usa el HEX configurado; son acciones diferentes.
- MCP stdio usa Mcp.ps1; stdout contiene únicamente JSON-RPC. Diagnósticos y
  compilación del lanzador van por stderr. No registrar globalmente al iniciar.
- MCP comparte la ventana mediante LocalBridge. Revisión GUI por defecto;
  autoaceptación opcional solo cuando el usuario marca la casilla de esa sesión,
  conforme al pedido AFR2-20261027-21. Registrar el modo de aprobación, mantener
  deduplicación, exclusión de trabajos simultáneos y asistentes físicos. No
  agregar ejecución libre de shell ni declarar conectado por solo registrarlo.
- Preservar trabajo ajeno y módulos Atmega/ESP/Web/CAD fuera del alcance solicitado.
- Loader0.6 incorpora carga USB opcional de paquetes ESP8266 AT para otros usos,
  por pedido AFR2-20261027-22. No convierte el firmware de producto a AT. Separar
  reglas Web/linker de AT/paquete del proveedor, con offsets/regiones explícitos;
  no inferir un filesystem AT ni trasladar offsets de ESP32 al ESP8266. Cambiar
  el layout requiere imágenes correspondientes y consulta de flash antes de escribir.
