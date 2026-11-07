# AFR2-20261107-44 — Informe de pinout actualizado (batería y AutoKEY/AutoRESET)

07/11/2026. Pedido del usuario: armar el informe de pinout con los agregados del sensor de batería y de AutoKEY/AutoRESET.

Estado real: documentación y texto de Ayuda de la Web. atmega/esp = sin cambios; web = `helpContent.ts` editado, sin compilar. Sin cargas ni pruebas físicas.

## Entregable

- `hardware/INFORME_PINOUT_ROBOT_20261107.html`: página publicada como artifact privado, https://claude.ai/artifact/9gyJCYHNiBfY9xF1irN8XL.
- Actualiza `hardware/INFORME_PINOUT_ROBOT_20261106.md` (AFR2-20261106-15), que se conserva sin cambios.

## Contenido nuevo o corregido respecto del 06/11

- Mapa de las cabeceras del Mega por origen: base, agregado propio y servicio.
- Sensor de batería en A5/PF5: esquema del divisor 10 k/10 k con 100 nF a masa y tabla de lecturas esperadas. 6,0/7,4/8,0/8,4 V corresponden a raw 614/758/819/860; a unos 10 V el ADC satura en 1023. Fórmula del firmware: `mV = (raw × 10000 + 512) / 1024`.
- AutoKEY D32/PC5 y AutoRESET D33/PC4: esquema de la etapa NPN (1 kΩ en serie, 10 kΩ de base a masa del lado del transistor, colector en paralelo con el pulsador) y reglas de reposo. Cotejado con `board_pinmap.h` y `service/esp_uart_bridge.c` (`KEY_MASK = PC5`, `RST_MASK = PC4`).
- Barra IR: salidas digitales con LOW sobre negro, según la hoja OSOYOO. IR1 (A4) a la izquierda, confirmado por el usuario. Referencia a AFR2-20261107-43.
- Nota de tracción: el sentido del chasis se corrige con los signos de la mezcla (AFR2-20261107-41), no en el cableado.

## Ayuda de la Web (`AutoFinalRobo2-Web/src/features/docs/helpContent.ts`)

Pedido posterior del usuario: actualizar el pinout de la documentación de la Web.

- Tarjeta ATmega: nuevo grupo «Servicio ESP (agregado)» con AutoKEY `D32 · PC5` y AutoRESET `D33 · PC4`. La línea indica IR1 a la izquierda y la batería indica el divisor.
- Capítulo de sensores: los IR ya no se describen como ADC sino como entradas digitales (LOW sobre negro, mayoría de 3, IR1 a la izquierda, calibración opcional). El ADC queda solo para la batería, con una lectura de ejemplo.
- Capítulo del driver OLED: autorreparación con la configuración cada 1 s y un tramo cada 250 ms (AFR2-20261107-46).
- Tarjeta ESP: KEY y RST con pulsador o AutoKEY D32 / AutoRESET D33, cable naranja = RESET. Se reemplazó el texto del cruce `D0`/`D1`, un método retirado, por el puente por software `USART0`↔`USART1`.

## Pendiente

- Cables de control: el usuario indica que el **naranja es RESET** (D33/PC4). El color del cable de KEY (D32) sigue sin anotar.
- Montaje y medición del divisor de batería y de la etapa NPN.
