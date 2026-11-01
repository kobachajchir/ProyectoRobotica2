# AFR2-20261029-06. Distribución por lista explícita; no copia perfiles ni datos del usuario.
[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
$destination = Join-Path (Split-Path $PSScriptRoot -Parent) 'AFR2-Loader-dist-1.1.2'
$archive = $destination + '.zip'
if ((Test-Path -LiteralPath $destination) -or (Test-Path -LiteralPath $archive)) { throw 'La distribución o su ZIP ya existen. Conservá esa entrega y elegí una nueva ubicación antes de regenerar.' }
$buildDirectory = Join-Path $PSScriptRoot ('.tmp/distribution-' + [Guid]::NewGuid().ToString('N'))
$gui = & (Join-Path $PSScriptRoot 'Compilar.ps1') -OutputDirectory $buildDirectory
$mcp = & (Join-Path $PSScriptRoot 'Compilar.ps1') -Mcp -OutputDirectory $buildDirectory
if (-not (Test-Path -LiteralPath $gui) -or -not (Test-Path -LiteralPath $mcp)) { throw 'Faltan ejecutables compilados.' }
[void][IO.Directory]::CreateDirectory($destination)
[void][IO.Directory]::CreateDirectory((Join-Path $destination 'runtime'))
Copy-Item -LiteralPath $gui -Destination (Join-Path $destination 'AFR2 Loader.exe')
Copy-Item -LiteralPath $mcp -Destination (Join-Path $destination 'runtime/AFR2Loader.Mcp.exe')
foreach ($name in @('MANUAL.html','MCP.md','FILESYSTEM_SCRIPTS.md','RegistrarMcp.ps1','perfil-ejemplo.json','paquete-at.plantilla.json')) {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot $name) -Destination $destination
}
foreach ($name in @('assets','scripts','tools')) {
    [void][IO.Directory]::CreateDirectory((Join-Path $destination $name))
}
[void][IO.Directory]::CreateDirectory((Join-Path $destination 'assets/brand'))
foreach ($name in @('afr2-loader-logo-blue.png','afr2-loader.ico','LogoUner.png','logoChicoUner.png')) {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot ('assets/brand/' + $name)) -Destination (Join-Path $destination 'assets/brand')
}
foreach ($name in @('Prepare-Web.ps1','Prepare-Files.example.ps1')) {
    Copy-Item -LiteralPath (Join-Path $PSScriptRoot ('scripts/' + $name)) -Destination (Join-Path $destination 'scripts')
}
Copy-Item -LiteralPath (Join-Path $PSScriptRoot 'tools/esp_bridge_esptool.py') -Destination (Join-Path $destination 'tools')
@'
[CmdletBinding()]
param([switch]$SoloCompilar, [ValidateSet('web','at')][string]$FirmwareEsp, [string]$Perfil)
$ErrorActionPreference = 'Stop'
try {
    $program = Join-Path $PSScriptRoot 'AFR2 Loader.exe'
    if ($SoloCompilar) { Write-Output $program; exit 0 } # Paquete ya compilado.
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
param()
$ErrorActionPreference = 'Stop'
try {
    [Console]::InputEncoding = New-Object System.Text.UTF8Encoding($false)
    [Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)
    $OutputEncoding = [Console]::OutputEncoding
    & (Join-Path $PSScriptRoot 'runtime/AFR2Loader.Mcp.exe') --mcp $PSScriptRoot
    exit $LASTEXITCODE
} catch { [Console]::Error.WriteLine('AFR2 MCP: ' + $_.Exception.Message); exit 1 }
'@ | Set-Content -LiteralPath (Join-Path $destination 'Mcp.ps1') -Encoding UTF8
@'
AFR2 Loader 1.1.2 — Distribución para compartir
Hecho por Koba Chajchir · Facultad de Ciencias de la Alimentación · UNER

1. Extraé toda la carpeta del ZIP a una ubicación local.
2. Abrí AFR2 Loader.exe. Conservá las carpetas runtime, assets, scripts y tools.
3. Consultá MANUAL.html. Configurá las rutas y herramientas de tu proyecto antes de compilar o cargar placas.

Esta entrega ya está compilada; no requiere código fuente ni compilador C# para abrirse.
Requiere Windows con .NET Framework 4.x y Windows PowerShell para los scripts/MCP.
Las herramientas de AVR, PlatformIO, esptool/Python y Node/npm, cuando tu script los use,
se instalan/configuran por separado. No se incluyen proyectos, dependencias, firmware,
paquetes AT, credenciales, perfiles personales, backups ni registros de otra persona.

FileSystem: elegí tu script en Configuración > ESP > Preparación FileSystem.
FILESYSTEM_SCRIPTS.md explica el contrato y los ejemplos. El script Web suministrado
requiere nuestro proyecto Web y su ESP hermano; para otros archivos usá uno propio.
La ruta inicial del script se obtiene de esta carpeta, no de la PC del desarrollador.

Servidor MCP: usá Mcp.ps1 de esta carpeta. El servidor está precompilado en runtime.
La GUI ofrece el prompt de configuración y Ver acciones MCP. Registrar el servidor
es opcional y no inicia cargas. No ejecutes el EXE de runtime como aplicación gráfica.

Iniciar.cmd/Iniciar.ps1 son accesos alternativos. SoloCompilar devuelve la ruta del
ejecutable existente; esta distribución no compila la aplicación al iniciarse.
Los perfiles y registros se guardan en LOCALAPPDATA/AutoFinalRobo2Loader de cada usuario.
Si ya usabas el Loader en esa PC, conserva tus ajustes existentes; revisá sus rutas.

Entrega compilada; no se abrió la GUI ni se probaron placas al preparar el paquete.
'@ | Set-Content -LiteralPath (Join-Path $destination 'LEEME.txt') -Encoding UTF8
$manualPath = Join-Path $destination 'MANUAL.html'
$manual = [IO.File]::ReadAllText($manualPath)
$manual = $manual.Replace('Este ejecutable inicia Iniciar.ps1 sin consola visible y muestra un aviso si falla. Conservá toda la carpeta: no es un instalador ni un EXE independiente.', 'Esta distribución abre directamente la aplicación precompilada, sin compilar al iniciar. Conservá toda la carpeta, incluido runtime para MCP. Consultá LEEME.txt para los requisitos de esta entrega.')
[IO.File]::WriteAllText($manualPath, $manual, (New-Object Text.UTF8Encoding($false)))
Compress-Archive -LiteralPath $destination -DestinationPath $archive -CompressionLevel Optimal
Write-Output $destination
Write-Output $archive

