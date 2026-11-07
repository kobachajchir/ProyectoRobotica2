# AFR2-20261107-45 — Loader 2.0.0 portable: sin rutas de esta PC y ZIP verificado

07/11/2026. Pedido del usuario: que el Loader 2.0.0 funcione en cualquier PC con Windows, sin rutas absolutas del equipo de desarrollo, y compilar y armar un ZIP listo para compartir.

Estado real: loader = compilado (GUI y MCP con csc de .NET Framework 4.x) y ZIP generado; sin abrir en otra PC. atmega/esp/web = sin cambios. No se abrieron puertos COM ni se flasheó.

## Revisión

- Fuentes C# (`src/`, `launcher/Launcher.cs`): ya resolvían todo desde `{App}` (carpeta del EXE o argumento de inicio), `%LOCALAPPDATA%\AutoFinalRobo2Loader` o `AFR2_LOADER_DATA_DIR`. Las herramientas se descubren en PATH, `PLATFORMIO_CORE_DIR` o `%USERPROFILE%\.platformio`, Arduino15 del usuario y Program Files. `{Repo}` se guarda relativo a `{App}`. Sin cambios.
- Perfiles de ejemplo (`perfil-ejemplo.json`, `paquete-at.plantilla.json`): solo referencias `{Repo}`, `{App}`, `{AtmegaDir}`, `{EspDir}`, `{Env}` y `%SystemRoot%`. Sin cambios.
- `codex-mcp.example.toml` usa el marcador `C:/RUTA/...` y no se distribuye.

## Cambios

- `Preparar-Distribucion.ps1`: quitados los reemplazos fijos de `D:\ProyectoRobotica2`; la carpeta del Loader y la del proyecto se calculan desde `$PSScriptRoot`. `PORTABILIDAD.md` también pasa por ese saneamiento.
- `Preparar-Distribucion.ps1`: antes de comprimir revisa cada archivo, incluidos los EXE (ASCII y UTF-16), y cancela el ZIP si aparece la carpeta del Loader o del proyecto, `USERPROFILE`, `LOCALAPPDATA`, `APPDATA`, `TEMP`, `PLATFORMIO_CORE_DIR`, el usuario o el nombre del equipo, o una ruta con letra de unidad en archivos de texto (se admite el marcador `C:\ruta\` de MCP.md).
- `Preparar-Distribucion.ps1`: borra la carpeta temporal de compilación (`.tmp/distribution-*`, que contiene el `.appdir` con la ruta local) y arma el ZIP con `System.IO.Compression` y separadores `/`. `Compress-Archive` de PowerShell 5.1 guardaba `\`, que otros extractores no interpretan.
- `Compilar-Loader-2.cmd`: el comentario tenía un carácter de retroceso en lugar de `\b` (de `...Loader\bin`) y doble CR por línea; reescrito en CRLF.
- `README.md`: el ejemplo `cd "D:\ProyectoRobotica2\AutoFinalRobo2-Loader"` pasa a un marcador genérico.
- `PORTABILIDAD.md`: título 2.0.0 (decía 1.2.0) y explicación del escaneo y del comando de empaquetado.

## Entrega

- ZIP: `AFR2-Loader-dist-2.0.0.zip` junto a la carpeta del Loader (raíz del proyecto; ignorado por Git), 1 425 336 bytes, SHA-256 `02244b74637d88700fb2cb85b079f9abc70a8ec4e2c4a43127eb54901b288b6e` (también en `.zip.sha256`). Carpeta extraída `AFR2-Loader-dist-2.0.0/` con `distribucion.json`.
- Versión 2.0.0: no cambió el código C#, solo scripts y documentación.
- Verificación propia aparte del escaneo del script: se extrajo el ZIP con `unzip` (sin avisos) y se buscaron `kobac`, `ProyectoRobotica`, `Users\`, `D:\`, `C:\`, `AppData`, el nombre del equipo y `Python3` en ASCII y UTF-16. En los EXE solo aparecen textos del programa: el enlace público de GitHub del autor, la ayuda «Carpeta ProyectoRobotica2» del campo Raíz y los nombres relativos `penv\Scripts\python.exe` y `.appdir`. No aparecen rutas, usuario, equipo ni credenciales.

## Sin probar

1. Abrir `AFR2 Loader.exe` desde el ZIP extraído en otra PC u otro usuario de Windows (con .NET Framework 4.8), incluso en una carpeta con espacios.
2. Detectar rutas vacías con herramientas instaladas en otras ubicaciones.
3. Registrar el MCP desde la carpeta extraída (`RegistrarMcp.ps1` o el prompt de la GUI) y `loader_status`.
4. Operaciones con placas: no se abrieron puertos ni se cargó firmware.
