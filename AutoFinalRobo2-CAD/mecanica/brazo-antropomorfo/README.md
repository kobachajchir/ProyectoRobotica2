# Brazo antropomorfo PVC32 — prototipo editable

**Preselección AFR2-20261028-06, sin nuevo CAD:** [efecto de los motores nuevos sobre las piezas actuales](preseleccion_actuadores_v10/README.md), con25 combinaciones de masa y comprobación de cargas, uniones, tubos, agarre y sensibilidad de vuelco. Cambiar por motores más pesados no cierra el caso50 ms. Opción ligera con96 g netos de incremento pasa la comparación de pares×2 a100 ms, con límites explícitos y selección aún pendiente. El maestro vigente sigue siendo v09.

**Revisión vigente v09 — AFR2-20261028-04:** [J1 GX3270BLS, eje/brida metálica, cartucho6000 y cuna reforzada baja](revision_v09/README.md), con [ensamble editable](AFR2_Brazo_PVC32_AGV_v09_J1_GX3270BLS.f3d). PLA PrintaLot definido por el usuario; perfiles y resistencia por orientación pendientes.3.087 configuraciones CAD sin penetraciones reportables;79.651 estados de predimensionado, incluyendo frenadas conjuntas de seis ejes. La mejora de J1 no aprueba el conjunto a200 g: J2/J3/J5 pierden el margen preliminar×2 en50 ms, y faltan estabilidad y ensayos. Las versiones inferiores son antecedentes conservados.

**Estudio AFR2-20261028-01:** [J1 y sus uniones: cargas, tensiones nominales y rediseño fundamentado](analisis_J1_uniones/README.md). Secciones reales y 7.075 estados evaluados; acople D, postes/nervios y contacto con chasis requieren revisión. Propuesta de eje Ø10, apoyos separados 16 mm y brida metálica sin elevar el plato, con estudio parcial de espacio. El CAD v08 se conserva; material/perfil aún indefinidos y resistencia física sin aprobar.

**Revisión anterior v08 — AFR2-20261027-21:** [housing GX3270BLS terminado y conjunto recomprobado](revision_v08/README.md), con [ensamble F3D](AFR2_Brazo_PVC32_AGV_v08_Housing.f3d) y [housing paramétrico independiente](revision_v08/Housing_GX3270BLS_Parametrico.f3d). Se corrigen las interferencias nuevas y se comprueban 3.141 configuraciones CAD sin penetraciones reportables, doce rodamientos y seis ejes. Masas/cargas recalculadas; J1, apoyos y alimentación siguen pendientes. Sin FEA ni capacidad física acreditada. Las revisiones inferiores son antecedentes.

**Revisión CAD vigente v07 — AFR2-20261027-20:** [GX3270BLS en el codo, alojamiento integrado y refuerzos locales](revision_v07/README.md). [Ensamble editable](AFR2_Brazo_PVC32_AGV_v07_GX3270BLS.f3d), [motor independiente](revision_v07/GX3270BLS_Referencia_Parametrica.f3d) y [cargas actualizadas](analisis_cargas_200g/07_implementacion_y_cargas_v07.md). Montura baja y longitudes conservadas; masas/estática y frenadas hipotéticas recalculadas. Sin barridos CAD, FEA ni capacidad física acreditada. La revisión geométrica de v06 no cubre estos cambios. Los párrafos de entregas anteriores que siguen son históricos.

**Informe integrado AFR2-20261027-18:** [por qué iteramos el diseño, cargas existentes y plan de frenada/rediseño v07](analisis_cargas_200g/INFORME_ITERACION_DISENO.md). Incluye historial, seis componentes de esfuerzo por sección y prioridades en base/hombro/primer eslabón. Solo integración documental; selección final, dinámica, FEA y geometría v07 pendientes.

**Análisis AFR2-20261027-17:** [masas, centros de masa y cargas con 200 g](analisis_cargas_200g/README.md), con copia horizontal de estudio, fuerzas por articulación, cargas en apoyos/engranajes y tensiones nominales de PVC. Se conserva la v06; no hay aún refuerzos nuevos ni estudio FEA. Los 200 g son requisito de cálculo, no carga útil física certificada.

**Revisión anterior v06:** [montura baja, contraplaca inferior única y revisión geométrica](revision_v06/README.md). [Maestro editable](AFR2_Brazo_PVC32_AGV_v06.f3d), parámetros en `parametros_prototipo_v06.csv`. Conserva la base a 6 mm del chasis y J1 retrasado 14 mm, con una sola pieza inferior de PETG. Revisión autorizada de ejes, rodamientos, engranajes y 3.141 configuraciones CAD sin penetraciones reportables. Despieces pendientes de la señal del usuario; [plan de láminas/skill](../../docs/PLAN_SKILL_LAMINAS_TECNICAS.md).

**Antecedente v05:** [montura baja](revision_v05/README.md). Sustituyó la elevación general por recorte del pan, motor J1 retrasado y puentes sobre la electrónica. Se guardó por pedido del usuario antes de revisar interferencias; esa revisión se realiza sobre v06.

**Antecedente v04, solución elevada descartada por el usuario:** [fijación bilateral y giro del HC-SR04](revision_v04/README.md). [Maestro conservado](AFR2_Brazo_PVC32_AGV_v04.f3d). Incorporó los dos anclajes izquierdos y la unión de pan; su elevación de 32 mm no forma parte del montaje vigente.

**Antecedente v03:** [brazo sobre el auto, continuidad mecánica y despieces](revision_v03/README.md). [Maestro editable](AFR2_Brazo_PVC32_AGV_v03.f3d), parámetros en `parametros_prototipo_v03.csv`. Introdujo la montura con electrónica vertical y corrigió guías, rodamientos, ejes, retenciones y fijaciones de motores. Su análisis trató el auto como fijo y no incluyó el pan del HC-SR04.

**Antecedente v02:** [pinza y muñeca, edición y evidencia de movimiento](revision_v02/README.md). Maestro `AFR2_Brazo_PVC32_Prototipo_v02.f3d`, parámetros en `parametros_prototipo_v02.csv`. La v02 sustituyó la transmisión de referencia de la v01, pero su análisis de penetraciones no detectó todas las discontinuidades y recortes ajenos corregidos en v03. Sus resultados no certifican el montaje físico ni la continuidad de la v03.

AFR2-20261026-05. Modelo de propuesta para revisión del usuario, creado en Fusion el 26/10/2026. El alcance es el diseño geométrico editable del brazo y la distribución de su control en la base. No acredita capacidad de carga ni aptitud de fabricación.

## Archivos y edición

- `AFR2_Brazo_PVC32_Prototipo_v01.f3d`: archivo nativo con componentes, croquis acotados, operaciones e historial. Abrir desde Fusion mediante **Archivo > Abrir desde mi equipo**. El documento también se guarda en el proyecto activo de Fusion.
- `parametros_prototipo.csv`: copia de los parámetros del documento, con expresión, unidad y procedencia.
- `modelo_prototipo.json`: identificación del documento Fusion y datos de la propuesta. El maestro contiene 88 parámetros de usuario y 96 componentes contando la raíz.
- `build_prototype.py`: fuente de la construcción mediante la API de Fusion; requiere su entorno Python `adsk`. No es un ejecutable independiente ni un validador. El F3D puede editarse sin ejecutar este script.
- [Fuentes y selección comercial](FUENTES_Y_COMPONENTES.md).
- Foto original del usuario: [PVC32](../../reference/brazo-prototipo/pvc32_usuario.jpg).
- Imágenes regenerables: `../../renders/2026-10-26/brazo_pvc32_prototipo.png` y `brazo_base_electronica.png`. La segunda muestra la distribución de placas con las paredes y tapa ocultas; los conectores quedan en sus posiciones de montaje sobre la carcasa oculta.

En **Modificar > Cambiar parámetros** se editan los parámetros de usuario. `link1`, `link2`, `shoulder_z`, `wrist45`, `wrist56` y `tcp_offset` gobiernan las distancias de la cadena; los orígenes de unión y los cortes de PVC usan sus expresiones. `gx_*`, `ds_*`, `mg_*` y `micro_*` gobiernan las envolventes de motor y las cavidades correspondientes. Cada pieza conserva sus propios croquis/operaciones; las medidas secundarias constantes también son cotas editables de esos croquis.

`q1` a `q6` son los ángulos de alineación de la pose de presentación, no una lectura de encoders. La base está fijada; las piezas internas están fijadas a su conjunto. Hay seis uniones de revolución, una unión auxiliar del piñón de hombro y dos correderas de mordazas vinculadas con movimiento opuesto. `grip_open` desplaza los orígenes de las guías; las correderas siguen teniendo libertad de movimiento. El cubo de referencia está sujeto por una unión rígida a la herramienta para mostrar el agarre. Para explorar la pinza sin él se puede ocultar `90_Pieza_REFERENCIA_50mm_100g`.

No se realizaron barridos de parámetros, pruebas de movimiento ni detección de interferencias. Las expresiones están construidas en el modelo; su rango práctico de edición queda pendiente de revisión. Por ejemplo, reducir un eslabón hasta que sus alojamientos se superpongan requiere rediseñar esos alojamientos.

## Medidas de la propuesta

| Elemento | Valor inicial | Estado |
| --- | --- | --- |
| Base | 160 × 160 mm; caja de 55 mm y tapa de 3 mm | Propuesto |
| Altura del eje de hombro | 120 mm sobre la mesa | Propuesto |
| Hombro–codo, `link1` | 130 mm entre ejes | Propuesto |
| Codo–giro de muñeca, `link2` | 120 mm entre ejes | Propuesto |
| Muñeca–TCP | 25 + 25 + 25 = 75 mm | Propuesto |
| PVC exterior/interior | Ø32 / Ø28,3 mm | Foto del usuario |
| Pared del PVC | (32 − 28,3) / 2 = **1,85 mm** | Derivada |
| Cortes de PVC | 80 y 70 mm | `link1 − 2 × tube_inset` y `link2 − 2 × tube_inset` |
| Manguitos | 22 mm de largo; solape nominal de 17 mm | Propuesto |
| Holgura diametral de PVC | 0,35 mm | Sin calibrar impresión |
| Holgura diametral de rodamientos | 0,25 mm | Sin calibrar impresión |
| Apertura de muestra / referencia máxima | 50 / 65 mm | Propuesto, no carrera certificada |
| Pieza de trabajo | 50 × 50 × 50 mm; 100 g | Requisito del usuario |

La imagen recibida también rotula `C = 3,2 mm`. Ese espesor no es compatible con los diámetros A/B de la misma imagen. El modelo utiliza A/B; **no utiliza una pared de 3,2 mm**. El tipo y la medida real del caño a comprar deberán coincidir con los diámetros modelados.

Los 130/120 mm son distancias entre articulaciones, no largos de caño. Los núcleos y manguitos ocupan los extremos. La extensión geométrica nominal de hombro a TCP es 325 mm antes de considerar límites y obstrucciones; no se declara un volumen de trabajo libre de colisiones. La referencia del auto es 245 × 200 mm en planta; el brazo se plantea como estación fija junto a la zona de entrega del AGV. El docking, la bandeja y la altura de entrega no se diseñaron en esta revisión.

## Árbol del ensamble

| Conjunto raíz | Contenido |
| --- | --- |
| `00_Base_control` | Caja, frente y tapa desmontables, bandeja, apoyos de placas, Nano, PCA9685, tres XL4016, conector/interruptor de referencia, servo J1 y apoyo 608 |
| `01_Giro_base` | Plataforma, torre, dos apoyos 608, servo GX y housing |
| `02_Brazo_PVC` | Primer caño y manguitos partidos, núcleo de hombro, rueda 32T, eje Ø8, horquilla de codo, dos 608, DS3218 y housing |
| `03_Antebrazo_PVC` | Segundo caño y manguitos, núcleo/eje de codo, alojamiento J4, 608, microservo desplazado y piñón J4 |
| `04_Giro_muneca` | Rotor/eje Ø8, rueda J4, horquilla J5, dos 625, MG946R y housing |
| `05_Inclinacion_muneca` | Núcleo/eje Ø5, alojamiento 625 J6, microservo y piñón J6 |
| `06_Herramienta` | Eje/rueda J6, palma, guías, microservo de pinza, engranajes de referencia y TCP |
| `07_...` / `08_...` | Mordazas, patines, cremalleras y almohadillas |
| `09_Pinion_hombro` | Piñón 16T de la reducción externa 2:1 |
| `90_Pieza_...` | Cubo de trabajo de referencia |

Los conjuntos principales usan orígenes de unión locales; no son una sola malla ni un único sólido importado. Los componentes comerciales también son geometría nativa simplificada. Se distingue PETG rojo, PVC blanco, carcasas negras, metales y placas de control por apariencia.

## Selección para esta revisión

| Función | Actuador propuesto | Cantidad | Transmisión representada |
| --- | --- | --- | --- |
| J1 giro de base | MG946R posicional, 180° | 1 | Directa, interfaz de horn de referencia |
| J2 hombro | GX3240MGV1 posicional, 270° | 1 | Piñón 16T / rueda 32T, módulo 1,5; relación 2:1 |
| J3 codo | DS3218MG posicional, 180° | 1 | Directa, eje apoyado en 608 |
| J4 giro de muñeca | MG90S posicional | 1 | Par 24T/24T; motor lateral |
| J5 inclinación | MG946R posicional, 180° | 1 | Directa, eje apoyado en 625 |
| J6 giro de herramienta | MG90S posicional | 1 | Par 20T/20T; motor lateral |
| Pinza | MG90S posicional | 1 | Transmisión y dos cremalleras de referencia |

**BOM principal del concepto:** un GX, un DS3218, dos MG946R, tres MG90S, seis 608-2Z/ZZ (8 × 22 × 7 mm), tres 625-2Z/ZZ (5 × 16 × 5 mm), ejes lisos Ø8/Ø5, dos cortes de caño Ø32, piezas impresas, tornillería M3/M4 y anclajes de base. La tornillería representada es parcial y no constituye una lista de compra cerrada. No sustituir servos posicionales por variantes de rotación continua.

Las articulaciones y encastres se proponen en **PETG rojo de 1,75 mm**, por ejemplo PrintaLot comercializado en Argentina. La apariencia del CAD no representa densidad, orientación de capas ni propiedades de una impresión. Las masas de los servos no se deducen de los sólidos simplificados.

## Control alojado en la base

Arduino Nano de referencia A000005 (45 × 18 mm), módulo PCA9685 de referencia comercial 70 × 25 mm y tres módulos XL4016 de referencia 65 × 48 mm. Se incluyen apoyos y espacio de volumen; los taladros de cada variante comercial siguen pendientes de relevar. No se creó un esquema eléctrico ni se cableó el CAD.

La organización eléctrica propuesta es entrada externa de 12 V, regulación separada para el GX, para el resto de servos y para lógica. Los valores finales de regulación y la fuente deben determinarse con las variantes elegidas y sus corrientes; los tres módulos representan la reserva de espacio. El PCA9685 genera señales PWM: **no es una etapa de potencia ni debe alimentar los servos desde la salida de 5 V del Nano**. El regulador de lógica debe respetar la alimentación admitida por la variante Nano y no enfrentarse con USB. No se dimensionaron corrientes, disipación, fusibles ni cableado en esta tarea.

## Límites de la entrega

Este archivo sirve para discutir proporciones, arquitectura, acceso a motores y distribución. Los engranajes tienen dientes rectangulares de representación, sin perfil de evolvente, juego ni fase de engrane resueltos. Los splines, horns, pestañas y patrones de agujeros son referencias editables; en particular la variante GX V1 local no tiene un plano completo confirmado. El apriete de ejes, acoples horn–eje, retención axial, tuercas, tapas de transmisión y unión definitiva de algunas piezas deben detallarse tras la revisión. No exportar estos dientes o interfaces como piezas finales de fabricación.

El parámetro `part_mass = 100 g` registra el requisito y no configura un ensayo de carga ni certifica 100 g de capacidad útil. No se ejecutaron cálculos de esfuerzos, pandeo, torque, estabilidad, interferencias, simulación dinámica, pruebas físicas ni validadores. No se modificaron firmware, Web, PCB ni el modelo del auto. No hubo commit ni publicación Git en esta entrega.

## Regeneración opcional de la fuente

El maestro entregado es el F3D. Para reconstruir desde cero en un documento nuevo de Fusion, ejecutar la fuente mediante su API con `PHASE` en este orden, una fase por llamada: `init`, `base`, `lid`, `electronics`, `tower`, `shoulder_motor`, `drive`, `link1`, `elbow`, `link2`, `roll_motor`, `wrist_roll`, `wrist_tilt`, `gripper`, `fingers`, `assembly`, `wrist_transmissions`, `gripper_transmission`, `mounts`, `presentation`, `save`. No repetir fases sobre el maestro existente porque agrega geometría. `scope_cuts` fue una reparación del modelado de esta sesión y no forma parte de esa secuencia: la fuente ya limita los cortes a su componente.
