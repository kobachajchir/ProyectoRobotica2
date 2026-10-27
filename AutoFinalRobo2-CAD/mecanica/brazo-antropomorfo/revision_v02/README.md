# Revisión v02: pinza, muñeca y movimiento

26/10/2026. Corrección solicitada por el usuario tras detectar piezas cruzadas en la pinza. Esta revisión autoriza comprobaciones de interferencias y movimiento en CAD; no es una prueba física ni un cálculo de carga.

## Cambios de arquitectura

La pinza usa un solo piñón de 24 dientes, módulo1 y ángulo de presión20°, entre dos cremalleras opuestas. Se eliminan los engranajes coaxiales superpuestos de la revisión v01. Las cremalleras ocupan carriles distintos; cada una mueve su mordaza y lleva su propio patín. La transmisión del servo a la corredera y la oposición de las mordazas se representan con vínculos de movimiento nativos de Fusion.

La relación de esta revisión es `apertura = 4,00708355 mm + 2 × 12 mm × ángulo_servo_en_radianes`. El cero geométrico se ajustó para que el ángulo109,8°, representable por el accionamiento nativo de Fusion, dé exactamente50 mm. El recorrido del servo de referencia0–180° corresponde a4,007–79,405 mm entre almohadillas. La prueba de ida y vuelta nativa dio50,000 mm a109,8° y60,011 mm a133,7°. Para entrar alrededor del cubo50 mm quedan aproximadamente5 mm por lado. Con la pieza presente se detiene el cierre en50 mm; el resto del cierre se comprueba con la pinza vacía.

J6 pasa a acople directo coaxial con ejeØ5 y apoyo625, eliminando el par lateral que invadía la pinza. Se separan el alojamiento fijo, el eje giratorio, la estructura de la palma y el servo de apertura. Se reorienta90° la abrazadera proximal del antebrazo para apartar sus orejas del plano de flexión del codo. Se integran los volúmenes impresos que forman un mismo soporte y se abren los pasos completos de los ejes. Los cortes se limitan a su pieza impresa; las envolventes completas de los siete servos se conservan en la revisión comprobada.

| Medida | v02 | Motivo |
| --- | --- | --- |
| Base | 160 ×160 mm, caja69 +tapa3 mm | Separar servo central, acople y rodamiento; conservar control |
| Hombro sobre mesa | 185 mm | Espacio para reducción, carcasas y plegado |
| Hombro–codo / codo–J4 | 130 /120 mm | Conservados |
| Cortes PVC | 80 /70 mm, Ø32/Ø28,3 | Conservados |
| J4–J5 / J5–J6 | 55 /65 mm | Despejar barridos y alojar servo coaxial |
| J6–TCP | 78,5 mm | Bastidor, servo y carrera de los dedos |
| J4–TCP | 198,5 mm | Sustituye los75 mm de la propuesta inicial |
| Palma | 118 ×60 mm | Guías que retienen los carros hasta apertura máxima |
| Reducción hombro | 20T/40T, módulo1,5, 2:1 | Perfil de evolvente con fase de engrane |

Estas separaciones mayores son una modificación de diseño necesaria con esta arquitectura y las envolventes usadas; no se conserva el volumen compacto de la v01 a costa de cruzar piezas. Las masas, rigidez y torques de este conjunto más grande siguen pendientes de un estudio de carga.

## Control de servos

Se mantienen servos posicionales con control interno y ordenPWM; no se agregan sensores externos. El controlador conoce el ángulo ordenado y su transformación geométrica, pero el PWM por sí mismo no devuelve una lectura del ángulo físico alcanzado. La relación pulso–ángulo y los extremos se ajustarán a los servos reales, sin confundir variantes posicionales con rotación continua.

| Eje | Recorrido nominal del servo usado como referencia CAD | Transformación al eje |
| --- | --- | --- |
| J1 MG946R | 0–180° | q1 = servo −90° |
| J2 GX3240MGV1 | 0–270° | q2 =57,5° +(135° −servo)/2; salida −10…125° |
| J3 DS3218MG | 0–180° | q3 =servo −90° |
| J4 MG90S | 0–180° | q4 =90° −servo; engranajes1:1 |
| J5 MG946R | 0–180° | q5 =servo −90° |
| J6 MG90S | 0–180° | q6 =servo −90° |
| Pinza MG90S | 0–180° | 4,00708355 +24 × θrad mm |

Las fases de montaje de horns y engranajes son offsets mecánicos, separados del giro pedido al eje. La disponibilidad de una variante anunciada como180° no demuestra que cualquier ancho de pulso de un clon alcance exactamente esos extremos.

En Fusion, usar **Accionar unión / Drive Joint** sobre J1…J6 para mover el brazo. J7 se define con centro en90° y desplazamiento nativo−90…+90°: ángulo del servo =90° +desplazamiento J7. El agarre50 mm corresponde aJ7=+19,8°; apertura de entrada60,011 mm aJ7=+43,7°. Los límites simétricos evitan la ambigüedad del cero de la unión de revolución en el solver. J7 arrastra ambas correderas mediante dos vínculos de movimiento; no desplazar las mordazas por separado.

Las dimensiones de usuario siguen vinculadas a croquis y orígenes. `q1`…`q6` de la fuente anterior quedan como referencias de pose, sin accionar automáticamente las uniones; el movimiento nativo se hace con Drive Joint. No confundir esos parámetros o `grip_open` de referencia con una medición del estado real.

## Método de comprobación

`check_motion_v02.py` copia los sólidos nativos a cuerpos temporales de Fusion y calcula intersecciones volumétricas entre conjuntos móviles mediante el núcleo geométrico. Aplica transformaciones de la cadena y conserva caché de configuraciones relativas. Las cajas envolventes solo descartan pares lejanos; no se presentan como colisiones. Se confirma por intersección sólida cualquier candidato de choque con la mesa z0. Umbral de reporte0,001 mm³.

Se documenta la comparación entre las matrices de la cadena calculada y las posiciones nativas del ensamble, el barrido de pinza vacía, los barridos individuales y las combinaciones de muñeca ensayadas. Una rejilla finita no prueba ausencia de choque para toda trayectoria continua ni para todas las combinaciones de seis ejes. Tampoco valida tolerancias de impresión, deformación o fuerza de agarre. Los contactos propios de un acople atornillado no se deben confundir con penetración entre dos piezas que giran una respecto de la otra.

La evidencia inicial `interferencias_v01_pose_inicial.json` conserva el diagnóstico de la revisión anterior. Los informes de la revisión final identificarán sus poses y pasos de muestreo.

## Evidencia y alcance

- `apertura_nativa_fusion.json`: nueve posiciones de ida y vuelta accionando J7, con abertura medida a partir de las posiciones nativas de las mordazas.
- `extremos_nativos_fusion.json`: mínimo, centro y máximo de cada uno de los seis ejes;18 accionamientos nativos y sus ángulos leídos del ensamble.
- `paridad_cinematica.json`: matrices de la cadena calculada contra las posiciones del ensamble nativo en la pose de presentación.
- `montajes_internos.json`: penetraciones entre piezas impresas y componentes alojados; excluye las subdivisiones superpuestas de la envolvente de un mismo servo comercial.
- `pose_nativa_interferencias.json`: análisis nativo de Fusion entre los conjuntos móviles en la pose de presentación; no incluye coincidencias de caras.
- `informe_movimiento.json`: acumulado del muestreo final; incluye181 ángulos de pinza vacía,2197 combinaciones de muñeca y524 posiciones en barridos individuales, total2902.

La rejilla de muñeca usa q4/q5/q6=−90…+90° cada15°, con q1=0°, q2=35°, q3=−15° y cubo sujeto50 mm. Los barridos individuales usan pasos de2° e incluyen el extremo superior: J1/J4/J5/J6 con la misma postura de referencia; hombro−10…125° con codo−90°; codo−90…90° con hombro0°. Los restantes ejes quedan en0°, salvo los valores expresamente indicados en cada escenario del JSON. Los resultados no se extrapolan a todas las combinaciones de hombro/codo con muñeca ni a trayectorias continuas entre puntos: esas trayectorias deben planificarse para evitar autochoque y la mesa. No hay un planificador ni una protección de trayectoria implementados en firmware.

El archivo nativo y los informes son evidencia geométrica virtual. No se fabricó el brazo, no se verificaron corrientes/torques ni se demostró fuerza de sujeción para100 g. Splines, fijación axial, variantes comerciales, acabado de los dientes y tolerancias de impresión conservan el carácter de prototipo.
