# Informe de masas, centros de masa y esfuerzos del brazo v17

AFR2-20261029-03. Resultados de la revision identificada porSHA, no de un brazo fabricado.

## Por que se itera el diseno

Se mantiene el antropomorfo y la pinzaV7. Se acortan los ejes a70/60mm,
se usan tubos de PVC de 25 x 1,2 y 20 x 1 mm y se lleva el motor de J3 junto al hombro. Una
correa con poleas de 20 y 50 dientes transmite hasta el codo, sin cruzar el giro axial J4.
El cambio se decide por masa y palanca, incluyendo el peso de los
actuadores nuevos antes de modificar cada alojamiento.

El motor de J3 esta a 19,533 mm sobre J2. Comparando el mismo
motor de 75,5 g situado a 70 mm y a 19,533 mm, su contribucion gravitatoria
al par de J2 con el brazo horizontal baja aproximadamente de 0,0518 a 0,0145 Nm.
Esta comparacion aislada no sustituye el calculo del conjunto: la
polea, correa, soportes, ejes, herrajes y sus inercias tambien cuentan.

Los tubos pesan aproximadamente5.46 y2.47g. El grueso de la masa
permanece en piezas impresas, metales, motores y base; hacer solamente
los tubos mas finos no resuelve por si solo el peso total.

## Modelo y masas actuales

CAD actual sin pieza: **2352.3g**. 
Propuesta con actuadores/reservas restantes: **2495.4g**, sin pieza.
La pieza de referencia mide 50 x 50 x 50 mm y se calcula con 200 g.
No se incluye el auto ni las referencias80/81/82 en esas masas.

Actuadores nativos: J3, J6, pinza, J2.
Montajes nuevos pendientes: J1, J4, J5.
No tomar las reservas como piezas ya construidas.

| Grupo actual | Masa g | CG local X,Y,Z mm |
|---|---:|---|
|01_Giro_base|575.65|2.85, 27.83, 45.99|
|02_Brazo_PVC|449.17|-3.86, -37.03, 22.19|
|03_Antebrazo_PVC|229.49|4.26, -15.77, 19.00|
|09_Pinion_hombro|25.65|-0.01, 62.89, 0.01|
|04_Giro_muneca|191.12|5.49, 30.65, 41.85|
|05_Inclinacion_muneca|104.31|1.97, 2.82, 22.81|
|06_Herramienta|102.03|1.90, -0.03, 25.15|
|07_Mandibula_izquierda|9.73|4.78, 5.90, 65.78|
|08_Mandibula_derecha|9.73|-4.78, -5.90, 65.78|
|10_Pinion_pinza|4.36|0.00, -0.00, 0.89|
|11_Pinion_J4|6.65|0.00, -0.00, 1.43|
|00_Base_control|614.06|-11.35, -11.43, 35.00|
|12_Polea_entrada_J3|30.37|0.01, -67.36, 0.00|

LosCG se expresan respecto al marco de cada grupo, no todos respecto
al suelo. Para elCG global se transforman mediante la cadena cinematica
y se ponderan por masa: rG=sum(mi*ri)/sum(mi).

| Material o masa comercial | Total actual g |
|---|---:|
|PLA PrintALot|879.73|
|Acero|554.68|
|Electronica|177.00|
|Aluminio|173.63|
|Servo|158.40|
|Servo FEETECH STS3250|151.00|
|Rodamiento|135.00|
|Servo FEETECH STS3032 A0|43.20|
|Aluminio2024|37.16|
|Rodamiento SKF625-2Z|20.00|
|PVC|7.93|
|Laton|7.84|
|Correa|3.02|
|Almohadilla|2.39|
|Horn polimero|1.35|

PLA macizo rho1.24g/cm3,Al6061=2.70,Al2024=2.78,PVC=1.45 supuesto.
La masa impresa real requiere laminado, perfil y orientacion todavia
no definidos. Motores, rodamientos y electronica usan masas comerciales
o estimadas por ocurrencia; compartir una definicion no divide su masa.
ElCG/inercia de un producto comercial se aproxima distribuyendo su masa
sobre la envolvente, sin representar sus interiores exactos.

## Caso de carga y limites del modelo

Se calcula gravedad, aceleracion lineal, aceleracion angular y termino
giroscopico. Se proyectan fuerzas/momentos mediante trabajo virtual
sobre cada eje. Se incluyen relaciones4 enJ2,2.5 enJ3 y1 enJ4.
El caso base tiene J2=90,J3=0 y variaciones de muneca; se muestrean
1760estados con combinaciones de signos de movimiento.

La frenada usada es una HIPOTESIS finita:30grados/s por eje y50ms con
perfil cubico de velocidad. Su desaceleracion pico es900grados/s2.
No es una parada instantanea ni una aceleracion maxima nominal medida
del fabricante. No se conoce la inercia interna del rotor ni el frenado
real del servo. El muestreo no demuestra un maximo global de6ejes.

## Pares para la propuesta con masas y reservas

| Eje | Actuador | Reduccion | Pico externo Nm | Capacidad estimada Nm | Margen |
|---|---|---:|---:|---:|---:|
|J1|STS3250|1|1.1130|1.4906|1.339|
|J2|STS3250|4|3.7070|5.0210|1.354|
|J3|STS3250|2.5|2.4667|3.1381|1.272|
|J4|STS3250|1|0.7903|1.2553|1.588|
|J5|STS3250|1|0.9825|1.4906|1.517|
|J6|STS3032|1|0.0113|0.1397|12.330|

Capacidades con par NOMINAL16kgfcm a12V paraSTS3250 y1.5kgfcm a6V
paraSTS3032, no con par de bloqueo. Eficiencias0.95directas y0.8en
transmisiones son hipotesis. Margen de preseleccion pedido:1.2.
La relacion par/velocidad se trata por separado en capacidad_actuadores_resultados.json.

## Cargas sobre base y primer eslabon

La fuerza resultante y el momento resultante incluyen cortante, flexion
y torsion. No son solamente el par que tiene que entregar el motor.
Componentes maximas individuales, no necesariamente simultaneas:

| Seccion propuesta | Fuerza resultante maxima N | Momento resultante maximo Nm | max Fx,Fy,Fz N | max Mx,My,Mz Nm |
|---|---:|---:|---|---|
|J1|27.041|3.836|2.437, 4.238, 26.827|1.140, 3.711, 1.113|
|J2|21.203|3.800|20.930, 4.214, 2.625|1.084, 3.707, 0.996|

EnJ1 el eje actuado esZ; Mx/My son flexion que toman eje, apoyos y
montura. EnJ2 el eje actuado esY. No usar el maximo de un motor para
aprobar automaticamente un cartucho, horquilla, manguito o piezaFDM.
La carga de una seccion de tubo requiere cortar el camino de cargas
correcto, incluyendo reacciones de correa y motores: no sustituirla
por un momento articular agregado donde puedan cancelarse cargas.

## Que falta para aprobar piezas

Las comprobaciones anteriores seleccionan actuadores y proporcionan
cargas; no son un analisis de tensiones de las uniones. Se necesitan
condiciones reales de impresionPLA, tolerancias, apriete, interfaces y
propiedades resistentes verificadas. Los materiales nativos marcados
SOLO MASA/SOLO DENSIDAD se excluyen de FEA. ElPLA existente declara
E1500hipotetico, sin resistencia intercapas certificada.

Shape Optimization queda preparado para una pasada posterior, con
interfaces/rodamientos/agujeros preservados. Su resultado requiere
redisenar CAD fabricable y comprobar tensiones por separado. No se
resolvio un estudio en nube. Por indicacion del usuario no se continua
calculando vuelco. La interfazTTL y alimentaciones12V/6V siguen pendientes.

## Trazabilidad

Geometria fuente: `geometria_v17.json`.
SHA256: `5b2874703c33126feb9d28906e3299bd4248df4da91c1c5b062e6b7bd25654b0`.
Calculos: calcular_cad_v17.py y preseleccionar_actuadores_integrados.py.
Revision de geometria: REVISION_MONTAJE_J2.md y JSON brutos asociados.
No se ejecutaron tests de software, compilaciones, flasheos ni ensayos fisicos.
