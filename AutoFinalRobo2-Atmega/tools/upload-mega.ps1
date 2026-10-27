<## Carga solo el firmware de producto compilado al bootloader Mega.
El usuario debe preparar el cableado según Contracts/hardware/CARGA_USB_COM16.md.
No cambia fuses, EEPROM, bootloader ni compila implícitamente.
##>
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)]
    [ValidatePattern('^COM[0-9]+$')]
    [string]$Port
)
$ErrorActionPreference = 'Stop'
$avrTools = Join-Path $env:LOCALAPPDATA 'Arduino15/packages/arduino/tools/avrdude/6.3.0-arduino17'
$avrUploader = Join-Path $avrTools 'bin/avrdude.exe'
$avrConfig = Join-Path $avrTools 'etc/avrdude.conf'
$firmwareRoot = Split-Path -Parent $PSScriptRoot
$firmwareHex = Join-Path $firmwareRoot 'build/afr2.hex'
foreach ($requiredFile in @($avrUploader, $avrConfig, $firmwareHex)) {
    if (-not (Test-Path -LiteralPath $requiredFile -PathType Leaf)) {
        throw "No se encontró el archivo requerido: $requiredFile"
    }
}
Push-Location -LiteralPath $firmwareRoot
try {
    & $avrUploader '-C' $avrConfig '-v' '-p' 'atmega2560' '-c' 'wiring' '-P' $Port '-b' '115200' '-D' '-U' 'flash:w:build/afr2.hex:i'
    $uploadExit = $LASTEXITCODE
} finally {
    Pop-Location
}
exit $uploadExit
