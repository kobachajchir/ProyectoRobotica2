# Escalas gráficas de radar — AFR2-20261102-18

Pedido humano confirmado: **25 cm · 50 cm · 1 m · 2 m**.
Esta ficha sustituye las escalas gráficas 4/2/1/0,5 m descritas en
QUICK_RADAR_ACTUATORS_9.md, sin cambiar sus gestos, rastro, comandos ni retornos.

| Índice local persistido | Radio máximo | Etiqueta de menú | Etiqueta compacta |
| --- | --- | --- | --- |
| 0 | 2.000 mm | 2 m | 2m |
| 1 | 1.000 mm | 1 m | 1m |
| 2 | 500 mm | 50 cm | 50cm |
| 3 | 250 mm | 25 cm | 25cm |

ATmega y copia raster Web usan `2000 >> índice` (índice inválido: 0), cuatro
anillos equidistantes y ocultación de ecos fuera de escala. El editor del servo
comparte radio y etiquetas. Web 3D ofrece las cuatro escalas en orden ascendente,
con metros internos y etiquetas cm/cm/m/m. Su selección de escala visual sigue
siendo independiente de la preferencia OLED; no envía comandos al cambiarla.

El formato de EEPROM no cambia. Las preferencias ya guardadas conservan su
índice y adoptan el radio nuevo. No se migra ni escribe memoria al editar fuentes.
La escala limita la representación; no modifica la adquisición, la validez
física del HC-SR04 ni sus límites de protección.

Radar OLED y editor conservan ScreenCodes 0x030031 y 0x030033, navegación y
flujo compartido. ESP conserva el transporte de framebuffer y comandos.

atmega/web: fuente actualizada. esp: sin cambios. Sin tests, compilaciones,
validaciones, ejecución del generador OLED, promoción ni flasheos.
