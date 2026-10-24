param([string]$ManifestPath = (Join-Path $PSScriptRoot 'candidatos-2026-10-23.json'))
# Descarga datos CAD; no ejecuta contenido externo ni importa/modela en Fusion.
$ErrorActionPreference = 'Stop'
$cadModule = Split-Path $PSScriptRoot -Parent
$cadManifest = Get-Content -LiteralPath $ManifestPath -Raw | ConvertFrom-Json
foreach ($cadFile in $cadManifest.files) {
    $cadDestination = Join-Path $cadModule $cadFile.path
    if (Test-Path -LiteralPath $cadDestination) {
        Write-Output "Existente, conservado: $($cadFile.path)"
        continue
    }
    New-Item -ItemType Directory -Force -Path (Split-Path $cadDestination -Parent) | Out-Null
    Invoke-WebRequest -Uri $cadFile.download_url -OutFile $cadDestination
    Write-Output "Descargado: $($cadFile.path)"
}
