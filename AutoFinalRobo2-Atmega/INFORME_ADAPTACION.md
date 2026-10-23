# Adaptación atmega

[Informe maestro](../INFORME_MAESTRO.md) · [Contracts](../AutoFinalRobo2-Contracts/README.md).

Se copió todo Core propio F4 como referencia autocontenida de librerías y composición; HAL/CMSIS/USB middleware y build STM32 no se migran. MPU, balance, AT y botón menú permanecen solo para localizar acoplamientos históricos. La base src/ contiene una adaptación parcial nueva, no el firmware del vehículo.

Código nuevo: services/afr2_i2c_manager.c, display/afr2_oled.c y font7x10.c, input/afr2_encoder_ui.c y ui/afr2_screens.c. API pública en include/. El Manager está reducido a un OLED, con start/poll/abort y callbacks en main. Wrappers usan snapshots. Fuente 7x10 con PROGMEM. Encoder largo/corto excluyentes y Volver por ítem. Falta backend TWI, main/reloj/GPIO/UART, sensores/motores/odometría/GOTO.

Makefile tiene ayuda por defecto y objetivo library para producir biblioteca con avr-gcc cuando se autorice. No hay .hex ni upload; no se ejecutó make. [Composición](docs/COMPOSICION.md).

## Guías junto al código

| Subsistema | Para qué sirve |
| --- | --- |
| [Comandos, parser y transporte](docs/modules/protocol.md) | Reutilizar parser de bytes, framing, cola, tablas de handlers y separación de transporte. UNER_CoreConfig permite aportar slots/pool; no hace falta HAL para el núcleo. uner_app.c es composición del robot anterior, no biblioteca neutra. |
| [I2C Manager, SSD1306 y recursos](docs/modules/i2c-oled.md) | Preservar propiedad del bus, callback diferido a main, deadline/recovery, framebuffer por páginas y separación dibujo/transporte. El define SSD1306_USE_DMA existe, pero su rama 0 usa mem_write en bucle; no convierte a TWI asíncrono. AM_I2C_Hardware original conserva métodos DMA. |
| [Wrappers, menús y pantallas](docs/modules/screens.md) | Wrappers preparan datos/contexto; renderer dibuja; navegación decide pantalla/selección; acciones llaman servicios. Reutilizar conceptos y recursos, no System/globales completos. La referencia incluye menús/OSK/avisos y toda la biblioteca propia para port posterior. |
| [Encoder de interfaz y pulsador único](docs/modules/encoder-ui.md) | encoder.c F4 procesa un mando rotativo y su pulsador. Con la ampliación solicitada vuelve a ser una referencia útil para UI; no mide las cuatro ruedas. user_button queda como antecedente del botón eliminado. |
| [Cinco sensores de línea](docs/modules/line-ir.md) | Reutilizar normalización, estado/calibración, filtrado y separación sensor/control. El origen modela ocho TCRT5000 (línea/objetos), adquisición ADC DMA y emisores por grupo. El módulo OSOYOO de cinco puntos no prueba esas mismas capacidades. |
| [Motores, PID, Mecanum y GOTO](docs/modules/motion.md) | Reutilizar arbitraje de fuentes, estados de misión/cancelación, límites, PID y estructura de perfiles. El origen es diferencial/balance; AutoMotion ready exige MPU/calibración/balance y objetos IR. El estimador no sustituye odometría de cuatro Hall. |
| [Wi-Fi, preferencias y composición AVR](docs/modules/wifi-composition.md) | Bridge UNER y estados de configuración muestran pedidos/respuestas y separación de dueño Wi-Fi. main/app_tasks/globals son mapa de integración, no código que se retargetea cambiando includes. |
| [Nuevo driver HC-SR04](docs/modules/hcsr04.md) | No se localizó un driver reusable directo en la fuente F4. El pinout recibido asigna TRIG D30 y ECHO D31. |

Cada carpeta contiene INFORME_CARPETA.md con enlaces. reference/ no participa en la configuración activa. Los archivos originales se identifican por hash en Contracts/provenance/files.json. No se modifica ni publica en AutoMicro.
