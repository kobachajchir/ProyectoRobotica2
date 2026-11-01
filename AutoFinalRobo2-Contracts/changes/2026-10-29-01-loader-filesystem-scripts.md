# AFR2-20261029-01 — Loader 1.1.0: preparación FileSystem por scripts

Solicitud: desacoplar la preparación del FileSystem de nuestra Web/Vite/npm y migrar también el flujo propio al nuevo lanzador. Documentar cómo crear y vincular scripts.

## Adaptaciones y autoría por archivo

Autoría original Koba Chajchir / AFR2 conservada; adaptación mediante Codex por pedido humano.

- `AutoFinalRobo2-Loader/src/Profile.cs`: campos FsScript, FsSourceDir, FsScriptHost y FsScriptArguments; argumentos JSON, rutas configurables y migración aditiva de perfiles schema 1. Fuentes iniciales heredadas de WebDir.
- `src/MainForm.Configuration.cs`: sección ESP/Preparación FileSystem.
- `src/Operations.Filesystem.cs`: lanzador de script con entorno explícito, carpetas de entradas/salida separadas y rechazo de salida vacía.
- `src/Operations.cs`: elimina npm/package.json/manifiesto del núcleo; ejecuta script y después buildfs; conserva tamaño, recibos/layout y preparación por sesión. Exige ruta FsBin correspondiente al entorno.
- `src/Runner.cs`: entorno adicional por proceso; conserva salida, timeout y cancelación existentes.
- `src/BuildPreparation.cs`: registra script, intérprete, entradas y salida completa para invalidar cambios antes de cargar. Exclusiones de entradas documentadas.
- `src/MainForm.Integration.cs`: detalle MCP muestra script, intérprete, argumentos y fuentes; build_web_fs conserva ID y usa el flujo común.
- `src/JobState.cs`: versión 1.1.0.
- `scripts/Prepare-Web.ps1`: nuestra preparación usa el mismo lanzador; npm run build:esp y verificación del manifiesto quedan en este script específico. Conserva promotor y presupuesto propios, exige ESP hermano de la Web.
- `scripts/Prepare-Files.example.ps1`: ejemplo no Web, copia payload con destino vacío; sin borrados silenciosos.
- `MANUAL.html`, `FILESYSTEM_SCRIPTS.md`, `README.md`, `MCP.md`: configuración, contrato, migración, ejemplos y límites.

## Estado real

Loader: cambios en fuente/documentación, lectura manual del código; scripts no ejecutados. Sin tests, validadores, compilación, GUI, puertos, flasheos ni pruebas físicas. Versión solicitada, no certificación de funcionamiento.

ATmega/ESP/Web: fuentes de producto y paquetes sin cambios por esta ficha. No se compiló ni promovió contenido. CAD sin intervención.

El contenido puede ser arbitrario; la imagen sigue siendo LittleFS generado por PlatformIO desde ESP/data con el firmware/layout correspondiente. No generaliza el compilador del firmware ni el formato FS de paquetes AT. El script gestiona archivos previos; errores pueden dejar salida parcial, sin nueva preparación acreditada. No se da por comprobada compatibilidad física, memoria disponible ni carga real.
