# 3. Rediseño que fundamentan las comprobaciones

[Índice](README.md) · [Uniones y materiales](02_uniones_y_materiales.md)

**Estado:** propuesta dimensional con comprobaciones parciales de espacio y carga. No se modificó la geometría nativa v08 ni se creó un nuevo ensamble aprobado. Las dimensiones siguientes deben incorporarse juntas y recalcularse; no son sustituciones independientes listas para fabricar.

## Actuador J1

El objetivo preliminar para el caso individual de 30°/s–50 ms con margen ×2 es **32,55 kgf·cm**. El GX3270BLS 270° ya modelado para el codo es un candidato compatible con una selección comercial argentina documentada y anuncia 63 kgf·cm a 7,4 V. El cociente de ese anuncio frente al pico calculado es ≈3,87; no constituye una curva continua o capacidad de frenado acreditada.

Su alimentación debe revisarse con el codo: dos unidades publican hasta 7 A de bloqueo cada una a 7,4 V. Son hasta 14 A anunciados para ambas, además de otras cargas; no se aprueba la batería o los XL4016 existentes por hacer esta suma. PWM de posición no impone por sí solo un límite de torque.

Con el MG946R actual, y manteniendo el mismo modelo ideal individual de 30°/s, el tiempo que igualaría su anuncio sería ≈62,6 ms sin margen, o ≈125,2 ms con margen ×2 respecto del anuncio. **No son tiempos configurados o aprobados**: faltan capacidad continua, transitorios y ejecución real. Tampoco resuelven el resto de las uniones.

## No elevar la montura: estudiar bajar el motor

Se probaron cuerpos BRep temporales contra 680 cuerpos del auto, electrónica, fijaciones conservadas y pan. No se movieron ocurrencias reales ni se guardaron cambios. Se excluyen el motor viejo y las piezas J1 que habría que rediseñar.

| Alternativa temporal | Resultado |
| --- | --- |
| MG946R actual, sin mover | Sin penetraciones contra los cuerpos incluidos |
| MG946R 8 mm más bajo | Interferencia de 18,91 mm³ con la cabeza del anclaje C; cuerpo queda1,1 mm sobre el plano superior de la montura |
| MG946R 16 mm más bajo | Cruza la montura e interfiere con casquillo central/M10 y C; se descarta como desplazamiento simple |
| GX3270BLS, cara del cuerpo Z=56 mm | Cuerpo desdeZ=14,3 mm; interferencia de 4,37 mm³ con la cabeza actual C |
| Mismo GX y C con cabeza avellanada al ras Z=12 mm | Sin penetraciones reportables contra los cuerpos incluidos |
| GX con cara Z=58 mm | Sin penetraciones en ese estudio parcial, incluso con cabeza actual C |

La alternativa de cara Z=56 mm permite estudiar un apoyo inferior más bajo conservando plato aZ=96 mm y hombro aZ=209 mm. La nueva fijación C se representó como una envolvente comercial avellanada, no cortando el tornillo existente. Para la referencia ISO10642 M3 de Böllhoff se reservan cabeza máxima Ø6,72 y altura 1,86 mm; una DIN7991 comprada puede tener otras cotas y debe usarse su ficha.

El alojamiento debe incorporar el avellanado y conservar sección suficiente en la placa. La cabeza al ras elimina el choque de ese volumen, pero **no demuestra por sí sola** que el nuevo agujero o montaje soporte la carga. [Resultado íntegro](espacio_bajar_motor.json).

## Eje y apoyos

Se propone estudiar **eje Ø10 mm con acero identificado**, dos 6000-2Z/ZZ (10×26×8 mm) y **16 mm entre centros**, con el apoyo inferior situado aproximadamente 8 mm por debajo del actual y sin elevar la plataforma. La serie 6000 es comercial en Argentina; debe identificarse la variante, juego y proveedor reales antes de cerrar compra.

El predimensionado de sección circular Ø10 reduce la referencia nominal equivalente a≈46,33 MPa y el requisito provisional de fluencia a≈185 MPa, frente a≈377 MPa del eje Ø8 con plano/taladro estudiado. Un acero con Rₚ₀,₂ identificado de al menos250 MPa sería un punto de partida **para ese predimensionado**, sin aprobar muescas, brida, taladros, chavetas, contacto o fatiga.

La distancia de 16 mm reduce aproximadamente a la mitad el par ideal de reacciones:565→283 N con la carga v08 sin actualizar. Al cambiar de 608 a 6000 también cambian ancho, hombros, tapas, diámetros de contacto y masa; no se traslada automáticamente el cálculo del608 al conjunto nuevo.

El estudio de espacio del motor **no comprueba** el nuevo eje, horn, apoyos, tapa, cuna de cuatroM3 ni su movimiento. Deben conservarse juego entre piezas móviles y fijas y retención axial por hombros/tapas, sin cargar axialmente el servo de forma indeterminada.

## Repartir el par con brida metálica

La referencia de estudio es:

- Brida Ø60 mm, espesor metálico inicial 3 mm, **cubo centralØ20 mm**, ligada positivamente al eje.
- CuatroM3 pasantes en círculo de paso Ø50 mm, a 45° respecto de los ejes del plato.
- Arandelas/tuercas y limitadores de compresión en el PETG; no confiar el par a una estría impresa ni únicamente a la cara D.
- Espesor efectivo PETG 5,5 mm para la estimación de presión de agujero; requiere definir el alojamiento de la brida en el plato y comprobarlo.

El reparto de referencia de esa brida da **≈16,88 N de cortante por tornillo y ≈62,58 N de tracción máxima**, en estados distintos, con presión nominal de agujero ≈1,02 MPa. Incluye fuerza transversal además del torque; el reparto simplificado por torque solo daría15,96 N por tornillo.

La placa metálica también necesita comprobación. Una idealización de cuatro sectores en voladizo, con 63 N por sector, raíz de radio 10 mm y placa3 mm, da ≈40 MPa de flexión; con n=2/Kt=2 requiere ≈160 MPa de fluencia. Sin ese cubo, tomando raíz de radio 5 mm, la referencia supera 100 MPa nominales y requiere más de 400 MPa con los mismos factores. **Una chapa fina grande conectada solo al extremo del eje no se considera automáticamente suficiente.**

La unión cubo/eje, su radio de raíz, posible soldadura, fijaciones, precarga y agujeros siguen abiertos; no se asigna al acero una resistencia por apariencia. Los tornillos y la brida deben caber sin tocar la tapa fija del cartucho durante todo el giro.

## Reacción del motor y unión al chasis

El servo nuevo no debe descansar sobre los dos postes estrechos actuales como única ruta resistente. Se propone estudiar una cuna cerrada y/o escuadras metálicas que unan sus cuatro pestañas a apoyos distribuidos y tornillería pasante. La pieza impresa puede posicionar y proteger el actuador, mientras se reparte la reacción de par evitando concentrarla en una raíz pequeña.

Los nervios deben terminar con transición y unión suficiente al puente; el posterior izquierdo debe alinearse con su columna. Mantener los seis anclajes laterales, el central y la contraplaca inferior única. Estudiar arandelas de mayor reparto, apoyo continuo y limitadores en PETG; no dar por empotrado todo el acrílico.

El caso de bloqueo del motor elegido también debe dimensionarse. El anuncio ≥63 kgf·cm no es un límite superior probado; la sensibilidad ya muestra que los postes originales no se pueden conservar por aumentar solamente el torque disponible.

## Cierre de la siguiente iteración

La secuencia que resulta del estudio es: definir eje/brida/horn y materiales comerciales; modelar juntos el J1 bajo y sus fijaciones; recalcular masa e inercias; repetir contactos, apertura y barridos; y analizar localmente asientos, raíces, placa/acrílico e insertos con material/proceso definido. El objetivo sigue siendo sostener200 g y soportar una frenada finita identificada, no una parada instantánea sin límite de tiempo o rigidez.
