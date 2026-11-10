# Actualiza indices/campos del DOCX con Word y exporta el PDF junto al DOCX.
# Uso: powershell -File exportar_pdf.ps1 -Docx <ruta\AFR2_Informe_D2.docx>
param([Parameter(Mandatory = $true)][string]$Docx)
$ErrorActionPreference = 'Stop'
$path = (Resolve-Path $Docx).Path
$pdf = [System.IO.Path]::ChangeExtension($path, '.pdf')
$word = New-Object -ComObject Word.Application
$word.Visible = $false
$word.DisplayAlerts = 0
try {
    $doc = $word.Documents.Open($path, $false, $false)
    # Imagenes centradas y ecuaciones de bloque centradas
    foreach ($s in $doc.InlineShapes) { $s.Range.ParagraphFormat.Alignment = 1 }
    for ($k = 0; $k -lt 2; $k++) { foreach ($t in $doc.TablesOfContents) { $t.Update() }; $doc.Fields.Update() | Out-Null }
    $doc.Save()
    # 17 = wdExportFormatPDF; CreateBookmarks 1 = encabezados
    $doc.ExportAsFixedFormat($pdf, 17, $false, 0, 0, 1, 1, 0, $true, $true, 1, $true, $true, $false)
    $doc.Close($false)
    Write-Output $pdf
} finally {
    $word.Quit()
    [System.Runtime.InteropServices.Marshal]::ReleaseComObject($word) | Out-Null
}
