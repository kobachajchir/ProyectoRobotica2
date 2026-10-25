# AFR2-20261023-12. Windows launcher; diagnostics only, never flash/erase.
[CmdletBinding()]
param(
    [string]$Port,
    [string]$Python,
    [switch]$CheckOnly,
    [switch]$ListPorts,
    [switch]$CheckTools
)
$ErrorActionPreference = 'Stop'
$probeArgs = @((Join-Path $PSScriptRoot 'esp_probe.py'))
if ($Port) { $probeArgs += @('--port', $Port) }
if ($CheckOnly) { $probeArgs += '--check-only' }
if ($ListPorts) { $probeArgs += '--list-ports' }
if ($CheckTools) { $probeArgs += '--check-tools' }
if ($Python) {
    & $Python @probeArgs
} else {
    $pioPython = Join-Path $env:USERPROFILE '.platformio\penv\Scripts\python.exe'
    if (Test-Path -LiteralPath $pioPython) { & $pioPython @probeArgs }
    elseif (Get-Command py -ErrorAction SilentlyContinue) { & py -3 @probeArgs }
    else { & python @probeArgs }
}
exit $LASTEXITCODE
