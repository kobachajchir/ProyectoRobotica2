# Prediseno liviano v13: aluminio, PVC menor y PLA localizado

AFR2-20261028-09. Continuacion autorizada de la busqueda de actuadores. **Es un predimensionamiento; no un brazo nuevo modelado ni una capacidad fisica aprobada.** El maestro Fusion v09 sigue guardado y sin modificar. No se hicieron tests de software, compilaciones, flasheos, FEA ni ensayos.

[Resultados numericos](RESUMEN_NUMERICO.md) · [Componentes del candidato](candidato_compacto_componentes.json) · [Cargas](resultados_liviano.json) · [Secciones](secciones_preliminares.json) · [Fuentes](FUENTES.md)

## Por que cambia la arquitectura

La opcion de actuadores RS01 resolvia parte del par trasladando el problema al peso. Se descarta como direccion de esta iteracion. La nueva propuesta aprovecha aluminio en placas/bridas, acero en ejes y fijaciones, PVC mas pequeno y PLA PrintaLot para manguitos, carcasas abiertas y apoyos locales. No se reemplazan ejes ni tornillos por aluminio sin comprobar su funcion.

En v09 hay aproximadamente **1015 g de PLA, 437 g de acero y solo 38 g de PVC**. Reducir el tubo por si solo no produce el ahorro principal. Se eliminan los grandes bloques impresos de plataforma, torre y horquillas; se proponen placas atornilladas y bosses locales. Las ventanas no pueden cortar asientos, ligamentos resistentes, bandas de carga ni accesos de montaje.

La pinza se conserva a partir de sus cuerpos v09, incluidos mordazas y pinon, con sus masas, centroides y tensores. Se presupuesta reemplazar su MG90S por MG92B posicional y adaptar el alojamiento. **Ese reemplazo no esta modelado:** sus dimensiones, horn y spline deben contrastarse. Evita inventar una pinza artificialmente mas corta o liviana para mejorar el calculo.

## Dimensiones e interfaces propuestas

Fusion confirma link1=130, link2=120, wrist45=55, wrist56=65 y grip_deck=36 mm en v09. El offset hasta el centro de la carga es grip_deck+52=88 mm. El hombro nativo esta a209 mm sobre la referencia del auto.

Se comparan los eslabones nativos130/120 con alternativas100/85 y **90/75 mm**. Se mantienen los offsets distales55/65/88. En el candidato menor el alcance horizontal desde el hombro al centro de la pieza cambia de **458 a373 mm**, una reduccion explicita de85 mm. El estudio no disfraza este cambio como conservacion del alcance anterior. Los manguitos de25mm dejan12.5mm de insercion presupuestada en cada extremo; esa insercion aun requiere comprobar apriete y contacto.

La base centrada propuesta tiene ejeY=-32 mm, platoZ=60 mm y hombroZ=100 mm. Son cotas de prediseño, inferiores a v09; no se resuelve el radar levantando todo el brazo. El motor de base se desplaza lateralmente. Su compatibilidad con Mega, cableado, radar y soportes requiere CAD nuevo y barrido; las posiciones de este presupuesto todavia no lo prueban.

Se presupuestan tubos OD25/ID22 y OD20/ID17 mm, pared1.5 mm. Hay oferta argentina de PVC soldable20/25, pero **los interiores exactos de la variante a comprar siguen pendientes**. No confundir PVC con PP-R de termofusion, ni diametro nominal con exterior. Parametrizar OD/ID medidos antes de fabricar los manguitos. La ficha Tigre Argentina localizada no pudo recuperarse porHTTP403; no se atribuyen a ella esos espesores.

## Actuacion comparada antes de nuevos housings

| Eje | Candidato compacto | Transmision externa |
|---|---|---|
| J1 | GX3270BLS,90g | 1:1,44/44T, desplazado |
| J2 | GX3270BLS,90g | 1.9:1,20/38T |
| J3 | GX3270BLS,90g | 1.4:1,20/28T |
| J4 | MG92B,13.8g | 0.85:1,40T motor/34T salida, fuera del PVC |
| J5 | GX3270BLS,90g | Directo |
| J6 | MG90S,13.4g | Directo |
| Pinza | MG92B,13.8g | Pinon24T y dos cremalleras existentes |

Los GX se filtran con **55kgfcm a6V**, no con70 a8.4V. El objetivo exige definir una alimentacion que mantenga esa tension minima bajo carga; no se prueba por tener bateria2S. Los77g de control/regulacion,90g de bateria y20g de cables siguen siendo reservas de compra, sin cierre de potencia/corriente ni nuevas conexiones implementadas. Nano/PCA9685 corresponden a un controlador separado propuesto; no se modifica el bus I2C del firmware actual del auto.

Se suponen eficiencias0.85 en las transmisiones externas. La primera reduccion2:1 del hombro exigia270 grados y no dejaba margen ante la tolerancia comercial+-10. Se corrige a1.9:1: requiere135x1.9=256.5 grados frente a260 inferiores de catalogo, dejando3.5 grados totales de margen de entrada. J3 requiere180x1.4=252. J4 usa una sobremarcha0.85:153 grados del MG92B producen180 grados de articulacion, dentro de los160 de su ficha posicional; pierde torque, incluido en el calculo, y ahorra41.2g frente al MG946R provisional. Son filtros nominales, no calibracion fisica. Las prestaciones publicadas son de bloqueo/sin carga, no pares continuos ni curvas de frenado certificadas. Deben confirmarse variantes posicionales de MG90S/MG92B y sus extremos.

## Metodo de cargas y cambios resistentes

Se conservan200g, seis ejes a30 grados/s y frenada suave de50ms. El perfil cubico de velocidad tiene aceleracion maxima900 grados/s2. Se usan5502 estados de262 trayectorias con final extendido y19 orientaciones de base, descartando los instantes fuera de limites de J1. Se incluyen gravedad, traslacion de centroides, inercia de cuerpos y acoplamientos. Los cuerpos nuevos usan envolventes uniformes aproximadas; la pinza conserva tensores de CAD. No se incluyen inercias internas desconocidas de estos servos ni elasticidad, impactos, backlash o control real de frenado.

La frenada instantanea no tiene esfuerzo finito definido sin tiempo/distancia/flexibilidad. Tampoco esta demostrado que esta familia de finales extendidos sea la envolvente absoluta de todas las poses y trayectorias posibles. Los maximos de fuerzas y momentos tabulados pueden ocurrir en estados diferentes.

La primera iteracion de secciones rechazaba ejes de5 mm en codo y J4. Se conserva ese antecedente en[secciones_iteracion_inicial.json](secciones_iteracion_inicial.json). Se proponen ahora **8 mm en codo,608;6 mm en J4,626-2Z**, con asientos asociados. J1/J2 son8 mm, J5/J6 permanecen5 mm bajo este filtro. Son ejes macizos: ranuras, planos, taladros y acoples pueden cambiar el resultado. Se requiere acero con certificadoRp>=600 MPa, no hierro generico.

El presupuesto incluye explicitamente eje de hombro8x46mm y J5 de44mm para atravesar los apoyos completos. La brida de base pasa a taladro nominal8mm con hub integral, evitando presupuestar una bridaID12 desconectada de eje8. La union de torque al hub, su retencion axial y las tolerancias todavia requieren diseño; un contacto nominal entre superficies no lo acredita.

Las placas se calculan por flexion del eje debil, con ligamentos netos supuestos tras agujeros, no usando ficticiamente la inercia de una placa maciza sin ventanas. Torre/hombro4mm, codo4mm, horquillaJ5 2.5mm y montura4mm; se requiere conservar bandas continuas. Se expresan resistencias efectivas minimas con n=2 y Kt=2 provisionales. Aluminio con Rp>=240 MPa es especificacion de compra, no propiedad atribuida a cualquier planchuela. El caso nativo largo falla otros filtros y no se adopta por su aspecto.

Los tubos se calculan con carga axial, corte, flexion y torsion combinadas de forma conservadora. Se publican requisitos, sin inventar resistencia efectiva al PVC o al PLA. Faltan aplastamiento, apriete, creep, ovalizacion, tornillos, retencion de rodamientos, contactos, rigidez y placa acrilica del auto. Los nuevos alojamientos aun no existen y no heredan el resultado geometrico de v09.

## Estabilidad: aligerar no basta

Se usa la masa de auto aproximada **1.5kg aportada por el usuario**, con CG centrado supuesto y auto detenido. El manual muestra245 mm exteriores, placa155x200 y ruedas80; no entrega separaciones exactas de contactos con el piso. No se toma el ancho exterior como trocha efectiva.

El CAD de referencia sugiere centrosX=+-107 yY=-32+-62 mm; su posicion es una inferencia del modelo. Se extrajeron superficies de52 cuerpos de rueda en Fusion. Los cuerpos de disco llegan unos0.49 mm mas abajo que ciertos rodillos: esa malla **no define un poligono de apoyo fisico fiable**. Tambien difiere el ancho exterior del modelo frente al manual. Se conservan las evidencias, sin presentarlas como mediciones del auto real.

El candidato compacto no supera el filtro de vuelco sobre el rectangulo de ejes inferidos. Por eso se estudian **cuatro pies desplegables para manipular con el auto detenido**, con reserva250g y huella hasta320x320 mm. Son una alternativa propuesta, no una condicion ya aceptada/fabricada. Se recoge la carga con apoyos desplegados; el AGV transportaria con brazo recogido y pies retraidos. No se declara estable el transporte o frenado del auto por ese calculo.

La utilizacion de vuelco<1 solo indica que el centro de presiones calculado cabe en el rectangulo supuesto. Friccion de traslacion y de guiñada se publican como requisitos necesarios separados; no equivalen a resolver todas las fuerzas de contacto simultaneamente. Los pies, sus bloqueos y la transferencia de carga al chasis necesitan diseño y resistencia propios.

## Traccion del auto

[Calculo](calcular_traccion.py),[sensibilidad](traccion_sensibilidad.csv) y[resumen](traccion_resumen.json). La ficha local del GM25-370-24140-75-14.5D10 da3kgfcm nominales a9V,100rpm y1.2A maximos. La variante instalada sigue por confirmar. Los9.5kgfcm de bloqueo no se usan como capacidad continua.

El limite ideal de cuatro ruedas80mm es29.42N en un eje y20.80N en diagonal. La sensibilidad considera7.4V de bateria, caida resistiva del puente, transferencia par/tension hipotetica y eficiencia de rodadura0.6. No es una curva medida del conjunto. Con aceleracion0.5m/s2,Crr0.05 y masa rotante equivalente1kg, se comparan masas totales2.8/3.90/7.319kg. Los2.8kg son una meta de comparacion, **no la masa del candidato con pies**; su resultado propio se agrega al resumen numerico.

No se deduce una carga maxima homologada en kg a partir del par. Faltan bateria bajo carga, corriente/temperatura, suelo, pendiente, direccion y aceleracion. La capacidad de traccion y el vuelco del brazo son verificaciones diferentes.

## Estado y reproduccion

El candidato compacto se evalua por par maximo bajo las hipotesis documentadas; eso no acredita par continuo, freno real ni resistencia de todo el robot. No se recomienda compra ni se comienza su housing como seleccion cerrada: faltan calibracion de recorridos, curvas/limites de servos, inercia interna, agarre real, apoyos y uniones/FDM. El objetivo sigue activo.

Fuentes editables de calculo y JSON se versionan. Reproducir, con Python+numpy, en orden: `reutilizar_pinza_v09.py`, `prediseno_liviano.py`, `calcular_liviano.py`, `calcular_secciones.py`, `calcular_traccion.py`, `redactar_resumen.py`. `calcular_liviano.py` requiere el NPZ de estadosv11 enCAD/exports, regenerable mediante la cadena documentada en[busqueda v11](../busqueda_actuadores_v11/README.md); exports se excluye de Git. El script `leer_apoyos_fusion.py` fue ejecutado en Fusion en modo lectura. El analisis matematico es el expresamente solicitado; no se ejecuto una bateria de tests del repositorio.

`prediseno_componentes.json`/`presupuesto_masa.json` son la plantilla de primitivas140/120, anterior a las sustituciones por variante. El presupuesto completo del candidato90/75, incluidos los motores/transmisiones corregidos y pies, esta en`candidato_compacto_componentes.json` y`candidato_masas_CG.csv`; no confundir las masas de ambos archivos.
