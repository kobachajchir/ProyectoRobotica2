# Selección de revisión: Yipin GX3270BLS, 270°

27/10/2026. Se selecciona **para el prototipo CAD v07 del codo J3** el GX3270BLS de 270°, Starware SKU 2982, con oferta argentina publicada con stock. No es una compra realizada ni una capacidad de 200 g/frenada certificada.

Fuente de oferta y datos: [Starware](https://tienda.starware.com.ar/producto/servo-motor-digital-brushless-yipin-gx3270bls-70kg-5-84v-270gr-ip68-metal/). El fabricante se identifica como Yipin en el servo y en el plano comercial de la galería. Su sitio GXSERVO no respondió durante esta consulta: el dibujo acotado y las fotografías se recuperaron desde la galería del proveedor. Se conserva [manifest con URL y SHA256](fuentes_actuador.json); las descargas recuperables están en vendor/, excluidas de Git. Licencia de las imágenes no identificada; modelo paramétrico propio elaborado desde cotas, sin CAD ajeno importado.

## Motivo de elección

Se requiere más margen que el DS3218, cuyo bloqueo anunciado máximo es 21,5 kgf·cm frente al objetivo estático preliminar de 31,65 kgf·cm del codo. La publicación del GX3270BLS anuncia ≥63 kgf·cm a 7,4 V, frente a 15,83 kgf·cm estáticos originales; la demanda deberá actualizarse con el montaje nuevo y la frenada.

Se priorizó esta referencia frente a los candidatos GX40/DYM70 por la combinación de oferta local, recorrido de 270°, corriente anunciada y plano mecánico identificable. No se usó solo el rótulo «70 kg». Tampoco se atribuye al máximo anunciado carácter de torque continuo ni frenado garantizado.

| Dato | Publicación SKU 2982 | Uso |
| --- | --- | --- |
| Torque máximo a 6/7,4/8,4 V | ≥55 / ≥63 / ≥70 kgf·cm | Filtro de capacidad de pico anunciado; curva continua no disponible |
| Corriente de bloqueo a 6/7,4/8,4 V | ≤5 / ≤7 / ≤9 A | Entrada para revisar alimentación; no corriente de operación calculada |
| Velocidad sin carga a 7,4 V | ≤0,10 s/60° | No se usa como velocidad nominal bajo carga |
| Masa sin accesorios | 90 g | Reemplaza los 60 g de DS3218 en presupuesto del codo |
| Cuerpo principal | 40,5×21×41,7 mm | Modelo de carcasa |
| Pestanas y patrón | 55,1 mm totales; centros48×10 mm; cuatro pasos Ø4,5; espesor 2,5 mm | Nueva fijación4M3 con arandelas e insertos |
| Estriado | Ø5,94/Z25; longitud5 mm | Envolvente cilíndrica, no dientes de fabricación |
| Giro | Variante270°; publicación contiene también 358° y fotos180° | Modelo elegido270°, recorrido articular conservado; cotejar unidad comprada |

Los valores eléctricos son del proveedor. No hay curva torque–velocidad bajo carga, par continuo, desaceleración nominal ni curva térmica trazables de la unidad. No se han inferido estos datos de su apariencia o de un modelo distinto.

## Lectura dimensional y reservas

El dibujo 8705 acota altura de cuerpo 41,7, nivel superior45,15 y estriado5. La foto 8703 usa45,15 como altura de otra envolvente. Esta discrepancia se conserva explícita: el modelo separa cuerpo 41,7, resalte 3,45 y estriado5, y reserva hasta 50,15 mm en total. Es una interpretación conservadora de montaje, pendiente de medir.

El offset longitudinal del eje no está acotado. Se propone **10,5 mm** al extremo corto, editable mediante v07_axis_offset. El resalte circular se simplifica como Ø20. No se afirma que esas dos cotas sean del fabricante. Por eso el housing es un prototipo ajustable, no un plano liberado de fabricación.

La cara de punta del estriado se conserva en Y=42 mm del grupo del primer eslabón y el horn en Y=40…42 mm. La nueva carcasa se extiende hacia el exterior. El eje articular y sus dos 608 no se desplazan para acomodar el servo.

## Montaje mecánico

La horquilla, el manguito distal y la cuna se diseñan como una pieza PETG roja. El nuevo patrón usa cuatro M3, arandelas sobre los pasos Ø4,5 del servo e insertos metálicos. La bandeja inferior y los largueros enlazan los bancos de pestañas con la horquilla; no quedan soportes independientes apoyados visualmente.

Se conserva la brida y el eje Ø8 con planos de arrastre y retención del codo. El horn móvil se representa metálico25T; su unión a la brida mantiene un patrón de adaptador de referencia que debe ajustarse al horn comercial antes de fabricar. No se imprimen estrías para reemplazar el horn metálico.

Los manguitos del primer eslabón amplían la guía/solape útil de 17 a 27 mm sin cambiar los caños de 80/70 mm ni los130/120 mm entre ejes. El modelo incorpora refuerzos locales; sus espesores son propuestas de diseño y requieren análisis de resistencia posterior.

## Base y alimentación

La montura sigue en Z = 6…12 mm y el hombro en Z = 209 mm sobre el chasis. Se agrega material alrededor del cartucho J1 y nervios a sus columnas y a la torre del hombro. La separación de los centros608 de J1 permanece8 mm: no se presenta el refuerzo como una reducción de la fuerza por apoyo. Separarlos más exige reubicar el accionamiento sin elevar globalmente el robot y sigue pendiente.

Se propone alimentar este servo con **7,4 V regulados**, separado de los servos de menor tensión máxima y la lógica. No conectar directamente una batería2S de 8,4 V máxima basándose en el título comercial: las fotos indican5–7,4 V. La publicación incluye8,4 V, pero el diseño conserva el límite menor hasta confirmar la variante.

El riel debe contemplar hasta 7 A de bloqueo anunciados de este actuador y la suma de las otras cargas que comparta. No se acredita que el XL4016 existente entregue esa corriente continua ni que una batería2S permita regular7,4 V en toda su descarga. Fuente/batería, regulador, disipación y cableado quedan por cerrar. La distribución de potencia no pasa a depender de la placa lógica o de sus pistas de señal PWM.

La variante 270° permitiría conservar el recorrido del codo sin reducción externa nueva. La relación nominal500–2500µs para270° daría, alrededor de 1500µs, aproximadamente833–2167µs para±90°; es un mapeo de referencia, no calibración física ni modificación de firmware.
