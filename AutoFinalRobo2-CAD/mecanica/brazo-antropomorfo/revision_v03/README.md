# Brazo sobre AGV, revisión v03

**Corrección posterior:** la [v04](../revision_v04/README.md) agrega fijaciones izquierdas y considera el giro del HC-SR04. El muestreo de esta v03 trató el auto como fijo y omitió ese movimiento; no demuestra holgura frente al pan del sensor.

**Entrega:** [maestro Fusion editable](../AFR2_Brazo_PVC32_AGV_v03.f3d) y [parámetros](../parametros_prototipo_v03.csv). Documento `AFR2_Brazo_PVC32_AGV_v03`,114 parámetros de usuario,323 componentes incluyendo raíz/referencia del auto,13 uniones y4 vínculos de movimiento. El auto original abierto en Fusion se conserva sin modificar. Las cotas finales y el inventario están en [modelo_v03.json](modelo_v03.json).

27/10/2026. Revisión pedida por el usuario para corregir piezas sueltas, ejes discontinuos y apoyos incompletos, y sustituir la base de mesa por una montura sobre el auto. La v02 se conserva como antecedente. Su muestreo de interferencias no demostraba continuidad física ni montaje de todas las piezas.

## Montura elegida

Se usa el CAD **AutoFinalRobo2** abierto en Fusion como referencia dimensional del auto; el maestro original no se modifica. La foto del usuario se conserva en `reference/brazo-prototipo/auto_superior_usuario_20261027.png`. El chasis superior de ese CAD tiene3 mm de espesor. No se infieren posiciones de tornillos midiendo píxeles de la imagen.

El eje del brazo se sitúa en X=0, Z=−32 mm en el sistema original del auto (Y es vertical). En el documento nuevo, el plano superior del chasis es z=0 y la conversión es `(x,y,z) = (X, −Z−32, Y−26)` en milímetros. La parte delantera libre queda alrededor del brazo; se conservan los módulos, sensores y placas del auto como referencia.

| Anclaje | X del CAD original | Z del CAD original | X local | Y local |
| --- | --- | --- | --- | --- |
| A | 31,051 | −51,739 | 31,051 | 19,739 |
| B | 67,997 | −51,739 | 67,997 | 19,739 |
| C | 31,051 | −10,964 | 31,051 | −21,036 |
| D | 68,418 | −10,964 | 68,418 | −21,036 |
| Paso central | 0,333 | −23,499 | 0,333 | −8,501 |

Los cuatro agujeros son aproximadamente rectangulares; no se reemplazan por un cuadrado perfecto. Los agujeros de referencia tienen diámetro cercano a3,366 mm; se propone tornillería M3 con pasos3,4 mm en el adaptador. El paso central mide aproximadamente29,456 mm y recibe un casquilloØ24 exterior con pasoØ10,5 para un tornillo M10. Estas son cotas del modelo de referencia; deben comprobarse en el chasis real antes de fabricar.

La placa superior de PETG tiene172 ×63 ×6 mm y se eleva22 mm sobre el chasis. Cuatro separadoresM3 definen esa altura. La contraplaca inferior de aluminio propuesta tiene94 ×63 ×3 mm y reparte el apriete por debajo del chasis; el casquillo central transmite la compresión entre contraplaca y placa superior. Se representan tornillos M3×40 y M10×45, arandelas y tuercas. Las tuercas se modelan con envolvente simplificada: no son una biblioteca de tornillería certificada.

El montaje se plantea desmontable. Colocar la contraplaca y el casquillo bajo el chasis, pasar el M10 y los cuatro M3, montar separadores y placa, y apretar de forma repartida. No se prescribe torque de apriete del plástico o chasis: no se calculó su resistencia.

## Continuidad del brazo

Los dos labios internos de la pinza reciben postes en los extremos, fuera de la carrera de los carros. Palma, paredes, labios y apoyos de guías deben formar una única pieza impresa continua. Los soportes de motor que pertenecen a una misma horquilla se integran en ella; las tapas desmontables quedan separadas y con tornillos representados.

J4 usa una rueda integrada al rotor y un eje metálico completo. Se incorpora una sección plana complementaria para transmitir giro, apoyos dobles y retención axial. J6 conserva accionamiento directo, con dos625 y un acople que pasa por ambos; la plataforma de herramienta tiene llave plana y prisionero. Los núcleos y las portarodamientos de la muñeca reciben puentes de material para eliminar tramos sueltos.

Los608 y625 se representan con sus aros y blindajes completos. Los asientos se cortan solo en las piezas impresas correspondientes; no deben eliminar material de ejes o rodamientos. Las tapas tienen escalón de retención del aro exterior y un paso central que deja libre el aro interior. Los rodamientos son envolventes de catálogo, sin bolas internas ni simulación de contacto de rodadura.

| Eje | Apoyos externos | Transmisión y retención representadas |
| --- | --- | --- |
| J1 base | 2 ×608, Ø8/22/7 mm | Horn atornillado a brida, ejeØ8 con plano, separadores interiores y tornillo/arandela superior |
| J2 hombro | 2 ×608 | Piñón20T/rueda40T, rueda separada del núcleo a través de ejeØ8 con planos, collar trasero y tornillo/arandela frontal |
| J3 codo | 2 ×608 | Horn, brida y ejeØ8 con plano; collar y separadores; housing unido a la horquilla |
| J4 giro muñeca | 2 ×608 | Piñones24T, rueda integrada al rotor, ejeØ8 continuo con plano, hombro inferior y retención superior |
| J5 inclinación | 2 ×625, Ø5/16/5 mm | Brida y ejeØ5 con plano, separadores y retención axial |
| J6 herramienta | 2 ×625 | Horn, brida y ejeØ5 con plano; plataforma con llave y prisionero; separadores interiores |

Los cuatro manguitos de PVC incorporan topes axiales de2 mm para definir la profundidad de inserción. Las abrazaderas partidas conservan holgura radial nominal para introducir el caño; el apriete real de ese juego depende de la deformación y del tornillo. Las guías de las mordazas también conservan juego de deslizamiento. Un análisis que exija contacto cero entre todos los sólidos no representa esos dos ajustes funcionales.

El modelo contiene24 sólidos impresos rojos. Se conservan los identificadores históricos de los componentes para mantener las referencias de croquis/operaciones; los volúmenes consumidos por una unión no son piezas adicionales. Los soportes principales se identifican así:

| Componente | Pieza impresa que reúne |
| --- | --- |
| `V03_Plataforma_sandwich_superior` | Montura, montantes de placas, soporte fijo J1 y cajetín de bornera |
| `A01_Plataforma_giratoria_PETG` | Plataforma, torre de hombro y apoyo del motor GX |
| `A05_Encastre_proximal_brazo_PETG` | Núcleo de hombro y manguito proximal |
| `A06_Encastre_distal_brazo_PETG` | Manguito distal, horquilla de codo y housing del DS3218 |
| `A11_Encastre_proximal_antebrazo_PETG` | Núcleo de codo y manguito proximal de antebrazo |
| `A12_Encastre_distal_antebrazo_PETG` | Manguito distal, doble apoyo J4 y soporte del microservo |
| `V02_A15_Horquilla_J5_PETG` | Rotor/rueda de J4, horquilla J5 y housing del MG946 |
| `V02_A18_Nucleo_J5_y_columna_J6` | Núcleo J5, cuna de servo J6 y portarodamientos |
| `V02_A20_Bastidor_pinza` | Plataforma J6, columnas, palma, paredes y labios de guías |

Se usan insertosM3 de latón en las horquillas para sujetar las tapas sin tuercas sueltas en la zona de giro. La envolventeM3 de4,5 mm exterior y4 mm de largo corresponde a la [oferta de un fabricante en Caseros](https://articulo.mercadolibre.com.ar/MLA-896938702-inserto-m3-para-termoformado-_JM). Los asientos CAD muestran contacto nominal; la medida de inserción por calor depende del inserto y la impresión elegidos y debe ajustarse antes de fabricar. Las roscas y el moleteado se simplifican; los insertosM2 y tornillería mantienen el carácter de referencias dimensionales por confirmar.

Las carcasas de los siete servos llevan dos fijaciones representadas cada una. Los motores estándar usan tornillosM3 e insertos; las tres cunas de microservo usan la envolvente nominal de tornillos para plásticoØ2 ×8 mm. La rosca real no se modela: el taladro piloto del PETG debe ajustarse al tornillo elegido. Como referencia de provisión argentina, [Hobbytrónica ofrece MG90S180° con brazos y juego de tornillos](https://hobbytronica.com.ar/productos/servo-motor-mg90-180-grados-premium-1-8kg-torque/), consultado el27/10/2026. La ficha no define las dimensiones de esos tornillos: no acredita que coincidan exactamente con la envolvente dibujada. Confirmar el kit antes de fijar los pilotos de impresión. Los tornillos centrales de horn se distinguen de estas fijaciones de carcasa.

## Control sobre el auto

Se conservan tres XL4016, Nano y PCA9685 del brazo. Los reguladores se colocan verticales, separados lateralmente, con sus componentes hacia el exterior; Nano y PCA quedan del otro lado del servo. Los bastidores nacen de la placa de montaje y retienen los bordes de las placas, sin atribuir un patrón de taladros confirmado a módulos genéricos. Los conectores siguen siendo referencias geométricas; no se modificó firmware ni cableado del auto.

La bornera del brazo se lleva al borde posterior libre de la montura para despejar el anclajeM3 B. Se deja un acceso alrededor de la cabezaM10. El voltímetro del auto conserva una ventana en la placa superior. Los módulos del auto siguen en sus posiciones de referencia.

## Edición

Abrir el maestro nativo v03 y usar **Modificar > Cambiar parámetros**. `link1`/`link2` mueven los ejes y cambian los cortes de caño; `pvc_od`/`pvc_id` definen el tubo. `agv_mount_z`/`agv_mount_t`, las coordenadas `agv_A_x`…`agv_D_y` y `agv_center_x`/`agv_center_y` gobiernan los anclajes. Las medidas secundarias de cartuchos, tapas, bridas y retenciones son cotas editables en sus operaciones. No se hizo un barrido de cambios de parámetros: una edición grande puede requerir rediseñar la geometría que quede sin espacio.

Las articulaciones se accionan por las uniones nativas y sus vínculos de movimiento. J7 se centra a90°: el desplazamiento de unión−90…90° representa la orden de servo0…180°. La pieza50 mm está vinculada a la herramienta solo como referencia del agarre; ocultarla para revisar el cierre vacío. Las mordazas están retenidas por guías con juego, no unidas rígidamente a la palma.

El auto se incorpora como copia geométrica congelada para revisar el montaje. No es un enlace paramétrico externo al archivo original: si se cambia el chasis, actualizar la referencia y las cotas de anclaje. Las cotas provienen del CAD, no de un relevamiento del vehículo construido.

La reserva de alimentación continúa siendo externa al esquema del auto: esta revisión no dimensiona la batería, corriente total ni distribución eléctrica del conjunto móvil. PWM sigue representando posición solicitada a cada servo; no se agrega telemetría de posición ni sensores externos.

## Alcance de la comprobación

Esta revisión distingue tres cosas: ausencia de penetraciones, continuidad de cada sólido impreso y conexión mecánica mediante contacto, alojamiento o tornillo. Un cuerpo sin colisiones puede estar suelto; por eso los resultados de la v02 no se heredan como aceptación del montaje nuevo.

Los resultados finales y los despieces se documentan junto al maestro. Son comprobaciones geométricas virtuales: no prueban capacidad de carga, estabilidad del auto con el brazo extendido, resistencia de los eslabones, fuerza de agarre, corrientes o ajuste de una impresión real. El movimiento del AGV con el brazo desplegado requiere comprobar esas condiciones antes de ensayarlo físicamente.

## Resultados de la revisión solicitada

| Revisión | Resultado |
| --- | --- |
| Piezas impresas |24 sólidos rojos, cada uno con una sola región de material conectada; tapas desmontables separadas |
| Ejes principales |6 componentes con un cuerpo continuo cada uno |
| Rodamientos |12 completos: tres cuerpos por rodamiento, volumen de cada aro/blindaje igual al de la envolvente completa, contacto de aro interior con eje y exterior con apoyo |
| PVC |2 tubos completos,80 y70 mm; volumen anular sin recortes de otras piezas |
| Pernos de abrazadera |4 completos y continuos, sin las muescas de los cortes antiguos |
| Montajes internos del brazo |0 penetraciones entre componentes distintos, incluidos tornillos e insertos |
| Conjuntos móviles y auto |2.904 posturas muestreadas,0 penetraciones reportables mayores que0,001 mm³ |
| Ensamble nativo |0 interferencias reportables entre conjuntos en la pose de presentación |
| Apertura nativa de pinza |50,000 mm a109,8°;60,011 mm a133,7°;79,405 mm a180°; cierre mínimo4,007 mm sin pieza |
| Accionamientos nativos |18 posiciones mínimo/centro/máximo de ejes y9 de ida/vuelta de pinza; errores angulares de orden numérico |

Las2.904 posturas son181 aperturas de pinza vacía cada1°,524 posiciones de barridos individuales cada2° con extremos incluidos,2.197 combinaciones de J4/J5/J6 cada15° con cubo sujeto y2 posturas de trabajo. El barrido de hombro usa codo−90°; el de codo usa hombro0°; los otros barridos usan la postura de referencia guardada en el JSON. Esto no demuestra que todas las combinaciones de hombro/codo/muñeca o cualquier trayectoria continua sean libres de choques.

El grafo de contacto reúne el conjunto principal en una cadena. Las dos mordazas con sus almohadillas quedan como conjuntos deslizantes separados por el juego de las guías; sus límites y vínculos nativos evitan tratarlas como cuerpos libres. El grafo agrupa internamente los módulos comerciales y excluye la referencia del auto y el cubo: no demuestra apriete, fricción ni resistencia de las uniones.

Se corrigió también el propietario de la operación antigua `Rebaje_625`: su croquis pertenece al portarodamiento de J6 y el corte había quedado asociado a la plataforma de base. La reparación limita sus participantes a su alojamiento. Se conservaron las características geométricas comparadas de los256 cuerpos del brazo; el historial final no presenta advertencias ni errores de cálculo. Las operaciones suprimidas y los registros sin estado geométrico, como capturas de posición, no se consideran errores.

| Evidencia final | Archivo |
| --- | --- |
| Integridad de piezas, tubos, pernos y rodamientos | [continuidad_y_rodamientos.json](continuidad_y_rodamientos.json) |
| Contactos nominales | [grafo_contactos.json](grafo_contactos.json) |
| Penetraciones internas | [montajes_internos.json](montajes_internos.json) |
| Muestreo de movimiento y condiciones de cada barrido | [informe_movimiento.json](informe_movimiento.json) |
| Medidas de apertura nativa | [apertura_nativa_fusion.json](apertura_nativa_fusion.json) |
| Extremos nativos de ejes | [extremos_nativos_fusion.json](extremos_nativos_fusion.json) |
| Correspondencia de cadena y uniones Fusion | [paridad_cinematica.json](paridad_cinematica.json) |
| Análisis nativo de presentación | [pose_nativa_interferencias.json](pose_nativa_interferencias.json) |
| Reparación del asiento sin cambio de características geométricas | [reparacion_asiento625.json](reparacion_asiento625.json) |

No hay planos de fabricación ni tolerancias cerradas para los ejes/bridas especiales. Las figuras muestran soluciones de prototipo; el método de fabricación, los ajustes y el costo de esas piezas deben resolverse antes de construir.

## Vistas y despieces

- [Conjunto sobre el auto](../../../renders/2026-10-27/brazo_agv_v03.png).
- [Montura y electrónica vertical](../../../renders/2026-10-27/montura_electronica_v03.png).
- [Pinza abierta60 mm](../../../renders/2026-10-27/pinza_v03_abierta_60mm.png) y [cerrada50 mm](../../../renders/2026-10-27/pinza_v03_cerrada_50mm.png). Se aísla la herramienta para mostrar las guías; el apoyo fijo de J6 queda oculto solo en estas vistas.
- [Postura de alcance hacia el suelo](../../../renders/2026-10-27/alcance_suelo_agv_v03.png), revisión virtual de una postura, sin simular fuerzas o estabilidad.
- [Despiece parcial de J4](../../../renders/2026-10-27/despiece_j4_v03.png), con eje, dos608, tapa, separadores, rotor/rueda y accionamiento.
- [Despiece de montura fija](../../../renders/2026-10-27/despiece_base_v03.png), con contraplaca, casquillo, separadores, electrónica y apoyos de J1.

Los despieces son copias geométricas derivadas: el maestro mantiene sus uniones y su pose armada. Sus F3D de consulta se guardan en [exports/brazo-v03](../../../exports/brazo-v03), con mapas de desplazamiento. Las imágenes y exportaciones derivadas son regenerables; el maestro editable y las fuentes permanecen en esta carpeta. Los scripts requieren Fusion y fases explícitas; no ejecutar de nuevo una fase aplicada del [registro de autoría](authoring_journal.jsonl).
