# AFR2-20261029-06 — Distribución AFR2-Loader-dist-1.1

Pedido: limpiar una copia para compartir, conservando solo archivos de funcionamiento y rotulándola AFR2-Loader-dist-1.1. Desarrollo original preservado.

Autoría Koba Chajchir / AFR2 conservada; adaptación mediante Codex.

Entregables generados bajo D:/ProyectoRobotica2:
- AFR2-Loader-dist-1.1/: aplicación gráfica precompilada como AFR2 Loader.exe, runtime MCP, entradas opcionales Iniciar/Mcp/RegistrarMcp, logos, manual, documentación de scripts/MCP, plantillas de perfil/paquete AT, scripts de contenido y puente esptool.
- AFR2-Loader-dist-1.1.zip: misma carpeta lista para compartir.

Sin src, launcher de desarrollo, compiladores C#, generadores, instrucciones AGENTS, cachés, dependencias instaladas, proyectos de producto, firmware, perfiles de usuario, credenciales, registros ni backups personales. El EXE principal es la app gráfica ya compilada, no el lanzador que compilaba al inicio. Iniciar.ps1 y Mcp.ps1 de la copia usan los binarios incluidos. LEEME.txt documenta requisitos y la conservación de carpetas.

Adaptaciones por archivo:
- AutoFinalRobo2-Loader/Preparar-Distribucion.ps1: empaquetado reproducible mediante lista explícita, genera entradas de runtime y LEEME, compila en directorio temporal aislado y crea ZIP. Rechaza sobrescribir una entrega existente.
- src/Profile.cs: ruta predeterminada de Prepare-Web.ps1 relativa a la carpeta de la aplicación, también fuera del repositorio original. Perfiles existentes conservan sus valores y requieren revisar rutas al migrar de PC.
- Compilar.ps1: OutputDirectory opcional para crear artefactos aislados sin reemplazar ejecutables activos; comportamiento anterior por defecto.
- .gitignore raíz: excluye carpetas/ZIP de distribución regenerables.

Compilación GUI y MCP completada con salida 0; inventario de archivos leído tras el empaquetado. Un primer intento encontró bloqueado el EXE MCP en LOCALAPPDATA; se agregó compilación aislada y se completó sin detener servidores ni procesos existentes.

Sin ejecución de la GUI ni MCP distribuido, tests, validadores, COM, builds AVR/ESP/Web, cargas ni pruebas físicas. Los destinatarios necesitan Windows/.NET Framework 4.x y herramientas externas según sus tareas (AVR, PlatformIO, Python/esptool y Node/npm solo si el script elegido los necesita). No es instalador autocontenido ni incluye el código/proyecto del robot. ATmega/ESP/Web/CAD sin cambios de producto por esta ficha.
