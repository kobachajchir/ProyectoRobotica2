# AFR2-20261106-04. Host-only ACK/QR and flash-text regression.
param([string]$Gcc='C:/Strawberry/c/bin/gcc.exe')
$ErrorActionPreference='Stop'
$contractsRoot=(Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$avrRoot=Join-Path $contractsRoot '../AutoFinalRobo2-Atmega'
$outDir=Join-Path $contractsRoot '.tmp/ram-optimization-04'
[void](New-Item -ItemType Directory -Force -Path $outDir)
$sources=@('services/afr2_i2c_manager.c','display/afr2_oled.c','display/font7x10.c','ui/afr2_screens.c','ui/afr2_menu_catalog.c','ui/afr2_icons.c','ui/afr2_sensor_screens.c','ui/afr2_ui_components.c','ui/afr2_dashboard.c','ui/afr2_network_screen.c','ui/afr2_sequence_screen.c','ui/afr2_qr_screen.c','third_party/qrcodegen.c') | ForEach-Object { Join-Path $avrRoot "src/$_" }
& $Gcc -std=c99 -O1 -Wall -ffunction-sections -fdata-sections -I (Join-Path $PSScriptRoot '../oled-parity') -I (Join-Path $avrRoot 'include') -I (Join-Path $contractsRoot 'contracts') (Join-Path $PSScriptRoot 'checks.c') @sources '-Wl,--gc-sections' -o (Join-Path $outDir 'host-checks.exe')
if($LASTEXITCODE){throw 'Host checks compile failed'}
& (Join-Path $outDir 'host-checks.exe')
if($LASTEXITCODE){throw 'Host checks failed'}
