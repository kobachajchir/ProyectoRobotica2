# AFR2-20261029-07 — Diagnóstico OLED activo comparado con Arduino/Adafruit

Autoría: Koba Chajchir / AFR2; adaptación mediante Codex, 29/10/2026.

## Evidencia y pedido

El usuario reporta OLED funcional con el ejemplo Arduino IDE Adafruit SSD1306
128x64, dirección corregida0x3C (byte0x78) y consulta de giro180grados. Solicita
modificar nuestro firmware de prueba de encoder/display con esa referencia.
Esto acredita funcionamiento reportado con ese sketch, no una prueba ejecutada
por el agente ni validación física de nuestra adaptación.

El servicio anterior solo enviaba probes: no podía mostrar imagen por diseño.
Además Wire AVR1.8.6 instalado activa pull-ups internos y nuestro backend los
desactivaba. La causa exacta de los NACK previos no queda aislada por lectura.

## Adaptaciones y procedencia por archivo

Rutas Atmega relativas a AutoFinalRobo2-Atmega. Código de adaptación original
AFR2, comentarios/autorías previos conservados. No se copia el ejemplo Adafruit,
su logo ni su biblioteca. Referencias consultadas: Wire del core Arduino AVR
1.8.6 instalado y [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306/blob/master/Adafruit_SSD1306.cpp).

| Archivo | Adaptación |
| --- | --- |
| src/platform/afr2_twi.c | Macro AFR2_TWI_INTERNAL_PULLUPS, default0 para producto. release/recovery preservan política de pulls configurada sin conducir alto como salida. Restaura reloj PRTWI explícitamente. |
| include/afr2_twi.h | Documenta opción de comparación eléctrica con Wire. |
| src/display/afr2_oled.c | Espera configurable AFR2_OLED_STARTUP_MS (default100ms producto,500ms diagnóstico). Desactiva scroll0x2E antes de display ON; conserva direccionamiento por páginas compatible con envío existente. |
| service/i2c_uart0_diagnostic.c | Integra Afr2Oled0x3C con framebuffer de barras móviles, rotación180, estado ACK de inicialización/error y tareas fuera de barridos. Encoder/USART0 y probes cada10s conservados; no hay envío simultáneo por otro dueño de bus. |
| Makefile | Enlaza driver/font existentes en diagnóstico; compila fuentes separadas con pulls1 y espera500ms solo para este objetivo. DIAG_PULLUPS=0 permite comparación explícita. |
| tools/flash-i2c-encoder-diagnostic.ps1 | Actualiza banner esperado y descripción a prueba OLED activa. Conserva compilación forzada antes de carga. |
| service/README.md, README.md, INFORME_ADAPTACION.md | Estado fuente y alcance de diagnóstico activo. |

Contracts: guía OLED_I2C_DIAGNOSTICO actualizada, esta ficha y entradas
README/CHANGELOG. No cambian ScreenCodes, pantallas de producto, recursos,
menús, navegación o copia Web. Las barras son un estímulo de framebuffer del
servicio de transporte, no una pantalla añadida a la biblioteca de producto.

## Estado real y límites

ATmega: fuente modificada, revisión manual. Sin tests, compilación, validadores,
COM, flasheo ni ejecución física. No se produce HEX nuevo por el agente y no
se afirma que los NACK estén resueltos. La futura prueba debe comprobar ACK
y patrón visible; INIT ACK no demuestra emisión de luz. Se añade un handle
Afr2Oled estático con framebuffer1024bytes y tx129bytes, además de estado:
presupuesto flash/SRAM/pila total pendiente de compilación autorizada.

Producto: macro de pull-ups sigue0 y espera100ms; comparte restauración de
reloj TWI y comando de desactivar scroll tras una futura recompilación. La
política de pull-ups1 está limitada al diagnóstico en su regla de Make.
ESP/Web/Loader/CAD sin cambios. Sin promoción LittleFS ni movimiento.

Los pull-ups a VCC del AVR reproducen una diferencia del sketch reportado;
no certifican tolerancia eléctrica de todos los OLED ni sustituyen adaptador
de niveles. Hardware/cableado no se alteran por crear fuentes.
