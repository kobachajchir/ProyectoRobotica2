> **Vigente AFR2-20261024-15:** [Matriz del maestro](../AutoFinalRobo2-Contracts/docs/ALINEACION_MAESTRO.md):33comandos y59identidades OLED con fuentes complementarias. Calibración Web adquiere control al escribir; ESP/Web informan FS/paquete. Web/ESP compilados15; AVR sin nueva fuente, build12/EX01v6/firmware0.8.0/gates0. Sin brecha software definida identificada tras lectura; aceptación física, memoria dinámica, flash/SPI y planos CAD pendientes. Solo documentación en CAD, sin diseño nuevo. Estados inferiores históricos.

# AutoFinalRobo2-CAD

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
| [reference](reference/README.md) | Insumos conservados, inventario y procedencia de dependencias |
| [docs](docs/ALCANCE_Y_PENDIENTES.md) | Alcance, estado, decisiones y datos pendientes |
| exports/ | STEP/STL/GLB, Gerber, taladros, planos u otras salidas regenerables; ignorada por Git |

Leer [AGENTS.md](AGENTS.md), [adaptación](INFORME_ADAPTACION.md) y [requisitos PCB](../AutoFinalRobo2-Contracts/hardware/PCB_OLED_ENCODER.md). Contracts sigue siendo autoridad para conexiones y cambios compartidos. No duplicar allí las fuentes CAD.

Los insumos de reference/supplied provienen de la antigua carpeta CADS. Sus nombres no acreditan medidas, componentes ni ajuste al auto real. El origen de descarga, autoría y licencia siguen pendientes. El GLB del robot anterior en Web/reference no representa este Mecanum.

Versionar fuentes editables y scripts de generación, junto con parámetros, manifests y bibliotecas propias. Dependencias descargables y salidas se mantienen fuera de Git con procedencia y pasos de recuperación documentados. El [plan de Fusion](docs/PLAN_MODELO_3D_FUSION.md) define la ejecución futura del auto base sin cables; permite inferencias documentadas y CAD de Internet. La pieza nueva, soporte y PCB siguen pendientes; preparar OLED 0,96 pulgadas/encoder sin breakout y reservar espacio. La entrega actual solo documenta/publica el plan; no genera geometría ni ejecuta validadores.

[Organización Git e historial](../AutoFinalRobo2-Contracts/docs/ESTRUCTURA_Y_GIT.md) · [Registro AFR2-20261023-04](../AutoFinalRobo2-Contracts/changes/2026-10-23-04-cad-and-monorepo.md).

[Plan completo para continuar en la otra PC](docs/PLAN_MODELO_3D_FUSION.md). Estado AFR2-20261023-05: planificación registrada; búsqueda/descarga de modelos y ensamble todavía no iniciados.
