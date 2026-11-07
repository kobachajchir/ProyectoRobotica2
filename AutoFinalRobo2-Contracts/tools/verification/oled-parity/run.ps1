# AFR2-20261106-02. Execute only when checks are explicitly authorized.
param([string]$Gcc = 'C:/Strawberry/c/bin/gcc.exe')
$ErrorActionPreference = 'Stop'
$contractsRoot = (Resolve-Path (Join-Path $PSScriptRoot '../../..')).Path
$avrRoot = Join-Path $contractsRoot '../AutoFinalRobo2-Atmega'
$outDir = Join-Path $contractsRoot '.tmp/oled-parity'
[void](New-Item -ItemType Directory -Force -Path $outDir)
$sourceFiles = @('services/afr2_i2c_manager.c','display/afr2_oled.c','display/font7x10.c','ui/afr2_screens.c','ui/afr2_menu_catalog.c','ui/afr2_icons.c','ui/afr2_sensor_screens.c','ui/afr2_ui_components.c','ui/afr2_dashboard.c','ui/afr2_network_screen.c','ui/afr2_sequence_screen.c','ui/afr2_qr_screen.c','third_party/qrcodegen.c') | ForEach-Object { Join-Path $avrRoot "src/$_" }
& $Gcc -std=c99 -O1 -ffunction-sections -fdata-sections -I $PSScriptRoot -I (Join-Path $avrRoot 'include') -I (Join-Path $contractsRoot 'contracts') (Join-Path $PSScriptRoot 'frames.c') @sourceFiles '-Wl,--gc-sections' -o (Join-Path $outDir 'frames.exe')
if ($LASTEXITCODE) { throw 'Host C compile failed' }
& (Join-Path $outDir 'frames.exe') (Join-Path $outDir 'avr.bin')
if ($LASTEXITCODE) { throw 'C fixtures failed' }
node (Join-Path $PSScriptRoot 'bundle.mjs') (Join-Path $outDir 'check.mjs')
if ($LASTEXITCODE) { throw 'TS bundle failed' }
node (Join-Path $outDir 'check.mjs') (Join-Path $outDir 'avr.bin') (Join-Path $outDir 'result.json')
if ($LASTEXITCODE) { throw 'Framebuffer parity failed' }
