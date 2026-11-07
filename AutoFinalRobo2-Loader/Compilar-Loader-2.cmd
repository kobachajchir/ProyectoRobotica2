@echo off
setlocal
rem AFR2 Loader 2.0.0: compila para este usuario en %LOCALAPPDATA%\AutoFinalRobo2Loader\bin. No flashea ni abre puertos.
rem ZIP portable para otra PC: Preparar-Distribucion.ps1 (ver PORTABILIDAD.md).
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Compilar.ps1" %*
if errorlevel 1 pause
