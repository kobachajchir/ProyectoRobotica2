# AFR2-20261106-15 — Informe de pinout completo y agregados propios

Pedido del usuario: informe del robot completo, identificando agregados propios;
incluir D32 y D33 como AutoKEY y AutoRESET.

Entregables: [informe editable](../hardware/INFORME_PINOUT_ROBOT_20261106.md)
y [PDF](../hardware/output/pdf/INFORME_PINOUT_ROBOT_20261106.pdf).
Incluye motores, Hall, IR digitales vigentes, HC-SR04, servo, panel OLED/encoder,
divisor de batería A5, UART, alimentación y control de servicio por NPN:
D32/PC5 AutoKEY y D33/PC4 AutoRESET. Marca base/agregado propio/servicio.

Se conserva la diferencia documentada de VCC del panel (montaje comunicado5V
frente a CAD+3V3) y paso de J2 (requisito inicial2,54mm frente a revisiónCAD2mm).
No se afirma montaje ni ensayo de los transistores. No se trasladan x4/CPR1500
del PDF BioLog al firmware AFR2 x1.

ATmega/ESP/Web/Loader/CAD: sin cambios en fuentes ni artefactos de producto.
Solo documentación y generación/revisión visual del informe PDF. No tests,
compilaciones, validadores de proyecto, COM, flasheos ni pruebas físicas.
Generar el informe no ejecuta herramientas ni scripts del firmware.

Procedencia y adaptación por archivo: [registro](../provenance/robot-pinout-report-20261106-15.json).
