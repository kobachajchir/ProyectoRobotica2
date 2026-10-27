[CmdletBinding()]
param([string]$Codex = 'codex')
$ErrorActionPreference = 'Stop'
try {
    $client = Get-Command $Codex -ErrorAction Stop
    $entry = Join-Path $PSScriptRoot 'Mcp.ps1'
    & $client.Source mcp add afr2-loader -- powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -File $entry
    if ($LASTEXITCODE -ne 0) { throw 'Codex no pudo registrar afr2-loader. Revisá la salida anterior.' }
    Write-Host 'Servidor registrado. Abrí Iniciar.cmd y reiniciá o reconectá el cliente MCP en Codex.'
    Write-Host 'Registrar no comprueba conexión con la ventana ni con las placas.'
} catch {
    [Console]::Error.WriteLine($_.Exception.Message)
    [Console]::Error.WriteLine('Si codex no está en PATH, indicá su ejecutable con -Codex "C:\ruta\codex.exe" o usá la configuración manual de MCP.md.')
    exit 1
}
