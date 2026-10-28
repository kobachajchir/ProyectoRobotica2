[CmdletBinding()]
param(
    [switch]$SoloCompilar,
    [ValidateSet('web', 'at')][string]$FirmwareEsp,
    [string]$Perfil
)
$ErrorActionPreference = 'Stop'
try {
    $program = & (Join-Path $PSScriptRoot 'Compilar.ps1')
    if (-not $SoloCompilar) {
        $loaderArguments = @('"' + $PSScriptRoot + '"')
        if ($Perfil) {
            if ($Perfil -ne [System.IO.Path]::GetFileName($Perfil) -or $Perfil.IndexOfAny([System.IO.Path]::GetInvalidFileNameChars()) -ge 0 -or -not $Perfil.EndsWith('.json', [StringComparison]::OrdinalIgnoreCase)) {
                throw 'Perfil debe ser el nombre de un JSON guardado en la biblioteca local, sin carpetas.'
            }
            $loaderArguments += @('--profile', ('"' + $Perfil + '"'))
        }
        if ($FirmwareEsp) { $loaderArguments += @('--esp-mode', $FirmwareEsp) }
        # Ventana visible solicitada: abrir el Loader. Elegir modo no abre COM ni carga.
        Start-Process -FilePath $program -ArgumentList ($loaderArguments -join ' ') -WorkingDirectory $PSScriptRoot
    }
} catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
