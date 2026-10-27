# 4. Evolución del diseño y fundamento de las correcciones

[Índice del informe](INFORME_ITERACION_DISENO.md) · [Anterior: tensiones y refuerzos](03_tensiones_preliminares_y_refuerzos.md) · [Siguiente: frenada](05_frenada_y_esfuerzos_combinados.md)

## De un prototipo geométrico a un mecanismo con camino de cargas

La primera versión buscó una arquitectura económica de PVC, piezas impresas y servos comerciales. Su geometría permitía discutir proporciones y disposición, pero todavía contenía transmisiones incompatibles, apoyos incompletos y cuerpos sin una unión fabricable. La evolución posterior no debe describirse como un simple cambio de apariencia: se corrigió cómo se transmite el movimiento y cómo puede viajar una fuerza desde la pinza hasta el chasis.

La ausencia de penetración entre sólidos no demuestra que estén unidos. Una guía puede no chocar y, sin embargo, quedar flotante. Dos elementos pueden tocarse visualmente y no transmitir torsión. Una simulación cinemática puede moverlos juntos mediante una unión virtual sin que exista un eje, un acople o tornillos capaces de producir ese movimiento.

## Registro de las iteraciones realizadas

| Revisión | Problema o requisito | Corrección documentada | Por qué se hizo y qué sigue pendiente |
| --- | --- | --- | --- |
| v01 | Definir proporciones, PVC32, articulaciones rojas y control | Componentes y geometría paramétrica inicial | Punto de partida para discutir diseño; no se había dimensionado la carga |
| [v02](../revision_v02/README.md) | Engranajes de pinza que se cruzaban y conflictos en muñeca | Piñón de 24 dientes con dos cremalleras opuestas; J6 directo y separaciones revisadas | Hacer coherentes apertura/cierre y transmisión; el barrido no detectaba todas las discontinuidades de montaje |
| [v03](../revision_v03/README.md) | Guías flotantes, soportes de motor partidos, ejes interrumpidos y rodamientos faltantes | Guías conectadas a palma, housings integrados en horquillas, ejes continuos, planos de arrastre, retenciones y 12 rodamientos | Crear caminos de carga fabricables; los contactos seguían siendo representaciones CAD |
| v03 | Llevar el brazo sobre el auto y alojar el control | Montura sobre agujeros existentes, apriete central, contraplacas y electrónica vertical | Acercar el prototipo a su aplicación móvil; la primera fijación omitía los anclajes izquierdos |
| [v04](../revision_v04/README.md) | Falta de sujeción izquierda y choque con el giro del HC-SR04 | Anclajes izquierdos y consideración del pan del sensor; solución elevada | Incorporar el movimiento omitido; la elevación general fue rechazada por el usuario |
| [v05](../revision_v05/README.md) | Mantener una base baja sin invadir el giro del sensor | Recorte de montura, J1 retrasado 14 mm, reubicación del motor y electrónica con puentes | Liberar el barrido mediante disposición geométrica; no aumentar la altura de todo el conjunto |
| [v06](../revision_v06/README.md) | Simplificar las dos piezas inferiores y revisar continuidad/movilidad | Contraplaca PETG única, base baja conservada, revisión geométrica documentada | Reducir número de piezas y montaje; la placa impresa de 3 mm sigue pendiente de dimensionamiento resistente |
| [Estudio de 200 g](02_estatica_y_reacciones.md) | Pasar de proporciones a masas, CG y esfuerzos | Presupuesto por cuerpo y estática horizontal | Identificar límites del codo, cargas de apoyos y zonas de refuerzo antes de añadir material |
| v07, propuesta | Incorporar selección de motor y frenada de emergencia | Plan de rediseño por eje y de análisis combinado | Aún no hay un nuevo actuador instalado en CAD ni resistencia dinámica acreditada |

## Pinza y muñeca: continuidad antes de resistencia

Los labios y apoyos de las guías de cremallera deben integrarse materialmente en la palma mediante paredes/postes fuera de la carrera del carro. La fuerza del dedo debe poder pasar a la guía y de esta a la palma. Un labio independiente sin tornillos o unión continua no cumple esa función.

En J4 se completó el eje metálico entre la transmisión y el rotor. Un plano de arrastre complementario, retenciones y apoyos dan sentido mecánico al movimiento solidario. En J6 se mantuvo un eje Ø5 con dos 625 y acople a la herramienta. Las tapas se distinguen de los rodamientos: retienen el aro correspondiente y necesitan dejar libre el giro de la otra parte.

En v06 la apertura virtual documentada es 50,000 mm a 109,8° del servo, 60,011 mm a 133,7° y 79,405 mm a 180°. Es una referencia geométrica útil para alojar el cubo de 50 mm; no determina la fuerza de cierre ni que los dedos retengan 200 g durante una frenada. El capítulo 3 conserva la sensibilidad al rozamiento.

## Montura baja: por qué se descartó elevar todo el brazo

La solución elevada resolvía un conflicto de espacio, pero subía las masas. Para una aceleración horizontal del auto, una masa a altura h produce una contribución al momento de vuelco de magnitud m·a·h; para gravedad, la estabilidad depende de la proyección del CG respecto del polígono de apoyo. Subir el brazo también cambia su inercia respecto de ejes del chasis. Por estas razones el usuario pidió resolver la interferencia mediante el montaje.

La v05, conservada en v06, bajó el hombro de 257 a 209 mm sobre el chasis frente a la v04 y la montura de Z=54…60 a Z=6…12 mm. Retrasó J1 y dejó libre la envolvente del sensor. Esta decisión reduce la altura de las masas frente a la alternativa elevada; no prueba estabilidad con el brazo extendido. Faltan masa/CG del auto, apoyos efectivos y aceleraciones.

Los sensores de línea y el HC-SR04 pertenecen al auto. El giro del HC-SR04 debe coexistir con la montura. El encoder de interfaz y los Hall de ruedas no son sensores de posición de las articulaciones del brazo.

## Contraplaca única: simplificación con una condición resistente nueva

En v06 se unieron la contraplaca central/derecha y la contrabarra izquierda mediante un puente de 12 mm. La pieza tiene 3 mm de espesor, seis pasos M3 y un paso central M10. Se conservan tuercas y arandelas metálicas; el plástico no se representa con roscas resistentes propias.

La simplificación elimina una pieza y alinea los apoyos en un único componente imprimible. Sin embargo, las referencias anteriores eran de aluminio y la nueva es PETG. Por ello no se puede trasladar su rigidez o capacidad de apriete por equivalencia visual. Deben estudiarse flexión, aplastamiento bajo arandela, deformación sostenida y reparto de precarga, incluyendo el acrílico.

## Lo que revela el peso propio

El primer eslabón y el antebrazo no pesan únicamente lo que pesan sus caños. Los dos tubos suman unos 38,1 g, mientras que los 25 sólidos rojos suman 1.045,38 g si se imprimieran macizos. Motores, ejes, rodamientos, horns y fijaciones añaden masa. En el hombro, el cubo aporta 0,898 N·m y el propio brazo 2,013 N·m.

Esto justifica priorizar refuerzos locales y distribución de componentes. Un refuerzo distal puede mejorar una sección y empeorar a la vez el codo, el hombro y el vuelco. La masa de cada modificación debe volver al presupuesto y conservar su ubicación, no sumarse toda en el extremo.

## Límites de las correcciones realizadas

La revisión v06 documentó 3.141 configuraciones CAD sin penetraciones mayores a 0,001 mm³ dentro de sus barridos. No es una cobertura continua de todas las combinaciones. Tampoco acredita ajuste de impresión, contacto real, fuerza de cierre, backlash, resistencia, corriente o temperatura.

Los ángulos del ensamble virtual y las consignas PWM no son mediciones del ángulo físico real. La próxima revisión debe conservar esta distinción y no convertir una mejora geométrica en una declaración de capacidad de carga.

**Conclusión:** las iteraciones v02–v06 corrigieron movimiento, continuidad y disposición. El estudio de 200 g aporta ahora las razones cuantitativas para revisar accionamientos y estructura. La frenada añade un problema dinámico que aún no está calculado.
