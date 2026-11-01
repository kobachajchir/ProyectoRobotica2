# AFR2-20261029-01. Ejemplo de contenido sin Web, Node ni npm.
# Guardar entradas en FsSourceDir/payload y usar FsSourceDir como carpeta de trabajo.
$ErrorActionPreference = 'Stop'
try {
    $source = Join-Path $env:AFR2_FS_SOURCE_DIR 'payload'
    $target = [IO.Path]::GetFullPath($env:AFR2_FS_DATA_DIR)
    if (-not (Test-Path -LiteralPath $source -PathType Container)) { throw 'Falta la carpeta payload en las fuentes.' }
    if (-not (Get-ChildItem -LiteralPath $source -File -Recurse | Select-Object -First 1)) { throw 'payload está vacío.' }
    # No borrar contenido previo silenciosamente: exige destino vacío para evitar mezclar datasets.
    if (Get-ChildItem -LiteralPath $target -Force | Select-Object -First 1) { throw 'ESP/data contiene archivos. Respaldalos y vaciá esa carpeta antes de preparar este dataset.' }
    Get-ChildItem -LiteralPath $source -Force | ForEach-Object { Copy-Item -LiteralPath $_.FullName -Destination $target -Recurse -Force }
    Write-Output 'Archivos preparados; el Loader generará LittleFS.'
    exit 0
} catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
