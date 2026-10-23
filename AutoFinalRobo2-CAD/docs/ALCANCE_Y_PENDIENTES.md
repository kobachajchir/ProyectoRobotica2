# Alcance y datos pendientes

Este módulo concentra el diseño interno del vehículo y la electrónica asociada. La organización está creada y el [plan de Fusion](PLAN_MODELO_3D_FUSION.md) define el ensamble futuro del auto base, sin cables y con inferencias documentadas. Pieza nueva/soporte/PCB siguen pendientes. La entrega actual solo guarda/publica el plan.

| Área | Entregables futuros | Datos pendientes |
| --- | --- | --- |
| PCB OLED/encoder | Proyecto KiCad editable, esquema, PCB, bibliotecas propias | Módulos y pinouts reales, VCC/pull-ups, huellas, contorno, orientación J1/J2 y reglas de fabricación |
| Pieza nueva | Fuente paramétrica editable, soporte/cubierta y plano acotado | Planos del usuario, material, espesores, espacio libre y fijación |
| Auto completo | Ensamblaje editable con componentes y referencias relativas | Reconstrucción documental de chasis y posición de placas/motores/ruedas/batería; inferencias explícitas; sin cables |
| Fabricación/impresión | Exportaciones derivadas de una revisión identificada | Formatos, tolerancias, proceso y autorización de verificaciones |

La [especificación PCB](../../AutoFinalRobo2-Contracts/hardware/PCB_OLED_ENCODER.md) establece dos conectores 2,54 mm (señales 1x5 y VCC/GND comunes 1x2) y fijación con dos tornillos/tuercas. No determina tamaños de tornillo, altura, espesor, tensión ni huellas. Leer también [pinout](../../AutoFinalRobo2-Contracts/hardware/PINOUT.md), [shield](../../AutoFinalRobo2-Contracts/hardware/WIFI_SHIELD.md) y [carga](../../AutoFinalRobo2-Contracts/hardware/FLASHING.md).

Antes de diseñar: identificar unidades/escala y planos fuente, registrar qué cotas vienen del usuario y cuáles son propuestas, elegir la herramienta/formato editable según los insumos e instrucciones, y conservar referencias relativas entre piezas, ensamblajes y bibliotecas KiCad. No afirmar que los insumos CAD coinciden con el auto.

Al adaptar un archivo, documentar origen/ruta/hash, autor y licencia conocidos, cambios, revisión, parámetros relevantes y estado. La trazabilidad de interfaces eléctricas/mecánicas compartidas se registra en Contracts con ficha AFR2, manteniendo los estados atmega/esp/web.

Los controles ERC/DRC, revisión de interferencias y pruebas de montaje se planearán cuando corresponda, pero no se ejecutan sin pedido expreso. No hay resultados de verificación o fabricación en esta entrega.
