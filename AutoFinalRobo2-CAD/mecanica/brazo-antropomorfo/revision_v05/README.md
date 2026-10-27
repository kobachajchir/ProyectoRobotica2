# Montura baja con J1 retrasado — v05

AFR2-20261027-12, 27/10/2026. Se reemplaza la solución elevada v04, rechazada por el usuario. El objetivo es mantener el montaje a la altura de la electrónica existente y reservar el giro del HC-SR04 mediante geometría y reubicación de apoyos.

## Disposición propuesta

| Cota respecto de la cara superior del chasis | v04 elevada | v05 baja |
| --- | ---: | ---: |
| Cara inferior de la montura | 54 mm | 6 mm |
| Cara superior de la montura | 60 mm | 12 mm |
| Eje J1, desplazamiento longitudinal | 0 mm | −14 mm, hacia atrás |
| Cara de salida del motor J1 | 112 mm | 64 mm |
| Interfaz giratoria de J1 | 144 mm | 96 mm |
| Centro del hombro | 257 mm | 209 mm |
| Separadores laterales | 54 mm | 6 mm |
| Casquillo central, largo | 57 mm | 9 mm |
| Tornillos laterales | M3×70 | M3×25 |
| Tornillo central | M10×80, cabeza saliente | M10×30 avellanado |

El cuerpo de referencia del Mega ocupa Z=4,5…19 mm y el shield Z=17,37…34,62 mm. La montura baja comienza 1,5 mm por encima de la cota inferior del Mega; no se eleva el conjunto para pasar por arriba del sensor. El motor y el brazo quedan 48 mm más bajos que en v04. El cuerpo del motor comienza a Z=21,1 mm. El plato giratorio de J1 queda a Z=96 mm, dejando 3 mm sobre los postes del rack XL que terminan a Z=93 mm. La torre local del hombro conserva 113 mm y los eslabones siguen midiendo 130/120 mm entre ejes.

El eje de J1 se retrasa hacia la electrónica existente sin desplazar el sensor. Los apoyos delanteros dejan libre el barrido y los traseros se sitúan delante del borde de las placas. La placa lleva un recorte de radio `pan_radius_bound + pan_clearance = 34,4 mm` alrededor del eje real del pan. La ventana del voltímetro sigue abierta. Un retiro posterior izquierdo deja 3 mm delante del Model Y, cuyo borde llega a Y=−30 mm; el pilote izquierdo posterior se adelanta 4 mm para conservar todo su apoyo en la placa recortada. Se conservan los seis agujeros de montaje A…F, sus contraplacas y el apoyo central; no se perfora el chasis para esta revisión.

Los XL4016 se retrasan 18 mm y se colocan a Z=26 mm. Sus postes delanteros se apoyan en la montura; los posteriores se conectan mediante puentes a Z=22…26 mm que pasan sobre el extremo izquierdo de la Mega, cuya envolvente termina a Z=19 mm. Se conservan 3 mm nominales en esa zona. Los clips de las placas se unen a los postes y los puentes; no son piezas flotantes. PCA9685 y Nano comienzan a Z=15 mm y la bornera se sitúa en un cajetín bajo a la derecha y atrás del motor.

El soporte de J1 conserva dos 608, tapa y tornillos desmontables, asientos con cotas independientes y el husillo continuo de la v03. Las partes impresas de la montura se integran en una pieza roja. El pan mantiene `J8_Pan_HCSR04` y los 25 sólidos móviles existentes, entre −90° y +90°.

## Apriete central y ensamblaje

Se representa un M10×30 avellanado DIN 7991 con cabeza nominal Ø20 × 5,5 mm, cono de 90° y cuerpo simplificado sin hélice de rosca. El largo incluye la cabeza. El tornillo queda enrasado a Z=12 mm y su extremo a Z=−18 mm; la tuerca y arandela inferiores se conservan. El asiento cónico transmite el apriete a la placa y el casquillo soporta la compresión.

Las fijaciones de la montura se deben montar y apretar **antes de instalar el motor J1, el plato giratorio y las placas**, por el espacio limitado para introducir la llave por arriba. El poste derecho tiene un canal vertical Ø7 mm sobre C; los puentes llevan retiros Ø9 mm sobre E/F para introducir las cabezas y la llave. Las bandas longitudinales de los puentes permanecen conectadas a ambos lados de esos retiros. La montura lleva el asiento cónico y el rebaje del borde de la cabeza central; su acabado y tolerancia dependen del tornillo real y la impresión.

Fuentes comerciales consultadas el 27/10/2026:

- [Fornis, DIN 7991](https://www.fornis.com.ar/subfamilia_print.asp?sfam_codigo=0003): figura M10×30, código 78.10.30. La inclusión en catálogo no confirma stock actual.
- [Fercor, catálogo de bulonería](https://www.fercor.com.ar/database/utilidades/archivos/7/BULONERIA.pdf): cabeza nominal Ø20 para M10.
- [Kotadia, dimensiones DIN 7991](https://kotadiainc.com/pg/din-7991-specifications/): cabeza M10 nominal 20/5,5 mm, llave 6 mm. Verificar la variante del fabricante antes de fabricar.

## Archivos y parámetros

- [F3D editable](../AFR2_Brazo_PVC32_AGV_v05.f3d).
- [Parámetros](../parametros_prototipo_v05.csv).
- [Fuente de modelado](../mount_low_v05.py).
- [Fuente de comprobación CAD](../check_low_v05.py).
- [Registro de fases](authoring_journal.jsonl).

`agv_mount_z`, `agv_mount_t`, `v05_axis_y`, `v05_motor_bottom`, `v05_motor_face`, `v05_rack_y`, `v05_rack_rail_z`, `electronics_z` y `v05_logic_z` gobiernan el montaje. `agv_pan_lift` se conserva por compatibilidad histórica y vale **0 mm**. Las referencias comerciales mantienen croquis y traslaciones nativas; las piezas nuevas tienen croquis y operaciones parametrizadas. La v04 completa no se sobrescribe.

## Estado y límites

Estado guardado por pedido del usuario el 27/10/2026 a las 13:25: maestro v05 en Fusion y F3D local, parámetros CSV e inventario [modelo_v05.json](modelo_v05.json). Montura baja, apoyo J1 retrasado, puentes/racks, accesos E/F y holgura detrás de XL1 modelados. **Pendiente la revisión de interferencias y movimiento de esta geometría.** Los resultados de v04 no se trasladan a v05.

Un montaje más bajo reduce la altura geométrica de sus masas; esta revisión no acredita estabilidad, resistencia, torque ni carga útil real. No se fabricó, imprimió ni ensayó el robot. Los componentes comerciales y las roscas son representaciones simplificadas.
