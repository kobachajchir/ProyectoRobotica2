# AFR2-20261028-21 — Script PowerShell del diagnóstico OLED/encoder

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 28/10/2026.
Pedido explícito: dejar el comando anterior como archivo ejecutable PowerShell.

## Adaptaciones por archivo

- `AutoFinalRobo2-Atmega/tools/flash-i2c-encoder-diagnostic.ps1`: fuente original
  nueva. Puerto COM obligatorio; rutas Make/AVR/avrdude configurables, por
  defecto las instalaciones identificadas en esta PC. Resuelve módulo desde
  PSScriptRoot, comprueba archivos, recompila objetivo de diagnóstico con -B,
  exige HEX existente/no vacío y después escribe/verifica por bootloader Mega
  wiring a115200. No usa el upload del producto ni modifica su selección HEX.
  Ante error aborta; restaura carpeta/PATH. No cambia políticas persistentes,
  fuses, EEPROM o bootloader; sin borrado explícito ni omisión de verificación.
- `AutoFinalRobo2-Atmega/service/README.md`: enlace y estado fuente del script.
- `AutoFinalRobo2-Contracts/hardware/OLED_I2C_DIAGNOSTICO.md`: comando completo,
  parámetros, requisitos y separación entre script preparado y ejecución.
- Esta ficha registra estado/procedencia; README y CHANGELOG de Contracts
  enlazan el cambio. Autorías de fuentes anteriores conservadas.

## Estado real

ATmega: script creado y revisado por lectura, sin ejecutar, compilar, validar,
tests, abrir COM, flashear o comprobar físicamente. No se genera un nuevo HEX.
El diagnóstico fuente conserva revisión20; este script es revisión21.
ESP/Web/Loader/CAD sin cambios por esta ficha. El script solo escribe si el
usuario lo ejecuta; reemplaza temporalmente el programa del auto y no prueba
por sí solo que el OLED o encoder funcionen. No agrega un flasher al Loader.

[Guía](../hardware/OLED_I2C_DIAGNOSTICO.md#objetivo-preparado-no-ejecutado).
