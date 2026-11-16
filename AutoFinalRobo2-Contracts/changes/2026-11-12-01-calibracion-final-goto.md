# AFR2-20261112-01 · Calibración final con Pruebas mínimas: llegada del GOTO y factor de giro

Versión: 1.28.1 (subversión mínima: ajuste, sin función nueva).
Estado: **atmega** fuente editada, sin compilar ni cargar; **esp** y **web** solo versión. Sin CMD nuevo ni cambio de trama.
EEPROM v13 con el mismo payload de 78 bytes (solo agrega una migración en RAM).

## Evidencia (export del 12/11/2026, producto 1.28.0, 100 mm/s, sin carga, tanda Ajuste)

| Ensayo (5 rep.) | Consigna | Odometría final | Medida | Error final | Deriva (odom − medida) |
|---|---|---|---|---|---|
| Avance 1,0 m | x 1000 | 989..991 | 989..991 | −10,2 ± 0,8 mm | −0,2 ± 0,5 mm |
| Lateral 1,0 m derecha | y −1000 | −989..−992 | −989..−992 | +10,2 ± 1,3 mm | 0,0 mm |
| Giro 90° CCW | ψ +90 | 88,6..89,0 | 91 | +1,0° | −2,2 ± 0,1° |
| Giro 90° CW | ψ −90 | −88,8..−89,2 | −91 | −1,0° | +2,0 ± 0,1° |

Lectura:

- **Avance y lateral:** la odometría coincide con la medida (factores 1,000 y 0,775 correctos). Los 10 mm que faltan son la
  tolerancia de llegada del GOTO (`|ex|,|ey| ≤ 10 mm`): el auto corta en el primer ciclo dentro de la banda, viniendo desde atrás.
- **Giros:** el control corta 1,1° antes (tolerancia 25 mrad ≈ 1,43°) y la odometría cuenta 2,1° menos que lo real en los dos
  sentidos: real/odometría = 91 / 88,9 = 1,024. Coincide con el 1,028 de AFR2-20261111-08 (360° → +10°) si el auto todavía tiene
  guardado CW/CCW = 1,000: la EEPROM v12 conserva lo guardado con 1.27.0 y el valor de fábrica 1,028 de 1.28.0 no se aplica solo.

## Cambios (ATmega)

- `afr2_product.h`: `AFR2_GOTO_POS_TOL_MM` = 4 y `AFR2_GOTO_YAW_TOL_MRAD` = 10 (≈ 0,57°), antes 10 mm / 25 mrad literales.
- `afr2_product.c`: la llegada, el corte de traslación en giro puro y el mínimo angular usan esas constantes.
- `afr2_app.c`: el encadenado de tramos de giro de Pruebas examen usa la misma tolerancia angular.
- Seguridad de la banda: a la velocidad mínima del GOTO (40 mm/s; 235 mrad/s) cada ciclo de 10 ms avanza 0,4 mm / 2,4 mrad, menos
  que el ancho de la banda (8 mm / 20 mrad), así que el auto no puede saltearla ni quedar oscilando. La resolución de odometría
  (370 pulsos por vuelta, rueda de 80 mm) es ≈ 0,68 mm por pulso y ≈ 1 mrad de giro.

- `afr2_preferences.c` / `afr2_product.h`: **factor de giro en firmware**. `AFR2_CONFIG_VERSION` 12 → 13 (mismo payload). Al leer
  un registro anterior a v13, CW/CCW que valgan 1000 pasan a `AFR2_TURN_ODOM_PERMILLE` (1028) en RAM, sin autoguardar, como las
  demás migraciones. Al guardar cualquier ajuste queda v13 y desde ahí se respeta cualquier valor elegido. Otros factores no cambian.
- `afr2_product.c` / `afr2_product.h`: **rumbo durante la traslación**. El usuario confirmó que en el lateral a la derecha el auto
  se iba girando antihorario y quedaba corrido de costado; con ganancia 1 (35 mrad de error → 35 mrad/s) el rumbo recién se
  corregía al llegar. `AFR2_GOTO_YAW_KP_TRANSLATE` = 3 mientras hay traslación; el giro puro conserva ganancia 1. Sigue la
  normalización por `mission_speed`, así que la velocidad de rueda máxima no cambia.

## Esperado tras cargar 1.28.1

Avance y lateral: −2..−4 mm. Giros: ≈ −0,5° (sin el sobregiro de +1° actual). Falta repetir la tanda para confirmarlo.

## Pendiente

- La presentación (`presentacion/`, filmina 6, D-04 y anexos) sigue diciendo 10 mm / 25 mrad; actualizar al cerrar la medición.
- Lateral derecha: confirmar que con ganancia 3 ya no se gira antihorario en el recorrido. Si persiste y la odometría no lo ve
  (deriva ψ cercana a 0), es patinamiento de los rodillos y hace falta otra corrección.
- Comprobar en Escala de factores que CW/CCW muestren 1,028 después de cargar.
- El lateral tarda 14,7 s contra 11,1 s del avance por el patinamiento de los rodillos (0,775); no se compensa.
