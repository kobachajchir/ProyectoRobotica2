[CmdletBinding()]
param([switch]$SoloCompilar)
$ErrorActionPreference = 'Stop'
try {
    $program = & (Join-Path $PSScriptRoot 'Compilar.ps1')
    if (-not $SoloCompilar) {
        Start-Process -FilePath $program -ArgumentList ('"' + $PSScriptRoot + '"') -WorkingDirectory $PSScriptRoot
    }
} catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
