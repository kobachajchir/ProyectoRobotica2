<#
AFR2-20261028-21. Koba Chajchir / AFR2; adaptation via Codex.
Build and optionally flash the standalone I2C + encoder + servo diagnostic.
AFR2-20261029-17 adds -BuildOnly: no port opened and no avrdude invocation.
Running this script replaces the vehicle application. No execution by authoring.
Prerequisites for upload: wheel motor power disconnected, Mega RESET free, ESP isolated from
D0/D1, serial monitors closed. Does not write EEPROM, fuses or bootloader.
Servo5V/GND/S1 and separate S1-to-D13 signal jumper for the intended test.
Servo starts PWM OFF; first encoder turn or C command enables servo pulses.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)]
    [ValidatePattern('^COM[0-9]+$')]
    [string]$Port,

    [switch]$BuildOnly,

    [string]$MakePath = 'C:\Program Files (x86)\Atmel\Studio\7.0\shellUtils\make.exe',
    [string]$AvrBin = "$env:LOCALAPPDATA\Arduino15\packages\arduino\tools\avr-gcc\7.3.0-atmel3.6.1-arduino7\bin",
    [string]$AvrdudeRoot = "$env:LOCALAPPDATA\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino17"
)
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$firmwareRoot = Split-Path -Parent $PSScriptRoot
$diagnosticHex = Join-Path $firmwareRoot 'build/afr2-i2c-diagnostic.hex'
$avrUploader = Join-Path $AvrdudeRoot 'bin/avrdude.exe'
$avrConfig = Join-Path $AvrdudeRoot 'etc/avrdude.conf'
$avrCompiler = Join-Path $AvrBin 'avr-gcc.exe'
$avrObjcopy = Join-Path $AvrBin 'avr-objcopy.exe'

$requiredFiles = @($MakePath, $avrCompiler, $avrObjcopy,
        (Join-Path $firmwareRoot 'Makefile'),
        (Join-Path $firmwareRoot 'service/i2c_uart0_diagnostic.c'))
if (-not $BuildOnly) { $requiredFiles += @($avrUploader, $avrConfig) }
foreach ($requiredFile in $requiredFiles) {
    if (-not (Test-Path -LiteralPath $requiredFile -PathType Leaf)) {
        throw "Required file not found: $requiredFile. Override tool paths using script parameters."
    }
}

Write-Host "Diagnostic target: ATmega2560 on $Port (bootloader wiring, 115200)."
Write-Host 'Image: I2C + encoder + servo. Upload replaces the vehicle application.'
if ($BuildOnly) { Write-Host 'BuildOnly: compilation only; no serial port will be opened.' }
else { Write-Host 'Wheel motor power OFF; RESET free; ESP isolated from D0/D1; serial monitor closed.' }

$previousPath = $env:PATH
Push-Location -LiteralPath $firmwareRoot
try {
    # Keep native tool paths out of shell command strings; also finds GCC helpers.
    $env:PATH = "$AvrBin;$previousPath"
    Write-Host 'Building diagnostic from current sources...'
    & $MakePath '-B' 'i2c-diagnostic-firmware' 'CC=avr-gcc.exe' 'OBJCOPY=avr-objcopy.exe'
    if ($LASTEXITCODE -ne 0) {
        throw "Diagnostic build failed (exit $LASTEXITCODE). Flash was not started."
    }
    if (-not (Test-Path -LiteralPath $diagnosticHex -PathType Leaf)) {
        throw 'Build did not produce the diagnostic HEX. Flash was not started.'
    }
    if ((Get-Item -LiteralPath $diagnosticHex).Length -eq 0) {
        throw 'Diagnostic HEX is empty. Flash was not started.'
    }
    if ($BuildOnly) {
        Write-Host "Diagnostic compiled: $diagnosticHex"
        Write-Host 'No upload requested. No serial port opened.'
        return
    }
    Write-Host "Writing and verifying diagnostic: $diagnosticHex"
    # Default avrdude verification retained: no -V, no erase/fuse/EEPROM command.
    & $avrUploader '-C' $avrConfig '-v' '-p' 'atmega2560' '-c' 'wiring' `
        '-P' $Port '-b' '115200' '-D' '-U' 'flash:w:build/afr2-i2c-diagnostic.hex:i'
    if ($LASTEXITCODE -ne 0) {
        throw "Flash or verification failed (exit $LASTEXITCODE). Diagnostic operation is not confirmed."
    }
    Write-Host 'Diagnostic write/verification completed by avrdude.'
    Write-Host "Open $Port at 115200 8N1. Expected banner: AFR2 SERVO+IR+ENC DIAG AFR2-20261029-18."
    Write-Host 'I2C: D20 SDA / D21 SCL. Encoder: D2 CLK / D3 DT / D42 SW.'
    Write-Host 'Servo: GND/5V/S1; separate jumper S1->D13. First encoder turn enables PWM;5deg per detent.'
    Write-Host 'C=center90deg; S=PWM OFF; R=rescan I2C. OLED shows angle/PWM/counter/button, rotation180.'
    Write-Host 'IR1..IR5=A4..A0 digital RAW,0=black. Read independently of product EEPROM/calibration.'
    Write-Host 'Confirm the image visually; serial INIT ACK alone is not proof. Restore vehicle firmware afterwards.'
} finally {
    $env:PATH = $previousPath
    Pop-Location
}
