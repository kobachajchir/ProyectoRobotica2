# Carpeta src

[Maestro](../../INFORME_MAESTRO.md) · [Informe del repositorio](../INFORME_ADAPTACION.md).

Base nueva de trabajo; ver estado de implementación en el informe del repositorio.

## Utilidad y reformas específicas

### [Encoder de interfaz y pulsador único](../docs/modules/encoder-ui.md)

encoder.c F4 procesa un mando rotativo y su pulsador. Con la ampliación solicitada vuelve a ser una referencia útil para UI; no mide las cuatro ruedas. user_button queda como antecedente del botón eliminado.

Reformas: CLK D40/PG1 y DT D41/PG0 se leen juntos; SW D42/PL7 activo bajo. Inicializar desde niveles reales, evitar gesto de arranque pulsado, rechazar transición cuadratura inválida y ajustar signo/transiciones por detent. Base: 1 ms poll, 20 ms debounce, 800 ms largo exclusivo; ajustar sin asumir hardware probado. El corto sale al soltar solo si no se emitió largo. No usar polling de UI para Hall motriz.

### [I2C Manager, SSD1306 y recursos](../docs/modules/i2c-oled.md)

Preservar propiedad del bus, callback diferido a main, deadline/recovery, framebuffer por páginas y separación dibujo/transporte. El define SSD1306_USE_DMA existe, pero su rama 0 usa mem_write en bucle; no convierte a TWI asíncrono. AM_I2C_Hardware original conserva métodos DMA.

Reformas: La base nueva reduce Manager a un cliente OLED y cambia la API (no es ABI drop-in). start/poll/abort no bloquean; dirección es 7 bits. Se incluye inicialización SSD1306 128x64, framebuffer 1024, snapshot TX de 129 y dirty pages. Falta backend TWI AVR y confirmación de controlador/voltaje/dirección OLED. Portar fuentes/iconos grandes a PROGMEM y revisar punteros/far. Mantener buffers hasta fin/abort; render y callbacks solo en main. No reinicializar un handle en vuelo. INVERT nuevo usa XOR, diferencia intencional del F4 histórico.

### [Wrappers, menús y pantallas](../docs/modules/screens.md)

Wrappers preparan datos/contexto; renderer dibuja; navegación decide pantalla/selección; acciones llaman servicios. Reutilizar conceptos y recursos, no System/globales completos. La referencia incluye menús/OSK/avisos y toda la biblioteca propia para port posterior.

Reformas: Nueva UI usa snapshot explícito sin consultas de hardware en renderer. Mantener identidad/rol/estado/acciones por separado. Largo SW abre menú; corto selecciona; atrás solo por ítem Volver. Retirar llamadas al botón menú y no reinterpretar largo como atrás. Main procesa acciones y respeta valid/unknown. Llevar menús/textos a flash; decidir contexto de edición/confirmación sin ejecutar motor al navegar. Skill en Contracts documenta cada pantalla; paridad Web pendiente.

## Subcarpetas

- [display](display/INFORME_CARPETA.md)
- [input](input/INFORME_CARPETA.md)
- [platform](platform/INFORME_CARPETA.md)
- [services](services/INFORME_CARPETA.md)
- [ui](ui/INFORME_CARPETA.md)

Estado: material preparado sin build, tests, validadores ni prueba física. Los hashes de fuentes copiadas están en [manifest](../../AutoFinalRobo2-Contracts/provenance/files.json).
