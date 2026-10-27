# AFR2-20261027-09 — Anclaje bilateral y espacio del pan HC-SR04

27/10/2026. El usuario detecta que la montura v03 usa solamente agujeros derechos y que el giro del HC-SR04 interfiere con la base. Pide agregar fijaciones izquierdas debajo del voltímetro y revisar solidariamente eje de salida, soporte y sensor entre −90° y +90°. Autoriza las modificaciones y la comprobación geométrica CAD.

## Cambio

Se crea la copia `AFR2_Brazo_PVC32_AGV_v04`. Dos anclajes M3 izquierdos usan los agujeros reales del CAD, con separadores y contrabarra inferior de aluminio de 3 mm. Se mantienen los cuatro derechos y el apriete central M10. La disposición de electrónica vertical se conserva.

La montura superior se eleva 32 mm: su cara inferior queda a 54 mm sobre el chasis. La altura máxima del conjunto móvil del pan es 51 mm. Los separadores laterales pasan a 54 mm, el casquillo central a 57 mm y se representan tornillos M3×70/M10×80. El hombro queda a 257 mm; la torre local, eslabones, pinza y transmisiones conservan sus dimensiones. Las nuevas posiciones y longitudes dependen de parámetros y operaciones nativas editables.

`J8_Pan_HCSR04` vincula el conjunto móvil de salida del servo, horn, tornillo, soporte y HC-SR04 con la referencia fija del auto. Límites CAD −90°…+90°. Se reserva un cilindro conservador de 360°, radio 31,4+3 mm y altura Z=5…51 mm. La reserva se conserva oculta como referencia en el documento.

## Estado de entrega

Maestro guardado en Fusion y [F3D editable local](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v04.f3d): 126 parámetros, 332 componentes contando raíz y referencias, 14 uniones y 4 vínculos. Historial sin advertencias/errores de cálculo. La [revisión v04](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v04/README.md) reúne cotas, fuentes de modificación, parámetros exportados y vistas.

El cilindro conservador contiene los 25 sólidos móviles y queda libre de la montura para todo el giro. Se muestrean 181 ángulos del pan en cada una de dos posturas del brazo y se agrega la presentación completa: 363 casos CAD, sin penetraciones mayores a 0,001 mm³. La revisión de montajes internos y el análisis nativo de interferencias entre conjuntos de la presentación quedan sin penetraciones reportables. J8 se acciona nativamente a −90°, 0° y +90°, con transformaciones y vistas registradas. Esto no recorre todas las combinaciones de los ejes del brazo.

La revisión interna detectó que el alojamiento de la pestaña de la tapa J1 dependía del cuerpo móvil de esa tapa. Al elevarlo aparecía una penetración de 274,45 mm³. Se sustituye esa dependencia por un rebaje con diámetro y posición parametrizados; los resultados anteriores corresponden al montaje final corregido.

| Módulo | Estado de esta ficha |
| --- | --- |
| CAD | Maestro v04 editable, anclaje bilateral, pan nativo, envolvente y muestreo geométrico |
| Atmega | Sin cambios de esta ficha |
| ESP | Sin cambios de esta ficha |
| Web | Sin cambios de esta ficha ni promoción del ensamble |
| Contracts | Alcance, decisiones, evidencia y límites registrados |

Se conserva el maestro del auto y las versiones anteriores del brazo.

El muestreo v03 había tratado el auto como fijo; no acreditaba espacio para este giro. La revisión actual es geométrica: no demuestra resistencia, estabilidad del AGV con el brazo elevado ni capacidad real para manipular 100 g. PWM representa una orden, no una posición física medida.
