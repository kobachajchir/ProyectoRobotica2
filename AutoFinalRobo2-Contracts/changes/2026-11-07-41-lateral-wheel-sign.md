# AFR2-20261107-41 - Izquierda y derecha corregidas al conducir

Pedido del usuario: «la izquierda y la derecha en conducir están cruzadas». Adelante/atrás y giros no se informaron mal, así que solo cambia el eje lateral.

## Cambio

Se agrega un signo lateral fijo a la referencia chasis↔ruedas de AFR2-20261030-13 (ver `contracts/MOTION_REFERENCE_13.md`), con el mismo criterio que los signos de avance y giro: se aplica una sola vez en la mezcla del ATmega y nunca en Web/ESP/botones.

- `contracts/afr2_motion_reference.h`: nuevo `AFR2_LATERAL_WHEEL_SIGN (-1)`.
- `contracts/motion_reference.json`: `lateralWheelSign` pasa de 1 a -1 (antes existía pero nadie lo leía).
- ATmega `src/services/afr2_product.c`:
  - `Afr2Product_Drive`: `vy *= AFR2_LATERAL_WHEEL_SIGN` (lazo cerrado, seguimiento de línea, GOTO).
  - `Afr2Product_DrivePwm`: `strafe *= AFR2_LATERAL_WHEEL_SIGN` (control táctil, botones de la Web, pantalla Mecanum de la OLED).
  - Odometría: `dy` multiplicado por el mismo signo, para que la pose Y siga siendo «izquierda positiva».
- Web `features/control/sequences/mecanumKinematics.ts`: cinemática inversa y directa usan `lateralWheelSign`, así la simulación y las secuencias calculan las mismas ruedas que el AVR.

Semántica sin cambios: Y lógico positivo = izquierda en `robot.drive`, `robot.drive.pwm`, GOTO y secuencias. Las diagonales conservan su componente de avance y corrigen la lateral. Los programas editados a nivel ruedas conservan sus valores literales.

## Estado

atmega=fuente editada, sin compilar ni cargar; web=fuente editada, sin compilar; esp=sin cambios. Sin prueba física: falta confirmar con el auto que lateral izquierda/derecha y las cuatro diagonales van hacia donde se pide, y que la pose Y por Hall acompaña. La paridad C/TS de cinemática no se regeneró.
