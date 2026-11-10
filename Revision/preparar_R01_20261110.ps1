$ErrorActionPreference = 'Stop'
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$ReviewRoot = Join-Path $PSScriptRoot 'R01_20261110'
if (Test-Path -LiteralPath $ReviewRoot) {
    throw 'R01 ya existe. No sobrescribir una entrega de revision con este script.'
}
New-Item -ItemType Directory -Path $ReviewRoot | Out-Null
$Origins = [System.Collections.Generic.List[object]]::new()

function Copy-ReviewFile {
    param([string]$SourceRelative, [string]$TargetRelative)
    $SourcePath = Join-Path $ProjectRoot $SourceRelative
    $TargetPath = Join-Path $ReviewRoot $TargetRelative
    $TargetDirectory = Split-Path -Parent $TargetPath
    New-Item -ItemType Directory -Path $TargetDirectory -Force | Out-Null
    Copy-Item -LiteralPath $SourcePath -Destination $TargetPath
    $Origins.Add([ordered]@{
        archivo_revision = $TargetRelative.Replace('\','/')
        origen_proyecto = $SourceRelative.Replace('\','/')
        operacion = 'copia documental; contenido sin editar'
    })
}

$ReportRelative = 'AutoFinalRobo2-Contracts/docs/informe_20261109'
$ReportSource = Join-Path $ProjectRoot $ReportRelative
foreach ($File in Get-ChildItem -LiteralPath $ReportSource -File -Recurse) {
    if ($File.Extension -eq '.py' -or $File.Name -eq 'README.md') { continue }
    $Tail = [System.IO.Path]::GetRelativePath($ReportSource, $File.FullName)
    Copy-ReviewFile "$ReportRelative/$Tail" "01_Informe/$Tail"
}

$PlansRelative = 'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/laminas_20261109'
$PlansSource = Join-Path $ProjectRoot $PlansRelative
foreach ($File in Get-ChildItem -LiteralPath $PlansSource -File) {
    if ($File.Extension -notin '.pdf','.csv','.json','.md' -or $File.Name -eq 'README.md') { continue }
    Copy-ReviewFile "$PlansRelative/$($File.Name)" "02_Planos/$($File.Name)"
}
foreach ($File in Get-ChildItem -LiteralPath (Join-Path $PlansSource 'tomos') -File) {
    Copy-ReviewFile "$PlansRelative/tomos/$($File.Name)" "02_Planos/tomos/$($File.Name)"
}

$InitialPlans = 'AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v17/laminas_20261102'
foreach ($Name in 'AFR2_Laminas_Iniciales_A3.pdf','AFR2_Fichas_Brazo_y_Robot_A3.pdf') {
    Copy-ReviewFile "$InitialPlans/$Name" "02_Planos/Antecedentes_20261102/$Name"
}
Copy-ReviewFile "$InitialPlans/AFR2_Fuente_Laminas_20261102.f3d" '02_Planos/Fuente_CAD/AFR2_Fuente_Laminas_20261102.f3d'

$Hardware = 'AutoFinalRobo2-Contracts/hardware'
Copy-ReviewFile "$Hardware/INFORME_PINOUT_ROBOT_20261107.html" '03_Pinout/INFORME_PINOUT_ROBOT_20261107.html'
Copy-ReviewFile "$Hardware/output/pdf/INFORME_PINOUT_ROBOT_20261106.pdf" '03_Pinout/Antecedentes/INFORME_PINOUT_ROBOT_20261106.pdf'
Copy-ReviewFile "$Hardware/output/pdf/PINOUT_DEL_AUTO.pdf" '03_Pinout/Antecedentes/PINOUT_DEL_AUTO.pdf'
foreach ($Name in 'INFORME_PINOUT_ROBOT_20261106.md','PINOUT.md','PINOUT_DEL_AUTO.json','PINOUT_PROPUESTO.csv') {
    Copy-ReviewFile "$Hardware/$Name" "03_Pinout/Referencias/$Name"
}

$Manifest = [ordered]@{
    revision = 'R01'
    fecha_entrega = '2026-11-10'
    estado = 'en revision; no aprobado como informe final'
    informe_incluido = 'Borrador D1 del 2026-11-09'
    planos_incluidos = 'Documentacion dimensional nominal v17, revision D del 2026-11-09'
    pinout_principal = 'HTML 2026-11-07; PDF 2026-11-06 como antecedente'
    originales = 'conservados en sus modulos; rutas relativas a la raiz del proyecto'
    fuentes_regeneracion = [ordered]@{
        informe = "$ReportRelative/generar_informe.py"
        planos = "$PlansRelative/componer_documentacion.py"
        pinout = "$Hardware/INFORME_PINOUT_ROBOT_20261107.html"
    }
    archivos = $Origins
}
$Manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $ReviewRoot 'ORIGENES.json') -Encoding utf8
Write-Output "R01 creada en $ReviewRoot; $($Origins.Count) archivos copiados. Originales conservados."
