# Cargas y resistencia nominal de la v09

[Índice](README.md) · [Datos de cálculo](resistencia_v09.json)

Se recalculan las masas y los tensores con geometría v09 y PLA PrintaLot de densidad1,24 g/cm³. Se evalúan **79,651 estados** a partir de 4,411 estados fuente y 211 casos, estática incluida. Se incluyen las64 combinaciones de sentido de seis ejes frenando juntos y las frenadas individuales. Todas las velocidades de trabajo son30°/s, con paradas suaves de200/100/50 ms y21 instantes por trayectoria. Son hipótesis de diseño, no prestaciones nominales medidas.

El punto final está totalmente extendido; J1 se orienta entre−90° y+90°, cada10°. Se descartan trayectorias que exceden sus límites. La pieza es un cubo50 mm y200 g, solidario a la herramienta en el cálculo: la pinza no se abre durante la frenada. No se presenta una envolvente continua de todas las posturas y velocidades.

## Presupuesto de masa y centro de masa

Masa del conjunto del brazo, montura/control y carga: **2.400,43 g**. Centro de masa en la postura horizontal de cálculo: **[-13.291, 110.93, 147.227] mm**. Se excluye la masa del auto de referencia: esto no demuestra estabilidad del AGV.

| Grupo | Masa (g) | CG global en esa postura (mm) |
| --- | ---: | --- |
| 01_Giro_base | 519,91 | [-17.41, -11.47, 137.53] |
| 02_Brazo_PVC | 410,06 | [-22.86, 58.58, 205.68] |
| 03_Antebrazo_PVC | 252,50 | [-0.07, 164.46, 204.97] |
| 09_Pinion_hombro | 8,26 | [-41.38, -14.0, 164.0] |
| 90_Pieza_REFERENCIA_50mm_100g | 200,00 | [0.0, 444.0, 209.0] |
| 04_Giro_muneca | 191,12 | [-30.65, 277.85, 203.51] |
| 05_Inclinacion_muneca | 87,32 | [-3.37, 312.37, 207.6] |
| 06_Herramienta | 87,15 | [0.03, 381.47, 207.7] |
| 07_Mandibula_izquierda | 9,73 | [-5.9, 421.78, 234.22] |
| 08_Mandibula_derecha | 9,73 | [5.9, 421.78, 183.78] |
| 10_Pinion_pinza | 3,93 | [-0.0, 401.03, 209.0] |
| 11_Pinion_J4 | 6,65 | [-0.0, 243.43, 173.0] |
| 00_Base_control | 614,06 | [-11.35, -11.43, 35.0] |

Las masas comerciales sustituyen la densidad de las envolventes: GX3270BLS90 g, GX3240100 g supuesto conservador y 6000-2Z20 g de referencia. Los impresos se consideran macizos; relleno, cables y herrajes reales deben contrastarse. El inventario externo y las inercias escaladas son la base de los esfuerzos, no una pesada del prototipo ni la masa global nativa del auto completo.

## Demanda de actuadores

| Articulación | Par estático del servo (kgf·cm) | Mayor demanda en frenadas conjuntas (kgf·cm) | Anuncio de selección |
| --- | ---: | ---: | --- |
| J1 | 0,00 | 16,30 | GX3270BLS ≥63 a7,4 V |
| J2 | 17,47 | 34,47 | GX3240MGV1 hasta40, variante/curva pendientes |
| J3 | 15,70 | 34,13 | GX3270BLS ≥63 a7,4 V |
| J4 | 0,68 | 1,42 | MG90S: curva y frenado pendientes |
| J5 | 4,33 | 10,58 | MG946R: curva y frenado pendientes |
| J6 | 0,00 | 0,06 | MG90S: curva y frenado pendientes |

J2 incluye relación2:1 y eficiencia0,85 supuesta. Los anuncios son picos de catálogo, no par continuo ni capacidad de frenada certificada. El margen de J2 y el objetivo preliminar×2 deben revisarse con los casos conjuntos; cambiar J1 no aprueba el resto del brazo. La sensibilidad adicional a600°/s sin carga se conserva aparte en las frenadas individuales, sin convertirla en una velocidad de trabajo aprobada.

## J1 y sus uniones

| Magnitud | Estática, máximo entre orientaciones | Mayor caso de cálculo |
| --- | ---: | ---: |
| Carga equivalente estática de referencia de un6000 | 225,81 N | 447,22 N |
| Referencia equivalente del eje | 30,59 MPa | 62,45 MPa |
| Tensión normal nominal del bastidor de dos paredes | 1,31 MPa | 3,15 MPa |
| Tensión normal de sección completa de cuna/cartucho | 1,26 MPa | 2,50 MPa |
| Demanda incremental de tracción M4 de brida | 40,45 N | 86,50 N |
| Cortante M4 de brida | 0,00 N | 16,94 N |
| Presión nominal casquilloØ6 contra PLA | 0,00 MPa | 0,35 MPa |
| Tracción incremental del tirante de cartucho | 80,20 N | 162,33 N |
| Tracción incremental de anclaje al auto | 42,33 N | 84,26 N |

El6000 usa C₀=1.960 N de catálogo; en el caso gobernante C₀/P₀=4,38. Ese cociente no es un factor de seguridad del asiento impreso ni una vida a fatiga. La separación real entre centros es13,00 mm.

Para el eje, la referencia de fluencia con n=2/Kt=2 supuestos es **249,81 MPa**. Se especifica SAE4140 bonificado con certificado Rp0,2≥600 MPa; todavía no hay pieza ni certificado de lote. La torsión circular/anular de referencia no resuelve exactamente los planos dobles, la ranura, las transiciones o los contactos.

Para las paredes impresas, el mismo criterio provisional pide **12,58 MPa** de resistencia normal efectiva en la orientación que soporte el esfuerzo. No se compara esa exigencia directamente con los53 MPa D882 publicados para el material: falta la caracterización FDM por orientación y el perfil real.

El doble plano metálico tiene engagement3,35 mm. Su reparto uniforme ideal de contacto da **53,01 MPa** en la frenada calculada. No equivale a una tensión uniaxial ni demuestra por sí solo la resistencia del cubo.

La brida metálica se estudia también con cuatro sectores en voladizo, raíz de radio10 mm y espesor3 mm. Los M4 llevan casquillos metálicos6/4,4×8 mm y arandelas para limitar el aplastamiento por apriete. Los tirantes del cartucho se cierran con tuercas/arandelas en alojamientos inferiores: no dependen únicamente del arrancamiento de insertos en las paredes altas.

## Método y límites

Los contornos de secciones proceden de BRep nativos, con discretización de cuerda0,0025 mm y plano desplazado0,0137 mm para evitar coincidencias. Se integran área, centroide y segundos momentos, incluidos huecos. Se separan reacciones radiales/axiales de los rodamientos y reacción de torque del motor. El apoyo axial se sitúa en el rodamiento superior, no en el servo.

El bastidor usa dos barras de sección variable con pies fijos, puente rígido, E=1.500 MPa y ν=0,38 supuestos. J=0,5(Ix+Iy) solo aproxima rigidez torsional. Se incluyen peso de hardware J1 e integración aproximada del peso de paredes, referidos al puente; no es una distribución exacta de cargas de volumen. El torque del motor se lleva conservadoramente a Z60 frente a pestañas Z43,95 para la sensibilidad de palanca. No se calculan tensiones torsionales locales exactas del impreso.

Los grupos de fijaciones asumen placa rígida; los incrementos normales negativos corresponden a compresión que debe equilibrar el contacto. No se acreditan pretensión, fricción, contacto unilateral, efecto palanca, tolerancias, par de apriete o capacidad de insertos. La brida, sus accesos de herramienta y el acrílico requieren análisis local de contacto y proceso; una tensión nominal baja no lo sustituye.

No se incluyen flexibilidad de reductores, rotor/reflejo de engranajes del servo, holguras reales, parada del AGV, impacto, fatiga, creep o comportamiento térmico. La comparación de catálogos no certifica parada de emergencia. Las propiedades de resistencia FDM siguen sin asignarse.

[Cálculo reproducible](resistencia_v09.py) · [Masas y dinámica](calcular_v09.py) · [Resultados](resistencia_v09.json) · [Estados completos comprimidos](estados_J1_v09.npz) · [Secciones nativas](secciones_CAD_v09.json)

## Decisión sobre el conjunto y límites de movimiento

La revisión mejora J1, pero **no aprueba todo el brazo a200 g con cualquier velocidad/frenada**. En el caso30°/s y50 ms, el objetivo preliminar de par×2 pide aproximadamente68,94 kgf·cm en el servo de J2,68,26 en J3 y21,17 en J5. Los anuncios usados como referencia son40,63 y13 respectivamente. Incluso esos anuncios son picos de catálogo, sin curva continua/frenado ni validación térmica. J2 y J5 son prioridades de la siguiente selección; J3 también pierde el margen×2 bajo esta frenada conjunta.

| Velocidad por eje | Parada | J1 | J2 | J3 | J4 | J5 | J6 |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| 30°/s | 200 ms | 4,10 | 21,72 | 20,31 | 0,88 | 5,90 | 0,02 |
| 30°/s | 100 ms | 8,17 | 25,97 | 24,91 | 1,06 | 7,46 | 0,03 |
| 30°/s | 50 ms | 16,30 | 34,47 | 34,13 | 1,42 | 10,58 | 0,06 |
| 20°/s | 200 ms | 2,73 | 20,30 | 18,77 | 0,81 | 5,38 | 0,01 |

Todos los pares de la tabla están referidos al servo, en kgf·cm, con las reducciones/eficiencias supuestas. La sensibilidad20°/s/200 ms tampoco alcanza estrictamente×2 en J2 frente al rótulo40. No se programa un límite de firmware ni se declara una envolvente autorizada por esta tabla.

El bastidor tiene una traslación máxima de referencia0,09 mm y rotación0,25° con el modelo de rigidez supuesto. La presión nominal máxima bajo las arandelas de los tirantes es5,33 MPa; no acredita compresión/creep del PLA ni el apriete real.

Las piezas impresas suman aproximadamente1,015 kg considerando geometría maciza. No es una predicción de consumo de laminado ni una masa medida; soportes, desperdicio y relleno real modifican consumo y resistencia. El PVC usaØ32 exterior/Ø28,3 interior, por lo que la pared geométrica es1,85 mm. La cota3,2 mm de la imagen original no es compatible simultáneamente con esos dos diámetros.

La masa del auto, batería, CG y puntos reales de contacto rueda/suelo son necesarios para evaluar vuelco. El CG del brazo/carga puede quedar fuera de la huella sin que los tornillos fallen. No se deduce estabilidad de los esfuerzos locales de J1. El frenado/traslación del AGV y las aceleraciones iniciales distintas de las trayectorias elegidas requieren otros casos.

Cierre: eje/brida, apoyos y cuna de J1 quedan modelados y predimensionados bajo las hipótesis declaradas. El material impreso requiere demostrar al menos la resistencia efectiva exigida por orientación, además de compresión/creep y contactos. La iteración no es un plano final de fabricación y la selección de los restantes actuadores sigue abierta.
