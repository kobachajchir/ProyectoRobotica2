# 6. Selección de actuadores y plan de rediseño v07

> **Continuación AFR2-20261027-20:** este capítulo conserva el plan inicial de la entrega18. Se ejecutó la sustitución del codo por GX3270BLS y el modelado/refuerzo v07, con masas y escenarios de frenada recalculados. [Resultado y pendientes reales](07_implementacion_y_cargas_v07.md). La selección operativa de todos los ejes sigue abierta.

[Índice del informe](INFORME_ITERACION_DISENO.md) · [Anterior: frenada](05_frenada_y_esfuerzos_combinados.md)

**Estado:** plan integrado, no ejecutado. Se conserva el maestro v06 y su copia de estudio. No hay motor nuevo aprobado, geometría v07 ni dimensiones finales de fabricación.

## Por qué se reabre la selección

El DS3218 del codo requiere 15,83 kgf·cm estáticos en el caso de PETG macizo con 200 g. El objetivo preliminar ×2 es 31,65 kgf·cm, frente a un máximo publicado de bloqueo de 21,5 kgf·cm a 6,8 V. Cambiar el motor está justificado por ese margen insuficiente; aún falta calcular lo requerido por la frenada.

El hombro necesita 17,46 kgf·cm en el servo con su reducción 2:1 y eficiencia supuesta 0,85; su objetivo estático preliminar es 34,92 kgf·cm. J5 necesita 4,36 kgf·cm, objetivo 8,71. Estos valores son filtros iniciales, no torques continuos certificados ni el dimensionamiento dinámico final.

La selección será por eje. J1, J4 y J6 también deben revisarse por inercia y flexión aunque su torque gravitatorio sea bajo en la postura simétrica. En pinza gobiernan además fuerza normal, rozamiento y aceleración de la pieza.

## Candidatos comerciales argentinos documentados

Consulta documental del 27/10/2026. La publicación de stock acredita oferta del vendedor, no una reserva o identidad con una ficha genérica.

| Candidato | Oferta consultada | Dato anunciado útil | Información pendiente antes del CAD de montaje |
| --- | --- | --- | --- |
| GX3240MGV1, SKU 3132 | [Starware](https://tienda.starware.com.ar/producto/servo-motor-digital-starware-gx3240mgv1-40k-5-74v-270g-metal-alto-torque/), publicado con stock | Hasta 40 kgf·cm, 270°, alimentación 5–7,4 V | Ficha de variante exacta, torque sostenido, corriente, masa neta y plano de fijaciones |
| DYM BLS-HV70, SKU 2996 | [Starware](https://tienda.starware.com.ar/producto/servo-motor-digital-brushless-dym-bls-hv70-70kg-5-84v-180gr-metalico-ip67/), publicado con stock | Bloqueo anunciado 68 kgf·cm a 7,4 V; 180° | Plano mecánico, masa neta, corriente y servicio térmico; identificación exacta del fabricante/variante |

El GX ofrece un escalón de torque con una envolvente de familia compacta; el DYM anuncia mayor margen. Estas observaciones no cierran la selección: faltan la demanda dinámica, el servicio y las cotas trazables. No se trasladan las dimensiones 2×5×5 cm del apartado comercial del DYM a su cuerpo mecánico. Tampoco se usa la referencia de familia GX 40×20×40,5 mm como plano completo de la variante V1.

No se incorporan precios al presupuesto final: no hay lista de compra cerrada. El costo se evaluará junto con transmisión, fuente/regulador, horn, herrajes y facilidad de reposición, no solo el precio del servo.

## Secuencia de trabajo y entregables

| Paso | Trabajo y motivo | Entregable |
| --- | --- | --- |
| 1. Cerrar condiciones de movimiento | Definir velocidades bajo carga, aceleración, frenada y duración de sostenimiento por eje | Tabla de operación y casos, con hipótesis identificadas |
| 2. Completar masas e inercias | Incorporar alternativas de motor, masas de montaje, tensor de cada conjunto y cableado | Presupuesto v07 y escenarios comparables, antes de aceptar una alternativa |
| 3. Obtener demandas | Resolver estática y dinámica por alternativa, incluyendo parada del auto y cargas internas | Pares, fuerzas/momentos por sección y casos gobernantes |
| 4. Cerrar motor y alimentación | Comparar capacidad continua/transitoria, velocidad bajo carga, recorrido, corriente y costo | Ficha de selección por eje, con datos faltantes explícitos |
| 5. Modelar el motor | Obtener cotas y crear componente independiente antes del housing | Modelo paramétrico de referencia con procedencia |
| 6. Rediseñar articulación | Adaptar horquilla, cuna, eje, acople, apoyos y retención a las nuevas cargas y dimensiones | Ensamble v07 editable, con piezas fabricables identificadas |
| 7. Recalcular por la modificación | El nuevo motor/refuerzo cambia masas, CG y esfuerzos proximales | Comparación antes/después, no extrapolación de resultados v06 |
| 8. Revisiones posteriores | Planear tensiones/contactos, geometría, fabricación y evidencia física con su alcance | Registro separado de cada resultado realmente obtenido |

Esta entrega no ejecuta esos cálculos o revisiones. Las comprobaciones CAD, FEA, compilaciones y ensayos se mantienen como trabajo posterior; no se inicia una batería de validaciones por crear el informe.

## Cotas y parámetros necesarios del nuevo actuador

El motor será un componente propio con cuerpo, pestañas, salida, horn y paso de cable. Los parámetros incluirán largo/ancho/alto de carcasa, espesor y altura de pestaña, patrón y diámetro de agujeros, offset del eje respecto de las caras, altura de salida, estriado y tornillo central. El recorrido y las cotas de cable/conector también forman parte de su envolvente útil.

Cada parámetro registrará unidad, valor, fuente, revisión del plano y condición de dato: fabricante, medición o inferencia. La apariencia se mantendrá separada del material de cálculo. Las dimensiones faltantes podrán usarse como referencias editables, pero no convertirse en cotas finales de un alojamiento para imprimir.

La cuna y la horquilla dependerán de esas referencias y de holguras de montaje separadas. La masa de catálogo se aplicará una sola vez al conjunto motor; se distinguirán horn y accesorios incluidos o no en ese peso.

## Rediseño estructural prioritario

| Zona | Dirección de cambio que se estudiará | Efecto que debe justificarse |
| --- | --- | --- |
| Cartucho J1 y montura | Mayor separación de apoyos, continuidad del asiento y nervios hacia placa; reubicar accionamiento si hace falta | Reducir reacciones locales sin elevar globalmente el brazo ni invadir el pan |
| Hombro | Refuerzo de raíces, apoyo del eje, cubo/planos y ancho/raíz de dientes | Transmitir par y fuerzas de engrane sin confiar en un contacto visual |
| Primer eslabón | Mayor solape útil del manguito, transiciones redondeadas y nervios localizados | Reducir concentración, giro/deslizamiento y aplastamiento; no engrosar todo el caño por defecto |
| Codo | Housing integrado a horquilla, acople trazable, eje continuo y apoyos externos | Usar el nuevo par con una ruta de carga completa y acceso de montaje |
| Contraplaca y acrílico | Área de reparto, rigidez, arandelas, casquillo de compresión y precarga | Evitar que el refuerzo del brazo traslade el problema a una placa o anclaje débil |

Los solapes de 25–30 mm o separaciones mayores de apoyos son direcciones de estudio del capítulo 3, no dimensiones ya aprobadas. El horn transmitirá el par según su diseño; los apoyos externos deben resolver las otras cargas de interfaz sin suponer que el servo admite cualquier carga radial o axial.

Las piezas impresas conservarán PETG rojo y una estrategia de impresión definida por la carga. Se documentarán orientación, perímetros, capas, radios, accesos y herrajes. Un nervio adicional debe mejorar una sección y justificar la masa que añade.

## Alimentación, control y recorrido

La tensión de selección será la que realmente pueda mantenerse en el servo. Deben revisarse batería mínima/máxima, margen de regulación, corriente simultánea, cableado y distribución de potencia. Los servos con distintas tensiones máximas pueden necesitar rieles separados; el módulo PWM no reemplaza una fuente de potencia adecuada.

Se mantienen servos posicionales y no se exige añadir sensores externos en esta fase. La orden PWM representa una posición solicitada, no una confirmación de que la articulación llegó o cumplió el tiempo de frenado. Las prestaciones pendientes no se acreditan por el controlador elegido.

Si se añade reducción i, idealmente θ_salida=θ_servo/i y el par aumenta según relación y eficiencia; también cambia la velocidad de salida y la inercia reflejada. Debe preservarse el recorrido articular necesario. Un motor de 180° con reducción 2:1 no conserva automáticamente 180° útiles en el codo.

## Registro de cada modificación futura

Cada cambio deberá conservar: identificador de pieza/junta, versión anterior, caso de carga que lo motiva, fuerza/momento y referencia usados, geometría/material/orientación nuevos, variación de masa y CG, efecto sobre motores proximales, y estado de la comprobación correspondiente.

La v07 se guardará como revisión nueva; v06 y los datos de 200 g seguirán siendo antecedentes reproducibles. Los despieces se producirán cuando el usuario indique iniciarlos. La documentación de una corrección no se redactará como «resiste» hasta disponer de resultados con alcance y condiciones identificados.
