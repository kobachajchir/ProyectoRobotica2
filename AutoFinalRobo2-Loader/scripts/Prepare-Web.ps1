# AFR2-20261029-01. Adaptación del flujo web AFR2 existente; no genera ni carga firmware.
$ErrorActionPreference = 'Stop'
try {
    $web = [IO.Path]::GetFullPath($env:AFR2_WEB_DIR)
    $data = [IO.Path]::GetFullPath($env:AFR2_FS_DATA_DIR)
    # El promotor actual de nuestra web usa el ESP hermano del proyecto Web.
    $expected = [IO.Path]::GetFullPath((Join-Path $web '../AutoFinalRobo2-ESP/data'))
    if ($data.TrimEnd('\') -ne $expected.TrimEnd('\')) { throw 'El promotor Web AFR2 requiere el ESP hermano de la carpeta Web. Usá otro script para otra distribución.' }
    if (-not (Test-Path -LiteralPath (Join-Path $web 'package.json'))) { throw 'No existe package.json en Proyecto Web.' }
    if (-not (Test-Path -LiteralPath $env:AFR2_NPM)) { throw 'Configurá la ruta de npm para este script Web.' }
    Push-Location -LiteralPath $web
    try {
        & $env:AFR2_NPM run build:esp
        if ($LASTEXITCODE -ne 0) { throw "build:esp terminó con código $LASTEXITCODE" }
    } finally { Pop-Location }
    $manifest = Get-Content -LiteralPath (Join-Path $data 'dist/manifest.json') -Raw | ConvertFrom-Json
    if ($manifest.buildProfile -ne 'esp') { throw 'El manifiesto promovido no pertenece al perfil esp.' }
    Write-Output 'Contenido web AFR2 preparado. El Loader generará LittleFS a continuación.'
    exit 0
} catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
