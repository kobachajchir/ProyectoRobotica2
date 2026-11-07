# Regresión de RAM/QR — AFR2-20261106-04

Con comprobaciones autorizadas: `./tools/verification/avr-ram/run.ps1`. Requiere GCC de host y los mismos fuentes/shim PROGMEM que oled-parity. No abre puertos ni usa hardware.

Comprueba clipping/scroll equivalente RAM/flash, truncado/deduplicación de avisos y copia de texto dinámico. El panel simulado conserva el puntero de transferencia y exige que tx/command no cambien durante el préstamo QR. Repite con comando y datos en vuelo, tanto ACK como fallo, y exige que el panel, framebuffer y sombra confirmada converjan al mismo contenido. Comprueba invalidación por chunks, borrado del scratch privado y préstamos fuera de rango. Las dos matrices OLED deben seguir siendo de 1024 bytes.

No mide temporización, voltajes, SRAM real, profundidad de pila ni comportamiento del panel físico. El compilador nativo con -Wall advierte de posibles truncados en formatos preexistentes (touch, small_number y tiempo de secuencia); son advertencias aparte del build AVR, que queda sin advertencias. Estos fixtures no certifican valores arbitrarios para todos los renderers.
