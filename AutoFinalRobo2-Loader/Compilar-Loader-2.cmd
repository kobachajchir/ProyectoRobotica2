@echo off
setlocal
rem AFR2 Loader 2.0.0: compila para este usuario en %LOCALAPPDATA%\AutoFinalRobo2Loaderin. No flashea ni abre puertos.
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Compilar.ps1" %*
if errorlevel 1 pause
