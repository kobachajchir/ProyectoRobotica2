# AFR2-20261106-03. Koba Chajchir / AFR2; lista explícita sin datos personales.
[CmdletBinding()]
param([string]$Destination)
$ErrorActionPreference = 'Stop'
$versionSource = [IO.File]::ReadAllText((Join-Path $PSScriptRoot 'src/JobState.cs'))
$match = [regex]::Match($versionSource, 'public const string Value = "([0-9]+\.[0-9]+\.[0-9]+)"')
if (-not $match.Success) { throw 'No se pudo determinar LoaderVersion.' }
$version = $match.Groups[1].Value
if (-not $Destination) { $Destination = Join-Path (Split-Path $PSScriptRoot -Parent) ('AFR2-Loader-dist-' + $version) }
$destination = [IO.Path]::GetFullPath($Destination)
$archive = $destination + '.zip'
if ((Test-Path -LiteralPath $destination) -or (Test-Path -LiteralPath $archive)) { throw 'La distribución o el ZIP ya existen. Elegí otro -Destination; no se sobrescriben entregas.' }
$buildDirectory = Join-Path $PSScriptRoot ('.tmp/distribution-' + [Guid]::NewGuid().ToString('N'))
$gui = & (Join-Path $PSScriptRoot 'Compilar.ps1') -OutputDirectory $buildDirectory
$mcp = & (Join-Path $PSScriptRoot 'Compilar.ps1') -Mcp -OutputDirectory $buildDirectory
if (-not (Test-Path -LiteralPath $gui) -or -not (Test-Path -LiteralPath $mcp)) { throw 'Faltan ejecutables compilados.' }
[void][IO.Directory]::CreateDirectory($destination)
foreach ($directory in @('runtime','assets/brand','scripts','tools','referencias')) { [void][IO.Directory]::CreateDirectory((Join-Path $destination $directory)) }
Copy-Item -LiteralPath $gui -Destination (Join-Path $destination 'AFR2 Loader.exe')
Copy-Item -LiteralPath $mcp -Destination (Join-Path $destination 'runtime/AFR2Loader.Mcp.exe')
foreach ($name in @('MANUAL.html','MCP.md','FILESYSTEM_SCRIPTS.md','PORTABILIDAD.md','RegistrarMcp.ps1','perfil-ejemplo.json','paquete-at.plantilla.json')) { Copy-Item -LiteralPath (Join-Path $PSScriptRoot $name) -Destination $destination }
foreach ($name in @('afr2-loader-logo-blue.png','afr2-loader.ico','LogoUner.png','logoChicoUner.png')) { Copy-Item -LiteralPath (Join-Path $PSScriptRoot ('assets/brand/' + $name)) -Destination (Join-Path $destination 'assets/brand') }
foreach ($name in @('Prepare-Web.ps1','Prepare-Files.example.ps1')) { Copy-Item -LiteralPath (Join-Path $PSScriptRoot ('scripts/' + $name)) -Destination (Join-Path $destination 'scripts') }
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'tools/esp_bridge_esptool.py') -Destination (Join-Path $destination 'tools')
$references = @{ '../AutoFinalRobo2-Contracts/hardware/WIFI_SHIELD.md' = 'WIFI_SHIELD.md'; '../AutoFinalRobo2-Contracts/contracts/LOADER_FLASH_MODES.md' = 'LOADER_FLASH_MODES.md' }
foreach ($source in $references.Keys) { Copy-Item -LiteralPath (Join-Path $PSScriptRoot $source) -Destination (Join-Path $destination ('referencias/' + $references[$source])) }
@'
[CmdletBinding()]
param([switch]$SoloCompilar, [ValidateSet('web','at')][string]$FirmwareEsp, [string]$Perfil, [string]$Datos)
$ErrorActionPreference = 'Stop'
try {
    if ($Datos) { $env:AFR2_LOADER_DATA_DIR = [IO.Path]::GetFullPath($Datos) }
    $program = Join-Path $PSScriptRoot 'AFR2 Loader.exe'
    if ($SoloCompilar) { Write-Output $program; exit 0 }
    $arguments = @('"' + $PSScriptRoot + '"')
    if ($Perfil) {
        if ($Perfil -ne [IO.Path]::GetFileName($Perfil) -or $Perfil.IndexOfAny([IO.Path]::GetInvalidFileNameChars()) -ge 0 -or -not $Perfil.EndsWith('.json', [StringComparison]::OrdinalIgnoreCase)) { throw 'Perfil debe ser el nombre de un JSON guardado en la biblioteca local.' }
        $arguments += @('--profile', ('"' + $Perfil + '"'))
    }
    if ($FirmwareEsp) { $arguments += @('--esp-mode', $FirmwareEsp) }
    Start-Process -FilePath $program -ArgumentList ($arguments -join ' ') -WorkingDirectory $PSScriptRoot
} catch { [Console]::Error.WriteLine($_.Exception.Message); exit 1 }
'@ | Set-Content -LiteralPath (Join-Path $destination 'Iniciar.ps1') -Encoding UTF8
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'Iniciar.cmd') -Destination $destination
@'
[CmdletBinding()]
param([string]$Datos)
$ErrorActionPreference = 'Stop'
try {
    if ($Datos) { $env:AFR2_LOADER_DATA_DIR = [IO.Path]::GetFullPath($Datos) }
    [Console]::InputEncoding = New-Object System.Text.UTF8Encoding($false)
    [Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)
    $OutputEncoding = [Console]::OutputEncoding
    & (Join-Path $PSScriptRoot 'runtime/AFR2Loader.Mcp.exe') --mcp $PSScriptRoot
    exit $LASTEXITCODE
} catch { [Console]::Error.WriteLine('AFR2 MCP: ' + $_.Exception.Message); exit 1 }
'@ | Set-Content -LiteralPath (Join-Path $destination 'Mcp.ps1') -Encoding UTF8
@"
AFR2 Loader $version — Distribución para compartir
Hecho por Koba Chajchir · Facultad de Ciencias de la Alimentación · UNER

1. Extraé TODO el ZIP a una carpeta local.
2. Abrí AFR2 Loader.exe. Conservá runtime, assets, scripts, tools y las guías.
3. Configurá tus proyectos y herramientas en Configuración.
4. Leé PORTABILIDAD.md para rutas, requisitos, datos de usuario y MCP.

Windows 10/11 y .NET Framework 4.8 para abrir. No requiere C# ni Visual Studio.
Windows PowerShell 5.1 para scripts/MCP. AVR, PlatformIO, Python/esptool y
Node/npm se instalan aparte cuando la operación los necesita.
No incluye firmware, dependencias, proyectos, credenciales, preferencias,
backups ni registros del desarrollador. No instala herramientas al iniciar.

{App} y las rutas relativas parten de esta carpeta.
{AtmegaDir}, {EspDir}, {WebDir} siguen tus carpetas configuradas.
{Repo} es opcional para el árbol AFR2; los módulos pueden estar separados.
El script por defecto es {App}/scripts/Prepare-Web.ps1.
Las herramientas descubiertas son las del usuario de ESTA computadora.
Revisá entorno PlatformIO, flash y linker; los ejemplos no miden hardware.

Los datos van a LOCALAPPDATA/AutoFinalRobo2Loader del usuario actual.
AFR2_LOADER_DATA_DIR o Iniciar.ps1 -Datos permiten elegir otro destino.
MCP y GUI deben usar el MISMO usuario y destino de datos.
Compartí este ZIP original, sin añadir tus perfiles personales.
Los perfiles anteriores se conservan; revisá sus rutas personalizadas.

Abrir no abre COM ni flashea. Preparar esta entrega no compila firmware.
"@ | Set-Content -LiteralPath (Join-Path $destination 'LEEME.txt') -Encoding UTF8
foreach ($name in @('MCP.md','MANUAL.html','FILESYSTEM_SCRIPTS.md','referencias/WIFI_SHIELD.md','referencias/LOADER_FLASH_MODES.md')) {
    $path = Join-Path $destination $name
    $document = [IO.File]::ReadAllText($path)
    $document = $document.Replace('D:\ProyectoRobotica2\AutoFinalRobo2-Loader', '.').Replace('D:/ProyectoRobotica2/AutoFinalRobo2-Loader', '.')
    $document = $document.Replace('D:\ProyectoRobotica2', 'CARPETA_DEL_PROYECTO').Replace('D:/ProyectoRobotica2', 'CARPETA_DEL_PROYECTO')
    foreach ($source in $references.Keys) { $document = $document.Replace($source, 'referencias/' + $references[$source]) }
    [IO.File]::WriteAllText($path, $document, (New-Object Text.UTF8Encoding($false)))
}
$manifestFiles = @(Get-ChildItem -LiteralPath $destination -Recurse -File | Sort-Object FullName | ForEach-Object { [ordered]@{ path = $_.FullName.Substring($destination.Length + 1).Replace('\','/'); bytes = $_.Length; sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant() } })
[ordered]@{ application = 'AFR2 Loader'; version = $version; schema = 1; containsUserData = $false; externalToolsBundled = $false; files = $manifestFiles } | ConvertTo-Json -Depth 6 | Set-Content -LiteralPath (Join-Path $destination 'distribucion.json') -Encoding UTF8
Compress-Archive -LiteralPath $destination -DestinationPath $archive -CompressionLevel Optimal
(Get-FileHash -LiteralPath $archive -Algorithm SHA256).Hash.ToLowerInvariant() + '  ' + [IO.Path]::GetFileName($archive) | Set-Content -LiteralPath ($archive + '.sha256') -Encoding ASCII
Write-Output $destination
Write-Output $archive
