[CmdletBinding()]
param()
$ErrorActionPreference = 'Stop'
try {
    [Console]::InputEncoding = New-Object System.Text.UTF8Encoding($false)
    [Console]::OutputEncoding = New-Object System.Text.UTF8Encoding($false)
    $OutputEncoding = [Console]::OutputEncoding
    $program = & (Join-Path $PSScriptRoot 'Compilar.ps1') -Mcp
    # No banners ni mezcla de stderr en stdout: stdout pertenece a JSON-RPC.
    & $program --mcp $PSScriptRoot
    exit $LASTEXITCODE
} catch {
    [Console]::Error.WriteLine('AFR2 MCP: ' + $_.Exception.Message)
    exit 1
}
