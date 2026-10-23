# AutoFinalRobo2-CAD

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

Versionar fuentes editables y scripts de generación, junto con parámetros, manifests y bibliotecas propias. Dependencias descargables y salidas se mantienen fuera de Git con procedencia y pasos de recuperación documentados. No ejecutar herramientas de diseño/validación ni generar geometría hasta recibir los planos e indicaciones correspondientes.

[Organización Git e historial](../AutoFinalRobo2-Contracts/docs/ESTRUCTURA_Y_GIT.md) · [Registro AFR2-20261023-04](../AutoFinalRobo2-Contracts/changes/2026-10-23-04-cad-and-monorepo.md).
