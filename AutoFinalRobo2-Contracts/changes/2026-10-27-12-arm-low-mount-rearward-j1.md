# AFR2-20261027-12 — Montura baja y J1 retrasado

27/10/2026. El usuario rechaza resolver el giro del HC-SR04 elevando todo el conjunto: pide mantener la montura cerca de la altura de la placa Mega y cambiar la geometría, posición del motor y apoyos. La revisión v04 elevada se conserva como antecedente rechazado para el montaje vigente.

## Diseño en elaboración

Copia `AFR2_Brazo_PVC32_AGV_v05`. Se propone montura a Z=6…12 mm sobre el chasis, frente al límite inferior Z=4,5 mm de la referencia Mega existente. Se elimina el alza general y se retrasa el eje de J1 14 mm. El cuerpo del motor empieza a 21,1 mm y su cara de salida queda a 64 mm; la interfaz J1 a 96 mm y el hombro a 209 mm. Así el plato giratorio queda 3 mm por encima de los postes del rack XL. Motor y brazo quedan 48 mm más bajos que en v04. Son cotas propuestas, no mediciones físicas ni resultado de estabilidad.

Se conserva el apriete bilateral con seis M3, contraplacas y casquillo central corto. La placa recibe un recorte circular para la envolvente del pan. Tres XL4016 se desplazan hacia atrás, sostenidos por postes delanteros y puentes que pasan sobre la Mega; los apoyos posteriores no descansan sobre la electrónica. PCA9685, Nano y bornera permanecen en la montura con nuevas cotas bajas. Se mantienen dos 608 en J1 y las uniones del brazo y el sensor.

El apriete central cambia a M10×30 avellanado DIN 7991, con asiento enrasado para liberar el espacio debajo del motor. Referencia de catálogo argentino: [Fornis](https://www.fornis.com.ar/subfamilia_print.asp?sfam_codigo=0003), código 78.10.30. Dimensiones nominales de la cabeza y fabricación final sujetas al proveedor.

Estado guardado por pedido del usuario el 27/10/2026 a las 13:25: maestro v05 en Fusion y [F3D editable local](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v05.f3d), parámetros CSV e inventario. Montura, apoyo J1, racks/puentes, accesos a tornillos y holgura XL1 modelados. Revisión de interferencias/movimiento de v05 pendiente; los resultados v04 no se trasladan a esta geometría. [Detalle](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v05/README.md). Sin cambios Atmega/ESP/Web, compilaciones, flasheos ni pruebas físicas.
