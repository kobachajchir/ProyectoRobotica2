# Compilación — sensores y radar EX01v8

25/10/2026, AFR2-20261025-14. Pedido humano posterior: ejecutar compilación y corregir errores. **ATmega, ESP y Web full terminan con exit0, sin errores ni advertencias finales.**

## Correcciones

- Makefile AVR: la creación de `build` con `cmd /c` no creó el directorio en este entorno y el enlazador no pudo escribir `build/afr2.map`. Se reemplazó por creación nativa mediante PowerShell, conservando mkdir para sistemas no Windows. La compilación completa posterior creó ELF/HEX correctamente.
- Se corrigieron seis advertencias de indentación engañosa en navegación y comandos AVR, y una en el adaptador ESP de parámetros de radar. Cambios de formato/control explícito, sin modificar cuerpos UART ni dibujos.
- Actualizada la referencia de fuente del renderer C tras esos cambios de formato y regenerada la biblioteca OLED durante el build Web. C y TypeScript conservan los mismos algoritmos.
- Se excluye `.pio-cache/` de Git como caché recuperable de compilación.

El primer intento restringido de AVR no pudo iniciar el ejecutable por Illegal System DLL Relocation; PlatformIO no pudo crear platforms.lock. Se repitieron las compilaciones con los permisos adecuados y se completaron. Esos bloqueos de entorno no fueron errores de código.

## Resultados

| Resultado | Valor |
|---|---|
| Firmware / Web / EX01 | 0.10.0 / 0.3.0 / 8 |
| Comandos / identidades OLED | 41 / 64 |
| ATmega producto/comunicación/biblioteca | Compilación completa con `make -B`, exit0 |
| ATmega .text/.data/.bss/.noinit | 71696 / 1094 / 4809 / 9 bytes |
| ATmega flash / SRAM estática | 72790 / 5912 bytes |
| SRAM no asignada estáticamente | 2280 de 8192 bytes, sin certificar pila/ISR |
| ESP RAM / flash | 47008 / 433787 bytes |
| Web full / activos gzip | 7 / 1515587 bytes |
| Web raw / estimación FS | 3138024 / 1605632 bytes |

El empaquetador full completó la ruta de publicación por archivos disponible en Windows (manifest al final). No se ejecutó build:esp/copy:esp; ESP/data/dist conserva el paquete previo.

## Evidencia

- [ATmega](atmega-build.txt), [secciones](atmega-size.txt).
- [ESP](esp-build.txt).
- [Web y empaquetado full](web-build.txt).
- [Manifest local](manifest.json), [resumen y hashes de artefactos](summary.json).

Comandos: GNU make de Atmel Studio con avr-gcc de Arduino7.3.0 y targets `-B firmware communication-firmware library`; PlatformIO `run -e esp12s_candidate`; `npm run build` (sincronizador OLED, TypeScript, Vite y postbuild de empaquetado full).

Solo compilación y lectura de artefactos. No se ejecutaron tests, validadores independientes, navegador, comparación de raster, imagen FS, serie, flasheos ni pruebas físicas. La SRAM estática no acredita pila ni tiempos; gates de memoria/Hall conservados. No hubo promoción de activos al ESP, commit ni publicación Git.

## Recompilación posterior solicitada

[Rebuild02](rebuild-02/README.md): reconstrucción ATmega, compilación ESP y Web perfil esp, todas exit0 y sin errores/advertencias. No hubo nuevas correcciones de fuente. Paquete esp local generado, sin copy:esp, tests ni flasheos. Los resultados full de esta página conservan su evidencia histórica.
