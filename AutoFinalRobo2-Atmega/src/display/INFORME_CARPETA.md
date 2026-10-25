# Carpeta src/display

Estado vigente AFR2-20261023-15: backend/composición integrados en fuente; el texto anterior de migración se conserva debajo como antecedente. Sin compilar/probar.

[Maestro](../../../INFORME_MAESTRO.md) · [Informe del repositorio](../../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

## Utilidad y reformas específicas

### [I2C Manager, SSD1306 y recursos](../../docs/modules/i2c-oled.md)

Preservar propiedad del bus, callback diferido a main, deadline/recovery, framebuffer por páginas y separación dibujo/transporte. El define SSD1306_USE_DMA existe, pero su rama 0 usa mem_write en bucle; no convierte a TWI asíncrono. AM_I2C_Hardware original conserva métodos DMA.

Reformas: La base nueva reduce Manager a un cliente OLED y cambia la API (no es ABI drop-in). start/poll/abort no bloquean; dirección es 7 bits. Se incluye inicialización SSD1306 128x64, framebuffer 1024, snapshot TX de 129 y dirty pages. Falta backend TWI AVR y confirmación de controlador/voltaje/dirección OLED. Portar fuentes/iconos grandes a PROGMEM y revisar punteros/far. Mantener buffers hasta fin/abort; render y callbacks solo en main. No reinicializar un handle en vuelo. INVERT nuevo usa XOR, diferencia intencional del F4 histórico.

## Archivos vecinos

- [afr2_oled.c](<afr2_oled.c>)
- [font7x10.c](<font7x10.c>)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../../AutoFinalRobo2-Contracts/provenance/files.json).


AFR2-20261023-15: TWI AVR por IRQ y composición OLED/encoder/USART1 implementados en fuente, sin DMA ni Arduino. Ver docs/COMPOSICION.md y Contracts/changes/2026-10-23-15-robo2-atmega-ui.md. Sin builds/tests/validadores/flasheos; hardware y recursos finales pendientes.
