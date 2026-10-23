# I2C Manager, SSD1306 y recursos

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

## Utilidad

Preservar propiedad del bus, callback diferido a main, deadline/recovery, framebuffer por páginas y separación dibujo/transporte. El define SSD1306_USE_DMA existe, pero su rama 0 usa mem_write en bucle; no convierte a TWI asíncrono. AM_I2C_Hardware original conserva métodos DMA.

## Adaptación

La base nueva reduce Manager a un cliente OLED y cambia la API (no es ABI drop-in). start/poll/abort no bloquean; dirección es 7 bits. Se incluye inicialización SSD1306 128x64, framebuffer 1024, snapshot TX de 129 y dirty pages. Falta backend TWI AVR y confirmación de controlador/voltaje/dirección OLED. Portar fuentes/iconos grandes a PROGMEM y revisar punteros/far. Mantener buffers hasta fin/abort; render y callbacks solo en main. No reinicializar un handle en vuelo. INVERT nuevo usa XOR, diferencia intencional del F4 histórico.

## Evidencia y alcance

- [reference/f4/Core/Src/i2c_manager.c](../../reference/f4/Core/Src/i2c_manager.c)
- [reference/f4/Core/Inc/i2c_port.h](../../reference/f4/Core/Inc/i2c_port.h)
- [reference/f4/Core/Src/ssd1306.c](../../reference/f4/Core/Src/ssd1306.c)
- [reference/f4/Core/Src/fonts.c](../../reference/f4/Core/Src/fonts.c)
- [src/services/afr2_i2c_manager.c](../../src/services/afr2_i2c_manager.c)
- [src/display/afr2_oled.c](../../src/display/afr2_oled.c)

Hallazgos: AFRDOC-001, AFRDOC-005, AFRDOC-009. [Registro](../../../AutoFinalRobo2-Contracts/documentation/review/FINDINGS.md).

Estado: lectura/copia o fuente parcial indicada; sin compilación, tests ni prueba física. La API detallada de origen está en el corpus AMDOC importado; no equivale a API AVR adaptada.
