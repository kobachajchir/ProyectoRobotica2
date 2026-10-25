> **Vigente AFR2-20261024-15:** [Matriz del maestro](../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

# Estado de adaptación CAD

AFR2-20261023-04, 23/10/2026. Módulo nuevo del Git global. Se preparan ubicaciones para KiCad, pieza nueva, componentes y ensamblaje completo del auto.

Se trasladan nueve insumos STEP/FreeCAD de CADS/mecanum-robot-car-chassis-motors-and-wheels a reference/supplied/mecanum-robot-car-chassis-motors-and-wheels sin modificar su contenido. [Inventario por archivo](reference/insumos-locales.json). El nombre del conjunto y de sus piezas no prueba que sean equivalentes al vehículo OSOYOO: hay nombres de motor 37 mm y ruedas 60/80 mm que deben contrastarse con la documentación y mediciones futuras. No se extrajeron cotas ni se abrió el proyecto en una herramienta CAD.

Origen remoto, versión, autoría y licencia de esos archivos: pendientes de identificar. No atribuirles autoría del proyecto ni presentarlos como un diseño nuevo. No eliminarlos como descarga reproducible hasta documentar su recuperación.

Aún no se adaptó ningún modelo, ni se crearon esquemas/PCB/pieza nueva. Atmega, ESP y Web conservan su estado anterior. Las fuentes mecánicas/eléctricas nuevas se registrarán por archivo y los cambios compartidos en Contracts. No se ejecutaron tests, builds, validadores, ERC/DRC, flasheos ni pruebas físicas.

## AFR2-20261023-05: plan de Fusion

[Plan completo](docs/PLAN_MODELO_3D_FUSION.md) documentado para la otra PC: ensamble base sin cables, CAD por inventario y calidad/correspondencia dimensional, inferencias explícitas, OLED 0,96 pulgadas y encoder mecánico KY-040 sin breakout/perilla como referencias pendientes. Pieza nueva/soporte/PCB pendientes y espacio reservado. No se descarga ni adapta ningún CAD en esta entrega. Git privado global acordado: kobachajchir/ProyectoRobotica2; rama remota main.
