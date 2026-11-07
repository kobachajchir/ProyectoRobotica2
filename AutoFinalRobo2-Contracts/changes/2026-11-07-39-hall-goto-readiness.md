# AFR2-20261107-39 - Hall listo para el go-to: concordancia PWM/Hall y velocidad mínima

## Auditoría de la cadena Hall -> odometría -> go-to (fuente ATmega)
- Conteo: ISR PCINT2 en A8..A15, flanco ascendente de A con B como sentido (x1), pull-ups internos. `hall_signs` invierte por rueda en `Afr2Sensors_Task`.
- Calibración «Hall una vuelta»: CPR medido por rueda girando a mano en sentido exterior horario; `hall_cw_mask` marca la rueda y `hall_signs` corrige el sentido. Con AFR2-20261106-34 ya no se aborta por detenciones de enlace.
- Odometría: cinemática Mecanum inversa coherente con la directa de `Afr2Product_Drive` (mismos coeficientes FL/FR/RL/RR y signos `AFR2_FORWARD/YAW_WHEEL_SIGN`). Pose expuesta a la Web por `robot.state.pose`.
- Go-to: P en marco del robot, tolerancia 10 mm / 25 mrad, tope 60 s, PID por rueda.

## Huecos encontrados y corregidos
1. **Signo PWM vs Hall sin verificar.** El PID asume que PWM positivo aumenta la cuenta normalizada de esa rueda. Si `motor_signs` o el sentido Hall están cruzados, el lazo cerrado es realimentación positiva: la rueda se dispara al PWM máximo y nada lo detectaba. Ahora cada rueda acumula un puntaje de concordancia (|PWM| >= 60, 300 ms después de un cambio de signo, ±20) en todos los modos, incluso sin CPR. En lazo cerrado (manual con Hall, línea, go-to, secuencias), un puntaje <= -10 detiene con **fallo 4**. PWM manual y prueba de rueda solo lo informan. El puntaje se reinicia al cambiar `motor_signs`, `hall_signs` o `hall_cw_mask`, y al borrar el fallo.
   - Pantalla Motores (AFR2_N_WHEEL_VIEW): `Inv:XX HP:????` por FL FR RL RR: `+` coincide, `!` cruzado, `?` sin datos.
   - Web: el chip «Fallo» de la barra operativa muestra el texto del código (1..4).
   - No se corrige solo: un cruce puede venir del motor o del Hall y elegir mal rompería el PWM manual que hoy se ve correcto.
2. **Go-to frenado cerca del objetivo.** La ley P pide ~error mm/s; a pocos centímetros queda bajo la zona muerta del motor y el auto se detenía hasta el tope de 60 s. Mínimo de 40 mm/s (o la velocidad de misión si es menor) en traslación y su equivalente angular en giro puro, solo fuera de tolerancia.

## Procedimiento sugerido antes del go-to (físico, pendiente)
1. Calibración → Hall: «Hall una vuelta» en las cuatro ruedas, girando cada una a mano en sentido exterior horario; Guardar.
2. Conducir en PWM manual hacia adelante unos segundos y revisar Motores: las cuatro deben mostrar `+`. Una `!` se corrige en «Signos de motor FL FR RL RR» (o repitiendo esa rueda en Hall si el sentido de giro fue el contrario) y se guarda.
3. Verificar geometría: diámetro de rueda y Lx+Ly son nominales de CAD (`AFR2_FIXED_*`); medirlos y comparar un avance conocido de 1 m.
4. Recién entonces probar go-to corto (p. ej. 300 mm) con el auto en el piso.

Compilación: ATmega PASS (avr-gcc 7.3.0), flash 127288 B (48,6 %), SRAM 5659 B (69,1 %, +18 B). Web `tsc -b` PASS. Sin cargar ni probar físicamente.
Estado: atmega=fuente editada y compilada, sin cargar; web=fuente editada y compilada; esp=sin cambios.
