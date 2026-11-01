# Brazo compacto: reducir primero masa y alcance

AFR2-20261029-01, 29/10/2026. **Predimensionamiento nuevo, no CAD modificado ni resistencia aprobada.**

Se mantienen 200 g, seis ejes a 30 grados/s y frenada finita de 50 ms.
Por decision humana se admiten importaciones con envio a Argentina y el auto
debe trabajar exclusivamente sobre sus ruedas. Los pies de v13 y los estudios
iniciales de v14 quedan descartados. El voladizo sobre ATmega tampoco se adopta:
agrega soporte sin descargar la punta. No se agrega lastre ni se traslada la
electronica como supuesto ahorro del brazo.

## Que se cambia y por que

El maestro v09 pesa 2200.43 g sin carga: aproximadamente 1015 g de PLA,
437 g de acero y apenas 38 g de PVC. Afinar solamente el tubo deja intacta
la mayor parte del problema. El nuevo presupuesto reemplaza bloques impresos
por mejillas de aluminio de 2 mm con bordes plegados de 8 mm, horquillas de
1.5 mm, encastres abiertos de PLA PrintaLot y ejes de acero conservados donde
transmiten torque. Los bordes deben comprobarse como perfiles abiertos;
no se les atribuye automaticamente la rigidez de una caja cerrada.

Se comparan eslabones mas cortos, PVC exterior/interior 25/22 y 20/17 mm
(producto comercial exacto pendiente), muneca 35+30 mm y distancia J6-centro
de carga de 45 mm. El hombro queda a 85 mm sobre la referencia superior del
auto. Son cotas propuestas, no medidas de piezas existentes. La altura debe
revisarse contra electronica y barrido del HC-SR04 antes de adoptarla.

En la alternativa extrema 55/50 mm los tramos de PVC quedan en 15/10 mm:
funcionan casi como separadores entre articulaciones. Esto cuestiona la
conveniencia constructiva del PVC a esa escala; no se declara resuelto un
eslabon tubular ni sus encastres. La opcion 80/70 conserva tramos 40/30 mm,
pero exige mas apoyo lateral del auto.

La pinza nueva se presupuesta pivotante, con dedos de aluminio de 2x10x45 mm,
palma con ventanas y accionamiento ligero. Se elimina la propuesta previa
de palma grande, cremalleras metalicas, guias y multiples rodamientos.
**No es una pinza terminada:** apertura exterior de al menos 60 mm, contacto
en el cubo de 50 mm, transmision, resortes, precarga, pads y recorrido real
siguen pendientes de dimensionar. Sus reservas de masa no prueban agarre.

## Masa y dimension resultantes

Se cuentan bateria 130 g y control/potencia 150 g adicionales al auto de
1.5 kg; no se presupone alimentacion compartida gratuita. Los siete servos
suman 254.4 g: tres ST-3215-C047 de 56 g y cuatro STS3032 de 21.6 g, incluida
la pinza. Frente a 375.2 g de servos de v09, la reduccion es 32.2 %.

| Eslabones mm | Reserva estructural | Alcance hombro-cubo mm | Brazo sin carga g | Con 200 g g | Margen externo minimo |
|---|---|---:|---:|---:|---:|
| 80/70 | Sin reserva | 260 | 1059.4 | 1259.4 | 2.61 |
| 80/70 | 25 % | 260 | 1190.6 | 1390.6 | 2.46 |
| 65/60 | 25 % | 235 | 1179.3 | 1379.3 | 2.38 |
| 55/50 | 25 % | 215 | 1170.3 | 1370.3 | 2.65 |

La reserva del 25 % se aplica a estructura, rodamientos, transmisiones,
cables y fijaciones; no a carga, servos, bateria ni electronica. No es una
medicion ni una garantia de masa final. Las cajas de inercia y CG locales
propuestos de cada componente estan en el JSON, junto con centroides/tensores
de cada grupo. Cada grupo se transforma con su propio eje de articulacion.

Desglose de la alternativa 55/50 con reserva:

| Material o funcion | Masa g |
|---|---:|
| Acero | 76.1 |
| Al6061 | 142.7 |
| Carga | 200.0 |
| Electronica | 150.0 |
| PLA | 105.0 |
| PVC | 4.6 |
| Potencia | 130.0 |
| Rodamiento | 127.5 |
| Servo | 254.4 |
| Transmision | 121.2 |
| Varios | 58.8 |

## Actuadores y cargas

El filtro usa 10 kgf cm nominales a 12 V para ST-3215-C047 y 1.5 kgf cm
a 6 V para STS3032, no sus pares de bloqueo como si fueran continuos.
Se compara con el menor entre nominal y una recta par-velocidad supuesta,
con bloqueo/speeds inferiores de ficha y eficiencia de reduccion 0.8.
Esta recta de accionamiento **no caracteriza frenado regenerativo ni demuestra
capacidad de parada**. Tampoco incluye inercias internas de motor/reductor.

Se calculan 684 trayectorias y 14364 estados por alternativa: 64 combinaciones
de signos de seis ejes y 12 movimientos individuales, nueve orientaciones
de J4/J6. Todas terminan extendidas. Una ley cubica de velocidad lleva
30 grados/s a cero en 50 ms; pico de desaceleracion 900 grados/s2. Se incluyen
gravedad, aceleracion del CG, inercia del cuerpo y termino giroscopico.
La posicion extendida no garantiza el maximo global para todo componente,
ni este conjunto muestreado envuelve cualquier trayectoria del workspace.

Se elige la menor relacion sencilla que supere 2.3 de margen externo,
conservando las reservas de transmision incluso donde queda accionamiento
directo. No se declara que esas reducciones ya quepan o sean resistentes.

Alternativa 55/50 con reserva, picos no necesariamente simultaneos:

| Eje | Reduccion propuesta | Par maximo de salida kgf cm | Margen externo | Fuerza resultante N | Momento resultante Nm |
|---|---:|---:|---:|---:|---:|
| J1 | 1:1 | 3.274 | 3.05 | 11.618 | 1.409 |
| J2 | 5:1 | 14.076 | 2.84 | 9.955 | 1.404 |
| J3 | 3:1 | 9.052 | 2.65 | 8.163 | 0.904 |
| J4 | 1:1 | 0.275 | 5.46 | 6.558 | 0.542 |
| J5 | 8:1 | 3.277 | 2.93 | 5.583 | 0.328 |
| J6 | 1:1 | 0.121 | 12.37 | 4.847 | 0.180 |

La reduccion J2=5 exige 675 grados de entrada para 135 de articulacion;
J3=3 exige 540 para 180 y J5=8 exige 1440 para 180. Requieren modo multivuelta,
gestion de referencia al encender y cableado compatible. La familia admite
multivuelta pero no conserva el numero de vueltas al perder alimentacion.
Se trata de servos TTL, distintos del control PWM anterior; no se modifica
firmware ni se da por integrada esa interfaz.

La inercia interna maxima necesaria para conservar x2 se guarda por eje en
`internal_inertia_necessary_max_kgm2`: procede del margen residual dividido
por n2*eta*aceleracion. Es una cota necesaria aislada, no una aprobacion;
faltan reacciones internas sobre articulaciones anteriores y datos reales.

Resultantes en la seccion local (Z longitudinal), incluyendo flexion y torsion:

| Seccion | Corte N | Axial N | Flexion Nm | Torsion Nm |
|---|---:|---:|---:|---:|
| J1 | 1.7793 | 11.5280 | 1.3866 | 0.3211 |
| J2 | 9.9551 | 0.2470 | 1.4028 | 0.0655 |
| J3 | 8.1626 | 0.2660 | 0.9036 | 0.0318 |
| J4 | 6.5577 | 0.2215 | 0.5410 | 0.0269 |
| J5 | 5.5828 | 0.2361 | 0.3276 | 0.0140 |
| J6 | 4.8465 | 0.2069 | 0.1804 | 0.0119 |

Estos esfuerzos son entradas para dimensionar nervios, ejes, dientes,
asientos, fijaciones y anclajes al acrilico. No hay analisis de tensiones
de esta geometria nueva, FEA, comprobacion de PLA FDM ni ensayo fisico.
Hacer mas fina una pieza no demuestra que resista; las cifras de par tampoco
aprueban el perfil abierto de aluminio, el PVC o las uniones.

## Estabilidad exclusivamente sobre ruedas

Se gira la familia de frenadas entre -90 y +90 grados de J1 en pasos de 10,
descartando estados fuera de limites J1, y se calcula la proyeccion dinamica
de la resultante sobre piso supuesto a Z=-80 mm. Se mantiene el auto parado,
de 1.5 kg y CG horizontal centrado como hipotesis. No incluye frenada del auto,
pendiente, deformacion/contacto de rodillos ni adherencia Mecanum.

| Eslabones | Reserva | Semiapoyo minimo X/Y mm | Utilizacion sobre rectangulo hipotetico 180x100 mm |
|---|---|---|---:|
| 80/70 | No | 58.69/59.88 | 1.198 |
| 80/70 | 25 % | 60.25/61.28 | 1.226 |
| 65/60 | 25 % | 51.82/52.87 | 1.057 |
| 55/50 | 25 % | 45.36/46.42 | 0.928 |

55/50 pasa solamente ese filtro de vuelco hipotetico: queda cerca del borde,
sin un factor de estabilidad robusto. Necesita aproximadamente 93 mm de
separacion transversal efectiva, y 111 mm si se exige 20 % adicional sobre
la semibase calculada. Un desplazamiento del CG del auto de 10 mm puede
consumir el margen restante. Los 245x200 mm del manual NO son dimensiones
del poligono de contacto. No hay aprobacion de estabilidad sobre ruedas.

## Decision y siguiente iteracion

La reduccion conjunta de estructura, actuadores y longitud merece continuar:
el presupuesto baja aproximadamente a la mitad de v09. La variante 80/70 es
menos extrema para construir; la 55/50 descarga mas el auto, pero deja PVC
demasiado corto y requiere resolver el empaquetado real. Ninguna se declara
seleccion final ni se manda a fabricar.

Antes de sustituir el maestro hay que cerrar pinza/ventaja mecanica, masas
de transmisiones y montajes reales, disponibilidad exacta de STS3032 con envio
a Argentina, inercia/freno, limites multivuelta y espacio de 5:1/3:1/8:1.
Luego debe comprobarse resistencia de secciones/uniones y geometria con
rodamientos y ejes positivos, barrido HC-SR04 y todas las articulaciones.
No se ejecutaron tests de software, builds, flasheos ni validadores CAD;
se realizaron los calculos mecanicos solicitados. Fusion v09 se conserva.

## Reproduccion y antecedentes

`C:/Python312/python.exe compacto.py` requiere Python y numpy. Genera el JSON
desde su propio BOM y trayectorias; no depende de NPZ ignorados ni de Fusion.
`redactar_compacto.py` genera este informe desde ese JSON.

`modelo_dinamico.py` deriva de las ecuaciones de v13 (commit 4b02f65).
`calcular_nominal.py`, `contactos.py` y sus resultados conservan experimentos
previos con pies **descartados**; `sin_pies.py` documenta la primera alternativa
sin pies que seguia pesada y no cumplia. Esos tres scripts dependen de NPZ
regenerables anteriores en exports. Sus resultados no son la seleccion actual.
Las uniones y gripper de `compacto.py` son presupuestos nuevos, no una lectura
del volumen de v09. [Fuentes y disponibilidad](FUENTES.md).
