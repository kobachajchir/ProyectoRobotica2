> **Vigente AFR2-20261024-15:** [Matriz del maestro](../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

> **AFR2-20261027-01:** [propuesta de recolector](mecanica/robot-recolector/README.md), con dibujo SVG superior/lateral y rig simplificado en Web. Ensamble nativo auto/brazo/deposito, soporte y cargas pendientes. Maestros existentes sin modificar; sin comprobaciones de esta revision.

# AutoFinalRobo2-CAD

**AFR2-20261028-01:** [comprobaciones de J1/uniones sobre v08](mecanica/brazo-antropomorfo/analisis_J1_uniones/README.md). 35 secciones, 7.075 estados, apoyos/eje, bastidor, acople D y anclajes; requisitos de resistencia sin material FDM asignado. Propuesta de brida, eje Ø10 y separación16 mm a la misma altura, con estudio parcial de espacio. No hay ensamble nuevo ni FEA de sólidos/contactos; maestro v08 sin modificar.

**AFR2-20261027-21, brazo vigente v08:** [housing GX3270BLS y nueva revisión geométrica](mecanica/brazo-antropomorfo/revision_v08/README.md), con [ensamble F3D](mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v08_Housing.f3d) y housing independiente. Reservas de montaje corregidas, 3.141 configuraciones sin penetraciones reportables, seis ejes/doce rodamientos completos. Masas/cargas actualizadas; sin FEA ni aceptación física de 200 g/frenada. Sustituye la geometría vigente v07 conservando su maestro.

**AFR2-20261027-20, brazo vigente v07:** [GX3270BLS 270° en el codo, cuna/horquilla integrada y refuerzos locales](mecanica/brazo-antropomorfo/revision_v07/README.md). [F3D](mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v07_GX3270BLS.f3d) y motor independiente, 158 parámetros, montura baja conservada. [Recálculo](mecanica/brazo-antropomorfo/analisis_cargas_200g/07_implementacion_y_cargas_v07.md): 2,267 kg con200 g, codo15,83 kgf·cm estáticos y frenadas hipotéticas. J1 y alimentación siguen abiertos; sin barridos, FEA ni capacidad física acreditada. Los estados inferiores son antecedentes.

**AFR2-20261027-18:** [informe integrado de esfuerzos e iteración del brazo PVC32](mecanica/brazo-antropomorfo/analisis_cargas_200g/INFORME_ITERACION_DISENO.md). Relaciona problemas, correcciones v01–v06, masas/estática de 200 g y plan v07 de actuadores/refuerzos. Frenada finita y cargas combinadas definidas como método; sin nuevos cálculos, CAD, FEA ni comprobaciones.

**AFR2-20261027-17:** [presupuesto de masas y esfuerzos de brazo v06 con 200 g](mecanica/brazo-antropomorfo/analisis_cargas_200g/README.md). Volúmenes/CG extraídos de Fusion, peso propio y carga separados, apoyos y engranajes, sensibilidad de masa FDM y criterios de refuerzo. Copia horizontal de estudio guardada; maestro v06 conservado. Cálculo estático externo, sin FEA ni refuerzos modelados; 200 g no acreditados como carga física.

**AFR2-20261027-15, brazo vigente v06:** [montura baja y contraplaca inferior única](mecanica/brazo-antropomorfo/revision_v06/README.md), con seis ejes y doce rodamientos completos, transmisiones revisadas y 3.141 configuraciones CAD sin penetraciones reportables. [F3D editable](mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v06.f3d). Sustituye la solución elevada v04; no acredita carga ni estabilidad física. Despieces aún sin generar por indicación humana; [plan de skill de láminas](docs/PLAN_SKILL_LAMINAS_TECNICAS.md).

**AFR2-20261027-09:** [brazo v04: montura bilateral y giro del HC-SR04](mecanica/brazo-antropomorfo/revision_v04/README.md). Dos M3 izquierdos con contrabarra completan seis laterales más el central. Alza paramétrica de 32 mm, misma disposición de electrónica y unión nativa para mover salida del servo, soporte y sensor entre −90° y +90°. [Maestro editable](mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v04.f3d). La v03 no había incluido ese giro del auto; evidencia y límites en el informe v04.

**AFR2-20261027-03:** [brazo v03 montado sobre el auto](mecanica/brazo-antropomorfo/revision_v03/README.md). Montura con cuatroM3 y apriete centralM10/contraplaca, control vertical, guías continuas, doce rodamientos externos, ejes y retenciones completos, fijaciones de servos y despieces derivados. [Maestro nativo](mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_AGV_v03.f3d). El maestro del auto y las versiones anteriores del brazo se conservan; carga, estabilidad y fabricación siguen pendientes. No se promueve este ensamble a Web/ESP.

**AFR2-20261026-06:** [brazo v02 y revisión de movimiento](mecanica/brazo-antropomorfo/revision_v02/README.md). Pinza con un piñón y dos cremalleras vinculadas, apertura60,011 mm y agarre50 mm; J6 directo, apoyos y separaciones corregidos. Comprobación CAD autorizada:2902 poses muestreadas sin penetraciones reportables; no acredita todo el espacio continuo ni capacidad de carga. [Maestro v02](mecanica/brazo-antropomorfo/AFR2_Brazo_PVC32_Prototipo_v02.f3d).

**AFR2-20261026-05:** [prototipo de brazo antropomorfo PVC32](mecanica/brazo-antropomorfo/README.md), con componentes y parámetros nativos de Fusion, articulaciones rojas, motores/housings, rodamientos y control alojado en la base. Diseño para revisión; sin cálculo de carga, interferencias ni fabricación. Es una ampliación autorizada del CAD; el estado del auto de los hitos anteriores se conserva.

**Final AFR2-20261025-13:** [auto con PCB y soporte del usuario](docs/MODELO_FINAL_WEB_20261025.md), maestros Fusion/Blender guardados y GLB **871.756 bytes** integrado en Web y paquete ESP. Importación/Draco y render Blender comprobados; sin ERC/DRC ni prueba física. Las revisiones inferiores son históricas.

**Vigente AFR2-20261025-12:** [Panel OLED/encoder](docs/PANEL_UI_20261025.md): esquema/PCB de 20×35 mm ruteada en B.Cu, modelos Fusion vinculados por STEP y soporte con bisel desmontable/torretas M3×15 creados. Sin ERC/DRC ni montaje físico; ranura M3 y niveles I2C pendientes. Los estados de CAD pendiente que siguen son históricos.

AFR2-20261024-01: [ruedas y Blender](docs/RUEDAS_Y_BLENDER_20261024.md). Fusion v16 y Blender guardados; fotos/renders y GLB **827.232 bytes**, materiales PBR y Draco. Casquillos Ø6→Ø4 de referencia autorizados. Estados anteriores históricos.

Actualización AFR2-20261023-07: [armado paso a paso en Fusion](docs/ARMADO_FUSION_PASO_A_PASO.md), versión 15 guardada y [maestro F3D exportado](mecanica/auto-completo/AutoFinalRobo2.f3d). Ensamble parcial de los CAD aportados en CADS; Model Y esencial al final. Ruedas de 80 mm y acoples pendientes, sin generar ruedas. Capas del chasis conservadas; inferencias y ejes documentados. Las secciones anteriores de entrega que siguen son históricas.

Actualización AFR2-20261023-06: [primer lote de candidatos CAD para aprobación](docs/REVISION_CANDIDATOS_3D.md), [estado de continuidad](docs/ESTADO_MODELO_3D.md). Descargas realizadas, sin iniciar modelado. El usuario indica modelar el chasis a mano. Los párrafos de entrega AFR2-20261023-05 de abajo describen el hito anterior.

Módulo interno para todo el diseño eléctrico y mecánico de AutoFinalRobo2: esquemas y PCB KiCad, pieza nueva/soporte/cubierta y modelo completo del auto. Pertenece al único Git de D:/ProyectoRobotica2; no tiene .git independiente.

Estado al 23/10/2026: estructura y documentación creadas; nueve insumos CAD existentes conservados sin adaptación. No hay esquema, PCB ni pieza nueva diseñados, ni ensamblaje del robot real comprobado.

| Carpeta | Uso |
| --- | --- |
| [kicad/oled-encoder](kicad/oled-encoder/README.md) | Proyecto editable de la PCB OLED/encoder, pendiente |
| [kicad/libraries](kicad/libraries/README.md) | Símbolos, huellas y modelos propios o adaptados |
| [mecanica/pieza-nueva](mecanica/pieza-nueva/README.md) | Pieza nueva, soporte y relación con la cubierta |
| [mecanica/auto-completo](mecanica/auto-completo/README.md) | Ensamblaje completo del vehículo |
| [mecanica/componentes](mecanica/componentes/README.md) | Modelos editables propios de componentes |
| [mecanica/brazo-antropomorfo](mecanica/brazo-antropomorfo/README.md) | Prototipo paramétrico del brazo PVC32, fichas y fuentes |
| [reference](reference/README.md) | Insumos conservados, inventario y procedencia de dependencias |
| [docs](docs/ALCANCE_Y_PENDIENTES.md) | Alcance, estado, decisiones y datos pendientes |
| exports/ | STEP/STL/GLB, Gerber, taladros, planos u otras salidas regenerables; ignorada por Git |

Leer [AGENTS.md](AGENTS.md), [adaptación](INFORME_ADAPTACION.md) y [requisitos PCB](../AutoFinalRobo2-Contracts/hardware/PCB_OLED_ENCODER.md). Contracts sigue siendo autoridad para conexiones y cambios compartidos. No duplicar allí las fuentes CAD.

Los insumos de reference/supplied provienen de la antigua carpeta CADS. Sus nombres no acreditan medidas, componentes ni ajuste al auto real. El origen de descarga, autoría y licencia siguen pendientes. El GLB del robot anterior en Web/reference no representa este Mecanum.

Versionar fuentes editables y scripts de generación, junto con parámetros, manifests y bibliotecas propias. Dependencias descargables y salidas se mantienen fuera de Git con procedencia y pasos de recuperación documentados. El [plan de Fusion](docs/PLAN_MODELO_3D_FUSION.md) define la ejecución futura del auto base sin cables; permite inferencias documentadas y CAD de Internet. La pieza nueva, soporte y PCB siguen pendientes; preparar OLED 0,96 pulgadas/encoder sin breakout y reservar espacio. La entrega actual solo documenta/publica el plan; no genera geometría ni ejecuta validadores.

[Organización Git e historial](../AutoFinalRobo2-Contracts/docs/ESTRUCTURA_Y_GIT.md) · [Registro AFR2-20261023-04](../AutoFinalRobo2-Contracts/changes/2026-10-23-04-cad-and-monorepo.md).

[Plan completo para continuar en la otra PC](docs/PLAN_MODELO_3D_FUSION.md). Estado AFR2-20261023-05: planificación registrada; búsqueda/descarga de modelos y ensamble todavía no iniciados.
