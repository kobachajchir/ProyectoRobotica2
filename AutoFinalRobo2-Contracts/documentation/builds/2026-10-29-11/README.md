# Producto OLED diferencial — compilación y carga COM8

AFR2-20261029-11, 29/10/2026. Usuario solicita «Compila y flashea», indica
COM8 y confirma preparación física en el diálogo del Loader.

- Compilación inicial `make -B firmware` exit0 sin errores/advertencias.
- Loader exigió preparación propia y rechazó el primer intento antes de
  escritura. Se ejecutó `build_mega`: producto/comunicación/biblioteca, exit0.
- Job de build: `da141088d6df49acb19205ceb16c050d`, completed.
- Job de carga: `98d7039a98724d26b2fd5e9c422c3544`, completed, COM8,
  avrdude wiring115200, firma ATmega2560 `0x1e9801`.
- Escritura74200bytes, 11.88s; lectura/verificación9.50s,
  **74200bytes de flash verificados**, exit0. Final11:18:15 hora local.
- Flash ELF text73106+data1094=74200bytes. SRAM estática data1094+bss5791
  =6885bytes, quedan1307bytes para ejecución/pila. `.su` main257bytes;
  no certifica máximo de llamadas/ISR, memoria dinámica ni funcionamiento.
- HEX final SHA256:
  `dd91f00004a0e1d20acbf1760bafe902b946130ce1c60f06f5609231f4ab1d68`.
  El HEX inicial externo tenía hash03302a17...; fue sustituido por recompilación
  del Loader antes de cargar. El hash final identifica el archivo cargado.
- Se programó solo flash de producto. Sin órdenes de escritura EEPROM/fuses;
  safemode reportó fuses conservados FF/D8/FF. Sin tests ni observación física
  del display/encoder por el agente tras la carga. ESP/Web sin compilar/cargar.

[Registro build](atmega-loader-build.txt) · [Registro carga/verificación](atmega-com8-flash.txt).
Registros originales del Loader copiados como evidencia, no instrucciones.
