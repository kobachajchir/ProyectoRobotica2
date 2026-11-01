# AFR2-20261029-03. Genera el icono y el acceso ejecutable; no inicia la GUI.
[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
Add-Type -TypeDefinition 'using System; using System.Runtime.InteropServices; public static class AFR2IconNative { [DllImport("user32.dll")] public static extern bool DestroyIcon(IntPtr handle); }'
$brand = Join-Path $PSScriptRoot 'assets/brand'
$iconPath = Join-Path $brand 'afr2-loader.ico'
$image = [Drawing.Image]::FromFile((Join-Path $brand 'afr2-loader-logo-blue.png'))
try {
    $bitmap = New-Object Drawing.Bitmap 64, 64
    try {
        $graphics = [Drawing.Graphics]::FromImage($bitmap)
        try { $graphics.Clear([Drawing.Color]::Transparent); $graphics.DrawImage($image, 0, 0, 64, 64) } finally { $graphics.Dispose() }
        $handle = $bitmap.GetHicon()
        try {
            $icon = [Drawing.Icon]::FromHandle($handle)
            $file = [IO.File]::Create($iconPath)
            try { $icon.Save($file) } finally { $file.Dispose(); $icon.Dispose() }
        } finally { [void][AFR2IconNative]::DestroyIcon($handle) }
    } finally { $bitmap.Dispose() }
} finally { $image.Dispose() }
$compiler = Join-Path ([Runtime.InteropServices.RuntimeEnvironment]::GetRuntimeDirectory()) 'csc.exe'
if (-not (Test-Path -LiteralPath $compiler)) { $compiler = Join-Path $env:WINDIR 'Microsoft.NET/Framework64/v4.0.30319/csc.exe' }
if (-not (Test-Path -LiteralPath $compiler)) { $compiler = Join-Path $env:WINDIR 'Microsoft.NET/Framework/v4.0.30319/csc.exe' }
if (-not (Test-Path -LiteralPath $compiler)) { throw 'No se encontró el compilador de .NET Framework 4.x.' }
$temporary = Join-Path $PSScriptRoot ('AFR2-Launcher-' + [Guid]::NewGuid().ToString('N') + '.exe')
try {
    & $compiler /nologo /target:winexe /optimize+ /codepage:65001 /reference:System.Core.dll /reference:System.Windows.Forms.dll ('/win32icon:' + $iconPath) ('/out:' + $temporary) (Join-Path $PSScriptRoot 'launcher/Launcher.cs')
    if ($LASTEXITCODE -ne 0) { throw 'No se pudo generar el lanzador AFR2 Loader.' }
    Move-Item -LiteralPath $temporary -Destination (Join-Path $PSScriptRoot 'AFR2 Loader.exe') -Force
} finally {
    if (Test-Path -LiteralPath $temporary) { Remove-Item -LiteralPath $temporary -Force }
}
Write-Output (Join-Path $PSScriptRoot 'AFR2 Loader.exe')
