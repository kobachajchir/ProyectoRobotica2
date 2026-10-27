# AFR2-20261026-06 — Pinza y muñeca corregidas con revisión de movimiento CAD

26/10/2026. El usuario detectó ruedas dentadas cruzadas en la pinza y pidió expresamente corregir choques y revisar apertura/cierre, muñeca y límites de los servos. Este pedido autoriza los análisis geométricos que la ficha05 dejó pendientes. No autoriza ni acredita un ensayo físico del brazo.

## Cambio y resultado

Se conserva el maestro v01 y se crea `AFR2_Brazo_PVC32_Prototipo_v02.f3d`. Pinza con un piñón evolvente24T módulo1 entre dos cremalleras opuestas y guías con retención de carros. J7 arrastra ambas mordazas mediante vínculos de movimiento nativos. Apertura medida en el ensamble:50,000 mm a109,8° de servo,60,011 mm a133,7° y79,405 mm a180°. El cierre inferior a50 mm se ensaya vacío; con el cubo se detiene en el contacto.

J6 se sustituye por un acople directo con ejeØ5 y rodamiento625. Se reorienta la abrazadera proximal del antebrazo fuera del plano de flexión del codo, se despejan alojamientos y pasos completos de ejes, y se integran los volúmenes de cada soporte impreso. Se limitan explícitamente los cortes a su pieza para conservar completas las envolventes de los siete motores.

Se conservan los eslabones130/120 mm y los cortes PVC80/70 mm. Para separar carcasas y barridos, la base pasa a69+3 mm de alto, el hombro a185 mm sobre mesa y las separaciones J4–J5/J5–J6/J6–TCP a55/65/78,5 mm. La reducción de hombro usa20T/40T, relación2:1. Son cambios reales de tamaño respecto de la propuesta05.

## Evidencia y límites

Con los motores completos se repitió el muestreo final:2902 poses, sin penetraciones superiores a0,001 mm³ entre conjuntos. Incluye181 posiciones de pinza vacía cada1°,2197 combinaciones de muñeca cada15° con cubo sujeto y524 posiciones de barridos individuales cada2° con extremos incluidos. Se accionaron además mínimo/centro/máximo de cada eje en Fusion (18 casos) y nueve posiciones de ida y vuelta de pinza; las matrices de la cadena y del ensamble nativo coinciden dentro de la precisión numérica registrada. Análisis nativo de la pose de presentación y revisión de montajes impresos sin penetraciones reportables.

Los otros ejes se mantuvieron en las posturas de referencia documentadas. El muestreo no prueba todas las trayectorias continuas ni todas las combinaciones de hombro/codo/muñeca. Esas combinaciones requieren planificación para evitar autochoque y mesa; no se implementó un planificador ni una protección en firmware. Tampoco se probó fuerza de sujeción100 g, torque, corriente, impresión ni montaje físico. Las interfaces comerciales conservan detalles de prototipo.

## Posición y módulos

Se mantienen servos posicionales y se agregan cero sensores externos. PWM significa ángulo solicitado al control interno del servo, no una medición recibida de la posición física. No se cambia protocolo ni se inventa telemetría de posición.

| Módulo | Estado de esta ficha |
| --- | --- |
| Atmega | Sin cambios ni compilación/flash |
| ESP | Sin cambios ni compilación/flash |
| Web/OLED | Sin cambios ni build |
| CAD | v02 guardada en Fusion/F3D, parámetros y componentes editables; revisión geométrica muestreada autorizada |
| Contracts | Registro de cambio, alcance y evidencia; sin cambio wire |

Sin commit ni push solicitados. Se preservan los cambios previos de KiCad, Gerber y Documentacion ajenos a este trabajo.

[Guía de edición, rangos y resultados](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v02/README.md) · [Manifest](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/modelo_prototipo_v02.json) · [Informe de2902 poses](../../AutoFinalRobo2-CAD/mecanica/brazo-antropomorfo/revision_v02/informe_movimiento.json).
