# AFR2-20261029-22 — PowerShell bloquea la preparación FileSystem

El usuario informa salida 1 / UnauthorizedAccess al ejecutar Prepare-Web.ps1 desde PrepareFilesystemFiles → BuildWebFs → BuildAll. PowerShell rechaza el script porque la ejecución está deshabilitada. Este error no acredita un intento de escritura ni un fallo del ESP.

Adaptación propia AFR2 por Codex, conservando autorías existentes. Profile.cs agrega -ExecutionPolicy Bypass antes de -File al valor predeterminado de FsScriptArguments y a su ayuda. Se aplica únicamente al proceso hijo; no se ejecuta Set-ExecutionPolicy ni se cambian políticas persistentes. Las políticas de grupo pueden prevalecer. Los argumentos personalizados y perfiles ya guardados se conservan.

Para resolverlo en la aplicación instalada: Configuración → ESP → Preparación FileSystem → Argumentos del script (JSON), usar `["-NoProfile","-ExecutionPolicy","Bypass","-File","{Script}"]` y guardar. No necesita recompilar el Loader. Preparar de nuevo cuando corresponda: cambiar argumentos invalida la preparación anterior. FILESYSTEM_SCRIPTS.md y MANUAL.html incluyen esta corrección y actualizan ejemplos PowerShell; Python conserva sus argumentos.

Estado: Loader corregido solo en fuente/documentación; ejecutable y distribución sin regenerar. Sin tests, validadores, compilaciones, ejecución del script, apertura COM, flasheos ni pruebas físicas. No se modificaron perfiles locales ni políticas de Windows. atmega/esp/web sin cambios de fuente, imágenes ni carga por esta ficha. El error original sigue siendo la única evidencia de ejecución de este incidente.

Adaptaciones por archivo en provenance/loader-powershell-filesystem-20261029-22.json.
