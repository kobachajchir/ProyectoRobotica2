[CmdletBinding()]
param([switch]$Mcp, [string]$OutputDirectory)
$ErrorActionPreference = 'Stop'
# Solo devuelve la ruta del EXE por stdout; mensajes de compilación por stderr.
$sourceRoot = Join-Path $PSScriptRoot 'src'
$outputRoot = Join-Path $env:LOCALAPPDATA 'AutoFinalRobo2Loader/bin'
if ($OutputDirectory) { $outputRoot = [IO.Path]::GetFullPath($OutputDirectory) }
[void][System.IO.Directory]::CreateDirectory($outputRoot)
$name = if ($Mcp) { 'AFR2Loader.Mcp' } else { 'AFR2Loader' }
$program = Join-Path $outputRoot ($name + '.exe')
$sources = @(Get-ChildItem -LiteralPath $sourceRoot -Filter '*.cs' | Sort-Object Name)
if ($sources.Count -eq 0) { throw 'No se encontraron los fuentes src/*.cs.' }
$stamp = Join-Path $outputRoot ($name + '.sha256')
$compiler = Join-Path ([System.Runtime.InteropServices.RuntimeEnvironment]::GetRuntimeDirectory()) 'csc.exe'
if (-not (Test-Path -LiteralPath $compiler)) { $compiler = Join-Path $env:WINDIR 'Microsoft.NET/Framework64/v4.0.30319/csc.exe' }
if (-not (Test-Path -LiteralPath $compiler)) { $compiler = Join-Path $env:WINDIR 'Microsoft.NET/Framework/v4.0.30319/csc.exe' }
$targetKind = if ($Mcp) { 'exe' } else { 'winexe' }
$sourceIdentity = 'afr2-launcher-v2-' + $targetKind + "`n" + (@($sources | ForEach-Object { $_.Name + ':' + (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash }) -join "`n")
$iconPath = Join-Path $PSScriptRoot 'assets/brand/afr2-loader.ico'
if (Test-Path -LiteralPath $iconPath) { $sourceIdentity += "`nicon:" + (Get-FileHash -LiteralPath $iconPath -Algorithm SHA256).Hash }
$rebuild = (-not (Test-Path -LiteralPath $program)) -or (-not (Test-Path -LiteralPath $stamp))
if (-not $rebuild) { $rebuild = [System.IO.File]::ReadAllText($stamp) -ne $sourceIdentity }
if ($rebuild) {
    if (-not (Test-Path -LiteralPath $compiler)) { throw 'Falta csc.exe de .NET Framework 4.x. Consultar README.md.' }
    [Console]::Error.WriteLine('Preparando ' + $name + ' para este usuario; no compila ni flashea el robot...')
    $temporary = Join-Path $outputRoot ($name + '.' + [Guid]::NewGuid().ToString('N') + '.exe')
    try {
        $compilerArgs = @('/nologo', ('/target:' + $targetKind), '/optimize+', '/codepage:65001',
            '/reference:System.dll', '/reference:System.Core.dll', '/reference:System.Drawing.dll',
            '/reference:System.Windows.Forms.dll', '/reference:System.Web.Extensions.dll', '/debug-',
            ('/out:' + $temporary)) + @($sources | ForEach-Object { $_.FullName })
        if (Test-Path -LiteralPath $iconPath) { $compilerArgs += ('/win32icon:' + $iconPath) }
        $compilerOutput = @(& $compiler @compilerArgs 2>&1)
        $compilerExit = $LASTEXITCODE
        foreach ($line in $compilerOutput) { [Console]::Error.WriteLine([string]$line) }
        if ($compilerExit -ne 0) { throw 'No se pudo generar el cargador. Conservar el error de compilación.' }
        try { [System.IO.File]::Copy($temporary, $program, $true) }
        catch { throw 'No se puede actualizar el ejecutable. Cerrá el Loader o desconectá el servidor MCP que lo esté usando y volvé a iniciar.' }
        [System.IO.File]::WriteAllText($stamp, $sourceIdentity)
    } finally {
        if (Test-Path -LiteralPath $temporary) { Remove-Item -LiteralPath $temporary -Force }
    }
}
# AFR2-20261106-16: local-only runtime resource hint; never shipped in the allowlisted ZIP.
$resourceHint = Join-Path $outputRoot ($name + '.appdir')
$resourceDirectory = [IO.Path]::GetFullPath($PSScriptRoot)
if (-not (Test-Path -LiteralPath $resourceHint) -or [IO.File]::ReadAllText($resourceHint) -ne $resourceDirectory) {
    [IO.File]::WriteAllText($resourceHint, $resourceDirectory, (New-Object Text.UTF8Encoding($false)))
}
Write-Output $program
