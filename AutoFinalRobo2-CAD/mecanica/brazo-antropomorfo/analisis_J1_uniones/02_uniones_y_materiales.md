# 2. Uniones, estructura impresa y contacto con el chasis

[Índice](README.md) · [Cargas y rodamientos](01_cargas_eje_y_rodamientos.md) · [Rediseño propuesto](03_redisenio_propuesto.md)

## Estado del material

El usuario confirmó que marca de PETG y perfil de impresión todavía no están definidos. Se usa E=1.500 MPa como **rigidez isotrópica equivalente de referencia** y ν=0,38 supuesto para localizar zonas; no se asigna una resistencia admisible al plástico real.

Como contraste documental, Prusament publica adhesión entre capas de 18±4 MPa y módulos de tracción de 1,5–1,6 GPa para sus ensayos/configuración. Es otro material/proceso y el dato de adhesión no es un valor de diseño universal. No se transfieren esos números a PrintaLot u otra marca sin evidencia. El relleno, orientación y contacto reales no se resuelven con el volumen CAD macizo. [Ficha del fabricante](https://prusament.com/wp-content/uploads/2022/10/PETG_Prusament_TDS_2021_10_EN.pdf).

## El acople D concentra el par

En el eje de radio r=4 mm el plano se encuentra a c=3,3 mm del centro. Su semiancho es a=√(r²−c²)=2,261 mm y el contacto nominal longitudinal h=8,8 mm. Sin acreditar fricción o pretensión, solo la parte de la cara que está comprimiéndose puede transmitir torque.

La fuerza normal del plano **no tiene brazo de 3,3 mm para el torque Z**: el brazo correspondiente es la coordenada lateral x de la presión. Para una media cara activa 0≤x≤a:

```text
T = h integral[ x p(x) dx ]
p_media >= T/(h a²)         (cota necesaria de media, sin fricción)
p_uniforme = 2T/(h a²)      (presión uniforme en media cara)
p_pico = 3T/(h a²)          (presión que crece linealmente desde el centro)
```

Con T=1,596 N·m resultan **35,49 MPa como cota inferior de media, 70,98 MPa para reparto uniforme y 106,47 MPa de pico triangular**. Son modelos simplificados de contacto, no un mapa FEA ni prueba de rotura inmediata.

No se compara p directamente con una resistencia a tracción Z: falta resolver aplastamiento, tensiones del cubo, juego, contacto con el tramo circular y creep. El resultado sí muestra que esa pequeña cara es una forma muy concentrada de transferir el par. Se propone una brida metálica con fijaciones repartidas en vez de confiar únicamente en la cara D del PETG.

## Columnas y nervios del cartucho

Se construye un bastidor idealizado de cuatro barras de sección variable, con pies fijos a Z=12,1 mm y puente rígido a Z=66 mm. Las secciones se toman del CAD dentro de regiones que excluyen rack/electrónica y la parte más exterior de los nervios. Se integra flexibilidad de axial, flexión y una aproximación de torsión mediante energía elástica; se ensamblan las rigideces en seis grados de libertad del puente. El script conserva coordenadas, regiones y propiedades.

Esta idealización omite flexión de placa de base/puente, cortante de viga, contacto y elasticidad ortótropa FDM. El J torsional aproximado se usa para sensibilidad de rigidez, no para calcular una tensión exacta de Saint-Venant. No es un mallado de sólidos o contacto de Fusion.

El máximo nominal es **8,78 MPa** en la zona superior de la columna/nervio posterior izquierdo; el máximo entre orientaciones en estática es ≈5,68 MPa. Ese nervio tiene una unión asimétrica con la columna y una terminación estrecha, que deben revisarse localmente. La inclinación del puente en el caso gobernante es ≈0,29° y su traslación lateral ≈0,094 mm con E=1.500 MPa. No son deformaciones físicas comprobadas del robot completo.

Con n=2 y Kt=2 **supuestos**, el requisito de resistencia normal sería ≈35,11 MPa. Si se imprime la base apoyada plana y las columnas verticales, la tracción longitudinal puede quedar gobernada por unión entre capas. Por ello no se aprueba esta zona con el material aún indefinido.

## Postes y fijaciones del servo

La reacción de torque se reparte como un par entre los dos apoyos existentes, separados 47,7 mm. El caso J1 a 50 ms requiere ≈33,46 N transversales por fijación. Los postes se calculan como voladizos conservadores, con secciones reales —incluido el recorte próximo al anclaje C— y momento por la altura al apoyo de pestaña.

- Tensión nominal máxima del poste: **5,68 MPa**; criterio provisional n=2/Kt=2: **22,72 MPa** de resistencia requerida.
- Cortante nominal del M3 sobre área resistente de rosca 5,03 mm²: ≈6,65 MPa.
- Presión proyectada transversal sobre inserto Ø4,5×4 mm: ≈1,86 MPa. No es capacidad de arrancamiento o giro del inserto.

La retención axial del cartucho es otro camino: tomando toda la carga axial máxima y dividiéndola entre dos tornillos de tapa, la demanda es ≈10,78 N por fijación. El promedio de esfuerzo sobre πDL del inserto sería ≈0,191 MPa. Es una exigencia geométrica media, no una resistencia demostrada del inserto instalado. El aro lleva carga radial a su asiento, no se asignan artificialmente 565 N de arrancamiento a cada tornillo de tapa.

Si un motor nuevo aplicase 63 kgf·cm a estos postes sin rediseñarlos, un escalado lineal daría ≈21,99 MPa nominales en el poste y ≈274,8 MPa de presión uniforme del modelo D. Es solo una sensibilidad: el valor anunciado ≥63 no es una cota superior segura del torque de fallo. **No se propone instalar el motor más fuerte conservando estas uniones.**

SPIROL explica que el desempeño de insertos depende de plástico, geometría de agujero e instalación. No se adopta una resistencia por tener un cilindro de latón en el CAD. [Guía del fabricante](https://www.spirol.com/resources/white-papers/how-to-design-the-proper-hole-for-heat-ultrasonic-inserts/).

## Anclajes y contraplaca

Se transporta la carga de J1 al plano Z=0 y se agrega el peso del control/montura fijos. Se estudian dos repartos de placa rígida: rigidez igual para las siete fijaciones y rigidez proporcional a área resistente/longitud de agarre. Se incluyen seis M3 y el M10 central con sus posiciones reales.

La tracción incremental máxima aparece en A: **60,70 N** con rigidez igual y **64,98 N** con At/L. El máximo equivalente de servicio del tornillo es ≈12,92 MPa, sin pretensión. Los incrementos negativos se interpretan como compresión que debe equilibrar el contacto; no se cuentan como resistencia de un tornillo a compresión.

Este cálculo **no resuelve** separación unilateral, contacto con acrílico, flexión local de placas, precarga ni efecto palanca (*prying*). El manual NASA sustenta el método de grupos de fijación y la necesidad de combinar cargas; no aporta propiedades de nuestra unión. [NASA RP-1228](https://ntrs.nasa.gov/api/citations/19900009424/downloads/19900009424.pdf).

Con arandela Ø7/3,4, la presión media de servicio para 64,98 N es ≈2,21 MPa. Sumando **100 N de pretensión hipotética**, sería ≈5,61 MPa. Ese valor de pretensión es una sensibilidad, no una instrucción de apriete.

La contraplaca PETG tiene 3 mm. Para mostrar el efecto de un apoyo imperfecto se usa una tira en voladizo supuesta de ancho 20 mm, sin pretensión:

| Luz sin apoyo supuesta | Tensión nominal 6FL/(b t²), con F=64,98 N |
| --- | ---: |
| 5 mm | 10,83 MPa |
| 10 mm | 21,66 MPa |
| 20 mm | 43,32 MPa |

Estas luces no fueron medidas ni deducidas del contacto del CAD. La tabla muestra por qué una contraplaca debe apoyar de forma continua: no autoriza a dar por resistente una placa de 3 mm solo porque los tornillos metálicos tienen poca carga. El punzonado medio ideal alrededor de Ø7 sería ≈0,98 MPa por servicio, o ≈2,50 MPa con la pretensión ilustrativa.

Se recomienda estudiar reparto metálico y limitadores de compresión en las piezas impresas, manteniendo la protección del acrílico y las fijaciones bilaterales. Espesores, agujeros y apriete deben definirse con el contacto y las piezas reales; no se presupone que el acrílico tenga una resistencia conocida.
