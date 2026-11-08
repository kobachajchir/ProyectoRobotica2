# AFR2-20261107-43 — Sensores de línea OSOYOO 5 canales: validez sin calibración obligatoria

07/11/2026. Pedido del usuario: los sensores de línea «no andan»; son digitales. Hay que revisar la hoja OSOYOO 5-Channel IR Tracker Sensor.

Estado real: atmega = fuente editada, sin compilar ni cargar; esp = sin cambios; web = sin cambios. Sin pruebas físicas.

## Hoja del fabricante (resumen)

- Alimentación de 3,3 a 5 V. Pines VCC, GND e IR1..IR5.
- Salidas digitales TTL, acondicionadas por un inversor Schmitt 74HC14D.
- Polaridad: **negro = LOW** (se enciende el LED rojo del canal) y **blanco = HIGH**.
- La distancia de detección se ajusta con el potenciómetro de la placa: de 0 a 3 cm.

## Cotejo con el firmware

- Cableado A4..A0 = PF4..PF0, lectura digital cada 5 ms y mayoría de 3 lecturas. Correcto para salidas digitales.
- El firmware libera JTAG (PF4) por software. Los buffers digitales DIDR0 están habilitados y sin pull-ups. Correcto.
- `line_invert = 31` por defecto: un canal en LOW (negro) queda activo. Coincide con la hoja.
- **Causa encontrada:** `line_valid` exigía `line_calibrated`. La confirmación de calibración solo se aceptaba si los **cinco** canales cambiaban entre la muestra de fondo y la de línea (`fondo ^ línea == 31`). Con una línea angosta, que queda bajo uno o dos sensores, eso no ocurre nunca. Por eso la línea nunca era válida y el seguimiento quedaba en «Calibrar línea».

## Cambio

- `afr2_sensors.c`: `line_valid` ya no depende de `line_calibrated`. La polaridad de la hoja ya la aplica `line_invert`.
- `afr2_app.c` (`AFR2_A_LINE_CONFIRM`): alcanza con la muestra de fondo (blanco), que pasa a ser `line_invert`. Si también hay muestra sobre la línea, al menos un canal tiene que cambiar. Un canal trabado en LOW sobre blanco (potenciómetro mal ajustado) queda inactivo en lugar de marcar línea falsa.
- `afr2_product.c` (modo LINE): sentido de corrección invertido según el lado confirmado de IR1 (ver punto 3).

## Pendientes y comprobaciones físicas

1. Con el auto quieto sobre blanco, el LED rojo de cada canal debe estar apagado, y encenderse al pasar cinta negra a 1 cm. Si no, ajustar el potenciómetro.
2. Verificar en la pantalla de sensores o en la Web que el tipo de línea sea digital (`line_kind = 1`). Una EEPROM guardada con 0 deja los sensores apagados, y la migración lo respeta.
3. El usuario confirmó que IR1 (A4) está a la izquierda, mirando el auto desde atrás. El seguimiento corregía al revés: con la línea bajo IR1 daba error −2 y giraba en sentido horario, alejándose de la línea. Ahora el error es `2 − i`: con la línea a la izquierda el error es positivo y el giro es antihorario, hacia la línea. Falta verificarlo en el auto.
4. El seguimiento usa `Afr2Product_Drive` en lazo cerrado y sigue exigiendo motores habilitados y Hall calibrado (CPR).
5. Compilación AVR, RAM y carga quedan para cuando se pidan.
